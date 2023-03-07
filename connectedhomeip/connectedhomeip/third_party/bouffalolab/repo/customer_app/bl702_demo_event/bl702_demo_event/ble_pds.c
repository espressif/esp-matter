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
#if defined(CFG_BLE_PDS) && !defined(CFG_ZIGBEE_PDS)
#include <FreeRTOS.h>
#include <task.h>
#include <bl_irq.h>
#include <bl_rtc.h>
#include <bl_timer.h>
#include <bl_flash.h>
#include <hosal_uart.h>
#include "hal_pds.h"
#include "ble_lib_api.h"

#define  DEBUG_PRINT  0

#if (!DEBUG_PRINT)
#define printf(...)                (void)0
#endif
extern bool pds_start;
extern bool wfi_disable;

void ble_pds_init(void)
{
    hal_pds_init();
}

static void bl_pds_restore(void)
{
   #if (DEBUG_PRINT)
    HOSAL_UART_DEV_DECL(uart_stdio, 0, 14, 15, 2000000);
    hosal_uart_init(&uart_stdio);
   #endif

#if defined(CFG_USB_CDC_ENABLE)
    extern void usb_cdc_restore(void);
    usb_cdc_restore();
#endif

    ble_controller_sleep_restore();
   // bl_sec_init();
}

#if ( configUSE_TICKLESS_IDLE != 0 )
#define TIME_5MS_IN_32768CYCLE  (164) // (5000/(1000000/32768))
extern bool le_check_valid_scan(void);
extern bool le_check_valid_conn(void);
//Allocate retention memory
void *bl_alloc_retmem(size_t xWantedSize )
{
    return pvPortMalloc(xWantedSize);    
}

void vApplicationSleep(TickType_t xExpectedIdleTime_ms)
{
    int32_t bleSleepDuration_32768cycles = 0;
    int32_t expectedIdleTime_32768cycles = 0;
    eSleepModeStatus eSleepStatus;
    bool freertos_max_idle = false;
#if (LE_PDS_FLASH)
    uint8_t ioMode;
    uint8_t contRead; 
#endif

    if (pds_start == 0 || le_check_valid_scan())
        return;

    if(xExpectedIdleTime_ms + xTaskGetTickCount() == portMAX_DELAY){
        freertos_max_idle = true;
    }else{   
        xExpectedIdleTime_ms -= 1;
        expectedIdleTime_32768cycles = 32768 * xExpectedIdleTime_ms / 1000;
    }

    if((!freertos_max_idle)&&(expectedIdleTime_32768cycles < TIME_5MS_IN_32768CYCLE)){
        return;
    }
        
    eSleepStatus = eTaskConfirmSleepModeStatus();
    if(eSleepStatus == eAbortSleep || ble_controller_sleep_is_ongoing())
    {
        return;
    }

    bleSleepDuration_32768cycles = ble_controller_sleep(0);

    if(bleSleepDuration_32768cycles < TIME_5MS_IN_32768CYCLE)
    {
        return;
    }
    else
    { 
        printf("Sleep_cycles=%ld\r\n", bleSleepDuration_32768cycles);
        uint8_t reduceSleepTime;
        SPI_Flash_Cfg_Type *flashCfg = bl_flash_get_flashCfg();
        uint8_t ioMode =  flashCfg->ioMode & 0xF;
        uint8_t contRead = flashCfg->cReadSupport;
        uint8_t cpuClk = GLB_Get_Root_CLK_Sel();
        if(ioMode == 4 && contRead == 1 && cpuClk == GLB_ROOT_CLK_XTAL)
        {
           reduceSleepTime = 100;
        }
        else if(ioMode == 1 && contRead == 0 && cpuClk == GLB_ROOT_CLK_XTAL)
        {
           #if (DEBUG_PRINT)
           reduceSleepTime = 130;
           #else
           //reduceSleepTime = 35;
           reduceSleepTime = 75;  // there is 1ms delay after xtal power on after pds wakeup
           #endif
        }
        else
        {
           reduceSleepTime = 130;
        }
        printf("reduceSleepTime=%d\r\n",reduceSleepTime);
        wfi_disable = true;
        if(eSleepStatus == eStandardSleep && ((!freertos_max_idle) && (expectedIdleTime_32768cycles < bleSleepDuration_32768cycles)))
        {
           hal_pds_enter_with_time_compensation(31, expectedIdleTime_32768cycles - reduceSleepTime);
        }
        else
        {
           hal_pds_enter_with_time_compensation(31, bleSleepDuration_32768cycles - reduceSleepTime);
        }
        wfi_disable = false;
        bl_pds_restore();
        
    }
}
#endif
#endif//#if defined(CFG_BLE_PDS)