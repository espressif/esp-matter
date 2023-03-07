#-------------------------------------------------------------------------------
# Copyright (c) 2018, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#A cmake script to merge two archives using GNU AR.
#
# The script will first run AR to get the list of files in the source archive.
# Then each file is:
#   -extracted
#   -added to the target archive
#   -deleted
#
# The loop is needed to avoid losing files with matching name in the source
# archive.
# The destination archive is updated in a way not to overwrite existing files
# with matching names.
#
#Examples:
#  cmake -DCMAKE_AR=arm-none-eabi-ar -DDESTINATION=new_lib.a -DSOURCE=/foo/bar/old_lib.a -P ./GNUArMerge.cmake
#
#Parameters:
#  SOURCE      - archive file to copy all members from
#  DESTINATION - archive file to copy members to. If file exists, then new
#                members are added without overwriting existing ones.
#  CMAKE_AR    - GNU AR executable
#

#Execute AR and capture its output
#
# Script execution will stop with a fatal error if AR execution fails.
#
#Examples:
#  List content of archive:
#    run_ar(RESULT t /foo/bar/my_lib.a)
#  Add object file to archive
#    run_ar(RESULT q /foo/bar/my_lib.a new_obj.o)
#
#INPUTS:
#    RESULT - (mandatory) - name of variable to put result in
#    All remaining parameters will be command line options to AR
#
#OUTPUTS
#    RESULT - text output of AR command
#
function(run_ar OUTPUT )
	execute_process(COMMAND ${CMAKE_AR} ${ARGN}
					TIMEOUT 120
					OUTPUT_VARIABLE _RES
					RESULT_VARIABLE _STATUS_CODE
					OUTPUT_STRIP_TRAILING_WHITESPACE)

	if (STATUS_CODE GREATER 0)
		message(FATAL_ERROR "ERROR: Failed to execute \"${CMAKE_AR} ${ARGN}\".")
	endif()
	set(${OUTPUT} ${_RES} PARENT_SCOPE)
endfunction()

#Delete a file
#
# Function to delete a file. No error will be reported if file is missing.
# Script execution will stop with a fatal error if AR execution fails.
#
#Examples:
#  rm(/foo/bar/my_lib.a)
#
#INPUTS:
#    FILE - path to file to delete
#
#OUTPUTS
#    N/A
#
function(rm FILE)
	execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${FILE}
					RESULT_VARIABLE _STATUS_CODE
					TIMEOUT 120)
	if (STATUS_CODE GREATER 0)
		message(FATAL_ERROR "ERROR: Failed to execute \"${CMAKE_COMMAND} -E remove ${FILE}\".")
	endif()
endfunction()


#############################################################################
# Entry point
#############################################################################
#Verify input variables.

if(NOT DEFINED SOURCE)
	message(FATAL_ERROR "GNUArMerge.cmake: Variable SOURCE is not defined.")
endif()

if(NOT DEFINED DESTINATION)
	message(FATAL_ERROR "GNUArMerge.cmake: Variable DESTINATION is not defined.")
endif()

if(NOT DEFINED CMAKE_AR)
	message(FATAL_ERROR "GNUArMerge.cmake: Variable CMAKE_AR is not defined.")
endif()


#Get list of archive members
run_ar("OBJ_LIST" t ${SOURCE})

#Convert AR output to cmake list
string(REPLACE "\n" ";" OBJ_LIST ${OBJ_LIST})

#Iterate over member list.
foreach(OBJ ${OBJ_LIST})
	#Extract member
	run_ar("_DUMMY" x ${SOURCE} ${OBJ})
	#Add member to destination archive
	run_ar("_DUMMY" q ${DESTINATION} ${OBJ})
	#Remove extracted member
	rm("${OBJ}")
endforeach()

#Update the symbol table
run_ar("_DUMMY" s ${DESTINATION})
