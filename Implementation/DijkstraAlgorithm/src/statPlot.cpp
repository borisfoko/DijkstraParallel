#include <QApplication>
#include <QtGui>
#include "statPlot.h"


class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw(const QTime &base) :
        baseTime(base)
    {
    }

    virtual QwtText label(double v) const
    {
        QTime upTime = baseTime.addSecs(static_cast<int> (v));
        return upTime.toString("hh:mm:ss");
    }
private:
    QTime baseTime;
};

class Background: public QwtPlotItem
{
public:
    Background()
    {
        setZ(0.0);
    }

    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }

    virtual void draw(QPainter *painter,
                      const QwtScaleMap &xMap,
                      const QwtScaleMap &yMap,
                      const QRectF &canvasRect) const
    {
        // static int i = 0;
        // qDebug() << i++;
        QColor c(Qt::white);
        QRectF r = canvasRect;

        int step = 10000;
        for (int i=10000000;i > 0;i -= step){  
            r.setBottom(yMap.transform( i - step )); 
            r.setTop(yMap.transform(i));
            painter->fillRect( r,c );

            c = c.dark( 100 );
        }
    }
};

class CpuCurve : public QwtPlotCurve
{
public:
    CpuCurve(const QString &title):QwtPlotCurve(title)
    {
        setRenderHint(QwtPlotItem::RenderAntialiased);
    }

    void setColor( const QColor &color)
    {
        QColor c = color;
        c.setAlpha(150);

        setPen(c);
//        setPen(QPen(Qt::NoPen));
//        setBrush(c);
    }


};

StatPlot::StatPlot(QWidget *parent) :
    QwtPlot(parent),
    dataCount(0)
{
    setAutoReplot(false);

    QwtPlotCanvas * canvas = new QwtPlotCanvas();
    canvas->setBorderRadius(10);

    setCanvas(canvas);

    plotLayout()->setAlignCanvasToScales(true);

    QwtLegend * legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    insertLegend(legend,QwtPlot::RightLegend);

    setAxisTitle(QwtPlot::xBottom," Number of Vertices");
    setAxisScale(QwtPlot::xBottom, minX, maxX);
    /*
     In situations, when there is a label at the most right position of the
     scale, additional space is needed to display the overlapping part
     of the label would be taken by reducing the width of scale and canvas.
     To avoid this "jumping canvas" effect, we add a permanent margin.
     We don't need to do the same for the left border, because there
     is enough space for the overlapping label below the left scale.
     */

    QwtScaleWidget * scaleWidget = axisWidget(QwtPlot::xBottom);
    const int fmh = QFontMetrics( scaleWidget->font()).height();
    scaleWidget->setMinBorderDist(0,fmh / 2);

    setAxisTitle(QwtPlot::yLeft,"Execution time [in micro sec]");
    setAxisScale(QwtPlot::yLeft, minY, maxY);

    Background * bg = new Background();
    bg->attach(this);

    CpuCurve * curve;

    curve = new CpuCurve("Standard");
    curve->setColor(Qt::blue);
    curve->attach(this);
    data[Standard].curve = curve;

    curve = new CpuCurve("OpenMP");
    curve->setColor(Qt::green);
    curve->setZ(curve->z()-1);
    curve->attach(this);
    data[OpenMP].curve = curve;

    curve = new CpuCurve("OpenCL");
    curve->setColor(Qt::red);
    curve->setZ(curve->z()-2);
    curve->attach(this);
    data[OpenCL].curve = curve;

    // Save curve 
    data[Standard].visible = true;
    data[OpenMP].visible = true;
    data[OpenCL].visible = true;
    showCurve(data[Standard].curve, true);
    showCurve(data[OpenMP].curve, true);
    showCurve(data[OpenCL].curve, true);

    for (int var = 0; var < HISTORY; var++)
        vertexData[var] = (var + 1) * 5;
    startTimer(1000);

    connect( legend, SIGNAL( checked( const QVariant &, bool, int ) ),SLOT( legendChecked( const QVariant &, bool ) ) );
}

// StatPlot::~StatPlot(){}

void StatPlot::timerEvent(QTimerEvent *)
{
    // Move curve to left
    // for ( int i = dataCount; i  > 0; --i ) {
    //     for ( int c = 0; c < NCputData; ++c ) {
    //         if (i < HISTORY)
    //             data[c].data[i] = data[c].data[i-1];
    //     }
    // }

    //  Read data
    lock.lockForWrite();
    list<QStringList> dataList = readStatistics();
    lock.unlock();

    dataCount = dataList.size();
    int i = 0;
    int yDataMaxVal = 10000;
    bool ok = false;
    // qDebug() << "dataCount: " << dataCount;
    for (QStringList dataElem : dataList) {
        vertexData[i] = dataElem[0].toDouble(&ok);
        data[Standard].data[i] = dataElem[1].toDouble(&ok);
        data[OpenMP].data[i] = dataElem[2].toDouble(&ok);
        data[OpenCL].data[i] = dataElem[3].toDouble(&ok);
        if (data[Standard].data[i] > yDataMaxVal && data[Standard].visible) {
            yDataMaxVal = data[Standard].data[i];
        }
        if (data[OpenMP].data[i] > yDataMaxVal && data[OpenMP].visible ) {
            yDataMaxVal = data[OpenMP].data[i];
        }
        if (data[OpenCL].data[i] > yDataMaxVal && data[OpenCL].visible ) {
            yDataMaxVal = data[OpenCL].data[i];
        }
        i++;
    }

    // if ( dataCount < HISTORY )
    //     dataCount++;

    //for (int j = 0; j < HISTORY; ++j)
        //vertexData[j]++;
    // setAxisScale(QwtPlot::xBottom, vertexData[HISTORY - 1], vertexData[0]);
    // Refresh Axis
    setAxisScale(QwtPlot::xBottom, vertexData[0], vertexData[dataCount - 1]);
    setAxisScale(QwtPlot::yLeft, 0, yDataMaxVal);
    for (int c = 0; c < NCputData; ++c) {
        data[c].curve->setRawSamples(vertexData, data[c].data, dataCount);
    }
    replot();
}

list<QStringList> StatPlot::readStatistics() {
    // Define local list
    list<QStringList> listContainer;
    // Open and read Statisctic file
    QFile file("RunLog.log");
    if (file.open(QIODevice::ReadOnly | QFile::Text) )
    {
        QTextStream textStream(&file);
        do {
            QString line = textStream.readLine();
            line = line.trimmed();
            QStringList lineParts = line.split(",");
            if ( lineParts.count() == 4) {
                listContainer.push_back(lineParts);
            }
        } while (!textStream.atEnd());
    }

    return listContainer;
}

void StatPlot::showCurve(QwtPlotItem *item, bool b)
{
    item->setVisible(b);

    QwtLegend * lgd = qobject_cast<QwtLegend*> ( legend() );
    /*!
     QwtLegend 
    */
    QList<QWidget*> legendWidgets =
            lgd->legendWidgets(itemToInfo(item));

    if( legendWidgets.size() == 1){
        QwtLegendLabel * legendLabel =
                qobject_cast<QwtLegendLabel *>( legendWidgets[0]);
        if (legendLabel->plainText() == "Standard") {
            data[Standard].visible = b;
        } else if(legendLabel->plainText() == "OpenMP") {
            data[OpenMP].visible = b;
        } else if(legendLabel->plainText() == "OpenCL") {
            data[OpenCL].visible = b;
        }

        if(legendLabel){
            legendLabel->setChecked(b);
        }
    }

    replot();
}

void StatPlot::legendChecked(const QVariant &itemInfo, bool b)
{
    QwtPlotItem *plotItem = infoToItem(itemInfo);
    if ( plotItem)
        showCurve(plotItem,b);

}