# DijkstraParallel
Dijkstra Algorithm with Standard and Parallel through OpenMP and OpenCL implementation in C++

## Projekt

### Thema

Kurzester Pfad https://de.wikipedia.org/wiki/K%C3%BCrzester_Pfad. Dijkstra-Algorithmus https://de.wikipedia.org/wiki/Dijkstra-Algorithmus

### Description:
 
Ein grundlegendes Problem in der Graphentheorie ist das Suche eines kürzesten
Pfades ausgehend von einem Startknoten zu allen Knoten in einem Graph. Das
Problem wird von Dijkstra Algorithmus effizient gelöst. Ziel dieser Aufgabe ist
es, den Algorithmus zu parallelisieren und in OpenCL zu implementieren. Als
Grundlage kann eine Arbeit von Harish und Narayanan (2008) mit dem Titel
Accelerating large graph algorithms on the GPU using CUDA dienen.

### Programmiersprache: 

C++ als Programmiersprache

### Compiler:

gcc with cmake

### Visualisierung

Visualisierung der berechneten Daten oftmals auch sinnvoll und werden positiv bewertet. Mit OpenGL wird die Visualisierung gebaut (Mögliche Schnittstellen sind SDL, OpenGL, Qt, wxWidgets oder GTK,
die für die meisten Betriebssysteme verfügbar sind).

### Parallelisierung APIs:
OpenCL (Grafikkarte) vs OpenMP (CPU)

1. OpenCl (Grafikkarte)
2. OpenMP (CPU)

### Video

Video soll 10 Minutes pro Teilnehmer (2 Pro Gruppe => 20 Minuten) dauern und präsentiert die Problematik, die Vorgehensweise (Vorgegeben, ) und die Ergebnisse (Visualisierung, etc.).

### Dokumentation

Die Dokumentation besteht aus vier Teile: 
- Teil 1: Der erste Teil führt kurz in die theoretischen Grundlagen der Problemstellung (Anwendung, mathematische Herleitung, etc.)
- Teil 2: Die eigene Vorgehensweise und die Implementation erläutern
- Teil 3: Die Ergebnisse präsentieren
- Teil 4: Die Arbeit diskutieren, auf Probleme und Schwierigkeiten eingehen.

### Install and setup cmake


### Setup opencl 

* Install opencl: `sudo apt update` and `sudo apt install ocl-icd-opencl-dev`
* Install opencl hpp heaqders using `sudo apt-get install opencl-clhpp-headers`
* On Intel run this additionally command `sudo apt-get install intel-opencl-icd`

### Install qt
1. Install all needed tools for qt development
Run the command below in your terminal:
    sudo apt-get install -y \
    build-essential \
    cmake \
    qtbase5-dev \
    qtdeclarative5-dev \
    qtquickcontrols2-5-dev \
    qml-module-qtquick2 \
    qml-module-qtquick-controls \
    qml-module-qtcharts \
    lubuntu-qt-core \
    libqt5sensors5-dev \
    libqwt-qt5-dev \
    qtscript5-dev

Install qtChart using `sudo apt install libqt5charts5-dev`

Find more details about how to get started with qt [here](https://www.apriorit.com/dev-blog/475-qt-qml-with-cmake)

### Setup SDF for the use interface

* Windows: For windows systems find all needed libraries here https://www.libsdl.org/download-2.0.php
* Linux: For Linux systems use the following command `sudo apt install cmake libsdl2-dev g++` to install SDL and all needed dependencies
For the rest of the installation follow the tutorial unter the link below: https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/ 

### Buid and run the application

Follow the next steps to build and run the project
- Setp 1: Go to the root folder of the project `$ cd <Project_Path>`
- Step 2: Generate Makefile using cmake with `$ cmake .`
- Step 3: Build the project with make with  `$ make`
- Step 4: Run the application with  `$ ./dijkstraAlgorithm`
- Step 5: Yes, you made it :)