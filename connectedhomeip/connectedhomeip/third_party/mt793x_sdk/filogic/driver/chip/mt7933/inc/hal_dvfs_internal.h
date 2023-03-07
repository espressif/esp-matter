/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __HAL_DVFS_INTERNAL_H__
#define __HAL_DVFS_INTERNAL_H__

#include "hal_platform.h"

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal.h"
#include "memory_attribute.h"
//#define HAL_DVFS_DEBUG_ENABLE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define DVFS_MODE_NONE (-1)
#define DVFS_NULL_HANDLE 0

typedef struct dvfs_lock_t {
    const char *name;
    unsigned int count;
    uint8_t lock_index;
    uint32_t dvfs_module_index;
    int count_1p3;
    int count_1p1;
    int count_0p9;
} dvfs_lock_t;

typedef enum {
    DVFS_LOW_SPEED_26M = 0,
    DVFS_HALF_SPEED_39M,
    DVFS_FULL_SPEED_78M,
    DVFS_HIGH_SPEED_156M,
    DVFS_MAX_SPEED,
} dvfs_voltage_mode_t;

typedef enum {
    DVFS_26M_SPEED = 0,
    DVFS_39M_SPEED,
    DVFS_78M_SPEED,
    DVFS_156M_SPEED,
    DVFS_CURRENT_SPEED,
    DVFS_ERROR_SPEED,
} dvfs_frequency_t;

typedef enum {
    DVFS_UNLOCK,
    DVFS_LOCK,
    DVFS_UNLOCK_ALONE,
    DVFS_LOCK_ALONE,
} dvfs_lock_parameter_t;
typedef struct {
    const char *name; //domain name
    bool initialized; //is initialized or not
    unsigned int cur_opp_index; //now frequency index from table ;dvfs_voltage_mode_t
    uint32_t opp_num; //  156M 78M 26M 1.3 1.1 0.9 0.7
    unsigned int module_num; // how much frequency  about VCORE /DSP
    const uint32_t *voltage; //how much voltage can use 1.3 1.1 0.9
    const uint32_t *frequency; //how muchfrequency  208000, 104000, 26000
    const uint32_t *pmu_mapping_volt;
    void (*switch_voltage)(unsigned int cur_opp, unsigned int next_opp);
    void (*switch_frequency)(unsigned int cur_opp, unsigned int next_opp);
    uint8_t basic_opp_index;
} dvfs_opp_t;

#define dvfs_enter_privileged_level() \
    do { \
        register uint32_t control = __get_CONTROL(); \
        CONTROL_Type pControl; \
        *(uint32_t *)&pControl = control; \
        if (pControl.b.SPSEL == 1) { \
            /* Alter MSP as stack pointer. */ \
            dvfs_switched_to_privileged = TRUE; \
            pControl.b.SPSEL = 0; \
            control = *(uint32_t *)&pControl; \
            __ISB(); \
            __DSB(); \
            __set_CONTROL(control); \
            __ISB(); \
            __DSB(); \
        } \
    } while(0)

#define dvfs_exit_privileged_level() \
    do { \
        register uint32_t control = __get_CONTROL(); \
        CONTROL_Type pControl; \
        if (dvfs_switched_to_privileged == TRUE) { \
            *(uint32_t *)&pControl = control; \
            dvfs_switched_to_privileged = FALSE; \
            pControl.b.SPSEL = 1; \
            control = *(uint32_t *)&pControl; \
            __ISB(); \
            __DSB(); \
            __set_CONTROL(control); \
            __ISB(); \
            __DSB(); \
        } \
    } while(0)
dvfs_opp_t *dvfs_domain_init(void);
extern bool dvfs_switched_to_privileged;
hal_dvfs_status_t dvfs_lock_control(char *handle_name, dvfs_frequency_t freq, dvfs_lock_parameter_t lock);
uint8_t dvfs_get_current_state(void);
void dvfs_upload_date_global_value(int case_index, int oper);
void dvfs_vcore_switch_voltage(unsigned int c_voltage, unsigned int n_voltage);
ATTR_TEXT_IN_SYSRAM void dvfs_vcore_switch_frequency(unsigned int cur_opp, unsigned int next_opp);
void dvfs_set_register_value(uint32_t address, short int mask, short int shift, short int value);
uint32_t dvfs_get_register_value(uint32_t address, short int mask, short int shift);
int dvfs_query_frequency(uint32_t freq, const uint32_t *frequency, uint32_t num, hal_dvfs_freq_relation_t relation);
int dvfs_check_pmic_voltage(unsigned int index);
int dvfs_search_opp(dvfs_opp_t *domain, uint32_t target_freq, hal_dvfs_freq_relation_t relation);
hal_dvfs_status_t hal_dvfs_reset_basic_setting(void);
hal_dvfs_status_t dvfs_debug_dump(void);
#endif /* #ifdef HAL_DVFS_MODULE_ENABLED */

#endif /* #ifndef __HAL_DVFS_INTERNAL_H__ */
