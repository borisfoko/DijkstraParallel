#pragma once
#include <limits.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
using namespace std;

namespace graph {
    class generator
    {
        private:
            int V; // Number of vertices in the graph
            int E; // Number of edges in the graph
            int minWeight;
            int maxWeight;
            void generate();
        public:
            generator(int V, int E, int minWeight, int maxWeight); // Constructor
            ~generator(); // Deconstructor
            int **graph;
            void printAdjMatrix();
    };
}