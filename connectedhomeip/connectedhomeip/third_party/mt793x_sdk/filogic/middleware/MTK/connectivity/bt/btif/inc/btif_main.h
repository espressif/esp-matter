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

#ifndef __BTIF_MAIN_H_
#define __BTIF_MAIN_H_

#include "FreeRTOS.h"
//#include "btif_dma.h"
#include "btif_util.h"
#include "btif_platform.h"
#include "event_groups.h"

/*
 * This macro is used to control main task timeout setting
 */
//#define MTK_BT_DRV_MAIN_TASK_TIMEOUT_SET

#ifndef TRUE
#define TRUE  (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

/* This macro is used for trace the sequence of main task event handler */
//#define BTIF_MAIN_TASK_TRACE

/* This macro is used for trace the usage of TX/RX buffer */
#define BUFFER_DEBUG_TRACE

#define BTIF_PORT_NR 1

/*
 * If change BTIF_RX_BUFFER_SIZE, please also change BTIF_RX_BUF_HIGH_LEVEL_SIZE
 * EX:BTIF_RX_BUFFER_SIZE = 32K, BTIF_RX_BUF_HIGH_LEVEL_SIZE can set minus 4K
 *    BTIF_RX_BUFFER_SIZE = 8K, BTIF_RX_BUF_HIGH_LEVEL_SIZE can set minus 1K
 */
#define BTIF_RX_BUFFER_SIZE (1024 * 32)
#define BTIF_RX_BUF_HIGH_LEVEL_SIZE (BTIF_RX_BUFFER_SIZE - 1024 * 4)
//#define BTIF_TX_BUFFER_FIFO_SIZE (1024 * 4)

// set smaller buffer for mt7933 env temporary  TBD
//#define BTIF_RX_BUFFER_SIZE (1024 * 4)
//#define BTIF_TX_BUFFER_FIFO_SIZE (1024 * 4)

//#define BTIF_IRQ_ID (56+32)
//#define BTIF_BASE            0x1100c000

/* RX Buffer Register: read only */
#define BTIF_RBR(base)       ((unsigned int)((base) + 0x0))
/* Rx Holding Register: write only */
#define BTIF_THR(base)       ((unsigned int)((base) + 0x0))
/* Interrupt Enable Register: read/write */
#define BTIF_IER(base)       ((unsigned int)((base) + 0x4))
/* Interrupt Identification Register: read only */
#define BTIF_IIR(base)       ((unsigned int)((base) + 0x8))
/* FIFO Control Register: write only */
#define BTIF_FIFOCTRL(base)  ((unsigned int)((base) + 0x8))
/* FAKE LCR Register: read/write */
#define BTIF_FAKELCR(base)   ((unsigned int)((base) + 0xC))
/* Line Status Register: read only */
#define BTIF_LSR(base)       ((unsigned int)((base) + 0x14))
/* Sleep Enable Register: read/write */
#define BTIF_SLEEP_EN(base)  ((unsigned int)((base) + 0x48))
/* DMA Enable Register: read/write */
#define BTIF_DMA_EN(base)    ((unsigned int)((base) + 0x4C))
/* Rx Timeout Count Register: read/write */
#define BTIF_RTOCNT(base)    ((unsigned int)((base) + 0x54))
/* Tx/Rx Trigger Level Control Register: read/write */
#define BTIF_TRI_LVL(base)   ((unsigned int)((base) + 0x60))
/* BTIF module wakeup Register: write only */
#define BTIF_WAK(base)       ((unsigned int)((base) + 0x64))
/* BTIF ASYNC Wait Time Control Register: read/write */
#define BTIF_WAT_TIME(base)  ((unsigned int)((base) + 0x68))
#define BTIF_HANDSHAKE(base) ((unsigned int)((base) + 0x6C))
#define BTIF_DBG_TX_FIFO_0(base) ((unsigned int)((base) + 0x70))
#define BTIF_DBG_TX_FIFO_1(base) ((unsigned int)((base) + 0x74))
#define BTIF_DBG_TX_FIFO_2(base) ((unsigned int)((base) + 0x78))
#define BTIF_DBG_TX_FIFO_3(base) ((unsigned int)((base) + 0x7C))
#define BTIF_DBG_TX_FIFO_4(base) ((unsigned int)((base) + 0x80))
#define BTIF_DBG_TX_FIFO_5(base) ((unsigned int)((base) + 0x84))
#define BTIF_DBG_TX_FIFO_6(base) ((unsigned int)((base) + 0x88))
#define BTIF_DBG_TX_FIFO_7(base) ((unsigned int)((base) + 0x8C))
#define BTIF_DBG_TX_FIFO_8(base) ((unsigned int)((base) + 0x90))
#define BTIF_DBG_TX_FIFO_9(base) ((unsigned int)((base) + 0x94))
#define BTIF_DBG_TX_FIFO_a(base) ((unsigned int)((base) + 0x98))
#define BTIF_DBG_TX_FIFO_b(base) ((unsigned int)((base) + 0x9C))
#define BTIF_DBG_TX_FIFO_c(base) ((unsigned int)((base) + 0xA0))
#define BTIF_DBG_TX_FIFO_d(base) ((unsigned int)((base) + 0xA4))
#define BTIF_DBG_TX_FIFO_e(base) ((unsigned int)((base) + 0xA8))
#define BTIF_DBG_TX_FIFO_f(base) ((unsigned int)((base) + 0xAC))
#define BTIF_DBG_RX_FIFO_0(base) ((unsigned int)((base) + 0xB0))
#define BTIF_DBG_RX_FIFO_1(base) ((unsigned int)((base) + 0xB4))
#define BTIF_DBG_RX_FIFO_2(base) ((unsigned int)((base) + 0xB8))
#define BTIF_DBG_RX_FIFO_3(base) ((unsigned int)((base) + 0xBC))
#define BTIF_DBG_RX_FIFO_4(base) ((unsigned int)((base) + 0xC0))
#define BTIF_DBG_RX_FIFO_5(base) ((unsigned int)((base) + 0xC4))
#define BTIF_DBG_RX_FIFO_6(base) ((unsigned int)((base) + 0xC8))
#define BTIF_DBG_RX_FIFO_7(base) ((unsigned int)((base) + 0xCC))
#define BTIF_DBG_TX_PTR(base)    ((unsigned int)((base) + 0xD0))
#define BTIF_DBG_RX_PTR(base)    ((unsigned int)((base) + 0xD4))

/* BTIF_IER bits */
#define BTIF_IER_TXEEN (0x1 << 1) /* 1: Tx holding register is empty */
#define BTIF_IER_RXFEN (0x1 << 0) /* 1: Rx buffer contains data */

/* BTIF_IIR bits */
#define BTIF_IIR_NINT       (0x1 << 0)  /* No INT Pending */
#define BTIF_IIR_TX_EMPTY   (0x1 << 1)  /* Tx Holding Register empty */
#define BTIF_IIR_RX         (0x1 << 2)  /* Rx data received */
#define BTIF_IIR_RX_TIMEOUT (0x3 << 2)  /* Rx data received */

/* BTIF_LSR bits*/
#define BTIF_LSR_DR_BIT   (0x1 << 0)
#define BTIF_LSR_THRE_BIT (0x1 << 5)
#define BTIF_LSR_TEMT_BIT (0x1 << 6)

/* BTIF_FIFOCTRL bits*/
#define BTIF_FIFOCTRL_CLR_TX (0x1 << 2) /*Clear Tx FIRO */
#define BTIF_FIFOCTRL_CLR_RX (0x1 << 1) /*Clear Rx FIRO */

/* BTIF_FAKELCR bits*/
#define BTIF_FAKELCR_NORMAL_MODE 0x0

/* BTIF_SLEEP_EN bits*/
#define BTIF_SLEEP_EN_BIT  (0x1 << 0) /*enable Sleep mode */
#define BTIF_SLEEP_DIS_BIT (0x0)     /*disable sleep mode */

/* BTIF_DMA_EN bits*/
#define BTIF_DMA_EN_RX (0x1 << 0) /*Enable Rx DMA */
#define BTIF_DMA_EN_TX (0x1 << 1) /*Enable Tx DMA */
#define BTIF_DMA_EN_AUTORST_EN  (0x1 << 2)
/* 1: timeout counter will be auto reset */
#define BTIF_DMA_EN_AUTORST_DIS (0x0 << 2)
/* 0: after Rx timeout happens, */
/* SW shall reset the interrupt by reading BTIF 0x4C */

/* BTIF_TRI_LVL bits*/
#define BTIF_TRI_LVL_TX_MASK ((0xf) << 0)
#define BTIF_TRI_LVL_RX_MASK ((0x7) << 4)

#define BTIF_TRI_LVL_TX(x) ((x & 0xf) << 0)
#define BTIF_TRI_LVL_RX(x) ((x & 0x7) << 4)

#define BTIF_TRI_LOOP_EN  (0x1 << 7)
#define BTIF_TRI_LOOP_DIS (0x0 << 7)

/* BTIF_WAK bits*/
#define BTIF_WAK_BIT (0x1 << 0)

/* BTIF_HANDSHAKE bits*/
#define BTIF_HANDSHAKE_EN_HANDSHAKE  1
#define BTIF_HANDSHAKE_DIS_HANDSHAKE 0

#define BTIF_TX_FIFO_SIZE 16 // TX FIFO register count 0~15
#define BTIF_RX_FIFO_SIZE 8  // RX FIFO register count 0~7

#define BTIF_TX_FIFO_THRE (BTIF_TX_FIFO_SIZE / 2)
#define BTIF_RX_FIFO_THRE 0x1 /* 0x5 */

#pragma pack(1)
struct fw_patch_header_t {
	uint8_t ucDateTime[16];
	uint8_t ucPlatform[4];
	uint16_t u2HwVer;
	uint16_t u2SwVer;
	uint32_t u4MagicNum;
};

#define TX_WRITE_TIMEOUT 3000 // For timeout setting of each Tx (ms)
enum btif_tx_status_t {
	TX_IDLE = 0,
	TX_PROCESSING,
};

void btif_set_tx_status(enum btif_tx_status_t status);

enum btif_dir_t {
	BTIF_TX = 0,
	BTIF_RX = BTIF_TX + 1,
	BTIF_MAX,
};

enum btif_mode_t {
	BTIF_MODE_PIO = 0,
	BTIF_MODE_DMA = BTIF_MODE_PIO + 1,
	BTIF_MODE_MAX,
};

enum btif_reg_id_t {
	REG_IIR = 0,         /*Interrupt Identification Register */
	REG_LSR = 1,         /*Line Status Register */
	REG_FAKE_LCR = 2,    /*Fake Lcr Regiseter */
	REG_FIFO_CTRL = 3,   /*FIFO Control Register */
	REG_IER = 4,         /*Interrupt Enable Register */
	REG_SLEEP_EN = 5,    /*Sleep Enable Register */
	REG_RTO_COUNTER = 6, /*Rx Timeout Counter Register */
	REG_DMA_EN = 7,      /*DMA Enalbe Register */
	REG_TRIG_LVL = 8,    /*Tx/Rx Trigger Level Register */
	REG_WAT_TIME = 9,    /*Async Wait Time Register */
	REG_HANDSHAKE = 10,  /*New HandShake Mode Register */
	REG_SLP_WAK = 11,    /*Sleep Wakeup Reigster */
	REG_ALL = 12,        /*all  registers */
	REG_IRQ = 13,        /*IRQ  registers */
	REG_MAX
};

struct btif_buf_str_t {
	unsigned int size;
	unsigned char *p_buf;
	/*
	 * For Tx: next Tx data pointer to FIFO;
	 * For Rx: next read data pointer from BTIF user
	 */
	unsigned int rd_idx;
	/*
	 * For Tx: next Tx data pointer from BTIF user;
	 * For Rx: next write data(from FIFO) pointer
	 */
	unsigned int wr_idx;
};

#ifdef BUFFER_DEBUG_TRACE
#define MAX_BUF_DEBUG_RECORD    5
#define MAX_BUF_DATA_RECORD_LEN 16

struct btif_buf_debug_tx_irq {
	unsigned int vff_len;
	unsigned int valid_len;
	unsigned int left_len;
	unsigned int wpt;
	unsigned int rpt;
	unsigned int seq_num;
	uint8_t own_type; // save current own type at dma_tx_irq_handler
};

struct btif_buf_debug_tx_vfifo {
	unsigned char buf_data[MAX_BUF_DATA_RECORD_LEN];
	unsigned char *buf_p;
	unsigned int length;
	unsigned int ava_len;
	unsigned int wpt;
	unsigned int wpt_wrap;
	unsigned int seq_num;
};

struct btif_buf_debug_rx_irq {
	unsigned int write_len;
	unsigned int emp_len;
	unsigned int use_len;
	unsigned int rd_idx; // read index
	unsigned int wr_idx; // write index
	unsigned int seq_num;
};

struct btif_buf_debug_rx_task_buf {
	unsigned char buf_data[MAX_BUF_DATA_RECORD_LEN];
	unsigned char *buf_p;
	unsigned int length;
	unsigned int seq_num;
};

struct btif_buf_debug_trace {
	struct btif_buf_debug_tx_irq tx_irq_trace[MAX_BUF_DEBUG_RECORD];
	unsigned int tx_irq_record_idx;

	struct btif_buf_debug_tx_vfifo tx_vfifo_trace[MAX_BUF_DEBUG_RECORD];
	unsigned int tx_vfifo_buf_record_idx;

	struct btif_buf_debug_rx_irq rx_irq_trace[MAX_BUF_DEBUG_RECORD];
	unsigned int rx_irq_record_idx;

	struct btif_buf_debug_rx_task_buf rx_task_buf_trace[MAX_BUF_DEBUG_RECORD];
	unsigned int rx_task_buf_record_idx;

	unsigned char trigger;
};

void btif_buffer_debug_init(void);
void btif_buffer_debug_deinit(void);
void btif_buffer_trace_restart(void);
void btif_buffer_debug(void);
void btif_tx_irq_trace_write(unsigned int vff_len, unsigned int valid_len,
							 unsigned int left_len, unsigned int wpt,
							 unsigned int rpt, uint8_t own);
void btif_tx_vfifo_trace_write(const unsigned char *buffer, unsigned int length,
							   unsigned int ava_len, unsigned int wpt, unsigned int wpt_wrap);
void btif_rx_task_buffer_trace_write(const unsigned char *buffer, unsigned int length);
#endif

#ifdef BTIF_MAIN_TASK_TRACE
#define MAX_MAIN_TASK_RECORD_SIZE 256
#define MAX_MAIN_TASK_RECORD_DATA_LEN 5

#define RECORD_SYMBOL_DRIVER_OWN 0xC0
#define RECORD_SYMBOL_FW_OWN     0xCF
#define RECORD_SYMBOL_SEND_TX    0xD0
#define RECORD_SYMBOL_WRITE_RX   0xDF
#define RECORD_SYMBOL_RX_IRQ     0xE0
#define RECORD_SYMBOL_WAKEUP_IRQ 0xEF
#define RECORD_SYMBOL_INVALID    0xFF

struct btif_main_task_record_info {
	uint32_t period;
	uint32_t raw_len;
	uint8_t  raw[MAX_MAIN_TASK_RECORD_DATA_LEN];
	uint8_t  pre_bits;
	uint8_t  event_bits;
};

struct btif_main_task_record {
	uint32_t index;
	struct btif_main_task_record_info info[MAX_MAIN_TASK_RECORD_SIZE];
	uint8_t trigger;  // 1 means dump is triggered
	portTickType in;
};

void btif_main_task_record_init(void);
void btif_main_task_record_deinit(void);
void btif_main_task_record_write(bool isr, uint8_t pre_bits, uint8_t event_bits,
								 uint8_t *raw, uint32_t raw_len);
void btif_main_task_record_dump(void);
#endif

typedef unsigned int (*btif_rx_buf_write)(void *p_btif_info, unsigned char *p_buf,
										  unsigned int buf_len);

typedef int (*btif_rx_cb)(unsigned char *p_buf, unsigned int len);

typedef void (*btif_rx_data_ready_cb)(void);

#define BTIF_TASK_EVENT_TIMEOUT     0 // ms
#define BTIF_TASK_EVENT_TX_DONE     (0x1 << 0)
#define BTIF_TASK_EVENT_TX_WAIT     (0x1 << 1)
#define BTIF_TASK_EVENT_TX          (0x1 << 2)
#define BTIF_TASK_EVENT_RX          (0x1 << 3)
#define BTIF_TASK_EVENT_DRIVER_OWN  (0x1 << 4)
#define BTIF_TASK_EVENT_FW_OWN      (0x1 << 5)
#define BTIF_TASK_EVENT_ALL         0x3F
struct mtk_btif {
	unsigned char tx_mode;
	unsigned char rx_mode;
	unsigned char enabled;
	unsigned char lpbk_flag; /*BTIF module enable flag */
	enum btif_tx_status_t tx_status;
	struct btif_buf_str_t btif_buf; // bbs buf
	btif_rx_cb rx_cb;
	xSemaphoreHandle ownship_semaphore;
	xSemaphoreHandle main_task_mtx;
	EventGroupHandle_t main_task_event_group;
	xTaskHandle main_task_hdl;
	/* for PIO mode */
	unsigned char *p_base;
	unsigned char irq_id;
	unsigned int tx_fifo_size; /*BTIF tx FIFO size */
	unsigned int rx_fifo_size; /*BTIF rx FIFO size */
	unsigned int tx_tri_lvl;   /*BTIFtx trigger level in FIFO mode */
	unsigned int rx_tri_lvl;   /*BTIFrx trigger level in FIFO mode */
	/* for DMA mode */
	void *p_tx_dma;
	void *p_rx_dma;
};

enum _ENUM_BTIF_STATE_ {
	B_S_OFF = 0,
	B_S_SUSPEND = B_S_OFF + 1,
	B_S_DPIDLE = B_S_SUSPEND + 1,
	B_S_ON = B_S_DPIDLE + 1,
	B_S_MAX,
};

#define BTIF_READ8(addr) (*(volatile unsigned char *const)(addr))

#define BTIF_READ32(addr) (*(volatile unsigned int *)(addr))
#define BTIF_WRITE32(addr, value) (*((volatile unsigned int *)(addr)) = (unsigned int)(value))

#define BTIF_SET_BIT(REG, BITVAL)													\
	do {																			\
		unsigned int v = (unsigned int)(*((volatile unsigned int *)(REG)));			\
		*(volatile unsigned int *)(REG) = v | (unsigned int)(BITVAL);				\
	} while (0)

#define BTIF_CLR_BIT(REG, BITVAL)													\
	do {																			\
		unsigned int v = (unsigned int)(*((volatile unsigned int *)(REG)));			\
		*(volatile unsigned int *)(REG) = v & (~(unsigned int)(BITVAL));			\
	} while (0)

#define BBS_PTR(ptr, idx) ((ptr->p_buf) + idx)

#define BBS_SIZE(ptr) ((ptr)->size)
#define BBS_MASK(ptr) (BBS_SIZE(ptr) - 1)
#define BBS_COUNT(ptr)											\
	((ptr)->wr_idx >= (ptr)->rd_idx	?							\
	(ptr)->wr_idx - (ptr)->rd_idx :								\
	BBS_SIZE(ptr) - ((ptr)->rd_idx - (ptr)->wr_idx))
#define BBS_COUNT_CUR(ptr, wr_idx)								\
	(wr_idx >= (ptr)->rd_idx ? wr_idx - (ptr)->rd_idx :			\
	BBS_SIZE(ptr) - ((ptr)->rd_idx - wr_idx))

#define BBS_LEFT(ptr) (BBS_SIZE(ptr) - BBS_COUNT(ptr))

#define BBS_AVL_SIZE(ptr) (BBS_SIZE(ptr) - BBS_COUNT(ptr))
#define BBS_FULL(ptr) (BBS_COUNT(ptr) - BBS_SIZE(ptr))
#define BBS_EMPTY(ptr) ((ptr)->wr_idx == (ptr)->rd_idx)
#define BBS_WRITE_MOVE_NEXT(ptr)								\
	((ptr)->wr_idx = ((ptr)->wr_idx + 1) & BBS_MASK(ptr))
#define BBS_READ_MOVE_NEXT(ptr)									\
	((ptr)->rd_idx = ((ptr)->rd_idx + 1) & BBS_MASK(ptr))

#define BBS_INIT(ptr)											\
	{															\
		(ptr)->rd_idx = (ptr)->wr_idx = 0;						\
		(ptr)->size = BTIF_RX_BUFFER_SIZE;						\
	}

int btif_irq_ctrl(unsigned int irq_id, unsigned char en);
void btif_clk_ctrl(unsigned char en);
void btif_tx_mode_ctrl(struct mtk_btif *p_btif);
void btif_rx_mode_ctrl(struct mtk_btif *p_btif);
void btif_loopback_ctrl(unsigned char en);
int btif_send_data(const unsigned char *p_buf, unsigned int buf_len);
void btif_consummer_nty(void);
void btif_rx_cb_reg(btif_rx_cb rx_cb);
void btif_dump_reg(void);

int BTIF_write(const unsigned char *p_buf, unsigned int buf_len);
unsigned int BTIF_read(unsigned char *buffer, unsigned int buf_len);

int BTIF_open(void);
void BTIF_close(void);
void BTIF_suspend(void);
void BTIF_resume(void);

#endif /*__BTIF_MAIN_H_*/
