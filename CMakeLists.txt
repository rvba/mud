# MUD

cmake_minimum_required(VERSION 2.8) 
project(Mud)

#add_subdirectory(lua)
include(lua/CMakeLists.txt)

add_definitions(-std=gnu89)


set(
	SRC_MUD

	mud.h
	mud.c
	modifier.h
	modifier.c
	mod_skin.c
	mod_matrix.c
	mod_rotation.c
	mod_array.c
	mud_tessellate.c
	)


if(OPT_TINYSPLINE)
	set(
		SRC_MUD
		${SRC_MUD}
		mud_spline.c
		mud_spline.h
		mud_spline_lua.c
		mud_spline_lua.h
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
	mud.i
	)

include_directories(lib/blenlib/blenlib)
include_directories(lib/blenlib/guardedalloc)
include_directories(umber)

if(OPT_LUA_5_1)
	message("LUA 5.1")
	include_directories(lib/lua_5.1/src)
	add_subdirectory(lib/lua_5.1)
elseif(OPT_LUA_5_3)
	message("LUA 5.3")
	include_directories(lib/lua/src)
	add_subdirectory(lib/lua)
else()
	message("USING LUAJIT")
	#include_directories(luajit)
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

add_library(mudlib ${LIB_TYPE} ${SRC_MUD})
add_library(stdutil ${LIB_TYPE} ${SRC_UTIL})
add_definitions(-lm)
target_link_libraries(stdutil m)

if(OPT_TINYSPLINE)
	target_link_libraries(mudlib tinyspline)
endif()

# MUD LUA LIB

set(
	MUD_LUA_SRC
	mud_lua.c
	lua_util.c
	)

find_library(LIB_MUD mud ${PATH_MUD} REQUIRED)
find_library(LIB_UTIL stdutil ${PATH_MUD} REQUIRED)
#find_library(LIB_COMPAT  luacompat ${PATH_MUD} REQUIRED)

add_library(lua_mud ${LIB_TYPE} ${MUD_LUA_SRC})
#add_library(lua_mud ${LIB_TYPE} ${MUD_LUA_SRC} ${LIB_COMPAT})

add_executable(
    mud
    mudc.c
)

target_link_libraries(mud libluajit dl m)
include_directories(lua/luajit/src)


#SWIG

option(OPT_SWIG OFF)
if(OPT_SWIG)
	include("swig.cmake")
endif()

