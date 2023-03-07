/*
 * Demo for libcoap on lwIP
 *
 * partially copied from lwip-contrib/ports/unix/proj/minimal/main.c
 *
 *
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 * RT timer modifications by Christiaan Simons
 * lwip adaptions: chrysn <chrysn@fsfe.org>
 * also, https://savannah.nongnu.org/bugs/?40245 was applied */

#include <lwip/init.h>
#include <lwip/timers.h>

#include <mintapif.h>
#include <netif/etharp.h>

#include "timer.h"
#include <signal.h>

#include <server-coap.h>

static ip_addr_t ipaddr, netmask, gw;

int
main(int argc, char **argv)
{
	struct netif netif;
	sigset_t mask, oldmask, empty;

	/* startup defaults (may be overridden by one or more opts) */
	IP4_ADDR(&gw, 192,168,0,1);
	IP4_ADDR(&ipaddr, 192,168,0,2);
	IP4_ADDR(&netmask, 255,255,255,0);

	lwip_init();

	printf("TCP/IP initialized.\n");

	netif_add(&netif, &ipaddr, &netmask, &gw, NULL, mintapif_init, ethernet_input);
	netif_set_default(&netif);
	netif_set_up(&netif);
#if LWIP_IPV6
	netif_create_ip6_linklocal_address(&netif, 1);
#endif 

	timer_init();

	sys_timeouts_init();

	/* start applications here */

	server_coap_init();

	printf("Applications started.\n");


	while (1) {

		/* poll for input packet and ensure
		 select() or read() arn't interrupted */
		sigemptyset(&mask);
		sigaddset(&mask, SIGALRM);
		sigprocmask(SIG_BLOCK, &mask, &oldmask);

		/* start of critical section,
		 poll netif, pass packet to lwIP */
		if (mintapif_select(&netif) > 0)
		{
			/* work, immediatly end critical section 
			   hoping lwIP ended quickly ... */
			sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}
		else
		{
			/* no work, wait a little (10 msec) for SIGALRM */
			  sigemptyset(&empty);
			  sigsuspend(&empty);
			/* ... end critical section */
			  sigprocmask(SIG_SETMASK, &oldmask, NULL);
		}

		sys_check_timeouts();

		server_coap_poll();
	}

	return 0;
}
