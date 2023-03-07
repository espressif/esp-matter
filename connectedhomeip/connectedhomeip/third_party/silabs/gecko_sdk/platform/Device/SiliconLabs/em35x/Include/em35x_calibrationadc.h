/**************************************************************************//**
* @file
* @brief em35x_calibrationadc Register and Bit Field definitions
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

#ifndef EM35X_CALIBRATIONADC_H
#define EM35X_CALIBRATIONADC_H

/**************************************************************************//**
 * @defgroup EM35X_CALIBRATIONADC
 * @{
 * @brief EM35X_CALIBRATIONADC Register Declaration
 *****************************************************************************/

typedef struct {
  __IM uint32_t DATA;
  __IOM uint32_t CONFIG;
} CALIBRATIONADC_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_CALIBRATIONADC_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for CALIBRATIONADC DATA */
#define _CALIBRATIONADC_DATA_RESETVALUE      0x00000000UL
#define _CALIBRATIONADC_DATA_MASK            0x0000FFFFUL
#define _CALIBRATIONADC_DATA_DATA_SHIFT      0
#define _CALIBRATIONADC_DATA_DATA_MASK       0xFFFFUL
#define _CALIBRATIONADC_DATA_DATA_DEFAULT    0x00000000UL
#define CALIBRATIONADC_DATA_DATA_DEFAULT     (_CALIBRATIONADC_DATA_DATA_DEFAULT << 0)

/* Bit fields for CALIBRATIONADC CONFIG */
#define _CALIBRATIONADC_CONFIG_RESETVALUE           0x00000000UL
#define _CALIBRATIONADC_CONFIG_MASK                 0x00007F87UL
#define _CALIBRATIONADC_CONFIG_RATE_SHIFT           12
#define _CALIBRATIONADC_CONFIG_RATE_MASK            0x7000UL
#define _CALIBRATIONADC_CONFIG_RATE_DEFAULT         0x00000000UL
#define CALIBRATIONADC_CONFIG_RATE_DEFAULT          (_CALIBRATIONADC_CONFIG_RATE_DEFAULT << 12)
#define _CALIBRATIONADC_CONFIG_MUX_SHIFT            7
#define _CALIBRATIONADC_CONFIG_MUX_MASK             0xF80UL
#define _CALIBRATIONADC_CONFIG_MUX_DEFAULT          0x00000000UL
#define CALIBRATIONADC_CONFIG_MUX_DEFAULT           (_CALIBRATIONADC_CONFIG_MUX_DEFAULT << 7)
#define CALIBRATIONADC_CONFIG_CLKSEL                (0x1UL << 2)
#define _CALIBRATIONADC_CONFIG_CLKSEL_SHIFT         2
#define _CALIBRATIONADC_CONFIG_CLKSEL_MASK          0x4UL
#define _CALIBRATIONADC_CONFIG_CLKSEL_DEFAULT       0x00000000UL
#define CALIBRATIONADC_CONFIG_CLKSEL_DEFAULT        (_CALIBRATIONADC_CONFIG_CLKSEL_DEFAULT << 2)
#define CALIBRATIONADC_CONFIG_DITHERDIS             (0x1UL << 1)
#define _CALIBRATIONADC_CONFIG_DITHERDIS_SHIFT      1
#define _CALIBRATIONADC_CONFIG_DITHERDIS_MASK       0x2UL
#define _CALIBRATIONADC_CONFIG_DITHERDIS_DEFAULT    0x00000000UL
#define CALIBRATIONADC_CONFIG_DITHERDIS_DEFAULT     (_CALIBRATIONADC_CONFIG_DITHERDIS_DEFAULT << 1)
#define CALIBRATIONADC_CONFIG_EN                    (0x1UL << 0)
#define _CALIBRATIONADC_CONFIG_EN_SHIFT             0
#define _CALIBRATIONADC_CONFIG_EN_MASK              0x1UL
#define _CALIBRATIONADC_CONFIG_EN_DEFAULT           0x00000000UL
#define CALIBRATIONADC_CONFIG_EN_DEFAULT            (_CALIBRATIONADC_CONFIG_EN_DEFAULT << 0)

/** @} End of group EM35X_CALIBRATIONADC_BitFields */

#endif // EM35X_CALIBRATIONADC_H
