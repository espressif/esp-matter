#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(_ARMCCLANG_CMAKE_LOADED)
  return()
endif()
set(_ARMCCLANG_CMAKE_LOADED TRUE)

get_filename_component(_CMAKE_C_TOOLCHAIN_LOCATION "${CMAKE_C_COMPILER}" PATH)
get_filename_component(_CMAKE_CXX_TOOLCHAIN_LOCATION "${CMAKE_CXX_COMPILER}" PATH)

set(CMAKE_EXECUTABLE_SUFFIX ".axf")

find_program(CMAKE_ARMCCLANG_LINKER armlink HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )
find_program(CMAKE_ARMCCLANG_AR     armar   HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )
find_program(CMAKE_ARMCCLANG_FROMELF fromelf HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )

set(CMAKE_LINKER "${CMAKE_ARMCCLANG_LINKER}" CACHE FILEPATH "The ARMCC linker" FORCE)
mark_as_advanced(CMAKE_ARMCCLANG_LINKER)
set(CMAKE_AR "${CMAKE_ARMCCLANG_AR}" CACHE FILEPATH "The ARMCC archiver" FORCE)
mark_as_advanced(CMAKE_ARMCCLANG_AR)

if(NOT DEFINED BUILD_DWARF_VERSION)
	set(BUILD_DWARF_VERSION "")
else()
	string(CONCAT BUILD_DWARF_VERSION "dwarf-" ${BUILD_DWARF_VERSION})
endif()

macro(__compiler_armclang lang)
  if(NOT CMAKE_${lang}_FLAGS_SET)
    set(CMAKE_${lang}_FLAGS_SET TRUE)
    string(APPEND CMAKE_${lang}_FLAGS_INIT " ")
    string(APPEND CMAKE_${lang}_FLAGS_DEBUG_INIT " -g${BUILD_DWARF_VERSION} -O0")
    string(APPEND CMAKE_${lang}_FLAGS_MINSIZEREL_INIT " -Oz -DNDEBUG")
    string(APPEND CMAKE_${lang}_FLAGS_RELEASE_INIT " -O3 -DNDEBUG")
    string(APPEND CMAKE_${lang}_FLAGS_RELWITHDEBINFO_INIT " -O3 -g${BUILD_DWARF_VERSION} -DNDEBUG")

    set(CMAKE_${lang}_OUTPUT_EXTENSION ".o")
    set(CMAKE_${lang}_OUTPUT_EXTENSION_REPLACE 1)
    set(CMAKE_STATIC_LIBRARY_PREFIX_${lang} "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX_${lang} ".a")

    set(CMAKE_${lang}_RESPONSE_FILE_LINK_FLAG "--via=")

    set(CMAKE_${lang}_LINK_EXECUTABLE      "<CMAKE_LINKER> <CMAKE_${lang}_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> <OBJECTS> -o <TARGET> --list <TARGET_BASE>.map")
    set(CMAKE_${lang}_CREATE_STATIC_LIBRARY  "<CMAKE_AR> --create -cr <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_${lang}_CREATE_PREPROCESSED_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -E <SOURCE> > <PREPROCESSED_SOURCE>")
    set(CMAKE_${lang}_CREATE_ASSEMBLY_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -S <SOURCE> -o <ASSEMBLY_SOURCE>")

    set(CMAKE_DEPFILE_FLAGS_${lang} "--depend=<DEPFILE> --depend_single_line --no_depend_system_headers")
  endif()
endmacro()
