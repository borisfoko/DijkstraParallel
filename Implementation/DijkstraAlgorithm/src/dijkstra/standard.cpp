#include "standard.h"
using namespace dijkstra;

standard::standard(int V) {
	this->V = V;
}

standard::~standard() {
	// Deconstructor code here
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
int standard::minDistance(int *dist, bool *sptSet)
{
	// Initialize min value
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (sptSet[v] == false && dist[v] <= min) {
			min = dist[v], min_index = v;
		}

	return min_index;
}

// A utility function to print the constructed distance array
void standard::printSolution(int *dist, int n)
{
	printf("Vertex Distance from Source\n");
	for (int i = 0; i < this->V; i++)
		printf("%d tt %d\n", i, dist[i]);
}


void standard::init(int *dist, bool *sptSet, int src) {
	// Initialize all distances as INFINITE and stpSet[] as false

	for (int i = 0; i < this->V; i++) {
		// std::cout << i;
		dist[i] = INT_MAX, sptSet[i] = false;
	}
	// Distance of source vertex from itself is always 0
	dist[src] = 0;
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void standard::dijkstra(int **graph, int src)
{
	int *dist = new int[V]; // The output array. dist[i] will hold the shortest
	// distance from src to i

	bool *sptSet = new bool[V]; // sptSet[i] will be true if vertex i is included in shortest
	// path tree or shortest distance from src to i is finalized


	init(dist, sptSet, src);
	// Find shortest path for all vertices
	for (int count = 0; count < V - 1; count++) {
		// Pick the minimum distance vertex from the set of vertices not
		// yet processed. u is always equal to src in the first iteration.
		int u = minDistance(dist, sptSet);


		// Mark the picked vertex as processed
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex.
		for (int v = 0; v < this->V; v++)

			// Update dist[v] only if is not in sptSet, there is an edge from
			// u to v, and total weight of path from src to v through u is
			// smaller than current value of dist[v]
			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
				&& dist[u] + graph[u][v] < dist[v])
				dist[v] = dist[u] + graph[u][v];

	}

	// print the constructed distance array
	// printSolution(dist, V);

	// Free space
	delete[] dist;
	delete[] sptSet;
}



