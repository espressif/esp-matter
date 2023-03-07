/* -*- Mode: C; tab-width: 4 -*- 
 *
 * Copyright (c) 2012 Apple Inc. All rights reserved.
 *
 * dnsctl.c 
 * Command-line tool using libdns_services.dylib 
 *   
 * To build only this tool, copy and paste the following on the command line:
 * On Apple 64bit Platforms ONLY OSX/iOS:
 * clang -Wall dnsctl.c /usr/lib/libdns_services.dylib -o dnsctl
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <net/if.h> // if_nametoindex()

#include <dispatch/dispatch.h>
#include "dns_services.h"

//*************************************************************************************************************
// Globals:
//*************************************************************************************************************

static const char kFilePathSep   =  '/';
static DNSXConnRef ClientRef     =  NULL;

//*************************************************************************************************************
// Utility Funcs:
//*************************************************************************************************************

static void printtimestamp(void) 
{
    struct tm tm; 
    int ms; 
    static char date[16];
    static char new_date[16];
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    localtime_r((time_t*)&tv.tv_sec, &tm);
    ms = tv.tv_usec/1000;
    strftime(new_date, sizeof(new_date), "%a %d %b %Y", &tm);
    //display date only if it has changed
    if (strncmp(date, new_date, sizeof(new_date)))
    {        
        printf("DATE: ---%s---\n", new_date);
        strncpy(date, new_date, sizeof(date));
    }        
    printf("%2d:%02d:%02d.%03d  ", tm.tm_hour, tm.tm_min, tm.tm_sec, ms); 
}

static void print_usage(const char *arg0)
{
    fprintf(stderr, "%s USAGE:                                                                  \n", arg0);
    fprintf(stderr, "%s -DP Enable DNS Proxy with Default Parameters                            \n", arg0);
    fprintf(stderr, "%s -DP [-o <output interface>] [-i <input interface(s)>] Enable DNS Proxy  \n", arg0);
}

//*************************************************************************************************************
// CallBack Funcs:
//*************************************************************************************************************

// DNSXEnableProxy Callback from the Daemon
static void dnsproxy_reply(DNSXConnRef connRef, DNSXErrorType errCode)
{
    (void) connRef;
    printtimestamp();
    switch (errCode)
    {
        case kDNSX_NoError          :  printf("  SUCCESS   \n");     break;
        case kDNSX_DictError        :  printf(" DICT ERROR \n");     break;
        case kDNSX_DaemonNotRunning :  printf(" NO DAEMON  \n");
                                       DNSXRefDeAlloc(ClientRef);    break;
        case kDNSX_Engaged          :  printf(" ENGAGED    \n");
                                       DNSXRefDeAlloc(ClientRef);    break;
        case kDNSX_UnknownErr       :
        default                     :  printf("UNKNOWN ERR \n");
                                       DNSXRefDeAlloc(ClientRef);    break;
    }

}

//*************************************************************************************************************

int main(int argc, char **argv)
{
    DNSXErrorType err;

    // Default i/p intf is lo0 and o/p intf is primary interface
    IfIndex Ipintfs[MaxInputIf] =  {1, 0, 0, 0, 0};
    IfIndex Opintf = kDNSIfindexAny;

    // Extract program name from argv[0], which by convention contains the path to this executable
    const char *a0 = strrchr(argv[0], kFilePathSep) + 1; 
    if (a0 == (const char *)1)
        a0 = argv[0];

    // Must run as root
    if (0 != geteuid()) 
    {        
        fprintf(stderr, "%s MUST run as root!!\n", a0); 
        exit(-1); 
    }
    if ((sizeof(argv) == 8))
        printf("dnsctl running in 64-bit mode\n");
    else if ((sizeof(argv) == 4))
        printf("dnsctl running in 32-bit mode\n");

    // expects atleast one argument
    if (argc < 2)
        goto Usage;

    if ( !strcmp(argv[1], "-DP") || !strcmp(argv[1], "-dp") )
    {
        if (argc == 2)
        {
            printtimestamp();
            printf("Proceeding to Enable DNSProxy on mDNSResponder with Default Parameters\n");
            dispatch_queue_t my_Q = dispatch_queue_create("com.apple.dnsctl.callback_queue", NULL);
            err = DNSXEnableProxy(&ClientRef, kDNSProxyEnable, Ipintfs, Opintf, my_Q, dnsproxy_reply);
        }            
        else if (argc > 2)
        {
            argc--;
            argv++;
            if (!strcmp(argv[1], "-o"))
            {
                Opintf = if_nametoindex(argv[2]);
                if (!Opintf) 
                    Opintf = atoi(argv[2]);
                if (!Opintf) 
                { 
                    fprintf(stderr, "Could not parse o/p interface [%s]: Passing default primary \n", argv[2]); 
                    Opintf = kDNSIfindexAny;
                }
                argc -= 2;
                argv += 2;
            }
            if (argc > 2 && !strcmp(argv[1], "-i")) 
            {
                int i;
                argc--;
                argv++;
                for (i = 0; i < MaxInputIf && argc > 1; i++)
                {
                    Ipintfs[i] = if_nametoindex(argv[1]);
                    if (!Ipintfs[i])
                        Ipintfs[i] = atoi(argv[1]);  
                    if (!Ipintfs[i])
                    {
                        fprintf(stderr, "Could not parse i/p interface [%s]: Passing default lo0 \n", argv[2]); 
                        Ipintfs[i] = 1;
                    }
                    argc--;
                    argv++;
                }
            }  
            printtimestamp();
            printf("Proceeding to Enable DNSProxy on mDNSResponder \n");
            dispatch_queue_t my_Q = dispatch_queue_create("com.apple.dnsctl.callback_queue", NULL);
            err = DNSXEnableProxy(&ClientRef, kDNSProxyEnable, Ipintfs, Opintf, my_Q, dnsproxy_reply);                
        }
    }
    else
    {
        goto Usage;
    }

    dispatch_main(); 

Usage:
    print_usage(a0);
    return 0;
}

