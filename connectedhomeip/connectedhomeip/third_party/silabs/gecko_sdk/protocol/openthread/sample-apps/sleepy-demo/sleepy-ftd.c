/***************************************************************************//**
 * @file
 * @brief FTD application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include <string.h>
#include <assert.h>

#include <openthread/cli.h>
#include <openthread/dataset_ftd.h>
#include <openthread/instance.h>
#include <openthread/message.h>
#include <openthread/thread.h>
#include <openthread/udp.h>
#include <openthread/platform/logging.h>
#include <common/code_utils.hpp>
#include <common/logging.hpp>

#include "sl_button.h"
#include "sl_simple_button.h"

#include "sl_component_catalog.h"

// Constants
#define MULTICAST_ADDR "ff03::1"
#define MULTICAST_PORT 123
#define RECV_PORT 234
#define MTD_MESSAGE "mtd button"
#define FTD_MESSAGE "ftd button"

// Forward declarations
otInstance *otGetInstance(void);
void sFtdReceiveCallback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);
extern void otSysEventSignalPending(void);

// Variables
static otUdpSocket         sFtdSocket;
static bool                sHaveSwitchAddress = false;
static otIp6Address        sSwitchAddress;
static bool                sFtdButtonPressed  = false;

void sleepyInit(void)
{
    otCliOutputFormat("sleepy-demo-ftd started\r\n");
}

/**
 * Override default network settings, such as panid, so the devices can join a network
 */
void setNetworkConfiguration(void)
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
    error = otDatasetSetActive(otGetInstance(), &aDataset);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("otDatasetSetActive failed with: %d, %s\r\n", error, otThreadErrorToString(error));
        return;
    }
}

void initUdp(void)
{
    otError    error;
    otSockAddr bindAddr;

    // Initialize bindAddr
    memset(&bindAddr, 0, sizeof(bindAddr));
    bindAddr.mPort = MULTICAST_PORT;

    // Open the socket
    error = otUdpOpen(otGetInstance(), &sFtdSocket, sFtdReceiveCallback, NULL);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("FTD failed to open udp multicast with: %d, %s\r\n", error, otThreadErrorToString(error));
        return;
    }

    // Bind to the socket. Close the socket if bind fails.
    error = otUdpBind(otGetInstance(), &sFtdSocket, &bindAddr, OT_NETIF_THREAD);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("FTD failed to bind udp multicast with: %d, %s\r\n", error, otThreadErrorToString(error));
        IgnoreReturnValue(otUdpClose(otGetInstance(), &sFtdSocket));
        return;
    }
}

void sl_button_on_change(const sl_button_t *handle)
{
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED)
    {
        sFtdButtonPressed = true;
        otSysEventSignalPending();
    }
}

#ifdef SL_CATALOG_KERNEL_PRESENT
#define applicationTick sl_ot_rtos_application_tick
#endif

void applicationTick(void)
{
    otMessageInfo messageInfo;
    otMessage    *message = NULL;
    const char   *payload = FTD_MESSAGE;

    // Check for button press
    if (sFtdButtonPressed)
    {
        sFtdButtonPressed = false;

        // Get a message buffer
        VerifyOrExit((message = otUdpNewMessage(otGetInstance(), NULL)) != NULL);

        // Setup messageInfo
        VerifyOrExit(sHaveSwitchAddress);
        memset(&messageInfo, 0, sizeof(messageInfo));
        memcpy(&messageInfo.mPeerAddr, &sSwitchAddress, sizeof messageInfo.mPeerAddr);
        messageInfo.mPeerPort = RECV_PORT;

        // Append the FTD_MESSAGE payload to the message buffer
        SuccessOrExit(otMessageAppend(message, payload, (uint16_t)strlen(payload)));

        // Send the button press message
        SuccessOrExit(otUdpSend(otGetInstance(), &sFtdSocket, message, &messageInfo));

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
