/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/**
 * @file mt_i2c.h
 * This i2c header file is used for i2c driver. It declares\n
 * the external functions which will be used in LK.
 */

/**
 * @defgroup IP_group_i2c I2C
 *
 *   @{
 *       @defgroup IP_group_i2c_external EXTERNAL
 *         The external API document for I2C. \n
 *
 *         @{
 *            @defgroup IP_group_i2c_external_function 1.function
 *              External function in i2c driver.
 *            @defgroup IP_group_i2c_external_struct 2.structure
 *              none.
 *            @defgroup IP_group_i2c_external_typedef 3.typedef
 *              none.
 *            @defgroup IP_group_i2c_external_enum 4.enumeration
 *              none.
 *            @defgroup IP_group_i2c_external_def 5.define
 *              none.
 *         @}
 *
 *       @defgroup IP_group_i2c_internal INTERNAL
 *         The internal API document for I2C. \n
 *
 *         @{
 *            @defgroup IP_group_i2c_internal_function 1.function
 *              none.
 *            @defgroup IP_group_i2c_internal_struct 2.structure
 *              Internal structure in i2c driver.
 *            @defgroup IP_group_i2c_internal_typedef 3.typedef
 *              none.
 *            @defgroup IP_group_i2c_internal_enum 4.enumeration
 *              Internal enumeration in i2c driver.
 *            @defgroup IP_group_i2c_internal_def 5.define
 *              Internal define in i2c driver.
 *         @}
 *   @}
 */

#ifndef __MTK_I2C_H__
#define __MTK_I2C_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "hal_i2c_master.h"
#include "hal_clock.h"
#include "hal_gpio.h"
#include "hal_nvic_internal.h"
#include "driver_api.h"
//#define CONFIG_MTK_FPGA

/** @ingroup IP_group_i2c_internal_def
 * @{
 */
#define MTK_I2C_SOURCE_CLK 26000
#define MTK_I2C_CLK_DIV 1
#define I2C_DEFAULT_CLK_DIV 2
#define I2C_CLK_DIV_100K 10
#define I2C_TIMING_100K 0x126
#define I2C_CLK_DIV_400K I2C_DEFAULT_CLK_DIV
#define I2C_TIMING_400K 0x637
#define I2C_CLK_DIV_1000K I2C_DEFAULT_CLK_DIV
#define I2C_TIMING_1000K 0x407
#define I2C_CLOCK_CG      (0x30020290)

#define I2CTAG "[I2C-LK] "
#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(x, y) (((x) + ((y) - 1)) / (y))
#endif /* #ifndef DIV_ROUND_UP */

#define I2CBIT(nr) (1UL << (nr))
#define I2CBITMASK(a, b) (I2CBIT(a+1) - I2CBIT(b))
#define I2C_POLL_VALUE 0xfffff


#define I2C_CONTROL_RS          I2CBIT(1)
#define I2C_CONTROL_DMA_EN      I2CBIT(2)
#define I2C_CONTROL_CLK_EXT_EN      I2CBIT(3)
#define I2C_CONTROL_DIR_CHANGE      I2CBIT(4)
#define I2C_CONTROL_ACKERR_DET_EN   I2CBIT(5)
#define I2C_CONTROL_TRANSFER_LEN_CHANGE I2CBIT(6)
#define I2C_CONTROL_AYNCS_MODE      I2CBIT(9)

#define I2C_RS_TRANSFER         I2CBIT(4)
#define I2C_ARB_LOSE            I2CBIT(3)
#define I2C_ACKERR              I2CBIT(1)
#define I2C_TRANSAC_COMP        I2CBIT(0)
#define I2C_TRANSAC_START       I2CBIT(0)
#define I2C_RS_MUL_CNFG         I2CBIT(15)
#define I2C_RS_MUL_TRIG         I2CBIT(14)
#define I2C_SOFT_RST            0x0001
#define I2C_FIFO_ADDR_CLR       0x0001

#define I2C_IO_CONFIG_OPEN_DRAIN    0x0003
#define I2C_IO_CONFIG_PUSH_PULL     0x0000
#define I2C_CONTROL_DEFAULT     0x0d00
#define I2C_DELAY_LEN           0x0002
#define I2C_ST_START_CON        0x7001
#define I2C_FS_START_CON        0x3001
#define I2C_DCM_OPEN            0x0003
#define I2C_WRRD_TRANAC_VALUE       0x0002
#define I2C_M_RD            0x0001

#define I2C_DMA_CON_TX          0x0000
#define I2C_DMA_CON_RX          0x0001
#define I2C_DMA_START_EN        0x0001
#define I2C_DMA_INT_FLAG_NONE       0x0000
#define I2C_DMA_CLR_FLAG        0x0000
#define I2C_DMA_HARD_RST        0x0002

#define I2C_FIFO_SIZE           16
#define I2C_DEFAULT_SPEED       100
#define MAX_FS_MODE_SPEED       400
#define MAX_FS_PLUS_SPEED       1000
#define MAX_SAMPLE_CNT_DIV      8
#define MAX_STEP_CNT_DIV        64
#define MAX_HS_STEP_CNT_DIV     8
#define I2C_TIME_DEFAULT_VALUE      0x0003

#define I2C_FIFO_FORCE          I2CBIT(0)
#define I2C_DCM_ENABLE          I2CBIT(1)
#define I2C_CONTI_TRANS         I2CBIT(2)
#define I2C_EXTEN_SET           I2CBIT(3)
#define I2C_ACTIME_SET          I2CBIT(4)
#define I2C_MULTI_TRANS         I2CBIT(5)
#define I2C_MULTI_STOP          I2CBIT(6)
#define I2C_CLOCK_STRETCH       I2CBIT(7)

#define I2C0_SCL_GPIO   HAL_GPIO_10
#define I2C0_SDA_GPIO   HAL_GPIO_11
#define I2C0_GPIO_PIMMUX     6
#define I2C1_SCL_GPIO   HAL_GPIO_20
#define I2C1_SDA_GPIO   HAL_GPIO_19
#define I2C1_GPIO_PIMMUX     3
#define I2C_PORT_NUM     2

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C controller register offset.
 */
enum I2C_REGS_OFFSET {
    OFFSET_DATA_PORT = 0x0,
    OFFSET_SLAVE_ADDR = 0x04,
    OFFSET_INTR_MASK = 0x08,
    OFFSET_INTR_STAT = 0x0c,
    OFFSET_CONTROL = 0x10,
    OFFSET_TRANSFER_LEN = 0x14,
    OFFSET_TRANSAC_LEN = 0x18,
    OFFSET_DELAY_LEN = 0x1c,
    OFFSET_TIMING = 0x20,
    OFFSET_START = 0x24,
    OFFSET_EXT_CONF = 0x28,
    OFFSET_FIFO_STAT1 = 0x2c,
    OFFSET_FIFO_STAT = 0x30,
    OFFSET_FIFO_THRESH = 0x34,
    OFFSET_FIFO_ADDR_CLR = 0x38,
    OFFSET_IO_CONFIG = 0x40,
    OFFSET_MULTI_MASTER = 0x44,
    OFFSET_HS = 0x48,
    OFFSET_SOFTRESET = 0x50,
    OFFSET_DCM_EN = 0x54,
    OFFSET_DEBUGSTAT = 0x64,
    OFFSET_DEBUGCTRL = 0x68,
    OFFSET_TRANSFER_LEN_AUX = 0x6c,
    OFFSET_CLOCK_DIV = 0x70,
    OFFSET_SCL_HL_RATIO = 0x74,
    OFFSET_HS_SCL_HL_RATIO = 0x78,
    OFFSET_SCL_MIS_COMP_POINT = 0x7C,
    OFFSET_STA_STOP_AC_TIME = 0x80,
    OFFSET_HS_STA_STOP_AC_TIME = 0x84,
    OFFSET_SDA_TIME = 0x88,
    OFFSET_FIFO_PAUSE = 0x8C,
};

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C transfer operation mode.
 */
enum mtk_trans_op {
    I2C_MASTER_WR = 1,
    I2C_MASTER_RD,
    I2C_MASTER_WRRD,
};

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C GDMA register offset.
 */
enum DMA_REGS_OFFSET {
    OFFSET_DMA_INT_FLAG       = 0x0,
    OFFSET_DMA_INT_EN         = 0x04,
    OFFSET_DMA_EN             = 0x08,
    OFFSET_DMA_RST            = 0x0C,
    OFFSET_DMA_CON            = 0x18,
    OFFSET_DMA_TX_MEM_ADDR    = 0x1C,
    OFFSET_DMA_RX_MEM_ADDR    = 0x20,
    OFFSET_DMA_TX_LEN         = 0x24,
    OFFSET_DMA_RX_LEN         = 0x28,
};

/** @brief This enum define the I2C transaction speed.  */
typedef enum {
    I2C_FREQUENCY_50K  = 50,          /**<  Transmit data with 50kbps. */
    I2C_FREQUENCY_100K = 100,          /**<  Transmit data with 100kbps. */
    I2C_FREQUENCY_200K = 200,          /**<  Transmit data with 200kbps. */
    I2C_FREQUENCY_300K = 300,          /**<  Transmit data with 300kbps. */
    I2C_FREQUENCY_400K = 400,          /**<  Transmit data with 400kbps. */
    I2C_FREQUENCY_1M   = 1000,         /**<  Transmit data with 1mbps. */
    I2C_FREQUENCY_MAX                  /**<  Max transaction speed, don't use this value. */
} i2c_frequency_t;

typedef enum {
    I2C_TRANSACTION_PUSH_PULL_SEND_POLLING = 0,
    I2C_TRANSACTION_PUSH_PULL_SEND_DMA     = 1,
    I2C_TRANSACTION_PUSH_PULL_RECEIVE_POLLING = 2,
    I2C_TRANSACTION_PUSH_PULL_RECEIVE_DMA = 3,
    I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_POLLING = 4,
    I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_DMA = 5,

    I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING = 6,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA     = 7,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING = 8,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA = 9,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING = 10,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA = 11,
} i2c_transaction_type_t;

/** @ingroup IP_group_i2c_internal_struct
 * @brief Struct keeping i2c message data.
 */
struct i2c_msg {
    /** slave address */
    unsigned short addr;
    /** i2c transfer operation mode */
    unsigned short flags;
    /** msg length */
    unsigned short len;
    /** pointer to msg data */
    unsigned char *buf;
};

/** @ingroup IP_group_i2c_internal_struct
 * @brief Struct keeping i2c driver data.
 */
struct mtk_i2c {
    /** dma mode flag */
    bool dma_en;
    /** polling mode flag */
    bool poll_en;
    /** IO config push-pull mode */
    bool pushpull;
    /** multi-transfer repeated start enable */
    bool auto_restart;
    /** bus busy flag */
    bool bus_busy;
    volatile bool msg_complete;
    /** slave device 7bits address */
    unsigned char addr;
    /** i2c bus number */
    unsigned char id;
    unsigned short irqnr;
    /** i2c interrupt status */
    volatile unsigned short irq_stat;
    /** clock_div register value */
    unsigned short clock_div_reg;
    /** timing register value */
    unsigned short timing_reg;
    unsigned short high_speed_reg;
    unsigned short con_num;
    unsigned short delay_len;
    unsigned short ext_time;
    unsigned short scl_ratio;
    unsigned short hs_scl_ratio;
    unsigned short scl_mis_comp;
    unsigned short sta_stop_time;
    unsigned short hs_sta_stop_time;
    unsigned short sda_time;
    /** i2c base address */
    unsigned int base;
    /** i2c dma base address */
    unsigned int dmabase;
    /** source clock KHz */
    unsigned int clk;
    /** source clock divide */
    unsigned int clk_src_div;
    /** i2c transfer speed */
    unsigned int speed;
    /* Malloc no-cache write buffer */
    unsigned char *write_buf;
    /* Malloc no-cache read buffer */
    unsigned char *read_buf;
    /* Restore user read buffer pointer */
    unsigned char *tmp_buf;
    /* DMA mode read size */
    unsigned int tmp_len;
    /* i2c transfer operation mode */
    enum mtk_trans_op op;
    /* i2c transfer complete semaphore pointer */
    QueueHandle_t semaph;
    /** User_data is a defined parameter provided
    * by #hal_i2c_master_register_callback().
    */
    void *user_data;
    /* This function is used to register user's DMA done callback */
    hal_i2c_callback_t i2c_callback;
};

int mtk_i2c_transfer(struct mtk_i2c *i2c, struct i2c_msg *msgs, int num);
int i2c_init(struct mtk_i2c *i2c);
int i2c_deinit(struct mtk_i2c *i2c);
void mtk_irq_handler_register(struct mtk_i2c *i2c, hal_i2c_callback_t i2c_callback, void *user_data);
int mtk_i2c_set_speed(struct mtk_i2c *i2c);
int mtk_i2c_soft_reset(unsigned char id);
char mtk_i2c_get_running_status(unsigned char id);
unsigned short mtk_i2c_get_rx_bytes(unsigned char id);
unsigned short mtk_i2c_get_tx_bytes(unsigned char id);
#endif /* #ifndef __MTK_I2C_H__ */
