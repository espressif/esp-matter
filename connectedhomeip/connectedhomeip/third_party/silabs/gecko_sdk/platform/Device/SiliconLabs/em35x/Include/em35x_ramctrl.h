/**************************************************************************//**
* @file
* @brief em35x_ramctrl Register and Bit Field definitions
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

#ifndef EM35X_RAMCTRL_H
#define EM35X_RAMCTRL_H

/**************************************************************************//**
 * @defgroup EM35X_RAMCTRL
 * @{
 * @brief EM35X_RAMCTRL Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t MEMPROT0;
  __IOM uint32_t MEMPROT1;
  __IOM uint32_t MEMPROT2;
  __IOM uint32_t MEMPROT3;
  __IOM uint32_t MEMPROT4;
  __IOM uint32_t MEMPROT5;
  __IOM uint32_t MEMPROT6;
  __IOM uint32_t MEMPROT7;
  __IOM uint32_t MEMPROT8;
  __IOM uint32_t MEMPROT9;
  __IOM uint32_t MEMPROT10;
  __IOM uint32_t MEMPROT11;
  __IM uint32_t DMAPROTADDR;
  __IM uint32_t DMAPROTCH;
  __IOM uint32_t MEMPROTEN;
} RAMCTRL_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_RAMCTRL_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for RAMCTRL MEMPROT0 */
#define _RAMCTRL_MEMPROT0_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT0_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT0_MEMPROT0_SHIFT      0
#define _RAMCTRL_MEMPROT0_MEMPROT0_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT0_MEMPROT0_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT0_MEMPROT0_DEFAULT     (_RAMCTRL_MEMPROT0_MEMPROT0_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT1 */
#define _RAMCTRL_MEMPROT1_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT1_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT1_MEMPROT1_SHIFT      0
#define _RAMCTRL_MEMPROT1_MEMPROT1_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT1_MEMPROT1_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT1_MEMPROT1_DEFAULT     (_RAMCTRL_MEMPROT1_MEMPROT1_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT2 */
#define _RAMCTRL_MEMPROT2_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT2_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT2_MEMPROT2_SHIFT      0
#define _RAMCTRL_MEMPROT2_MEMPROT2_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT2_MEMPROT2_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT2_MEMPROT2_DEFAULT     (_RAMCTRL_MEMPROT2_MEMPROT2_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT3 */
#define _RAMCTRL_MEMPROT3_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT3_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT3_MEMPROT3_SHIFT      0
#define _RAMCTRL_MEMPROT3_MEMPROT3_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT3_MEMPROT3_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT3_MEMPROT3_DEFAULT     (_RAMCTRL_MEMPROT3_MEMPROT3_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT4 */
#define _RAMCTRL_MEMPROT4_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT4_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT4_MEMPROT4_SHIFT      0
#define _RAMCTRL_MEMPROT4_MEMPROT4_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT4_MEMPROT4_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT4_MEMPROT4_DEFAULT     (_RAMCTRL_MEMPROT4_MEMPROT4_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT5 */
#define _RAMCTRL_MEMPROT5_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT5_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT5_MEMPROT5_SHIFT      0
#define _RAMCTRL_MEMPROT5_MEMPROT5_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT5_MEMPROT5_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT5_MEMPROT5_DEFAULT     (_RAMCTRL_MEMPROT5_MEMPROT5_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT6 */
#define _RAMCTRL_MEMPROT6_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT6_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT6_MEMPROT6_SHIFT      0
#define _RAMCTRL_MEMPROT6_MEMPROT6_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT6_MEMPROT6_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT6_MEMPROT6_DEFAULT     (_RAMCTRL_MEMPROT6_MEMPROT6_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT7 */
#define _RAMCTRL_MEMPROT7_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT7_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT7_MEMPROT7_SHIFT      0
#define _RAMCTRL_MEMPROT7_MEMPROT7_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT7_MEMPROT7_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT7_MEMPROT7_DEFAULT     (_RAMCTRL_MEMPROT7_MEMPROT7_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT8 */
#define _RAMCTRL_MEMPROT8_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT8_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT8_MEMPROT8_SHIFT      0
#define _RAMCTRL_MEMPROT8_MEMPROT8_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT8_MEMPROT8_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT8_MEMPROT8_DEFAULT     (_RAMCTRL_MEMPROT8_MEMPROT8_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT9 */
#define _RAMCTRL_MEMPROT9_RESETVALUE          0x00000000UL
#define _RAMCTRL_MEMPROT9_MASK                0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT9_MEMPROT9_SHIFT      0
#define _RAMCTRL_MEMPROT9_MEMPROT9_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT9_MEMPROT9_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT9_MEMPROT9_DEFAULT     (_RAMCTRL_MEMPROT9_MEMPROT9_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT10 */
#define _RAMCTRL_MEMPROT10_RESETVALUE           0x00000000UL
#define _RAMCTRL_MEMPROT10_MASK                 0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT10_MEMPROT10_SHIFT      0
#define _RAMCTRL_MEMPROT10_MEMPROT10_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT10_MEMPROT10_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT10_MEMPROT10_DEFAULT     (_RAMCTRL_MEMPROT10_MEMPROT10_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROT11 */
#define _RAMCTRL_MEMPROT11_RESETVALUE           0x00000000UL
#define _RAMCTRL_MEMPROT11_MASK                 0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT11_MEMPROT11_SHIFT      0
#define _RAMCTRL_MEMPROT11_MEMPROT11_MASK       0xFFFFFFFFUL
#define _RAMCTRL_MEMPROT11_MEMPROT11_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROT11_MEMPROT11_DEFAULT     (_RAMCTRL_MEMPROT11_MEMPROT11_DEFAULT << 0)

/* Bit fields for RAMCTRL DMAPROTADDR */
#define _RAMCTRL_DMAPROTADDR_RESETVALUE                 0x20000000UL
#define _RAMCTRL_DMAPROTADDR_MASK                       0xFFFFFFFFUL
#define _RAMCTRL_DMAPROTADDR_DMAPROTADDROFFS_SHIFT      14
#define _RAMCTRL_DMAPROTADDR_DMAPROTADDROFFS_MASK       0xFFFFC000UL
#define _RAMCTRL_DMAPROTADDR_DMAPROTADDROFFS_DEFAULT    0x00008000UL
#define RAMCTRL_DMAPROTADDR_DMAPROTADDROFFS_DEFAULT     (_RAMCTRL_DMAPROTADDR_DMAPROTADDROFFS_DEFAULT << 14)
#define _RAMCTRL_DMAPROTADDR_DMAPROTADDR_SHIFT          0
#define _RAMCTRL_DMAPROTADDR_DMAPROTADDR_MASK           0x3FFFUL
#define _RAMCTRL_DMAPROTADDR_DMAPROTADDR_DEFAULT        0x00000000UL
#define RAMCTRL_DMAPROTADDR_DMAPROTADDR_DEFAULT         (_RAMCTRL_DMAPROTADDR_DMAPROTADDR_DEFAULT << 0)

/* Bit fields for RAMCTRL DMAPROTCH */
#define _RAMCTRL_DMAPROTCH_RESETVALUE           0x00000000UL
#define _RAMCTRL_DMAPROTCH_MASK                 0x00000007UL
#define _RAMCTRL_DMAPROTCH_DMAPROTCH_SHIFT      0
#define _RAMCTRL_DMAPROTCH_DMAPROTCH_MASK       0x7UL
#define _RAMCTRL_DMAPROTCH_DMAPROTCH_DEFAULT    0x00000000UL
#define RAMCTRL_DMAPROTCH_DMAPROTCH_DEFAULT     (_RAMCTRL_DMAPROTCH_DMAPROTCH_DEFAULT << 0)

/* Bit fields for RAMCTRL MEMPROTEN */
#define _RAMCTRL_MEMPROTEN_RESETVALUE                0x00000000UL
#define _RAMCTRL_MEMPROTEN_MASK                      0x00000007UL
#define RAMCTRL_MEMPROTEN_FORCEPROT                  (0x1UL << 2)
#define _RAMCTRL_MEMPROTEN_FORCEPROT_SHIFT           2
#define _RAMCTRL_MEMPROTEN_FORCEPROT_MASK            0x4UL
#define _RAMCTRL_MEMPROTEN_FORCEPROT_DEFAULT         0x00000000UL
#define RAMCTRL_MEMPROTEN_FORCEPROT_DEFAULT          (_RAMCTRL_MEMPROTEN_FORCEPROT_DEFAULT << 2)
#define RAMCTRL_MEMPROTEN_DMAPROTENMAC               (0x1UL << 1)
#define _RAMCTRL_MEMPROTEN_DMAPROTENMAC_SHIFT        1
#define _RAMCTRL_MEMPROTEN_DMAPROTENMAC_MASK         0x2UL
#define _RAMCTRL_MEMPROTEN_DMAPROTENMAC_DEFAULT      0x00000000UL
#define RAMCTRL_MEMPROTEN_DMAPROTENMAC_DEFAULT       (_RAMCTRL_MEMPROTEN_DMAPROTENMAC_DEFAULT << 1)
#define RAMCTRL_MEMPROTEN_DMAPROTENOTHER             (0x1UL << 0)
#define _RAMCTRL_MEMPROTEN_DMAPROTENOTHER_SHIFT      0
#define _RAMCTRL_MEMPROTEN_DMAPROTENOTHER_MASK       0x1UL
#define _RAMCTRL_MEMPROTEN_DMAPROTENOTHER_DEFAULT    0x00000000UL
#define RAMCTRL_MEMPROTEN_DMAPROTENOTHER_DEFAULT     (_RAMCTRL_MEMPROTEN_DMAPROTENOTHER_DEFAULT << 0)

/** @} End of group EM35X_RAMCTRL_BitFields */

#endif // EM35X_RAMCTRL_H
