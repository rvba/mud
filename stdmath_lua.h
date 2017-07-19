/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __STD_MATH_LUA_H__
#define __STD_MATH_LUA_H__

// ==========================================================
// VECTORS
// ==========================================================

#define L_VECTOR "L_VECTOR"
#define VEC_X 1
#define VEC_Y 2
#define VEC_Z 3

struct MN_Vector;
struct MN_MAT4;
struct MN_MAT3;

typedef struct Lua_Vector
{
	struct MN_Vector *v;

}t_lua_vector;


// STDMATH
void lua_stdmath_register( struct lua_State *L);

// ==========================================================
// MAT4
// ==========================================================

#define L_MAT4 "L_MAT4"

typedef struct Lua_Mat4
{
	struct MN_MAT4 *m;

}t_lua_mat4;

t_lua_mat4 *lua_mat4_check( lua_State *L, int index);
void lua_mat4_register( lua_State *L);


// ==========================================================
// MAT3
// ==========================================================

#define L_MAT3 "L_MAT3"

typedef struct Lua_Mat3
{
	struct MN_MAT3 *m;

}t_lua_mat3;

#endif
