// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#pragma once

/* Clock setup */
#define HSI_VALUE 16000000U
#define LSI_VALUE 32000U

// The F429-disc has an 8Mhz external crystal
#define HSE_VALUE 8000000U
#define HSE_STARTUP_TIMEOUT 100U

// The F429-disc has no LSE
#define LSE_VALUE 0U
#define LSE_STARTUP_TIMEOUT 5000U

#define EXTERNAL_CLOCK_VALUE 0U

/* HAL Config */
#define TICK_INT_PRIORITY 0x0FU
#define USE_RTOS 0U
#define PREFETCH_ENABLE 1U
#define INSTRUCTION_CACHE_ENABLE 1U
#define DATA_CACHE_ENABLE 1U

#define assert_param(expr) ((void)0U)

/* Ethernet driver buffers size + count
 * (also used by FreeRTOS_Plus_TCP's stm32 driver) */
#define ETH_RX_BUF_SIZE ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE ETH_MAX_PACKET_SIZE
#define ETH_RXBUFNB 4U
#define ETH_TXBUFNB 4U

/* Ethernet PHY Defines (unused by FreeRTOS_Plus_TCP's driver) */
#define PHY_RESET_DELAY 0x000000FFU
#define PHY_CONFIG_DELAY 0x00000FFFU

#define PHY_READ_TO 0x0000FFFFU
#define PHY_WRITE_TO 0x0000FFFFU

/* Common PHY Registers */
#define PHY_BCR ((uint16_t)0x0000)
#define PHY_BSR ((uint16_t)0x0001)

#define PHY_RESET ((uint16_t)0x8000)
#define PHY_LOOPBACK ((uint16_t)0x4000)
#define PHY_FULLDUPLEX_100M ((uint16_t)0x2100)
#define PHY_HALFDUPLEX_100M ((uint16_t)0x2000)
#define PHY_FULLDUPLEX_10M ((uint16_t)0x0100)
#define PHY_HALFDUPLEX_10M ((uint16_t)0x0000)
#define PHY_AUTONEGOTIATION ((uint16_t)0x1000)
#define PHY_RESTART_AUTONEGOTIATION ((uint16_t)0x0200)
#define PHY_POWERDOWN ((uint16_t)0x0800)
#define PHY_ISOLATE ((uint16_t)0x0400)

#define PHY_AUTONEGO_COMPLETE ((uint16_t)0x0020)
#define PHY_LINKED_STATUS ((uint16_t)0x0004)
#define PHY_JABBER_DETECTION ((uint16_t)0x0002)

/* Extended PHY Registers */
#define PHY_SR ((uint16_t)0x0010)
#define PHY_MICR ((uint16_t)0x0011)
#define PHY_MISR ((uint16_t)0x0012)

#define PHY_LINK_STATUS ((uint16_t)0x0001)
#define PHY_SPEED_STATUS ((uint16_t)0x0002)
#define PHY_DUPLEX_STATUS ((uint16_t)0x0004)

#define PHY_MICR_INT_EN ((uint16_t)0x0002)
#define PHY_MICR_INT_OE ((uint16_t)0x0001)

#define PHY_MISR_LINK_INT_EN ((uint16_t)0x0020)
#define PHY_LINK_INTERRUPT ((uint16_t)0x2000)

// SPI config
#define USE_SPI_CRC 1U

/** HAL Headers: comment out defines + include to remove **/
/* primary HAL headers */
#define HAL_CORTEX_MODULE_ENABLED
#include "stm32f4xx_hal_cortex.h"

#define HAL_DMA_MODULE_ENABLED
#include "stm32f4xx_hal_dma.h"

#define HAL_EXTI_MODULE_ENABLED
#include "stm32f4xx_hal_exti.h"

#define HAL_GPIO_MODULE_ENABLED
#include "stm32f4xx_hal_gpio.h"

#define HAL_RCC_MODULE_ENABLED
#include "stm32f4xx_hal_rcc.h"

/* remaining headers (can be commented out if desired) */
#define HAL_ADC_MODULE_ENABLED
#define USE_HAL_ADC_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_adc.h"

#define HAL_CAN_MODULE_ENABLED
#define USE_HAL_CAN_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_can.h"

// #define HAL_CAN_LEGACY_MODULE_ENABLED
// #include "stm32f4xx_hal_can_legacy.h"

#define HAL_CEC_MODULE_ENABLED
#define USE_HAL_CEC_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_cec.h"

#define HAL_CRC_MODULE_ENABLED
#include "stm32f4xx_hal_crc.h"

#define HAL_CRYP_MODULE_ENABLED
#define USE_HAL_CRYP_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_cryp.h"

#define HAL_DAC_MODULE_ENABLED
#define USE_HAL_DAC_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_dac.h"

#define HAL_DCMI_MODULE_ENABLED
#define USE_HAL_DCMI_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_dcmi.h"

#define HAL_DMA2D_MODULE_ENABLED
#define USE_HAL_DMA2D_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_dma2d.h"

#define HAL_DFSDM_MODULE_ENABLED
#define USE_HAL_DFSDM_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_dfsdm.h"

#define HAL_DSI_MODULE_ENABLED
#define USE_HAL_DSI_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_dsi.h"

#define HAL_ETH_MODULE_ENABLED
#define USE_HAL_ETH_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_eth.h"

#define HAL_FLASH_MODULE_ENABLED
#include "stm32f4xx_hal_flash.h"

#define HAL_FMPI2C_MODULE_ENABLED
#define USE_HAL_FMPI2C_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_fmpi2c.h"

#define HAL_FMPSMBUS_MODULE_ENABLED
#define USE_HAL_FMPSMBUS_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_fmpsmbus.h"

#define HAL_HASH_MODULE_ENABLED
#define USE_HAL_HASH_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_hash.h"

#define HAL_HCD_MODULE_ENABLED
#define USE_HAL_HCD_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_hcd.h"

#define HAL_I2C_MODULE_ENABLED
#define USE_HAL_I2C_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_i2c.h"

#define HAL_I2S_MODULE_ENABLED
#define USE_HAL_I2S_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_i2s.h"

#define HAL_IRDA_MODULE_ENABLED
#define USE_HAL_IRDA_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_irda.h"

#define HAL_IWDG_MODULE_ENABLED
#include "stm32f4xx_hal_iwdg.h"

#define HAL_LPTIM_MODULE_ENABLED
#define USE_HAL_LPTIM_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_lptim.h"

#define HAL_LTDC_MODULE_ENABLED
#define USE_HAL_LTDC_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_ltdc.h"

#define HAL_MMC_MODULE_ENABLED
#define USE_HAL_MMC_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_mmc.h"

#define HAL_NAND_MODULE_ENABLED
#define USE_HAL_NAND_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_nand.h"

#define HAL_NOR_MODULE_ENABLED
#define USE_HAL_NOR_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_nor.h"

#define HAL_PCCARD_MODULE_ENABLED
#define USE_HAL_PCCARD_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_pccard.h"

#define HAL_PCD_MODULE_ENABLED
#define USE_HAL_PCD_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_pcd.h"

#define HAL_PWR_MODULE_ENABLED
#include "stm32f4xx_hal_pwr.h"

#define HAL_QSPI_MODULE_ENABLED
#define USE_HAL_QSPI_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_qspi.h"

#define HAL_RNG_MODULE_ENABLED
#define USE_HAL_RNG_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_rng.h"

#define HAL_RTC_MODULE_ENABLED
#define USE_HAL_RTC_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_rtc.h"

#define HAL_SAI_MODULE_ENABLED
#define USE_HAL_SAI_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_sai.h"

#define HAL_SD_MODULE_ENABLED
#define USE_HAL_SD_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_sd.h"

#define HAL_SDRAM_MODULE_ENABLED
#define USE_HAL_SDRAM_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_sdram.h"

#define HAL_SMARTCARD_MODULE_ENABLED
#define USE_HAL_SMARTCARD_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_smartcard.h"

#define HAL_SMBUS_MODULE_ENABLED
#define USE_HAL_SMBUS_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_smbus.h"

#define HAL_SPDIFRX_MODULE_ENABLED
#define USE_HAL_SPDIFRX_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_spdifrx.h"

#define HAL_SPI_MODULE_ENABLED
#define USE_HAL_SPI_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_spi.h"

#define HAL_SRAM_MODULE_ENABLED
#define USE_HAL_SRAM_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_sram.h"

#define HAL_TIM_MODULE_ENABLED
#define USE_HAL_TIM_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_tim.h"

#define HAL_UART_MODULE_ENABLED
#define USE_HAL_UART_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_uart.h"

#define HAL_USART_MODULE_ENABLED
#define USE_HAL_USART_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_usart.h"

#define HAL_WWDG_MODULE_ENABLED
#define USE_HAL_WWDG_REGISTER_CALLBACKS 0U
#include "stm32f4xx_hal_wwdg.h"
