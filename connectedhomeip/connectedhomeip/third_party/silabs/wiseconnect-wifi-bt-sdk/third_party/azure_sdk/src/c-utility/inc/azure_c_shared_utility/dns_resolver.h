// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/** @file ssl_socket.h
 *    @brief     Implements socket creation for TLSIO adapters.
 */

#ifndef AZURE_IOT_DNS_H
#define AZURE_IOT_DNS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "azure_macro_utils/macro_utils.h"
#include "umock_c/umock_c_prod.h"

    typedef void* DNSRESOLVER_HANDLE;

    // If options are added in future, DNSRESOLVER_OPTIONS will become a struct containing the options
    typedef void DNSRESOLVER_OPTIONS;

    /**
    * @brief    Begin the process of an asynchronous DNS lookup.
    *
    * @param   hostname    The url of the host to look up.
    *
    * @return    @c The newly created DNSRESOLVER_HANDLE.
    */
    MOCKABLE_FUNCTION(, DNSRESOLVER_HANDLE, dns_resolver_create, const char*, hostname, int, port, const DNSRESOLVER_OPTIONS*, options);

    /**
    * @brief    Continue the lookup process and report its completion state. Must be polled repeatedly for completion.
    *
    * @param   dns    The DNSRESOLVER_HANDLE.
    *
    * @return    @c A bool to indicate completion.
    */
    MOCKABLE_FUNCTION(, bool, dns_resolver_is_lookup_complete, DNSRESOLVER_HANDLE, dns);

    /**
    * @brief    Return the IPv4 of a completed lookup process. Call only after dns_resolver_is_lookup_complete indicates completion.
    *
    * @param   dns    The DNSRESOLVER_HANDLE.
    *
    * @return    @c A uint32_t IPv4 address. 0 indicates failure or not finished.
    */
    MOCKABLE_FUNCTION(, uint32_t, dns_resolver_get_ipv4, DNSRESOLVER_HANDLE, dns);

    /**
    * @brief    Return the addrInfo of a completed lookup process. Call only after dns_resolver_is_lookup_complete indicates completion.
    *
    * @param   dns    The DNSRESOLVER_HANDLE.
    *
    * @return    @c A struct addrinfo. 0 indicates failure or not finished.
    */
    MOCKABLE_FUNCTION(, struct addrinfo*, dns_resolver_get_addrInfo, DNSRESOLVER_HANDLE, dns);
    

    /**
    * @brief    Destroy the module.
    *
    * @param   dns    The DNSRESOLVER_HANDLE.
    */
    MOCKABLE_FUNCTION(, void, dns_resolver_destroy, DNSRESOLVER_HANDLE, dns);


#ifdef __cplusplus
}
#endif

#endif /* AZURE_IOT_DNS_H */
