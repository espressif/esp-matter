/***************************************************************************//**
 * @file
 * @brief NCP Daemon
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

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "app_log.h"
#include "sl_bgapi.h"
#include "sl_ncp_daemon.h"
#include "ncp_sec_host.h"
#include "config.h"
#include "sl_bt_api.h"

#define MSG_HEADER_MASK (uint8_t)((sl_bgapi_dev_type_btmesh  \
                                   | sl_bgapi_msg_type_evt   \
                                   | SL_BGAPI_BIT_ENCRYPTED) \
                                  & ((uint8_t)(~sl_bgapi_dev_type_bt)))

static sl_bgapi_context_t *ncp_target = NULL;
static char tbuf[MAX_PACKET_SIZE * 2];
static unsigned tlen;

void sl_bgapi_reset(sl_bgapi_context_t *ctx)
{
  ctx->recv_len = 0;
}

void sl_bgapi_recv_data(sl_bgapi_context_t *ctx, char *buf, int len)
{
  uint32_t hdr;

  if (ctx->recv_len + len > sizeof(ctx->recv_buf)) {
    //buffer overflowing, move old data to at least fit new data
    memmove(&ctx->recv_buf[0], &ctx->recv_buf[ctx->recv_len],
            sizeof(ctx->recv_buf) - ctx->recv_len);
    ctx->recv_len -= len;
  }
  //copy new data at end of buffer
  memcpy(&ctx->recv_buf[ctx->recv_len], buf, len);
  ctx->recv_len += len;
  if (len < 1) {
    return;
  }
  //validate packet
  while (ctx->recv_len > SL_BGAPI_MSG_HEADER_LEN) {
    memcpy(&hdr, ctx->recv_buf, SL_BGAPI_MSG_HEADER_LEN);
    if (((uint8_t)((uint8_t)(ctx->recv_buf[0]) & (uint8_t)(~MSG_HEADER_MASK))
         != (uint8_t)sl_bgapi_dev_type_bt)
        || (SL_BT_MSG_LEN(hdr) > SL_BGAPI_MAX_PAYLOAD_SIZE)) {
      //invalid packet, drop 1st byte
      ctx->recv_len--;
      memmove(&ctx->recv_buf[0], &ctx->recv_buf[1], ctx->recv_len - 1);
      continue;
    }
    unsigned packet_len = SL_BT_MSG_LEN(hdr) + SL_BGAPI_MSG_HEADER_LEN;
    if (packet_len > ctx->recv_len) {//not enough data for a packet
      return;
    }
    if (SL_BT_MSG_ENCRYPTED(hdr)) {
      tlen = packet_len;

      security_decrypt_packet(&ctx->recv_buf[0], tbuf, &tlen);

      sl_bgapi_process_packet_cb(ctx, tbuf, tlen, 1);
    } else {
      switch (SL_BT_MSG_ID(hdr)) {
        case sl_bt_rsp_user_message_to_target_id:
          //response to security handshake
          if (packet_len == 80
              && ctx->recv_buf[4] == 0 && ctx->recv_buf[5] == 0
              && ctx->recv_buf[6] == 0x49 && ctx->recv_buf[7] == 0x0) {
            uint8_t public[64];
            uint8_t target_iv_to_target[4];
            uint8_t target_iv_to_host[4];
            memcpy(public, &ctx->recv_buf[8], 64);
            memcpy(target_iv_to_target, &ctx->recv_buf[72], 4);
            memcpy(target_iv_to_host, &ctx->recv_buf[76], 4);
            security_increase_security_rsp(public, target_iv_to_target,
                                           target_iv_to_host);
            memmove(&ctx->recv_buf[0], &ctx->recv_buf[packet_len],
                    ctx->recv_len - packet_len);
            ctx->recv_len -= packet_len;
            return;
          } else {
            app_log_warning("NCP Encryption Failed 0x%02x%02x" APP_LOG_NL,
                            ctx->recv_buf[5],
                            ctx->recv_buf[4]);
          }
          break;
        case sl_bt_evt_system_boot_id:
        case sl_bt_evt_dfu_boot_id:
          //ncp target is resetted, reset security system
          app_log_info("NCP Target Reset" APP_LOG_NL);
          security_reset();
          break;
      }
      //process packet
      sl_bgapi_process_packet_cb(ctx, &ctx->recv_buf[0], packet_len, 0);
    }

    //drop packet from buffer
    memmove(&ctx->recv_buf[0], &ctx->recv_buf[packet_len],
            ctx->recv_len - packet_len);
    ctx->recv_len -= packet_len;
  }
}

void sl_bgapi_context_init(sl_bgapi_context_t *ctx, int fd)
{
  ctx->fd = fd;
}

void sl_bgapi_set_ncp_target(sl_bgapi_context_t *ctx)
{
  ncp_target = ctx;
}

int sl_bgapi_user_cmd_increase_security(uint8_t *public_key,
                                        uint8_t *host_iv_to_target,
                                        uint8_t *host_iv_to_host)
{
  uint8_t bgapi_packet[78] = { 0x20, 0x4a, 0xff, 0x00, 0x49, 0x00 };
  memcpy(bgapi_packet + 6, public_key, 64);
  memcpy(bgapi_packet + 70, host_iv_to_target, 4);
  memcpy(bgapi_packet + 74, host_iv_to_host, 4);
  int len = sizeof(bgapi_packet);
  uint8_t *ptr = (uint8_t* )bgapi_packet;
  if (ncp_target != NULL) {
    while (len) {
      int l = write(ncp_target->fd, ptr, len);
      if (l < 0) {
        // Error
        return -1;
      }
      len -= l;
      ptr += l;
    }
  }
  return 0;
}
