/**
 * @file
 * @brief Configuration file for Wall Controller sample application.
 * @details This file contains definitions for the Z-Wave+ Framework as well for the sample app.
 *
 * @copyright 2018 Silicon Laboratories Inc.
 */
#ifndef _CONFIG_APP_H_
#define _CONFIG_APP_H_

#include <ZW_product_id_enum.h>
#include <CC_ManufacturerSpecific.h>
#include "config/CC_ManufacturerSpecific_config.h"

/****************************************************************************
 *
 * Application version, which is generated during release of SDK.
 * The application developer can freely alter the version numbers
 * according to his needs.
 *
 ****************************************************************************/
#define APP_VERSION           ZAF_VERSION_MAJOR
#define APP_REVISION          ZAF_VERSION_MINOR
#define APP_PATCH             ZAF_VERSION_PATCH

/**
 * Defines generic and specific device types for a Central Scene device type
 */
//@ [GENERIC_TYPE_ID]
#define GENERIC_TYPE          GENERIC_TYPE_WALL_CONTROLLER
#define SPECIFIC_TYPE         SPECIFIC_TYPE_NOT_USED
//@ [GENERIC_TYPE_ID]

/**
 * See ZW_basis_api.h for ApplicationNodeInformation field deviceOptionMask
 */
//@ [DEVICE_OPTIONS_MASK_ID]
#define DEVICE_OPTIONS_MASK   APPLICATION_NODEINFO_LISTENING
//@ [DEVICE_OPTIONS_MASK_ID]


/**
 * Defines used to initialize the Z-Wave Plus Info Command Class.
 */
//@ [APP_TYPE_ID]
#define APP_ROLE_TYPE         ZWAVEPLUS_INFO_REPORT_ROLE_TYPE_SLAVE_ALWAYS_ON
#define APP_ICON_TYPE         ICON_TYPE_GENERIC_WALL_CONTROLLER
#define APP_USER_ICON_TYPE    ICON_TYPE_GENERIC_WALL_CONTROLLER
//@ [APP_TYPE_ID]

/**
 * Defines used to initialize the Manufacturer Specific Command Class.
 */
#define APP_MANUFACTURER_ID   MFG_ID_ZWAVE
#define APP_PRODUCT_ID        PRODUCT_ID_WallController

#define APP_FIRMWARE_ID       APP_PRODUCT_ID | (PRODUCT_TYPE_ID << 8)

/**
 * Defines used to initialize the Association Group Information (AGI)
 * Command Class.
 *
 * @attention
 * The sum of NUMBER_OF_ENDPOINTS, MAX_ASSOCIATION_GROUPS and MAX_ASSOCIATION_IN_GROUP
 * may not exceed 18 as defined by ASSOCIATION_ALLOCATION_MAX or an error will be thrown
 * during compilation.
 *
 * @attention
 * It is advised not to change the parameters once a product has been launched, as subsequent
 * upgrades will erase the old structure's content and start a fresh association table.
 */
#define NUMBER_OF_ENDPOINTS         0
#define MAX_ASSOCIATION_GROUPS      7 // Number of elements in AGITABLE_ROOTDEVICE_GROUPS plus one (for Lifeline)
#define MAX_ASSOCIATION_IN_GROUP    5

/*
 * File identifiers for application file system
 * Range: 0x00000 - 0x0FFFF
 */
#define FILE_ID_APPLICATIONDATA     (0x00000)

//@ [AGI_TABLE_ID]
//@ [AGITABLE_ROOTDEVICE_GROUPS]
#define  AGITABLE_ROOTDEVICE_GROUPS \
 { /* Group 2 */ \
   {ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL, ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY01}, \
   1, \
   {{COMMAND_CLASS_BASIC_V2, BASIC_SET_V2}}, \
   "BTN0" \
 }, \
 { /* Group 3 */ \
   {ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL, ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY01}, \
   2, \
   {{COMMAND_CLASS_SWITCH_MULTILEVEL_V4, SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V4}, {COMMAND_CLASS_SWITCH_MULTILEVEL_V4, SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V4}}, \
   "BTN0" \
 }, \
 { /* Group 4 */ \
   {ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL, ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY02}, \
   1, \
   {{COMMAND_CLASS_BASIC_V2, BASIC_SET_V2}}, \
   "BTN2" \
 }, \
 { /* Group 5 */ \
   {ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL, ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY02}, \
   2, \
   {{COMMAND_CLASS_SWITCH_MULTILEVEL_V4, SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V4}, {COMMAND_CLASS_SWITCH_MULTILEVEL_V4, SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V4}}, \
   "BTN2" \
 }, \
 { /* Group 6 */ \
   {ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL, ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY03}, \
   1, \
   {{COMMAND_CLASS_BASIC_V2, BASIC_SET_V2}}, \
   "BTN3" \
 }, \
 { /* Group 7 */ \
   {ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL, ASSOCIATION_GROUP_INFO_REPORT_PROFILE_CONTROL_KEY03}, \
   2, \
   {{COMMAND_CLASS_SWITCH_MULTILEVEL_V4, SWITCH_MULTILEVEL_START_LEVEL_CHANGE_V4}, {COMMAND_CLASS_SWITCH_MULTILEVEL_V4, SWITCH_MULTILEVEL_STOP_LEVEL_CHANGE_V4}}, \
   "BTN3" \
 }
//@ [AGITABLE_ROOTDEVICE_GROUPS]
//@ [AGI_TABLE_ID]

/**
 * Security keys
 */
//@ [REQUESTED_SECURITY_KEYS_ID]
#define REQUESTED_SECURITY_KEYS   (SECURITY_KEY_S0_BIT | SECURITY_KEY_S2_UNAUTHENTICATED_BIT | SECURITY_KEY_S2_AUTHENTICATED_BIT)
//@ [REQUESTED_SECURITY_KEYS_ID]

#endif /* _CONFIG_APP_H_ */

