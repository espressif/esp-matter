/*
** ###################################################################
**     Version:             rev. 1.0, 2020-09-07
**     Build:               b201223
**
**     Abstract:
**         Chip specific module features.
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2020 NXP
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2020-09-07)
**         Initial version.
**
** ###################################################################
*/

#ifndef _88MW320_FEATURES_H_
#define _88MW320_FEATURES_H_

/* SOC module features */

/* @brief ACOMP availability on the SoC. */
#define FSL_FEATURE_SOC_ACOMP_COUNT (1)
/* @brief MW_ADC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_ADC_COUNT (1)
/* @brief MW_AES availability on the SoC. */
#define FSL_FEATURE_SOC_MW_AES_COUNT (1)
/* @brief MW_BG availability on the SoC. */
#define FSL_FEATURE_SOC_MW_BG_COUNT (1)
/* @brief MW_CRC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_CRC_COUNT (1)
/* @brief MW_DAC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_DAC_COUNT (1)
/* @brief MW_DMAC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_DMAC_COUNT (1)
/* @brief MW_FLASHC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_FLASHC_COUNT (1)
/* @brief MW_GPIO availability on the SoC. */
#define FSL_FEATURE_SOC_MW_GPIO_COUNT (1)
/* @brief MW_GPT availability on the SoC. */
#define FSL_FEATURE_SOC_MW_GPT_COUNT (4)
/* @brief MW_I2C availability on the SoC. */
#define FSL_FEATURE_SOC_MW_I2C_COUNT (2)
/* @brief MW_PMU availability on the SoC. */
#define FSL_FEATURE_SOC_MW_PMU_COUNT (1)
/* @brief MW_QSPI availability on the SoC. */
#define FSL_FEATURE_SOC_MW_QSPI_COUNT (1)
/* @brief MW_PINMUX availability on the SoC. */
#define FSL_FEATURE_SOC_MW_PINMUX_COUNT (1)
/* @brief MW_RC32 availability on the SoC. */
#define FSL_FEATURE_SOC_MW_RC32_COUNT (1)
/* @brief MW_RTC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_RTC_COUNT (1)
/* @brief MW_SSP availability on the SoC. */
#define FSL_FEATURE_SOC_MW_SSP_COUNT (3)
/* @brief MW_SYS_CTL availability on the SoC. */
#define FSL_FEATURE_SOC_MW_SYS_CTL_COUNT (1)
/* @brief MW_UART availability on the SoC. */
#define FSL_FEATURE_SOC_MW_UART_COUNT (3)
/* @brief MW_USBC availability on the SoC. */
#define FSL_FEATURE_SOC_MW_USBC_COUNT (1)
/* @brief MW_WDT availability on the SoC. */
#define FSL_FEATURE_SOC_MW_WDT_COUNT (1)
/* @brief SDIOC availability on the SoC. */
#define FSL_FEATURE_SOC_SDIOC_COUNT (1)

/* MW_ACOMP module features */

/* No feature definitions */

/* MW_DMAC module features */

/* @brief Number of DMA channels in the SoC. */
#define FSL_FEATURE_MW_DMAC_MODULE_CHANNEL (32)

/* MW_GPIO module features */

/* @brief Number of GPIO in the SoC. */
#define FSL_FEATURE_MW_GPIO_IO_NUMBER (50)

#endif /* _88MW320_FEATURES_H_ */

