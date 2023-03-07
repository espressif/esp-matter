/**************************************************************************//**
* @file
* @brief em358x_sleeptmr Register and Bit Field definitions
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

#ifndef EM358X_SLEEPTMR_H
#define EM358X_SLEEPTMR_H

/**************************************************************************//**
 * @defgroup EM358X_SLEEPTMR
 * @{
 * @brief EM358X_SLEEPTMR Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t CFG;
  __IM uint32_t CNTH;
  __IM uint32_t CNTL;
  __IOM uint32_t CMPAH;
  __IOM uint32_t CMPAL;
  __IOM uint32_t CMPBH;
  __IOM uint32_t CMPBL;
} SLEEPTMR_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM358X_SLEEPTMR_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for SLEEPTMR CFG */
#define _SLEEPTMR_CFG_RESETVALUE          0x00000400UL
#define _SLEEPTMR_CFG_MASK                0x00001CF1UL
#define SLEEPTMR_CFG_REVERSE              (0x1UL << 12)
#define _SLEEPTMR_CFG_REVERSE_SHIFT       12
#define _SLEEPTMR_CFG_REVERSE_MASK        0x1000UL
#define _SLEEPTMR_CFG_REVERSE_DEFAULT     0x00000000UL
#define SLEEPTMR_CFG_REVERSE_DEFAULT      (_SLEEPTMR_CFG_REVERSE_DEFAULT << 12)
#define SLEEPTMR_CFG_ENABLE               (0x1UL << 11)
#define _SLEEPTMR_CFG_ENABLE_SHIFT        11
#define _SLEEPTMR_CFG_ENABLE_MASK         0x800UL
#define _SLEEPTMR_CFG_ENABLE_DEFAULT      0x00000000UL
#define SLEEPTMR_CFG_ENABLE_DEFAULT       (_SLEEPTMR_CFG_ENABLE_DEFAULT << 11)
#define SLEEPTMR_CFG_DBGPAUSE             (0x1UL << 10)
#define _SLEEPTMR_CFG_DBGPAUSE_SHIFT      10
#define _SLEEPTMR_CFG_DBGPAUSE_MASK       0x400UL
#define _SLEEPTMR_CFG_DBGPAUSE_DEFAULT    0x00000001UL
#define SLEEPTMR_CFG_DBGPAUSE_DEFAULT     (_SLEEPTMR_CFG_DBGPAUSE_DEFAULT << 10)
#define _SLEEPTMR_CFG_CLKDIV_SHIFT        4
#define _SLEEPTMR_CFG_CLKDIV_MASK         0xF0UL
#define _SLEEPTMR_CFG_CLKDIV_DEFAULT      0x00000000UL
#define SLEEPTMR_CFG_CLKDIV_DEFAULT       (_SLEEPTMR_CFG_CLKDIV_DEFAULT << 4)
#define SLEEPTMR_CFG_CLKSEL               (0x1UL << 0)
#define _SLEEPTMR_CFG_CLKSEL_SHIFT        0
#define _SLEEPTMR_CFG_CLKSEL_MASK         0x1UL
#define _SLEEPTMR_CFG_CLKSEL_DEFAULT      0x00000000UL
#define SLEEPTMR_CFG_CLKSEL_DEFAULT       (_SLEEPTMR_CFG_CLKSEL_DEFAULT << 0)

/* Bit fields for SLEEPTMR CNTH */
#define _SLEEPTMR_CNTH_RESETVALUE            0x00000000UL
#define _SLEEPTMR_CNTH_MASK                  0x0000FFFFUL
#define _SLEEPTMR_CNTH_CNTH_FIELD_SHIFT      0
#define _SLEEPTMR_CNTH_CNTH_FIELD_MASK       0xFFFFUL
#define _SLEEPTMR_CNTH_CNTH_FIELD_DEFAULT    0x00000000UL
#define SLEEPTMR_CNTH_CNTH_FIELD_DEFAULT     (_SLEEPTMR_CNTH_CNTH_FIELD_DEFAULT << 0)

/* Bit fields for SLEEPTMR CNTL */
#define _SLEEPTMR_CNTL_RESETVALUE            0x00000000UL
#define _SLEEPTMR_CNTL_MASK                  0x0000FFFFUL
#define _SLEEPTMR_CNTL_CNTL_FIELD_SHIFT      0
#define _SLEEPTMR_CNTL_CNTL_FIELD_MASK       0xFFFFUL
#define _SLEEPTMR_CNTL_CNTL_FIELD_DEFAULT    0x00000000UL
#define SLEEPTMR_CNTL_CNTL_FIELD_DEFAULT     (_SLEEPTMR_CNTL_CNTL_FIELD_DEFAULT << 0)

/* Bit fields for SLEEPTMR CMPAH */
#define _SLEEPTMR_CMPAH_RESETVALUE             0x0000FFFFUL
#define _SLEEPTMR_CMPAH_MASK                   0x0000FFFFUL
#define _SLEEPTMR_CMPAH_CMPAH_FIELD_SHIFT      0
#define _SLEEPTMR_CMPAH_CMPAH_FIELD_MASK       0xFFFFUL
#define _SLEEPTMR_CMPAH_CMPAH_FIELD_DEFAULT    0x0000FFFFUL
#define SLEEPTMR_CMPAH_CMPAH_FIELD_DEFAULT     (_SLEEPTMR_CMPAH_CMPAH_FIELD_DEFAULT << 0)

/* Bit fields for SLEEPTMR CMPAL */
#define _SLEEPTMR_CMPAL_RESETVALUE             0x0000FFFFUL
#define _SLEEPTMR_CMPAL_MASK                   0x0000FFFFUL
#define _SLEEPTMR_CMPAL_CMPAL_FIELD_SHIFT      0
#define _SLEEPTMR_CMPAL_CMPAL_FIELD_MASK       0xFFFFUL
#define _SLEEPTMR_CMPAL_CMPAL_FIELD_DEFAULT    0x0000FFFFUL
#define SLEEPTMR_CMPAL_CMPAL_FIELD_DEFAULT     (_SLEEPTMR_CMPAL_CMPAL_FIELD_DEFAULT << 0)

/* Bit fields for SLEEPTMR CMPBH */
#define _SLEEPTMR_CMPBH_RESETVALUE             0x0000FFFFUL
#define _SLEEPTMR_CMPBH_MASK                   0x0000FFFFUL
#define _SLEEPTMR_CMPBH_CMPBH_FIELD_SHIFT      0
#define _SLEEPTMR_CMPBH_CMPBH_FIELD_MASK       0xFFFFUL
#define _SLEEPTMR_CMPBH_CMPBH_FIELD_DEFAULT    0x0000FFFFUL
#define SLEEPTMR_CMPBH_CMPBH_FIELD_DEFAULT     (_SLEEPTMR_CMPBH_CMPBH_FIELD_DEFAULT << 0)

/* Bit fields for SLEEPTMR CMPBL */
#define _SLEEPTMR_CMPBL_RESETVALUE             0x0000FFFFUL
#define _SLEEPTMR_CMPBL_MASK                   0x0000FFFFUL
#define _SLEEPTMR_CMPBL_CMPBL_FIELD_SHIFT      0
#define _SLEEPTMR_CMPBL_CMPBL_FIELD_MASK       0xFFFFUL
#define _SLEEPTMR_CMPBL_CMPBL_FIELD_DEFAULT    0x0000FFFFUL
#define SLEEPTMR_CMPBL_CMPBL_FIELD_DEFAULT     (_SLEEPTMR_CMPBL_CMPBL_FIELD_DEFAULT << 0)

/** @} End of group EM358X_SLEEPTMR_BitFields */

#endif // EM358X_SLEEPTMR_H
