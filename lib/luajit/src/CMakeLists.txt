include(ExternalProject)
message("LUAJIT DIR")

#add_custom_target(
#	MYTARGET
#	OUTPUT STATIC lualib
#	COMMAND make
#)

#${CMAKE_CURRENT_SOURCE_FILE}

message("curent dir" ${CMAKE_CURRENT_SOURCE_DIR})
ExternalProject_Add(
	luajit
	DOWNLOAD_COMMAND ""
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	URL ${CMAKE_CURRENT_SOURCE_DIR}
	BUILD_COMMAND make
	BINARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/libluajit.a
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/thirdparty/libluajit.a
	)

ExternalProject_Add_Step(
	luajit 
	forcebuild 
	ALWAYS 1
	)

ExternalProject_Get_Property(luajit install_dir)

add_library(lualib STATIC IMPORTED)
set_target_properties(lualib PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/thirdparty/libluajit.a )
add_dependencies(lualib luajit)



#ADD_LIBRARY ( lualib ${SRC} )

#ADD_EXECUTABLE ( lua luajit.c)

#TARGET_LINK_LIBRARIES ( lua lualib m dl readline)

