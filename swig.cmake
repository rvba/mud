
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
set(Python_ADDITIONAL_VERSIONS 3.4)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# PYTHON
find_library(PYTHON python3.4 /usr/lib/python3.4/config-3.4m-x86_64-linux-gnu REQUIRED)
include_directories(/usr/include/python3.4)

SWIG_ADD_MODULE(stone python stone.i stucco.c)
SWIG_LINK_LIBRARIES(stone ${PYTHON})
SWIG_LINK_LIBRARIES(stone stdutil)

message("Making Stone Package")
set(STONE_PATH ${PATH_MODUS_SRC}/libs/stone)
#message(".." ${STONE_PATH})
execute_process(COMMAND ${STONE_PATH}/make.sh WORKING_DIRECTORY ${STONE_PATH} RESULT_VARIABLE RES OUTPUT_VARIABLE LOG)
#message(">> " ${RES})
#message(">> " ${LOG})

# LUA
option(OPT_SWIG_LUA "Swig_Lua" OFF)
if(OPT_SWIG_LUA)
	
	find_library(LUA_LIB lua ${PATH_LUA_LIB} REQUIRED)
	include_directories(${PATH_LUA_LIB})

	SWIG_ADD_MODULE(stone_lua lua stone.i stone.c)
	SWIG_LINK_LIBRARIES(stone_lua ${LUA_LIB})
	SWIG_LINK_LIBRARIES(stone_lua stdutil)

endif()
