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

static int lua_vector_new( lua_State *L)
{
	float x = luaL_checknumber( L, 1);
	float y = luaL_checknumber( L, 2);
	float z = luaL_checknumber( L, 3);

	t_mn_vector *vector = mn_vector_new( x,y,z);
	t_lua_vector *lua_vector = ( t_lua_vector *) lua_newuserdata( L, sizeof( t_lua_vector));
	lua_vector->v = vector;
	luaL_setmetatable( L, L_VECTOR); 

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

static int lua_vector_length( lua_State *L)
{
	t_lua_vector *self = ( t_lua_vector *) luaL_checkudata( L, 1, L_VECTOR);
	lua_pushnumber( L, mn_vector_length( self->v));
	return 1;
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

static int lua_vector_get( lua_State *L)
{
	t_lua_vector *v = lua_vector_check( L, 1);
	int val = luaL_checkinteger( L, 2);
	switch( val)
	{
		case VEC_X: lua_pushnumber( L, v->v->x); break;
		case VEC_Y: lua_pushnumber( L, v->v->y); break;
		case VEC_Z: lua_pushnumber( L, v->v->z); break;
		default: lua_pushnumber( L, -1); break;
	}

	return 1;
}

static int lua_vector_print( lua_State *L)
{
	t_lua_vector *v = lua_vector_check( L, 1);
	mn_vector_print(v->v);
	return 0;
}

static const struct luaL_Reg vector_methods[] =
{
	{"add", lua_vector_add},
	{"sub", lua_vector_sub},
	{"mul", lua_vector_mul},
	{"length", lua_vector_length},
	{"get", lua_vector_get},
	{"copy", lua_vector_copy},
	{"print", lua_vector_print},
	{ NULL, NULL}
};

static const struct luaL_Reg stdmath[] = 
{
	{"new", lua_vector_new},
	{ NULL, NULL}
};

static const Xet_reg_pre vector_getters[] = {
	//{"id",   get_int,    offsetof(t_lua_edge,id)   },
	/*
	{"a",    get_vertex_a, 0   },
	{"b",    get_vertex_b, 0   },
	*/
	{0,0}
};

static const Xet_reg_pre vector_setters[] = {
	/*
	{"id",  set_int,    offsetof(t_lua_edge,id)  },
	{"x",    set_number, offsetof(t_lua_edge,x)    },
	{"y",    set_number, offsetof(t_lua_edge,y)    },
	*/
	{0,0}
};

void lua_stdmath_make_meta_vector( lua_State *L, int methods, int metatable)
{
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methods);    /* dup methods table*/
	lua_rawset(L, metatable);     /* hide metatable:
					 metatable.__metatable = methods */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);  /* upvalue index 1 */
	Xet_add(L, vector_getters);     /* fill metatable with getters */
	lua_pushvalue(L, methods);    /* upvalue index 2 */
	lua_pushcclosure(L, index_handler, 2);
	lua_rawset(L, metatable);     /* metatable.__index = index_handler */

	lua_pushliteral(L, "__newindex");
	lua_newtable(L);              /* table for members you can set */
	Xet_add(L, vector_setters);     /* fill with setters */
	lua_pushcclosure(L, newindex_handler, 1);
	lua_rawset(L, metatable);     /* metatable.__newindex = newindex_handler */

	lua_pop(L, 1);                /* drop metatable */
	//return 1;                     /* return methods on the stack */
}

void lua_make_table_vector( lua_State *L)
{
	int methods, metatable;

	luaL_newmetatable( L, L_VECTOR);

	metatable = lua_gettop(L);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, vector_methods, 0);

	methods = lua_gettop(L);

	lua_stdmath_make_meta_vector( L, methods, metatable);
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
