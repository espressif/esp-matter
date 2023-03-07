/***************************************************************************//**
 * @file
 * @brief ZigBee 3.0 zdo test harness functionality
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

#define BEACON_REQUEST_COMMAND        (0x07)
#define BEACON_ASSOCIATION_PERMIT_BIT (0x80)
#define BEACON_HEADER_SIZE            (7)
#define BEACON_EXTENDED_PAN_ID_INDEX  (7)

// Zigbee part of MAC Payload, AKA the 'Beacon Payload'
#define BEACON_PAYLOAD_SIZE           (15)

// The size of a 802.15.4 MAC beacon payload in bytes.
#define MAC_BEACON_SIZE (4)

EmberPacketAction emAfPluginTestHarnessZ3ModifyBeaconPayload(uint8_t *beaconPayload,
                                                             uint8_t *payloadLength);
