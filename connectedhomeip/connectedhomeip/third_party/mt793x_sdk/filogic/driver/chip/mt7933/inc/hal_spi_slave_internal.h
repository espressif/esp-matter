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

#ifndef __HAL_SPI_SLAVE_INTERNAL_H__
#define __HAL_SPI_SLAVE_INTERNAL_H__

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

/*****************reg define*******************/
#define SPIS_IRQ_EN_REG             0x0
#define DATA_DONE_EN                BIT(2)
#define RX_FIFO_FULL_EN             BIT(3)
#define RX_FIFO_EMPTY_EN            BIT(4)
#define TX_FIFO_FULL_EN             BIT(5)
#define TX_FIFO_EMPTY_EN            BIT(6)
#define DMA_DONE_EN                 BIT(7)

#define SPIS_IRQ_CLR_REG            0x4
#define DATA_DONE_CLR               BIT(2)
#define RX_FIFO_FULL_CLR            BIT(3)
#define RX_FIFO_EMPTY_CLR           BIT(4)
#define TX_FIFO_FULL_CLR            BIT(5)
#define TX_FIFO_EMPTY_CLR           BIT(6)
#define DMA_DONE_CLR                BIT(7)

#define SPIS_IRQ_ST_REG             0x8
#define DATA_DONE_ST                BIT(2)
#define RX_FIFO_FULL_ST             BIT(3)
#define RX_FIFO_EMPTY_ST            BIT(4)
#define TX_FIFO_FULL_ST             BIT(5)
#define TX_FIFO_EMPTY_ST            BIT(6)
#define DMA_DONE_ST                 BIT(7)

#define SPIS_IRQ_MASK_REG           0xc
#define DATA_DONE_MASK              BIT(2)
#define RX_FIFO_FULL_MASK           BIT(3)
#define RX_FIFO_EMPTY_MASK          BIT(4)
#define TX_FIFO_FULL_MASK           BIT(5)
#define TX_FIFO_EMPTY_MASK          BIT(6)
#define DMA_DONE_MASK               BIT(7)

#define SPIS_CFG_REG                0x10
#define SPIS_RX_EN                  BIT(0)
#define SPIS_TX_EN                  BIT(1)
#define SPIS_CPOL                   BIT(2)
#define SPIS_CPHA                   BIT(3)
#define SPIS_RXMSBF                 BIT(4)
#define SPIS_TXMSBF                 BIT(5)
#define SPIS_RX_ENDIAN              BIT(6)
#define SPIS_TX_ENDIAN              BIT(7)

#define SPIS_RX_DATA_REG            0x14
#define SPIS_TX_DATA_REG            0x18
#define SPIS_RX_DST_REG             0x1c
#define SPIS_TX_SRC_REG             0x20

#define SPIS_FIFO_ST_REG            0x28
#define TX_FIFO_EMPTY               BIT(7)
#define TX_FIFO_FULL                BIT(8)
#define RX_FIFO_EMPTY               BIT(23)
#define RX_FIFO_FULL                BIT(24)

#define SPIS_DMA_CFG_REG            0x30
#define TX_DMA_LEN_MASK             0xFFFFF
#define RX_DMA_EN                   BIT(29)
#define TX_DMA_EN                   BIT(30)
#define TX_DMA_TRIG_EN              BIT(31)

#define SPIS_FIFO_THR_REG           0x34
#define SPIS_DEBUG_ST_REG           0x38

#define SPIS_BYTE_CNT_REG           0x3c
#define SPIS_BYTE_CNT_CLR           BIT(31)

#define SPIS_SOFT_RST_REG           0x40
#define SPIS_SOFT_RST1              BIT(0)
#define SPIS_DMA_ADDR_LOAD          BIT(1)

#define SPIS_TX_FIFO_DEFAULT_REG    0x44
/********************************************/
#define SPI_SLAVE_BUSY 0
#define SPI_SLAVE_IDLE 1
#define SPI_SLAVE_CHECK_AND_SET_BUSY(spi_port, busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(g_spi_slave_status[spi_port] == SPI_SLAVE_BUSY){ \
        busy_status = HAL_SPI_SLAVE_STATUS_ERROR_BUSY; \
    } else { \
        g_spi_slave_status[spi_port] = SPI_SLAVE_BUSY;  \
        busy_status = HAL_SPI_SLAVE_STATUS_OK; \
    } \
       restore_interrupt_mask(saved_mask); \
}while(0)

#define SPI_SLAVE_SET_IDLE(spi_port)   \
do{  \
       g_spi_slave_status[spi_port] = SPI_SLAVE_IDLE;  \
}while(0)

/***************Configurations******************/
#define HAL_SPI_SLAVE_MAX 1
#define MTK_SPIS_FIFO_DEPTH 128

struct spis_transfer {
    const uint8_t   *tx_buf;
    uint8_t *rx_buf;
    uint32_t    len;
    uint32_t timeout;
    uint32_t    tx_dma;
    uint32_t    rx_dma;
};

/********************************************/
void spi_slave_lisr(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data);
hal_spi_slave_status_t spi_slave_init(hal_spi_slave_port_t spi_port, const hal_spi_slave_config_t *spi_config);
hal_spi_slave_status_t spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size);
hal_spi_slave_status_t spi_slave_send_and_receive(hal_spi_slave_port_t spi_port, const uint8_t *tx_buf, uint8_t *rx_buf, uint32_t size);
#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_slave_backup_register_callback(void *data);
void spi_slave_restore_register_callback(void *data);
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */
#endif /* #ifdef HAL_SPI_SLAVE_MODULE_ENABLED */

#endif /* #ifndef __HAL_SPI_SLAVE_INTERNAL_H__ */

