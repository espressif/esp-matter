/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (c) 2010 MediaTek Inc.
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
 *  uart_hw.h
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
 *  Jensen Hu
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *
 *******************************************************************************/

#ifndef __MTK_CTP_UART_HW_H__
#define __MTK_CTP_UART_HW_H__

#include "reg_base.h"

#define CLOCK_RATE_6M               6000000
#define CLOCK_RATE_10M              10000000
#define CLOCK_RATE_12M              12000000
#define CLOCK_RATE_26M              26000000
#define CLOCK_RATE_40M              40000000
#define CLOCK_RATE_52M              52000000

#define UART_CONSOLE_BAUD_RATE          921600
#define UART_CONSOLE_PORT_ADDR          AP_UART0_BASE

#ifdef MTK_FPGA_ENABLE
#define UART_CLOCK_RATE             CLOCK_RATE_12M
#else /* #ifdef MTK_FPGA_ENABLE */
#define UART_CLOCK_RATE             CLOCK_RATE_26M
#endif /* #ifdef MTK_FPGA_ENABLE */

#define UART_TX_FIFO_LENGTH         32
#define UART_RX_FIFO_LENGTH         32

/* Request UART to sleep*/
#define UART_CLK_OFF_REQ            (1 << 0)
/*---------------------------------------------------------------------------*/
/* UART sleep ack*/
#define UART_CLK_OFF_ACK            (1 << 0)
#define WAIT_UART_ACK_TIMES         10

#define UART_NEW_FEATURE_SWITCH_OPTION  (0)

/*used in Task or normal function*/
#define UART_RBR(_baseaddr)         (_baseaddr+0x0)      /* Read only */
#define UART_THR(_baseaddr)         (_baseaddr+0x0)      /* Write only */
#define UART_IER(_baseaddr)         (_baseaddr+0x4)      /* Read Write */
#define UART_IIR(_baseaddr)         (_baseaddr+0x8)      /* Read only */
#define UART_FCR(_baseaddr)         (_baseaddr+0x8)      /* Write only */
#define UART_LCR(_baseaddr)         (_baseaddr+0xc)
#define UART_MCR(_baseaddr)         (_baseaddr+0x10)
#define UART_LSR(_baseaddr)         (_baseaddr+0x14)
#define UART_MSR(_baseaddr)         (_baseaddr+0x18)
#define UART_SCR(_baseaddr)         (_baseaddr+0x1c)
#if UART_NEW_FEATURE_SWITCH_OPTION
#define UART_DLL(_baseaddr)         (_baseaddr+0x90)
#define UART_DLH(_baseaddr)         (_baseaddr+0x94)
#else /* #if UART_NEW_FEATURE_SWITCH_OPTION */
#define UART_DLL(_baseaddr)         (_baseaddr+0x0)
#define UART_DLH(_baseaddr)         (_baseaddr+0x4)
#endif /* #if UART_NEW_FEATURE_SWITCH_OPTION */

#define UART_FEATURE_SEL(_baseaddr) (_baseaddr+0x9C)

#if UART_NEW_FEATURE_SWITCH_OPTION
#define UART_EFR(_baseaddr)         (_baseaddr+0x98)
#define UART_XON1(_baseaddr)        (_baseaddr+0xA0)
#define UART_XON2(_baseaddr)        (_baseaddr+0xA4)
#define UART_XOFF1(_baseaddr)       (_baseaddr+0xA8)
#define UART_XOFF2(_baseaddr)       (_baseaddr+0xAc)
#else /* #if UART_NEW_FEATURE_SWITCH_OPTION */
#define UART_EFR(_baseaddr)         (_baseaddr+0x8)      /* Only when LCR = 0xbf */
#define UART_XON1(_baseaddr)        (_baseaddr+0x10)     /* Only when LCR = 0xbf */
#define UART_XON2(_baseaddr)        (_baseaddr+0x14)     /* Only when LCR = 0xbf */
#define UART_XOFF1(_baseaddr)       (_baseaddr+0x18)     /* Only when LCR = 0xbf */
#define UART_XOFF2(_baseaddr)       (_baseaddr+0x1c)     /* Only when LCR = 0xbf */
#endif /* #if UART_NEW_FEATURE_SWITCH_OPTION */
#define UART_AUTOBAUD(_baseaddr)        (_baseaddr+0x20)
#define UART_HIGHSPEED(_baseaddr)       (_baseaddr+0x24)
#define UART_SAMPLE_COUNT(_baseaddr)    (_baseaddr+0x28)
#define UART_SAMPLE_POINT(_baseaddr)    (_baseaddr+0x2c)
#define UART_AUTOBAUD_REG(_baseaddr)    (_baseaddr+0x30)
#define UART_RATE_FIX_REG(_baseaddr)    (_baseaddr+0x34)
#define UART_AUTO_BAUDSAMPLE(_baseaddr) (_baseaddr+0x38)
#define UART_GUARD(_baseaddr)           (_baseaddr+0x3c)
#define UART_ESCAPE_DAT(_baseaddr)      (_baseaddr+0x40)
#define UART_ESCAPE_EN(_baseaddr)       (_baseaddr+0x44)
#define UART_SLEEP_EN(_baseaddr)        (_baseaddr+0x48)
#define UART_DMA_EN(_baseaddr)      (_baseaddr+0x4c)
#define UART_RXTRI_AD(_baseaddr)        (_baseaddr+0x50)
#define UART_FRACDIV_L(_baseaddr)       (_baseaddr+0x54)
#define UART_FRACDIV_M(_baseaddr)       (_baseaddr+0x58)
#define UART_FCR_RD(_baseaddr)          (_baseaddr+0x5C)
#define UART_USB_RX_SEL(_baseaddr) (_baseaddr+0xB0)
#define UART_SLEEP_REQ(_baseaddr)       (_baseaddr+0xB4)
#define UART_SLEEP_ACK(_baseaddr)       (_baseaddr+0xB8)
#define UART_SPM_SEL(_baseaddr) (_baseaddr+0xBC)


//IER
#define UART_IER_ERBFI              0x0001
#define UART_IER_ETBEI          0x0002
#define UART_IER_ELSI               0x0004
#define UART_IER_EDSSI              0x0008
#define UART_IER_XOFFI              0x0020
#define UART_IER_RTSI               0x0040
#define UART_IER_CTSI               0x0080
#define IER_HW_NORMALINTS           0x000d
#define IER_HW_ALLINTS              0x000f
#define IER_SW_NORMALINTS           0x002d
#define IER_SW_ALLINTS              0x002f

#define UART_IER_ALLOFF             0x0000
#define UART_IER_VFIFO              0x0001

//FCR
#define UART_FCR_FIFOEN             0x0001
#define UART_FCR_CLRR           0x0002
#define UART_FCR_CLRT           0x0004
#define UART_FCR_FIFOINI        (UART_FCR_FIFOEN | UART_FCR_CLRR | UART_FCR_CLRT)
#define UART_FCR_RX1Byte_Level      0x0000
#define UART_FCR_RX6Byte_Level      0x0040
#define UART_FCR_RX12Byte_Level     0x0080
#define UART_FCR_RX16Byte_Level     0x00c0

#define UART_FCR_TX1Byte_Level      0x0000
#define UART_FCR_TX4Byte_Level      0x0010
#define UART_FCR_TX8Byte_Level      0x0020
#define UART_FCR_TX14Byte_Level     0x0030
#define UART_FCR_NORMAL_TRIG        (UART_FCR_TX1Byte_Level | UART_FCR_RX12Byte_Level | UART_FCR_FIFOINI)
#define UART_FCR_NORMAL             (UART_FCR_TX1Byte_Level | UART_FCR_RX12Byte_Level | UART_FCR_FIFOINI)
#define UART_TX_THRESHOLD_SETTING   1   //should same as TX level at UART_FCR_NORMAL_TRIG

//IIR,RO
#define UART_IIR_INT_INVALID        0x0001
#define UART_IIR_RLS                0x0006  // Receiver Line Status
#define UART_IIR_RDA                0x0004  // Receive Data Available
#define UART_IIR_CTI                0x000C  // Character Timeout Indicator
#define UART_IIR_THRE               0x0002  // Transmit Holding Register Empty
#define UART_IIR_MS                 0x0000  // Check Modem Status Register
#define UART_IIR_SWFlowCtrl         0x0010  // Receive XOFF characters
#define UART_IIR_HWFlowCtrl         0x0020  // CTS or RTS Rising Edge
#define UART_IIR_FIFOS_ENABLED      0x00c0
#define UART_IIR_NO_INTERRUPT_PENDING   0x0001
#define UART_IIR_INT_MASK           0x003f

//===============================LCR================================
//WLS
#define UART_WLS_8                  0x0003
#define UART_WLS_7                  0x0002
#define UART_WLS_6                  0x0001
#define UART_WLS_5                  0x0000
#define UART_DATA_MASK              0x0003

//Parity
#define UART_NONE_PARITY            0x0000
#define UART_ODD_PARITY             0x0008
#define UART_EVEN_PARITY            0x0018
#define UART_MARK_PARITY            0x0028
#define UART_SPACE_PARITY           0x0038
#define UART_PARITY_MASK            0x0038

//Stop bits
#define UART_1_STOP                 0x0000
#define UART_1_5_STOP               0x0004  // Only valid for 5 data bits
#define UART_2_STOP                 0x0004
#define UART_STOP_MASK              0x0004

#define UART_LCR_DLAB               0x0080
//===============================LCR================================


//MCR
#define UART_MCR_DTR                0x0001
#define UART_MCR_RTS                0x0002
#define UART_MCR_LOOPB              0x0010
#define UART_MCR_IRE                0x0040  //Enable IrDA modulation/demodulation
#define UART_MCR_XOFF               0x0080
#define UART_MCR_Normal             (UART_MCR_DTR | UART_MCR_RTS)

//LSR
#define UART_LSR_DR                 0x0001
#define UART_LSR_OE                 0x0002
#define UART_LSR_PE                 0x0004
#define UART_LSR_FE                 0x0008
#define UART_LSR_BI                 0x0010
#define UART_LSR_THRE               0x0020
#define UART_LSR_TEMT               0x0040
#define UART_LSR_FIFOERR            0x0080

//MSR
#define UART_MSR_DCTS               0x0001
#define UART_MSR_DDSR               0x0002
#define UART_MSR_TERI               0x0004
#define UART_MSR_DDCD               0x0008
#define UART_MSR_CTS                0x0010
#define UART_MSR_DSR                0x0020
#define UART_MSR_RI                 0x0040
#define UART_MSR_DCD                0x0080

//DLL
//DLM
//EFR
#define UART_EFR_AutoCTS            0x0080
#define UART_EFR_AutoRTS            0x0040
#define UART_EFR_Enchance           0x0010
#define UART_EFR_SWCtrlMask         0x000f
#define UART_EFR_NoSWFlowCtrl       0x0000
#define UART_EFR_ALLOFF             0x0000
#define UART_EFR_AutoRTSCTS         0x00c0

//Tx/Rx XON1/Xoff1 as flow control word
#define UART_EFR_SWFlowCtrlX1       0x000a

//Tx/Rx XON2/Xoff2 as flow control word
#define UART_EFR_SWFlowCtrlX2       0x0005

//Tx/Rx XON1&XON2/Xoff1&Xoff2 as flow control word
#define UART_EFR_SWFlowCtrlXAll     0x000f

#define UART_RATEFIX_EN             0x0001
#define UART_RATEFIX_AUTOBAUD_EN    0x0002
#define UART_RATEFIX_FREQ_13        0x0004

#define UART_AUTOBAUD_EN                0x1
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

#endif /* #ifndef __MTK_CTP_UART_HW_H__ */
