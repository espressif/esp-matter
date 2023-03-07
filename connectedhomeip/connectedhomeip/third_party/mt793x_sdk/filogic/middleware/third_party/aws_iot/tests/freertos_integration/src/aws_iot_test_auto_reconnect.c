/*
* Copyright 2015-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
* http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/

/**
 * @file aws_iot_test_auto_reconnect.c
 * @brief Integration Test for automatic reconnect
 */

#include "aws_iot_test_integration_common.h"

//static char ModifiedPathBuffer[PATH_MAX + 1];
//char root_CA[PATH_MAX + 1];

bool terminate_yield_with_rc_thread = false;
IoT_Error_t yieldRC;
bool captureYieldReturnCode = false;
static AWS_IoT_Client client;

bool has_autoconnect = false;
bool has_disconnect = false;

#if 0
/**
 * This function renames the rootCA.crt file to a temporary name to cause connect failure
 */
int aws_iot_mqtt_tests_block_tls_connect() {
	char replaceFileName[] = {"rootCATemp.crt"};
	char *pFileNamePosition = NULL;

	char mvCommand[2 * PATH_MAX + 10];
	strcpy(ModifiedPathBuffer, root_CA);
	pFileNamePosition = strstr(ModifiedPathBuffer, AWS_IOT_ROOT_CA_FILENAME);
	strncpy(pFileNamePosition, replaceFileName, strlen(replaceFileName));
	snprintf(mvCommand, 2 * PATH_MAX + 10, "mv %s %s", root_CA, ModifiedPathBuffer);
	return system(mvCommand);
}

/**
 * Always ensure this function is called after block_tls_connect
 */
int aws_iot_mqtt_tests_unblock_tls_connect() {
	char mvCommand[2 * PATH_MAX + 10];
	snprintf(mvCommand, 2 * PATH_MAX + 10, "mv %s %s", ModifiedPathBuffer, root_CA);
	return system(mvCommand);
}
#endif

void aws_iot_mqtt_tests_yield_with_rc(void *ptr) {
	IoT_Error_t rc = SUCCESS;

	static int cntr = 0;
	AWS_IoT_Client *pClient = &client;// (AWS_IoT_Client *) client;

	while(terminate_yield_with_rc_thread == false
		  && (NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)) {
		vTaskDelay(500000 / 1000);
		printf(" Client state : %d \n", aws_iot_mqtt_get_client_state(pClient));
		rc = aws_iot_mqtt_yield(pClient, 100);
		printf("yield rc %d\n", rc);
		if(captureYieldReturnCode && SUCCESS != rc) {
			printf("yield rc capture %d\n", rc);
			captureYieldReturnCode = false;
			yieldRC = rc;
		}
	}
    printf("[AutoReconnect] yield task finished\n");
    vTaskDelete(NULL);
}

unsigned int disconnectedCounter = 0;

void aws_iot_mqtt_tests_disconnect_callback_handler(AWS_IoT_Client *pClient, void *param) {
	disconnectedCounter++;
}

int aws_iot_mqtt_tests_auto_reconnect() {

	int test_result = 0;

	char clientId[50];

	IoT_Error_t rc = SUCCESS;

	snprintf(clientId, 50, "%s_%d", INTEGRATION_TEST_CLIENT_ID, 10);

	IoT_Client_Init_Params initParams;
	initParams.pHostURL = AWS_IOT_MQTT_HOST;
	initParams.port = 8883;
	initParams.pRootCALocation = AWS_IOT_ROOT_CA_CERT;
	initParams.pDeviceCertLocation = AWS_IOT_DEVICE_CERT;
	initParams.pDevicePrivateKeyLocation = AWS_IOT_PRIVATE_KEY;
	initParams.mqttCommandTimeout_ms = 20000;
	initParams.tlsHandshakeTimeout_ms = 15000;
	initParams.disconnectHandler = aws_iot_mqtt_tests_disconnect_callback_handler;
	initParams.enableAutoReconnect = false;
	aws_iot_mqtt_init(&client, &initParams);

	IoT_Client_Connect_Params connectParams;
	connectParams.keepAliveIntervalInSec = 5;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = (char *) &clientId;
	connectParams.clientIDLen = strlen(clientId);
	connectParams.isWillMsgPresent = 0;
	connectParams.pUsername = NULL;
	connectParams.usernameLen = 0;
	connectParams.pPassword = NULL;
	connectParams.passwordLen = 0;

	rc = aws_iot_mqtt_connect(&client, &connectParams);
	if(rc != SUCCESS) {
		printf("ERROR Connecting %d\n", rc);
		return -1;
	}
    xTaskCreate(aws_iot_mqtt_tests_yield_with_rc, "yr_1",
                        (2 * 1024) / ((uint32_t)sizeof(StackType_t)), &client, TASK_PRIORITY_NORMAL, NULL);

	/*
	 * Test disconnect handler
	 */
	printf("1. Test Disconnect Handler\n");
//	aws_iot_mqtt_tests_block_tls_connect();
	iot_tls_disconnect(&(client.networkStack));

	vTaskDelay(10000 / portTICK_RATE_MS);
	if(disconnectedCounter == 1) {
		printf("Success invoking Disconnect Handler\n");
	} else {
//		aws_iot_mqtt_tests_unblock_tls_connect();
		printf("Failure to invoke Disconnect Handler\n");
        aws_iot_mqtt_disconnect(&client);
		return -1;
	}
//	aws_iot_mqtt_tests_unblock_tls_connect();
	terminate_yield_with_rc_thread = true;
	vTaskDelay(1000);

	/*
	 * Manual Reconnect Test
	 */
	printf("2. Test Manual Reconnect, Current Client state : %d \n", aws_iot_mqtt_get_client_state(&client));
	rc = aws_iot_mqtt_attempt_reconnect(&client);
	if(rc != NETWORK_RECONNECTED) {
		printf("ERROR reconnecting manually %d\n", rc);
		return -4;
	}
    printf("Test Manu Reconnect :: Attempt Reconnect Result : %d\n", rc);
	terminate_yield_with_rc_thread = false;

    xTaskCreate(aws_iot_mqtt_tests_yield_with_rc, "yr_2",
                    (2 * 1024) / ((uint32_t)sizeof(StackType_t)), &client, TASK_PRIORITY_NORMAL, NULL);

	yieldRC = FAILURE;
	captureYieldReturnCode = true;

	// ensure atleast 1 cycle of yield is executed to get the yield status to SUCCESS
	vTaskDelay(1000);
    
	if(!captureYieldReturnCode) {
		if(yieldRC == NETWORK_ATTEMPTING_RECONNECT) {
			printf("Success reconnecting manually\n");
		} else {
			printf("Failure to reconnect manually\n");
            aws_iot_mqtt_disconnect(&client);
			return -3;
		}
	}
	terminate_yield_with_rc_thread = true;
	/*
	 * Auto Reconnect Test
	 */
	printf("3. Test Auto_reconnect \n");

	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	if(rc != SUCCESS) {
		printf("Error: Failed to enable auto-reconnect %d \n", rc);
	}

	yieldRC = FAILURE;
	captureYieldReturnCode = true;

	// Disconnect
//	aws_iot_mqtt_tests_block_tls_connect();
	iot_tls_disconnect(&(client.networkStack));

	terminate_yield_with_rc_thread = false;

    xTaskCreate(aws_iot_mqtt_tests_yield_with_rc, "yr_3",
                            (5 * 1024) / ((uint32_t)sizeof(StackType_t)), /*&client*/ NULL, TASK_PRIORITY_NORMAL, NULL);

	vTaskDelay(10000 / portTICK_RATE_MS);
	if(!captureYieldReturnCode) {
		if(yieldRC == NETWORK_ATTEMPTING_RECONNECT) {
			printf("Success attempting reconnect\n");
		} else {
			printf("Failure to attempt to reconnect\n");
            aws_iot_mqtt_disconnect(&client);
			return -6;
		}
	}
	if(disconnectedCounter == 2) {
		printf("Success: disconnect handler invoked on enabling auto-reconnect\n");
	} else {
		printf("Failure: disconnect handler not invoked on enabling auto-reconnect : %d\n", disconnectedCounter);
        aws_iot_mqtt_disconnect(&client);
		return -7;
	}
//	aws_iot_mqtt_tests_unblock_tls_connect();
	//vTaskDelay((connectParams.keepAliveIntervalInSec + 1) * 1000);
	vTaskDelay(10000 / portTICK_RATE_MS);
	captureYieldReturnCode = true;
    terminate_yield_with_rc_thread = true;
	//vTaskDelay((connectParams.keepAliveIntervalInSec + 1) * 1000);
	vTaskDelay(5000 / portTICK_RATE_MS);
	if(!captureYieldReturnCode) {
		if(yieldRC == SUCCESS) {
			printf("Success attempting reconnect\n");
		} else {
			printf("Failure to attempt to reconnect\n");
			return -6;
		}
	}
	if(true == aws_iot_mqtt_is_client_connected(&client)) {
		printf("Success: is Mqtt connected api\n");
	} else {
		printf("Failure: is Mqtt Connected api\n");
		return -7;
	}
#if 0

    while (1)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
        aws_iot_mqtt_yield(&client, 100);

        if (yieldRC == NETWORK_ATTEMPTING_RECONNECT){
			printf("auto: Success attempting reconnect\n");
            has_autoconnect = true;
            continue;
        }

         if (has_autoconnect && disconnectedCounter == 2) {
			printf("auto: Success disconnect\n");
            has_disconnect = true;
            continue;
        }

        if (has_disconnect && aws_iot_mqtt_is_client_connected(&client)) {
    		printf("Success: is Mqtt connected api\n");
            break;
        }
    }
    
    printf("Success: auto reconnect");
#endif

	rc = aws_iot_mqtt_disconnect(&client);
	return rc;
}
