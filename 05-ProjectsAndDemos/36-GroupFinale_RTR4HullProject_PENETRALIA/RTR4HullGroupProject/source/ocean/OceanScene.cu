//#include <cuda_runtime.h>
//#include <cufft.h>
//#include <math_constants.h>

#include"Ocean.h"
#include"OceanShader.h"
#include<cuda_runtime.h>
#include<cuda_gl_interop.h>
#include <cufft.h>
#include <math_constants.h>
#include <model/BoatModel.h>
//#include "Ocean.cu.h"


const unsigned int meshSize = 1024;
const unsigned int spectrumW = meshSize + 4;
const unsigned int spectrumH = meshSize + 1;

// simulation parameters
const float g = 9.81f;        // gravitational constant
const float A = 1e-7f;        // wave scale factor
const float patchSize = 100;  // patch size
float windSpeed = 5.0f;
float windDir = CUDART_PI_F / 3.0f;
float dirDepend = 0.07f;

#define OCEAN_HEIGHT_SCALE 1.0f
#define OCEAN_SCALE_X 2048.0f
#define OCEAN_SCALE_Y 1000.0f
#define OCEAN_SCALE_Z 2048.0f
#define OCEAN_TRANSLATE_Y 0.0f

cudaError_t cudaResult;
struct cudaGraphicsResource* graphicsResource = NULL;

GLuint vao;

GLuint posVertexBuffer;
GLuint indexBuffer;
GLuint heightVertexBuffer, slopeVertexBuffer;
struct cudaGraphicsResource* cuda_posVB_resource, * cuda_heightVB_resource,
	* cuda_slopeVB_resource;  // handles OpenGL-CUDA exchange


// FFT data
cufftHandle fftPlan;
float2* d_h0 = 0;  // heightfield at time 0
float2* h_h0 = 0;
float2* d_ht = 0;  // heightfield at time t
float2* d_slope = 0;

// pointers to device object
float* g_hptr = NULL;
float2* g_sptr = NULL;


float animTime = 0.0f;

float h_boatPivotYDeviation;
float h_boatFrontYDeviation;


// Round a / b to nearest higher integer value
int cuda_iDivUp(int a, int b) { return (a + (b - 1)) / b; }

// complex math functions
__device__ float d_boatPivotYDeviation;
__device__ float d_boatFrontYDeviation;

__device__ float2 conjugate(float2 arg) { return make_float2(arg.x, -arg.y); }

__device__ float2 complex_exp(float arg) {
	return make_float2(cosf(arg), sinf(arg));
}

__device__ float2 complex_add(float2 a, float2 b) {
	return make_float2(a.x + b.x, a.y + b.y);
}

__device__ float2 complex_mult(float2 ab, float2 cd) {
	return make_float2(ab.x * cd.x - ab.y * cd.y, ab.x * cd.y + ab.y * cd.x);
}

// generate wave heightfield at time t based on initial heightfield and
// dispersion relationship
__global__ void generateSpectrumKernel(float2* h0, float2* ht,
	unsigned int in_width,
	unsigned int out_width,
	unsigned int out_height, float t,
	float patchSize) {
	unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
	unsigned int in_index = y * in_width + x;
	unsigned int in_mindex =
		(out_height - y) * in_width + (out_width - x);  // mirrored
	unsigned int out_index = y * out_width + x;

	// calculate wave vector
	float2 k;
	k.x = (-(int)out_width / 2.0f + x) * (2.0f * CUDART_PI_F / patchSize);
	k.y = (-(int)out_width / 2.0f + y) * (2.0f * CUDART_PI_F / patchSize);

	// calculate dispersion w(k)
	float k_len = sqrtf(k.x * k.x + k.y * k.y);
	float w = sqrtf(9.81f * k_len);

	if ((x < out_width) && (y < out_height)) {
		float2 h0_k = h0[in_index];
		float2 h0_mk = h0[in_mindex];

		// output frequency-space complex values
		ht[out_index] =
			complex_add(complex_mult(h0_k, complex_exp(w * t)),
				complex_mult(conjugate(h0_mk), complex_exp(-w * t)));
		// ht[out_index] = h0_k;
	}
}

// update height map values based on output of FFT
__global__ void updateHeightmapKernel(float* heightMap, float2* ht,
	unsigned int width, unsigned int boatPivotX, unsigned int boatPivotZ, unsigned int boatFrontX, unsigned int boatFrontZ) {
	unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
	unsigned int i = y * width + x;

	// cos(pi * (m1 + m2))
	float sign_correction = ((x + y) & 0x01) ? -1.0f : 1.0f;

	heightMap[i] = ht[i].x * sign_correction;
	if (x == boatPivotX && y == boatPivotZ)
	{
		d_boatPivotYDeviation = heightMap[i];
	}

	if (x == boatFrontX && y == boatFrontZ)
	{
		d_boatFrontYDeviation = heightMap[i];
	}
}

// update height map values based on output of FFT
__global__ void updateHeightmapKernel_y(float* heightMap, float2* ht,
	unsigned int width) {
	unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
	unsigned int i = y * width + x;

	// cos(pi * (m1 + m2))
	float sign_correction = ((x + y) & 0x01) ? -1.0f : 1.0f;

	heightMap[i] = ht[i].y * sign_correction;
}

// generate slope by partial differences in spatial domain
__global__ void calculateSlopeKernel(float* h, float2* slopeOut,
	unsigned int width, unsigned int height) {
	unsigned int x = blockIdx.x * blockDim.x + threadIdx.x;
	unsigned int y = blockIdx.y * blockDim.y + threadIdx.y;
	unsigned int i = y * width + x;

	float2 slope = make_float2(0.0f, 0.0f);

	if ((x > 0) && (y > 0) && (x < width - 1) && (y < height - 1)) {
		slope.x = h[i + 1] - h[i - 1];
		slope.y = h[i + width] - h[i - width];
	}

	slopeOut[i] = slope;
}



// wrapper functions
void cudaGenerateSpectrumKernel(float2* d_h0, float2* d_ht,
	unsigned int in_width,
	unsigned int out_width,
	unsigned int out_height,
	float animTime, float patchSize) {
	dim3 block(8, 8, 1);
	dim3 grid(cuda_iDivUp(out_width, block.x), cuda_iDivUp(out_height, block.y),
		1);
	generateSpectrumKernel << <grid, block >> > (d_h0, d_ht, in_width, out_width,
		out_height, animTime, patchSize);
}

void cudaUpdateHeightmapKernel(float* d_heightMap, float2* d_ht,
	unsigned int width,
	unsigned int height, 
	unsigned int boatPivotX,
	unsigned int boatPivotZ,
	unsigned int boatFrontX,
	unsigned int boatFrontZ,
	bool autoTest) {
	dim3 block(8, 8, 1);
	dim3 grid(cuda_iDivUp(width, block.x), cuda_iDivUp(height, block.y), 1);
	if (autoTest) {
		updateHeightmapKernel_y << <grid, block >> > (d_heightMap, d_ht, width);
	}
	else {
		updateHeightmapKernel << <grid, block >> > (d_heightMap, d_ht, width, boatPivotX, boatPivotZ, boatFrontX, boatFrontZ);
	}
}

void cudaCalculateSlopeKernel(float* hptr, float2* slopeOut,
	unsigned int width,
	unsigned int height) {
	dim3 block(8, 8, 1);
	dim3 grid2(cuda_iDivUp(width, block.x), cuda_iDivUp(height, block.y), 1);
	calculateSlopeKernel << <grid2, block >> > (hptr, slopeOut, width, height);
}



bool InitializeOcean(void)
{
	// function prototype
	void createVBO(GLuint * vbo, int size);
	void generate_h0(float2 * h0);
	void createMeshPositionVBO(GLuint * id, int w, int h);
	void createMeshIndexBuffer(GLuint * id, int w, int h);
	void createVBOHeight(GLuint * vbo, int size);
	void createVBOSlope(GLuint * vbo, int size);

	InitializeOceanShader();

	// find cuda device
	//Cuda initialization
	int dev_count = 0;
	cudaResult = cudaGetDeviceCount(&dev_count);
	if (cudaResult != cudaSuccess)
	{
		ErrorLog(NULL, "CUDA Runtime API Error - cudaGetDeviceCount() Failed due to %s \n", cudaGetErrorString(cudaResult));
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}
	else if (dev_count == 0)
	{
		ErrorLog(NULL, "There is no CUDA supported device on this system\n");
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);
	}

	//select CUDA supported device
	cudaSetDevice(0);  //selecting the default 0th CUDA supported device


	cufftPlan2d(&fftPlan, meshSize, meshSize, CUFFT_C2C);

	int spectrumSize = spectrumW * spectrumH * sizeof(float2);
	cudaResult = (cudaMalloc((void**)&d_h0, spectrumSize));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}
	h_h0 = (float2*)malloc(spectrumSize);
	generate_h0(h_h0);
	cudaResult = (cudaMemcpy(d_h0, h_h0, spectrumSize, cudaMemcpyHostToDevice));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}

	int outputSize = meshSize * meshSize * sizeof(float2);
	cudaResult = (cudaMalloc((void**)&d_ht, outputSize));
	if (cudaResult != cudaSuccess)
	{
		//fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}
	cudaResult = (cudaMalloc((void**)&d_slope, outputSize));
	if (cudaResult != cudaSuccess)
	{
		//fprintf(gpFile, "CUDA Runtime API Error - cudaMalloc() Failed due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	createMeshPositionVBO(&posVertexBuffer, meshSize, meshSize);
	createMeshIndexBuffer(&indexBuffer, meshSize, meshSize);

	createVBOHeight(&heightVertexBuffer, meshSize * meshSize * sizeof(float));

	createVBOSlope(&slopeVertexBuffer, outputSize);

	glBindVertexArray(0);



	//create CUDA Interoperability resource
	cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_heightVB_resource, heightVertexBuffer, cudaGraphicsMapFlagsWriteDiscard);
	if (cudaResult != cudaSuccess)
	{
		//fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsGLRegisterBuffer() Failed due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}


	cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_slopeVB_resource, slopeVertexBuffer, cudaGraphicsMapFlagsWriteDiscard);
	if (cudaResult != cudaSuccess)
	{
		//fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsGLRegisterBuffer() Failed due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: return(false) instead of exit().
		exit(EXIT_FAILURE);

	}

}


////////////////////////////////////////////////////////////////////////////////
//! Run the Cuda kernels
////////////////////////////////////////////////////////////////////////////////
void runCuda() {

	////fprintf(gpFile, "Start : %s\n", __FUNCTION__);
	void   UninitializeOcean();

	size_t num_bytes;

	// generate wave spectrum in frequency domain
	cudaGenerateSpectrumKernel(d_h0, d_ht, spectrumW, meshSize, meshSize,
		animTime, patchSize);
	cudaError_t err = cudaGetLastError();        // Get error code
	if (err != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Error in cudaGenerateSpectrumKernel: %s\n", cudaGetErrorString(err));
		// TODO_00: Do uninitialization first instead of exit().
		exit(-1);
	}

	// execute inverse FFT to convert to spatial domain
	(cufftExecC2C(fftPlan, d_ht, d_ht, CUFFT_INVERSE));

	// update heightmap values in vertex buffer
	cudaResult = (cudaGraphicsMapResources(1, &cuda_heightVB_resource, 0));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsMapResources() Failed for cuda_heightVB_resource due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: Do uninitialization first instead of exit().
		exit(EXIT_FAILURE);

	}

	cudaResult = (cudaGraphicsResourceGetMappedPointer((void**)&g_hptr, &num_bytes, cuda_heightVB_resource));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsResourceGetMappedPointer() Failed for cuda_heightVB_resource due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: Do uninitialization first instead of exit().
		exit(EXIT_FAILURE);
	}

	unsigned int boatPivotX = (512 + 1340 / 4) - 1;		// TODO_00: Make this value dependent on boat size and current position.
	unsigned int boatPivotZ = (512 + (-880 / 4)) - 1;	// TODO_00: Make this value dependent on boat size and current position.
	unsigned int boatFrontX = (512 + 1340 / 4) - 1;		// TODO_00: Make this value dependent on boat size and current position.
	unsigned int boatFrontZ = (512 + (-890 / 4)) - 1;	// TODO_00: Make this value dependent on boat size and current position.

	cudaUpdateHeightmapKernel(g_hptr, d_ht, meshSize, meshSize, boatPivotX, boatPivotZ, boatFrontX, boatFrontZ, false);
	err = cudaGetLastError();        // Get error code
	if (err != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Error in cudaUpdateHeightmapKernel: %s\n", cudaGetErrorString(err));
		// TODO_00: Do uninitialization first instead of exit().
		exit(-1);
	}

	DebugLog(NULL, "runCuda()::h_boatPivotYDeviation before cudaMemcpyFromSymbol(): %f", h_boatPivotYDeviation);

	err = cudaMemcpyFromSymbol(&h_boatPivotYDeviation, d_boatPivotYDeviation, sizeof(h_boatPivotYDeviation), 0, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		ErrorLog(NULL, "runCuda()::cudaMemcpyFromSymbol() failed for d_boatPivotYDeviation: %s", cudaGetErrorString(err));
		// TODO_00: Do uninitialization first instead of exit().
		exit(-1);
	}
	currentBoatPivotDeviation = vmath::vec3(0.0f, h_boatPivotYDeviation, 0.0f);
	currentBoatPivotDeviation = currentBoatPivotDeviation * OCEAN_HEIGHT_SCALE * OCEAN_SCALE_Y;

	DebugLog(NULL, "runCuda()::h_boatPivotYDeviation after cudaMemcpyFromSymbol(): %f", h_boatPivotYDeviation);

	DebugLog(NULL, "runCuda()::h_boatFrontYDeviation before cudaMemcpyFromSymbol(): %f", h_boatFrontYDeviation);

	err = cudaMemcpyFromSymbol(&h_boatFrontYDeviation, d_boatFrontYDeviation, sizeof(h_boatFrontYDeviation), 0, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		ErrorLog(NULL, "runCuda()::cudaMemcpyFromSymbol() failed for d_boatFrontYDeviation: %s", cudaGetErrorString(err));
		// TODO_00: Do uninitialization first instead of exit().
		exit(-1);
	}
	currentBoatFrontDeviation = vmath::vec3(0.0f, h_boatFrontYDeviation, 0.0f);
	currentBoatFrontDeviation = currentBoatFrontDeviation * OCEAN_HEIGHT_SCALE * OCEAN_SCALE_Y;

	//DebugLog(NULL, "runCuda()::h_boatPivotYDeviation after cudaMemcpyFromSymbol(): %f", h_boatPivotYDeviation);

	// calculate slope for shading
	cudaResult = (cudaGraphicsMapResources(1, &cuda_slopeVB_resource, 0));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsMapResources() Failed for cuda_slopeVB_resource due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: Do uninitialization first instead of exit().
		exit(EXIT_FAILURE);
	}


	cudaResult = (cudaGraphicsResourceGetMappedPointer((void**)&g_sptr, &num_bytes, cuda_slopeVB_resource));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsResourceGetMappedPointer() Failed for cuda_slopeVB_resource due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: Do uninitialization first instead of exit().
		exit(EXIT_FAILURE);
	}

	cudaCalculateSlopeKernel(g_hptr, g_sptr, meshSize, meshSize);
	err = cudaGetLastError();        // Get error code
	if (err != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Error in cudaCalculateSlopeKernel: %s\n", cudaGetErrorString(err));
		// TODO_00: Do uninitialization first instead of exit().
		exit(-1);
	}

	//DebugLog(NULL, "runCuda()::h_boatInclination before cudaMemcpyFromSymbol(): x: %f, y: %f", h_boatInclination[0], h_boatInclination[1]);

	//err = cudaMemcpyFromSymbol(&h_boatInclination, d_boatInclination, sizeof(h_boatInclination), 0, cudaMemcpyDeviceToHost);
	//if (err != cudaSuccess)
	//{
	//	ErrorLog(NULL, "runCuda()::cudaMemcpyFromSymbol() failed for d_boatInclination: %s", cudaGetErrorString(err));
	//	// TODO_00: Do uninitialization first instead of exit().
	//	exit(-1);
	//}
	//currentBoatPivotDeviation = vmath::vec3(0.0f, h_boatPivotYDeviation, 0.0f);
	//currentBoatPivotDeviation = currentBoatPivotDeviation * OCEAN_HEIGHT_SCALE * OCEAN_SCALE_Y;

	//DebugLog(NULL, "runCuda()::h_boatInclination after cudaMemcpyFromSymbol(): x: %f, y: %f", h_boatInclination[0], h_boatInclination[1]);


	cudaResult = (cudaGraphicsUnmapResources(1, &cuda_heightVB_resource, 0));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsUnmapResources() Failed for cuda_heightVB_resource due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: Do uninitialization first instead of exit().
		exit(EXIT_FAILURE);
	}

	cudaResult = (cudaGraphicsUnmapResources(1, &cuda_slopeVB_resource, 0));
	if (cudaResult != cudaSuccess)
	{
		////fprintf(gpFile, "CUDA Runtime API Error - cudaGraphicsResourceGetMappedPointer() Failed for cuda_slopeVB_resource due to %s \n", cudaGetErrorString(cudaResult));
		UninitializeOcean();
		// TODO_00: Do uninitialization first instead of exit().
		exit(EXIT_FAILURE);
	}

}



// Generate base heightfield in frequency space
void generate_h0(float2* h0) {
	float phillips(float Kx, float Ky, float Vdir, float V, float A,
		float dir_depend);
	float gauss();

	for (unsigned int y = 0; y <= meshSize; y++) {
		for (unsigned int x = 0; x <= meshSize; x++) {
			float kx = (-(int)meshSize / 2.0f + x) * (2.0f * CUDART_PI_F / patchSize);
			float ky = (-(int)meshSize / 2.0f + y) * (2.0f * CUDART_PI_F / patchSize);

			float P = sqrtf(phillips(kx, ky, windDir, windSpeed, A, dirDepend));

			if (kx == 0.0f && ky == 0.0f) {
				P = 0.0f;
			}

			// float Er = urand()*2.0f-1.0f;
			// float Ei = urand()*2.0f-1.0f;
			float Er = gauss();
			float Ei = gauss();

			float h0_re = Er * P * CUDART_SQRT_HALF_F;
			float h0_im = Ei * P * CUDART_SQRT_HALF_F;

			int i = y * spectrumW + x;
			h0[i].x = h0_re;
			h0[i].y = h0_im;
		}
	}
}


float urand() { return rand() / (float)RAND_MAX; }

// Generates Gaussian random number with mean 0 and standard deviation 1.
float gauss() {
	float u1 = urand();
	float u2 = urand();

	if (u1 < 1e-6f) {
		u1 = 1e-6f;
	}

	return sqrtf(-2 * logf(u1)) * cosf(2 * CUDART_PI_F * u2);
}


// Phillips spectrum
// (Kx, Ky) - normalized wave vector
// Vdir - wind angle in radians
// V - wind speed
// A - constant
float phillips(float Kx, float Ky, float Vdir, float V, float A,
	float dir_depend) {
	float k_squared = Kx * Kx + Ky * Ky;

	if (k_squared == 0.0f) {
		return 0.0f;
	}

	// largest possible wave from constant wind of velocity v
	float L = V * V / g;

	float k_x = Kx / sqrtf(k_squared);
	float k_y = Ky / sqrtf(k_squared);
	float w_dot_k = k_x * cosf(Vdir) + k_y * sinf(Vdir);

	float phillips = A * expf(-1.0f / (k_squared * L * L)) /
		(k_squared * k_squared) * w_dot_k * w_dot_k;

	// filter out waves moving opposite to wind
	if (w_dot_k < 0.0f) {
		phillips *= dir_depend;
	}

	// damp out waves with very small length w << l
	// float w = L / 10000;
	// phillips *= expf(-k_squared * w * w);

	return phillips;
}


// create index buffer for rendering quad mesh
void createMeshIndexBuffer(GLuint* id, int w, int h) {
	int size = ((w * 2) + 2) * (h - 1) * sizeof(GLuint);

	// create index buffer
	glGenBuffers(1, id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

	// fill with indices for rendering mesh as triangle strips
	GLuint* indices =
		(GLuint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

	if (!indices) {
		return;
	}

	for (int y = 0; y < h - 1; y++) {
		for (int x = 0; x < w; x++) {
			*indices++ = y * w + x;
			*indices++ = (y + 1) * w + x;
		}

		// start new strip with degenerate triangle
		*indices++ = (y + 1) * w + (w - 1);
		*indices++ = (y + 1) * w;
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// create fixed vertex buffer to store mesh vertices
void createMeshPositionVBO(GLuint* id, int w, int h) {

	void createVBO(GLuint * vbo, int size);
	createVBO(id, w * h * 4 * sizeof(float));

	glBindBuffer(GL_ARRAY_BUFFER, *id);
	float* pos = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	if (!pos) {
		return;
	}

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			float u = x / (float)(w - 1);
			float v = y / (float)(h - 1);
			*pos++ = u * 2.0f - 1.0f;
			*pos++ = 0.0f;
			*pos++ = v * 2.0f - 1.0f;
			*pos++ = 1.0f;
		}
	}

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


////////////////////////////////////////////////////////////////////////////////
//! Create VBO
////////////////////////////////////////////////////////////////////////////////
void createVBO(GLuint* vbo, int size) {
	// create buffer object
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void createVBOHeight(GLuint* vbo, int size) {
	// create buffer object
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_HEIGHT, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_HEIGHT);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}


void createVBOSlope(GLuint* vbo, int size) {
	// create buffer object
	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXTURE1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXTURE1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}




void DrawOcean(void)
{
	runCuda();

	//transformation
	mat4 modelMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();

	mat4 translationMatrix = vmath::translate(0.0f, OCEAN_TRANSLATE_Y, 0.0f);
	mat4 scaleMatrix = vmath::scale(OCEAN_SCALE_X, OCEAN_SCALE_Y, OCEAN_SCALE_Z);
	modelMatrix = translationMatrix *scaleMatrix;

	/*vec4 deepColor = vec4(0.019607f, 0.074509f, 0.858823f, 1.0f);
	vec4 shallowColor = vec4(0.019607f, 0.749019f, 0.584313f, 1.0f);
	vec4 skyColor = vec4(0.725490f, 0.909803f, 0.988235f, 1.0f);*/
	vec4 deepColor = vec4(0.0f, 0.1f, 0.4f, 1.0f);
	vec4 shallowColor = vec4(0.1f, 0.3f, 0.3f, 1.0f);
	vec4 skyColor = vec4(0.0f, 0.0f, 0.8f, 1.0f);

	SetOceanShader(modelMatrix, OCEAN_HEIGHT_SCALE, 1.0f, vec2(meshSize, meshSize),
		deepColor, shallowColor, skyColor,vec3(0.0f, 1.0f, 0.0f));

	// render from the vbo
	glBindBuffer(GL_ARRAY_BUFFER, posVertexBuffer);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, heightVertexBuffer);
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(1, GL_FLOAT, 0, 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, slopeVertexBuffer);
	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Bind VAO
	glBindVertexArray(vao);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	//glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLE_STRIP, ((meshSize * 2) + 2) * (meshSize - 1),
		GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	UnsetOceanShader();

	animTime += 0.01;
}

void UpdateOcean(void)
{
	animTime += 0.01;
}

void UninitializeOcean(void)
{
	UninitializeOceanShader();
}
