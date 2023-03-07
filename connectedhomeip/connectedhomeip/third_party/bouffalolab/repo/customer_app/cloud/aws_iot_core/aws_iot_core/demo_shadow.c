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


static void ShadowUpdateStatusCallback(const char *pThingName, ShadowActions_t action, Shadow_Ack_Status_t status,
								const char *pReceivedJsonDocument, void *pContextData) {
	
	if(SHADOW_ACK_TIMEOUT == status) {
		printf("Update Timeout--\r\n");
	} else if(SHADOW_ACK_REJECTED == status) {
		printf("Update RejectedXX\r\n");
	} else if(SHADOW_ACK_ACCEPTED == status) {
		printf("Update Accepted !!\r\n");
	}
}

static int _update_mqtt_config_default(ShadowInitParameters_t *sp, ShadowConnectParameters_t *scp)
{
    sp->pHost = TEST_MQTT_HOST;
    printf(" URL:%s\r\n", sp->pHost);

    sp->port = TEST_MQTT_PORT;
    printf(", port is %d\r\n", sp->port);

    sp->pClientCRT = TEST_CERTIFICATE_FILENAME;
    sp->pClientKey = TEST_PRIVATE_KEY_FILENAME;
    sp->pRootCA = TEST_ROOT_CA_FILENAME;
    sp->enableAutoReconnect = false;
    sp->disconnectHandler = NULL;

    scp->pMqttClientId = TEST_MQTT_CLIENT_ID;
    printf(", THING ID is %s\r\n", scp->pMqttClientId);
    scp->mqttClientIdLen = strlen(scp->pMqttClientId);

    printf("thing name len is %d", strlen(TEST_MY_THING_NAME));
    scp->pMyThingName = TEST_MY_THING_NAME;
    printf(", THING name is %s\r\n", scp->pMyThingName);

    return 0;
}

void aws_iot_demo_shadow(void *arg) 
{
    IoT_Error_t rc = FAILURE;
	int testflag = 0;
	ShadowInitParameters_t sp = ShadowInitParametersDefault;
    ShadowConnectParameters_t scp = ShadowConnectParametersDefault;

	char JsonDocumentBuffer[200];
	size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);
	char LightStatus[10];
	memset(LightStatus, 0, 10);
	memcpy(LightStatus, "OFF", strlen("OFF"));
	jsonStruct_t lightstatus;
	lightstatus.cb = NULL;
	lightstatus.pData = LightStatus;
	lightstatus.dataLength = strlen(LightStatus);
	lightstatus.pKey = "lightstatus";
	lightstatus.type = SHADOW_JSON_STRING;

	// initialize the mqtt client
	AWS_IoT_Client mqttClient;
	_update_mqtt_config_default(&sp, &scp);

	printf("Shadow Init\r\n");
	rc = aws_iot_shadow_init(&mqttClient, &sp);
	if(SUCCESS != rc) {
		printf("Shadow Connection Error\r\n");
		goto exit;
	}

	printf("Shadow Connect\r\n");
	rc = aws_iot_shadow_connect(&mqttClient, &scp);
	if(SUCCESS != rc) {
		printf("Shadow Connection Error\r\n");
		goto exit;
	}

	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_shadow_set_autoreconnect_status(&mqttClient, true);
	if(SUCCESS != rc) {
		printf("Unable to set Auto Reconnect to true - %d \r\n", rc);
		goto exit; 
	}

	rc = aws_iot_shadow_register_delta(&mqttClient, &lightstatus);
	if(SUCCESS != rc) {
		printf("Shadow Register Delta Error\r\n");
		goto exit;
	}

	// loop and publish a change in temperature
	while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
		rc = aws_iot_shadow_yield(&mqttClient, 200);
		if(NETWORK_ATTEMPTING_RECONNECT == rc) {
			vTaskDelay(1000);
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}
        if (!testflag){
			testflag = 1;
			memset(LightStatus, 0, 10);
			memcpy(LightStatus, "ON", strlen("ON"));
        }else{
            testflag = 0;
			memset(LightStatus, 0, 10);
			memcpy(LightStatus, "OFF", strlen("OFF"));
        }
		lightstatus.pData = LightStatus;
		lightstatus.dataLength = strlen(LightStatus);
		rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
		if(SUCCESS == rc) {
			rc = aws_iot_shadow_add_reported(JsonDocumentBuffer, sizeOfJsonDocumentBuffer, 1, &lightstatus);
			if(SUCCESS == rc) {
				rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
				if(SUCCESS == rc) {
					printf("Update Shadow: %s\r\n", JsonDocumentBuffer);
					rc = aws_iot_shadow_update(&mqttClient, TEST_MY_THING_NAME, JsonDocumentBuffer,
											   ShadowUpdateStatusCallback, NULL, 4, true);
				}
			}
		}
		
		vTaskDelay(1000);
	}
 exit:
	printf("\ntest task exit \r\n");
	if(SUCCESS != rc) {
        printf("An error occurred in the loop %d\r\n", rc);
    }
	aws_iot_shadow_yield(&mqttClient, 100);
	aws_iot_shadow_disconnect(&mqttClient);
	aws_iot_shadow_free(&mqttClient);
	vTaskDelete(NULL);   
}


