/***************************************************************************//**
 * @file
 * @brief sl_flex_ieee802154_support.h
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

#ifndef SL_FLEX_IEEE802154_SUPPORT_H_
#define SL_FLEX_IEEE802154_SUPPORT_H_

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "rail.h"
#include "rail_features.h"
#include "sl_component_catalog.h"
#include "rail_ieee802154.h"
#include "sl_flex_util_802154_protocol.h"
#include "sl_flex_packet_asm.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Default FCF settings for default starting broadcast
#define DEFAULT_FCF_FIELD                 (MAC_FRAME_TYPE_DATA                \
                                           | MAC_FRAME_FLAG_PANID_COMPRESSION \
                                           | MAC_FRAME_DESTINATION_MODE_SHORT \
                                           | MAC_FRAME_VERSION_2006           \
                                           | MAC_FRAME_SOURCE_MODE_SHORT)
/// Default value of PAN ID for broadcast
#define DEFAULT_BROADCAST_PAN_ID          (0xFFFF)
/// Default value of source address for broadcast
#define DEFAULT_BROADCAST_SRC_ADDR        (0x0000)
/// Default value of destination address for broadcast
#define DEFAULT_BROADCAST_DEST_ADDR       (0xFFFF)

// some board does not support the unwhitening.
#if RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_RX \
  && RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_TX
  #define APP_WHITENING_EN                 (false)
  #define APP_WHITENING_ON_OFF             SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_OFF
#else
  #define APP_WHITENING_EN                 (true)
  #define APP_WHITENING_ON_OFF             SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON
#endif

/// IEEE 802.15.4 error
#define SL_FLEX_IEEE802154_ERROR                    (-1)
/// IEEE 802.15.4 ok
#define SL_FLEX_IEEE802154_OK                       (0)
/// IEEE 802.15.4 unsupported protocol
#define SL_FLEX_IEEE802154_UNSUPPORTED_PROTOCOL     (1)
/// IEEE 802.15.4 unsupported RF band
#define SL_FLEX_IEEE802154_UNSUPPORTED_RF_BAND      (2)

// IEEE 802.15.4 macros
/// First 2.4GHz channel that can be used for IEEE Std. 802.15.4 (e.g 11)
#define SL_FLEX_IEEE802154_CHANNEL_2P4              (0x0B)
/// First 863MHz channel that can be used for IEEE Std. 802.15.4g (e.g 128)
#define SL_FLEX_IEEE802154_CHANNEL_863              (0x80)
/// First 863MHz channel that can be used for IEEE Std. 802.15.4g (e.g 224)
#define SL_FLEX_IEEE802154_CHANNEL_915              (0xE0)

/// Two IEEE Std. 802.15.4 variants
typedef enum {
  SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ,     //!< IEEE 802.15.4 basic (2P4GHz)
  SL_FLEX_IEEE802154_STD_IEEE802154G_863MHZ,    //!< IEEE 802.15.4g (863MHz)
  SL_FLEX_IEEE802154_STD_IEEE802154G_915MHZ     //!< IEEE 802.15.4g (915MHz)
} sl_flex_ieee802154_std_t;

/// Structure for IEEE 802154 frames
typedef struct {
  uint8_t phr_config;
  sl_flex_802154_packet_mhr_frame_t mhr_config;
  uint16_t payload_size;
  void *payload;
} sl_flex_ieee802154_frame_t;

/// Structure for CLI requests
typedef struct {
  bool std_requested;
  bool panid_requested;
  bool srcaddr_requested;
  bool destaddr_requested;
  bool ack_requested;
  bool cfgdw_requested;
  bool cfgcrc_requested;
} sl_flex_ieee802154_cli_requests;

/// Structure for CLI desired setting values
typedef struct {
  sl_flex_ieee802154_std_t desired_std;
  uint16_t desired_panid;
  uint16_t desired_srcaddr;
  uint16_t desired_destaddr;
  bool desired_ack;
  bool desired_dw;
  uint8_t desired_crc;
} sl_flex_ieee802154_cli_data;

/// Struct for status of the current set
typedef struct {
  sl_flex_ieee802154_std_t std;
  uint16_t channel;
  bool auto_ack;
  bool ack;
  uint16_t destination_pan_id;
  uint16_t destination_address;
  uint16_t source_address;
  bool data_whitening;
  uint8_t crc_length;
} sl_flex_ieee802154_status_t;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * The function initializes the IEEE802.15.4 according to selected protocol.
 *
 * @param[in] r_handle            RAIL handle
 * @param[in] protocol            radio config or IEEE 802.15.4 standards
 * @retval SL_FLEX_IEEE802154_ERROR   the initialization failed
 * @retval SL_FLEX_IEEE802154_OK      the initialization run successfully
 * @retval SL_FLEX_IEEE802154_UNSUPPORTED_PROTOCOL no IEEE 802.15.4 protocol
 * @retval SL_FLEX_IEEE802154_UNSUPPORTED_RF_BAND not supported RF band
 * @retval SL_FLEX_IEEE802154_RADIO_CONFIG the radio config is selected
 *****************************************************************************/
int16_t sl_flex_ieee802154_protocol_init(RAIL_Handle_t r_handle,
                                         sl_flex_util_802154_protocol_type_t protocol);

/**************************************************************************//**
 * The function gives back the initialized IEEE 802.15.4 protocol.
 *
 * @return initialized IEEE 802.15.4 protocol
 *
 * @remark It is worth calling this after sl_flex_ieee802154_protocol_init() that
 * initializes and sets the IEEE 802.15.4.
 *****************************************************************************/
sl_flex_ieee802154_std_t sl_flex_ieee802154_get_std(void);

/**************************************************************************//**
 * The function gives back the used channel.
 *
 * @return used channel
 *****************************************************************************/
uint16_t sl_flex_ieee802154_get_channel(void);

/**************************************************************************//**
 * The function prints the frame based on the desired standard.
 *
 * @param[in] std       used standard, IEEE 802.15.4 or IEEE 802.15.4g
 * @param[in] frame     frame contains all information for the statndard frame
 *****************************************************************************/
void sl_flex_ieee802154_print_frame(sl_flex_ieee802154_std_t std,
                                    sl_flex_ieee802154_frame_t *frame);

/**************************************************************************//**
 * The function prints the raw ACK frame based on the desired standard.
 *
 * @param[in] std       used standard, IEEE 802.15.4 or IEEE 802.15.4g
 * @param[in] buffer    buffer contains raw ACK frame
 *****************************************************************************/
void sl_flex_ieee802154_print_ack(sl_flex_ieee802154_std_t std,
                                  const uint8_t *buffer);

/**************************************************************************//**
 * The function initializes the settings when a new request is coming and
 * updates the status.
 *
 * @param[in] r_handle      RAIL handle
 * @param[in,out] tx_frame  frame for TX direction for editing
 * @param[in] cli_req       structure of CLI requests
 * @param[in] cli_desired   structure of CLI desired values of requests
 *****************************************************************************/
void sl_flex_ieee802154_request_manager(RAIL_Handle_t r_handle,
                                        sl_flex_ieee802154_frame_t *tx_frame,
                                        volatile sl_flex_ieee802154_cli_requests *cli_req,
                                        volatile sl_flex_ieee802154_cli_data *cli_desired);

/**************************************************************************//**
 * The function prepares the frame for packing: sets the payload and increases
 * the sequence number.
 *
 * @param[in,out] frame     frame for TX direction (prepared before sending)
 * @param[in] payload       payload desired data
 * @param[in] payload_size  size of the payload
 *****************************************************************************/
void sl_flex_ieee802154_prepare_sending(sl_flex_ieee802154_frame_t *frame,
                                        void* payload, uint16_t payload_size);

/**************************************************************************//**
 * The function updates the status belongs to IEEE 802154 based on the settings
 * and the parameters of the TX frame.
 *
 * @param[out] comm_status  status of the application/communication
 * @param[in] tx_frame      Frame of TX
 *****************************************************************************/
void sl_flex_ieee802154_update_status(sl_flex_ieee802154_status_t *comm_status,
                                      const sl_flex_ieee802154_frame_t *tx_frame);

/**************************************************************************//**
 * The function processes the requests.
 *
 * @param[in] cli_requests    CLI requests
 * @return TRUE if a/more requests come otherwise FALSE.
 *****************************************************************************/
bool sl_flex_ieee802154_is_change_requested(
  volatile sl_flex_ieee802154_cli_requests *cli_requests);

/**************************************************************************//**
 * The function transmits the packet using CSMA/AC.
 *
 * @param[in] rail_handle     RAIL handle
 * @param[in] packet          frame in packet shape, raw RAIL data packet
 * @param[in] packet_size     size of the packet
 *****************************************************************************/
RAIL_Status_t sl_flex_ieee802154_transmission(RAIL_Handle_t rail_handle,
                                              const uint8_t *packet,
                                              uint16_t packet_size);

/**************************************************************************//**
 * This function packs the IEEE 802.15.4 frame based on the standard into a buffer.
 *
 * @param[in] std             desired standard
 * @param[in] *frame          frame in parts
 * @param[in] *frame_size     frame size
 * @param[in] *frame_buffer   buffer for the packed frame
 * @retval APP_IEEE802154_ERROR   it failed
 * @retval APP_IEEE802154_OK      it run successfully
 *****************************************************************************/
int16_t sl_flex_ieee802154_pack_data_frame(sl_flex_ieee802154_std_t std,
                                           sl_flex_ieee802154_frame_t *frame,
                                           uint16_t *frame_size,
                                           uint8_t *frame_buffer);

/**************************************************************************//**
 * This function unpacks the IEEE 802.15.4 frame based on the standard from a buffer.
 *
 * @param[in] std             desired standard
 * @param[out] *frame         frame in parts (for unpacked frame)
 * @param[in] *frame_buffer   buffer of unpacking frame
 * @param[in] max_frame_size  Maximum size of the frame
 * @retval APP_IEEE802154_ERROR   it failed
 * @retval APP_IEEE802154_OK      it run successfully
 *****************************************************************************/
int16_t sl_flex_ieee802154_unpack_data_frame(sl_flex_ieee802154_std_t std,
                                             sl_flex_ieee802154_frame_t *frame,
                                             uint8_t *frame_buffer,
                                             uint16_t max_frame_size);

#endif /* SL_FLEX_IEEE802154_SUPPORT_H_ */
