/**************************************************************************//**
* @file
* @brief em355x_gpio Register and Bit Field definitions
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

#ifndef EM355X_GPIO_H
#define EM355X_GPIO_H

/**************************************************************************//**
 * @defgroup EM355X_GPIO
 * @{
 * @brief EM355X_GPIO Register Declaration
 *****************************************************************************/

typedef struct {
  GPIO_P_TypeDef P[3];          /**< Port configuration bits */
  uint32_t RESERVED0[384];
  __IOM uint32_t DBGCFG;
  __IM uint32_t DBGSTAT;
  __IOM uint32_t WAKE[3];
  uint32_t RESERVED1[3];
  __IOM uint32_t IRQCSEL;
  __IOM uint32_t IRQDSEL;
  __IOM uint32_t WAKEFILT;
  __IOM uint32_t DBGCFG2;
} GPIO_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM355X_GPIO_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for GPIO DBGCFG */
#define _GPIO_DBGCFG_RESETVALUE            0x00000010UL
#define _GPIO_DBGCFG_MASK                  0x00000038UL
#define GPIO_DBGCFG_DEBUGDIS               (0x1UL << 5)
#define _GPIO_DBGCFG_DEBUGDIS_SHIFT        5
#define _GPIO_DBGCFG_DEBUGDIS_MASK         0x20UL
#define _GPIO_DBGCFG_DEBUGDIS_DEFAULT      0x00000000UL
#define GPIO_DBGCFG_DEBUGDIS_DEFAULT       (_GPIO_DBGCFG_DEBUGDIS_DEFAULT << 5)
#define GPIO_DBGCFG_EXTREGEN               (0x1UL << 4)
#define _GPIO_DBGCFG_EXTREGEN_SHIFT        4
#define _GPIO_DBGCFG_EXTREGEN_MASK         0x10UL
#define _GPIO_DBGCFG_EXTREGEN_DEFAULT      0x00000001UL
#define GPIO_DBGCFG_EXTREGEN_DEFAULT       (_GPIO_DBGCFG_EXTREGEN_DEFAULT << 4)
#define GPIO_DBGCFG_DBGCFGRSVD             (0x1UL << 3)
#define _GPIO_DBGCFG_DBGCFGRSVD_SHIFT      3
#define _GPIO_DBGCFG_DBGCFGRSVD_MASK       0x8UL
#define _GPIO_DBGCFG_DBGCFGRSVD_DEFAULT    0x00000000UL
#define GPIO_DBGCFG_DBGCFGRSVD_DEFAULT     (_GPIO_DBGCFG_DBGCFGRSVD_DEFAULT << 3)

/* Bit fields for GPIO DBGSTAT */
#define _GPIO_DBGSTAT_RESETVALUE          0x00000000UL
#define _GPIO_DBGSTAT_MASK                0x0000000BUL
#define GPIO_DBGSTAT_BOOTMODE             (0x1UL << 3)
#define _GPIO_DBGSTAT_BOOTMODE_SHIFT      3
#define _GPIO_DBGSTAT_BOOTMODE_MASK       0x8UL
#define _GPIO_DBGSTAT_BOOTMODE_DEFAULT    0x00000000UL
#define GPIO_DBGSTAT_BOOTMODE_DEFAULT     (_GPIO_DBGSTAT_BOOTMODE_DEFAULT << 3)
#define GPIO_DBGSTAT_FORCEDBG             (0x1UL << 1)
#define _GPIO_DBGSTAT_FORCEDBG_SHIFT      1
#define _GPIO_DBGSTAT_FORCEDBG_MASK       0x2UL
#define _GPIO_DBGSTAT_FORCEDBG_DEFAULT    0x00000000UL
#define GPIO_DBGSTAT_FORCEDBG_DEFAULT     (_GPIO_DBGSTAT_FORCEDBG_DEFAULT << 1)
#define GPIO_DBGSTAT_SWEN                 (0x1UL << 0)
#define _GPIO_DBGSTAT_SWEN_SHIFT          0
#define _GPIO_DBGSTAT_SWEN_MASK           0x1UL
#define _GPIO_DBGSTAT_SWEN_DEFAULT        0x00000000UL
#define GPIO_DBGSTAT_SWEN_DEFAULT         (_GPIO_DBGSTAT_SWEN_DEFAULT << 0)

/* Bit fields for GPIO WAKE */
#define _GPIO_WAKE_RESETVALUE     0x00000000UL
#define _GPIO_WAKE_MASK           0x000000FFUL
#define GPIO_WAKE_Px7             (0x1UL << 7)
#define _GPIO_WAKE_Px7_SHIFT      7
#define _GPIO_WAKE_Px7_MASK       0x80UL
#define _GPIO_WAKE_Px7_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px7_DEFAULT     (_GPIO_WAKE_Px7_DEFAULT << 7)
#define GPIO_WAKE_Px6             (0x1UL << 6)
#define _GPIO_WAKE_Px6_SHIFT      6
#define _GPIO_WAKE_Px6_MASK       0x40UL
#define _GPIO_WAKE_Px6_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px6_DEFAULT     (_GPIO_WAKE_Px6_DEFAULT << 6)
#define GPIO_WAKE_Px5             (0x1UL << 5)
#define _GPIO_WAKE_Px5_SHIFT      5
#define _GPIO_WAKE_Px5_MASK       0x20UL
#define _GPIO_WAKE_Px5_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px5_DEFAULT     (_GPIO_WAKE_Px5_DEFAULT << 5)
#define GPIO_WAKE_Px4             (0x1UL << 4)
#define _GPIO_WAKE_Px4_SHIFT      4
#define _GPIO_WAKE_Px4_MASK       0x10UL
#define _GPIO_WAKE_Px4_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px4_DEFAULT     (_GPIO_WAKE_Px4_DEFAULT << 4)
#define GPIO_WAKE_Px3             (0x1UL << 3)
#define _GPIO_WAKE_Px3_SHIFT      3
#define _GPIO_WAKE_Px3_MASK       0x8UL
#define _GPIO_WAKE_Px3_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px3_DEFAULT     (_GPIO_WAKE_Px3_DEFAULT << 3)
#define GPIO_WAKE_Px2             (0x1UL << 2)
#define _GPIO_WAKE_Px2_SHIFT      2
#define _GPIO_WAKE_Px2_MASK       0x4UL
#define _GPIO_WAKE_Px2_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px2_DEFAULT     (_GPIO_WAKE_Px2_DEFAULT << 2)
#define GPIO_WAKE_Px1             (0x1UL << 1)
#define _GPIO_WAKE_Px1_SHIFT      1
#define _GPIO_WAKE_Px1_MASK       0x2UL
#define _GPIO_WAKE_Px1_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px1_DEFAULT     (_GPIO_WAKE_Px1_DEFAULT << 1)
#define GPIO_WAKE_Px0             (0x1UL << 0)
#define _GPIO_WAKE_Px0_SHIFT      0
#define _GPIO_WAKE_Px0_MASK       0x1UL
#define _GPIO_WAKE_Px0_DEFAULT    0x00000000UL
#define GPIO_WAKE_Px0_DEFAULT     (_GPIO_WAKE_Px0_DEFAULT << 0)

/* Bit fields for GPIO IRQCSEL */
#define _GPIO_IRQCSEL_RESETVALUE          0x0000000FUL
#define _GPIO_IRQCSEL_MASK                0x0000003FUL
#define _GPIO_IRQCSEL_SEL_GPIO_SHIFT      0
#define _GPIO_IRQCSEL_SEL_GPIO_MASK       0x3FUL
#define _GPIO_IRQCSEL_SEL_GPIO_DEFAULT    0x0000000FUL
#define GPIO_IRQCSEL_SEL_GPIO_DEFAULT     (_GPIO_IRQCSEL_SEL_GPIO_DEFAULT << 0)

/* Bit fields for GPIO IRQDSEL */
#define _GPIO_IRQDSEL_RESETVALUE          0x00000010UL
#define _GPIO_IRQDSEL_MASK                0x0000003FUL
#define _GPIO_IRQDSEL_SEL_GPIO_SHIFT      0
#define _GPIO_IRQDSEL_SEL_GPIO_MASK       0x3FUL
#define _GPIO_IRQDSEL_SEL_GPIO_DEFAULT    0x00000010UL
#define GPIO_IRQDSEL_SEL_GPIO_DEFAULT     (_GPIO_IRQDSEL_SEL_GPIO_DEFAULT << 0)

/* Bit fields for GPIO WAKEFILT */
#define _GPIO_WAKEFILT_RESETVALUE      0x00000000UL
#define _GPIO_WAKEFILT_MASK            0x0000000FUL
#define GPIO_WAKEFILT_IRQD             (0x1UL << 3)
#define _GPIO_WAKEFILT_IRQD_SHIFT      3
#define _GPIO_WAKEFILT_IRQD_MASK       0x8UL
#define _GPIO_WAKEFILT_IRQD_DEFAULT    0x00000000UL
#define GPIO_WAKEFILT_IRQD_DEFAULT     (_GPIO_WAKEFILT_IRQD_DEFAULT << 3)
#define GPIO_WAKEFILT_SC2              (0x1UL << 2)
#define _GPIO_WAKEFILT_SC2_SHIFT       2
#define _GPIO_WAKEFILT_SC2_MASK        0x4UL
#define _GPIO_WAKEFILT_SC2_DEFAULT     0x00000000UL
#define GPIO_WAKEFILT_SC2_DEFAULT      (_GPIO_WAKEFILT_SC2_DEFAULT << 2)
#define GPIO_WAKEFILT_SC1              (0x1UL << 1)
#define _GPIO_WAKEFILT_SC1_SHIFT       1
#define _GPIO_WAKEFILT_SC1_MASK        0x2UL
#define _GPIO_WAKEFILT_SC1_DEFAULT     0x00000000UL
#define GPIO_WAKEFILT_SC1_DEFAULT      (_GPIO_WAKEFILT_SC1_DEFAULT << 1)
#define GPIO_WAKEFILT_GPIO             (0x1UL << 0)
#define _GPIO_WAKEFILT_GPIO_SHIFT      0
#define _GPIO_WAKEFILT_GPIO_MASK       0x1UL
#define _GPIO_WAKEFILT_GPIO_DEFAULT    0x00000000UL
#define GPIO_WAKEFILT_GPIO_DEFAULT     (_GPIO_WAKEFILT_GPIO_DEFAULT << 0)

/* Bit fields for GPIO DBGCFG2 */
#define _GPIO_DBGCFG2_RESETVALUE            0x00000000UL
#define _GPIO_DBGCFG2_MASK                  0x00000003UL
#define GPIO_DBGCFG2_USBMON_EN              (0x1UL << 1)
#define _GPIO_DBGCFG2_USBMON_EN_SHIFT       1
#define _GPIO_DBGCFG2_USBMON_EN_MASK        0x2UL
#define _GPIO_DBGCFG2_USBMON_EN_DEFAULT     0x00000000UL
#define GPIO_DBGCFG2_USBMON_EN_DEFAULT      (_GPIO_DBGCFG2_USBMON_EN_DEFAULT << 1)
#define GPIO_DBGCFG2_USBSTIM_EN             (0x1UL << 0)
#define _GPIO_DBGCFG2_USBSTIM_EN_SHIFT      0
#define _GPIO_DBGCFG2_USBSTIM_EN_MASK       0x1UL
#define _GPIO_DBGCFG2_USBSTIM_EN_DEFAULT    0x00000000UL
#define GPIO_DBGCFG2_USBSTIM_EN_DEFAULT     (_GPIO_DBGCFG2_USBSTIM_EN_DEFAULT << 0)

/** @} End of group EM355X_GPIO_BitFields */

#endif // EM355X_GPIO_H
