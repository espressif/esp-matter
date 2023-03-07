/***************************************************************************//**
 * @file
 * @brief   Primary header file. Contains internal global declarations and
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef EM_USBXPRESS_INTERNAL_H
#define EM_USBXPRESS_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include "em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Macro Definitions

#define USBXPRESS_LIBRARY_VERSION      0x0500   /**< USBXpress firmware library
                                                   version Example: 0x0241 =>
                                                   v2.41 */

// Define USBXCORE_apiEa bit masks
#define  APIEA_GIE                     0x01     /**< API Global Interrupt
                                                   Enable */
#define  APIEA_GIE_TEMP                0x02     /**< Temporary storage of API
                                                   GIE bit when internally
                                                   disabled */

#define USBXCORE_MAXLEN                19       /**< Maximum length of array
                                                   passed by
                                                   USBXCORE_Read/Write */

// Control command requests and values
#define SI_USBXPRESS_REQUEST            0x02    //!< Request
#define SI_USBXPRESS_FLUSH_BUFFERS      0x0001  //!< Value
#define SI_USBXPRESS_CLEAR_TO_SEND      0x0002  //!< Value
#define SI_USBXPRESS_NOT_CLEAR_TO_SEND  0x0004  //!< Value
#define SI_USBXPRESS_GET_VERSION        0x0008  //!< Value

#define SI_CP210X_REQUEST               0xFF    //!< Request
#define SI_CP210X_GET_PART_NUMBER       0x370B  //!< Value

// USBXpress Part Numbers
#define SI_USBXPRESS_PART_NUMBER_EFM8   0x80    //!< EFM8 USBXpress Part Number
#define SI_USBXPRESS_PART_NUMBER_EFM32  0x81    //!< EFM32 USBXpress Part Number

// -----------------------------------------------------------------------------
// Global Variable Declarations

extern uint32_t USBXCORE_apiIntValue;
extern uint8_t USBXCORE_configDesc[32];
extern uint32_t USBXCORE_apiEa;
extern uint32_t* USBXCORE_byteCountInPtr;
extern uint32_t* USBXCORE_byteCountOutPtr;
extern uint32_t USBXCORE_readSize;
extern uint32_t USBXCORE_writeSize;

//! @cond DOXYGEN_SKIP
// Doxygen can't handle function pointers
extern void (*USBXCORE_apiCallback)(void);
//! @endcond

// -----------------------------------------------------------------------------
// Global Function Declarations

// Internal USBXpress Functions
void USBX_jumpCallback(void);
void USBXCORE_Read(uint8_t *, uint8_t *, uint8_t);
void USBXCORE_Write(uint8_t *, uint8_t *, uint8_t);
void USBXCORE_resetState(void);

// EFM32 USB Library Call-back Functions
void USBX_ResetCb(void);
void USBX_DeviceStateChangeCb(USBD_State_TypeDef oldState,
                              USBD_State_TypeDef newState);
int USBX_SetupCmdCb(const USB_Setup_TypeDef *setup);
int USBX_inXferCompleteCb(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);
int USBX_outXferCompleteCb(USB_Status_TypeDef status, uint32_t xferred, uint32_t remaining);

#ifdef __cplusplus
}
#endif

#endif   // EM_USBXPRESS_INTERNAL_H
