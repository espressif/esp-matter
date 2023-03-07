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
#ifndef __PING_H__
#define __PING_H__

#include <lwip/opt.h>
#if LWIP_RAW /* don't build if not configured for use in lwipopts.h */
#include <stdbool.h>

#include <utils_list.h>
#include <lwip/ip_addr.h>
#include <lwip/raw.h>
#include <utils_memp.h>

struct ping_var
{
    u32_t total_count;
    u32_t requests_count;
    u16_t interval;
    u16_t data_size;
    u16_t seq_num;
    u16_t id;
    u16_t timeout;
    u8_t  node_num;
    ip_addr_t dest;
    utils_memp_pool_t *pool;
    struct raw_pcb *pcb;
    struct utils_list req_list;
};

struct t_hdr
{
    struct t_hdr *next;
    u16_t ping_seq;
    u32_t send_time;
};

int network_netutils_ping_cli_register();
struct ping_var *ping_api_init(u16_t interval, u16_t size, u32_t count, u16_t timeout, ip_addr_t *dest);

#endif
#endif
