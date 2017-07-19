/* 
 * Copyright (c) 2016 Milovann Yanatchkov 
 *
 * This file is part of Mud, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __LUA_UTIL_H__
#define __LUA_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_NO_COMPAT
extern void luaL_setmetatable (lua_State *L, const char *tname); 
extern void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);
#define luaL_newlibtable(L, l) \
  (lua_createtable(L, 0, sizeof(l)/sizeof(*(l))-1))
#define luaL_newlib(L, l) \
  (luaL_newlibtable(L, l), luaL_register(L, NULL, l))
#endif

typedef int (*Xet_func) (lua_State *L, void *v);

/* member info for get and set handlers */
typedef const struct{
	const char *name;  /* member name */
	Xet_func func;     /* get or set function for type of member */
	size_t offset;     /* offset of member within your_t */
}  Xet_reg_pre;

typedef Xet_reg_pre * Xet_reg;

void Xet_add (lua_State *L, Xet_reg l);
int index_handler (lua_State *L);
int newindex_handler (lua_State *L);

void lua_set_getters_setters( lua_State *L, int methods, int metatable, Xet_reg setters, Xet_reg getters);


#ifdef __cplusplus
}
#endif


#endif
