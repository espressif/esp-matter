/***************************************************************************//**
 * @brief driver wrapper for Ethernet controller Micrel KSZ8851SNL used in lwIP
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef _LWIP_DRIVER_H_
#define _LWIP_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/**************************************************************************//**
* @addtogroup Lwip
* @{
******************************************************************************/

#include <stdbool.h>
#include "netif/etharp.h"


#define ETH_MAC_ADDR_LEN    6   /**< MAC Address length */
#define ETH_HEADER_LEN      14  /**< Ethernet frame header length */

#define IFNAME0             'e' /**< Interface first byte of name */
#define IFNAME1             'n' /**< Interface second byte of name */

err_t ksz8851snl_driver_init(struct netif *netif);
void  ksz8851snl_driver_input(struct netif *netif);
err_t ksz8851snl_driver_output(struct netif *netif, struct pbuf *p);
void  ksz8851snl_driver_isr(struct netif *netif);

/** @} (end group Lwip) */
/** @} (end group Drivers) */

#ifdef __cplusplus
}
#endif

#endif /* _LWIP_DRIVER_H_ */
