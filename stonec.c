/* 
 * Copyright (c) 2015 Milovann Yanatchkov 
 *
 * This file is part of Stone, a free software
 * licensed under the GNU General Public License v2
 * see /LICENSE for more information
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static lua_State *LUA_STATE = NULL; 

static void stone_main_lua_init( void)
{
	// New Lua state
	LUA_STATE = luaL_newstate();

	// Open Lua Std Libs
	luaL_openlibs(LUA_STATE);
}

static void mn_lua_error( void)
{
	printf( "[ERROR] %s\n", lua_tostring( LUA_STATE, -1));
}

static void stone_main_exe( const char *file_path)
{

	printf("[LOADING] %s\n", file_path);

	FILE *file = fopen( file_path, "r");
	int data_size;
	char *data;

	if( file)
	{
		fseek ( file, 0, SEEK_END);
		data_size = ftell( file);
		rewind( file);
		data = (char *) malloc( sizeof( char) * data_size);
		size_t r = fread ( data, 1, data_size, file);

		if( r != data_size)
		{
			printf("[ERROR] Can't read file %s\n", file_path);
			return;
		}	

		if( luaL_loadbuffer( LUA_STATE, data, data_size, "buffer") == LUA_OK )
		{
			printf("[PROCCESSING] ... ");
			if( !lua_pcall( LUA_STATE, 0, 0, 0))
			{
				printf(" done\n");
			}
			else
			{
				printf("\n");
				mn_lua_error();
			}
		}
		else
		{
			mn_lua_error();
		}
	}
	else
	{
		printf("Can't open %s\n", file_path);
	}
}

int main( int argc, char **argv)
{
	if( argc == 1)
	{
		printf("Stone\n");
		printf("Usage: stone file [options]\n\n");
		return 0;
	}

	if( argc > 2)
	{
	}
	else
	{
		char *file = argv[1];
		stone_main_lua_init();
		stone_main_exe( file);
	}

	return 0;
}
