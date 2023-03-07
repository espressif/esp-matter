/***************************************************************************//**
 * @file
 * @brief Definitions for the ZLL Commissioning Common plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SILABS_ZIGBEE_ZLL_COMMISSIONING_H
#define SILABS_ZIGBEE_ZLL_COMMISSIONING_H

// *******************************************************************
// * zll-commissioning.h
// *
// *
// Copyright 2010-2018 Silicon Laboratories, Inc.
// *******************************************************************

// This is the master header file for the zll-profile API. Pull in the individual
// plugin files for the application.
#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "zll-commissioning-common.h"
#ifdef SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_CLIENT_PRESENT
#include "zll-commissioning-client.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_SERVER_PRESENT
#include "zll-commissioning-server.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_NETWORK_PRESENT
#include "zll-commissioning-network.h"
#endif
#else // !UC_BUILD
#include "zll-commissioning-common.h"
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT
#include "app/framework/plugin/zll-commissioning-client/zll-commissioning-client.h"
#endif
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER
#include "app/framework/plugin/zll-commissioning-server/zll-commissioning-server.h"
#endif
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_NETWORK
#include "app/framework/plugin/zll-commissioning-network/zll-commissioning-network.h"
#endif
#endif // UC_BUILD

#endif // SILABS_ZIGBEE_ZLL_COMMISSIONING_H
