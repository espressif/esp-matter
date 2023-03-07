#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to link the cc312 libraries. It will
#also add the dependency between the project and the cc312 build target.
cmake_minimum_required(VERSION 3.7)

add_dependencies(${PROJECT_NAME} ${CC312_TARGET_NAME})

# generate cc312 before tfm_s_obj_lib, so the incdir gets generated before
# anything that tries to use it
if (PROJECT_NAME STREQUAL "tfm_crypto")
add_dependencies(tfm_s_obj_lib ${CC312_TARGET_NAME})
endif()

#Build the CC312 install header dir before mbedtls
add_dependencies(${MBEDCRYPTO_TARGET_NAME} ${CC312_TARGET_NAME})

get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)

if (TARGET_TYPE STREQUAL "EXECUTABLE")
	target_link_libraries(${PROJECT_NAME} "${CC312_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}libcc_312${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
	target_link_libraries(${PROJECT_NAME} "${CC312_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}libpal_no_os${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
	target_link_libraries(${PROJECT_NAME} "${MBEDCRYPTO_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
elseif (TARGET_TYPE STREQUAL "STATIC_LIBRARY")
	compiler_merge_library(DEST ${PROJECT_NAME} LIBS "${CC312_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}libcc_312${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
	compiler_merge_library(DEST ${PROJECT_NAME} LIBS "${CC312_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}libpal_no_os${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
else()
	message(FATAL_ERROR "Unknown target type ${TARGET_TYPE}")
endif()
