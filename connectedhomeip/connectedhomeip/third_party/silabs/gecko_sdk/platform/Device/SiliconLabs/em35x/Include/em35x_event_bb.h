/**************************************************************************//**
* @file
* @brief em35x_event_bb Register and Bit Field definitions
* @version 5.8.3
******************************************************************************
* @section License
* <b>(C) Copyright 2014 Silicon Labs, www.silabs.com</b>
*******************************************************************************
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
* obligation to support this Software. Silicon Labs is providing the
* Software "AS IS", with no express or implied warranties of any kind,
* including, but not limited to, any implied warranties of merchantability
* or fitness for any particular purpose or warranties against infringement
* of any proprietary rights of a third party.
*
* Silicon Labs will not be liable for any consequential, incidental, or
* special damages, or any other relief, or for any claim by any third party,
* arising from your use of this Software.
*
******************************************************************************/

#ifndef EM35X_EVENT_BB_H
#define EM35X_EVENT_BB_H

/**************************************************************************//**
 * @defgroup EM35X_EVENT_BB
 * @{
 * @brief EM35X_EVENT_BB Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t SRC;
  uint32_t RESERVED0[15];
  __IOM uint32_t MASK;
} EVENT_BB_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_EVENT_BB_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_BB SRC */
#define _EVENT_BB_SRC_RESETVALUE          0x00000000UL
#define _EVENT_BB_SRC_MASK                0x00000003UL
#define EVENT_BB_SRC_RSSI                 (0x1UL << 1)
#define _EVENT_BB_SRC_RSSI_SHIFT          1
#define _EVENT_BB_SRC_RSSI_MASK           0x2UL
#define _EVENT_BB_SRC_RSSI_DEFAULT        0x00000000UL
#define EVENT_BB_SRC_RSSI_DEFAULT         (_EVENT_BB_SRC_RSSI_DEFAULT << 1)
#define EVENT_BB_SRC_BASEBAND             (0x1UL << 0)
#define _EVENT_BB_SRC_BASEBAND_SHIFT      0
#define _EVENT_BB_SRC_BASEBAND_MASK       0x1UL
#define _EVENT_BB_SRC_BASEBAND_DEFAULT    0x00000000UL
#define EVENT_BB_SRC_BASEBAND_DEFAULT     (_EVENT_BB_SRC_BASEBAND_DEFAULT << 0)

/* Bit fields for EVENT_BB MASK */
#define _EVENT_BB_MASK_RESETVALUE          0x00000000UL
#define _EVENT_BB_MASK_MASK                0x00000003UL
#define EVENT_BB_MASK_RSSI                 (0x1UL << 1)
#define _EVENT_BB_MASK_RSSI_SHIFT          1
#define _EVENT_BB_MASK_RSSI_MASK           0x2UL
#define _EVENT_BB_MASK_RSSI_DEFAULT        0x00000000UL
#define EVENT_BB_MASK_RSSI_DEFAULT         (_EVENT_BB_MASK_RSSI_DEFAULT << 1)
#define EVENT_BB_MASK_BASEBAND             (0x1UL << 0)
#define _EVENT_BB_MASK_BASEBAND_SHIFT      0
#define _EVENT_BB_MASK_BASEBAND_MASK       0x1UL
#define _EVENT_BB_MASK_BASEBAND_DEFAULT    0x00000000UL
#define EVENT_BB_MASK_BASEBAND_DEFAULT     (_EVENT_BB_MASK_BASEBAND_DEFAULT << 0)

/** @} End of group EM35X_EVENT_BB_BitFields */

#endif // EM35X_EVENT_BB_H
