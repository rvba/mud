cmake_minimum_required(VERSION 2.8) 
include(ExternalProject)
message("LUAJIT LIB !!!!!!!!!!!!!!!!!!!!")
set(
	SRC
	lauxlib.h
	lib_aux.c
	lib_base.c
	lib_bit.c
	lib_debug.c
	lib_ffi.c
	lib_init.c
	lib_io.c
	lib_jit.c
	lib_math.c
	lib_os.c
	lib_package.c
	lib_string.c
	lib_table.c
	lj_alloc.c
	lj_alloc.h
	ljamalg.c
	lj_api.c
	lj_arch.h
	lj_asm_arm.h
	lj_asm.c
	lj_asm.h
	lj_asm_mips.h
	lj_asm_ppc.h
	lj_asm_x86.h
	lj_bc.c
	lj_bcdump.h
	lj_bc.h
	lj_bcread.c
	lj_bcwrite.c
	lj_carith.c
	lj_carith.h
	lj_ccallback.c
	lj_ccallback.h
	lj_ccall.c
	lj_ccall.h
	lj_cconv.c
	lj_cconv.h
	lj_cdata.c
	lj_cdata.h
	lj_char.c
	lj_char.h
	lj_clib.c
	lj_clib.h
	lj_cparse.c
	lj_cparse.h
	lj_crecord.c
	lj_crecord.h
	lj_ctype.c
	lj_ctype.h
	lj_debug.c
	lj_debug.h
	lj_def.h
	lj_dispatch.c
	lj_dispatch.h
	lj_emit_arm.h
	lj_emit_mips.h
	lj_emit_ppc.h
	lj_emit_x86.h
	lj_err.c
	lj_err.h
	lj_errmsg.h
	lj_ff.h
	lj_ffrecord.c
	lj_ffrecord.h
	lj_frame.h
	lj_func.c
	lj_func.h
	lj_gc.c
	lj_gc.h
	lj_gdbjit.c
	lj_gdbjit.h
	lj_ir.c
	lj_ircall.h
	lj_ir.h
	lj_iropt.h
	lj_jit.h
	lj_lex.c
	lj_lex.h
	lj_lib.c
	lj_lib.h
	lj_load.c
	lj_mcode.c
	lj_mcode.h
	lj_meta.c
	lj_meta.h
	lj_obj.c
	lj_obj.h
	lj_opt_dce.c
	lj_opt_fold.c
	lj_opt_loop.c
	lj_opt_mem.c
	lj_opt_narrow.c
	lj_opt_sink.c
	lj_opt_split.c
	lj_parse.c
	lj_parse.h
	lj_record.c
	lj_record.h
	lj_snap.c
	lj_snap.h
	lj_state.c
	lj_state.h
	lj_str.c
	lj_str.h
	lj_strscan.c
	lj_strscan.h
	lj.supp
	lj_tab.c
	lj_tab.h
	lj_target_arm.h
	lj_target.h
	lj_target_mips.h
	lj_target_ppc.h
	lj_target_x86.h
	lj_trace.c
	lj_traceerr.h
	lj_trace.h
	lj_udata.c
	lj_udata.h
	lj_vmevent.c
	lj_vmevent.h
	lj_vm.h
	lj_vmmath.c
	luaconf.h
	lua.h
	luajit.c
	luajit.h
	lualib.h
)

#add_custom_target(
#	MYTARGET
#	OUTPUT STATIC lualib
#	COMMAND make
#)

#${CMAKE_CURRENT_SOURCE_FILE}

message("*************************************")
message("curent dir" ${CMAKE_CURRENT_SOURCE_DIR})
ExternalProject_Add(
	luajit
	DOWNLOAD_COMMAND ""
	SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	BUILD_COMMAND make
	)

ExternalProject_Get_Property(luajit install_dir)

add_library(lualib STATIC IMPORTED)
set_property(TARGET luajit PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libluajit-5.3.a )
add_dependencies(lualib luajit)



#ADD_LIBRARY ( lualib ${SRC} )

#ADD_EXECUTABLE ( lua luajit.c)

#TARGET_LINK_LIBRARIES ( lua lualib m dl readline)

