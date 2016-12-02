/* 
 * Copyright (c) 2016 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#ifndef __LUA_UTIL_H__
#define __LUA_UTIL_H__

#ifdef __cplusplus
extern "C" {
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
