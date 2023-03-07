/**************************************************************************//**
* @file
* @brief sky66107_event_nmi Register and Bit Field definitions
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

#ifndef SKY66107_EVENT_NMI_H
#define SKY66107_EVENT_NMI_H

/**************************************************************************//**
 * @defgroup SKY66107_EVENT_NMI
 * @{
 * @brief SKY66107_EVENT_NMI Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[1];
  __IOM uint32_t TESTFORCEALL;
} EVENT_NMI_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup SKY66107_EVENT_NMI_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_NMI FLAG */
#define _EVENT_NMI_FLAG_RESETVALUE        0x00000000UL
#define _EVENT_NMI_FLAG_MASK              0x00000003UL
#define EVENT_NMI_FLAG_CLK24M             (0x1UL << 1)
#define _EVENT_NMI_FLAG_CLK24M_SHIFT      1
#define _EVENT_NMI_FLAG_CLK24M_MASK       0x2UL
#define _EVENT_NMI_FLAG_CLK24M_DEFAULT    0x00000000UL
#define EVENT_NMI_FLAG_CLK24M_DEFAULT     (_EVENT_NMI_FLAG_CLK24M_DEFAULT << 1)
#define EVENT_NMI_FLAG_WDOG               (0x1UL << 0)
#define _EVENT_NMI_FLAG_WDOG_SHIFT        0
#define _EVENT_NMI_FLAG_WDOG_MASK         0x1UL
#define _EVENT_NMI_FLAG_WDOG_DEFAULT      0x00000000UL
#define EVENT_NMI_FLAG_WDOG_DEFAULT       (_EVENT_NMI_FLAG_WDOG_DEFAULT << 0)

/* Bit fields for EVENT_NMI TESTFORCEALL */
#define _EVENT_NMI_TESTFORCEALL_RESETVALUE             0x00000000UL
#define _EVENT_NMI_TESTFORCEALL_MASK                   0x00000001UL
#define _EVENT_NMI_TESTFORCEALL_FORCEALLINT_SHIFT      0
#define _EVENT_NMI_TESTFORCEALL_FORCEALLINT_MASK       0x1UL
#define _EVENT_NMI_TESTFORCEALL_FORCEALLINT_DEFAULT    0x00000000UL
#define EVENT_NMI_TESTFORCEALL_FORCEALLINT_DEFAULT     (_EVENT_NMI_TESTFORCEALL_FORCEALLINT_DEFAULT << 0)

/** @} End of group SKY66107_EVENT_NMI_BitFields */

#endif // SKY66107_EVENT_NMI_H
