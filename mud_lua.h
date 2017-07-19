#ifndef __MUD_LUA_H__
#define __MUD_LUA_H__

// STRUCTS

typedef struct Lua_Mud
{
	t_mud *mud;
	char *name;
	short is_built;

}t_lua_mud;

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


extern void (* MUD_BUILD_FUNCTION)( t_lua_mud *mud);
void lua_mud_register( lua_State *L);
t_lua_mud *lua_mud_get( lua_State * L);

#endif

