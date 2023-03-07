#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file defines project-specific overrides to cmake default behaviour for:
#	* compiler detection
#	* target system detection
cmake_minimum_required(VERSION 3.3) #IN_LIST was introduced in 3.3
cmake_policy(SET CMP0057 NEW)

Include(CMakeParseArguments)

#The CMAKE_SYSTEM_XXX settings make cmake to stop applying some target system specific settings.
#Tell cmake we are compiling for ARM chips.
set (CMAKE_SYSTEM_PROCESSOR "arm" CACHE INTERNAL "Set target processor type to force cmake include some of our scripts." FORCE)
#Tell cmake this is an "Embedded" system
set(CMAKE_SYSTEM_NAME "Embedded"  CACHE INTERNAL "Set target system name to force cmake include some of our scripts." FORCE)

#Stop built in CMakeDetermine<lang>.cmake scripts to run.
set (CMAKE_CXX_COMPILER_ID_RUN 1)
#Stop cmake run compiler tests.
set (CMAKE_CXX_COMPILER_FORCED true)
#Stop built in CMakeDetermine<lang>.cmake scripts to run.
set (CMAKE_C_COMPILER_ID_RUN 1)
#Stop cmake run compiler tests.
set (CMAKE_C_COMPILER_FORCED true)

#This macro is used to enforce the ARM project structure.
#Inputs: (This macro uses some "global" variables)
#	global variable PROJ_CONFIG - a global configuration file to be used by all projects.
#								  It overrides value of CONFIG parameter.
#	CONFIG - the configuration file which shall be used
#Examples
#	To use global project config:
#		embedded_project_start()
#	To use config file relative to the top level CmakeLists.txt:
#		embedded_project_start(./configs/ConfigDefault.cmake)
#	To use config file relative to the CmakeLists.txt file where this macro is used:
#		embedded_project_start(${TFM_ROOT_DIR}/configs/ConfigDefault.cmake)
macro(embedded_project_start)
	#Default project configuration file
	if (DEFINED PROJ_CONFIG) #Take the global setting as default value
		set(_PROJ_CONFIG ${PROJ_CONFIG})
	endif()

	set( _OPTIONS_ARGS )					#No option (on/off) arguments
	set( _ONE_VALUE_ARGS CONFIG)	#Single option arguments (e.g. PROJ_NAME "bubu_project")
	set( _MULTI_VALUE_ARGS )		#One list argument (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check passed parameters
	if(NOT _MY_PARAMS_CONFIG)
		if(NOT DEFINED _PROJ_CONFIG)
			set(_PROJ_CONFIG "${TFM_ROOT_DIR}/configs/${TFM_CONFIG_FILE}")
			message(STATUS "embedded_project_start: no project configuration file defined, falling back to default.")
		endif()
	elseif(NOT DEFINED PROJ_CONFIG)
		set(_PROJ_CONFIG ${_MY_PARAMS_CONFIG})
	endif()

	get_filename_component(_ABS_PROJ_CONFIG ${_PROJ_CONFIG} ABSOLUTE)
	message(STATUS "embedded_project_start: using project specific config file (PROJ_CONFIG = ${_ABS_PROJ_CONFIG})")
	include("${_PROJ_CONFIG}")
endmacro()

#Override CMake default behaviour
macro(embedded_project_fixup)
	get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)

	#Merge CPU and configuration specific compiler and linker flags.
	foreach(LNG ${languages})
		include(Common/CompilerDetermine${LNG})
		#since all CMake "built in" scripts already executed, we need fo fix up some things here.
		embedded_fixup_build_type_vars(${LNG})

		#Apply CPU specific and configuration specific compile flags.
		if(NOT CMAKE_${LNG}_FLAGS MATCHES ".*${CMAKE_${LNG}_FLAGS_CPU}.*")
			set(CMAKE_${LNG}_FLAGS "${CMAKE_${LNG}_FLAGS} ${CMAKE_${LNG}_FLAGS_CPU}")
		endif()
		#Fix output file extension.
		set (CMAKE_EXECUTABLE_SUFFIX_${LNG} ".axf")
		unset(ALL_SRC_${LNG})
		unset(ALL_SRC_${LNG}_S)
		unset(ALL_SRC_${LNG}_NS)
	endforeach()
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_LINK_FLAGS_CPU}")
endmacro()

#Allow project specific script to do configuration after all targets are specified; it has to be done for each target defined
macro (embedded_project_end TARGET)
	get_property(_MAC DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY MACROS)
	if ("embedded_project_build_config_apply" IN_LIST _MAC)
		message(WARNING "embedded_project_end(): macro embedded_project_build_config_apply() is defined. Your build config file may be out-dated.")
	endif()

	#Apply compile flags.
	_embedded_apply_compile_flags(${TARGET})
	#Apply macro definitions
	_embedded_apply_compile_defines(${TARGET})
	#Apply include paths
	_embedded_apply_include_directories(${TARGET})
	#Apply linker flags.
	_embedded_apply_link_flags(${TARGET})
	#If target is executable, apply linker command file setting.
	get_property(_TGT_TYPE TARGET ${TARGET} PROPERTY TYPE)
	if(_TGT_TYPE STREQUAL "EXECUTABLE")
		_embedded_apply_linker_cmd_file_setting(${TARGET})
	endif()
endmacro()

macro(embedded_fixup_build_type_vars LANG)
	#since all CMake "built in" scripts already executed, we need fo fix up some things here.
	set (CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG_INIT}" CACHE STRING "Flags used by the compiler during debug builds." FORCE)
	set (CMAKE_${LANG}_FLAGS_MINSIZEREL "${CMAKE_${LANG}_FLAGS_MINSIZEREL_INIT}" CACHE STRING "Flags used by the compiler during release builds for minimum size." FORCE)
	set (CMAKE_${LANG}_FLAGS_RELEASE "${CMAKE_${LANG}_FLAGS_RELEASE_INIT}" CACHE STRING "Flags used by the compiler during release builds." FORCE)
	set (CMAKE_${LANG}_FLAGS_RELWITHDEBINFO "${CMAKE_${LANG}_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "Flags used by the compiler during release builds with debug info." FORCE)
endmacro()


#Convert a CMake list to a string
#
#Examples:
#  list_to_string(my_string 1 2 3 4)
#  list_to_string(my_string ${CMAKE_C_FLAGS})
#
#INPUTS:
#    RES  - (mandatory) - name of variable to put result in
#    The list to be converted.
#
#OUTPUTS
#    List items concatenated to a string.
#
function(list_to_string RES)
	foreach(_ITEM ${ARGN})
		set(_RES "${_RES} ${_ITEM}")
	endforeach()
	set(${RES} "${_RES}" PARENT_SCOPE)
endfunction()

#Ensure current generator is supported.
#
# This function takes a list of supported generators (e.g. "Unix Makefiles") and
# exits with fatal error is the current generator is not on the list.
#Examples:
#  assert_generator_is("MSYS Makefiles" "MinGW Makefiles")
#
#INPUTS:
#    The list of supported generators.
#
#OUTPUTS
#    n/a
#
function(assert_generator_is)
		if (NOT CMAKE_GENERATOR IN_LIST ARGN)
			message(FATAL_ERROR "assert_generator_is(): Generator '${CMAKE_GENERATOR}' is not on the list of supported generators.")
		endif()
endfunction()

#Check the value of a cache variable whether it is valid.
#
# This function currently only supports 'STRING' type variables and uses
# the 'STRINGS' cache entry property as the validation list.
#
#Examples:
#  validate_cache_value(MCUBOOT_SIGNATURE_TYPE)
#
#INPUTS:
#  variable_name - (mandatory) - Name of the cache variable to be checked.
#
#OUTPUTS:
#  n/a
#
function(validate_cache_value variable_name)
	#Check if the type of the variable is STRING.
	get_property(_type CACHE ${variable_name} PROPERTY TYPE)
	if(NOT ${_type} STREQUAL "STRING")
		message(FATAL_ERROR "validate_cache_value: type of CACHE variable must be 'STRING', the type of '${variable_name}' variable is '${_type}'.")
	endif()
	get_property(_validation_list CACHE ${variable_name} PROPERTY STRINGS)
	#Check if validation list is set.
	if (NOT _validation_list)
		message(FATAL_ERROR "validate_cache_value: CACHE variable '${variable_name}' has no 'STRINGS' validation list set.")
	endif()
	#See if the value of the variable is in the list.
	if (NOT ${${variable_name}} IN_LIST _validation_list)
		message(FATAL_ERROR "validate_cache_value: variable value '${${variable_name}}' of variable '${variable_name}' is not matching the validation list ${_validation_list}.")
	endif()
endfunction()

#Specify an include path for the compiler
#
# Specify a global include directory for all non external targets in the current
# build.
# The parameter ABSOLUTE can be set to true to ask CMake to convert the PATH to
# absolute. This gives better looking command line arguments, and also helps
# removing duplicates.
#
#Examples:
#  embedded_include_directories(PATH "C:/fo/bar/include")
#  embedded_include_directories(PATH "C:/fo/bar/include" ABSOLUTE)
#
#INPUTS:
#    PATH  - (mandatory) - the include path to add
#    ABSOLUTE - (optional) - whether the path shall be converted to absolute
#
#OUTPUTS
#    Modified list of include directories.
#
function (embedded_include_directories)
	#Parse our arguments
	set( _OPTIONS_ARGS ABSOLUTE)		#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  PATH)		#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )		#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_PATH)
		failure("embedded_include_directories(): Missing PATH parameter!")
	endif()

	if(DEFINED _MY_PARAMS_ABSOLUTE AND ${_MY_PARAMS_ABSOLUTE})
		get_filename_component(_MY_PARAMS_PATH ${_MY_PARAMS_PATH} ABSOLUTE)
	endif()

	include_directories(${_MY_PARAMS_PATH})
endfunction()

#Return the language of a source file.
#
# Language is either specified by the LANGUAGE property of the source file or
# determined based on the extension of the file.
# Search is limited for languages enabled in the current project.
#
#Examples:
#  To get language of "foo/bar.c" written into _LNG:
#  embedded_get_source_language("foo/bar.c" _LNG)
#
#INPUTS:
#   FILE  - (mandatory) - The file to determine language of.
#   RES   - (mandatory) - Name of the variable to write the result into.
#
#OUTPUTS
#   ${RES} - language string (e.g. "C", "CXX", "ASM"). empty string for files
#			 with no language.
#
function(embedded_get_source_language FILE RES)
	if (ARGN)
		message(FATAL_ERROR "embedded_get_source_language(): too many parameters passed.")
	endif()
	#If language property is set, use that.
	get_property(_LNG SOURCE ${_SRC} PROPERTY LANGUAGE)
	if (NOT ${_LNG} STREQUAL "")
		set(${RES} ${_LNG} PARENT_SCOPE)
	else()
		#Set empty return value.
		set(${RES} "" PARENT_SCOPE)
		#Property not set, use extension of the file to determine
		#language.
		string(REGEX MATCH "[^.]*$" _EXT "${_SRC}")
		#Get list of enabled languages.
		get_property(_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)
		foreach(_LNG ${_LANGUAGES})
			#See if the extension is contained in the list of file extensions
			#of this language.
			if(_EXT IN_LIST CMAKE_${_LNG}_SOURCE_FILE_EXTENSIONS)
				set(${RES} ${_LNG} PARENT_SCOPE)
				break()
			endif()
		endforeach()
	endif()
endfunction()

#Set compilation flags for the specified target.
#
# Store compilation flags for the specified target and language pair. Flags are
# stored in a global property and will
# be applied to source files in the current directory and sub-directories.
# Property name must follow a specific scheme (see outputs).
# See: _embedded_apply_compile_flags()
#
#Examples:
#  embedded_set_target_compile_flags(my_app C "-fchar-unsigned")
#  embedded_set_target_compile_flags(my_lib CXX "-fchar-unsigned")
#  embedded_set_target_compile_flags(my_app ASM "-fchar-unsigned")
#
#INPUTS:
#   TARGET   - (mandatory) - The target to apply settings to.
#   LANGUAGE - (mandatory) - Programming language of source files settings shall
#							 be applied to.
#	FLAGS    - (mandatory) - List with the compiler flags.
#	APPEND   - (optional)  - True if FLAGS shall be appended.
#
#OUTPUTS
#	Directory property EMBEDDED_COMPILE_FLAGS_TTT_LLL is set, where TTT is the
#	target name, and LLL is the language.
#
function (embedded_set_target_compile_flags)
	set( _OPTIONS_ARGS APPEND)		#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  TARGET LANGUAGE)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS FLAGS)		#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	if (NOT DEFINED _MY_PARAMS_TARGET)
		message(FATAL_ERROR "embedded_set_target_compile_flags(): mandatory parameter 'TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_LANGUAGE)
		message(FATAL_ERROR "embedded_set_target_compile_flags(): mandatory parameter 'LANGUAGE' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_FLAGS)
		message(FATAL_ERROR "embedded_set_target_compile_flags(): mandatory parameter 'FLAGS' missing.")
	endif()

	if (_MY_PARAMS_APPEND)
		set_property(GLOBAL APPEND PROPERTY EMBEDDED_COMPILE_FLAGS_${_MY_PARAMS_TARGET}_${_MY_PARAMS_LANGUAGE} ${_MY_PARAMS_FLAGS})
	else()
		set_property(GLOBAL PROPERTY EMBEDDED_COMPILE_FLAGS_${_MY_PARAMS_TARGET}_${_MY_PARAMS_LANGUAGE} ${_MY_PARAMS_FLAGS})
	endif()
endfunction()

#Apply compilation flag settings for the specified target.
#
# Compilation flags stored in a global property and are applied to source files.
# Note:
#	- Directory property name must follow a specific scheme.
#	- This is an internal function.
#	- This function only supports make and ninja generators.
#
# See: embedded_set_target_compile_flags()
#
#Examples:
#  _embedded_apply_compile_flags(my_app)
#
#INPUTS:
#   TARGET  - (mandatory) 			- The target to apply settings to.
#	Directory property - (optional) - Flags to apply.
#
#OUTPUTS
#    n/a
#
function(_embedded_apply_compile_flags TARGET)
	#Check if the parameter is a target.
  	if(NOT TARGET ${TARGET})
		message(FATAL_ERROR "_embedded_apply_compile_flags(): target '${TARGET}' is not defined.")
	endif()
	#Get list of enabled languages.
	get_property(_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)
	foreach(_LNG ${_LANGUAGES})
		#Get the flags for this language.
		get_property(_FLAGS GLOBAL PROPERTY EMBEDDED_COMPILE_FLAGS_${TARGET}_${_LNG})

		#The generator expression below is only supported by the make and ninja
		#generators.
		assert_generator_is(_GENERATORS_OK "MSYS Makefiles" "MinGW Makefiles" "NMake Makefiles" "NMake Makefiles JOM" "Unix Makefiles" "Watcom WMake" "Ninja")
		target_compile_options(${TARGET} PRIVATE $<$<COMPILE_LANGUAGE:${_LNG}>:${_FLAGS}>)
	endforeach()
endfunction()

#Set compilation defines for the specified target.
#
# Store compilation defines for the specified target and language pair. Macros
# are stored in a global property and will
# be applied to source files in the current directory and sub-directories.
# Property name must follow a specific scheme (see outputs).
# See: _embedded_apply_compile_defines()
#
#Examples:
#  embedded_set_target_compile_defines(my_app C "FOO BAR=1")
#  embedded_set_target_compile_defines(my_lib CXX "FOO BAR=1")
#  embedded_set_target_compile_defines(my_app ASM "FOO BAR=1")
#
#INPUTS:
#   TARGET   - (mandatory) - The target to apply settings to.
#   LANGUAGE - (mandatory) - Programming language of source files settings shall
#							 be applied to.
#	DEFINES  - (mandatory) - List with the compiler flags.
#	APPEND   - (optional)  - Present if FLAGS shall be appended.
#
#OUTPUTS
#	Directory property EMBEDDED_COMPILE_DEFINES_TTT_LLL is set, where TTT is the
#	target name, and LLL is the language.
#
function (embedded_set_target_compile_defines)
	set( _OPTIONS_ARGS APPEND)		#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  TARGET LANGUAGE)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS DEFINES)		#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	if (NOT DEFINED _MY_PARAMS_TARGET)
		message(FATAL_ERROR "embedded_set_target_compile_defines(): mandatory parameter 'TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_LANGUAGE)
		message(FATAL_ERROR "embedded_set_target_compile_defines(): mandatory parameter 'LANGUAGE' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_DEFINES)
		message(FATAL_ERROR "embedded_set_target_compile_defines(): mandatory parameter 'DEFINES' missing.")
	endif()

	if (_MY_PARAMS_APPEND)
		set_property(GLOBAL APPEND PROPERTY EMBEDDED_COMPILE_DEFINES_${_MY_PARAMS_TARGET}_${_MY_PARAMS_LANGUAGE} ${_MY_PARAMS_DEFINES})
	else()
		set_property(GLOBAL PROPERTY EMBEDDED_COMPILE_DEFINES_${_MY_PARAMS_TARGET}_${_MY_PARAMS_LANGUAGE} ${_MY_PARAMS_DEFINES})
	endif()
endfunction()

#Apply compilation defines for the specified target.
#
# Macro definitions are stored in a global property and are applied to
# source files.
#
# Note:
#	- Directory property name must follow a specific scheme.
#	- This is an internal function.
#	- This function only supports make and ninja generators.
#
# See: embedded_set_target_compile_defines()
#
#Examples:
#  _embedded_apply_compile_defines(my_app)
#
#INPUTS:
#   TARGET  - (mandatory) 			- The target to apply settings to.
#	Directory property - (optional) - Flags to apply.
#
#OUTPUTS
#    n/a
#
function(_embedded_apply_compile_defines TARGET)
	#Check if the parameter is a target.
  	if(NOT TARGET ${TARGET})
		message(FATAL_ERROR "_embedded_apply_compile_defines(): target '${TARGET}' is not defined.")
	endif()
	#Get list of enabled languages.
	get_property(_LANGUAGES GLOBAL PROPERTY ENABLED_LANGUAGES)
	foreach(_LNG ${_LANGUAGES})
		#Get the flags for this language.
		get_property(_FLAGS GLOBAL PROPERTY EMBEDDED_COMPILE_DEFINES_${TARGET}_${_LNG})
		#The generator expression below is only supported by the make and ninja
		#generators.
		assert_generator_is(_GENERATORS_OK "MSYS Makefiles" "MinGW Makefiles" "NMake Makefiles" "NMake Makefiles JOM" "Unix Makefiles" "Watcom WMake" "Ninja")
		target_compile_definitions(${TARGET} PRIVATE $<$<COMPILE_LANGUAGE:${_LNG}>:${_FLAGS}>)
	endforeach()
endfunction()

#Specify an include path for the compiler affecting a specific build target (all
# languages).
#
# Store include paths for the specified target. PATH is stored in a global
# property. The property name must follow a specific scheme (see outputs).
# See: _embedded_apply_include_directories()
#
#Examples:
#  embedded_target_include_directories(TARGET foo PATH "C:/fo/bar/include")
#  embedded_target_include_directories(TARGET foo PATH "C:/fo/bar/include" APPEND)
#  embedded_target_include_directories(TARGET foo PATH "C:/fo/bar/include" ABSOLUTE)
#
#INPUTS:
#    ABSOLUTE - (optional)- whether the path shall be converted to absolute
#	 APPEND - (optional)  - if set append path to existing values
#    PATH  - (mandatory)  - the include path to add
#	 TARGET - (mandatory) - name of target to apply settings to
#
#OUTPUTS
#	Directory property EMBEDDED_COMPILE_INCLUDES_TTT is set, where TTT is
#	the target name.
#
function (embedded_target_include_directories)
	#Parse our arguments
	set( _OPTIONS_ARGS ABSOLUTE APPEND)	#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  PATH TARGET) #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )		#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_PATH)
		failure("embedded_target_include_directories(): Missing PATH parameter!")
	endif()

	if(NOT _MY_PARAMS_TARGET)
		failure("embedded_target_include_directories(): Missing TARGET parameter!")
	endif()

	if(DEFINED _MY_PARAMS_ABSOLUTE AND ${_MY_PARAMS_ABSOLUTE})
		get_filename_component(_MY_PARAMS_PATH ${_MY_PARAMS_PATH} ABSOLUTE)
	endif()

	if (_MY_PARAMS_APPEND)
		set_property(GLOBAL APPEND PROPERTY EMBEDDED_COMPILE_INCLUDES_${_MY_PARAMS_TARGET} ${_MY_PARAMS_PATH})
	else()
		set_property(GLOBAL PROPERTY EMBEDDED_COMPILE_INCLUDES_${_MY_PARAMS_TARGET} ${_MY_PARAMS_PATH})
	endif()
endfunction()

#Apply include path settings for the specified target.
#
# Include paths are stored in a global property and are applied to source files.
# Note:
#	- Directory property name must follow a specific scheme.
#	- This is an internal function.
#
# See: embedded_target_include_directories()
#
#Examples:
#  _embedded_apply_include_directories(my_app)
#
#INPUTS:
#   TARGET  - (mandatory) 			- The target to apply settings to.
#	Directory property - (optional) - Flags to apply.
#
#OUTPUTS
#    n/a
#
function(_embedded_apply_include_directories TARGET)
	#Check if the parameter is a target.
  	if(NOT TARGET ${TARGET})
		message(FATAL_ERROR "_embedded_apply_include_directories(): target '${TARGET}' is not defined.")
	endif()
	#Get the flags for this language.
	get_property(_FLAGS GLOBAL PROPERTY EMBEDDED_COMPILE_INCLUDES_${TARGET})
	#If we have flags to apply for this language.
	if (NOT _FLAGS STREQUAL "")
		target_include_directories(${TARGET} PRIVATE ${_FLAGS})
	endif()
endfunction()

#Set linker flags for the specified target.
#
# Store linker flags for the specified target in a global property.
# See: _embedded_apply_link_flags()
#
#Examples:
#  embedded_set_target_link_flags(my_app "-M my_map_file.map")
#
#INPUTS:
#   TARGET  - (mandatory) - The target to apply settings to.
#	FLAGS   - (mandatory) - List with the compiler flags.
#	APPEND  - (optional)  - True if FLAGS shall be appended.
#
#OUTPUTS
#	Directory property EMBEDDED_LINKER_FLAGS_TTT is set, where TTT is the
#	target name.
#
function(embedded_set_target_link_flags)
	set( _OPTIONS_ARGS APPEND)		#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  TARGET)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS FLAGS)		#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	if (NOT DEFINED _MY_PARAMS_TARGET)
		message(FATAL_ERROR "embedded_set_target_link_flags(): mandatory parameter 'TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_FLAGS)
		message(FATAL_ERROR "embedded_set_target_link_flags(): mandatory parameter 'FLAGS' missing.")
	endif()

	if (_MY_PARAMS_APPEND)
		set_property(GLOBAL APPEND PROPERTY EMBEDDED_LINKER_FLAGS_${_MY_PARAMS_TARGET} ${_MY_PARAMS_FLAGS})
	else()
		set_property(GLOBAL PROPERTY EMBEDDED_LINKER_FLAGS_${_MY_PARAMS_TARGET} ${_MY_PARAMS_FLAGS})
	endif()
endfunction()

#Apply linker flags for the specified target.
#
# Linker flags stored in a global property are applied.
#
# Note:
#	- Directory property name must follow a specific scheme.
#	- This is an internal function.
#
# See: embedded_set_target_link_flags()
#
#Examples:
#  _embedded_apply_link_flags(my_app)
#
#INPUTS:
#   TARGET  - (mandatory) 			- The target to apply settings to.
#	Directory property - (optional) - Flags to apply.
#
#OUTPUTS
#    n/a
#
function(_embedded_apply_link_flags TARGET)
	#Check if the parameter is a target.
	if(NOT TARGET ${TARGET})
		message(FATAL_ERROR "_embedded_apply_link_flags(): target '${TARGET}' is not defined.")
	endif()
	#Get the stored flags.
	get_property(_FLAGS GLOBAL PROPERTY EMBEDDED_LINKER_FLAGS_${TARGET})
	#Apply flags if defined.
	if (NOT _FLAGS STREQUAL "")
		list_to_string(_STR_FLAGS ${_FLAGS})
		set_property(TARGET ${TARGET} APPEND_STRING PROPERTY LINK_FLAGS  ${_STR_FLAGS})
	endif()
endfunction()

#Set linker command file for the specified target.
#
# Store path to linker command file for the specified target in a global
# property.
#
# See: _embedded_apply_linker_cmd_file_setting()
#
#Examples:
#  embedded_set_target_linker_file(TARGET my_app PATH "foo/my_linker_cmd.sct")
#
#INPUTS:
#  TARGET  - (mandatory) - The target to apply settings to.
#  PATH - (mandatory)    - Path to linker script.
#
#OUTPUTS
#  Directory property EMBEDDED_LINKER_CMD_FILE_TTT is set, where TTT is the
#  target name.
#
function(embedded_set_target_linker_file)
	set( _OPTIONS_ARGS )				#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  TARGET PATH)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )			#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	if (NOT DEFINED _MY_PARAMS_TARGET)
		message(FATAL_ERROR "embedded_set_target_linker_file(): mandatory parameter 'TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_PATH)
		message(FATAL_ERROR "embedded_set_target_linker_file(): mandatory parameter 'PATH' missing.")
	endif()

	set_property(GLOBAL PROPERTY EMBEDDED_LINKER_CMD_FILE_${_MY_PARAMS_TARGET} ${_MY_PARAMS_PATH})
endfunction()

#Set pre-processor defines for the linker command file.
#
# Store preprocessor defines for the linker command file of the specified target
# in a global property.
#
# See: _embedded_apply_linker_cmd_file_setting()
#
#Examples:
#  embedded_set_target_link_defines(my_app "BL2=1" "USE_TLS=1")
#
#INPUTS:
#   TARGET  - (mandatory) - The target to apply settings to.
#   DEFINES - (mandatory) - List of macro value definitions.
#
#OUTPUTS
#   Directory property EMBEDDED_LINKER_DEFINES_TTT is set, where TTT is the
#   target name.
#
function(embedded_set_target_link_defines)
	set( _OPTIONS_ARGS )				#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS TARGET)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS DEFINES)			#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	if (NOT DEFINED _MY_PARAMS_TARGET)
		message(FATAL_ERROR "embedded_set_target_link_defines(): mandatory parameter 'TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_DEFINES)
		message(FATAL_ERROR "embedded_set_target_link_defines(): mandatory parameter 'DEFINES' missing.")
	endif()

	set_property(GLOBAL APPEND PROPERTY EMBEDDED_LINKER_DEFINES_${_MY_PARAMS_TARGET} ${_MY_PARAMS_DEFINES})
endfunction()


#Set pre-processor include paths for the linker command file.
#
# Store preprocessor include paths for the linker command file of the specified
# target in a global property.
#
# See: _embedded_apply_linker_cmd_file_setting()
#
#Examples:
#  embedded_set_target_link_includes(my_app "c:/foo" "../bar")
#
#INPUTS:
#   TARGET  - (mandatory)  - The target to apply settings to.
#   INCLUDES - (mandatory) - List of include paths.
#
#OUTPUTS
#   Directory property EMBEDDED_LINKER_INCLUDES_TTT is set, where TTT is the
#   target name.
#
function(embedded_set_target_link_includes)
	set( _OPTIONS_ARGS )				#Option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  TARGET)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS INCLUDES)			#List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	if (NOT DEFINED _MY_PARAMS_TARGET)
		message(FATAL_ERROR "embedded_set_target_link_includes(): mandatory parameter 'TARGET' missing.")
	endif()

	if (NOT DEFINED _MY_PARAMS_INCLUDES)
		message(FATAL_ERROR "embedded_set_target_link_includes(): mandatory parameter 'INCLUDES' missing.")
	endif()

	set_property(GLOBAL APPEND PROPERTY EMBEDDED_LINKER_INCLUDES_${_MY_PARAMS_TARGET} ${_MY_PARAMS_INCLUDES})
endfunction()


#Apply linker linker command file setting for the specified target.
#
# Path to linker command file, macro definitions and include paths stored in
# global properties are applied.
#
# Note:
#  - Directory property names must follow a specific scheme.
#  - This is an internal function.
#
# See: embedded_set_target_linker_file(), embedded_set_target_link_includes()
#      embedded_set_target_link_defines()
#
#Examples:
#  _embedded_apply_linker_cmd_file_setting(my_app)
#
#INPUTS:
#   TARGET  - (mandatory) - The target to apply settings to.
#   Directory properties
#
#OUTPUTS
#    n/a
#
function(_embedded_apply_linker_cmd_file_setting TARGET)
	#Check if the parameter is a target.
	if(NOT TARGET ${TARGET})
		message(FATAL_ERROR "_embedded_apply_linker_cmd_file_setting(): target '${TARGET}' is not defined.")
	endif()
	#Check if target is an executable.
	get_property(_TGT_TYPE TARGET ${TARGET} PROPERTY TYPE)
	if(NOT _TGT_TYPE STREQUAL "EXECUTABLE")
		message(FATAL_ERROR "_embedded_apply_linker_cmd_file_setting(): target '${TARGET}' is not an executable.")
	endif()

	#Check if executable has a linker command file set.
	get_property(_LINKER_CMD_FILE GLOBAL PROPERTY EMBEDDED_LINKER_CMD_FILE_${TARGET} SET)
	if (NOT _LINKER_CMD_FILE)
		message(FATAL_ERROR "_embedded_apply_linker_cmd_file_setting(): Please set linker command file for target '${TARGET}' using embedded_set_target_linker_file().")
	endif()
	#Get the path to the linker command file.
	get_property(_LINKER_CMD_FILE GLOBAL PROPERTY EMBEDDED_LINKER_CMD_FILE_${TARGET})

	#Get macro defines and include paths set for the target.
	get_property(_LINKER_DEFINES GLOBAL PROPERTY EMBEDDED_LINKER_DEFINES_${TARGET})
	get_property(_LINKER_INCLUDES GLOBAL PROPERTY EMBEDDED_LINKER_INCLUDES_${TARGET})
	compiler_set_linkercmdfile(TARGET ${TARGET} PATH ${_LINKER_CMD_FILE} DEFINES ${_LINKER_DEFINES} INCLUDES ${_LINKER_INCLUDES})
endfunction()
