#pragma once

#include "My_Vector.h"

__declspec(dllexport) void InitSphereVertices(float radius, float slices, float stacks, FILE* gpFile, vec_2d_float_t** sphereVertices, vec_2d_float_t** sphereNormals, vec_2d_float_t** sphereTexcoords, vec_2d_int_t** sphereIndices);

__declspec(dllexport) int GetNumberOfSphereVertices();

__declspec(dllexport) int GetNumberOfSphereIndices();
