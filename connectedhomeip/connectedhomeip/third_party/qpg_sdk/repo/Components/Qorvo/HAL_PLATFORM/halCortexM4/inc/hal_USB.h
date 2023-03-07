/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the USB interface.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _HAL_USB_H_
#define _HAL_USB_H_

/*****************************************************************************
 *                    USB
 *****************************************************************************/


typedef void (* hal_cbUSBTxDone_t) (UInt16);
typedef void (* hal_cbUSBRxDone_t) (UInt8* data, UInt16 length);

#ifdef __cplusplus
extern "C" {
#endif

void hal_USBDisable();
void hal_USBEnable();
void hal_USBTxNewData(UInt8 endpoint);

void hal_USBComStart(hal_cbUSBRxDone_t cbRx, hal_cbUSBTxDone_t cbTx, UInt8 endpoint);
void hal_USBComReStart(void);
//void hal_USBComFlush(UInt8 endpoint);
Bool hal_USBTxEnabled();
Bool hal_USBRxEnabled();
//void hal_USBRxComFlush(UInt8 endpoint);

//#define HAVE_HAL_USB_FLUSH
//#define HAVE_HAL_USB_RX_FLUSH

UInt16 hal_USBTransmitBlock(UInt16 length, UInt8* data);

/*****************************************************************************
 *                    ucHal UART interface
 *****************************************************************************/

#define HAL_USB_WMAXPACKETSIZE (64)
#define HAL_USB_EP_DATA                          0

#define HAL_USB_COM_START(cbRx, cbTx) do { \
    hal_USBComStart(cbRx , cbTx, HAL_USB_EP_DATA); \
} while(false)

#define HAL_USB_COM_POWERDOWN()              hal_USBDisable()
#define HAL_USB_COM_TX_ENABLED()             hal_USBTxEnabled()
#define HAL_USB_COM_TX_NEW_DATA()            hal_USBTxNewData(HAL_USB_EP_DATA)

void hal_cbUSBComReady(void);
void hal_InitUSB(void);
void hal_USBFlush(void);
#ifdef __cplusplus
}
#endif

#endif //_HAL_USB_H_
