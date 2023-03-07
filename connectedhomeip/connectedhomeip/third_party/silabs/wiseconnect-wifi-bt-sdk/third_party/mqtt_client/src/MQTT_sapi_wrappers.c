/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

/*******************************************************************************
* @file  MQTT_sapi_wrappers.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#include "MQTT_wrappers.h"
#ifndef RSI_SAMPLE_HAL
#include "timetick.h"
#endif
#include <rsi_driver.h>
#include "rsi_pkt_mgmt.h"
#include "rsi_nwk.h"
#include "rsi_utils.h"
#include "rsi_timer.h"
#include "rsi_wlan_apis.h"
#include <string.h>
char expired(Timer* timer) 
{
	long left = timer->end_time - rsi_timer_read_counter();
	return (left < 0);
}


void countdown_ms_mqtt(Timer* timer, unsigned int timeout)
{
	timer->end_time = rsi_timer_read_counter() + timeout;
}


void countdown(Timer* timer, unsigned int timeout) 
{
	timer->end_time = rsi_timer_read_counter() + (timeout * 1000);
}


int left_ms_mqtt(Timer* timer)
{
	long left = timer->end_time -rsi_timer_read_counter();
	return (left < 0) ? 0 : left;
}


void InitTimer(Timer* timer)
{
	timer->end_time = 0;
}


int rsi_mqtt_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	int bytes = 0;
	int32_t err = 0;
	struct rsi_timeval timeout;

	memset(&timeout,0,sizeof(timeout));
  

	if(timeout_ms == 0)
	{
		//! Set error

		return RSI_SOCK_ERROR;
	}

	timeout.tv_usec = timeout_ms*1000; 

	timeout.tv_sec = 0;

	rsi_setsockopt(n->my_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	while (bytes < len)
	{

		int rc = rsi_recv(n->my_socket, &buffer[bytes],(len - bytes), 0);
		if (rc == -1)
		{
			err = rsi_wlan_get_nwk_status();
			if (err != RSI_ERROR_ENOTCONN && err != RSI_ERROR_ECONNRESET)
			{
				bytes = -1;
				break;
			}
		}
		else
			bytes += rc;
	}
	return bytes;
}


int rsi_mqtt_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	UNUSED_PARAMETER(timeout_ms);
	int rc =  rsi_send(n->my_socket, (const int8_t *)buffer,len, 0);
	return rc;
}


void mqtt_disconnect(Network* n)
{
	rsi_shutdown(n->my_socket,0);
}


void NewNetwork(Network* n)
{
	n->my_socket = 0;
	n->mqttread = rsi_mqtt_read;
	n->mqttwrite = rsi_mqtt_write;
	n->disconnect = mqtt_disconnect;
}

#ifdef ASYNC_MQTT
int ConnectNetwork(Network* n, uint8_t flags,char* addr, int dst_port, int src_port, void (*callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length))
#else
int ConnectNetwork(Network* n, uint8_t flags,char* addr, int dst_port, int src_port)
#endif
{
	int type = SOCK_STREAM;
	struct rsi_sockaddr_in address,clientAddr;
	struct rsi_sockaddr_in6 address_v6,clientAddr_v6;
	int rc = -1,status = 0;


	memset(&address, 0, sizeof(address));
	memset(&address_v6, 0, sizeof(address_v6));

	if(flags == RSI_IPV6)
	{

		address_v6.sin6_family = AF_INET6;

		address_v6.sin6_port = htons(dst_port);

		memcpy(&address_v6.sin6_addr._S6_un._S6_u8, addr, RSI_IPV6_ADDRESS_LENGTH);

#ifdef ASYNC_MQTT
		if(flags & RSI_SSL_ENABLE)
			n->my_socket = rsi_socket_async(AF_INET, type, RSI_SOCKET_FEAT_SSL,callback);
		else
			n->my_socket = rsi_socket_async(AF_INET, type, 0, callback);

#else
		if(flags & RSI_SSL_ENABLE)
			n->my_socket = rsi_socket(AF_INET6, type, RSI_SOCKET_FEAT_SSL);
		else
			n->my_socket = rsi_socket(AF_INET6, type,0);
#endif

		clientAddr_v6.sin6_family= AF_INET6;
		clientAddr_v6.sin6_port = htons(src_port);


	}
	else
	{
		//! Set family type
		address.sin_family= AF_INET;

		//! Set local port number
		address.sin_port = htons(dst_port);


		address.sin_addr.s_addr = rsi_bytes4R_to_uint32((uint8_t *)addr);


#ifdef ASYNC_MQTT
		if(flags & RSI_SSL_ENABLE)
			n->my_socket = rsi_socket_async(AF_INET, type, RSI_SOCKET_FEAT_SSL,callback);
		else
			n->my_socket = rsi_socket_async(AF_INET, type, 0, callback);

#else
		if(flags & RSI_SSL_ENABLE)
			n->my_socket = rsi_socket(AF_INET, type, RSI_SOCKET_FEAT_SSL);
		else
			n->my_socket = rsi_socket(AF_INET, type, 0);
#endif
		clientAddr.sin_family= AF_INET;
		clientAddr.sin_port = htons(src_port);

		/* Set all bits of the padding field to 0 */
		memset(clientAddr.sin_zero, '\0', sizeof(clientAddr.sin_zero));

	}

	if (n->my_socket == -1)
	{
		status = rsi_wlan_get_nwk_status();
		return status;
	}

	if(flags == RSI_IPV6)
	{
		//! Bind socket
		status = rsi_bind(n->my_socket, (struct rsi_sockaddr *) &clientAddr_v6, sizeof(clientAddr_v6));

	}
	else
	{
		//! Bind socket
		status = rsi_bind(n->my_socket, (struct rsi_sockaddr *) &clientAddr, sizeof(clientAddr));


	}
	if(status != RSI_SUCCESS)
	{
		status = rsi_wlan_get_nwk_status();
        //! Shut Down the port
        mqtt_disconnect(n);
		return status;
	}
	if(flags == RSI_IPV6)
	{
		rc = rsi_connect(n->my_socket, (struct rsi_sockaddr*)&address_v6, sizeof(address_v6));
	}
	else
	{
		rc = rsi_connect(n->my_socket, (struct rsi_sockaddr*)&address, sizeof(address));

	}
	if(rc == -1)
	{
      status = rsi_wlan_get_nwk_status();
      //! Shut Down the port
      mqtt_disconnect(n);
	}
  return status;

}
