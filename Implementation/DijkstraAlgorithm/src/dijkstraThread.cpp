#include "dijkstraThread.h"

#define minEdgeWeight 1
#define maxEdgeWeight 15

DijkstraThread::DijkstraThread(QObject *parent) : 
QObject(parent), m_minV(0), m_maxV(0), m_vIncr(1)
{

}

bool DijkstraThread::running() const
{
    return m_running;
}
 
int DijkstraThread::minV() const
{
    return m_minV;
}
 
int DijkstraThread::maxV() const
{
    return m_maxV;
}

int DijkstraThread::vIncr() const
{
    return m_vIncr;
}

QString DijkstraThread::log() const
{
	return m_log;
}

int DijkstraThread::currentV() const
{
	return m_currentV;
}

int DijkstraThread::currentE() const
{
	return m_currentE;
}

int DijkstraThread::standard() const
{
	return m_standard;
}

int DijkstraThread::openMP() const
{
	return m_openMP;
}

int DijkstraThread::openCL() const
{
	return m_openCL;
}

QString DijkstraThread::status() const
{
	return m_status;
}


void DijkstraThread::runDijkstra(int V, int E, QString * statistics) {
	// Let us generate an example graph
	graph::generator graph_g(V, E, minEdgeWeight, maxEdgeWeight);
	
	// Print the adjacent matrix of the generated graph
	// graph_g.printAdjMatrix();
	// Standard dijkstra algorithm
	// std::cout << "Dijkstra_standard " << std::endl;
	dijkstra::standard ds(V);
	// Start chrono for Standard dijkstra algorithm
	auto startStandard = high_resolution_clock::now();
	ds.dijkstra(graph_g.graph, 0);
	// Stop chrono for Standard dijkstra algorithm
	auto stopStandard = high_resolution_clock::now();

	// Calculate execution time for standard algo without parallel execution
	auto durationStandard = duration_cast<microseconds>(stopStandard - startStandard);
	// std::cout << "Execution time: " << durationStandard.count() << std::endl;

	// Dijkstra algorithm parallel with openmp
	// std::cout << "Dijkstra_openmp " << std::endl;
	dijkstra::openmp dsOmp(V);

	// Start chrono for Standard dijkstra algorithm parallel with openmp
	auto startOpenMP = high_resolution_clock::now();
	dsOmp.dijkstra(graph_g.graph, 0);

	// Stop chrono for Standard dijkstra algorithm parallel with openmp
	auto stopOpenMP = high_resolution_clock::now();

	// Calculate execution time for algo with openmp
	auto durationOpenMP = duration_cast<microseconds>(stopOpenMP - startOpenMP);
	// std::cout << "Execution time with opnmp: " << durationOpenMP.count() << std::endl; 

	// Dijkstra algorithm parallel with opencl
	// std::cout << "dijkstra_opencl " << std::endl;
	// dijkstra_opencl dsOcl(V);
	// dsOcl.dijkstra(graph, 0);  

	// Dijkstra algorithm parallel with opencl
	// std::cout << "\ndijkstra_opencl " << std::endl;
	dijkstra::opencl dsOcl(V);

	// Start chrono for Standard dijkstra algorithm parallel with opencl
	auto startOpenCL = high_resolution_clock::now();
	try {
		dsOcl.dijkstra(graph_g.graph, 0);
	} catch(const std::exception& e) {
		qDebug() << "ERROR: " << e.what();
	}

	// Stop chrono for Standard dijkstra algorithm parallel with opencl
	auto stopOpenCL = high_resolution_clock::now();

	
	// Calculate execution time for algo with opencl
	auto durationOpenCL = duration_cast<microseconds>(stopOpenCL - startOpenCL);
	// std::cout << "Execution time with opnCL: " << durationOpenCL.count() << std::endl; 

	statistics[0] =  QString("%1").arg(durationStandard.count());
	statistics[1] =  QString("%1").arg(durationOpenMP.count());
	statistics[2] =  QString("%1").arg(durationOpenCL.count());
}

void DijkstraThread::write(QString filename, QString inputText)
{
    QFile file{filename};
    // Trying to open in ReadWrite and Text mode
    if(!file.open(QFile::ReadWrite |
                  QFile::Text))
    {
        qDebug() << " Could not open file for writing";
        return;
    }

    // To write text, we use operator<<(),
    // which is overloaded to take
    // a QTextStream on the left
    // and data types (including QString) on the right
	QTextStream out(&file);
	while (!out.atEnd()) {
		out.readLine();
		// qDebug() << "TextLine: " << line;
	}
    out << inputText + "\n";
    file.flush();
    file.close();
}

void DijkstraThread::runDijkstraOnRange(int startV, int endV, int vIncrValue) {
    // Remove File before starting
	// Statistics
	QString statistics[3]; 
	QFile file ("RunLog.log");
    QFileInfo fileInfo(file);
    if (fileInfo.exists() && fileInfo.isFile()) {
        file.remove();
    }
    
	int localStart = 10 > startV ? 10 : startV;
	// Calculate appropriate step
	int step = vIncrValue;

    // Start execution
	for (int V = localStart; V <= endV; V= V + step) {
		if (!m_running) {
			qDebug() << "Stopped";
			setStatus("Stopped");
         	break;
      	}
		setCurrentV(V);
		setCurrentE(V * 2 + 1);
		runDijkstra(V, V * 2 + 1, statistics);
		//qDebug() << "statistics: " << statistics << ", size: " << (sizeof(statistics)/sizeof(*statistics));
		if ((sizeof(statistics)/sizeof(*statistics)) == 3) {
			setLog(QString("%1, %2, %3, %4").arg(V).arg(statistics[0]).arg(statistics[1]).arg(statistics[2]));
			setStandard(statistics[0].toInt());
			setOpenMP(statistics[1].toInt());
			setOpenCL(statistics[2].toInt());
			write("RunLog.log", QString("%1,%2,%3,%4").arg(V).arg(statistics[0]).arg(statistics[1]).arg(statistics[2]));
		}
	}
}
 
// The most important method, which will be carried out "useful" work object
void DijkstraThread::run()
{
    count = 0;
    // Variable m_running responsible for the object in the stream.
    // If false, the work is completed
    while (m_running)
    {
        count++;
        // Run the work logic
        if (count == 1) {
			setStatus("Running");
			lock.lockForWrite();
			runDijkstraOnRange(m_minV, m_maxV, m_vIncr);
			lock.unlock();
			setStatus("Finished");
        } else {
            setRunning(false);
        }

    }
    emit finished();
}
 
void DijkstraThread::setRunning(bool running)
{
	if (m_running == running)
        return;
 
    m_running = running;
    emit runningChanged(running);
}
 
void DijkstraThread::setMinV(int minV)
{
    if (m_minV == minV)
        return;
 
    m_minV = minV;
    emit minVChanged(minV);
}
 
void DijkstraThread::setMaxV(int maxV)
{
    if (m_maxV == maxV)
        return;
 
    m_maxV = maxV;
    emit maxVChanged(maxV);
}

void DijkstraThread::setVIncr(int vIncr)
{
    if (m_vIncr == vIncr)
        return;
 
    m_vIncr = vIncr;
    emit vIncrChanged(vIncr);
}

void DijkstraThread::setLog(QString log)
{
    if (m_log == log)
        return;
 
    m_log = log;
    emit logChanged(log);
}

void DijkstraThread::setCurrentV(int currentV)
{
    if (m_currentV == currentV)
        return;
 
    m_currentV = currentV;
    emit currentVEChanged(currentV, m_currentE);
}

void DijkstraThread::setCurrentE(int currentE)
{
    if (m_currentE == currentE)
        return;
 
    m_currentE = currentE;
    emit currentVEChanged(m_currentV, currentE);
}

void DijkstraThread::setStandard(int standard)
{
    if (m_standard == standard)
        return;
 
    m_standard = standard;
	emit currentStatisticsChanged(standard, m_openMP, m_openCL);
}

void DijkstraThread::setOpenMP(int openMP)
{
    if (m_openMP == openMP)
        return;
 
    m_openMP = openMP;
    emit currentStatisticsChanged(m_standard, openMP, m_openCL);
}

void DijkstraThread::setOpenCL(int openCL)
{
    if (m_openCL == openCL)
        return;
 
    m_openCL = openCL;
    emit currentStatisticsChanged(m_standard, m_openMP, openCL);
}

void DijkstraThread::setStatus(QString status)
{
    if (m_status == status)
        return;
 
    m_status = status;
    emit statusChanged(status);
}
