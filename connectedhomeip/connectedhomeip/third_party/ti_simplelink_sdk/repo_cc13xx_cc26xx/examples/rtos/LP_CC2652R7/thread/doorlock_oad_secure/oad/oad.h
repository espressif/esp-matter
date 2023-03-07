/******************************************************************************

 @file oad.h

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

#ifndef _OAD_H_
#define _OAD_H_
#include <ti/sysbios/knl/Queue.h>

/* User configurable OAD parameters */
/* OAD DL timeout in ms */
/* Set the timeout value of the OAD. OAD aborts after timeout elapses,
 * if the device is still attached. If the device is not attached,
 * OAD will resume once the connection is re-established.
 */
#define OAD_DELAY_TIMEOUT             140000
/*OAD registration timeout in ms */
#define OAD_REG_KEEPALIVE_TIME        600000
#define OAD_REG_INIT_TIME              60000

/* factory image type in case downloading different tech type images */
#define OAD_USER_CFG_FACTORY_IMG      OAD_WIRELESS_TECH_THREAD


/* ID for OAD NV */
#define NV_OAD_ID    0x0001

#ifdef CC2652RB_OAD
/* Platform type for distinguishing CC2652RB devices */
#define CC2652RB_PLATFORM_OAD 0x04
#endif

#define DEFAULT_COAP_HEADER_TOKEN_LEN 2

typedef enum
{
  oad_evtNtf                 = 1, ///< Ntf event received
  oad_evtImgBlockReq,             ///< Block Req event received
  oad_evtFwVersionReq,            ///< Fw Version Req event received
  oad_evtDLResume,                ///< OAD DL resume evt
  oad_evtDLComplete,              ///< OAD DL complete evt
} oadEvent_e;

typedef enum
{
  OAD_FW_VERSION_REQ                 = 0, ///< Firmware version request
  OAD_FW_VER_RSP,                         ///< Firmware version response
  OAD_IMG_NTF_REQ,                        ///< OAD update image identify request
  OAD_IMG_NTF_RES,                        ///< OAD update image identify response
  OAD_IMG_BLOCK_REQ,                      ///< OAD update image block request
  OAD_IMG_BLOCK_RSP,                      ///< OAD update image block request
} oadFrameType_e;
/*!
 * Stores information related to OAD write event
 */
/* OAD Block size */
#ifndef OAD_BLOCK_SIZE
#define OAD_BLOCK_SIZE   128
#endif
#define OAD_IMG_URI    "oad/img"
#define OAD_REG_URI    "oad/reg"
#define OAD_NTF_URI    "oad/ntf"
#define OAD_FWV_URI    "oad/fwv"

/*!
 * @defgroup OAD_STATUS_CODES OAD return status codes
 * @{
 */
#define OAD_SUCCESS                         0   //!< OAD succeeded
#define OAD_CRC_ERR                         1   //!< Downloaded image CRC doesn't match
#define OAD_FLASH_ERR                       2   //!< Flash function failure (int, ext)
#define OAD_BUFFER_OFL                      3   //!< Block Number doesn't match requested.
#define OAD_ALREADY_STARTED                 4   //!< OAD is already is progress
#define OAD_NOT_STARTED                     5   //!< OAD has not yet started
#define OAD_DL_NOT_COMPLETE                 6   //!< An OAD is ongoing
#define OAD_NO_RESOURCES                    7   //!< If memory allocation fails
#define OAD_IMAGE_TOO_BIG                   8   //!< Candidate image is too big
#define OAD_INCOMPATIBLE_IMAGE              9   //!< Image signing failure, boundary mismatch
#define OAD_INVALID_FILE                    10  //!< If Invalid image ID received
#define OAD_INCOMPATIBLE_FILE               11  //!< BIM or FW mismatch
#define OAD_AUTH_FAIL                       12  //!< Authorization failed
#define OAD_EXT_NOT_SUPPORTED               13  //!< Ctrl point command not supported
#define OAD_DL_COMPLETE                     14  //!< OAD image payload download complete
#define OAD_CCCD_NOT_ENABLED                15  //!< CCCD is not enabled, notif can't be sent
#define OAD_IMG_ID_TIMEOUT                  16  //!< Image identify timed out, too many failures
#define OAD_IMG_VALID                       17  //!< Image valid
#define OAD_IMG_REJECTED                    18  //!< Image rejected
/** @} End OAD_STATUS_CODES */

/**
 * This structure represents the local and peer IPv6 socket addresses.
 */
typedef struct __attribute__((packed))
{
    otIp6Address mPeerAddr;           //!< The peer IPv6 address
    uint16_t     mPeerPort;           //!< The peer transport-layer port
} oad_srcAddrInfo_t;

/**
 * Stores information related to OAD receive event
 */
typedef struct
{
    Queue_Elem        _elem;          //!< New queue element to contain write evt info
    oadEvent_e        event;          //!< Event that occurred
    uint16_t          len;            //!< received msg len
    uint8_t           *msg;           //!< received msg
    otMessage         *responseMsg;   //!< generated response message
    oad_srcAddrInfo_t messageInfo;    //!< src Addr Info
} oad_targetReceive_t;

/* Image Identify Payload */
typedef struct __attribute__((packed))
{
    uint8_t   imgID[8];       //!< User-defined Image Identification bytes. */
    uint8_t   bimVer;         //!< BIM version */
    uint8_t   metaVer;        //!< Metadata version */
    uint8_t   imgCpStat;      //!< Image copy status bytes */
    uint8_t   crcStat;        //!< CRC status */
    uint8_t   imgType;        //!< Image Type */
    uint8_t   imgNo;          //!< Image number of 'image type' */
    uint32_t  len;            //!< Image length in octets
    uint8_t   softVer[4];     //!< Software version of the image */
} oad_imgIdentifyPld_t;

/*!
 * Req/Rsp Packet Types
 */
typedef struct  __attribute__((packed)){
    otIp6Address ipAddr;
    char         fwVerStr[16];
    uint8_t      imgID[8];
    uint8_t      platformType;
} oad_regMsg_t;

/*!
 * Req/Rsp Packet Types
 */
typedef struct  __attribute__((packed)){
    uint8_t  img_id;
    uint16_t blkNum;
    uint16_t totalBlocks;
} oad_imgBlockReq_t;

typedef struct  __attribute__((packed)){
    uint8_t              imgId;
    oad_imgIdentifyPld_t imgHdr;
} oad_imgNtfReq_t;

typedef struct  __attribute__((packed)){
    uint8_t imgId;
    uint8_t status;
} oad_imgNtfRsp_t;

typedef struct  __attribute__((packed)){
    uint8_t  img_id;
    uint16_t blkNum;
    uint8_t  data[OAD_BLOCK_SIZE];
} oad_imgBlockRsp_t;

void OAD_open(void);

void OAD_close(void);

void OAD_abort(void);

void OAD_resume(void);

void OAD_pause(void);

void OAD_processQueue(void);

void OAD_processCtrlEvents(appEvent_e event);

void oad_sendRegMsg(void);

uint8_t oad_infoSet(void *p, uint16_t len, uint8_t idx);

uint8_t oad_infoGet(void *p, uint16_t len, uint8_t idx);

void oad_sendRspMsg(oad_targetReceive_t *oadWriteEvt, uint8_t *pData, uint16_t len);

#endif
