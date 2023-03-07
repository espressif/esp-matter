/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app/framework/include/af.h"
#include "stack/include/mfglib.h"
#include "sl_cli.h"
#include "sl_cli_handles.h"
#include "StandardizedRfTesting_config.h"

#if SL_SRFT_HIGH_CHANNEL < SL_SRFT_LOW_CHANNEL
  #error SL_SRFT_HIGH_CHANNEL may not be less than SL_SRFT_LOW_CHANNEL
#endif

//////////////////////////////////////////////
// Defines and variables
//////////////////////////////////////////////

// Command frame headers
#define BYTE_SIGNATURE_FIRST  0xDE
#define BYTE_SIGNATURE_SECOND 0xDE
#define BYTE_TYPE_COMMAND 0x00
#define BYTE_TYPE_DATA    0x01

// Custom commands
#define BYTE_COMMAND_PING                     0x00
#define BYTE_COMMAND_ACK                      0x01
#define BYTE_COMMAND_SET_CHANNEL              0x02
#define BYTE_COMMAND_SET_POWER                0x05
#define BYTE_COMMAND_GET_POWER                0x06
#define BYTE_COMMAND_GET_POWER_RESPONSE       0x07
#define BYTE_COMMAND_STREAM                   0x09
#define BYTE_COMMAND_START_RX_TEST            0x0A
#define BYTE_COMMAND_END_RX_TEST              0x0B
#define BYTE_COMMAND_REPORT                   0x0C
#define BYTE_GET_HARDWARE_VERSION             0x11
#define BYTE_GET_HARDWARE_VERSION_RESPONSE    0x12
#define BYTE_GET_SOFTWARE_VERSION             0x13
#define BYTE_GET_SOFTWARE_VERSION_RESPONSE    0x14

// Custom silabs commands
#define BYTE_COMMAND_GET_CHANNEL_RESPONSE     0x04
#define BYTE_COMMAND_REBOOT                   0x0D
#define BYTE_COMMAND_GET_REBOOT_BYTE_RESPONSE 0x10

// Buffers
#define TX_CMD_BUFF_LENGTH 30
#define RX_CMD_BUFF_LENGTH 30

enum {
  STATE_LOCAL_INIT,
  STATE_LOCAL_PINGING,
  STATE_LOCAL_REBOOTING_DELAYING,
  STATE_LOCAL_REBOOTING_PINGING,
  STATE_LOCAL_CHANGING_CHANNEL_DELAYING,
  STATE_LOCAL_CHANGING_CHANNEL_PINGING,
  STATE_LOCAL_FINDING_DUT_DELAYING,
  STATE_LOCAL_FINDING_DUT_PINGING,
  STATE_LOCAL_STREAMING,
  STATE_LOCAL_GETTING_VERSION,
  STATE_LOCAL_GETTING_HW_VERSION,
  STATE_LOCAL_SENDING_HW_VERSION,
  STATE_LOCAL_GETTING_SW_VERSION,
  STATE_LOCAL_SENDING_SW_VERSION,
};

ALIGNMENT(4) // alignment to avoid MG22 crash.
static uint8_t txCmdBuff[TX_CMD_BUFF_LENGTH];
static uint32_t numPackets = 0;
static uint32_t numProtocolPackets = 0;
static uint32_t totalLqi = 0;
static uint32_t totalRssi = 0;

static uint8_t deviceState;

// for passing to events
static uint8_t  globalChannelMaskByte0;
static uint8_t  globalChannelMaskByte1;
static uint8_t  globalChannelMaskByte2;
static uint8_t  globalChannelMaskByte3;
static int8_t   globalPower;
static uint8_t  globalDuration0;
static uint8_t  globalDuration1;
static uint8_t  globalRebootByte;

// for transmitting
static uint16_t globalNumPackets;
static uint16_t globalTotalNumPackets;

// for rping timeout (in ms)
static uint16_t globalRpingTimeout = SL_SRFT_PING_TIMEOUT_MS;

//-------
// Events

static sl_zigbee_event_t send_ping_event;
static sl_zigbee_event_t send_ack_event;
static sl_zigbee_event_t send_set_channel_event;
static sl_zigbee_event_t send_get_channel_response_event;
static sl_zigbee_event_t send_set_power_event;
static sl_zigbee_event_t send_get_power_event;
static sl_zigbee_event_t send_get_power_response_event;
static sl_zigbee_event_t send_stream_event;
static sl_zigbee_event_t send_start_rx_test_event;
static sl_zigbee_event_t send_end_rx_test_event;
static sl_zigbee_event_t send_report_event;
static sl_zigbee_event_t send_reboot_event;
static sl_zigbee_event_t send_get_reboot_byte_response_event;
static sl_zigbee_event_t send_get_hardware_version_event;
static sl_zigbee_event_t send_get_hardware_version_response_event;
static sl_zigbee_event_t send_get_software_version_event;
static sl_zigbee_event_t send_get_software_version_response_event;
static sl_zigbee_event_t ping_timer_event;
static sl_zigbee_event_t set_channel_event;
static sl_zigbee_event_t change_channel_with_ping_event;
static sl_zigbee_event_t reboot_with_ping_event;
static sl_zigbee_event_t find_with_ping_event;
static sl_zigbee_event_t stream_timer_event;
static sl_zigbee_event_t custom_1_event; // rebootControl
static sl_zigbee_event_t custom_2_event;
static sl_zigbee_event_t custom_3_event;
static sl_zigbee_event_t custom_4_event;
static sl_zigbee_event_t custom_5_event;
static sl_zigbee_event_t custom_6_event;
static sl_zigbee_event_t init_event; //for initialization

//////////////////////////////////////////////
// Forward declarations
//////////////////////////////////////////////

static void mfglibRxHandler(uint8_t *packet, uint8_t linkQuality, int8_t rssi);

// Helper functions
static void helper_send_ack(void);
static void helper_local_channel_with_power(uint8_t channel);
static uint8_t helper_from_channel_mask_to_channel(uint32_t channelMask);
static void helper_set_channel_from_mask(uint32_t channelMask);
static void helper_from_channel_mask_to_global_channel_mask_bytes(uint32_t channelMask);
static void helper_send_set_channel(void);
static uint32_t helper_from_channel_to_channel_mask(uint8_t channel);
static void helper_stream(uint16_t timeout);
static void helper_init(void);
static void helper_set_power(void);
static void helper_local_reboot(void);
static void helper_send_reboot(void);
static int8_t helper_get_tx_power(uint8_t channel);
static void helper_app_init(void);
static void helper_run_events(void);

//////////////////////////////////////////////
// Event handlers
//////////////////////////////////////////////

static void send_ping_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_PING;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("ping (st 0x%02X)\r\n", status);
}

static void send_ack_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_ACK;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("ack (st 0x%02X)\r\n", status);
}

static void send_set_channel_event_handler(sl_zigbee_event_t *event)
{
  helper_send_set_channel();
}

static void send_get_channel_response_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 10; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_GET_CHANNEL_RESPONSE;
  txCmdBuff[5] = globalChannelMaskByte0;
  txCmdBuff[6] = globalChannelMaskByte1;
  txCmdBuff[7] = globalChannelMaskByte2;
  txCmdBuff[8] = globalChannelMaskByte3;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get ch resp (st 0x%02X)\r\n", status);
}

static void send_set_power_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 7; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_SET_POWER;
  txCmdBuff[5] = globalPower;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("set pow (st 0x%02X)\r\n", status);
}

static void send_get_power_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_GET_POWER;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get pow (st 0x%02X)\r\n", status);
}

static void send_get_power_response_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 7; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_GET_POWER_RESPONSE;
  txCmdBuff[5] = globalPower;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get pow resp (st 0x%02X)\r\n", status);
}

static void send_stream_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 8; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_STREAM;
  txCmdBuff[5] = globalDuration0;
  txCmdBuff[6] = globalDuration1;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("stream (st 0x%02X)\r\n", status);
}

static void send_start_rx_test_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_START_RX_TEST;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("start rx test (st 0x%02X)\r\n", status);
}

static void send_end_rx_test_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_END_RX_TEST;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("end rx test (st 0x%02X)\r\n", status);
}

static void send_report_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 22; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_REPORT;
  txCmdBuff[5] = BYTE_3(numPackets);
  txCmdBuff[6] = BYTE_2(numPackets);
  txCmdBuff[7] = BYTE_1(numPackets);
  txCmdBuff[8] = BYTE_0(numPackets);
  txCmdBuff[9] = BYTE_3(numProtocolPackets);
  txCmdBuff[10] = BYTE_2(numProtocolPackets);
  txCmdBuff[11] = BYTE_1(numProtocolPackets);
  txCmdBuff[12] = BYTE_0(numProtocolPackets);
  txCmdBuff[13] = BYTE_3(totalLqi);
  txCmdBuff[14] = BYTE_2(totalLqi);
  txCmdBuff[15] = BYTE_1(totalLqi);
  txCmdBuff[16] = BYTE_0(totalLqi);
  txCmdBuff[17] = BYTE_3(totalRssi);
  txCmdBuff[18] = BYTE_2(totalRssi);
  txCmdBuff[19] = BYTE_1(totalRssi);
  txCmdBuff[20] = BYTE_0(totalRssi);

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("report (st 0x%02X)\r\n", status);

  //clear after sending report
  helper_init();
}

static void send_reboot_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_REBOOT;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("reboot (st 0x%02X)\r\n", status);
}

static void send_get_reboot_byte_response_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 7; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_GET_REBOOT_BYTE_RESPONSE;
  txCmdBuff[5] = globalRebootByte;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get reboot byte (st 0x%02X)\r\n", status);
}

static void send_get_hardware_version_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_GET_HARDWARE_VERSION;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get hw ver (st 0x%02X)\r\n", status);
}

static void send_get_hardware_version_response_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  deviceState = STATE_LOCAL_INIT;

  txCmdBuff[0] = 7; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_GET_HARDWARE_VERSION_RESPONSE;
  txCmdBuff[5] = SL_SRFT_DEVICE_HW_VERSION;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get hw ver resp (st 0x%02X)\r\n", status);
}

static void send_get_software_version_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  deviceState = STATE_LOCAL_INIT;

  txCmdBuff[0] = 6; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_GET_SOFTWARE_VERSION;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get sw ver (st 0x%02X)\r\n", status);
}

static void send_get_software_version_response_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 7; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_GET_SOFTWARE_VERSION_RESPONSE;
  txCmdBuff[5] = SL_SRFT_DEVICE_SW_VERSION;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("get sw ver resp (st 0x%02X)\r\n", status);
}

static void ping_timer_event_handler(sl_zigbee_event_t *event)
{
  deviceState = STATE_LOCAL_INIT;
  sl_zigbee_app_debug_print("NO PING ACK\r\n");
}

static void set_channel_event_handler(sl_zigbee_event_t *event)
{
  uint8_t byte0 = globalChannelMaskByte0;
  uint8_t byte1 = globalChannelMaskByte1;
  uint8_t byte2 = globalChannelMaskByte2;
  uint8_t byte3 = globalChannelMaskByte3;
  uint32_t channelMask = (byte0 << 24) + (byte1 << 16) + (byte2 << 8) + byte3;

  helper_set_channel_from_mask(channelMask);
}

static void change_channel_with_ping_event_handler(sl_zigbee_event_t *event)
{
  uint8_t byte0 = globalChannelMaskByte0;
  uint8_t byte1 = globalChannelMaskByte1;
  uint8_t byte2 = globalChannelMaskByte2;
  uint8_t byte3 = globalChannelMaskByte3;
  uint32_t channelMask = (byte0 << 24) + (byte1 << 16) + (byte2 << 8) + byte3;
  helper_set_channel_from_mask(channelMask);
  deviceState = STATE_LOCAL_CHANGING_CHANNEL_PINGING;
  sl_zigbee_event_set_active(&send_ping_event);
  sl_zigbee_event_set_delay_ms(&ping_timer_event, globalRpingTimeout);
}

static void reboot_with_ping_event_handler(sl_zigbee_event_t *event)
{
  helper_local_reboot();
  deviceState = STATE_LOCAL_REBOOTING_PINGING;
  sl_zigbee_event_set_active(&send_ping_event);
  sl_zigbee_event_set_delay_ms(&ping_timer_event, globalRpingTimeout);
}

static void find_with_ping_event_handler(sl_zigbee_event_t *event)
{
  deviceState = STATE_LOCAL_FINDING_DUT_PINGING;
  sl_zigbee_event_set_active(&send_ping_event);
  sl_zigbee_event_set_delay_ms(&ping_timer_event, globalRpingTimeout);
}

static void stream_timer_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;
  deviceState = STATE_LOCAL_INIT;
  status = mfglibStopStream();
  sl_zigbee_app_debug_print("stop stream (st 0x%02X)\r\n", status);
}

static void custom_1_event_handler(sl_zigbee_event_t *event)
{
  halReboot();
}

static void custom_2_event_handler(sl_zigbee_event_t *event)
{
  EmberStatus status;

  txCmdBuff[0] = 19; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = 0x09;
  txCmdBuff[2] = 0x08;
  txCmdBuff[3] = 0x01;
  txCmdBuff[4] = 0xEF;
  txCmdBuff[5] = 0xBE;
  txCmdBuff[6] = 0xA8;
  txCmdBuff[7] = 0xA6;
  txCmdBuff[8] = 0x00;
  txCmdBuff[9] = 0x01;
  txCmdBuff[10] = 0x02;
  txCmdBuff[11] = 0x03;
  txCmdBuff[12] = 0x04;
  txCmdBuff[13] = 0x05;
  txCmdBuff[14] = 0x06;
  txCmdBuff[15] = 0x07;
  txCmdBuff[16] = 0x08;
  txCmdBuff[17] = 0x09;

  status = mfglibSendPacket(txCmdBuff, 0);

  /* Check for error */
  if (status != EMBER_SUCCESS) {
    /* If error, print status and don't arm event */
    sl_zigbee_app_debug_print("mfglib TX failure, status=0x%02X\r\n", status);
    return;
  }

  globalNumPackets++;

  if (globalNumPackets >= globalTotalNumPackets) {
    sl_zigbee_app_debug_print("TX complete, 0x%x packets sent\r\n", globalNumPackets);
  } else {
    sl_zigbee_event_set_delay_ms(&custom_2_event, 100);
  }
}

static void custom_3_event_handler(sl_zigbee_event_t *event)
{
}

static void custom_4_event_handler(sl_zigbee_event_t *event)
{
}

static void custom_5_event_handler(sl_zigbee_event_t *event)
{
}

static void custom_6_event_handler(sl_zigbee_event_t *event)
{
}

/* This function allows us to delay init after startup to give the radio a chance to initialize */
static void init_event_handler(sl_zigbee_event_t *event)
{
  helper_app_init();
}

//////////////////////////////////////////////
// helper functions
//////////////////////////////////////////////

EmberStatus mfglibStart(void (*)(uint8_t *, uint8_t, int8_t));

static void helper_app_init(void)
{
  EmberStatus status;
  uint8_t channel;
  int8_t txPower;

  status = mfglibStart(mfglibRxHandler);
  sl_zigbee_app_debug_print("INIT: mfglib start (st 0x%02X)\r\n", status);
  helper_local_channel_with_power(SL_SRFT_DEFAULT_CHANNEL);
  channel = mfglibGetChannel();
  sl_zigbee_app_debug_print("INIT: ch 0x%02X\r\n", channel);
  txPower = mfglibGetPower();
  sl_zigbee_app_debug_print("INIT: pow 0x%02x\r\n", txPower);

  deviceState = STATE_LOCAL_INIT;
}

static void helper_run_events(void)
{
  sli_zigbee_run_events();
}

static void helper_send_ack(void)
{
  sl_zigbee_event_set_active(&send_ack_event);
}

static int8_t helper_get_tx_power(uint8_t channel)
{
  int8_t txPower = 0;
  switch (channel) {
    case 11:
      txPower = SL_SRFT_TX_POWER_CHANNEL_11;
      break;
    case 12:
      txPower = SL_SRFT_TX_POWER_CHANNEL_12;
      break;
    case 13:
      txPower = SL_SRFT_TX_POWER_CHANNEL_13;
      break;
    case 14:
      txPower = SL_SRFT_TX_POWER_CHANNEL_14;
      break;
    case 15:
      txPower = SL_SRFT_TX_POWER_CHANNEL_15;
      break;
    case 16:
      txPower = SL_SRFT_TX_POWER_CHANNEL_16;
      break;
    case 17:
      txPower = SL_SRFT_TX_POWER_CHANNEL_17;
      break;
    case 18:
      txPower = SL_SRFT_TX_POWER_CHANNEL_18;
      break;
    case 19:
      txPower = SL_SRFT_TX_POWER_CHANNEL_19;
      break;
    case 20:
      txPower = SL_SRFT_TX_POWER_CHANNEL_20;
      break;
    case 21:
      txPower = SL_SRFT_TX_POWER_CHANNEL_21;
      break;
    case 22:
      txPower = SL_SRFT_TX_POWER_CHANNEL_22;
      break;
    case 23:
      txPower = SL_SRFT_TX_POWER_CHANNEL_23;
      break;
    case 24:
      txPower = SL_SRFT_TX_POWER_CHANNEL_24;
      break;
    case 25:
      txPower = SL_SRFT_TX_POWER_CHANNEL_25;
      break;
    case 26:
      txPower = SL_SRFT_TX_POWER_CHANNEL_26;
      break;
  }

  return txPower;
}

static void helper_local_channel_with_power(uint8_t channel)
{
  EmberStatus status;
  int8_t txPower = helper_get_tx_power(channel);

  status = mfglibSetChannel(channel);
  sl_zigbee_app_debug_print("ch %x (st 0x%02X)\r\n", channel, status);

  status = mfglibSetPower(SL_SRFT_TX_POWER_MODE, txPower);
  sl_zigbee_app_debug_print("pow %d (st 0x%02X)\r\n", txPower, status);
}

// the index of a set bit within the channelMask correspond
// to the actual channel.
// e.g. 0x800 is channel 11.
static uint8_t helper_from_channel_mask_to_channel(uint32_t channelMask)
{
  uint8_t i = 0;
  for (i = SL_SRFT_LOW_CHANNEL; i <= SL_SRFT_HIGH_CHANNEL; i++) {
    if (channelMask == (1U << i)) {
      return i;
    }
  }

  sl_zigbee_app_debug_print("invalid channel mask\r\n");
  return 0; /* No channels in mask */
}

static uint32_t helper_from_channel_to_channel_mask(uint8_t channel)
{
  if (SL_SRFT_LOW_CHANNEL <= channel && channel <= SL_SRFT_HIGH_CHANNEL) {
    return 1 << channel;
  } else {
    sl_zigbee_app_debug_print("invalid channel\r\n");
    return 0; /* No channels in mask */
  }
}

static void helper_set_channel_from_mask(uint32_t channelMask)
{
  uint8_t channel = helper_from_channel_mask_to_channel(channelMask);
  helper_local_channel_with_power(channel);
}

static void helper_from_channel_mask_to_global_channel_mask_bytes(uint32_t channelMask)
{
  globalChannelMaskByte0 = (channelMask & 0xFF000000) >> 24;
  globalChannelMaskByte1 = (channelMask & 0x00FF0000) >> 16;
  globalChannelMaskByte2 = (channelMask & 0x0000FF00) >> 8;
  globalChannelMaskByte3 = (channelMask & 0x000000FF);
}

static void helper_send_set_channel(void)
{
  EmberStatus status;

  txCmdBuff[0] = 10; // length does not include length byte / leave 2 for crc
  txCmdBuff[1] = BYTE_SIGNATURE_FIRST;
  txCmdBuff[2] = BYTE_SIGNATURE_SECOND;
  txCmdBuff[3] = BYTE_TYPE_COMMAND;
  txCmdBuff[4] = BYTE_COMMAND_SET_CHANNEL;
  txCmdBuff[5] = globalChannelMaskByte0;
  txCmdBuff[6] = globalChannelMaskByte1;
  txCmdBuff[7] = globalChannelMaskByte2;
  txCmdBuff[8] = globalChannelMaskByte3;

  status = mfglibSendPacket(txCmdBuff, 0);
  sl_zigbee_app_debug_print("set channel (st 0x%02X)\r\n", status);
}

static void helper_stream(uint16_t timeout)
{
  EmberStatus status;

  deviceState = STATE_LOCAL_STREAMING;

  status = mfglibStartStream();
  sl_zigbee_app_debug_print("stream (st 0x%02X)\r\n", status);

  // infinity if timeout is equal to 0
  if (timeout != 0) {
    sl_zigbee_event_set_delay_ms(&stream_timer_event, timeout);
  }
}

static void helper_init(void)
{
  numPackets = 0;
  numProtocolPackets = 0;
  totalLqi = 0;
  totalRssi = 0;
}

static void helper_set_power(void)
{
  EmberStatus status;

  status = mfglibSetPower(SL_SRFT_TX_POWER_MODE, globalPower);
  sl_zigbee_app_debug_print("pow (st 0x%02XX)\r\n", status);
}

static void helper_local_reboot(void)
{
  sl_zigbee_event_set_active(&custom_1_event);
}

static void helper_send_reboot(void)
{
  sl_zigbee_event_set_active(&send_reboot_event);
}

//////////////////////////////////////////////
// CLI commands
//////////////////////////////////////////////

// Commands
void sli_srft_cli_customRping(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customLpingtimeout(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSetchannel(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customLsetchannel(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRsetchannel(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customLgetchannel(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customLsetpower(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRsetpower(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customLgetpower(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRgetpower(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRstream(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRstart(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRend(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customReboot(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customLreboot(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRreboot(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customFind(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRhardwareversion(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customRsoftwareversion(sl_cli_command_arg_t *arguments);

// Silabs commands
void sli_srft_cli_customSilabsTest(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsGetLocalVersion(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsTest16(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsTest32(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsSetChannel(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsGetChannel(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsGetPower(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsTx (sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsListChannelPower(sl_cli_command_arg_t *arguments);
void sli_srft_cli_customSilabsLocalReport(sl_cli_command_arg_t *arguments);

/**
 * Custom CLI.  This command tree is executed by typing "custom <command>"
 * See the CLI component documentation for more detail on writing commands.
 **/
static const sl_cli_command_info_t srft_cli_cmd_customSilabsTest =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsTest,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsGetLocalVersion =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsGetLocalVersion,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsTest16 =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsTest16,
                 "",
                 "byte0" SL_CLI_UNIT_SEPARATOR "byte1" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsTest32 =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsTest32,
                 "",
                 "byte0" SL_CLI_UNIT_SEPARATOR "byte1" SL_CLI_UNIT_SEPARATOR "byte2" SL_CLI_UNIT_SEPARATOR "byte3" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsSetChannel =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsSetChannel,
                 "",
                 "byte0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsGetChannel =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsGetChannel,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsGetPower =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsGetPower,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsTx =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsTx,
                 "",
                 "byte0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsListChannelPower =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsListChannelPower,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_info_t srft_cli_cmd_customSilabsLocalReport =
  SL_CLI_COMMAND(sli_srft_cli_customSilabsLocalReport,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_entry_t srft_cli_custom_silabs_command_table[] = {
  { "test", &srft_cli_cmd_customSilabsTest, false },
  { "lver", &srft_cli_cmd_customSilabsGetLocalVersion, false },
  { "test16", &srft_cli_cmd_customSilabsTest16, false },
  { "test32", &srft_cli_cmd_customSilabsTest32, false },
  { "setchannel", &srft_cli_cmd_customSilabsSetChannel, false },
  { "getchannel", &srft_cli_cmd_customSilabsGetChannel, false },
  { "getpower", &srft_cli_cmd_customSilabsGetPower, false },
  { "tx", &srft_cli_cmd_customSilabsTx, false },
  { "listchpower", &srft_cli_cmd_customSilabsListChannelPower, false },
  { "lreport", &srft_cli_cmd_customSilabsLocalReport, false },
  { NULL, NULL, false },
};

static const sl_cli_command_info_t srft_cli_custom_silabs_group =
  SL_CLI_COMMAND_GROUP(srft_cli_custom_silabs_command_table, "Custom Silabs commands");

/* Sample Custom CLI commands */
//
// Reference for CLI command syntax:
// Zigbee Alliance 19-01701-00
// RF Performance (TRP/TIS) Test Plan & Specification Version 1.0
//
// Each of the following commands is preceded by a comment that shows the CLI command syntax as
// defined in the reference and optionally NOTEs any discrepancy or clarification.
//
// IMPORTANT:
//
// The CLI syntax specifies EACH BYTE of a multibyte parameter to be a SEPARATE
// command line argument, e.g. "<duration(ms):1> <duration(ms):0>" are the two bytes of a
// 16-bit value, and the numeric suffixes indicate the descending order from highest order
// byte to lowest order byte (0) of the multibyte value.
//
// IN CONTRAST to this, the numbering in the names of globalXxx variables indicates the
// order of the multibyte parameter arguments as they are encountered on the command line,
// in ascending order starting from zero. So for example, globalDuration0 contains the
// first duration argument <duration(ms):1>, and globalDuration1 contains the second
// duration argument <duration(ms):0>.
//

// custom rping
static const sl_cli_command_info_t srft_cli_cmd_customRping =
  SL_CLI_COMMAND(sli_srft_cli_customRping,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom lpingtimeout <timeout(ms):1> <timeout(ms):0>
static const sl_cli_command_info_t srft_cli_cmd_customLpingtimeout =
  SL_CLI_COMMAND(sli_srft_cli_customLpingtimeout,
                 "",
                 "timeout(ms):1" SL_CLI_UNIT_SEPARATOR "timeout(ms):0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

// custom setchannel <channel:3> <channel:2> <channel:1> <channel:0>
static const sl_cli_command_info_t srft_cli_cmd_customSetchannel =
  SL_CLI_COMMAND(sli_srft_cli_customSetchannel,
                 "",
                 "channel:3" SL_CLI_UNIT_SEPARATOR "channel:2" SL_CLI_UNIT_SEPARATOR "channel:1" SL_CLI_UNIT_SEPARATOR "channel:0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

// custom lsetchannel <channel:3> <channel:2> <channel:1> <channel:0>
static const sl_cli_command_info_t srft_cli_cmd_customLsetchannel =
  SL_CLI_COMMAND(sli_srft_cli_customLsetchannel,
                 "",
                 "channel:3" SL_CLI_UNIT_SEPARATOR "channel:2" SL_CLI_UNIT_SEPARATOR "channel:1" SL_CLI_UNIT_SEPARATOR "channel:0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

// custom rsetchannel <channel>
// NOTE: spec typo? args should be same as lsetchannel: <channel:3-0>.
//       The existing argument list reflected this.
static const sl_cli_command_info_t srft_cli_cmd_customRsetchannel =
  SL_CLI_COMMAND(sli_srft_cli_customRsetchannel,
                 "",
                 "channel:3" SL_CLI_UNIT_SEPARATOR "channel:2" SL_CLI_UNIT_SEPARATOR "channel:1" SL_CLI_UNIT_SEPARATOR "channel:0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

// custom lgetchannel
static const sl_cli_command_info_t srft_cli_cmd_customLgetchannel =
  SL_CLI_COMMAND(sli_srft_cli_customLgetchannel,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom lsetpower <mode:1> <mode:0> <power>
// NOTE: <mode:1-0> are ignored; <power> is signed 8-bit.
static const sl_cli_command_info_t srft_cli_cmd_customLsetpower =
  SL_CLI_COMMAND(sli_srft_cli_customLsetpower,
                 "mode is ignored; power is signed 8-bit",
                 "mode:1" SL_CLI_UNIT_SEPARATOR "mode:0" SL_CLI_UNIT_SEPARATOR "power" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_INT8,
  SL_CLI_ARG_END,
});

// custom rsetpower <mode:1> <mode:0> <power>
// NOTE: <mode:1-0> are ignored; <power> is signed 8-bit.
static const sl_cli_command_info_t srft_cli_cmd_customRsetpower =
  SL_CLI_COMMAND(sli_srft_cli_customRsetpower,
                 "mode is ignored; power is signed 8-bit",
                 "mode:1" SL_CLI_UNIT_SEPARATOR "mode:0" SL_CLI_UNIT_SEPARATOR "power" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_INT8,
  SL_CLI_ARG_END,
});

// custom lgetpower
static const sl_cli_command_info_t srft_cli_cmd_customLgetpower =
  SL_CLI_COMMAND(sli_srft_cli_customLgetpower,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom rgetpower
static const sl_cli_command_info_t srft_cli_cmd_customRgetpower =
  SL_CLI_COMMAND(sli_srft_cli_customRgetpower,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom rstream <duration(ms):1> <duration(ms):0>
static const sl_cli_command_info_t srft_cli_cmd_customRstream =
  SL_CLI_COMMAND(sli_srft_cli_customRstream,
                 "",
                 "duration(ms):1" SL_CLI_UNIT_SEPARATOR "duration(ms):0" SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_UINT8,
  SL_CLI_ARG_END,
});

// custom rstart
static const sl_cli_command_info_t srft_cli_cmd_customRstart =
  SL_CLI_COMMAND(sli_srft_cli_customRstart,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom rend
static const sl_cli_command_info_t srft_cli_cmd_customRend =
  SL_CLI_COMMAND(sli_srft_cli_customRend,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// NOTE: spec DOES NOT define a "custom reboot" command.
static const sl_cli_command_info_t srft_cli_cmd_customReboot =
  SL_CLI_COMMAND(sli_srft_cli_customReboot,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom lreboot
static const sl_cli_command_info_t srft_cli_cmd_customLreboot =
  SL_CLI_COMMAND(sli_srft_cli_customLreboot,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// NOTE: spec DOES NOT define a "custom rreboot" command.
static const sl_cli_command_info_t srft_cli_cmd_customRreboot =
  SL_CLI_COMMAND(sli_srft_cli_customRreboot,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom find
static const sl_cli_command_info_t srft_cli_cmd_customFind =
  SL_CLI_COMMAND(sli_srft_cli_customFind,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom rhardwareversion
static const sl_cli_command_info_t srft_cli_cmd_customRhardwareversion =
  SL_CLI_COMMAND(sli_srft_cli_customRhardwareversion,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

// custom rsoftwareversion
static const sl_cli_command_info_t srft_cli_cmd_customRsoftwareversion =
  SL_CLI_COMMAND(sli_srft_cli_customRsoftwareversion,
                 "",
                 SL_CLI_UNIT_SEPARATOR,
{
  SL_CLI_ARG_END,
});

static const sl_cli_command_entry_t srft_cli_custom_command_table[] = {
  { "rping", &srft_cli_cmd_customRping, false },
  { "lpingtimeout", &srft_cli_cmd_customLpingtimeout, false },
  { "setchannel", &srft_cli_cmd_customSetchannel, false },
  { "lsetchannel", &srft_cli_cmd_customLsetchannel, false },
  { "rsetchannel", &srft_cli_cmd_customRsetchannel, false },
  { "lgetchannel", &srft_cli_cmd_customLgetchannel, false },
  { "lsetpower", &srft_cli_cmd_customLsetpower, false },
  { "rsetpower", &srft_cli_cmd_customRsetpower, false },
  { "lgetpower", &srft_cli_cmd_customLgetpower, false },
  { "rgetpower", &srft_cli_cmd_customRgetpower, false },
  { "rstream", &srft_cli_cmd_customRstream, false },
  { "rstart", &srft_cli_cmd_customRstart, false },
  { "rend", &srft_cli_cmd_customRend, false },
  { "reboot", &srft_cli_cmd_customReboot, false },
  { "lreboot", &srft_cli_cmd_customLreboot, false },
  { "rreboot", &srft_cli_cmd_customRreboot, false },
  { "find", &srft_cli_cmd_customFind, false },
  { "rhardwareversion", &srft_cli_cmd_customRhardwareversion, false },
  { "rsoftwareversion", &srft_cli_cmd_customRsoftwareversion, false },
  // (dispatch to SiLabs supplementary commands, see srft_cli_custom_silabs_command_table above)
  { "silabs", &srft_cli_custom_silabs_group, false },
  { NULL, NULL, false },
};

static const sl_cli_command_info_t srft_cli_custom_group =
  SL_CLI_COMMAND_GROUP(srft_cli_custom_command_table, "Custom commands based on Zigbee RF Performance spec");

static const sl_cli_command_entry_t srft_cli_command_table[] = {
  { "custom", &srft_cli_custom_group, false },
  { NULL, NULL, false },
};

static sl_cli_command_group_t srft_cli_group = {
  NULL,
  false,
  srft_cli_command_table,
};

//////////////////////////////////////////////
// Command functions
//////////////////////////////////////////////

void sli_srft_cli_customRping(sl_cli_command_arg_t *arguments)
{
  deviceState = STATE_LOCAL_PINGING;
  sl_zigbee_event_set_active(&send_ping_event);
  sl_zigbee_event_set_delay_ms(&ping_timer_event, globalRpingTimeout);
}

void sli_srft_cli_customLpingtimeout(sl_cli_command_arg_t *arguments)
{
  uint8_t byte0 = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t byte1 = sl_cli_get_argument_uint8(arguments, 1);

  globalRpingTimeout = (byte0 << 8) + byte1;

  sl_zigbee_app_debug_print("globalRpingTimeout 0x%02X (%d)\r\n",
                            globalRpingTimeout,
                            globalRpingTimeout);
}

void sli_srft_cli_customSetchannel(sl_cli_command_arg_t *arguments)
{
  globalChannelMaskByte0 = sl_cli_get_argument_uint8(arguments, 0);
  globalChannelMaskByte1 = sl_cli_get_argument_uint8(arguments, 1);
  globalChannelMaskByte2 = sl_cli_get_argument_uint8(arguments, 2);
  globalChannelMaskByte3 = sl_cli_get_argument_uint8(arguments, 3);
  sl_zigbee_event_set_active(&send_set_channel_event);
  deviceState = STATE_LOCAL_CHANGING_CHANNEL_DELAYING;
  sl_zigbee_event_set_delay_ms(&change_channel_with_ping_event, 500);
}

void sli_srft_cli_customLsetchannel(sl_cli_command_arg_t *arguments)
{
  globalChannelMaskByte0 = sl_cli_get_argument_uint8(arguments, 0);
  globalChannelMaskByte1 = sl_cli_get_argument_uint8(arguments, 1);
  globalChannelMaskByte2 = sl_cli_get_argument_uint8(arguments, 2);
  globalChannelMaskByte3 = sl_cli_get_argument_uint8(arguments, 3);

  sl_zigbee_event_set_active(&set_channel_event);
}

void sli_srft_cli_customRsetchannel(sl_cli_command_arg_t *arguments)
{
  globalChannelMaskByte0 = sl_cli_get_argument_uint8(arguments, 0);
  globalChannelMaskByte1 = sl_cli_get_argument_uint8(arguments, 1);
  globalChannelMaskByte2 = sl_cli_get_argument_uint8(arguments, 2);
  globalChannelMaskByte3 = sl_cli_get_argument_uint8(arguments, 3);

  sl_zigbee_event_set_active(&send_set_channel_event);
}

void sli_srft_cli_customLgetchannel(sl_cli_command_arg_t *arguments)
{
  uint8_t localChannel = mfglibGetChannel();
  uint32_t channelMask = helper_from_channel_to_channel_mask(localChannel);
  helper_from_channel_mask_to_global_channel_mask_bytes(channelMask);

  sl_zigbee_app_debug_print("channel 0x%02X%02X%02X%02X\r\n",
                            globalChannelMaskByte0, globalChannelMaskByte1, globalChannelMaskByte2, globalChannelMaskByte3);
}

void sli_srft_cli_customLsetpower(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;

  // should use helper here
  // ignoring the <mode:1-0> params in CLI arguments 0-1
  int8_t pow = sl_cli_get_argument_int8(arguments, 2);
  status = mfglibSetPower(SL_SRFT_TX_POWER_MODE, pow);
  sl_zigbee_app_debug_print("pow (st 0x%02X)\r\n", status);
}

void sli_srft_cli_customRsetpower(sl_cli_command_arg_t *arguments)
{
  // ignoring the <mode:1-0> params in CLI arguments 0-1
  globalPower = sl_cli_get_argument_int8(arguments, 2);
  sl_zigbee_event_set_active(&send_set_power_event);
}

void sli_srft_cli_customLgetpower(sl_cli_command_arg_t *arguments)
{
  int8_t pow = mfglibGetPower();
  sl_zigbee_app_debug_print("power 0x%x\r\n", pow);
}

void sli_srft_cli_customRgetpower(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_event_set_active(&send_get_power_event);
}

void sli_srft_cli_customRstream(sl_cli_command_arg_t *arguments)
{
  globalDuration0 = sl_cli_get_argument_uint8(arguments, 0);
  globalDuration1 = sl_cli_get_argument_uint8(arguments, 1);

  sl_zigbee_event_set_active(&send_stream_event);
}

void sli_srft_cli_customRstart(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_event_set_active(&send_start_rx_test_event);
}

void sli_srft_cli_customRend(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_event_set_active(&send_end_rx_test_event);
}

void sli_srft_cli_customReboot(sl_cli_command_arg_t *arguments)
{
  helper_send_reboot();
  deviceState = STATE_LOCAL_REBOOTING_DELAYING;
  sl_zigbee_event_set_delay_ms(&reboot_with_ping_event, 500);
}

void sli_srft_cli_customLreboot(sl_cli_command_arg_t *arguments)
{
  helper_local_reboot();
}

void sli_srft_cli_customRreboot(sl_cli_command_arg_t *arguments)
{
  helper_send_reboot();
}

void sli_srft_cli_customFind(sl_cli_command_arg_t *arguments)
{
  uint8_t localChannel;

  localChannel = mfglibGetChannel();
  uint32_t channelMask = helper_from_channel_to_channel_mask(localChannel);
  helper_from_channel_mask_to_global_channel_mask_bytes(channelMask);

  helper_local_channel_with_power(11);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(12);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(13);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(14);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(15);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(16);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(17);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(18);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(19);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(20);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(21);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(22);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(23);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(24);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(25);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(26);
  helper_send_set_channel();
  helper_run_events();
  helper_local_channel_with_power(localChannel);
  deviceState = STATE_LOCAL_FINDING_DUT_DELAYING;
  sl_zigbee_event_set_active(&find_with_ping_event);
}

void sli_srft_cli_customRhardwareversion(sl_cli_command_arg_t *arguments)
{
  deviceState = STATE_LOCAL_GETTING_HW_VERSION;
  sl_zigbee_event_set_active(&send_get_hardware_version_event);
}

void sli_srft_cli_customRsoftwareversion(sl_cli_command_arg_t *arguments)
{
  deviceState = STATE_LOCAL_GETTING_SW_VERSION;
  sl_zigbee_event_set_active(&send_get_software_version_event);
}

void sli_srft_cli_customSilabsTest(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_app_debug_print("test1\r\n");
}

void sli_srft_cli_customSilabsGetLocalVersion(sl_cli_command_arg_t *arguments)
{
  sl_zigbee_app_debug_print("Local RF Application Version 0x%x%x\r\n",
                            SL_SRFT_APP_VERSION_MAJOR,
                            SL_SRFT_APP_VERSION_MINOR);
}

void sli_srft_cli_customSilabsTest16(sl_cli_command_arg_t *arguments)
{
  uint8_t byte0 = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t byte1 = sl_cli_get_argument_uint8(arguments, 1);
  uint16_t result = (byte0 << 8) + byte1;

  sl_zigbee_app_debug_print("customSilabsTest16 0x%02X 0x%02X 0x%04X %d\r\n",
                            byte0, byte1, result, result);
}

void sli_srft_cli_customSilabsTest32(sl_cli_command_arg_t *arguments)
{
  uint8_t byte0 = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t byte1 = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t byte2 = sl_cli_get_argument_uint8(arguments, 2);
  uint8_t byte3 = sl_cli_get_argument_uint8(arguments, 3);
  uint32_t result = (byte0 << 24) + (byte1 << 16) + (byte2 << 8) + byte3;

  sl_zigbee_app_debug_print("customSilabsTest32 0x%02X 0x%02X 0x%02X 0x%02XX 0x%08X %d\r\n",
                            byte0, byte1, byte2, byte3, result, result);
}

void sli_srft_cli_customSilabsSetChannel(sl_cli_command_arg_t *arguments)
{
  uint8_t tempChannel = sl_cli_get_argument_uint8(arguments, 0);
  helper_local_channel_with_power(tempChannel);
}

void sli_srft_cli_customSilabsGetChannel(sl_cli_command_arg_t *arguments)
{
  uint8_t channel = mfglibGetChannel();
  sl_zigbee_app_debug_print("ch 0x%x\r\n", channel);
}

void sli_srft_cli_customSilabsGetPower(sl_cli_command_arg_t *arguments)
{
  int8_t txPower = mfglibGetPower();
  sl_zigbee_app_debug_print("pow 0x%x\r\n", txPower);
}

void sli_srft_cli_customSilabsTx(sl_cli_command_arg_t *arguments)
{
  globalNumPackets = 0;
  globalTotalNumPackets = sl_cli_get_argument_uint16(arguments, 0);

  sl_zigbee_event_set_active(&custom_2_event);
}

void sli_srft_cli_customSilabsListChannelPower(sl_cli_command_arg_t *arguments)
{
  uint8_t channel;
  int8_t txPower;

  sl_zigbee_app_debug_print("\r\n");
  /* List default power value for all channels */
  for (channel = SL_SRFT_LOW_CHANNEL; channel <= SL_SRFT_HIGH_CHANNEL; channel++) {
    txPower = helper_get_tx_power(channel);
    sl_zigbee_app_debug_print("Default power level for channel %d is %d\r\n", channel, txPower);
  }
}

/* Local RX report - allows self-contained verification of RX sensitivity */
void sli_srft_cli_customSilabsLocalReport(sl_cli_command_arg_t *arguments)
{
  /* Print report */
  sl_zigbee_app_debug_print("Local Report: [total]0x%x [protocol]0x%x [totalLqi]0x%x [totalRssiMgnitude]0x%x\r\n",
                            numPackets, numProtocolPackets, totalLqi, totalRssi);

  //clear values after printing report
  helper_init();
}

//////////////////////////////////////////////
// Callbacks
//////////////////////////////////////////////

void app_init(void)
{
  // Initialize events
  sl_zigbee_event_init(&change_channel_with_ping_event, change_channel_with_ping_event_handler);
  sl_zigbee_event_init(&custom_1_event, custom_1_event_handler);
  sl_zigbee_event_init(&custom_2_event, custom_2_event_handler);
  sl_zigbee_event_init(&custom_3_event, custom_3_event_handler);
  sl_zigbee_event_init(&custom_4_event, custom_4_event_handler);
  sl_zigbee_event_init(&custom_5_event, custom_5_event_handler);
  sl_zigbee_event_init(&custom_6_event, custom_6_event_handler);
  sl_zigbee_event_init(&find_with_ping_event, find_with_ping_event_handler);
  sl_zigbee_event_init(&init_event, init_event_handler);
  sl_zigbee_event_init(&ping_timer_event, ping_timer_event_handler);
  sl_zigbee_event_init(&reboot_with_ping_event, reboot_with_ping_event_handler);
  sl_zigbee_event_init(&send_ack_event, send_ack_event_handler);
  sl_zigbee_event_init(&send_end_rx_test_event, send_end_rx_test_event_handler);
  sl_zigbee_event_init(&send_get_channel_response_event, send_get_channel_response_event_handler);
  sl_zigbee_event_init(&send_get_hardware_version_event, send_get_hardware_version_event_handler);
  sl_zigbee_event_init(&send_get_hardware_version_response_event, send_get_hardware_version_response_event_handler);
  sl_zigbee_event_init(&send_get_power_event, send_get_power_event_handler);
  sl_zigbee_event_init(&send_get_power_response_event, send_get_power_response_event_handler);
  sl_zigbee_event_init(&send_get_reboot_byte_response_event, send_get_reboot_byte_response_event_handler);
  sl_zigbee_event_init(&send_get_software_version_event, send_get_software_version_event_handler);
  sl_zigbee_event_init(&send_get_software_version_response_event, send_get_software_version_response_event_handler);
  sl_zigbee_event_init(&send_ping_event, send_ping_event_handler);
  sl_zigbee_event_init(&send_reboot_event, send_reboot_event_handler);
  sl_zigbee_event_init(&send_report_event, send_report_event_handler);
  sl_zigbee_event_init(&send_set_channel_event, send_set_channel_event_handler);
  sl_zigbee_event_init(&send_set_power_event, send_set_power_event_handler);
  sl_zigbee_event_init(&send_start_rx_test_event, send_start_rx_test_event_handler);
  sl_zigbee_event_init(&send_stream_event, send_stream_event_handler);
  sl_zigbee_event_init(&set_channel_event, set_channel_event_handler);
  sl_zigbee_event_init(&stream_timer_event, stream_timer_event_handler);

  for (uint8_t i = 0; i < sl_cli_handles_count; i++) {
    sl_cli_command_add_command_group(sl_cli_handles[i], &srft_cli_group);
  }

  sl_zigbee_event_set_delay_ms(&init_event, 1000); //initialize after a 1 sec delay
}

static void mfglibRxHandler(uint8_t *packet, uint8_t linkQuality, int8_t rssi)
{
  // if incoming is a command
  if ((packet[1] == BYTE_SIGNATURE_FIRST) && (packet[2] == BYTE_SIGNATURE_SECOND) && (packet[3] == BYTE_TYPE_COMMAND)) {
    if (packet[4] == BYTE_COMMAND_PING) {
      helper_send_ack();
    } else if (packet[4] == BYTE_COMMAND_ACK) {
      if (deviceState == STATE_LOCAL_PINGING) {
        deviceState = STATE_LOCAL_INIT;
        sl_zigbee_event_set_inactive(&ping_timer_event);
        sl_zigbee_app_debug_print("PING ACK\r\n");
      }
      if (deviceState == STATE_LOCAL_CHANGING_CHANNEL_PINGING) {
        deviceState = STATE_LOCAL_INIT;
        sl_zigbee_event_set_inactive(&ping_timer_event);
        sl_zigbee_app_debug_print("CHANNEL ACK\r\n");
      }
      if (deviceState == STATE_LOCAL_REBOOTING_PINGING) {
        deviceState = STATE_LOCAL_INIT;
        sl_zigbee_event_set_inactive(&ping_timer_event);
        sl_zigbee_app_debug_print("REBOOT ACK\r\n");
      }
      if (deviceState == STATE_LOCAL_FINDING_DUT_PINGING) {
        deviceState = STATE_LOCAL_INIT;
        sl_zigbee_event_set_inactive(&ping_timer_event);
        sl_zigbee_app_debug_print("FIND ACK\r\n");
      }
    } else if (packet[4] == BYTE_COMMAND_SET_CHANNEL) {
      globalChannelMaskByte0 = packet[5];
      globalChannelMaskByte1 = packet[6];
      globalChannelMaskByte2 = packet[7];
      globalChannelMaskByte3 = packet[8];

      sl_zigbee_event_set_active(&set_channel_event);
    } else if (packet[4] == BYTE_COMMAND_GET_CHANNEL_RESPONSE) {
    } else if (packet[4] == BYTE_COMMAND_SET_POWER) {
      // ignore the mode bytes and use the token!!
      globalPower = packet[5];
      helper_set_power();
    } else if (packet[4] == BYTE_COMMAND_GET_POWER) {
      globalPower = mfglibGetPower();
      sl_zigbee_event_set_active(&send_get_power_response_event);
    } else if (packet[4] == BYTE_COMMAND_GET_POWER_RESPONSE) {
      sl_zigbee_app_debug_print("POW 0x%02X\r\n", packet[5]);
    } else if (packet[4] == BYTE_COMMAND_STREAM) {
      uint16_t timeout = HIGH_LOW_TO_INT((uint8_t)packet[5], (uint8_t)packet[6]);
      helper_stream(timeout);
    } else if (packet[4] == BYTE_COMMAND_START_RX_TEST) {
      helper_init();
    } else if (packet[4] == BYTE_COMMAND_END_RX_TEST) {
      sl_zigbee_event_set_active(&send_report_event);
    } else if (packet[4] == BYTE_COMMAND_REPORT) {
      sl_zigbee_app_debug_print("[total]0x%02X%02X%02X%02X [protocol]0x%02X%02X%02X%02X [totalLqi]0x%02X%02X%02X%02X [totalRssiMgnitude]0x%02X%02X%02X%02X\r\n",
                                packet[5], packet[6], packet[7], packet[8], packet[9], packet[10], packet[11], packet[12],
                                packet[13], packet[14], packet[15], packet[16], packet[17], packet[18], packet[19], packet[20]);
    } else if (packet[4] == BYTE_COMMAND_REBOOT) {
      helper_local_reboot();
    } else if (packet[4] == BYTE_COMMAND_GET_REBOOT_BYTE_RESPONSE) {
    } else if (packet[4] == BYTE_GET_HARDWARE_VERSION) {
      deviceState = STATE_LOCAL_SENDING_HW_VERSION;
      sl_zigbee_event_set_active(&send_get_hardware_version_response_event);
    } else if (packet[4] == BYTE_GET_HARDWARE_VERSION_RESPONSE) {
      deviceState = STATE_LOCAL_INIT;
      sl_zigbee_app_debug_print("HW VER 0x%02X\r\n", packet[5]);
    } else if (packet[4] == BYTE_GET_SOFTWARE_VERSION) {
      deviceState = STATE_LOCAL_SENDING_SW_VERSION;
      sl_zigbee_event_set_active(&send_get_software_version_response_event);
    } else if (packet[4] == BYTE_GET_SOFTWARE_VERSION_RESPONSE) {
      deviceState = STATE_LOCAL_INIT;
      sl_zigbee_app_debug_print("SW VER 0x%02X\r\n", packet[5]);
    }
  } else {
    uint8_t rssiMagnitude;

    numPackets++;
    numProtocolPackets++;

    totalLqi = totalLqi + (uint32_t)linkQuality;

    //assume that rssi will be negative
    assert(rssi < 0);
    rssiMagnitude = 0 - rssi;
    totalRssi = totalRssi + (uint32_t)rssiMagnitude;
  }
}

/** @brief
 *
 * Application framework equivalent of ::emberRadioNeedsCalibratingHandler
 */
void emberAfRadioNeedsCalibratingCallback(void)
{
  sl_mac_calibrate_current_channel();
}
