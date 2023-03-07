#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(_IARARM_CMAKE_LOADED_TFM)
  return()
endif()
set(_IARARM_CMAKE_LOADED_TFM TRUE)

get_filename_component(_CMAKE_C_TOOLCHAIN_LOCATION "${CMAKE_C_COMPILER}" PATH)
get_filename_component(_CMAKE_CXX_TOOLCHAIN_LOCATION "${CMAKE_CXX_COMPILER}" PATH)

set(CMAKE_EXECUTABLE_SUFFIX ".axf")

find_program(CMAKE_IARARM_LINKER ilinkarm HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )
find_program(CMAKE_IARARM_AR     iarchive   HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )
find_program(CMAKE_IARARM_IELFTOOL ielftool HINTS "${_CMAKE_C_TOOLCHAIN_LOCATION}" "${_CMAKE_CXX_TOOLCHAIN_LOCATION}" )

set(CMAKE_LINKER "${CMAKE_IARARM_LINKER}" CACHE FILEPATH "The IAR linker" FORCE)
mark_as_advanced(CMAKE_IARARM_LINKER)
set(CMAKE_AR "${CMAKE_IARARM_AR}" CACHE FILEPATH "The IAR archiver" FORCE)
mark_as_advanced(CMAKE_IARARM_AR)

macro(__compiler_iararm lang)
  if(NOT CMAKE_${lang}_FLAGS_SET)
    set(CMAKE_${lang}_FLAGS_SET TRUE)
    set(CMAKE_${lang}_RESPONSE_FILE_LINK_FLAG "-f ")
    string(APPEND CMAKE_${lang}_FLAGS_INIT " ")

    if (NOT ${lang} STREQUAL "ASM")
      string(APPEND CMAKE_${lang}_FLAGS_DEBUG_INIT " --debug -On")
      string(APPEND CMAKE_${lang}_FLAGS_MINSIZEREL_INIT " -Ohz -DNDEBUG")
      string(APPEND CMAKE_${lang}_FLAGS_RELEASE_INIT " -Ohs -DNDEBUG")
      string(APPEND CMAKE_${lang}_FLAGS_RELWITHDEBINFO_INIT " -Ohs --debug")
    endif()

    set(CMAKE_${lang}_OUTPUT_EXTENSION ".o")
    set(CMAKE_${lang}_OUTPUT_EXTENSION_REPLACE 1)
    set(CMAKE_STATIC_LIBRARY_PREFIX_${lang} "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX_${lang} ".a")

    set(CMAKE_${lang}_USE_RESPONSE_FILE_FOR_OBJECTS 1)
    set(CMAKE_${lang}_RESPONSE_FILE_LINK_FLAG "-f ")

    set(CMAKE_${lang}_LINK_EXECUTABLE      "<CMAKE_LINKER> <CMAKE_${lang}_LINK_FLAGS> <LINK_FLAGS> <LINK_LIBRARIES> <OBJECTS> -o <TARGET> --map <TARGET_BASE>.map")
    set(CMAKE_${lang}_CREATE_STATIC_LIBRARY  "<CMAKE_AR> --create <TARGET> <LINK_FLAGS> <OBJECTS>")
    set(CMAKE_${lang}_CREATE_PREPROCESSED_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <INCLUDES> <FLAGS> --preprocess=n <PREPROCESSED_SOURCE> <SOURCE> ")
    set(CMAKE_${lang}_CREATE_ASSEMBLY_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <INCLUDES> <FLAGS> <SOURCE> -la <ASSEMBLY_SOURCE>")

    set(CMAKE_DEPFILE_FLAGS_${lang} "--dependencies=ins <DEPFILE>")
  endif()
endmacro()
