/*
 * multithreadedTest.c
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_log.h"

#include "aws_iot_integ_tests_config.h"
#include "aws_iot_test_integration_common.h"


static bool terminate_yield_thread;
static bool terminate_subUnsub_thread;

static unsigned int countArray[MAX_PUB_THREAD_COUNT][PUBLISH_COUNT];
static unsigned int rxMsgBufferTooBigCounter;
static unsigned int rxUnexpectedNumberCounter;
static unsigned int rePublishCount;
static unsigned int wrongYieldCount;
static unsigned int threadStatus[MAX_PUB_THREAD_COUNT];

typedef struct ThreadData {
	int threadId;
	AWS_IoT_Client *client;
} ThreadData;

static void aws_iot_mqtt_tests_message_aggregator(AWS_IoT_Client *pClient, char *topicName,
							  uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData) {
	char tempBuf[30];
	char *temp = NULL;
	unsigned int tempRow = 0, tempCol = 0;
	IoT_Error_t rc;

	IOT_UNUSED(pClient);
	IOT_UNUSED(topicName);
	IOT_UNUSED(topicNameLen);
	IOT_UNUSED(pData);

	if(30 >= params->payloadLen) {
		snprintf(tempBuf, params->payloadLen, params->payload);
		printf("\n Message received : %s", tempBuf);
		temp = strtok(tempBuf, " ,:");
		temp = strtok(NULL, " ,:");
		if(NULL == temp) {
			return;
		}
		tempRow = atoi(temp);
		temp = strtok(NULL, " ,:");
		temp = strtok(NULL, " ,:");
		tempCol = atoi(temp);
		if(NULL == temp) {
			return;
		}
		if(((tempRow - 1) < MAX_PUB_THREAD_COUNT) && (tempCol < PUBLISH_COUNT)) {
			countArray[tempRow - 1][tempCol]++;
		} else {
			IOT_WARN(" \nUnexpected Thread : %d, Message : %d ", tempRow, tempCol);
			rxUnexpectedNumberCounter++;
		}
		rc = aws_iot_mqtt_yield(pClient, 10);
		if(MQTT_CLIENT_NOT_IDLE_ERROR != rc) {
			IOT_ERROR("\n Yield succeeded in callback!!! Client state : %d Rc : %d\n",
				  aws_iot_mqtt_get_client_state(pClient), rc);
			wrongYieldCount++;
		}
	} else {
		rxMsgBufferTooBigCounter++;
	}
}

static void aws_iot_mqtt_tests_disconnect_callback_handler(AWS_IoT_Client *pClient, void *param) {
	IOT_UNUSED(pClient);
	IOT_UNUSED(param);
}

static IoT_Error_t aws_iot_mqtt_tests_subscribe_to_test_topic(AWS_IoT_Client *pClient, QoS qos) {
	IoT_Error_t rc = SUCCESS;
	rc = aws_iot_mqtt_subscribe(pClient, INTEGRATION_TEST_TOPIC, strlen(INTEGRATION_TEST_TOPIC), qos,
								aws_iot_mqtt_tests_message_aggregator, NULL);
	IOT_DEBUG(" Sub response : %d\n", rc);

	return rc;
}

static void aws_iot_mqtt_tests_yield_thread_runner(void *ptr) {
	IoT_Error_t rc = SUCCESS;
	AWS_IoT_Client *pClient = (AWS_IoT_Client *) ptr;
	while(SUCCESS == rc && false == terminate_yield_thread) {
		do {
			vTaskDelay(THREAD_SLEEP_INTERVAL_USEC / 1000);
			//DEBUG("\n Yielding \n");
			rc = aws_iot_mqtt_yield(pClient, 100);
		} while(MQTT_CLIENT_NOT_IDLE_ERROR == rc);

		if(SUCCESS != rc) {
			IOT_ERROR("\nYield Returned : %d ", rc);
		}
	}
    IOT_DEBUG("[multithreading-validation] aws_iot_mqtt_tests_yield_thread_runner :: Exist\n");
    vTaskDelete(NULL);
}

static void aws_iot_mqtt_tests_publish_thread_runner(void *ptr) {
	int itr = 0;
	char cPayload[30];
	IoT_Publish_Message_Params params;
	IoT_Error_t rc = SUCCESS;
	ThreadData *threadData = (ThreadData *) ptr;
	AWS_IoT_Client *pClient = threadData->client;
	int threadId = threadData->threadId;

	for(itr = 0; itr < PUBLISH_COUNT; itr++) {
		snprintf(cPayload, 30, "Thread : %d, Msg : %d", threadId, itr);
		printf("\nMsg being published: %s \n", cPayload);
		params.payload = (void *) cPayload;
		params.payloadLen = strlen(cPayload) + 1;
		params.qos = QOS1;
		params.isRetained = 0;

		do {
			rc = aws_iot_mqtt_publish(pClient, INTEGRATION_TEST_TOPIC, strlen(INTEGRATION_TEST_TOPIC), &params);
			vTaskDelay(THREAD_SLEEP_INTERVAL_USEC / 1000);
		} while(MUTEX_LOCK_ERROR == rc || MQTT_CLIENT_NOT_IDLE_ERROR == rc);
		if(SUCCESS != rc) {
			IOT_WARN("\nFailed attempt 1 Publishing Thread : %d, Msg : %d, cs : %d --> %d\n ", threadId, itr, rc,
				 aws_iot_mqtt_get_client_state(pClient));
			do {
				rc = aws_iot_mqtt_publish(pClient, INTEGRATION_TEST_TOPIC, strlen(INTEGRATION_TEST_TOPIC), &params);
				vTaskDelay(THREAD_SLEEP_INTERVAL_USEC / 1000);
			} while(MUTEX_LOCK_ERROR == rc || MQTT_CLIENT_NOT_IDLE_ERROR == rc);
			rePublishCount++;
			if(SUCCESS != rc) {
				IOT_ERROR("\nFailed attempt 2 Publishing Thread : %d, Msg : %d, cs : %d --> %d Second Attempt \n", threadId,
					  itr, rc, aws_iot_mqtt_get_client_state(pClient));
			}
		}
	}
	threadStatus[threadId - 1] = 1;
    IOT_DEBUG("[multithreading-validation] aws_iot_mqtt_tests_publish_thread_runner %d :: Exist\n", threadId);
    vTaskDelete(NULL);
}

static void aws_iot_mqtt_tests_sub_unsub_thread_runner(void *ptr) {
	IoT_Error_t rc = SUCCESS;
	AWS_IoT_Client *pClient = (AWS_IoT_Client *) ptr;
	char testTopic[50];
	snprintf(testTopic, 50, "%s_temp", INTEGRATION_TEST_TOPIC);
	while(SUCCESS == rc && false == terminate_subUnsub_thread) {
		do {
			vTaskDelay(THREAD_SLEEP_INTERVAL_USEC / 1000);
			rc = aws_iot_mqtt_subscribe(pClient, testTopic, strlen(testTopic), QOS1,
										aws_iot_mqtt_tests_message_aggregator, NULL);
		} while(MQTT_CLIENT_NOT_IDLE_ERROR == rc);

		if(SUCCESS != rc) {
			IOT_ERROR("Subscribe Returned : %d ", rc);
		}

		do {
			vTaskDelay(THREAD_SLEEP_INTERVAL_USEC / 1000);
			rc = aws_iot_mqtt_unsubscribe(pClient, testTopic, strlen(testTopic));
		} while(MQTT_CLIENT_NOT_IDLE_ERROR == rc);

		if(SUCCESS != rc) {
			IOT_ERROR("Unsubscribe Returned : %d ", rc);
		}
	}
    IOT_DEBUG("[multithreading-validation] aws_iot_mqtt_tests_sub_unsub_thread_runner :: Exist\n");
    vTaskDelete(NULL);
}

int aws_iot_mqtt_tests_multi_threading_validation() {

	char clientId[50];
	IoT_Client_Init_Params initParams;
	IoT_Client_Connect_Params connectParams;
	int threadId[MAX_PUB_THREAD_COUNT];
	int pubThreadReturn[MAX_PUB_THREAD_COUNT];
	int yieldThreadReturn = 0, subUnsubThreadReturn = 0;
	float percentOfRxMsg = 0.0;
	int finishedThreadCount = 0;
	IoT_Error_t rc = SUCCESS;
	int i, rxMsgCount = 0, j = 0;

	unsigned int connectCounter = 0;
	int test_result = 0;
	ThreadData threadData[MAX_PUB_THREAD_COUNT];
	AWS_IoT_Client client;
	terminate_yield_thread = false;
	rxMsgBufferTooBigCounter = 0;
	rxUnexpectedNumberCounter = 0;
	rePublishCount = 0;
	wrongYieldCount = 0;
	for(j = 0; j < MAX_PUB_THREAD_COUNT; j++) {
		threadId[j] = j + 1;
		threadStatus[j] = 0;
		for(i = 0; i < PUBLISH_COUNT; i++) {
			countArray[j][i] = 0;
		}
	}

	printf(".... Connecting Client ....\n");
	do {
		snprintf(clientId, 50, "%s_%d", INTEGRATION_TEST_CLIENT_ID, connectCounter);

		initParams.pHostURL = AWS_IOT_MQTT_HOST;
		initParams.port = 8883;
		initParams.pRootCALocation = AWS_IOT_ROOT_CA_CERT;
		initParams.pDeviceCertLocation = AWS_IOT_DEVICE_CERT;
		initParams.pDevicePrivateKeyLocation = AWS_IOT_PRIVATE_KEY;
		initParams.mqttCommandTimeout_ms = 10000;
		initParams.tlsHandshakeTimeout_ms = 10000;
		initParams.disconnectHandler = aws_iot_mqtt_tests_disconnect_callback_handler;
		initParams.enableAutoReconnect = false;
		initParams.isBlockOnThreadLockEnabled = true;
		aws_iot_mqtt_init(&client, &initParams);

		connectParams.keepAliveIntervalInSec = 10;
		connectParams.isCleanSession = true;
		connectParams.MQTTVersion = MQTT_3_1_1;
		connectParams.pClientID = (char *)&clientId;
		connectParams.clientIDLen = strlen(clientId);
		connectParams.isWillMsgPresent = false;
		connectParams.pUsername = NULL;
		connectParams.usernameLen = 0;
		connectParams.pPassword = NULL;
		connectParams.passwordLen = 0;

		rc = aws_iot_mqtt_connect(&client, &connectParams);
		if(SUCCESS != rc) {
			IOT_ERROR("ERROR Connecting %d\n", rc);
			return -1;
		}

		connectCounter++;
	} while(SUCCESS != rc && connectCounter < CONNECT_MAX_ATTEMPT_COUNT);

	if(SUCCESS == rc) {
		IOT_ERROR("## Connect Success.\n");
	} else {
		IOT_ERROR("## Connect Failed. error code %d\n", rc);
		return -1;
	}

	aws_iot_mqtt_tests_subscribe_to_test_topic(&client, QOS1);

	printf("\nRunning Test! \n");

//	yieldThreadReturn = pthread_create(&yield_thread, NULL, aws_iot_mqtt_tests_yield_thread_runner, &client);
//	subUnsubThreadReturn = pthread_create(&sub_unsub_thread, NULL, aws_iot_mqtt_tests_sub_unsub_thread_runner, &client);

    xTaskCreate(aws_iot_mqtt_tests_yield_thread_runner, "mv_yield_runner",
                            (5 * 1024) / ((uint32_t)sizeof(StackType_t)), &client, TASK_PRIORITY_NORMAL, NULL);
    xTaskCreate(aws_iot_mqtt_tests_sub_unsub_thread_runner, "sub_unsub_runner",
                            (5 * 1024) / ((uint32_t)sizeof(StackType_t)), &client, TASK_PRIORITY_NORMAL, NULL);

	for(i = 0; i < MAX_PUB_THREAD_COUNT; i++) {
		threadData[i].client = &client;
		threadData[i].threadId = threadId[i];
//		pubThreadReturn[i] = pthread_create(&publish_thread[i], NULL, aws_iot_mqtt_tests_publish_thread_runner,
//											&threadData[i]);

        xTaskCreate(aws_iot_mqtt_tests_publish_thread_runner, "pub_runner",
                            (5 * 1024) / ((uint32_t)sizeof(StackType_t)), &threadData[i], TASK_PRIORITY_NORMAL, NULL);
	}

	/* Wait until all publish threads have finished */
	do {
		finishedThreadCount = 0;
		for(i = 0; i < MAX_PUB_THREAD_COUNT; i++) { finishedThreadCount += threadStatus[i]; }
		printf("\nFinished thread count : %d \n", finishedThreadCount);
		vTaskDelay(1000);
	} while(finishedThreadCount < MAX_PUB_THREAD_COUNT);

	printf("\nFinished publishing!!");
    
	terminate_yield_thread = true;
	terminate_subUnsub_thread = true;

	/* Allow time for yield_thread and sub_sunsub thread to exit */
	vTaskDelay(5000);

	/* Not using pthread_join because all threads should have terminated gracefully at this point. If they haven't,
	 * which should not be possible, something below will fail. */

	printf("\n\nCalculating Results!! \n\n");
	for(i = 0; i < PUBLISH_COUNT; i++) {
		for(j = 0; j < MAX_PUB_THREAD_COUNT; j++) {
			if(countArray[j][i] > 0) {
				rxMsgCount++;
			}
		}
	}

	printf("\n\nResult : \n");
	percentOfRxMsg = (float) rxMsgCount * 100 / (PUBLISH_COUNT * MAX_PUB_THREAD_COUNT);
	if(RX_RECEIVE_PERCENTAGE <= percentOfRxMsg  && 0 == rxMsgBufferTooBigCounter && 0 == rxUnexpectedNumberCounter &&
	   0 == wrongYieldCount) {
		printf("\nSuccess: %f \%\n", percentOfRxMsg);
		printf("Published Messages: %d , Received Messages: %d \n", PUBLISH_COUNT * MAX_PUB_THREAD_COUNT, rxMsgCount);
		printf("QoS 1 re publish count %d\n", rePublishCount);
		printf("Connection Attempts %d\n", connectCounter);
		printf("Yield count without error during callback %d\n", wrongYieldCount);
		test_result = 0;
	} else {
		printf("\nFailure: %f\n", percentOfRxMsg);
		printf("\"Received message was too big than anything sent\" count: %d\n", rxMsgBufferTooBigCounter);
		printf("\"The number received is out of the range\" count: %d\n", rxUnexpectedNumberCounter);
		printf("Yield count without error during callback %d\n", wrongYieldCount);
		test_result = -2;
	}
	aws_iot_mqtt_disconnect(&client);
	return test_result;
}
/*
int main() {
	printf("\n\n");
	printf("******************************************************************\n");
	printf("* Starting MQTT Version 3.1.1 Multithreading Validation Test     *\n");
	printf("******************************************************************\n");
	int rc = aws_iot_mqtt_tests_multi_threading_validation();
	if(0 != rc) {
		printf("\n*******************************************************************\n");
		printf("*MQTT Version 3.1.1 Multithreading Validation Test FAILED! RC : %d \n", rc);
		printf("*******************************************************************\n");
		return 1;
	}

	printf("******************************************************************\n");
	printf("* MQTT Version 3.1.1 Multithreading Validation Test SUCCESS!!    *\n");
	printf("******************************************************************\n");

	return 0;
}
*/

