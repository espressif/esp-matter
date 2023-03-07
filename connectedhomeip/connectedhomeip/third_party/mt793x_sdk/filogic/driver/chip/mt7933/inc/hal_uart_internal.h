/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (c) 2020 MediaTek Inc.
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 * Filename:
 * ---------
 *  hal_uart_internal.h
 *
 * Project:
 * --------
 *   Device Test
 *
 * Description:
 * ------------
 *   This file is intends for UART driver.
 *
 * Author:
 * -------
 *  Yuchen Huang
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef __MTK_CTP_UART_HW_H__
#define __MTK_CTP_UART_HW_H__

#include <string.h>
#include <reg_base.h>
#include "hal_uart.h"
#include "typedefs.h"
#include "hal_platform.h"
//#include "semphr.h"
//#include "event_groups.h"


/*Configure region*/
#define     __UART_VIRTUAL_FIFO__
#define     __ENABLE_INTR__
#define     __SLEEP_FUNC_EN__
#define     __ENABLE_FRACTIONAL__
#define     __TIMEOUT_FEATURE__
#define     __UART_VFIFO_TX__

#define UART_DMA_MALLOC_LEN     0x20000
#define UART_DMA_PROTECT_LEN    0x10000
#define UART_DMA_USE_LEN    1000

#define UART_BUF_LEN        64
#define UART_RETRY_MAX_CNT  32
#define UART_VFF_LEN        4096
/* High level logic buffer */
#define UART_RING_BUFF_LEN      4096


#define CLOCK_RATE_6M               6000000
#define CLOCK_RATE_10M              10000000
#define CLOCK_RATE_12M              12000000
#define CLOCK_RATE_26M              26000000
#define CLOCK_RATE_40M              40000000
#define CLOCK_RATE_52M              52000000

#ifndef CFG_FPGA
#define UART_SRC_CLK        (CLOCK_RATE_26M) //use in real chip
#else /* #ifndef CFG_FPGA */
#define UART_SRC_CLK        (CLOCK_RATE_12M) //use in FPGA
#endif /* #ifndef CFG_FPGA */

#ifdef MTK_FPGA_ENABLE
#define UART_CLOCK_RATE             CLOCK_RATE_12M
#else /* #ifdef MTK_FPGA_ENABLE */
#define UART_CLOCK_RATE             CLOCK_RATE_26M
#endif /* #ifdef MTK_FPGA_ENABLE */

#define UART_CONSOLE_BAUD_RATE          921600
#define UART_CONSOLE_PORT_ADDR          CM33_UART_BASE

#define UART_TX_FIFO_LENGTH             32
#define UART_RX_FIFO_LENGTH             32

#define DRV_SUPPORT_UART_PORTS          4
#define DRV_SUPPORT_VFF_CHANNEL (DRV_SUPPORT_UART_PORTS * 2)

#define UART_BASE_ADDR_NA       0
#define UART0_BASE_ADDR         CM33_UART_BASE
#define UART1_BASE_ADDR         AP_UART0_BASE
#define UART2_BASE_ADDR         AP_UART1_BASE
#define UART3_BASE_ADDR         DSP_UART_BASE
#define UART4_BASE_ADDR         UART_BASE_ADDR_NA
#define UART5_BASE_ADDR         UART_BASE_ADDR_NA


#define UART_IRQ_BIT_ID_NA      0
#define UART0_IRQ_ID            UART_IRQn
#define UART1_IRQ_ID            TOP_UART0_IRQn
#define UART2_IRQ_ID            TOP_UART1_IRQn
#define UART3_IRQ_ID            DSP_UART_IRQn
#define UART4_IRQ_ID            UART_IRQ_BIT_ID_NA
#define UART5_IRQ_ID            UART_IRQ_BIT_ID_NA


/* DMA HW information */
#define UART_DMA_OFFSET         (0x280)
#define UART_DMA_BASE           (AP_DMA_BASE + UART_DMA_OFFSET)
/* Attention! Some IC's DMA channel not continues */

/* Only top uart0/ uart1 support APDMA  on MT7663*/
#define VFF_BASE_CH_0           0                               //cm33_uart_tx
#define VFF_BASE_CH_1           0                               //cm33_uart_rx
#define VFF_BASE_CH_2           (UART_DMA_BASE + 0x0080*0)  //top_uart0_tx
#define VFF_BASE_CH_3           (UART_DMA_BASE + 0x0080*1)  //top_uart0_rx
#define VFF_BASE_CH_4           (UART_DMA_BASE + 0x0080*2)  //top_uart1_tx
#define VFF_BASE_CH_5           (UART_DMA_BASE + 0x0080*3)  //top_uart1_rx
#define VFF_BASE_CH_6           0                               //dsp_uart_tx
#define VFF_BASE_CH_7           0                               //dsp_uart_rx

#define AP_DMA_UART_IRQ_ID_NA   0
#define VFF_UART0_TX_IRQ_ID     AP_DMA_UART_IRQ_ID_NA
#define VFF_UART0_RX_IRQ_ID     AP_DMA_UART_IRQ_ID_NA
#define VFF_UART1_TX_IRQ_ID     APDMA4_IRQn             //top uart0_rx
#define VFF_UART1_RX_IRQ_ID     APDMA5_IRQn             //top_uart0_rx
#define VFF_UART2_TX_IRQ_ID     APDMA6_IRQn             //top_uart1_tx
#define VFF_UART2_RX_IRQ_ID     APDMA7_IRQn             //top_uart1_rx
#define VFF_UART3_TX_IRQ_ID     AP_DMA_UART_IRQ_ID_NA
#define VFF_UART3_RX_IRQ_ID     AP_DMA_UART_IRQ_ID_NA


enum uart_port_ID {
    UART_PORT0 = 0,     //cm33
    UART_PORT1,     //top uart0
    UART_PORT2,     //top uart1
    UART_PORT3,     //dsp uart
    UART_PORT_MAX,
};

enum vff_channel_ID {
    VFF_UART0_TX_ID = 0,
    VFF_UART0_RX_ID,
    VFF_UART1_TX_ID,
    VFF_UART1_RX_ID,
    VFF_UART2_TX_ID,
    VFF_UART2_RX_ID,
    VFF_UART3_TX_ID,
    VFF_UART3_RX_ID,
};


#define UART_CLK_OFF_REQ                (1 << 0)
#define UART_CLK_OFF_ACK                (1 << 0)
#define WAIT_UART_ACK_TIMES             10

#define UART_NEW_FEATURE_SWITCH_OPTION      (0)
#define UART_FEATURE_SEL_NEW_MAP        (1 << 0)
#define UART_FEATURE_SEL_NO_DOWNLOAD        (1 << 7)


/*UART HW Register*/
#define UART_RBR(uart)                    ((uart)+0x0)  /* Read only */
#define UART_THR(uart)                    ((uart)+0x0)  /* Write only */
#define UART_IER(uart)                    ((uart)+0x4)
#define UART_IIR(uart)                    ((uart)+0x8)  /* Read only */
#define UART_FCR(uart)                    ((uart)+0x8)  /* Write only */
#define UART_LCR(uart)                    ((uart)+0xc)
#define UART_MCR(uart)                    ((uart)+0x10)
#define UART_LSR(uart)                    ((uart)+0x14)
#define UART_MSR(uart)                    ((uart)+0x18)
#define UART_SCR(uart)                    ((uart)+0x1c)
#define UART_DLL(uart)                    ((uart)+0x0)  /* Only when LCR.DLAB = 1 */
#define UART_DLH(uart)                    ((uart)+0x4)  /* Only when LCR.DLAB = 1 */
#define UART_EFR(uart)                    ((uart)+0x8)  /* Only when LCR = 0xbf */
#define UART_XON1(uart)                   ((uart)+0x10) /* Only when LCR = 0xbf */
#define UART_XON2(uart)                   ((uart)+0x14) /* Only when LCR = 0xbf */
#define UART_XOFF1(uart)                  ((uart)+0x18) /* Only when LCR = 0xbf */
#define UART_XOFF2(uart)                  ((uart)+0x1c) /* Only when LCR = 0xbf */
//#define UART_AUTOBAUD_EN(uart)            ((uart)+0x20)
#define UART_HIGHSPEED(uart)              ((uart)+0x24)
#define UART_SAMPLE_COUNT(uart)           ((uart)+0x28)
#define UART_SAMPLE_POINT(uart)           ((uart)+0x2c)
#define UART_AUTOBAUD_REG(uart)           ((uart)+0x30)
#define UART_RATE_FIX_AD(uart)            ((uart)+0x34)
#define UART_AUTOBAUD_SAMPLE(uart)        ((uart)+0x38)
#define UART_GUARD(uart)                  ((uart)+0x3c)
#define UART_ESCAPE_DAT(uart)             ((uart)+0x40)
#define UART_ESCAPE_EN(uart)              ((uart)+0x44)
#define UART_SLEEP_EN(uart)               ((uart)+0x48)
#define UART_DMA_EN(uart)                 ((uart)+0x4c)
#define UART_RXTRI_AD(uart)               ((uart)+0x50)
#define UART_FRACDIV_L(uart)              ((uart)+0x54)
#define UART_FRACDIV_M(uart)              ((uart)+0x58)
#define UART_FCR_RD(uart)                 ((uart)+0x5C)
#define UART_FEATURE_SEL(uart)            ((uart)+0x9C)
#define UART_SLEEP_REQ(uart)              ((uart)+0xB4)
#define UART_SLEEP_ACK(uart)                ((uart)+0xB8)
#define UART_SPM_SEL(uart)                 ((uart)+0xBC)

/* LSR */
#define UART_LSR_DR                 (1 << 0)
#define UART_LSR_OE                 (1 << 1)
#define UART_LSR_PE                 (1 << 2)
#define UART_LSR_FE                 (1 << 3)
#define UART_LSR_BI                 (1 << 4)
#define UART_LSR_THRE               (1 << 5)
#define UART_LSR_TEMT               (1 << 6)
#define UART_LSR_FIFOERR            (1 << 7)

/* LCR */
#define UART_LCR_BREAK              (1 << 6)
#define UART_LCR_DLAB               (1 << 7)

#define UART_WLS_5                  (0 << 0)
#define UART_WLS_6                  (1 << 0)
#define UART_WLS_7                  (2 << 0)
#define UART_WLS_8                  (3 << 0)
#define UART_WLS_MASK               (3 << 0)


#define UART_1_STOP                 (0 << 2)
#define UART_2_STOP                 (1 << 2)
#define UART_1_5_STOP               (1 << 2)    /* Only when WLS=5 */
#define UART_STOP_MASK              (1 << 2)

#define UART_NONE_PARITY            (0 << 3)
#define UART_ODD_PARITY             (0x1 << 3)
#define UART_EVEN_PARITY            (0x3 << 3)
#define UART_MARK_PARITY            (0x5 << 3)
#define UART_SPACE_PARITY           (0x7 << 3)
#define UART_PARITY_MASK            (0x7 << 3)

/* IER */
#define UART_IER_ERBFI              0x0001
#define UART_IER_ETBEI              0x0002
#define UART_IER_ELSI               0x0004
#define UART_IER_EDSSI              0x0008
#define UART_IER_XOFFI              0x0020
#define UART_IER_RTSI               0x0040
#define UART_IER_CTSI               0x0080
#define IER_HW_NORMALINTS           0x0005
#define IER_HW_ALLINTS              0x0007
#define IER_SW_NORMALINTS           0x002d
#define IER_SW_ALLINTS              0x002f
#define UART_IER_ALLOFF             0x0000
#define UART_IER_VFIFO              0x0001

/* FCR */
#define UART_FCR_FIFOE              (1 << 0)
#define UART_FCR_CLRR               (1 << 1)
#define UART_FCR_CLRT               (1 << 2)
#define UART_FCR_RXFIFO_1B_TRI      (0 << 6)
#define UART_FCR_RXFIFO_6B_TRI      (1 << 6)
#define UART_FCR_RXFIFO_12B_TRI     (2 << 6)
#define UART_FCR_RXFIFO_RX_TRI      (3 << 6)
#define UART_FCR_TXFIFO_1B_TRI      (0 << 4)
#define UART_FCR_TXFIFO_4B_TRI      (1 << 4)
#define UART_FCR_TXFIFO_8B_TRI      (2 << 4)
#define UART_FCR_TXFIFO_14B_TRI     (3 << 4)
#define UART_FCR_FIFO_INIT          (UART_FCR_FIFOE|UART_FCR_CLRR|UART_FCR_CLRT)
#define UART_FCR_NORMAL             (UART_FCR_FIFO_INIT|UART_FCR_TXFIFO_4B_TRI|UART_FCR_RXFIFO_12B_TRI)
#define UART_FCR_TRIGGER_LEVEL      (UART_FCR_TXFIFO_4B_TRI|UART_FCR_RXFIFO_6B_TRI)
#define UART_TX_THRESHOLD_SETTING   4   //should same as TX level at UART_FCR_NORMAL_TRIG

/* IIR,RO */
#define   UART_IIR_INT_INVALID            0x0001
#define   UART_IIR_RLS                    0x0006  // Receiver Line Status
#define   UART_IIR_RDA                    0x0004  // Receive Data Available
#define   UART_IIR_CTI                    0x000C  // Character Timeout Indicator
#define   UART_IIR_THRE                   0x0002  // Transmit Holding Register Empty
#define   UART_IIR_MS                     0x0000  // Check Modem Status Register
#define   UART_IIR_SWFlowCtrl             0x0010  // Receive XOFF characters
#define   UART_IIR_HWFlowCtrl             0x0020  // CTS or RTS Rising Edge
#define   UART_IIR_FIFOS_ENABLED          0x00c0
#define   UART_IIR_NO_INTERRUPT_PENDING   0x0001
#define   UART_IIR_INT_MASK               0x003f

//MCR
#define UART_MCR_DTR                0x0001
#define UART_MCR_RTS                0x0002
#define UART_MCR_LOOPB              0x0010
#define UART_MCR_IRE                0x0040  //Enable IrDA modulation/demodulation
#define UART_MCR_XOFF               0x0080
#define UART_MCR_Normal             UART_MCR_RTS

//MSR
#define UART_MSR_DCTS               0x0001
#define UART_MSR_DDSR               0x0002
#define UART_MSR_TERI               0x0004
#define UART_MSR_DDCD               0x0008
#define UART_MSR_CTS                0x0010
#define UART_MSR_DSR                0x0020
#define UART_MSR_RI                 0x0040
#define UART_MSR_DCD                0x0080

//EFR
#define UART_EFR_AutoCTS            0x0080
#define UART_EFR_AutoRTS            0x0040
#define UART_EFR_Enchance           0x0010
#define UART_EFR_SWCtrlMask         0x000f
#define UART_EFR_NoSWFlowCtrl       0x0000
#define UART_EFR_ALLOFF             0x0000
#define UART_EFR_AutoRTSCTS         0x00c0

//DMA_EN
#define UART_RX_DMA_EN              0x0001
#define UART_TX_DMA_EN              0x0002
#define UART_TO_CNT_AUTORST             0x0004

//Tx/Rx XON1/Xoff1 as flow control word
#define UART_EFR_SWFlowCtrlX1       0x000a

//Tx/Rx XON2/Xoff2 as flow control word
#define UART_EFR_SWFlowCtrlX2       0x0005

//Tx/Rx XON1&XON2/Xoff1&Xoff2 as flow control word
#define UART_EFR_SWFlowCtrlXAll     0x000f

#define UART_RATEFIX_EN             0x0001
#define UART_RATEFIX_AUTOBAUD_EN    0x0002
#define UART_RATEFIX_FREQ_13        0x0004

//#define UART_AUTOBAUD_EN                  0x1
#define UART_AUTOBAUD_NON_STANDARD  0x2


#define AUTOBAUDSAMPLE_13M          0x7
#define AUTOBAUDSAMPLE_26M          0xf
#define AUTOBAUDSAMPLE_52M          0x1b

#define UART_RXDMA_ON               0x0001
#define UART_RXDMA_OFF              0x0000

#define UART_VFIFO_ON               0x0007
#define UART_VFIFO_OFF              0x0000

#define UART_TxFIFO_DEPTH           16
#define UART_RxFIFO_DEPTH           16
#define UART_VFIFO_DEPTH            7

//#define UART_CLK_CG_CTRL (ADSP_CK_CTRL_BASE + 0x10)
#define UART0_CG_ENABLE ((1<<5))
#define UART0_CLK ((1<<0))
#define UART0_RST (1<<3)

#define VFIFO_ENABLE_FLAG_NULL  (0)
#define VFIFO_TX_ENABLE_FLAG    (1<<0)
#define VFIFO_RX_ENABLE_FLAG    (1<<1)
#define DMA_WARM_RST            (1<<0)

#define UART_CHAR_XON1      0x11
#define UART_CHAR_XOFF1     0x22
#define UART_CHAR_XON2      0x18
#define UART_CHAR_XOFF2     0x28
#define UART_CHAR_ESCAPE    0x77

typedef struct __BUFFER_INFO {
    uint16_t    Read;
    uint16_t    Write;
    uint16_t    Length;
    uint8_t    *CharBuffer;
} BUFFER_INFO;

#define ResetFifo(Buffer)           (Buffer)->Write = (Buffer)->Read = 0
#define BWrite(Buffer)          ((Buffer)->Write)
#define BRead(Buffer)           ((Buffer)->Read)
#define BLength(Buffer)         ((Buffer)->Length)
#define BuffWrite(Buffer)           ((Buffer)->CharBuffer+(Buffer)->Write)
#define BuffRead(Buffer)        ((Buffer)->CharBuffer+(Buffer)->Read)
#define BuffStart(Buffer)           ((Buffer)->CharBuffer)
#define BuffEnd(Buffer)             ((Buffer)->CharBuffer+(Buffer)->Length-1)


#define Buff_isEmpty                1
#define Buff_notEmpty               0
#define Buff_isFull             1
#define Buff_notFull                0
#define Buff_PushOK             0
#define Buff_PushErr                1
#define Buff_PopOK              0
#define Buff_PopErr             1

void Buff_init(BUFFER_INFO *Buf, uint8_t *Buffaddr, uint16_t uTotalSize);
void Buff_Push(BUFFER_INFO *Buf, const uint8_t *pushData);
void Buff_Pop(BUFFER_INFO *Buf, uint8_t *popData);
uint8_t Buff_IsEmpty(BUFFER_INFO *Buf);
uint8_t Buff_IsFull(BUFFER_INFO *Buf);
uint16_t Buff_GetRoomLeft(BUFFER_INFO *Buf);
uint16_t Buff_GetBytesAvail(BUFFER_INFO *Buf);
uint16_t Buff_GetLength(BUFFER_INFO *Buf);
void Buff_Flush(BUFFER_INFO *Buf);


typedef enum {
    DIR_TX,
    DIR_RX,
} UART_DIR;

typedef enum {
    uart_port0 = 0, //cm33 uart
    uart_port1, //top uart0
    uart_port2, //top uart1
    uart_port3, //dsp uart
    uart_max_port,
} UART_PORT;

typedef enum {
    len_5 = 0,
    len_6,
    len_7,
    len_8,
} UART_bits_per_ch;

typedef enum {
    sb_1 = 0,
    sb_2,
    sb_1_5,
} UART_stop_bits;

typedef enum {
    pa_none = 0,
    pa_odd,
    pa_even,
    pa_space,
    pa_mark,
} UART_parity;

typedef enum {
    fc_none = 0,
    fc_hw,
    fc_sw,
    fc_sw_mtk,
} UART_flow_ctrl_mode;

typedef struct {
    uint32_t            baud;
    UART_bits_per_ch    dataBits;
    UART_stop_bits      stopBits;
    UART_parity     parity;
    UART_flow_ctrl_mode flowControl;
    uint8_t         xonChar;
    uint8_t         xoffChar;
    uint8_t         xon1Char;
    uint8_t         xoff1Char;
    uint8_t         escapeChar;
} UART_dcb_struct;

typedef struct {
    uint8_t *rx_adrs;
    uint8_t *tx_adrs;
    uint16_t    rx_len;
    uint16_t    tx_len;
} UART_ring_buffer_struct;

typedef struct {
    U8      *read_buff;
    U32     offset;
} UART_read_buffer_struct;


typedef struct {
    uint32_t            port_id;
    uint32_t            initialized;
    uint32_t            opened;
    uint32_t            use_vfifo;
    UART_dcb_struct     dcb;
    BUFFER_INFO     rx_buffer;  /* receive buffer */
    BUFFER_INFO     tx_buffer;  /* transmit buffer */
    uint32_t            uart_reg_base;
    uint32_t            uart_irq_id;
    uint32_t            vff_mode;
    uint32_t            rx_dma_reg_base;
    uint32_t            tx_dma_reg_base;
    uint32_t            rx_dma_irq_id;
    uint32_t            tx_dma_irq_id;
    uint32_t            dropped;
    uint32_t            flag;
    uint32_t            init_flag;
    void           *rx_dma_static_info;
    void           *rx_dma_run_time_info;
    void           *tx_dma_static_info;
    void           *tx_dma_run_time_info;
    void           *uart_static_info;
    void           *uart_run_time_info;
} mtk_uart_setting;

typedef struct {
    unsigned int ier;
    unsigned int mcr;
    unsigned int msr;
    unsigned int scr;
    unsigned int dll;
    unsigned int dlm;
    unsigned int lcr;
    unsigned int efr;
    unsigned int xon1;
    unsigned int xon2;
    unsigned int xoff1;
    unsigned int xoff2;
    unsigned int highspeed;
    unsigned int sample_count;
    unsigned int sample_point;
    unsigned int guard;
    unsigned int escape_dat;
    unsigned int escape_en;
    unsigned int sleep_en;
    unsigned int dma_en;
    unsigned int rxtri_ad;
    unsigned int fracdiv_l;
    unsigned int fracdiv_m;
    unsigned int fcr;
    unsigned int fcr_rd;
    unsigned int rx_sel;
} mtk_uart_register;

typedef struct {
    unsigned int vff_int_flag;
    unsigned int vff_int_en;
    unsigned int vff_en;
    unsigned int vff_addr;
    unsigned int vff_len;
    unsigned int vff_thre;
    unsigned int vff_rx_fc;
} mtk_apdma_uart_register;

typedef struct gpio_info {
    int gpio_pin;
    int gpio_mode;
} gpio_T;

#ifdef __UART_VIRTUAL_FIFO__

typedef struct {
    unsigned int reg_base;
    unsigned int irq_id: 8;
    unsigned int direction: 8;
    unsigned int channel: 8;
    unsigned int bind_uart_port: 8;
} VFIFO_static_info;

typedef struct {
    unsigned int reg_base;
    unsigned int irq_id;
    unsigned int mem_base;
    unsigned int mem_length;
    unsigned int direction;
    unsigned int is_power_on;
    unsigned int channel;
    VFIFO_static_info *p_info;
    void         *uart_ctrl_head;
} VFIFO_ctrl_struct;

#endif /* #ifdef __UART_VIRTUAL_FIFO__ */

#define EnableRxIntr(_baseaddr)   \
{\
      uint16_t _IER;\
      _IER = DRV_Reg32(UART_IER(_baseaddr));\
      _IER |= (UART_IER_ERBFI | UART_IER_ELSI);\
      DRV_WriteReg32(UART_IER(_baseaddr),_IER);\
}

#define DisableRxIntr(_baseaddr)   \
{\
   uint16_t _IER;\
   uint32_t _savedMask;\
   _savedMask = SaveAndSetIRQMask();\
   _IER = DRV_Reg32(UART_IER(_baseaddr));\
   _IER &= ~(UART_IER_ERBFI|UART_IER_ELSI);\
   DRV_WriteReg32(UART_IER(_baseaddr),_IER);\
   RestoreIRQMask(_savedMask);\
}


#define EnableTxIntr(_baseaddr)   \
{\
   uint16_t _IER;\
   _IER = DRV_Reg32(UART_IER(_baseaddr));\
   _IER |= UART_IER_ETBEI;\
   DRV_WriteReg32(UART_IER(_baseaddr),_IER);\
}

#define DisableTxIntr(_baseaddr)   \
{\
   uint16_t _IER;\
   uint32_t _savedMask;\
   _savedMask = SaveAndSetIRQMask();\
   _IER = DRV_Reg32(UART_IER(_baseaddr));\
   _IER &= ~UART_IER_ETBEI;\
   DRV_WriteReg32(UART_IER(_baseaddr),_IER);\
   RestoreIRQMask(_savedMask);\
}

#define DisableRLSIntr(_baseaddr)   \
{\
   uint16_t _IER;\
   uint32_t _savedMask;\
   _savedMask = SaveAndSetIRQMask();\
   _IER = DRV_Reg32(UART_IER(_baseaddr));\
   _IER &= ~(UART_IER_ELSI);\
   DRV_WriteReg32(UART_IER(_baseaddr),_IER);\
   RestoreIRQMask(_savedMask);\
}

/* IO functions*/
#define __IO_WRITE32__(addr,data)   ((*(volatile unsigned int *)(addr)) = (unsigned int)data)
#define __IO_READ32__(addr)     (*(volatile unsigned int *)(addr))
#define __IO_WRITE16__(addr,data)   ((*(volatile unsigned short *)(addr)) = (unsigned short)data)
#define __IO_READ16__(addr)     (*(volatile unsigned short *)(addr))
#define __IO_WRITE8__(addr,data)    ((*(volatile unsigned char *)(addr)) = (unsigned char)data)
#define __IO_READ8__(addr)      (*(volatile unsigned char *)(addr))


/*APDMA HW Register*/
#define VFF_INT_FLAG(_b)        (_b+0x0000)
#define VFF_INT_EN(_b)          (_b+0x0004)
#define VFF_EN(_b)              (_b+0x0008)
#define VFF_RST(_b)             (_b+0x000C)
#define VFF_STOP(_b)            (_b+0x0010)
#define VFF_FLUSH(_b)           (_b+0x0014)
#define VFF_ADDR(_b)            (_b+0x001C)
#define VFF_LEN(_b)             (_b+0x0024)
#define VFF_THRE(_b)            (_b+0x0028)
#define VFF_WPT(_b)             (_b+0x002C)
#define VFF_RPT(_b)             (_b+0x0030)
#define VFF_RX_FLOWCTL_THRE(_b) (_b+0x0034)
#define VFF_INT_BUF_SIZE(_b)    (_b+0x0038)
#define VFF_VALID_SIZE(_b)      (_b+0x003C)
#define VFF_LEFT_SIZE(_b)       (_b+0x0040)
#define VFF_DEBUG_STATUS(_b)    (_b+0x0050)

#define DMA_GLOBAL_SEC_EN           (AP_DMA_BASE + 0x10)
#define DMA_GLOBAL_GSEC_EN          (AP_DMA_BASE + 0x14)
#define DMA_GLOBAL_VIO_DBG1         (AP_DMA_BASE + 0x18)
#define DMA_GLOBAL_VIO_DBG0         (AP_DMA_BASE + 0x1C)
#define APDMA_UART0_TX_SEC_EN       (AP_DMA_BASE + 0x30)
#define APDMA_GLOBAL_DOMAIN_CFG0    (AP_DMA_BASE + 0x74)

/*VFF_INT_FLAG */
#define VFF_RX_INT_FLAG0_B      (1 << 0)    /*rx_vff_valid_size >= rx_vff_thre*/
#define VFF_RX_INT_FLAG1_B      (1 << 1)    /*when UART issues flush to DMA and all data in UART VFIFO is transferred to VFF*/
#define VFF_TX_INT_FLAG0_B      (1 << 0)    /*tx_vff_left_size >= tx_vff_thrs*/
#define VFF_RX_INT_FLAG_CLR_B   ((1 << 0)|(1 << 1)) /* write 1 to clear INT*/
#define VFF_TX_INT_FLAG_CLR_B   (0 << 0)    /* write 0 to clear INT*/
/*VFF_INT_EN*/
#define VFF_RX_INT_EN0_B        (1 << 0)    /*rx_vff_valid_size >= rx_vff_thre*/
#define VFF_RX_INT_EN1_B        (1 << 1)    /*when UART issues flush to DMA and all data in UART VFIFO is transferred to VFF*/
#define VFF_TX_INT_EN_B         (1 << 0)    /*tx_vff_left_size >= tx_vff_thrs*/
#define VFF_INT_EN_CLR_B        (0 << 0)
/*VFF_RST*/
#define VFF_WARM_RST_B          (1 << 0)
#define VFF_HARD_RST_B          (1 << 1)
/*VFF_EN*/
#define VFF_EN_B                (1 << 0)
/*VFF_STOP*/
#define VFF_STOP_B              (1 << 0)
#define VFF_STOP_CLR_B          (0 << 0)
/*VFF_FLUSH*/
#define VFF_FLUSH_B             (1 << 0)
#define VFF_FLUSH_CLR_B         (0 << 0)

#define VFF_TX_THRE(n)          ((n)*7/8)   /* tx_vff_left_size >= tx_vff_thrs*/
#define VFF_RX_THRE(n)          ((n)*3/4)   /* trigger level of rx vfifo */

#define VFIFO_GET_LEFT(base)    __IO_READ32__(VFF_LEFT_SIZE(base))
#define VFIFO_GET_AVAIL(base)   __IO_READ32__(VFF_VALID_SIZE(base))
#define VFIFO_IS_FULL(base)     (0 == VFIFO_GET_LEFT(base))

#define DMA_SLOW_DOWN_TX        0xFFC1
#define DMA_SLOW_DOWN_RX        0xFFC10000

/*DMA GLOBAL SEC*/
#define DMA_GSEC_EN_B           (1 << 0)

#define BIT_REV                 (1 << 0)
#define BIT_NOTIFY_RESET        (1 << 1)
#define BIT_ALL                 (BIT_REV | BIT_NOTIFY_RESET)

#define UART_BAUD_AUTO          0
#define UART_BAUD_75            75
#define UART_BAUD_110           110
#define UART_BAUD_150           150
#define UART_BAUD_300           300
#define UART_BAUD_600           600
#define UART_BAUD_1200          1200
#define UART_BAUD_2400          2400
#define UART_BAUD_4800          4800
#define UART_BAUD_7200          7200
#define UART_BAUD_9600          9600
#define UART_BAUD_14400         14400
#define UART_BAUD_19200         19200
#define UART_BAUD_28800         28800
#define UART_BAUD_33900         33900
#define UART_BAUD_38400         38400
#define UART_BAUD_57600         57600
#define UART_BAUD_115200        115200
#define UART_BAUD_230400        230400
#define UART_BAUD_460800        460800
#define UART_BAUD_921600        921600
#define UART_BAUD_1000000       1000000
#define UART_BAUD_1500000       1500000
#define UART_BAUD_2000000       2000000
#define UART_BAUD_2500000       2500000
#define UART_BAUD_3000000       3000000
#define UART_BAUD_3250000       3250000
#define UART_BAUD_3500000       3500000
#define UART_BAUD_4000000       4000000

/* UART default register value */
#define UART_THR_DEF        0x0         /* RO, WO */
#define UART_IER_DEF        0x0         /* RW */
#define UART_IIR_DEF        0x1         /* RO */
#define UART_LCR_DEF        0x0         /* RW */
#define UART_FCR_DEF        0x0         /* WO */

#define UART_MCR_DEF        0x0         /* RW */
#define UART_LSR_DEF        0x60            /* RO */
#define UART_MSR_DEF        0x0         /* RW */
#define UART_SCR_DEF        0x0         /* RW */
#define UART_DLL_DEF        0x1         /* RW */
#define UART_DLM_DEF        0x0         /* RW */
#define UART_EFR_DEF        0x0         /* RW */
#define UART_XON1_DEF       0x0         /* RW */
#define UART_XON2_DEF       0x0         /* RW */
#define UART_XOFF1_DEF      0x0         /* RW */
#define UART_XOFF2_DEF      0x0         /* RW */
#define UART_AUTOBAUD_DEF   0x0         /* RW */
#define UART_HIGHSPEED_DEF  0x0         /* RW */
#define UART_SAMPLE_COUNT_DEF   0x0         /* RW */
#define UART_SAMPLE_POINT_DEF   0xFF            /* RW */
#define UART_AUTOBAUD_DEF   0x0         /* RO */
#define UART_RATEFIX_AD_DEF 0x0         /* RW */
#define UART_AUTOBAUD_SAMPLE_DEF 0xD            /* RW */
#define UART_GUARD_DEF      0xF         /* RW */

#define UART_ESCAPE_DAT_DEF 0xFF            /* RW */
#define UART_ESCAPE_EN_DEF  0x0         /* RW */
#define UART_SLEEP_EN_DEF   0x0         /* RW */
#define UART_DMA_EN_DEF     0x0         /* RW */
#define UART_RXTRI_AD_DEF   0x0         /* RW */
#define UART_FRACDIV_L_DEF  0x0         /* RW */
#define UART_FRACDIV_M_DEF  0x0         /* RW */
#define UART_FCR_RD_DEF     0x0         /* RO */
#define UART_FEATURE_SEL_DEF    0x0         /* RW */
#define UART_USB_RX_SEL_DEF 0x0         /* RW */
#define UART_SLEEP_REQ_DEF  0x0         /* RW */
#define UART_SLEEP_ACK_DEF  0x1         /* RO */
#define UART_SPM_SEL_DEF    0x0         /* RW */

void uart_early_init(unsigned int addr,
                     unsigned int baudrate, unsigned int uart_clk);

void uart_register_irq(void);

void prvUARTLoopback(void *pvParameters);

int mtk_uart_init_ut(unsigned int port);

#endif /* #ifndef __MTK_CTP_UART_HW_H__ */
