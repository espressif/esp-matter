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

#ifndef __HAL_MTK_SDIO_H__
#define __HAL_MTK_SDIO_H__

#include "hal_sdio.h"

#ifdef HAL_SDIO_MODULE_ENABLED

#include "hal_sd_define.h"

/*define use sdio dat1 pin interrupt*/
#define USE_SDIO_DTA1_INT

#define MSDC_TIMEOUT_PERIOD_INIT        1100000        /* us*/
#define MSDC_TIMEOUT_PERIOD_DATA        1500000        /* us*/
#define MSDC_TIMEOUT_PERIOD_COMMAND     500000         /* us*/


/*MSDC Command configuration define*/
#define COMMAND_NO_RESPONSE         0x0000
#define COMMAND_R1_RESPONSE         0x0080
#define COMMAND_R2_RESPONSE         0x0100
#define COMMAND_R3_RESPONSE         0x0180
#define COMMAND_R4_RESPONSE         0x0200
#define COMMAND_R5_RESPONSE         0x0280
#define COMMAND_R6_RESPONSE         0x0300
#define COMMAND_R1B_RESPONSE        0x0380

#define COMMAND_NO_ARGUMENT           (0)

#define SD_MID_BYTE_SHIFT             (15)
#define COMMAND_RCA_ARGUMENT_SHIFT    (16)

#define SDIO_CCCR_SIZE   (32)
#define SDIO_FBR_SIZE    (256)
#define SDIO_TUPLE_SIZE  (256)

#define CISTPL_NULL (0)
#define CISTPL_END  (0xff)
#define CISTPL_MANFID  (0x20)

/*CCCR register define,card capability(Byte 0x13)*/
#define SDIO_CCCR_EHS_BIT_SHIFT       (1)
#define SDIO_CCCR_SHS_BIT_SHIFT       (0)

#define SDIO_CCCR_EHS_BIT_MASK        (1 << SDIO_CCCR_EHS_BIT_SHIFT)
#define SDIO_CCCR_SHS_BIT_MASK        (1 << SDIO_CCCR_SHS_BIT_SHIFT)

/*CCCR register define,card capability(Byte 8)*/
#define SDIO_CCCR_4BLS_BIT_SHIFT       (7)
#define SDIO_CCCR_LSC_BIT_SHIFT        (6)
#define SDIO_CCCR_E4MI_BIT_SHIFT       (5)
#define SDIO_CCCR_S4MI_BIT_SHIFT       (4)

#define SDIO_CCCR_4BLS_BIT_MASK        (1 << SDIO_CCCR_4BLS_BIT_SHIFT)
#define SDIO_CCCR_LSC_BIT_MASK         (1 << SDIO_CCCR_LSC_BIT_SHIFT)
#define SDIO_CCCR_E4MI_BIT_MASK        (1 << SDIO_CCCR_E4MI_BIT_SHIFT)
#define SDIO_CCCR_S4MI_BIT_MASK        (1 << SDIO_CCCR_S4MI_BIT_SHIFT)

/*CCCR register define,bus interface control(Byte 7)*/
#define SDIO_CCCR_CD_BIT_SHIFT        (7)
#define SDIO_CCCR_BUS1_BIT_SHIFT      (1)
#define SDIO_CCCR_BUS0_BIT_SHIFT      (0)

#define SDIO_CCCR_CD_BIT_MASK         (1 << SDIO_CCCR_CD_BIT_SHIFT)
#define SDIO_CCCR_BUS1_BIT_MASK       (1 << SDIO_CCCR_BUS1_BIT_SHIFT)
#define SDIO_CCCR_BUS0_BIT_MASK       (1 << SDIO_CCCR_BUS0_BIT_SHIFT)

#define COMMAND_STOP_TRANSMISSION                   0x4000
#define COMMAND_NOT_STOP_TRANSMISSION               0x0000


#define COMMAND3_SET_RELATIVE_ADDR      0x03    /* Assigns relative address to the card in identification state */
#define COMMAND7_SELECT_CARD            0x07
#define COMMAND12_STOP_TRANSMISSION     0x0C    /* Stop data transmission */

#define MSDC_COMMAND3           (COMMAND3_SET_RELATIVE_ADDR  | COMMAND_R6_RESPONSE)
#define MSDC_COMMAND7           (COMMAND7_SELECT_CARD  | COMMAND_R1B_RESPONSE)
#define MSDC_COMMAND12          (COMMAND12_STOP_TRANSMISSION | COMMAND_R1B_RESPONSE | COMMAND_STOP_TRANSMISSION)    /* value = 0x438c */



/*SDIO command*/
#define COMMAND5_READ_OCR                  (0x05)
#define COMMAND52_READ_WRITE_DIRECT        (0x34)
#define COMMAND53_READ_WRITE_EXTENDED      (0x35)

/*SDIO cmomand, for MSDC use directly */
#define MSDC_COMMAND5_SDIO       (COMMAND5_READ_OCR | COMMAND_R3_RESPONSE)              /* value = 0x0185, use R3 to replace R4 */
#define MSDC_COMMAND52_SDIO      (COMMAND52_READ_WRITE_DIRECT | COMMAND_R5_RESPONSE)    /* value = 0x02B4 */
#define MSDC_COMMAND53_SDIO      (COMMAND53_READ_WRITE_EXTENDED | COMMAND_R1_RESPONSE)  /* value = 0x0B5 */


/**********************************************************************************************************/
/******************************************CMD5 RESPONSE***************************************************/
/**********************************************************************************************************/
/*| 1 | 2 |     6    | 1 |           3            |       1        |      3     |    24   |     7    | 1 |*/
/*| S | D | Reserved | C | Numberof I/O functions | Memory Present | Stuff Bits | I/O OCR | Reserved | E |*/
/**********************************************************************************************************/
#define SDIO_COMMAND5_RESPONSE_IS_READY_BIT_SHIFT          (31)
#define SDIO_COMMAND5_RESPONSE_IO_FUNCTION_BIT_SHIFT       (28)
#define SDIO_COMMAND5_RESPONSE_MEMORY_PRESENT_BIT_SHIFT    (27)
#define SDIO_COMMAND5_RESPONSE_OCR_BIT_SHIFT               (0)


#define SDIO_COMMAND5_RESPONSE_IS_READY_BIT_MASK           ((uint32_t)0x01 << SDIO_COMMAND5_RESPONSE_IS_READY_BIT_SHIFT)
#define SDIO_COMMAND5_RESPONSE_IO_FUNCTION_BIT_MASK        (0x07 << SDIO_COMMAND5_RESPONSE_IO_FUNCTION_BIT_SHIFT)
#define SDIO_COMMAND5_RESPONSE_MEMORY_PRESENT_BIT_MASK     (1 << SDIO_COMMAND5_RESPONSE_MEMORY_PRESENT_BIT_SHIFT)
#define SDIO_COMMAND5_RESPONSE_OCR_BIT_MASK                (0xffffff << SDIO_COMMAND5_RESPONSE_OCR_BIT_SHIFT)

/********************************************************************/
/**************************CMD52 RESPONSE (R5)***********************/
/********************************************************************/
/*| 1 | 1 |       6       |  16   |       8       |   8  |  7  | 1 |*/
/*| S | D | Command index | stuff | response flag | data | CRC | E |*/
/********************************************************************/
#define SDIO_R5_RESPONSE_FLAG_BIT_SHIFT   (8)
#define SDIO_R5_RESPONSE_DATA_BIT_SHIFT   (0)

#define SDIO_R5_RESPONSE_FLAG_BIT_MASK    (0xff << SDIO_R5_RESPONSE_FLAG_BIT_SHIFT)
#define SDIO_R5_RESPONSE_DATA_BIT_MASK    (0xff << SDIO_R5_RESPONSE_DATA_BIT_SHIFT)

/*R5 response flag define*/
#define SDIO_R5_RESPONSE_FLAG_STATE_BIT_SHIFT   (4 + SDIO_R5_RESPONSE_FLAG_BIT_SHIFT)

#define SDIO_R5_RESPONSE_FLAG_STATE_BIT_MASK    (0x3 << SDIO_R5_RESPONSE_FLAG_STATE_BIT_SHIFT)
#define SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK    (0xcb)


#define SDIO_CARD_STATUS_ERROR_MASK          (0xffff0000)


/**************************************************************************************************************************/
/************************************************CMD52 ********************************************************************/
/**************************************************************************************************************************/
/*| 1 | 1 |       6       |  1  |           3            |  1  |  1    |         17       |   1   |      8     |  7  | 1 |*/
/*| S | D | Command index | R/W | Numberof I/O functions | RAW | stuff | register address | stuff | write data | CRC | E |*/
/**************************************************************************************************************************/

#define COMMAND52_DIRECTION_BIT_SHIFT         (31)
#define COMMAND52_FUNCTION_NUMBER_BIT_SHIFT   (28)
#define COMMAND52_RAW_BIT_SHIFT               (27)
#define COMMAND52_REGISTER_ADDRESS_BIT_SHIFT  (9)
#define COMMAND52_WRITE_DATA_BIT_SHIFT        (0)

#define COMMAND52_DIRECTION_BIT_MASK          ((uint32_t)1 << COMMAND52_DIRECTION_BIT_SHIFT)
#define COMMAND52_FUNCTION_NUMBER_BIT_MASK    (0x7 << COMMAND52_FUNCTION_NUMBER_BIT_SHIFT)
#define COMMAND52_RAW_BIT_MASK                (1 << COMMAND52_RAW_BIT_SHIFT)
#define COMMAND52_REGISTER_ADDRESS_BIT_MASK   (0x1ffff << COMMAND52_REGISTER_ADDRESS_BIT_SHIFT)
#define COMMAND52_WRITE_DATA_BIT_MASK         (0xff << COMMAND52_WRITE_DATA_BIT_SHIFT)


/*********************************************************************************************************************************/
/************************************************CMD53 ***************************************************************************/
/*********************************************************************************************************************************/
/*| 1 | 1 |       6       |  1  |           3            |      1     |    1    |         17       |         9        |  7  | 1 |*/
/*| S | D | Command index | R/W | Numberof I/O functions | Block mode | OP code | register address | byte/block count | CRC | E |*/
/*********************************************************************************************************************************/
#define COMMAND53_DIRECTION_BIT_SHIFT               (31)
#define COMMAND53_FUNCTION_NUMBER_BIT_SHIFT         (28)
#define COMMAND53_BLOCK_MODE_BIT_SHIFT              (27)
#define COMMAND53_OP_BIT_SHIFT                      (26)
#define COMMAND53_REGISTER_ADDRESS_BIT_SHIFT        (9)
#define COMMAND53_COUNT_BIT_SHIFT                   (0)

#define COMMAND53_DIRECTION_BIT_MASK                ((uint32_t)1 << COMMAND53_DIRECTION_BIT_SHIFT)
#define COMMAND53_FUNCTION_NUMBER_BIT_MASK          (0x07 << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT)
#define COMMAND53_BLOCK_MODE_BIT_MASK               (1 << COMMAND53_BLOCK_MODE_BIT_SHIFT)
#define COMMAND53_OP_BIT_MASK                       (1 << COMMAND53_OP_BIT_SHIFT)
#define COMMAND53_REGISTER_ADDRESS_BIT_MASK         (0x1ffff << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT)
#define COMMAND53_COUNT_BIT_MASK                    (0x1ff << COMMAND53_COUNT_BIT_SHIFT)


#ifdef HAL_SLEEP_MANAGER_ENABLED
typedef struct {
    hal_sdio_config_t config;
    hal_sdio_port_t   sdio_port;
    bool            is_initialized;
} sdio_backup_parameter_t;
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */

typedef struct {
    uint8_t number_function;
    uint8_t response;
    hal_sdio_bus_width_t bus_width;
    uint8_t power_control;
    bool    is_memory_present;
    bool    is_io_ready;
    bool    is_ocr_valid;
    bool    is_stop;
    bool    is_initialized;
    sdio_state_t state;
    uint8_t capability;
    uint8_t high_speed;
    uint32_t rca;
    uint32_t ocr;
    uint32_t block_size[HAL_SDIO_FUNCTION_1 + 1];
    bool     is_busy;
    sdio_cis_t cis;
} sdio_information_t;


mtk_sdio_status_t sdio_command52(hal_sdio_command52_config_t *command52_config);
mtk_sdio_status_t sdio_wait_command_ready(void);
mtk_sdio_status_t sdio_send_command(uint32_t msdc_command, uint32_t argument);
mtk_sdio_status_t sdio_send_command5(uint32_t ocr);
mtk_sdio_status_t sdio_get_rca(void);
mtk_sdio_status_t sdio_wait_card_not_busy(void);
mtk_sdio_status_t sdio_wait_data_ready(void);
mtk_sdio_status_t sdio_check_card_status(void);
mtk_sdio_status_t sdio_select_card(uint32_t rca);
mtk_sdio_status_t sdio_get_cccr(void);
void sdio_get_capacity(void);
void sdio_get_power_control(void);
bool sdio_is_support_lsc(void);
bool sdio_is_support_s4mi(void);
bool sdio_is_support_4bls(void);
bool sdio_is_support_mps(void);
mtk_sdio_status_t sdio_set_mps(bool is_enable);
mtk_sdio_status_t sdio_set_e4mi(bool is_enable);
mtk_sdio_status_t sdio_set_io(hal_sdio_function_id_t function, bool is_enable);
mtk_sdio_status_t sdio_check_io_ready(hal_sdio_function_id_t function, bool *is_ready);
mtk_sdio_status_t sdio_get_fbr(hal_sdio_function_id_t function);
mtk_sdio_status_t sdio_get_cis(hal_sdio_function_id_t function);
mtk_sdio_status_t sdio_wait_last_block_complete(void);
mtk_sdio_status_t sdio_stop(void);
mtk_sdio_status_t sdio_command12_stop(void);
void sdio_record_error_status(mtk_sdio_status_t sdio_status);
void sdio_isr(void);
mtk_sdio_status_t sdio_set_ehs(bool is_enable);
void sdio_get_high_speed(void);
bool sdio_is_support_shs(void);
hal_sdio_status_t sdio_wait_dma_interrupt_transfer_ready(void);
hal_sdio_status_t sdio_interrupt_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback, void *user_data);
mtk_sdio_status_t sdio_set_function_interrupt(hal_sdio_function_id_t function, bool is_enable);


#ifdef HAL_SLEEP_MANAGER_ENABLED
void sdio_backup_all(void *data);
void sdio_restore_all(void *data);
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */
#endif /* #ifdef HAL_SDIO_MODULE_ENABLED */
#endif /* #ifndef __HAL_MTK_SDIO_H__ */

