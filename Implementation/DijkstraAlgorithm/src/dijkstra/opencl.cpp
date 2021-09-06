#include "opencl.h"
using namespace dijkstra;

opencl::opencl(int V): standard(V){}

std::string minDistance_kernel{ R"CLC(
	__kernel void minDistance(__global int* dist, __global bool* sptSet,
	 __global int* localMD, __global int* globalMins, __global int* globalIndex,
	  __global int* graph, const int sizeOfPartial, const int V, const int src)
{
	uint gid = get_global_id(0);
	uint global_size = get_global_size(0);
	uint lid = get_local_id(0);
	uint group_size = get_local_size(0);

	__local int localMins[1024];
	__local int localIndex[512];

	//init dist array
	dist[gid] = INT_MAX;
	sptSet[gid] = false;
	
	if(gid == 0)	{
		dist[src] = 0;
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	for (int count = 0; count < V - 1; count++){

		localMD[gid] = INT_MAX;

		barrier(CLK_GLOBAL_MEM_FENCE);

		if (sptSet[gid] == false) {
		localMD[gid] = dist[gid];}

		barrier(CLK_GLOBAL_MEM_FENCE);

		// load value from global mem to local mem
		localMins[lid] = localMD[gid];

		barrier(CLK_LOCAL_MEM_FENCE);

		bool isFirstIt = true;


		for(uint stride = group_size >> 1; stride>0; stride >>= 1) {	
				
			if(lid < stride) {
				if(localMins[lid] <= localMins[lid + stride]){
					if(!isFirstIt){
						localIndex[lid] = localIndex[lid];
					}else{
						localIndex[lid] = lid;
						isFirstIt = false;
					}
				}else{
					localMins[lid] = localMins[lid + stride];
					if(!isFirstIt){
						localIndex[lid] = localIndex[lid + stride];
					}else{
						localIndex[lid] = lid + stride;
						isFirstIt = false;
					}
					
				}
				
			}
			barrier(CLK_LOCAL_MEM_FENCE);
			if(stride != 1 && stride % 2 != 0){
				stride++;
			}
			
		}

		if(lid == 0){
			globalMins[get_group_id(0)] = localMins[0];
			globalIndex[get_group_id(0)] = localIndex[0];
			
		}

			barrier(CLK_GLOBAL_MEM_FENCE);

		if(sizeOfPartial > 1){
			if(gid == 0){
				
				int min = INT_MAX;
				int u = -1;
				for(uint i = 0; i < sizeOfPartial; i++){
					if(globalMins[i] <= min){
						min = globalMins[i];
						u = globalIndex[i] + group_size * i;
					}
					barrier(CLK_GLOBAL_MEM_FENCE);
				}
				globalIndex[0] = u;
				globalMins[0] = min;
			}
		}

		barrier(CLK_GLOBAL_MEM_FENCE);

		int u;
		u = globalIndex[0];
		sptSet[u] = true;
			
		
		barrier(CLK_GLOBAL_MEM_FENCE);

		// Update dist value of the adjacent vertices of the picked vertex.
		if(gid < V){
			// Update dist[v] only if is not in sptSet, there is an edge from
			// u to v, and total weight of path from src to v through u is
			// smaller than current value of dist[v]

			if (!sptSet[gid] && graph[u*V + gid] && dist[u] != INT_MAX
				&& dist[u] + graph[u * V + gid] < dist[gid]){
				dist[gid] = dist[u] + graph[u* V + gid];
				}
		}
		
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
})CLC" };

opencl::~opencl() {
	// Deconstructor code here
}



// A utility function to print the constructed distance array
void opencl::printSolution(int *dist, int n)
{
	printf("Vertex Distance from Source\n");
	for (int i = 0; i < V; i++)
		printf("%d tt %d\n", i, dist[i]);
}

cl::Device opencl::getOCLDevice(){
//init opencl
    //get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get( &all_platforms );
	
	if ( all_platforms.size() == 0 ) {
		std::cout << "OpenCL couldn't find any platforms on your Computer, which can be used!\n";
		exit(1);
	}
	cl::Platform default_platform = all_platforms[0];
	// std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	//get default device of the default platform
	std::vector<cl::Device> all_devices;

	default_platform.getDevices( CL_DEVICE_TYPE_GPU, &all_devices );
    
	if ( all_devices.size() == 0 ) {
		//std::cout << "No GPU could be found by OpenCL. Searching for CPU ...\n";
		default_platform.getDevices( CL_DEVICE_TYPE_CPU, &all_devices );
		if(all_devices.size() == 0){
			std::cout << "No CPU could be found by OpenCL. OpenCL is not able to run!\n";
			exit(1);
		}
	}
    
	return all_devices[0];
	// std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
}


void opencl::flatArray(int** array, int* flat_array, int width, int height){
	for (int i = 0; i < width; i++)
	{	
		for (int j = 0; j < height; j++)
		{
			flat_array[i*width + j] = array[j][i];
		}	
	}
}


cl::Kernel opencl::createKernel(cl::Device default_device, cl::Context context, std::string sKernel, char* kernelName){
	cl::Program::Sources sources;
	cl_int err = 0;
    sources.push_back( { sKernel.c_str(), sKernel.length() } );

	//build the program
    cl::Program program( context, sources, &err);

	//if errors exist build
	if ( program.build( { default_device } ) != CL_SUCCESS ) {
		std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>( default_device ) << "\n";
		exit(1);
	}

	//create the kernel
	cl::Kernel kernel(program, kernelName, &err);

	return kernel;
}

// Function that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void opencl::dijkstra(int **graph, int src)
{
	
	cl_int err = 0;
	int* graph_flat = (int *)malloc(V*V * sizeof(int));
	flatArray(graph, graph_flat, V, V);

	auto start = high_resolution_clock::now();
	
	cl::Device default_device = getOCLDevice();

	//cout << default_device.getInfo<CL_DEVICE_NAME>() << endl;

	//create context
    cl::Context context({default_device});

	//creating the command queue
    cl::CommandQueue queue( context, default_device);

	int size;

	if(V % 2 == 0){
		size = V;
	}else{
		size = V+1;
	}

	int* dist = new int [size]; // The output array. dist[i] will hold the shortest
	// distance from src to i


	bool* sptSet = new bool [size]; // sptSet[i] will be true if vertex i is included in shortest
	// path tree or shortest distance from src to i is finalized

	cl::Kernel kernel_minDist = createKernel(default_device, context, minDistance_kernel, "minDistance");

	auto workGroupSize = kernel_minDist.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(default_device);

	int numWorkGroups;
	if(size > workGroupSize){
		numWorkGroups = size/workGroupSize;
	}else{
		numWorkGroups = 1;
	}


	size_t global_size = size; 
	size_t local_size;
	int count = 2;
	bool getLocalsize = true;

	if(global_size <= workGroupSize){
		local_size = global_size;
	}else{
		while (getLocalsize)
		{
			if(global_size % count == 0 && global_size/count <= workGroupSize){
				local_size = global_size/count;
				getLocalsize = false;
			}
			count++;
		}
		
	}


		//allocate memory on the GPU
	cl::Buffer dev_globalMins( context, CL_MEM_WRITE_ONLY,  sizeof(int) * numWorkGroups, nullptr );
	cl::Buffer dev_dist( context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR ,  sizeof(int) * size, dist);
	cl::Buffer dev_sptSet( context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,  sizeof(bool) * size, sptSet );
	cl::Buffer dev_localMD (context, CL_MEM_READ_WRITE, sizeof(int) * size, nullptr );
	cl::Buffer dev_globalIndex( context, CL_MEM_WRITE_ONLY,  sizeof(int) * numWorkGroups, nullptr );
	cl::Buffer dev_graph( context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,  sizeof(int) * (V * V), graph_flat);



	//set args of the kernel
		err = kernel_minDist.setArg( 0, dev_dist );
		err = kernel_minDist.setArg( 1, dev_sptSet );
		err = kernel_minDist.setArg( 2, dev_localMD );
		err = kernel_minDist.setArg( 3, dev_globalMins );
		err = kernel_minDist.setArg( 4, dev_globalIndex );
		err = kernel_minDist.setArg( 5, dev_graph );
		err = kernel_minDist.setArg( 6, numWorkGroups );
		err = kernel_minDist.setArg( 7, V );
		err = kernel_minDist.setArg( 8, src );



	//call the kernel function minDistance
	err = queue.enqueueNDRangeKernel( kernel_minDist, cl::NullRange, cl::NDRange(global_size), cl::NDRange(local_size) );



	//read the result data from GPU
	err = queue.enqueueReadBuffer(dev_dist, CL_TRUE, 0, sizeof(int) * size, dist);


	if(err != 0){
		cout << "OpenCL Error Code: " << err << endl;
		exit(1);
	}

	queue.finish();
	// print the constructed distance array
	//printSolution(dist, V);
	
	// Free space
	delete[] dist;
	delete[] sptSet;

	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = duration_cast<microseconds>(stop - start);

	printf("%i\n", duration);
}

