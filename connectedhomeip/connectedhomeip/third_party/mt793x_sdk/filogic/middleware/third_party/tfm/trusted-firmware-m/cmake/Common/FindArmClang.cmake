#-------------------------------------------------------------------------------
# Copyright (c) 2017, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Find the location of the ARMClang C/C++ compiler.
#
# Find armclang on the specified location or on the PATH and optionally validate its version.
#
#Inputs:
#   ARMCLANG_PATH - (optional)- install path of armclang compiler to use. If not set the
#								compiler on the PATH is used.
#   ARMCLANG_VER  - (optional)- version number. If set the module will validate the compiler version.
#
#outputs:
#   ARMCLANG_PATH   - will be set to the root directory of the compiler. Only set if undefined.
#   ARMCLANG_VER    - will be set to the version number found. Only set if undefined.
#   ARMCLANG_MODULE - set to the name of the cmake module to be included for this ARMClang version.
#

#Include some dependencies
Include(Common/Utils)

#Get the version of armasm.
#
# Execute armasm and extract its version number for its output.
#
#Exmaples:
#  Get the version reported by armasm at location c:/foo/bin/armasm to variable VER
#    get_armasm_version(ARMASM "c:/foo/bin/armasm" RES VER)
#
#INPUTS:
#    ARMASM  - (mandatory) - armasm executable
#    RES     - (mandatory) - variable name to put result to
#
#OUTPUTS
#    The variable named after "RES" will be set to the version number matches
#
function(get_armasm_version)
	#Parse our arguments
	set( _OPTIONS_ARGS )			#No option (on/off) arguments (e.g. IGNORE_CASE)
	set( _ONE_VALUE_ARGS ARMASM RES)	#Single option arguments (e.g. PATH "./foo/bar")
	set( _MULTI_VALUE_ARGS )		#One list argument (e.g. LANGUAGES C ASM CXX)
	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}" "${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check mandatory parameters
	if(NOT _MY_PARAMS_RES)
		message (FATAL_ERROR "get_armasm_version(): Missing result parameter!")
	endif()
	set (_RES ${_MY_PARAMS_RES})

	if(NOT _MY_PARAMS_ARMASM)
		message (FATAL_ERROR "get_armasm_version(): Missing ARMASM parameter!")
	endif()
	set (_ARMASM ${_MY_PARAMS_ARMASM})

	#Call specified executable
	execute_process(COMMAND "${_ARMASM}"
					OUTPUT_VARIABLE _OUTPUT
					ERROR_VARIABLE _OUTPUT
					)
	#Cut off version number. Just the numbers ignore anything after.
	STRING(REGEX REPLACE ".*ARM Compiler (([0-9]+\.)+[0-9]+).*" "\\1" _VER "${_OUTPUT}")

	if (NOT _VER)
		message (FATAL_ERROR "get_armasm_version(): Failed to extract version number from armasm output.")
	endif()

    set(${_RES} ${_VER} PARENT_SCOPE)
endfunction()

#If the install location needs to be found.
if(NOT DEFINED ARMCLANG_PATH)
	#Set ARMCLANG_PATH to default value.
	set (ARMCLANG_PATH "ARMCLANG_PATH-NOTFOUND")

	#First check if armclang is on the PATH
	#find_program puts() its output to the cmake cache. We don't want that, so we use a local variable, which
	#is unset later.
	find_program (
	  _ARMCLANG_PATH
	  armclang
	  PATHS env PATH
	  DOC "ARMCLANG compiler location."
	)

	#Yes, check the version number if it is specified.
	if(_ARMCLANG_PATH STREQUAL "_ARMCLANG_PATH-NOTFOUND")
		message (FATAL_ERROR "armclang install location is unset. Either put armclang on the PATH or set ARMCLANG_PATH.")
	endif()

	#Cut off executable name directory name to get install location.
	STRING(REGEX REPLACE "(.*)/bin/armclang.*" "\\1" ARMCLANG_PATH "${_ARMCLANG_PATH}")

	#Remove unwanted junk from CMake cache.
	unset(_ARMCLANG_PATH CACHE)
endif()

get_armasm_version(ARMASM "${ARMCLANG_PATH}/bin/armasm" RES _VER)

#Check the version if needed
if(NOT DEFINED ARMCLANG_VER)
	set(ARMCLANG_VER ${_VER})
endif()

if(NOT "${ARMCLANG_VER}" VERSION_EQUAL "${_VER}")
	message (FATAL_ERROR "FindArmClang.cmake: armclang compiler version ${_VER} does not match ${ARMCLANG_VER}.")
endif()

STRING(REGEX REPLACE "([0-9]+)\.([0-9]+)(\.[0-9]+)*.*" "CompilerArmClang\\1\\2" ARMCLANG_MODULE "${ARMCLANG_VER}")

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${ARMCLANG_MODULE}.cmake")
	message(FATAL_ERROR "ERROR: Unsupported ARMCLANG compiler version found on PATH.")
endif()
