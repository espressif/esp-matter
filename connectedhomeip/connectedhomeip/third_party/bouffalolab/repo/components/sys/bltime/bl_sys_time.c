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
#include <FreeRTOS.h>
#include <task.h>

#include <bl_sys_time.h>

static uint64_t epoch_time = 0; //in ms
static uint64_t time_synced = 0; //in ms

static uint32_t init_tick_rtos = 0;
static uint64_t init_cnt_rtc = 0;
static int sync_init = 0;

void bl_sys_time_update(uint64_t epoch)
{
    time_synced = xTaskGetTickCount();
    epoch_time = epoch;
}

int bl_sys_time_get(uint64_t *epoch)
{
    int ms_diff;

    if (0 == epoch_time) {
        return -1;
    }

    //TODO we should use RTC hardware to track time
    ms_diff = ((int32_t)xTaskGetTickCount()) - ((int32_t)time_synced);
    *epoch = epoch_time + ms_diff;

    return 0;
}

void bl_sys_time_sync_init(void)
{
    taskENTER_CRITICAL();
    init_tick_rtos = xTaskGetTickCount();
    init_cnt_rtc = bl_rtc_get_counter();
    taskEXIT_CRITICAL();
    
    sync_init = 1;
}

int bl_sys_time_sync_state(uint32_t *xTicksToJump)
{
    uint32_t deltaTickRtos;
    uint32_t deltaTickRtc;
    
    if(!sync_init){
        return -1;
    }
    
    taskENTER_CRITICAL();
    deltaTickRtos = xTaskGetTickCount() - init_tick_rtos;
    deltaTickRtc = (uint32_t)bl_rtc_get_delta_time_ms(init_cnt_rtc);
    taskEXIT_CRITICAL();
    
    if(deltaTickRtc > deltaTickRtos){
        *xTicksToJump = deltaTickRtc - deltaTickRtos;
        return 1;
    }else{
        return 0;
    }
}

uint32_t bl_sys_time_sync(void)
{
    uint32_t xTicksToJump = 0;
    
    if(bl_sys_time_sync_state(&xTicksToJump) > 0){
        vTaskStepTickSafe(xTicksToJump);
    }
    
    return xTicksToJump;
}
