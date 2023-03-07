#-------------------------------------------------------------------------------
# Copyright (c) 2019, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#When included, this file will add a target to link the CC312 libraries. It will
#also add the dependency between the project and the CC312 build target.
cmake_minimum_required(VERSION 3.7)

add_dependencies(${PROJECT_NAME} ${CC312_TARGET_NAME})

get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)

if (TARGET_TYPE STREQUAL "EXECUTABLE")
	target_link_libraries(${PROJECT_NAME} "${CC312_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}libcmpu${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
	target_link_libraries(${PROJECT_NAME} "${CC312_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}libdmpu${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
	target_link_libraries(${PROJECT_NAME} "${MBEDCRYPTO_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX_C}mbedcrypto${CMAKE_STATIC_LIBRARY_SUFFIX_C}")
else()
	message(FATAL_ERROR "Unknown target type ${TARGET_TYPE}")
endif()
