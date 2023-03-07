/***************************************************************************//**
 * @file
 * @brief Scanning for beacons.
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

#ifndef SCAN_H
#define SCAN_H

#include "upper-mac.h"
#include "mac-command.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The maximum channels per page are 27
 * page bits 31...27, channel bits 26...0.
 */
#define SL_MAX_CHANNELS_PER_PAGE   27

#define PG_BITMASK32(macPgChan)  ((uint32_t)((((macPgChan) >> CHANNEL_BITS) \
                                              | 0X18)) << SL_MAX_CHANNELS_PER_PAGE)

#define PG_CH_BITMASK32(macPgChan)                                   \
  ((uint32_t)(((macPgChan) >> CHANNEL_BITS) == 0) ? BIT32(macPgChan) \
   : PG_BITMASK32(macPgChan) | BIT32((macPgChan) & 0x1F))

#define ENERGY_SAMPLE_BUFFER_TOTAL_READS_INDEX    0
#define ENERGY_SAMPLE_BUFFER_READS_COUNT_INDEX    1
#define ENERGY_SAMPLE_BUFFER_READS_OFFSET         2

typedef uint32_t sl_channel_mask_t;

typedef void (*sl_mac_energy_scan_result_callback_t) (uint8_t channel, int8_t rssi_min, int8_t rssi_max, int8_t rssi_avg);

typedef void (*sl_mac_scan_complete_callback_t)(sl_channel_mask_t unscannedChannelsMask);

typedef void (*sli_mac_energy_scan_complete_callback_t)(Buffer result);

typedef enum {
  SL_MAC_SCAN_REQUEST_STAY_ON_CHANNEL = 0,
  SL_MAC_SCAN_GOTO_NEXT_CHANNEL = 1,
  SL_MAC_SCAN_ABORT = 2,
} sl_mac_scan_request_result_t;  // or request??

typedef sl_mac_scan_request_result_t (*sl_mac_send_scan_request_callback_t)(uint8_t channel, Buffer *scan_packet);

/** @brief could be used to start the scan state machine, without actually sending any packet out
 *
 * @param mask                       channel mask for scans
 *
 * @param scan_duration              scan(stay) duration for each channel
 *
 * @param send_scan_packet_callback  callback used to let the caller know scan state machine is ready to accept their packets,
 *                                   and should return to the state machine if it should stay/move on/ abort the scan for the next channel
 *
 * @param tx_power                   tx_power during scan
 *
 * @param response_callback          any packet that is received during the scan, will be directly handled to the caller through this callback
 *
 * @param scan_complete_callback     After the scan is complete, this callback would get called so that the caller could do its final operation
 *
 * @param set_pan_id_to_broadcast_pan Set local radio PAN ID to broadcast PAN ID
 *  (0xFFFF) when scanning. Certain callers of this function, like zigbee's ZLL,
 *  do not want to set the local radio PAN ID to 0xFFFF when scanning.
 */
sl_status_t sl_mac_raw_active_scan(sl_channel_mask_t mask,
                                   uint8_t scan_duration,
                                   sl_mac_send_scan_request_callback_t send_scan_request_callback,
                                   int8_t tx_power,
                                   sl_mac_receive_callback_t response_callback,
                                   sl_mac_scan_complete_callback_t scan_complete_callback,
                                   bool set_pan_id_to_broadcast_pan);

/** @brief could be used to start an active scan, and send out beacon requests
 *
 * @param mask                       channel mask for scans
 *
 * @param scan_duration              scan(stay) duration for each channel
 *
 * @param tx_power                   tx_power during scan
 *
 * @param response_callback          any packet that is received during the scan, will be directly handled to the caller through this callback
 *
 * @param scan_complete_callback     After the scan is complete, this callback would get called so that the caller could do its final operation
 */
sl_status_t sl_mac_active_scan (sl_channel_mask_t mask,
                                uint8_t scan_duration,
                                int8_t tx_power,
                                sl_mac_receive_callback_t response_callback,
                                sl_mac_scan_complete_callback_t scan_complete_callback);

/** @brief could be used to start an energy scan
 *
 * @param mask                       channel mask for scans
 *
 * @param scan_duration              scan(stay) duration for each channel
 *
 * @param num_energy_reads            desired number of energy reads per channel, an internal value would replace this if it is set to 0
 *
 * @param response_callback          all the energy readings on one cahnnel are passed up to the caller through this callback
 *
 * @param scan_complete_callback     After the scan is complete, this callback would get called so that the caller could do its final operation
 */
sl_status_t sl_mac_energy_scan (sl_channel_mask_t mask,
                                uint8_t scan_duration,
                                sl_mac_energy_scan_result_callback_t response_callback,
                                sl_mac_scan_complete_callback_t scan_complete_callback);

/** @brief could be used to start an internal energy scan
 *
 * @param mask                       channel mask for scans
 *
 * @param scan_duration              scan(stay) duration for each channel
 *
 * @param num_energy_reads            desired number of energy reads per channel, an internal value would replace this if it is set to 0
 *
 * @param scan_complete_callback     After the scan is complete, this callback would get called so that the caller could do its final operation
 */

sl_status_t sli_mac_energy_scan(sl_channel_mask_t mask,
                                uint8_t scan_duration,
                                sli_mac_energy_scan_complete_callback_t scan_complete_callback);

/** @brief Returns whether a scan is in progress */
bool sl_mac_scan_pending(void);

void sl_scan_timer_handler(void);
void sl_mac_mark_scan_buffer(void);
void sl_mac_scan_event_handler(void);
void sli_scan_stop(void);

sl_status_t sli_mac_alt_mac_scan(sl_mac_scan_complete_callback_t scan_complete_callback);

#ifdef SL_CATALOG_LOWER_MAC_SPINEL_PRESENT
void sli_set_energy_scan_results(int8_t rssiValue);
#endif

#ifdef __cplusplus
}
#endif

#endif // NO_SCAN
