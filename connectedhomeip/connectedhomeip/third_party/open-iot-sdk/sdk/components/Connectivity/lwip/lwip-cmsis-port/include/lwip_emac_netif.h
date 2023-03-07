/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LWIP_EMAC_NETIF_H_
#define LWIP_EMAC_NETIF_H_

#include "cmsis_os2.h"
#include "hal/emac_api.h"
#include "iotsdk/ip_network_api.h"
#include "lwip/def.h"
#include "lwip/opt.h"
#include "netif/ethernet.h"

#include <stdbool.h>

typedef enum connection_status_t {
    CONNECTION_STATUS_DISCONNECTED,
    CONNECTION_STATUS_CONNECTING,
    CONNECTION_STATUS_GLOBAL_UP,
    CONNECTION_STATUS_LOCAL_UP
} connection_status_t;

typedef enum address_status_t {
    ADDRESS_STATUS_HAS_ANY_ADDR = 1,
    ADDRESS_STATUS_HAS_PREF_ADDR = 2,
    ADDRESS_STATUS_HAS_BOTH_ADDR = 4
} address_status_t;

typedef struct netif_context_t {
    mdh_emac_t *emac;
    struct netif lwip_netif;
    osSemaphoreId_t receive_semaphore;
    connection_status_t connected;
    bool dhcp_has_to_be_set;
    bool dhcp_started;
    uint8_t has_addr_state;
    network_state_callback_t network_state_callback;
} netif_context_t;

err_t ethernetif_init(struct netif *netif);
void ethernetif_process_input(netif_context_t *netif);

// This function must be provided by the application until a factory is available
// from MCU Driver HAL
mdh_emac_t *mdh_emac_get_default_instance(void);

#endif // LWIP_EMAC_NETIF_H_
