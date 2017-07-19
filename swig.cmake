
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
set(Python_ADDITIONAL_VERSIONS 3.4)
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})

# PYTHON
find_library(PYTHON python3.4 /usr/lib/python3.4/config-3.4m-x86_64-linux-gnu REQUIRED)
include_directories(/usr/include/python3.4)

SWIG_ADD_MODULE(mud python mud.i stucco.c)
SWIG_LINK_LIBRARIES(mud ${PYTHON})
SWIG_LINK_LIBRARIES(mud stdutil)

message("Making Stone Package")
set(MUD_PATH ${PATH_MODUS_SRC}/libs/mud)
#message(".." ${MUD_PATH})
execute_process(COMMAND ${MUD_PATH}/make.sh WORKING_DIRECTORY ${MUD_PATH} RESULT_VARIABLE RES OUTPUT_VARIABLE LOG)
#message(">> " ${RES})
#message(">> " ${LOG})

# LUA
option(OPT_SWIG_LUA "Swig_Lua" OFF)
if(OPT_SWIG_LUA)
	
	find_library(LUA_LIB lua ${PATH_LUA_LIB} REQUIRED)
	include_directories(${PATH_LUA_LIB})

	SWIG_ADD_MODULE(mud_lua lua mud.i mud.c)
	SWIG_LINK_LIBRARIES(mud_lua ${LUA_LIB})
	SWIG_LINK_LIBRARIES(mud_lua stdutil)

endif()
