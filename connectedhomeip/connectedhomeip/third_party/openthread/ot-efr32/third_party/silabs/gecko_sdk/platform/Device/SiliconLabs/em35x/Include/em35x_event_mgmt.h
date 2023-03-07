/**************************************************************************//**
* @file
* @brief em35x_event_mgmt Register and Bit Field definitions
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

#ifndef EM35X_EVENT_MGMT_H
#define EM35X_EVENT_MGMT_H

/**************************************************************************//**
 * @defgroup EM35X_EVENT_MGMT
 * @{
 * @brief EM35X_EVENT_MGMT Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[15];
  __IOM uint32_t CFG;
} EVENT_MGMT_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_EVENT_MGMT_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_MGMT FLAG */
#define _EVENT_MGMT_FLAG_RESETVALUE          0x00000000UL
#define _EVENT_MGMT_FLAG_MASK                0x0000001FUL
#define EVENT_MGMT_FLAG_DMAPROT              (0x1UL << 4)
#define _EVENT_MGMT_FLAG_DMAPROT_SHIFT       4
#define _EVENT_MGMT_FLAG_DMAPROT_MASK        0x10UL
#define _EVENT_MGMT_FLAG_DMAPROT_DEFAULT     0x00000000UL
#define EVENT_MGMT_FLAG_DMAPROT_DEFAULT      (_EVENT_MGMT_FLAG_DMAPROT_DEFAULT << 4)
#define EVENT_MGMT_FLAG_CALADC               (0x1UL << 3)
#define _EVENT_MGMT_FLAG_CALADC_SHIFT        3
#define _EVENT_MGMT_FLAG_CALADC_MASK         0x8UL
#define _EVENT_MGMT_FLAG_CALADC_DEFAULT      0x00000000UL
#define EVENT_MGMT_FLAG_CALADC_DEFAULT       (_EVENT_MGMT_FLAG_CALADC_DEFAULT << 3)
#define EVENT_MGMT_FLAG_FPEC                 (0x1UL << 2)
#define _EVENT_MGMT_FLAG_FPEC_SHIFT          2
#define _EVENT_MGMT_FLAG_FPEC_MASK           0x4UL
#define _EVENT_MGMT_FLAG_FPEC_DEFAULT        0x00000000UL
#define EVENT_MGMT_FLAG_FPEC_DEFAULT         (_EVENT_MGMT_FLAG_FPEC_DEFAULT << 2)
#define EVENT_MGMT_FLAG_OSC24MHI             (0x1UL << 1)
#define _EVENT_MGMT_FLAG_OSC24MHI_SHIFT      1
#define _EVENT_MGMT_FLAG_OSC24MHI_MASK       0x2UL
#define _EVENT_MGMT_FLAG_OSC24MHI_DEFAULT    0x00000000UL
#define EVENT_MGMT_FLAG_OSC24MHI_DEFAULT     (_EVENT_MGMT_FLAG_OSC24MHI_DEFAULT << 1)
#define EVENT_MGMT_FLAG_OSC24MLO             (0x1UL << 0)
#define _EVENT_MGMT_FLAG_OSC24MLO_SHIFT      0
#define _EVENT_MGMT_FLAG_OSC24MLO_MASK       0x1UL
#define _EVENT_MGMT_FLAG_OSC24MLO_DEFAULT    0x00000000UL
#define EVENT_MGMT_FLAG_OSC24MLO_DEFAULT     (_EVENT_MGMT_FLAG_OSC24MLO_DEFAULT << 0)

/* Bit fields for EVENT_MGMT CFG */
#define _EVENT_MGMT_CFG_RESETVALUE          0x00000000UL
#define _EVENT_MGMT_CFG_MASK                0x0000001FUL
#define EVENT_MGMT_CFG_DMAPROT              (0x1UL << 4)
#define _EVENT_MGMT_CFG_DMAPROT_SHIFT       4
#define _EVENT_MGMT_CFG_DMAPROT_MASK        0x10UL
#define _EVENT_MGMT_CFG_DMAPROT_DEFAULT     0x00000000UL
#define EVENT_MGMT_CFG_DMAPROT_DEFAULT      (_EVENT_MGMT_CFG_DMAPROT_DEFAULT << 4)
#define EVENT_MGMT_CFG_CALADC               (0x1UL << 3)
#define _EVENT_MGMT_CFG_CALADC_SHIFT        3
#define _EVENT_MGMT_CFG_CALADC_MASK         0x8UL
#define _EVENT_MGMT_CFG_CALADC_DEFAULT      0x00000000UL
#define EVENT_MGMT_CFG_CALADC_DEFAULT       (_EVENT_MGMT_CFG_CALADC_DEFAULT << 3)
#define EVENT_MGMT_CFG_FPEC                 (0x1UL << 2)
#define _EVENT_MGMT_CFG_FPEC_SHIFT          2
#define _EVENT_MGMT_CFG_FPEC_MASK           0x4UL
#define _EVENT_MGMT_CFG_FPEC_DEFAULT        0x00000000UL
#define EVENT_MGMT_CFG_FPEC_DEFAULT         (_EVENT_MGMT_CFG_FPEC_DEFAULT << 2)
#define EVENT_MGMT_CFG_OSC24MHI             (0x1UL << 1)
#define _EVENT_MGMT_CFG_OSC24MHI_SHIFT      1
#define _EVENT_MGMT_CFG_OSC24MHI_MASK       0x2UL
#define _EVENT_MGMT_CFG_OSC24MHI_DEFAULT    0x00000000UL
#define EVENT_MGMT_CFG_OSC24MHI_DEFAULT     (_EVENT_MGMT_CFG_OSC24MHI_DEFAULT << 1)
#define EVENT_MGMT_CFG_OSC24MLO             (0x1UL << 0)
#define _EVENT_MGMT_CFG_OSC24MLO_SHIFT      0
#define _EVENT_MGMT_CFG_OSC24MLO_MASK       0x1UL
#define _EVENT_MGMT_CFG_OSC24MLO_DEFAULT    0x00000000UL
#define EVENT_MGMT_CFG_OSC24MLO_DEFAULT     (_EVENT_MGMT_CFG_OSC24MLO_DEFAULT << 0)

/** @} End of group EM35X_EVENT_MGMT_BitFields */

#endif // EM35X_EVENT_MGMT_H
