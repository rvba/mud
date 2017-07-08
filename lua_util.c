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

#ifdef HAVE_LUA_5_1
#include "compat-5.3.h"
#endif

#include "lua_util.h"


void Xet_add (lua_State *L, Xet_reg l)
{
	for (; l->name; l++) {
		lua_pushstring(L, l->name);
		lua_pushlightuserdata(L, (void*)l);
		lua_settable(L, -3);
	}
}

int Xet_call (lua_State *L)
{
	/* for get: stack has userdata, index, lightuserdata */
	/* for set: stack has userdata, index, value, lightuserdata */
	Xet_reg m = (Xet_reg)lua_touserdata(L, -1);  /* member info */
	lua_pop(L, 1);                               /* drop lightuserdata */
	luaL_checktype(L, 1, LUA_TUSERDATA);
	return m->func(L, (void *)((char *)lua_touserdata(L, 1) + m->offset));
}

int index_handler (lua_State *L)
{
	/* stack has userdata, index */
	lua_pushvalue(L, 2);                     /* dup index */
	lua_rawget(L, lua_upvalueindex(1));      /* lookup member by name */
	if (!lua_islightuserdata(L, -1)) {
		lua_pop(L, 1);                         /* drop value */
		lua_pushvalue(L, 2);                   /* dup index */
		lua_gettable(L, lua_upvalueindex(2));  /* else try methods */
		if (lua_isnil(L, -1))                  /* invalid member */
			luaL_error(L, "cannot get member '%s'", lua_tostring(L, 2));
		return 1;
	}
	return Xet_call(L);                      /* call get function */
}

int newindex_handler (lua_State *L)
{
	/* stack has userdata, index, value */
	lua_pushvalue(L, 2);                     /* dup index */
	lua_rawget(L, lua_upvalueindex(1));      /* lookup member by name */
	if (!lua_islightuserdata(L, -1))         /* invalid member */
		luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));
	return Xet_call(L);                      /* call set function */
}

void lua_set_getters_setters( lua_State *L, int methods, int metatable, Xet_reg getters , Xet_reg setters)
{
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methods);    /* dup methods table*/
	lua_rawset(L, metatable);     /* hide metatable:
					 metatable.__metatable = methods */
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, metatable);  /* upvalue index 1 */
	Xet_add(L, getters);     /* fill metatable with getters */
	lua_pushvalue(L, methods);    /* upvalue index 2 */
	lua_pushcclosure(L, index_handler, 2);
	lua_rawset(L, metatable);     /* metatable.__index = index_handler */

	lua_pushliteral(L, "__newindex");
	lua_newtable(L);              /* table for members you can set */
	Xet_add(L, setters);     /* fill with setters */
	lua_pushcclosure(L, newindex_handler, 1);
	lua_rawset(L, metatable);     /* metatable.__newindex = newindex_handler */

	lua_pop(L, 1);                /* drop metatable */
	//return 1;                     /* return methods on the stack */
}
