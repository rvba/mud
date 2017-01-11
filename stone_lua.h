#ifndef __STONE_LUA_H__
#define __STONE_LUA_H__

// STRUCTS

struct Object;

typedef struct Lua_Stone
{
	t_stone *stone;
	struct Object *object;
	char *name;

}t_lua_stone;

typedef struct Lua_Vertex
{
	s_vertex *v;

}t_lua_vertex;

typedef struct Lua_Edge
{
	s_edge *e;

}t_lua_edge;

typedef struct Lua_Face
{
	s_face *f;

}t_lua_face;


extern void (* STONE_BUILD_FUNCTION)( t_lua_stone *stone);
void lua_stone_register( lua_State *L);
t_lua_stone *lua_stone_get( lua_State * L);

#endif

