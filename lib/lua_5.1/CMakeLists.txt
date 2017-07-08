PROJECT(lua)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ADD_DEFINITIONS(-D_fPIC ) 
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wl,-E -Wextra -DLUA_COMPAT_5_1 -DLUA_USE_LINUX") 
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wl,-E -Wextra -DLUA_USE_LINUX") 
SET (SRC
	src/lapi.c
	src/lauxlib.c
	src/lbaselib.c
	src/lcode.c
	src/ldblib.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/linit.c
	src/liolib.c
	src/llex.c
	src/lmathlib.c
	src/lmem.c
	src/loadlib.c
	src/lobject.c
	src/lopcodes.c
	src/loslib.c
 	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/lstrlib.c
	src/ltable.c
	src/ltablib.c
	src/ltm.c
	src/lundump.c
	src/lvm.c
	src/lzio.c
	src/llex.c
	src/lua.h
)


ADD_LIBRARY ( lualib ${SRC} )

ADD_EXECUTABLE ( lua src/lua.c)

TARGET_LINK_LIBRARIES ( lua lualib m dl readline)

