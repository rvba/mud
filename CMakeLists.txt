# STONE
cmake_minimum_required(VERSION 2.8) 

project(Stone)

option(OPT_SWIG OFF)

add_executable(
    stone
    stonec.c
)

target_link_libraries(stone lualib m dl readline)
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
	stone_tessellate.c
	)

if(OPT_TINYSPLINE)
	set(
		SRC_STONE
		${SRC_STONE}
		stone_spline.c
		stone_spline.h
		stone_spline_lua.c
		stone_spline_lua.h
		)
endif()

set(
	SRC_UTIL

	
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
if(OPT_LUA_5_1)
	message("LUA 5.1")
	include_directories(lib/lua_5.1/src)
	add_subdirectory(lib/lua_5.1)
else()
	message("LUA 5.3")
	include_directories(lib/lua/src)
	add_subdirectory(lib/lua)
endif()

include_directories(umber)
add_subdirectory(lib/blenlib)
add_subdirectory(umber)

if(OPT_TINYSPLINE)
	include_directories(lib/tinyspline)
	add_subdirectory(lib/tinyspline)
endif()

if(OPT_LPEG)
	add_subdirectory(lib/lpeg)
endif()

add_library(stonelib ${LIB_TYPE} ${SRC_STONE})
add_library(stdutil ${LIB_TYPE} ${SRC_UTIL})
add_definitions(-lm)
target_link_libraries(stdutil m)

if(OPT_TINYSPLINE)
	target_link_libraries(stonelib tinyspline)
endif()

# STONE LUA LIB

set(
	STONE_LUA_SRC
	stone_lua.c
	lua_util.c
	)

find_library(LIB_STONE stone ${PATH_STONE} REQUIRED)
find_library(LIB_UTIL stdutil ${PATH_STONE} REQUIRED)

add_library(lua_stone ${LIB_TYPE} ${STONE_LUA_SRC})

# SWIG
if(OPT_SWIG)
	include("swig.cmake")
endif()

