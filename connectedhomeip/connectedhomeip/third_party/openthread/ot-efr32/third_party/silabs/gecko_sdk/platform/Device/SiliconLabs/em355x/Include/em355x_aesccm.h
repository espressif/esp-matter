/**************************************************************************//**
* @file
* @brief em355x_aesccm Register and Bit Field definitions
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

#ifndef EM355X_AESCCM_H
#define EM355X_AESCCM_H

/**************************************************************************//**
 * @defgroup EM355X_AESCCM
 * @{
 * @brief EM355X_AESCCM Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t CONFIG;
  __IM uint32_t STATUS;
  __IOM uint32_t CBCSTATE0;
  __IOM uint32_t CBCSTATE1;
  __IOM uint32_t CBCSTATE2;
  __IOM uint32_t CBCSTATE3;
  uint32_t RESERVED0[4];
  __IOM uint32_t PLAINTEXT;
  uint32_t RESERVED1[1];
  __IM uint32_t CIPHERTEXT;
  uint32_t RESERVED2[1];
  __IOM uint32_t KEY0;
  __IOM uint32_t KEY1;
  __IOM uint32_t KEY2;
  __IOM uint32_t KEY3;
} AESCCM_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM355X_AESCCM_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for AESCCM CONFIG */
#define _AESCCM_CONFIG_RESETVALUE             0x00000000UL
#define _AESCCM_CONFIG_MASK                   0x000000FFUL
#define AESCCM_CONFIG_SECRST                  (0x1UL << 7)
#define _AESCCM_CONFIG_SECRST_SHIFT           7
#define _AESCCM_CONFIG_SECRST_MASK            0x80UL
#define _AESCCM_CONFIG_SECRST_DEFAULT         0x00000000UL
#define AESCCM_CONFIG_SECRST_DEFAULT          (_AESCCM_CONFIG_SECRST_DEFAULT << 7)
#define AESCCM_CONFIG_CTRIN                   (0x1UL << 6)
#define _AESCCM_CONFIG_CTRIN_SHIFT            6
#define _AESCCM_CONFIG_CTRIN_MASK             0x40UL
#define _AESCCM_CONFIG_CTRIN_DEFAULT          0x00000000UL
#define AESCCM_CONFIG_CTRIN_DEFAULT           (_AESCCM_CONFIG_CTRIN_DEFAULT << 6)
#define AESCCM_CONFIG_MICXORCT                (0x1UL << 5)
#define _AESCCM_CONFIG_MICXORCT_SHIFT         5
#define _AESCCM_CONFIG_MICXORCT_MASK          0x20UL
#define _AESCCM_CONFIG_MICXORCT_DEFAULT       0x00000000UL
#define AESCCM_CONFIG_MICXORCT_DEFAULT        (_AESCCM_CONFIG_MICXORCT_DEFAULT << 5)
#define AESCCM_CONFIG_CBCXORPT                (0x1UL << 4)
#define _AESCCM_CONFIG_CBCXORPT_SHIFT         4
#define _AESCCM_CONFIG_CBCXORPT_MASK          0x10UL
#define _AESCCM_CONFIG_CBCXORPT_DEFAULT       0x00000000UL
#define AESCCM_CONFIG_CBCXORPT_DEFAULT        (_AESCCM_CONFIG_CBCXORPT_DEFAULT << 4)
#define AESCCM_CONFIG_CTTOCBCST               (0x1UL << 3)
#define _AESCCM_CONFIG_CTTOCBCST_SHIFT        3
#define _AESCCM_CONFIG_CTTOCBCST_MASK         0x8UL
#define _AESCCM_CONFIG_CTTOCBCST_DEFAULT      0x00000000UL
#define AESCCM_CONFIG_CTTOCBCST_DEFAULT       (_AESCCM_CONFIG_CTTOCBCST_DEFAULT << 3)
#define AESCCM_CONFIG_WAITCTREAD              (0x1UL << 2)
#define _AESCCM_CONFIG_WAITCTREAD_SHIFT       2
#define _AESCCM_CONFIG_WAITCTREAD_MASK        0x4UL
#define _AESCCM_CONFIG_WAITCTREAD_DEFAULT     0x00000000UL
#define AESCCM_CONFIG_WAITCTREAD_DEFAULT      (_AESCCM_CONFIG_WAITCTREAD_DEFAULT << 2)
#define AESCCM_CONFIG_WAITPTWRITE             (0x1UL << 1)
#define _AESCCM_CONFIG_WAITPTWRITE_SHIFT      1
#define _AESCCM_CONFIG_WAITPTWRITE_MASK       0x2UL
#define _AESCCM_CONFIG_WAITPTWRITE_DEFAULT    0x00000000UL
#define AESCCM_CONFIG_WAITPTWRITE_DEFAULT     (_AESCCM_CONFIG_WAITPTWRITE_DEFAULT << 1)
#define AESCCM_CONFIG_STARTAES                (0x1UL << 0)
#define _AESCCM_CONFIG_STARTAES_SHIFT         0
#define _AESCCM_CONFIG_STARTAES_MASK          0x1UL
#define _AESCCM_CONFIG_STARTAES_DEFAULT       0x00000000UL
#define AESCCM_CONFIG_STARTAES_DEFAULT        (_AESCCM_CONFIG_STARTAES_DEFAULT << 0)

/* Bit fields for AESCCM STATUS */
#define _AESCCM_STATUS_RESETVALUE         0x00000000UL
#define _AESCCM_STATUS_MASK               0x00000001UL
#define _AESCCM_STATUS_SECBUSY_SHIFT      0
#define _AESCCM_STATUS_SECBUSY_MASK       0x1UL
#define _AESCCM_STATUS_SECBUSY_DEFAULT    0x00000000UL
#define AESCCM_STATUS_SECBUSY_DEFAULT     (_AESCCM_STATUS_SECBUSY_DEFAULT << 0)

/* Bit fields for AESCCM CBCSTATE0 */
#define _AESCCM_CBCSTATE0_RESETVALUE          0x00000000UL
#define _AESCCM_CBCSTATE0_MASK                0xFFFFFFFFUL
#define _AESCCM_CBCSTATE0_CBCSTATE_SHIFT      0
#define _AESCCM_CBCSTATE0_CBCSTATE_MASK       0xFFFFFFFFUL
#define _AESCCM_CBCSTATE0_CBCSTATE_DEFAULT    0x00000000UL
#define AESCCM_CBCSTATE0_CBCSTATE_DEFAULT     (_AESCCM_CBCSTATE0_CBCSTATE_DEFAULT << 0)

/* Bit fields for AESCCM CBCSTATE1 */
#define _AESCCM_CBCSTATE1_RESETVALUE           0x00000000UL
#define _AESCCM_CBCSTATE1_MASK                 0xFFFFFFFFUL
#define _AESCCM_CBCSTATE1_CBCSTATE1_SHIFT      0
#define _AESCCM_CBCSTATE1_CBCSTATE1_MASK       0xFFFFFFFFUL
#define _AESCCM_CBCSTATE1_CBCSTATE1_DEFAULT    0x00000000UL
#define AESCCM_CBCSTATE1_CBCSTATE1_DEFAULT     (_AESCCM_CBCSTATE1_CBCSTATE1_DEFAULT << 0)

/* Bit fields for AESCCM CBCSTATE2 */
#define _AESCCM_CBCSTATE2_RESETVALUE           0x00000000UL
#define _AESCCM_CBCSTATE2_MASK                 0xFFFFFFFFUL
#define _AESCCM_CBCSTATE2_CBCSTATE2_SHIFT      0
#define _AESCCM_CBCSTATE2_CBCSTATE2_MASK       0xFFFFFFFFUL
#define _AESCCM_CBCSTATE2_CBCSTATE2_DEFAULT    0x00000000UL
#define AESCCM_CBCSTATE2_CBCSTATE2_DEFAULT     (_AESCCM_CBCSTATE2_CBCSTATE2_DEFAULT << 0)

/* Bit fields for AESCCM CBCSTATE3 */
#define _AESCCM_CBCSTATE3_RESETVALUE           0x00000000UL
#define _AESCCM_CBCSTATE3_MASK                 0xFFFFFFFFUL
#define _AESCCM_CBCSTATE3_CBCSTATE3_SHIFT      0
#define _AESCCM_CBCSTATE3_CBCSTATE3_MASK       0xFFFFFFFFUL
#define _AESCCM_CBCSTATE3_CBCSTATE3_DEFAULT    0x00000000UL
#define AESCCM_CBCSTATE3_CBCSTATE3_DEFAULT     (_AESCCM_CBCSTATE3_CBCSTATE3_DEFAULT << 0)

/* Bit fields for AESCCM PLAINTEXT */
#define _AESCCM_PLAINTEXT_RESETVALUE    0x00000000UL
#define _AESCCM_PLAINTEXT_MASK          0xFFFFFFFFUL
#define _AESCCM_PLAINTEXT_PT_SHIFT      0
#define _AESCCM_PLAINTEXT_PT_MASK       0xFFFFFFFFUL
#define _AESCCM_PLAINTEXT_PT_DEFAULT    0x00000000UL
#define AESCCM_PLAINTEXT_PT_DEFAULT     (_AESCCM_PLAINTEXT_PT_DEFAULT << 0)

/* Bit fields for AESCCM CIPHERTEXT */
#define _AESCCM_CIPHERTEXT_RESETVALUE    0x00000000UL
#define _AESCCM_CIPHERTEXT_MASK          0xFFFFFFFFUL
#define _AESCCM_CIPHERTEXT_CT_SHIFT      0
#define _AESCCM_CIPHERTEXT_CT_MASK       0xFFFFFFFFUL
#define _AESCCM_CIPHERTEXT_CT_DEFAULT    0x00000000UL
#define AESCCM_CIPHERTEXT_CT_DEFAULT     (_AESCCM_CIPHERTEXT_CT_DEFAULT << 0)

/* Bit fields for AESCCM KEY0 */
#define _AESCCM_KEY0_RESETVALUE      0x00000000UL
#define _AESCCM_KEY0_MASK            0xFFFFFFFFUL
#define _AESCCM_KEY0_KEYO_SHIFT      0
#define _AESCCM_KEY0_KEYO_MASK       0xFFFFFFFFUL
#define _AESCCM_KEY0_KEYO_DEFAULT    0x00000000UL
#define AESCCM_KEY0_KEYO_DEFAULT     (_AESCCM_KEY0_KEYO_DEFAULT << 0)

/* Bit fields for AESCCM KEY1 */
#define _AESCCM_KEY1_RESETVALUE      0x00000000UL
#define _AESCCM_KEY1_MASK            0xFFFFFFFFUL
#define _AESCCM_KEY1_KEY1_SHIFT      0
#define _AESCCM_KEY1_KEY1_MASK       0xFFFFFFFFUL
#define _AESCCM_KEY1_KEY1_DEFAULT    0x00000000UL
#define AESCCM_KEY1_KEY1_DEFAULT     (_AESCCM_KEY1_KEY1_DEFAULT << 0)

/* Bit fields for AESCCM KEY2 */
#define _AESCCM_KEY2_RESETVALUE      0x00000000UL
#define _AESCCM_KEY2_MASK            0xFFFFFFFFUL
#define _AESCCM_KEY2_KEY2_SHIFT      0
#define _AESCCM_KEY2_KEY2_MASK       0xFFFFFFFFUL
#define _AESCCM_KEY2_KEY2_DEFAULT    0x00000000UL
#define AESCCM_KEY2_KEY2_DEFAULT     (_AESCCM_KEY2_KEY2_DEFAULT << 0)

/* Bit fields for AESCCM KEY3 */
#define _AESCCM_KEY3_RESETVALUE      0x00000000UL
#define _AESCCM_KEY3_MASK            0xFFFFFFFFUL
#define _AESCCM_KEY3_KEY3_SHIFT      0
#define _AESCCM_KEY3_KEY3_MASK       0xFFFFFFFFUL
#define _AESCCM_KEY3_KEY3_DEFAULT    0x00000000UL
#define AESCCM_KEY3_KEY3_DEFAULT     (_AESCCM_KEY3_KEY3_DEFAULT << 0)

/** @} End of group EM355X_AESCCM_BitFields */

#endif // EM355X_AESCCM_H
