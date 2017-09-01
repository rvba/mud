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

#include "mud.h"
#include "modifier.h"
#include "stash.h"
#include "llist.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define L_MUD "L_MUD"
#define L_VERTEX "mud_vertex"
#define L_EDGE "mud_edge"
#define L_FACE "mud_face"

#include "stdmath_lua.h"
#include "mud_lua.h"
#include "lua_util.h"

void (* MUD_BUILD_FUNCTION)( t_lua_mud *mud) = NULL;

// CHECK

static int lua_mud_set( lua_State * L)
{
	const char *name = luaL_checkstring( L ,1);
	if( iseq(name,"tesselate"))
	{
		const char *what = luaL_checkstring( L ,2);
		if( iseq(what,"debug"))
		{
			int val = lua_toboolean( L, 3);
			mud_tess_debug(val);
		}
		else if( iseq( what, "tolerance"))
		{
			double val = luaL_checknumber( L, 3);
			mud_tess_set("tolerance", &val);
		}
		else if( iseq( what, "winding"))
		{
			/* odd, nonzero, positive, negative, abs */
			const char *which = luaL_checkstring( L ,3);
			mud_tess_set("winding", (void *) which);
		}
	}
	return 0;
}

static t_lua_vertex *lua_mud_check_vertex( lua_State *L, int index)
{
	t_lua_vertex *v;
	luaL_checktype( L, index, LUA_TUSERDATA);
	v = ( t_lua_vertex *) luaL_checkudata( L, index, L_VERTEX);
	if ( v == NULL) luaL_error( L, "check_vertex: Not a vertex");
	return v;
}

static t_lua_edge *lua_mud_check_edge( lua_State *L, int index)
{
	t_lua_edge *e;
	luaL_checktype( L, index, LUA_TUSERDATA);
	e = ( t_lua_edge *) luaL_checkudata( L, index, L_EDGE);
	if ( e == NULL) luaL_error( L, "check_edge: Not an edge");
	return e;
}

static t_lua_face *lua_mud_check_face( lua_State *L, int index)
{
	t_lua_face *f;
	luaL_checktype( L, index, LUA_TUSERDATA);
	f = ( t_lua_face *) luaL_checkudata( L, index, L_FACE);
	if ( f == NULL) luaL_error( L, "check_face: Not a face");
	return f;
}

// PUSH

static t_lua_face *lua_mud_push_face( lua_State *L, mud_face *face)
{
	t_lua_face *f = ( t_lua_face *) lua_newuserdata( L, sizeof( t_lua_face));
	f->f = face;
	luaL_getmetatable( L, L_FACE);
	lua_setmetatable( L, -2);
	return f;
}

static t_lua_edge *lua_mud_push_edge( lua_State *L, mud_edge *edge)
{
	t_lua_edge *e = ( t_lua_edge *) lua_newuserdata( L, sizeof( t_lua_edge));
	e->e = edge;
	luaL_getmetatable( L, L_EDGE);
	lua_setmetatable( L, -2);
	return e;
}

static t_lua_vertex *lua_mud_push_vertex( lua_State *L, mud_vertex *vertex)
{
	t_lua_vertex *v = ( t_lua_vertex *) lua_newuserdata( L, sizeof( t_lua_vertex));
	v->v = vertex;
	luaL_getmetatable( L, L_VERTEX);
	lua_setmetatable( L, -2);
	return v;
}

// GET

t_lua_mud *lua_mud_get( lua_State * L)
{
	return ( t_lua_mud *) luaL_checkudata( L, 1, L_MUD);
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
	mud_edge *edge = e->e;
	lua_mud_push_vertex( L, edge->a);

	return 1;
}

static int get_vertex_b( lua_State *L, void *v)
{
	t_lua_edge *e = ( t_lua_edge *) v;
	mud_edge *edge = e->e;
	lua_mud_push_vertex( L, edge->b);
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


static int lua_mud_set_vertex( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);

	int i = luaL_checkinteger( L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);

	float vector[] = { x, y, z};

	mud_vertex_update( lua_mud->mud, i, vector);

	return 1;
}

static int lua_mud_get_name( lua_State *L, void *v)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) v;
	lua_pushstring(L, lua_mud->name);
	return 1;
}

static int lua_mud_get_vertex_count( lua_State *L, void *v)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) v;
	t_mud *mud = lua_mud->mud;
	lua_pushinteger(L, mud->vertex_count);
	return 1;
}

static int lua_mud_get_face_count( lua_State *L, void *v)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) v;
	t_mud *mud = lua_mud->mud;
	lua_pushinteger(L, mud->face_count);
	return 1;
}

static int lua_mud_get_vertex_location( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	int i = luaL_checkinteger( L, 2);
	mud_vertex *vertex = mud_get_vertex( lua_mud->mud, i);
	if( vertex)
	{
		lua_pushnumber(L, vertex->co[0]);
		lua_pushnumber(L, vertex->co[1]);
		lua_pushnumber(L, vertex->co[2]);
		return 3;
	}
	else 
	{
		return 0;
	}
}

// MUD

static t_lua_mud *lua_mud_userdata_new( lua_State *L)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) lua_newuserdata( L, sizeof( t_lua_mud));
	lua_mud->mud = NULL;
	lua_mud->name = NULL;
	lua_mud->is_built = 0;
	luaL_setmetatable( L, L_MUD); 

	return lua_mud;
}

static int lua_mud_new( lua_State *L)
{
	t_lua_mud *lua_mud;
	const char *name = luaL_checkstring(L, 1);

	if( name == NULL) luaL_error(L, "name cannot be empty");

	lua_mud = lua_mud_userdata_new( L);
	lua_mud->mud = mud_new( name);
	lua_mud->name = strdup( name);

	return 1;
}

void lua_mud_free( t_lua_mud *mud)
{
	free( mud);
}

static int lua_mud_copy( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_lua_mud *lua_mud_new  = lua_mud_userdata_new( L);

	t_mud *new = mud_copy( lua_mud->mud);
	lua_mud_new->mud = new;
	lua_mud_new->name = strdup( lua_mud_new->mud->name);

	return 1;
}

static int lua_mud_copy_with_name( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	const char *name = luaL_checkstring(L, 2);
	t_lua_mud *lua_mud_new  = lua_mud_userdata_new( L);

	t_mud *new = mud_copy_with_name( lua_mud->mud,name);
	lua_mud_new->mud = new;
	lua_mud_new->name = strdup( lua_mud_new->mud->name);

	return 1;
}

static int lua_mud_delete( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	mud_free( lua_mud->mud);
	return 0;
}

static int lua_mud_build( lua_State *L)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) luaL_checkudata( L, 1, L_MUD);

	if( MUD_BUILD_FUNCTION)
	{
		MUD_BUILD_FUNCTION( lua_mud);
	}

	return 1;
}

// GET

static int lua_mud_get_vertex_list( lua_State *L, void *v)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) v;
	
	t_mud *mud = lua_mud->mud;
	int count = mud->vertex_count;

	lua_createtable(L, count+1, 0);
	int newTable = lua_gettop(L);
	int index = 1;

	t_lnode *node;
	for( node = mud->vertex->first; node; node = node->next)
	{
		mud_vertex *vertex = ( mud_vertex *) node->data;
		lua_mud_push_vertex( L, vertex);
		lua_rawseti(L, newTable, index);
		++index;
	}

	return 1;
}

static int lua_mud_get_edge_list( lua_State *L, void *v)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) v;
	
	t_mud *mud = lua_mud->mud;
	int count = mud->edge_count;

	lua_createtable(L, count+1, 0);
	int newTable = lua_gettop(L);
	int index = 1;

	t_lnode *node;
	for( node = mud->edge->first; node; node = node->next)
	{
		mud_edge *edge = ( mud_edge *) node->data;
		lua_mud_push_edge( L, edge);
		lua_rawseti(L, newTable, index);
		++index;
	}

	return 1;
}

static int lua_mud_get_face_list( lua_State *L, void *v)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) v;
	
	t_mud *mud = lua_mud->mud;
	int count = mud->face_count;

	lua_createtable(L, count+1, 0);
	int newTable = lua_gettop(L);
	int index = 1;

	t_lnode *node;
	for( node = mud->face->first; node; node = node->next)
	{
		mud_face *face = ( mud_face *) node->data;
		lua_mud_push_face( L, face);
		lua_rawseti(L, newTable, index);
		++index;
	}

	return 1;
}

// VERTEX

static int lua_mud_vertex_print( lua_State *L)
{
	t_lua_vertex *vertex = lua_mud_check_vertex( L, 1);
	mud_vertex *v = vertex->v;
	printf("[mud lua] vertex: %d %f %f %f\n",v->indice,v->co[0],v->co[1],v->co[2]);
	return 0;
}

static int lua_mud_vertex_x( lua_State *L, void *v)
{
	t_lua_vertex *vertex = ( t_lua_vertex *) v;
	mud_vertex *_v = vertex->v;
	lua_pushnumber(L, _v->co[0]);
	return 1;
}

static int lua_mud_vertex_y( lua_State *L, void *v)
{
	t_lua_vertex *vertex = ( t_lua_vertex *) v;
	mud_vertex *_v = vertex->v;
	lua_pushnumber(L, _v->co[1]);
	return 1;
}

static int lua_mud_vertex_z( lua_State *L, void *v)
{
	t_lua_vertex *vertex = ( t_lua_vertex *) v;
	mud_vertex *_v = vertex->v;
	lua_pushnumber(L, _v->co[2]);
	return 1;
}

static int lua_mud_vertex_copy( lua_State *L)
{
	t_lua_vertex *vertex = lua_mud_check_vertex( L, 1);
	mud_vertex *_v = vertex->v;
	mud_vertex *copy = mud_copy_vertex(_v);
	lua_mud_push_vertex( L, copy);
	return 1;
}

static int lua_mud_vertex_translate( lua_State *L)
{
	t_lua_vertex *vertex = lua_mud_check_vertex( L, 1);
	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);
	mud_vertex *_v = vertex->v;
	_v->co[0]+= x;
	_v->co[1]+= y;
	_v->co[2]+= z;
	return 0;
}

static int lua_mud_get_vertex_co( lua_State *L, void *v)
{
	t_lua_vertex *_v = ( t_lua_vertex *) v;
	mud_vertex *vertex = ( mud_vertex *) _v->v;

	lua_createtable(L, 3, 0);
	int newTable = lua_gettop(L);
	int index = 1;

	lua_pushnumber(L, vertex->co[0]);
	lua_rawseti(L, newTable, index);
	++index;

	lua_pushnumber(L, vertex->co[1]);
	lua_rawseti(L, newTable, index);
	++index;

	lua_pushnumber(L, vertex->co[2]);
	lua_rawseti(L, newTable, index);
	++index;

	return 1;
}

static int lua_mud_get_indice( lua_State *L, void *v)
{
	t_lua_vertex *_v = ( t_lua_vertex *) v;
	mud_vertex *vertex = ( mud_vertex *) _v->v;

	lua_pushinteger(L, vertex->indice);

	return 1;
}

// ADD

static int lua_mud_add_next( lua_State *L)
{
	t_lua_mud *lua_mud = ( t_lua_mud *) luaL_checkudata( L, 1, L_MUD);
	t_lua_mud *lua_next = ( t_lua_mud *) luaL_checkudata( L, 2, L_MUD);
	mud_add_next( lua_mud->mud, lua_next->mud);
	return 0;
}

static int lua_mud_add_vertex( lua_State *L)
{
	t_lua_mud *lua_mud;

	lua_mud = ( t_lua_mud *) luaL_checkudata( L, 1, L_MUD);
	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);

	mud_vertex *v = mud_add_vertex( lua_mud->mud, x, y, z);
	lua_mud_push_vertex( L, v);

	return 1;
}

static int lua_mud_add_edge( lua_State *L)
{
	t_lua_mud *lua_mud;

	lua_mud = ( t_lua_mud *) luaL_checkudata( L, 1, L_MUD);
	t_lua_vertex *a = lua_mud_check_vertex( L, 2);
	t_lua_vertex *b = lua_mud_check_vertex( L, 3);

	mud_edge *e = mud_add_edge( lua_mud->mud, a->v, b->v);
	lua_mud_push_edge( L, e);

	return 1;
}

static int lua_mud_add_face( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);

	t_lua_vertex *a = lua_mud_check_vertex( L, 2);
	t_lua_vertex *b = lua_mud_check_vertex( L, 3);
	t_lua_vertex *c = lua_mud_check_vertex( L, 4);
	t_lua_vertex *d = NULL;

	int n = lua_gettop( L);
	mud_face *face;

	if( n == 5)
	{
		d = lua_mud_check_vertex( L, 5); 
		face = mud_add_face( lua_mud->mud, a->v, b->v, c->v, d->v);
	}
	else
	{
		face = mud_add_face( lua_mud->mud, a->v, b->v, c->v, NULL);
	}

	lua_mud_push_face( L, face);

	return 1;
}

static int lua_mud_delete_face( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_lua_face *face = lua_mud_check_face( L, 2);
	t_mud *s = lua_mud->mud;
	mud_face *f = face->f;
	mud_delete_face(s,f);

	return 0;
}

static int lua_mud_add_modifier_matrix( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 2);
	mud_add_modifier_matrix( lua_mud->mud, lua_mat->m);
	return 0;
}

static int lua_mud_add_modifier_skin( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	mud_add_modifier_skin( lua_mud->mud);
	return 0;
}

static int lua_mud_add_modifier_array( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	float count = luaL_checkinteger( L, 2);
	t_lua_mat4 *lua_mat = lua_mat4_check( L, 3);

	mud_add_modifier_array( lua_mud->mud, count, lua_mat->m);
	return 0;
}

static int lua_mud_add_modifier_rotation( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);

	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);

	float a  = luaL_checknumber( L, 5);


	float v[] = {x,y,z};

	mud_add_modifier_rotation( lua_mud->mud, v, a);
	return 0;
}

// EXTRUDE

static int lua_mud_extrude_vertex( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_lua_vertex *p = lua_mud_check_vertex( L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	float v[] = {x,y,z};

	mud_edge *edge = mud_extrude_vertex( lua_mud->mud, p->v, v);
	lua_mud_push_edge( L, edge);
	return 1;
}

static int lua_mud_extrude_edge( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_lua_edge *e = lua_mud_check_edge(L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	float v[] = {x,y,z};

	mud_face *extrude = mud_extrude_edge( lua_mud->mud, e->e, v);
	lua_mud_push_face( L, extrude);
	return 1;
}

static int lua_mud_extrude_face( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_lua_face *face = lua_mud_check_face( L, 2);
	float x = luaL_checknumber( L, 3);
	float y = luaL_checknumber( L, 4);
	float z = luaL_checknumber( L, 5);
	float v[] = {x,y,z};
	mud_face *f = mud_extrude_face( lua_mud->mud, face->f, v);
	lua_mud_push_face( L, f);
	return 1;
}

static int lua_mud_translate( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	float x = luaL_checknumber( L, 2);
	float y = luaL_checknumber( L, 3);
	float z = luaL_checknumber( L, 4);
	float v[3] = {x, y, z};

	mud_translate( lua_mud->mud, v);

	return 0;
}

static int lua_mud_tessellate( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	t_mud *mud = lua_mud->mud;
	mud_tessellate( mud);
	return 0;
}

static int lua_mud_print( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	mud_show( lua_mud->mud);
	return 0;
}

static int lua_mud_rename( lua_State *L)
{
	t_lua_mud *lua_mud = lua_mud_get( L);
	const char *name = luaL_checkstring(L, 2);
	lua_mud->name = strdup( name);
	strncpy( lua_mud->mud->name, name, MUD_NAME);
	return 0;
}

static const struct luaL_Reg mud_methods[] =
{
	{"get_vertex_location", lua_mud_get_vertex_location},
	{"set_vertex", lua_mud_set_vertex},
	{"add_next", lua_mud_add_next},
	{"add_face", lua_mud_add_face},
	{"add_vertex", lua_mud_add_vertex},
	{"add_edge", lua_mud_add_edge},
	{"delete_face", lua_mud_delete_face},
	{"add_modifier_matrix", lua_mud_add_modifier_matrix},
	{"add_modifier_skin", lua_mud_add_modifier_skin},
	{"add_modifier_rotation", lua_mud_add_modifier_rotation},
	{"add_modifier_array", lua_mud_add_modifier_array},
	{"extrude_vertex", lua_mud_extrude_vertex},
	{"extrude_edge", lua_mud_extrude_edge},
	{"extrude_face", lua_mud_extrude_face},
	{"build", lua_mud_build},
	{"translate", lua_mud_translate},
	{"copy", lua_mud_copy},
	{"copy_with_name", lua_mud_copy_with_name},
	{"delete", lua_mud_delete},
	{"print", lua_mud_print},
	{"rename", lua_mud_rename},
	{"tessellate", lua_mud_tessellate},
	{ NULL, NULL}
};

static const Xet_reg_pre mud_getters[] = {
	{"name", lua_mud_get_name},
	{"vertex_count", lua_mud_get_vertex_count},
	{"face_count", lua_mud_get_face_count},
	{"vertices", lua_mud_get_vertex_list},
	{"edges", lua_mud_get_edge_list},
	{"faces", lua_mud_get_face_list},
	{0,0}
};

static const Xet_reg_pre mud_setters[] = {
	{0,0}
};

// EDGE

static int lua_mud_edge_get_a( lua_State *L)
{
	t_lua_edge *edge = lua_mud_check_edge( L, 1);
	mud_vertex *v = edge->e->a;
	lua_mud_push_vertex( L, v);
	return 1;
}

static int lua_mud_edge_get_b( lua_State *L)
{
	t_lua_edge *edge = lua_mud_check_edge( L, 1);
	mud_vertex *v = edge->e->b;
	lua_mud_push_vertex( L, v);
	return 1;
}

static int lua_mud_edge_print( lua_State *L)
{
	t_lua_edge *edge = lua_mud_check_edge( L, 1);
	//mud_vertex *v = edge->e->b;
	printf("[mud lua] edge: a:%d b:%d\n",edge->e->a->indice, edge->e->b->indice);
	return 0;
}

static const Xet_reg_pre vertex_getters[] = {
	{"indice", lua_mud_get_indice},
	{"co", lua_mud_get_vertex_co},
	{"x", lua_mud_vertex_x},
	{"y", lua_mud_vertex_y},
	{"z", lua_mud_vertex_z},
	{0,0}
};

static const Xet_reg_pre vertex_setters[] = {
	{0,0}
};

static const struct luaL_Reg mud_vertex_methods[] =
{
	{"print", lua_mud_vertex_print},
	{"copy", lua_mud_vertex_copy},
	{"translate", lua_mud_vertex_translate},
	{ NULL, NULL}
};

static const struct luaL_Reg mud_edge_methods[] =
{
	{"get_a", lua_mud_edge_get_a},
	{"get_b", lua_mud_edge_get_b},
	{"print", lua_mud_edge_print},
	{ NULL, NULL}
};

static const Xet_reg_pre edge_getters[] = {
	{"a",    get_vertex_a, 0   },
	{"b",    get_vertex_b, 0   },
	{0,0}
};

static const Xet_reg_pre edge_setters[] = {
	{0,0}
};

// LIB

static const struct luaL_Reg mud[] = 
{
	{"new", lua_mud_new},
	{"set", lua_mud_set},
	{ NULL, NULL}
};

void lua_mud_make_table_mud( lua_State *L)
{
	int methods, metatable;

	luaL_newmetatable( L, L_MUD);
	metatable = lua_gettop(L);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, mud_methods, 0);

	methods = lua_gettop(L);

	lua_set_getters_setters( L, methods, metatable, mud_getters, mud_setters);
}

void lua_mud_make_table_vertex( lua_State *L)
{
	int methods, metatable;
	luaL_newmetatable( L, L_VERTEX);

	metatable = lua_gettop(L);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, mud_vertex_methods, 0);

	methods = lua_gettop(L);

	lua_set_getters_setters( L, methods, metatable, vertex_getters, vertex_setters);
}

void lua_mud_make_table_edge( lua_State *L)
{
	int methods, metatable;
	luaL_newmetatable( L, L_EDGE);

	metatable = lua_gettop(L);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, mud_edge_methods, 0);

	methods = lua_gettop(L);

	lua_set_getters_setters( L, methods, metatable, edge_getters, edge_setters);
}

void lua_mud_make_table_face( lua_State *L)
{
	luaL_newmetatable( L, L_FACE);
}

void lua_mud_init( lua_State *L)
{
	lua_mud_make_table_mud( L);
	lua_mud_make_table_vertex( L);
	lua_mud_make_table_edge( L);
	lua_mud_make_table_face( L);
	luaL_newlib( L, mud);
}

void lua_mud_register( lua_State *L)
{
	lua_mud_init( L);
	lua_setglobal(L,"mud");
}

LUALIB_API int luaopen_mud( lua_State *L)
{
	lua_mud_init( L);
	return 1;
}
