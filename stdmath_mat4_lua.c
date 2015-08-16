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

#include <stdlib.h>
#include <string.h>

#include "stdmath.h"
#include "stdmath_lua.h"

#include "blenlib.h"
#include "stdvec.h"

// CHECK

t_lua_mat4 *lua_mat4_check( lua_State *L, int index)
{
	t_lua_mat4 *m;
	luaL_checktype( L, index, LUA_TUSERDATA);
	m = ( t_lua_mat4 *) luaL_checkudata( L, index, L_MAT4);
	if ( m == NULL) luaL_error( L, "Mat4 type error");
	return m;
}

static int lua_mat4_new( lua_State *L)
{
	float mat4[4][4] = { { 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 0, 0, 0}};

	t_mn_mat4 *mat = mn_mat4_new( mat4);
	t_lua_mat4 *lua_mat = ( t_lua_mat4 *) lua_newuserdata( L, sizeof( t_lua_mat4));
	lua_mat->m = mat;
	luaL_setmetatable( L, L_MAT4); 

	return 1;
}

static int lua_mat4_set( lua_State *L)
{
	t_lua_mat4 *matrix = lua_mat4_check( L, 1);

	float a = luaL_checknumber( L, 2);
	float b = luaL_checknumber( L, 3);
	float c = luaL_checknumber( L, 4);
	float d = luaL_checknumber( L, 5);

	float e = luaL_checknumber( L, 6);
	float f = luaL_checknumber( L, 7);
	float g = luaL_checknumber( L, 8);
	float h = luaL_checknumber( L, 9);

	float i = luaL_checknumber( L, 10);
	float j = luaL_checknumber( L, 11);
	float k = luaL_checknumber( L, 12);
	float l = luaL_checknumber( L, 13);

	float m = luaL_checknumber( L, 14);
	float n = luaL_checknumber( L, 15);
	float o = luaL_checknumber( L, 16);
	float p = luaL_checknumber( L, 17);

	float mat[4][4] = { {  a, b, c, d}, { e, f, g, h}, { i, j, k, l}, { m, n, o, p}};

	mn_mat4_set( matrix->m, mat);

	return 0;
}

static int lua_mat4_print( lua_State *L)
{
	t_lua_mat4 *mat = lua_mat4_check( L, 1);
	mn_mat4_print( mat->m);
	return 0;
}
 
static int lua_mat4_rotate( lua_State *L)
{
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 1);
	const char *axis = luaL_checkstring( L, 2);
	float angle = luaL_checknumber( L, 3);
	float eul[3];
	t_mn_mat4 *mat = lua_mat->m;
	rotate_eul( eul, axis[0], deg_to_rad(angle));
	eul_to_mat4( mat->m, eul);
	return 0;
}

static int lua_mat4_translate( lua_State *L)
{
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 1);
	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);
	t_mn_mat4 *mat = lua_mat->m;
	translate_m4( mat->m, x, y, z);
	return 0;
}

static int lua_mat4_identity( lua_State *L)
{
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 1);
	unit_m4( lua_mat->m->m);
	return 0;
}

static int lua_mat4_multiply( lua_State *L)
{
	t_lua_mat4 *lua_mat_1 = lua_mat4_check( L, 1);
	t_lua_mat4 *lua_mat_2 = lua_mat4_check( L, 2);
	t_mn_mat4 *mat1 = lua_mat_1->m;
	t_mn_mat4 *mat2 = lua_mat_2->m;
	mul_m4_m4m4( mat1->m, mat1->m, mat2->m);
	return 0;
}

static int lua_mat4_add( lua_State *L)
{
	t_lua_mat4 *lua_mat_1 = lua_mat4_check( L, 1);
	t_lua_mat4 *lua_mat_2 = lua_mat4_check( L, 2);
	t_mn_mat4 *mat1 = lua_mat_1->m;
	t_mn_mat4 *mat2 = lua_mat_2->m;
	add_m4_m4m4( mat1->m, mat1->m, mat2->m);
	return 0;
}

static const struct luaL_Reg mat4_methods[] =
{
	{"multiply", lua_mat4_multiply},
	{"add", lua_mat4_add},
	{"rotate", lua_mat4_rotate},
	{"translate", lua_mat4_translate},
	{"print", lua_mat4_print},
	{"identity", lua_mat4_identity},
	{"set", lua_mat4_set},
	{ NULL, NULL}
};

static const struct luaL_Reg mat4[] = 
{
	{"new", lua_mat4_new},
	{ NULL, NULL}
};

void lua_make_table_mat4( lua_State *L)
{
	luaL_newmetatable( L, L_MAT4);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, mat4_methods, 0);
}

void lua_mat4_register( lua_State *L)
{
	lua_make_table_mat4( L);
	luaL_newlib( L, mat4);
	lua_setglobal( L, "mat4");
}

LUALIB_API int luaopen_mat4( lua_State *L)
{
	lua_make_table_mat4( L);
	luaL_newlib( L, mat4);
	return 1;
}
