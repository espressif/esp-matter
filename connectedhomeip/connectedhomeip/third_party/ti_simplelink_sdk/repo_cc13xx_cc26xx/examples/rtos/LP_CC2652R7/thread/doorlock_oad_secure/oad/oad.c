/******************************************************************************

 @file oad.c

 @brief OAD processing and utility functions

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>
#include <openthread-core-config.h>


/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>

/* OpenThread public API Header files */
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/coap.h>
#include <openthread/platform/logging.h>
#include <openthread/dataset.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"
#include "platform/nv/nvintf.h"
#include "platform/nv/nvocmp.h"

/* Board Header files */
#include "ti_drivers_config.h"

#include "images.h"
#include "doorlock.h"
#include "utils/code_utils.h"

#include "disp_utils.h"
#include "otstack.h"
/* TIRTOS specific header files */
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/BIOS.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>

/* driverlib specific header */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/chipinfo.h)

#include "doorlock.h"
#include "oad.h"
#include "oad_image_header.h"
#include "oad_storage.h"
#include "flash_interface.h"
#include <ti/sysbios/knl/Queue.h>

/* OAD CoAP Resources */
static otCoapResource oadNtfResource;
static otCoapResource oadFwvResource;

/* OAD Timeout timer */
static timer_t oadTimeoutTimerID;
/* Registration Timer ID */
static timer_t oadRegTimerID;

// OAD Queue
static Queue_Struct oadQ;
static Queue_Handle hOadQ;

oad_imgBlockReq_t   oadImgBlock;

/* port to report the temperature to */
uint16_t peerPort = OT_DEFAULT_COAP_PORT;

static otError oad_setupCoapServer(otInstance *aInstance, otCoapResource *oadResource);
static void    oad_configureTimeoutTimer(uint32_t timeout);
static void    oad_timeoutCB(union sigval val);
static void    oad_regTimeoutCB(union sigval val);

/* Static local variables */
static NVINTF_nvFuncts_t sNvoctpOADFps = { 0 };

/* settings API */
void oad_settingsNVInit(void)
{
    /* Load NVOCMP function pointers, extended API */
    NVOCMP_loadApiPtrsExt(&sNvoctpOADFps);
}

/**
 * @brief Callback function periodic registration timer event.
 *
 * @param val Argument passed by the clock if set up.
 *
 * @return None
 */
static void oad_regTimeoutCB(union sigval val)
{
    app_postEvt(OAD_CtrlRegEvt);

    (void) val;
}


/*********************************************************************
 * @brief Configure the timer.
 *
 * @return None
 */
static void oad_configureRegistrationTimer(void)
{
    struct sigevent event =
    {
        .sigev_notify_function = oad_regTimeoutCB,
        .sigev_notify          = SIGEV_SIGNAL,
    };

    timer_create(CLOCK_MONOTONIC, &event, &oadRegTimerID);
}

/*********************************************************************
 * @brief Starts the timer.
 *
 * @param timeout Time in milliseconds.
 * @param timerID Timer ID to start
 * Should be called after the timer has been created.
 */
static void oad_startTimer(uint32_t timeout, timer_t timerID)
{
    struct itimerspec newTime  = {0};
    struct itimerspec zeroTime = {0};
    struct itimerspec currTime;

    newTime.it_value.tv_sec  = (timeout / 1000U);
    newTime.it_value.tv_nsec = ((timeout % 1000U) * 1000000U);

    /* Disarm timer if currently armed */
    timer_gettime(timerID, &currTime);
    if ((currTime.it_value.tv_sec != 0) || (currTime.it_value.tv_nsec != 0))
    {
        timer_settime(timerID, 0, &zeroTime, NULL);
    }
    /*Disable keep-alive registration timer */
    DISPUTILS_SERIALPRINTF(0, 0, "start timerID = %x\n", timerID);
    /* Arm timer */
    timer_settime(timerID, 0, &newTime, NULL);
}

/*********************************************************************
 * @brief Stop the timer.
 *
 * @param timerID Timer which is to be stopped.
 *
 */
static void oad_cancelTimer(timer_t timerID)
{
    struct itimerspec zeroTime = {0};

    /*Disable keep-alive registration timer */
    DISPUTILS_SERIALPRINTF(0, 0, "canceling timer\n");

    timer_settime(timerID, 0, &zeroTime, NULL);
}

/*********************************************************************
 * @fn      oad_enqueueMsg
 *
 * @brief   Create Queue element and add it to the OAD message
 *
 * @param   event        Event type:
 *                       OAD_WRITE_IDENTIFY_REQ
 *                       OAD_WRITE_BLOCK_REQ
 *                       OAD_IMAGE_COMPLETE
 *                       OAD_EXT_CTRL_WRITE_CMD
 * @param   aContext     The connection Handle this request is from.
 * @param   aMessage     Pointer to CoAP message
 * @param   aMessageInfo src/dst addr, port information of the msg
 * @param   aResponseMsg Pointer to otMessage that will be a response, NULL if
 *                       unsolicited
 *
 * @return  status - whether or not the operation succeeded.
 */
uint8_t oad_enqueueMsg(oadEvent_e event, void *aContext, otMessage *aMessage,
        const otMessageInfo *aMessageInfo, otMessage *aResponseMsg)
{
    uint8_t  status = OAD_SUCCESS;
    uint16_t length = otMessageGetLength(aMessage) - otMessageGetOffset(aMessage);

    /* Pointer points to header and payload after it. */
    oad_targetReceive_t *oadTargetReceive = malloc(sizeof(oad_targetReceive_t) + length);
    otEXPECT_ACTION(oadTargetReceive != NULL, status = OAD_NO_RESOURCES);

    /* C type array indexing to get a pointer after the header to the payload */
    oadTargetReceive->msg = (uint8_t *)(oadTargetReceive + 1);
    oadTargetReceive->event = event;

    /* Add the message to the Queue for processing */
    oadTargetReceive->messageInfo.mPeerAddr = aMessageInfo->mPeerAddr;
    oadTargetReceive->messageInfo.mPeerPort = aMessageInfo->mPeerPort;

    /* Allocate a response message to create a coap response */
    oadTargetReceive->responseMsg = aResponseMsg;

    oadTargetReceive->len = length;
    otMessageRead(aMessage, otMessageGetOffset(aMessage), oadTargetReceive->msg, length);

    Queue_put(hOadQ, (Queue_Elem *)oadTargetReceive);

    /* send OAD event */
    app_postEvt(OAD_queueEvt);

exit:
    if (OAD_SUCCESS != status && oadTargetReceive)
    {
        free(oadTargetReceive);
    }
    return (status);
}

/**
 * @brief Callback function registered with the Coap server for resource "oad/ntf".
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void oad_handleNtfServer(void *aContext, otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    uint8_t    stat = OAD_SUCCESS;
    otError    error;
    otMessage *responseMsg = NULL;

    responseMsg = otCoapNewMessage((otInstance*)aContext, NULL);
    otEXPECT_ACTION(responseMsg != NULL, stat = OAD_NO_RESOURCES);

    error = otCoapMessageInitResponse(responseMsg, aMessage,
                                      OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_VALID);
    otEXPECT_ACTION(OT_ERROR_NONE == error, stat = OAD_NO_RESOURCES);

    stat = oad_enqueueMsg(oad_evtNtf, aContext, aMessage, aMessageInfo,
                          responseMsg);

exit:
    if (stat != OAD_SUCCESS)
    {
        /*error from oadEnqueueMsg  */
        DISPUTILS_SERIALPRINTF(0, 0, "DL abort - Enqueue Ntf");
        OAD_abort();
    }
}

/**
 * @brief Callback function registered with the Coap server for resource "oad/fwv".
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void oad_handleFwvServer(void *aContext, otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    uint8_t    stat = OAD_SUCCESS;
    otError    error;
    otMessage *responseMsg = NULL;

    responseMsg = otCoapNewMessage((otInstance*)aContext, NULL);
    otEXPECT_ACTION(responseMsg != NULL, stat = OAD_NO_RESOURCES);

    error = otCoapMessageInitResponse(responseMsg, aMessage,
                                      OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_VALID);
    otEXPECT_ACTION(OT_ERROR_NONE == error, stat = OAD_NO_RESOURCES);

    stat = oad_enqueueMsg(oad_evtFwVersionReq, aContext, aMessage,
                          aMessageInfo, responseMsg);

exit:
    if (stat != OAD_SUCCESS)
    {
        /*error from oadEnqueueMsg  */
        DISPUTILS_SERIALPRINTF(0, 0, "DL abort -Enqueue Fwv");
        OAD_abort();
    }
}

/**
 * @brief sets up the application coap server.
 *
 * @param aInstance    A pointer to the OT instance
 * @param oadResource  Pointer to OAD Resource
 *
 * @return OT_ERROR_NONE if successful, else error code
 */
static otError oad_setupCoapServer(otInstance *aInstance, otCoapResource *oadResource)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    OtRtosApi_lock();
    otCoapAddResource(aInstance, oadResource);
    OtRtosApi_unlock();
exit:
    return error;
}

/**
 * @brief Function to send broadcast register msg.
 *
 * @param oadWriteEvt  Pointer to receive data
 * @param pData        Pointer to response msg
 * @param len          Length of response msg
 *
 * @return None
 */
void oad_sendRspMsg(oad_targetReceive_t *oadWriteEvt, uint8_t *pData, uint16_t len)
{
    otError       error = OT_ERROR_NONE;
    otMessageInfo messageInfo;
    const otIp6Address *ipAddr_p;

    OtRtosApi_lock();
    otCoapMessageSetPayloadMarker(oadWriteEvt->responseMsg);
    error = otMessageAppend(oadWriteEvt->responseMsg, pData, len);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    memset(&messageInfo, 0, sizeof(otMessageInfo));
    OtRtosApi_lock();
    ipAddr_p = otThreadGetMeshLocalEid(OtInstance_get());
    OtRtosApi_unlock();
    messageInfo.mSockAddr = *ipAddr_p;
    messageInfo.mSockPort = OT_DEFAULT_COAP_PORT;

    messageInfo.mPeerAddr = oadWriteEvt->messageInfo.mPeerAddr;
    messageInfo.mPeerPort = oadWriteEvt->messageInfo.mPeerPort;

    OtRtosApi_lock();
    error = otCoapSendResponse(OtInstance_get(), oadWriteEvt->responseMsg, &messageInfo);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

exit:

    if (error != OT_ERROR_NONE)
    {
        otMessageFree(oadWriteEvt->responseMsg);
    }
}

/**
 * @brief Function to build the firmware version string.
 *
 * @param fwVersionStr          pointer to fw version req. data
 *
 * @return None
 */
void oad_getFwVersion(char *fwVersionStr)
{
    /*snprintf not being used here, to save flash space */
    memcpy(fwVersionStr, "sv:", 3);
    memcpy(&fwVersionStr[3], oad_image_header.h.softVer, 4);
    memcpy(&fwVersionStr[7], " bv:", 4);

    //convert bimVer to string
    fwVersionStr[12] = (oad_image_header.h.bimVer & 0x0F) < 0xA ?
            ((oad_image_header.h.bimVer & 0x0F) + '0'):
            ((oad_image_header.h.bimVer & 0x0F) + 'A');
    fwVersionStr[11] = ((oad_image_header.h.bimVer & 0xF0) >> 4) < 0xA ?
            (((oad_image_header.h.bimVer & 0xF0) >> 4) + '0'):
            (((oad_image_header.h.bimVer & 0xF0) >> 4) + 'A');
}

/**
 * @brief Function to send firmware version msg.
 *
 * @param oadWriteEvt Pointer to fw version req. Data
 *
 * @return None
 */
void oad_processFwVersion(oad_targetReceive_t *oadWriteEvt)
{
    char fwVersionStr[32] = {0};

    oad_getFwVersion(fwVersionStr);
    oad_sendRspMsg(oadWriteEvt, (uint8_t *)fwVersionStr, (uint16_t)sizeof(fwVersionStr));
}

/**
 * @brief Function to send CoAP request message.
 *
 * @param oadWriteEvt  Pointer to msg information
 * @param coapType     Type of the CoAP message; POST, PUT, GET, DEL
 * @param coapCode     Return code of the CoAP message
 * @param uriPath      CoAP resource path name
 * @param handler      Callback function for the msg
 *
 * @return None
 */
void oad_sendReqMsg(oad_targetReceive_t *oadWriteEvt, otCoapType coapType, otCoapCode coapCode, const char* uriPath, otCoapResponseHandler handler)
{
    otError            error           = OT_ERROR_NONE;
    otMessage          *requestMessage = NULL;
    otMessageInfo      messageInfo;
    otInstance         *instance       = OtInstance_get();
    const otIp6Address *ipAddr_p;

    OtRtosApi_lock();
    requestMessage = otCoapNewMessage(instance, NULL);
    OtRtosApi_unlock();
    otEXPECT_ACTION(requestMessage != NULL, error = OT_ERROR_NO_BUFS);

    OtRtosApi_lock();
    otCoapMessageInit(requestMessage, coapType, coapCode);
    otCoapMessageGenerateToken(requestMessage, DEFAULT_COAP_HEADER_TOKEN_LEN);
    error = otCoapMessageAppendUriPathOptions(requestMessage, uriPath);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    OtRtosApi_lock();
    otCoapMessageSetPayloadMarker(requestMessage);
    error = otMessageAppend(requestMessage, oadWriteEvt->msg, oadWriteEvt->len);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    memset(&messageInfo, 0, sizeof(otMessageInfo));

    ipAddr_p = otThreadGetMeshLocalEid(OtInstance_get());
    messageInfo.mSockAddr = *ipAddr_p;
    messageInfo.mSockPort = OT_DEFAULT_COAP_PORT;
    messageInfo.mPeerAddr = oadWriteEvt->messageInfo.mPeerAddr;
    messageInfo.mPeerPort = oadWriteEvt->messageInfo.mPeerPort;

    OtRtosApi_lock();
    error = otCoapSendRequest(instance, requestMessage, &messageInfo, handler, NULL);
    OtRtosApi_unlock();

exit:
    if(error != OT_ERROR_NONE && requestMessage != NULL)
    {
        DISPUTILS_SERIALPRINTF(0, 0, "Reg. Msg Failure = %d", error);
        OtRtosApi_lock();
        otMessageFree(requestMessage);
        OtRtosApi_unlock();
    }
}

/**
 * @brief callback for CoAp Registration request
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 * @param  aResult       Status of the msg. sent
 *
 * @return None
 */
void oad_regReqHandler(void *aContext,
                       otMessage             *aMessage,
                       const otMessageInfo   *aMessageInfo,
                       otError                aResult)
{
    uint32_t timeout;

    DISPUTILS_SERIALPRINTF(0, 0, "result = %d, COAP code =%d", aResult, otCoapMessageGetCode(aMessage));
    if((aResult == OT_ERROR_NONE) &&
        ((otCoapMessageGetCode(aMessage) >= OT_COAP_CODE_RESPONSE_MIN) &&
         (otCoapMessageGetCode(aMessage) <= OT_COAP_CODE_CONTENT)))
    {
        /* keep_alive timer */
        timeout = OAD_REG_KEEPALIVE_TIME;
        DISPUTILS_SERIALPRINTF(0, 0, "starting registration keep-alive timer=%d", timeout);
        oad_startTimer(timeout, oadRegTimerID);
        /* check OAD DL in progress - OAD_resume */
        OAD_resume();
    }
    else
    {
        //start timer
        timeout = OAD_REG_INIT_TIME;

        oad_startTimer(timeout, oadRegTimerID);
        DISPUTILS_SERIALPRINTF(0, 0, "setting registration timer=%d", timeout);
    }

}
/**
 * @brief Function to send broadcast register msg.
 *
 * @return None
 */
void oad_sendRegMsg(void)
{
    otIp6Address peerAddress = {
        .mFields.m8 = {
            0xff, 0x03, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x02
        },
    };
    const otIp6Address *ipAddr_p;
    oad_regMsg_t        oadRegMsg;

    /* print the reported value to the terminal */
    DISPUTILS_SERIALPRINTF(0, 0, "Sending Reg. Msg");

    oad_targetReceive_t oadWriteEvt;

    OtRtosApi_lock();
    ipAddr_p = otThreadGetMeshLocalEid(OtInstance_get());
    memcpy(&oadRegMsg.ipAddr, ipAddr_p, sizeof(otIp6Address));
    OtRtosApi_unlock();

    /* add fw and BIM version */
    oad_getFwVersion(oadRegMsg.fwVerStr);
    /* platform type - IMG ID */
    memcpy(&oadRegMsg.imgID, oad_image_header.h.imgID, sizeof(oadRegMsg.imgID));
#ifdef CC2652RB_OAD
    oadRegMsg.platformType = CC2652RB_PLATFORM_OAD;
#else
    oadRegMsg.platformType = ChipInfo_GetChipType();//read
#endif
    oadWriteEvt.msg = (uint8_t *)&oadRegMsg;
    oadWriteEvt.len = sizeof(oadRegMsg);
    oadWriteEvt.messageInfo.mPeerAddr = peerAddress;
    oadWriteEvt.messageInfo.mPeerPort = OT_DEFAULT_COAP_PORT;

    oad_sendReqMsg(&oadWriteEvt, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_PUT, OAD_REG_URI, oad_regReqHandler);
}

/**
 * @brief Function to process notification request message and send a response.
 *
 * @param oadWriteEvt    A pointer to the OAD receive event
 * @param oadImgBlock_p  Pointer to the OAD image block
 *
 * @return status = OAD_IMG_REJECTED/ACCEPTED
 */
static uint8_t oad_processNtf(oad_targetReceive_t *oadWriteEvt, oad_imgBlockReq_t *oadImgBlock_p)
{
    uint8_t status = 0;
    oad_imgNtfRsp_t oadImgNtfRsp;
    uint16_t totalBlocks;

    /* TBD: check if CoAP filters duplicate msgs */
    oadImgNtfRsp.imgId = oadImgBlock_p->img_id = oadWriteEvt->msg[0];
    /* init. value */
    oadImgBlock_p->blkNum = 0;

    OADStorage_init();

    oadImgBlock_p->totalBlocks = OADStorage_imgIdentifyWrite(&oadWriteEvt->msg[1]);

    oadImgNtfRsp.status = status = (oadImgBlock_p->totalBlocks != 0);
    DISPUTILS_SERIALPRINTF( 0, 0, "OAD Img blocks = %d", oadImgBlock_p->totalBlocks);

    /* Write Image Info to NV in case of interruption in download
     * Store imgHdr and srcADdInfo
     */
    oad_infoSet(&oadWriteEvt->msg[0], sizeof(oad_imgNtfReq_t), 0);
    oad_infoSet(&oadWriteEvt->messageInfo, sizeof(oad_srcAddrInfo_t), 1);

    /*write total blocks to indicate valid image */
    totalBlocks = oadImgBlock_p->totalBlocks;
    oad_infoSet(&totalBlocks, sizeof(uint16_t), 3);

    oad_sendRspMsg(oadWriteEvt, (uint8_t *)&oadImgNtfRsp, (uint16_t)sizeof(oad_imgNtfRsp_t));

    return status;
}

/**
 * @brief callback for CoAp img block request
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 * @param  aResult       Status of the msg. sent
 *
 * @return None
 */
void oad_blockReqHandler(void *aContext,
        otMessage             *aMessage,
        const otMessageInfo   *aMessageInfo,
        otError                aResult)
{
    uint8_t    stat;
    otCoapCode msgCode;

    DISPUTILS_SERIALPRINTF(0, 0, "result = %d, COAP code =%d", aResult, otCoapMessageGetCode(aMessage));
    if(aResult == OT_ERROR_NONE)
    {
        msgCode = otCoapMessageGetCode(aMessage);
        if((msgCode >= OT_COAP_CODE_RESPONSE_MIN) && (msgCode <= OT_COAP_CODE_CONTENT))
        {
            stat = oad_enqueueMsg(oad_evtImgBlockReq, aContext, aMessage,
                                  aMessageInfo, NULL);
            if (stat != OAD_SUCCESS)
            {
                /*reached here, error from oadEnqueueMsg  */
                DISPUTILS_SERIALPRINTF(0, 0, "DL abort - enqueue");
                OAD_abort();
            }
        }
        else if((msgCode >= OT_COAP_CODE_BAD_REQUEST) && (msgCode <= OT_COAP_CODE_UNSUPPORTED_FORMAT))
        {
            DISPUTILS_SERIALPRINTF(0, 0, "DL abort - 4.0x");
            OAD_abort();
        }
    }
    else
    {
        //start timer
        DISPUTILS_SERIALPRINTF(0, 0, "starting OAD timeout timer=%d", aResult);

        /*connection issues/server down? - pause OAD */
        OAD_pause();

        oad_startTimer(OAD_REG_INIT_TIME, oadRegTimerID);

        /*timeout for OAD - stop reg/shut down */
        oad_startTimer(OAD_DELAY_TIMEOUT, oadTimeoutTimerID);
    }
}

/**
 * @brief Function to check the validity of the image when the DL is complete
 *
 * @param oadImgBlockReq_p  A pointer to oadImgBlockReq struct
 *
 * @return None
 */
void oad_checkDLComplete(oad_imgBlockReq_t *oadImgBlockReq_p)
{
    uint8_t dl_status;

    /*
     * Check that CRC is correct and mark the image as new
     * image to be booted in to by BIM on next reset
     */
    dl_status = OADStorage_imgFinalise();

    if(!dl_status)
    {
        OAD_close();
        /* perform activity related to the un lock event */
        DISPUTILS_SERIALPRINTF(0, 0, "Download complete, rebooting");
        /* reset to BIM */
        SysCtrlSystemReset();

    }
    else
    {
        /* perform activity related to the un lock event */
        DISPUTILS_SERIALPRINTF(0, 0, "Download CRC Error!");
        OAD_abort();
    }
}

/**
 * @brief write the received img Block to ext. flash
 *
 * @param oadWriteEvt     A pointer to the received event
 * @param oadImgBlock_p   Pointer to imgBlockReq struct
 *
 * @return status (flash write success/failure)
 */
uint8_t oad_writeImgBlock(oad_targetReceive_t *oadWriteEvt, oad_imgBlockReq_t *oadImgBlock_p)
{
    uint8_t  status;
    uint16_t blkNum;
    oad_imgBlockRsp_t *oadImgBlockRsp_p = (oad_imgBlockRsp_t *)oadWriteEvt->msg;

    DISPUTILS_SERIALPRINTF( 0, 0, "OAD Block write blkNum=%d, len = %d, data=%d",
                            oadImgBlock_p->blkNum, sizeof(oadImgBlockRsp_p->data), (oadImgBlockRsp_p->data[0]));

    status = OADStorage_imgBlockWrite((oadImgBlock_p->blkNum),
                                      oadImgBlockRsp_p->data,
                                      sizeof(oadImgBlockRsp_p->data));

    /* Write received BlockNum to NV */
    blkNum = oadImgBlock_p->blkNum;
    oad_infoSet((void *)&(blkNum), sizeof(uint16_t), 2);

    return status;
}

/**
 * @brief sends the next block request.
 *
 * @param oadWriteEvt      A pointer to the OAD Rx event
 * @param oadImgBlock_p    A pointer to the OAD image block info
 *
 * @return None
 */
void oad_processImgBlockReq(oad_targetReceive_t *oadWriteEvt, oad_imgBlockReq_t *oadImgBlock_p)
{
    /* print the reported value to the terminal */
    DISPUTILS_SERIALPRINTF(0, 0, "Requesting Block Num:%d", oadImgBlock_p->blkNum);

    oadWriteEvt->msg = (uint8_t *)oadImgBlock_p;
    oadWriteEvt->len = sizeof(oad_imgBlockReq_t);

    oad_sendReqMsg(oadWriteEvt, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_GET, OAD_IMG_URI, oad_blockReqHandler);
}

/**
 * @brief sends the next block request.
 *
 * @param oadWriteEvt      A pointer to the OAD Rx event
 * @param oadImgBlock_p    A pointer to the OAD image block info
 *
 * @return None
 */
void oad_lastImgBlockReq(oad_targetReceive_t *oadWriteEvt, oad_imgBlockReq_t *oadImgBlock_p)
{
    /* print the reported value to the terminal */
    DISPUTILS_SERIALPRINTF(0, 0, "Requesting Block Num:%d", oadImgBlock_p->blkNum);

    oadWriteEvt->msg = (uint8_t *)oadImgBlock_p;
    oadWriteEvt->len = sizeof(oad_imgBlockReq_t);

    oad_sendReqMsg(oadWriteEvt, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_GET, OAD_IMG_URI, NULL);
}

void oad_processDLComplete(void)
{
}

/**
 * @brief Set OAD NVs
 *
 * @param p    A pointer to the NV data
 * @param len  len of the NV
 * @param idx  sub_id of OAD NV
 *
 * @return status (NV write success/failure)
 */
uint8_t oad_infoSet(void *p, uint16_t len, uint8_t idx)
{
    NVINTF_itemID_t id;

    /* Setup NV ID */
    id.systemID = NVINTF_SYSID_APP;
    id.itemID   = NV_OAD_ID;
    id.subID    = idx;
    /* Write the NV item */
    if((sNvoctpOADFps.writeItem != NULL) && (p != NULL))
    {
        return(!sNvoctpOADFps.writeItem(id, len, p));
    }
    else
    {
        return(0);
    }
}

/**
 * @brief Get OAD NVs
 *
 * @param p    A pointer to the NV data
 * @param len  len of the NV
 * @param idx  sub_id of OAD NV
 *
 * @return status (NV read success/failure)
 */
uint8_t oad_infoGet(void *p, uint16_t len, uint8_t idx)
{
    NVINTF_itemID_t id;

    /* Setup NV ID */
    id.systemID = NVINTF_SYSID_APP;
    id.itemID   = NV_OAD_ID;
    id.subID    = idx;
    /* Read the NV item */
    if((sNvoctpOADFps.readItem != NULL) && (p != NULL))
    {
        return(!sNvoctpOADFps.readItem(id, 0, len, p));
    }
    else
    {
        return(0);
    }
}

/*********************************************************************
 * @fn      oad_storage_resume
 *
 * @brief   Reopens storage and clears current page
 *
 * @param   blockNum  Pointer to the last blockNum at which the OAD download was interrupted
 *
 * @return  none
 */
void oad_storage_resume(uint16_t *blockNum)
{
    uint32_t page = (*blockNum * OADStorage_BLOCK_SIZE)/EFL_PAGE_SIZE;

    //erase the page in case it was corrupted during a reset / power cycle
    OADStorage_eraseImgPage(page);

    /* Round block number to nearest page boundary in case flash page is
     * corrupted
     */
    *blockNum = (page * EFL_PAGE_SIZE)/ OADStorage_BLOCK_SIZE;
}

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param val           argument passed by the clock if set up.
 *
 * @return None
 */
static void oad_timeoutCB(union sigval val)
{
    /* Delayed abort. Check if device role attached if detached, DL will resume
     * when attached again, do not abort.
     */
    if(otThreadGetDeviceRole(OtInstance_get()) != OT_DEVICE_ROLE_DETACHED)
    {
     OAD_abort();
    }
}

/**
 * @brief Configure the timer.
 *
 * @param timeout       time in milliseconds.
 *
 * @return None
 */
static void oad_configureTimeoutTimer(uint32_t timeout)
{
    struct sigevent event =
    {
        .sigev_notify_function = oad_timeoutCB,
        .sigev_notify          = SIGEV_SIGNAL,
    };

    timer_create(CLOCK_MONOTONIC, &event, &oadTimeoutTimerID);
}

/**
 * @brief Initialize OAD module
 *
 * @return None
 */
void OAD_open(void)
{
    // Initialize NV settings
    oad_settingsNVInit();

    /*Initialize delayed abort timer */
    oad_configureTimeoutTimer(OAD_DELAY_TIMEOUT);

    // Construct the OAD Queue
    Queue_construct(&oadQ, NULL);

    // Get the handle to the newly constructed Queue
    hOadQ = Queue_handle(&oadQ);

    oadNtfResource.mHandler = &oad_handleNtfServer;
    oadNtfResource.mUriPath = (const char*)OAD_NTF_URI;
    oadNtfResource.mContext = OtInstance_get();
    oad_setupCoapServer(OtInstance_get(), &oadNtfResource);

    oadFwvResource.mHandler = &oad_handleFwvServer;
    oadFwvResource.mUriPath = (const char*)OAD_FWV_URI;
    oadFwvResource.mContext = OtInstance_get();
    oad_setupCoapServer(OtInstance_get(), &oadFwvResource);

    // check if external flash memory available
    if(hasExternalFlash() == true)
    {
        // Create factory image if there isn't one
        if(!OADStorage_checkFactoryImage())
        {
            OADStorage_createFactoryImageBackup();
        }
        else
        {
            /*check factory image tech type matches to user requested image */
            if(OADStorage_getFactoryImage() != OAD_USER_CFG_FACTORY_IMG)
            {
                OADStorage_createFactoryImageBackup();
            }
        }
    }

    /*Periodic timer to send reg. message */
    oad_configureRegistrationTimer();

    /* Send Device Address in the Reg. message to BA, every time device re-boots.*/
    oad_sendRegMsg();
}

/**
 * @brief Close the OAD module
 *
 * @return None
 */
void OAD_close(void)
{
    oad_imgNtfReq_t oadImgInfo;
    uint16_t        blkNum;
    uint16_t        totalBlocks;

    memset(&oadImgBlock, 0, sizeof(oad_imgBlockReq_t));
    memset(&oadImgInfo, 0, sizeof(oad_imgNtfReq_t));

    /* erase NVs */
    oad_infoSet((void *)(&oadImgInfo), sizeof(oad_imgNtfReq_t), 0);
    blkNum = oadImgBlock.blkNum;
    oad_infoSet((void *)(&blkNum), sizeof(uint16_t), 2);
    /*write total blocks to indicate valid image */
    totalBlocks = oadImgBlock.totalBlocks;
    oad_infoSet(&totalBlocks, sizeof(uint16_t), 3);

    OADStorage_close();
}

/**
 * @brief Abort OAD module
 *
 * @return None
 */
void OAD_abort(void)
{
    /* abort */
    DISPUTILS_SERIALPRINTF(0, 0, "DL abort");

    DISPUTILS_SERIALPRINTF(0, 0, "start OAD registration timer again!");

    oad_startTimer(OAD_REG_INIT_TIME, oadRegTimerID);

    OAD_close();
}
/**
 * @brief Pause OAD module, if valid DL in prog.
 *
 * @return None
 */
void OAD_pause(void)
{
    uint16_t imgValid = 0;

    /* if download in progress */
    oad_infoGet(&imgValid, sizeof(uint16_t), 3);
    if(imgValid)
    {
        /* pause */
        DISPUTILS_SERIALPRINTF(0, 0, "DL pause");

        //OADStorage_close();
    }
}

/**
 * @brief Resume OAD module
 *
 * @return None
 */
void OAD_resume(void)
{
    oad_imgNtfReq_t     oadImgInfo;
    uint8_t             *pBlockData;
    uint16_t            blkNum;

    OADStorage_init();

    oad_infoGet((void *)(&oadImgInfo), sizeof(oad_imgNtfReq_t), 0);

    /*Restore imgBlock info */
    pBlockData = (uint8_t *)&oadImgInfo.imgHdr;
    oadImgBlock.totalBlocks = OADStorage_imgIdentifyWrite(pBlockData);
    oadImgBlock.img_id = oadImgInfo.imgId;
    /*Get current blkNum in progress */
    oad_infoGet((void *)(&blkNum), sizeof(uint16_t), 2);
    oadImgBlock.blkNum = blkNum;

    /*check image DL in progress */
    if(oadImgBlock.totalBlocks > 0)
    {
        /* resume */
        DISPUTILS_SERIALPRINTF(0, 0, "DL resume");

        oad_targetReceive_t *oadWriteEvt = malloc(sizeof(oad_targetReceive_t));

        if ( oadWriteEvt != NULL )
        {
            /*Restore OAD storage parameters */
            blkNum = oadImgBlock.blkNum;
            oad_storage_resume(&blkNum);
            oadImgBlock.blkNum = blkNum;

            /*restore OAD Src Addrinfo */
            oad_infoGet((void *)(&oadWriteEvt->messageInfo), sizeof(oad_srcAddrInfo_t), 1);
            /*DL in progress - Disable keep-alive registration timer */
            oad_cancelTimer(oadRegTimerID);
            /*timeout for OAD - stop reg/shut down */
            oad_cancelTimer(oadTimeoutTimerID);
            /*send BlockReq */

            /*TBD: needs to throw an event */
            oadWriteEvt->event = oad_evtDLResume;
            Queue_put(hOadQ, (Queue_Elem *)oadWriteEvt);

            /* send OAD event */
            app_postEvt(OAD_queueEvt);

        }
    }
}

/**
 * @brief A function to process the OAD periodic ctrl events.
 *
 * @return None
 */
void OAD_processCtrlEvents(appEvent_e event)
{

    if(event & OAD_CtrlRegEvt)
    {
        /* Send reg message on timeout.*/
        oad_sendRegMsg();
    }
}
/**
 * @brief A function to process the OAD events.
 *
 * @return None
 */
void OAD_processQueue(void)
{
    // Status of the event
    uint8_t status;
    oad_imgBlockRsp_t *oadImgBlockRsp_p;

    // Get the message at the Queue head
    oad_targetReceive_t *oadWriteEvt = Queue_get(hOadQ);

    // To Do:Iterate over all the messages in the queue, in a thread safe manner
    if (oadWriteEvt != NULL)
    {
        // Extract the event from the message
        oadEvent_e event = oadWriteEvt->event;
        switch(event)
        {
        case oad_evtNtf:
            /* perform activity related to the lock event. */
            DISPUTILS_SERIALPRINTF( 0, 0, "OAD NTF Event received");

            status = oad_processNtf(oadWriteEvt, &oadImgBlock);
            if(status)
            {
                /*DL in progress - Disable keep-alive registration timer */
                oad_cancelTimer(oadRegTimerID);
                /*send first block req message */
                oad_processImgBlockReq(oadWriteEvt, &oadImgBlock);
            }
            else
            {
                DISPUTILS_SERIALPRINTF( 0, 0, "OAD Img Rejected!");
                OAD_abort();
            }

            break;
        case oad_evtImgBlockReq:
            /* perform activity related to img block rx event. */
            DISPUTILS_SERIALPRINTF( 0, 0, "OAD BlockRsp Event received");
            oadImgBlockRsp_p = (oad_imgBlockRsp_t *)oadWriteEvt->msg;
            if(oadImgBlock.blkNum != oadImgBlockRsp_p->blkNum)
            {
                DISPUTILS_SERIALPRINTF( 0, 0, "OAD not valid pkt req=%d rxed=%d, discarding",
                                        oadImgBlock.blkNum,
                                        oadImgBlockRsp_p->blkNum);
            }
            else
            {
                status = oad_writeImgBlock(oadWriteEvt, &oadImgBlock);

                if(status)
                {
                    DISPUTILS_SERIALPRINTF( 0, 0, "OAD Block Write failure");
                    OAD_abort();
                }
                else
                {
                    oadImgBlock.blkNum++;
                    /*check download complete */
                    if(oadImgBlock.blkNum <= (oadImgBlock.totalBlocks-1))
                    {
                        /*send next block req message */
                        oad_processImgBlockReq(oadWriteEvt, &oadImgBlock);
                    }
                    else
                    {
                        /*send last block req message */
                        oad_lastImgBlockReq(oadWriteEvt, &oadImgBlock);
                        oad_checkDLComplete(&oadImgBlock);
                    }
                }
            }
            break;
        case oad_evtDLResume:
            oad_processImgBlockReq(oadWriteEvt, &oadImgBlock);
            break;
        case oad_evtFwVersionReq:
            oad_processFwVersion(oadWriteEvt);
            break;
        case oad_evtDLComplete:
        default:
            break;
        }
        free(oadWriteEvt);
    }
}

