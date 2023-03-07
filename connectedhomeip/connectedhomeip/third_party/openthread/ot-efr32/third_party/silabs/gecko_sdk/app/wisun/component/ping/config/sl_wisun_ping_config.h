/***************************************************************************//**
 * @file
 * @brief Wi-SUN Ping component configuration header.
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_WISUN_PING_CONFIG_H
#define SL_WISUN_PING_CONFIG_H

/**************************************************************************//**
 * @defgroup SL_WISUN_PING_CONFIG Configuration
 * @ingroup SL_WISUN_PING
 * @{
 *****************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Ping configuration

// <o SL_WISUN_PING_PACKET_INTERVAL> Packet Interval
// <i> Default: 1000
#define SL_WISUN_PING_PACKET_INTERVAL            1000U

// <o SL_WISUN_PING_MAX_PAYLOAD_LENGTH> Maximum length of payload
// <i> Default: 32
#define SL_WISUN_PING_MAX_PAYLOAD_LENGTH         128U

// <o SL_WISUN_PING_TIMEOUT_MS> Ping timeout
// <i> Default: 10000
#define SL_WISUN_PING_TIMEOUT_MS                 10000U

// <o SL_WISUN_PING_PACKET_COUNT> Default packet count for ping
// <i> Default: 4
#define SL_WISUN_PING_PACKET_COUNT               4U

// <o SL_WISUN_PING_PACKET_SIZE> Default packet size for ping
// <i> Default: 40
#define SL_WISUN_PING_PACKET_SIZE                40U

// </h>

// <<< end of configuration section >>>

/** @}*/

#endif // SL_WISUN_PING_CONFIG_H
