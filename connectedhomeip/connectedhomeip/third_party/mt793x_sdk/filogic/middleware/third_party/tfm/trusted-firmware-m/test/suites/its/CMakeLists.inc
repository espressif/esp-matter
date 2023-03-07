#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Definitions to compile the "internal trusted storage test" module.
#This file assumes it will be included from a project specific cmakefile, and
#will not create a library or executable.
#Inputs:
#   TFM_ROOT_DIR - root directory of the TF-M repo.
#
#Outputs:
#   Will modify include directories to make the source compile.
#   ALL_SRC_C: C source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   ALL_SRC_CXX: C++ source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   ALL_SRC_ASM: assembly source files to be compiled will be added to this list. This shall be added to your add_executable or add_library command.
#   Include directories will be modified by using the include_directories() commands as needed.

#Get the current directory where this file is located.
set(ITS_TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
if(NOT DEFINED TFM_ROOT_DIR)
    message(FATAL_ERROR "Please set TFM_ROOT_DIR before including this file.")
endif()

if (NOT DEFINED ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS)
    message(FATAL_ERROR "Incomplete build configuration: ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS is undefined. ")
elseif (ENABLE_INTERNAL_TRUSTED_STORAGE_SERVICE_TESTS)
    list(APPEND ALL_SRC_C_NS "${ITS_TEST_DIR}/non_secure/psa_its_ns_interface_testsuite.c"
                "${ITS_TEST_DIR}/its_tests_common.c")

    list(APPEND ALL_SRC_C_S "${ITS_TEST_DIR}/secure/psa_its_s_interface_testsuite.c"
                "${ITS_TEST_DIR}/secure/psa_its_s_reliability_testsuite.c"
                "${ITS_TEST_DIR}/its_tests_common.c")

    if (NOT ITS_RAM_FS AND NOT (REFERENCE_PLATFORM OR ${TARGET_PLATFORM} STREQUAL "AN524"))
        # Show flash warning message only when the RAM FS is not in use or the tests are compiled to
        # be executed in the reference plaforms (AN519 and AN521) & AN524. The reference platforms and AN524
        # use RAM memory to emulated a flash device as they do not have one.
        set_property(SOURCE ${ALL_SRC_C_S} APPEND PROPERTY COMPILE_DEFINITIONS ITS_SHOW_FLASH_WARNING)
        set_property(SOURCE ${ALL_SRC_C_NS} APPEND PROPERTY COMPILE_DEFINITIONS ITS_SHOW_FLASH_WARNING)
    endif()

    #Setting include directories
    embedded_include_directories(PATH ${TFM_ROOT_DIR} ABSOLUTE)
    embedded_include_directories(PATH ${TFM_ROOT_DIR}/interface/include ABSOLUTE)
endif()
