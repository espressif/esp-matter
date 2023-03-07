/***************************************************************************//**
 * @file mac-command.h
 * @brief Implementation of 802.15.4 mac command frames
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
#ifndef MAC_COMMAND_H
#define MAC_COMMAND_H
// 15.4 command definitions
//
//
#include PLATFORM_HEADER
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "buffer_manager/buffer-management.h"
#include "mac-types.h"

#ifdef __cplusplus
extern "C" {
#endif
// 15.4 MAC commands
enum {
  MAC_ASSOCIATION_REQUEST         = 0x01,   // <capability info (1)>
  MAC_ASSOCIATION_RESPONSE        = 0x02,   // <short address (2)>
                                            // <association status (1)>
  MAC_DISASSOCIATION_NOTIFICATION = 0x03,   // <disassociation reason (1)>
  MAC_DATA_REQUEST                = 0x04,   // nothing
  MAC_PAN_ID_CONFLICT             = 0x05,   // nothing
  MAC_ORPHAN_NOTIFICATION         = 0x06,   // nothing
  MAC_BEACON_REQUEST              = 0x07,   // nothing
  MAC_COORDINATOR_REALIGN         = 0x08,   // <PAN identifier (2)>
                                            // <parent short address (2)>
                                            // <logical channel (1)>
                                            // <child short address (2)>
  MAC_GTS_REQUEST                 = 0x09,   // <GTS characteristics>
  MAC_GTS_CONFIRM                 = 0x0A,   // ? spec doesn't seem to say
  MAC_EMBER_BOOTLOAD              = 0x7C,   // 'em' <version (1)><bl type (1)>
  MAC_MESSAGE_CANCELLED           = 0xFE,
  MAC_INVALID_COMMAND             = 0xFF
};

//#define MAC_INFO_INDEX       1

#define SL_BROADCAST_PAN_ID   0xFFFF
#define SL_BROADCAST_ADDRESS  0xFFFF

Buffer sl_mac_make_command(uint8_t command,
                           uint16_t macFrameControl,
                           uint8_t *destination,
                           uint16_t destinationPanId,
                           uint8_t *source,
                           uint16_t sourcePanId,
                           uint8_t *frame,
                           uint8_t frameLength,
                           Buffer payloadBuffer,
                           Buffer infoElementBuffer,
                           uint8_t nwk_index);

Buffer sl_mac_make_association_request(uint8_t mac_index, uint8_t nwk_index, EmberNodeId parent_id, EmberPanId pan_id, uint8_t capabilities);
Buffer sl_mac_make_association_response(uint8_t mac_index, uint8_t nwk_index, EmberEUI64 long_addr, EmberNodeId short_addr, uint8_t status);
Buffer sl_mac_make_data_request(uint8_t mac_index, uint8_t nwk_index);
Buffer sl_mac_make_orphan_notification(uint8_t mac_index, uint8_t nwk_index);
Buffer sl_mac_make_beacon_request(uint8_t mac_index, uint8_t nwk_index);
Buffer sl_mac_make_enhanced_beacon_request(uint8_t mac_index, uint8_t nwk_index, Buffer payload_info_elements);
Buffer sl_mac_make_coordinator_realign(uint8_t mac_index, uint8_t nwk_index, EmberEUI64 long_addr, EmberNodeId short_addr);
Buffer sl_mac_make_beacon(uint8_t mac_index, uint8_t nwk_index, uint16_t superframe, Buffer beacon_payload);
Buffer sl_mac_make_enhanced_beacon(uint8_t mac_index, uint8_t nwk_index, uint16_t superframe, Buffer beacon_payload, Buffer payload_info_elements);

Buffer sli_mac_make_enhanced_beacon_request_payload_info(uint8_t mac_index, uint8_t nwk_index, bool first_join);

#ifdef __cplusplus
}
#endif

#endif //MAC_COMMAND_H
