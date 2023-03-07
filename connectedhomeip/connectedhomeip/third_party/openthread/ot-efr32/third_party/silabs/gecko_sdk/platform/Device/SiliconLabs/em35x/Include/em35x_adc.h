/**************************************************************************//**
* @file
* @brief em35x_adc Register and Bit Field definitions
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

#ifndef EM35X_ADC_H
#define EM35X_ADC_H

/**************************************************************************//**
 * @defgroup EM35X_ADC
 * @{
 * @brief EM35X_ADC Register Declaration
 *****************************************************************************/

typedef struct {
  __IM uint32_t DATA;
  __IOM uint32_t CFG;
  __IOM uint32_t OFFSET;
  __IOM uint32_t GAIN;
  __IOM uint32_t DMACFG;
  __IM uint32_t DMASTAT;
  __IOM uint32_t DMABEG;
  __IOM uint32_t DMASIZE;
  __IM uint32_t DMACUR;
  __IM uint32_t DMACNT;
} ADC_TypeDef;              /** @} */

/**************************************************************************//**
 * @defgroup EM35X_ADC_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for ADC DATA */
#define _ADC_DATA_RESETVALUE      0x00000000UL
#define _ADC_DATA_MASK            0x0000FFFFUL
#define _ADC_DATA_DATA_SHIFT      0
#define _ADC_DATA_DATA_MASK       0xFFFFUL
#define _ADC_DATA_DATA_DEFAULT    0x00000000UL
#define ADC_DATA_DATA_DEFAULT     (_ADC_DATA_DATA_DEFAULT << 0)

/* Bit fields for ADC CFG */
#define _ADC_CFG_RESETVALUE           0x00001800UL
#define _ADC_CFG_MASK                 0x0000FFFFUL
#define _ADC_CFG_PERIOD_SHIFT         13
#define _ADC_CFG_PERIOD_MASK          0xE000UL
#define _ADC_CFG_PERIOD_DEFAULT       0x00000000UL
#define ADC_CFG_PERIOD_DEFAULT        (_ADC_CFG_PERIOD_DEFAULT << 13)
#define ADC_CFG_CFGRSVD12             (0x1UL << 12)
#define _ADC_CFG_CFGRSVD12_SHIFT      12
#define _ADC_CFG_CFGRSVD12_MASK       0x1000UL
#define _ADC_CFG_CFGRSVD12_DEFAULT    0x00000001UL
#define ADC_CFG_CFGRSVD12_DEFAULT     (_ADC_CFG_CFGRSVD12_DEFAULT << 12)
#define ADC_CFG_CFGRSVD11             (0x1UL << 11)
#define _ADC_CFG_CFGRSVD11_SHIFT      11
#define _ADC_CFG_CFGRSVD11_MASK       0x800UL
#define _ADC_CFG_CFGRSVD11_DEFAULT    0x00000001UL
#define ADC_CFG_CFGRSVD11_DEFAULT     (_ADC_CFG_CFGRSVD11_DEFAULT << 11)
#define _ADC_CFG_MUXP_SHIFT           7
#define _ADC_CFG_MUXP_MASK            0x780UL
#define _ADC_CFG_MUXP_DEFAULT         0x00000000UL
#define ADC_CFG_MUXP_DEFAULT          (_ADC_CFG_MUXP_DEFAULT << 7)
#define _ADC_CFG_MUXN_SHIFT           3
#define _ADC_CFG_MUXN_MASK            0x78UL
#define _ADC_CFG_MUXN_DEFAULT         0x00000000UL
#define ADC_CFG_MUXN_DEFAULT          (_ADC_CFG_MUXN_DEFAULT << 3)
#define ADC_CFG_1MHZCLK               (0x1UL << 2)
#define _ADC_CFG_1MHZCLK_SHIFT        2
#define _ADC_CFG_1MHZCLK_MASK         0x4UL
#define _ADC_CFG_1MHZCLK_DEFAULT      0x00000000UL
#define ADC_CFG_1MHZCLK_DEFAULT       (_ADC_CFG_1MHZCLK_DEFAULT << 2)
#define ADC_CFG_CFGRSVD               (0x1UL << 1)
#define _ADC_CFG_CFGRSVD_SHIFT        1
#define _ADC_CFG_CFGRSVD_MASK         0x2UL
#define _ADC_CFG_CFGRSVD_DEFAULT      0x00000000UL
#define ADC_CFG_CFGRSVD_DEFAULT       (_ADC_CFG_CFGRSVD_DEFAULT << 1)
#define ADC_CFG_ENABLE                (0x1UL << 0)
#define _ADC_CFG_ENABLE_SHIFT         0
#define _ADC_CFG_ENABLE_MASK          0x1UL
#define _ADC_CFG_ENABLE_DEFAULT       0x00000000UL
#define ADC_CFG_ENABLE_DEFAULT        (_ADC_CFG_ENABLE_DEFAULT << 0)

/* Bit fields for ADC OFFSET */
#define _ADC_OFFSET_RESETVALUE        0x00000000UL
#define _ADC_OFFSET_MASK              0x0000FFFFUL
#define _ADC_OFFSET_OFFSET_SHIFT      0
#define _ADC_OFFSET_OFFSET_MASK       0xFFFFUL
#define _ADC_OFFSET_OFFSET_DEFAULT    0x00000000UL
#define ADC_OFFSET_OFFSET_DEFAULT     (_ADC_OFFSET_OFFSET_DEFAULT << 0)

/* Bit fields for ADC GAIN */
#define _ADC_GAIN_RESETVALUE      0x00008000UL
#define _ADC_GAIN_MASK            0x0000FFFFUL
#define _ADC_GAIN_GAIN_SHIFT      0
#define _ADC_GAIN_GAIN_MASK       0xFFFFUL
#define _ADC_GAIN_GAIN_DEFAULT    0x00008000UL
#define ADC_GAIN_GAIN_DEFAULT     (_ADC_GAIN_GAIN_DEFAULT << 0)

/* Bit fields for ADC DMACFG */
#define _ADC_DMACFG_RESETVALUE             0x00000000UL
#define _ADC_DMACFG_MASK                   0x00000013UL
#define ADC_DMACFG_DMARST                  (0x1UL << 4)
#define _ADC_DMACFG_DMARST_SHIFT           4
#define _ADC_DMACFG_DMARST_MASK            0x10UL
#define _ADC_DMACFG_DMARST_DEFAULT         0x00000000UL
#define ADC_DMACFG_DMARST_DEFAULT          (_ADC_DMACFG_DMARST_DEFAULT << 4)
#define ADC_DMACFG_DMAAUTOWRAP             (0x1UL << 1)
#define _ADC_DMACFG_DMAAUTOWRAP_SHIFT      1
#define _ADC_DMACFG_DMAAUTOWRAP_MASK       0x2UL
#define _ADC_DMACFG_DMAAUTOWRAP_DEFAULT    0x00000000UL
#define ADC_DMACFG_DMAAUTOWRAP_DEFAULT     (_ADC_DMACFG_DMAAUTOWRAP_DEFAULT << 1)
#define ADC_DMACFG_DMALOAD                 (0x1UL << 0)
#define _ADC_DMACFG_DMALOAD_SHIFT          0
#define _ADC_DMACFG_DMALOAD_MASK           0x1UL
#define _ADC_DMACFG_DMALOAD_DEFAULT        0x00000000UL
#define ADC_DMACFG_DMALOAD_DEFAULT         (_ADC_DMACFG_DMALOAD_DEFAULT << 0)

/* Bit fields for ADC DMASTAT */
#define _ADC_DMASTAT_RESETVALUE        0x00000000UL
#define _ADC_DMASTAT_MASK              0x00000003UL
#define ADC_DMASTAT_DMAOVF             (0x1UL << 1)
#define _ADC_DMASTAT_DMAOVF_SHIFT      1
#define _ADC_DMASTAT_DMAOVF_MASK       0x2UL
#define _ADC_DMASTAT_DMAOVF_DEFAULT    0x00000000UL
#define ADC_DMASTAT_DMAOVF_DEFAULT     (_ADC_DMASTAT_DMAOVF_DEFAULT << 1)
#define ADC_DMASTAT_DMAACT             (0x1UL << 0)
#define _ADC_DMASTAT_DMAACT_SHIFT      0
#define _ADC_DMASTAT_DMAACT_MASK       0x1UL
#define _ADC_DMASTAT_DMAACT_DEFAULT    0x00000000UL
#define ADC_DMASTAT_DMAACT_DEFAULT     (_ADC_DMASTAT_DMAACT_DEFAULT << 0)

/* Bit fields for ADC DMABEG */
#define _ADC_DMABEG_RESETVALUE             0x20000000UL
#define _ADC_DMABEG_MASK                   0xFFFFFFFFUL
#define _ADC_DMABEG_DMABEGFIXED_SHIFT      14
#define _ADC_DMABEG_DMABEGFIXED_MASK       0xFFFFC000UL
#define _ADC_DMABEG_DMABEGFIXED_DEFAULT    0x00008000UL
#define ADC_DMABEG_DMABEGFIXED_DEFAULT     (_ADC_DMABEG_DMABEGFIXED_DEFAULT << 14)
#define _ADC_DMABEG_DMABEG_SHIFT           0
#define _ADC_DMABEG_DMABEG_MASK            0x3FFFUL
#define _ADC_DMABEG_DMABEG_DEFAULT         0x00000000UL
#define ADC_DMABEG_DMABEG_DEFAULT          (_ADC_DMABEG_DMABEG_DEFAULT << 0)

/* Bit fields for ADC DMASIZE */
#define _ADC_DMASIZE_RESETVALUE         0x00000000UL
#define _ADC_DMASIZE_MASK               0x00001FFFUL
#define _ADC_DMASIZE_DMASIZE_SHIFT      0
#define _ADC_DMASIZE_DMASIZE_MASK       0x1FFFUL
#define _ADC_DMASIZE_DMASIZE_DEFAULT    0x00000000UL
#define ADC_DMASIZE_DMASIZE_DEFAULT     (_ADC_DMASIZE_DMASIZE_DEFAULT << 0)

/* Bit fields for ADC DMACUR */
#define _ADC_DMACUR_RESETVALUE             0x20000000UL
#define _ADC_DMACUR_MASK                   0xFFFFFFFFUL
#define _ADC_DMACUR_DMACURFIXED_SHIFT      14
#define _ADC_DMACUR_DMACURFIXED_MASK       0xFFFFC000UL
#define _ADC_DMACUR_DMACURFIXED_DEFAULT    0x00008000UL
#define ADC_DMACUR_DMACURFIXED_DEFAULT     (_ADC_DMACUR_DMACURFIXED_DEFAULT << 14)
#define _ADC_DMACUR_DMACUR_SHIFT           0
#define _ADC_DMACUR_DMACUR_MASK            0x3FFFUL
#define _ADC_DMACUR_DMACUR_DEFAULT         0x00000000UL
#define ADC_DMACUR_DMACUR_DEFAULT          (_ADC_DMACUR_DMACUR_DEFAULT << 0)

/* Bit fields for ADC DMACNT */
#define _ADC_DMACNT_RESETVALUE        0x00000000UL
#define _ADC_DMACNT_MASK              0x00001FFFUL
#define _ADC_DMACNT_DMACNT_SHIFT      0
#define _ADC_DMACNT_DMACNT_MASK       0x1FFFUL
#define _ADC_DMACNT_DMACNT_DEFAULT    0x00000000UL
#define ADC_DMACNT_DMACNT_DEFAULT     (_ADC_DMACNT_DMACNT_DEFAULT << 0)

/** @} End of group EM35X_ADC_BitFields */

#endif // EM35X_ADC_H
