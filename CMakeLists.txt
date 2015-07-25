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

# STONE LUA SHARED

add_library(stone_shared SHARED ${SRC_STONE})
add_library(stdutil SHARED ${SRC_UTIL})
install(TARGETS stone_shared stdutil LIBRARY DESTINATION /usr/local/lib)
target_link_libraries(stone_shared blenlib_shared)
target_link_libraries(stone_shared guardedalloc_shared)

# STONE STATIC

add_library(stone_static STATIC ${SRC_STONE})
add_library(stdutil_static STATIC ${SRC_UTIL})
add_definitions(-lm)
target_link_libraries(stdutil_static m)

# STONE LUA LIB

set(
	STONE_LUA_SRC
	stone_lua.c
	)

set(PATH_LUA /home/milovann/SOURCES/LUA/lua)
set(PATH_STONE /home/milovann/SOURCES/MINUIT/modus-build/src/stone)
include_directories(/home/milovann/SOURCES/LUA/lua/src)
set( CMAKE_SKIP_BUILD_RPATH true )


find_library(LIB_STONE stone ${PATH_STONE} REQUIRED)
find_library(LIB_UTIL stdutil ${PATH_STONE} REQUIRED)

add_library(lua_stone_shared SHARED ${STONE_LUA_SRC})
set_target_properties(lua_stone_shared PROPERTIES OUTPUT_NAME stone PREFIX "")

target_link_libraries(lua_stone_shared ${PATH_LUA}/src/liblua.so)
target_link_libraries(lua_stone_shared ${STONE_SHARED})
target_link_libraries(lua_stone_shared ${STDMATH})


#target_link_libraries(lua_stone_shared ${PATH_LUA}/src/liblua.so)
#target_link_libraries(lua_stone_shared ${PATH_STONE}/libstone_shared.so)
#target_link_libraries(lua_stone_shared ${PATH_STONE}/libstdutil.so)

add_dependencies(lua_stone_shared stone)
add_dependencies(lua_stone_shared stdutil)

install(TARGETS lua_stone_shared LIBRARY DESTINATION /usr/local/lib/lua/5.3)

# STDMATH
add_library(stdmath SHARED "stdmath_lua.c")
set_target_properties(stdmath PROPERTIES OUTPUT_NAME stdmath PREFIX "")
target_link_libraries(stdmath stdutil)

# MAT3
add_library(mat3 SHARED "stdmath_mat3_lua.c")
set_target_properties(mat3 PROPERTIES OUTPUT_NAME mat3 PREFIX "")
target_link_libraries(mat3 stdutil)

# MAT4
add_library(mat4 SHARED "stdmath_mat4_lua.c")
set_target_properties(mat4 PROPERTIES OUTPUT_NAME mat4 PREFIX "")
target_link_libraries(mat4 stdutil)
target_link_libraries(mat4 stdutil)
target_link_libraries(mat4 blenlib_shared)
target_link_libraries(mat4 guardedalloc_shared)


add_library(lua_stone STATIC ${STONE_LUA_SRC})

# SWIG
if(OPT_SWIG)
	include("swig.cmake")
endif()

