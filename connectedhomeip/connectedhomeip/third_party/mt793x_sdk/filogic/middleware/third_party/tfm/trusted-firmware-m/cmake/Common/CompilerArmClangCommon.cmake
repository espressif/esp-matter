#-------------------------------------------------------------------------------
# Copyright (c) 2017-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file contains settings to specify how ARMCLANG shall be used

function(check_armclang_input_vars MY_VERSION)
	#Specify where armclang is
	if (NOT DEFINED ARMCLANG_PATH)
		message(FATAL_ERROR "Please set ARMCLANG_PATH to the root directory of the armclang installation. e.g. set(ARMCLANG_PATH \"C:/Program Files/ARMCompiler${MY_VERSION}\")")
	endif()

	STRING(REGEX REPLACE "([0-9]+).([0-9]+).*" "\\1.\\2" _MY_MAJOR_MINOR "${MY_VERSION}")
	STRING(REGEX REPLACE "([0-9]+).([0-9]+).*" "\\1.\\2" _ARMCLANG_MAJOR_MINOR "${ARMCLANG_VER}")

	#Check armclang version.
	if (NOT "${_MY_MAJOR_MINOR}" VERSION_EQUAL "${_ARMCLANG_MAJOR_MINOR}")
		message(FATAL_ERROR "ARMClang version (ARMCLANG_VER=${ARMCLANG_VER}) does not match ${MY_VERSION}")
	endif()

	if (NOT DEFINED ARM_CPU_ARCHITECTURE AND NOT DEFINED ARM_CPU_TYPE)
		message(FATAL_ERROR "ARM_CPU_TYPE and ARM_CPU_ARCHITECTURE is not defined! Please include the CPU specific config file before this one.")
	endif()

endfunction()

message(STATUS "Using armclang compiler package v${ARMCLANG_VER} from ${ARMCLANG_PATH}")


#Tell cmake which compiler we use
if (EXISTS "c:/")
	set (CMAKE_C_COMPILER "${ARMCLANG_PATH}/bin/armclang.exe")
	set (CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/bin/armclang.exe")
	set (CMAKE_ASM_COMPILER "${ARMCLANG_PATH}/bin/armasm.exe")
else()
	set (CMAKE_C_COMPILER "${ARMCLANG_PATH}/bin/armclang")
	set (CMAKE_CXX_COMPILER "${ARMCLANG_PATH}/bin/armclang")
	set (CMAKE_ASM_COMPILER "${ARMCLANG_PATH}/bin/armasm")
endif()

if("CXX" IN_LIST languages)
	set(CMAKE_CXX_COMPILER_ID "ARMCLANG" CACHE INTERNAL "CXX compiler ID" FORCE)
	include(Compiler/ARMClang-CXX)
endif()

if("C" IN_LIST languages)
	set(CMAKE_C_COMPILER_ID "ARMCLANG" CACHE INTERNAL "C compiler ID" FORCE)
	include(Compiler/ARMClang-C)
endif()

if("ASM" IN_LIST languages)
	set(CMAKE_C_COMPILER_ID "ARMCLANG" CACHE INTERNAL "ASM compiler ID" FORCE)
	include(Compiler/ARMClang-ASM)
endif()

function(compiler_get_preinclude_option_string INCLUDE RES)
	set(${RES} "-include ${INCLUDE}" PARENT_SCOPE)
endfunction()

function(compiler_set_preinclude_file)
	#Option (on/off) arguments.
	set( _OPTIONS_ARGS GLOBAL)
	#Single option arguments.
	set( _ONE_VALUE_ARGS INCLUDE)
	#List arguments
	set( _MULTI_VALUE_ARGS TARGETS FILES)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )
	if(NOT DEFINED _MY_PARAMS)
        	message(FATAL_ERROR "compiler_set_preinclude_file: missing mandatory parameter INCLUDE.")
	endif()
	compiler_get_preinclude_option_string(${INCLUDE} _OPTION_STRING)
	#If include is to be set globally, we ignore TARGETS and FILES
	if(_MY_PARAMS_GLOBAL)
		set_property(DIRECTORY ${CMAKE_SOURCE_DIR} APPEND PROPERTY COMPILE_OPTIONS "${_OPTION_STRING}")
	else()
	#If GLOBAL was not passed, then either TARGETS or FILES must be present
		if(NOT DEFINED _MY_PARAM_TARGETS AND NOT DEFINED _MY_PARAM_FILES)
			message(FATAL_ERROR "compiler_set_preinclude_file: missing mandatory parameter. Either TARGETS and/or FILES must be specified.")
		endif()
		#Iterate over targets. Note: call embedded_set_target_compile_flags to
		#allow the target to be defined after this function call. This helps
		#modularisation
		foreach(_TGT IN_LISTS _MY_PARAM_TARGETS)
			embedded_set_target_compile_flags(TARGET ${_TGT} LANGUAGE "C" APPEND FLAGS "${_OPTION_STRING}")
		endforeach()
		#Iterate over files
		foreach(_FILE IN_LISTS _MY_PARAM_FILES)
			set_property(FILE ${_FILE} APPEND PROPERTY COMPILE_OPTIONS "${_OPTION_STRING}")
		endforeach()
	endif()
endfunction()

function(compiler_set_linkercmdfile)
	set( _OPTIONS_ARGS )							#Option (on/off) arguments.
	set( _ONE_VALUE_ARGS TARGET PATH)				#Single option arguments.
	set( _MULTI_VALUE_ARGS DEFINES INCLUDES)		#List arguments
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check passed parameters
	if(NOT _MY_PARAMS_TARGET)
		message(FATAL_ERROR "compiler_set_linkercmdfile: mandatory parameter 'TARGET' is missing.")
	endif()
	if (NOT TARGET ${_MY_PARAMS_TARGET})
		message(FATAL_ERROR "compiler_set_linkercmdfile: value of parameter 'TARGET' is invalid.")
	endif()

	if(NOT _MY_PARAMS_PATH)
		message(FATAL_ERROR "compiler_set_linkercmdfile: mandatory parameter 'PATH' is missing.")
	endif()
	set(_FILE_PATH ${_MY_PARAMS_PATH})

	#Compose additional command line switches from macro definitions.
	set(_FLAGS "")
	if (_MY_PARAMS_DEFINES)
		foreach(_DEFINE IN LISTS _MY_PARAMS_DEFINES)
			string(APPEND _FLAGS " --predefine=\"-D${_DEFINE}\"")
		endforeach()
	endif()
	#Compose additional command line switches from include paths.
	if (_MY_PARAMS_INCLUDES)
		foreach(_INCLUDE_P IN LISTS _MY_PARAMS_INCLUDES)
			string(APPEND _FLAGS " --predefine=\"-I${_INCLUDE_P}\"")
		endforeach()
	endif()

	#Note: the space before the option is important!
	set_property(TARGET ${_MY_PARAMS_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${_FLAGS} --scatter=${_FILE_PATH}")
	set_property(TARGET ${_MY_PARAMS_TARGET} APPEND PROPERTY LINK_DEPENDS ${_FILE_PATH})
	#Tell cmake .map files shall be removed when project is cleaned (make clean)
	get_filename_component(_TARGET_BASE_NAME ${_MY_PARAMS_TARGET} NAME_WE)
	get_directory_property(_ADDITIONAL_MAKE_CLEAN_FILES DIRECTORY "./" ADDITIONAL_MAKE_CLEAN_FILES)
	set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_ADDITIONAL_MAKE_CLEAN_FILES} ${_TARGET_BASE_NAME}.map")
endfunction()

function(compiler_set_cmse_output TARGET FILE_PATH)
	#Note: the space before the option is important!
	set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS " --import_cmse_lib_out=${FILE_PATH}")
	#Tell cmake cmse output is a generated object file.
	SET_SOURCE_FILES_PROPERTIES("${FILE_PATH}" PROPERTIES EXTERNAL_OBJECT true GENERATED true)
	#Tell cmake cmse output shall be removed by clean target.
	get_directory_property(_ADDITIONAL_MAKE_CLEAN_FILES DIRECTORY "./" ADDITIONAL_MAKE_CLEAN_FILES)
	set_directory_properties(PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${_ADDITIONAL_MAKE_CLEAN_FILES} ${FILE_PATH}")
endfunction()

function(compiler_merge_library)
	set( _OPTIONS_ARGS )			#Option (on/off) arguments.
	set( _ONE_VALUE_ARGS DEST)		#Single option arguments.
	set( _MULTI_VALUE_ARGS LIBS)	#List arguments
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check passed parameters
	if(NOT _MY_PARAMS_DEST)
		message(FATAL_ERROR "embedded_merge_library: no destination library target specified.")
	endif()
	#Check if destination is a target
	if(NOT TARGET ${_MY_PARAMS_DEST})
		message(FATAL_ERROR "embedded_merge_library: parameter DEST must be a target already defined.")
	endif()
	#Check if destination is a library
	get_target_property(_tmp ${_MY_PARAMS_DEST} TYPE)
	if(NOT "${_tmp}" STREQUAL "STATIC_LIBRARY")
		message(FATAL_ERROR "embedded_merge_library: parameter DEST must be a static library target.")
	endif()

	#Check list if libraries to be merged
	if(NOT _MY_PARAMS_LIBS)
		message(FATAL_ERROR "embedded_merge_library: no source libraries specified. Please see the LIBS parameter.")
	endif()

	#Mark each library file as a generated external object. This is needed to
	#avoid error because CMake has no info how these can be built.
	SET_SOURCE_FILES_PROPERTIES(
		${_MY_PARAMS_LIBS}
		PROPERTIES
		EXTERNAL_OBJECT true
		GENERATED true)

	#Add additional input to target
	target_sources(${_MY_PARAMS_DEST} PRIVATE ${_MY_PARAMS_LIBS})
endfunction()

function(compiler_generate_binary_output TARGET)
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_ARMCCLANG_FROMELF} ARGS --bincombined --output=$<TARGET_FILE_DIR:${TARGET}>/${TARGET}.bin $<TARGET_FILE:${TARGET}>)
endfunction()

# Function for creating a new target that preprocesses a .c file
#INPUTS:
#    SRC - (mandatory) - file to be preprocessed
#    DST - (mandatory) - output file for the preprocessing
#    TARGET_PREFIX - (optional) - prefix for the target that this function creates and which manages the preprocessing
#    BEFORE_TARGET - (optional) - target which is dependent on the preprocessing target in the below function
#    DEFINES - (optional) - additional command line switches from macro definitions for preprocessing
#    INCLUDES - (optional) - additional command line switches from include paths for preprocessing
function(compiler_preprocess_file)
	#Option (on/off) arguments.
	set( _OPTIONS_ARGS)
	#Single option arguments.
	set( _ONE_VALUE_ARGS SRC DST TARGET_PREFIX BEFORE_TARGET)
	#List arguments
	set( _MULTI_VALUE_ARGS DEFINES INCLUDES)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check passed parameters
	if(NOT DEFINED _MY_PARAMS_SRC)
		message(FATAL_ERROR "compiler_preprocess_file: mandatory parameter 'SRC' is missing.")
	endif()

	if(NOT DEFINED _MY_PARAMS_DST)
		message(FATAL_ERROR "compiler_preprocess_file: mandatory parameter 'DST' is missing.")
	endif()

	if(DEFINED _MY_PARAMS_BEFORE_TARGET)
		if(NOT TARGET ${_MY_PARAMS_BEFORE_TARGET})
			message(FATAL_ERROR "compiler_preprocess_file: optional parameter 'BEFORE_TARGET' is not target.")
		endif()
	endif()

	#Compose additional command line switches from macro definitions.
	set(_FLAGS "")
	if (_MY_PARAMS_DEFINES)
		foreach(_DEFINE IN LISTS _MY_PARAMS_DEFINES)
			list(APPEND _FLAGS "-D${_DEFINE}")
		endforeach()
	endif()

	#Compose additional command line switches from include paths.
	if (_MY_PARAMS_INCLUDES)
		foreach(_INCLUDE IN LISTS _MY_PARAMS_INCLUDES)
			list(APPEND _FLAGS "-I${_INCLUDE}")
		endforeach()
	endif()

	#The compiler flag might contain leading spaces which can fail the preprocess operation, these are removed
	STRING(STRIP ${CMAKE_C_FLAGS_CPU} _MY_TEMP_CMAKE_C_FLAGS_CPU)
	#If a string contains spaces, then it is inserted amongst quotation marks. Furthermore the compiler fails if it is
	#called with multiple switches included in one quotation mark. If the extra spaces are replaced by semicolons,
	#then the insertion will be advantageous for the compiler.
	STRING(REPLACE " " ";" _MY_TEMP2_CMAKE_C_FLAGS_CPU ${_MY_TEMP_CMAKE_C_FLAGS_CPU})
	set(_LOCAL_CMAKE_C_FLAGS_CPU "")
	foreach(_C_FLAG IN LISTS _MY_TEMP2_CMAKE_C_FLAGS_CPU)
		list(APPEND _LOCAL_CMAKE_C_FLAGS_CPU "${_C_FLAG}")
	endforeach()

	add_custom_command(OUTPUT ${_MY_PARAMS_DST}
		COMMAND ${CMAKE_C_COMPILER} ${_LOCAL_CMAKE_C_FLAGS_CPU} -E -P -xc ${_FLAGS} ${_MY_PARAMS_SRC} -o ${_MY_PARAMS_DST}
		DEPENDS ${_MY_PARAMS_SRC}
		COMMENT "Preprocess the ${_MY_PARAMS_SRC} file"
	)

	set(_MY_TARGET_PREFIX "")
	if(TARGET ${_MY_PARAMS_TARGET_PREFIX})
		set(_MY_TARGET_PREFIX "${_MY_PARAMS_TARGET_PREFIX}")
	endif()
	#The preprocessing related target name is obtained by indexing the file's name that is to be preprocessed
	get_filename_component(_MY_FILENAME_TO_BE_INDEXED ${_MY_PARAMS_SRC} NAME_WE)
	foreach(_SUFFIX RANGE 1 100)
		if (NOT TARGET ${_MY_TARGET_PREFIX}_pp_${_MY_FILENAME_TO_BE_INDEXED}_${_SUFFIX})
				set(_PREPROCESS_TARGET_NAME "${_MY_TARGET_PREFIX}_pp_${_MY_FILENAME_TO_BE_INDEXED}_${_SUFFIX}")
				break()
		endif()
		if (_SUFFIX EQUAL 100)
			message(FATAL_ERROR "You have called 'compiler_preprocess_file' too many times (${_SUFFIX} function calls).")
		endif()
	endforeach()

	#Make the original target depend on the new one.
	if(TARGET ${_MY_PARAMS_BEFORE_TARGET})
		add_custom_target(${_PREPROCESS_TARGET_NAME} DEPENDS ${_MY_PARAMS_DST})
		add_dependencies(${_MY_PARAMS_BEFORE_TARGET} ${_PREPROCESS_TARGET_NAME})
	else()
		add_custom_target(${_PREPROCESS_TARGET_NAME} ALL DEPENDS ${_MY_PARAMS_DST})
	endif()
endfunction()
