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
#ifndef __WIFI_MGMR_PROFILE_H__
#define __WIFI_MGMR_PROFILE_H__
#include "wifi_mgmr.h"

int wifi_mgmr_profile_add(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index, uint8_t isActive);
int wifi_mgmr_profile_add_by_idx(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index, uint8_t isActive);
int wifi_mgmr_profile_del(wifi_mgmr_t *mgmr, char *ssid, int len);
int wifi_mgmr_profile_del_by_idx(wifi_mgmr_t *mgmr, uint8_t index);
int wifi_mgmr_profile_set_active_by_idx(wifi_mgmr_t *mgmr, uint8_t index, uint8_t isActive);
int wifi_mgmr_profile_get(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, uint8_t isActive);
int wifi_mgmr_profile_get_by_idx(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, uint8_t index);
int wifi_mgmr_profile_autoreconnect_is_enabled(wifi_mgmr_t *mgmr, int index);
int wifi_mgmr_profile_autoreconnect_disable(wifi_mgmr_t *mgmr, int index);
int wifi_mgmr_profile_autoreconnect_enable(wifi_mgmr_t *mgmr, int index);
#endif
