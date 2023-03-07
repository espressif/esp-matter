/***************************************************************************//**
 * @file
 * @brief Direct Test Mode core logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdlib.h>
#include "dtm.h"
#include "rail_features.h"
#include "sl_bt_api.h"
#include "app_assert.h"
#include "dtm_config.h"

#if DTM_CONFIG_SPECIFY_TX_POWER == 1
#define DTM_DEFAULT_TX_POWER  DTM_CONFIG_DEFAULT_MAX_TX_POWER
#else
#define DTM_DEFAULT_TX_POWER  MAX_POW_LEVEL_REQ
#endif

#define MAX_PDU_OCTETS 255

#define MAX_POW_LEVEL_REQ 0x7F
#define MIN_POW_LEVEL_REQ 0x7E
#define MAX_POW_LEVEL 20
#define MIN_POW_LEVEL -127
#define MAX_POW_INDICATE_BIT 0x200
#define MIN_POW_INDICATE_BIT 0x100

#define T_MIN 6  // Inter byte timeout is 5 msec in spec, add 1 msec for tolerance

#define TRANSCEIVER_LENGTH_HIGH_MAX 3

#define BIT(value, bitpos) (value << (bitpos))

#define CONTROL_MASK 0x3F
#define CMD_PACKET_CONTROL_BYTE 0
#define CMD_PACKET_PARAM_BYTE 1
#define MAX_CTE_LENGTH 20 //160 us

#define CALC_MAX_PDU_TIME(octet_time, packet_overhead_time) (MAX_PDU_OCTETS * (octet_time) + (packet_overhead_time))

#define SUPPORTED_FEATURES (                                                   \
    BIT(1, FEATURE_PACKET_EXTENSION)                                           \
    | BIT(RAIL_BLE_SUPPORTS_2MBPS, FEATURE_2M_PHY)                             \
    | BIT(0, FEATURE_STABLE_MODULATION_INDEX)                                  \
    | BIT(RAIL_FEAT_BLE_CODED, FEATURE_CODED_PHY)                              \
    | BIT(RAIL_BLE_SUPPORTS_CTE, FEATURE_CTE_SUPPORTED)                        \
    | BIT(RAIL_BLE_SUPPORTS_ANTENNA_SWITCHING, FEATURE_ANTENNA_SWITCH_SUPPORT) \
    | BIT(RAIL_FEAT_BLE_AOX_SUPPORTED, FEATURE_1US_SAMPLING_AOD_TRANS_SUPPORT) \
    | BIT(RAIL_FEAT_BLE_AOX_SUPPORTED, FEATURE_1US_SAMPLING_AOD_REC_SUPPORT)   \
    | BIT(RAIL_FEAT_BLE_AOX_SUPPORTED, FEATURE_1US_SWITCH_SAMPLE_AOA_SUPPORT))

enum feature {
  FEATURE_PACKET_EXTENSION = 0,
  FEATURE_2M_PHY = 1,
  FEATURE_STABLE_MODULATION_INDEX = 2,
  FEATURE_CODED_PHY = 3,
  FEATURE_CTE_SUPPORTED = 4,
  FEATURE_ANTENNA_SWITCH_SUPPORT = 5,
  FEATURE_1US_SAMPLING_AOD_TRANS_SUPPORT = 6,
  FEATURE_1US_SAMPLING_AOD_REC_SUPPORT = 7,
  FEATURE_1US_SWITCH_SAMPLE_AOA_SUPPORT = 8
};

enum phy {
  PHY_NONE = 0,
  PHY_1M = 1,
  PHY_2M = 2,
  PHY_CODED_S_8 = 3,
  PHY_CODED_S_2 = 4,
  PHY_MAX
};

typedef struct {
  uint8_t data[2];
  uint8_t len;
  uint32_t last_byte_time;
} cmd_buffer_t;

typedef struct {
  uint8_t transceiver_length_high;
  uint8_t phy;
  uint8_t cte_present;
  uint8_t cte_time;
  uint8_t cte_type;
  uint8_t num_ant;
  uint8_t ant_array_length;
  uint8_t switch_pat;
  uint8_t *ant_array;
  uint8_t slot_duration;
  int16_t transmitter_power_level_dbm;
} setup_t;

typedef enum {
  CMD_TYPE_SETUP = 0,
  CMD_TYPE_RXTEST = 1,
  CMD_TYPE_TXTEST = 2,
  CMD_TYPE_TESTEND = 3,
  CMD_TYPE_MAX
} cmd_type_t;

typedef struct {
  uint8_t control;
  uint8_t parameter;
  uint8_t dc;
} setup_cmd_packet_t;

typedef struct {
  uint8_t frequency;
  uint8_t length;
  uint8_t pkt;
} transceiver_cmd_packet_t;

typedef struct {
  cmd_type_t cmd_type;
  union {
    setup_cmd_packet_t setup;
    transceiver_cmd_packet_t transceiver;
  } cmd;
} cmd_packet_t;

typedef struct {
  cmd_type_t transceiver_cmd;
} test_state_t;

enum setup_cmd {
  SETUP_CMD_RESET = 0,
  SETUP_CMD_TRANSCEIVER_LENGTH_HIGH = 1,
  SETUP_CMD_PHY = 2,
  SETUP_CMD_TX_MODULATION_INDEX = 3,
  SETUP_CMD_READ_SUPPORTED_FEATURES = 4,
  SETUP_CMD_READ_PDU_PARAMETERS = 5,
  SETUP_CMD_CTE_INFO = 6,
  SETUP_CMD_SLOT_DURATIONS = 7,
  SETUP_CMD_SWITCHING_PATERN = 8,
  SETUP_CMD_TRANSMITTER_POWER_LEVEL = 9
};

enum setup_parameter {
  STANDARD_MODULATION_INDEX = 0,
};

enum pdu_parameter {
  PDU_PARAMETER_MAX_TX_OCTETS = 0,
  PDU_PARAMETER_MAX_TX_TIME = 1,
  PDU_PARAMETER_MAX_RX_OCTETS = 2,
  PDU_PARAMETER_MAX_RX_TIME = 3,
  PDU_PARAMETER_MAX_CTE_LENGTH = 4
};

enum test_status {
  TEST_STATUS_SUCCESS = 0,
  TEST_STATUS_ERROR = 1,
};

// Enum of standard UART Test Interface command packet types
enum packet_type {
  PACKET_TYPE_PRBS9 = 0,     // binary 00 -> PRBS9 Packet Payload
  PACKET_TYPE_11110000 = 1,  // binary 00 -> 11110000 Packet Payload
  PACKET_TYPE_10101010 = 2,  // binary 10 -> 10101010 Packet Payload
  PACKET_TYPE_11111111 = 3,  // binary 11 -> On the LE Coded PHY: 11111111
  PACKET_TYPE_MAX
};

static testmode_config_t cfg;

static const uint16_t MAX_PDU_TIME[] = {
  [PHY_1M] = CALC_MAX_PDU_TIME(8, 80),
  [PHY_2M] = CALC_MAX_PDU_TIME(4, 44),
  [PHY_CODED_S_8] = CALC_MAX_PDU_TIME(64, 720),
  [PHY_CODED_S_2] = CALC_MAX_PDU_TIME(16, 462),
};

static const setup_t default_setup = {
  .transceiver_length_high = 0,
  .phy = PHY_1M,
  .cte_present = 0,
  .cte_time = 0,
  .cte_type = 0,
  .num_ant = 0,
  .ant_array_length = 0,
  .switch_pat = 0,
  .ant_array = 0,
  .slot_duration = 1,
  .transmitter_power_level_dbm = 0,
};

static cmd_buffer_t cmd_buffer;
static setup_t setup;
static test_state_t test_state;

// Array for mapping standard UART Test Interface command packet types to bgapi sl_bt_test_packet_type
static sl_bt_test_packet_type_t std_to_bgapi_pkt_types[PACKET_TYPE_MAX] = {
  [PACKET_TYPE_PRBS9] = sl_bt_test_pkt_prbs9,
  [PACKET_TYPE_11110000] = sl_bt_test_pkt_11110000,
  [PACKET_TYPE_10101010] = sl_bt_test_pkt_10101010,
  [PACKET_TYPE_11111111] = sl_bt_test_pkt_11111111
};

static void reset_setup(void);
static void reset_cmd_buffer(void);
static void reset_transceiver_test_state(void);
static void set_transceiver_test_state(cmd_type_t cmd);
static cmd_type_t get_transceiver_test_state(void);
static inline uint32_t t_min_in_ticks(void);
static void send_test_status(uint8_t status, uint16_t response);
static void send_packet_counter(uint16_t counter);
static void parse_cmd_buffer(cmd_packet_t *result);
static void process_setup_command(setup_cmd_packet_t *cmd);
static void process_transceiver_command(cmd_type_t cmd_type,
                                        transceiver_cmd_packet_t *cmd);
static void process_command(void);
static void handle_dtm_completed(sl_bt_msg_t *evt);
static void tx_power_set(int8_t tx_power_dbm,
                         int16_t *tx_power_out_dbm,
                         uint16_t *response,
                         uint8_t *status);

/**************************************************************************//**
 * Initialize testmode library.
 *****************************************************************************/
void testmode_init(const testmode_config_t *config)
{
  cfg = *config;
  reset_setup();
  reset_cmd_buffer();
  reset_transceiver_test_state();
}

/**************************************************************************//**
 * Test mode process command byte.
 *****************************************************************************/
void testmode_process_command_byte(uint8_t byte)
{
  if (cmd_buffer.len >= sizeof(cmd_buffer.data)) {
    // Processing previous command => ignore byte
    return;
  }

  uint32_t current_byte_time = cfg.get_ticks();

  if (cmd_buffer.len
      && current_byte_time - cmd_buffer.last_byte_time > t_min_in_ticks()) {
    // Inter byte timeout occurred
    reset_cmd_buffer();
  }

  cmd_buffer.last_byte_time = current_byte_time;
  cmd_buffer.data[cmd_buffer.len++] = byte;

  if (cmd_buffer.len == sizeof(cmd_buffer.data)) {
    sl_bt_external_signal(cfg.command_ready_signal);
  }
}

/**************************************************************************//**
 * Test mode handle Bluetooth events.
 *****************************************************************************/
void testmode_handle_gecko_event(sl_bt_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      // Set TX power to default, ignore output
      tx_power_set(DTM_DEFAULT_TX_POWER,
                   &setup.transmitter_power_level_dbm,
                   NULL,
                   NULL);
      break;
    case sl_bt_evt_system_external_signal_id:
      if (evt->data.evt_system_external_signal.extsignals & cfg.command_ready_signal) {
        process_command();
      }
      break;

    case sl_bt_evt_test_dtm_completed_id:
      handle_dtm_completed(evt);
      break;

    default:
      break;
  }
}

static void reset_setup(void)
{
  int16_t tx_power_dbm_save = setup.transmitter_power_level_dbm;
  setup = default_setup;
  setup.transmitter_power_level_dbm = tx_power_dbm_save;
}

static void reset_cmd_buffer(void)
{
  cmd_buffer.len = 0;
}

static void reset_transceiver_test_state(void)
{
  test_state.transceiver_cmd = CMD_TYPE_MAX;
}

static void set_transceiver_test_state(cmd_type_t cmd)
{
  test_state.transceiver_cmd = cmd;
}

static cmd_type_t get_transceiver_test_state(void)
{
  return test_state.transceiver_cmd;
}

static inline uint32_t t_min_in_ticks(void)
{
  return T_MIN * cfg.ticks_per_second / 1000;
}

static void send_test_status(uint8_t status, uint16_t response)
{
  response &= 0x3fff;

  uint8_t response_high = response >> 7;
  uint8_t response_low = response & 0x7f;

  cfg.write_response_byte(response_high);
  cfg.write_response_byte((response_low << 1) | (status ? TEST_STATUS_ERROR : TEST_STATUS_SUCCESS));
}

static void send_packet_counter(uint16_t counter)
{
  counter |= 0x8000;  // EV bit on
  cfg.write_response_byte(counter >> 8);
  cfg.write_response_byte(counter & 0xff);
}

static void parse_cmd_buffer(cmd_packet_t *result)
{
  result->cmd_type = (cmd_type_t)(cmd_buffer.data[CMD_PACKET_CONTROL_BYTE] >> 6);

  switch (result->cmd_type) {
    case CMD_TYPE_SETUP:
    case CMD_TYPE_TESTEND:
      result->cmd.setup.control = cmd_buffer.data[CMD_PACKET_CONTROL_BYTE] & CONTROL_MASK;
      result->cmd.setup.parameter = cmd_buffer.data[CMD_PACKET_PARAM_BYTE];
      result->cmd.setup.dc = cmd_buffer.data[CMD_PACKET_PARAM_BYTE] & 0x03;
      break;

    case CMD_TYPE_RXTEST:
    case CMD_TYPE_TXTEST:
      result->cmd.transceiver.frequency = cmd_buffer.data[CMD_PACKET_CONTROL_BYTE] & CONTROL_MASK;
      result->cmd.transceiver.length = cmd_buffer.data[CMD_PACKET_PARAM_BYTE] >> 2;
      result->cmd.transceiver.pkt = cmd_buffer.data[CMD_PACKET_PARAM_BYTE] & 0x03;
      break;
    default:
      break;
  }
}

/**************************************************************************//**
 * Set TX power in dBm
 *****************************************************************************/
static void tx_power_set(int8_t tx_power_dbm,
                         int16_t *tx_power_out_dbm,
                         uint16_t *response,
                         uint8_t *status)
{
  int16_t support_min;
  int16_t support_max;
  int16_t support_min_dbm;
  int16_t support_max_dbm;
  int16_t set_min;
  int16_t set_max;
  int16_t rf_path_gain;
  sl_status_t sc;
  int16_t tx_power_to_set_dbm = 0;

  if (response != NULL) {
    *response = 0;
  }
  sc = sl_bt_system_get_tx_power_setting(&support_min,
                                         &support_max,
                                         &set_min,
                                         &set_max,
                                         &rf_path_gain);

  app_assert_status(sc);

  support_min_dbm = (support_min) > 0 ? ((support_min) + 5) / 10 : ((support_min) - 5) / 10;
  support_max_dbm = (support_max) > 0 ? ((support_max) + 5) / 10 : ((support_max) - 5) / 10;

  if (tx_power_dbm == MIN_POW_LEVEL_REQ) {
    tx_power_to_set_dbm = support_min_dbm;
    if (response != NULL) {
      *response |= MIN_POW_INDICATE_BIT;
    }
  } else if (tx_power_dbm == MAX_POW_LEVEL_REQ) {
    tx_power_to_set_dbm = support_max_dbm;
    if (response != NULL) {
      *response |= MAX_POW_INDICATE_BIT;
    }
  } else if (tx_power_dbm > support_max_dbm) {
    if (status != NULL) {
      *status = TEST_STATUS_ERROR;
    }
    return;
  } else if (tx_power_dbm < support_min_dbm) {
    if (status != NULL) {
      *status = TEST_STATUS_ERROR;
    }
    return;
  } else {
    tx_power_to_set_dbm = (int16_t)tx_power_dbm;
  }

  sc = sl_bt_system_set_tx_power(support_min,
                                 tx_power_to_set_dbm * 10,
                                 &set_min,
                                 &set_max);
  app_assert_status(sc);

  // Set outputs
  if (response != NULL) {
    *response |= (uint8_t)(tx_power_to_set_dbm);
  }
  if (tx_power_out_dbm != NULL) {
    *tx_power_out_dbm = tx_power_to_set_dbm;
  }
}

static void process_setup_command(setup_cmd_packet_t *cmd)
{
  uint8_t n = 0;
  uint8_t i = 0;
  uint8_t j = 0;
  sl_status_t sc;
  int8_t tx_power = 0;
  uint8_t cte_time = 0;

  if (cmd->control == SETUP_CMD_RESET) {
    cmd->parameter = cmd->parameter >> 2;
    reset_setup();
    sc = sl_bt_test_dtm_end();
    app_assert_status(sc);
    /* Don't send test status here, because when DTM end is processed, a DTM
     * completed event is emitted, and the test status is sent in the event
     * handler. */
    return;
  }

  uint8_t status = TEST_STATUS_SUCCESS;
  uint16_t response = 0;

  switch (cmd->control) {
    case SETUP_CMD_PHY:
      cmd->parameter = cmd->parameter >> 2;
      if (cmd->parameter == PHY_NONE || cmd->parameter >= PHY_MAX) {
        status = TEST_STATUS_ERROR;
      } else {
        setup.phy = cmd->parameter;
      }
      break;

    case SETUP_CMD_TRANSCEIVER_LENGTH_HIGH:
      cmd->parameter = cmd->parameter >> 2;
      if (cmd->parameter > TRANSCEIVER_LENGTH_HIGH_MAX) {
        status = TEST_STATUS_ERROR;
      } else {
        setup.transceiver_length_high = cmd->parameter;
      }
      break;

    case SETUP_CMD_TX_MODULATION_INDEX:
      cmd->parameter = cmd->parameter >> 2;
      // only standard modulation index is supported.
      if (cmd->parameter != STANDARD_MODULATION_INDEX) {
        status = TEST_STATUS_ERROR;
      }
      break;

    case SETUP_CMD_READ_SUPPORTED_FEATURES:
      cmd->parameter = cmd->parameter >> 2;
      if (cmd->parameter != 0) {
        status = TEST_STATUS_ERROR;
      } else {
        response = SUPPORTED_FEATURES;
      }
      break;

    case SETUP_CMD_READ_PDU_PARAMETERS:
      cmd->parameter = cmd->parameter >> 2;
      switch (cmd->parameter) {
        case PDU_PARAMETER_MAX_TX_OCTETS:
        case PDU_PARAMETER_MAX_RX_OCTETS:
          response = MAX_PDU_OCTETS;
          break;

        case PDU_PARAMETER_MAX_TX_TIME:
        case PDU_PARAMETER_MAX_RX_TIME:

          if (RAIL_FEAT_BLE_CODED) {
            response = MAX_PDU_TIME[PHY_CODED_S_8] / 2;
          } else {
            response = MAX_PDU_TIME[PHY_1M] / 2;
          }
          break;

        case PDU_PARAMETER_MAX_CTE_LENGTH:
          response = MAX_CTE_LENGTH; //*8us
          break;
        default:
          status = TEST_STATUS_ERROR;
          break;
      }
      break;

    case SETUP_CMD_CTE_INFO:

      if (cmd->parameter != 0) {
        cte_time = (cmd->parameter & 0x1F);
        if (cte_time <= 20 && cte_time >= 2) {
          setup.cte_time = cte_time;
          setup.cte_present = 1;
          setup.cte_type = ((cmd->parameter >> 6) & 0x03);
        } else {
          status = TEST_STATUS_ERROR;
        }
      } else {
        setup.cte_present = 0;
        setup.cte_time = 0;
        setup.cte_type = 0;
      }
      break;

    case SETUP_CMD_SLOT_DURATIONS:
      if (cmd->parameter > 0x02 || cmd->parameter < 0x01) {
        status = TEST_STATUS_ERROR;
      } else {
        setup.slot_duration = (cmd->parameter & 0x03);
      }
      break;

    case SETUP_CMD_SWITCHING_PATERN:
      if (setup.ant_array != 0) {
        free(setup.ant_array);
      }

      setup.num_ant = (cmd->parameter & 0x3F);
      setup.switch_pat = ((cmd->parameter >> 7) & 0x01);

      if (setup.num_ant < 0x01 || setup.num_ant > 0x4B) {
        status = TEST_STATUS_ERROR;
      } else {
        if (setup.switch_pat) {
          setup.ant_array_length = (setup.num_ant * 2) - 1;
          setup.ant_array = (uint8_t *)calloc(setup.ant_array_length,
                                              sizeof(uint8_t));

          for (i = 0; i < setup.num_ant; i++) {
            setup.ant_array[i] = i;
          }
          for (j = i - 1; j < n; j++, i--) {
            setup.ant_array[j] = i;
          }
        } else {
          setup.ant_array_length = setup.num_ant;
          setup.ant_array = (uint8_t *)calloc(setup.ant_array_length,
                                              sizeof(uint8_t));

          for (i = 0; i < n; i++) {
            setup.ant_array[i] = i;
          }
        }
      }
      break;

    case SETUP_CMD_TRANSMITTER_POWER_LEVEL:

      tx_power = cmd->parameter;
      tx_power_set(tx_power,
                   &setup.transmitter_power_level_dbm,
                   &response,
                   &status);
      break;

    default:
      /* Unsupported setup command */
      status = TEST_STATUS_ERROR;
      break;
  }

  send_test_status(status, response);
  reset_cmd_buffer();
}

static void process_transceiver_command(cmd_type_t cmd_type,
                                        transceiver_cmd_packet_t *cmd)
{
  sl_status_t sc;
  uint16_t length = (setup.transceiver_length_high << 6) | cmd->length;

  switch (cmd_type) {
    case CMD_TYPE_RXTEST:

      #ifdef SL_CATALOG_BLUETOOTH_FEATURE_AOA_RECEIVER_PRESENT
      if (setup.cte_present) {
        if (setup.ant_array != 0) {
          sc = sl_bt_cte_receiver_set_dtm_parameters(setup.cte_time,
                                                     setup.cte_type,
                                                     setup.slot_duration,
                                                     setup.ant_array_length,
                                                     setup.ant_array);
          app_assert_status(sc);
        }
      } else {
        (void)sl_bt_cte_receiver_clear_dtm_parameters();
      }
      #endif // SL_CATALOG_BLUETOOTH_FEATURE_AOA_RECEIVER_PRESENT

      (void)sl_bt_test_dtm_rx(cmd->frequency, setup.phy);
      break;

    case CMD_TYPE_TXTEST:

      tx_power_set(setup.transmitter_power_level_dbm,
                   NULL,
                   NULL,
                   NULL);

      #ifdef SL_CATALOG_BLUETOOTH_FEATURE_AOA_TRANSMITTER_PRESENT
      if (setup.cte_present) {
        if (setup.ant_array != 0) {
          sc = sl_bt_cte_transmitter_set_dtm_parameters(setup.cte_time,
                                                        setup.cte_type,
                                                        setup.ant_array_length,
                                                        setup.ant_array);
          app_assert_status(sc);
        }
      } else {
        (void)sl_bt_cte_transmitter_clear_dtm_parameters();
      }
      #endif // SL_CATALOG_BLUETOOTH_FEATURE_AOA_TRANSMITTER_PRESENT

      // Do the test only if UART Test Interface command packet type is valid
      if (PACKET_TYPE_MAX > cmd->pkt) {
        sc = sl_bt_test_dtm_tx_v4(std_to_bgapi_pkt_types[cmd->pkt],
                                  length,
                                  cmd->frequency,
                                  setup.phy,
                                  setup.transmitter_power_level_dbm);
        app_assert_status(sc);
      }
      break;

    default:
      break;
  }
}

static void process_command(void)
{
  cmd_packet_t cmd_packet;
  parse_cmd_buffer(&cmd_packet);

  switch (cmd_packet.cmd_type) {
    case CMD_TYPE_SETUP:
      process_setup_command(&cmd_packet.cmd.setup);
      break;

    case CMD_TYPE_RXTEST:
    case CMD_TYPE_TXTEST:
      process_transceiver_command(cmd_packet.cmd_type,
                                  &cmd_packet.cmd.transceiver);
      set_transceiver_test_state(cmd_packet.cmd_type);
      break;

    case CMD_TYPE_TESTEND:
      (void)sl_bt_test_dtm_end();
      break;
    default:
      break;
  }
}

static void handle_dtm_completed(sl_bt_msg_t *evt)
{
  cmd_packet_t cmd_packet;
  parse_cmd_buffer(&cmd_packet);

  if (cmd_packet.cmd_type == CMD_TYPE_TESTEND) {
    if (get_transceiver_test_state() == CMD_TYPE_RXTEST) {
      send_packet_counter(evt->data.evt_test_dtm_completed.number_of_packets);
    } else {
      send_packet_counter(0);
    }
  } else {
    send_test_status(evt->data.evt_test_dtm_completed.result, 0);
  }

  // Command is executed and the response is sent => reset command buffer for next command
  reset_cmd_buffer();
  if (cmd_packet.cmd_type == CMD_TYPE_TESTEND) {
    reset_transceiver_test_state();
  }
}
