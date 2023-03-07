/***************************************************************************//**
 * @file
 * @brief sl_flex_packet_asm.c
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
#include <stdbool.h>
#include "em_device.h"
#include "app_log.h"
#include "sl_flex_packet_asm.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * This helper function converts the length. It changes the order of the bits
 * (MSB <-> LSB) of the length for IEEE 802.15.4g PHR field.
 *
 * @param[out] payload     pointer where the order-changed
 *                         number copied
 * @param[in] length       uint16_t pointer where the order changed number copied
 * @return None
 *
 * @remark This function is used for converting the length of the IEEE 802.15.4g
 *         frame to pack it into the PHR field of the frame.
 *****************************************************************************/
static void sl_flex_ieee802154_set_length(uint8_t *payload, uint16_t length);

/**************************************************************************//**
 * This helper function converts the payload comes from the PHR field of
 * IEEE 802.15.4g. It changes the order of the bits (MSB <-> LSB), and return
 * the number.
 *
 * @param[in] payload     pointer where the order-changed number is.
 * @return                number/length
 *
 * @remark This function is used for obtain the length of the IEEE 802.15.4g
 *         frame from the PHR field of the frame.
 *****************************************************************************/
static uint16_t sl_flex_ieee802154_get_length(uint8_t *payload);

/**************************************************************************//**
 * Helper function to fill buffer with a 2 bytes template
 *
 * @param[out] *buffer     buffer pointer
 * @param[in]  length      length to fill
 *
 * @return                void
 *****************************************************************************/
static void fill_buffer(uint8_t *buffer, uint32_t length);

/**************************************************************************//**
 * Calculate BLE header length
 *
 * @param[in] payload_len  length of payload
 *
 * @return                header length
 *****************************************************************************/
static inline sl_flex_ble_packet_size_t
calc_ble_header_length(const sl_flex_ble_packet_size_t payload_len);

/**************************************************************************//**
 * Calculate BLE payload length
 *
 * @param[in] header_len  length of header
 *
 * @return                payload length
 *****************************************************************************/
static inline sl_flex_ble_packet_size_t
calc_ble_payload_length(const sl_flex_ble_packet_size_t header_len);
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
int16_t sl_flex_802154_packet_pack_g_opt_data_frame(uint8_t phr_cfg,
                                                    sl_flex_802154_packet_mhr_frame_t *mhr_cfg,
                                                    uint16_t payload_size,
                                                    void *payload,
                                                    uint16_t *frame_size,
                                                    uint8_t *frame_buffer)
{
  uint8_t *tmp_data = frame_buffer;
  uint8_t crc_size = 0;
  uint16_t length;

  if ((mhr_cfg == NULL) || (payload == NULL)
      || (frame_size == NULL) || (frame_buffer == NULL)) {
    app_log_warning("app_ieee802154_pack_frame ERR: parameter\r\n");
    return SL_FLEX_802154_PACKET_ERROR;
  }

  // PHR
  // to get the size of the CRC for frame length
  if ((phr_cfg & SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE)
      == SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE) {
    crc_size = SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE;
  } else {
    crc_size = SL_FLEX_IEEE802154G_CRC_LENGTH_4BYTE;
  }

  // check the size of the payload
  if (payload_size
      > (SL_FLEX_IEEE802154G_LEN_MAX - crc_size - SL_FLEX_IEEE802154_MHR_LENGTH)) {
    // the payload is too large
    app_log_warning("app_ieee802154_pack_frame (802.15.4g) ERR: payload size\r\n");
    return SL_FLEX_802154_PACKET_ERROR;
  }

  // calculates and gets the length (inverse Bit order of the length)
  length = SL_FLEX_IEEE802154_MHR_LENGTH + payload_size + crc_size;
  sl_flex_ieee802154_set_length(tmp_data, length);

  // set the rest of the PHR field
  tmp_data[0] |= phr_cfg;

  // step forward to MHR
  tmp_data += SL_FLEX_IEEE802154G_PHR_LENGTH;

  // MHR
  memcpy(tmp_data, &mhr_cfg->frame_control, sizeof(mhr_cfg->frame_control));
  tmp_data += sizeof(mhr_cfg->frame_control);

  memcpy(tmp_data, &mhr_cfg->sequence_number, sizeof(mhr_cfg->sequence_number));
  tmp_data += sizeof(mhr_cfg->sequence_number);

  memcpy(tmp_data, &mhr_cfg->destination_pan_id,
         sizeof(mhr_cfg->destination_pan_id));
  tmp_data += sizeof(mhr_cfg->destination_pan_id);

  memcpy(tmp_data, &mhr_cfg->destination_address,
         sizeof(mhr_cfg->destination_address));
  tmp_data += sizeof(mhr_cfg->destination_address);

  memcpy(tmp_data, &mhr_cfg->source_address, sizeof(mhr_cfg->source_address));
  tmp_data += sizeof(mhr_cfg->source_address);

  // payload
  // copy the payload into the frame
  memcpy(tmp_data, payload, payload_size);
  tmp_data += payload_size;

  // gets the frame size
  *frame_size = tmp_data - frame_buffer;

  // return 0 if the frame is ready
  return 0;
}

int16_t sl_flex_802154_packet_pack_data_frame(const sl_flex_802154_packet_mhr_frame_t *mhr_cfg,
                                              uint16_t payload_size,
                                              void *payload,
                                              uint16_t *frame_size,
                                              uint8_t *frame_buffer)
{
  uint8_t *tmp_data = frame_buffer;
  uint8_t crc_size = 0;
  uint16_t length;

  if ((mhr_cfg == NULL) || (payload == NULL)
      || (frame_size == NULL) || (frame_buffer == NULL)) {
    app_log_warning("app_ieee802154_pack_frame ERR: parameter\r\n");
    return SL_FLEX_802154_PACKET_ERROR;
  }

  // PHR
  crc_size = SL_FLEX_IEEE802154_CRC_LENGTH;

  // check the size of the payload
  if (payload_size
      > (SL_FLEX_IEEE802154_LEN_MAX - crc_size - SL_FLEX_IEEE802154_MHR_LENGTH)) {
    // the payload is too large
    app_log_warning("app_ieee802154_pack_frame (802.15.4) ERR: payload size\r\n");
    return SL_FLEX_802154_PACKET_ERROR;
  }

  // first byte is the PHR, put the size into it
  length = SL_FLEX_IEEE802154_MHR_LENGTH + payload_size + crc_size;
  tmp_data[0] = length;

  // step forward to MHR
  tmp_data += SL_FLEX_IEEE802154_PHR_LENGTH;

  // MHR
  memcpy(tmp_data, &mhr_cfg->frame_control, sizeof(mhr_cfg->frame_control));
  tmp_data += sizeof(mhr_cfg->frame_control);

  memcpy(tmp_data, &mhr_cfg->sequence_number, sizeof(mhr_cfg->sequence_number));
  tmp_data += sizeof(mhr_cfg->sequence_number);

  memcpy(tmp_data, &mhr_cfg->destination_pan_id,
         sizeof(mhr_cfg->destination_pan_id));
  tmp_data += sizeof(mhr_cfg->destination_pan_id);

  memcpy(tmp_data, &mhr_cfg->destination_address,
         sizeof(mhr_cfg->destination_address));
  tmp_data += sizeof(mhr_cfg->destination_address);

  memcpy(tmp_data, &mhr_cfg->source_address, sizeof(mhr_cfg->source_address));
  tmp_data += sizeof(mhr_cfg->source_address);

  // payload
  // copy the payload into the frame
  memcpy(tmp_data, payload, payload_size);
  tmp_data += payload_size;

  // gets the frame size
  *frame_size = tmp_data - frame_buffer;

  // return 0 if the frame is ready
  return 0;
}

int16_t sl_flex_802154_packet_pack_ofdm_data_frame(uint8_t rate,
                                                   uint8_t scrambler,
                                                   uint16_t payload_size,
                                                   const uint8_t *payload,
                                                   uint16_t *frame_size,
                                                   uint8_t *frame_buffer)
{
  uint32_t phr = 0U;
  uint16_t frameLength = 0U;
  uint8_t phr_size = 4U;
  uint8_t fcs_size = 4U;

  // Checking input parameters
  if ((rate & 0xE0)
      || (scrambler & 0xFC)
      || (payload_size == 0)
      || (payload == NULL)
      || (frame_size == NULL)
      || (frame_buffer == NULL)) {
    app_log_warning("sl_flex_802154_packet_pack_ofdm_data_frame ERR: parameter\r\n");
    return SL_FLEX_802154_PACKET_ERROR;
  }

  // The Frame Length field (L10-L0) specifies the total number of octets contained in the PSDU (prior to FEC encoding). The PSDU field carries the data of the PHY packet.
  frameLength = ((payload_size + phr_size) - phr_size) + fcs_size;
  phr = (rate << 19) | (frameLength << 7) | (scrambler << 3);
  // Flip the 32 bits for all SUN modulations
  phr = __RBIT(phr);

  // Write the phr in the payload
  for (uint8_t index = 0; index < phr_size; index++) {
    frame_buffer[index] = (uint8_t)((phr & (0xFF << index * 8)) >> index * 8);
  }
  // Add payload bytes
  *frame_size = payload_size + phr_size;
  for (uint8_t index = phr_size; index < *frame_size; index++) {
    frame_buffer[index] = payload[index - phr_size];
  }

  // return SL_FLEX_802154_PACKET_OK if the frame is ready
  return SL_FLEX_802154_PACKET_OK;
}

int16_t sl_flex_802154_packet_pack_oqpsk_data_frame(bool spreadingMode,
                                                    uint8_t rateMode,
                                                    uint16_t payload_size,
                                                    const uint8_t *payload,
                                                    uint16_t *frame_size,
                                                    uint8_t *frame_buffer)
{
  uint32_t phr = 0U;
  uint16_t frameLength = 0U;
  uint8_t phr_size = 4U;
  uint8_t fcs_size = 4U;

  // Checking input parameters
  if ((rateMode & 0xFC)
      || (payload_size == 0)
      || (payload == NULL)
      || (frame_size == NULL)
      || (frame_buffer == NULL)) {
    app_log_warning("sl_flex_802154_packet_pack_oqpsk_data_frame ERR: parameter\r\n");
    return SL_FLEX_802154_PACKET_ERROR;
  }

  // The Frame Length field (L10-L0) specifies the total number of octets contained in the PSDU (prior to FEC encoding). The PSDU field carries the data of the PHY packet.
  frameLength = ((payload_size + phr_size) - phr_size) + fcs_size;
  phr = ((uint8_t)spreadingMode << 15) | (rateMode << 13) | frameLength;
  // Flip the 32 bits for all SUN modulations
  phr = __RBIT(phr);

  // Write the phr in the payload
  for (uint8_t index = 0; index < phr_size; index++) {
    frame_buffer[index] = (uint8_t)((phr & (0xFF << index * 8)) >> index * 8);
  }
  // Add payload bytes
  *frame_size = payload_size + phr_size;
  for (uint8_t index = phr_size; index < *frame_size; index++) {
    frame_buffer[index] = payload[index - phr_size];
  }

  // return SL_FLEX_802154_PACKET_OK if the frame is ready
  return SL_FLEX_802154_PACKET_OK;
}

uint8_t *sl_flex_802154_packet_unpack_g_opt_data_frame(uint8_t *phr_cfg,
                                                       sl_flex_802154_packet_mhr_frame_t *mhr_cfg,
                                                       uint16_t *payload_size,
                                                       uint8_t *frame_buffer)
{
  uint8_t *tmp = frame_buffer;
  uint16_t length = 0U;
  uint8_t crc_size = 0U;

  if ((phr_cfg == NULL) || (payload_size == NULL) || (mhr_cfg == NULL)
      || (frame_buffer == NULL)) {
    app_log_warning("app_ieee802154_unpack_frame_get_payload_mhr ERR: parameter\r\n");
    return NULL;
  }

  // get the PHR config
  *phr_cfg = tmp[0] & SL_FLEX_IEEE802154G_PHR_GET_PHR_CFG_MASK;

  // get size of the CRC
  if ((*phr_cfg & SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE)
      == SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE) {
    crc_size = SL_FLEX_IEEE802154G_CRC_LENGTH_2BYTE;
  } else {
    crc_size = SL_FLEX_IEEE802154G_CRC_LENGTH_4BYTE;
  }

  // get length
  length = sl_flex_ieee802154_get_length(tmp);

  // step forward
  tmp += SL_FLEX_IEEE802154G_PHR_LENGTH;

  // MHR
  memcpy(&mhr_cfg->frame_control, tmp, sizeof(mhr_cfg->frame_control));
  tmp += sizeof(mhr_cfg->frame_control);

  memcpy(&mhr_cfg->sequence_number, tmp, sizeof(mhr_cfg->sequence_number));
  tmp += sizeof(mhr_cfg->sequence_number);

  memcpy(&mhr_cfg->destination_pan_id, tmp,
         sizeof(mhr_cfg->destination_pan_id));
  tmp += sizeof(mhr_cfg->destination_pan_id);

  memcpy(&mhr_cfg->destination_address, tmp,
         sizeof(mhr_cfg->destination_address));
  tmp += sizeof(mhr_cfg->destination_address);

  memcpy(&mhr_cfg->source_address, tmp, sizeof(mhr_cfg->source_address));
  tmp += sizeof(mhr_cfg->source_address);

  // calculates the size of the data
  *payload_size = length - crc_size - SL_FLEX_IEEE802154_MHR_LENGTH;

  return tmp;
}

uint8_t *sl_flex_802154_packet_unpack_data_frame(sl_flex_802154_packet_mhr_frame_t *mhr_cfg,
                                                 uint16_t *payload_size,
                                                 uint8_t *frame_buffer)
{
  uint8_t *tmp = frame_buffer;
  uint16_t length = 0U;
  uint8_t crc_size = 0U;

  if ((payload_size == NULL) || (mhr_cfg == NULL) || (frame_buffer == NULL)) {
    app_log_warning("app_ieee802154_unpack_frame_get_payload_mhr ERR: parameter\r\n");
    return NULL;
  }

  //PHR
  // get size
  length = (uint16_t)tmp[0];

  crc_size = SL_FLEX_IEEE802154_CRC_LENGTH;

  // step forward
  tmp += SL_FLEX_IEEE802154_PHR_LENGTH;

  // MHR
  memcpy(&mhr_cfg->frame_control, tmp, sizeof(mhr_cfg->frame_control));
  tmp += sizeof(mhr_cfg->frame_control);

  memcpy(&mhr_cfg->sequence_number, tmp, sizeof(mhr_cfg->sequence_number));
  tmp += sizeof(mhr_cfg->sequence_number);

  memcpy(&mhr_cfg->destination_pan_id, tmp,
         sizeof(mhr_cfg->destination_pan_id));
  tmp += sizeof(mhr_cfg->destination_pan_id);

  memcpy(&mhr_cfg->destination_address, tmp,
         sizeof(mhr_cfg->destination_address));
  tmp += sizeof(mhr_cfg->destination_address);

  memcpy(&mhr_cfg->source_address, tmp, sizeof(mhr_cfg->source_address));
  tmp += sizeof(mhr_cfg->source_address);

  // calculates the size of the data
  *payload_size = length - crc_size - SL_FLEX_IEEE802154_MHR_LENGTH;

  return tmp;
}

uint8_t *sl_flex_802154_packet_unpack_ofdm_data_frame(const RAIL_RxPacketInfo_t *packet_information,
                                                      uint8_t *rate,
                                                      uint8_t *scrambler,
                                                      uint16_t *payload_size,
                                                      uint8_t *frame_buffer)
{
  uint32_t phr = 0U;
  uint8_t *tmp = frame_buffer;
  uint8_t phr_size = 4U;
  uint8_t soft_modem_trailing_bytes = 6U;

  if ((packet_information == NULL)
      || (rate == NULL)
      || (scrambler == NULL)
      || (payload_size == NULL)
      || (frame_buffer == NULL)) {
    app_log_warning("sl_flex_802154_packet_unpack_ofdm_data_frame ERR: parameter\r\n");
    return NULL;
  }

  for (uint8_t index = 0; index < phr_size; index++) {
    phr |= frame_buffer[index] << (index * 8);
  }
  phr = __RBIT(phr);

  *rate = (phr >> 19) & 0x1F;
  *scrambler = (phr >> 3) & 0x03;

  *payload_size = (packet_information->packetBytes - phr_size - soft_modem_trailing_bytes);
  tmp += phr_size;

  return tmp;
}

uint8_t *sl_flex_802154_packet_unpack_oqpsk_data_frame(const RAIL_RxPacketInfo_t *packet_information,
                                                       bool *spreadingMode,
                                                       uint8_t *rateMode,
                                                       uint16_t *payload_size,
                                                       uint8_t *frame_buffer)
{
  uint32_t phr = 0U;
  uint8_t *tmp = frame_buffer;
  uint8_t phr_size = 4U;
  uint8_t soft_modem_trailing_bytes = 6U;

  if ((packet_information == NULL)
      || (spreadingMode == NULL)
      || (rateMode == NULL)
      || (payload_size == NULL)
      || (frame_buffer == NULL)) {
    app_log_warning("sl_flex_802154_packet_unpack_oqpsk_data_frame ERR: parameter\r\n");
    return NULL;
  }

  for (uint8_t index = 0; index < phr_size; index++) {
    phr |= frame_buffer[index] << (index * 8);
  }
  phr = __RBIT(phr);

  *spreadingMode = (bool)((phr >> 15) & 0x01);
  *rateMode = (phr >> 13) & 0x03;

  *payload_size = (packet_information->packetBytes - phr_size - soft_modem_trailing_bytes);
  tmp += phr_size;

  return tmp;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * This helper function sets the length.
 ******************************************************************************/
static void sl_flex_ieee802154_set_length(uint8_t *payload, uint16_t length)
{
  payload[1] = __RBIT(length & 0xFF) >> 24;
  payload[0] = __RBIT(length >> 8) >> 24;
}

/*******************************************************************************
 * This helper function gets the length.
 ******************************************************************************/
static uint16_t sl_flex_ieee802154_get_length(uint8_t *payload)
{
  uint16_t length = __RBIT(payload[1]) >> 24;
  length |= (__RBIT(payload[0]) >> 16) & 0x0700;
  return length;
}

/*******************************************************************************
 * Calculate ble header length from payload length
 ******************************************************************************/
static inline sl_flex_ble_packet_size_t
calc_ble_header_length(const sl_flex_ble_packet_size_t payload_len)
{
  return (payload_len + SL_FLEX_BLE_HEADER_LEN_BASE);
}

/*******************************************************************************
 * Calculate payload length from header length
 ******************************************************************************/
static inline sl_flex_ble_packet_size_t
calc_ble_payload_length(const sl_flex_ble_packet_size_t header_len)
{
  if (header_len < SL_FLEX_BLE_HEADER_LEN_BASE) {
    return 0;
  }
  return (header_len - SL_FLEX_BLE_HEADER_LEN_BASE);
}

/*******************************************************************************
 * Prepare BLE advertising packet
 ******************************************************************************/
void sl_flex_ble_prepare_packet(sl_flex_ble_advertising_packet_t *packet, const uint8_t *payload, const sl_flex_ble_packet_size_t payload_length)
{
  sl_flex_ble_packet_size_t padding_size;

  if (packet == NULL || payload == NULL || payload_length == 0 || payload_length > SL_FLEX_BLE_PAYLOAD_LEN_MAX) {
    app_log_warning("sl_flex_ble_prepare_packet ERR: parameter\r\n");
    return;
  }

  // BLE advertisement header
  packet->header.type = SL_FLEX_BLE_HEADER_LSB;   //BLE_BLE_ADV_NONCONN_IND

  // packet->header.length = 0x14;
  packet->header.length = calc_ble_header_length(payload_length);

  // LL advertiser's address
  packet->advAddr[0] = 0xC1;
  packet->advAddr[1] = 0x29;
  packet->advAddr[2] = 0xD8;
  packet->advAddr[3] = 0x57;
  packet->advAddr[4] = 0x0B;
  packet->advAddr[5] = 0x00;

  // AD Structure: Flags
  packet->flags.length = sizeof(packet->flags.advertising_type) + sizeof(packet->flags.flags);   // Length of field: Type + Flags
  packet->flags.advertising_type = SL_FLEX_BLE_ADSTRUCT_TYPE_FLAG;   // AD type: Flags
  packet->flags.flags = SL_FLEX_BLE_DISABLE_BR_EDR | SL_FLEX_BLE_GENERAL_DISCOVERABLE_MODE;   // Flags: BR/EDR is disabled, LE General Discoverable Mode
  // AD Structure: Manufacturer specific
  packet->manufactSpec.length = sizeof(packet->manufactSpec.advertising_type)
                                + sizeof(packet->manufactSpec.company_id)
                                + sizeof(packet->manufactSpec.version)
                                + payload_length;
  packet->manufactSpec.advertising_type = SL_FLEX_BLE_ADSTRUCT_TYPE_MANUFACTURER_SPECIFIC;   // AD type: Manufacturer Specific Data
  packet->manufactSpec.company_id = SL_FLEX_BLE_COMPANY_ID;
  packet->manufactSpec.version = 0x01;

  // Reset Payload
  memset(packet->manufactSpec.payload, 0, payload_length);

  // Copy payload to frame
  memcpy(packet->manufactSpec.payload, payload, payload_length);

  // Fill remaining space if it's necessary
  if (payload_length < SL_FLEX_BLE_PAYLOAD_LEN_MIN) {
    padding_size = SL_FLEX_BLE_PAYLOAD_LEN_MIN - payload_length;
    fill_buffer(&packet->manufactSpec.payload[payload_length], padding_size);
  }
}

/*******************************************************************************
 * Get BLE packet size
 ******************************************************************************/
sl_flex_ble_packet_size_t sl_flex_ble_get_packet_size(const sl_flex_ble_advertising_packet_t *packet)
{
  // Check arguments
  if (packet == NULL) {
    app_log_warning("sl_flex_ble_get_packet_size ERR: parameter\r\n");
    return 0;
  }
  return (sizeof(sl_flex_ble_advertising_packet_t) - SL_FLEX_BLE_PAYLOAD_LEN_MAX + sl_flex_ble_get_payload_len(packet));
}

/*******************************************************************************
 * Get BLE Packet from buffer
 ******************************************************************************/
void sl_flex_ble_copy_packet_from_buff(sl_flex_ble_advertising_packet_t *packet, const uint8_t *rx_data)
{
  // Check arguments
  if (packet == NULL || rx_data == NULL) {
    app_log_warning("sl_flex_ble_get_packet ERR: parameter\r\n");
    packet = NULL;
    return;
  }
  memcpy(packet, rx_data, sizeof(sl_flex_ble_advertising_packet_t));
}

/*******************************************************************************
 * Get BLE Packet pointer from data buffer
 ******************************************************************************/
sl_flex_ble_advertising_packet_t * sl_flex_ble_get_packet(uint8_t *data)
{
  return (sl_flex_ble_advertising_packet_t *) data;
}

/*******************************************************************************
 * Copy BLE packet payload
 ******************************************************************************/
void sl_flex_ble_copy_payload(sl_flex_ble_advertising_packet_t *packet, uint8_t *dest, const sl_flex_ble_packet_size_t payload_length)
{
  // Check arguments
  if (packet == NULL || dest == NULL || payload_length == 0) {
    app_log_warning("sl_flex_ble_copy_payload ERR: parameter\r\n");
    return;
  }
  memcpy(dest, packet->manufactSpec.payload, (payload_length > SL_FLEX_BLE_PAYLOAD_LEN_MAX) ? SL_FLEX_BLE_PAYLOAD_LEN_MAX : payload_length);
}

/*******************************************************************************
 * Get BLE payload
 ******************************************************************************/
uint8_t *sl_flex_ble_get_payload(sl_flex_ble_advertising_packet_t *packet)
{
  if (packet == NULL) {
    return NULL;
  }
  return packet->manufactSpec.payload;
}

/*******************************************************************************
 * Get BLE payload len
 ******************************************************************************/
inline sl_flex_ble_packet_size_t sl_flex_ble_get_payload_len(const sl_flex_ble_advertising_packet_t *packet)
{
  if (packet == NULL) {
    return 0;
  }
  return calc_ble_payload_length(packet->header.length);
}

/*******************************************************************************
 * Fill buffer with a template (0xAA, 0x55)
 ******************************************************************************/
static void fill_buffer(uint8_t *buffer, uint32_t length)
{
  bool parity = (length % 2) ? false : true;
  for (uint32_t i = 0; i < length; ++i) {
    buffer[i] = (parity) ? (0xAAU) : (0x55U);
    parity = !parity;
  }
}
