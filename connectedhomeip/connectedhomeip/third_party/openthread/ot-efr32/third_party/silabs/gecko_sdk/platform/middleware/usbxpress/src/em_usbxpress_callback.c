/***************************************************************************//**
 * @file
 * @brief   Contains call-back functions from the EFM32 USB Library.
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
#include "em_usbxpress_internal.h"
#include "em_usbxpress_descriptors.h"
#include "em_usbxpress.h"

// -----------------------------------------------------------------------------
// Variables

/// USBXpress Part Number
SL_ALIGN(4)
static uint8_t usbxpressPartNumber SL_ATTRIBUTE_ALIGN(4) =
  SI_USBXPRESS_PART_NUMBER_EFM32;

/// USBXpress Library Version
SL_ALIGN(4)
static uint8_t usbxpressVersion[2] SL_ATTRIBUTE_ALIGN(4) =
{
  (USBXPRESS_LIBRARY_VERSION & 0xFF),
  ((USBXPRESS_LIBRARY_VERSION & 0xFF00) >> 8)
};

// -----------------------------------------------------------------------------
// Extern Variable Declarations

extern uint8_t USBXCORE_overflowBuffer[];
extern bool USBXCORE_zlpActive;
extern bool USBXCORE_rxOverflowPacketAvailable;
extern uint32_t USBXCORE_rxOverflowPacketSize;

// -----------------------------------------------------------------------------
// Functions

/**************************************************************************//**
 * @brief   USB Reset call-back
 *
 * Jump to user API RESET call-back.
 *
 *****************************************************************************/
void USBX_ResetCb(void)
{
  USBXCORE_apiIntValue = USBX_RESET;
  USBXCORE_resetState();

  if ((USBXCORE_apiEa & APIEA_GIE) && (USBXCORE_apiIntValue)) {
    // Jump to API ISR
    USBX_jumpCallback();
  }
}

/**************************************************************************//**
 * @brief   USB device state change call-back
 *
 * Set new state and jump to user API call-back.
 *
 *****************************************************************************/
void USBX_DeviceStateChangeCb(USBD_State_TypeDef oldState,
                              USBD_State_TypeDef newState)
{
  (void) oldState;    // Suppress compiler warning: unused parameter

  // Entering suspend mode, power internal and external blocks down
  if (newState == USBD_STATE_SUSPENDED) {
    USBXCORE_apiIntValue |= USBX_DEV_SUSPEND;
    USBXCORE_resetState();
  }
  if (newState == USBD_STATE_CONFIGURED) {
    USBXCORE_apiIntValue |= USBX_DEV_CONFIGURED;
  }
  if (newState < USBD_STATE_CONFIGURED) {
    USBXCORE_resetState();
  }

  if ((USBXCORE_apiEa & APIEA_GIE) && (USBXCORE_apiIntValue)) {
    // Call to assembly function to cleanup stack and jump to API ISR
    USBX_jumpCallback();
  }
}

/**************************************************************************//**
 * @brief   USB setup command call-back
 *
 * If the setup command is a vendor request, pass to the USB command request
 * parsing routine and acknowledge.  Otherwise ignore the request.
 *
 *****************************************************************************/
int USBX_SetupCmdCb(const USB_Setup_TypeDef *setup)
{
  USB_Status_TypeDef retval = USB_STATUS_REQ_UNHANDLED;
  uint16_t length;

  // Handle open and close events
  if (setup->Type == USB_SETUP_TYPE_VENDOR) {
    // Look for vendor-specific requests
    switch (setup->bRequest) {
      // Requests directed to a USBXpress Device
      case SI_USBXPRESS_REQUEST:
        switch (setup->wValue) {
          // Flush Buffers
          case SI_USBXPRESS_FLUSH_BUFFERS:
            if (USBXCORE_apiEa & APIEA_GIE) {
              USBXCORE_apiEa &= ~APIEA_GIE;      // Turn off bit 1
              USBXCORE_apiEa |= APIEA_GIE_TEMP;   // Turn on bit 2
            } else {
              USBXCORE_apiEa &= ~APIEA_GIE_TEMP;  // Turn off bit 2
            }

            // Abort the current write transfer.
            // This will flush any data in the FIFO.
            USBD_AbortTransfer(USBXPRESS_IN_EP_ADDR);

            USBXCORE_resetState();

            // Clear all other interrupts, set flush buffer interrupt
            USBXCORE_apiIntValue = USBX_FIFO_PURGE;

            if (USBXCORE_apiEa & APIEA_GIE_TEMP) {
              USBXCORE_apiEa |= APIEA_GIE;
            }
            retval = USB_STATUS_OK;
            break;

          // Enable
          case SI_USBXPRESS_CLEAR_TO_SEND:
            USBXCORE_apiIntValue |= USBX_DEV_OPEN;
            retval = USB_STATUS_OK;
            break;

          // Disable
          case SI_USBXPRESS_NOT_CLEAR_TO_SEND:
            USBXCORE_apiIntValue |= USBX_DEV_CLOSE;
            retval = USB_STATUS_OK;
            break;

          // Get Device Library Version
          case SI_USBXPRESS_GET_VERSION:
            USBD_Write(USBXPRESS_SETUP_EP_ADDR, &usbxpressVersion, 2, NULL);
            retval = USB_STATUS_OK;
            break;
        }
        break;

      // Requests directed to a CP210x Device
      case SI_CP210X_REQUEST:
        // Get Part Number
        if (setup->wValue == SI_CP210X_GET_PART_NUMBER) {
          USBD_Write(USBXPRESS_SETUP_EP_ADDR, &usbxpressPartNumber, 1, NULL);
          retval = USB_STATUS_OK;
        }
        break;
    }
  }

  // Jump to API ISR if a valid command was received.
  if (retval == USB_STATUS_OK) {
    if ((USBXCORE_apiEa & APIEA_GIE) && (USBXCORE_apiIntValue)) {
      // Jump to API ISR
      USBX_jumpCallback();
    }

    return retval;
  }

  // Intercept the Microsoft OS Descriptor Requests
  if (setup->bmRequestType == (USB_SETUP_DIR_D2H
                               | USB_SETUP_TYPE_STANDARD
                               | USB_SETUP_RECIPIENT_DEVICE)) {
    if ((setup->bRequest == GET_DESCRIPTOR)
        && ((setup->wValue >> 8) == USB_STRING_DESCRIPTOR)) {
      if ((setup->wValue & 0xFF) == 0xEE) {
        USBD_Write(USBXPRESS_SETUP_EP_ADDR,
                   (uint8_t *) &USBXCORE_microsoftOsDesc,
                   USBXCORE_microsoftOsDesc[0],
                   NULL);

        retval = USB_STATUS_OK;
      }
    }
  }
  // Vendor specific IN request - Get Windows OS Compatibility ID Descriptor
  else if ((setup->bmRequestType == (USB_SETUP_DIR_D2H
                                     | USB_SETUP_TYPE_VENDOR_MASK
                                     | USB_SETUP_RECIPIENT_DEVICE))
           && (setup->bRequest == EXT_COMP_VENDOR_CODE)
           && (setup->wIndex == FEATURE_EXTENDED_COMPATIBILITY_ID)) {
    length = EXT_COMP_DESC_SIZE;

    if (length > setup->wLength) {
      length = setup->wLength;
    }

    USBD_Write(USBXPRESS_SETUP_EP_ADDR,
               (uint8_t *) &USBXCORE_extendedCompatIdOsFeatureDesc,
               length,
               NULL);
    retval = USB_STATUS_OK;
  }
  // Vendor specific IN request - Get Windows OS Extended Properties Descriptor
  else if ((setup->bmRequestType == (USB_SETUP_DIR_D2H
                                     | USB_SETUP_TYPE_VENDOR_MASK
                                     | USB_SETUP_RECIPIENT_INTERFACE))
           && (setup->bRequest == EXT_COMP_VENDOR_CODE)
           && ((setup->wIndex == FEATURE_EXTENDED_PROPERTIES_ID)
               || (setup->wIndex == 0))) {
    length = PROPERTIES_DESCRIPTOR_SIZE;

    if (length > setup->wLength) {
      length = setup->wLength;
    }

    USBD_Write(USBXPRESS_SETUP_EP_ADDR,
               (uint8_t *) &USBXCORE_extendedPropertiesDesc,
               length,
               NULL);
    retval = USB_STATUS_OK;
  }

  return retval;
}

/**************************************************************************//**
 * @brief   USBXpress IN Endpoint Transfer Complete Callback
 *
 * Gets the number of IN bytes transferred and passes them to the user API
 * call-back.
 *
 *****************************************************************************/
int USBX_inXferCompleteCb(USB_Status_TypeDef status,
                          uint32_t xferred,
                          uint32_t remaining)
{
  (void) remaining;   // Suppress compiler warning: unused parameter

  if (status == USB_STATUS_OK) {
    *USBXCORE_byteCountInPtr += xferred;
    USBXCORE_writeSize -= xferred;

    // If the transfer was a multiple of the maximum packet size, send a ZLP
    // to the host to signal the end of the transfer.
    if (USBXCORE_writeSize == 0) {
      if ((xferred) && (xferred % USB_FS_BULK_EP_MAXSIZE == 0)) {
        USBD_Write(USBXPRESS_IN_EP_ADDR,
                   NULL,
                   0,
                   (USB_XferCompleteCb_TypeDef) USBX_inXferCompleteCb);
      } else {
        // Notify of transmit complete
        USBXCORE_apiIntValue |= USBX_TX_COMPLETE;
      }
    }
  }

  if ((USBXCORE_apiEa & APIEA_GIE) && (USBXCORE_apiIntValue)) {
    // Call to assembly function to cleanup stack and jump to API ISR
    USBX_jumpCallback();
  }

  return 0;
}

/**************************************************************************//**
 * @brief   USBXpress OUT Endpoint Transfer Complete Callback
 *
 * Gets the number of OUT bytes transferred and passes them to the user API
 * call-back.
 *
 *****************************************************************************/
int USBX_outXferCompleteCb(USB_Status_TypeDef status,
                           uint32_t xferred,
                           uint32_t remaining)
{
  (void) remaining;   // Suppress compiler warning: unused parameter

  if (status == USB_STATUS_OK) {
    if (xferred <= USBXCORE_readSize) {
      USBXCORE_readSize -= xferred;
      *USBXCORE_byteCountOutPtr += xferred;
    } else {
      *USBXCORE_byteCountOutPtr += USBXCORE_readSize;
      USBXCORE_readSize = 0;
    }

    // If the total read size is not decremented to zero, the transfer has ended.
    if (USBXCORE_readSize) {
      // Notify of receive complete
      USBXCORE_apiIntValue |= USBX_RX_COMPLETE;
    }
    // If this was a ZLP, mark USBX_RX_COMPLETE and USBX_RX_OVERRUN, if necessary
    else if (USBXCORE_zlpActive) {
      // Notify of receive complete
      USBXCORE_apiIntValue |= USBX_RX_COMPLETE;

      USBXCORE_zlpActive = false;

      // If we received data, notify of receive overrun
      if (xferred > 0) {
        USBXCORE_apiIntValue |= USBX_RX_OVERRUN;
        USBXCORE_rxOverflowPacketAvailable = true;
        USBXCORE_rxOverflowPacketSize = xferred;
      }
    } else {
      USBXCORE_zlpActive = true;
      USBD_Read(USBXPRESS_OUT_EP_ADDR,
                USBXCORE_overflowBuffer,
                0,
                (USB_XferCompleteCb_TypeDef) USBX_outXferCompleteCb);
    }
  }

  if ((USBXCORE_apiEa & APIEA_GIE) && (USBXCORE_apiIntValue)) {
    // Call to assembly function to cleanup stack and jump to API ISR
    USBX_jumpCallback();
  }

  return 0;
}
