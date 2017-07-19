/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __STDMATH_H__
#define __STDMATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define lua_setConst(L,name) { lua_pushnumber(L,name); lua_setglobal(L,#name);}

struct lua_State;

// MATRICES

typedef struct MN_MAT3
{
	float m[3][3];

}t_mn_mat3;

t_mn_mat3 *mn_mat3_new( float m[3][3]);
void mn_mat3_print( t_mn_mat3 *mat);

typedef struct MN_MAT4
{
	float m[4][4];

}t_mn_mat4;

t_mn_mat4 *mn_mat4_new( float m[4][4]);
void mn_mat4_print( t_mn_mat4 *mat);

// VECTORS

typedef struct MN_Vector
{
	float x;
	float y;
	float z;

}t_mn_vector;

t_mn_vector *mn_vector_new( float x, float y, float z);
void mn_vector_free( t_mn_vector *v);
void mn_vector_add( t_mn_vector *a, t_mn_vector *b);
void mn_vector_sub( t_mn_vector *a, t_mn_vector *b);
void mn_vector_mul( t_mn_vector *self, float i);
void mn_vector_normalize( t_mn_vector *self);
t_mn_vector *mn_vector_cross( t_mn_vector *v1, t_mn_vector *v2);
float mn_vector_length( t_mn_vector *self);
float mn_vector_angle2d( t_mn_vector *self, float x, float y);
void mn_vector_print( t_mn_vector *self);
void mn_vector_print_with_name( t_mn_vector *v, const char *name);
t_mn_vector *mn_vector_copy( t_mn_vector *self);


float *std_circle(float center[3],double radius,int divisions);
float *std_arc(float center[3],double radius,int divisions, int start, int end);


// MAT4
void mn_mat4_set( t_mn_mat4 *mat, float m[4][4]);
void mn_mat4_free( t_mn_mat4 *mat);


#ifdef __cplusplus
}
#endif

#endif

