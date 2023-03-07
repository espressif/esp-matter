/**
 * @file
 * Defines a platform abstraction layer for the Z-Wave radio.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef ZPAL_RADIO_H_
#define ZPAL_RADIO_H_

#include <stdbool.h>
#include <stdint.h>
#include "zpal_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup zpal
 * @brief Z-Wave Platform Abstraction Layer.
 * @{
 * @addtogroup zpal-radio
 * @brief Defines a platform abstraction layer for the Z-Wave radio.
 *
 * The ZPAL Radio module contains several APIs which are required
 * to be implemented.
 *
 * A user of this radio module shall first define all parameters
 * of the Z-Wave Radio Profile (i.e., zpal_radio_profile_t) and
 * initialise the radio using zpal_radio_init API.
 *
 * After the initialisation of the radio is executed, the user can use any
 * of the radio APIs to execute radio related paradigm for instance
 * - zpal_radio_transmit shall be used to transmit a Z-Wave frame on radio
 * - zpal_radio_start_receive shall be used to enable the reception of Z-Wave frame
 * 
 * The radio API assumes that the radio will return to receive mode with channel 
 * hopping enabled after transmitting a frame.
 * 
 * Initialization of the radio
 * ---------------------------
 * 
 * void
 * RXHandlerFromISR(zpal_radio_event_t rxStatus)
 * {
 *    // Rx handle in ISR context
 * }
 * 
 * void
 * TXHandlerFromISR(zpal_radio_event_t txStatus)
 * {
 *   // Tx complete handle in ISR context
 * }
 * 
 * void
 * RegionChangeHandler(zpal_radio_event_t regionChangeStatus)
 * {
 *   // Region changed, make sure region specific data and statistics are cleared
 * }
 * 
 * void
 * RadioAssertHandler(zpal_radio_event_t assertVal)
 * {
 *   // Radio driver or hardware asserted, handle it
 * }
 * 
 * initialize_radio()
 * {
 *   static zpal_radio_profile_t RfProfile;
 *   static zpal_radio_network_stats_t sNetworkStatistic = {0};
 *   static uint8_t network_homeid[4] = {0xDE, 0xAD, 0xBE, 0xEF};
 *   
 *   // Set radio for US always on mode
 *   static zpal_radio_profile_t RfProfile = {.region = REGION_US,
 *                                     .wakeup = ZPAL_RADIO_WAKEUP_ALWAYS_LISTEN,
 *                                     .listen_before_talk_threshold = ELISTENBEFORETALKTRESHOLD_DEFAULT,
 *                                     .tx_power_max = 0,
 *                                     .tx_power_adjust = 33,
 *                                     .tx_power_max_lr = 140,
 *                                     .home_id = &network_homeid,
 *                                     .rx_cb = RXHandlerFromISR,
 *                                     .tx_cb = TXHandlerFromISR,
 *                                     .region_change_cb = RegionChangeHandler,
 *                                     .assert_cb = RadioAssertHandler,
 *                                     .network_stats = &sNetworkStatistic,
 *                                     .radio_debug_enable = false,
 *                                     .primary_lr_channel = ZPAL_RADIO_LR_CHANNEL_A};
 *   
 *   zpal_radio_init(RfProfile);
 * }
 * 
 * 
 * Transmitting a frame
 * --------------------
 * 
 * #define TX_FAILED          0
 * #define TX_FRAME_SUCCESS   1
 * #define TX_BEAM_SUCCESS    2
 * 
 * static const zpal_radio_transmit_parameter_t TxParameter100kCh1 = {.speed = ZPAL_RADIO_SPEED_100K, 
 *                                                                    .channel_id = 0, 
 *                                                                    .crc = ZPAL_RADIO_CRC_16_BIT_CCITT,
 *                                                                    .preamble = 0x55, 
 *                                                                    .preamble_length = 40, 
 *                                                                    .start_of_frame = 0xF0, 
 *                                                                    .repeats = 0};
 * 
 * zpal_status_t transmit_frame()
 * {
 *   // Singlecast MAC header from node 1 to node 2
 *   uint8_t header_buffer[9] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x41, 0x01, 14, 0x02};
 *   // Basic set On frame
 *   uint8_t payload_buffer[3] = {0x20, 0x01, 0xFF};
 *   
 *   return zpal_radio_transmit(&TxParameter100kCh1,         // use 100kbps FSK profile
 *                              9,
 *                              (uint8_t *)&header_buffer,
 *                              3,
 *                              (uint8_t *)&payload_buffer,
 *                              true,                        // Do LBT before transmit
 *                              0);                          // Transmit power 0dBm
 * }
 * 
 * void
 * TXHandlerFromISR(zpal_radio_event_t txStatus)
 * {
 *   uint8_t status;
 *
 *   if ((ZPAL_RADIO_EVENT_TX_FAIL == txStatus) || (ZPAL_RADIO_EVENT_TX_FAIL_LBT == txStatus))
 *   {
 *     status = TX_FAILED;
 *   }
 *   else if (txStatus & ZPAL_RADIO_EVENT_FLAG_BEAM)
 *   {
 *     status = TX_BEAM_SUCCESS;
 *   }
 *   else
 *   {
 *     status = TX_FRAME_SUCCESS;
 *   }
 *   / Get out of ISR context and handle transmit complete
 *   HandleTransmitComplete(status);
 * }
 * 
 *
 * Receiving a frame
 * -----------------
 *
 * #define RX_ABORT                 0
 * #define RX_BEAM                  1
 * #define RX_FRAME                 2
 * #define RX_CALIBRATION_NEEDED    3
 *
 * void
 * RXHandlerFromISR(zpal_radio_event_t rxStatus)
 * {
 *   uint8_t status;
 *
 *   switch (rxStatus)
 *   {
 *     case ZPAL_RADIO_EVENT_RX_BEAM:
 *       // Beam received
 *       status = RX_BEAM;
 *       break;
 *     case ZPAL_RADIO_EVENT_RX_ABORT:
 *       // Receive was aborted due to an error in the frame
 *       status = RX_ABORT;
 *       break;
 *     case ZPAL_RADIO_EVENT_RX:
 *       // Valid frame received
 *       status = RX_FRAME;
 *       break;
 *     case ZPAL_RADIO_EVENT_RXTX_CALIBRATE:
 *       // Radio reports that calibration is needed
 *       status = RX_CALIBRATION_NEEDED;
 *       break;
 *   }
 *
 *   / Get out of ISR context and handle frame
 *   HandleFrame(status);
 * }
 * 
 * @{
 */
   
/**
 * @brief RSSI value is invalid or not measured
 */
#define ZPAL_RADIO_INVALID_RSSI_DBM (-128)
#define ZPAL_RADIO_RSSI_NOT_AVAILABLE (127)

//deci-dBm values
#define ZW_TX_POWER_10DBM  100
#define ZW_TX_POWER_14DBM  140
#define ZW_TX_POWER_20DBM  200

/**
 * @brief Node ID type.
 */
typedef uint16_t node_id_t;

/**
 * @brief Parameter type to store deci dBm values.
 */
typedef int16_t zpal_tx_power_t;

typedef enum
{
  ZPAL_RADIO_MODE_NON_LISTENING,    ///< The radio is not listening unless configured to for a period.
  ZPAL_RADIO_MODE_ALWAYS_LISTENING, ///< The radio is always listening.
  ZPAL_RADIO_MODE_FLIRS,            ///< The radio is frequently listening.
} zpal_radio_mode_t;

/**
 * @brief Wakeup interval for the radio. A FLiRS node will use 250 or 1000 ms interval, all other
 * nodes should be configured as always listening.
 */
typedef enum
{
  ZPAL_RADIO_WAKEUP_NEVER_LISTEN,  ///< Node is not listening (Only listen when application requests it).
  ZPAL_RADIO_WAKEUP_ALWAYS_LISTEN, ///< Node is always listening.
  ZPAL_RADIO_WAKEUP_EVERY_250ms,   ///< Node wakes up every 250 ms interval to listen for a wakeup beam.
  ZPAL_RADIO_WAKEUP_EVERY_1000ms,  ///< Node wakes up every 1000 ms interval to listen for a wakeup beam.
} zpal_radio_wakeup_t;

/**
 * @brief Enumeration containing supported checksum types in Z-Wave.
 */
typedef enum
{
  ZPAL_RADIO_CRC_NONE,         ///< No checksum.
  ZPAL_RADIO_CRC_8_BIT_XOR,    ///< 8 bit XOR checksum.
  ZPAL_RADIO_CRC_16_BIT_CCITT, ///< 16 bit CRC-CCITT checksum.
} zpal_radio_crc_t;

/**
 * @brief Enumeration containing supported baud rates.
 */
typedef enum
{
  ZPAL_RADIO_SPEED_UNDEFINED,
  ZPAL_RADIO_SPEED_9600,
  ZPAL_RADIO_SPEED_40K,
  ZPAL_RADIO_SPEED_100K,
  ZPAL_RADIO_SPEED_100KLR,
} zpal_radio_speed_t;

/**
 * @brief Enumeration containing Long Range Channels.
 */
typedef enum
{
  ZPAL_RADIO_LR_CHANNEL_UNINITIALIZED, ///< Long Range Channel setting not initialized.
  ZPAL_RADIO_LR_CHANNEL_A,             ///< Long Range Channel A.
  ZPAL_RADIO_LR_CHANNEL_B,             ///< Long Range Channel B.
  ZPAL_RADIO_LR_CHANNEL_UNKNOWN,       ///< Long Range Channel Unknown.
} zpal_radio_lr_channel_t;

/**
 * @brief Enumeration containing Z-Wave channels known as protocol modes.
 */
typedef enum
{
  ZPAL_RADIO_PROTOCOL_MODE_1,                ///< 2 Channel Protocol Mode.
  ZPAL_RADIO_PROTOCOL_MODE_2,                ///< 3 Channel Protocol Mode.
  ZPAL_RADIO_PROTOCOL_MODE_3,                ///< 4 Channel Protocol Mode - Combination of 2 Channel (9.6kb/40kb, 100kb) and Long Range Channel.
  ZPAL_RADIO_PROTOCOL_MODE_4,                ///< 2 Long Range Channel Protocol Mode - End Device.
  ZPAL_RADIO_PROTOCOL_MODE_UNDEFINED = 0xFF, ///< Protocol mode undefined means invalid region code.
} zpal_radio_protocol_mode_t;

/**
 * @brief Enumeration official Z-Wave regions.
 */
typedef enum
{
  REGION_EU = 0,                                          ///< Radio is located in Region EU. 2 Channel region.
  REGION_US,                                              ///< Radio is located in Region US. 2 Channel region.
  REGION_ANZ,                                             ///< Radio is located in Region Australia/New Zealand. 2 Channel region.
  REGION_HK,                                              ///< Radio is located in Region Hong Kong. 2 Channel region.
  REGION_IN = 5,                                          ///< Radio is located in Region India. 2 Channel region.
  REGION_IL,                                              ///< Radio is located in Region Israel. 2 Channel region.
  REGION_RU,                                              ///< Radio is located in Region Russia. 2 Channel region.
  REGION_CN,                                              ///< Radio is located in Region China. 2 Channel region.
  REGION_US_LR,                                           ///< Radio is located in Region US. 2 Channel LR region.
  REGION_US_LR_BACKUP,                                    ///< Radio is located in Region US. 2 Channel LR Backup region.
  REGION_2CH_NUM = (REGION_US_LR_BACKUP - REGION_EU) + 1, ///< Number of 2 channel region values. For checking if value is out of range.
  REGION_JP = 32,                                         ///< Radio is located in Region Japan. 3 Channel region.
  REGION_KR,                                              ///< Radio is located in Region Korea. 3 Channel region.
  REGION_3CH_NUM = (REGION_KR - REGION_JP) + 1,           ///< Number of 3 channel region values. For checking if value is out of range.
  REGION_US_LR_END_DEVICE = 48,                           ///< Radio is located in Region US Long Range End Device. 2 Long Range Channel Region.
  REGION_LR_END_DEVICE_NUM = 1,
  REGION_UNDEFINED = 0xFE,
  REGION_DEFAULT = 0xFF, ///< Radio is located in Library Default Region EU. 2 Channel region.
} zpal_radio_region_t;

/**
 * @brief Enumeration test frequency region. NOTE these regions should only be used for test purposes
 */
typedef enum
{
  TF_REGION_866 = 64,
  TF_REGION_870, 
  TF_REGION_906, 
  TF_REGION_910,
  TF_REGION_878,
  TF_REGION_882,
  TF_REGION_886, 
  TF_REGION_2CH_NUM = (TF_REGION_886 - TF_REGION_866) + 1,
  TF_REGION_932_3CH = 96,
  TF_REGION_940_3CH,
  TF_REGION_835_3CH,
  TF_REGION_840_3CH,
  TF_REGION_850_3CH, 
  TF_REGION_3CH_NUM = (TF_REGION_850_3CH - TF_REGION_932_3CH) + 1,
} zpal_radio_region_tf_t;

/**
 * @brief Enumeration containing Tx power settings.
 */
typedef enum
{
  ZPAL_RADIO_TX_POWER_DEFAULT, ///< Max Tx power.
  ZPAL_RADIO_TX_POWER_MINUS1_DBM,
  ZPAL_RADIO_TX_POWER_MINUS2_DBM,
  ZPAL_RADIO_TX_POWER_MINUS3_DBM,
  ZPAL_RADIO_TX_POWER_MINUS4_DBM,
  ZPAL_RADIO_TX_POWER_MINUS5_DBM,
  ZPAL_RADIO_TX_POWER_MINUS6_DBM,
  ZPAL_RADIO_TX_POWER_MINUS7_DBM,
  ZPAL_RADIO_TX_POWER_MINUS8_DBM,
  ZPAL_RADIO_TX_POWER_MINUS9_DBM,
  ZPAL_RADIO_TX_POWER_UNINITIALIZED = 126,
  ZPAL_RADIO_TX_POWER_REDUCED = 127, ///< Default reduced Tx power.
} zpal_radio_tx_power_t;

/**
 * @brief Parameters required when transmitting a frame.
 */
typedef struct
{
  zpal_radio_speed_t speed; ///< Channel Speed to use when transmitting a frame.
  uint8_t channel_id;       ///< Channel id to use when transmitting a frame.
  zpal_radio_crc_t crc;     ///< CRC Type to use. XOR is used in 2 channel at 9600 and 40k baud rate. CRC CCITT is used in 100k frames.
  uint8_t preamble;         ///< The byte value used for the preamble sequence.
  uint8_t preamble_length;  ///< Length of the preamble. Minimum preamble length is specified in ITU G.9959-2015.
  uint8_t start_of_frame;   ///< The start of frame byte used to indicate the end of preamble and the start of frame.
  uint8_t repeats;          ///< Number of repetitions for the frame. This is used for wakeup beams where the beam is to be repeated for 250 or 1000 ms.
} zpal_radio_transmit_parameter_t;

/**
 * @brief Z-Wave Frame header format types.
 */
typedef enum
{
  ZPAL_RADIO_HEADER_TYPE_2CH,       ///< 2 Channel header format.
  ZPAL_RADIO_HEADER_TYPE_3CH,       ///< 3 Channel header format.
  ZPAL_RADIO_HEADER_TYPE_LR,        ///< LR Channel header format.
  ZPAL_RADIO_HEADER_TYPE_UNDEFINED, ///< Undefined Channel header format.
} zpal_radio_header_type_t;

/**
 * @brief Structure with radio parameters for a received frame.
 */
typedef struct
{
  zpal_radio_speed_t speed;                       ///< Speed for the frame received.
  uint8_t channel_id;                             ///< Channel id on which the frame was received.
  zpal_radio_header_type_t channel_header_format; ///< Z-Wave Header format used in channel frame was received on.
  int8_t rssi;                                    ///< Rssi value.
} zpal_radio_rx_parameters_t;

/**
 * @brief Enumeratio radio events.
 */
typedef enum
{
  ZPAL_RADIO_EVENT_IDLE,
  ZPAL_RADIO_EVENT_RX,                          ///< Frame received
  ZPAL_RADIO_EVENT_TX,                          ///< Transmit complete
  ZPAL_RADIO_EVENT_RX_FLIRS,
  ZPAL_RADIO_EVENT_RX_BEAM,                     ///< Beam received
  ZPAL_RADIO_EVENT_RX_ABORT,                    ///< Receive was aborted
  ZPAL_RADIO_EVENT_TX_FAIL,                     ///< Transmit failed
  ZPAL_RADIO_EVENT_TX_FAIL_LBT,                 ///< Transmit failed because of an LBT failure
  ZPAL_RADIO_EVENT_RXTX_CALIBRATE,			    ///< Radio needs calibration 
  ZPAL_RADIO_EVENT_MASK = 0x1F,
  ZPAL_RADIO_EVENT_FLAG_PACKET = 0x20,          ///< The frmae was a normal package
  ZPAL_RADIO_EVENT_FLAG_BEAM = 0x40,            ///< The frame was a wakeup beam
} zpal_radio_event_t;

typedef void (*zpal_radio_callback_t)(zpal_radio_event_t event);

/**
 * @brief Z-Wave receive frame.
 */
typedef struct
{
  uint8_t frame_content_length; ///< Length of payload following this frame.
  uint8_t frame_content[];      ///< Array with complete frame data received.
} zpal_radio_receive_frame_t;

/**
 * @brief Function pointer declaration for handling of received frames.
 *
 * @details When a frame is received this function will be invoked in order to process the frame.
 *
 * @param[in] rx_parameters Pointer to the structure with channel and rssi values.
 * @param[in] frame         Pointer to the received frame. The frame is expected to be located in
 *                          Z-Wave stack reserved memory and allocated throughout lifetime of stack
 *                          processing. Application should copy payload data if required for
 *                          unsynchronized data processing.
 *
 */
typedef void (*zpal_radio_receive_handler_t)(zpal_radio_rx_parameters_t *rx_parameters, zpal_radio_receive_frame_t *frame);

/**
 * @brief Network statistics structure
 */
typedef struct
{
  uint32_t tx_frames;          ///< Transmitted Frames.
  uint32_t tx_lbt_back_offs;   ///< LBT backoffs.
  uint32_t rx_frames;          ///< Received Frames (No errors).
  uint32_t rx_lrc_errors;      ///< Checksum Errors.
  uint32_t rx_crc_errors;      ///< CRC16 Errors.
  uint32_t rx_foreign_home_id; ///< Foreign Home ID.
  uint32_t tx_time_channel_0;  ///< Accumulated transmission time in ms for channel 0.
  uint32_t tx_time_channel_1;  ///< Accumulated transmission time in ms for channel 1.
  uint32_t tx_time_channel_2;  ///< Accumulated transmission time in ms for channel 2.
  uint32_t tx_time_channel_3;  ///< Accumulated transmission time in ms for channel 3, i.e., US_LR1.
  uint32_t tx_time_channel_4;  ///< Accumulated transmission time in ms for channel 4, i.e., US_LR2.
} zpal_radio_network_stats_t;

/**
 * @brief Radio Profile containing region, baud rate, and wakeup interval for this device.
 */
typedef struct
{
  zpal_radio_region_t region;                      ///< Region in which this system operates.
  zpal_radio_wakeup_t wakeup;                      ///< Wakeup interval for the radio.
  uint8_t primary_lr_channel;                      ///< Primary Long Range Channel.
  int8_t listen_before_talk_threshold;             ///< LBT Threshold for Transmit backoff in dBm.
  zpal_tx_power_t tx_power_max;                    ///< Z-Wave Transmit Power in deci dBm.
  zpal_tx_power_t tx_power_adjust;                 ///< Adjustment for antenna gain in deci dBm.
  zpal_tx_power_t tx_power_max_lr;                 ///< Max transmit power for Z-Wave LR in deci dBm.
  uint8_t *home_id;                                ///< Pointer to current HomeID(uint8_t homeID[4]).
  zpal_radio_callback_t rx_cb;                     ///< Pointer to function called by RF on Rx Completion.
  zpal_radio_callback_t tx_cb;                     ///< Pointer to function called by RF on Tx Completion.
  zpal_radio_callback_t region_change_cb;          ///< Pointer to function called by RF on Region change.
  zpal_radio_callback_t assert_cb;                 ///< Pointer to function called by RF on fatal Assert.
  zpal_radio_network_stats_t *network_stats;       ///< Pointer to structure where to RF Statistics are placed.
  uint8_t radio_debug_enable;                      ///< Enable radio debugging which is vendor specific.
  zpal_radio_receive_handler_t receive_handler_cb; ///< Pointer to receive handler.
} zpal_radio_profile_t;

/**
 * @brief Sets the home ID, the node ID and the radio mode.
 *
 * @param[in] home_id         Network home ID.
 * @param[in] node_id         Network node ID.
 * @param[in] mode            Radio mode: listening, non-listening or FLiRS.
 * @param[in] home_id_hash    Radio mode: listening, non-listening or FLiRS.
 */
void zpal_radio_set_network_ids(uint32_t home_id, node_id_t node_id, zpal_radio_mode_t mode, uint8_t home_id_hash);

/**
 * @brief Initializes the radio.
 *
 * @param[in] @p profile Pointer to the profile with information to configure the radio.
 */
void zpal_radio_init(zpal_radio_profile_t * const profile);

/**
 * @brief Function for changing REGION runtime.
 *
 * @param[in] region Region to change to.
 * @return @ref ZPAL_STATUS_OK if the region was successfully changed and @ref ZPAL_STATUS_FAIL otherwise.
 */
zpal_status_t zpal_radio_change_region(zpal_radio_region_t region);

/**
 * @brief Function for getting REGION runtime.
 *
 * @return Current region.
 */
zpal_radio_region_t zpal_radio_get_region(void);

/**
 * @brief Function for transmitting a Z-Wave frame though the radio.
 *
 * @param[in] tx_parameters         Parameter setting specifying speed, channel, wakeup, crc for transmission.
 * @param[in] frame_header_length   Length of frame header data to transmit.
 * @param[in] frame_header_buffer   Pointer to data array containing the frame header.
 * @param[in] frame_payload_length  Length of frame payload data to transmit.
 * @param[in] frame_payload_buffer  Pointer to data array containing the frame payload.
 * @param[in] use_lbt               if set to 1, LBT will be done prior radioTransmit.
 * @param[in] tx_power              The RF tx power to use for transmitting in dBm.
 * @return @ref ZPAL_STATUS_OK if the data was successfully transmit, @ref ZPAL_STATUS_BUFFER_FULL when queue is full.
 */
zpal_status_t zpal_radio_transmit(zpal_radio_transmit_parameter_t const *const tx_parameters,
                                  uint8_t frame_header_length,
                                  uint8_t const *const frame_header_buffer,
                                  uint8_t frame_payload_length,
                                  uint8_t const *const frame_payload_buffer,
                                  uint8_t use_lbt,
                                  int8_t tx_power);

/**
 * @brief Function for transmitting a Z-Wave Beam frame though the radio.
 *
 * @param[in] tx_parameters Parameter setting specifying speed, channel, wakeup.
 * @param[in] beam_data_len Length of the Beam data to transmit.
 * @param[in] beam_data     Pointer to data array containing the BEAM data.
 * @param[in] tx_power      The RF tx power to use for transmitting in dbm.
 * @return @ref ZPAL_STATUS_OK if the data was successfully transmit, @ref ZPAL_STATUS_BUFFER_FULL when queue is full.
 */
zpal_status_t zpal_radio_transmit_beam(zpal_radio_transmit_parameter_t const *const tx_parameters,
                                       uint8_t beam_data_len,
                                       uint8_t const *const beam_data,
                                       int8_t tx_power);

/**
 * @brief Starts the receiver and enables reception of frames.
 * If the receiver is already started, nothing will happen.
 */
void zpal_radio_start_receive(void);

/**
 * @brief Function to get last received frame.
 * If a frame is received, @ref zpal_radio_receive_handler_t will be invoked.
 */
void zpal_radio_get_last_received_frame(void);

/**
 * @brief Powers down the radio transceiver.
 */
void zpal_radio_power_down(void);

/**
 * @brief Function to get the protocol mode used in the configured region.
 *
 * @return Protocol mode used in the configured region.
 */
zpal_radio_protocol_mode_t zpal_radio_get_protocol_mode(void);

/**
 * @brief Get last beam channel.
 * Retrieve the channel on which we have last received a beam on.
 *
 * @return Last beam channel.
 */
uint8_t zpal_radio_get_last_beam_channel(void);

/**
 * @brief Get last beam RSSI.
 * Retrieve the RSSI of the last beam received.
 *
 * @return Last beam rssi.
 */
int8_t zpal_radio_get_last_beam_rssi(void);

/**
 * @brief Function for setting the LBT RSSI level.
 *
 * @param[in] channel  uint8_t channel to set LBT threshold for.
 * @param[in] level    int8_t LBT RSSI level in dBm.
 */
void zpal_radio_set_lbt_level(uint8_t channel, int8_t level);

/**
 * @brief Enable or disables reception of broadcast beam.
 * Enable or disable FLiRS broadcast address.
 *
 * @param[in] enable  true to enable FLiRS broadcast address, false to disable.
 */
void zpal_radio_enable_rx_broadcast_beam(bool enable);

/**
 * @brief Function for clearing current Channel Transmit timers.
 */
void zpal_radio_clear_tx_timers(void);

/**
 * @brief Function for clearing current Network statistics.
 */
void zpal_radio_clear_network_stats(void);

/**
 * @brief Returns the background RSSI.
 *
 * @param[in]   channel   uint8_t channel Id for measurement.
 * @param[in]   force_rx  bool will force RAIL to RX mode so RSSI can be measured.
 * @param[out]  rssi      pointer to background RSSI.
 * @return @ref ZPAL_STATUS_OK if a valid RSSI value is available and read.
 *         @ref ZPAL_STATUS_FAIL if an RSSI value cannot be read. The value of *rssi is invalid.
 *         @ref ZPAL_STATUS_INVALID_ARGUMENT if rssi is NULL.
 */
zpal_status_t zpal_radio_get_background_rssi(uint8_t channel, bool force_rx, int8_t *rssi);

/**
 * @brief Function for getting the current reduce RF tx power compared to the default normal power in dBm.
 *
 * @return The current reduce RF TX power in dBm.
 */
uint8_t zpal_radio_get_reduce_tx_power(void);

/**
 * @brief Allows the radio to go into FLiRS receive mode.
 */
void zpal_radio_enable_flirs(void);

/**
 * @brief Returns whether FLiRS mode is enabled in the radio.
 *
 * @return True when FLiRS mode is enabled.
 */
bool zpal_radio_is_flirs_enabled(void);

/**
 * @brief Starts the receiver after power down.
 *
 * @param[in] wait_for_beam If set to true, the radio will listen for a beam. Otherwise, it will
 *                          listen normally.
 */
void zpal_radio_start_receive_after_power_down(bool wait_for_beam);

/**
 * @brief Resets the radio configuration to receive mode after having received a beam.
 *
 * @param[in] start_receiver If set to true, the receiver will start listening. Otherwise, it will
 *                           stay inactive.
 */
void zpal_radio_reset_after_beam_receive(bool start_receiver);

/**
 * @brief Returns whether use of fragmented beams is enabled or not for the active region.
 *
 * @return True if use of fragmented beams is enabled, false otherwise.
 */
bool zpal_radio_is_fragmented_beam_enabled(void);

/**
 * @brief Returns whether use of receive beams is enabled or not for the active region.
 *
 * @return True if use of fragmented beams is enabled, false otherwise.
 */
bool zpal_radio_is_receive_beam_enabled(void);

/**
 * @brief Calibrates the radio.
 * Z-Wave expects the radio ZPAL implementation to generate a @ref ZPAL_RADIO_EVENT_RXTX_CALIBRATE event when
 * calibration is required. The event will invoke this function in non-interrupt context.
 */
void zpal_radio_calibrate(void);

/**
 * @brief Returns whether listen before talk (LBT) is enabled.
 *
 * @return True if LBT is enabled, false otherwise.
 */
bool zpal_radio_is_lbt_enabled(void);

/**
 * @brief Returns the node ID associated with most recently received beam frame.
 *
 * @return Node ID associated with the most recently received beam frame.
 */
node_id_t zpal_radio_get_beam_node_id(void);

/**
 * @brief Returns the minimum transmit power for Z-Wave Long Range.
 *
 * @return Minimum TX power in dBm.
 */
zpal_tx_power_t zpal_radio_get_minimum_lr_tx_power(void);

/**
 * @brief Returns the maximum transmit power for Z-Wave Long Range.
 *
 * @return Maximum TX power in dBm.
 */
zpal_tx_power_t zpal_radio_get_maximum_lr_tx_power(void);

/**
 * @brief Returns whether debug is enabled or disabled.
 *
 * @return True if debug is enabled, false otherwise.
 */
bool zpal_radio_is_debug_enabled(void);

/**
 * @brief Function to read current Primary Long Range Channel.
 *
 * @return @ref ZPAL_RADIO_LR_CHANNEL_A or @ref ZPAL_RADIO_LR_CHANNEL_B.
 */
zpal_radio_lr_channel_t zpal_radio_get_primary_long_range_channel(void);

/**
 * @brief Function to set the Primary Long Range Channel.
 *
 * @param[in] channel  @ref zpal_radio_lr_channel_t Long Range Channel to
 *                     set as Primary Long Range Channel.
 * @return Region needed for using the specified Primary Long Range Channel.
 */
zpal_radio_region_t zpal_radio_set_primary_long_range_channel(zpal_radio_lr_channel_t channel);

/**
 * @brief Function to the set Long Range channel Locked status.
 *
 * @param[in] lock Long Range channel Locked status.
 */
void zpal_radio_set_long_range_lock(bool lock);

/**
 * @brief Function to read the Long Range channel Locked status.
 *
 * @return True if node shall use Long Range channel only.
 */
bool zpal_radio_is_long_range_locked(void);

/**
 * @brief Read the saved tx power of the last received long-range beam.
 *
 * @return The tx power of the last received long-range beam.
 */
int8_t zpal_radio_get_flirs_beam_tx_power(void);

/**
 * @brief Check if transmission is allowed for specified channel.
 *
 * @param[in] channel         The channel to check.
 * @param[in] frame_length    The length of the frame to send.
 * @param[in] frame_priority  The Tx priority of the frame.
 * @return True if node shall use Long Range channel only.
 *
 * @note  In Japan all communication must comply to a max 100ms transmit
 *        followed by min 100ms silence period.
 * @note  Cause a channel shift.
 */
bool zpal_radio_is_transmit_allowed(uint8_t channel, uint8_t frame_length, uint8_t frame_priority);

/**
 * @brief Function to reduce Tx power of classic non-listening devices.
 *
 * @param[in] adjust_tx_power  Reduces the devices default Tx power in dB. Valid range: 0-9 dB.
 *
 * @return  True when reduction is allowed, false for listening devices and out of range input.
 */
bool zpal_radio_attenuate(uint8_t adjust_tx_power);

/**
 * @brief Function to radio calibration.
 *
 * @param[in] forced  If true, radio calibration is performed regardless if it is required.
 *                    If false, radio calibration is performed only if it is required.
 */
void zpal_radio_request_calibration(bool forced);

/**
 * @} //zpal-radio
 * @} //zpal
 */

#ifdef __cplusplus
}
#endif

#endif /* ZPAL_RADIO_H_ */
