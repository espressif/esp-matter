#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#A script to fill out the "toctree" file lists in index.rst.in to put design
#documents into the correct section based on their state.
#
#First rst files are parsed under a defined directory, to identify document
#status based on value of the ":Status:" field. Each state will have a list
#being generated to which matching documents are added. These lists are used
#then to fill template values using configure_file()
#
#The ":Status:" field is optional according to design proposal process.
#Apparently, only the documents in "Accepted" status will be merged and included
#in Sphinx build. Therefore, a design document without ":Status:" field will be
#put in "Accepted" category by default.
#If there are design document drafts in local environment, it is assumed that
#developers are aware of the status of these drafts and won't be confused.
#A message will be still thrown out when a design document doesn't contain
#":Status:" field. It can be removed if more and more design documents don't
#maintain that field.
#
#Functions are used whenever possible to avoid global variable name space
#pollution.
#

Include(CMakeParseArguments)

#Set the status value here to avoid any typo or upper/lower case trouble.
set(ACCEPTED_STATUS "ACCEPTED")
set(DRAFT_STATUS "DRAFT")
set(DETAILED_STATUS "DETAILED")
#"NO_SET" is used to mark documents without status field.
set(NO_SET "NO_SET")
#"INVALID_STATUS" is used to mark a document which specifies the status but the
#status value is invalid or unrecognized.
set(INVALID_STATUS "INVALID_STATUS")

#This function will search for .rst files in a given directory, read them and
#check if the ":Status:" field is defined in them. Then will add each file to a
#list with a name matching the status value.
#See the definition of _STATE_VALUES below for a list of valid state values.
#Files without state value will be placed on the both "Accepted" list and
#"NO_SET" list.
#State value comparison is case insensitive.
#
#The output lists will have the prefix specified in the PREFIX parameter.
#
#Inputs:
#    DIR    Directory to scan for rst files.
#    PREFIX The prefix of output list variables.
#
#Outputs:
#   <prefix>_<state>        - list; all files with a valid state value.
#   <prefix>_NO_SET         - list; all files without status field.
#   <prefix>_INVALID_STATUS - list; all files with invalid status field.
#
#Examples
#   sphinx_categorize_rst(DIR ${TFM_ROOT_DIR}/docs/design_documents
#                          PREFIX "DESIGN_DOCS")
#
function(sphinx_categorize_rst)
	#Valid state values.
	set(_STATE_VALUES ${DRAFT_STATUS} ${ACCEPTED_STATUS} ${DETAILED_STATUS}
					  ${NO_SET} ${INVALID_STATUS})

	#No option (on/off) arguments
	set( _OPTIONS_ARGS )
	#Single option arguments (e.g. PROJ_NAME "bubu_project")
	set( _ONE_VALUE_ARGS DIR PREFIX)
	#List arguments (e.g. LANGUAGES C ASM CXX)
	set( _MULTI_VALUE_ARGS )

	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}"
							"${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check parameters
	foreach(_PARAM IN LISTS _ONE_VALUE_ARGS)
		if (NOT DEFINED _MY_PARAMS_${_PARAM})
			message(FATAL_ERROR "Parameter ${_PARAM} is missing!")
		endif()
	endforeach()

	#Search for .rst files
	file(GLOB_RECURSE _RST_FILES
		LIST_DIRECTORIES False
		${_MY_PARAMS_DIR}/**.rst)

	#Loop over .rst files
	foreach(_FILE IN LISTS _RST_FILES)
		#Read the file and return the first status line.
		file(STRINGS ${_FILE} _CONTENT REGEX ":Status:" LIMIT_COUNT 1)

		#Nothing read -> the field is missing
		if (_CONTENT STREQUAL "")
			#If a document doesn't maintain a status field, put it in
			#Accepted list by default.
			list(APPEND _STATUS_${ACCEPTED_STATUS} ${_FILE})
			#Also add the file to the "NO_SET" list. Thus it can be
			#highlighted later.
			list(APPEND _STATUS_${NO_SET} ${_FILE})
		else()
			#convert to upper case for case insensitive matching.
			string(TOUPPER ${_CONTENT} _CONTENT)

			#loop over status values
			foreach(_STATUS IN LISTS _STATE_VALUES)
				#convert to upper case for case insensitive matching.
				string(TOUPPER ${_STATUS} _STATUS)
				#does status match the value from the file?
				if (_CONTENT MATCHES ":STATUS:[ ]*${_STATUS}")
					#add it to the list
					list(APPEND _STATUS_${_STATUS} ${_FILE})
					#and exit the loop
					break()
				endif()

				#If the status value is invalid.
				if (_STATUS STREQUAL ${INVALID_STATUS})
					list(APPEND _STATUS_${INVALID_STATUS} ${_FILE})
				endif()
			endforeach()
		endif()
	endforeach()

	#return the lists to the caller
	foreach(_STATUS IN LISTS _STATE_VALUES)
		string(TOUPPER ${_STATUS} _STATUS)

		if (DEFINED _STATUS_${_STATUS})
			set(${_MY_PARAMS_PREFIX}_${_STATUS} ${_STATUS_${_STATUS}}
					PARENT_SCOPE)
		endif()
	endforeach()
endfunction()

#Configure (fill in) the Sphinx index.rst.in template file.
#Call sphinx_categorize_rst() to get the .rst files sorted to lists, and then
#create the list of files for each design document section (convert the CMake
#lists to properly formatted text).
#Finally call configure_file() to fill in the values.
#
#Inputs:
#    SRC      Full path to template index file
#    DST      Full patch to configured output file.
#    DOC_DIR  Path to design documents directory.
#    DOC_ROOT Path to root directory of documentation
#
#Outputs:
#   Configured <DST> file.
#
#Examples
#   sphinx_categorize_rst(DIR ${TFM_ROOT_DIR}/docs/design_documents
#                          PREFIX "DESIGN_DOCS")
#
function(sphinx_configure_index)
	set(_STATE_VALUES ${DRAFT_STATUS} ${ACCEPTED_STATUS} ${DETAILED_STATUS}
					  ${DEFAULT_STATUS} ${INVALID_STATUS})

	#No option (on/off) arguments
	set( _OPTIONS_ARGS )
	 #Single option arguments (e.g. PROJ_NAME "bubu_project")
	set( _ONE_VALUE_ARGS SRC DST DOC_ROOT DOC_DIR)
	#List arguments (e.g. LANGUAGES C ASM CXX)
	set( _MULTI_VALUE_ARGS )

	cmake_parse_arguments(_MY_PARAMS "${_OPTIONS_ARGS}" "${_ONE_VALUE_ARGS}"
							"${_MULTI_VALUE_ARGS}" ${ARGN} )

	#Check parameters
	foreach(_PARAM IN LISTS _ONE_VALUE_ARGS)
		if (NOT DEFINED _MY_PARAMS_${_PARAM})
			message(FATAL_ERROR "Parameter ${_PARAM} is missing!")
		endif()
	endforeach()

	#Assign design documents to lists based on their status
	sphinx_categorize_rst(DIR ${_MY_PARAMS_DOC_DIR} PREFIX "_DD")

	#Highlight documents without status field
	if (DEFINED _DD_${NO_SET})
		string(REPLACE ";" "\n \t" _DD_${NO_SET} "${_DD_${NO_SET}}")
		message(" The following documents are put into Accepted category without status field:\n \t${_DD_${NO_SET}}")
	endif()

	#Look for invalid documents
	if (DEFINED _DD_${INVALID_STATUS})
		string(REPLACE ";" "\n \t" _DD_${INVALID_STATUS} "${_DD_${INVALID_STATUS}}")
		message(WARNING " The following documents provide invalid status information:\n \t${_DD_${INVALID_STATUS}}")
	endif()

	#The document root must be an absolute path
	get_filename_component(_MY_PARAMS_DOC_ROOT "${_MY_PARAMS_DOC_ROOT}"
							ABSOLUTE)

	#Loop over status lists
	foreach(_STATUS IN ITEMS ${DRAFT_STATUS} ${DETAILED_STATUS} ${ACCEPTED_STATUS})
		#Create an empty file list for this status
		set(${_STATUS}_DD_LIST "")
		#If the source list is empty go to next iteration
		if (NOT DEFINED _DD_${_STATUS})
			continue()
		endif()

		#Loop over files on the list of this status
		foreach(_FILE IN LISTS _DD_${_STATUS})

			# Strip path from the filesince index is placed in same location
			get_filename_component(_FILE ${_FILE} NAME)
			#Detailed and Draft files go to the same section
			if (_STATUS STREQUAL ${DETAILED_STATUS})
				set(_STATUS ${DRAFT_STATUS})
			endif()

			#Append the file to the output string
			string(APPEND ${_STATUS}_DD_LIST "\n    ${_FILE}")
		endforeach()
	endforeach()

	#Call configure file to fill out the template.
	configure_file(${_MY_PARAMS_SRC} ${_MY_PARAMS_DST} @ONLY)
endfunction()

#If being run in script mode (cmake -P) and not included.
if (DEFINED CMAKE_SCRIPT_MODE_FILE
			 AND (CMAKE_CURRENT_LIST_FILE STREQUAL CMAKE_SCRIPT_MODE_FILE))
	#Check input variables.
	foreach(_PARAM IN ITEMS SPHINX_TEMPLATE_INDEX_FILE SPHINX_CONFIGURED_INDEX_FILE
							SPHINX_DESIGN_DOC_ROOT TFM_ROOT_DIR)
		if (NOT DEFINED ${_PARAM})
			message(FATAL_ERROR "Parameter ${_PARAM} is not set!")
		endif()
	endforeach()

	sphinx_configure_index(SRC ${SPHINX_TEMPLATE_INDEX_FILE}
					DST ${SPHINX_CONFIGURED_INDEX_FILE}
					DOC_DIR ${SPHINX_DESIGN_DOC_ROOT}
					DOC_ROOT ${TFM_ROOT_DIR})
endif()
