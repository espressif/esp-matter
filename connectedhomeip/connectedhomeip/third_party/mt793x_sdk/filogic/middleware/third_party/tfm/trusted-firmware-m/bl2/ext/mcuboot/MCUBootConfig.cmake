#-------------------------------------------------------------------------------
# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

#Include BL2 bootloader related functions
include("${CMAKE_CURRENT_LIST_DIR}/MCUBoot.cmake")

set(BL2 True CACHE BOOL "Configure TF-M to use BL2 and enable building BL2")

if (BL2)
	add_definitions(-DBL2)

	set(MCUBOOT_REPO "TF-M" CACHE STRING "Configure which repository use the MCUBoot from")
	set_property(CACHE MCUBOOT_REPO PROPERTY STRINGS "TF-M;UPSTREAM")
	validate_cache_value(MCUBOOT_REPO)

	set(MCUBOOT_IMAGE_NUMBER 2 CACHE STRING "Configure the number of separately updatable firmware images")
	set_property(CACHE MCUBOOT_IMAGE_NUMBER PROPERTY STRINGS "1;2")
	validate_cache_value(MCUBOOT_IMAGE_NUMBER STRINGS)

	set(MCUBOOT_UPGRADE_STRATEGY "OVERWRITE_ONLY" CACHE STRING "Configure BL2 which upgrade strategy to use")
	set_property(CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS "OVERWRITE_ONLY;SWAP;NO_SWAP;RAM_LOADING")
	validate_cache_value(MCUBOOT_UPGRADE_STRATEGY)

	set(MCUBOOT_SIGNATURE_TYPE "RSA-3072" CACHE STRING "Algorithm used by MCUBoot to validate signatures.")
	set_property(CACHE MCUBOOT_SIGNATURE_TYPE PROPERTY STRINGS "RSA-3072;RSA-2048")
	validate_cache_value(MCUBOOT_SIGNATURE_TYPE)

	if (MCUBOOT_REPO STREQUAL "TF-M")
		set(MCUBOOT_HW_KEY On CACHE BOOL "Configure to use HW key for image verification. Otherwise key is embedded in MCUBoot image.")
	else() #Using upstream MCUBoot
		if (MCUBOOT_HW_KEY)
			message(WARNING "Cannot use HW key for image verification when building against upstream MCUBoot."
				" Your choice was overriden (MCUBOOT_HW_KEY=Off).")
		endif()
		set(MCUBOOT_HW_KEY Off)
	endif()

	set(MCUBOOT_LOG_LEVEL "LOG_LEVEL_INFO" CACHE STRING "Configure the level of logging in MCUBoot.")
	set_property(CACHE MCUBOOT_LOG_LEVEL PROPERTY STRINGS "LOG_LEVEL_OFF;LOG_LEVEL_ERROR;LOG_LEVEL_WARNING;LOG_LEVEL_INFO;LOG_LEVEL_DEBUG")
	if (NOT CMAKE_BUILD_TYPE STREQUAL "debug")
		set(MCUBOOT_LOG_LEVEL "LOG_LEVEL_OFF")
	endif()

	validate_cache_value(MCUBOOT_LOG_LEVEL)

	if ((${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "NO_SWAP" OR
		 ${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "RAM_LOADING") AND
		NOT (MCUBOOT_IMAGE_NUMBER EQUAL 1))
		message(WARNING "The number of separately updatable images with the NO_SWAP or the RAM_LOADING"
			" upgrade strategy can be only '1'. Your choice was overriden.")
		set(MCUBOOT_IMAGE_NUMBER 1)
	endif()

	if (MCUBOOT_REPO STREQUAL "UPSTREAM")
		set_property(CACHE MCUBOOT_UPGRADE_STRATEGY PROPERTY STRINGS "OVERWRITE_ONLY;SWAP")
		if (${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "NO_SWAP" OR
			${MCUBOOT_UPGRADE_STRATEGY} STREQUAL "RAM_LOADING")
			message(WARNING "The ${MCUBOOT_UPGRADE_STRATEGY} upgrade strategy cannot be used when building against"
				" upstream MCUBoot. Your choice was overriden.")
			mcuboot_override_upgrade_strategy("OVERWRITE_ONLY")
		endif()

		if (DEFINED SECURITY_COUNTER OR
			DEFINED SECURITY_COUNTER_S OR
			DEFINED SECURITY_COUNTER_NS)
				message(WARNING "Ignoring the values of SECURITY_COUNTER and/or SECURITY_COUNTER_* variables as"
					" upstream MCUBoot does not support rollback protection.")
				set(SECURITY_COUNTER "")
				set(SECURITY_COUNTER_S "")
				set(SECURITY_COUNTER_NS "")
		endif()

	endif()

else() #BL2 is turned off

	if (DEFINED MCUBOOT_IMAGE_NUMBER OR
		DEFINED MCUBOOT_UPGRADE_STRATEGY OR
		DEFINED MCUBOOT_SIGNATURE_TYPE OR
		DEFINED MCUBOOT_HW_KEY OR
		DEFINED MCUBOOT_LOG_LEVEL)
			message(WARNING "Ignoring the values of MCUBOOT_* variables as BL2 option is set to False.")
			set(MCUBOOT_IMAGE_NUMBER "")
			set(MCUBOOT_UPGRADE_STRATEGY "")
			set(MCUBOOT_SIGNATURE_TYPE "")
			set(MCUBOOT_HW_KEY "")
			set(MCUBOOT_LOG_LEVEL "")
	endif()

	if (DEFINED SECURITY_COUNTER OR
		DEFINED SECURITY_COUNTER_S OR
		DEFINED SECURITY_COUNTER_NS)
			message(WARNING "Ignoring the values of SECURITY_COUNTER and/or SECURITY_COUNTER_* variables as BL2 option is set to False.")
			set(SECURITY_COUNTER "")
			set(SECURITY_COUNTER_S "")
			set(SECURITY_COUNTER_NS "")
	endif()

	if (DEFINED IMAGE_VERSION OR
		DEFINED IMAGE_VERSION_S OR
		DEFINED IMAGE_VERSION_NS)
			message(WARNING "Ignoring the values of IMAGE_VERSION and/or IMAGE_VERSION_* variables as BL2 option is set to False.")
			set(IMAGE_VERSION "")
			set(IMAGE_VERSION_S "")
			set(IMAGE_VERSION_NS "")
	endif()
	if (DEFINED S_IMAGE_MIN_VER OR
		DEFINED NS_IMAGE_MIN_VER)
			message(WARNING "Ignoring the values of *_IMAGE_MIN_VER variables as BL2 option is set to False.")
			set(S_IMAGE_MIN_VER "")
			set(NS_IMAGE_MIN_VER "")
	endif()
endif()
