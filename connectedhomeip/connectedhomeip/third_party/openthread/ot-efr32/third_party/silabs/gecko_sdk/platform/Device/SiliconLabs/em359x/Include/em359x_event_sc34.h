/**************************************************************************//**
* @file
* @brief em359x_event_sc34 Register and Bit Field definitions
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

#ifndef EM359X_EVENT_SC34_H
#define EM359X_EVENT_SC34_H

/**************************************************************************//**
 * @defgroup EM359X_EVENT_SC34
 * @{
 * @brief EM359X_EVENT_SC34 Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[1];
  __IOM uint32_t CFG;
  uint32_t RESERVED1[1];
  __IOM uint32_t INTMODE;
} EVENT_SC34_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM359X_EVENT_SC34_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_SC34 FLAG */
#define _EVENT_SC34_FLAG_RESETVALUE        0x00000000UL
#define _EVENT_SC34_FLAG_MASK              0x00007FFFUL
#define EVENT_SC34_FLAG_PARERR             (0x1UL << 14)
#define _EVENT_SC34_FLAG_PARERR_SHIFT      14
#define _EVENT_SC34_FLAG_PARERR_MASK       0x4000UL
#define _EVENT_SC34_FLAG_PARERR_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_PARERR_DEFAULT     (_EVENT_SC34_FLAG_PARERR_DEFAULT << 14)
#define EVENT_SC34_FLAG_FRMERR             (0x1UL << 13)
#define _EVENT_SC34_FLAG_FRMERR_SHIFT      13
#define _EVENT_SC34_FLAG_FRMERR_MASK       0x2000UL
#define _EVENT_SC34_FLAG_FRMERR_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_FRMERR_DEFAULT     (_EVENT_SC34_FLAG_FRMERR_DEFAULT << 13)
#define EVENT_SC34_FLAG_TXULDB             (0x1UL << 12)
#define _EVENT_SC34_FLAG_TXULDB_SHIFT      12
#define _EVENT_SC34_FLAG_TXULDB_MASK       0x1000UL
#define _EVENT_SC34_FLAG_TXULDB_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_TXULDB_DEFAULT     (_EVENT_SC34_FLAG_TXULDB_DEFAULT << 12)
#define EVENT_SC34_FLAG_TXULDA             (0x1UL << 11)
#define _EVENT_SC34_FLAG_TXULDA_SHIFT      11
#define _EVENT_SC34_FLAG_TXULDA_MASK       0x800UL
#define _EVENT_SC34_FLAG_TXULDA_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_TXULDA_DEFAULT     (_EVENT_SC34_FLAG_TXULDA_DEFAULT << 11)
#define EVENT_SC34_FLAG_RXULDB             (0x1UL << 10)
#define _EVENT_SC34_FLAG_RXULDB_SHIFT      10
#define _EVENT_SC34_FLAG_RXULDB_MASK       0x400UL
#define _EVENT_SC34_FLAG_RXULDB_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_RXULDB_DEFAULT     (_EVENT_SC34_FLAG_RXULDB_DEFAULT << 10)
#define EVENT_SC34_FLAG_RXULDA             (0x1UL << 9)
#define _EVENT_SC34_FLAG_RXULDA_SHIFT      9
#define _EVENT_SC34_FLAG_RXULDA_MASK       0x200UL
#define _EVENT_SC34_FLAG_RXULDA_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_RXULDA_DEFAULT     (_EVENT_SC34_FLAG_RXULDA_DEFAULT << 9)
#define EVENT_SC34_FLAG_NAK                (0x1UL << 8)
#define _EVENT_SC34_FLAG_NAK_SHIFT         8
#define _EVENT_SC34_FLAG_NAK_MASK          0x100UL
#define _EVENT_SC34_FLAG_NAK_DEFAULT       0x00000000UL
#define EVENT_SC34_FLAG_NAK_DEFAULT        (_EVENT_SC34_FLAG_NAK_DEFAULT << 8)
#define EVENT_SC34_FLAG_CMDFIN             (0x1UL << 7)
#define _EVENT_SC34_FLAG_CMDFIN_SHIFT      7
#define _EVENT_SC34_FLAG_CMDFIN_MASK       0x80UL
#define _EVENT_SC34_FLAG_CMDFIN_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_CMDFIN_DEFAULT     (_EVENT_SC34_FLAG_CMDFIN_DEFAULT << 7)
#define EVENT_SC34_FLAG_TXFIN              (0x1UL << 6)
#define _EVENT_SC34_FLAG_TXFIN_SHIFT       6
#define _EVENT_SC34_FLAG_TXFIN_MASK        0x40UL
#define _EVENT_SC34_FLAG_TXFIN_DEFAULT     0x00000000UL
#define EVENT_SC34_FLAG_TXFIN_DEFAULT      (_EVENT_SC34_FLAG_TXFIN_DEFAULT << 6)
#define EVENT_SC34_FLAG_RXFIN              (0x1UL << 5)
#define _EVENT_SC34_FLAG_RXFIN_SHIFT       5
#define _EVENT_SC34_FLAG_RXFIN_MASK        0x20UL
#define _EVENT_SC34_FLAG_RXFIN_DEFAULT     0x00000000UL
#define EVENT_SC34_FLAG_RXFIN_DEFAULT      (_EVENT_SC34_FLAG_RXFIN_DEFAULT << 5)
#define EVENT_SC34_FLAG_TXUND              (0x1UL << 4)
#define _EVENT_SC34_FLAG_TXUND_SHIFT       4
#define _EVENT_SC34_FLAG_TXUND_MASK        0x10UL
#define _EVENT_SC34_FLAG_TXUND_DEFAULT     0x00000000UL
#define EVENT_SC34_FLAG_TXUND_DEFAULT      (_EVENT_SC34_FLAG_TXUND_DEFAULT << 4)
#define EVENT_SC34_FLAG_RXOVF              (0x1UL << 3)
#define _EVENT_SC34_FLAG_RXOVF_SHIFT       3
#define _EVENT_SC34_FLAG_RXOVF_MASK        0x8UL
#define _EVENT_SC34_FLAG_RXOVF_DEFAULT     0x00000000UL
#define EVENT_SC34_FLAG_RXOVF_DEFAULT      (_EVENT_SC34_FLAG_RXOVF_DEFAULT << 3)
#define EVENT_SC34_FLAG_TXIDLE             (0x1UL << 2)
#define _EVENT_SC34_FLAG_TXIDLE_SHIFT      2
#define _EVENT_SC34_FLAG_TXIDLE_MASK       0x4UL
#define _EVENT_SC34_FLAG_TXIDLE_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_TXIDLE_DEFAULT     (_EVENT_SC34_FLAG_TXIDLE_DEFAULT << 2)
#define EVENT_SC34_FLAG_TXFREE             (0x1UL << 1)
#define _EVENT_SC34_FLAG_TXFREE_SHIFT      1
#define _EVENT_SC34_FLAG_TXFREE_MASK       0x2UL
#define _EVENT_SC34_FLAG_TXFREE_DEFAULT    0x00000000UL
#define EVENT_SC34_FLAG_TXFREE_DEFAULT     (_EVENT_SC34_FLAG_TXFREE_DEFAULT << 1)
#define EVENT_SC34_FLAG_RXVAL              (0x1UL << 0)
#define _EVENT_SC34_FLAG_RXVAL_SHIFT       0
#define _EVENT_SC34_FLAG_RXVAL_MASK        0x1UL
#define _EVENT_SC34_FLAG_RXVAL_DEFAULT     0x00000000UL
#define EVENT_SC34_FLAG_RXVAL_DEFAULT      (_EVENT_SC34_FLAG_RXVAL_DEFAULT << 0)

/* Bit fields for EVENT_SC34 CFG */
#define _EVENT_SC34_CFG_RESETVALUE        0x00000000UL
#define _EVENT_SC34_CFG_MASK              0x00007FFFUL
#define EVENT_SC34_CFG_PARERR             (0x1UL << 14)
#define _EVENT_SC34_CFG_PARERR_SHIFT      14
#define _EVENT_SC34_CFG_PARERR_MASK       0x4000UL
#define _EVENT_SC34_CFG_PARERR_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_PARERR_DEFAULT     (_EVENT_SC34_CFG_PARERR_DEFAULT << 14)
#define EVENT_SC34_CFG_FRMERR             (0x1UL << 13)
#define _EVENT_SC34_CFG_FRMERR_SHIFT      13
#define _EVENT_SC34_CFG_FRMERR_MASK       0x2000UL
#define _EVENT_SC34_CFG_FRMERR_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_FRMERR_DEFAULT     (_EVENT_SC34_CFG_FRMERR_DEFAULT << 13)
#define EVENT_SC34_CFG_TXULDB             (0x1UL << 12)
#define _EVENT_SC34_CFG_TXULDB_SHIFT      12
#define _EVENT_SC34_CFG_TXULDB_MASK       0x1000UL
#define _EVENT_SC34_CFG_TXULDB_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_TXULDB_DEFAULT     (_EVENT_SC34_CFG_TXULDB_DEFAULT << 12)
#define EVENT_SC34_CFG_TXULDA             (0x1UL << 11)
#define _EVENT_SC34_CFG_TXULDA_SHIFT      11
#define _EVENT_SC34_CFG_TXULDA_MASK       0x800UL
#define _EVENT_SC34_CFG_TXULDA_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_TXULDA_DEFAULT     (_EVENT_SC34_CFG_TXULDA_DEFAULT << 11)
#define EVENT_SC34_CFG_RXULDB             (0x1UL << 10)
#define _EVENT_SC34_CFG_RXULDB_SHIFT      10
#define _EVENT_SC34_CFG_RXULDB_MASK       0x400UL
#define _EVENT_SC34_CFG_RXULDB_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_RXULDB_DEFAULT     (_EVENT_SC34_CFG_RXULDB_DEFAULT << 10)
#define EVENT_SC34_CFG_RXULDA             (0x1UL << 9)
#define _EVENT_SC34_CFG_RXULDA_SHIFT      9
#define _EVENT_SC34_CFG_RXULDA_MASK       0x200UL
#define _EVENT_SC34_CFG_RXULDA_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_RXULDA_DEFAULT     (_EVENT_SC34_CFG_RXULDA_DEFAULT << 9)
#define EVENT_SC34_CFG_NAK                (0x1UL << 8)
#define _EVENT_SC34_CFG_NAK_SHIFT         8
#define _EVENT_SC34_CFG_NAK_MASK          0x100UL
#define _EVENT_SC34_CFG_NAK_DEFAULT       0x00000000UL
#define EVENT_SC34_CFG_NAK_DEFAULT        (_EVENT_SC34_CFG_NAK_DEFAULT << 8)
#define EVENT_SC34_CFG_CMDFIN             (0x1UL << 7)
#define _EVENT_SC34_CFG_CMDFIN_SHIFT      7
#define _EVENT_SC34_CFG_CMDFIN_MASK       0x80UL
#define _EVENT_SC34_CFG_CMDFIN_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_CMDFIN_DEFAULT     (_EVENT_SC34_CFG_CMDFIN_DEFAULT << 7)
#define EVENT_SC34_CFG_TXFIN              (0x1UL << 6)
#define _EVENT_SC34_CFG_TXFIN_SHIFT       6
#define _EVENT_SC34_CFG_TXFIN_MASK        0x40UL
#define _EVENT_SC34_CFG_TXFIN_DEFAULT     0x00000000UL
#define EVENT_SC34_CFG_TXFIN_DEFAULT      (_EVENT_SC34_CFG_TXFIN_DEFAULT << 6)
#define EVENT_SC34_CFG_RXFIN              (0x1UL << 5)
#define _EVENT_SC34_CFG_RXFIN_SHIFT       5
#define _EVENT_SC34_CFG_RXFIN_MASK        0x20UL
#define _EVENT_SC34_CFG_RXFIN_DEFAULT     0x00000000UL
#define EVENT_SC34_CFG_RXFIN_DEFAULT      (_EVENT_SC34_CFG_RXFIN_DEFAULT << 5)
#define EVENT_SC34_CFG_TXUND              (0x1UL << 4)
#define _EVENT_SC34_CFG_TXUND_SHIFT       4
#define _EVENT_SC34_CFG_TXUND_MASK        0x10UL
#define _EVENT_SC34_CFG_TXUND_DEFAULT     0x00000000UL
#define EVENT_SC34_CFG_TXUND_DEFAULT      (_EVENT_SC34_CFG_TXUND_DEFAULT << 4)
#define EVENT_SC34_CFG_RXOVF              (0x1UL << 3)
#define _EVENT_SC34_CFG_RXOVF_SHIFT       3
#define _EVENT_SC34_CFG_RXOVF_MASK        0x8UL
#define _EVENT_SC34_CFG_RXOVF_DEFAULT     0x00000000UL
#define EVENT_SC34_CFG_RXOVF_DEFAULT      (_EVENT_SC34_CFG_RXOVF_DEFAULT << 3)
#define EVENT_SC34_CFG_TXIDLE             (0x1UL << 2)
#define _EVENT_SC34_CFG_TXIDLE_SHIFT      2
#define _EVENT_SC34_CFG_TXIDLE_MASK       0x4UL
#define _EVENT_SC34_CFG_TXIDLE_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_TXIDLE_DEFAULT     (_EVENT_SC34_CFG_TXIDLE_DEFAULT << 2)
#define EVENT_SC34_CFG_TXFREE             (0x1UL << 1)
#define _EVENT_SC34_CFG_TXFREE_SHIFT      1
#define _EVENT_SC34_CFG_TXFREE_MASK       0x2UL
#define _EVENT_SC34_CFG_TXFREE_DEFAULT    0x00000000UL
#define EVENT_SC34_CFG_TXFREE_DEFAULT     (_EVENT_SC34_CFG_TXFREE_DEFAULT << 1)
#define EVENT_SC34_CFG_RXVAL              (0x1UL << 0)
#define _EVENT_SC34_CFG_RXVAL_SHIFT       0
#define _EVENT_SC34_CFG_RXVAL_MASK        0x1UL
#define _EVENT_SC34_CFG_RXVAL_DEFAULT     0x00000000UL
#define EVENT_SC34_CFG_RXVAL_DEFAULT      (_EVENT_SC34_CFG_RXVAL_DEFAULT << 0)

/* Bit fields for EVENT_SC34 INTMODE */
#define _EVENT_SC34_INTMODE_RESETVALUE             0x00000000UL
#define _EVENT_SC34_INTMODE_MASK                   0x00000007UL
#define EVENT_SC34_INTMODE_TXIDLELEVEL             (0x1UL << 2)
#define _EVENT_SC34_INTMODE_TXIDLELEVEL_SHIFT      2
#define _EVENT_SC34_INTMODE_TXIDLELEVEL_MASK       0x4UL
#define _EVENT_SC34_INTMODE_TXIDLELEVEL_DEFAULT    0x00000000UL
#define EVENT_SC34_INTMODE_TXIDLELEVEL_DEFAULT     (_EVENT_SC34_INTMODE_TXIDLELEVEL_DEFAULT << 2)
#define EVENT_SC34_INTMODE_TXFREELEVEL             (0x1UL << 1)
#define _EVENT_SC34_INTMODE_TXFREELEVEL_SHIFT      1
#define _EVENT_SC34_INTMODE_TXFREELEVEL_MASK       0x2UL
#define _EVENT_SC34_INTMODE_TXFREELEVEL_DEFAULT    0x00000000UL
#define EVENT_SC34_INTMODE_TXFREELEVEL_DEFAULT     (_EVENT_SC34_INTMODE_TXFREELEVEL_DEFAULT << 1)
#define EVENT_SC34_INTMODE_RXVALLEVEL              (0x1UL << 0)
#define _EVENT_SC34_INTMODE_RXVALLEVEL_SHIFT       0
#define _EVENT_SC34_INTMODE_RXVALLEVEL_MASK        0x1UL
#define _EVENT_SC34_INTMODE_RXVALLEVEL_DEFAULT     0x00000000UL
#define EVENT_SC34_INTMODE_RXVALLEVEL_DEFAULT      (_EVENT_SC34_INTMODE_RXVALLEVEL_DEFAULT << 0)

/** @} End of group EM359X_EVENT_SC34_BitFields */

#endif // EM359X_EVENT_SC34_H
