/***************************************************************************//**
 * @file
 * @brief app_measurement.c
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
#include <string.h>
#include <stdlib.h>
#include "em_core.h"
#include "rail.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
#include "sl_rail_util_init.h"
#endif
#include "rail_ieee802154.h"
#include "rail_ble.h"
#include "app_measurement.h"
#if defined(SL_CATALOG_GLIB_PRESENT)
#include "app_graphics.h"
#endif
#include "app_menu.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_rail_util_pa_config.h"
#include "em_common.h"
#include "pa_curve_types_efr32.h"
#include "pa_conversions_efr32.h"
#include "printf.h"

#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
#include "rail_config.h"
#endif

#ifdef  SL_CATALOG_RANGE_TEST_STD_COMPONENT_PRESENT
#include "app_measurement_standard.h"
#endif

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
#include "app_bluetooth.h"
#include "gatt_db.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

#if defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_4FSK_9P6KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_2FSK_4P8KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_2FSK_2P4KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_450MHZ_2FSK_4P8KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_450MHZ_4FSK_9P6KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_896MHZ_2FSK_40KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_915MHZ_2FSK_10KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_920MHZ_4FSK_400KBPS)
#undef RAIL0_CHANNEL_GROUP_1_PROFILE_BASE
#define RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK
#endif

#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN) || defined(RAIL0_CHANNELS_FOR_915_PROFILE_WISUN) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FAN_1_0) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_HAN) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_SUN_OQPSK)
#include "sl_flex_packet_asm.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Macro to print out variable name
#define GET_VARIABLE_NAME(NAME) #NAME

/// Macro to easly print out errors from RAIL_Handler
#define PRINT_AND_CLEAR_FLAG(flag)  if (flag) { flag = 0; app_log_info(" %s\n", GET_VARIABLE_NAME(flag)); }

/// Time between send in microseconds
#define RANGE_TEST_SEND_TIME ((uint32_t) 100000)

/// RAIL buffer sizes
#define RAIL_TX_BUFFER_SIZE         (128U)
#define RAIL_RX_BUFFER_SIZE         (256U)

/// structre to keep the error flags in a small variable
typedef struct error_flags_t {
  struct tx_errors{
    uint8_t tx_aborted : 1;
    uint8_t tx_blocked : 1;
    uint8_t tx_underflow : 1;
    uint8_t tx_channel_busy : 1;
    uint8_t tx_scheduled_tx_missed : 1;
  }tx_errors;
  struct rx_errors{
    uint8_t rx_aborted : 1;
    uint8_t rx_frame_error : 1;
    uint8_t rx_fifo_overflow : 1;
    uint8_t rx_address_filtered : 1;
    uint8_t rx_scheduled_rx_missed : 1;
  }rx_errors;
}error_flags_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  Function to generate the payload of the packet to be sent.
 ******************************************************************************/
static void range_test_generate_payload(uint8_t *data, uint16_t data_length);

/*******************************************************************************
 * @brief  Function get packet and send it on selected handler
 ******************************************************************************/
static void send_packet(uint16_t packet_number);

/*******************************************************************************
 * @brief  This function inserts a number of bits into the moving average
 *         history.
 ******************************************************************************/
static void range_test_MA_set(uint32_t nr);

/*******************************************************************************
 * @brief  This function clears the most recent bit in the moving average
 *         history. This indicates that last time we did not see any missing
 *         packages.
 ******************************************************************************/
static void range_test_MA_clear(void);

/*******************************************************************************
 * @brief  Returns the moving average of missing pacakges based on the
 *         history data.
 ******************************************************************************/
static uint8_t range_test_MA_get(void);

/*******************************************************************************
 * @brief  Clears the history of the moving average calculation.
 ******************************************************************************/
static void range_test_MA_clear_all(void);

/*******************************************************************************
 * @brief  Function to count how many bits has the value of 1.
 ******************************************************************************/
static uint32_t range_test_count_bits(uint32_t u);

/*******************************************************************************
 * @brief  Stops RX and TX and set custom rail handler to IDLE
 ******************************************************************************/
static void prepare_radio_config_packet(uint16_t packet_number, uint8_t *tx_buffer);

/*******************************************************************************
 * @brief  Stops RX and TX and set custom rail handler to IDLE
 ******************************************************************************/
void set_custom_handler_to_idle(void);

/*******************************************************************************
 * @brief  Prints out the TX side of the logs
 ******************************************************************************/
static inline void print_tx_logs(void);

/*******************************************************************************
 * @brief  Prints out the RX side of the logs
 ******************************************************************************/
static inline void print_rx_logs(void);

/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *
 * @param rx_destination Where should the full packet be unpacked
 * @param packet_information Where should all the information of the packet stored
 * @param start_of_payload Pointer where the payload starts
 * @return The length of the received payload
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload);

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *
 * @param rail_handle Which rail handlers should be used for the TX FIFO writing
 * @param out_data The payload buffer
 * @param length The length of the payload
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// All setting regarding the range test
volatile range_test_settings_t range_test_settings;
/// All data regarding the range test
volatile range_test_measurement_t range_test_measurement;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Scheduling setting for TX part of the range test
static RAIL_ScheduleTxConfig_t schedule = {
  .mode = RAIL_TIME_DELAY,
#if defined(HARDWARE_BOARD_HAS_EFF)
  // double the time for EFF to not to cause overheating
  .when = RANGE_TEST_SEND_TIME*2,
#else
  .when = RANGE_TEST_SEND_TIME,
#endif
};

/// Memory allocation for RAIL TX FIFO
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_TX_BUFFER_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[RAIL_TX_BUFFER_SIZE];
} tx_buffer = { .fifo = { 0x00 } };

/// Receive FIFO
static uint8_t rx_fifo[RAIL_RX_BUFFER_SIZE] = { 0x00 };

/// Flag for TX, shows if the sending was successful and happened
static volatile bool schedule_is_ready = false;

/// Flag for TX, shows if the sending wasn't successful and happened
static volatile bool send_failed = false;

/// Flag for RX, shows that CRC error happened
static volatile bool rx_crc_error_happpend = false;

/// Counter for RX, shows that packet was received without error
static volatile uint8_t rx_packet_received = 0;

/// Flag for printing out log on UART set true to print out the current status
static volatile bool logging_needed = false;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// RAIL Rx packet handle
static volatile RAIL_RxPacketHandle_t rx_packet_handle;

/// Variable to store received packet info
static RAIL_RxPacketInfo_t packet_info;

/// Variable to store recevied packet details
static RAIL_RxPacketDetails_t packet_details;

/// Number of the user defined pyhs
static uint8_t number_of_custom_phys = 0;

/// variable to hold all rail rx and tx errors from RAIL_Handle
static error_flags_t error_flags = { 0 };

/// Scheduler setting for dmp case
#if defined(SL_CATALOG_KERNEL_PRESENT)
static RAIL_SchedulerInfo_t scheduler_info = {
  .priority = 100,
  .slipTime = 100000,
  .transactionTime = 2500
};

#endif

// -----------------------------------------------------------------------------
//                          Weak Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK bool standard_phy_are_present(void)
{
  return 0;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK uint8_t current_phy_standard_value(void)
{
  return range_test_settings.current_phy;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK RAIL_Handle_t get_standard_rail_handler(void)
{
  return NULL;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void get_rail_standard_config_data(uint32_t *base_frequency, uint32_t *channel_spacing)
{
  *base_frequency  = 0;
  *channel_spacing = 0;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void get_rail_standard_channel_range(uint16_t *min, uint16_t *max)
{
  *min = 0;
  *max = 0;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void get_rail_standard_payload_range(uint8_t *payload_min, uint8_t *payload_max)
{
  *payload_min = 0;
  *payload_max = 0;
}

/*******************************************************************************
 * Genarete a string for the BLE mobile APP from standared phys
 * @param phy_index: which phy name is needed
 * @param buffer: where to write the caracters
 * @param length: the length of the hole written string
 ******************************************************************************/
SL_WEAK void std_phy_list_generation(uint8_t phy_index, uint8_t *buffer, uint8_t *length)
{
  (void)phy_index;
  (void)buffer;
  (void)length;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void init_ranget_test_standard_phys(uint8_t* number_of_phys)
{
  (void)number_of_phys;  // Unused parameter.
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void set_ieee_handler_to_idle(void)
{
  return;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK void set_ble_handler_to_idle(void)
{
  return;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK bool is_current_phy_ble(void)
{
  return false;
}

/*******************************************************************************
 * @brief  This function is defined in app_measurement_standard.c
 *         If that file is not present then it is an only custom phy range test
 ******************************************************************************/
SL_WEAK range_test_packet_t* get_start_of_payload_for_standard(uint8_t* received_buffer)
{
  (void)received_buffer;  // Unused parameter.
  return NULL;
}

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Get the minimum Power Amplifier setting in deci-dBm units
 *
 * @param None
 * @returns minimum PA value in deci-dBm
 ******************************************************************************/
int16_t get_min_tx_power_deci_dbm(void)
{
  RAIL_Handle_t rail_handle;
  RAIL_TxPowerConfig_t tx_power_config;
  RAIL_Status_t status;
  RAIL_TxPowerMode_t tx_power_mode;
  const RAIL_TxPowerCurves_t *tx_power_curve_ptr = NULL;
  bool rail_init_ready = true;

#ifdef  SL_CATALOG_RANGE_TEST_STD_COMPONENT_PRESENT
  rail_init_ready = is_init_ranget_test_standard_ready();
#endif

  if (rail_init_ready) {
    rail_handle = get_current_rail_handler();
    status = RAIL_GetTxPowerConfig(rail_handle, &tx_power_config);
    if (status != RAIL_STATUS_NO_ERROR) {
      app_log_error("get_min_tx_power_deci_dbm:RAIL_GetTxPowerConfig failed with %d\n", status);
    }
    tx_power_mode = tx_power_config.mode;
    tx_power_curve_ptr = RAIL_GetTxPowerCurve(tx_power_mode);
    if (tx_power_curve_ptr == NULL) {
      app_log_error("get_min_tx_power_deci_dbm:RAIL_GetTxPowerCurve failed with NULL pointer.\n");
    }
  }
  if (tx_power_curve_ptr == NULL) {
    int16_t minPower;
    minPower = -260;
    return minPower;   // if not yet initialized return the possible lowest value
  }
  return tx_power_curve_ptr->minPower;
}

/*******************************************************************************
 * Get the maximum Power Amplifier setting in deci-dBm units
 *
 * @param None
 * @returns maximum PA value in deci-dBm
 ******************************************************************************/
int16_t get_max_tx_power_deci_dbm(void)
{
  RAIL_Handle_t rail_handle;
  RAIL_TxPowerConfig_t tx_power_config;
  RAIL_Status_t status;
  RAIL_TxPowerMode_t tx_power_mode;
  const RAIL_TxPowerCurves_t *tx_power_curve_ptr = NULL;
  bool rail_init_ready = true;

#ifdef  SL_CATALOG_RANGE_TEST_STD_COMPONENT_PRESENT
  rail_init_ready = is_init_ranget_test_standard_ready();
#endif
  if (rail_init_ready) {
    rail_handle = get_current_rail_handler();
    status = RAIL_GetTxPowerConfig(rail_handle, &tx_power_config);
    if (status != RAIL_STATUS_NO_ERROR) {
      app_log_error("get_max_tx_power_deci_dbm:RAIL_GetTxPowerConfig failed with %d\n", status);
    }
    tx_power_mode = tx_power_config.mode;
    tx_power_curve_ptr = RAIL_GetTxPowerCurve(tx_power_mode);
    if (tx_power_curve_ptr == NULL) {
      app_log_error("get_max_tx_power_deci_dbm:RAIL_GetTxPowerCurve failed with NULL pointer.\n");
    }
  }
  if (tx_power_curve_ptr == NULL) {
    int16_t maxPower;
    maxPower = 200;
    return maxPower;   // if not yet initialized return the possible highest value
  }
  return tx_power_curve_ptr->maxPower;
}

/*******************************************************************************
 * Helper function to apply the new power level from the range test settings
 *
 * @param None
 * @returns None
 ******************************************************************************/
void update_tx_power(void)
{
  RAIL_Status_t power_status = RAIL_STATUS_NO_ERROR;
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  power_status = RAIL_SetTxPowerDbm(rail_handle, range_test_settings.tx_power);
  if (power_status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_SetTxPowerDbm failed with %d\n", power_status);
  }
}

/*******************************************************************************
 * Helper function to return the custom phy number
 *
 * @param None
 * @returns number_of_custom_phys
 ******************************************************************************/
uint8_t get_number_of_custom_phys(void)
{
  return number_of_custom_phys;
}

/*******************************************************************************
 * Helper function to differentiate custom and BLE or IEEE phys.
 * If only custom phys are in the project the return value will be false always.
 *
 * @param None
 * @returns if custom false otherwise true
 ******************************************************************************/
bool is_current_phy_standard(void)
{
  return standard_phy_are_present();
}

/*******************************************************************************
 * Get the rail handler which is selected in the phy selection
 *
 * @param None
 * @returns RAIL_Handle_t a pointer to the will be used handler
 ******************************************************************************/
RAIL_Handle_t get_current_rail_handler(void)
{
  if (is_current_phy_standard()) {
    return get_standard_rail_handler();
  } else {
    #if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
    return sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  #else
    return NULL;
    #endif
  }
}

/*******************************************************************************
 * Set power level to the defined max and init it if needed
 *
 * @param init: id true it will reinit the config
 * @returns None
 ******************************************************************************/
void set_power_level_to_max(bool init)
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  RAIL_TxPowerConfig_t tx_power_config_original;
  bool tx_power_needs_reinit = false;

  RAIL_GetTxPowerConfig(rail_handle, &tx_power_config_original);

  #if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
  if (!is_current_phy_standard()) {
    RAIL_ConfigChannels(rail_handle,
                        channelConfigs[range_test_settings.current_phy], NULL);
    uint16_t channel = range_test_settings.channel;
    if ((channel
         > channelConfigs[range_test_settings.current_phy]->configs[0U].channelNumberEnd)
        || (init)) {
      // Initialize the channel to the first one in channel config
      range_test_settings.channel =
        channelConfigs[range_test_settings.current_phy]->configs[0U].channelNumberStart;
    }

    if (channelConfigs[range_test_settings.current_phy]->configs[0].baseFrequency
        < 1000000000UL) {
      if (tx_power_config_original.mode != SL_RAIL_UTIL_PA_SELECTION_SUBGHZ) {
        tx_power_config_original.mode = SL_RAIL_UTIL_PA_SELECTION_SUBGHZ;
        tx_power_needs_reinit = true;
      }
    } else {
      if (tx_power_config_original.mode != SL_RAIL_UTIL_PA_SELECTION_2P4GHZ) {
        tx_power_config_original.mode = SL_RAIL_UTIL_PA_SELECTION_2P4GHZ;
        tx_power_needs_reinit = true;
      }
    }
  }
  #endif

  if (tx_power_needs_reinit || init) {
    if (RAIL_ConfigTxPower(rail_handle, &tx_power_config_original) != RAIL_STATUS_NO_ERROR) {
      // Error: The PA could not be initialized due to an improper configuration.
      // Please ensure your configuration is valid for the selected part.
      while (1) ;
    }
    RAIL_SetTxPowerDbm(rail_handle, get_max_tx_power_deci_dbm());
  }
}

/*******************************************************************************
 * Hides the configs from other parts of the application
 *
 * @param base_frequency: The current frequency of the radio
 * @param channel_spacing: The current channel spacing of the selected phy
 * @param power: The current power of the radio
 * @returns None
 ******************************************************************************/
void get_rail_config_data(uint32_t *base_frequency, uint32_t *channel_spacing, int16_t *power)
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  int16_t power_buffer = (int16_t)(RAIL_GetTxPowerDbm(rail_handle));

  *power = power_buffer;
  if (is_current_phy_standard()) {
    get_rail_standard_config_data(base_frequency, channel_spacing);
  } else {
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
    *base_frequency = channelConfigs[range_test_settings.current_phy]->configs[0].baseFrequency;
    *channel_spacing = channelConfigs[range_test_settings.current_phy]->configs[0U].channelSpacing;
#endif
  }
}

/*******************************************************************************
 * Hides the configs from other parts of the application
 *
 * @param start: minimum number of channels in phy
 * @param end: maximum number of channels in phy
 * @return None
 ******************************************************************************/
void get_rail_channel_range(uint16_t *start, uint16_t *end)
{
  if (is_current_phy_standard()) {
    get_rail_standard_channel_range(start, end);
  } else {
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
    *start = channelConfigs[range_test_settings.current_phy]->configs[0U].channelNumberStart;
    *end = channelConfigs[range_test_settings.current_phy]->configs[0U].channelNumberEnd;
#endif
  }
}

/*******************************************************************************
 * Hides the configs from other parts of the application
 *
 * @param payload_min: minimum number of payload in phy
 * @param payload_max: maximum number of payload in phy
 * @return None
 ******************************************************************************/
void get_rail_payload_range(uint8_t *payload_min, uint8_t *payload_max)
{
  if (is_current_phy_standard()) {
    get_rail_standard_payload_range(payload_min, payload_max);
  } else {
  #if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
    *payload_min = RANGETEST_PAYLOAD_LEN_MIN;
    *payload_max = RANGETEST_PAYLOAD_LEN_MAX;
  #endif
  }
}

/*******************************************************************************
 * Genarete a string for the BLE mobile APP
 * @param buffer: where to write the caracters
 * @param length: the length of the hole written string
 ******************************************************************************/
void phy_list_generation(uint8_t *buffer, uint8_t *length)
{
  for (uint8_t phy_index = 0; phy_index < number_of_phys; phy_index++) {
    if (number_of_custom_phys == 0 || number_of_custom_phys <= phy_index) {
      std_phy_list_generation(phy_index, buffer, length);
    } else {
      // *buffer length comes from app_bluetooth.c
      snprintf((char*)(&buffer[*length]), 255, "%u:custom_%u,", phy_index, phy_index);
      *length = strlen((char*)(buffer));
    }
  }
}

/*******************************************************************************
 * Inits the phys for the range test, and counts them.
 *
 * @returns None
 ******************************************************************************/
void init_range_test_phys(void)
{
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
  for (number_of_custom_phys = 0; number_of_custom_phys < 255; ++number_of_custom_phys) {
    if (channelConfigs[number_of_custom_phys] == NULL) {
      break;
    }
  }
#endif
  number_of_phys = number_of_custom_phys;
  init_ranget_test_standard_phys(&number_of_phys);
}

/*******************************************************************************
 * @brief  Resets the internal status of the Range Test and sets up needed
 *         settings
 *
 * @return None.
 ******************************************************************************/
void range_test_init()
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  range_test_reset_values();
  if (!is_current_phy_standard()) {
    RAIL_SetFixedLength(rail_handle, range_test_settings.payload_length);
  }

  if (range_test_settings.radio_mode == RADIO_MODE_TX) {
    RAIL_SetTxFifo(rail_handle, tx_buffer.fifo, 0, RAIL_TX_BUFFER_SIZE);
  }
}

/*******************************************************************************
 * @brief  Resets all measurment data
 *
 * @return None.
 ******************************************************************************/
void range_test_reset_values(void)
{
  range_test_measurement.packets_received_counter = 0U;
  range_test_measurement.first_recevied_packet_offset = 0U;
  range_test_measurement.packets_received_correctly = RANGETEST_PACKET_COUNT_INVALID;
  range_test_measurement.packets_sent = 0U;
  range_test_measurement.packets_with_crc_error = 0U;
  schedule_is_ready = true;
}

/*******************************************************************************
 * @brief  TX part of the measurement, manages counting and logging/updating
 *
 * @return refresh_screen_is_needed: true if LCD needed to be updated
 ******************************************************************************/
bool send_measurment(void)
{
  bool refresh_screen_is_needed = false;
  uint16_t packets_sent = range_test_measurement.packets_sent;
  if (packets_sent < range_test_settings.packets_repeat_number) {
    if (schedule_is_ready || send_failed) {
      schedule_is_ready = false;
      if (!send_failed) {
        range_test_measurement.packets_sent++;
      } else {
        send_failed = false;
      }
      send_packet(range_test_measurement.packets_sent);
      refresh_screen_is_needed = true;

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
      add_bluetooth_indication(gattdb_pktsSent);
#endif
    }
  } else {
    range_test_measurement.tx_is_running = false;
    refresh_screen_is_needed = true;
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
    if (!is_bluetooth_connected()) {
      activate_bluetooth();
    }
    add_bluetooth_indication(gattdb_isRunning);
#endif
  }
  if (range_test_settings.usart_log_enable) {
    logging_needed = refresh_screen_is_needed;
  }
  return refresh_screen_is_needed;
}

/*******************************************************************************
 * @brief  Set all handlers to IDLE state, useful as multiply handlers can
 *         occupy the radio same time and block each other at worst cases
 *
 * @return None
 ******************************************************************************/
void set_all_radio_handlers_to_idle(void)
{
#if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
  set_custom_handler_to_idle();
#endif
  set_ieee_handler_to_idle();
  set_ble_handler_to_idle();
}

/*******************************************************************************
 * @brief  Prepaire the selected handler to RX measurement
 *
 * @return None
 ******************************************************************************/
void receive_setup_radio(void)
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  RAIL_Status_t status = 0;

  range_test_measurement.tx_is_running = false;

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  deactivate_bluetooth();
  scheduler_info = (RAIL_SchedulerInfo_t){.priority = 200 };
  status = RAIL_StartRx(rail_handle, range_test_settings.channel, &scheduler_info);
#else
  status = RAIL_StartRx(rail_handle, range_test_settings.channel, NULL);
#endif
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_error("RAIL_StartRx failed with code %d", status);
  }
  rx_crc_error_happpend = false;
  rx_packet_received = 0;
#if defined(SL_CATALOG_GLIB_PRESENT)
  graphics_clear_rssi_buffer();
#endif
}

/*******************************************************************************
 * @brief  Stops the RX measurment and sets the radio handler to IDLE
 *
 * @return None
 ******************************************************************************/
void stop_recive_measurement(void)
{
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  RAIL_Idle(rail_handle, RAIL_IDLE, true);
#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
  activate_bluetooth();
#endif
}

/*******************************************************************************
 * @brief  RX measurement logic, here is the packet number check and etc.
 *
 * @return refresh_screen_is_needed: true if the LCD is needed to be updated
 ******************************************************************************/
bool receive_measurment(void)
{
  range_test_packet_t *rx_packet = NULL;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;
  bool refresh_screen_is_needed = false;
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  static uint32_t last_received_packet_count = 0U;
  int8_t rssi_value = 0;

  if (rx_crc_error_happpend) {
    rx_crc_error_happpend = false;
    if (range_test_measurement.packets_with_crc_error < 0xFFFF) {
      range_test_measurement.packets_with_crc_error++;
      refresh_screen_is_needed = true;
    }

#if defined(SL_CATALOG_KERNEL_PRESENT)
    scheduler_info = (RAIL_SchedulerInfo_t){.priority = 200 };
    rail_status = RAIL_StartRx(rail_handle, range_test_settings.channel, &scheduler_info);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with %d", rail_status);
    }
    #else
    rail_status = RAIL_StartRx(rail_handle, range_test_settings.channel, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartRx failed with %d", rail_status);
    }
    #endif
  }

  if (rx_packet_received > 0) {
    CORE_ATOMIC_SECTION(
      rx_packet_received--;
      )

#if defined(SL_CATALOG_KERNEL_PRESENT)
    if (rx_packet_received > 0) {
      app_task_notify();
    }
#endif
    // read packet details
    rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle,
                                            RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
    RAIL_GetRxPacketDetails(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_details);
    if (is_current_phy_standard()) {
      // copy the content of the packet
      RAIL_CopyRxPacket(rx_fifo, &packet_info);
      RAIL_ReleaseRxPacket(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE);
      rx_packet = get_start_of_payload_for_standard(rx_fifo);
    } else {
      uint8_t *start_of_packet = 0;
      (void)unpack_packet(rx_fifo, &packet_info, &start_of_packet);
      rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
      rx_packet = (range_test_packet_t*) start_of_packet;
    }

#if defined(SL_CATALOG_KERNEL_PRESENT)
    scheduler_info = (RAIL_SchedulerInfo_t){.priority = 200 };
    RAIL_StartRx(rail_handle, range_test_settings.channel, &scheduler_info);
#else
    RAIL_StartRx(rail_handle, range_test_settings.channel, NULL);
#endif

    // Make sure the packet addressed to me
    if (rx_packet->destination_id != range_test_settings.source_id) {
      return refresh_screen_is_needed;
    }

    // Make sure the packet sent by the selected remote
    if (rx_packet->source_id != range_test_settings.destination_id) {
      return refresh_screen_is_needed;
    }

    if ((RANGETEST_PACKET_COUNT_INVALID == range_test_measurement.packets_received_correctly)
        || (rx_packet->packet_counter <= range_test_measurement.packets_received_counter)) {
      // First packet received OR
      // Received packet counter lower than already received counter.

#if defined(SL_CATALOG_RANGE_TEST_DMP_COMPONENT_PRESENT)
      deactivate_bluetooth();
#endif

      // Reset received counter
      range_test_measurement.packets_received_correctly = 0U;
      // Set counter offset
      range_test_measurement.first_recevied_packet_offset = rx_packet->packet_counter - 1U;

#if defined(SL_CATALOG_GLIB_PRESENT)
      // Clear RSSI Chart
      graphics_clear_rssi_buffer();
#endif

      // Clear Moving-Average history
      range_test_MA_clear_all();

      // Restart Moving-Average calculation
      last_received_packet_count = 0U;
    }

    if (range_test_measurement.packets_received_correctly < 0xFFFF) {
      range_test_measurement.packets_received_correctly++;
    }

    range_test_measurement.packets_received_counter = rx_packet->packet_counter - range_test_measurement.first_recevied_packet_offset;
    range_test_measurement.rssi_latch_value = packet_details.rssi;

    // Store RSSI value from the latch
    rssi_value = range_test_measurement.rssi_latch_value;
    // Limit stored RSSI values to the displayable range
#if defined(SL_CATALOG_GLIB_PRESENT)
    rssi_value = rssi_value_for_display(range_test_measurement.rssi_latch_value);
#endif
#if defined(SL_CATALOG_GLIB_PRESENT)
    // Store RSSI value in ring buffer
    graphics_add_to_rssi_buffer(rssi_value);
#endif

    // Calculate recently lost packets number based on newest counter
    if ((range_test_measurement.packets_received_counter - last_received_packet_count) > 1U) {
      // At least one packet lost
      range_test_MA_set(range_test_measurement.packets_received_counter - last_received_packet_count - 1U);
    }
    // Current packet is received
    range_test_MA_clear();
    last_received_packet_count = range_test_measurement.packets_received_counter;

    // Calculate Moving-Average Error Rate
    range_test_measurement.moving_average = (range_test_MA_get() * 100.0F)
                                            / range_test_settings.moving_average_window_size;

    uint16_t packets_received_counter = range_test_measurement.packets_received_counter;
    // Calculate Packet Error Rate
    range_test_measurement.PER = (packets_received_counter)   // Avoid zero division
                                 ? (((float) (packets_received_counter - range_test_measurement.packets_received_correctly)
                                     * 100.0F) / packets_received_counter)   // Calculate PER
                                 :
                                 0.0F;   // By default PER is 0.0%

    refresh_screen_is_needed = true;
  }

  if (range_test_settings.usart_log_enable) {
    logging_needed = refresh_screen_is_needed;
  }
  return refresh_screen_is_needed;
}

/*******************************************************************************
 * @brief Logging function which prints out the current received or sent packet,
 *        and statistics.
 *
 * @return None
 ******************************************************************************/
void print_log(void)
{
  if (logging_needed) {
    logging_needed = false;
    if (RADIO_MODE_RX == range_test_settings.radio_mode) {
      print_rx_logs();
    } else if (RADIO_MODE_TX == range_test_settings.radio_mode) {
      if (range_test_measurement.tx_is_running) {
        print_tx_logs();
      }
    }
  }
}

/******************************************************************************
* RAIL callback, called if a RAIL event occurs
******************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  (void) rail_handle;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (range_test_settings.radio_mode == RADIO_MODE_TX) {
    if (events & RAIL_EVENT_SCHEDULER_STATUS) {
      send_failed = true;
      RAIL_YieldRadio(rail_handle);
    }
  }
#endif

  // Handle Tx events
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      schedule_is_ready = true;
    } else if (events & RAIL_EVENT_TX_ABORTED) {
      error_flags.tx_errors.tx_aborted = 1;
    } else if (events & RAIL_EVENT_TX_BLOCKED) {
      error_flags.tx_errors.tx_blocked = 1;
    } else if (events & RAIL_EVENT_TX_UNDERFLOW) {
      error_flags.tx_errors.tx_underflow = 1;
    } else if (events & RAIL_EVENT_TX_CHANNEL_BUSY) {
      error_flags.tx_errors.tx_channel_busy = 1;
    } else if (events & RAIL_EVENT_TX_SCHEDULED_TX_MISSED) {
      error_flags.tx_errors.tx_scheduled_tx_missed = 1;
      send_failed = true;
    }
#if defined(SL_CATALOG_KERNEL_PRESENT)
    RAIL_YieldRadio(rail_handle);
#endif
  }

  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      CORE_ATOMIC_SECTION(
        rx_packet_received++;
        )
      RAIL_HoldRxPacket(rail_handle);
    } else if (events & RAIL_EVENT_RX_FRAME_ERROR) {
      rx_crc_error_happpend = true;
      error_flags.rx_errors.rx_frame_error = 1;
    } else if (events & RAIL_EVENT_RX_PACKET_ABORTED) {
      error_flags.rx_errors.rx_aborted = 1;
    } else if (events & RAIL_EVENT_RX_FIFO_OVERFLOW) {
      error_flags.rx_errors.rx_fifo_overflow = 1;
    } else if (events & RAIL_EVENT_RX_ADDRESS_FILTERED) {
      error_flags.rx_errors.rx_address_filtered = 1;
    } else if (events & RAIL_EVENT_RX_SCHEDULED_RX_MISSED) {
      error_flags.rx_errors.rx_scheduled_rx_missed = 1;
    }
#if defined(SL_CATALOG_KERNEL_PRESENT)
    if (range_test_settings.radio_mode == RADIO_MODE_RX) {
      RAIL_YieldRadio(rail_handle);
    }
#endif
  }

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
#if defined(SL_CATALOG_KERNEL_PRESENT)
    RAIL_YieldRadio(rail_handle);
#endif
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * @brief  Prints out the errors that happened during RAIL_Handle on RX or TX
 *
 * @param  None
 *
 * @return None
 ******************************************************************************/
void print_errors_from_rail_handler(void)
{
  PRINT_AND_CLEAR_FLAG(error_flags.tx_errors.tx_aborted);
  PRINT_AND_CLEAR_FLAG(error_flags.tx_errors.tx_blocked);
  PRINT_AND_CLEAR_FLAG(error_flags.tx_errors.tx_underflow);
  PRINT_AND_CLEAR_FLAG(error_flags.tx_errors.tx_channel_busy);
  PRINT_AND_CLEAR_FLAG(error_flags.tx_errors.tx_scheduled_tx_missed);
  PRINT_AND_CLEAR_FLAG(error_flags.rx_errors.rx_aborted);
  PRINT_AND_CLEAR_FLAG(error_flags.rx_errors.rx_frame_error);
  PRINT_AND_CLEAR_FLAG(error_flags.rx_errors.rx_fifo_overflow);
  PRINT_AND_CLEAR_FLAG(error_flags.rx_errors.rx_address_filtered);
  PRINT_AND_CLEAR_FLAG(error_flags.rx_errors.rx_scheduled_rx_missed);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * @brief  Stops RX and TX and set custom rail handler to IDLE
 *
 * @param  None.
 *
 * @return None.
 ******************************************************************************/
void set_custom_handler_to_idle(void)
{
  #if defined(SL_CATALOG_RADIO_CONFIG_SIMPLE_RAIL_SINGLEPHY_PRESENT)
  RAIL_Handle_t rail_handle_custom = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  RAIL_Idle(rail_handle_custom, RAIL_IDLE, true);
  #endif
}

/*******************************************************************************
 * @brief  Stops RX and TX and set custom rail handler to IDLE
 *
 * @param  packet_number: the actual packet count to be put into the package
 *
 * @param  tx_buffer: pointer to the buffer where the packet is put together
 *
 * @return None.
 ******************************************************************************/
static void prepare_radio_config_packet(uint16_t packet_number, uint8_t *tx_buffer)
{
  range_test_packet_t *tx_packet;
  tx_packet = (range_test_packet_t*)tx_buffer;

  tx_packet->packet_counter = packet_number;
  tx_packet->destination_id = range_test_settings.destination_id;
  tx_packet->source_id = range_test_settings.source_id;

  uint16_t packet_length = range_test_settings.payload_length;

  range_test_generate_payload(tx_buffer, packet_length);
}

/*******************************************************************************
 * @brief  Function to generate the payload of the packet to be sent.
 *
 * @param  data: TX buffer to fill out with the gene.
 *
 * @param  data_length: how many filling is needed for packet size
 *
 * @return None.
 ******************************************************************************/
static void range_test_generate_payload(uint8_t *data, uint16_t data_length)
{
  for (uint8_t i = sizeof(range_test_packet_t); i < data_length; i++) {
    data[i] = (i % 2U) ? (0x55U) : (0xAAU);
  }
}

SL_WEAK void prepare_ieee802154_data_frame(uint16_t packet_number, uint8_t *tx_buffer)
{
  (void)packet_number;  // Unused parameter.
  (void)tx_buffer;  // Unused parameter.
}

SL_WEAK void prepare_ble_advertising_channel_pdu(uint16_t packet_number, uint8_t *tx_buffer)
{
  (void)packet_number;  // Unused parameter.
  (void)tx_buffer;  // Unused parameter.
}

SL_WEAK uint8_t get_ieee802154_data_frame_length(void)
{
  return 0;
}

/*******************************************************************************
 * @brief  Function get packet and send it on selected handler
 *
 * @param  packet_number: this number will be put into the packet
 *
 * @return None.
 ******************************************************************************/
static void send_packet(uint16_t packet_number)
{
  RAIL_Status_t rail_status;
  RAIL_Handle_t rail_handle = get_current_rail_handler();
  bool set_tx_failed = false;
  uint8_t send_buffer[RAIL_TX_BUFFER_SIZE] = { 0 };
  uint16_t tx_length = 0;

  if (is_current_phy_standard()) {
    if (!is_current_phy_ble()) {
      prepare_ieee802154_data_frame(packet_number, send_buffer);
      tx_length = get_ieee802154_data_frame_length();
    } else {
      prepare_ble_advertising_channel_pdu(packet_number, send_buffer);
      tx_length = send_buffer[1] + 2;
    }
  } else {
    prepare_radio_config_packet(packet_number, send_buffer);
    tx_length = range_test_settings.payload_length;
  }

  prepare_package(rail_handle, send_buffer, tx_length);

  if (!set_tx_failed) {
#if defined(SL_CATALOG_KERNEL_PRESENT)
    scheduler_info = (RAIL_SchedulerInfo_t){ .priority = 100,
                                             .slipTime = 100000,
                                             .transactionTime = 2500 };

    rail_status = RAIL_StartScheduledTx(rail_handle,
                                        range_test_settings.channel, RAIL_TX_OPTIONS_DEFAULT,
                                        &schedule,
                                        &scheduler_info);
#else
    rail_status = RAIL_StartScheduledTx(rail_handle,
                                        range_test_settings.channel, RAIL_TX_OPTIONS_DEFAULT, &schedule,
                                        NULL);
#endif
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_error("RAIL_StartTx() error 0x%0X\n", rail_status);
    }
  }
}

/*******************************************************************************
 * @brief  This function clears the most recent bit in the moving average
 *         history. This indicates that last time we did not see any missing
 *         packages.
 *
 * @return None.
 ******************************************************************************/
static void range_test_MA_clear(void)
{
  // Buffering volatile value
  uint8_t  ma_finger = range_test_measurement.moving_average_current_point_value;

  range_test_measurement.moving_average_history[ma_finger >> 5U] &= ~(1U << (ma_finger % 32U));

  ma_finger++;
  if (ma_finger >= range_test_settings.moving_average_window_size) {
    ma_finger = 0U;
  }
  // Updating new value back to volatile
  range_test_measurement.moving_average_current_point_value = ma_finger;
}

/*******************************************************************************
 * @brief  This function inserts a number of bits into the moving average
 *         history.
 *
 * @param  nr: The value to be inserted into the history.
 *
 * @return None.
 ******************************************************************************/
static void range_test_MA_set(uint32_t nr)
{
  uint8_t i;
  // Buffering volatile fields
  uint8_t  ma_finger = range_test_measurement.moving_average_current_point_value;

  if (nr >= range_test_settings.moving_average_window_size) {
    // Set all bits to 1's
    i = range_test_settings.moving_average_window_size;

    while (i >> 5U) {
      range_test_measurement.moving_average_history[(i >> 5U) - 1U] = 0xFFFFFFFFUL;
      i -= 32U;
    }
    return;
  }

  while (nr) {
    range_test_measurement.moving_average_history[ma_finger >> 5U] |= (1U << ma_finger % 32U);
    ma_finger++;
    if (ma_finger >= range_test_settings.moving_average_window_size) {
      ma_finger = 0U;
    }
    nr--;
  }
  // Update the bufferd value back to the volatile field
  range_test_measurement.moving_average_current_point_value = ma_finger;
}

/*******************************************************************************
 * @brief  Returns the moving average of missing pacakges based on the
 *         history data.
 *
 * @return The current moving average .
 ******************************************************************************/
static uint8_t range_test_MA_get(void)
{
  uint8_t return_value = 0U;

  for (uint8_t i = 0U; i < (range_test_settings.moving_average_window_size >> 5U); i++) {
    return_value += range_test_count_bits(range_test_measurement.moving_average_history[i]);
  }
  return return_value;
}

/*******************************************************************************
 * @brief  Clears the history of the moving average calculation.
 *
 * @return None.
 ******************************************************************************/
static void range_test_MA_clear_all(void)
{
  range_test_measurement.moving_average_history[0U] = range_test_measurement.moving_average_history[1U]
                                                        = range_test_measurement.moving_average_history[2U]
                                                            = range_test_measurement.moving_average_history[3U]
                                                                = 0U;
}

/*******************************************************************************
 * @brief  Function to count how many bits has the value of 1.
 *
 * @param  u: Input value to count its '1' bits.
 *
 * @return Number of '1' bits in the input.
 ******************************************************************************/
static uint32_t range_test_count_bits(uint32_t u)
{
  uint32_t count = u
                   - ((u >> 1U) & 033333333333)
                   - ((u >> 2U) & 011111111111);

  return  (((count + (count >> 3U)) & 030707070707) % 63U);
}

/*******************************************************************************
 * @brief  Prints out the TX side of the logs
 *
 * @param  None
 *
 * @return None
 ******************************************************************************/
static inline void print_tx_logs(void)
{
  uint16_t packets_sent = range_test_measurement.packets_sent;
  uint16_t packets_repeat_number = range_test_settings.packets_repeat_number;
  uint8_t source_id = range_test_settings.source_id;
  uint8_t destination_id = range_test_settings.destination_id;

  app_log_info("Sent, Actual:%u, Max:%u, IdS:%u, IdR:%u\n",
               packets_sent,
               packets_repeat_number,
               source_id,
               destination_id);
}

/*******************************************************************************
 * @brief  Prints out the RX side of the logs
 *
 * @param  None
 *
 * @return None
 ******************************************************************************/
static inline void print_rx_logs(void)
{
  if (RANGETEST_PACKET_COUNT_INVALID != range_test_measurement.packets_received_correctly ) {
    range_test_measurement_t range_test_measurement_buf = range_test_measurement;
    range_test_settings_t range_test_settings_buf = range_test_settings;
    app_log_error(
      "Rcvd, "          //6
      "OK:%u, "    //10
      "CRC:%u, "    //11
      "Sent:%u, "    //12
      "Payld:%u, "    //10
      "MASize:%u, "    //12
      "PER:%3.1f, "    //11
      "MA:%3.1f, "    //10
      "RSSI:% 3d, "    //12
      "IdS:%u, "    //8
      "IdR:%u"    //8
      "\n",    //1+1
      range_test_measurement_buf.packets_received_correctly,
      range_test_measurement_buf.packets_with_crc_error,
      range_test_measurement_buf.packets_received_counter,
      range_test_settings_buf.payload_length,
      range_test_settings_buf.moving_average_window_size,
      range_test_measurement_buf.PER,
      range_test_measurement_buf.moving_average,
      range_test_measurement_buf.rssi_latch_value,
      range_test_settings_buf.source_id,
      range_test_settings_buf.destination_id);
  }
}

#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FAN_1_0) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_HAN) || defined(RAIL0_CHANNELS_FOR_915_PROFILE_WISUN)
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  sl_flex_802154_packet_mhr_frame_t rx_mhr = { 0 };
  uint16_t payload_size = 0;
  uint8_t rx_phr_config = 0U;
  RAIL_CopyRxPacket(rx_destination, packet_information);

  *start_of_payload = sl_flex_802154_packet_unpack_g_opt_data_frame(&rx_phr_config,
                                                                    &rx_mhr,
                                                                    &payload_size,
                                                                    rx_destination);
  return ((payload_size > (RAIL_RX_BUFFER_SIZE - SL_FLEX_IEEE802154_MHR_LENGTH)) ? (RAIL_RX_BUFFER_SIZE - SL_FLEX_IEEE802154_MHR_LENGTH) : payload_size);
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  uint16_t packet_size = 0U;
  uint8_t tx_phr_config = SL_FLEX_IEEE802154G_PHR_MODE_SWITCH_OFF
                          | SL_FLEX_IEEE802154G_PHR_CRC_4_BYTE
                          | SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON;
  sl_flex_802154_packet_mhr_frame_t tx_mhr = {
    .frame_control          = MAC_FRAME_TYPE_DATA                \
                              | MAC_FRAME_FLAG_PANID_COMPRESSION \
                              | MAC_FRAME_DESTINATION_MODE_SHORT \
                              | MAC_FRAME_VERSION_2006           \
                              | MAC_FRAME_SOURCE_MODE_SHORT,
    .sequence_number        = 0U,
    .destination_pan_id     = (0xFFFF),
    .destination_address    = (0xFFFF),
    .source_address         = (0x0000)
  };
  uint8_t tx_frame_buffer[256];
  sl_flex_802154_packet_pack_g_opt_data_frame(tx_phr_config,
                                              &tx_mhr,
                                              length,
                                              out_data,
                                              &packet_size,
                                              tx_frame_buffer);
  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, tx_frame_buffer, packet_size, true);
  app_assert(bytes_writen_in_fifo == packet_size,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             packet_size);
}
#elif defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM)
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  uint16_t payload_size = 0U;
  uint8_t rate = 0U;
  uint8_t scrambler = 0U;

  RAIL_CopyRxPacket(rx_destination, packet_information);
  *start_of_payload = sl_flex_802154_packet_unpack_ofdm_data_frame(packet_information,
                                                                   &rate,
                                                                   &scrambler,
                                                                   &payload_size,
                                                                   rx_destination);
  return payload_size;
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  uint16_t packet_size = 0U;
  uint8_t tx_frame_buffer[256];
  uint8_t rate = 0x06;     // rate: 5 bits wide, The Rate field (RA4-RA0) specifies the data rate of the payload and is equal to the numerical value of the MCS
                           // 0x0 BPSK, coding rate 1/2, 4 x frequency repetition
                           // 0x1 BPSK, coding rate 1/2, 2 x frequency repetition
                           // 0x2 QPSK, coding rate 1/2, 2 x frequency repetition
                           // 0x3 QPSK, coding rate 1/2
                           // 0x4 QPSK, coding rate 3/4
                           // 0x5 16-QAM, coding rate 1/2
                           // 0x6 16-QAM, coding rate 3/4
  uint8_t scrambler = 0; // scrambler: 2 bits wide, The Scrambler field (S1-S0) specifies the scrambling seed

  sl_flex_802154_packet_pack_ofdm_data_frame(rate,
                                             scrambler,
                                             length,
                                             out_data,
                                             &packet_size,
                                             tx_frame_buffer);
  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, tx_frame_buffer, packet_size, true);
  app_assert(bytes_writen_in_fifo == packet_size,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             packet_size);
}
#elif defined(RAIL0_CHANNEL_GROUP_1_PROFILE_SUN_OQPSK)
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  uint16_t payload_size = 0U;
  bool spreadingMode = false;
  uint8_t rateMode = 0U;

  RAIL_CopyRxPacket(rx_destination, packet_information);
  *start_of_payload = sl_flex_802154_packet_unpack_oqpsk_data_frame(packet_information,
                                                                    &spreadingMode,
                                                                    &rateMode,
                                                                    &payload_size,
                                                                    rx_destination);
  return payload_size;
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  uint16_t packet_size = 0U;
  uint8_t tx_frame_buffer[256];
  bool spreadingMode = false;
  uint8_t rateMode = 0; // rateMode: 2 bits wide

  sl_flex_802154_packet_pack_oqpsk_data_frame(spreadingMode,
                                              rateMode,
                                              length,
                                              out_data,
                                              &packet_size,
                                              tx_frame_buffer);

  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, tx_frame_buffer, packet_size, true);
  app_assert(bytes_writen_in_fifo == packet_size,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             packet_size);
}
#else
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  RAIL_CopyRxPacket(rx_destination, packet_information);
  *start_of_payload = rx_destination;
  return ((packet_information->packetBytes > RAIL_RX_BUFFER_SIZE) ? RAIL_RX_BUFFER_SIZE : packet_information->packetBytes);
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, out_data, length, true);
  app_assert(bytes_writen_in_fifo == length,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             length);
}
#endif
