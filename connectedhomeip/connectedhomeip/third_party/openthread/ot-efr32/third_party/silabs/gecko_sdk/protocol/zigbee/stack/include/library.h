/***************************************************************************//**
 * @file
 * @brief Interface for querying library status.
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

#ifndef SILABS_LIBRARY_H
#define SILABS_LIBRARY_H

/**
 * @addtogroup stack_info
 *
 * This file provides definitions relevant to libraries. A running image may
 * probe information about included libraries using the APIs defined in this
 * section.
 *
 * See library.h for more information.
 * @{
 */

// A library's status is an 8-bit value with information about it.
// The high bit indicates whether the library is present (1), or if it is a
// stub (0).  The lower 7-bits can be used for codes specific to the library.
// This allows a library, like the security library, to specify what additional
// features are present.
// A value of 0xFF is reserved, it indicates an error in retrieving the
// library status.

/** @brief This indicates the presence, absence, or status of an Ember
 *    stack library.
 */#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberLibraryStatus
#else
typedef uint8_t EmberLibraryStatus;
enum {
  // Base return codes. These may be ORed with statuses further below.
  EMBER_LIBRARY_PRESENT_MASK  = 0x80,
  EMBER_LIBRARY_IS_STUB       = 0x00,
  EMBER_LIBRARY_ERROR         = 0xFF,

  // The ZigBee Pro library uses the following to indicate additional
  // functionality:
  EMBER_ZIGBEE_PRO_LIBRARY_END_DEVICE_ONLY        = 0x00, // no router capability
  EMBER_ZIGBEE_PRO_LIBRARY_HAVE_ROUTER_CAPABILITY = 0x01,
  EMBER_ZIGBEE_PRO_LIBRARY_ZLL_SUPPORT            = 0x02,

  // The Security library uses the following to indicate additional
  // functionality:
  EMBER_SECURITY_LIBRARY_END_DEVICE_ONLY      = 0x00,
  EMBER_SECURITY_LIBRARY_HAVE_ROUTER_SUPPORT  = 0x01,  // router or trust center support

  // The Packet Validate library may be globally turned on/off.
  // Bit 0 indicates whether the library is enabled/disabled.
  EMBER_PACKET_VALIDATE_LIBRARY_DISABLED      = 0x00,
  EMBER_PACKET_VALIDATE_LIBRARY_ENABLED       = 0x01,
  EMBER_PACKET_VALIDATE_LIBRARY_ENABLE_MASK   = 0x01
};
#endif

/** @brief An enumerated list of library identifiers.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmberLibraryId
#else
typedef uint8_t EmberLibraryId;
enum {
  EMBER_FIRST_LIBRARY_ID              = 0x00,

  EMBER_ZIGBEE_PRO_LIBRARY_ID         = 0x00,
  EMBER_BINDING_LIBRARY_ID            = 0x01,
  EMBER_END_DEVICE_BIND_LIBRARY_ID    = 0x02,
  EMBER_SECURITY_CORE_LIBRARY_ID      = 0x03,
  EMBER_SECURITY_LINK_KEYS_LIBRARY_ID = 0x04,
  EMBER_ALARM_LIBRARY_ID              = 0x05,
  EMBER_CBKE_LIBRARY_ID               = 0x06,
  EMBER_CBKE_DSA_SIGN_LIBRARY_ID      = 0x07,
  EMBER_ECC_LIBRARY_ID                = 0x08,
  EMBER_CBKE_DSA_VERIFY_LIBRARY_ID    = 0x09,
  EMBER_PACKET_VALIDATE_LIBRARY_ID    = 0x0A,
  EMBER_INSTALL_CODE_LIBRARY_ID       = 0x0B,
  EMBER_ZLL_LIBRARY_ID                = 0x0C,
  EMBER_CBKE_LIBRARY_283K1_ID         = 0x0D,
  EMBER_ECC_LIBRARY_283K1_ID          = 0x0E,
  EMBER_CBKE_CORE_LIBRARY_ID          = 0x0F,
  EMBER_NCP_LIBRARY_ID                = 0x10,
  EMBER_MULTI_NETWORK_LIBRARY_ID      = 0x11,
  EMBER_ENHANCED_BEACON_REQUEST_LIBRARY_ID = 0x12,
  EMBER_CBKE_283K1_DSA_VERIFY_LIBRARY_ID   = 0x13,
  EMBER_MULTI_PAN_LIBRARY_ID          = 0x14,

  EMBER_NUMBER_OF_LIBRARIES           = 0x15,
  EMBER_NULL_LIBRARY_ID               = 0xFF
};
#endif

#define EMBER_LIBRARY_NAMES \
  "Zigbee Pro",             \
  "Binding",                \
  "End Device Bind",        \
  "Security Core",          \
  "Security Link Keys",     \
  "",  /*"Alarm",*/         \
  "CBKE 163K1",             \
  "CBKE DSA Sign",          \
  "ECC",                    \
  "CBKE DSA Verify",        \
  "Packet Validate",        \
  "Install Code",           \
  "ZLL",                    \
  "CBKE 283K1",             \
  "ECC 283K1",              \
  "CBKE core",              \
  "NCP",                    \
  "Multi network",          \
  "Enhanced Beacon Req",    \
  "CBKE 283K1 DSA Verify",  \
  "Multi-PAN",              \

#if !defined(EZSP_HOST)
// This will be defined elsewhere for the EZSP Host applications.

/** @brief This routine takes a library identifier and returns whether the
 * library is enabled or not. See ::EmberLibraryStatus for return codes.
 */
EmberLibraryStatus emberGetLibraryStatus(EmberLibraryId libraryId);
#endif

#endif // SILABS_LIBRARY_H
