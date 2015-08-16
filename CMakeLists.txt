# STONE
cmake_minimum_required(VERSION 2.8) 

project(Stone)

option(OPT_SWIG OFF)

add_executable(
    stone
    stonec.c
)

target_link_libraries(stone lualib m)

set(
	SRC_STONE

	stone.h
	stone.c
	modifier.h
	modifier.c
	mod_skin.c
	mod_matrix.c
	mod_rotation.c
	mod_array.c
	)
set(
	SRC_UTIL

	stash.h
	stash.c
	
	llist.h
	llist.c

	stdmath.c
	stdmath.h
	stdvec.c
	stdvec.h

	stdmath_mat3_lua.c
	stdmath_mat4_lua.c
	stdmath_lua.c
	)

set(
	SWIG_FILES
	stone.i
	)

include_directories(lib/blenlib/blenlib)
include_directories(lib/blenlib/guardedalloc)
include_directories(lib/lua/src)
add_subdirectory(lib/blenlib)
add_subdirectory(lib/lua)

add_library(stone_static STATIC ${SRC_STONE})
add_library(stdutil_static STATIC ${SRC_UTIL})
add_definitions(-lm)
target_link_libraries(stdutil_static m)

# STONE LUA LIB

set(
	STONE_LUA_SRC
	stone_lua.c
	)

find_library(LIB_STONE stone ${PATH_STONE} REQUIRED)
find_library(LIB_UTIL stdutil ${PATH_STONE} REQUIRED)

add_library(lua_stone STATIC ${STONE_LUA_SRC})

# SWIG
if(OPT_SWIG)
	include("swig.cmake")
endif()

