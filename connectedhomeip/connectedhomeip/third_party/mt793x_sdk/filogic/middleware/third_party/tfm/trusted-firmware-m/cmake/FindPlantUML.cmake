#-------------------------------------------------------------------------------
# Copyright (c) 2018-2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#FindPlantuml
#-----------
#PlantUML is a diagram generation tool. It can generate various UML and non-UML
#diagrams. See: http://plantuml.com/
#
#This module checks PlantUML availability and checks if the Java runtime is
#available.
#For Windows PlantUML is distributed as a jar archive and thus there is no
#standard install location where it could be searched for.
#Most Linux distributions come with a proper PlantUML package which installs
#a shell script to easy starting PlantUML, but the location of the .jar file
#is hidden.
#Thus there is no standard location to search for the .jar file and this module
#depends on user input.
#
#This module has the following parameters:
#   PLANTUML_JAR_PATH   = variable specifying where the PlantUML java archive
#                         (plantuml.jar) can be found. If it is not defined,
#                         the environment variable with the same name is used.
#                         If both is missing, that is an error.
#
#This module defines the following variables:
#   PLANTUML_VERSION        = The version reported by "plantuml.jar -version"
#   PLANTUML_FOUND          = Was the .jar file found and sucesfuly executed.
#

include(Common/Utils)

find_package(Java 1.8 COMPONENTS Runtime)
if(Java_Runtime_FOUND)
	#Check if the jar file is at the user defined location.
	#Prefer the cmake variable to the environment setting.
	if (NOT DEFINED PLANTUML_JAR_PATH)
		if (DEFINED ENV{PLANTUML_JAR_PATH})
			set(PLANTUML_JAR_PATH "$ENV{PLANTUML_JAR_PATH}" CACHE STRING "PLANTUML location." )
		endif()
	endif()

	if (NOT DEFINED PLANTUML_JAR_PATH)
		message(STATUS "PLANTUML_JAR_PATH variable is missing, PlantUML jar location is unknown.")
	else()
		win_fix_dir_sep(PATH PLANTUML_JAR_PATH)
		#Get plantuml version
		execute_process(COMMAND "${Java_JAVA_EXECUTABLE}" "-jar" "${PLANTUML_JAR_PATH}" "-version" OUTPUT_VARIABLE _PLANTUML_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
		#Parse plantuml output
		if(_PLANTUML_VERSION)
			if(_PLANTUML_VERSION MATCHES ".*PlantUML version ([0-9.]+).*")
				string(REGEX REPLACE ".*PlantUML version ([0-9.]+).*" "\\1" PLANTUML_VERSION "${_PLANTUML_VERSION}")
			endif()
		endif()
	endif()
endif()

#Set "standard" find module return values
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Plantuml REQUIRED_VARS PLANTUML_JAR_PATH PLANTUML_VERSION VERSION_VAR PLANTUML_VERSION)
