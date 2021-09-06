#pragma once
#include <limits.h>
#include <stdio.h>
#include <iostream>
using namespace std;

namespace dijkstra {
	class standard
	{
		protected:
			int V; // Number of vertices in the graph
		public:
			standard(int V);
			~standard();
			int minDistance(int *dist, bool *sptSet);
			void printSolution(int *dist, int n);
			void init(int *dist, bool *sptSet, int src);
			void dijkstra(int **graph, int src);
	};
}