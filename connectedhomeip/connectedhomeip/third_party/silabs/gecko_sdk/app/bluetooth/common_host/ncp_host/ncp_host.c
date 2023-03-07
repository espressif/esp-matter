/***************************************************************************//**
 * @file
 * @brief NCP host application module.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "app_log.h"
#include "app_assert.h"
#include "sl_bt_ncp_host.h"
#include "ncp_host.h"
#include "app_sleep.h"
#include "ncp_host_config.h"
#include "host_comm_config.h"

// Default parameter values.
#define MAX_OPT_LEN                   255

#if PEEK_US_SLEEP == 0
#define MSG_RECV_TIMEOUT_COUNT        10000
#else
#define MSG_RECV_TIMEOUT_COUNT        MSG_RECV_TIMEOUT_MS * 1000 / PEEK_US_SLEEP
#endif

#define MSG_HEADER_MASK (uint32_t)((sl_bgapi_dev_type_btmesh  \
                                    | sl_bgapi_msg_type_evt   \
                                    | SL_BGAPI_BIT_ENCRYPTED) \
                                   & ((uint32_t)(~sl_bgapi_dev_type_bt)))

#if defined(SECURITY) && SECURITY == 1
#define SEC_BGAPI_RSP_BASE            4
#define SEC_BGAPI_RSP_BASE_LEN        4
#define SEC_BGAPI_RSP_MSG_LEN         80
#define SEC_BGAPI_CMD_BASE            6
#define SEC_BGAPI_CMD_MSG_LEN         78
#endif // defined(SECURITY) && SECURITY == 1

// RX/TX buffer
typedef struct {
  uint16_t len;
  uint8_t buf[DEFAULT_HOST_BUFLEN];
} buf_ncp_host_t;

static buf_ncp_host_t buf_ncp_raw = { 0 };
static buf_ncp_host_t buf_ncp_in = { 0 };

#if defined(SECURITY) && SECURITY == 1
static buf_ncp_host_t buf_ncp_out = { 0 };
static bool enable_security = false;
#endif // defined(SECURITY) && SECURITY == 1

static int32_t ncp_host_peek_timeout(uint32_t len, uint32_t timeout);
void ncp_sec_host_command_handler(buf_ncp_host_t *buf);

/**************************************************************************//**
 * Initialize NCP connection.
 *****************************************************************************/
sl_status_t ncp_host_init(void)
{
  sl_status_t sc;

  sc = sl_bt_api_initialize_nonblock(ncp_host_tx, ncp_host_rx, ncp_host_peek);

  if (sc == SL_STATUS_OK) {
    sc = host_comm_init();
  }

#if defined(SECURITY) && SECURITY == 1
  if (sc == SL_STATUS_OK) {
    sc = security_init();
  }
#endif // defined(SECURITY) && SECURITY == 1

  return sc;
}

/**************************************************************************//**
 * Set NCP connection options.
 *****************************************************************************/
sl_status_t ncp_host_set_option(char option, char *value)
{
  sl_status_t sc = SL_STATUS_OK;

  switch (option) {
#if defined(SECURITY) && SECURITY == 1
    case 's':
      enable_security = true;
      break;
#endif // defined(SECURITY) && SECURITY == 1
    default:
      sc = host_comm_set_option((char)option, value);
      break;
  }
  return sc;
}

/**************************************************************************//**
 * Deinitialize NCP connection.
 *****************************************************************************/
void ncp_host_deinit(void)
{
  host_comm_deinit();
}

/**************************************************************************//**
 * Write data to NCP.
 *****************************************************************************/
void ncp_host_tx(uint32_t len, uint8_t* data)
{
  int32_t ret;

#if  defined(SECURITY) && SECURITY == 1
  if (enable_security) {
    memset(buf_ncp_out.buf, 0, sizeof(buf_ncp_out.buf));
    security_encrypt((char*)data, (char*)&buf_ncp_out.buf, &len);
    ret = host_comm_tx(len, buf_ncp_out.buf);
  } else
#endif // defined(SECURITY) && SECURITY == 1
  {
    ret = host_comm_tx(len, data);
  }
  if (ret < 0) {
    host_comm_deinit();
    app_assert(false, "Failed to write data\n");
  }
}

/**************************************************************************//**
 * Read data from NCP.
 *****************************************************************************/
int32_t ncp_host_rx(uint32_t len, uint8_t* data)
{
  int32_t ret;

  if (buf_ncp_in.len == 0) {
    ret = ncp_host_peek();
  } else {
    ret = buf_ncp_in.len;
  }
  if (ret > 0) {
    if (len <= buf_ncp_in.len) {
      memcpy(data, buf_ncp_in.buf, len);
      buf_ncp_in.len -= len;
      memmove(buf_ncp_in.buf, &buf_ncp_in.buf[len], buf_ncp_in.len);
    } else {
      ret = -1;
    }
  } else {
    ret = -1;
  }
  return ret;
}

/**************************************************************************//**
 * Peek if readable data exists with timeout option.
 *****************************************************************************/
int32_t ncp_host_peek_timeout(uint32_t len, uint32_t timeout)
{
  int32_t ret;
  uint32_t timeout_counter = 0;

  do {
    ret = host_comm_peek();
    timeout_counter++;
    app_sleep_us(PEEK_US_SLEEP);
  } while ((ret < len) && (timeout_counter < timeout));

  return ret;
}

/**************************************************************************//**
 * Peek if readable data exists.
 *****************************************************************************/
int32_t ncp_host_peek(void)
{
  int32_t msg_len;

  msg_len = host_comm_peek();
  if (msg_len) {
    int32_t ret;
    uint8_t msg_header = 0;
    // Read first byte
    ret = host_comm_rx(1, &buf_ncp_raw.buf[0]);
    if (ret < 0) {
      return -1;
    }
    msg_header = (uint8_t)(buf_ncp_raw.buf[0] & 0xff);
    // Check if proper ncp header arrived
    if ((msg_header & (uint32_t)(~MSG_HEADER_MASK)) == sl_bgapi_dev_type_bt) {
      // If header seems to be ok, read length
      ret = ncp_host_peek_timeout(1, MSG_RECV_TIMEOUT_COUNT);
      if (ret < 0) {
        return -1;
      }
      ret = host_comm_rx(1, (void*) &buf_ncp_raw.buf[1]);
      if (ret < 0) {
        return -1;
      }
      msg_len = buf_ncp_raw.buf[1] + 2;
      // Check if length will fit to buffer
      if (msg_len >= DEFAULT_HOST_BUFLEN - 2) {
        return -1;
      }
      ret = ncp_host_peek_timeout(msg_len, MSG_RECV_TIMEOUT_COUNT);
      if (ret < 0) {
        return -1;
      }
      // Read the rest of the message
      ret = host_comm_rx(msg_len, (void*) &buf_ncp_raw.buf[2]);
      if (ret < 0) {
        return -1;
      }
      msg_len += 2;
      buf_ncp_raw.len = msg_len;
#if defined(SECURITY) && SECURITY == 1
      if (SL_BT_MSG_ENCRYPTED((uint8_t)msg_header) !=  0) {
        security_decrypt((char *)&buf_ncp_raw.buf[0], (char *)&buf_ncp_in.buf[0], (unsigned *)&msg_len);
      } else
#endif // defined(SECURITY) && SECURITY == 1
      {
        memcpy(buf_ncp_in.buf, buf_ncp_raw.buf, msg_len);
      }
      memset(buf_ncp_raw.buf, 0, sizeof(buf_ncp_raw.buf));
      buf_ncp_in.len = msg_len;
#if defined(SECURITY) && SECURITY == 1
      if (enable_security) {
        ncp_sec_host_command_handler(&buf_ncp_in);
      }
#endif // defined(SECURITY) && SECURITY == 1
    }
  } else {
    app_sleep_us(PEEK_US_SLEEP);
  }
  return msg_len;
}

#if defined(SECURITY) && SECURITY == 1
void ncp_sec_host_command_handler(buf_ncp_host_t *buf)
{
  uint8_t response[DEFAULT_HOST_BUFLEN];
  sl_bt_msg_t *command = NULL;
  sl_bt_msg_t *resp_cmd = NULL;
  int32_t ret;

  command = (sl_bt_msg_t *)buf->buf;

  switch (SL_BT_MSG_ID(command->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
    case sl_bt_evt_dfu_boot_id:
      security_reset();
      // Wait for the security handshake response (80 bytes length)
      ret = ncp_host_peek_timeout(SEC_BGAPI_RSP_MSG_LEN,
                                  MSG_RECV_TIMEOUT_COUNT * 10);
      if (ret < 0) {
        return;
      }
      // Read the rest of the message
      ret = host_comm_rx(SEC_BGAPI_RSP_MSG_LEN, (void *) &response);
      resp_cmd = (sl_bt_msg_t *)response;
      if (SL_BT_MSG_ID(resp_cmd->header)
          == sl_bt_rsp_user_message_to_target_id) {
        // Check if the correct response was received to security handshake
        // Length has to be 80, and 0x00004900 has to come after 4 byte header
        if (ret == SEC_BGAPI_RSP_MSG_LEN
            && response[SEC_BGAPI_RSP_BASE] == 0x00
            && response[SEC_BGAPI_RSP_BASE + 1] == 0x00
            && response[SEC_BGAPI_RSP_BASE + 2] == 0x49
            && response[SEC_BGAPI_RSP_BASE + 3] == 0x00) {
          uint8_t public[PUBLIC_KEYPAIR_SIZE];
          uint8_t target_iv_to_target[IV_SIZE];
          uint8_t target_iv_to_host[IV_SIZE];
          memcpy(public, &response[SEC_BGAPI_RSP_BASE + SEC_BGAPI_RSP_BASE_LEN],
                 PUBLIC_KEYPAIR_SIZE);
          memcpy(target_iv_to_target, &response[PUBLIC_KEYPAIR_SIZE
                                                + SEC_BGAPI_RSP_BASE + SEC_BGAPI_RSP_BASE_LEN], IV_SIZE);
          memcpy(target_iv_to_host, &response[PUBLIC_KEYPAIR_SIZE
                                              + SEC_BGAPI_RSP_BASE + SEC_BGAPI_RSP_BASE_LEN + IV_SIZE], IV_SIZE);
          security_increase_security_rsp(public, target_iv_to_target,
                                         target_iv_to_host);
          return;
        } else {
          app_log_warning("NCP Encryption Failed 0x%02x%02x"
                          APP_LOG_NL,
                          response[SEC_BGAPI_RSP_BASE + 1],
                          response[SEC_BGAPI_RSP_BASE]);
        }
      }
      break;
    default:
      break;
  }
}

void security_state_change_cb(security_state_t state)
{
  switch (state) {
    case SECURITY_STATE_UNENCRYPTED:
      //start security handshake
      security_start();
      break;
    case SECURITY_STATE_ENCRYPTED:
      app_log_info("Communication encrypted" APP_LOG_NL);
    default:
      break;
  }
}

int sl_bgapi_user_cmd_increase_security(uint8_t *public_key,
                                        uint8_t *host_iv_to_target,
                                        uint8_t *host_iv_to_host)
{
  uint8_t bgapi_packet[SEC_BGAPI_CMD_MSG_LEN] =
  { 0x20, 0x4a, 0xff, 0x00, 0x49, 0x00 };
  memcpy(bgapi_packet + SEC_BGAPI_CMD_BASE, public_key,
         PUBLIC_KEYPAIR_SIZE);
  memcpy(bgapi_packet + SEC_BGAPI_CMD_BASE + PUBLIC_KEYPAIR_SIZE,
         host_iv_to_target, IV_SIZE);
  memcpy(bgapi_packet + SEC_BGAPI_CMD_BASE + PUBLIC_KEYPAIR_SIZE
         + IV_SIZE, host_iv_to_host, IV_SIZE);

  ncp_host_tx(sizeof(bgapi_packet), bgapi_packet);
  return 0;
}
#endif // defined(SECURITY) && SECURITY == 1
