#-------------------------------------------------------------------------------
# Copyright (c) 2018-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#This file adds build and install targets to build the Doxygen documentation
#for TF-M. This currently means the "Reference Manual". Further documentation
#builds may be added in the future.

Include(CMakeParseArguments)

#This function will find the location of tools needed to build the
#documentation. These are:
#    - Mandatory:
#        - Doxygen v1.8.0 or higher
#        - dot
#        - PlantUML and it's dependencies
#   - Optional
#        - LateX/PDFLateX
#
#Inputs:
#    none (some global variables might be used by FindXXX modules used. For
#         details please check the used modules.)
#
#Outputs:
#   NODOC                  - will be defined and set to true is any mandatory
#                            tool is missing.
#   LATEX_PDFLATEX_FOUND   - defined if PDFLateX is available.
#   DOXYGEN_EXECUTABLE     - path to doxygen
#   DOXYGEN_DOT_EXECUTABLE - path to dot
#   PLANTUML_JAR_PATH      - path to PlantUML

#Examples
#   DoxyFindTools()
#
function(DoxyFindTools)
	#Find doxygen and dot.
	find_package(Doxygen 1.8.0)
	#find_package(Doxygen) will not print an error if dot is not available,
	#emit a message here to help users.
	if (NOT DOXYGEN_DOT_FOUND)
		message(STATUS "Could NOT find Graphviz (missing:  DOXYGEN_DOT_EXECUTABLE).")
	endif()

	#Find plantUML
	find_package(PlantUML)

	#Find tools needed for PDF generation.
	find_package(LATEX COMPONENTS PDFLATEX)
	set (LATEX_PDFLATEX_FOUND ${LATEX_PDFLATEX_FOUND} PARENT_SCOPE)

	if (DOXYGEN_FOUND AND DOXYGEN_DOT_FOUND AND PLANTUML_FOUND)
		#Export executable locations to global scope.
		set(DOXYGEN_EXECUTABLE "${DOXYGEN_EXECUTABLE}" PARENT_SCOPE)
		set(DOXYGEN_DOT_EXECUTABLE "${DOXYGEN_DOT_EXECUTABLE}" PARENT_SCOPE)
		set(PLANTUML_JAR_PATH "${PLANTUML_JAR_PATH}" PARENT_SCOPE)
		set(NODOC False PARENT_SCOPE)
	else()
		message(WARNING "Some tools are missing for document generation. Document generation target is not created.")
		set(NODOC True PARENT_SCOPE)
	endif()
endfunction()

#Find the needed tools.
DoxyFindTools()

#If mandatory tools are missing, skip creating document generation targets.
#This means missing documentation tools is not a crytical error, and building
#TF-M is still possible.
if (NOT NODOC)
	#The doxygen configuration file needs some project specific configuration.
	#Variables with DOXYCFG_ prefix are settings related to that.
	set(DOXYCFG_TEMPLATE_FILE ${TFM_ROOT_DIR}/doxygen/Doxyfile.in)
	set(DOXYCFG_CONFIGURED_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	#This is where doxygen output will be written
	set(DOXYCFG_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/doc")

	#Eclipse help ID. The help files shall be installed under the plugin
	#directory into a directory with a name matching this ID.
	set(DOXYCFG_ECLIPSE_DOCID "org.arm.tf-m-refman")

	#Version ID of TF-M.
	#TODO: this shall not be hard-coded here. A process need to defined for
	#      versioning the document (and TF-M).
	set(DOXYCFG_TFM_VERSION "v1.0")

	#Using add_custom_command allows CMake to generate proper clean commands
	#for document generation.
	add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doc"
			COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYCFG_CONFIGURED_FILE}"
			WORKING_DIRECTORY "${TFM_ROOT_DIR}"
			COMMENT "Running doxygen.")

	#Create build target to generate HTML documentation.
	add_custom_target(doc_refman
		WORKING_DIRECTORY ${TFM_ROOT_DIR}
		COMMENT "Generating TF-M Reference Manual..."
		DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/doc"
		VERBATIM)

	#Add the HTML documentation to install content
	install(DIRECTORY ${DOXYCFG_OUTPUT_PATH}/html DESTINATION doc/reference_manual
		COMPONENT refman EXCLUDE_FROM_ALL)

	#Add the HTML documentation to install content. This time to be copied to
	#eclipse plugin directory.
	#TODO: I could not find a working description how the doxygen output
	#can be installed to eclipse. Re-enable eclipse help generation after
	#this is investigated. (See Doxyfile.in::GENERATE_ECLIPSEHELP)

	#install(DIRECTORY "${DOXYCFG_OUTPUT_PATH}/html/"
	#   DESTINATION "doc/reference_manual/${DOXYCFG_ECLIPSE_DOCID}"
	#   COMPONENT refman
	#   EXCLUDE_FROM_ALL)

	#If PDF documentation is being made.
	if (LATEX_PDFLATEX_FOUND)
		if (NOT CMAKE_GENERATOR MATCHES "Makefiles")
			message(WARNING "Generator is not make based. PDF document generation target is not created.")
		else()
			#This file shall not be included before cmake did finish finding the make tool and thus
			#setting CMAKE_MAKE_PROGRAM. Currently the search is triggered by the project() command.
			if(NOT CMAKE_MAKE_PROGRAM)
				message(FATAL_ERROR "CMAKE_MAKE_PROGRAM is not set. This file must be included after the project command is run.")
			endif()

			#The add_custom_command is not used here to get proper clean
			#command since the clean rules "added" above will remove the entire
			#doc directory, and thus clean the PDF output too.
			add_custom_target(doc_refman_pdf
				COMMAND "${CMAKE_MAKE_PROGRAM}" pdf
				WORKING_DIRECTORY ${DOXYCFG_OUTPUT_PATH}/latex
				COMMENT "Generating PDF version of TF-M reference manual..."
				DEPENDS doc_refman
				VERBATIM)

			#Add the pdf documentation to install content
			install(FILES "${DOXYCFG_OUTPUT_PATH}/latex/refman.pdf" DESTINATION "doc/reference_manual"
				RENAME "tf-m_reference_manual.pdf"
				COMPONENT refman EXCLUDE_FROM_ALL)

			set(DOXYCFG_TFM_GENERATE_PDF $(DOXYCFG_TFM_VERSION))
		endif()
	else()
		message(WARNING "PDF generation tools are missing. PDF document generation target is not created.")
	endif()

	#Generate build target which installs the documentation.
	if (TARGET doc_refman_pdf)
		add_custom_target(install_doc
			DEPENDS doc_refman doc_refman_pdf
			COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=refman
			-P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
	else()
		add_custom_target(install_doc
			DEPENDS doc_refman
			COMMAND "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=refman
			-P "${CMAKE_BINARY_DIR}/cmake_install.cmake")
	endif()

	#Now instantiate a doxygen configuration file from the template.
	message(STATUS "Writing doxygen configuration...")
	configure_file(${DOXYCFG_TEMPLATE_FILE} ${DOXYCFG_CONFIGURED_FILE} @ONLY)
endif()
