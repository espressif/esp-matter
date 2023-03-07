/**************************************************************************//**
* @file
* @brief em35x_wdog Register and Bit Field definitions
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

#ifndef EM35X_WDOG_H
#define EM35X_WDOG_H

/**************************************************************************//**
 * @defgroup EM35X_WDOG
 * @{
 * @brief EM35X_WDOG Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t CFG;
  __IOM uint32_t KEY;
  __IOM uint32_t RESET;
} WDOG_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_WDOG_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for WDOG CFG */
#define _WDOG_CFG_RESETVALUE         0x00000002UL
#define _WDOG_CFG_MASK               0x00000003UL
#define WDOG_CFG_DISABLE             (0x1UL << 1)
#define _WDOG_CFG_DISABLE_SHIFT      1
#define _WDOG_CFG_DISABLE_MASK       0x2UL
#define _WDOG_CFG_DISABLE_DEFAULT    0x00000001UL
#define WDOG_CFG_DISABLE_DEFAULT     (_WDOG_CFG_DISABLE_DEFAULT << 1)
#define WDOG_CFG_ENABLE              (0x1UL << 0)
#define _WDOG_CFG_ENABLE_SHIFT       0
#define _WDOG_CFG_ENABLE_MASK        0x1UL
#define _WDOG_CFG_ENABLE_DEFAULT     0x00000000UL
#define WDOG_CFG_ENABLE_DEFAULT      (_WDOG_CFG_ENABLE_DEFAULT << 0)

/* Bit fields for WDOG KEY */
#define _WDOG_KEY_RESETVALUE           0x00000000UL
#define _WDOG_KEY_MASK                 0x0000FFFFUL
#define _WDOG_KEY_KEY_FIELD_SHIFT      0
#define _WDOG_KEY_KEY_FIELD_MASK       0xFFFFUL
#define _WDOG_KEY_KEY_FIELD_DEFAULT    0x00000000UL
#define WDOG_KEY_KEY_FIELD_DEFAULT     (_WDOG_KEY_KEY_FIELD_DEFAULT << 0)

/* Bit fields for WDOG RESET */
#define _WDOG_RESET_RESETVALUE    0x00000000UL
#define _WDOG_RESET_MASK          0x00000000UL

/** @} End of group EM35X_WDOG_BitFields */

#endif // EM35X_WDOG_H
