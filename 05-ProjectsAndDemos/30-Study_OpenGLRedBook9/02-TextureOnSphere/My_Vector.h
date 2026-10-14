#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//struct defination
typedef struct vec_2d_int
{
	GLint* p_arr;
	size_t size;
}vec_2d_int_t;

typedef struct vec_2d_float
{
	GLfloat* p_arr;
	size_t size;
}vec_2d_float_t;

// vector routines implementation
vec_2d_int_t* create_vec_2d_int(void)
{
	// functio prototype
	void* xcalloc(int num_elements, size_t size_per_elements);

	// code
	return (vec_2d_int_t*)xcalloc(1, sizeof(vec_2d_int_t));
}

vec_2d_float_t* create_vec_2d_float(void)
{
	// function prototype
	void* xcalloc(int num_elements, size_t size_per_elements);

	//code
	return (vec_2d_float_t*)xcalloc(1, sizeof(vec_2d_float_t));
}

void push_back_vec_2d_int(vec_2d_int_t* p_vec, GLint p_arr)
{
	// function prototype
	void* xrealloc(void* p, size_t new_size);

	// code
	p_vec->p_arr = (GLint*)xrealloc(p_vec->p_arr,(p_vec->size + 1) * sizeof(int*));

	p_vec->size++;
	p_vec->p_arr[p_vec->size - 1] = p_arr;
}

void push_back_vec_2d_float(vec_2d_float_t* p_vec, GLfloat p_arr)
{
	// function prototype
	void* xrealloc(void* p, size_t new_size);

	// code
	p_vec->p_arr = (GLfloat*)xrealloc(p_vec->p_arr,(p_vec->size + 1) * sizeof(GLfloat*));

	p_vec->size++;
	p_vec->p_arr[p_vec->size - 1] = p_arr;
}

void clean_vec_2d_int(vec_2d_int_t* p_vec)
{
	// code

	free(p_vec->p_arr);
	free(p_vec);
	p_vec = NULL;
}

void clean_vec_2d_float(vec_2d_float_t* p_vec)
{
	// code

	free(p_vec->p_arr);
	free(p_vec);
	p_vec = NULL;
}

void* xcalloc(int num_elemnets, size_t size_per_element)
{
	// function prototype
	void UnInitialize(void);

	// code
	void* p = calloc(num_elemnets, size_per_element);
	if (!p)
	{
		
		
	}
	return (p);
}

void* xrealloc(void* p, size_t new_size)
{

	// function prototype
	void UnInitialize(void);

	// code
	void* ptr = realloc(p, new_size);
	if (!ptr)
	{
		
		
	}
	return (ptr);
}
