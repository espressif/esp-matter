/**************************************************************************//**
* @file
* @brief em355x_event_adc Register and Bit Field definitions
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

#ifndef EM355X_EVENT_ADC_H
#define EM355X_EVENT_ADC_H

/**************************************************************************//**
 * @defgroup EM355X_EVENT_ADC
 * @{
 * @brief EM355X_EVENT_ADC Register Declaration
 *****************************************************************************/

typedef struct {
  __IOM uint32_t FLAG;
  uint32_t RESERVED0[15];
  __IOM uint32_t CFG;
} EVENT_ADC_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM355X_EVENT_ADC_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for EVENT_ADC FLAG */
#define _EVENT_ADC_FLAG_RESETVALUE         0x00000000UL
#define _EVENT_ADC_FLAG_MASK               0x0000001FUL
#define EVENT_ADC_FLAG_OVF                 (0x1UL << 4)
#define _EVENT_ADC_FLAG_OVF_SHIFT          4
#define _EVENT_ADC_FLAG_OVF_MASK           0x10UL
#define _EVENT_ADC_FLAG_OVF_DEFAULT        0x00000000UL
#define EVENT_ADC_FLAG_OVF_DEFAULT         (_EVENT_ADC_FLAG_OVF_DEFAULT << 4)
#define EVENT_ADC_FLAG_SAT                 (0x1UL << 3)
#define _EVENT_ADC_FLAG_SAT_SHIFT          3
#define _EVENT_ADC_FLAG_SAT_MASK           0x8UL
#define _EVENT_ADC_FLAG_SAT_DEFAULT        0x00000000UL
#define EVENT_ADC_FLAG_SAT_DEFAULT         (_EVENT_ADC_FLAG_SAT_DEFAULT << 3)
#define EVENT_ADC_FLAG_ULDFULL             (0x1UL << 2)
#define _EVENT_ADC_FLAG_ULDFULL_SHIFT      2
#define _EVENT_ADC_FLAG_ULDFULL_MASK       0x4UL
#define _EVENT_ADC_FLAG_ULDFULL_DEFAULT    0x00000000UL
#define EVENT_ADC_FLAG_ULDFULL_DEFAULT     (_EVENT_ADC_FLAG_ULDFULL_DEFAULT << 2)
#define EVENT_ADC_FLAG_ULDHALF             (0x1UL << 1)
#define _EVENT_ADC_FLAG_ULDHALF_SHIFT      1
#define _EVENT_ADC_FLAG_ULDHALF_MASK       0x2UL
#define _EVENT_ADC_FLAG_ULDHALF_DEFAULT    0x00000000UL
#define EVENT_ADC_FLAG_ULDHALF_DEFAULT     (_EVENT_ADC_FLAG_ULDHALF_DEFAULT << 1)
#define EVENT_ADC_FLAG_DATA                (0x1UL << 0)
#define _EVENT_ADC_FLAG_DATA_SHIFT         0
#define _EVENT_ADC_FLAG_DATA_MASK          0x1UL
#define _EVENT_ADC_FLAG_DATA_DEFAULT       0x00000000UL
#define EVENT_ADC_FLAG_DATA_DEFAULT        (_EVENT_ADC_FLAG_DATA_DEFAULT << 0)

/* Bit fields for EVENT_ADC CFG */
#define _EVENT_ADC_CFG_RESETVALUE         0x00000000UL
#define _EVENT_ADC_CFG_MASK               0x0000001FUL
#define EVENT_ADC_CFG_OVF                 (0x1UL << 4)
#define _EVENT_ADC_CFG_OVF_SHIFT          4
#define _EVENT_ADC_CFG_OVF_MASK           0x10UL
#define _EVENT_ADC_CFG_OVF_DEFAULT        0x00000000UL
#define EVENT_ADC_CFG_OVF_DEFAULT         (_EVENT_ADC_CFG_OVF_DEFAULT << 4)
#define EVENT_ADC_CFG_SAT                 (0x1UL << 3)
#define _EVENT_ADC_CFG_SAT_SHIFT          3
#define _EVENT_ADC_CFG_SAT_MASK           0x8UL
#define _EVENT_ADC_CFG_SAT_DEFAULT        0x00000000UL
#define EVENT_ADC_CFG_SAT_DEFAULT         (_EVENT_ADC_CFG_SAT_DEFAULT << 3)
#define EVENT_ADC_CFG_ULDFULL             (0x1UL << 2)
#define _EVENT_ADC_CFG_ULDFULL_SHIFT      2
#define _EVENT_ADC_CFG_ULDFULL_MASK       0x4UL
#define _EVENT_ADC_CFG_ULDFULL_DEFAULT    0x00000000UL
#define EVENT_ADC_CFG_ULDFULL_DEFAULT     (_EVENT_ADC_CFG_ULDFULL_DEFAULT << 2)
#define EVENT_ADC_CFG_ULDHALF             (0x1UL << 1)
#define _EVENT_ADC_CFG_ULDHALF_SHIFT      1
#define _EVENT_ADC_CFG_ULDHALF_MASK       0x2UL
#define _EVENT_ADC_CFG_ULDHALF_DEFAULT    0x00000000UL
#define EVENT_ADC_CFG_ULDHALF_DEFAULT     (_EVENT_ADC_CFG_ULDHALF_DEFAULT << 1)
#define EVENT_ADC_CFG_DATA                (0x1UL << 0)
#define _EVENT_ADC_CFG_DATA_SHIFT         0
#define _EVENT_ADC_CFG_DATA_MASK          0x1UL
#define _EVENT_ADC_CFG_DATA_DEFAULT       0x00000000UL
#define EVENT_ADC_CFG_DATA_DEFAULT        (_EVENT_ADC_CFG_DATA_DEFAULT << 0)

/** @} End of group EM355X_EVENT_ADC_BitFields */

#endif // EM355X_EVENT_ADC_H
