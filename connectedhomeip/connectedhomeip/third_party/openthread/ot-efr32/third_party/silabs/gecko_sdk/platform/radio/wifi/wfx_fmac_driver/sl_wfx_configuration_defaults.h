/**************************************************************************//**
 * Copyright 2022, Silicon Laboratories Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_WFX_CONFIGURATION_H
#define SL_WFX_CONFIGURATION_H

#include "sl_wfx_configuration.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_WFX_BUS_SPI_PRESENT)
#define SL_WFX_USE_SPI
#endif

#if defined(SL_CATALOG_WFX_BUS_SDIO_PRESENT)
#include "sl_wfx_bus_sdio_configuration.h"
#define SL_WFX_USE_SDIO
#endif

#if defined(SL_CATALOG_WFX_SECURE_LINK_PRESENT)
#include "sl_wfx_secure_link_configuration.h"
#define SL_WFX_USE_SECURE_LINK
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#define SLEEP_ENABLED
#endif

/**
 * @brief The timeout period in milliseconds for commands
 * @warning This should be configured from the FMAC Driver plugin options.
 */
#ifndef SL_WFX_DEFAULT_REQUEST_TIMEOUT_MS
#define SL_WFX_DEFAULT_REQUEST_TIMEOUT_MS 5000
#endif

/**
 * @brief Whether to use CURVE25519 or KDF crypto
 * @warning This should be configured from the Secure Link plugin options.
 */
#ifndef SL_WFX_SLK_CURVE25519
#define SL_WFX_SLK_CURVE25519             0
#endif

/**
 * @brief SDIO block size
 * @warning This should be configured from the Bus SDIO plugin options.
 */
#ifndef SL_WFX_SDIO_BLOCK_SIZE
#define SL_WFX_SDIO_BLOCK_SIZE            64
#endif

#endif // SL_WFX_CONFIGURATION_H
