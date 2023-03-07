/*
 *  Copyright (c) 2020, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "bsp.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "gpiointerrupt.h"
#include "hal-config.h"
#include "hal_common.h"
#include "openthread-system.h"
#include <assert.h>
#include <common/code_utils.hpp>
#include <common/logging.hpp>
#include <openthread-core-config.h>
#include <string.h>
#include <openthread/cli.h>
#include <openthread/config.h>
#include <openthread/dataset_ftd.h>
#include <openthread/diag.h>
#include <openthread/instance.h>
#include <openthread/message.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/udp.h>
#include <openthread/platform/logging.h>

// Constants
#define MULTICAST_ADDR "ff03::1"
#define MULTICAST_PORT 123
#define RECV_PORT 234
#define MTD_MESSAGE "mtd button"
#define FTD_MESSAGE "ftd button"

// Types
typedef struct ButtonArray
{
    GPIO_Port_TypeDef port;
    unsigned int      pin;
} ButtonArray_t;

// Prototypes
void setNetworkConfiguration(otInstance *aInstance);
void handleNetifStateChanged(uint32_t aFlags, void *aContext);
void gpioInit(void (*gpioCallback)(uint8_t pin));
void buttonCallback(uint8_t pin);
void initUdp(void);
void applicationTick(void);
void sFtdReceiveCallback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

/**
 * This function initializes the CLI app.
 *
 * @param[in]  aInstance  The OpenThread instance structure.
 *
 */
extern void otAppCliInit(otInstance *aInstance);

// Variables
static otInstance *        instance;
static otUdpSocket         sFtdSocket;
static bool                sHaveSwitchAddress = false;
static otIp6Address        sSwitchAddress;
static bool                sFtdButtonPressed              = false;
static const ButtonArray_t sButtonArray[BSP_BUTTON_COUNT] = BSP_BUTTON_INIT;

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

int main(int argc, char *argv[])
{
    otSysInit(argc, argv);
    gpioInit(buttonCallback);

    instance = otInstanceInitSingle();
    assert(instance);

    otAppCliInit(instance);
    otCliOutputFormat("sleepy-demo-ftd started\r\n");

    setNetworkConfiguration(instance);
    otSetStateChangedCallback(instance, handleNetifStateChanged, instance);
    initUdp();

    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);

    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);
        applicationTick();
    }

    otInstanceFinalize(instance);
    return 0;
}

/*
 * Provide, if required an "otPlatLog()" function
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif

/**
 * Override default network settings, such as panid, so the devices can join a network
 */
void setNetworkConfiguration(otInstance *aInstance)
{
    static char          aNetworkName[] = "SleepyEFR32";
    otError              error;
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Key, PSKc, Security Policy
     */
    aDataset.mActiveTimestamp.mSeconds             = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 15 */
    aDataset.mChannel                      = 15;
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    aDataset.mPanId                      = (otPanId)0x2222;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xC0, 0xDE, 0x1A, 0xB5, 0xC0, 0xDE, 0x1A, 0xB5};
    memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    /* Set network key to 1234C0DE1AB51234C0DE1AB51234C0DE */
    uint8_t key[OT_NETWORK_KEY_SIZE] = {0x12, 0x34, 0xC0, 0xDE, 0x1A, 0xB5, 0x12, 0x34,
                                        0xC0, 0xDE, 0x1A, 0xB5, 0x12, 0x34, 0xC0, 0xDE};
    memcpy(aDataset.mNetworkKey.m8, key, sizeof(aDataset.mNetworkKey));
    aDataset.mComponents.mIsNetworkKeyPresent = true;

    /* Set Network Name to SleepyEFR32 */
    size_t length = strlen(aNetworkName);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, aNetworkName, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    /* Set the Active Operational Dataset to this dataset */
    error = otDatasetSetActive(aInstance, &aDataset);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("otDatasetSetActive failed with: %d, %s\r\n", error, otThreadErrorToString(error));
        return;
    }
}

void handleNetifStateChanged(uint32_t aFlags, void *aContext)
{
    if ((aFlags & OT_CHANGED_THREAD_ROLE) != 0)
    {
        otDeviceRole changedRole = otThreadGetDeviceRole(aContext);

        switch (changedRole)
        {
        case OT_DEVICE_ROLE_LEADER:
            otCliOutputFormat("sleepy-demo-ftd changed to leader\r\n");
            break;
        case OT_DEVICE_ROLE_ROUTER:
            otCliOutputFormat("sleepy-demo-ftd changed to router\r\n");
            break;

        case OT_DEVICE_ROLE_CHILD:
            break;

        case OT_DEVICE_ROLE_DETACHED:
        case OT_DEVICE_ROLE_DISABLED:
            break;
        }
    }
}

void gpioInit(void (*callback)(uint8_t pin))
{
    // set up button GPIOs to input with pullups
    for (int i = 0; i < BSP_BUTTON_COUNT; i++)
    {
        GPIO_PinModeSet(sButtonArray[i].port, sButtonArray[i].pin, gpioModeInputPull, 1);
    }
    // set up interrupt based callback function on falling edge
    GPIOINT_Init();
    GPIOINT_CallbackRegister(sButtonArray[0].pin, callback);
    GPIOINT_CallbackRegister(sButtonArray[1].pin, callback);
    GPIO_IntConfig(sButtonArray[0].port, sButtonArray[0].pin, false, true, true);
    GPIO_IntConfig(sButtonArray[1].port, sButtonArray[1].pin, false, true, true);
}

void initUdp(void)
{
    otError    error;
    otSockAddr bindAddr;

    // Initialize bindAddr
    memset(&bindAddr, 0, sizeof(bindAddr));
    bindAddr.mPort = MULTICAST_PORT;

    // Open the socket
    error = otUdpOpen(instance, &sFtdSocket, sFtdReceiveCallback, NULL);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("FTD failed to open udp multicast with: %d, %s\r\n", error, otThreadErrorToString(error));
        return;
    }

    // Bind to the socket. Close the socket if bind fails.
    error = otUdpBind(instance, &sFtdSocket, &bindAddr, OT_NETIF_THREAD);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("FTD failed to bind udp multicast with: %d, %s\r\n", error, otThreadErrorToString(error));
        IgnoreReturnValue(otUdpClose(instance, &sFtdSocket));
        return;
    }
}

void buttonCallback(uint8_t pin)
{
    OT_UNUSED_VARIABLE(pin);
    sFtdButtonPressed = true;
}

void applicationTick(void)
{
    otMessageInfo messageInfo;
    otMessage *   message = NULL;
    const char *  payload = FTD_MESSAGE;

    // Check for button press
    if (sFtdButtonPressed)
    {
        sFtdButtonPressed = false;

        // Get a message buffer
        VerifyOrExit((message = otUdpNewMessage(instance, NULL)) != NULL);

        // Setup messageInfo
        VerifyOrExit(sHaveSwitchAddress);
        memset(&messageInfo, 0, sizeof(messageInfo));
        memcpy(&messageInfo.mPeerAddr, &sSwitchAddress, sizeof messageInfo.mPeerAddr);
        messageInfo.mPeerPort = RECV_PORT;

        // Append the FTD_MESSAGE payload to the message buffer
        SuccessOrExit(otMessageAppend(message, payload, (uint16_t)strlen(payload)));

        // Send the button press message
        SuccessOrExit(otUdpSend(instance, &sFtdSocket, message, &messageInfo));

        // Set message pointer to NULL so it doesn't get free'd by this function.
        // otUdpSend() executing successfully means OpenThread has taken ownership
        // of the message buffer.
        message = NULL;
    }

exit:
    if (message != NULL)
    {
        otMessageFree(message);
    }
    return;
}

void sFtdReceiveCallback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aMessageInfo);
    uint8_t buf[64];
    int     length;

    // Read the received message's payload
    length      = otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, sizeof(buf) - 1);
    buf[length] = '\0';

    // Check that the payload matches MTD_MESSAGE
    VerifyOrExit(strncmp((char *)buf, MTD_MESSAGE, sizeof(MTD_MESSAGE)) == 0);

    // Store the MTD's address
    sHaveSwitchAddress = true;
    memcpy(&sSwitchAddress, &aMessageInfo->mPeerAddr, sizeof sSwitchAddress);

    otCliOutputFormat("Message Received: %s\r\n", buf);

exit:
    return;
}
