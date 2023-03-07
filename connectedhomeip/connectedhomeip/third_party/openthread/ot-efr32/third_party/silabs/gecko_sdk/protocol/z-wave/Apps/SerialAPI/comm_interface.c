/**
 * @file comm_interface.c
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include "comm_interface.h"
#include "zpal_uart.h"
#include "ZW_SerialAPI.h"
#include "SwTimer.h"
#include "utils.h"
#include <string.h>
#include "AppTimer.h"
#include "Assert.h"
#include "serial_api_config.h"

#define BUFFER_CHECK_TIME_MS    250
#define DEFAULT_ACK_TIMEOUT_MS  1500
#define DEFAULT_BYTE_TIMEOUT_MS 150
#define HEADER_LEN              4
#define ACK_LEN                 1
#define CRC_LEN                 1

#define COMM_INT_TX_BUFFER_SIZE 200
#define COMM_INT_RX_BUFFER_SIZE 200
#define TRANSMIT_BUFFER_SIZE    COMM_INT_TX_BUFFER_SIZE


typedef enum
{
  COMM_INTERFACE_STATE_SOF      = 0,
  COMM_INTERFACE_STATE_LEN      = 1,
  COMM_INTERFACE_STATE_TYPE     = 2,
  COMM_INTERFACE_STATE_CMD      = 3,
  COMM_INTERFACE_STATE_DATA     = 4,
  COMM_INTERFACE_STATE_CHECKSUM = 5,
} comm_interface_state_t;

typedef struct
{
  transport_t transport;
  SSwTimer ack_timer;
  bool ack_timeout;
  uint32_t ack_timeout_ms;
  SSwTimer byte_timer;
  bool byte_timeout;
  uint32_t byte_timeout_ms;
  SSwTimer buffer_check_timer;
  comm_interface_state_t state;
  uint8_t expect_bytes;
  bool ack_needed;
  uint8_t buffer_len;
  uint8_t buffer[RECEIVE_BUFFER_SIZE];
  bool rx_active;
  uint8_t rx_wait_count;
} comm_interface_t;

typedef struct
{
  uint8_t sof;
  uint8_t len;
  uint8_t type;
  uint8_t cmd;
  uint8_t payload[UINT8_MAX];
} tx_frame_t;


static comm_interface_t comm_interface;
comm_interface_frame_ptr const serial_frame = (comm_interface_frame_ptr)comm_interface.buffer;

static uint8_t tx_data[COMM_INT_TX_BUFFER_SIZE];
static uint8_t rx_data[COMM_INT_RX_BUFFER_SIZE];

static void set_expect_bytes(uint8_t level)
{
  vPortEnterCritical();

  if (zpal_uart_get_available(comm_interface.transport.handle) >= level)
  {
    comm_interface.expect_bytes = 0;
    TriggerNotification(EAPPLICATIONEVENT_SERIALDATARX);
  }
  else
  {
    comm_interface.expect_bytes = level;
  }

  vPortExitCritical();
}

static void receive_callback(const zpal_uart_handle_t handle, size_t available)
{
  UNUSED(handle);

  if (available >= comm_interface.expect_bytes)
  {
    comm_interface.expect_bytes = 0;
    TriggerNotification(EAPPLICATIONEVENT_SERIALDATARX);
  }
}

static void ack_timer_cb(SSwTimer *timer)
{
  UNUSED(timer);
  comm_interface.ack_timeout = true;
  TriggerNotification(EAPPLICATIONEVENT_SERIALTIMEOUT);
}

static void byte_timer_cb(SSwTimer *timer)
{
  UNUSED(timer);
  comm_interface.byte_timeout = true;
  TriggerNotification(EAPPLICATIONEVENT_SERIALTIMEOUT);
}

static void buffer_check_timer_cb(SSwTimer *timer)
{
  UNUSED(timer);

  if(zpal_uart_get_available(comm_interface.transport.handle))
  {
    TriggerNotification(EAPPLICATIONEVENT_SERIALDATARX);
  }
}

static uint8_t xor_checksum(uint8_t init, const uint8_t *data, uint8_t len)
{
  uint8_t checksum = init;

  for (int i = 0; i < len; i++)
  {
    checksum ^= data[i];
  }

  return checksum;
}

static zpal_status_t comm_interface_transmit(transport_t *transport, const uint8_t *data, size_t len, transmit_done_cb_t cb)
{
  if (transport)
  {
    switch (transport->type)
    {
      case TRANSPORT_TYPE_UART:
        return zpal_uart_transmit(transport->handle, data, len, cb);

      default:
        break;
    }
  }

  return ZPAL_STATUS_FAIL;
}

void comm_interface_transmit_frame(uint8_t cmd, uint8_t type, const uint8_t *payload, uint8_t len, transmit_done_cb_t cb)
{
  tx_frame_t frame;
  static uint8_t _len, _type, _cmd, _checksum;
  static const uint8_t *_payload;

  TimerStop(&comm_interface.ack_timer);
  TimerStop(&comm_interface.byte_timer);
  TimerStop(&comm_interface.buffer_check_timer);

  comm_interface.byte_timeout = false;
  comm_interface.ack_timeout = false;

  frame.sof = SOF;

  if (payload != NULL)
  {
    frame.len = len + 3;
    frame.type = type;
    frame.cmd = cmd;
    memcpy(frame.payload, payload, len);
    frame.payload[len] = xor_checksum(0xFF, &frame.len, frame.len);

    /* store input arguments for retransmission */
    _len = len;
    _type = type;
    _cmd = cmd;
    _payload = payload;
    _checksum = frame.payload[len];
  }
  else
  {
    /* retransmit last frame */
    frame.len = _len + 3;
    frame.type = _type;
    frame.cmd = _cmd;
    memcpy(frame.payload, _payload, _len);
    frame.payload[_len] = _checksum;
  }

  comm_interface.ack_needed = true;
  set_expect_bytes(ACK_LEN);
  comm_interface_transmit(&comm_interface.transport, (uint8_t *)&frame, frame.len + 2, cb);
  TimerStart(&comm_interface.ack_timer, comm_interface_get_ack_timeout_ms());
  TimerStart(&comm_interface.buffer_check_timer, BUFFER_CHECK_TIME_MS);
}

void comm_interface_wait_transmit_done(void)
{
  while(zpal_uart_transmit_in_progress(comm_interface.transport.handle));
}

void comm_interface_init(void)
{
  const zpal_uart_config_t uart_config =
  {
#if defined(SERIAL_API_TX_PIN)
    .tx_pin = SERIAL_API_TX_PIN,
#endif /* defined(SERIAL_API_TX_PIN) */
#if defined(SERIAL_API_TX_PORT)
    .tx_port = SERIAL_API_TX_PORT,
#endif /* defined(SERIAL_API_TX_PORT) */
#if defined(SERIAL_API_TX_LOC)
    .tx_loc = SERIAL_API_TX_LOC,
#endif /* defined(SERIAL_API_TX_LOC) */
#if defined(SERIAL_API_RX_PIN)
    .rx_pin = SERIAL_API_RX_PIN,
#endif /* defined(SERIAL_API_RX_PIN) */
#if defined(SERIAL_API_RX_PORT)
    .rx_port = SERIAL_API_RX_PORT,
#endif /* defined(SERIAL_API_RX_PORT) */
#if defined(SERIAL_API_RX_LOC)
    .rx_loc = SERIAL_API_RX_LOC,
#endif /* defined(SERIAL_API_RX_LOC) */
    .tx_buffer = tx_data,
    .tx_buffer_len = COMM_INT_TX_BUFFER_SIZE,
    .rx_buffer = rx_data,
    .rx_buffer_len = COMM_INT_RX_BUFFER_SIZE,
    .id = ZPAL_UART0,
    .baud_rate = 115200,
    .data_bits = 8,
    .parity_bit = ZPAL_UART_NO_PARITY,
    .stop_bits = ZPAL_UART_STOP_BITS_1,
    .receive_callback = receive_callback,
    .ptr = NULL,
  };

  comm_interface_set_ack_timeout_ms(DEFAULT_ACK_TIMEOUT_MS);
  comm_interface_set_byte_timeout_ms(DEFAULT_BYTE_TIMEOUT_MS);

  zpal_status_t status = zpal_uart_init(&uart_config, &comm_interface.transport.handle);
  ASSERT(status == ZPAL_STATUS_OK);
  status = zpal_uart_enable(comm_interface.transport.handle);
  ASSERT(status == ZPAL_STATUS_OK);
  comm_interface.transport.type = TRANSPORT_TYPE_UART;

  AppTimerRegister(&comm_interface.ack_timer, false, ack_timer_cb);
  TimerStop(&comm_interface.ack_timer);

  AppTimerRegister(&comm_interface.byte_timer, false, byte_timer_cb);
  TimerStop(&comm_interface.byte_timer);

  AppTimerRegister(&comm_interface.buffer_check_timer, true, buffer_check_timer_cb);
  TimerStop(&comm_interface.buffer_check_timer);

  comm_interface.state = COMM_INTERFACE_STATE_SOF;
  comm_interface.buffer_len = 0;
  set_expect_bytes(HEADER_LEN);
}

uint32_t comm_interface_get_ack_timeout_ms(void)
{
  return comm_interface.ack_timeout_ms;
}

void comm_interface_set_ack_timeout_ms(uint32_t t)
{
  comm_interface.ack_timeout_ms = t;
}

uint32_t comm_interface_get_byte_timeout_ms(void)
{
  return comm_interface.byte_timeout_ms;
}

void comm_interface_set_byte_timeout_ms(uint32_t t)
{
  comm_interface.byte_timeout_ms = t;
}

static void store_byte(uint8_t byte)
{
  if (TimerIsActive(&comm_interface.byte_timer))
    TimerRestart(&comm_interface.byte_timer);
  else
    TimerStart(&comm_interface.byte_timer, comm_interface_get_byte_timeout_ms());

  comm_interface.byte_timeout = false;
  comm_interface.buffer[comm_interface.buffer_len] = byte;
  comm_interface.buffer_len++;
}

static comm_interface_parse_result_t handle_sof(uint8_t input)
{
  comm_interface_parse_result_t result = PARSE_IDLE;

  if (input == SOF)
  {
    comm_interface.state = COMM_INTERFACE_STATE_LEN;
    comm_interface.buffer_len = 0;
    comm_interface.rx_active = true; // now we're receiving - check for timeout
    store_byte(input);
  }
  else
  {
    if (comm_interface.ack_needed)
    {
      if ((input == ACK) || (input == NAK))
      {
        comm_interface.ack_needed = false; // Done
        comm_interface.ack_timeout = false;
        comm_interface.byte_timeout = false;
        TimerStop(&comm_interface.ack_timer);
        TimerStop(&comm_interface.byte_timer);
        TimerStop(&comm_interface.buffer_check_timer);
      }
      if (input == ACK)
      {
        result = PARSE_FRAME_SENT;
      }
      else if (input == NAK)
      {
        result = PARSE_TX_TIMEOUT;
      }
      else
      {
        // Bogus character received...
      }
    }
    else
    {
      comm_interface.ack_timeout = false;
      TimerStop(&comm_interface.ack_timer);
      TimerStop(&comm_interface.buffer_check_timer);
    }
  }
  return result;
}

static void handle_len(uint8_t input)
{
  // Check for length to be inside valid range
  if ((input < FRAME_LENGTH_MIN) || (input > FRAME_LENGTH_MAX))
  {
    comm_interface.state = COMM_INTERFACE_STATE_SOF; // Restart looking for SOF
    comm_interface.rx_active = false;  // Not really active now...
    TimerStop(&comm_interface.byte_timer);
    comm_interface.byte_timeout = false;
  }
  else
  {
    comm_interface.state = COMM_INTERFACE_STATE_TYPE;
    store_byte(input);
  }
}

static void handle_type(uint8_t input)
{
  if (input > RESPONSE)
  {
    comm_interface.state = COMM_INTERFACE_STATE_SOF; // Restart looking for SOF
    comm_interface.rx_active = false;  // Not really active now...
    TimerStop(&comm_interface.byte_timer);
    comm_interface.byte_timeout = false;
  }
  else
  {
    comm_interface.state = COMM_INTERFACE_STATE_CMD;
    store_byte(input);
  }
}

static void handle_cmd(uint8_t input)
{
  store_byte(input);

  if(serial_frame->len > 3)
  {
    comm_interface.rx_wait_count = serial_frame->len - 3;
    comm_interface.state = COMM_INTERFACE_STATE_DATA;
  }
  else
  {
    comm_interface.rx_wait_count = 1;
    comm_interface.state = COMM_INTERFACE_STATE_CHECKSUM;
  }
}

static void handle_data(uint8_t input)
{
  comm_interface.rx_wait_count--;
  store_byte(input);

  if ((comm_interface.buffer_len >= RECEIVE_BUFFER_SIZE) ||
      (comm_interface.buffer_len > serial_frame->len))      //buffer_len - sizeof(sof) >= serial_frame->len
  {
    comm_interface.state = COMM_INTERFACE_STATE_CHECKSUM;
  }
}

static comm_interface_parse_result_t handle_checksum(uint8_t input, bool ack)
{
  TimerStop(&comm_interface.byte_timer);
  comm_interface.byte_timeout = false;
  comm_interface.state = COMM_INTERFACE_STATE_SOF; // Restart looking for SOF
  comm_interface.rx_active = false;  // Not really active

  /* Default values for ack == false */
  /* It means we are in the process of looking for an acknowledge to a callback request */
  /* Drop the new frame we received - we don't have time to handle it. */
  comm_interface_parse_result_t result = PARSE_IDLE;
  uint8_t response = CAN;

  /* Do we send ACK/NAK according to checksum... */
  /* if not then the received frame is dropped! */
  if (ack)
  {
    uint8_t checksum = xor_checksum(0xFF, &serial_frame->len, serial_frame->len);
    result = (input == checksum) ? PARSE_FRAME_RECEIVED : PARSE_FRAME_ERROR;
    response = (input == checksum) ? ACK : NAK;
  }

  comm_interface_transmit(&comm_interface.transport, &response, sizeof(response), NULL);

  return result;
}

static void handle_default(void)
{
  comm_interface.state = COMM_INTERFACE_STATE_SOF; // Restart looking for SOF
  comm_interface.rx_active = false;  // Not really active now...
  comm_interface.ack_timeout = false;
  comm_interface.byte_timeout = false;
  TimerStop(&comm_interface.ack_timer);
  TimerStop(&comm_interface.buffer_check_timer);
  TimerStop(&comm_interface.byte_timer);
}

comm_interface_parse_result_t comm_interface_parse_data(bool ack)
{
  uint8_t rx_byte;
  comm_interface_parse_result_t result = PARSE_IDLE;

  while ((result == PARSE_IDLE) && zpal_uart_get_available(comm_interface.transport.handle))
  {
    zpal_uart_receive(comm_interface.transport.handle, &rx_byte, sizeof(rx_byte));

    switch (comm_interface.state)
    {
      case COMM_INTERFACE_STATE_SOF:
        result = handle_sof(rx_byte);
        break;

      case COMM_INTERFACE_STATE_LEN:
        handle_len(rx_byte);
        break;

      case COMM_INTERFACE_STATE_TYPE:
        handle_type(rx_byte);
        break;

      case COMM_INTERFACE_STATE_CMD:
        handle_cmd(rx_byte);
        break;

      case COMM_INTERFACE_STATE_DATA:
        handle_data(rx_byte);
        break;

      case COMM_INTERFACE_STATE_CHECKSUM:
        result = handle_checksum(rx_byte, ack);
        break;

      default :
        handle_default();
        break;
    }
  }

  /* Check for timeouts - if no other events detected */
  if (result == PARSE_IDLE)
  {
    /* Are we in the middle of collecting a frame and have we timed out? */
    if (comm_interface.rx_active && comm_interface.byte_timeout)
    {
      comm_interface.byte_timeout = false;
      /* Reset to SOF hunting */
      comm_interface.state = COMM_INTERFACE_STATE_SOF;
      comm_interface.rx_active = false; /* Not inframe anymore */
      result = PARSE_RX_TIMEOUT;
    }

    /* Are we waiting for ACK and have we timed out? */
    if (comm_interface.ack_needed && comm_interface.ack_timeout)
    {
      comm_interface.ack_timeout = false;
      /* Reset to SOF hunting */
      comm_interface.state = COMM_INTERFACE_STATE_SOF;
      /* Not waiting for ACK anymore */
      comm_interface.ack_needed = false;
      /* Tell upper layer we could not get the frame through */
      result = PARSE_TX_TIMEOUT;
    }
  }

  /*Check how many bytes we need depending on state.*/
  switch(comm_interface.state)
  {
    case COMM_INTERFACE_STATE_SOF:
      set_expect_bytes(comm_interface.ack_needed ? ACK_LEN : HEADER_LEN);
      break;

    case COMM_INTERFACE_STATE_LEN:
      set_expect_bytes(HEADER_LEN - 1);
      break;

    case COMM_INTERFACE_STATE_TYPE:
      set_expect_bytes(HEADER_LEN - 2);
      break;

    case COMM_INTERFACE_STATE_CMD:
      set_expect_bytes(HEADER_LEN - 3);
      break;

    case COMM_INTERFACE_STATE_DATA:
      set_expect_bytes(comm_interface.rx_wait_count);
      break;

    case COMM_INTERFACE_STATE_CHECKSUM:
      set_expect_bytes(CRC_LEN);
      break;

    default:
      break;
  }

  return result;
}