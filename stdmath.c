/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "stdmath.h"
#include "stdvec.h"
#include "blenlib.h"
#include <math.h>
#define _PI 3.14159655f

// MAT3

void mn_mat3_set( t_mn_mat3 *mat, float m[3][3])
{
	mat->m[0][0] = m[0][0];
	mat->m[0][1] = m[0][1];
	mat->m[0][2] = m[0][2];
	mat->m[1][0] = m[1][0];
	mat->m[1][1] = m[1][1];
	mat->m[1][2] = m[1][2];
	mat->m[2][0] = m[2][0];
	mat->m[2][1] = m[2][1];
	mat->m[2][2] = m[2][2];
}

t_mn_mat3 *mn_mat3_new( float m[3][3])
{
	t_mn_mat3 *mat = ( t_mn_mat3 *) calloc( 1, sizeof( t_mn_mat3));
	mn_mat3_set( mat, m);
	return mat;
}

void mn_mat3_print( t_mn_mat3 *mat)
{
	printf("[%f %f %f]\n", mat->m[0][0], mat->m[0][1], mat->m[0][2]);
	printf("[%f %f %f]\n", mat->m[1][0], mat->m[1][1], mat->m[1][2]);
	printf("[%f %f %f]\n", mat->m[2][0], mat->m[2][1], mat->m[2][2]);
}

// MAT4

void mn_mat4_set( t_mn_mat4 *mat, float m[4][4])
{
	mat->m[0][0] = m[0][0];
	mat->m[0][1] = m[0][1];
	mat->m[0][2] = m[0][2];
	mat->m[0][3] = m[0][3];

	mat->m[1][0] = m[1][0];
	mat->m[1][1] = m[1][1];
	mat->m[1][2] = m[1][2];
	mat->m[1][3] = m[1][3];

	mat->m[2][0] = m[2][0];
	mat->m[2][1] = m[2][1];
	mat->m[2][2] = m[2][2];
	mat->m[2][3] = m[2][3];

	mat->m[3][0] = m[3][0];
	mat->m[3][1] = m[3][1];
	mat->m[3][2] = m[3][2];
	mat->m[3][3] = m[3][3];
}

void mn_mat4_free( t_mn_mat4 *mat)
{
	free( mat);
}

t_mn_mat4 *mn_mat4_new( float m[4][4])
{
	t_mn_mat4 *mat = ( t_mn_mat4 *) calloc( 1, sizeof( t_mn_mat4));
	mn_mat4_set( mat, m);
	return mat;
}

void mn_mat4_print( t_mn_mat4 *mat)
{
	printf("[%f %f %f %f]\n", mat->m[0][0], mat->m[0][1], mat->m[0][2], mat->m[0][3]);
	printf("[%f %f %f %f]\n", mat->m[1][0], mat->m[1][1], mat->m[1][2], mat->m[1][3]);
	printf("[%f %f %f %f]\n", mat->m[2][0], mat->m[2][1], mat->m[2][2], mat->m[2][3]);
	printf("[%f %f %f %f]\n", mat->m[3][0], mat->m[3][1], mat->m[3][2], mat->m[3][3]);
}

// VECTORS

t_mn_vector *mn_vector_new( float x, float y, float z)
{
	t_mn_vector *vector = ( t_mn_vector *) calloc( 1, sizeof( t_mn_vector));
	vector->x = x;
	vector->y = y;
	vector->z = z;
	return vector;
}

void mn_vector_free( t_mn_vector *v)
{
	free(v);
}

void mn_vector_add( t_mn_vector *self, t_mn_vector *v)
{
	self->x += v->x;
	self->y += v->y;
	self->z += v->z;
}

void mn_vector_sub( t_mn_vector *self, t_mn_vector *v)
{
	self->x -= v->x;
	self->y -= v->y;
	self->z -= v->z;
}

void mn_vector_mul( t_mn_vector *self, float i)
{
	self->x *= i;
	self->y *= i;
	self->z *= i;
}

t_mn_vector *mn_vector_cross( t_mn_vector *v1, t_mn_vector *v2)
{
	float _r[3] = {0,0,0};
	const float _v1[3] = {v1->x,v1->y,v1->z};
	const float _v2[3] = {v2->x,v2->y,v2->z};
	cross_v3_v3v3( _r, _v1, _v2);
	t_mn_vector *r = mn_vector_new(_r[0],_r[1],_r[2]);
	return r;
}

void mn_vector_normalize( t_mn_vector *self)
{
	float v[3] = { self->x, self->y, self->z};
	normalize_v3(v);
	self->x = v[0];
	self->y = v[1];
	self->z = v[2];
}

float mn_vector_length( t_mn_vector *self)
{
	float v[] = {self->x, self->y, self->z};
	return len_v3( v);
}

float mn_vector_angle2d( t_mn_vector *self, float x, float y)
{
	const float v1[] = {x,y};
	float v2[] = {self->x, self->y, self->z};
	float a = angle_v2v2(v1,v2);
	return a;
}

void mn_vector_print( t_mn_vector *v)
{
	printf("[%f %f %f]\n", v->x, v->y, v->z);
}

void mn_vector_print_with_name( t_mn_vector *v, const char *name)
{
	printf("%s [%f %f %f]\n", name, v->x, v->y, v->z);
}

t_mn_vector *mn_vector_copy( t_mn_vector *self)
{
	return mn_vector_new(self->x, self->y, self->z);
}

float *std_circle(float center[3],double radius,int divisions)
{
	float *points = (float *)malloc(sizeof(float)*divisions*3);
	int i;
	int j=0;

	double delta = (double)((double)(_PI * 2) / divisions);

	for (i=0;i<divisions;i++)
	{
		float r[3];
		float result[3];
		vset(r,cos(i*delta),sin(i*delta),0);
		mul_v3_fl(r,(float)radius);
		add_v3_v3v3(result,center,r);
		points[j]=result[0];
		points[j+1]=result[1];
		points[j+2]=result[2];
		j+=3;
	}

	return points;
}

float *std_arc(float center[3],double radius,int divisions, int start, int end)
{
	float *points = (float *)malloc(sizeof(float)*(divisions*3));
	int i;
	int j=0;

	int angle = end - start;
	float phi = deg_to_rad(angle);

	double delta = (double)((double)(phi ) / (divisions -1 ));
	double a = 0;

	for (i=0;i<divisions;i++)
	{
		float r[3];
		float result[3];
		vset(r,cos(a),sin(a),0);
		a += delta;

		mul_v3_fl(r,(float)radius);
		add_v3_v3v3(result,center,r);
		points[j]=result[0];
		points[j+1]=result[1];
		points[j+2]=result[2];
		j+=3;
	}

	return points;
}

