#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(_ARMASM_CMAKE_LOADED)
  return()
endif()
set(_ARMASM_CMAKE_LOADED TRUE)

get_filename_component(_CMAKE_ASM_TOOLCHAIN_LOCATION "${CMAKE_ASM_COMPILER}" PATH)

set(CMAKE_EXECUTABLE_SUFFIX ".axf")

find_program(CMAKE_ARMCCLANG_LINKER armlink HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )
find_program(CMAKE_ARMCCLANG_AR     armar   HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )
find_program(CMAKE_ARMCCLANG_FROMELF fromelf HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )

set(CMAKE_LINKER "${CMAKE_ARMCCLANG_LINKER}" CACHE FILEPATH "The ARMCC linker" FORCE)
mark_as_advanced(CMAKE_ARMCCLANG_LINKER)
set(CMAKE_AR "${CMAKE_ARMCCLANG_AR}" CACHE FILEPATH "The ARMCC archiver" FORCE)
mark_as_advanced(CMAKE_ARMCCLANG_AR)

macro(__compiler_armcc_asm)
  set(lang "ASM")
  if(NOT CMAKE_${lang}_FLAGS_SET)
    set(CMAKE_${lang}_FLAGS_SET TRUE)
    string(APPEND CMAKE_${lang}_FLAGS_INIT " ")
    string(APPEND CMAKE_${lang}_FLAGS_DEBUG_INIT " -g")
    string(APPEND CMAKE_${lang}_FLAGS_RELWITHDEBINFO_INIT " -g")

    set(CMAKE_${lang}_OUTPUT_EXTENSION ".o")
    set(CMAKE_${lang}_OUTPUT_EXTENSION_REPLACE 1)
    set(CMAKE_STATIC_LIBRARY_PREFIX_${lang} "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX_${lang} ".a")

    set(CMAKE_${lang}_LINK_EXECUTABLE      "<CMAKE_LINKER> <CMAKE_${lang}_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> <OBJECTS> -o <TARGET> --list <TARGET_BASE>.map")
    set(CMAKE_${lang}_CREATE_STATIC_LIBRARY  "<CMAKE_AR> --create -cr <TARGET> <LINK_FLAGS> <OBJECTS>")

    set(CMAKE_DEPFILE_FLAGS_${lang} "--depend=<DEPFILE>  --depend_format=unix_quoted")
  endif()
endmacro()
__compiler_armcc_asm()
