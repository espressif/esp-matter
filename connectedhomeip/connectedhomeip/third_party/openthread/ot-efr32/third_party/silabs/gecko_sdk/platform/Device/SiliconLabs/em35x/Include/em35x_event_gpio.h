/**************************************************************************//**
* @file
* @brief em35x_event_gpio Register and Bit Field definitions
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

#ifndef EM35X_EVENT_GPIO_H
#define EM35X_EVENT_GPIO_H

/**************************************************************************//**
 * @defgroup EM35X_EVENT_GPIO
 * @{
 * @brief EM35X_EVENT_GPIO Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[18];
  __IOM uint32_t CFGA;
  __IOM uint32_t CFGB;
  __IOM uint32_t CFGC;
  __IOM uint32_t CFGD;
} EVENT_GPIO_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_EVENT_GPIO_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_GPIO FLAG */
#define _EVENT_GPIO_FLAG_RESETVALUE      0x00000000UL
#define _EVENT_GPIO_FLAG_MASK            0x0000000FUL
#define EVENT_GPIO_FLAG_IRQD             (0x1UL << 3)
#define _EVENT_GPIO_FLAG_IRQD_SHIFT      3
#define _EVENT_GPIO_FLAG_IRQD_MASK       0x8UL
#define _EVENT_GPIO_FLAG_IRQD_DEFAULT    0x00000000UL
#define EVENT_GPIO_FLAG_IRQD_DEFAULT     (_EVENT_GPIO_FLAG_IRQD_DEFAULT << 3)
#define EVENT_GPIO_FLAG_IRQC             (0x1UL << 2)
#define _EVENT_GPIO_FLAG_IRQC_SHIFT      2
#define _EVENT_GPIO_FLAG_IRQC_MASK       0x4UL
#define _EVENT_GPIO_FLAG_IRQC_DEFAULT    0x00000000UL
#define EVENT_GPIO_FLAG_IRQC_DEFAULT     (_EVENT_GPIO_FLAG_IRQC_DEFAULT << 2)
#define EVENT_GPIO_FLAG_IRQB             (0x1UL << 1)
#define _EVENT_GPIO_FLAG_IRQB_SHIFT      1
#define _EVENT_GPIO_FLAG_IRQB_MASK       0x2UL
#define _EVENT_GPIO_FLAG_IRQB_DEFAULT    0x00000000UL
#define EVENT_GPIO_FLAG_IRQB_DEFAULT     (_EVENT_GPIO_FLAG_IRQB_DEFAULT << 1)
#define EVENT_GPIO_FLAG_IRQA             (0x1UL << 0)
#define _EVENT_GPIO_FLAG_IRQA_SHIFT      0
#define _EVENT_GPIO_FLAG_IRQA_MASK       0x1UL
#define _EVENT_GPIO_FLAG_IRQA_DEFAULT    0x00000000UL
#define EVENT_GPIO_FLAG_IRQA_DEFAULT     (_EVENT_GPIO_FLAG_IRQA_DEFAULT << 0)

/* Bit fields for EVENT_GPIO CFGA */
#define _EVENT_GPIO_CFGA_RESETVALUE      0x00000000UL
#define _EVENT_GPIO_CFGA_MASK            0x000001E0UL
#define EVENT_GPIO_CFGA_FILT             (0x1UL << 8)
#define _EVENT_GPIO_CFGA_FILT_SHIFT      8
#define _EVENT_GPIO_CFGA_FILT_MASK       0x100UL
#define _EVENT_GPIO_CFGA_FILT_DEFAULT    0x00000000UL
#define EVENT_GPIO_CFGA_FILT_DEFAULT     (_EVENT_GPIO_CFGA_FILT_DEFAULT << 8)
#define _EVENT_GPIO_CFGA_MOD_SHIFT       5
#define _EVENT_GPIO_CFGA_MOD_MASK        0xE0UL
#define _EVENT_GPIO_CFGA_MOD_DEFAULT     0x00000000UL
#define EVENT_GPIO_CFGA_MOD_DEFAULT      (_EVENT_GPIO_CFGA_MOD_DEFAULT << 5)

/* Bit fields for EVENT_GPIO CFGB */
#define _EVENT_GPIO_CFGB_RESETVALUE      0x00000000UL
#define _EVENT_GPIO_CFGB_MASK            0x000001E0UL
#define EVENT_GPIO_CFGB_FILT             (0x1UL << 8)
#define _EVENT_GPIO_CFGB_FILT_SHIFT      8
#define _EVENT_GPIO_CFGB_FILT_MASK       0x100UL
#define _EVENT_GPIO_CFGB_FILT_DEFAULT    0x00000000UL
#define EVENT_GPIO_CFGB_FILT_DEFAULT     (_EVENT_GPIO_CFGB_FILT_DEFAULT << 8)
#define _EVENT_GPIO_CFGB_MOD_SHIFT       5
#define _EVENT_GPIO_CFGB_MOD_MASK        0xE0UL
#define _EVENT_GPIO_CFGB_MOD_DEFAULT     0x00000000UL
#define EVENT_GPIO_CFGB_MOD_DEFAULT      (_EVENT_GPIO_CFGB_MOD_DEFAULT << 5)

/* Bit fields for EVENT_GPIO CFGC */
#define _EVENT_GPIO_CFGC_RESETVALUE      0x00000000UL
#define _EVENT_GPIO_CFGC_MASK            0x000001E0UL
#define EVENT_GPIO_CFGC_FILT             (0x1UL << 8)
#define _EVENT_GPIO_CFGC_FILT_SHIFT      8
#define _EVENT_GPIO_CFGC_FILT_MASK       0x100UL
#define _EVENT_GPIO_CFGC_FILT_DEFAULT    0x00000000UL
#define EVENT_GPIO_CFGC_FILT_DEFAULT     (_EVENT_GPIO_CFGC_FILT_DEFAULT << 8)
#define _EVENT_GPIO_CFGC_MOD_SHIFT       5
#define _EVENT_GPIO_CFGC_MOD_MASK        0xE0UL
#define _EVENT_GPIO_CFGC_MOD_DEFAULT     0x00000000UL
#define EVENT_GPIO_CFGC_MOD_DEFAULT      (_EVENT_GPIO_CFGC_MOD_DEFAULT << 5)

/* Bit fields for EVENT_GPIO CFGD */
#define _EVENT_GPIO_CFGD_RESETVALUE      0x00000000UL
#define _EVENT_GPIO_CFGD_MASK            0x000001E0UL
#define EVENT_GPIO_CFGD_FILT             (0x1UL << 8)
#define _EVENT_GPIO_CFGD_FILT_SHIFT      8
#define _EVENT_GPIO_CFGD_FILT_MASK       0x100UL
#define _EVENT_GPIO_CFGD_FILT_DEFAULT    0x00000000UL
#define EVENT_GPIO_CFGD_FILT_DEFAULT     (_EVENT_GPIO_CFGD_FILT_DEFAULT << 8)
#define _EVENT_GPIO_CFGD_MOD_SHIFT       5
#define _EVENT_GPIO_CFGD_MOD_MASK        0xE0UL
#define _EVENT_GPIO_CFGD_MOD_DEFAULT     0x00000000UL
#define EVENT_GPIO_CFGD_MOD_DEFAULT      (_EVENT_GPIO_CFGD_MOD_DEFAULT << 5)

/** @} End of group EM35X_EVENT_GPIO_BitFields */

#endif // EM35X_EVENT_GPIO_H

/**************************************************************************//**
 * @defgroup EM35X_EVENT_GPIO_CFGx_MOD_Values
 * @{
 *****************************************************************************/
/* Values for bit field EVENT_GPIO_CFGx_MOD */
#define _EVENT_GPIO_CFGx_MOD_DISABLED      (0x0U)                                                            /**< Mode DISABLED for CFGx_MOD */
#define EVENT_GPIO_CFGx_MOD_DISABLED       (_EVENT_GPIO_CFGx_MOD_DISABLED << _EVENT_GPIO_CFGA_MOD_SHIFT)     /**< Shifted mode DISABLED for CFGx_MOD */
#define _EVENT_GPIO_CFGx_MOD_RISING_EDGE   (0x1U)                                                            /**< Mode RISING_EDGE for CFGx_MOD */
#define EVENT_GPIO_CFGx_MOD_RISING_EDGE    (_EVENT_GPIO_CFGx_MOD_RISING_EDGE << _EVENT_GPIO_CFGA_MOD_SHIFT)  /**< Shifted mode RISING_EDGE for CFGx_MOD */
#define _EVENT_GPIO_CFGx_MOD_FALLING_EDGE  (0x2U)                                                            /**< Mode FALLING_EDGE for CFGx_MOD */
#define EVENT_GPIO_CFGx_MOD_FALLING_EDGE   (_EVENT_GPIO_CFGx_MOD_FALLING_EDGE << _EVENT_GPIO_CFGA_MOD_SHIFT) /**< Shifted mode FALLING_EDGE for CFGx_MOD */
#define _EVENT_GPIO_CFGx_MOD_BOTH_EDGES    (0x3U)                                                            /**< Mode BOTH_EDGES for CFGx_MOD */
#define EVENT_GPIO_CFGx_MOD_BOTH_EDGES     (_EVENT_GPIO_CFGx_MOD_BOTH_EDGES << _EVENT_GPIO_CFGA_MOD_SHIFT)   /**< Shifted mode BOTH_EDGES for CFGx_MOD */
#define _EVENT_GPIO_CFGx_MOD_HIGH_LEVEL    (0x4U)                                                            /**< Mode HIGH_LEVEL for CFGx_MOD */
#define EVENT_GPIO_CFGx_MOD_HIGH_LEVEL     (_EVENT_GPIO_CFGx_MOD_HIGH_LEVEL << _EVENT_GPIO_CFGA_MOD_SHIFT)   /**< Shifted mode HIGH_LEVEL for CFGx_MOD */
#define _EVENT_GPIO_CFGx_MOD_LOW_LEVEL     (0x5U)                                                            /**< Mode LOW_LEVEL for CFGx_MOD */
#define EVENT_GPIO_CFGx_MOD_LOW_LEVEL      (_EVENT_GPIO_CFGx_MOD_LOW_LEVEL << _EVENT_GPIO_CFGA_MOD_SHIFT)    /**< Shifted mode LOW_LEVEL for CFGx_MOD */

/** @} End of group EM35X_EVENT_GPIO_CFGx_MOD_Values */
