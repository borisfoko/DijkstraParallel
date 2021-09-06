#include "openmp.h"
using namespace dijkstra;

openmp::openmp(int V): standard(V) {}

openmp::~openmp() {
	// Deconstructor code here
}

// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
void openmp::minDistance(int *dist, bool *sptSet, int start, int end, int *local_private_md, int *local_private_u)
{
	// Initialize min value
	*local_private_md = INT_MAX;
	*local_private_u = -1;
	int i, min_index;
	for (i = start; i <= end; i++)
		if (sptSet[i] == false && dist[i] <= *local_private_md) {
			*local_private_md = dist[i], *local_private_u = i;
		}
}

// A utility function to print the constructed distance array
void openmp::printSolution(int *dist)
{
	printf("Vertex Distance from Source\n");
	for (int i = 0; i < V; i++)
		printf("%d tt %d\n", i, dist[i]);
}


void openmp::init(int *dist, bool *sptSet, int src) {
	// Initialize all distances as INFINITE and stpSet[] as false
	int i;
	for (i = 0; i < V; i++) {
		dist[i] = INT_MAX, sptSet[i] = false;
	}

	// Distance of source vertex from itself is always 0
	// With critical wait until all the threads finish before running the next instruction
	dist[src] = 0;
}

void openmp::updateDist(int *dist, bool *sptSet, int **graph, int start, int end, int u) {
	int i;
	for (i = start; i <= end; i++)

	// Update dist[i] only if is not in sptSet, there is an edge from
	// u to i, and total weight of path from src to i through u is
	// smaller than current value of dist[i] 
	if (!sptSet[i] && graph[u][i] && dist[u] != INT_MAX && dist[u] + graph[u][i] < dist[i])
		dist[i] = dist[u] + graph[u][i];
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void openmp::dijkstra(int **graph, int src)
{
	int *dist = new int[V]; // The output array. dist[i] will hold the shortest
	// distance from src to i

	bool *sptSet = new bool[V]; // sptSet[i] will be true if vertex i is included in shortest
	// path tree or shortest distance from src to i is finalized

	init(dist, sptSet, src);
	// Find shortest path for all vertices
	int nth, u, md, local_id, local_start, local_end, local_step, local_u, local_md;

	//
	//  Begin the parallel region.
	//
	# pragma omp parallel private ( local_id, local_start, local_end, local_step, local_u, local_md ) shared (u, md, V, sptSet, dist, graph )
	{
		local_id = omp_get_thread_num ();
		nth = omp_get_num_threads (); 
		local_start = ( local_id * V ) / nth;
		local_end  = (( local_id + 1 ) * V ) / nth - 1;
		
		//
		//  The SINGLE directive means that the block is to be executed by only
		//  one thread, and that thread will be whichever one gets here first.
		//
		// # pragma omp single
		// {
		// 	cout << "\n";
		// 	cout << "  P" << local_id
		// 		<< ": Parallel region begins with " << nth << " threads.\n";
		// 	cout << "\n";
		// }

		// cout << "  P" << local_id
        //  << ":  Start: " << local_start
        //  << "  End: " << local_end << "\n";

		//
		//  Attach one more node on each iteration.
		//
		for (local_step = 0; local_step < V - 1; local_step++) {
			//
			//  Before we compare the results of each thread, set the shared variable 
			//  MD to a big value.  Only one thread needs to do this.
			//
			# pragma omp single 
			{
				md = INT_MAX;
				u = -1; 
			}
			// Pick the minimum distance vertex from the set of vertices not
			// yet processed. u is always equal to src in the first iteration.
			minDistance(dist, sptSet, local_start, local_end, &local_md, &local_u);

			//
			//  In order to determine the minimum of all the MY_MD's, we must insist
			//  that only one thread at a time execute this block!
			//
			# pragma omp critical
			{
				if ( local_md < md )  
				{
					md = local_md;
					u = local_u;
				}
			}
			//
			//  This barrier means that ALL threads have executed the critical
			//  block, and therefore MD and MV have the correct value.  Only then
			//  can we proceed.
			//
			# pragma omp barrier
			//
			//  If MV is -1, then NO thread found an unconnected node, so we're done early. 
			//  OpenMP does not like to BREAK out of a parallel region, so we'll just have 
			//  to let the iteration run to the end, while we avoid doing any more updates.
			//
			//  Otherwise, we connect the nearest node.
			//
			# pragma omp single 
			{
				if ( u != - 1 )
				{
					sptSet[u] = true;
					// cout << "  P" << local_id
					// 	<< ": Connecting node " << u << "\n";;
				}
			}

			//
			//  Again, we don't want any thread to proceed until the value of
			//  CONNECTED is updated.
			//
			# pragma omp barrier
			//
			//  Now each thread should update its portion of the MIND vector,
			//  by checking to see whether the trip from 0 to MV plus the step
			//  from MV to a node is closer than the current record.
			//
			if ( u != -1 )
			{
				updateDist( dist, sptSet, graph, local_start, local_end, u );
			}
			//
			//  Before starting the next step of the iteration, we need all threads 
			//  to complete the updating, so we set a BARRIER here.
			//
			#pragma omp barrier
		}


	}
	// print the constructed distance array
	// printSolution(dist);
	// Free space
	delete[] dist;
	delete[] sptSet;
}

