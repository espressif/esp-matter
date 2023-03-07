/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    Copyright (c) 2019 Google LLC.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "capsense.h"
#include "mbed_power_mgmt.h"
#include "mbed_trace.h"

using namespace std::literals::chrono_literals;

#define TRACE_GROUP "CAP"
#define SLIDER_NUM_TOUCH (1u)
#define LED_OFF (1u)
#define LED_ON (0u)

#define CAPSENSE_SCAN_PERIOD_MS (20ms)
#define EZI2C_INTERRUPT_PRIORITY (3u)

#define TUNER_ENABLE (1u)

const cy_stc_sysint_t CapSense_ISR_cfg = { .intrSrc = CYBSP_CSD_IRQ, .intrPriority = 4u };

Semaphore capsense_sem;
EventQueue queue;

static cy_stc_scb_ezi2c_context_t EZI2C_context;
static uint32_t prevBtn0Status                       = 0u;
static uint32_t prevBtn1Status                       = 0u;
static uint32_t prevSliderPos                        = 0u;
static volatile uint32_t irr                         = 0;
static volatile uint32_t fails                       = 0;
static cy_stc_syspm_callback_params_t callbackParams = { .base = CYBSP_CSD_HW, .context = &cy_capsense_context };

static cy_stc_syspm_callback_t capsenseDeepSleepCb = { Cy_CapSense_DeepSleepCallback,
                                                       CY_SYSPM_DEEPSLEEP,
                                                       (CY_SYSPM_SKIP_CHECK_FAIL | CY_SYSPM_SKIP_BEFORE_TRANSITION |
                                                        CY_SYSPM_SKIP_AFTER_TRANSITION),
                                                       &callbackParams,
                                                       NULL,
                                                       NULL };

static cy_stc_scb_ezi2c_context_t ezi2c_context;
static cyhal_ezi2c_t sEzI2C;
static cyhal_ezi2c_slave_cfg_t sEzI2C_sub_cfg;
static cyhal_ezi2c_cfg_t sEzI2C_cfg;

void handle_error(void)
{

    __disable_irq();

    CY_ASSERT(0);
}

void Capsense::RunCapSenseScan()
{
    Cy_CapSense_Wakeup(&cy_capsense_context);

    if (CY_CAPSENSE_NOT_BUSY == Cy_CapSense_IsBusy(&cy_capsense_context))
    {
        Cy_CapSense_ScanAllWidgets(&cy_capsense_context);
    }
    capsense_sem.acquire();
    Cy_CapSense_ProcessAllWidgets(&cy_capsense_context);
#if TUNER_ENABLE
    Cy_CapSense_RunTuner(&cy_capsense_context);
#endif
    ProcessTouchStatus();
}

void Capsense::initialize_capsense_tuner()
{
    cy_rslt_t result;

    sEzI2C_sub_cfg.buf             = (uint8 *) &cy_capsense_tuner;
    sEzI2C_sub_cfg.buf_rw_boundary = sizeof(cy_capsense_tuner);
    sEzI2C_sub_cfg.buf_size        = sizeof(cy_capsense_tuner);
    sEzI2C_sub_cfg.slave_address   = 8U;

    sEzI2C_cfg.data_rate              = CYHAL_EZI2C_DATA_RATE_400KHZ;
    sEzI2C_cfg.enable_wake_from_sleep = true;
    sEzI2C_cfg.slave1_cfg             = sEzI2C_sub_cfg;
    sEzI2C_cfg.sub_address_size       = CYHAL_EZI2C_SUB_ADDR16_BITS;
    sEzI2C_cfg.two_addresses          = false;
    result                            = cyhal_ezi2c_init(&sEzI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL, &sEzI2C_cfg);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("i2c error.\r\n");
        handle_error();
    }
}

void Capsense::ProcessTouchStatus()
{
    uint32_t currSliderPos;
    uint32_t currBtn0Status =
        Cy_CapSense_IsSensorActive(CY_CAPSENSE_BUTTON0_WDGT_ID, CY_CAPSENSE_BUTTON0_SNS0_ID, &cy_capsense_context);
    uint32_t currBtn1Status =
        Cy_CapSense_IsSensorActive(CY_CAPSENSE_BUTTON1_WDGT_ID, CY_CAPSENSE_BUTTON1_SNS0_ID, &cy_capsense_context);
    cy_stc_capsense_touch_t * sldrTouch = Cy_CapSense_GetTouchInfo(CY_CAPSENSE_LINEARSLIDER0_WDGT_ID, &cy_capsense_context);

    if (currBtn0Status != prevBtn0Status)
    {
        // printf("Button_0 status: %lu\r\n", (unsigned long) currBtn0Status);
        prevBtn0Status = currBtn0Status;
        if (!currBtn0Status)
        {
            Capsense::getInstance().on_button_released(0);
        }
        else
        {
            Capsense::getInstance().on_button_pressed(0);
        }
    }

    if (currBtn1Status != prevBtn1Status)
    {
        // printf("Button_1 status: %lu\r\n", (unsigned long) currBtn1Status);
        prevBtn1Status = currBtn1Status;
        if (!currBtn1Status)
        {
            Capsense::getInstance().on_button_released(1);
        }
        else
        {
            Capsense::getInstance().on_button_pressed(1);
        }
    }

    if (sldrTouch->numPosition == SLIDER_NUM_TOUCH)
    {
        currSliderPos = sldrTouch->ptrPosition->x;

        if (currSliderPos != prevSliderPos)
        {
            prevSliderPos = currSliderPos;
            Capsense::getInstance().on_slider(currSliderPos);
        }
    }
}

void Capsense::CapSense_InterruptHandler(void)
{
    Cy_CapSense_InterruptHandler(CYBSP_CSD_HW, &cy_capsense_context);
}

void Capsense::CapSenseEndOfScanCallback(cy_stc_active_scan_sns_t * ptrActiveScan)
{
    capsense_sem.release();
}

void Capsense::init()
{
    cybsp_init();
    initialize_capsense_tuner();
    sleep_manager_lock_deep_sleep();

    cy_status status = Cy_CapSense_Init(&cy_capsense_context);

    if (CY_RET_SUCCESS != status)
    {
        tr_err("CapSense initialization failed. Status code: %lu\r\n", (unsigned long) status);
        return;
    }

    Cy_SysInt_Init(&CapSense_ISR_cfg, &CapSense_InterruptHandler);
    NVIC_ClearPendingIRQ(CapSense_ISR_cfg.intrSrc);
    NVIC_EnableIRQ(CapSense_ISR_cfg.intrSrc);

    Cy_CapSense_Enable(&cy_capsense_context);
    Cy_SysPm_RegisterCallback(&capsenseDeepSleepCb);
    Cy_CapSense_RegisterCallback(CY_CAPSENSE_END_OF_SCAN_E, CapSenseEndOfScanCallback, &cy_capsense_context);

    static Thread thread(osPriorityNormal, OS_STACK_SIZE, NULL, "CapSense Scan Thread");
    thread.start(callback(&queue, &EventQueue::dispatch_forever));
    queue.call_every(CAPSENSE_SCAN_PERIOD_MS, RunCapSenseScan);

    Cy_CapSense_ScanAllWidgets(&cy_capsense_context);
}

namespace mbed {

void CapsenseButton::rise(Callback<void()> func)
{
    if (func)
    {
        _rise = func;
    }
    else
    {
        _rise = nullptr;
    }
}

void CapsenseButton::fall(Callback<void()> func)
{
    if (func)
    {
        _fall = func;
    }
    else
    {
        _fall = nullptr;
    }
}

void CapsenseSlider::on_move(Callback<void(int)> func)
{
    if (func)
    {
        _on_move = func;
    }
    else
    {
        _on_move = nullptr;
    }
}
} // namespace mbed