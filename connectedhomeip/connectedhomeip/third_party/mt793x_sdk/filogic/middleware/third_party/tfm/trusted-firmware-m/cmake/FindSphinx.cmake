#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#FindSphinx
#-----------
#Sphinx is a document generation tool written in Python.
#See http://www.sphinx-doc.org/en/master/
#
#This module checks availability of the Sphinx document generator
#(sphinx-build) and it's dependences (Python).
#Sphinx is distributed as pip package or on Linux as a distribution specific
#package (i.e. python-sphinx for Ubuntu). Independent of the distribution
#method this module expects sphix-build to be either available on the PATH,
#or to be located in a host OS specific standard location.
#
#This modules has the following parameters:
#   SPHINX_PATH   = variable specifying where sphinx-build can be found.
#                         If it is not defined the environment variable with
#                         the same name is used. If that is also undefined,
#                         then OS specific standard locations will be
#                         searched.
#
# This modules defines the following variables:
#   SPHINX_VERSION   = The version reported by "sphinx-build --version"
#   SPHINX_FOUND     = True is sphinx-build was found and executed fine
#

Include(CMakeParseArguments)

#Sphinx needs Python.
find_package(PythonInterp 3)
if (NOT PYTHONINTERP_FOUND)
	message(STATUS "Can not find Python3.x interpreter. Pyhton3 must be installed and available on the PATH.")
	message(STATUS "Sphinx documentation targets will not be created.")
	return()
endif()

if (NOT DEFINED SPHINX_PATH)
	if (DEFINED $ENV{SPHINX_PATH})
	set(SPHINX_PATH $ENV{SPHINX_PATH})
	endif()
endif()


if (DEFINED SPHINX_PATH)
	#Find the Sphinx executable. Search only at SPHINX_PATH.
	find_program(SPHINX_EXECUTABLE
		NAMES sphinx-build
		DOC "Sphinx Documentation Builder (sphinx-doc.org)"
		PATH ${SPHINX_PATH}
		NO_DEFAULT_PATH
		NO_CMAKE_ENVIRONMENT_PATH
		NO_CMAKE_PATH
		NO_SYSTEM_ENVIRONMENT_PATH
		NO_CMAKE_SYSTEM_PATH
		NO_CMAKE_FIND_ROOT_PATH
	)
	if (SPHINX_EXECUTABLE-NOTFOUND)
		message(STATUS "Failed to find sphinx-build at ${SPHINX_PATH}.")
		message(STATUS "Sphinx documentation targets will not be created.")
		return()
	endif()
else()
	#Find the Sphinx executable. Search OS specific default locations.
	find_program(SPHINX_EXECUTABLE
	  NAMES sphinx-build
	  DOC "Sphinx Documentation Builder (sphinx-doc.org)"
	)

	if (SPHINX_EXECUTABLE-NOTFOUND)
		message(STATUS "Failed to find sphinx-build at OS specific default locations.")
		message(STATUS "Sphinx documentation targets will not be created.")
		return()
	endif()
endif()

#Get Sphinx version
execute_process(COMMAND "${SPHINX_EXECUTABLE}" "--version" OUTPUT_VARIABLE _SPHINX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
#Parse output
if(_SPHINX_VERSION)
	if(_SPHINX_VERSION MATCHES ".*sphinx-build[^0-9.]*([0-9.]+).*")
		string(REGEX REPLACE ".*sphinx-build ([0-9.]+).*" "\\1" SPHINX_VERSION "${_SPHINX_VERSION}")
	endif()
endif()

#Set "standard" find module return values
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx REQUIRED_VARS SPHINX_EXECUTABLE SPHINX_VERSION VERSION_VAR SPHINX_VERSION)
