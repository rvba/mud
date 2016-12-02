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

#include "stone.h"
#include "modifier.h"
#include "stash.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define L_STONE "L_STONE"
#define L_VERTEX "s_vertex"
#define L_EDGE "s_edge"
#define L_FACE "s_face"

#include "stdmath_lua.h"
#include "stone_lua.h"
#include "lua_util.h"

#include "object.h"
#include "scene.h"
#include "ctx.h"
#include "op.h"
#include "mesh.h"
#include "vlst.h"

void (* STONE_BUILD_FUNCTION)( t_lua_stone *stone) = NULL;

// CHECK

static t_lua_vertex *lua_stone_check_vertex( lua_State *L, int index)
{
	t_lua_vertex *v;
	luaL_checktype( L, index, LUA_TUSERDATA);
	v = ( t_lua_vertex *) luaL_checkudata( L, index, L_VERTEX);
	if ( v == NULL) luaL_error( L, "check_vertex: Not a vertex");
	return v;
}

static t_lua_edge *lua_stone_check_edge( lua_State *L, int index)
{
	t_lua_edge *e;
	luaL_checktype( L, index, LUA_TUSERDATA);
	e = ( t_lua_edge *) luaL_checkudata( L, index, L_EDGE);
	if ( e == NULL) luaL_error( L, "check_edge: Not an edge");
	return e;
}

static t_lua_face *lua_stone_check_face( lua_State *L, int index)
{
	t_lua_face *f;
	luaL_checktype( L, index, LUA_TUSERDATA);
	f = ( t_lua_face *) luaL_checkudata( L, index, L_FACE);
	if ( f == NULL) luaL_error( L, "check_face: Not a face");
	return f;
}

// PUSH

static t_lua_face *lua_stone_push_face( lua_State *L, s_face *face)
{
	t_lua_face *f = ( t_lua_face *) lua_newuserdata( L, sizeof( t_lua_face));
	f->f = face;
	luaL_getmetatable( L, L_FACE);
	lua_setmetatable( L, -2);
	return f;
}

static t_lua_edge *lua_stone_push_edge( lua_State *L, s_edge *edge)
{
	t_lua_edge *e = ( t_lua_edge *) lua_newuserdata( L, sizeof( t_lua_edge));
	e->e = edge;
	luaL_getmetatable( L, L_EDGE);
	lua_setmetatable( L, -2);
	return e;
}

static t_lua_vertex *lua_stone_push_vertex( lua_State *L, s_vertex *vertex)
{
	t_lua_vertex *v = ( t_lua_vertex *) lua_newuserdata( L, sizeof( t_lua_vertex));
	v->v = vertex;
	luaL_getmetatable( L, L_VERTEX);
	lua_setmetatable( L, -2);
	return v;
}

// GET

static int get_id( lua_State *L, void *v)
{
	t_lua_stone *s = ( t_lua_stone *) v;
	t_object *object = ( t_object *) s->object;
	int id = object->id.id;
	lua_pushinteger( L, id);

	return 1;
}

t_lua_stone *lua_stone_get( lua_State * L)
{
	return ( t_lua_stone *) luaL_checkudata( L, 1, L_STONE);
}

// Members Get

/*
static int get_int (lua_State *L, void *v)
{
	lua_pushnumber(L, *(int*)v);
	return 1;
}

static int get_number (lua_State *L, void *v)
{
	lua_pushnumber(L, *(lua_Number*)v);
	return 1;
}
*/

static int get_vertex_a( lua_State *L, void *v)
{
	t_lua_edge *e = ( t_lua_edge *) v;
	s_edge *edge = e->e;
	lua_stone_push_vertex( L, edge->a);

	return 1;
}

static int get_vertex_b( lua_State *L, void *v)
{
	t_lua_edge *e = ( t_lua_edge *) v;
	s_edge *edge = e->e;
	lua_stone_push_vertex( L, edge->b);
	return 1;
}

// Members Set

/*
static int set_int (lua_State *L, void *v)
{
	*(int*)v = luaL_checkinteger(L, 3);
	return 0;
}


static int set_number (lua_State *L, void *v)
{
	*(lua_Number*)v = luaL_checknumber(L, 3);
	return 0;
}
*/


// STONE

static t_lua_stone *lua_stone_userdata_new( lua_State *L)
{
	// Push a new userdata on the stack
	t_lua_stone *lua_stone = ( t_lua_stone *) lua_newuserdata( L, sizeof( t_lua_stone));
	lua_stone->stone = NULL;
	lua_stone->name = NULL;

	// Set the metatable on the userdata
	luaL_setmetatable( L, L_STONE); 

	return lua_stone;
}

static int lua_stone_new( lua_State *L)
{
	t_lua_stone *lua_stone;
	const char *name = luaL_checkstring(L, 1);

	if( name == NULL) luaL_error(L, "name cannot be empty");

	lua_stone = lua_stone_userdata_new( L);
	lua_stone->stone = stone_new( name);
	lua_stone->name = strdup( name);

	return 1;
}

void lua_stone_free( t_lua_stone *stone)
{
	free( stone);
}

static int lua_stone_copy( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	t_lua_stone *lua_stone_new  = lua_stone_userdata_new( L);
	lua_stone_new->stone = stone_copy( lua_stone->stone);
	lua_stone->name = strdup( lua_stone->name);

	return 1;
}

static int lua_stone_delete( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	// Free Object
	/*
	 


	mn_lua_free_object( lua_stone->object);



	*/
	// Free Stone
	stone_free( lua_stone->stone);
	// Free Stone_Lua
	// Garbage collector ??
	//lua_stone_free( lua_stone);
	return 0;
}

static int lua_stone_build_object( lua_State *L)
{
	t_lua_stone *lua_stone = ( t_lua_stone *) luaL_checkudata( L, 1, L_STONE);

	if( STONE_BUILD_FUNCTION)
	{
		STONE_BUILD_FUNCTION( lua_stone);
	}

	return 1;
}

// ADD

static int lua_stone_add_next( lua_State *L)
{
	t_lua_stone *lua_stone = ( t_lua_stone *) luaL_checkudata( L, 1, L_STONE);
	t_lua_stone *lua_next = ( t_lua_stone *) luaL_checkudata( L, 2, L_STONE);
	stone_add_next( lua_stone->stone, lua_next->stone);
	return 0;
}

static int lua_stone_add_vertex( lua_State *L)
{
	t_lua_stone *lua_stone;

	lua_stone = ( t_lua_stone *) luaL_checkudata( L, 1, L_STONE);
	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);

	s_vertex *v = stone_add_vertex( lua_stone->stone, x, y, z);
	lua_stone_push_vertex( L, v);

	return 1;
}

//static int lua_stone_add_edge( lua_State *L)
static int lua_stone_add_edge( lua_State *L)
{
	t_lua_stone *lua_stone;

	lua_stone = ( t_lua_stone *) luaL_checkudata( L, 1, L_STONE);
	t_lua_vertex *a = lua_stone_check_vertex( L, 2);
	t_lua_vertex *b = lua_stone_check_vertex( L, 3);

	s_edge *e = stone_add_edge( lua_stone->stone, a->v, b->v);
	lua_stone_push_edge( L, e);

	return 1;
}

static int lua_stone_add_face( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);

	t_lua_vertex *a = lua_stone_check_vertex( L, 2);
	t_lua_vertex *b = lua_stone_check_vertex( L, 3);
	t_lua_vertex *c = lua_stone_check_vertex( L, 4);
	t_lua_vertex *d = NULL;

	int n = lua_gettop( L);
	s_face *face;

	if( n == 5)
	{
		d = lua_stone_check_vertex( L, 5); 
		face = stone_add_face( lua_stone->stone, a->v, b->v, c->v, d->v);
	}
	else
	{
		face = stone_add_face( lua_stone->stone, a->v, b->v, c->v, NULL);
	}

	lua_stone_push_face( L, face);

	return 1;
}

static int lua_stone_add_modifier_matrix( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 2);
	stone_add_modifier_matrix( lua_stone->stone, lua_mat->m);
	return 0;
}

static int lua_stone_add_modifier_skin( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	stone_add_modifier_skin( lua_stone->stone);
	return 0;
}

static int lua_stone_add_modifier_array( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	float count = luaL_checkinteger( L, 2);
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 3);

	stone_add_modifier_array( lua_stone->stone, count, lua_mat->m);
	return 0;
}

static int lua_stone_add_modifier_rotation( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);

	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);

	float a  = luaL_checknumber( L, 5);


	float v[] = {x,y,z};

	stone_add_modifier_rotation( lua_stone->stone, v, a);
	return 0;
}

// EXTRUDE

static int lua_stone_extrude_vertex( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	t_lua_vertex *p = lua_stone_check_vertex( L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	float v[] = {x,y,z};

	s_edge *edge = stone_extrude_vertex( lua_stone->stone, p->v, v);
	lua_stone_push_edge( L, edge);
	return 1;
}

static int lua_stone_extrude_edge( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	t_lua_edge *e = lua_stone_check_edge(L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	float v[] = {x,y,z};

	s_face *extrude = stone_extrude_edge( lua_stone->stone, e->e, v);
	lua_stone_push_face( L, extrude);
	return 1;
}

static int lua_stone_extrude_face( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	t_lua_face *face = lua_stone_check_face( L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	float v[] = {x,y,z};
	s_face *f = stone_extrude_face( lua_stone->stone, face->f, v);
	lua_stone_push_face( L, f);
	return 1;
}

static int lua_stone_build_circle( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	int resolution  = luaL_checkinteger( L, 2);
	float radius = luaL_checknumber( L, 3);

	stone_build_circle( lua_stone->stone, resolution, radius);

	return 0;
}

static int lua_stone_translate( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);
	float v[3] = {x, y, z};

	stone_translate( lua_stone->stone, v);

	return 0;
}

static int lua_stone_print( lua_State *L)
{
	t_lua_stone *lua_stone = lua_stone_get( L);
	stone_show( lua_stone->stone);
	return 0;
}

static const struct luaL_Reg stone_methods[] =
{
	{"add_next", lua_stone_add_next},
	{"add_face", lua_stone_add_face},
	{"add_vertex", lua_stone_add_vertex},
	{"add_edge", lua_stone_add_edge},
	{"add_modifier_matrix", lua_stone_add_modifier_matrix},
	{"add_modifier_skin", lua_stone_add_modifier_skin},
	{"add_modifier_rotation", lua_stone_add_modifier_rotation},
	{"add_modifier_array", lua_stone_add_modifier_array},
	{"extrude_vertex", lua_stone_extrude_vertex},
	{"extrude_edge", lua_stone_extrude_edge},
	{"extrude_face", lua_stone_extrude_face},
	{"build_object", lua_stone_build_object},
	{"build_circle", lua_stone_build_circle},
	{"translate", lua_stone_translate},
	{"copy", lua_stone_copy},
	{"delete", lua_stone_delete},
	{"print", lua_stone_print},
	{ NULL, NULL}
};

static const Xet_reg_pre stone_getters[] = {
	{"id",    get_id, 0   },
	{0,0}
};

static const Xet_reg_pre stone_setters[] = {
	{0,0}
};

// EDGE

static int lua_stone_edge_get_a( lua_State *L)
{
	t_lua_edge *edge = lua_stone_check_edge( L, 1);
	s_vertex *v = edge->e->a;
	lua_stone_push_vertex( L, v);
	return 1;
}

static int lua_stone_edge_get_b( lua_State *L)
{
	t_lua_edge *edge = lua_stone_check_edge( L, 1);
	s_vertex *v = edge->e->b;
	lua_stone_push_vertex( L, v);
	return 1;
}

static const struct luaL_Reg stone_edge_methods[] =
{
	{"get_a", lua_stone_edge_get_a},
	{"get_b", lua_stone_edge_get_b},
	{ NULL, NULL}
};

static const Xet_reg_pre edge_getters[] = {
	//{"id",   get_int,    offsetof(t_lua_edge,id)   },
	{"a",    get_vertex_a, 0   },
	{"b",    get_vertex_b, 0   },
	{0,0}
};

static const Xet_reg_pre edge_setters[] = {
	/*
	{"id",  set_int,    offsetof(t_lua_edge,id)  },
	{"x",    set_number, offsetof(t_lua_edge,x)    },
	{"y",    set_number, offsetof(t_lua_edge,y)    },
	*/
	{0,0}
};

// LIB

static const struct luaL_Reg stone[] = 
{
	{"new", lua_stone_new},
	{ NULL, NULL}
};


void lua_stone_make_meta_stone( lua_State *L, int methods, int metatable)
{
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methods);    /* dup methods table*/
	lua_rawset(L, metatable);     /* hide metatable:
					 metatable.__metatable = methods */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);  /* upvalue index 1 */
	Xet_add(L, stone_getters);     /* fill metatable with getters */
	lua_pushvalue(L, methods);    /* upvalue index 2 */
	lua_pushcclosure(L, index_handler, 2);
	lua_rawset(L, metatable);     /* metatable.__index = index_handler */

	lua_pushliteral(L, "__newindex");
	lua_newtable(L);              /* table for members you can set */
	Xet_add(L, stone_setters);     /* fill with setters */
	lua_pushcclosure(L, newindex_handler, 1);
	lua_rawset(L, metatable);     /* metatable.__newindex = newindex_handler */

	lua_pop(L, 1);                /* drop metatable */
	//return 1;                     /* return methods on the stack */
}

void lua_stone_make_table_stone( lua_State *L)
{
	int methods, metatable;

	// Create a metable and put it on the stack
	luaL_newmetatable( L, L_STONE);

	metatable = lua_gettop(L);

	/* Duplicate the metatable on the stack (We know have 2). */
	lua_pushvalue(L, -1);
	/* Pop the first metatable off the stack and assign it to __index
	 *of the second one. We set the metatable for the table to itself.
	 *This is equivalent to the following in lua:
	 *metatable = {}
	 *metatable.__index = metatable
	 **/
	lua_setfield(L, -2, "__index");

	/* Set the methods to the metatable that should be accessed via object:func */
	luaL_setfuncs(L, stone_methods, 0);

	methods = lua_gettop(L);

	lua_stone_make_meta_stone( L, methods, metatable);
}

void lua_stone_make_table_vertex( lua_State *L)
{
	luaL_newmetatable( L, L_VERTEX);
	/*
	   lua_pushvalue(L, -1);
	   lua_setfield(L, -2, "__index");
	   luaL_setfuncs(L, stone_vertex_methods, 0);
	   */
}


void lua_stone_make_meta_edge( lua_State *L, int methods, int metatable)
{
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methods);    /* dup methods table*/
	lua_rawset(L, metatable);     /* hide metatable:
					 metatable.__metatable = methods */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);  /* upvalue index 1 */
	Xet_add(L, edge_getters);     /* fill metatable with getters */
	lua_pushvalue(L, methods);    /* upvalue index 2 */
	lua_pushcclosure(L, index_handler, 2);
	lua_rawset(L, metatable);     /* metatable.__index = index_handler */

	lua_pushliteral(L, "__newindex");
	lua_newtable(L);              /* table for members you can set */
	Xet_add(L, edge_setters);     /* fill with setters */
	lua_pushcclosure(L, newindex_handler, 1);
	lua_rawset(L, metatable);     /* metatable.__newindex = newindex_handler */

	lua_pop(L, 1);                /* drop metatable */
	//return 1;                     /* return methods on the stack */
}

void lua_stone_make_table_edge( lua_State *L)
{
	int methods, metatable;
	luaL_newmetatable( L, L_EDGE);

	metatable = lua_gettop(L);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, stone_edge_methods, 0);

	methods = lua_gettop(L);

	lua_stone_make_meta_edge( L, methods, metatable);
}

void lua_stone_make_table_face( lua_State *L)
{
	luaL_newmetatable( L, L_FACE);
	/*
	   lua_pushvalue(L, -1);
	   lua_setfield(L, -2, "__index");
	   luaL_setfuncs(L, stone_face_methods, 0);
	   */
}

void lua_stone_init( lua_State *L)
{
	lua_stone_make_table_stone( L);
	lua_stone_make_table_vertex( L);
	lua_stone_make_table_edge( L);
	lua_stone_make_table_face( L);
	/* Register the object.func functions into the table that is at the top of the
	 * * stack. */
	luaL_newlib( L, stone);
}

void lua_stone_register( lua_State *L)
{
	lua_stone_init( L);
	lua_setglobal(L,"stone");
}

LUALIB_API int luaopen_stone( lua_State *L)
{
	lua_stone_init( L);
	return 1;
}
