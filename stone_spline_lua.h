/* 
 * Copyright (c) 2016 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */


#ifndef __STONE_SPLINE_LUA_H__
#define __STONE_SPLINE_LUA_H__

#define L_SPLINE "L_SPLINE"

typedef struct Lua_Spline
{
	struct Spline *spline;

}t_lua_spline;

void lua_stone_spline_register( lua_State *L);


#endif

