// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "mbed.h"

#include <stddef.h>
#include "TCPSocket.h"
#include "azure_c_shared_utility/tcpsocketconnection_c.h"

// The NetworkInterface instance of network device
extern NetworkInterface *_defaultSystemNetwork;

static volatile bool tcpsocketconnection_isConnected = false;

TCPSOCKETCONNECTION_HANDLE tcpsocketconnection_create(void)
{
	TCPSocket* tcpSocket = new TCPSocket();
	tcpSocket->open(_defaultSystemNetwork);
    return tcpSocket;
}

void tcpsocketconnection_set_blocking(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, bool blocking, unsigned int timeout)
{
	if (tcpSocketConnectionHandle != NULL)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		tsc->set_blocking(blocking);
		tsc->set_timeout(timeout);
	}
}

void tcpsocketconnection_destroy(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
	if (tcpSocketConnectionHandle != NULL)
	{
		delete (TCPSocket*)tcpSocketConnectionHandle;
	}
}

int tcpsocketconnection_connect(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* host, const int port)
{
	int ret = 0;
	if (tcpSocketConnectionHandle != NULL && !tcpsocketconnection_isConnected)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		if ( (ret = tsc->connect(host, port)) == 0)
		{
			tcpsocketconnection_isConnected = true;
		}
	}
	return ret;
}

bool tcpsocketconnection_is_connected(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
	TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
	return tcpsocketconnection_isConnected;
}

void tcpsocketconnection_close(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle)
{
	if (tcpSocketConnectionHandle != NULL && tcpsocketconnection_isConnected)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		tsc->close();
		tcpsocketconnection_isConnected = false;
	}
}

int tcpsocketconnection_send(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* data, int length)
{
	if (tcpSocketConnectionHandle != NULL)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		return tsc->send((char*)data, length);
	}
	
	return -1;
}

int tcpsocketconnection_send_all(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, const char* data, int length)
{
	if (tcpSocketConnectionHandle != NULL)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		return tsc->send((char*)data, length);
	}
	return -1;
}

int tcpsocketconnection_receive(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, char* data, int length)
{
	if (tcpSocketConnectionHandle != NULL)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		return tsc->recv(data, length);
	}
	return -1;
}

int tcpsocketconnection_receive_all(TCPSOCKETCONNECTION_HANDLE tcpSocketConnectionHandle, char* data, int length)
{
	if (tcpSocketConnectionHandle != NULL)
	{
		TCPSocket* tsc = (TCPSocket*)tcpSocketConnectionHandle;
		return tsc->recv(data, length);
	}
	return -1;
}
