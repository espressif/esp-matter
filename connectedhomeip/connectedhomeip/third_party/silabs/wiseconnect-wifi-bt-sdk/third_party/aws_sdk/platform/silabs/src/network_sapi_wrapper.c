/*******************************************************************************
* @file network_sapi_wrapper.c
* @brief 
*******************************************************************************/ 
/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/*******************************************************************************
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>
#include <timer_platform.h>
#include <network_interface.h>
#include "aws_iot_error.h"
#include "aws_iot_log.h"
#include "network_interface.h"
#include "network_platform.h"
#include "rsi_socket.h"
#include "rsi_wlan_apis.h"
#include "rsi_common_apis.h"
#include "rsi_data_types.h"
#include "rsi_error.h"
#include "rsi_wlan.h"
#include "rsi_nwk.h"
#include "rsi_utils.h"
#include "rsi_driver.h"


/* This is the value used for ssl read timeout */
#define IOT_SSL_READ_TIMEOUT 10

/*This is the value used for dns request count*/
#define DNS_REQ_COUNT         5

#define RSI_SSL_BIT_ENABLE    1  

uint8_t ssl_bit_map       = RSI_SSL_BIT_ENABLE;
uint32_t ssl_cert_bit_map = RSI_CERT_INDEX_0; 

/* This defines the value of the debug buffer that gets allocated.
 * The value can be altered based on memory constraints
 */
#ifdef ENABLE_IOT_DEBUG
#define MBEDTLS_DEBUG_BUFFER_SIZE 2048
#endif

//! Client port number
#define CLIENT_PORT       4002

#define AWS_DOMAIN_NAME  "a25jwtlmds8eip-ats.iot.us-east-2.amazonaws.com"


/*
 * This is a function to do further verification if needed on the cert received
 */

int32_t get_aws_error(int32_t status)
{
	switch(status)
	{
	case RSI_ERROR_EBADF:/* Bad file number */
	case RSI_ERROR_EPROTOTYPE:/* Protocol wrong type for socket */
	case RSI_ERROR_EINVAL :/* Invalid argument */
	case RSI_ERROR_ENOBUFS:/* No buffer space available */
	case RSI_ERROR_PKT_ALLOCATION_FAILURE:
	case RSI_ERROR_ENOPROTOOPT:/* Protocol not available */
	case RSI_ERROR_EFAULT:/* Bad address */
	case RSI_ERROR_EAFNOSUPPORT:/* Address family not supported by protocol */
	case RSI_ERROR_EMSGSIZE:/* Message too long */
		return status;

	case 0xFF7E:
	case 0xBBED:
	case 0x003E:
	case 0x0030:
	case 0xBB3E:
		return LIMIT_EXCEEDED_ERROR;

	case 0x0015:
		return NETWORK_SSL_READ_TIMEOUT_ERROR;

	case 0xFF87:/* Connection refused */
	case 0xBB38:		
		/* module tried to connect to a non-existent TCPserver */
		return TCP_CONNECTION_ERROR;

	case 0x00D2:/* Software caused connection abort */
		/*SSL Handshake Failed. Socket will be closed*/
		return SSL_CONNECTION_ERROR;

	case 0xFF6C:/* Connection timed out */
	case 0xFF6A :
	case RSI_ERROR_RESPONSE_TIMEOUT:
		/*Web socket creation timeout*/
		return NETWORK_SSL_READ_TIMEOUT_ERROR;

	case 0x0021:/* Permission denied */
	case 0xFF82 :
	case 0xFF74:
	case 0xBB27:
	case 0xFF80:
	case 0x00D1:
		/*Feature not supported*/
		return TCP_CONNECTION_ERROR;

	case 0xBB22:/* Transport endpoint is already connected */
	case 0xBB42:
		return NETWORK_ALREADY_CONNECTED_ERROR;

	case 0xFFFF:/* Transport endpoint is not connected */
	case 0xBB50:
		return NETWORK_DISCONNECTED_ERROR;

	case 0xBB33:/* Address already in use */
	case 0xBB23:
	case 0xBB45:
	case 0xBB46:
		return NETWORK_ALREADY_CONNECTED_ERROR;

	case 0xBB36:/* Socket operation on non-socket */
		/* Socket is closed or in process of closing*/
		return NETWORK_DISCONNECTED_ERROR;
	default:
		return FAILURE;
	}
}

int _iot_tls_verify_cert(void *data, int *crt, int depth, uint32_t *flags)
{
  UNUSED_PARAMETER(data);
  UNUSED_PARAMETER(crt);
  UNUSED_PARAMETER(depth);
  UNUSED_PARAMETER(flags);
	return 0;
}

void _iot_tls_set_connect_params(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
		char *pDevicePrivateKeyLocation, char *pDestinationURL,
		uint16_t destinationPort, uint32_t timeout_ms, bool ServerVerificationFlag)
{
	pNetwork->tlsConnectParams.DestinationPort = destinationPort;
	pNetwork->tlsConnectParams.pDestinationURL = pDestinationURL;
	pNetwork->tlsConnectParams.pDeviceCertLocation = pDeviceCertLocation;
	pNetwork->tlsConnectParams.pDevicePrivateKeyLocation = pDevicePrivateKeyLocation;
	pNetwork->tlsConnectParams.pRootCALocation = pRootCALocation;
	pNetwork->tlsConnectParams.timeout_ms = timeout_ms;
	pNetwork->tlsConnectParams.ServerVerificationFlag = ServerVerificationFlag;
}

IoT_Error_t iot_tls_init(Network *pNetwork, char *pRootCALocation, char *pDeviceCertLocation,
		char *pDevicePrivateKeyLocation, char *pDestinationURL,
		uint16_t destinationPort, uint32_t timeout_ms, bool ServerVerificationFlag)
{
	_iot_tls_set_connect_params(pNetwork, pRootCALocation, pDeviceCertLocation, pDevicePrivateKeyLocation,
			pDestinationURL, destinationPort, timeout_ms, ServerVerificationFlag);

	pNetwork->connect = iot_tls_connect;

	pNetwork->read = iot_tls_read;
	pNetwork->write = iot_tls_write;
	pNetwork->disconnect = iot_tls_disconnect;
	pNetwork->isConnected = iot_tls_is_connected;
	pNetwork->destroy = iot_tls_destroy;

	return SUCCESS;
}

IoT_Error_t iot_tls_is_connected(Network *pNetwork) {
  UNUSED_PARAMETER(pNetwork);
	/* Use this to add implementation which can check for physical layer disconnect */
	return NETWORK_PHYSICAL_LAYER_CONNECTED;
}


int ConnecttoNetwork(Network* n, uint8_t flags,char* addr, int dst_port, int src_port)
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

		if(flags & RSI_SSL_ENABLE)
			n->socket_id = rsi_socket(AF_INET6, type, RSI_SOCKET_FEAT_SSL);
		else
			n->socket_id = rsi_socket(AF_INET6, type,0);

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

		if(flags & RSI_SSL_ENABLE) {
			n->socket_id = rsi_socket(AF_INET, type, 0);
    	status = rsi_setsockopt(n->socket_id,SOL_SOCKET,SO_SSL_ENABLE,&ssl_bit_map,sizeof(ssl_bit_map));
    	status = rsi_setsockopt(n->socket_id,SOL_SOCKET,SO_CERT_INDEX,&ssl_cert_bit_map,sizeof(ssl_cert_bit_map));
    }
		else
			n->socket_id = rsi_socket(AF_INET, type, 0);

		clientAddr.sin_family= AF_INET;
		clientAddr.sin_port = htons(src_port);

		/* Set all bits of the padding field to 0 */
		memset(clientAddr.sin_zero, '\0', sizeof(clientAddr.sin_zero));
	}
	if (n->socket_id == -1)
	{
		return NETWORK_ERR_NET_SOCKET_FAILED;
	}

	if(flags == RSI_IPV6)
	{
		//! Bind socket
		status = rsi_bind(n->socket_id, (struct rsi_sockaddr *) &clientAddr_v6, sizeof(clientAddr_v6));

	}
	else
	{
		//! Bind socket
		status = rsi_bind(n->socket_id, (struct rsi_sockaddr *) &clientAddr, sizeof(clientAddr));


	}
	if(status != 0)
	{
		//! Shut Down the port
		//  mqtt_disconnect(n);
		status = rsi_wlan_socket_get_status(n->socket_id);
		return get_aws_error(status);
	}
	if(flags == RSI_IPV6)
	{
		rc = rsi_connect(n->socket_id, (struct rsi_sockaddr*)&address_v6, sizeof(address_v6));
	}
	else
	{
		rc = rsi_connect(n->socket_id, (struct rsi_sockaddr*)&address, sizeof(address));

	}
	if(rc == -1)
	{
		status = rsi_wlan_socket_get_status(n->socket_id);
		return get_aws_error(status);
		//! Shut Down the port
		//mqtt_disconnect(n);
	}
	return status;
}
IoT_Error_t iot_tls_connect(Network *pNetwork, TLSConnectParams *params)
{
  UNUSED_PARAMETER(params);
	int32_t     status       = 0;
	rsi_rsp_dns_query_t dns_query_rsp;
	uint32_t    server_address =  0;
	uint8_t  count = DNS_REQ_COUNT;

  do{
    status = rsi_dns_req(RSI_IP_VERSION_4, (uint8_t *)pNetwork->tlsConnectParams.pDestinationURL, NULL, NULL, &dns_query_rsp, sizeof(dns_query_rsp));
    if(status == SUCCESS)
    {			
      break;
    }		
    count --;
  }while(count != 0);

	if(status != SUCCESS)
	{
		return NETWORK_ERR_NET_UNKNOWN_HOST;
	}
	server_address = rsi_bytes4R_to_uint32(dns_query_rsp.ip_address[0].ipv4_address);

	status = ConnecttoNetwork(pNetwork, 2,(char *) &server_address, pNetwork->tlsConnectParams.DestinationPort, CLIENT_PORT);       //fixme:flags kept as 0

	return (IoT_Error_t)status;
}

IoT_Error_t iot_tls_write(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *written_len)
{
	size_t written_so_far;
	bool isErrorFlag = false;
	int frags;
	int ret = 0;
	for(written_so_far = 0, frags = 0;
			written_so_far < len && !has_timer_expired(timer); written_so_far += ret, frags++)
	{
		while(!has_timer_expired(timer) &&
				(ret = rsi_send(pNetwork->socket_id,(int8_t *)(pMsg + written_so_far), len - written_so_far,0)) <= 0)      //FIXME:flags parameter kept as 0
		{
			isErrorFlag = true;
		}
		if(isErrorFlag)
		{
			break;
		}
	}

	*written_len = written_so_far;

	if(isErrorFlag)
	{
		return NETWORK_SSL_WRITE_ERROR;
	} 
	else if(has_timer_expired(timer) && written_so_far != len)
	{
		return NETWORK_SSL_WRITE_TIMEOUT_ERROR;
	}
	return SUCCESS;
}

IoT_Error_t iot_tls_read(Network *pNetwork, unsigned char *pMsg, size_t len, Timer *timer, size_t *read_len)
{
	size_t rxLen = 0;
	int ret;
	int32_t     status       = 0;
	struct rsi_timeval timeout;

	memset(&timeout,0,sizeof(timeout));

	timeout.tv_usec = timer->timeout*1000;

	timeout.tv_sec = 0;

	status = rsi_setsockopt(pNetwork->socket_id, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	if(status != SUCCESS)
	{
		return NETWORK_SSL_READ_ERROR;
	}
	while (len > 0)
	{
		// This read will timeout after IOT_SSL_READ_TIMEOUT if there's no data to be read
		ret = rsi_recv(pNetwork->socket_id,pMsg, len, 0);  //FIXME:flags parameter kept as 0
		if (ret > 0)
		{
			rxLen += ret;
			pMsg += ret;
			len -= ret;
		}
		else if (ret == 0)
		{
			return NETWORK_SSL_READ_ERROR;
		}
		else //ret<0
		{
			status = rsi_wlan_socket_get_status(pNetwork->socket_id);
			return (IoT_Error_t)get_aws_error(status);
		}

		// Evaluate timeout after the read to make sure read is done at least once
		if (has_timer_expired(timer))
		{
			break;
		}
	}

	if (len == 0)
	{
		*read_len = rxLen;
		return SUCCESS;
	}

	if (rxLen == 0)
	{
		return NETWORK_SSL_NOTHING_TO_READ;
	} else
	{
		return NETWORK_SSL_READ_TIMEOUT_ERROR;
	}
}

IoT_Error_t iot_tls_disconnect(Network *pNetwork)
{
	int32_t   status = RSI_SUCCESS;
	status = rsi_shutdown(pNetwork->socket_id,0);
	if(status != RSI_SUCCESS)
	{
		return (IoT_Error_t)rsi_wlan_socket_get_status(pNetwork->socket_id);
	}
	return (IoT_Error_t)status;
}

IoT_Error_t iot_tls_destroy(Network *pNetwork)
{
	return (IoT_Error_t)rsi_wlan_socket_get_status(pNetwork->socket_id);
}

#ifdef __cplusplus
}
#endif
