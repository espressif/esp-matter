#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <em_device.h>
#include <em_gpio.h>
#include <em_core.h>
#include "sl_hci_common_transport.h"
#include "sl_hci_cpc.h"
#include "sl_btctrl_hci_packet.h"

static uint8_t *read_buf;
static hci_packet_t *packet;
static enum hci_packet_state state;
static uint16_t bytes_remaining;

static void read_complete(uint16_t bytes_read, bool last_fragment)
{
  hci_common_transport_receive(read_buf, bytes_read, last_fragment);
  sl_hci_cpc_free(read_buf);
  sl_hci_cpc_rx_done();
}

static void reception_failure(void)
{
  read_complete(0, false);
  state = hci_packet_state_read_header;
}

void sl_btctrl_hci_packet_read(void)
{
  uint16_t bytes_read;
  uint16_t packet_data_read;

  /* Check if data available */
  if (sl_hci_cpc_new_data() <= 0) {
    return;
  }

  bytes_read = sl_hci_cpc_read(&read_buf);
  if (bytes_read == 0) {
    sl_hci_cpc_free(read_buf);
    sl_hci_cpc_rx_done();
    return; // CPC Secondary returned error
  } else {
    packet = (hci_packet_t *) read_buf;
  }

  switch (state) {
    case hci_packet_state_read_header:
    {
      switch (packet->packet_type) {
        case hci_packet_type_ignore:
        {
          state = hci_packet_state_read_header;
          return;
        }
        case hci_packet_type_command:
        {
          packet_data_read = bytes_read - (hci_command_header_size + 1);
          if (packet->hci_cmd.param_len > packet_data_read) { // 1 byte for packet type
            bytes_remaining = packet->hci_cmd.param_len - packet_data_read;
          } else {
            bytes_remaining = 0;
          }
          break;
        }
        case hci_packet_type_acl_data:
        {
          packet_data_read = bytes_read - (hci_acl_data_header_size + 1);
          if (packet->acl_pkt.length > packet_data_read) {
            bytes_remaining = packet->acl_pkt.length - packet_data_read;
          } else {
            bytes_remaining = 0;
          }
          break;
        }
        default:
        {
          reception_failure();
          return;
        }
      }

      if (bytes_remaining == 0) {
        read_complete(bytes_read, true);
        state = hci_packet_state_read_header;
        return;
      } else {
        read_complete(bytes_read, false);
        state = hci_packet_state_read_data;
      }
      break;
    }
    case hci_packet_state_read_data:
    {
      if (bytes_remaining > bytes_read) {
        bytes_remaining -= bytes_read;
        read_complete(bytes_read, false);
      } else {
        read_complete(bytes_read, true);
        state = hci_packet_state_read_header;
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
  return sl_hci_cpc_write(data, len);
}

void hci_common_transport_init(void)
{
  state = hci_packet_state_read_header;
  sl_hci_cpc_init();
}
