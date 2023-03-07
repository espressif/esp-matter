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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <bl702.h>

#include "bl_efuse.h"
#include "bl_wireless.h"
#include "bl_irq.h"

typedef struct _bl_wireless_env {
    uint8_t mac_addr[8];
    int8_t power_offset_zigbee[16];
    int8_t power_offset_ble[40];
    uint8_t tcal_en;
} bl_wireless_env_t;

bl_wireless_env_t wireless_env;


int bl_wireless_mac_addr_set(uint8_t mac[8])
{
    memcpy(wireless_env.mac_addr, mac, 8);
    return 0;
}

int bl_wireless_mac_addr_get(uint8_t mac[8])
{
    memcpy(mac, wireless_env.mac_addr, 8);
    return 0;
}

int bl_wireless_power_offset_set(int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[40])
{
    memcpy(wireless_env.power_offset_zigbee, poweroffset_zigbee, 16);
    memcpy(wireless_env.power_offset_ble, poweroffset_ble, 40);
    return 0;
}

int bl_wireless_power_offset_get(int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[40])
{
    memcpy(poweroffset_zigbee, wireless_env.power_offset_zigbee, 16);
    memcpy(poweroffset_ble, wireless_env.power_offset_ble, 40);
    return 0;
}

int8_t bl_wireless_power_offset_zigbee_get(uint8_t ch)
{
    if(ch < 11){
        return wireless_env.power_offset_zigbee[0];
    }else if(ch > 26){
        return wireless_env.power_offset_zigbee[15];
    }else{
        return wireless_env.power_offset_zigbee[ch - 11];
    }
}

int8_t bl_wireless_power_offset_ble_get(uint8_t ch)
{
    if(ch > 39){
        return wireless_env.power_offset_ble[39];
    }else{
        return wireless_env.power_offset_ble[ch];
    }
}

void bl_wireless_tcal_en_set(uint8_t en)
{
    wireless_env.tcal_en = en;
}

uint8_t bl_wireless_tcal_en_get(void)
{
    return wireless_env.tcal_en;
}
