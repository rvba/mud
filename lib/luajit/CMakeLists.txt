# NOT IN USE
include(ExternalProject)
message("ENTERING LUAJIT")
set(
	SRC_TEST
	test.c
	)
add_library(fffrag ${SRC_TEST})

add_custom_target(
	mytarget
	${PATH_MODUS_SRC}/src/mud/lib/luajit/build.sh
	)

add_dependencies(fffrag mytarget)

#add_library(lualib STATIC IMPORTED)
#set_target_properties(luajit PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/thirdparty/libluajit.a )
#add_dependencies(lualib luajit)


#add_subdirectory(src)
