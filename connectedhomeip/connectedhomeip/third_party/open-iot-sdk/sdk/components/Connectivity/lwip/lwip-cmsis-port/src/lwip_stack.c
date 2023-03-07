/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lwip_stack.h"

#include "cmsis_os2.h"
#include "lwip/dhcp.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip_emac_netif.h"
#include "lwip_memory_manager.h"
#include "lwipopts.h"

#include <stdbool.h>

/* secure sockets requires errno to communicate error codes */
int errno = 0;

static netif_context_t lwip_context = {0};

static err_t set_dhcp(struct netif *netif)
{
#if LWIP_DHCP
    netif_context_t *context = (netif_context_t *)(netif->state);

    if (context->dhcp_has_to_be_set) {
        err_t err = dhcp_start(netif);
        if (err) {
            context->connected = CONNECTION_STATUS_DISCONNECTED;
            return ERR_IF;
        }

        context->dhcp_has_to_be_set = false;
        context->dhcp_started = true;
    }
#endif

    return ERR_OK;
}

#if LWIP_IPV4
static const ip_addr_t *get_ipv4_addr(const struct netif *netif)
{
    if (!netif_is_up(netif)) {
        return NULL;
    }

    if (!ip4_addr_isany(netif_ip4_addr(netif))) {
        return netif_ip_addr4(netif);
    }
    return NULL;
}
#endif

#if LWIP_IPV6
static const ip_addr_t *get_ipv6_addr(const struct netif *netif)
{
    if (!netif_is_up(netif)) {
        return NULL;
    }

    for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) && !ip6_addr_islinklocal(netif_ip6_addr(netif, i))) {
            return netif_ip_addr6(netif, i);
        }
    }

    for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
            return netif_ip_addr6(netif, i);
        }
    }
    return NULL;
}
#endif

static const ip_addr_t *get_ip_addr(bool any_addr, const struct netif *netif)
{
    const ip_addr_t *pref_ip_addr = 0;

#if LWIP_IPV4 && LWIP_IPV6
    const ip_addr_t *npref_ip_addr = 0;
#if IP_VERSION_PREF == PREF_IPV4
    pref_ip_addr = get_ipv4_addr(netif);
    npref_ip_addr = get_ipv6_addr(netif);
#else
    pref_ip_addr = get_ipv6_addr(netif);
    npref_ip_addr = get_ipv4_addr(netif);
#endif
#elif LWIP_IPV6
    pref_ip_addr = get_ipv6_addr(netif);
#elif LWIP_IPV4
    pref_ip_addr = get_ipv4_addr(netif);
#else
#error At least one of LWIP_IPV6 and LWIP_IPV4 is required
#endif

    if (pref_ip_addr) {
        return pref_ip_addr;
    }
#if LWIP_IPV4 && LWIP_IPV6
    else if (npref_ip_addr && any_addr) {
        return npref_ip_addr;
    }
#endif

    return NULL;
}

static void netif_link_irq(struct netif *netif)
{
    netif_context_t *context = (netif_context_t *)(netif->state);

    if (netif_is_link_up(netif) && context->connected == CONNECTION_STATUS_CONNECTING) {
        netif_set_up(netif);
    } else {
        if (netif_is_up(netif)) {
            context->connected = CONNECTION_STATUS_CONNECTING;
        }
        netif_set_down(netif);
    }
}

static void netif_status_irq(struct netif *netif)
{
    netif_context_t *context = (netif_context_t *)(netif->state);

    if (netif_is_up(netif) && netif_is_link_up(netif)) {
        if (context->dhcp_has_to_be_set) {
            set_dhcp(netif);
        } else {
            if (!(context->has_addr_state & ADDRESS_STATUS_HAS_ANY_ADDR) && get_ip_addr(true, netif)) {
                context->has_addr_state |= ADDRESS_STATUS_HAS_ANY_ADDR;
            }
#if PREF_ADDR_TIMEOUT
            if (!(context->has_addr_state & ADDRESS_STATUS_HAS_PREF_ADDR) && get_ip_addr(false, netif)) {
                context->has_addr_state |= ADDRESS_STATUS_HAS_PREF_ADDR;
            }
#endif
#if BOTH_ADDR_TIMEOUT
            if (!(context->has_addr_state & ADDRESS_STATUS_HAS_BOTH_ADDR) && get_ipv4_addr(netif)
                && get_ipv6_addr(netif)) {
                context->has_addr_state |= ADDRESS_STATUS_HAS_BOTH_ADDR;
            }
#endif
            if (context->has_addr_state & ADDRESS_STATUS_HAS_ANY_ADDR) {
                context->connected = CONNECTION_STATUS_GLOBAL_UP;

                if (ip_addr_islinklocal(get_ip_addr(true, netif))) {
                    context->connected = CONNECTION_STATUS_LOCAL_UP;
                }

                if (context->network_state_callback) {
                    context->network_state_callback(NETWORK_UP);
                }
            }
        }
    } else if (!netif_is_up(netif) && netif_is_link_up(netif)) {
        context->connected = CONNECTION_STATUS_DISCONNECTED;

        if (context->network_state_callback) {
            context->network_state_callback(NETWORK_DOWN);
        }
    }
}

static err_t ethernet_interface_bringup(netif_context_t *context)
{
    if (context->connected == CONNECTION_STATUS_GLOBAL_UP) {
        return ERR_ISCONN;
    } else if (context->connected == CONNECTION_STATUS_CONNECTING) {
        return ERR_INPROGRESS;
    }

#if LWIP_DHCP
    context->dhcp_has_to_be_set = true;
#endif /* LWIP_DHCP */

#if LWIP_IPV6
    netif_create_ip6_linklocal_address(&context->lwip_netif, (u8_t)(context->lwip_netif.hwaddr_len == ETH_HWADDR_LEN));
#endif /* LWIP_IPV6 */

    context->connected = CONNECTION_STATUS_CONNECTING;

    return ERR_OK;
}

static err_t ethernet_interface_init(netif_context_t *context)
{
    struct netif *n = netif_add(&context->lwip_netif,
#if LWIP_IPV4
                                0,
                                0,
                                0,
#endif
                                context /* state */,
                                &ethernetif_init,
                                tcpip_input);

    if (!n) {
        return ERR_IF;
    }

    netif_set_link_callback(&context->lwip_netif, &netif_link_irq);
    netif_set_status_callback(&context->lwip_netif, &netif_status_irq);

    netif_set_default(&context->lwip_netif);

    return ERR_OK;
}

static void tcpip_init_done(void *arg)
{
    sys_sem_signal(arg);
}

static err_t lwip_network_init(netif_context_t *context)
{
    sys_sem_t tcpip_done;
    if (ERR_OK != sys_sem_new(&tcpip_done, 0)) {
        LWIP_PLATFORM_ASSERT("Cannot create semaphore");
        return ERR_IF;
    }

    /* Initialise LwIP, providing the callback function and callback semaphore */
    tcpip_init(tcpip_init_done, &tcpip_done);

    sys_arch_sem_wait(&tcpip_done, 0);
    sys_sem_free(&tcpip_done);

    return ERR_OK;
}

void signal_receive(netif_context_t *context)
{
    sys_sem_signal(&context->receive_semaphore);
}

void lwip_task(void *network_state_callback)
{
    lwip_context.network_state_callback = (network_state_callback_t)(network_state_callback);

    err_t ret = sys_sem_new(&lwip_context.receive_semaphore, 0);
    LWIP_ASSERT("Cannot create semaphore", (ret == ERR_OK));

    ret = lwip_network_init(&lwip_context);
    LWIP_ASSERT("Cannot create semaphore", (ret == ERR_OK));

    ethernet_interface_init(&lwip_context);
    ethernet_interface_bringup(&lwip_context);

    /* emac is missing this event so we need to trigger it ourselves*/
    tcpip_callback_with_block((tcpip_callback_fn)netif_set_link_up, &lwip_context.lwip_netif, 1);

    /* this never exits and runs the lwip network stack on this thread */
    while (1) {
        /* handle input */
        if (sys_arch_sem_wait(&lwip_context.receive_semaphore, 0) == 0) {
            ethernetif_process_input(&lwip_context);
        }
    }
}
