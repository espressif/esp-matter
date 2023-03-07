/***************************************************************************//**
 * @file
 * @brief Board Init
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "sl_board_control.h"
#include "sl_board_control_config.h"

#include "em_cmu.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MX25_FLASH_SHUTDOWN_USART_PRESENT) || defined(SL_CATALOG_MX25_FLASH_SHUTDOWN_EUSART_PRESENT)
#include "sl_mx25_flash_shutdown.h"
#endif

void sl_board_default_init(void);

void sl_board_init(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Errata fixes and default pin states
  sl_board_default_init();

#if defined(SL_BOARD_ENABLE_SENSOR_RHT) && SL_BOARD_ENABLE_SENSOR_RHT
  sl_board_enable_sensor(SL_BOARD_SENSOR_RHT);
#endif
#if defined(SL_BOARD_ENABLE_SENSOR_LIGHT) && SL_BOARD_ENABLE_SENSOR_LIGHT
  sl_board_enable_sensor(SL_BOARD_SENSOR_LIGHT);
#endif
#if defined(SL_BOARD_ENABLE_SENSOR_PRESSURE) && SL_BOARD_ENABLE_SENSOR_PRESSURE
  sl_board_enable_sensor(SL_BOARD_SENSOR_PRESSURE);
#endif
#if defined(SL_BOARD_ENABLE_SENSOR_HALL) && SL_BOARD_ENABLE_SENSOR_HALL
  sl_board_enable_sensor(SL_BOARD_SENSOR_HALL);
#endif
#if defined(SL_BOARD_ENABLE_SENSOR_GAS) && SL_BOARD_ENABLE_SENSOR_GAS
  sl_board_enable_sensor(SL_BOARD_SENSOR_GAS);
#endif
#if defined(SL_BOARD_ENABLE_SENSOR_IMU) && SL_BOARD_ENABLE_SENSOR_IMU
  sl_board_enable_sensor(SL_BOARD_SENSOR_IMU);
#endif
#if defined(SL_BOARD_ENABLE_SENSOR_MICROPHONE) && SL_BOARD_ENABLE_SENSOR_MICROPHONE
  sl_board_enable_sensor(SL_BOARD_SENSOR_MICROPHONE);
#endif

#if defined(SL_BOARD_ENABLE_DISPLAY) && SL_BOARD_ENABLE_DISPLAY
  sl_board_enable_display();
#endif

#if defined(SL_BOARD_ENABLE_MEMORY_SDCARD) && SL_BOARD_ENABLE_MEMORY_SDCARD
  sl_board_enable_memory(SL_BOARD_MEMORY_SDCARD);
#endif
#if defined(SL_BOARD_ENABLE_MEMORY_QSPI) && SL_BOARD_ENABLE_MEMORY_QSPI
  sl_board_enable_memory(SL_BOARD_MEMORY_QSPI);
#endif

#if (defined(SL_CATALOG_MX25_FLASH_SHUTDOWN_USART_PRESENT) || defined(SL_CATALOG_MX25_FLASH_SHUTDOWN_EUSART_PRESENT)) && \
    defined(SL_BOARD_DISABLE_MEMORY_SPI) && SL_BOARD_DISABLE_MEMORY_SPI
  sl_mx25_flash_shutdown();
#endif
}

void sl_board_preinit(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

#if defined(SL_BOARD_ENABLE_OSCILLATOR_TCXO) && SL_BOARD_ENABLE_OSCILLATOR_TCXO
  sl_board_enable_oscillator(SL_BOARD_OSCILLATOR_TCXO);
#endif
}
