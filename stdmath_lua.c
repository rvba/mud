/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lua_util.h"

#include <stdlib.h>
#include <string.h>

#include "stdmath.h"
#include "stdmath_lua.h"


// CHECK

static t_lua_vector *lua_vector_check( lua_State *L, int index)
{
	t_lua_vector *v;
	luaL_checktype( L, index, LUA_TUSERDATA);
	v = ( t_lua_vector *) luaL_checkudata( L, index, L_VECTOR);
	if ( v == NULL) luaL_error( L, "Vector type error");
	return v;
}

// PUSH

/*
static t_lua_vector *lua_vector_push( lua_State *L, t_mn_vector *vector)
{
	t_lua_vector *v = ( t_lua_vector *) lua_newuserdata( L, sizeof( t_lua_vector));
	v->v = vector;
	luaL_getmetatable( L, L_VECTOR);
	lua_setmetatable( L, -2);
	return v;
}
*/

// STONE

void _lua_vector_new( lua_State *L, float x, float y, float z)
{
	t_mn_vector *vector = mn_vector_new( x,y,z);
	t_lua_vector *lua_vector = ( t_lua_vector *) lua_newuserdata( L, sizeof( t_lua_vector));
	lua_vector->v = vector;
	luaL_setmetatable( L, L_VECTOR); 
}

static int lua_vector_new( lua_State *L)
{
	float x = luaL_checknumber( L, 1);
	float y = luaL_checknumber( L, 2);
	float z = luaL_checknumber( L, 3);

	_lua_vector_new( L, x, y, z);

	return 1;
}

static int lua_vector_add( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	t_lua_vector *v = ( t_lua_vector *) luaL_checkudata( L, 2, L_VECTOR);
	mn_vector_add( self->v, v->v);
	return 0;
}

static int lua_vector_sub( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	t_lua_vector *v = ( t_lua_vector *) luaL_checkudata( L, 2, L_VECTOR);
	mn_vector_sub( self->v, v->v);
	return 0;
}

static int lua_vector_mul( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	float i = luaL_checknumber( L, 2);
	mn_vector_mul( self->v, i);
	return 0;
}

static int lua_vector_cross( lua_State *L)
{
	t_lua_vector *v1 = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	t_lua_vector *v2 = ( t_lua_vector *) luaL_checkudata( L, 2, L_VECTOR);
	t_mn_vector *v = mn_vector_cross( v1->v, v2->v);
	_lua_vector_new( L, v->x, v->y, v->z);
	mn_vector_free(v);
	return 1;
}

static int lua_vector_length( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	lua_pushnumber( L, mn_vector_length( self->v));
	return 1;
}

static int lua_vector_angle2d( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	float x = luaL_checknumber(L, 2);
	float y = luaL_checknumber(L, 3);
	float a = mn_vector_angle2d(self->v,x,y);
	lua_pushnumber( L, a);
	return 1;
}

static int lua_vector_norm( lua_State *L)
{
	t_lua_vector *v = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	mn_vector_normalize( v->v);
	return 0;
}

static int lua_vector_copy( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	t_mn_vector *vector = mn_vector_new( self->v->x,self->v->y,self->v->z);
	t_lua_vector *lua_vector = ( t_lua_vector *) lua_newuserdata( L, sizeof( t_lua_vector));
	lua_vector->v = vector;
	luaL_setmetatable( L, L_VECTOR); 
	return 1;
}

/* get */

static int lua_vector_get_x( lua_State *L, void *v)
{
	t_lua_vector *vec = ( t_lua_vector *) v;
	lua_pushnumber( L, vec->v->x);
	return 1;
}

static int lua_vector_get_y( lua_State *L, void *v)
{
	t_lua_vector *vec = ( t_lua_vector *) v;
	lua_pushnumber( L, vec->v->y);
	return 1;
}

static int lua_vector_get_z( lua_State *L, void *v)
{
	t_lua_vector *vec = ( t_lua_vector *) v;
	lua_pushnumber( L, vec->v->z);
	return 1;
}

/* set */

static int lua_vector_set_x( lua_State *L, void *v)
{
	t_lua_vector *vec = ( t_lua_vector *) v;
	float val = luaL_checknumber( L, 3);
	vec->v->x = val;
	return 0;
}

static int lua_vector_set_y( lua_State *L, void *v)
{
	t_lua_vector *vec = ( t_lua_vector *) v;
	float val = luaL_checknumber( L, 3);
	vec->v->y = val;
	return 0;
}

static int lua_vector_set_z( lua_State *L, void *v)
{
	t_lua_vector *vec = ( t_lua_vector *) v;
	float val = luaL_checknumber( L, 3);
	vec->v->z = val;
	return 0;
}

static int lua_vector_print( lua_State *L)
{
	t_lua_vector *v = lua_vector_check( L, 1);
	mn_vector_print(v->v);
	return 0;
}

static int lua_vector_print_with_name( lua_State *L)
{
	t_lua_vector *v = lua_vector_check( L, 1);
	const char *name = luaL_checkstring( L, 2);
	mn_vector_print_with_name(v->v, name);
	return 0;
}

static const struct luaL_Reg vector_methods[] =
{
	{"add", lua_vector_add},
	{"sub", lua_vector_sub},
	{"mul", lua_vector_mul},
	{"norm", lua_vector_norm},
	{"length", lua_vector_length},
	{"angle2d", lua_vector_angle2d},
	{"copy", lua_vector_copy},
	{"print", lua_vector_print},
	{"print_with_name", lua_vector_print_with_name},
	{ NULL, NULL}
};

static const struct luaL_Reg stdmath[] = 
{
	{"new", lua_vector_new},
	{"cross", lua_vector_cross},
	{ NULL, NULL}
};

static const Xet_reg_pre vector_getters[] = {
	{"x",    lua_vector_get_x, 0 },
	{"y",    lua_vector_get_y, 0 },
	{"z",    lua_vector_get_z, 0 },
	{0,0}
};

static const Xet_reg_pre vector_setters[] = {
	{"x",    lua_vector_set_x, 0 },
	{"y",    lua_vector_set_y, 0 },
	{"z",    lua_vector_set_z, 0 },
	{0,0}
};

void lua_make_table_vector( lua_State *L)
{
	int methods, metatable;

	luaL_newmetatable( L, L_VECTOR);

	metatable = lua_gettop(L);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, vector_methods, 0);

	methods = lua_gettop(L);

	lua_set_getters_setters( L, methods, metatable, vector_getters, vector_setters);
}

void lua_stdmath_register( lua_State *L)
{
	lua_make_table_vector( L);
	lua_setConst(L,VEC_X);
	lua_setConst(L,VEC_Y);
	lua_setConst(L,VEC_Z);
	luaL_newlib( L, stdmath);
	lua_setglobal( L, "smath");
}

LUALIB_API int luaopen_stdmath( lua_State *L)
{
	lua_make_table_vector( L);
	lua_setConst(L,VEC_X);
	lua_setConst(L,VEC_Y);
	lua_setConst(L,VEC_Z);
	luaL_newlib( L, stdmath);
	return 1;
}
