/**************************************************************************//**
* @file
* @brief sky66107_event_tim Register and Bit Field definitions
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

#ifndef SKY66107_EVENT_TIM_H
#define SKY66107_EVENT_TIM_H

/**************************************************************************//**
 * @defgroup SKY66107_EVENT_TIM
 * @{
 * @brief SKY66107_EVENT_TIM Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[5];
  __IOM uint32_t MISS;
  uint32_t RESERVED1[9];
  __IOM uint32_t CFG;
} EVENT_TIM_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup SKY66107_EVENT_TIM_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_TIM FLAG */
#define _EVENT_TIM_FLAG_RESETVALUE       0x00000000UL
#define _EVENT_TIM_FLAG_MASK             0x00001E5FUL
#define _EVENT_TIM_FLAG_RSVD_SHIFT       9
#define _EVENT_TIM_FLAG_RSVD_MASK        0x1E00UL
#define _EVENT_TIM_FLAG_RSVD_DEFAULT     0x00000000UL
#define EVENT_TIM_FLAG_RSVD_DEFAULT      (_EVENT_TIM_FLAG_RSVD_DEFAULT << 9)
#define EVENT_TIM_FLAG_TIF               (0x1UL << 6)
#define _EVENT_TIM_FLAG_TIF_SHIFT        6
#define _EVENT_TIM_FLAG_TIF_MASK         0x40UL
#define _EVENT_TIM_FLAG_TIF_DEFAULT      0x00000000UL
#define EVENT_TIM_FLAG_TIF_DEFAULT       (_EVENT_TIM_FLAG_TIF_DEFAULT << 6)
#define EVENT_TIM_FLAG_CC4IF             (0x1UL << 4)
#define _EVENT_TIM_FLAG_CC4IF_SHIFT      4
#define _EVENT_TIM_FLAG_CC4IF_MASK       0x10UL
#define _EVENT_TIM_FLAG_CC4IF_DEFAULT    0x00000000UL
#define EVENT_TIM_FLAG_CC4IF_DEFAULT     (_EVENT_TIM_FLAG_CC4IF_DEFAULT << 4)
#define EVENT_TIM_FLAG_CC3IF             (0x1UL << 3)
#define _EVENT_TIM_FLAG_CC3IF_SHIFT      3
#define _EVENT_TIM_FLAG_CC3IF_MASK       0x8UL
#define _EVENT_TIM_FLAG_CC3IF_DEFAULT    0x00000000UL
#define EVENT_TIM_FLAG_CC3IF_DEFAULT     (_EVENT_TIM_FLAG_CC3IF_DEFAULT << 3)
#define EVENT_TIM_FLAG_CC2IF             (0x1UL << 2)
#define _EVENT_TIM_FLAG_CC2IF_SHIFT      2
#define _EVENT_TIM_FLAG_CC2IF_MASK       0x4UL
#define _EVENT_TIM_FLAG_CC2IF_DEFAULT    0x00000000UL
#define EVENT_TIM_FLAG_CC2IF_DEFAULT     (_EVENT_TIM_FLAG_CC2IF_DEFAULT << 2)
#define EVENT_TIM_FLAG_CC1IF             (0x1UL << 1)
#define _EVENT_TIM_FLAG_CC1IF_SHIFT      1
#define _EVENT_TIM_FLAG_CC1IF_MASK       0x2UL
#define _EVENT_TIM_FLAG_CC1IF_DEFAULT    0x00000000UL
#define EVENT_TIM_FLAG_CC1IF_DEFAULT     (_EVENT_TIM_FLAG_CC1IF_DEFAULT << 1)
#define EVENT_TIM_FLAG_UIF               (0x1UL << 0)
#define _EVENT_TIM_FLAG_UIF_SHIFT        0
#define _EVENT_TIM_FLAG_UIF_MASK         0x1UL
#define _EVENT_TIM_FLAG_UIF_DEFAULT      0x00000000UL
#define EVENT_TIM_FLAG_UIF_DEFAULT       (_EVENT_TIM_FLAG_UIF_DEFAULT << 0)

/* Bit fields for EVENT_TIM MISS */
#define _EVENT_TIM_MISS_RESETVALUE           0x00000000UL
#define _EVENT_TIM_MISS_MASK                 0x00001E7FUL
#define EVENT_TIM_MISS_MISSCC4IF             (0x1UL << 12)
#define _EVENT_TIM_MISS_MISSCC4IF_SHIFT      12
#define _EVENT_TIM_MISS_MISSCC4IF_MASK       0x1000UL
#define _EVENT_TIM_MISS_MISSCC4IF_DEFAULT    0x00000000UL
#define EVENT_TIM_MISS_MISSCC4IF_DEFAULT     (_EVENT_TIM_MISS_MISSCC4IF_DEFAULT << 12)
#define EVENT_TIM_MISS_MISSCC3IF             (0x1UL << 11)
#define _EVENT_TIM_MISS_MISSCC3IF_SHIFT      11
#define _EVENT_TIM_MISS_MISSCC3IF_MASK       0x800UL
#define _EVENT_TIM_MISS_MISSCC3IF_DEFAULT    0x00000000UL
#define EVENT_TIM_MISS_MISSCC3IF_DEFAULT     (_EVENT_TIM_MISS_MISSCC3IF_DEFAULT << 11)
#define EVENT_TIM_MISS_MISSCC2IF             (0x1UL << 10)
#define _EVENT_TIM_MISS_MISSCC2IF_SHIFT      10
#define _EVENT_TIM_MISS_MISSCC2IF_MASK       0x400UL
#define _EVENT_TIM_MISS_MISSCC2IF_DEFAULT    0x00000000UL
#define EVENT_TIM_MISS_MISSCC2IF_DEFAULT     (_EVENT_TIM_MISS_MISSCC2IF_DEFAULT << 10)
#define EVENT_TIM_MISS_MISSCC1IF             (0x1UL << 9)
#define _EVENT_TIM_MISS_MISSCC1IF_SHIFT      9
#define _EVENT_TIM_MISS_MISSCC1IF_MASK       0x200UL
#define _EVENT_TIM_MISS_MISSCC1IF_DEFAULT    0x00000000UL
#define EVENT_TIM_MISS_MISSCC1IF_DEFAULT     (_EVENT_TIM_MISS_MISSCC1IF_DEFAULT << 9)
#define _EVENT_TIM_MISS_MISSRSVD_SHIFT       0
#define _EVENT_TIM_MISS_MISSRSVD_MASK        0x7FUL
#define _EVENT_TIM_MISS_MISSRSVD_DEFAULT     0x00000000UL
#define EVENT_TIM_MISS_MISSRSVD_DEFAULT      (_EVENT_TIM_MISS_MISSRSVD_DEFAULT << 0)

/* Bit fields for EVENT_TIM CFG */
#define _EVENT_TIM_CFG_RESETVALUE       0x00000000UL
#define _EVENT_TIM_CFG_MASK             0x0000005FUL
#define EVENT_TIM_CFG_TIF               (0x1UL << 6)
#define _EVENT_TIM_CFG_TIF_SHIFT        6
#define _EVENT_TIM_CFG_TIF_MASK         0x40UL
#define _EVENT_TIM_CFG_TIF_DEFAULT      0x00000000UL
#define EVENT_TIM_CFG_TIF_DEFAULT       (_EVENT_TIM_CFG_TIF_DEFAULT << 6)
#define EVENT_TIM_CFG_CC4IF             (0x1UL << 4)
#define _EVENT_TIM_CFG_CC4IF_SHIFT      4
#define _EVENT_TIM_CFG_CC4IF_MASK       0x10UL
#define _EVENT_TIM_CFG_CC4IF_DEFAULT    0x00000000UL
#define EVENT_TIM_CFG_CC4IF_DEFAULT     (_EVENT_TIM_CFG_CC4IF_DEFAULT << 4)
#define EVENT_TIM_CFG_CC3IF             (0x1UL << 3)
#define _EVENT_TIM_CFG_CC3IF_SHIFT      3
#define _EVENT_TIM_CFG_CC3IF_MASK       0x8UL
#define _EVENT_TIM_CFG_CC3IF_DEFAULT    0x00000000UL
#define EVENT_TIM_CFG_CC3IF_DEFAULT     (_EVENT_TIM_CFG_CC3IF_DEFAULT << 3)
#define EVENT_TIM_CFG_CC2IF             (0x1UL << 2)
#define _EVENT_TIM_CFG_CC2IF_SHIFT      2
#define _EVENT_TIM_CFG_CC2IF_MASK       0x4UL
#define _EVENT_TIM_CFG_CC2IF_DEFAULT    0x00000000UL
#define EVENT_TIM_CFG_CC2IF_DEFAULT     (_EVENT_TIM_CFG_CC2IF_DEFAULT << 2)
#define EVENT_TIM_CFG_CC1IF             (0x1UL << 1)
#define _EVENT_TIM_CFG_CC1IF_SHIFT      1
#define _EVENT_TIM_CFG_CC1IF_MASK       0x2UL
#define _EVENT_TIM_CFG_CC1IF_DEFAULT    0x00000000UL
#define EVENT_TIM_CFG_CC1IF_DEFAULT     (_EVENT_TIM_CFG_CC1IF_DEFAULT << 1)
#define EVENT_TIM_CFG_UIF               (0x1UL << 0)
#define _EVENT_TIM_CFG_UIF_SHIFT        0
#define _EVENT_TIM_CFG_UIF_MASK         0x1UL
#define _EVENT_TIM_CFG_UIF_DEFAULT      0x00000000UL
#define EVENT_TIM_CFG_UIF_DEFAULT       (_EVENT_TIM_CFG_UIF_DEFAULT << 0)

/** @} End of group SKY66107_EVENT_TIM_BitFields */

#endif // SKY66107_EVENT_TIM_H
