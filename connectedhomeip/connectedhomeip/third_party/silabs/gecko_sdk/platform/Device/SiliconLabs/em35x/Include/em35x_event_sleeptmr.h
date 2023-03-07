/**************************************************************************//**
* @file
* @brief em35x_event_sleeptmr Register and Bit Field definitions
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

#ifndef EM35X_EVENT_SLEEPTMR_H
#define EM35X_EVENT_SLEEPTMR_H

/**************************************************************************//**
 * @defgroup EM35X_EVENT_SLEEPTMR
 * @{
 * @brief EM35X_EVENT_SLEEPTMR Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[2];
  __IOM uint32_t FORCE;
  uint32_t RESERVED1[12];
  __IOM uint32_t CFG;
} EVENT_SLEEPTMR_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_EVENT_SLEEPTMR_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_SLEEPTMR FLAG */
#define _EVENT_SLEEPTMR_FLAG_RESETVALUE      0x00000000UL
#define _EVENT_SLEEPTMR_FLAG_MASK            0x00000007UL
#define EVENT_SLEEPTMR_FLAG_CMPB             (0x1UL << 2)
#define _EVENT_SLEEPTMR_FLAG_CMPB_SHIFT      2
#define _EVENT_SLEEPTMR_FLAG_CMPB_MASK       0x4UL
#define _EVENT_SLEEPTMR_FLAG_CMPB_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_FLAG_CMPB_DEFAULT     (_EVENT_SLEEPTMR_FLAG_CMPB_DEFAULT << 2)
#define EVENT_SLEEPTMR_FLAG_CMPA             (0x1UL << 1)
#define _EVENT_SLEEPTMR_FLAG_CMPA_SHIFT      1
#define _EVENT_SLEEPTMR_FLAG_CMPA_MASK       0x2UL
#define _EVENT_SLEEPTMR_FLAG_CMPA_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_FLAG_CMPA_DEFAULT     (_EVENT_SLEEPTMR_FLAG_CMPA_DEFAULT << 1)
#define EVENT_SLEEPTMR_FLAG_WRAP             (0x1UL << 0)
#define _EVENT_SLEEPTMR_FLAG_WRAP_SHIFT      0
#define _EVENT_SLEEPTMR_FLAG_WRAP_MASK       0x1UL
#define _EVENT_SLEEPTMR_FLAG_WRAP_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_FLAG_WRAP_DEFAULT     (_EVENT_SLEEPTMR_FLAG_WRAP_DEFAULT << 0)

/* Bit fields for EVENT_SLEEPTMR FORCE */
#define _EVENT_SLEEPTMR_FORCE_RESETVALUE      0x00000000UL
#define _EVENT_SLEEPTMR_FORCE_MASK            0x00000007UL
#define EVENT_SLEEPTMR_FORCE_CMPB             (0x1UL << 2)
#define _EVENT_SLEEPTMR_FORCE_CMPB_SHIFT      2
#define _EVENT_SLEEPTMR_FORCE_CMPB_MASK       0x4UL
#define _EVENT_SLEEPTMR_FORCE_CMPB_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_FORCE_CMPB_DEFAULT     (_EVENT_SLEEPTMR_FORCE_CMPB_DEFAULT << 2)
#define EVENT_SLEEPTMR_FORCE_CMPA             (0x1UL << 1)
#define _EVENT_SLEEPTMR_FORCE_CMPA_SHIFT      1
#define _EVENT_SLEEPTMR_FORCE_CMPA_MASK       0x2UL
#define _EVENT_SLEEPTMR_FORCE_CMPA_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_FORCE_CMPA_DEFAULT     (_EVENT_SLEEPTMR_FORCE_CMPA_DEFAULT << 1)
#define EVENT_SLEEPTMR_FORCE_WRAP             (0x1UL << 0)
#define _EVENT_SLEEPTMR_FORCE_WRAP_SHIFT      0
#define _EVENT_SLEEPTMR_FORCE_WRAP_MASK       0x1UL
#define _EVENT_SLEEPTMR_FORCE_WRAP_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_FORCE_WRAP_DEFAULT     (_EVENT_SLEEPTMR_FORCE_WRAP_DEFAULT << 0)

/* Bit fields for EVENT_SLEEPTMR CFG */
#define _EVENT_SLEEPTMR_CFG_RESETVALUE      0x00000000UL
#define _EVENT_SLEEPTMR_CFG_MASK            0x00000007UL
#define EVENT_SLEEPTMR_CFG_CMPB             (0x1UL << 2)
#define _EVENT_SLEEPTMR_CFG_CMPB_SHIFT      2
#define _EVENT_SLEEPTMR_CFG_CMPB_MASK       0x4UL
#define _EVENT_SLEEPTMR_CFG_CMPB_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_CFG_CMPB_DEFAULT     (_EVENT_SLEEPTMR_CFG_CMPB_DEFAULT << 2)
#define EVENT_SLEEPTMR_CFG_CMPA             (0x1UL << 1)
#define _EVENT_SLEEPTMR_CFG_CMPA_SHIFT      1
#define _EVENT_SLEEPTMR_CFG_CMPA_MASK       0x2UL
#define _EVENT_SLEEPTMR_CFG_CMPA_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_CFG_CMPA_DEFAULT     (_EVENT_SLEEPTMR_CFG_CMPA_DEFAULT << 1)
#define EVENT_SLEEPTMR_CFG_WRAP             (0x1UL << 0)
#define _EVENT_SLEEPTMR_CFG_WRAP_SHIFT      0
#define _EVENT_SLEEPTMR_CFG_WRAP_MASK       0x1UL
#define _EVENT_SLEEPTMR_CFG_WRAP_DEFAULT    0x00000000UL
#define EVENT_SLEEPTMR_CFG_WRAP_DEFAULT     (_EVENT_SLEEPTMR_CFG_WRAP_DEFAULT << 0)

/** @} End of group EM35X_EVENT_SLEEPTMR_BitFields */

#endif // EM35X_EVENT_SLEEPTMR_H
