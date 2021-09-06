#include "generator.h"
using namespace graph;

generator::generator(int V, int E, int minWeight, int maxWeight) {
	this->V = V;
    this->E = E;
    this->minWeight = minWeight;
    this->maxWeight = maxWeight;
     this->graph = new int*[this->V];
    for (int i = 0; i < this->V; i++) {
		this->graph[i] = new int[this->V];
	}

    this->generate();
}

generator::~generator() {
    for(int i=0; i<this->V; i++)
		delete[] this->graph[i];

	delete[] this->graph;
}

void generator::generate() {
    /* initialize random seed: */
    srand (time(NULL));

    // Generate random edge list
    int i, j, count;
    int * edge[E];
    for (i=0; i<E; i++)
         edge[i] = (int *)malloc(2 * sizeof(int));

	i = 0;
	// Build a connection between two random vertex.
	while(i < E)
	{
		edge[i][0] = rand()%V+1;
		edge[i][1] = rand()%V+1;
 
		if(edge[i][0] == edge[i][1])
			continue;
		else
		{
			for(j = 0; j < i; j++)
			{
				if((edge[i][0] == edge[j][0] && edge[i][1] == edge[j][1]) || (edge[i][0] == edge[j][1] && edge[i][1] == edge[j][0]))
					i--;
			}
		}
		i++;
	}

    // Init graph graph with zero
    for(int i = 0; i < V; i++)
    {
        for(int j = 0; j < V; j++)
        {
            graph[i][j] = 0;
        }
    }

    // Generate random weight for edges
    // Loop on edges
	for(i = 0; i < V; i++)
	{
		count = 0;
		for(j = 0; j < E; j++)
		{
			if(edge[j][0] == i+1)
			{
                int r = rand() % maxWeight + minWeight;
                graph[i][edge[j][1] - 1] = r;
                graph[edge[j][1] - 1][i] = r;
				count++;
			}
		}
	}
}

void generator::printAdjMatrix() {
    cout<<"\n"<< "{";
    for(int i = 0; i < V; i++)
    {
        cout<<"{";
        for(int j = 0; j < V; j++)
        {
            cout<<graph[i][j]<<", ";
        }
        cout<<"},\n";
    }
    cout<<"}";
}