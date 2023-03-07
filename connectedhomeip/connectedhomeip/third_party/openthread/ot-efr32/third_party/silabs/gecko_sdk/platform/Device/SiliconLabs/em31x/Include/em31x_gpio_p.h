/**************************************************************************//**
* @file
* @brief em31x_gpio_p Register and Bit Field definitions
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

#ifndef EM31X_GPIO_P_H
#define EM31X_GPIO_P_H

/**************************************************************************//**
 * @defgroup EM31X_GPIO_P
 * @{
 * @brief EM31X_GPIO_P Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t CFGL;
  __IOM uint32_t CFGH;
  __IOM uint32_t IN;
  __IOM uint32_t OUT;
  __IOM uint32_t SET;
  __IOM uint32_t CLR;
  /*This RESERVED memory is necessary so all ports in "GPIO_P_TypeDef P[]" found in gpio.h align with the proper addresses.*/
  uint32_t RESERVED[250];
} GPIO_P_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM31X_GPIO_P_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for GPIO_P CFGL */
#define _GPIO_P_CFGL_RESETVALUE     0x00004444UL
#define _GPIO_P_CFGL_MASK           0x0000FFFFUL
#define _GPIO_P_CFGL_Px3_SHIFT      12
#define _GPIO_P_CFGL_Px3_MASK       0xF000UL
#define _GPIO_P_CFGL_Px3_DEFAULT    0x00000004UL
#define _GPIO_P_CFGL_Px3_OUT        0x00000001UL
#define _GPIO_P_CFGL_Px3_OUT_OD     0x00000005UL
#define _GPIO_P_CFGL_Px3_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGL_Px3_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGL_Px3_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGL_Px3_ANALOG     0x00000000UL
#define _GPIO_P_CFGL_Px3_IN         0x00000004UL
#define _GPIO_P_CFGL_Px3_SWDIO      0x00000006UL
#define _GPIO_P_CFGL_Px3_IN_PUD     0x00000008UL
#define GPIO_P_CFGL_Px3_DEFAULT     (_GPIO_P_CFGL_Px3_DEFAULT << 12)
#define GPIO_P_CFGL_Px3_OUT         (_GPIO_P_CFGL_Px3_OUT << 12)
#define GPIO_P_CFGL_Px3_OUT_OD      (_GPIO_P_CFGL_Px3_OUT_OD << 12)
#define GPIO_P_CFGL_Px3_OUT_ALT     (_GPIO_P_CFGL_Px3_OUT_ALT << 12)
#define GPIO_P_CFGL_Px3_OUT_ALT_OD    (_GPIO_P_CFGL_Px3_OUT_ALT_OD << 12)
#define GPIO_P_CFGL_Px3_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGL_Px3_OUT_ALT_SPI_SLAVE_MISO << 12)
#define GPIO_P_CFGL_Px3_ANALOG      (_GPIO_P_CFGL_Px3_ANALOG << 12)
#define GPIO_P_CFGL_Px3_IN          (_GPIO_P_CFGL_Px3_IN << 12)
#define GPIO_P_CFGL_Px3_SWDIO       (_GPIO_P_CFGL_Px3_SWDIO << 12)
#define GPIO_P_CFGL_Px3_IN_PUD      (_GPIO_P_CFGL_Px3_IN_PUD << 12)
#define _GPIO_P_CFGL_Px2_SHIFT      8
#define _GPIO_P_CFGL_Px2_MASK       0xF00UL
#define _GPIO_P_CFGL_Px2_DEFAULT    0x00000004UL
#define _GPIO_P_CFGL_Px2_OUT        0x00000001UL
#define _GPIO_P_CFGL_Px2_OUT_OD     0x00000005UL
#define _GPIO_P_CFGL_Px2_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGL_Px2_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGL_Px2_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGL_Px2_ANALOG     0x00000000UL
#define _GPIO_P_CFGL_Px2_IN         0x00000004UL
#define _GPIO_P_CFGL_Px2_SWDIO      0x00000006UL
#define _GPIO_P_CFGL_Px2_IN_PUD     0x00000008UL
#define GPIO_P_CFGL_Px2_DEFAULT     (_GPIO_P_CFGL_Px2_DEFAULT << 8)
#define GPIO_P_CFGL_Px2_OUT         (_GPIO_P_CFGL_Px2_OUT << 8)
#define GPIO_P_CFGL_Px2_OUT_OD      (_GPIO_P_CFGL_Px2_OUT_OD << 8)
#define GPIO_P_CFGL_Px2_OUT_ALT     (_GPIO_P_CFGL_Px2_OUT_ALT << 8)
#define GPIO_P_CFGL_Px2_OUT_ALT_OD    (_GPIO_P_CFGL_Px2_OUT_ALT_OD << 8)
#define GPIO_P_CFGL_Px2_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGL_Px2_OUT_ALT_SPI_SLAVE_MISO << 8)
#define GPIO_P_CFGL_Px2_ANALOG      (_GPIO_P_CFGL_Px2_ANALOG << 8)
#define GPIO_P_CFGL_Px2_IN          (_GPIO_P_CFGL_Px2_IN << 8)
#define GPIO_P_CFGL_Px2_SWDIO       (_GPIO_P_CFGL_Px2_SWDIO << 8)
#define GPIO_P_CFGL_Px2_IN_PUD      (_GPIO_P_CFGL_Px2_IN_PUD << 8)
#define _GPIO_P_CFGL_Px1_SHIFT      4
#define _GPIO_P_CFGL_Px1_MASK       0xF0UL
#define _GPIO_P_CFGL_Px1_DEFAULT    0x00000004UL
#define _GPIO_P_CFGL_Px1_OUT        0x00000001UL
#define _GPIO_P_CFGL_Px1_OUT_OD     0x00000005UL
#define _GPIO_P_CFGL_Px1_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGL_Px1_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGL_Px1_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGL_Px1_ANALOG     0x00000000UL
#define _GPIO_P_CFGL_Px1_IN         0x00000004UL
#define _GPIO_P_CFGL_Px1_SWDIO      0x00000006UL
#define _GPIO_P_CFGL_Px1_IN_PUD     0x00000008UL
#define GPIO_P_CFGL_Px1_DEFAULT     (_GPIO_P_CFGL_Px1_DEFAULT << 4)
#define GPIO_P_CFGL_Px1_OUT         (_GPIO_P_CFGL_Px1_OUT << 4)
#define GPIO_P_CFGL_Px1_OUT_OD      (_GPIO_P_CFGL_Px1_OUT_OD << 4)
#define GPIO_P_CFGL_Px1_OUT_ALT     (_GPIO_P_CFGL_Px1_OUT_ALT << 4)
#define GPIO_P_CFGL_Px1_OUT_ALT_OD    (_GPIO_P_CFGL_Px1_OUT_ALT_OD << 4)
#define GPIO_P_CFGL_Px1_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGL_Px1_OUT_ALT_SPI_SLAVE_MISO << 4)
#define GPIO_P_CFGL_Px1_ANALOG      (_GPIO_P_CFGL_Px1_ANALOG << 4)
#define GPIO_P_CFGL_Px1_IN          (_GPIO_P_CFGL_Px1_IN << 4)
#define GPIO_P_CFGL_Px1_SWDIO       (_GPIO_P_CFGL_Px1_SWDIO << 4)
#define GPIO_P_CFGL_Px1_IN_PUD      (_GPIO_P_CFGL_Px1_IN_PUD << 4)
#define _GPIO_P_CFGL_Px0_SHIFT      0
#define _GPIO_P_CFGL_Px0_MASK       0xFUL
#define _GPIO_P_CFGL_Px0_DEFAULT    0x00000004UL
#define _GPIO_P_CFGL_Px0_OUT        0x00000001UL
#define _GPIO_P_CFGL_Px0_OUT_OD     0x00000005UL
#define _GPIO_P_CFGL_Px0_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGL_Px0_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGL_Px0_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGL_Px0_ANALOG     0x00000000UL
#define _GPIO_P_CFGL_Px0_IN         0x00000004UL
#define _GPIO_P_CFGL_Px0_SWDIO      0x00000006UL
#define _GPIO_P_CFGL_Px0_IN_PUD     0x00000008UL
#define GPIO_P_CFGL_Px0_DEFAULT     (_GPIO_P_CFGL_Px0_DEFAULT << 0)
#define GPIO_P_CFGL_Px0_OUT         (_GPIO_P_CFGL_Px0_OUT << 0)
#define GPIO_P_CFGL_Px0_OUT_OD      (_GPIO_P_CFGL_Px0_OUT_OD << 0)
#define GPIO_P_CFGL_Px0_OUT_ALT     (_GPIO_P_CFGL_Px0_OUT_ALT << 0)
#define GPIO_P_CFGL_Px0_OUT_ALT_OD    (_GPIO_P_CFGL_Px0_OUT_ALT_OD << 0)
#define GPIO_P_CFGL_Px0_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGL_Px0_OUT_ALT_SPI_SLAVE_MISO << 0)
#define GPIO_P_CFGL_Px0_ANALOG      (_GPIO_P_CFGL_Px0_ANALOG << 0)
#define GPIO_P_CFGL_Px0_IN          (_GPIO_P_CFGL_Px0_IN << 0)
#define GPIO_P_CFGL_Px0_SWDIO       (_GPIO_P_CFGL_Px0_SWDIO << 0)
#define GPIO_P_CFGL_Px0_IN_PUD      (_GPIO_P_CFGL_Px0_IN_PUD << 0)

/* Bit fields for GPIO_P CFGH */
#define _GPIO_P_CFGH_RESETVALUE     0x00004444UL
#define _GPIO_P_CFGH_MASK           0x0000FFFFUL
#define _GPIO_P_CFGH_Px7_SHIFT      12
#define _GPIO_P_CFGH_Px7_MASK       0xF000UL
#define _GPIO_P_CFGH_Px7_DEFAULT    0x00000004UL
#define _GPIO_P_CFGH_Px7_OUT        0x00000001UL
#define _GPIO_P_CFGH_Px7_OUT_OD     0x00000005UL
#define _GPIO_P_CFGH_Px7_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGH_Px7_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGH_Px7_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGH_Px7_ANALOG     0x00000000UL
#define _GPIO_P_CFGH_Px7_IN         0x00000004UL
#define _GPIO_P_CFGH_Px7_SWDIO      0x00000006UL
#define _GPIO_P_CFGH_Px7_IN_PUD     0x00000008UL
#define GPIO_P_CFGH_Px7_DEFAULT     (_GPIO_P_CFGH_Px7_DEFAULT << 12)
#define GPIO_P_CFGH_Px7_OUT         (_GPIO_P_CFGH_Px7_OUT << 12)
#define GPIO_P_CFGH_Px7_OUT_OD      (_GPIO_P_CFGH_Px7_OUT_OD << 12)
#define GPIO_P_CFGH_Px7_OUT_ALT     (_GPIO_P_CFGH_Px7_OUT_ALT << 12)
#define GPIO_P_CFGH_Px7_OUT_ALT_OD    (_GPIO_P_CFGH_Px7_OUT_ALT_OD << 12)
#define GPIO_P_CFGH_Px7_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGH_Px7_OUT_ALT_SPI_SLAVE_MISO << 12)
#define GPIO_P_CFGH_Px7_ANALOG      (_GPIO_P_CFGH_Px7_ANALOG << 12)
#define GPIO_P_CFGH_Px7_IN          (_GPIO_P_CFGH_Px7_IN << 12)
#define GPIO_P_CFGH_Px7_SWDIO       (_GPIO_P_CFGH_Px7_SWDIO << 12)
#define GPIO_P_CFGH_Px7_IN_PUD      (_GPIO_P_CFGH_Px7_IN_PUD << 12)
#define _GPIO_P_CFGH_Px6_SHIFT      8
#define _GPIO_P_CFGH_Px6_MASK       0xF00UL
#define _GPIO_P_CFGH_Px6_DEFAULT    0x00000004UL
#define _GPIO_P_CFGH_Px6_OUT        0x00000001UL
#define _GPIO_P_CFGH_Px6_OUT_OD     0x00000005UL
#define _GPIO_P_CFGH_Px6_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGH_Px6_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGH_Px6_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGH_Px6_ANALOG     0x00000000UL
#define _GPIO_P_CFGH_Px6_IN         0x00000004UL
#define _GPIO_P_CFGH_Px6_SWDIO      0x00000006UL
#define _GPIO_P_CFGH_Px6_IN_PUD     0x00000008UL
#define GPIO_P_CFGH_Px6_DEFAULT     (_GPIO_P_CFGH_Px6_DEFAULT << 8)
#define GPIO_P_CFGH_Px6_OUT         (_GPIO_P_CFGH_Px6_OUT << 8)
#define GPIO_P_CFGH_Px6_OUT_OD      (_GPIO_P_CFGH_Px6_OUT_OD << 8)
#define GPIO_P_CFGH_Px6_OUT_ALT     (_GPIO_P_CFGH_Px6_OUT_ALT << 8)
#define GPIO_P_CFGH_Px6_OUT_ALT_OD    (_GPIO_P_CFGH_Px6_OUT_ALT_OD << 8)
#define GPIO_P_CFGH_Px6_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGH_Px6_OUT_ALT_SPI_SLAVE_MISO << 8)
#define GPIO_P_CFGH_Px6_ANALOG      (_GPIO_P_CFGH_Px6_ANALOG << 8)
#define GPIO_P_CFGH_Px6_IN          (_GPIO_P_CFGH_Px6_IN << 8)
#define GPIO_P_CFGH_Px6_SWDIO       (_GPIO_P_CFGH_Px6_SWDIO << 8)
#define GPIO_P_CFGH_Px6_IN_PUD      (_GPIO_P_CFGH_Px6_IN_PUD << 8)
#define _GPIO_P_CFGH_Px5_SHIFT      4
#define _GPIO_P_CFGH_Px5_MASK       0xF0UL
#define _GPIO_P_CFGH_Px5_DEFAULT    0x00000004UL
#define _GPIO_P_CFGH_Px5_OUT        0x00000001UL
#define _GPIO_P_CFGH_Px5_OUT_OD     0x00000005UL
#define _GPIO_P_CFGH_Px5_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGH_Px5_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGH_Px5_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGH_Px5_ANALOG     0x00000000UL
#define _GPIO_P_CFGH_Px5_IN         0x00000004UL
#define _GPIO_P_CFGH_Px5_SWDIO      0x00000006UL
#define _GPIO_P_CFGH_Px5_IN_PUD     0x00000008UL
#define GPIO_P_CFGH_Px5_DEFAULT     (_GPIO_P_CFGH_Px5_DEFAULT << 4)
#define GPIO_P_CFGH_Px5_OUT         (_GPIO_P_CFGH_Px5_OUT << 4)
#define GPIO_P_CFGH_Px5_OUT_OD      (_GPIO_P_CFGH_Px5_OUT_OD << 4)
#define GPIO_P_CFGH_Px5_OUT_ALT     (_GPIO_P_CFGH_Px5_OUT_ALT << 4)
#define GPIO_P_CFGH_Px5_OUT_ALT_OD    (_GPIO_P_CFGH_Px5_OUT_ALT_OD << 4)
#define GPIO_P_CFGH_Px5_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGH_Px5_OUT_ALT_SPI_SLAVE_MISO << 4)
#define GPIO_P_CFGH_Px5_ANALOG      (_GPIO_P_CFGH_Px5_ANALOG << 4)
#define GPIO_P_CFGH_Px5_IN          (_GPIO_P_CFGH_Px5_IN << 4)
#define GPIO_P_CFGH_Px5_SWDIO       (_GPIO_P_CFGH_Px5_SWDIO << 4)
#define GPIO_P_CFGH_Px5_IN_PUD      (_GPIO_P_CFGH_Px5_IN_PUD << 4)
#define _GPIO_P_CFGH_Px4_SHIFT      0
#define _GPIO_P_CFGH_Px4_MASK       0xFUL
#define _GPIO_P_CFGH_Px4_DEFAULT    0x00000004UL
#define _GPIO_P_CFGH_Px4_OUT        0x00000001UL
#define _GPIO_P_CFGH_Px4_OUT_OD     0x00000005UL
#define _GPIO_P_CFGH_Px4_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGH_Px4_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGH_Px4_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGH_Px4_ANALOG     0x00000000UL
#define _GPIO_P_CFGH_Px4_IN         0x00000004UL
#define _GPIO_P_CFGH_Px4_SWDIO      0x00000006UL
#define _GPIO_P_CFGH_Px4_IN_PUD     0x00000008UL
#define GPIO_P_CFGH_Px4_DEFAULT     (_GPIO_P_CFGH_Px4_DEFAULT << 0)
#define GPIO_P_CFGH_Px4_OUT         (_GPIO_P_CFGH_Px4_OUT << 0)
#define GPIO_P_CFGH_Px4_OUT_OD      (_GPIO_P_CFGH_Px4_OUT_OD << 0)
#define GPIO_P_CFGH_Px4_OUT_ALT     (_GPIO_P_CFGH_Px4_OUT_ALT << 0)
#define GPIO_P_CFGH_Px4_OUT_ALT_OD    (_GPIO_P_CFGH_Px4_OUT_ALT_OD << 0)
#define GPIO_P_CFGH_Px4_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGH_Px4_OUT_ALT_SPI_SLAVE_MISO << 0)
#define GPIO_P_CFGH_Px4_ANALOG      (_GPIO_P_CFGH_Px4_ANALOG << 0)
#define GPIO_P_CFGH_Px4_IN          (_GPIO_P_CFGH_Px4_IN << 0)
#define GPIO_P_CFGH_Px4_SWDIO       (_GPIO_P_CFGH_Px4_SWDIO << 0)
#define GPIO_P_CFGH_Px4_IN_PUD      (_GPIO_P_CFGH_Px4_IN_PUD << 0)

/* Bit fields for GPIO_P IN */
#define _GPIO_P_IN_RESETVALUE     0x00000000UL
#define _GPIO_P_IN_MASK           0x000000FFUL
#define GPIO_P_IN_Px7             (0x1UL << 7)
#define _GPIO_P_IN_Px7_SHIFT      7
#define _GPIO_P_IN_Px7_MASK       0x80UL
#define _GPIO_P_IN_Px7_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px7_DEFAULT     (_GPIO_P_IN_Px7_DEFAULT << 7)
#define GPIO_P_IN_Px6             (0x1UL << 6)
#define _GPIO_P_IN_Px6_SHIFT      6
#define _GPIO_P_IN_Px6_MASK       0x40UL
#define _GPIO_P_IN_Px6_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px6_DEFAULT     (_GPIO_P_IN_Px6_DEFAULT << 6)
#define GPIO_P_IN_Px5             (0x1UL << 5)
#define _GPIO_P_IN_Px5_SHIFT      5
#define _GPIO_P_IN_Px5_MASK       0x20UL
#define _GPIO_P_IN_Px5_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px5_DEFAULT     (_GPIO_P_IN_Px5_DEFAULT << 5)
#define GPIO_P_IN_Px4             (0x1UL << 4)
#define _GPIO_P_IN_Px4_SHIFT      4
#define _GPIO_P_IN_Px4_MASK       0x10UL
#define _GPIO_P_IN_Px4_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px4_DEFAULT     (_GPIO_P_IN_Px4_DEFAULT << 4)
#define GPIO_P_IN_Px3             (0x1UL << 3)
#define _GPIO_P_IN_Px3_SHIFT      3
#define _GPIO_P_IN_Px3_MASK       0x8UL
#define _GPIO_P_IN_Px3_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px3_DEFAULT     (_GPIO_P_IN_Px3_DEFAULT << 3)
#define GPIO_P_IN_Px2             (0x1UL << 2)
#define _GPIO_P_IN_Px2_SHIFT      2
#define _GPIO_P_IN_Px2_MASK       0x4UL
#define _GPIO_P_IN_Px2_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px2_DEFAULT     (_GPIO_P_IN_Px2_DEFAULT << 2)
#define GPIO_P_IN_Px1             (0x1UL << 1)
#define _GPIO_P_IN_Px1_SHIFT      1
#define _GPIO_P_IN_Px1_MASK       0x2UL
#define _GPIO_P_IN_Px1_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px1_DEFAULT     (_GPIO_P_IN_Px1_DEFAULT << 1)
#define GPIO_P_IN_Px0             (0x1UL << 0)
#define _GPIO_P_IN_Px0_SHIFT      0
#define _GPIO_P_IN_Px0_MASK       0x1UL
#define _GPIO_P_IN_Px0_DEFAULT    0x00000000UL
#define GPIO_P_IN_Px0_DEFAULT     (_GPIO_P_IN_Px0_DEFAULT << 0)

/* Bit fields for GPIO_P OUT */
#define _GPIO_P_OUT_RESETVALUE     0x00000000UL
#define _GPIO_P_OUT_MASK           0x000000FFUL
#define GPIO_P_OUT_Px7             (0x1UL << 7)
#define _GPIO_P_OUT_Px7_SHIFT      7
#define _GPIO_P_OUT_Px7_MASK       0x80UL
#define _GPIO_P_OUT_Px7_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px7_DEFAULT     (_GPIO_P_OUT_Px7_DEFAULT << 7)
#define GPIO_P_OUT_Px6             (0x1UL << 6)
#define _GPIO_P_OUT_Px6_SHIFT      6
#define _GPIO_P_OUT_Px6_MASK       0x40UL
#define _GPIO_P_OUT_Px6_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px6_DEFAULT     (_GPIO_P_OUT_Px6_DEFAULT << 6)
#define GPIO_P_OUT_Px5             (0x1UL << 5)
#define _GPIO_P_OUT_Px5_SHIFT      5
#define _GPIO_P_OUT_Px5_MASK       0x20UL
#define _GPIO_P_OUT_Px5_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px5_DEFAULT     (_GPIO_P_OUT_Px5_DEFAULT << 5)
#define GPIO_P_OUT_Px4             (0x1UL << 4)
#define _GPIO_P_OUT_Px4_SHIFT      4
#define _GPIO_P_OUT_Px4_MASK       0x10UL
#define _GPIO_P_OUT_Px4_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px4_DEFAULT     (_GPIO_P_OUT_Px4_DEFAULT << 4)
#define GPIO_P_OUT_Px3             (0x1UL << 3)
#define _GPIO_P_OUT_Px3_SHIFT      3
#define _GPIO_P_OUT_Px3_MASK       0x8UL
#define _GPIO_P_OUT_Px3_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px3_DEFAULT     (_GPIO_P_OUT_Px3_DEFAULT << 3)
#define GPIO_P_OUT_Px2             (0x1UL << 2)
#define _GPIO_P_OUT_Px2_SHIFT      2
#define _GPIO_P_OUT_Px2_MASK       0x4UL
#define _GPIO_P_OUT_Px2_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px2_DEFAULT     (_GPIO_P_OUT_Px2_DEFAULT << 2)
#define GPIO_P_OUT_Px1             (0x1UL << 1)
#define _GPIO_P_OUT_Px1_SHIFT      1
#define _GPIO_P_OUT_Px1_MASK       0x2UL
#define _GPIO_P_OUT_Px1_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px1_DEFAULT     (_GPIO_P_OUT_Px1_DEFAULT << 1)
#define GPIO_P_OUT_Px0             (0x1UL << 0)
#define _GPIO_P_OUT_Px0_SHIFT      0
#define _GPIO_P_OUT_Px0_MASK       0x1UL
#define _GPIO_P_OUT_Px0_DEFAULT    0x00000000UL
#define GPIO_P_OUT_Px0_DEFAULT     (_GPIO_P_OUT_Px0_DEFAULT << 0)

/* Bit fields for GPIO_P SET */
#define _GPIO_P_SET_RESETVALUE                0x00000000UL
#define _GPIO_P_SET_MASK                      0x0000FFFFUL
#define _GPIO_P_SET_GPIO_PXSETRSVD_SHIFT      8
#define _GPIO_P_SET_GPIO_PXSETRSVD_MASK       0xFF00UL
#define _GPIO_P_SET_GPIO_PXSETRSVD_DEFAULT    0x00000000UL
#define GPIO_P_SET_GPIO_PXSETRSVD_DEFAULT     (_GPIO_P_SET_GPIO_PXSETRSVD_DEFAULT << 8)
#define GPIO_P_SET_Px7                        (0x1UL << 7)
#define _GPIO_P_SET_Px7_SHIFT                 7
#define _GPIO_P_SET_Px7_MASK                  0x80UL
#define _GPIO_P_SET_Px7_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px7_DEFAULT                (_GPIO_P_SET_Px7_DEFAULT << 7)
#define GPIO_P_SET_Px6                        (0x1UL << 6)
#define _GPIO_P_SET_Px6_SHIFT                 6
#define _GPIO_P_SET_Px6_MASK                  0x40UL
#define _GPIO_P_SET_Px6_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px6_DEFAULT                (_GPIO_P_SET_Px6_DEFAULT << 6)
#define GPIO_P_SET_Px5                        (0x1UL << 5)
#define _GPIO_P_SET_Px5_SHIFT                 5
#define _GPIO_P_SET_Px5_MASK                  0x20UL
#define _GPIO_P_SET_Px5_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px5_DEFAULT                (_GPIO_P_SET_Px5_DEFAULT << 5)
#define GPIO_P_SET_Px4                        (0x1UL << 4)
#define _GPIO_P_SET_Px4_SHIFT                 4
#define _GPIO_P_SET_Px4_MASK                  0x10UL
#define _GPIO_P_SET_Px4_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px4_DEFAULT                (_GPIO_P_SET_Px4_DEFAULT << 4)
#define GPIO_P_SET_Px3                        (0x1UL << 3)
#define _GPIO_P_SET_Px3_SHIFT                 3
#define _GPIO_P_SET_Px3_MASK                  0x8UL
#define _GPIO_P_SET_Px3_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px3_DEFAULT                (_GPIO_P_SET_Px3_DEFAULT << 3)
#define GPIO_P_SET_Px2                        (0x1UL << 2)
#define _GPIO_P_SET_Px2_SHIFT                 2
#define _GPIO_P_SET_Px2_MASK                  0x4UL
#define _GPIO_P_SET_Px2_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px2_DEFAULT                (_GPIO_P_SET_Px2_DEFAULT << 2)
#define GPIO_P_SET_Px1                        (0x1UL << 1)
#define _GPIO_P_SET_Px1_SHIFT                 1
#define _GPIO_P_SET_Px1_MASK                  0x2UL
#define _GPIO_P_SET_Px1_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px1_DEFAULT                (_GPIO_P_SET_Px1_DEFAULT << 1)
#define GPIO_P_SET_Px0                        (0x1UL << 0)
#define _GPIO_P_SET_Px0_SHIFT                 0
#define _GPIO_P_SET_Px0_MASK                  0x1UL
#define _GPIO_P_SET_Px0_DEFAULT               0x00000000UL
#define GPIO_P_SET_Px0_DEFAULT                (_GPIO_P_SET_Px0_DEFAULT << 0)

/* Bit fields for GPIO_P CLR */
#define _GPIO_P_CLR_RESETVALUE     0x00000000UL
#define _GPIO_P_CLR_MASK           0x000000FFUL
#define GPIO_P_CLR_Px7             (0x1UL << 7)
#define _GPIO_P_CLR_Px7_SHIFT      7
#define _GPIO_P_CLR_Px7_MASK       0x80UL
#define _GPIO_P_CLR_Px7_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px7_DEFAULT     (_GPIO_P_CLR_Px7_DEFAULT << 7)
#define GPIO_P_CLR_Px6             (0x1UL << 6)
#define _GPIO_P_CLR_Px6_SHIFT      6
#define _GPIO_P_CLR_Px6_MASK       0x40UL
#define _GPIO_P_CLR_Px6_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px6_DEFAULT     (_GPIO_P_CLR_Px6_DEFAULT << 6)
#define GPIO_P_CLR_Px5             (0x1UL << 5)
#define _GPIO_P_CLR_Px5_SHIFT      5
#define _GPIO_P_CLR_Px5_MASK       0x20UL
#define _GPIO_P_CLR_Px5_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px5_DEFAULT     (_GPIO_P_CLR_Px5_DEFAULT << 5)
#define GPIO_P_CLR_Px4             (0x1UL << 4)
#define _GPIO_P_CLR_Px4_SHIFT      4
#define _GPIO_P_CLR_Px4_MASK       0x10UL
#define _GPIO_P_CLR_Px4_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px4_DEFAULT     (_GPIO_P_CLR_Px4_DEFAULT << 4)
#define GPIO_P_CLR_Px3             (0x1UL << 3)
#define _GPIO_P_CLR_Px3_SHIFT      3
#define _GPIO_P_CLR_Px3_MASK       0x8UL
#define _GPIO_P_CLR_Px3_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px3_DEFAULT     (_GPIO_P_CLR_Px3_DEFAULT << 3)
#define GPIO_P_CLR_Px2             (0x1UL << 2)
#define _GPIO_P_CLR_Px2_SHIFT      2
#define _GPIO_P_CLR_Px2_MASK       0x4UL
#define _GPIO_P_CLR_Px2_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px2_DEFAULT     (_GPIO_P_CLR_Px2_DEFAULT << 2)
#define GPIO_P_CLR_Px1             (0x1UL << 1)
#define _GPIO_P_CLR_Px1_SHIFT      1
#define _GPIO_P_CLR_Px1_MASK       0x2UL
#define _GPIO_P_CLR_Px1_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px1_DEFAULT     (_GPIO_P_CLR_Px1_DEFAULT << 1)
#define GPIO_P_CLR_Px0             (0x1UL << 0)
#define _GPIO_P_CLR_Px0_SHIFT      0
#define _GPIO_P_CLR_Px0_MASK       0x1UL
#define _GPIO_P_CLR_Px0_DEFAULT    0x00000000UL
#define GPIO_P_CLR_Px0_DEFAULT     (_GPIO_P_CLR_Px0_DEFAULT << 0)

/** @} End of group EM31X_GPIO_P_BitFields */

#endif // EM31X_GPIO_P_H

/**************************************************************************//**
 * @defgroup GPIO_P_OUT_Pxy_Values
 * @{
 *****************************************************************************/
/* Values for bit field GPIO_P_OUT_Pxy */
#define GPIO_P_OUT_Pxy_PULLUP      (0x1U)
#define GPIO_P_OUT_Pxy_PULLDOWN    (0x0U)

/** @} End of group GPIO_P_OUT_Pxy_Values */

/**************************************************************************//**
 * @defgroup GPIO_P_CFGz_Pxy_Values
 * @{
 *****************************************************************************/
/* Values for bit field GPIO_P_CFGz_Pxy */
#define GPIO_P_CFGz_Pxy             (0x1UL << 0)
#define _GPIO_P_CFGz_Pxy_SHIFT      0
#define _GPIO_P_CFGz_Pxy_MASK       0xFUL
#define _GPIO_P_CFGz_Pxy_DEFAULT    0x00000004UL
#define _GPIO_P_CFGz_Pxy_OUT        0x00000001UL
#define _GPIO_P_CFGz_Pxy_OUT_OD     0x00000005UL
#define _GPIO_P_CFGz_Pxy_OUT_ALT    0x00000009UL
#define _GPIO_P_CFGz_Pxy_OUT_ALT_OD    0x0000000DUL
#define _GPIO_P_CFGz_Pxy_OUT_ALT_SPI_SLAVE_MISO    0x0000000BUL
#define _GPIO_P_CFGz_Pxy_ANALOG     0x00000000UL
#define _GPIO_P_CFGz_Pxy_IN         0x00000004UL
#define _GPIO_P_CFGz_Pxy_SWDIO      0x00000006UL
#define _GPIO_P_CFGz_Pxy_IN_PUD     0x00000008UL
#define GPIO_P_CFGz_Pxy_DEFAULT     (_GPIO_P_CFGz_Pxy_DEFAULT << 0)
#define GPIO_P_CFGz_Pxy_OUT         (_GPIO_P_CFGz_Pxy_OUT << 0)
#define GPIO_P_CFGz_Pxy_OUT_OD      (_GPIO_P_CFGz_Pxy_OUT_OD << 0)
#define GPIO_P_CFGz_Pxy_OUT_ALT     (_GPIO_P_CFGz_Pxy_OUT_ALT << 0)
#define GPIO_P_CFGz_Pxy_OUT_ALT_OD    (_GPIO_P_CFGz_Pxy_OUT_ALT_OD << 0)
#define GPIO_P_CFGz_Pxy_OUT_ALT_SPI_SLAVE_MISO    (_GPIO_P_CFGz_Pxy_OUT_ALT_SPI_SLAVE_MISO << 0)
#define GPIO_P_CFGz_Pxy_ANALOG      (_GPIO_P_CFGz_Pxy_ANALOG << 0)
#define GPIO_P_CFGz_Pxy_IN          (_GPIO_P_CFGz_Pxy_IN << 0)
#define GPIO_P_CFGz_Pxy_SWDIO       (_GPIO_P_CFGz_Pxy_SWDIO << 0)
#define GPIO_P_CFGz_Pxy_IN_PUD      (_GPIO_P_CFGz_Pxy_IN_PUD << 0)

/** @} End of group GPIO_P_CFGz_Pxy_Values */
