#-------------------------------------------------------------------------------
# Copyright (c) 2017-2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Find the location of the GNUARM C/C++ compiler.
#
# Find gnuarm on the specified location or on the PATH and optionally validate
# its version.
#
#Inputs:
#  GNUARM_PATH - (optional)- install path of GNUARM compiler to use. If not set
#                the compiler on the PATH is used.
#  GNUARM_VER  - (optional)- version number. If set the module will validate
#                the compiler version.
#
#outputs:
#  GNUARM_PATH   - will be set to the root directory of the compiler. Only set
#                  if undefined.
#  GNUARM_VER    - will be set to the version number found. Only set if
#                  undefined.
#  GNUARM_MODULE - set to the name of the cmake module to be included for this
#                  GNUARM version.
#

#Include some dependencies
Include(Common/Utils)

set(_GCC_BINARY_NAME "arm-none-eabi-gcc")

#Get the version of armgcc.
#
# Execute gcc and extract the version number for its output.
#
#Examples:
#  Get the version reported by gcc at location c:/foo/bin/ to
#  variable VER get_armgcc_version(GCC "c:/foo/bin/arm-none-eabi-gcc" RES VER)
#
#INPUTS:
#  GCC  - (mandatory) - gcc executable
#  RES  - (mandatory) - variable name to put result to
#
#OUTPUTS
#    The variable named after "RES" will be set to the version number
#
function(get_armgcc_version)
	###Parse our arguments
	#No option (on/off) arguments (e.g. IGNORE_CASE)
	set( _OPTIONS_ARGS )
	#Single option arguments (e.g. PATH "./foo/bar")
	set( _ONE_VALUE_ARGS GCC RES)
	#List arguments (e.g. LANGUAGES C ASM CXX)
	set( _MULTI_VALUE_ARGS )
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}"
									"${_ONE_VALUE_ARGS}"
									 "${_MULTI_VALUE_ARGS}"
									  ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_RES)
		message (FATAL_ERROR "get_armgcc_version(): Missing result parameter!")
	endif()
	set (_RES ${_MY_PARAMS_RES})

	if(NOT _MY_PARAMS_GCC)
		message (FATAL_ERROR "get_armgcc_version(): Missing GCC parameter!")
	endif()
	set (_GCC ${_MY_PARAMS_GCC})

	#Call specified executable
	execute_process(COMMAND "${_GCC}" -v
					OUTPUT_VARIABLE _OUTPUT
					ERROR_VARIABLE _OUTPUT
					)
	#Cut off version number. Just the numbers ignore anything after.
	STRING(REGEX REPLACE  ".*gcc version ([0-9.]+) .*" "\\1" _VER "${_OUTPUT}")

	if (NOT _VER)
		string(CONCAT _msg "get_armgcc_version(): Failed to extract version"
							" number from ${_GCC_BINARY_NAME} output.")
		message (FATAL_ERROR "${_msg}")
	endif()

	set(${_RES} ${_VER} PARENT_SCOPE)
endfunction()

#Check if the executable is present
if(NOT DEFINED GNUARM_PATH)
	#If the location is not set, try to find executable on PATH.
	#Set GNUARM_PATH to default value.
	set (GNUARM_PATH "GNUARM_PATH-NOTFOUND")

	#First check if gcc is on the PATH
	#find_program puts() its output to the cmake cache. We don't want that, so
	# we use a local variable, which is unset later.
	find_program (
	  _GNUARM_PATH
	  ${_GCC_BINARY_NAME}
	  PATHS env PATH
	  DOC "GNUARM compiler location."
	)
else()
	#Check executable is available on the specified path.
	#find_program puts() its output to the cmake cache. We don't want that, so
	# we use a local variable, which is unset later.
	find_program (
	  _GNUARM_PATH
	  ${_GCC_BINARY_NAME}
	  PATHS ${GNUARM_PATH}/bin NO_DEFAULT_PATH
	  DOC "GNUARM compiler location."
	)
endif()

#Is executable present?
if(_GNUARM_PATH STREQUAL "_GNUARM_PATH-NOTFOUND")
	string(CONCAT _msg "${_GCC_BINARY_NAME} can not be found. Either put"
						" ${_GCC_BINARY_NAME} on the PATH or set GNUARM_PATH"
						" properly.")
	message (FATAL_ERROR ${_msg})
endif()

#Cut off executable and directory name to get install location.
STRING(REGEX REPLACE "(.*)/bin/${_GCC_BINARY_NAME}.*"
						"\\1" GNUARM_PATH "${_GNUARM_PATH}")

#Remove unwanted junk from CMake cache.
unset(_GNUARM_PATH CACHE)

get_armgcc_version(GCC "${GNUARM_PATH}/bin/${_GCC_BINARY_NAME}" RES _VER)

#Check the version if needed
if(NOT DEFINED GNUARM_VER)
	set(GNUARM_VER ${_VER})
endif()

if(NOT "${GNUARM_VER}" VERSION_EQUAL "${_VER}")
	string(CONCAT _msg "FindGNUARM.cmake: ${_GCC_BINARY_NAME} compiler version"
						" ${_VER} does not match ${GNUARM_VER}.")
	message (FATAL_ERROR "${_msg}")
endif()


STRING(REGEX REPLACE "([0-9]+)\.([0-9]+)(\.[0-9]+)*.*" "CompilerGNUARM\\1\\2"
						 GNUARM_MODULE "${GNUARM_VER}")

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${GNUARM_MODULE}.cmake")
	string(CONCAT _msg "ERROR: Unsupported GNUARM compiler version found on"
						" PATH.")
	message(FATAL_ERROR "${_msg}")
endif()
