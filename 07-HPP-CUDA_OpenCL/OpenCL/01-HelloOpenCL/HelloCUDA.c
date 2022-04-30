// Header Files
// Standard Headers
#include <stdio.h>
#include <stdlib.h>

// OpeenCL Headers
#include <CL/opencl.h>		

// Global Variables
const int iNumberOfArrayElements = 5;

cl_platform_id oclPlatformID;
cl_device_id oclDeviceID;

cl_context oclContext;
cl_command_queue oclCommandQueue;

cl_program oclProgram;
cl_kernel oclKernel;

float* hostInput1 = NULL;
float* hostInput2 = NULL;
float* hostOutput = NULL;

cl_mem deviceInput1 = NULL;
cl_mem deviceInput2 = NULL;
cl_mem deviceOutput = NULL;

// OpenCL Kernel
const char* oclSourceCode =
"__kernel void vecAddGPU(__global float* in1, __global float* in2, __global float* out, int len)"\
"{"\

"int i = get_global_id(0);"\

"if (i < len)"\
"{"\
"out[i] = in1[i] + in2[i];"\
"}"\
"}";

// Entry-Point Function
int main(void)
{
	// Function Declarations
	void cleanup(void);

	// Variable Declarations
	int size = iNumberOfArrayElements * sizeof(float);
	cl_int result;

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
	oclProgram = clCreateProgramWithSource(oclContext, 1,(const char **)&oclSourceCode, NULL, &result);
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
	oclKernel = clCreateKernel(oclProgram, "vecAddGPU", &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateKernel() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Device Memory Allocation
	deviceInput1 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for deviceInput1 array : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	deviceInput2 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for deviceInput2 array : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	deviceOutput = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &result);
	if (result != CL_SUCCESS)
	{
		printf("clCreateBuffer() failed for deviceOutput array : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 0th Argument i.e. deviceInput1
	result = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&deviceInput1);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 1st Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}
	
	// Set 0 based 1th Argument i.e. deviceInput2
	result = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void*)&deviceInput2);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 2nd Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 2nd Argument i.e. deviceOutput
	result = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void*)&deviceOutput);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 3rd Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Set 0 based 3rd Argument i.e. len
	result = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void*)&iNumberOfArrayElements);
	if (result != CL_SUCCESS)
	{
		printf("clSetKernelArg() Failed For 4th Argument : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Write Above 'input' Device buffer To Device Memory
	result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput1, CL_FALSE, 0, size, hostInput1, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueWriteBuffer() Failed For 1st Input Device buffer : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput2, CL_FALSE, 0, size, hostInput2, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueWriteBuffer() Failed For 2nd Input Device buffer : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Kernel Configuration
	size_t global_size = 5;			// 1-D 5 Element Array Operation
	result = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueNDRangeKernel() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}

	// Finish OpenCL Command Queue
	clFinish(oclCommandQueue);

	// Read Back Result From The Device (i.e. from deviceOutput) into CPU variable (i.e. hostOutput)
	result = clEnqueueReadBuffer(oclCommandQueue, deviceOutput, CL_TRUE, 0, size, hostOutput, 0, NULL, NULL);
	if (result != CL_SUCCESS)
	{
		printf("clEnqueueReadBuffer() Failed : %d \n", result);
		cleanup();
		exit(EXIT_FAILURE);
	}
	
	// Display Results
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
		clReleaseMemObject(deviceOutput);
		deviceOutput = NULL;
	}

	if (deviceInput2)
	{
		clReleaseMemObject(deviceInput2);
		deviceInput2 = NULL;
	}

	if (deviceInput1)
	{
		clReleaseMemObject(deviceInput1);
		deviceInput1 = NULL;
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
