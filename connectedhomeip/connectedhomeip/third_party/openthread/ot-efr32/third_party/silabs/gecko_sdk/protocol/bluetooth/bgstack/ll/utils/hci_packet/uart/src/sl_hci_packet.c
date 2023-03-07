#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <em_device.h>
#include <em_gpio.h>
#include <em_core.h>
#include "sl_hci_common_transport.h"
#include "sl_hci_uart.h"
#include "sl_btctrl_hci_packet.h"

#define RX_BUFFER_LEN 64

SL_ALIGN(4)
static uint8_t hci_rx_buffer[RX_BUFFER_LEN] SL_ATTRIBUTE_ALIGN(4);
/* buffer pointer for transferring bytes to hci_rx_buffer */
static uint8_t *buf_byte_ptr;
static hci_packet_t *const PACKET = (hci_packet_t *) hci_rx_buffer;
static enum hci_packet_state state;
static uint16_t bytes_remaining; // first message byte contains packet type
static uint16_t total_bytes_read;
static uint16_t buffer_remaining;

static void reset(void)
{
  memset(hci_rx_buffer, 0, RX_BUFFER_LEN);
  state = hci_packet_state_read_packet_type;
  buf_byte_ptr = hci_rx_buffer;
  bytes_remaining = 1;
  total_bytes_read = 0;
  buffer_remaining = RX_BUFFER_LEN;
}

static void reception_failure(void)
{
  hci_common_transport_receive(NULL, 0, false);
  reset();
}

/**
 *  Called from sl_service_process_event(). Immediately returns if no data
 *  available to read. Reading of data consists of three phases: packet type,
 *  header, and data. The amount of data read during each phase is dependent
 *  upon the command. For a given phase, an attempt is made to read the full
 *  amount of data pertaining to that phase. If less than this amount is in
 *  the buffer, the amount read is subtracted from the remaining amount and
 *  function execution returns. When all data for a phase has been read, the
 *  next phase is started, or hci_common_transport_receive() is called if all
 *  data has been read.
 */
void sl_btctrl_hci_packet_read(void)
{
  uint16_t bytes_read;
  uint16_t len;

  /* Check if data available */
  if (sl_hci_uart_rx_buffered_length() <= 0) {
    return;
  }

  if (bytes_remaining >= buffer_remaining) {
    len = buffer_remaining;
  } else {
    len = bytes_remaining;
  }

  bytes_read = sl_hci_uart_read(buf_byte_ptr, len);
  buf_byte_ptr += bytes_read;
  total_bytes_read += bytes_read;
  bytes_remaining -= bytes_read;
  buffer_remaining -= bytes_read;

  if (bytes_remaining == 1 && state == hci_packet_state_read_data) {
    sl_hci_disable_sleep(true);
  }

  if (bytes_remaining > 0 && buffer_remaining > 0) {
    return;
  }

  switch (state) {
    case hci_packet_state_read_packet_type:
    {
      switch (PACKET->packet_type) {
        case hci_packet_type_ignore:
        {
          reset();
          return;
        }
        case hci_packet_type_command:
        {
          bytes_remaining = hci_command_header_size;
          break;
        }
        case hci_packet_type_acl_data:
        {
          bytes_remaining = hci_acl_data_header_size;
          break;
        }
        default:
        {
          reception_failure();
          return;
        }
      }

      state = hci_packet_state_read_header;
      break;
    }
    case hci_packet_state_read_header:
    {
      switch (PACKET->packet_type) {
        case hci_packet_type_command:
        {
          bytes_remaining = PACKET->hci_cmd.param_len;
          break;
        }
        case hci_packet_type_acl_data:
        {
          bytes_remaining = PACKET->acl_pkt.length;
          break;
        }
        default:
        {
          reception_failure();
          return;
        }
      }

      if (bytes_remaining == 0) {
        hci_common_transport_receive(hci_rx_buffer, total_bytes_read, true);
        reset();
        return;
      } else {
        state = hci_packet_state_read_data;
      }
      break;
    }
    case hci_packet_state_read_data:
    {
      if (bytes_remaining > 0) {
        hci_common_transport_receive(hci_rx_buffer, RX_BUFFER_LEN, false);
        buffer_remaining = RX_BUFFER_LEN;
        buf_byte_ptr = hci_rx_buffer;
      } else {
        sl_hci_disable_sleep(false);
        hci_common_transport_receive(hci_rx_buffer, RX_BUFFER_LEN - buffer_remaining, true);
        reset();
      }
      break;
    }
    default:
    {
      reception_failure();
      return;
    }
  }
}

uint32_t hci_common_transport_transmit(uint8_t *data, int16_t len)
{
  return sl_hci_uart_write(data, len);
}

void hci_common_transport_init(void)
{
  reset();
  sl_hci_uart_init();
}
