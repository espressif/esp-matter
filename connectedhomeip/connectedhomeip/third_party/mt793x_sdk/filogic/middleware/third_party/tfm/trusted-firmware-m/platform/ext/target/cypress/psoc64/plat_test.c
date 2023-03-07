/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "cy_tcpwm_counter.h"
#include "device_definition.h"
#include "tfm_plat_defs.h"
#include "tfm_plat_test.h"

void tfm_plat_test_secure_timer_start(void)
{
    cy_en_tcpwm_status_t rc;

    if (!CY_TCPWM0_TIMER0_DEV_S.is_initialized) {
        Cy_TCPWM_Counter_Disable(
            CY_TCPWM0_TIMER0_DEV_S.tcpwm_base,
            CY_TCPWM0_TIMER0_DEV_S.tcpwm_counter_num
        );

        CY_TCPWM0_TIMER0_DEV_S.tcpwm_config->period   = TIMER0_MATCH;
        CY_TCPWM0_TIMER0_DEV_S.tcpwm_config->compare0 = TIMER0_MATCH;
        rc = Cy_TCPWM_Counter_Init(CY_TCPWM0_TIMER0_DEV_S.tcpwm_base,
                                   CY_TCPWM0_TIMER0_DEV_S.tcpwm_counter_num,
                                   CY_TCPWM0_TIMER0_DEV_S.tcpwm_config);
        if (rc == CY_TCPWM_SUCCESS) {
            CY_TCPWM0_TIMER0_DEV_S.is_initialized = true;
        } else {
            return;
        }
    }
    Cy_TCPWM_Counter_Enable(
        CY_TCPWM0_TIMER0_DEV_S.tcpwm_base,
        CY_TCPWM0_TIMER0_DEV_S.tcpwm_counter_num
    );
    Cy_TCPWM_TriggerStart(
        CY_TCPWM0_TIMER0_DEV_S.tcpwm_base,
        (1UL << CY_TCPWM0_TIMER0_DEV_S.tcpwm_counter_num)
    );
}

void tfm_plat_test_secure_timer_stop(void)
{
    Cy_TCPWM_Counter_Disable(
        CY_TCPWM0_TIMER0_DEV_S.tcpwm_base,
        CY_TCPWM0_TIMER0_DEV_S.tcpwm_counter_num
    );
}

void tfm_plat_test_non_secure_timer_start(void)
{
    cy_en_tcpwm_status_t rc;

    if (!CY_TCPWM0_TIMER1_DEV_NS.is_initialized) {
        Cy_TCPWM_Counter_Disable(
            CY_TCPWM0_TIMER1_DEV_NS.tcpwm_base,
            CY_TCPWM0_TIMER1_DEV_NS.tcpwm_counter_num
        );

        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_config->period   = TIMER1_MATCH;
        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_config->compare0 = TIMER1_MATCH;
        rc = Cy_TCPWM_Counter_Init(CY_TCPWM0_TIMER1_DEV_NS.tcpwm_base,
                                   CY_TCPWM0_TIMER1_DEV_NS.tcpwm_counter_num,
                                   CY_TCPWM0_TIMER1_DEV_NS.tcpwm_config);
        if (rc == CY_TCPWM_SUCCESS) {
            CY_TCPWM0_TIMER1_DEV_NS.is_initialized = true;
        } else {
            return;
        }
    }
    Cy_TCPWM_Counter_Enable(
        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_base,
        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_counter_num
    );
    Cy_TCPWM_TriggerStart(
        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_base,
        (1UL << CY_TCPWM0_TIMER1_DEV_NS.tcpwm_counter_num)
    );
}

void tfm_plat_test_non_secure_timer_stop(void)
{
    Cy_TCPWM_Counter_Disable(
        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_base,
        CY_TCPWM0_TIMER1_DEV_NS.tcpwm_counter_num
    );
}
