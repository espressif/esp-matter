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

#include <vfs.h>
#include <fs/vfs_romfs.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"


#define USR_AWS_IOT_ROOT_CA_FILENAME       \
                                        "-----BEGIN CERTIFICATE-----\r\n"\
                                        "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\r\n"\
                                        "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\r\n"\
                                        "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\r\n"\
                                        "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\r\n"\
                                        "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\r\n"\
                                        "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\r\n"\
                                        "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\r\n"\
                                        "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\r\n"\
                                        "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\r\n"\
                                        "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\r\n"\
                                        "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\r\n"\
                                        "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\r\n"\
                                        "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\r\n"\
                                        "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\r\n"\
                                        "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\r\n"\
                                        "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\r\n"\
                                        "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\r\n"\
                                        "rqXRfboQnoZsG4q5WTP468SQvvG5\r\n"\
                                        "-----END CERTIFICATE-----\r\n"

#define USR_AWS_IOT_CERTIFICATE_FILENAME   \
                                        "-----BEGIN CERTIFICATE-----\r\n"\
                                        "MIIDWTCCAkGgAwIBAgIUDZRDqMsfKZr4roz/ynivk744n/wwDQYJKoZIhvcNAQEL\r\n"\
                                        "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\r\n"\
                                        "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMDIyMzAyMzY1\r\n"\
                                        "M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\r\n"\
                                        "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKdWK3pSgVtbKW2oYOmo\r\n"\
                                        "zxsNs01gN7vusTrxfP4dwdUqyV44l7dc8LkY+G4ipEk8UPD9nJZBKWEBQWRJrEP4\r\n"\
                                        "7/zPgr2nHSS84xXARXFvYz3VSlYLrVLcJUlgsu/ztqjdV1LBK/VIDJzxoIBiS1Ud\r\n"\
                                        "yZapbaCBhWdkpjcNzEcMV+31qiZZ0UbxnHQvtriaQj9zC83ovCm2OyhYZZqjwOck\r\n"\
                                        "Xpt7Ou0Knehf/YJt6iri3540OEpa6JkeZl9MS17lX3Jkzs/01UM5y+vCISXPfAlR\r\n"\
                                        "AtdxaTkujA2wbeN3sanS6u068Hcr+7iTCdkHmKPRX/JYIBbBB00sQf8cARwAjDcm\r\n"\
                                        "UZECAwEAAaNgMF4wHwYDVR0jBBgwFoAUzX9h+6r31KUGbBM/sH+p9KTx2RYwHQYD\r\n"\
                                        "VR0OBBYEFMes3BZLUAOWsXzYOoNW8iUqZzwbMAwGA1UdEwEB/wQCMAAwDgYDVR0P\r\n"\
                                        "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAqhcfpKQZvY0gg64RCDBXGbirS\r\n"\
                                        "GuG07bXpIoj31Q+DSLGsO4hDiMuuf2W4RBEciBq2CRZwLDyXRapmZctkRgXJNw9g\r\n"\
                                        "J0e23398YvcG4HaXrlf2WPfnPV0arLFSdczd7ezIWwqBdYGCktrMpbyBhFo9IUNt\r\n"\
                                        "WlClvGGnzp63Kp/xbHTVqDqLXrhew4ZbAD2apbinxMw2g4igq7yKrEpWNsfMxs+B\r\n"\
                                        "1x73U43oW140blV2BlhbHfO4zfADA+i6YM0CQUHZtyx9WNOj0+DROJYcUBq3ez1/\r\n"\
                                        "u0q+3TmX94aSoukhO3Hyp+y9U3DhNt+2oADQtb4TXkQSbHZYK0Q/8vZJdWro\r\n"\
                                        "-----END CERTIFICATE-----\r\n"

#define USR_AWS_IOT_PRIVATE_KEY_FILENAME   \
                                        "-----BEGIN RSA PRIVATE KEY-----\r\n"\
                                        "MIIEowIBAAKCAQEAp1YrelKBW1spbahg6ajPGw2zTWA3u+6xOvF8/h3B1SrJXjiX\r\n"\
                                        "t1zwuRj4biKkSTxQ8P2clkEpYQFBZEmsQ/jv/M+CvacdJLzjFcBFcW9jPdVKVgut\r\n"\
                                        "UtwlSWCy7/O2qN1XUsEr9UgMnPGggGJLVR3JlqltoIGFZ2SmNw3MRwxX7fWqJlnR\r\n"\
                                        "RvGcdC+2uJpCP3MLzei8KbY7KFhlmqPA5yRem3s67Qqd6F/9gm3qKuLfnjQ4Slro\r\n"\
                                        "mR5mX0xLXuVfcmTOz/TVQznL68IhJc98CVEC13FpOS6MDbBt43exqdLq7Trwdyv7\r\n"\
                                        "uJMJ2QeYo9Ff8lggFsEHTSxB/xwBHACMNyZRkQIDAQABAoIBAGJ74YzVgBJvXbuP\r\n"\
                                        "LLJ4SqPmdU+lwEeYHLDPLbw1cT3vN6J/djNRvM+DdbsEImKD4DNlX8JCyYKxBXZr\r\n"\
                                        "sbozibLlwlGySAG2NYQ4q+YuI19h56UqrDCnJj7Aba+xZSCPXHR1AlUSuXGo98h3\r\n"\
                                        "kVSf+mP0hXPApu4KG5UtRrvN9PYz+p8SdIpfxahqAsDIOcEzUbpAMQO+C01bZ+gn\r\n"\
                                        "nb7tL1KOVDeVr5UDHwB6eoexkJSmVq4JM5zWExUef51mcNd9aIwF55i4bZnc/D22\r\n"\
                                        "BufMKpw0fpDizfOXc8gR5f3rt9IoE2s6dufWoTVvAIg5AGBO2h2jF0kCydotktvA\r\n"\
                                        "7Oh5yE0CgYEA2jns/yvZ3VNpe8stOI1CSMxqjxSW5O6wYgLP4Be1Bae/srqsY86A\r\n"\
                                        "azm8pYKpi3rXZDxtgLumUaJ02nHudub2gDCtlARLqT+TQveqlhM5DZKCdZkRisLS\r\n"\
                                        "U/iE4nxSqNdSu25MLihP7eQqgPirNu3V35xmGLbBbqbWIUWkN5GSdDcCgYEAxE02\r\n"\
                                        "q83f4b73JrZRcZ+t2F+gp3U/TrdKx+HEqqQnob/n2Xmyn5PuiAhDSNnDQK08YJe9\r\n"\
                                        "uiAN/MGpsy2OdWQoTiXovQD+FsOAgqL4AeXnuunP0zvbdvPYgqQqcqGNhVO0tLy/\r\n"\
                                        "UfBN4YQtaIpOGuKZKQ1mJjMBkz5rJ59GKjnyFHcCgYBHcqmujdvhw9qIZmgi2NT3\r\n"\
                                        "4r2MpPAWejOTk1oeA4UIGfkAem/QqZ6P/LJP+iyqdwioT8SGXmnA1pCtqJmrokjY\r\n"\
                                        "hZd/G7zt59u+FDpsslR+phZCjGAa71hPl9FiGdQyZXU32o/kmNw6hfaIjWH5NPtO\r\n"\
                                        "H/WpZ8SDBuqasCNo8idiFwKBgDatAwNLboZkOFbSBVFOlo9j402BPMbiCrQt0Cdv\r\n"\
                                        "XZKR7zQFz5jfDVtaYdNSxC2pkt2cpj3HW94A2xGLKNjNsuRBbtfmBN+2SH8agVWW\r\n"\
                                        "AbtwEHDXhUNWnAGXVDNliBOQww5sXCWodyjPNg6eJy8Z2msyzjbpkhC/G/2HQXXV\r\n"\
                                        "efX1AoGBAMcd2gvzVyJhVxKwNi3wB+7Wh5jYV2y5SP9JDg54hJZ4ow6rdBxW00v0\r\n"\
                                        "QQEH3W4Xz3Wvr4ccLGFI8wM1xWBqht0mKd1IJca7sfynL/hXnF5dHum6mAbxeIfx\r\n"\
                                        "vkdM9PAt8TwVWjQvMAobVcHm6vFSRAb7ZMgtL1lxVbYMNSDNH43A\r\n"\
                                        "-----END RSA PRIVATE KEY-----\r\n"


#define USR_AWS_IOT_CONFIG_FILE        "/romfs/aws/config" ///< Device private key filename
#define AWS_IOT_MQTT_HOST_TEST         "tpoz9vm9qivie.deviceadvisor.iot.us-east-1.amazonaws.com"
#define AWS_IOT_MQTT_PORT_TEST         443 ///< default port for MQTT/S
#define AWS_IOT_MQTT_CLIENT_ID_TEST    "myiotthing" 
#define AWS_IOT_MY_THING_NAME_TEST     "myiotthing" 
#define TEST_STEP_RETRY_COUNT          2


#define MYPUBTOPIC   "/alex/myiotthing/pub"
#define MYSUBTOPIC   "/alex/myiotthing/sub"

#define TESTJSON     "{\"state\":{\"reported\":{\"temperature\":25.500000,\"windowOpen\":false}}, \"clientToken\":\"bl60xDevRjwang-1\"}"

typedef enum {
	TEST_STEP_TLS_CONNECT,
	TEST_STEP_TLS_UNSECURE_SERVER_CERT,
	TEST_STEP_TLS_INCORRECT_SUBJECT_NAME_SERVER_CERT,
	TEST_STEP_MQTT_CONNECT,
	TEST_STEP_MQTT_SUBSCRIBE,
	TEST_STEP_MQTT_PUBLISH,
	TEST_STEP_WAIT_MQTT_DISCONNECT,
	TEST_STEP_SEND_MQTT_PUBLISH,
	TEST_STEP_SEND_MQTT_SUBSCRIBE,
	TEST_STEP_IDLE,
}TestStep;

typedef enum {
	TEST_MQTT_CONNECT,
	TEST_MQTT_SUBSCRIBE,
	TEST_MQTT_PUBLISH,
	TEST_MQTT_IDLE,
}MqttStep;



void windowActuate_Callback(const char *pJsonString, uint32_t JsonStringDataLen, jsonStruct_t *pContext) {
    IOT_UNUSED(pJsonString);
    IOT_UNUSED(JsonStringDataLen);

    if(pContext != NULL) {
        printf("Delta - Window state changed to %d\r\n", *(bool *) (pContext->pData));
    }
}

static int _update_mqtt_config_default(ShadowInitParameters_t *sp, ShadowConnectParameters_t *scp)
{
    sp->pHost = AWS_IOT_MQTT_HOST_TEST;
    printf(" URL:%s\r\n", sp->pHost);

    sp->port = AWS_IOT_MQTT_PORT_TEST;
    printf(", port is %d\r\n", sp->port);

    sp->pClientCRT = USR_AWS_IOT_CERTIFICATE_FILENAME;
    sp->pClientKey = USR_AWS_IOT_PRIVATE_KEY_FILENAME;
    sp->pRootCA = USR_AWS_IOT_ROOT_CA_FILENAME;
    sp->enableAutoReconnect = false;
    sp->disconnectHandler = NULL;

    scp->pMqttClientId = AWS_IOT_MQTT_CLIENT_ID_TEST;
    printf(", THING ID is %s\r\n", scp->pMqttClientId);
    scp->mqttClientIdLen = strlen(scp->pMqttClientId);

    printf("thing name len is %d", strlen(AWS_IOT_MY_THING_NAME_TEST));
    scp->pMyThingName = AWS_IOT_MY_THING_NAME_TEST;
    printf(", THING name is %s\r\n", scp->pMyThingName);

    return 0;
}


static void shadow_delta_callback(AWS_IoT_Client *pClient, char *topicName,
								  uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData) 
{
	printf("shadow_delta_callback\r\n");
}


void aws_device_advisor_entry(void *arg) 
{
    IoT_Error_t rc = FAILURE;
    ShadowInitParameters_t sp = ShadowInitParametersDefault;
    ShadowConnectParameters_t scp = ShadowConnectParametersDefault;
    
    printf("AWS IoT SDK Version %d.%d.%d-%s\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);
    // initialize the mqtt client
    AWS_IoT_Client mqttClient;

    _update_mqtt_config_default(&sp, &scp);
	
    printf("Shadow Init\r\n");
    rc = aws_iot_shadow_init(&mqttClient, &sp);
    if(SUCCESS != rc) {
        printf("aws_iot_shadow_init returned error %d, aborting...\r\n", rc);
        while (1) {
            vTaskDelay(1000);
        }
    }
	TestStep teststep = TEST_STEP_MQTT_CONNECT;
    MqttStep mqttstep = TEST_MQTT_CONNECT;
	AWS_IoT_Client*pClient = &mqttClient;
	int count = 0;
	while(1)
	{
		switch(teststep)
		{
			case TEST_STEP_TLS_CONNECT:
			case TEST_STEP_TLS_UNSECURE_SERVER_CERT:
			case TEST_STEP_TLS_INCORRECT_SUBJECT_NAME_SERVER_CERT:
			case TEST_STEP_MQTT_CONNECT:
			case TEST_STEP_MQTT_SUBSCRIBE:
			case TEST_STEP_MQTT_PUBLISH:
			{
				printf("teststep = %d\r\n", teststep);
				if ((teststep == TEST_STEP_TLS_CONNECT)||(teststep == TEST_STEP_MQTT_CONNECT)
					||(teststep == TEST_STEP_MQTT_SUBSCRIBE)||(teststep == TEST_STEP_MQTT_PUBLISH))
				{
					rc = aws_iot_shadow_connect(&mqttClient, &scp);
				}
				else
				{
					rc = pClient->networkStack.connect(&(pClient->networkStack), NULL);
				}
			    if(SUCCESS != rc) 
				{
			        printf("connect returned error = %d\r\n", rc);
					if ((teststep == TEST_STEP_MQTT_CONNECT)||(teststep == TEST_STEP_MQTT_SUBSCRIBE)
						||(teststep == TEST_STEP_MQTT_PUBLISH))
					{
						aws_iot_shadow_disconnect(&mqttClient);
						count++;
						if (count >= TEST_STEP_RETRY_COUNT)
						{
							count = 0;
							teststep = TEST_STEP_IDLE;
							mqttstep = TEST_MQTT_IDLE;
						}
						break;
					}
					if ((teststep == TEST_STEP_TLS_CONNECT)&&(rc == NETWORK_SSL_READ_ERROR))
					{
						aws_iot_shadow_disconnect(&mqttClient); 
						vTaskDelay(2000);
						teststep = TEST_STEP_TLS_UNSECURE_SERVER_CERT;
						break;
					}
					pClient->networkStack.disconnect(&(pClient->networkStack));
					rc = pClient->networkStack.destroy(&(pClient->networkStack));
				    if(SUCCESS != rc) {
				        printf("Disconnect error %d\r\n", rc);
				    }
					printf("disconnect sucess\r\n");
					vTaskDelay(3000);
					teststep++;
					if (teststep >= TEST_STEP_MQTT_CONNECT)
					{
						teststep = TEST_STEP_IDLE;
					}
			    } 
				else
				{
					printf("Connect sucess\r\n"); 
					count = 0;
					if (mqttstep == TEST_MQTT_CONNECT)
					{
						teststep = TEST_STEP_WAIT_MQTT_DISCONNECT;
					}
					else if (mqttstep == TEST_MQTT_SUBSCRIBE)
					{
						teststep = TEST_STEP_SEND_MQTT_SUBSCRIBE;
					}
					else if (mqttstep == TEST_MQTT_PUBLISH)
					{
						teststep = TEST_STEP_SEND_MQTT_PUBLISH;
					}
					
					if (teststep == TEST_STEP_TLS_CONNECT)
					{
						aws_iot_shadow_disconnect(&mqttClient); 
						teststep = TEST_STEP_TLS_UNSECURE_SERVER_CERT;
					}
				}
			}
			break;
			case TEST_STEP_WAIT_MQTT_DISCONNECT:
			{
				if (mqttstep == TEST_MQTT_CONNECT)
				{
					vTaskDelay(1000);
					aws_iot_shadow_disconnect(&mqttClient); 
				    teststep = TEST_STEP_MQTT_SUBSCRIBE;
				    mqttstep = TEST_MQTT_SUBSCRIBE;
				}
				else if (mqttstep == TEST_MQTT_SUBSCRIBE)
				{
					teststep = TEST_STEP_MQTT_PUBLISH;
					mqttstep = TEST_MQTT_PUBLISH;
					if (aws_iot_mqtt_is_client_connected(&mqttClient))
					{
						aws_iot_shadow_disconnect(&mqttClient); 
						vTaskDelay(1000);
					}
				}
				else if (mqttstep == TEST_MQTT_PUBLISH)
				{
					teststep = TEST_STEP_TLS_CONNECT;
					mqttstep = TEST_MQTT_IDLE;
					if (aws_iot_mqtt_is_client_connected(&mqttClient))
					{
						aws_iot_shadow_disconnect(&mqttClient); 
						vTaskDelay(1000); 
					}
				}
			}
			break;
			case TEST_STEP_SEND_MQTT_PUBLISH:
			{
				printf("TEST_STEP_SEND_MQTT_PUBLISH\r\n");
				IoT_Publish_Message_Params msgParams;
				msgParams.qos = QOS0;
				msgParams.isRetained = 0;
				msgParams.payloadLen = strlen(TESTJSON);
				msgParams.payload = (char *)TESTJSON;
				rc = aws_iot_mqtt_publish(&mqttClient, MYPUBTOPIC, (uint16_t) strlen(MYPUBTOPIC), &msgParams);
                if (rc != SUCCESS)  
                {
                    printf("MQTT_PUBLISH fail\r\n");
					count++;
					if (count >= TEST_STEP_RETRY_COUNT)
					{
						count = 0;
						teststep = TEST_STEP_WAIT_MQTT_DISCONNECT;
						break;
					}
					teststep = TEST_STEP_SEND_MQTT_PUBLISH;
					break;
                }
				else
				{
					printf("MQTT_PUBLISH sucess\r\n");
				}
				teststep = TEST_STEP_WAIT_MQTT_DISCONNECT;
			}
			break;

			case TEST_STEP_SEND_MQTT_SUBSCRIBE:
			{
				printf("TEST_STEP_SEND_MQTT_SUBSCRIBE\r\n");
				rc = aws_iot_mqtt_subscribe(&mqttClient, MYSUBTOPIC, (uint16_t) strlen(MYSUBTOPIC), QOS0,
									             shadow_delta_callback, NULL);
				if (rc != SUCCESS)  
                {
                    printf("MQTT_SUBSCRIBE fail\r\n");
					count++;
					if (count >= TEST_STEP_RETRY_COUNT)
					{
						count = 0;
						teststep = TEST_STEP_WAIT_MQTT_DISCONNECT;
						break;
					}
					teststep = TEST_STEP_SEND_MQTT_SUBSCRIBE;
					break;
                }
				else
				{
					printf("MQTT_SUBSCRIBE sucess\r\n");
				}
				teststep = TEST_STEP_WAIT_MQTT_DISCONNECT;
			}
			break;

			case TEST_STEP_IDLE:
			{
				printf("test idle\r\n");
				vTaskDelay(1000); 
			}
			break;
			
			default:
				teststep = TEST_STEP_IDLE;
				break;
		}
		
		vTaskDelay(1000);
	}

	vTaskDelete(NULL);
}
