#ifndef DIJKSTRATHREAD_H
#define DIJKSTRATHREAD_H

#include <limits.h>
#include <chrono>
#include <QObject>
#include <QString>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTextStream>
#include <QReadWriteLock>
#include "dijkstra/standard.h"
#include "dijkstra/openmp.h"
#include "dijkstra/opencl.h"
#include "graph/generator.h"
using namespace std::chrono;

class DijkstraThread : public QObject
{
    Q_OBJECT
    // A property that controls the work of thread
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int minV READ minV WRITE setMinV NOTIFY minVChanged)
    Q_PROPERTY(int maxV READ maxV WRITE setMaxV NOTIFY maxVChanged)
    Q_PROPERTY(int vIncr READ vIncr WRITE setVIncr NOTIFY vIncrChanged)
    Q_PROPERTY(QString log READ log WRITE setLog NOTIFY logChanged)
    Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
    bool m_running;
    int m_minV;
    int m_maxV;
    int m_vIncr;
    QString m_log;
    int m_currentV;
    int m_currentE;
    int m_standard;
    int m_openMP;
    int m_openCL;
    QString m_status;

public:
    explicit DijkstraThread(QObject *parent = 0);
    bool running() const;
    int minV() const;
    int maxV() const;
    int vIncr() const;
    QString log() const;
    int currentV() const;
    int currentE() const;
    int standard() const;
    int openMP() const;
    int openCL() const;
    QString status() const;

    int count; 
    void runDijkstra(int V, int E, QString * statistics);
    void write(QString filename, QString inputText);
    void runDijkstraOnRange(int startV, int endV, int vIncrValue);
private:
    QReadWriteLock lock;

signals:
    void finished();    // The signal, in which we will complete the thread after the end of the run method
    void runningChanged(bool running);
    void minVChanged(int minV);
    void maxVChanged(int maxV);
    void vIncrChanged(int vIncr);
    void logChanged(QString log);
    void currentVEChanged(int currentV, int currentE);
    void currentStatisticsChanged(int standard, int openMP, int openCL);
    void statusChanged(QString status);
 
public slots:
    void run(); // Method with payload which can be carried out in a loop
    void setRunning(bool running);
    void setMinV(int minV);
    void setMaxV(int maxV);
    void setVIncr(int vIncr);
    void setLog(QString log);
    void setCurrentV(int currentV);
    void setCurrentE(int currentE);
    void setStandard(int standard);
    void setOpenMP(int openMP);
    void setOpenCL(int openCL);
    void setStatus(QString status);
};

#endif // DIJKSTRATHREAD_H
