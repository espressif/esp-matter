/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP component configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef SL_WISUN_COAP_CONFIG_H
#define SL_WISUN_COAP_CONFIG_H

/**************************************************************************//**
 * @defgroup SL_WISUN_COAP_CONFIG Configuration
 * @ingroup SL_WISUN_COAP
 * @{
 *****************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Wi-SUN CoAP configuration

// <q SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL> Enable Static Memory Pool usage
// <i> Enables the static memory pool usage in CoAP component, if it's available (Micrium doesn't support this feature).
#define SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL    1

// </h>

// <h>Wi-SUN CoAP Resource Configuration
// <o SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES> Maximum capacity of the CoAP Resource Table
// <i> Default: 10
#define SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES                  10U

// </h>

// <<< end of configuration section >>>

/** @}*/

#endif // SL_WISUN_COAP_CONFIG_H
