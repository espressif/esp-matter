/***************************************************************************//**
 * @file
 * @brief APIs and defines for the HC 11073 Tunnel plugin, which implements the
 *        tunneling of 11073 data over zigbee.
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

/**
 * @defgroup hc-11073-tunnel HC 11073 Tunnel
 * @ingroup component
 * @brief API and Callbacks for the Health Care 11073 Tunnel Component
 *
 * Silicon Labs implementation of the Health Care 11073 protocol tunnel.
 * This component handles the life cycle of the 11073 tunnel, which is used
 * by health care devices to transmit 11073 data over the Zigbee protocol.
 *
 */

/**
 * @addtogroup hc-11073-tunnel
 * @{
 */

// These are statically defined by the spec. Defines provided here
// to improve plugin readability.
#define CLUSTER_ID_11073_TUNNEL 0x0614
#define ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET 0x0001
#define ATTRIBUTE_11073_TUNNEL_MANAGER_ENDPOINT 0x0002
#define ATTRIBUTE_11073_TUNNEL_CONNECTED 0x0003
#define ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE 0x0004
#define ATTRIBUTE_11073_TUNNEL_IDLE_TIMEOUT 0x0005

// These are variable and should be defined by the application using
// this plugin.
#ifndef HC_11073_TUNNEL_ENDPOINT
  #define HC_11073_TUNNEL_ENDPOINT 1
#endif

/** @} */ // end of hc-11073-tunnel
