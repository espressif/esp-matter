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

#ifndef __HAL_SPI_MASTER_INTERNAL_H__
#define __HAL_SPI_MASTER_INTERNAL_H__

#include "hal_platform.h"

#ifdef HAL_SPI_MASTER_MODULE_ENABLED

#define SPI_CFG0_REG    0x0000
#define SPI_CFG1_REG    0x0004
#define SPI_TX_SRC_REG  0x0008
#define SPI_RX_DST_REG  0x000C
#define SPI_TX_DATA_REG 0x0010
#define SPI_RX_DATA_REG 0x0014
#define SPI_CMD_REG     0x0018
#define SPI_STATUS0_REG 0x001C
#define SPI_STATUS1_REG 0x0020
#define SPI_CFG2_REG    0x0028

/* SPI_CFG0_REG */
enum {
    SPI_CFG0_CS_HOLD_SHIFT = 0,
    SPI_CFG0_CS_SETUP_SHIFT = 16,

    SPI_CFG0_CS_HOLD_MASK = 0Xffff << SPI_CFG0_CS_HOLD_SHIFT,
    SPI_CFG0_CS_SETUP_MASK = 0Xffff << SPI_CFG0_CS_SETUP_SHIFT,
};

/* SPI_CFG1_REG */
enum {
    SPI_CFG1_CS_IDLE_SHIFT = 0,
    SPI_CFG1_PACKET_LOOP_SHIFT = 8,
    SPI_CFG1_PACKET_LENGTH_SHIFT = 16,
    SPI_CFG1_GET_TICK_DLY_SHIFT = 29,

    SPI_CFG1_CS_IDLE_MASK = 0xff << SPI_CFG1_CS_IDLE_SHIFT,
    SPI_CFG1_PACKET_LOOP_MASK = 0xff << SPI_CFG1_PACKET_LOOP_SHIFT,
    SPI_CFG1_PACKET_LENGTH_MASK = 0x3ff << SPI_CFG1_PACKET_LENGTH_SHIFT,
    SPI_CFG1_GET_TICK_DLY_MASK = 0x7 << SPI_CFG1_GET_TICK_DLY_SHIFT,
};

/* SPI_CFG2_REG */
enum {
    SPI_CFG2_SCK_HIGH_SHIFT = 0,
    SPI_CFG2_SCK_LOW_SHIFT = 16,
    SPI_CFG2_SCK_HIGH_MASK = 0xffff,
    SPI_CFG2_SCK_LOW_MASK = 0xffff0000,
};

/* SPI_CMD */
enum {
    SPI_CMD_ACT_SHIFT = 0,
    SPI_CMD_RESUME_SHIFT = 1,
    SPI_CMD_RST_SHIFT = 2,
    SPI_CMD_PAUSE_EN_SHIFT = 4,
    SPI_CMD_DEASSERT_SHIFT = 5,
    SPI_CMD_SAMPLE_SEL_SHIFT = 6,
    SPI_CMD_CS_POL_SHIFT = 7,
    SPI_CMD_CPHA_SHIFT = 8,
    SPI_CMD_CPOL_SHIFT = 9,
    SPI_CMD_RX_DMA_SHIFT = 10,
    SPI_CMD_TX_DMA_SHIFT = 11,
    SPI_CMD_TXMSBF_SHIFT = 12,
    SPI_CMD_RXMSBF_SHIFT = 13,
    SPI_CMD_RX_ENDIAN_SHIFT = 14,
    SPI_CMD_TX_ENDIAN_SHIFT = 15,
    SPI_CMD_FINISH_IE_SHIFT = 16,
    SPI_CMD_PAUSE_IE_SHIFT = 17,
    SPI_CMD_CS_PIN_SEL_SHIFT = 18,

    SPI_CMD_ACT_EN = 1 << SPI_CMD_ACT_SHIFT,
    SPI_CMD_RESUME_EN = 1 << SPI_CMD_RESUME_SHIFT,
    SPI_CMD_RST_EN = 1 << SPI_CMD_RST_SHIFT,
    SPI_CMD_PAUSE_EN = 1 << SPI_CMD_PAUSE_EN_SHIFT,
    SPI_CMD_DEASSERT_EN = 1 << SPI_CMD_DEASSERT_SHIFT,
    SPI_CMD_CS_POL_EN = 1 << SPI_CMD_CS_POL_SHIFT,
    SPI_CMD_SAMPLE_SEL_EN = 1 << SPI_CMD_SAMPLE_SEL_SHIFT,
    SPI_CMD_CPHA_EN = 1 << SPI_CMD_CPHA_SHIFT,
    SPI_CMD_CPOL_EN = 1 << SPI_CMD_CPOL_SHIFT,
    SPI_CMD_RX_DMA_EN = 1 << SPI_CMD_RX_DMA_SHIFT,
    SPI_CMD_TX_DMA_EN = 1 << SPI_CMD_TX_DMA_SHIFT,
    SPI_CMD_TXMSBF_EN = 1 << SPI_CMD_TXMSBF_SHIFT,
    SPI_CMD_RXMSBF_EN = 1 << SPI_CMD_RXMSBF_SHIFT,
    SPI_CMD_RX_ENDIAN_EN = 1 << SPI_CMD_RX_ENDIAN_SHIFT,
    SPI_CMD_TX_ENDIAN_EN = 1 << SPI_CMD_TX_ENDIAN_SHIFT,
    SPI_CMD_FINISH_IE_EN = 1 << SPI_CMD_FINISH_IE_SHIFT,
    SPI_CMD_PAUSE_IE_EN = 1 << SPI_CMD_PAUSE_IE_SHIFT,
    SPI_CMD_CS_PIN_SEL_EN = 1 << SPI_CMD_CS_PIN_SEL_SHIFT,
};

enum {
    MTK_SPI_BUSY_STATUS = 0,
    MTK_SPI_IDLE_STATUS = 1,
    MTK_SPI_FINISH_STATUS = 1,
    MTK_SPI_PAUSE_STATUS = 2,
    MTK_SPI_PAUSE_FINISH_INT_STATUS = 3
};

enum {
    MTK_FIFO_DEPTH = 32,
    MTK_PACKET_LENGTH = 1024,
    MTK_PACKET_LOOP_CNT = 256,
    MTK_TIMEOUT_MS = 10 * 1000,
    MTK_ARBITRARY_VALUE = 0xdeaddead
};

struct mtk_spim_bus {
    unsigned long reg_base;
    int irq_num;
};

typedef enum {
    SPI_MASTER_STATUS_UNINITIALIZED,
    SPI_MASTER_STATUS_POLLING_MODE,
    SPI_MASTER_STATUS_DMA_MODE
} spi_master_status_t;

typedef enum {
    SPI_MASTER_TX = 0,
    SPI_MASTER_RX = 1
} spi_master_direction_t;

typedef enum {
    SPI_MASTER_MODE_FIFO = 0,
    SPI_MASTER_MODE_DMA  = 1
} spi_master_mode_t;

#define SPI_BUSY 1
#define SPI_IDLE 0
#define SPI_CHECK_AND_SET_BUSY(spi_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(g_spi_master_status[spi_port] == SPI_BUSY){ \
        busy_status = HAL_SPI_MASTER_STATUS_ERROR_BUSY; \
    } else { \
        g_spi_master_status[spi_port] = SPI_BUSY;  \
        busy_status = HAL_SPI_MASTER_STATUS_OK; \
    } \
       restore_interrupt_mask(saved_mask); \
}while(0)

#define SPI_SET_IDLE(spi_port)   \
do{  \
       g_spi_master_status[spi_port] = SPI_IDLE;  \
}while(0)

#define HAL_SPI_MASTER_MAX 2

/* clock frequency related macro */
#define  SPI_MASTER_INPUT_CLOCK_FREQUENCY 200000000

/* packet length and packet count macro */
#define  SPI_MASTER_MAX_SIZE_FOR_NON_PAUSE      0x1000000
#define  SPI_MASTER_MAX_PACKET_LENGTH           0x10000
#define  SPI_MASTER_MAX_PACKET_LENGTH_MASK      0xFFFF
#define  SPI_MASTER_MAX_PACKET_COUNT            0x100
#define  SPI_MASTER_MAX_PACKET_COUNT_MASK       0xFF

/* function prototype */
void spi_master_isr_handler(hal_spi_master_port_t master_port, hal_spi_master_callback_t user_callback, void *user_data);
void spi_master_init(hal_spi_master_port_t master_port, const hal_spi_master_config_t *spi_config);
#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
void spi_master_set_advanced_config(hal_spi_master_port_t master_port, const hal_spi_master_advanced_config_t *advanced_config);
#endif /* #ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG */
uint32_t spi_master_get_status(hal_spi_master_port_t master_port);
void spi_master_set_rwaddr(hal_spi_master_port_t master_port, spi_master_direction_t type, const uint8_t *addr);

hal_spi_master_status_t spi_master_push_data(hal_spi_master_port_t master_port, const uint8_t *data, uint32_t size, uint32_t total_size);
hal_spi_master_status_t spi_master_pop_data(hal_spi_master_port_t master_port, uint8_t *buffer, uint32_t size);
void spi_master_set_interrupt(hal_spi_master_port_t master_port, bool status);
void spi_master_clear_fifo(hal_spi_master_port_t master_port);
void spi_master_set_mode(hal_spi_master_port_t master_port, spi_master_direction_t type, spi_master_mode_t mode);
void spi_master_start_transfer_fifo(hal_spi_master_port_t master_port, bool is_write);
void spi_master_start_transfer_dma(hal_spi_master_port_t master_port, bool is_continue, bool is_write);
void spi_master_start_transfer_dma_blocking(hal_spi_master_port_t master_port, bool is_write);
hal_spi_master_status_t spi_master_analyse_transfer_size(hal_spi_master_port_t master_port, uint32_t size);
#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
void spi_master_set_chip_select_timing(hal_spi_master_port_t master_port, hal_spi_master_chip_select_timing_t chip_select_timing);
#endif /* #ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING */
#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
void spi_master_set_deassert(hal_spi_master_port_t master_port, hal_spi_master_deassert_t deassert);
#endif /* #ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG */
#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_master_backup_register_callback(void *data);
void spi_master_restore_register_callback(void *data);
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */
#ifdef HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE
void spi_master_set_type(hal_spi_master_port_t master_port, hal_spi_master_mode_t mode);
void spi_master_set_command_bytes(hal_spi_master_port_t master_port, uint8_t command_bytes);
void spi_master_set_dummy_bits(hal_spi_master_port_t master_port, uint8_t dummy_bits);
#endif /* #ifdef HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE */
void spi_master_reset_default_value(hal_spi_master_port_t master_port);
#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_master_backup_register_callback(void *data);
void spi_master_restore_register_callback(void *data);
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */

#endif /* #ifdef HAL_SPI_MASTER_MODULE_ENABLED */

#endif /* #ifndef __HAL_SPI_MASTER_INTERNAL_H__ */
