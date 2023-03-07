/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef _HAL_NVIC_INTERNAL_H_
#define _HAL_NVIC_INTERNAL_H_
#include "hal_nvic.h"

#ifdef HAL_NVIC_MODULE_ENABLED
/* Please refer to configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY= 4 in freertos_config.h */
#define FREERTOS_ISR_API_PRIORITY  (4)          /* please keep the setting */
/* DEFAULT_IRQ_PRIORITY is from FREERTOS_ISR_API_PRIORITY =  4  to  configKERNEL_INTERRUPT_PRIORITY -1 = (0xFF - 1)
    as pend_sv should be lowest irq priority to avoid unneccessary freert_os schedule */
#define DEFAULT_IRQ_PRIORITY  (FREERTOS_ISR_API_PRIORITY + 1)     /* please keep the setting */

#define RESERVED_IRQ_PRIORITY   ((1<<__NVIC_PRIO_BITS) - 1)

#define WIC_INT_IRQ_PRIORITY                 (DEFAULT_IRQ_PRIORITY + 0)
#define GPT_IRQ_PRIORITY                     (DEFAULT_IRQ_PRIORITY + 1)
#define GPT3_IRQ_PRIORITY                    (DEFAULT_IRQ_PRIORITY + 2)
#define WDT_IRQ_PRIORITY                     (DEFAULT_IRQ_PRIORITY + 3)
#define UART_IRQ_PRIORITY                    (DEFAULT_IRQ_PRIORITY + 4)
#define INFRA_BUS_IRQ_PRIORITY               (DEFAULT_IRQ_PRIORITY + 5)
#define CDBGPWRUPREQ_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 6)
#define CDBGPWRUPACK_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 7)
#define DSP_UART_IRQ_PRIORITY                (DEFAULT_IRQ_PRIORITY + 8)
#define TOP_UART0_IRQ_PRIORITY               (DEFAULT_IRQ_PRIORITY + 9)
#define TOP_UART1_IRQ_PRIORITY               (DEFAULT_IRQ_PRIORITY + 10)
#define I2C0_IRQ_PRIORITY                    (DEFAULT_IRQ_PRIORITY + 11)
#define I2C1_IRQ_PRIORITY                    (DEFAULT_IRQ_PRIORITY + 12)
#define SDCTL_TOP_FW_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 13)
#define SDCTL_TOP_FW_QOUT_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 14)
#define SPIM0_IRQ_PRIORITY                   (DEFAULT_IRQ_PRIORITY + 15)
#define SPIM1_IRQ_PRIORITY                   (DEFAULT_IRQ_PRIORITY + 16)
#define SPIS_IRQ_PRIORITY                    (DEFAULT_IRQ_PRIORITY + 17)
#define KP_IRQ_PRIORITY                      (DEFAULT_IRQ_PRIORITY + 18)
#define IRRX_IRQ_PRIORITY                    (DEFAULT_IRQ_PRIORITY + 19)
#define DSP_TIMER_IRQ_PRIORITY               (DEFAULT_IRQ_PRIORITY + 20)
#define SSUSB_XHCI_IRQ_PRIORITY              (DEFAULT_IRQ_PRIORITY + 21)
#define SSUSB_OTG_IRQ_PRIORITY               (DEFAULT_IRQ_PRIORITY + 22)
#define SSUSB_DEV_IRQ_PRIORITY               (DEFAULT_IRQ_PRIORITY + 23)
#define AFE_MCU_IRQ_PRIORITY                 (DEFAULT_IRQ_PRIORITY + 24)



/* use 'C statement expression' syntax */
#define save_and_set_interrupt_mask() \
    ( \
        { \
            uint32_t _m = __get_PRIMASK(); \
            __disable_irq(); \
            __DMB(); \
            __ISB(); \
            _m; \
        } \
    )
#define restore_interrupt_mask(_m) \
    do { \
        if (!(_m)) \
            __enable_irq(); \
        __DMB(); \
        __ISB(); \
    } while (0)


extern hal_nvic_status_t nvic_irq_software_trigger(hal_nvic_irq_t irq_number);
uint32_t hal_nvic_query_exception_number(void);
void nvic_mask_all_interrupt(void);
void nvic_unmask_all_interrupt(void);
void nvic_clear_all_pending_interrupt(void);

#endif /* #ifdef HAL_NVIC_MODULE_ENABLED */
#endif /* #ifndef _HAL_NVIC_INTERNAL_H_ */

