/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __NCP_CONFIG__
#define __NCP_CONFIG__

#define EMBER_AF_DEVICE_NAME "zigbeed"
#define EMBER_AF_API_HAL "hal.h"
#define APPLICATION_TOKEN_HEADER "../../app/xncp/xncp-token.h"
#define EMBER_BINDING_TABLE_SIZE 128
#define EMBER_MAX_END_DEVICE_CHILDREN 64
#define EMBER_SOURCE_ROUTE_TABLE_SIZE 254
#define EMBER_ROUTE_TABLE_SIZE 254
#define EMBER_DISCOVERY_TABLE_SIZE 64
#define EMBER_ADDRESS_TABLE_SIZE 128
#define EMBER_NEIGHBOR_TABLE_SIZE 26
#define EMBER_PACKET_BUFFER_COUNT 255
#define EMBER_CUSTOM_MAC_FILTER_TABLE_SIZE 64
#define EMBER_MAC_FILTER_TABLE_SIZE 32
#define EMBER_CHILD_TABLE_SIZE 64
#define EMBER_AF_PLUGIN_ZIGBEE_PRO_STACK_CHILD_TABLE_SIZE 64
#define  EMBER_KEY_TABLE_SIZE 128
#define  EMBER_APS_UNICAST_MESSAGE_COUNT 32
#define  EMBER_APS_MESSAGE_COUNT 64
#endif // __NCP_CONFIG__
