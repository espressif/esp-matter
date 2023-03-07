/***************************************************************************//**
 * @file
 * @brief
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

#ifndef LOWER_MAC_H
#define LOWER_MAC_H

#if (!defined(PHY_NULL) && !defined(ZIGBEE_STACK_ON_HOST)) && (defined(PHY_EM3XX) || defined(MAC_DUAL_PRESENT) || SINGLE_PHY_MULTIPAGE_SUPPORT || defined (UNIX) || defined (UNIX_SIMULATION) || defined(PHY_RAIL_LEGACY_PHY))
#include "phy/phy.h"
#else
#include "mac-phy.h"
#endif

#include "multi-mac.h"
#include "mac-multi-network.h"
#include "sl_status.h"

//------------------------------------------------------------------------------
// Merged from both zigbee/flex:

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum LowerMacState
#else
typedef uint8_t LowerMacState;
enum
#endif
{
  // Common states
  EMBER_MAC_STATE_IDLE,
  EMBER_MAC_STATE_EXPECTING_DATA,
  EMBER_MAC_STATE_TX,   // also pro2plus driver
  EMBER_MAC_STATE_TX_ACK_COMPLETE,

  // These apply to the pro2 driver and the RAIL driver
  EMBER_MAC_STATE_BUSY, // also scripted tests
  EMBER_MAC_STATE_TX_NO_ACK,
  EMBER_MAC_STATE_TX_WAITING_FOR_ACK,

  // These states are specific to the pro2 driver
  EMBER_MAC_STATE_SENDING_ACK,
  EMBER_MAC_STATE_CCA_FAILED,
  EMBER_MAC_STATE_BACKOFF,

  // These states are specific to RAIL
  EMBER_MAC_STATE_RADIO_INIT,

#ifdef CSL_SUPPORT
  // These states are specific to the CSL feature within the lower mac
  EMBER_MAC_STATE_WUF_RX, // a wake up frame has been received and we have scheduled an RX
#endif // CSL_SUPPORT
};

extern LowerMacState emLowerMacState;

// MAC TX Options Bitmask
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum sl_mac_tx_options_bitmask_t
#else
typedef uint8_t sl_mac_tx_options_bitmask_t;
enum
#endif
{
  EMBER_MAC_USE_NO_TX_OPTION  = 0x00, // 00000000
  EMBER_MAC_USE_CSMA          = 0x01, // 00000001
  EMBER_MAC_USE_WAKEUP_FRAMES = 0x02, // 00000010
};

bool sl_mac_lower_mac_is_idle(uint8_t mac_index);
void sl_mac_lower_mac_set_idle(uint8_t mac_index);
#ifdef CSL_SUPPORT
bool sl_mac_lower_mac_rx_wakeup_frame(uint8_t mac_index);
#endif // CSL_SUPPORT

void sl_mac_lower_mac_send(uint8_t mac_index, uint8_t *packet, sl_mac_tx_options_bitmask_t tx_options);
void sl_mac_packet_send_complete_callback(uint8_t mac_index, sl_status_t status);

sl_status_t sl_mac_receive_callback(uint8_t mac_index, PacketHeader rawPacket);

void sl_mac_lower_mac_tick(void);
void sl_mac_lower_mac_init(uint8_t mac_index, RadioPowerMode mode);
sl_status_t sl_mac_lower_mac_set_radio_channel(uint8_t mac_index, uint8_t channel);

sl_status_t sl_mac_lower_mac_set_radio_idle_mode(uint8_t mac_index, RadioPowerMode mode);
RadioPowerMode sl_mac_lower_mac_get_radio_idle_mode(uint8_t mac_index);

void sl_mac_purge_incoming_queue(uint8_t mac_index);
bool sl_mac_lower_mac_is_expecting_data(void);
bool sl_mac_lower_mac_radio_is_on(uint8_t mac_index);
void sl_mac_lower_mac_update_active_radio_config(void);

// Symbol timer APIs.
sl_status_t sl_mac_lower_mac_schedule_symbol_timer(uint8_t mac_index, uint16_t symbols);
void sl_mac_lower_mac_cancel_symbol_timer(uint8_t mac_index);

// This handler is invoked when the symbol timer expires.
void sl_mac_symbol_timer_expired_handler_isr(uint8_t mac_index);

//mfglib requirement
bool sl_mac_lower_mac_get_check_cca(uint8_t mac_index);

void sl_mac_lower_mac_disable_check_cca(uint8_t mac_index);

void sl_mac_lower_mac_enable_check_cca(uint8_t mac_index);

void sl_mac_lower_mac_start_transmit_stream(uint8_t mac_index);

void sl_mac_lower_mac_stop_transmit_stream(uint8_t mac_index);

void sl_mac_lower_mac_start_transmit_tone(uint8_t mac_index);

void sl_mac_lower_mac_stop_transmit_tone(uint8_t mac_index);

//zigbee specific, and only needed for mac certification
uint8_t sl_mac_lower_mac_convert_rssi_to_ed(uint8_t mac_index, int8_t rssi);

bool emRadioPacketTraceEnabled(void);

// This function will handle radio state for force sleep/wakeup calls
// to the device when component zigbee_force_sleep_and_wakeup is enabled
// sleep is TRUE for force-sleep and FALSE to undo force-sleep
void sli_mac_lower_mac_force_sleep(bool sleep);

//------------------------------------------------------------------------------
// zigbee only:

#define MAC_MAX_ACKD_RETRIES_DEFAULT (3)
#define TRANSMIT_BUFFER_SIZE 126

//------------------------------------------------------------------------------
// flex only:

// CSMA-CA default parameters
#define EMBER_MAC_MAX_CSMA_BACKOFF_DEFAULT                  4
#define EMBER_MAC_BACKOFF_EXPONENT_MIN_DEFAULT              3
#define EMBER_MAC_BACKOFF_EXPONENT_MAX_DEFAULT              5

#define DEFAULT_MAC_MAX_RETRIES                             3

#define FORCE_TX_AFTER_FAILED_CCA_ATTEMPTS_LIMIT            3

// The number of symbols forming the basic time period used by the CSMA-CA
// algorithm.
#define EMBER_MAC_UNIT_BACKOFF_PERIOD_SYMBOLS               20

// TODO: read the spec and set the proper DATA timeout
#define EMBER_MAC_DATA_REQUEST_TIMEOUT_MS                   100

// macSIFSPeriod is 12 symbols for all non-UWB PHYs
#define EMBER_MAC_ACK_TURNAROUND_DELAY_SYMBOLS              12

// Defines for the scheduler to estimate packet time on air
// (stolen from platform/base/phy/rail/phy.c)
//@TODO: This should be re-evaluated at some point, but for now this will help
// the scheduler make better choices
#define CSMA_OVERHEAD_US                                    1000

#define RADIO_SCHEDULER_BACKGROUND_RX_DEFAULT_PRIORITY      255
#define RADIO_SCHEDULER_ACTIVE_RX_DEFAULT_PRIORITY          255
#define RADIO_SCHEDULER_TX_DEFAULT_PRIORITY                 100

#define EMBER_APPENDED_INFO_FLAG_ACK_FRAME_PENDING_SET      0x01
// Appended info defines
#ifdef EMBER_STACK_CONNECT
#define EMBER_APPENDED_INFO_RSSI_OFFSET                     0
#define EMBER_APPENDED_INFO_RSSI_LENGTH                     1
#define EMBER_APPENDED_INFO_SYNC_TIME_OFFSET                1
#define EMBER_APPENDED_INFO_SYNC_TIME_LENGTH                4
#define EMBER_APPENDED_INFO_FLAGS_OFFSET                    5
#define EMBER_APPENDED_INFO_FLAGS_LENGTH                    1
#define EMBER_APPENDED_INFO_TOTAL_LENGTH  \
  (EMBER_APPENDED_INFO_RSSI_LENGTH        \
   + EMBER_APPENDED_INFO_SYNC_TIME_LENGTH \
   + EMBER_APPENDED_INFO_FLAGS_LENGTH)
#else
  #define EMBER_APPENDED_INFO_TOTAL_LENGTH 8
  #define APPENDED_INFO_FRAME_STATUS_BYTE_1_INDEX 0 // MSByte
  #define APPENDED_INFO_FRAME_STATUS_BYTE_0_INDEX 1 // LSByte
  #define APPENDED_INFO_RSSI_BYTE_INDEX           2
  #define APPENDED_INFO_LQI_BYTE_INDEX            3
  #define APPENDED_INFO_GAIN_BYTE_INDEX           4
  #define APPENDED_INFO_MAC_TIMER_BYTE_2_INDEX    5 // MSByte
  #define APPENDED_INFO_MAC_TIMER_BYTE_1_INDEX    6
  #define APPENDED_INFO_MAC_TIMER_BYTE_0_INDEX    7 // LSByte
#define APPENDED_INFO_CHANNEL_OVERWRITE_BYTE_INDEX 0
// Multi-network: we overwrite the second byte of the frame status with the
// network index. This will make the stack aware at higher levels of the network
// on which the packet was received.
#define APPENDED_INFO_NETWORK_OVERWRITE_BYTE_INDEX 1
#define APPENDED_INFO_NETWORK_OVERWRITE_NET_ID_MASK 0x0F
#define APPENDED_INFO_NETWORK_OVERWRITE_NET_ID_SHIFT 0
#define APPENDED_INFO_NETWORK_OVERWRITE_PHY_ID_MASK 0xF0
#define APPENDED_INFO_NETWORK_OVERWRITE_PHY_ID_SHIFT 4

#endif

extern uint8_t emberMacAckTimeoutMS;
extern int8_t emRadioCcaThreshold;

typedef struct {
  uint8_t maxCcaAttempts;
  uint8_t minBackoffExp;
  uint8_t maxBackoffExp;
  uint8_t maxRetries;
  bool checkCca;
} sl_mac_csma_parameters_t;

//------------------------------------------------------------------------------
// Flex: Internal API prototypes

bool sl_mac_incoming_pending(uint8_t mac_index);
bool sl_mac_activity_pending(void);

void sl_mac_timer_isr(void);

void sl_mac_set_csma_params(sl_mac_csma_parameters_t *csmaParams);
void sl_mac_get_csma_params(sl_mac_csma_parameters_t *csmaParams);

// Returns the current packet RSSI.
// This API should be called only in the context of processing an incoming
// packet.
int8_t sl_mac_get_current_packet_rssi(void);

// This API looks at the appended info of the current incoming packet and
// returns the elapsed time in microseconds since the sync word was received.
// This API should be called only in the context of processing an incoming
// packet.
uint32_t sl_mac_get_current_packet_sync_word_detection_elapsed_time_us(void);

// Returns the current packet appended flags.
// This API should be called only in the context of processing an incoming
// packet.
uint8_t sl_mac_get_current_packet_appended_flags(void);

void sl_mac_set_aux_address_filtering(EmberNodeId nodeId, uint8_t index);

// nwk radio address params
void sl_mac_lower_mac_set_node_id(uint8_t mac_index, uint8_t addr_filter_index, EmberNodeId nodeId);
void sl_mac_lower_mac_set_pan_id(uint8_t mac_index, uint8_t addr_filter_index, EmberPanId panId);
void sl_mac_lower_mac_set_eui64(uint8_t mac_index, uint8_t addr_filter_index, EmberEUI64 eui64);

// Radio/PHY related APIs.
sl_status_t sl_mac_lower_mac_set_radio_power(uint8_t mac_index, int8_t power);
int8_t sl_mac_lower_mac_get_radio_power(uint8_t mac_index);
uint8_t sl_mac_lower_mac_get_radio_channel(uint8_t mac_index);
void sl_mac_radio_ok_to_idle(void);
void sl_mac_set_coordinator(bool isCoordinator);
void sl_mac_set_cca_threshold(int8_t threshold);
int8_t sl_mac_get_ed_cca_threshold(void);
uint16_t sl_mac_get_symbol_duration_us(void);
uint16_t sl_mac_get_bit_duration_us(void);
int8_t sl_mac_radio_energy_detection(uint8_t mac_index);
void sl_mac_calibrate_current_channel(void);
void sl_mac_lower_mac_radio_sleep(void);
void sl_mac_lower_mac_radio_wakeup(void);
uint32_t sl_mac_get_mac_timer(void);
bool sl_mac_lower_mac_channel_is_valid(uint8_t mac_index, uint8_t mac_page_chan);

//------------------------------------------------------------------------------
// New APIs created after the zigbee/flex merge

sl_status_t sl_mac_set_cca_mode(uint8_t ccaMode);

// CSL related structs and APIs
typedef struct {
  uint16_t frame_type : 3;
  uint16_t long_frame_control : 1;
  uint16_t dest_addr_mode : 2;
  uint16_t src_addr_mode : 2;
  uint16_t pan_id_present : 1;
  uint16_t security_enabled : 1;
  uint16_t seq_num_suppression : 1;
  uint16_t frame_pending : 1;
  uint16_t frame_version : 2;
  uint16_t ack_request : 1;
  uint16_t ie_present : 1;
} sl_mac_long_frame_control_t;

typedef struct {
  uint16_t length : 7;
  uint16_t element_id : 8;
  uint16_t type : 1;
} sl_mac_header_ie_t;

typedef struct {
  uint16_t rendezvous_time;
  uint16_t wakeup_interval;
} sl_mac_rendezvous_ie_t;

sl_status_t sl_mac_set_mac_csl_max_period(uint8_t mac_index, uint16_t mac_csl_max_period_ms);
uint16_t sl_mac_get_mac_csl_max_period(uint8_t mac_index);
sl_status_t sl_mac_set_mac_csl_period(uint8_t mac_index, uint16_t mac_csl_period_ms);
uint16_t sl_mac_get_mac_csl_period(uint8_t mac_index);
sl_status_t sl_mac_set_mac_csl_channel_sample(uint8_t mac_index, uint16_t mac_csl_channel_sample_ms);
uint16_t sl_mac_get_mac_csl_channel_sample(uint8_t mac_index);
void sli_mac_get_eui64(EmberEUI64 eui64);
// 802.15.4 software filtering APIs (see lower-mac-802.15.4-filtering.c)

// Return true if the packet is accepted, false if the packet is filtered.
bool sl_mac_802154_filter(uint8_t *packet);

#define RADIO_CCA_ATTEMPT_MAX_DEFAULT      5 // 1 + macMaxCSMAAttempts' retries
#define RADIO_BACKOFF_EXPONENT_MIN_DEFAULT 3
#define RADIO_BACKOFF_EXPONENT_MAX_DEFAULT 5
#define RADIO_MINIMUM_BACKOFF_DEFAULT      0
#ifndef EMBER_STACK_CONNECT // Causes a warning on Connect
//workraound
//#undef  MAX_RADIO_POWER
//#define MAX_RADIO_POWER 3
#endif

#ifdef EMBER_STACK_CONNECT
#define EVENT_CONTROL_SYSTEM
#endif

#ifndef EVENT_CONTROL_SYSTEM
#define sli_mac_inactivate_event(x) emberEventSetInactive(&(x))
#define sli_mac_activate_event(x) emberEventSetActive(&(x))
#define sli_mac_set_event_delay_ms(x, y) emberEventSetDelayMs(&(x), y)
#define sli_mac_event_is_active(x) emberEventIsScheduled(&(x))
#else
#define sli_mac_inactivate_event(x) emberEventControlSetInactive(x)
#define sli_mac_activate_event(x) emberEventControlSetActive(x)
#define sli_mac_set_event_delay_ms(x, y) emberEventControlSetDelayMS(x, y)
#define sli_mac_event_is_active(x) emberEventControlGetActive(x)
#endif

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif

#ifdef RAIL_MUX // from jam
#define SL_CATALOG_RAIL_MULTIPLEXER_PRESENT 1
#endif

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
sl_status_t sli_lower_mac_lock_radio (void);
sl_status_t sli_lower_mac_unlock_radio (void);
#else
#define sli_lower_mac_lock_radio() SL_STATUS_OK
#define sli_lower_mac_unlock_radio() SL_STATUS_OK
#endif //SL_CATALOG_RAIL_MULTIPLEXER_PRESENT

#endif //LOWER_MAC_H
