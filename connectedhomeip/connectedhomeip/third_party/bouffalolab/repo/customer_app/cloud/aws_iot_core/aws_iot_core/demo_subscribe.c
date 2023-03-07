/*
 * Copyright (c) 2020 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#include <FreeRTOS.h>
#include <task.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
#include "aws_test_cert.h"

static void testdisconnectCallbackHandler(AWS_IoT_Client *pClient, void *data){
	printf("MQTT Disconnect\r\n");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		printf("Auto Reconnect is enabled, Reconnecting attempt will start now\r\n");
	} else {
		printf("Auto Reconnect not enabled. Starting manual reconnect...\r\n");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			printf("Manual Reconnect Successful\r\n");
		} else {
			printf("Manual Reconnect Failed - %d\r\n", rc);
		}
	}
}

static void test_iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
	printf("Subscribe callback\r\n");
	printf("%.*s\t%.*s\r\n", topicNameLen, topicName, (int) params->payloadLen, (char *) params->payload);
}

void aws_iot_demo_subscribe(void *arg) 
{
	IoT_Error_t rc = FAILURE;

	AWS_IoT_Client client;
	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	printf("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	mqttInitParams.enableAutoReconnect = false; 
	mqttInitParams.pHostURL = TEST_MQTT_HOST;
	mqttInitParams.port = TEST_MQTT_PORT;
	mqttInitParams.pRootCALocation = TEST_ROOT_CA_FILENAME;
	mqttInitParams.pDeviceCertLocation = TEST_CERTIFICATE_FILENAME;
	mqttInitParams.pDevicePrivateKeyLocation = TEST_PRIVATE_KEY_FILENAME;
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.isSSLHostnameVerify = true;
	mqttInitParams.disconnectHandler = testdisconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;

	rc = aws_iot_mqtt_init(&client, &mqttInitParams);
	if(SUCCESS != rc) {
		printf("aws_iot_mqtt_init returned error : %d \r\n", rc);
		goto exit;
	}

	connectParams.keepAliveIntervalInSec = 600;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = TEST_MQTT_CLIENT_ID;
	connectParams.clientIDLen = (uint16_t) strlen(TEST_MQTT_CLIENT_ID);
	connectParams.isWillMsgPresent = false;

	printf("Connecting...\r\n");
	rc = aws_iot_mqtt_connect(&client, &connectParams);
	if(SUCCESS != rc) {
		printf("Error(%d) connecting to %s:%d \r\n", rc, mqttInitParams.pHostURL, mqttInitParams.port);
		goto exit;
	}
	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *	#AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *	#AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	if(SUCCESS != rc) {
		printf("Unable to set Auto Reconnect to true - %d\r\n", rc);
		goto exit;
	}

	printf("Subscribing...");
	rc = aws_iot_mqtt_subscribe(&client, TEST_MYSUBTOPIC, strlen(TEST_MYSUBTOPIC), QOS0, test_iot_subscribe_callback_handler, NULL);
	if(SUCCESS != rc) {
		printf("Error subscribing : %d \r\n", rc);
		goto exit; 
	}
	
	while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)) {
		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(&client, 100);
		if(NETWORK_ATTEMPTING_RECONNECT == rc) {
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}
		
		vTaskDelay(100);
	}
exit:
	printf("\ntest task exit \r\n");
	if(SUCCESS != rc) {
        printf("An error occurred in the loop %d\r\n", rc);
    }
	aws_iot_mqtt_yield(&client, 100);
	aws_iot_mqtt_disconnect(&client);
	aws_iot_mqtt_free(&client);
	vTaskDelete(NULL);
}

