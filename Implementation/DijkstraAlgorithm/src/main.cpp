#include <limits.h>
#include <stdio.h>
#include <chrono>
#include <iostream>
#include <QApplication>
#include <QDesktopWidget>
#include <QString>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>
#include <QThread>
#include <QTimer>
#include "statPlot.h"
#include "ctkrangeslider.h"
#include "dijkstraThread.h"

using namespace std::chrono;
using namespace dijkstra;
using namespace graph;

#define minV 10
#define maxV 200000
#define minVValue 300
#define maxVValue 600

int runQTGui(int argc, char** argv) {
	// Clear previous logs
	QFile file ("RunLog.log");
    QFileInfo fileInfo(file);
    if (fileInfo.exists() && fileInfo.isFile()) {
        file.remove();
    }
	// Setup Background Thread
	QThread dijkstraThread;
	DijkstraThread dijkstraThreadObject;

	// Link Thread to dijkstra worker
	QObject::connect(&dijkstraThread, &QThread::started, &dijkstraThreadObject, &DijkstraThread::run);
	QObject::connect(&dijkstraThreadObject, &DijkstraThread::finished, &dijkstraThread, &QThread::terminate);
	dijkstraThreadObject.setMinV(minVValue);
	dijkstraThreadObject.setMaxV(maxVValue);
	
	QApplication app(argc,argv);
	QDesktopWidget dw;
	int x = dw.width() * 0.7;
	int y = dw.height() * 0.7;

	QWidget vBox;
	vBox.setWindowTitle( "Dijkstra Algorithm Parallelizer" );

	QGridLayout *gridLayout = new QGridLayout;
	// Set Layout strech
	for(int i = 0; i < 9; i++) {
		gridLayout->setRowStretch( i, 1);
		gridLayout->setColumnStretch( i, 1);
	}
	
	// Define label font
	QFont f( "Arial", 15, QFont::Bold);

	// Vertice Label
	QString lbStringVertice = QString( "Vertices (from: %1, to: %2)" ).arg(minVValue).arg(maxVValue);
	QLabel *lbVertice = new QLabel( lbStringVertice, &vBox );
	lbVertice->setFont(f);
	lbVertice->setMargin(0);
	lbVertice->setIndent(0);

	// Vertice RangeSlider 
	ctkRangeSlider rsVertice(Qt::Orientation::Horizontal, &vBox);
	rsVertice.setMinimum(minV);
  	rsVertice.setMaximum(maxV);
	rsVertice.setValues(minVValue, maxVValue);
	rsVertice.setTracking(true);
	QObject::connect(&rsVertice, &ctkRangeSlider::valuesChanged, [lbVertice, &dijkstraThreadObject](int min, int max){ 
		lbVertice->setText(QString("Vertices (from: %1, to: %2)").arg(min).arg(max));
		dijkstraThreadObject.setMinV(min);
		dijkstraThreadObject.setMaxV(max);
	});

	// Vertice Increment Label
	QString lbStringVerticeIncr = QString("V-Increment: %1").arg(1);
	QLabel *lbVerticeIncr = new QLabel( lbStringVerticeIncr, &vBox );
	lbVerticeIncr->setFont(f);
	lbVerticeIncr->setMargin(0);
	lbVerticeIncr->setIndent(0);

	// V-Increment Slider
	QSlider *vIncrSlider = new QSlider(Qt::Horizontal, &vBox);
    vIncrSlider->setFocusPolicy(Qt::StrongFocus);
    // vIncrSlider->setTickPosition(QSlider::TicksBothSides);
    vIncrSlider->setSingleStep(1);
	vIncrSlider->setMinimum(1);
	vIncrSlider->setMaximum(200);
	QObject::connect(vIncrSlider, &QSlider::valueChanged, [lbVerticeIncr, &dijkstraThreadObject](int value){ 
		lbVerticeIncr->setText(QString("V-Increment: %1").arg(value));
		dijkstraThreadObject.setVIncr(value);
	});

	// Text Edit (text Area)
	QTextEdit *tEditLog = new QTextEdit(&vBox);
	tEditLog->setReadOnly(true);
	tEditLog->setText("V, Standard, OpenMP, OpenCL");
	QObject::connect(&dijkstraThreadObject, &DijkstraThread::logChanged, &vBox, [tEditLog](QString log){ 
		tEditLog->setText(QString("%1\n%2").arg(tEditLog->toPlainText()).arg(log));
	});

	// Current status Labels
	QString lbStringCurrentVAndE( "" );
	QLabel *lbCurrentVAndE = new QLabel( lbStringCurrentVAndE, &vBox );
	lbCurrentVAndE->setFont(f);
	QObject::connect(&dijkstraThreadObject, &DijkstraThread::currentVEChanged, &vBox, [lbCurrentVAndE](int currentV, int currentE){ 
		lbCurrentVAndE->setText(QString("Graph's Vertices: %1, Edges: %2").arg(currentV).arg(currentE));
	});

	QString lbStringCurrentETimeVallues("" );
	QLabel *lbCurrentETimeValues = new QLabel( lbStringCurrentETimeVallues, &vBox );
	lbCurrentETimeValues->setFont(f);
	QObject::connect(&dijkstraThreadObject, &DijkstraThread::currentStatisticsChanged, &vBox, [lbCurrentETimeValues](int standard, int openMP, int openCL){ 
		lbCurrentETimeValues->setText(QString("Standard: %1, MP: %2, CL: %3").arg(standard).arg(openMP).arg(openCL));
	});

	QString lbStringCurrentStatusDecoration( "Status: " );
	QLabel *lbCurrentStatusDecoration = new QLabel( lbStringCurrentStatusDecoration, &vBox );
	lbCurrentStatusDecoration->setFont(f);

	QString lbStringCurrentStatus( "" );
	QLabel *lbCurrentStatus = new QLabel( lbStringCurrentStatus, &vBox );
	lbCurrentStatus->setFont(f);
	// Timer for blink animation
	int TimerStateValue = 0;
	int* TimerState = &TimerStateValue;
	QTimer* timer = new QTimer(&vBox);  //create timer with optional parent object
	QObject::connect(timer,&QTimer::timeout, &vBox, [lbCurrentStatus, TimerState](){ 
		//qDebug() << "TimerState: " << *TimerState;
		if (*TimerState == 0) {
			lbCurrentStatus->setStyleSheet("QLabel { background-color : transparent; color : transparent; }");
			*TimerState = 1;
		} else {
			lbCurrentStatus->setStyleSheet("QLabel { background-color : transparent; color : blue; }");
			*TimerState = 0;
		}
	});

	// Set Status change trigger
	QObject::connect(&dijkstraThreadObject, &DijkstraThread::statusChanged, &vBox, [lbCurrentStatus, timer](QString status){ 
		lbCurrentStatus->setText(QString("%1").arg(status));
		// Change Label color
		if (status == "Running") {
			lbCurrentStatus->setStyleSheet("QLabel { background-color : transparent; color : blue; }");
			timer->start(1500); //start with a 1s interval
		} else if (status == "Finished") {
			timer->stop();
			lbCurrentStatus->setStyleSheet("QLabel { background-color : transparent; color : green; }");
		} else if (status == "Stopped") {
			timer->stop();
			lbCurrentStatus->setStyleSheet("QLabel { background-color : transparent; color : red; }");
		}
	});

	QPushButton *btnStart = new QPushButton("Start");
	QObject::connect(btnStart, &QPushButton::clicked, [&dijkstraThreadObject, &dijkstraThread](){ 
		dijkstraThreadObject.setRunning(true);
		dijkstraThread.start();
	});
    QPushButton *btnStop = new QPushButton("Stop");
	QObject::connect(btnStop, &QPushButton::clicked, [&dijkstraThreadObject](){ 
		dijkstraThreadObject.setRunning(false);
	});

	StatPlot *plot = new StatPlot( &vBox );
	plot->setTitle( "Live" );

	const int margin = 5;
	plot->setContentsMargins( margin, margin, margin, margin );

	QString info( "Press the legend to en/disable a curve" );

	QLabel *lbLegend = new QLabel( info, &vBox );

	// addWidget(*Widget, row, column, rowspan, colspan)
	// Add Controls to the gridLayout
	gridLayout->addWidget(lbVertice,0,0,1,2);
    gridLayout->addWidget(&rsVertice,1,0,1,2);

	// Add V-Increment Label and slider
	gridLayout->addWidget(lbVerticeIncr,2,0,1,2);
	gridLayout->addWidget(vIncrSlider,3,0,1,2);

	// Add Log Text Edit Label 
	gridLayout->addWidget(tEditLog,4,0,4,2);

	// Add Current Status Label 
	gridLayout->addWidget(lbCurrentVAndE,8,0,1,2);

	// Add Executiontime
	gridLayout->addWidget(lbCurrentETimeValues,9,0,1,2);
	
	gridLayout->addWidget(lbCurrentStatusDecoration,10,0,1,1);
	gridLayout->addWidget(lbCurrentStatus,10,1,1,1);

	// Add action button
	gridLayout->addWidget(btnStart,11,0,1,1);
	gridLayout->addWidget(btnStop,11,1,1,1);
	// Add Plotter
	gridLayout->addWidget(plot,0,2,11,7);

	// Add legend label on bottom
	gridLayout->addWidget(lbLegend,11,7,1,3);
	gridLayout->setSpacing(0);
	vBox.setLayout(gridLayout);

	// Start background Thread before showing the GUI
	dijkstraThreadObject.moveToThread(&dijkstraThread);

	vBox.resize( x,y );
	vBox.show();

	return app.exec();
}

// driver program to test above function
int main(int argc, char** argv)
{
	// Run from CMD without GUI (Not yet implemented)
	// Run the QT Gui
	return runQTGui(argc, argv);
}