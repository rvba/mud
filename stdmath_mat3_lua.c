/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#ifdef HAVE_LUA_5_1
#include "compat-5.3.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "stdmath.h"
#include "stdmath_lua.h"

// CHECK

static t_lua_mat3 *lua_mat3_check( lua_State *L, int index)
{
	t_lua_mat3 *m;
	luaL_checktype( L, index, LUA_TUSERDATA);
	m = ( t_lua_mat3 *) luaL_checkudata( L, index, L_MAT3);
	if ( m == NULL) luaL_error( L, "Mat3 type error");
	return m;
}

static int lua_mat3_new( lua_State *L)
{
	float a = luaL_checknumber( L, 1);
	float b = luaL_checknumber( L, 2);
	float c = luaL_checknumber( L, 3);

	float d = luaL_checknumber( L, 4);
	float e = luaL_checknumber( L, 5);
	float f = luaL_checknumber( L, 6);

	float g = luaL_checknumber( L, 7);
	float h = luaL_checknumber( L, 8);
	float i = luaL_checknumber( L, 9);

	float m[3][3] = { {a, b, c}, {d, e, f}, {g, h, i}};

	t_mn_mat3 *mat = mn_mat3_new( m);
	t_lua_mat3 *lua_mat = ( t_lua_mat3 *) lua_newuserdata( L, sizeof( t_lua_mat3));
	lua_mat->m = mat;
	luaL_setmetatable( L, L_MAT3); 

	return 1;
}

static int lua_mat3_print( lua_State *L)
{
	t_lua_mat3 *mat = lua_mat3_check( L, 1);
	mn_mat3_print( mat->m);
	return 0;
}

static const struct luaL_Reg mat3_methods[] =
{
	{"print", lua_mat3_print},
	{ NULL, NULL}
};

static const struct luaL_Reg mat3[] = 
{
	{"new", lua_mat3_new},
	{ NULL, NULL}
};

void lua_make_table_mat3( lua_State *L)
{
	luaL_newmetatable( L, L_MAT3);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, mat3_methods, 0);
}

LUALIB_API int luaopen_mat3( lua_State *L)
{
	lua_make_table_mat3( L);
	luaL_newlib( L, mat3);
	return 1;
}
