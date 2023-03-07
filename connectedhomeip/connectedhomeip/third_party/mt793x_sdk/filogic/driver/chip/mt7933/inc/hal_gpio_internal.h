/* Copyright Statement:
 *
 * (C) 2005-2020  MediaTek Inc. All rights reserved.
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

#ifndef __HAL_GPIO_INTERNAL_H__
#define __HAL_GPIO_INTERNAL_H__

#include "hal_platform.h"
#include "hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/*************************** GPIO register definition start line  *******************************
 */

typedef struct {
    __IO uint32_t RW;                   /*!< GPIO RW register */
    __IO uint32_t SET;                  /*!< GPIO set register */
    __IO uint32_t CLR;                  /*!< GPIO clear register */
    uint32_t  RESERVED;                 /*!< reserved */
} GPIO_REGISTER_T;


typedef struct {
    __IO uint32_t R;                    /*!< GPIO input data register */
    uint32_t  RESERVED[3];              /*!< reserved */
} GPIO_DIN_REGISTER_T;



typedef struct {
    __IO uint32_t RW;                /*!< GPIO RW register */
    __IO uint32_t SET;               /*!< GPIO set register */
    __IO uint32_t CLR;               /*!< GPIO clear register */
    __IO uint32_t MOD;               /*!< GPIO mod register */
} GPIO_MODE_REGISTER_T;


typedef struct {
    __IO uint32_t DUMMY;
    __IO uint32_t DUMMY_SET;
    __IO uint32_t DUMMY_CLR;
    uint32_t  GPIO_OFFSET;
} GPIO_DUMMY_REGISTER_T;

typedef struct {
    GPIO_REGISTER_T         GPIO_DIR[2];               /*!< GPIO direction register 0x000~0x01f */
    GPIO_REGISTER_T         reserved0[14];
    GPIO_REGISTER_T         GPIO_DOUT[2];              /*!< GPIO output data register 0x100~0x11f */
    GPIO_REGISTER_T         reserved1[14];
    GPIO_DIN_REGISTER_T     GPIO_DIN[2];               /*!< 4 input data register 0x200~0x21f */
    GPIO_REGISTER_T         reserved2[14];
    GPIO_MODE_REGISTER_T    GPIO_MODE[7];              /*!< GPIO mode register 0x300~0x36f */
} GPIO_BASE_REGISTER_T;

typedef struct {
    GPIO_REGISTER_T         AIO_ANALOG[2];          /*!< GPIO AIO_ANALOG register 0x000~0x01f */
    GPIO_REGISTER_T         GPIO_DRV[3];            /*!< GPIO DRV register 0x020~0x04f */
    GPIO_REGISTER_T         GPIO_EH0[2];            /*!< GPIO EH0 register 0x050~0x06f */
    GPIO_REGISTER_T         GPIO_EH[2];             /*!< GPIO EH register 0x070~0x08f */
    GPIO_REGISTER_T         GPIO_G[2];              /*!< GPIO G register 0x090~0x0af */
    GPIO_REGISTER_T         GPIO_IES[2];            /*!< GPIO IES register 0x0b0~0x0cf */
    GPIO_REGISTER_T         GPIO_PUPD[2];           /*!< GPIO PUPD register 0x0d0~0x0ef */
    GPIO_REGISTER_T         GPIO_R0[2];             /*!< GPIO R0 register 0x0f0~0x10f */
    GPIO_REGISTER_T         GPIO_R1[2];             /*!< GPIO R1 register 0x110~0x12f */
} GPIO_CFG0_REGISTER_T;


#define GPIO_BASE_REGISTER  ((GPIO_BASE_REGISTER_T*)GPIO_BASE_BASE)
#define GPIO_CFG0_REGISTER  ((GPIO_CFG0_REGISTER_T*)CHIP_PINMUX_BASE)



#ifdef HAL_GPIO_MODULE_ENABLED

#define HAL_GPIO_FEATURE_SET_IES

#define GPIO_REG_ONE_BIT_SET_CLR            0x1
#define GPIO_REG_TWO_BIT_SET_CLR            0x3
#define GPIO_REG_FOUR_BIT_SET_CLR           0xF
#define GPIO_MODE_MIN_NUMBER                0       /*  minimum function index of GPIO  */
#define GPIO_MODE_MAX_NUMBER                8      /*  maximum function index of GPIO  */
#define GPIO_REG_CTRL_PIN_NUM_OF_32         32      /*  pin munber controlled by one register is 32  */
#define GPIO_MODE_FUNCTION_CTRL_BITS        4       /*  mode function is controlled by 4 bits  */
#define GPIO_TDSEL_FUNCTION_CTRL_BITS       2       /*  TDSEL function is controlled by 2 bits  */
#define GPIO_DRV_FUNCTION_CTRL_BITS         2       /*  DRV function is controlled by 2 bits  */
#define GPIO_DIR_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by direction register is 32  */
#define GPIO_MODE_REG_CTRL_PIN_NUM          8       /*  pin munber controlled by mode register is 8  */
#define GPIO_DIN_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by inputd data register is 32  */
#define GPIO_DOUT_REG_CTRL_PIN_NUM          32      /*  pin munber controlled by output data register is 32  */
#ifdef HAL_GPIO_FEATURE_INVERSE
#define GPIO_DINV_REG_CTRL_PIN_NUM          32      /*  pin munber controlled by inputd data inverse register is 32  */
#endif /* #ifdef HAL_GPIO_FEATURE_INVERSE */
#define GPIO_PULL_CTRL_REG_CTRL_PIN_NUM     32      /*  pin munber controlled by pull control register is 32  */
#ifdef HAL_GPIO_FEATURE_PUPD
#define GPIO_PUPD_CTRL_REG_CTRL_PIN_NUM     32      /*  pin munber controlled by PUPD control register is 32  */
#endif /* #ifdef HAL_GPIO_FEATURE_PUPD */
#define GPIO_TDSEL_REG_CTRL_PIN_NUM         16      /*  pin munber controlled by TDSEL register is 16  */
#define GPIO_IES_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by IES register is 32  */
#define GPIO_SR_REG_CTRL_PIN_NUM            32      /*  pin munber controlled by SR register is 32  */
#define GPIO_DRV_REG_CTRL_PIN_NUM           16      /*  pin munber controlled by DRV register is 16  */

typedef enum {
    GPIO_NO_PULL  = 0,
    GPIO_PU_10K   = 1,
    GPIO_PD_10K   = 2,
    GPIO_PU_50K   = 3,
    GPIO_PD_50K   = 4,
    GPIO_PU_75K   = 5,
    GPIO_PD_75K   = 6,
    GPIO_PU_2K   = 7,
    GPIO_PD_2K   = 8,
    GPIO_PU_200K   = 9,
    GPIO_PD_200K   = 10,
    GPIO_PU_50K_10K   = 11,
    GPIO_PD_50K_10K   = 12,
    GPIO_PU_2K_75K   = 13,
    GPIO_PD_2K_75K   = 14,
    GPIO_PU_200K_75K   = 15,
    GPIO_PD_200K_75K   = 16,
    GPIO_PUPD_ERR = 17,
} gpio_pull_type_t;
typedef struct {
    uint8_t mode;
    uint8_t dir;
    hal_gpio_data_t din;
    hal_gpio_data_t dout;
    gpio_pull_type_t pull_type;
    hal_gpio_driving_current_t current_type;
} gpio_state_t;

void gpio_get_state(hal_gpio_pin_t gpio_pin, gpio_state_t *gpio_state);

void hal_gpio_suspend(void *data);
void hal_gpio_resume(void *data);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifdef HAL_GPIO_MODULE_ENABLED */
#endif /* #ifndef __HAL_GPIO_INTERNAL_H__ */

