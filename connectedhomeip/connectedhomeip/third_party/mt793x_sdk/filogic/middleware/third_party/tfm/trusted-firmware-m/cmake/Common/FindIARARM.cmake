#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Find the location of the IAR C/C++ compiler.
#
# Find iccarm on the specified location or on the PATH and optionally validate its version.
#
#Inputs:
#   IARARM_PATH - (optional)- install path of iccarm compiler to use. If not set the
#								compiler on the PATH is used.
#   IARARM_VER  - (optional)- version number. If set the module will validate the compiler version.
#
#outputs:
#   IARARM_PATH   - will be set to the root directory of the compiler. Only set if undefined.
#   IARARM_VER    - will be set to the version number found. Only set if undefined.
#   IARARM_MODULE - set to the name of the cmake module to be included for this iccarm version.
#

#Include some dependencies
Include(Common/Utils)

#Get the version of iccarm.
#
# Execute iccarm and extract its version number for its output.
#
#Exmaples:
#  Get the version reported by iccarm at location c:/foo/bin/iccarm to variable VER
#    get_iccarm_version(ICCARM "c:/foo/bin/iccarm" RES VER)
#
#INPUTS:
#    ICCARM  - (mandatory) - iccarm executable
#    RES     - (mandatory) - variable name to put result to
#
#OUTPUTS
#    The variable named after "RES" will be set to the version number matches
#
function(get_iccarm_version)
	#Parse our arguments
	set( _OPTIONS_ARGS )			#No option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS ICCARM RES)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )		#One list argument (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_RES)
		message (FATAL_ERROR "get_iccarm_version(): Missing result parameter!")
	endif()
	set (_RES ${_MY_PARAMS_RES})

	if(NOT _MY_PARAMS_ICCARM)
		message (FATAL_ERROR "get_iccarm_version(): Missing ICCARM parameter!")
	endif()
	set (_ICCARM ${_MY_PARAMS_ICCARM})

	#Call specified executable
	execute_process(COMMAND "${_ICCARM}" --version
					OUTPUT_VARIABLE _OUTPUT
					ERROR_VARIABLE _OUTPUT
					)
	#Cut off version number. Just the numbers ignore anything after.
	STRING(REGEX REPLACE "IAR.* Compiler V(([0-9]+\.)+[0-9]+).*" "\\1" _VER "${_OUTPUT}")

	if (NOT _VER)
		message (FATAL_ERROR "get_iccarm_version(): Failed to extract version number from iccarm output.")
	endif()

    set(${_RES} ${_VER} PARENT_SCOPE)
endfunction()

#If the install location needs to be found.
if(NOT DEFINED IARARM_PATH)
	#Set IARARM_PATH to default value.
	set (IARARM_PATH "IARARM_PATH-NOTFOUND")

	#First check if iccarm is on the PATH
	#find_program puts() its output to the cmake cache. We don't want that, so we use a local variable, which
	#is unset later.
	find_program (
	  _IARARM_PATH
	  iccarm
	  PATHS env PATH
	  DOC "IARARM compiler location."
	)

	#Yes, check the version number if it is specified.
	if(_IARARM_PATH STREQUAL "_IARARM_PATH-NOTFOUND")
		message (FATAL_ERROR "iccarm install location is unset. Either put iccarm on the PATH or set IARARM_PATH.")
	endif()

	#Cut off executable name directory name to get install location.
	STRING(REGEX REPLACE "(.*)/bin/iccarm.*" "\\1" IARARM_PATH "${_IARARM_PATH}")

	#Remove unwanted junk from CMake cache.
	unset(_IARARM_PATH CACHE)
endif()

get_iccarm_version(ICCARM "${IARARM_PATH}/bin/iccarm" RES _VER)

#Check the version if needed
if(NOT DEFINED IARARM_VER)
	set(IARARM_VER ${_VER})
endif()

if(NOT "${IARARM_VER}" VERSION_EQUAL "${_VER}")
	message (FATAL_ERROR "FindIARArm.cmake: iccarm compiler version ${_VER} does not match ${IARARM_VER}.")
endif()

STRING(REGEX REPLACE "([0-9]+)\.([0-9]+)(\.[0-9]+)*.*" "CompilerIarArm\\1\\2" IARARM_MODULE "${IARARM_VER}")

message(STATUS "Version: ${IARARM_VER}/${IARARM_MODULE}")

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${IARARM_MODULE}.cmake")
	message(FATAL_ERROR "ERROR: Unsupported IARARM compiler version found on PATH.")
endif()
