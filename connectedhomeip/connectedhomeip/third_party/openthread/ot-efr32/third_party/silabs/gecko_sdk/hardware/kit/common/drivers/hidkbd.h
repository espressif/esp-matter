/***************************************************************************//**
 * @file
 * @brief USB Human Interface Devices (HID) class keyboard driver.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef __SILICON_LABS_HIDKBD_H__
#define __SILICON_LABS_HIDKBD_H__

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup HidKeyboard
 * @{
 ******************************************************************************/

#include <stdint.h>

#include "em_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/** HID keyboard input report definition. */
SL_PACK_START(1)
typedef struct {
  uint8_t modifier; /**< Key modifier byte as defined in HID spec. section "8.3  Report Format for Array Items". */
  uint8_t reserved; /**< Reserved, should be set to zero. */
  uint8_t key[6];   /**< Array of 6 key array indices, key index codes are defined in the "Keyboard page" section
                         of "USB HID Usage Tables document", also refer to usbkbdscancodes.c. */
} SL_ATTRIBUTE_PACKED HIDKBD_KeyReport_t;
SL_PACK_END()

/**************************************************************************//**
 * @brief
 *   Callback function pointer for HID output reports.
 *   This function will be called by the driver each time an output report is
 *   received by the device.
 *
 * @param[in] report Output report byte.
 *                   @n Bit 0 : State of keyboard NumLock LED.
 *                   @n Bit 1 : State of keyboard CapsLock LED.
 *                   @n Bit 2 : State of keyboard ScrollLock LED.
 *****************************************************************************/
typedef void (*HIDKBD_SetReportFunc_t)(uint8_t report);

/** HidKeyboard driver initialization structure.
 *  This data structure contains configuration options that the driver
 *  needs. The structure must be passed to @ref HIDKBD_Init() when initializing
 *  the driver.
 */
typedef struct {
  void                    *hidDescriptor; /**< Pointer to the HID class descriptor in the user application. */
  HIDKBD_SetReportFunc_t  setReportFunc;  /**< Callback function pointer for HID output reports, may be NULL when no callback is needed. */
} HIDKBD_Init_t;

extern const char HIDKBD_ReportDescriptor[69];

void HIDKBD_Init(HIDKBD_Init_t *init);
int  HIDKBD_SetupCmd(const USB_Setup_TypeDef *setup);
void HIDKBD_StateChangeEvent(USBD_State_TypeDef oldState,
                             USBD_State_TypeDef newState);
void HIDKBD_KeyboardEvent(HIDKBD_KeyReport_t *report);

#ifdef __cplusplus
}
#endif

/** @} (end group HidKeyboard) */
/** @} (end group kitdrv) */

#endif /* __SILICON_LABS_HIDKBD_H__ */
