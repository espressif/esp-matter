#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#FindPythonModules
#-----------
#This module checks availability of Python modules.
#
#This module has the following parameters:
#   PYTHON_EXECUTABLE - Location of python interpreter.
#   COMPONENTS        - List of python modules to look for.
#
#This module defines the following variables:
#   PY_XXX       - Cached string variable with the location of the module.
#   PY_XXX_FOUND - Set if the module is available.
#
#   Where XXX is the upper case name of the module.
#
#Examples
#   To look for m2r and report error if not found
#       find_module(PythonModules COMPONENTS m2r)
#       if (PY_M2R_FOUND)
#           do something
#       endif()
#
#   To look for m2r and do not report error if not found
#       find_module(PythonModules OPTIONAL_COMPONENTS m2r)
#       if (PY_M2R_FOUND)
#           do something
#       endif()

if(NOT DEFINED PYTHON_EXECUTABLE)
	message(FATAL_ERROR "FindPythonModules: mandatory parameter PYTHON_EXECUTABLE is missing.")
endif()

include(Common/Utils)

foreach(_mod ${PythonModules_FIND_COMPONENTS})
	string(TOUPPER ${_mod} _mod_upper)
	string(REPLACE "-" "_" _modname "${_mod}")
	if (NOT PY_${_mod_upper})
		#Execute python and try to include the module.
		execute_process(
			COMMAND ${PYTHON_EXECUTABLE} -c "import ${_modname}; print(${_modname}.__file__);"
			RESULT_VARIABLE ${_mod}_status
			OUTPUT_VARIABLE ${_mod}_path
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE)
		#If suceeded
		if(NOT ${_mod}_status)
			#Avoid trouble with directory separator on windows.
			win_fix_dir_sep(PATH ${_mod}_path)
			set("PY_${_mod_upper}" "${${_mod}_path}" CACHE STRING
				"Location of Python module ${_mod}")
		endif()
	endif()
	find_package_handle_standard_args(PY_${_mod_upper}
		FOUND_VAR PY_${_mod_upper}_FOUND
		REQUIRED_VARS PY_${_mod_upper}
		FAIL_MESSAGE "Can not find Python module ${_mod}")
endforeach()
