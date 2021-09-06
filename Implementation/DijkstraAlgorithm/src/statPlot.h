
#ifndef CPUCURVE_H
#define CPUCURVE_H

#include <list>
#include <QDebug>
#include <QString>
#include <QReadWriteLock>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_item.h>
#include <qwt/qwt_legend_label.h>

// #include "cpupiemarker.h"

using namespace std;

#define HISTORY 199990
#define minX 10.0
#define maxX 1000.0
#define minY 0.0
#define maxY 10000.0

class StatPlot : public QwtPlot
{
    Q_OBJECT
public:
    enum CpuData
    {
        OpenMP,
        Standard,
        OpenCL,

        NCputData
    };

    explicit StatPlot(QWidget * = 0);
//    virtual ~StatPlot() {}

    const QwtPlotCurve * cpuCurve ( int id ) const
    {
        return data[id].curve;
    }



protected:
    void timerEvent(QTimerEvent *);
    list<QStringList> readStatistics();
private:
    void showCurve( QwtPlotItem * ,bool );
    QReadWriteLock lock;
    struct
    {
        QwtPlotCurve * curve;
        double data[HISTORY];
        bool visible; 
    }data[NCputData];

    double vertexData[HISTORY];

    int dataCount;

public  slots:
    void legendChecked(const QVariant &,bool);
};

#endif // CPUCURVE_H