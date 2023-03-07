/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __BTIF_DMA_H_
#define __BTIF_DMA_H_

#include "btif_main.h"

/*Register Address Mapping*/
#define DMA_INT_FLAG_OFFSET 0x00
#define DMA_INT_EN_OFFSET   0x04
#define DMA_EN_OFFSET       0x08
#define DMA_RST_OFFSET      0x0C
#define DMA_STOP_OFFSET     0x10
#define DMA_FLUSH_OFFSET    0x14

#define DMA_BASE_OFFSET 0x1C
#define DMA_LEN_OFFSET  0x24

#define DMA_THRE_OFFSET      0x28
#define DMA_WPT_OFFSET       0x2C
#define DMA_RPT_OFFSET       0x30
#define DMA_VALID_OFFSET     0x3C
#define DMA_LEFT_OFFSET      0x40
#define DMA_VFF_BIT29_OFFSET 0x01

/* BTIF Tx Virtual FIFO Interrupt Flag Register */
#define TX_DMA_INT_FLAG(base)       ((unsigned int)((base) + 0x0))
/* BTIF Tx Virtual FIFO Interrupt Enable Register */
#define TX_DMA_INT_EN(base)         ((unsigned int)((base) + 0x4))
/* BTIF Tx Virtual FIFO Enable Register */
#define TX_DMA_EN(base)             ((unsigned int)((base) + 0x08))
/* BTIF Tx Virtual FIFO  Reset Register */
#define TX_DMA_RST(base)            ((unsigned int)((base) + 0x0C))
/* BTIF Tx Virtual FIFO STOP  Register */
#define TX_DMA_STOP(base)           ((unsigned int)((base) + 0x10))
/* BTIF Tx Virtual FIFO Flush Register */
#define TX_DMA_FLUSH(base)          ((unsigned int)((base) + 0x14))
/* BTIF Tx Virtual FIFO Base Address Register */
#define TX_DMA_VFF_ADDR(base)       ((unsigned int)((base) + 0x1C))
/* BTIF Tx Virtual FIFO Length Register */
#define TX_DMA_VFF_LEN(base)        ((unsigned int)((base) + 0x24))
/* BTIF Tx Virtual FIFO Threshold Register */
#define TX_DMA_VFF_THRE(base)       ((unsigned int)((base) + 0x28))
/* BTIF Tx Virtual FIFO Write Pointer Register */
#define TX_DMA_VFF_WPT(base)        ((unsigned int)((base) + 0x2C))
/* BTIF Tx Virtual FIFO Read Pointer  Register */
#define TX_DMA_VFF_RPT(base)        ((unsigned int)((base) + 0x30))
/* BTIF Tx Virtual FIFO Internal Tx Write Buffer Size Register */
#define TX_DMA_W_INT_BUF_SIZE(base) ((unsigned int)((base) + 0x34))
/* BTIF Tx Virtual FIFO Internal Tx Buffer Size Register */
#define TX_DMA_INT_BUF_SIZE(base)   ((unsigned int)((base) + 0x38))
/* BTIF Tx Virtual FIFO Valid Size Register */
#define TX_DMA_VFF_VALID_SIZE(base) ((unsigned int)((base) + 0x3C))
/* BTIF Tx Virtual FIFO Left Size Register */
#define TX_DMA_VFF_LEFT_SIZE(base)  ((unsigned int)((base) + 0x40))
/* BTIF Tx Virtual FIFO Debug Status Register */
#define TX_DMA_DEBUG_STATUS(base)   ((unsigned int)((base) + 0x50))
/* BTIF Tx Virtual FIFO Base High Address Register */
#define TX_DMA_VFF_ADDR_H(base)     ((unsigned int)((base) + 0x54))
#define TX_DMA_VFF_WPT_VALID(base)  ((unsigned int)((base) + 0x58))
#define TX_DMA_VFF_WPT_VALID2(base) ((unsigned int)((base) + 0x5C))
#define TX_DMA_FLUSH_ACT(base)      ((unsigned int)((base) + 0x60))
#define TX_DMA_HW_FLUSH(base)       ((unsigned int)((base) + 0x64))
#define TX_DMA_VFF_WPT_REAL(base)   ((unsigned int)((base) + 0x68))
#define TX_DMA_SEC_EN(base)         ((unsigned int)((base) + 0xB20))

/* Rx Register Address Mapping*/
#define RX_DMA_INT_FLAG(base)   ((unsigned int)((base) + 0x0))
/* BTIF Rx Virtual FIFO Interrupt Flag Register */
#define RX_DMA_INT_EN(base)     ((unsigned int)((base) + 0x4))
/* BTIF Rx Virtual FIFO Interrupt Enable Register */
#define RX_DMA_EN(base)         ((unsigned int)((base) + 0x08))
/* BTIF Rx Virtual FIFO Enable Register */
#define RX_DMA_RST(base)        ((unsigned int)((base) + 0x0C))
/* BTIF Rx Virtual FIFO Reset Register */
#define RX_DMA_STOP(base)       ((unsigned int)((base) + 0x10))
/* BTIF Rx Virtual FIFO Stop Register */
#define RX_DMA_FLUSH(base)      ((unsigned int)((base) + 0x14))
/* BTIF Rx Virtual FIFO Flush Register */
#define RX_DMA_VFF_ADDR(base)   ((unsigned int)((base) + 0x1C))
/* BTIF Rx Virtual FIFO Base Address Register */
#define RX_DMA_VFF_LEN(base)    ((unsigned int)((base) + 0x24))
/* BTIF Rx Virtual FIFO Length Register */
#define RX_DMA_VFF_THRE(base)   ((unsigned int)((base) + 0x28))
/* BTIF Rx Virtual FIFO Threshold Register */
#define RX_DMA_VFF_WPT(base)    ((unsigned int)((base) + 0x2C))
/* BTIF Rx Virtual FIFO Write Pointer Register */
#define RX_DMA_VFF_RPT(base)    ((unsigned int)((base) + 0x30))
/* BTIF Rx Virtual FIFO Read Pointer Register */
#define RX_DMA_FLOW_CTRL_THRE(base) ((unsigned int)((base) + 0x34))
/* BTIF Rx Virtual FIFO Flow Control  Register */
#define RX_DMA_INT_BUF_SIZE(base)   ((unsigned int)((base) + 0x38))
/* BTIF Rx Virtual FIFO Internal Buffer Register */
#define RX_DMA_VFF_VALID_SIZE(base) ((unsigned int)((base) + 0x3C))
/* BTIF Rx Virtual FIFO Valid Size Register */
#define RX_DMA_VFF_LEFT_SIZE(base)  ((unsigned int)((base) + 0x40))
/* BTIF Rx Virtual FIFO Left Size Register */
#define RX_DMA_DEBUG_STATUS(base)   ((unsigned int)((base) + 0x50))
/* BTIF Rx Virtual FIFO Debug Status Register */
#define RX_DMA_VFF_ADDR_H(base)     ((unsigned int)((base) + 0x54))
/* BTIF Rx Virtual FIFO Base High Address Register */
#define RX_DMA_SEC_EN(base)         ((unsigned int)((base) + 0xAA4))

#define DMA_EN_BIT    (0x1)
#define DMA_STOP_BIT  (0x1)
#define DMA_RST_BIT   (0x1)
#define DMA_FLUSH_BIT (0x1)

#define DMA_WARM_RST (0x1 << 0)
#define DMA_HARD_RST (0x1 << 1)

#define DMA_WPT_MASK (0x0000FFFF)
#define DMA_WPT_WRAP (0x00010000)

#define DMA_RPT_MASK (0x0000FFFF)
#define DMA_RPT_WRAP (0x00010000)

/*APDMA BTIF Tx Reg Ctrl Bit*/
#define TX_DMA_INT_FLAG_MASK (0x1)

#define TX_DMA_INTEN_BIT (0x1)

#define TX_DMA_ADDR_MASK (0xFFFFFFF8)
#define TX_DMA_LEN_MASK  (0x0000FFF8)

#define TX_DMA_THRE_MASK (0x0000FFFF)

#define TX_DMA_W_INT_BUF_MASK (0x000000FF)

#define TX_DMA_VFF_VALID_MASK (0x0000FFFF)
#define TX_DMA_VFF_LEFT_MASK  (0x0000FFFF)

/*APDMA BTIF Rx Reg Ctrl Bit*/
#define RX_DMA_INT_THRE (0x1 << 0)
#define RX_DMA_INT_DONE (0x1 << 1)

#define RX_DMA_INT_THRE_EN (0x1 << 0)
#define RX_DMA_INT_DONE_EN (0x1 << 1)

#define RX_DMA_ADDR_MASK (0xFFFFFFF8)
#define RX_DMA_LEN_MASK  (0x0000FFF8)

#define RX_DMA_THRE_MASK (0x0000FFFF)

#define RX_DMA_FLOW_CTRL_THRE_MASK (0x000000FF)

#define RX_DMA_INT_BUF_SIZE_MASK   (0x0000001F)

#define RX_DMA_VFF_VALID_MASK (0x0000001F)

#define RX_DMA_VFF_LEFT_MASK  (0x0000FFFF)

/*Tx vFIFO Len must be 8 Byte allignment */
#define BTIF_DMA_TX_VFF_SIZE (1024 * 8)
/*Rx vFIFO Len must be 8 Byte allignment */
#define BTIF_DMA_RX_VFF_SIZE (1024 * 8)

#define BTIF_DMA_TX_THRE(n) (n - 7) /* Tx Trigger Level */
#define BTIF_DMA_RX_THRE(n) (1)     /* Rx Trigger Level */

#define BTIF_STOP_DMA_TIME_MS 10 /* 10ms */

enum btif_dma_dir {
	DMA_DIR_TX = 0,
	DMA_DIR_RX = DMA_DIR_TX + 1,
	DAM_DIR_BOTH,
};

enum btif_dma_ctrl {
	DMA_CTRL_DISABLE = 0,
	DMA_CTRL_ENABLE = DMA_CTRL_DISABLE + 1,
	DMA_CTRL_BOTH,
};

typedef int (*dma_rx_data_cb)(unsigned char *p_buf, unsigned int buf_len);

struct btif_dma_t {
	unsigned char dir;
	unsigned char enabled;
	struct mtk_btif *p_btif;
	unsigned char *p_base;
	unsigned char irq_id;

	unsigned char *p_vfifo;
	unsigned int vfifo_size;
	unsigned int thre;
	unsigned int wpt;
	unsigned int last_wpt_wrap;
	unsigned int rpt;
	unsigned int last_rpt_wrap;
	unsigned char *p_cur;

	xSemaphoreHandle dma_tx_semaphore;

	dma_rx_data_cb rx_cb;
	unsigned int cb_enabled;
};

int btif_dma_hw_init(struct btif_dma_t *p_dma_info);
void btif_dma_rx_cb_reg(struct btif_dma_t *p_dma_info, dma_rx_data_cb rx_cb);
void btif_dma_clk_ctrl(unsigned char en);
struct btif_dma_t *btif_dma_info_get(unsigned char dir);
int btif_dma_tx_semaphore_init(struct btif_dma_t *p_dma_info);
void btif_dma_tx_semaphore_deinit(struct btif_dma_t *p_dma_info);
int btif_dma_vfifo_init(struct btif_dma_t *p_dma_info);
void btif_dma_vfifo_deinit(struct btif_dma_t *p_dma_info);
int btif_dma_tx_setup(struct mtk_btif *p_btif);
int btif_dma_rx_setup(struct mtk_btif *p_btif);
void btif_dma_free(struct btif_dma_t *p_dma_info);
void btif_dma_ctrl_enable(struct btif_dma_t *p_dma_info);
void btif_dma_ctrl_disable(struct btif_dma_t *p_dma_info);
int btif_dma_tx_write(struct btif_dma_t *p_dma_info,
					  const unsigned char *p_buf, unsigned int buf_len);
void btif_dma_dump_reg(struct btif_dma_t *p_dma_info);
bool btif_dma_check_RX_empty(void);
enum own_type_place_t btif_dma_state_get(void);
int btif_dma_tx_ctrl(struct btif_dma_t *p_dma_info, enum btif_dma_ctrl ctrl_id);
int btif_dma_rx_ctrl(struct btif_dma_t *p_dma_info, enum btif_dma_ctrl ctrl_id);
int btif_dma_ctrl(struct btif_dma_t *p_dma_info, enum btif_dma_ctrl ctrl_id);
void btif_dma_rx_irq_handler(int irq, void *dev);
void btif_dma_tx_irq_handler(int irq, void *dev);

#endif /*__BTIF_DMA_H_*/
