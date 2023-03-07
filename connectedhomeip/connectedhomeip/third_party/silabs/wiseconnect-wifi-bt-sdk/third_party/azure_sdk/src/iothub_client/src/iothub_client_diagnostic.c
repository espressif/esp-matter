// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/agenttime.h"
#include "azure_c_shared_utility/buffer_.h"

#include "internal/iothub_client_diagnostic.h"

#define TIME_STRING_BUFFER_LEN 30

static const int BASE_36 = 36;

#define INDEFINITE_TIME ((time_t)-1)

static char* get_epoch_time(char* timeBuffer)
{
    char* result;
    time_t epochTime;
    int timeLen = sizeof(time_t);

    if ((epochTime = get_time(NULL)) == INDEFINITE_TIME)
    {
        LogError("Failed getting current time");
        result = NULL;
    }
    else if (timeLen == sizeof(int64_t))
    {
        if (sprintf(timeBuffer, "%"PRIu64, (int64_t)epochTime) < 0)
        {
            LogError("Failed sprintf to timeBuffer with 8 bytes of time_t");
            result = NULL;
        }
        else
        {
            result = timeBuffer;
        }
    }
    else if (timeLen == sizeof(int32_t))
    {
        if (sprintf(timeBuffer, "%"PRIu32, (int32_t)epochTime) < 0)
        {
            LogError("Failed sprintf to timeBuffer with 4 bytes of time_t");
            result = NULL;
        }
        else
        {
            result = timeBuffer;
        }
    }
    else
    {
        LogError("Unknown size of time_t");
        result = NULL;
    }

    return result;
}

static char get_base36_char(unsigned char value)
{
    return value <= 9 ? '0' + value : 'a' + value - 10;
}

static char* generate_eight_random_characters(char *randomString)
{
    int i;
    char* randomStringPos = randomString;
    for (i = 0; i < 4; ++i)
    {
        int rawRandom = rand();
        int first = rawRandom % BASE_36;
        int second = rawRandom / BASE_36 % BASE_36;
        *randomStringPos++ = get_base36_char((unsigned char)first);
        *randomStringPos++ = get_base36_char((unsigned char)second);
    }
    *randomStringPos = 0;

    return randomString;
}

static bool should_add_diagnostic_info(IOTHUB_DIAGNOSTIC_SETTING_DATA* diagSetting)
{
    bool result = false;
    if (diagSetting->diagSamplingPercentage > 0)
    {
        double number;
        double percentage;

        if (diagSetting->currentMessageNumber == UINT32_MAX)
        {
            diagSetting->currentMessageNumber %= diagSetting->diagSamplingPercentage * 100;
        }
        ++diagSetting->currentMessageNumber;

        number = diagSetting->currentMessageNumber;
        percentage = diagSetting->diagSamplingPercentage;
        result = (floor((number - 2) * percentage / 100.0) < floor((number - 1) * percentage / 100.0));
    }
    return result;
}

static IOTHUB_MESSAGE_DIAGNOSTIC_PROPERTY_DATA* prepare_message_diagnostic_data()
{
    IOTHUB_MESSAGE_DIAGNOSTIC_PROPERTY_DATA* result = (IOTHUB_MESSAGE_DIAGNOSTIC_PROPERTY_DATA*)malloc(sizeof(IOTHUB_MESSAGE_DIAGNOSTIC_PROPERTY_DATA));
    if (result == NULL)
    {
        LogError("malloc for DiagnosticData failed");
    }
    else
    {
        char* diagId = (char*)malloc(9);
        if (diagId == NULL)
        {
            LogError("malloc for diagId failed");
            free(result);
            result = NULL;
        }
        else
        {
            char* timeBuffer;

            (void)generate_eight_random_characters(diagId);
            result->diagnosticId = diagId;

            timeBuffer = (char*)malloc(TIME_STRING_BUFFER_LEN);
            if (timeBuffer == NULL)
            {
                LogError("malloc for timeBuffer failed");
                free(result->diagnosticId);
                free(result);
                result = NULL;
            }
            else if (get_epoch_time(timeBuffer) == NULL)
            {
                LogError("Failed getting current time");
                free(result->diagnosticId);
                free(result);
                free(timeBuffer);
                result = NULL;
            }
            else
            {
                result->diagnosticCreationTimeUtc = timeBuffer;
            }
        }
    }
    return result;
}

int IoTHubClient_Diagnostic_AddIfNecessary(IOTHUB_DIAGNOSTIC_SETTING_DATA* diagSetting, IOTHUB_MESSAGE_HANDLE messageHandle)
{
    int result;
    /* Codes_SRS_IOTHUB_DIAGNOSTIC_13_001: [ IoTHubClient_Diagnostic_AddIfNecessary should return nonezero if diagSetting or messageHandle is NULL. ]*/
    if (diagSetting == NULL || messageHandle == NULL)
    {
        result = MU_FAILURE;
    }
    /* Codes_SRS_IOTHUB_DIAGNOSTIC_13_003: [ If diagSamplingPercentage is equal to 0, message number should not be increased and no diagnostic properties added ]*/
    else if (should_add_diagnostic_info(diagSetting))
    {
        /* Codes_SRS_IOTHUB_DIAGNOSTIC_13_004: [ If diagSamplingPercentage is equal to 100, diagnostic properties should be added to all messages]*/
        /* Codes_SRS_IOTHUB_DIAGNOSTIC_13_005: [ If diagSamplingPercentage is between(0, 100), diagnostic properties should be added based on percentage]*/

        IOTHUB_MESSAGE_DIAGNOSTIC_PROPERTY_DATA* diagnosticData;
        if ((diagnosticData = prepare_message_diagnostic_data()) == NULL)
        {
            result = MU_FAILURE;
        }
        else
        {
            if (IoTHubMessage_SetDiagnosticPropertyData(messageHandle, diagnosticData) != IOTHUB_MESSAGE_OK)
            {
                /* Codes_SRS_IOTHUB_DIAGNOSTIC_13_002: [ IoTHubClient_Diagnostic_AddIfNecessary should return nonezero if failing to add diagnostic property. ]*/
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }

            free(diagnosticData->diagnosticCreationTimeUtc);
            free(diagnosticData->diagnosticId);
            free(diagnosticData);
            diagnosticData = NULL;
        }
    }
    else
    {
        result = 0;
    }

    return result;
}
