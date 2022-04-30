// Header Files
// Standard Headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>		// fabs()

// OpeenCL Headers
#include <CL/opencl.h>	

#include "helper_timer.h"

// Macros
#define BLOCK_WIDTH 64

// Global Variables
cl_platform_id oclPlatformID;
cl_device_id oclDeviceID;

cl_context oclContext;
cl_command_queue oclCommandQueue;

cl_program oclProgran;
cl_kernel oclKernel;

int* hostA = NULL;
int* hostB = NULL;
int* hostC = NULL;
int* gold = NULL;

cl_mem deviceInput1 = NULL;
cl_mem deviceInput2 = NULL;
cl_mem deviceOutput = NULL;

float timeOnCPU = 0.0f;
float timeOnGPU = 0.0f;

// OpenCL Kernel
const char* oclSourceCode =
"__kernel void matMulGPU(__global int *A, __global int *B, __global int *C, int numARows, int numAColumns, int numBColumns, int numCColumns)" /
"{" /
// Variable Declarations
"int row = get_global_id(0);" /
"nt column = get_global_id(1);" /

// Code
"if ((row < numARows) && (column < numBColumns))" /
"{" /
"int value = 0;" /
"for (int k = 0; k < numAColumns; k++)" /
"{" /
"int a = A[row * numAColumns + k];" /
"int b = B[k * numBColumns + column];" /
"value += a * b;" /
"}" /
"C[row * numCColumns + column] = value;" /
"}" /
"}";

// Entry-Point Function
int main(int argc, char *argv[])
{
	// Function Declarations
	void InitA(int* data, int, int);
	void InitB(int* data, int, int);
	void matMulCPU(int*, int*, int*, int, int, int, int);
	void cleanup(void);

	// Variable Declarations
	int numARows = BLOCK_WIDTH;
	int numAColumns = BLOCK_WIDTH;
	int numBRows = BLOCK_WIDTH;
	int numBColumns = BLOCK_WIDTH;
	int numCRows = numARows;
	int numCColumns = numBColumns;

	int numGoldRows = numARows;
	int numGoldColumns = numBColumns;

	int sizeA = numARows * numAColumns * sizeof(int);
	int sizeB = numBRows * numBColumns * sizeof(int);
	int sizeC = numCRows * numCColumns * sizeof(int);
	int sizeGold = numGoldRows * numGoldColumns * sizeof(int);

	cl_int result;

	// Code
	// Host Memory Allocation
	hostA = (int*)malloc(sizeA);
	if (hostA == NULL)
	{
		printf("Host memory allocation is failed for hostA matrix. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostB = (int*)malloc(sizeB);
	if (hostB == NULL)
	{
		printf("Host memory allocation is failed for hostB matrix. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostC = (int*)malloc(sizeC);
	if (hostC == NULL)
	{
		printf("Host memory allocation is failed for hostC matrix. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	gold = (int*)malloc(sizeGold);
	if (gold == NULL)
	{
		printf("Host memory allocation is failed for gold matrix. \n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Printing Matrix Diimentions And Sizes
	printf("The dimensions of matrix 'hostA' are : %d x %d \n", numARows, numAColumns);
	printf("The dimensions of matrix 'hostB' are : %d x %d \n", numBRows, numBColumns);
	printf("The dimensions of matrix 'hostC' are : %d x %d \n", numCRows, numCColumns);
	printf("The dimensions of matrix 'gold' are : %d x %d \n", numGoldRows, numGoldColumns);

	printf("Size of matrix hostA = %d\n", sizeA);
	printf("Size of matrix hostB = %d\n", sizeB);
	printf("Size of matrix hostC = %d\n", sizeC);
	printf("Size of matrix gold = %d\n", sizeGold);

	// Fill Source Matrices
	InitA(hostA, numARows, numAColumns);
	InitB(hostB, numBRows, numBColumns);

	// Get OpenCL Supporting Platform's ID
	result = clGetPlatformIDs(1, &oclPlatformID, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clGetPlatformIDs() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Get OpenCL Supporting CPU Device's ID
	result = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, 1, &oclDeviceID, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clGetDeviceIDs() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Create OpenCL Compute Context
	oclContext = clCreateContext(NULL, 1, &oclDeviceID, NULL, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateContext() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Create Command Queue
	oclCommandQueue = clCreateCommandQueue(oclContext, oclDeviceID, 0, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateCommandQueue() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Create OpenCL Program From .cl
	oclProgram = clCreateProgramWithSource(oclContext, 1, (const char**)&oclSourceCode, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateProgramWithSource() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Build OpenCL Program
	result = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		size_t len;
		char buffer[2048];
		clGetProgramBuildInfo(oclProgram, oclDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("Program Build Log : %s \n", buffer);
		printf("clBuildProgram() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Create OpenCL Kernel By Passing Kernel Function Name That We Used In .cl File
	oclKernel = clCreateKernel(oclProgram, "matMulGPU", &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateKernel() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}



	// Device Memory Allocation
	deviceA = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, sizeA, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for deviceA array : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	deviceB = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, sizeB, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for deviceB array : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	deviceC = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, sizeC, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for deviceC array : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 0th Argument i.e. deviceA
	result = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void*)&deviceA);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 1st Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 1st Argument i.e. deviceB
	result = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void*)&deviceB);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 2nd Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 2nd Argument i.e. deviceC
	result = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void*)&deviceC);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 3rd Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 3rd Argument i.e. numARows
	result = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void*)&numARows);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 4th Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 4rd Argument i.e. numAColumns
	result = clSetKernelArg(oclKernel, 4, sizeof(cl_int), (void*)&numAColumns);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 5th Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 5th Argument i.e. numBColumns
	result = clSetKernelArg(oclKernel, 5, sizeof(cl_int), (void*)&numBColumns);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 6th Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 6th Argument i.e. numCColumns
	result = clSetKernelArg(oclKernel, 6, sizeof(cl_int), (void*)&numCColumns);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 7th Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Write Above 'input' Device buffer To Device Memory
	result = clEnqueueWriteBuffer(oclCommandQueue, deviceA, CL_FALSE, 0, size, hostA, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueWriteBuffer() Failed For 1st Input Device buffer : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = clEnqueueWriteBuffer(oclCommandQueue, deviceB, CL_FALSE, 0, size, hostB, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueWriteBuffer() Failed For 2nd Input Device buffer : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Kernel Configuration
	size_t globalWorkSize[2];
	globalWorkSize[0] = BLOCK_WIDTH;
	globalWorkSize[1] = BLOCK_WIDTH;
	globalWorkSize = roundGlobalSizeToNearestMultipleOfLocalSize(localWorkSize, iNumberOfArrayElements);

	// Start Timer
	StopWatchInterface* timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	result = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueNDRangeKernel() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Finish OpenCL Command Queue
	clFinish(oclCommandQueue);

	// Stop Timer
	sdkStopTimer(&timer);
	timeOnGPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
	timer = NULL;

	// Read Back Result From The Device (i.e. from deviceOutput) into CPU variable (i.e. hostOutput)
	result = clEnqueueReadBuffer(oclCommandQueue, deviceC, CL_TRUE, 0, sizeC, hostC, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueReadBuffer() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Matrix Multiplication On Host
	matMulCPU(hostA, hostB, gold, numARows, numAColumns, numBColumns, numCColumns);

	// Comparison
	int breakValue = -1;
	bool bAccuracy = true;
	for (int i = 0; i < numCRows * numCColumns; i++)
	{
		int val1 = gold[i];
		int val2 = hostC[i];
		if (val1 != val2)
		{
			bAccuracy = false;
			breakValue = i;
			break;
		}
	}

	char str[128];
	if (bAccuracy == false)
		sprintf(str, "Comparison of CPU and GPU matrix multiplication is not accurate at array index %d", breakValue);
	else
		sprintf(str, "Comparison of CPU and GPU vector addition is accurate");

	// Output
	printf("Time taken for Matrix Multiplication on CPU = %.6f \n", timeOnCPU);
	printf("Time taken for Matrix Multiplication on GPU = %.6f \n", timeOnGPU);
	printf("%s \n", str);

	// Cleanup
	cleanup();

	return 0;
}

void InitA(int* data, int row, int col)
{
	// Local Variable Declarations
	int num = 1;

	// Code
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			*(data + i * col + j) = num;
			num++;
		}
	}
}

void InitB(int* data, int row, int col)
{
	// Local Variable Declarations
	int num = BLOCK_WIDTH;

	// Code
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			*(data + i * col + j) = num;
			num--;
		}
	}
}

void matMulCPU(int* A, int* B, int* C, int numARows, int numAColumns, int numBColumns, int numCColumns)
{
	// Code
	StopWatchInterface* timer = NULL;
	sdkCreateTimer(&timer);
	sdkstartTimer(&timer);

	for (int i = 0; i < numARows; i++)
	{
		for (int j = 0; j < numBColumns; j++)
		{
			int value = 0;
			for (int k = 0; k < numAColumns; k++)
			{
				int a = A[i * numAColumns + k];
				int b = B[k * numBColumns + j];
				value += a * b;
			}
			C[i * numCColumns + j] = value;
		}
	}

	sdkStopTimer(&timer);
	timeOnCPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
	timer = NULL;
}

void cleanup(void)
{
	// Code
	if (deviceC)
	{
		clReleaseMemObject(deviceC);
		deviceC = NULL;
	}

	if (deviceB)
	{
		clReleaseMemObject(deviceB);
		deviceB = NULL;
	}

	if (deviceA)
	{
		clReleaseMemObject(deviceA);
		deviceA = NULL;
	}

	if (oclKernel)
	{
		clReleaseKernel(oclKernel);
		oclKernel = NULL;
	}

	if (oclProgram)
	{
		clReleaseProgram(oclProgram);
		oclProgram = NULL;
	}

	if (oclCommandQueue)
	{
		clReleaseCommandQueue(oclCommandQueue);
		oclCommandQueue = NULL;
	}

	if (oclContext)
	{
		clReleaseContext(oclContext);
		oclContext = NULL;
	}

	if (gold)
	{
		free(gold);
		gold = NULL;
	}

	if (hostC)
	{
		free(hostC);
		hostC = NULL;
	}

	if (hostB)
	{
		free(hostB);
		hostB = NULL;
	}

	if (hostA)
	{
		free(hostA);
		hostA = NULL;
	}
}
