/**************************************************************************//**
* @file
* @brief em35x_event_miss Register and Bit Field definitions
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

#ifndef EM35X_EVENT_MISS_H
#define EM35X_EVENT_MISS_H

/**************************************************************************//**
 * @defgroup EM35X_EVENT_MISS
 * @{
 * @brief EM35X_EVENT_MISS Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t MISS;
} EVENT_MISS_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_EVENT_MISS_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_MISS MISS */
#define _EVENT_MISS_MISS_RESETVALUE        0x00000000UL
#define _EVENT_MISS_MISS_MASK              0x0000FFFCUL
#define EVENT_MISS_MISS_IRQD               (0x1UL << 15)
#define _EVENT_MISS_MISS_IRQD_SHIFT        15
#define _EVENT_MISS_MISS_IRQD_MASK         0x8000UL
#define _EVENT_MISS_MISS_IRQD_DEFAULT      0x00000000UL
#define EVENT_MISS_MISS_IRQD_DEFAULT       (_EVENT_MISS_MISS_IRQD_DEFAULT << 15)
#define EVENT_MISS_MISS_IRQC               (0x1UL << 14)
#define _EVENT_MISS_MISS_IRQC_SHIFT        14
#define _EVENT_MISS_MISS_IRQC_MASK         0x4000UL
#define _EVENT_MISS_MISS_IRQC_DEFAULT      0x00000000UL
#define EVENT_MISS_MISS_IRQC_DEFAULT       (_EVENT_MISS_MISS_IRQC_DEFAULT << 14)
#define EVENT_MISS_MISS_IRQB               (0x1UL << 13)
#define _EVENT_MISS_MISS_IRQB_SHIFT        13
#define _EVENT_MISS_MISS_IRQB_MASK         0x2000UL
#define _EVENT_MISS_MISS_IRQB_DEFAULT      0x00000000UL
#define EVENT_MISS_MISS_IRQB_DEFAULT       (_EVENT_MISS_MISS_IRQB_DEFAULT << 13)
#define EVENT_MISS_MISS_IRQA               (0x1UL << 12)
#define _EVENT_MISS_MISS_IRQA_SHIFT        12
#define _EVENT_MISS_MISS_IRQA_MASK         0x1000UL
#define _EVENT_MISS_MISS_IRQA_DEFAULT      0x00000000UL
#define EVENT_MISS_MISS_IRQA_DEFAULT       (_EVENT_MISS_MISS_IRQA_DEFAULT << 12)
#define EVENT_MISS_MISS_ADC                (0x1UL << 11)
#define _EVENT_MISS_MISS_ADC_SHIFT         11
#define _EVENT_MISS_MISS_ADC_MASK          0x800UL
#define _EVENT_MISS_MISS_ADC_DEFAULT       0x00000000UL
#define EVENT_MISS_MISS_ADC_DEFAULT        (_EVENT_MISS_MISS_ADC_DEFAULT << 11)
#define EVENT_MISS_MISS_MACRX              (0x1UL << 10)
#define _EVENT_MISS_MISS_MACRX_SHIFT       10
#define _EVENT_MISS_MISS_MACRX_MASK        0x400UL
#define _EVENT_MISS_MISS_MACRX_DEFAULT     0x00000000UL
#define EVENT_MISS_MISS_MACRX_DEFAULT      (_EVENT_MISS_MISS_MACRX_DEFAULT << 10)
#define EVENT_MISS_MISS_MACTX              (0x1UL << 9)
#define _EVENT_MISS_MISS_MACTX_SHIFT       9
#define _EVENT_MISS_MISS_MACTX_MASK        0x200UL
#define _EVENT_MISS_MISS_MACTX_DEFAULT     0x00000000UL
#define EVENT_MISS_MISS_MACTX_DEFAULT      (_EVENT_MISS_MISS_MACTX_DEFAULT << 9)
#define EVENT_MISS_MISS_MACTMR             (0x1UL << 8)
#define _EVENT_MISS_MISS_MACTMR_SHIFT      8
#define _EVENT_MISS_MISS_MACTMR_MASK       0x100UL
#define _EVENT_MISS_MISS_MACTMR_DEFAULT    0x00000000UL
#define EVENT_MISS_MISS_MACTMR_DEFAULT     (_EVENT_MISS_MISS_MACTMR_DEFAULT << 8)
#define EVENT_MISS_MISS_AESCCM             (0x1UL << 7)
#define _EVENT_MISS_MISS_AESCCM_SHIFT      7
#define _EVENT_MISS_MISS_AESCCM_MASK       0x80UL
#define _EVENT_MISS_MISS_AESCCM_DEFAULT    0x00000000UL
#define EVENT_MISS_MISS_AESCCM_DEFAULT     (_EVENT_MISS_MISS_AESCCM_DEFAULT << 7)
#define EVENT_MISS_MISS_SC2                (0x1UL << 6)
#define _EVENT_MISS_MISS_SC2_SHIFT         6
#define _EVENT_MISS_MISS_SC2_MASK          0x40UL
#define _EVENT_MISS_MISS_SC2_DEFAULT       0x00000000UL
#define EVENT_MISS_MISS_SC2_DEFAULT        (_EVENT_MISS_MISS_SC2_DEFAULT << 6)
#define EVENT_MISS_MISS_SC1                (0x1UL << 5)
#define _EVENT_MISS_MISS_SC1_SHIFT         5
#define _EVENT_MISS_MISS_SC1_MASK          0x20UL
#define _EVENT_MISS_MISS_SC1_DEFAULT       0x00000000UL
#define EVENT_MISS_MISS_SC1_DEFAULT        (_EVENT_MISS_MISS_SC1_DEFAULT << 5)
#define EVENT_MISS_MISS_SLEEP              (0x1UL << 4)
#define _EVENT_MISS_MISS_SLEEP_SHIFT       4
#define _EVENT_MISS_MISS_SLEEP_MASK        0x10UL
#define _EVENT_MISS_MISS_SLEEP_DEFAULT     0x00000000UL
#define EVENT_MISS_MISS_SLEEP_DEFAULT      (_EVENT_MISS_MISS_SLEEP_DEFAULT << 4)
#define EVENT_MISS_MISS_BB                 (0x1UL << 3)
#define _EVENT_MISS_MISS_BB_SHIFT          3
#define _EVENT_MISS_MISS_BB_MASK           0x8UL
#define _EVENT_MISS_MISS_BB_DEFAULT        0x00000000UL
#define EVENT_MISS_MISS_BB_DEFAULT         (_EVENT_MISS_MISS_BB_DEFAULT << 3)
#define EVENT_MISS_MISS_MGMT               (0x1UL << 2)
#define _EVENT_MISS_MISS_MGMT_SHIFT        2
#define _EVENT_MISS_MISS_MGMT_MASK         0x4UL
#define _EVENT_MISS_MISS_MGMT_DEFAULT      0x00000000UL
#define EVENT_MISS_MISS_MGMT_DEFAULT       (_EVENT_MISS_MISS_MGMT_DEFAULT << 2)

/** @} End of group EM35X_EVENT_MISS_BitFields */

#endif // EM35X_EVENT_MISS_H
