/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <stdbool.h>

typedef void *(*bl_pkt_eth_input_hook_cb_t)(bool is_sta, void *pkt, void *arg);

extern bl_pkt_eth_input_hook_cb_t bl_wifi_pkt_eth_input_hook;
extern void *bl_wifi_pkt_eth_input_hook_arg;

/**
 * @brief  Register a callback that is called before an eth packet is passed to TCP/IP stack.
 *
 * @param cb      Callback. cb may inspect, manipulate or even duplicate the packet.
 *                The first argument passed to cb is a struct pbuf *, second being cb_arg.
 *                cb should return NULL if cb drops the packet(cb might need to free it if so).
 *                If non-NULL is returned by cb, the returned pbuf will be passed to TCP/IP stack.
 * @param cb_arg  Callback argument.
 */
void bl_pkt_eth_input_hook_register(bl_pkt_eth_input_hook_cb_t cb, void *cb_arg);

/**
 * @brief  Unregister the callback previously registered.
 */
void bl_pkt_eth_input_hook_unregister(void);

typedef void *(*bl_pkt_eth_output_hook_cb_t)(bool is_sta, void *pkt, void *arg);

extern bl_pkt_eth_output_hook_cb_t bl_wifi_pkt_eth_output_hook;
extern void *bl_wifi_pkt_eth_output_hook_arg;


/**
 * @brief  Register a callback that is called before an eth packet from TCP/IP stack is sent by Wi-Fi.
 *
 * @param cb      Callback. cb may inspect, manipulate or even duplicate the packet.
 *                cb should return NULL if it drops the packet(cb should NOT free it if so).
 *                If non-NULL is returned by cb, the returned pbuf will be sent by Wi-Fi.
 * @param cb_arg  Callback argument.
 */
void bl_pkt_eth_output_hook_register(bl_pkt_eth_output_hook_cb_t cb, void *cb_arg);

/**
 * @brief  Unregister the callback previously registered.
 */
void bl_pkt_eth_output_hook_unregister(void);
