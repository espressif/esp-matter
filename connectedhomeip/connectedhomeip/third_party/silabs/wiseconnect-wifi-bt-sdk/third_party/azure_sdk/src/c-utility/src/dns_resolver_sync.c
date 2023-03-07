// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// This file is OS-specific, and is identified by setting include directories
// in the project
#include "socket_async_os.h"

#include "azure_c_shared_utility/dns_resolver.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"


// EXTRACT_IPV4 pulls the uint32_t IPv4 address out of an addrinfo struct
// This will not be needed for the asynchronous design
// The default definition handles lwIP. Please add comments for other systems tested.
#define EXTRACT_IPV4(ptr) ((struct sockaddr_in *) ptr->ai_addr)->sin_addr.s_addr

typedef struct
{
    char* hostname;
    int port;
    uint32_t ip_v4;
    bool is_complete;
    bool is_failed;
    struct addrinfo* addrInfo;
} DNSRESOLVER_INSTANCE;

DNSRESOLVER_HANDLE dns_resolver_create(const char* hostname, int port, const DNSRESOLVER_OPTIONS* options)
{
    /* Codes_SRS_dns_resolver_30_012: [ The optional options parameter shall be ignored. ]*/
    DNSRESOLVER_INSTANCE* result;
    (void)options;
    if (hostname == NULL)
    {
        /* Codes_SRS_dns_resolver_30_011: [ If the hostname parameter is NULL, dns_resolver_create shall log an error and return NULL. ]*/
        LogError("NULL hostname");
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(DNSRESOLVER_INSTANCE));
        if (result == NULL)
        {
            /* Codes_SRS_dns_resolver_30_014: [ On any failure, dns_resolver_create shall log an error and return NULL. ]*/
            LogError("malloc instance failed");
            result = NULL;
        }
        else
        {
            int ms_result;
            result->is_complete = false;
            result->is_failed = false;
            result->ip_v4 = 0;
            result->port = port;
            /* Codes_SRS_dns_resolver_30_010: [ dns_resolver_create shall make a copy of the hostname parameter to allow immediate deletion by the caller. ]*/
            ms_result = mallocAndStrcpy_s(&result->hostname, hostname);
            if (ms_result != 0)
            {
                /* Codes_SRS_dns_resolver_30_014: [ On any failure, dns_resolver_create shall log an error and return NULL. ]*/
                free(result);
                result = NULL;
            }
        }
    }
    return result;
}

/* Codes_SRS_dns_resolver_30_021: [ dns_resolver_is_create_complete shall perform the asynchronous work of DNS lookup and log any errors. ]*/
bool dns_resolver_is_lookup_complete(DNSRESOLVER_HANDLE dns_in)
{
    DNSRESOLVER_INSTANCE* dns = (DNSRESOLVER_INSTANCE*)dns_in;

    bool result;
    if (dns == NULL)
    {
        /* Codes_SRS_dns_resolver_30_020: [ If the dns parameter is NULL, dns_resolver_is_create_complete shall log an error and return false. ]*/
        LogError("NULL dns");
        result = false;
    }
    else
    {
        if (dns->is_complete)
        {
            /* Codes_SRS_dns_resolver_30_024: [ If dns_resolver_is_create_complete has previously returned true, dns_resolver_is_create_complete shall do nothing and return true. ]*/
            result = true;
        }
        else
        {
            char portString[16];
            struct addrinfo *addrInfo = NULL;
            struct addrinfo *ptr = NULL;
            struct addrinfo hints;
            int getAddrResult;

            //--------------------------------
            // Setup the hints address info structure
            // which is passed to the getaddrinfo() function
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = 0;

            //--------------------------------
            // Call getaddrinfo(). If the call succeeds,
            // the result variable will hold a linked list
            // of addrinfo structures containing response
            // information
            if (sprintf(portString, "%u", dns->port) < 0)
            {
                LogError("Failure: sprintf failed to encode the port.");
                result = MU_FAILURE;
            }

            getAddrResult = getaddrinfo(dns->hostname, portString, &hints, &addrInfo);
            if (getAddrResult == 0)
            {
                dns->addrInfo = addrInfo;
                
                // If we find the AF_INET address, use it as the return value
                for (ptr = addrInfo; ptr != NULL; ptr = ptr->ai_next)
                {
                    switch (ptr->ai_family)
                    {
                    case AF_INET:
                        /* Codes_SRS_dns_resolver_30_032: [ If dns_resolver_is_create_complete has returned true and the lookup process has succeeded, dns_resolver_get_ipv4 shall return the discovered IPv4 address. ]*/
                        dns->ip_v4 = EXTRACT_IPV4(ptr);
                        break;
                    }
                }
                /* Codes_SRS_dns_resolver_30_033: [ If dns_resolver_is_create_complete has returned true and the lookup process has failed, dns_resolver_get_ipv4 shall return 0. ]*/
                dns->is_failed = (dns->ip_v4 == 0);
            }
            else
            {
                /* Codes_SRS_dns_resolver_30_033: [ If dns_resolver_is_create_complete has returned true and the lookup process has failed, dns_resolver_get_ipv4 shall return 0. ]*/
                LogInfo("Failed DNS lookup for %s: %d", dns->hostname, getAddrResult);
                dns->is_failed = true;
            }

            /* Codes_SRS_dns_resolver_30_021: [ dns_resolver_is_create_complete shall perform the asynchronous work of DNS lookup and log any errors. ]*/
            // Only make one attempt at lookup for this
            // synchronous implementation
            dns->is_complete = true;

            // This synchronous implementation is incapable of being incomplete, so SRS_dns_resolver_30_023 does not ever happen
            /* Codes_SRS_dns_resolver_30_023: [ If the DNS lookup process is not yet complete, dns_resolver_is_create_complete shall return false. ]*/
            /* Codes_SRS_dns_resolver_30_022: [ If the DNS lookup process has completed, dns_resolver_is_create_complete shall return true. ]*/
            result = true;
        }
    }

    return result;
}

void dns_resolver_destroy(DNSRESOLVER_HANDLE dns_in)
{
    DNSRESOLVER_INSTANCE* dns = (DNSRESOLVER_INSTANCE*)dns_in;
    if (dns == NULL)
    {
        /* Codes_SRS_dns_resolver_30_050: [ If the dns parameter is NULL, dns_resolver_destroy shall log an error and do nothing. ]*/
        LogError("NULL dns");
    }
    else
    {
        /* Codes_SRS_dns_resolver_30_051: [ dns_resolver_destroy shall delete all acquired resources and delete the DNSRESOLVER_HANDLE. ]*/
        if(dns->is_complete && !dns->is_failed && dns->addrInfo != NULL)
        {
            freeaddrinfo(dns->addrInfo);
        }

        if(dns->hostname != NULL)
        {
            free(dns->hostname);
        }
        
        free(dns);
        dns = NULL;
    }
}

uint32_t dns_resolver_get_ipv4(DNSRESOLVER_HANDLE dns_in)
{
    DNSRESOLVER_INSTANCE* dns = (DNSRESOLVER_INSTANCE*)dns_in;
    uint32_t result;
    if (dns == NULL)
    {
        /* Codes_SRS_dns_resolver_30_030: [ If the dns parameter is NULL, dns_resolver_get_ipv4 shall log an error and return 0. ]*/
        LogError("NULL dns");
        result = 0;
    }
    else
    {
        if (dns->is_complete)
        {
            if (dns->is_failed)
            {
                /* Codes_SRS_dns_resolver_30_033: [ If dns_resolver_is_create_complete has returned true and the lookup process has failed, dns_resolver_get_ipv4 shall return 0. ]*/
                result = 0;
            }
            else
            {
                /* Codes_SRS_dns_resolver_30_032: [ If dns_resolver_is_create_complete has returned true and the lookup process has succeeded, dns_resolver_get_ipv4 shall return the discovered IPv4 address. ]*/
                result = dns->ip_v4;
            }
        }
        else
        {
            /* Codes_SRS_dns_resolver_30_031: [ If dns_resolver_is_create_complete has not yet returned true, dns_resolver_get_ipv4 shall log an error and return 0. ]*/
            LogError("dns_resolver_get_ipv4 when not complete");
            result = 0;
        }
    }
    return result;
}

struct addrinfo* dns_resolver_get_addrInfo(DNSRESOLVER_HANDLE dns_in)
{
    DNSRESOLVER_INSTANCE* dns = (DNSRESOLVER_INSTANCE*)dns_in;
    struct addrinfo* result;
    if (dns == NULL)
    {
        /* Codes_SRS_dns_resolver_30_030: [ If the dns parameter is NULL, dns_resolver_get_ipv4 shall log an error and return 0. ]*/
        LogError("NULL dns");
        result = NULL;
    }
    else
    {
        if (dns->is_complete)
        {
            if (dns->is_failed)
            {
                /* Codes_SRS_dns_resolver_30_033: [ If dns_resolver_is_create_complete has returned true and the lookup process has failed, dns_resolver_get_ipv4 shall return 0. ]*/
                result = NULL;
            }
            else
            {
                /* Codes_SRS_dns_resolver_30_032: [ If dns_resolver_is_create_complete has returned true and the lookup process has succeeded, dns_resolver_get_ipv4 shall return the discovered IPv4 address. ]*/
                result = dns->addrInfo;
            }
        }
        else
        {
            /* Codes_SRS_dns_resolver_30_031: [ If dns_resolver_is_create_complete has not yet returned true, dns_resolver_get_ipv4 shall log an error and return 0. ]*/
            LogError("dns_resolver_get_ipv4 when not complete");
            result = NULL;
        }
    }
    return result;
}
