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

#if 0
#define USR_AWS_IOT_ROOT_CA_FILENAME       "/romfs/aws/root-CA.crt" ///< Root CA file name
#define USR_AWS_IOT_CERTIFICATE_FILENAME   "/romfs/aws/certificate.pem.crt" ///< device signed certificate file name
#define USR_AWS_IOT_PRIVATE_KEY_FILENAME   "/romfs/aws/private.pem.key" ///< Device private key filename
#else
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
                                        "MIIDWjCCAkKgAwIBAgIVAOosWE2Km3IxrIwFACK/p1K1hvclMA0GCSqGSIb3DQEB\r\n"\
                                        "CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\r\n"\
                                        "IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTA5MDQwNjUw\r\n"\
                                        "NTFaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\r\n"\
                                        "dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDOLZXvbTzYCvzEhyxP\r\n"\
                                        "GtF8chZ+r3FZszUdqicDnUqIcKAZiiUC2P/errXnaK/eEye2QUSZmUDpn1xybde4\r\n"\
                                        "eG1Kz8BvSnp6UYuoF1hPnLQvnm02ILwANcdjScql6CpYIwuGNvCxP1re+iOnOx7V\r\n"\
                                        "kFP02d0w36voNMZHyK755IjjWz5PSEIzXx6RCP/wqkJlz+oZvIrDZZ1dXa5X3ffV\r\n"\
                                        "bftYCPj2gDW8AohWYMapDJpRfEdo6g6NkP4VOPS+nZ9Fp9rY6BZqJR6m4otjdMna\r\n"\
                                        "I7p3FY5wqOdcF3DPXYMHR0j5SjtDLIeNpX8AP6ekpZsO9k1/VLSK+wxXIHltsYW/\r\n"\
                                        "lQtzAgMBAAGjYDBeMB8GA1UdIwQYMBaAFLcyZBzcJOBSL0d+yMxZh3trcG7rMB0G\r\n"\
                                        "A1UdDgQWBBSgGPuINfSBCTZxtcV3epnSuxPdlTAMBgNVHRMBAf8EAjAAMA4GA1Ud\r\n"\
                                        "DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAki9Ito1X7mxn8wCFqmhhXMnh\r\n"\
                                        "U6GPvTJTBuTTO+exe8zPiBwHqE0GwJi7Gpu7mlXdlb0vCeNDPBIaDmdcXF/UvTQb\r\n"\
                                        "ebD3ifawME605r2DFb4IYXVypZzZ2/gKY1K7uKqL9dvyyPdWQJtQY2Kf5AZmKfQ0\r\n"\
                                        "PhM6ed1GoMyoW6FRBhjds/znMAduxzvCGaJntKHLppDxl6YrzewgDHsc4iGRnjd2\r\n"\
                                        "C8cX+aw7EwNrBwFCYKz+5kA4UFD6coAuMQlAqTSYvD6MJvFSKp8JvlvUVCqPTSHh\r\n"\
                                        "Pb9Nq++uF9n+F+X8lzA6ul6IoPt5DkzNhOJRdvkggKYuVCUGkMdkZJVKeUHq1g==\r\n"\
                                        "-----END CERTIFICATE-----\r\n"

#define USR_AWS_IOT_PRIVATE_KEY_FILENAME   \
                                        "-----BEGIN RSA PRIVATE KEY-----\r\n"\
                                        "MIIEpgIBAAKCAQEAzi2V72082Ar8xIcsTxrRfHIWfq9xWbM1HaonA51KiHCgGYol\r\n"\
                                        "Atj/3q6152iv3hMntkFEmZlA6Z9ccm3XuHhtSs/Ab0p6elGLqBdYT5y0L55tNiC8\r\n"\
                                        "ADXHY0nKpegqWCMLhjbwsT9a3vojpzse1ZBT9NndMN+r6DTGR8iu+eSI41s+T0hC\r\n"\
                                        "M18ekQj/8KpCZc/qGbyKw2WdXV2uV9331W37WAj49oA1vAKIVmDGqQyaUXxHaOoO\r\n"\
                                        "jZD+FTj0vp2fRafa2OgWaiUepuKLY3TJ2iO6dxWOcKjnXBdwz12DB0dI+Uo7QyyH\r\n"\
                                        "jaV/AD+npKWbDvZNf1S0ivsMVyB5bbGFv5ULcwIDAQABAoIBAQCkRtoibU1/bjG8\r\n"\
                                        "fV+mn92uZFDcG/oa2JlHy51OWfNbBzHpZ5LZ1AD7cwVpHpaSQXzw0MxR/jQkcV99\r\n"\
                                        "pVuulfWvGB1iD1vLPlIiWod414kSMA8gg7CIElGOIOugiX3DwBiclgXtlGdZJsFl\r\n"\
                                        "q8uDqn3aqAR/vpgAapxZ8eLJJXiUfV7IaAu7dXr0Es1WEmyXV0xbtppEm9BIgjw9\r\n"\
                                        "33YmSvTkBRMMfSBN5u0xp53axiwZ77I8ZNRROlUy/ki5myWazTcjAYdDnzO7M8of\r\n"\
                                        "/iMaOY7UOcROKEzWCzTOucncrGg+Uv+YD0kMe37Aj3g3wK4BZcMA0LdiY9N31S0F\r\n"\
                                        "l5IVbzfxAoGBAOv+4rURrsjyvbyqaWS3Y30k5bGsaA/m7Bwmyey/0NXOza02KqRa\r\n"\
                                        "/ZfIB8jCChVkNmMFfc4Aqp4VsV8YZCxnQzjteiIRWurfZD9Xgf8fF4OYmZpUqZ6S\r\n"\
                                        "U8GX8rd88RTkb5N6VlDRKK8aqhKRJ/OE4VF+c6p0PHITzaN42LEGldCFAoGBAN+n\r\n"\
                                        "qFUyO9k0SvYz1LQFVgXF3kzIjzjI4ca/CYg8vYllS9n0wE0/m4KrA3wA0Qxpl5tI\r\n"\
                                        "D8H1Br1G4iy+4n3Zwtb2wEa+KXYjSmHzxJNKiBOK5Ks6k4CbScswy7wV2fgPqUIK\r\n"\
                                        "yoBLfdNlnhbcDHdWXwy84ea59pcRdjVMBVFrFumXAoGBAOHaDR08zwmBRhRJApRg\r\n"\
                                        "7dXWefyQAwc0+MH4XUc2S6ZTRIOAU073xbp8tPZsvTm7rlEV8FA2UxL3r8hSj4vv\r\n"\
                                        "R2Yz36667UfagxYbqU1/dAF9CYOpgqs/XujngmIYMKDN/fomGnU9trHd0uXyBUoI\r\n"\
                                        "4ZocapW2qCKE0UihXsAMjVQBAoGBALH8n/blGkHZ3soeJ2j4UiQX+rhKZ8g9/YeI\r\n"\
                                        "IpLcG85K4dlbcOpI835CkGxPzg29vpwh9gBZ+foJfhcz9dVHxkoCZrQlw6MzpI9d\r\n"\
                                        "SpQZHT/IKgCmhhoz+r8MXyaYkTVbaUyYf90ntsPUO8JfbkBOaSge7PwMBFTm3Za+\r\n"\
                                        "oADSY5KTAoGBAK3f+otZPfZA1LTpWYbOpCl10Ci/R/xU13Ak7omahkpXX/dMHt3s\r\n"\
                                        "d0fpsuduQwrQOGJLEZG/Z1zeNSp8QtGKH/RH57tWJBFR1XWEJ9kyH/FPVaighzPU\r\n"\
                                        "T9VIJw2g//PLTT7yV/D9HmiNMj0hQYkTP3bGKvaiOrhC7a0Us4KhMGFc\r\n"\
                                        "-----END RSA PRIVATE KEY-----\r\n"
#endif
#define USR_AWS_IOT_CONFIG_FILE        "/romfs/aws/config" ///< Device private key filename
#define AWS_IOT_MQTT_HOST              "ap3im43fimllm-ats.iot.us-east-1.amazonaws.com"
#define AWS_IOT_MQTT_PORT              443 ///< default port for MQTT/S
#define AWS_IOT_MQTT_CLIENT_ID         "bl60xDevRjwang"
#define AWS_IOT_MY_THING_NAME          "bl60xDevRjwang"

#define ROOMTEMPERATURE_UPPERLIMIT 32.0f
#define ROOMTEMPERATURE_LOWERLIMIT 25.0f
#define STARTING_ROOMTEMPERATURE ROOMTEMPERATURE_LOWERLIMIT

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200
static void simulateRoomTemperature(float *pRoomTemperature) {
    static float deltaChange;

    if(*pRoomTemperature >= ROOMTEMPERATURE_UPPERLIMIT) {
        deltaChange = -0.5f;
    } else if(*pRoomTemperature <= ROOMTEMPERATURE_LOWERLIMIT) {
        deltaChange = 0.5f;
    }

    *pRoomTemperature += deltaChange;
}

static bool shadowUpdateInProgress;

void ShadowUpdateStatusCallback(const char *pThingName, ShadowActions_t action, Shadow_Ack_Status_t status,
                                const char *pReceivedJsonDocument, void *pContextData) {
    IOT_UNUSED(pThingName);
    IOT_UNUSED(action);
    IOT_UNUSED(pReceivedJsonDocument);
    IOT_UNUSED(pContextData);

    shadowUpdateInProgress = false;

    if(SHADOW_ACK_TIMEOUT == status) {
        printf("Update timed out\r\n");
    } else if(SHADOW_ACK_REJECTED == status) {
        printf("Update rejected\r\n");
    } else if(SHADOW_ACK_ACCEPTED == status) {
        printf("Update accepted\r\n");
    }
}

void windowActuate_Callback(const char *pJsonString, uint32_t JsonStringDataLen, jsonStruct_t *pContext) {
    IOT_UNUSED(pJsonString);
    IOT_UNUSED(JsonStringDataLen);

    if(pContext != NULL) {
        printf("Delta - Window state changed to %d\r\n", *(bool *) (pContext->pData));
    }
}

static int _update_mqtt_config(ShadowInitParameters_t *sp, ShadowConnectParameters_t *scp)
{
    int fd, len;
    char *c_ptr;
    romfs_filebuf_t filebuf;
    char port[8];

    fd = aos_open(USR_AWS_IOT_CONFIG_FILE, 0);
    if (fd < 0) {
        printf("Open config file %s failed\r\n", USR_AWS_IOT_CONFIG_FILE);
        return -1;
    }
    aos_ioctl(fd, IOCTL_ROMFS_GET_FILEBUF, (long unsigned int)&filebuf);
    c_ptr = filebuf.buf;

    /*get host URL*/
    len = 0;
    while (*c_ptr != '\n' && *c_ptr != '\r') {
        c_ptr++;
        len++;
    }
    printf("URL len is %d,", len);
    sp->pHost = pvPortMalloc(len);
    if (NULL == sp->pHost) {
        //TODO use ASSERT here
        printf("NULL pHost %d\r\n", __LINE__);
        while (1) {
            vTaskDelay(1000);
        }
    }
    memcpy(sp->pHost, c_ptr - len, len);
    sp->pHost[len] = '\0';
    printf(" URL:%s\r\n", sp->pHost);

    /*get host port*/
    while (*c_ptr == '\n' || *c_ptr == '\r') {
        c_ptr++;
    }
    len = 0;
    while (*c_ptr != '\n' && *c_ptr != '\r') {
        c_ptr++;
        len++;
    }
    printf("port len is %d", len);
    if (len > 5) {
        printf("Too long port\r\n");
        while (1) {
            vTaskDelay(1000);
        }
    }
    memcpy(port, c_ptr - len, len);
    port[len] = '\0';
    sp->port = atoi(port);
    printf(", port is %d\r\n", sp->port);

    sp->pClientCRT = USR_AWS_IOT_CERTIFICATE_FILENAME;
    sp->pClientKey = USR_AWS_IOT_PRIVATE_KEY_FILENAME;
    sp->pRootCA = USR_AWS_IOT_ROOT_CA_FILENAME;
    sp->enableAutoReconnect = false;
    sp->disconnectHandler = NULL;

    /*get thing id*/
    while (*c_ptr == '\n' || *c_ptr == '\r') {
        c_ptr++;
    }
    len = 0;
    while (*c_ptr != '\n' && *c_ptr != '\r') {
        c_ptr++;
        len++;
    }
    printf("thing id len is %d", len);
    scp->pMqttClientId = pvPortMalloc(len);
    if (NULL == scp->pMqttClientId) {
        //TODO use ASSERT here
        printf("NULL pMqttClientId %d\r\n", __LINE__);
        while (1) {
            vTaskDelay(1000);
        }
    }
    memcpy((void*)scp->pMqttClientId, c_ptr - len, len);
    ((char*)scp->pMqttClientId)[len] = '\0';
    printf(", THING ID is %s\r\n", scp->pMqttClientId);
    scp->mqttClientIdLen = strlen(scp->pMqttClientId);

    /*get thing name*/
    while (*c_ptr == '\n' || *c_ptr == '\r') {
        c_ptr++;
    }
    len = 0;
    while (*c_ptr != '\n' && *c_ptr != '\r') {
        c_ptr++;
        len++;
    }
    printf("thing name len is %d", len);
    scp->pMyThingName = pvPortMalloc(len);
    if (NULL == scp->pMyThingName) {
        //TODO use ASSERT here
        printf("NULL pMyThingName %d\r\n", __LINE__);
        while (1) {
            vTaskDelay(1000);
        }
    }
    memcpy((void*)scp->pMyThingName, c_ptr - len, len);
    ((char*)scp->pMyThingName)[len] = '\0';
    printf(", THING name is %s\r\n", scp->pMyThingName);

    aos_close(fd);
    return 0;
}

static int _update_mqtt_config_default(ShadowInitParameters_t *sp, ShadowConnectParameters_t *scp)
{
    sp->pHost = AWS_IOT_MQTT_HOST;
    printf(" URL:%s\r\n", sp->pHost);

    sp->port = AWS_IOT_MQTT_PORT;
    printf(", port is %d\r\n", sp->port);

    sp->pClientCRT = USR_AWS_IOT_CERTIFICATE_FILENAME;
    sp->pClientKey = USR_AWS_IOT_PRIVATE_KEY_FILENAME;
    sp->pRootCA = USR_AWS_IOT_ROOT_CA_FILENAME;
    sp->enableAutoReconnect = false;
    sp->disconnectHandler = NULL;

    scp->pMqttClientId = AWS_IOT_MQTT_CLIENT_ID;
    printf(", THING ID is %s\r\n", scp->pMqttClientId);
    scp->mqttClientIdLen = strlen(scp->pMqttClientId);

    printf("thing name len is %d", strlen(AWS_IOT_MY_THING_NAME));
    scp->pMyThingName = AWS_IOT_MY_THING_NAME;
    printf(", THING name is %s\r\n", scp->pMyThingName);

    return 0;
}

void aws_main_entry(void *param)
{
    IoT_Error_t rc = FAILURE;
    ShadowInitParameters_t sp = ShadowInitParametersDefault;
    ShadowConnectParameters_t scp = ShadowConnectParametersDefault;

    char JsonDocumentBuffer[MAX_LENGTH_OF_UPDATE_JSON_BUFFER];
    size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);
    float temperature = 0.0;

    bool windowOpen = false;
    jsonStruct_t windowActuator;
    windowActuator.cb = windowActuate_Callback;
    windowActuator.pData = &windowOpen;
    windowActuator.pKey = "windowOpen";
    windowActuator.type = SHADOW_JSON_BOOL;
    windowActuator.dataLength = sizeof(bool);

    jsonStruct_t temperatureHandler;
    temperatureHandler.cb = NULL;
    temperatureHandler.pKey = "temperature";
    temperatureHandler.pData = &temperature;
    temperatureHandler.type = SHADOW_JSON_FLOAT;
    temperatureHandler.dataLength = sizeof(float);

    printf("AWS IoT SDK Version %d.%d.%d-%s\r\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

    // initialize the mqtt client
    AWS_IoT_Client mqttClient;

    if (_update_mqtt_config(&sp, &scp)) {
        _update_mqtt_config_default(&sp, &scp);
    }
    printf("Shadow Init\r\n");
    rc = aws_iot_shadow_init(&mqttClient, &sp);
    if(SUCCESS != rc) {
        printf("aws_iot_shadow_init returned error %d, aborting...\r\n", rc);
        while (1) {
            vTaskDelay(1000);
        }
    }

    printf("Shadow Connect\r\n");
    rc = aws_iot_shadow_connect(&mqttClient, &scp);
    if(SUCCESS != rc) {
        printf("aws_iot_shadow_connect returned error %d, aborting...\r\n", rc);
        while (1) {
            vTaskDelay(1000);
        }
    }

    /*
     * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
     *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
     *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
     */
    rc = aws_iot_shadow_set_autoreconnect_status(&mqttClient, true);
    if(SUCCESS != rc) {
        printf("Unable to set Auto Reconnect to true - %d, aborting...\r\n", rc);
        while (1) {
            vTaskDelay(1000);
        }
    }

    rc = aws_iot_shadow_register_delta(&mqttClient, &windowActuator);

    if(SUCCESS != rc) {
        printf("Shadow Register Delta Error\r\n");
        while (1) {
            vTaskDelay(1000);
        }
    }
    temperature = STARTING_ROOMTEMPERATURE;

    // loop and publish a change in temperature
    while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
        rc = aws_iot_shadow_yield(&mqttClient, 200);
        if(NETWORK_ATTEMPTING_RECONNECT == rc || shadowUpdateInProgress) {
            rc = aws_iot_shadow_yield(&mqttClient, 1000);
            // If the client is attempting to reconnect, or already waiting on a shadow update,
            // we will skip the rest of the loop.
            continue;
        }
        printf("=======================================================================================\r\n");
        printf("On Device: window state %s\r\n", windowOpen ? "true" : "false");
        simulateRoomTemperature(&temperature);

        rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
        if(SUCCESS == rc) {
            rc = aws_iot_shadow_add_reported(JsonDocumentBuffer, sizeOfJsonDocumentBuffer, 2, &temperatureHandler,
                                             &windowActuator);
            if(SUCCESS == rc) {
                rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
                if(SUCCESS == rc) {
                    printf("Update Shadow: %s\r\n", JsonDocumentBuffer);
                    rc = aws_iot_shadow_update(&mqttClient, scp.pMyThingName, JsonDocumentBuffer,
                                               ShadowUpdateStatusCallback, NULL, 4, true);
                    shadowUpdateInProgress = true;
                }
            }
        }
        printf("*****************************************************************************************\r\n");
        printf("Stack remaining for task '%s' is %ld bytes\r\n", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));

        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    if(SUCCESS != rc) {
        printf("An error occurred in the loop %d\r\n", rc);
    }

    printf("Disconnecting\r\n");
    rc = aws_iot_shadow_disconnect(&mqttClient);

    if(SUCCESS != rc) {
        printf("Disconnect error %d\r\n", rc);
    }

    vTaskDelete(NULL);
}
