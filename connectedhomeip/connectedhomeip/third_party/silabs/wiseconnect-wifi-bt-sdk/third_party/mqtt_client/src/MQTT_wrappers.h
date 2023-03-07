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

#ifndef __MQTT_LINUX_
#define __MQTT_LINUX_
#include "rsi_data_types.h"
#include "rsi_socket.h"


typedef struct Timer Timer;

struct Timer {
	uint32_t systick_period;
	uint32_t end_time;
};

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

int rsi_mqtt_read(Network* , unsigned char* , int , int );
int rsi_mqtt_write(Network* , unsigned char* , int , int );
char expired(Timer*);
void countdown_ms_mqtt(Timer*, unsigned int);
void countdown(Timer*, unsigned int);
int left_ms_mqtt(Timer*);

void InitTimer(Timer*);

int rsi_read(Network*, unsigned char*, int, int);
int rsi_write(Network*, unsigned char*, int, int);
void rsi_disconnect(Network*);
void NewNetwork(Network*);
void mqtt_disconnect(Network* n);

#ifdef ASYNC_MQTT
int ConnectNetwork(Network* n, uint8_t flags,char* addr, int dst_port, int src_port, void (*callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length));
#else
int ConnectNetwork(Network* n, uint8_t flags,char* addr, int dst_port, int src_port);
#endif

#endif
