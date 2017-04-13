#ifndef __STONE_LUA_H__
#define __STONE_LUA_H__

// STRUCTS

typedef struct Lua_Stone
{
	t_stone *stone;
	char *name;
	short is_built;

}t_lua_stone;

typedef struct Lua_Vertex
{
	s_vertex *v;

}t_lua_vertex;

typedef struct Lua_Edge
{
	s_edge *e;

}t_lua_edge;

/**
 * @ref Lua_Face
 * @brief lua face
 * @struct Lua_Face
 */
typedef struct Lua_Face
{
	s_face *f;

}t_lua_face;


extern void (* STONE_BUILD_FUNCTION)( t_lua_stone *stone);
void lua_stone_register( lua_State *L);
t_lua_stone *lua_stone_get( lua_State * L);

#endif

