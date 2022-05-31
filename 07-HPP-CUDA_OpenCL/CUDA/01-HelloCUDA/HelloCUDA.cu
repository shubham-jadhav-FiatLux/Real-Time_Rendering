// Header Files
// Standard Headers
#include <stdio.h>


// CUDA Headers
#include <cuda.h>		// For CUDA Functions

// Global Variables
const int iNumberOfArrayElements = 5;

float* hostInput1 = NULL;
float* hostInput2 = NULL;
float* hostOutput = NULL;

float* deviceInput1 = NULL;
float* deviceInput2 = NULL;
float* deviceOutput = NULL;

// CUDA Kernel
__global__ void vecAddGPU(float *in1, float *in2, float *out, int len)
{
	// Code
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	if (i < len)
	{
		out[i] = in1[i] + in2[i];
	}
}

// Entry-Point Function
int main(void)
{
	// Function Declarations
	void cleanup(void);

	// Variable Declarations
	int size = iNumberOfArrayElements * sizeof(float);
	cudaError_t result = cudaSuccess;

	// Code
	// Host Memory Allocation
	hostInput1 = (float*)malloc(size);
	if (hostInput1 == NULL)
	{
		printf("Host memory allocation is failed for hostInput1 array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostInput2 = (float*)malloc(size);
	if (hostInput2 == NULL)
	{
		printf("Host memory allocation is failed for hostInput2 array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostOutput = (float*)malloc(size);
	if (hostOutput == NULL)
	{
		printf("Host memory allocation is failed for hostOutput array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Filling Values Into Host Arrays
	hostInput1[0] = 101.0;
	hostInput1[1] = 102.0;
	hostInput1[2] = 103.0;
	hostInput1[3] = 104.0;
	hostInput1[4] = 105.0;

	hostInput2[0] = 201.0;
	hostInput2[1] = 202.0;
	hostInput2[2] = 203.0;
	hostInput2[3] = 204.0;
	hostInput2[4] = 205.0;

	// Device Memory Allocation
	result = cudaMalloc((void**)&deviceInput1, size);
	if (result != cudaSuccess)
	{
		printf("Device memory allocation is failed for deviceInput1 array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = cudaMalloc((void**)&deviceInput2, size);
	if (result != cudaSuccess)
	{
		printf("Device memory allocation is failed for deviceInput2 array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = cudaMalloc((void**)&deviceOutput, size);
	if (result != cudaSuccess)
	{
		printf("Device memory allocation is failed for deviceOutput array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Copy Data From Host Arrays Into Device Arrays
	result = cudaMemcpy(deviceInput1, hostInput1, size, cudaMemcpyHostToDevice);
	if (result != cudaSuccess)
	{
		printf("Host to device data copy is failed for deviceInput1 array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = cudaMemcpy(deviceInput2, hostInput2, size, cudaMemcpyHostToDevice);
	if (result != cudaSuccess)
	{
		printf("Host to device data copy is failed for deviceInput2 array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}


	dim3 dimGrid = dim3(iNumberOfArrayElements, 1, 1);
	dim3 dimBlock = dim3(1, 1, 1);

	// CUDA Kernel For Vector Addition
	vecAddGPU <<< dimGrid, dimBlock >>> (deviceInput1, deviceInput2, deviceOutput, iNumberOfArrayElements);


	// Copy Data From Device Array To Host Array
	result = cudaMemcpy(hostOutput, deviceOutput, size, cudaMemcpyDeviceToHost);
	if (result != cudaSuccess)
	{
		printf("Device to host data copy is failed for hostOutput array. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}
	
	// Vector Addition On Host
	for (int i = 0; i < iNumberOfArrayElements; i++)
	{
		printf("%f + %f = %f \n", hostInput1[i], hostInput2[i], hostOutput[i]);
	}

	// Cleanup
	cleanup();

	return 0;
}

void cleanup(void)
{
	// Code
	if (deviceOutput)
	{
		cudaFree(deviceOutput);
		deviceOutput = NULL;
	}

	if (deviceInput2)
	{
		cudaFree(deviceInput2);
		deviceInput2 = NULL;
	}

	if (deviceInput1)
	{
		cudaFree(deviceInput1);
		deviceInput1 = NULL;
	}

	if (hostOutput)
	{
		free(hostOutput);
		hostOutput = NULL;
	}

	if (hostInput2)
	{
		free(hostInput2);
		hostInput2 = NULL;
	}

	if (hostInput1)
	{
		free(hostInput1);
		hostInput1 = NULL;
	}
}
