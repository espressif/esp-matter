/***************************************************************************//**
 * @file
 * @brief sl_flex_ieee802154_support.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>
#include "em_system.h"
#include "app_log.h"
#include "sl_component_catalog.h"
#include "sl_flex_util_802154_protocol.h"
#include "sl_flex_ieee802154_support.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define ADDRESS_FILTER_INDEX                        (0U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void sl_flex_ieee802154_change_std(RAIL_Handle_t rail_handle,
                                          sl_flex_ieee802154_std_t std,
                                          sl_flex_ieee802154_frame_t *tx_frame,
                                          sl_flex_ieee802154_std_t* current_std,
                                          bool* current_ack,
                                          uint16_t *channel);

/**************************************************************************//**
 * The function initializes the IEEE 802.15.4 or IEEE 802.15.4g.
 *
 * @param[in] r_handle        RAIL handle
 * @param[in] std             desired std. (IEEE 802.15.4 or IEEE 802.15.4g)
 * @return int16_t
 * @retval APP_IEEE802154_ERROR   the initialization failed
 * @retval APP_IEEE802154_OK      the initialization run successfully
 *****************************************************************************/
static int16_t sl_flex_ieee802154_reinit(RAIL_Handle_t r_handle,
                                         sl_flex_ieee802154_std_t std);

/**************************************************************************//**
 * The function gives back if the ACK requested or not.
 *
 * @return ACK request status
 * @retval true ACK requested
 * @retval false ACK not requested
 *****************************************************************************/
bool sl_flex_ieee802154_get_ack_status(void);

/**************************************************************************//**
 * This function sets the PAN ID address for address filter.
 *
 * @param[in] r_handle        RAIL handle
 * @param[in] address         desired PAN ID address
 * @return int16_t
 * @retval APP_IEEE802154_ERROR   it failed
 * @retval APP_IEEE802154_OK      it run successfully
 *****************************************************************************/
static int16_t sl_flex_ieee802154_set_pan_id_filter(RAIL_Handle_t r_handle,
                                                    uint16_t address);

/**************************************************************************//**
 * This function sets the Short/Source address for address filter.
 *
 * @param[in] r_handle        RAIL handle
 * @param[in] address         desired Short/Source address
 * @return int16_t
 * @retval APP_IEEE802154_ERROR   it failed
 * @retval APP_IEEE802154_OK      it run successfully
 *****************************************************************************/
static int16_t sl_flex_ieee802154_set_short_addr_filter(RAIL_Handle_t r_handle,
                                                        uint16_t address);

/**************************************************************************//**
 * This function sets the Long address for address filter.
 *
 * @param[in] r_handle        RAIL handle
 * @param[in] *address        desired Long address's pointer
 * @return int16_t
 * @retval APP_IEEE802154_ERROR   it failed
 * @retval APP_IEEE802154_OK      it run successfully
 *****************************************************************************/
static int16_t sl_flex_ieee802154_set_long_addr_filter(RAIL_Handle_t r_handle,
                                                       uint64_t *address);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// currently used standard
static sl_flex_ieee802154_std_t standard_protocol =
  SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ;
/// currently requested or not ACK
static bool is_ack_requested = false;
/// currently used channel for standard
static uint16_t channel = 0U;
/// system number, all devices have unique number.
static uint64_t system_number = 0U;
/// CSMA/CA configuration structure for IEEE 502.15.4 2003 2p4 GHz
static RAIL_CsmaConfig_t csma_config_2p4 =
  RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;
/// CSMA/CA configuration structure for IEEE 502.15.4g
static RAIL_CsmaConfig_t csma_config_sub = {
  .csmaMinBoExp = 3,      // 2^3-1 for 0..7 backoffs on 1st try
  .csmaMaxBoExp = 5,      // 2^5-1 for 0..31 backoffs on 3rd+ tries
  .csmaTries = 5,         // 5 tries overall (4 re-tries)
  .ccaThreshold = -81,    // 10 dB above sensitivity
  .ccaBackoff = 1160,     // 1ms+ ccaDuration
  .ccaDuration = 160,     // 8 symbols at 20 us/symbol
  .csmaTimeout = 0,       // no timeout
};
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
int16_t sl_flex_ieee802154_protocol_init(RAIL_Handle_t r_handle,
                                         sl_flex_util_802154_protocol_type_t protocol)
{
  // based on the protocol (used via UC) setups the communication considering
  // the RF bands as well
  int16_t status = SL_FLEX_IEEE802154_OK;

  switch (protocol) {
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ:
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV:
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_COEX:
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ_ANTDIV_COEX:
      standard_protocol = SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ;
      channel = SL_FLEX_IEEE802154_CHANNEL_2P4;
      break;
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ:
      standard_protocol = SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ;
      channel = SL_FLEX_IEEE802154_CHANNEL_915;
      break;
    case SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ:
      standard_protocol = SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ;
      channel = SL_FLEX_IEEE802154_CHANNEL_863;
      break;
    default:
      status = SL_FLEX_IEEE802154_UNSUPPORTED_PROTOCOL;
      break;
  }

  if (status != SL_FLEX_IEEE802154_OK) {
    return status;
  }

  // sets the PAN ID (by default for broadcast)
  (void)sl_flex_ieee802154_set_pan_id_filter(r_handle,
                                             DEFAULT_BROADCAST_PAN_ID);
  // sets the Short address/Source address (by default for Broadcast)
  (void)sl_flex_ieee802154_set_short_addr_filter(r_handle,
                                                 DEFAULT_BROADCAST_SRC_ADDR);
  // obtains the unique number of the system
  system_number = SYSTEM_GetUnique();
  // sets the long address
  (void)sl_flex_ieee802154_set_long_addr_filter(r_handle, &system_number);

  return status;
}

sl_flex_ieee802154_std_t sl_flex_ieee802154_get_std(void)
{
  return standard_protocol;
}

uint16_t sl_flex_ieee802154_get_channel(void)
{
  return channel;
}

void sl_flex_ieee802154_print_frame(sl_flex_ieee802154_std_t std,
                                    sl_flex_ieee802154_frame_t *frame)
{
  if (frame == NULL) {
    app_log_error("sl_flex_ieee802154_print_frame ERR: parameter\n");
    return;
  }

  const uint8_t *tmp_payload = (uint8_t*)frame->payload;

  // prints the PHR information in case of g option
  if ((std == SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ)
      || (std == SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ)) {
    app_log_info("PHR DW,CRC cfg (0x%02X):\n", frame->phr_config);
    app_log_info("  Whitening:         %s\n",
                 frame->phr_config & SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON ? "ON" : "OFF");
    app_log_info("  CRC length:        %s\n",
                 frame->phr_config & SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE ? "2B" : "4B");
  }

  // prints the MHR parameters
  app_log_info("MHR field:\n");
  app_log_info("  Frame control:     0x%04X\n", frame->mhr_config.frame_control);
  app_log_info("  Seq. number:       0x%02X\n", frame->mhr_config.sequence_number);
  app_log_info("  PAN ID:            0x%04X\n", frame->mhr_config.destination_pan_id);
  app_log_info("  Dest. address:     0x%04X\n", frame->mhr_config.destination_address);
  app_log_info("  Src address:       0x%04X\n", frame->mhr_config.source_address);

  // prints the payload
  app_log_info("Payload: ");
  for (uint8_t i = 0; i < frame->payload_size; i++) {
    app_log_info("0x%02X, ", tmp_payload[i]);
  }
  app_log_info("\n");
}

void sl_flex_ieee802154_print_ack(sl_flex_ieee802154_std_t std,
                                  const uint8_t *buffer)
{
  uint8_t size = 0U;

  if (buffer == NULL) {
    app_log_error("sl_flex_ieee802154_print_ack ERR: parameter\n");
    return;
  }

  if (std == SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ) {
    size = SL_FLEX_IEEE802154_ACK_PACKET_LENGTH;
  } else {
    size = SL_FLEX_IEEE802154G_ACK_PACKET_LENGTH;
  }

  app_log_info("ACK: ");
  for (uint8_t i = 0; i < size; i++) {
    app_log_info("0x%02X, ", buffer[i]);
  }
  app_log_info("\n");
}

void sl_flex_ieee802154_request_manager(RAIL_Handle_t r_handle,
                                        sl_flex_ieee802154_frame_t *tx_frame,
                                        volatile sl_flex_ieee802154_cli_requests *cli_req,
                                        volatile sl_flex_ieee802154_cli_data *cli_desired)
{
  if (tx_frame == NULL || cli_req == NULL || cli_desired == NULL) {
    app_log_error("sl_flex_ieee802154_request_manager ERR: parameter\n");
    return;
  }

  // Std change request received
  if (cli_req->std_requested) {
    cli_req->std_requested = false;
    sl_flex_ieee802154_change_std(r_handle,
                                  cli_desired->desired_std, tx_frame,
                                  &standard_protocol, &is_ack_requested,
                                  &channel);
  }

  // PAN ID change request received
  if (cli_req->panid_requested) {
    cli_req->panid_requested = false;
    tx_frame->mhr_config.destination_pan_id = cli_desired->desired_panid;
    // re-set the addresses
    (void)sl_flex_ieee802154_set_pan_id_filter(r_handle,
                                               tx_frame->mhr_config.destination_pan_id);
  }

  // Source address change request received
  if (cli_req->srcaddr_requested) {
    cli_req->srcaddr_requested = false;
    tx_frame->mhr_config.source_address = cli_desired->desired_srcaddr;
    // re-set the addresses
    (void)sl_flex_ieee802154_set_short_addr_filter(r_handle,
                                                   tx_frame->mhr_config.source_address);
  }

  // Destination address change request received
  if (cli_req->destaddr_requested) {
    cli_req->destaddr_requested = false;
    tx_frame->mhr_config.destination_address = cli_desired->desired_destaddr;
  }

  // Auto-ACK change request received
  if (cli_req->ack_requested) {
    cli_req->ack_requested = false;
    is_ack_requested = cli_desired->desired_ack;
    if (is_ack_requested == true) {
      // sets the ACK req. in the FCF
      tx_frame->mhr_config.frame_control |= MAC_FRAME_FLAG_ACK_REQUIRED;
    } else {
      // clear the ACK req. in  the FCF
      tx_frame->mhr_config.frame_control &= ~MAC_FRAME_FLAG_ACK_REQUIRED;
    }
  }

  // Data whitening change request received
  if (cli_req->cfgdw_requested) {
    cli_req->cfgdw_requested = false;
    if (cli_desired->desired_dw) {
      tx_frame->phr_config |= SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON;
    } else {
      tx_frame->phr_config &= ~SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON;
    }
  }

  // CRC size change request received
  if (cli_req->cfgcrc_requested) {
    cli_req->cfgcrc_requested = false;
    if (cli_desired->desired_crc == SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE) {
      // 2 bytes
      tx_frame->phr_config |= SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE;
    } else {
      // 4 bytes
      tx_frame->phr_config &= ~SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE;
    }
  }
}

void sl_flex_ieee802154_prepare_sending(sl_flex_ieee802154_frame_t *tx_frame,
                                        void* payload, uint16_t payload_size)
{
  if (tx_frame == NULL || payload == NULL || payload_size == 0) {
    app_log_error("sl_flex_ieee802154_prepare_sending ERR: parameter\n");
    return;
  }
  tx_frame->mhr_config.sequence_number++;
  tx_frame->payload = payload;
  tx_frame->payload_size = payload_size;
}

void sl_flex_ieee802154_update_status(sl_flex_ieee802154_status_t *comm_status,
                                      const sl_flex_ieee802154_frame_t *tx_frame)
{
  if (comm_status == NULL || tx_frame == NULL) {
    app_log_error("sl_flex_ieee802154_update_status ERR: parameter\n");
    return;
  }

  comm_status->std = sl_flex_ieee802154_get_std();
  comm_status->ack = sl_flex_ieee802154_get_ack_status();
  comm_status->channel = sl_flex_ieee802154_get_channel();
  comm_status->destination_pan_id = tx_frame->mhr_config.destination_pan_id;
  comm_status->source_address = tx_frame->mhr_config.source_address;
  comm_status->destination_address = tx_frame->mhr_config.destination_address;
  comm_status->destination_pan_id = tx_frame->mhr_config.destination_pan_id;

  if ((comm_status->std == SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ)
      || (comm_status->std == SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ)) {
    if ((tx_frame->phr_config & SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON)
        == SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON) {
      comm_status->data_whitening = true;
    } else {
      comm_status->data_whitening = false;
    }

    if ((tx_frame->phr_config & SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE)
        == SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE) {
      comm_status->crc_length = SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE;
    } else {
      comm_status->crc_length = SL_FLEX_IEEE802154G_CRC_LENGTH_4BYTE;
    }
  }
}

bool sl_flex_ieee802154_is_change_requested(
  volatile sl_flex_ieee802154_cli_requests *cli_requests)
{
  if (cli_requests == NULL) {
    app_log_error("sl_flex_ieee802154_is_change_requested ERR: parameter\n");
    return false;
  }

  return cli_requests->std_requested || cli_requests->panid_requested
         || cli_requests->srcaddr_requested || cli_requests->destaddr_requested
         || cli_requests->ack_requested || cli_requests->cfgdw_requested
         || cli_requests->cfgcrc_requested;
}

RAIL_Status_t sl_flex_ieee802154_transmission(RAIL_Handle_t rail_handle,
                                              const uint8_t *packet,
                                              uint16_t packet_size)
{
  RAIL_Status_t status;
  RAIL_TxOptions_t txOptions = RAIL_TX_OPTIONS_NONE;
  RAIL_SchedulerInfo_t* scheduler_ptr = NULL;
#ifdef SL_CATALOG_BLUETOOTH_PRESENT
  RAIL_SchedulerInfo_t scheduler_info = (RAIL_SchedulerInfo_t){ .priority = 100,
                                                                .slipTime = 100000,
                                                                .transactionTime = 2500 };
  scheduler_ptr = &scheduler_info;
#endif

  if (packet == NULL || packet_size == 0) {
    app_log_error("sl_flex_ieee802154_transmission ERR: parameter\n");
    return RAIL_STATUS_INVALID_PARAMETER;
  }

  // sets the tx options based on the current ACK settings (auto-ACK enabled?)
  if (sl_flex_ieee802154_get_ack_status()) {
    txOptions = RAIL_TX_OPTION_WAIT_FOR_ACK;
  } else {
    txOptions = RAIL_TX_OPTIONS_NONE;
  }

  // writes the TX FIFO
  RAIL_WriteTxFifo(rail_handle, packet, packet_size, true);

  if (sl_flex_ieee802154_get_std() == SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ) {
    // starts the TX on the desired channel with the tx option and CSMA
    // functionality
    status = RAIL_StartCcaCsmaTx(rail_handle,
                                 sl_flex_ieee802154_get_channel(),
                                 txOptions,
                                 &csma_config_2p4,
                                 scheduler_ptr);
    if (status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartCcaCsmaTx status: %d failed", status);
    }
  } else {
    // starts the TX on the desired channel with the tx option and CSMA
    // functionality
    status = RAIL_StartCcaCsmaTx(rail_handle,
                                 sl_flex_ieee802154_get_channel(),
                                 txOptions,
                                 &csma_config_sub,
                                 scheduler_ptr);

    if (status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartCcaCsmaTx status: %d failed", status);
    }
  }

  return status;
}

int16_t sl_flex_ieee802154_pack_data_frame(sl_flex_ieee802154_std_t std,
                                           sl_flex_ieee802154_frame_t *frame,
                                           uint16_t *frame_size,
                                           uint8_t *frame_buffer)
{
  uint16_t ret_val = 0;

  if ((frame == NULL) || (frame_size == NULL) || (frame_buffer == NULL)) {
    app_log_error("sl_flex_ieee802154_pack_data_frame ERR: parameter");
    return SL_FLEX_IEEE802154_ERROR;
  }

  switch (std) {
    case SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ:
      ret_val = sl_flex_802154_packet_pack_data_frame(&frame->mhr_config,
                                                      frame->payload_size,
                                                      frame->payload,
                                                      frame_size,
                                                      frame_buffer);
      break;
    case SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ:
    case SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ:
      ret_val = sl_flex_802154_packet_pack_g_opt_data_frame(frame->phr_config,
                                                            &frame->mhr_config,
                                                            frame->payload_size,
                                                            frame->payload,
                                                            frame_size,
                                                            frame_buffer);
      break;
    default:
      ret_val = SL_FLEX_IEEE802154_UNSUPPORTED_PROTOCOL;
      break;
  }

  return ret_val;
}

int16_t sl_flex_ieee802154_unpack_data_frame(sl_flex_ieee802154_std_t std,
                                             sl_flex_ieee802154_frame_t *frame,
                                             uint8_t *frame_buffer,
                                             uint16_t max_frame_size)
{
  uint16_t max_payload_size = 0U;
  int16_t status = SL_FLEX_IEEE802154_OK;

  if ((frame == NULL) || (frame_buffer == NULL)) {
    app_log_error("sl_flex_ieee802154_unpack_data_frame ERR: parameter");
    return SL_FLEX_IEEE802154_ERROR;
  }

  switch (std) {
    case SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ:
      frame->payload = sl_flex_802154_packet_unpack_data_frame(&frame->mhr_config,
                                                               &frame->payload_size,
                                                               frame_buffer);
      if (frame->payload == NULL) {
        app_log_warning("sl_flex_802154_packet_unpack_data_frame failed");
        status = SL_FLEX_IEEE802154_ERROR;
        break;
      }

      // max payload calculation
      max_payload_size = max_frame_size
                         - SL_FLEX_IEEE802154_PHR_LENGTH
                         - SL_FLEX_IEEE802154_MHR_LENGTH
                         - SL_FLEX_IEEE802154_CRC_LENGTH;

      // checks the size of the received payload
      if (frame->payload_size > max_payload_size) {
        app_log_warning("sl_flex_802154_packet_unpack_data_frame failed: Corrupted frame received: payload is too large.");
        status = SL_FLEX_IEEE802154_ERROR;
        break;
      }

      break;
    case SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ:
    case SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ:
      frame->payload = sl_flex_802154_packet_unpack_g_opt_data_frame(
        &frame->phr_config,
        &frame->mhr_config,
        &frame->payload_size,
        frame_buffer);

      if (frame->payload == NULL) {
        app_log_warning("sl_flex_802154_packet_unpack_data_frame failed");
        status = SL_FLEX_IEEE802154_ERROR;
        break;
      }

      // considering the size of the CRC the max payload is calculated
      if (frame->phr_config & SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE) {
        max_payload_size = max_frame_size
                           - SL_FLEX_IEEE802154G_PHR_LENGTH
                           - SL_FLEX_IEEE802154_MHR_LENGTH
                           - SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE;
      } else {
        max_payload_size = max_frame_size
                           - SL_FLEX_IEEE802154G_PHR_LENGTH
                           - SL_FLEX_IEEE802154_MHR_LENGTH
                           - SL_FLEX_IEEE802154G_CRC_LENGTH_4BYTE;
      }

      // checks the size of the received payload
      if (frame->payload_size > max_payload_size) {
        app_log_warning("sl_flex_802154_packet_unpack_data_frame failed: Corrupted frame received: payload is too large.");
        status = SL_FLEX_IEEE802154_ERROR;
        break;
      }

      break;
    default:
      status = SL_FLEX_IEEE802154_UNSUPPORTED_PROTOCOL;
      break;
  }

  return status;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * This helper function handles the standard change.
 ******************************************************************************/
static void sl_flex_ieee802154_change_std(RAIL_Handle_t rail_handle,
                                          sl_flex_ieee802154_std_t std,
                                          sl_flex_ieee802154_frame_t *tx_frame,
                                          sl_flex_ieee802154_std_t* current_std,
                                          bool* current_ack,
                                          uint16_t *new_channel)
{
  *current_std = std;

  // clear the seq.#
  tx_frame->mhr_config.sequence_number = 0U;
  // clear the ACK req. in  the FCF
  tx_frame->mhr_config.frame_control &= ~MAC_FRAME_FLAG_ACK_REQUIRED;
  *current_ack = false;
  // set default value
  tx_frame->mhr_config.destination_pan_id = DEFAULT_BROADCAST_PAN_ID;
  // set default value
  tx_frame->mhr_config.source_address = DEFAULT_BROADCAST_SRC_ADDR;
  // set default value
  tx_frame->mhr_config.destination_address = DEFAULT_BROADCAST_DEST_ADDR;

  if ((*current_std == SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ)
      || (*current_std == SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ)) {
    // release PHR config
    tx_frame->phr_config = 0x00;
    // 2-bytes CRC
    tx_frame->phr_config |= SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE;
    // Data whitening default settings
    if ((RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_RX == 0)
        && (RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_TX == 0)) {
      tx_frame->phr_config |= SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON;
    } else {
      tx_frame->phr_config &= ~SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON;
    }
  }

  // setting the channel
  switch (*current_std) {
    case SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ:
      *new_channel = SL_FLEX_IEEE802154_CHANNEL_2P4;
      break;
    case SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ:
      *new_channel = SL_FLEX_IEEE802154_CHANNEL_863;
      break;
    case SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ:
      *new_channel = SL_FLEX_IEEE802154_CHANNEL_915;
      break;
    default:
      break;
  }

  // initializes the IEEE 802.15.4 or the IEEE 802.15.4G
  (void)sl_flex_ieee802154_reinit(rail_handle, *current_std);
  // sets the previous PAN ID
  (void)sl_flex_ieee802154_set_pan_id_filter(rail_handle,
                                             tx_frame->mhr_config.destination_pan_id);
  // sets the previous short address
  (void)sl_flex_ieee802154_set_short_addr_filter(rail_handle,
                                                 tx_frame->mhr_config.source_address);
  // sets the long address
  (void)sl_flex_ieee802154_set_long_addr_filter(rail_handle, &system_number);
}

/*******************************************************************************
 * This helper function reinit the IEEE 802.15.4
 ******************************************************************************/
static int16_t sl_flex_ieee802154_reinit(RAIL_Handle_t r_handle,
                                         sl_flex_ieee802154_std_t std)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

  switch (std) {
    case SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ:
      // configures the IEEE 802.15.4 on 2.4GHz
      sl_flex_util_802154_protocol_config(r_handle,
                                          SL_FLEX_UTIL_PROTOCOL_IEEE802154_2P4GHZ);
      if (status != RAIL_STATUS_NO_ERROR) {
        app_log_error("sl_rail_util_protocol_config status: %d failed", status);
        return SL_FLEX_IEEE802154_ERROR;
      }
      break;
    case SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ:
      sl_flex_util_802154_protocol_config(r_handle,
                                          SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_863MHZ);
      if (status != RAIL_STATUS_NO_ERROR) {
        app_log_error("sl_rail_util_protocol_config status: %d failed", status);
        return SL_FLEX_IEEE802154_ERROR;
      }
      break;
    case SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ:
      sl_flex_util_802154_protocol_config(r_handle,
                                          SL_FLEX_UTIL_PROTOCOL_IEEE802154_GB868_915MHZ);
      if (status != RAIL_STATUS_NO_ERROR) {
        app_log_error("sl_rail_util_protocol_config status: %d failed", status);
        return SL_FLEX_IEEE802154_ERROR;
      }
      break;
    default:
      status = RAIL_STATUS_INVALID_PARAMETER;
      break;
  }

  if (status != RAIL_STATUS_NO_ERROR) {
    return SL_FLEX_IEEE802154_ERROR;
  }

  // configures G options for IEEE 802.15.4g (863/915MHz)
  if ((std == SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ)
      || (std == SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ)) {
    // sets the IEEE802154g
    status = RAIL_IEEE802154_ConfigGOptions(r_handle,
                                            RAIL_IEEE802154_G_OPTION_GB868,
                                            RAIL_IEEE802154_G_OPTION_GB868);
    if (status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_IEEE802154_ConfigGOptions status: %d failed", status);
      return SL_FLEX_IEEE802154_ERROR;
    }
  }

  // all settings has been successful.
  return SL_FLEX_IEEE802154_OK;
}

/*******************************************************************************
 * This helper function returns a boolean about the ACK requested or not.
 ******************************************************************************/
bool sl_flex_ieee802154_get_ack_status(void)
{
  return is_ack_requested;
}

/*******************************************************************************
 * This helper function sets the PAN ID into the filter.
 ******************************************************************************/
static int16_t sl_flex_ieee802154_set_pan_id_filter(RAIL_Handle_t r_handle,
                                                    uint16_t address)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  // sets the addresses for filtering
  status = RAIL_IEEE802154_SetPanId(r_handle, address, ADDRESS_FILTER_INDEX);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_error("app_ieee802154_set_pan_id_filter() status: %d failed", status);
    return SL_FLEX_IEEE802154_ERROR;
  }

  return SL_FLEX_IEEE802154_OK;
}

/*******************************************************************************
 * This helper function sets the short address into filter.
 ******************************************************************************/
static int16_t sl_flex_ieee802154_set_short_addr_filter(RAIL_Handle_t r_handle,
                                                        uint16_t address)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  // sets the addresses for filtering
  status = RAIL_IEEE802154_SetShortAddress(r_handle, address,
                                           ADDRESS_FILTER_INDEX);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_IEEE802154_SetShortAddress() status: %d failed", status);
    return SL_FLEX_IEEE802154_ERROR;
  }

  return SL_FLEX_IEEE802154_OK;
}

/*******************************************************************************
 * This helper function sets the long address into filter.
 ******************************************************************************/
static int16_t sl_flex_ieee802154_set_long_addr_filter(RAIL_Handle_t r_handle,
                                                       uint64_t *address)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  // sets the addresses for filtering
  status = RAIL_IEEE802154_SetLongAddress(r_handle, (uint8_t*)address,
                                          ADDRESS_FILTER_INDEX);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_IEEE802154_SetLongAddress() status: %d failed", status);
    return SL_FLEX_IEEE802154_ERROR;
  }

  return SL_FLEX_IEEE802154_OK;
}
