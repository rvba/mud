/* 
 * Copyright (c) 2016 Milovann Yanatchkov 
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

#include "stone_spline.h"
#include "stone_spline_lua.h"

static int lua_stone_spline_new( lua_State *L)
{
	int degree = luaL_checkinteger( L, 1);
	int dimension = luaL_checkinteger( L, 2);
	int count = luaL_checkinteger( L, 3);

	t_spline *spline = stone_spline_new( degree, dimension, count);
	t_lua_spline *lua_spline = ( t_lua_spline *) lua_newuserdata( L, sizeof( t_lua_spline));
	lua_spline->spline = spline;
	luaL_setmetatable( L, L_SPLINE); 

	return 1;
}

t_lua_spline *lua_spline_get( lua_State * L)
{
	return ( t_lua_spline *) luaL_checkudata( L, 1, L_SPLINE);
}

static int lua_stone_spline_point_set( lua_State *L)
{
	t_lua_spline *spline = lua_spline_get( L);
	int indice = luaL_checkinteger( L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	stone_spline_point_set( spline->spline, indice, x, y,  z);
	return 0;
}

static int lua_stone_spline_eval( lua_State *L)
{
	t_lua_spline *spline = lua_spline_get( L);
	float p = luaL_checknumber( L, 2);
	float r[3];
	stone_spline_eval(spline->spline,p,r);
	lua_pushnumber( L, r[0]);
	lua_pushnumber( L, r[1]);
	lua_pushnumber( L, r[2]);
	return 3;
}

static const struct luaL_Reg spline_methods[] =
{
	{"set_point", lua_stone_spline_point_set},
	{"eval", lua_stone_spline_eval},
	{ NULL, NULL}
};

static const struct luaL_Reg spline[] = 
{
	{"new", lua_stone_spline_new},
	{ NULL, NULL}
};

static const Xet_reg_pre spline_getters[] = {
	/*
	{"x",    lua_vector_get_x, 0 },
	{"y",    lua_vector_get_y, 0 },
	{"z",    lua_vector_get_z, 0 },
	*/
	{0,0}
};

static const Xet_reg_pre spline_setters[] = {
	/*
	{"x",    lua_vector_set_x, 0 },
	{"y",    lua_vector_set_y, 0 },
	{"z",    lua_vector_set_z, 0 },
	*/
	{0,0}
};

void lua_make_table_spline( lua_State *L)
{
	int methods, metatable;

	luaL_newmetatable( L, L_SPLINE);

	metatable = lua_gettop(L);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, spline_methods, 0);

	methods = lua_gettop(L);

	lua_set_getters_setters( L, methods, metatable, spline_getters, spline_setters);
}

void lua_stone_spline_register( lua_State *L)
{
	lua_make_table_spline( L);
	luaL_newlib( L, spline);
	lua_setglobal( L, "spline");
}

LUALIB_API int luaopen_spline( lua_State *L)
{
	lua_make_table_spline( L);
	luaL_newlib( L, spline);
	return 1;
}
