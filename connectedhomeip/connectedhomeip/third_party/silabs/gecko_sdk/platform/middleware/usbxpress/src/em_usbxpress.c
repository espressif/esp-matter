/***************************************************************************//**
 * @file
 * @brief   USBXpress global variable declaration, initialization, and call-back
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

#include <stddef.h>
#include <stdbool.h>
#include "em_usbxpress_internal.h"
#include "em_usbxpress_descriptors.h"
#include "em_usbxpress.h"

// -----------------------------------------------------------------------------
// Variable Definitions

/// Copy of device descriptor, so it can be edited
SL_ALIGN(4)
USB_DeviceDescriptor_TypeDef USBXCORE_deviceDesc SL_ATTRIBUTE_ALIGN(4);

/// Copy of configuration descriptor
SL_ALIGN(4)
uint8_t USBXCORE_configDesc[32] SL_ATTRIBUTE_ALIGN(4);

/// Table of pointers to various string descriptors
USB_StringDescriptor_TypeDef const * USBXCORE_stringDescTable[4];

/// Byte holding the current USB_API interrupts
uint32_t USBXCORE_apiIntValue;

/// Enable or disable status of USB_API interrupts
uint32_t USBXCORE_apiEa;

/// Buffer to hold overflow rx data if a ZLP read returns more than zero bytes.
USBX_BUF(USBXCORE_overflowBuffer, USB_FS_BULK_EP_MAXSIZE);

/// Boolean indicating whether a ZLP read is active.
bool USBXCORE_zlpActive = false;

/// Boolean indicating if data was received while expecting a ZLP.
bool USBXCORE_rxOverflowPacketAvailable = false;

/// Size of Rx Overflow Packet
uint32_t USBXCORE_rxOverflowPacketSize = 0;

//! @cond DOXYGEN_SKIP
/// Pointer to user call-back function
void (*USBXCORE_apiCallback)(void);
//! @endcond

/// Pointer to variable holding number of bytes written
uint32_t* USBXCORE_byteCountInPtr;

/// Pointer to variable holding number of bytes read
uint32_t* USBXCORE_byteCountOutPtr;

/// Number of bytes sent to USBX_blockRead() as numBytes
uint32_t USBXCORE_readSize;

/// Number of bytes sent to USBX_blockWrite() as numBytes
uint32_t USBXCORE_writeSize;

// -----------------------------------------------------------------------------
// Functions

/**************************************************************************//**
 * @brief   User API USB initialization function.
 * @ingroup firmware_api
 *
 * @param   p:
 *   Pointer to USBX_Init_t structure. Not modified by this routine.
 *
 * The use of this initialization function is similar to that of the USBD_Init()
 * function in the EFM32 USB Library.  The primary differences are:
 *    - Parameters are placed in USBX_Init_t structure for code organization.
 *
 *****************************************************************************/
void USBX_init(USBX_Init_t * p)
{
  static const USBD_Callbacks_TypeDef callbacks =
  {
    .usbReset        = USBX_ResetCb,
    .usbStateChange  = USBX_DeviceStateChangeCb,
    .setupCmd        = (USBD_SetupCmdCb_TypeDef) USBX_SetupCmdCb,
    .isSelfPowered   = NULL,
    .sofInt          = NULL
  };

  // Initialization structure for EFM32 USB library
  static const USBD_Init_TypeDef USBXCORE_initStruct =
  {
    .deviceDescriptor    = (USB_DeviceDescriptor_TypeDef*) &USBXCORE_deviceDesc,
    .configDescriptor    = USBXCORE_configDesc,
    .stringDescriptors   = (const void *) &USBXCORE_stringDescTable,
    .numberOfStrings     = 4,
    .callbacks           = &callbacks,
    .bufferingMultiplier = USBXCORE_buffMult,
    .reserved            = 0
  };

  uint32_t i;

  // Initialize USB descriptors from code memory and passed startup parameters
  // Copy Device Descriptor from code space to RAM so that it can be
  // altered by user software.
  for (i = 0; i < 18; i++) {
    *((uint8_t*)&USBXCORE_deviceDesc + i) =
      *((uint8_t*)&USBXCORE_deviceDescInit + i);
  }

  // Copy Configuration Descriptor from code space to RAM
  for (i = 0; i < 32; i++) {
    USBXCORE_configDesc[i] = USBXCORE_configDescInit[i];
  }

  // Load the Vendor and Product ID.
  USBXCORE_deviceDesc.idVendor = p->vendorId;
  USBXCORE_deviceDesc.idProduct = p->productId;

  // Load the string language and descriptors.
  USBXCORE_stringDescTable[0] =
    (USB_StringDescriptor_TypeDef *) &USBXCORE_stringLangDesc;
  USBXCORE_stringDescTable[1] = p->manufacturerString;
  USBXCORE_stringDescTable[2] = p->productString;
  USBXCORE_stringDescTable[3] = p->serialString;

  // Modify default value if user value is less than 0xFA (500 mA).
  if (p->maxPower < 0xFA) {
    USBXCORE_configDesc[8] = (uint8_t)(p->maxPower);
  }

  // Load BCD-coded release number and device power attribute to descriptors.
  USBXCORE_deviceDesc.bcdDevice = p->releaseBcd;
  USBXCORE_configDesc[7] = (uint8_t)(p->powerAttribute);

  // Clear USBXpress API interrupts and interrupt enable.
  // User must explicitly enable via USB_Int_Enable().
  USBXCORE_apiIntValue = 0;
  USBXCORE_apiEa = 0;

  // EFM32 USB Library initialization routine.
  // Last part of USBXpress USBX_init().
  USBD_Init(&USBXCORE_initStruct);
}

void USBX_apiCallbackDisable(void)
{
  // Clear USBXCORE_API_EA.0 to disable  USB API call-back generation.
  USBXCORE_apiEa &= ~APIEA_GIE;
}

void USBX_apiCallbackEnable(USBX_apiCallback_t f)
{
  USBXCORE_apiCallback = f;       // Save API call-back pointer

  USBXCORE_apiEa |= APIEA_GIE;    // Set USBXCORE_API_EA.0 to enable
                                  // USB API call-back generation
}

int USBX_blockRead(uint8_t *block,
                   uint32_t numBytes,
                   uint32_t *countPtr)
{
  uint32_t i;

  USBXCORE_byteCountOutPtr = countPtr;
  *USBXCORE_byteCountOutPtr = 0;

  // If the Rx Overflow Packet has data in it, copy that data to the buffer.
  if (USBXCORE_rxOverflowPacketAvailable) {
    for (i = 0; i < USBXCORE_rxOverflowPacketSize; i++) {
      *block = USBXCORE_overflowBuffer[i];
      block++;
    }

    USBXCORE_rxOverflowPacketAvailable = false;

    // If the amount of data in the overflow queue was less than the requested
    // amount of data, issue a read for the remaining data.
    if (((numBytes - USBXCORE_rxOverflowPacketSize) > 0)
        && (USBXCORE_rxOverflowPacketSize % USB_FS_BULK_EP_MAXSIZE) == 0) {
      *USBXCORE_byteCountOutPtr += USBXCORE_rxOverflowPacketSize;
      USBXCORE_readSize = numBytes;
      numBytes -= USBXCORE_rxOverflowPacketSize;
      USBXCORE_rxOverflowPacketSize = 0;

      return USBD_Read(USBXPRESS_OUT_EP_ADDR,
                       block,
                       numBytes,
                       (USB_XferCompleteCb_TypeDef) USBX_outXferCompleteCb);
    } else {
      i = USBXCORE_rxOverflowPacketSize;
      USBXCORE_rxOverflowPacketSize = 0;
      USBXCORE_readSize = numBytes;

      return USBX_outXferCompleteCb(USB_STATUS_OK, i, 0);
    }
  } else {
    USBXCORE_readSize = numBytes;
    return USBD_Read(USBXPRESS_OUT_EP_ADDR,
                     block,
                     numBytes,
                     (USB_XferCompleteCb_TypeDef) USBX_outXferCompleteCb);
  }
}

int USBX_blockWrite(uint8_t *block,
                    uint32_t numBytes,
                    uint32_t *countPtr)
{
  USBXCORE_byteCountInPtr = countPtr;
  *USBXCORE_byteCountInPtr = 0;
  USBXCORE_writeSize = numBytes;
  return USBD_Write(USBXPRESS_IN_EP_ADDR,
                    block,
                    numBytes,
                    (USB_XferCompleteCb_TypeDef) USBX_inXferCompleteCb);
}

uint32_t USBX_getCallbackSource(void)
{
  uint32_t temp = USBXCORE_apiIntValue;

  USBXCORE_apiIntValue = 0;
  return temp;
}

void USBX_disable(void)
{
  USBD_Stop();
}

uint_least16_t USBX_getLibraryVersion(void)
{
  return USBXPRESS_LIBRARY_VERSION;
}

/**************************************************************************//**
 * @brief   Conditionally jumps to the user call-back routine
 *
 * If the user call-back routine is already in progress, this function does
 * nothing. Otherwise, this function jumps to the call-back defined via
 * USBX_callbackEnable().
 *****************************************************************************/
void USBX_jumpCallback(void)
{
  (*USBXCORE_apiCallback)();
}

/**************************************************************************//**
 * @brief Resets internal USBXpress variables.
 *
 * This function resets the internal USBXpress variables. It should be called
 * after a cancel-type event (USBXpress Close, USB de-configure, FIFO flush,
 * etc.).
 *****************************************************************************/
void USBXCORE_resetState(void)
{
  USBXCORE_readSize = 0;
  USBXCORE_writeSize = 0;
  USBXCORE_zlpActive = false;
  USBXCORE_rxOverflowPacketAvailable = false;
  USBXCORE_rxOverflowPacketSize = 0;
}
