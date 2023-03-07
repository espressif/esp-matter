// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// This file is OS-specific, and is identified by setting include directories
// in the project
#include "socket_async_os.h"

#include "azure_c_shared_utility/dns_resolver.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/xlogging.h"
#include "ares.h"

// EXTRACT_IPV4 pulls the uint32_t IPv4 address out of an addrinfo struct
// The default definition handles lwIP. Please add comments for other systems tested.
#define EXTRACT_IPV4(ptr) ((struct sockaddr_in *) ptr->ai_addr)->sin_addr.s_addr

typedef struct
{
    char* hostname;
    int port;
    uint32_t ip_v4;
    bool is_complete;
    bool is_failed;
    bool in_progress;
    struct addrinfo* addrInfo;
    ares_channel ares_resolver;
} DNSRESOLVER_INSTANCE;

DNSRESOLVER_HANDLE dns_resolver_create(const char* hostname, int port, const DNSRESOLVER_OPTIONS* options)
{
    /* Codes_SRS_dns_resolver_30_012: [ The optional options parameter shall be ignored. ]*/
    DNSRESOLVER_INSTANCE* result;
    int status;
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
            result->in_progress = false;
            result->ip_v4 = 0;
            result->port = port;
            /* Codes_SRS_dns_resolver_30_010: [ dns_resolver_create shall make a copy of the hostname parameter to allow immediate deletion by the caller. ]*/
            ms_result = mallocAndStrcpy_s(&result->hostname, hostname);
            if (ms_result != 0)
            {
                /* Codes_SRS_dns_resolver_30_014: [ On any failure, dns_resolver_create shall log an error and return NULL. ]*/
                LogError("dns_resolver_create: hostname allocation failed");
                free(result);
                result = NULL;
            }
            else
            {
                status = ares_library_init(ARES_LIB_INIT_WIN32);
                if (status != ARES_SUCCESS)
                {
                    LogError("ares_library_init failed: %s\n", ares_strerror(status));
                    free(result);
                    result = NULL;
                }
                else 
                {
                    status = ares_init(&(result->ares_resolver));
                    if(status != ARES_SUCCESS)
                    {
                        LogError("ares_init failed: %s\n", ares_strerror(status));
                        ares_library_cleanup();
                        free(result);
                        result = NULL;
                    }
                }
            }
        }
    }
    
    if(result != NULL)
    {
        result->addrInfo = NULL;
    }

    return result;
}

static void query_completed_cb(void *arg, int status, int timeouts, struct hostent *he)
{
    int i;
    struct addrinfo *ptr = NULL;
    struct sockaddr_in *addr;
    
    DNSRESOLVER_INSTANCE *dns = (DNSRESOLVER_INSTANCE *)arg;
    (void)timeouts;

    if(status != ARES_SUCCESS)
    {
        LogError("ARES error: %d", status);
    }
    else
    {
        dns->addrInfo = calloc(1, sizeof(struct addrinfo));
        if(dns->addrInfo == NULL)
        {
            LogError("dns addrInfo: allocation failed");
            dns->is_failed = true;
            dns->is_complete = true;
            dns->in_progress = false;
        }
        else
        {
            ptr = dns->addrInfo;
            
            ptr->ai_addr = calloc(1, sizeof(struct sockaddr_in));
            if(ptr->ai_addr == NULL)
            {
                LogError("dns addrinfo ai_addr: allocation failed");
                free(dns->addrInfo);
                dns->is_failed = true;
                dns->is_complete = true;
                dns->in_progress = false;
            }
            else
            {
                addr = (void *)ptr->ai_addr;

                if (he->h_addrtype == AF_INET)
                {
                    memcpy(&addr->sin_addr, he->h_addr_list[0], sizeof(struct in_addr));
                    addr->sin_family = he->h_addrtype;
                    addr->sin_port = htons((unsigned short)dns->port);

                    /* Codes_SRS_dns_resolver_30_033: [ If dns_resolver_is_create_complete has returned true and the lookup process has failed, dns_resolver_get_ipv4 shall return 0. ]*/
                    dns->ip_v4 = EXTRACT_IPV4(ptr);
                    dns->is_failed = (dns->ip_v4 == 0);
                    dns->is_complete = true;
                    dns->in_progress = false;
                }

            }
        }
    }
}

/* Codes_SRS_dns_resolver_30_021: [ dns_resolver_is_create_complete shall perform the asynchronous work of DNS lookup and log any errors. ]*/
bool dns_resolver_is_lookup_complete(DNSRESOLVER_HANDLE dns_in)
{
    DNSRESOLVER_INSTANCE* dns = (DNSRESOLVER_INSTANCE*)dns_in;
    ares_socket_t socket;

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
        else if(dns->is_failed)
        {
            dns->in_progress = false;
            result = false;
        }
        else if(!dns->in_progress)
        {
            ares_gethostbyname(dns->ares_resolver, dns->hostname, AF_INET, query_completed_cb, (void*)dns);
            dns->in_progress = true;
            // This synchronous implementation is incapable of being incomplete, so SRS_dns_resolver_30_023 does not ever happen
            /* Codes_SRS_dns_resolver_30_023: [ If the DNS lookup process is not yet complete, dns_resolver_is_create_complete shall return false. ]*/
            /* Codes_SRS_dns_resolver_30_022: [ If the DNS lookup process has completed, dns_resolver_is_create_complete shall return true. ]*/
            result = false;
        }
        else
        {
            ares_getsock((ares_channel)dns->ares_resolver, &socket, 1);
            ares_process_fd((ares_channel)dns->ares_resolver, socket, socket);
            result = false;
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
        ares_destroy(dns->ares_resolver);
        ares_library_cleanup();

        if(dns->addrInfo != NULL)
        {
            if(dns->addrInfo->ai_addr != NULL)
            {
                free(dns->addrInfo->ai_addr);
            }
            free(dns->addrInfo);
        }
        free(dns->hostname);
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
