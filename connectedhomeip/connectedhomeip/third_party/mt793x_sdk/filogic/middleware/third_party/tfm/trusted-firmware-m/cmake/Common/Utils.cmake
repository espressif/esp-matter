#-------------------------------------------------------------------------------
# Copyright (c) 2017-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------


#Print a message and exit with failure
#
#Examples:
#   failure("Something is wrong!")

function(failure)
	message(FATAL_ERROR "${ARGN}")
endfunction()

#Append a value to a string if not already present
#
#Append an item to a string if no item with matching key is already on the string.
#This function's intended purpose is to append unique flags to command line switches.
#
#Examples:
#   string_append_unique_item(STRING C_FLAGS KEY "--target" VAL "--target=armv8m-arm-none-eabi")
#
#INPUTS:
#   STRING  - (mandatory) - name of the string to operate on
#   KEY     - (mandatory) - string to look for
#   VAL     - (mandatory) - value to put be added to the string
#
#OUTPUTS
#   STRING is modified as needed.
#
function(string_append_unique_item)
	#Parse our arguments
	set( _OPTIONS_ARGS )                    #No option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  STRING KEY VAL)   #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )                #List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_STRING)
		failure("string_append_unique_item(): Missing STRING parameter!")
	endif()
	set(_STRING ${_MY_PARAMS_STRING})

	if(NOT _MY_PARAMS_KEY)
		failure("string_append_unique_item(): Missing KEY parameter!")
	endif()
	set(_KEY ${_MY_PARAMS_KEY})

	if(NOT _MY_PARAMS_VAL)
		failure("string_append_unique_item(): Missing VAL parameter!")
	endif()
	set(_VAL ${_MY_PARAMS_VAL})

	#Scan the string.
	STRING(REGEX MATCH "( |^) *${_KEY}" _FOUND "${${_STRING}}")
	if("${_FOUND}" STREQUAL "")
		set(${_STRING} "${${_STRING}}  ${_VAL}" PARENT_SCOPE)
	endif()
endfunction()


#Convert \ directory separators to / on windows systems
#
#Convert the directory separators to forward slash on windows. Avoid
#conversion if path contains any forward slashes to avoid mixing up cygwin or
#mingw paths where an extra caharacter is escaped (i.e. "/c/Program\ Files/")
#
#Examples:
#   set(MY_PATH "C:\foo\bar")
#   win_fix_dir_sep(PATH MY_PATH)
#
#INPUTS:
#   PATH  - (mandatory) - name of the string variable to operate on
#
#OUTPUTS
#   PATH is modified as needed.
#
function(win_fix_dir_sep)
	#Parse our arguments
	set( _OPTIONS_ARGS )            #No option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS  PATH )    #Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )        #List arguments (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_PATH)
		failure("win_fix_dir_sep(): Missing mandatory parameter PATH!")
	endif()
	set(_PATH ${_MY_PARAMS_PATH})

	#To avoid trouble on windows change directory separator.
	if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
		#Do not convert directory separator if there are forward slashes
		#present. This is to avoid mixing up escaped characters in cygwin
		#or mingw paths (i.e. c:/Program\ Files/something)
		string(FIND "${${_PATH}}" "/" _is_found)
		if (_is_found LESS 0)
			string(REPLACE "\\" "/" ${_PATH} "${${_PATH}}")
			set(${_PATH} "${${_PATH}}" PARENT_SCOPE)
		endif()
	endif()
endfunction()
