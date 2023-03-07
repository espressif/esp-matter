/**
 * @file comm_interface.h
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef __COMM_INTERFACE__
#define __COMM_INTERFACE__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define RECEIVE_BUFFER_SIZE     180
#define FRAME_LENGTH_MIN        3
#define FRAME_LENGTH_MAX        RECEIVE_BUFFER_SIZE

typedef enum
{
  TRANSPORT_TYPE_UART,
  TRANSPORT_TYPE_SPI,
  TRANSPORT_TYPE_ETHERNET,
} transport_type_t;

typedef enum
{
  PARSE_IDLE,             // returned if nothing special has happened
  PARSE_FRAME_RECEIVED,   // returned when a valid frame has been received
  PARSE_FRAME_SENT,       // returned if frame was ACKed by the other end
  PARSE_FRAME_ERROR,      // returned if frame has error in Checksum
  PARSE_RX_TIMEOUT,       // returned if Rx timeout has happened
  PARSE_TX_TIMEOUT        // returned if Tx timeout (waiting for ACK) ahs happened
} comm_interface_parse_result_t;

typedef void * transport_handle_t;

typedef void (*transmit_done_cb_t)(transport_handle_t transport);

typedef struct _transport_t
{
  transport_type_t type;
  transport_handle_t handle;
} transport_t;

typedef struct
{
  uint8_t sof;
  uint8_t len;
  uint8_t type;
  uint8_t cmd;
  uint8_t payload[RECEIVE_BUFFER_SIZE]; //size defined to fix SonarQube errors
} * comm_interface_frame_ptr;

extern comm_interface_frame_ptr const serial_frame;

static inline uint8_t frame_payload_len(const comm_interface_frame_ptr frame)
{
  return frame->len - 3;
}

void comm_interface_transmit_frame(uint8_t cmd, uint8_t type, const uint8_t *payload, uint8_t len, transmit_done_cb_t cb);
void comm_interface_wait_transmit_done(void);
void comm_interface_init(void);
uint32_t comm_interface_get_ack_timeout_ms(void);
void comm_interface_set_ack_timeout_ms(uint32_t t);
uint32_t comm_interface_get_byte_timeout_ms(void);
void comm_interface_set_byte_timeout_ms(uint32_t t);
comm_interface_parse_result_t comm_interface_parse_data(bool ack);

#endif /* __COMM_INTERFACE__ */