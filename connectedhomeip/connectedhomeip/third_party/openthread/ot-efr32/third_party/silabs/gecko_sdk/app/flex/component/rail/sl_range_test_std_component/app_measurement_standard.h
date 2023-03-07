/***************************************************************************//**
 * @file
 * @brief app_measurement_standard.h
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
#ifndef APP_MEASUREMENT_STANDARD_H
#define APP_MEASUREMENT_STANDARD_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_measurement.h"
#include "rail.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// BLE macros
#define BLE_PHYSICAL_CH    (0U)
#define BLE_LOGICAL_CH     (37U)
#define BLE_CRC_INIT       (0x555555)
#define BLE_ACCESS_ADDRESS (0x12345678) //0x8E89BED6
#define ADV_NONCONN_IND    (0x02)
#define BLE_PDU_TYPE       (ADV_NONCONN_IND)
#define BLE_HEADER_LSB     (BLE_PDU_TYPE)      // RFU(0)|ChSel(0)|TxAdd(0)|RxAdd(0)
#define COMPANY_ID         ((uint16_t) 0x02FF) // Company Identifier of Silicon Labs
#define DISABLE_WHITENING  (false)
// ADV_NONVONN_IND::Adv Data (32 Bytes)
// - AD struct: Flags (3 Bytes)
// - AD struct: Manufacturer specific static part (10 Bytes):
// Length (1 Byte)
// Company ID (2 Bytes)
// Struct version (1 Byte)
// pktSent + destId + srcId + repeat (5 Bytes)
// Max remaining (0x55, 0xAA, 0x55, etc.) (19 Bytes)
#define BLE_PAYLOAD_LEN_MAX          (24U)
// AD Structure: Flags
#define ADSTRUCT_TYPE_FLAG           (0x01)
#define DISABLE_BR_EDR               (0x04)
#define LE_GENERAL_DISCOVERABLE_MODE (0x02)
// AD Structure: Manufacturer Specific
#define ADSTRUCT_TYPE_MANUFACTURER_SPECIFIC (0xFF)

/// IEEE 802.15.4 macros
#define IEEE802154_CHANNEL           (11U)    // 11 is the first 1st 2.4GHz channel
// Indicates the effective payload length excluding CRC (2 Bytes)
#define IEEE802154_PAYLOAD_LEN_MAX   (116U)   // 127bytes - 2bytes (CRC) - 9bytes (Data Frame::MHR)
#define IEEE802154_CRC_LENGTH        (2U)     // IEEE 802.15.4 CRC length is 2 bytes
#define IEEE802154_PHR_LENGTH        (1U)     // IEEE 802.15.4 PHR length is 1 byte
// Data Frame Format::Frame Control (2 bytes)
#define FRAME_TYPE                   (0x0001) // Data Frame
#define SECURITY_ENABLED             (0x0000) // Not enabled
#define FRAME_PENDING                (0x0000) // Not enabled
#define AR                           (0x0000) // Ack not required
#define PAN_ID_COMPRESSION           (0x0040) // Enabled
#define SEQUENCE_NUMBER_SUPPRESSION  (0x0000) // Sequence number is not suppressed
#define IE_PRESENT                   (0x0000) // IEs is not contained
#define DESTINATION_ADDRESSING_MODE  (0x0800) // Address field contains a short address (16 bit)
#define FRAME_VERSION                (0x2000) // IEEE Std 802.15.4
#define SOURCE_ADDRESSING_MODE       (0x8000) // Address field contains a short address (16 bit)
#define FRAME_CONTROL                (FRAME_TYPE | SECURITY_ENABLED | FRAME_PENDING           \
                                      | AR | PAN_ID_COMPRESSION | SEQUENCE_NUMBER_SUPPRESSION \
                                      | IE_PRESENT | DESTINATION_ADDRESSING_MODE | FRAME_VERSION | SOURCE_ADDRESSING_MODE)

/// BLE and IEEE 802.15.4 common macros
// Default payload length value
#define PAYLOAD_LEN_MIN   (sizeof(range_test_packet_t))

/// In case of Series 2 (RD4171A,BRD4180A and BRD4181A) channelSpacing can not be set
#define NO_CHANNEL_SPACING  (0U)
#define _2450_MHZ           ((uint32_t) 2450000000U)

#pragma pack(1)
/// IEEE 802.15.4 Data Frame:: MHR
typedef struct mhr_t{
  uint16_t frame_control; ///> Frame Control
  uint8_t  sequence_number;  ///> Sequence Number
  uint16_t destination_pan_id;    ///> Destination PAN ID
  uint16_t destination_address;     ///> Destination Address
  uint16_t source_address;      ///> Source Address
}mhr_t;

/// IEEE 802.15.4 Data Frame
typedef struct data_frame_format_t{
  mhr_t mhr; ///> Data Frame:: MHR
  range_test_packet_t payload; ///> Payload of the Data Frame
  uint8_t remainder[IEEE802154_PAYLOAD_LEN_MAX - PAYLOAD_LEN_MIN]; ///> Payload, filled with 0x55, 0xAA
} data_frame_format_t;

/// BLE Header
typedef struct {
  uint8_t type;
  uint8_t length;
} advertising_channel_pdu_header_t;

/// BLE flags
typedef struct {
  uint8_t length;
  uint8_t advertising_type;
  uint8_t flags;
} advertising_flags_t;

/// BLE manufacture specific parts
typedef struct {
  uint8_t length;
  uint8_t advertising_type;
  uint16_t company_id;
  uint8_t version;
  range_test_packet_t payload; ///> Payload of the Data Frame
  uint8_t  remainder[BLE_PAYLOAD_LEN_MAX - PAYLOAD_LEN_MIN]; ///> Remainder available in BLE advertisement PDU.
} advertising_manufacture_specific_t;

typedef struct {
  advertising_channel_pdu_header_t header;
  uint8_t advAddr[6];
  advertising_flags_t flags;
  advertising_manufacture_specific_t manufactSpec;
} advertising_nonconnectable_undirected_t;
#pragma pack()

/// Enumeration of the Predefined PHYs. Newly added predefined PHY shall be placed before _NUM_OF_PREDEFINED_PHYS
typedef enum {
  IEEE802154_250KBPS,    ///> IEEE80215.4, 250kbps
  IEEE802154_250KBPS_ANTDIV,    ///> IEEE80215.4, 250kbps with ANT DIV
  BLE_125KBPS,           ///> BLE 125kbps
  BLE_500KBPS,           ///> BLE 500kbps
  BLE_1MBPS,             ///> BLE 1Mbps
  BLE_2MBPS,             ///> BLE 2Mbps
  NUM_OF_PREDEFINED_PHYS
} predefined_phy_t;

/// Readability enum for protocols
typedef enum {
  PROT_BLE              = 0,
  PROT_IEEE802154       = 1,
  PROT_NO_OF_ELEMENTS
} range_test_protocols_t;

/// Structure for the standard phys for easier use
typedef struct {
  predefined_phy_t phy;
  range_test_protocols_t protocol;
  bool is_supported;
}range_test_std_phys_t;
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  The function resets payload length to the minimum value
 *         It prevents failures if the previously set payload length
 *         (of the previous PHY) is bigger then the maximum payload length
 *         of the actually set
 *
 * @param  None
 *
 * @returns None
 ******************************************************************************/
void reset_payload_length_for_standard(void);

/*******************************************************************************
 * @brief The function handles payload length overflow of BLE and IEEE802154
 *
 * @param None
 *
 * @returns None
 ******************************************************************************/
void handle_payload_length_for_standard(void);

/*******************************************************************************
* @brief  The function prepares a std. IEEE 802.15.4 Data frame format
* Data frame format
* - MHR:
*   - Frame Control (2 bytes)
*   - Sequence number (0/1)
*   - Addressing fields (variable)
*   - Auxiliary Security Header (variable)
*   - Header IEs (variable)
* - MAC Payload:
*   - Payload IEs (variable)
*   - Data Payload (variable)
* - MFR (2/4)
*
* @param packet_number: payload packet counter
* @param tx_buffer: this buffer contains the prepared std packet
*
* @returns None
*******************************************************************************/
void prepare_ieee802154_data_frame(uint16_t packet_number, uint8_t *tx_buffer);

/*******************************************************************************
* @brief The function prepares the following BLE PDU:
*         - ADV_NONCONN_IND
*         - LL advertiser address
*         - AD Structure: Flags
*         - AD Structure: Manufacturer specific
*           - Company ID
*           - Structure type; used for backward compatibility
*           - rangeTestPacket_t
*           - 0x55, 0xAA, 0x55, 0xAA... (only if payload length is more than sizeof(rangeTestPacket_t))
*
* @param packet_number: payload packet counter
* @param tx_buffer: this buffer contains the prepared BLE PDU packet
*
* @returns None
*******************************************************************************/
void prepare_ble_advertising_channel_pdu(uint16_t packet_number, uint8_t *tx_buffer);

/*******************************************************************************
 * @brief Points to the start of the payload in case of
 *        standard phy communication
 *
 * @param received_buffer: where the radio received the packet
 *
 * @returns payload: The start of the payload address
 ******************************************************************************/
range_test_packet_t* get_start_of_payload_for_standard(uint8_t* received_buffer);

/*******************************************************************************
 * @brief Return True if the Standards init function run successfully
 *
 * @param None
 *
 * @returns bool: True if the Standards init function run successfully
 ******************************************************************************/
bool is_init_ranget_test_standard_ready(void);

/*******************************************************************************
 * @brief Init block for the standard phys
 *
 * @param None
 *
 * @returns None
 ******************************************************************************/
void init_ranget_test_standard_phys(uint8_t* number_of_phys);

/*******************************************************************************
 * @brief Change the protocol on the BLE handler
 *
 * @param None
 *
 * @returns is_supported: true if the radio can use this protocol
 ******************************************************************************/
bool ble_protocol_change(void);

/*******************************************************************************
 * @brief Return the standard handler which is used currently
 *
 * @param None
 *
 * @returns RAIL_Handle_t: pointer for actual rail standard handler
 ******************************************************************************/
RAIL_Handle_t get_standard_rail_handler(void);

/*******************************************************************************
 * @brief  Returns the currently used standard phy, and its data
 *
 * @param None
 *
 * @returns range_test_std_phys_t
 ******************************************************************************/
range_test_std_phys_t get_current_std_phy(void);

/*******************************************************************************
 * @brief Checks if the phy or protocol is supported by the device
 *
 * @param None
 *
 * @returns is_supported: returns true if yes
 ******************************************************************************/
bool current_std_phy_is_supported(void);

/*******************************************************************************
 * @brief Checks if the phy or protocol is supported by the device
 *
 * @param index: of the phy
 *
 * @returns is_supported: returns true if yes
 ******************************************************************************/
bool std_phy_is_supported(uint8_t index);

/*******************************************************************************
 * @brief Helper function to check if phy of index is standard
 *
 * @param index: index of the phy
 *
 * @returns None
 ******************************************************************************/
bool is_phy_standard(uint8_t index);

/*******************************************************************************
 * @brief Print function for menu, also updates the channel settings
 *
 * @param print_buffer: where the phy name should be written
 *
 * @returns void
 ******************************************************************************/
void print_standard_name(char *print_buffer);

/*******************************************************************************
 * @brief Set the correct value for the standared phy channels
 *
 * @param None
 *
 * @returns None
 ******************************************************************************/
void set_standard_phy_channel(void);

/*******************************************************************************
 * @brief Set IEEE handler to IDLE state, useful as multiple handlers can
 *        occupy the radio same time and block each other at worst cases
 *
 * @param None
 *
 * @returns None
 ******************************************************************************/
void set_ieee_handler_to_idle(void);

/*******************************************************************************
 * @brief Set BLE handler to IDLE state, useful as multiply handlers can
 *        occupy the radio same time and block each other at worst cases
 *
 * @param None
 *
 * @returns None
 ******************************************************************************/
void set_ble_handler_to_idle(void);

/*******************************************************************************
 * @brief Helper function to check if BLE or IEEE is enabled for range test
 *
 * @param None
 *
 * @returns None
 ******************************************************************************/
bool standard_phy_are_present(void);

/*******************************************************************************
 * @brief Helper function to return the offsetted value for comparison
 *
 * @param None
 *
 * @returns uint8_t: the phy number of the current std minus the custom phys
 ******************************************************************************/
uint8_t current_phy_standard_value(void);

/*******************************************************************************
 * @brief Hides the configs from other parts of the application
 *
 * @param base_frequency: The current frequency of the radio
 * @param channel_spacing: The current channel spacing of the selected phy
 *
 * @returns None
 ******************************************************************************/
void get_rail_standard_config_data(uint32_t *base_frequency, uint32_t *channel_spacing);

/*******************************************************************************
 * @brief Hides the configs from other parts of the application
 *
 * @param start: minimum number of channels in phy
 * @param end: maximum number of channels in phy
 *
 * @returns None
 ******************************************************************************/
void get_rail_standard_channel_range(uint16_t *min, uint16_t *max);

/*******************************************************************************
 * @brief Generate a string for the BLE mobile APP from standared phys
 *
 * @param phy_index: which phy name is needed
 * @param buffer: where to write the caracters
 * @param length: the length of the whole written string
 *
 * @returns None
 ******************************************************************************/
void std_phy_list_generation(uint8_t phy_index, uint8_t *buffer, uint8_t *length);

/*******************************************************************************
 * @brief Hides the configs from other parts of the application
 *
 * @param payload_min: minimum number of channels in phy
 * @param payload_max: maximum number of channels in phy
 *
 * @returns None
 ******************************************************************************/
void get_rail_standard_payload_range(uint8_t *payload_min, uint8_t *payload_max);

/*******************************************************************************
 * @brief Checks if the current phy is BLE or IEEE
 *
 * @param None
 *
 * @returns bool: true if current phy is a BLE protocol phy
 ******************************************************************************/
bool is_current_phy_ble(void);

#endif  // APP_MEASUREMENT_STANDARD_H
