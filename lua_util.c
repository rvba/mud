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
