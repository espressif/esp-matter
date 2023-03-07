/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//*****************************************************************************
//
//! \addtogroup file_operations
//! @{
//
//*****************************************************************************

// Standard includes


#include <ifmod/debug_if.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>

#include <ti/drivers/net/wifi/simplelink.h>
#include <ti/drivers/power/PowerCC32XX.h>
#include <ti/devices/cc32xx/inc/hw_memmap.h>
#include <ti/devices/cc32xx/inc/hw_gprcm.h>
#include <ti/devices/cc32xx/inc/hw_types.h>
#include <ti/devices/cc32xx/driverlib/prcm.h>
#include <ti/net/ota/ota.h>
#include <ti/net/ota/source/OtaArchive.h>
#include <ti/net/http/httpclient.h>

#include "ota_if.h"
#include "httpsrv_if.h"
#include "utils_if.h"
#include "debug_if.h"

#if OTA_SUPPORT
//*****************************************************************************
//                 MODULE DEFINITIONS
//*****************************************************************************
#undef DEBUG_IF_NAME
#define DEBUG_IF_NAME       "OTA"
#undef DEBUG_IF_SEVERITY
#define DEBUG_IF_SEVERITY   OTA_IF_DEBUG_LEVEL

//*****************************************************************************
//                 MACROS and TYPE DEFINITION
//*****************************************************************************
#define OTA_BUFF_SIZE          1024

#define IS_FLASH_DEVICE()       (m_ota.localDevType & 0x001)

#define USER_AGENT "HTTPClient (ARM; TI-RTOS)"
#define MAX_SERVER_NAME         64


typedef int (*StartSession_f)(void **ph, uint8_t *pbuff, uint16_t buffLen, void *pParams);
typedef int (*GetChunk_f)(void *h, uint8_t *pBuff, uint16_t buffLen);
typedef int (*EndSession_f)(void *h, int status);


/****************************
 * OTA Task Messages
 ****************************
 */
/* OTA_IF States */
typedef enum {
    OTA_STATE_UNINITIALIZED,    /* Before OTA_IF_init is called */
    OTA_STATE_READY,            /* OTA_IF is initialized and idle */
    OTA_STATE_IN_PROGRESS,      /* Image loading in process */
    OTA_STATES_MAX
} OtaIfState;

typedef enum {
    OTA_STATUS_IDLE             = 0,
    OTA_STATUS_READY_TO_LOAD,   /* 1 (Local OTA) */
    OTA_STATUS_LOADING,         /* 2 */
    OTA_STATUS_LOAD_COMPLETE,   /* 3 */
    OTA_STATUS_PENDING_COMMIT,  /* 4 */
    OTA_STATUS_COMMITTED,       /* 5 */
    OTA_STATUS_ERROR,           /* 6 */
} OtaIfStatus;

/* OTA_IF Task Commands */
typedef enum {
    OTA_CMD_CHECK_STATE,        /* Check Pending-Commit state (upon OTA_IF_init) */
    OTA_CMD_GET_DOWNLOAD_LINK,  /* Get Download Link */
    OTA_CMD_START_LOADING,      /* Trigger a Download/Upload/Read image */
    OTA_CMD_PROCESS,            /* Process a chunk of loaded data */
    OTA_CMDS_MAX
} OtaIfCmd_e;

/* OTA_IF Load Load Methods */
typedef enum {
#if CLOUD_OTA_SUPPORT
    OTA_TYPE_CLOUD,             /* Cloud OTA - load from a remote HTTP Server */
#endif
#if LOCAL_OTA_SUPPORT
    OTA_TYPE_LOCAL,             /* Local OTA - Upload by a remote HTTP Client */
#endif
#if INTERNAL_UPDATE_SUPPORT
    OTA_TYPE_INTERNAL,          /* Internal update - Read Image from the file system */
#endif
    OTA_TYPES_MAX
} OtaIfType_e;


typedef struct
{

    StartSession_f      fStartSession;  /* Callback (per method) used by the OTA
                                           task for starting the load session */

    GetChunk_f          fGetChunk;      /* Callback (per method) used by the OTA
                                           task for requesting next chunk of data */

    EndSession_f        fEndSession;    /* Callback (per method) to terminate the
                                           OTA session */

} OtaCallbacks_t;


typedef struct
{
    OtaIfCmd_e     msgId;
    union {
        struct {
            ota_getDownloadLink_f   primaryVendorCB;
            ota_getDownloadLink_f   backupVendorCB;
        } getDownloadLink;
        struct {
            OtaIfType_e             type;
            void                    *pParams;
            uint32_t                flags;
        } start;
        struct {
            uint8_t                 *pPayload;
            uint16_t                payloadLen;
        } process;
    } u;
} otaMsg_t;




typedef struct
{
    unsigned int stack_size;
    uint8_t thread_priority;
} otaInitParams_t;




typedef struct
{
    unsigned int        completed;
    unsigned int        warnings;
    unsigned int        errors;
} OtaCounters_t;


/* Parameters (timeout) for the Image Commit watchdog */
#define APPS_WDT_START_KEY          0xAE42DB15

typedef struct
{
    uint8_t     ucActiveImg;
    uint32_t    ulImgStatus;
    uint32_t    ulStartWdtKey;
    uint32_t    ulStartWdtTime;
} sBootInfo_t;


//*****************************************************************************
//                 GLOBAL VARIABLES
//*****************************************************************************


static struct
{
    int                 state;
    uint32_t            flags;
    ota_eventCallback_f fAppCallback;
    uint32_t            localDevType;
    uint8_t             localMacAddress[SL_MAC_ADDR_LEN];
    char                localNetworkSSID[32];
    SlNetCfgIpV4Args_t  localIpInfo;

    workQ_t             hWQ;
    char                *pVersion;
    char                newVersion[VERSION_STR_SIZE+1];
    uint32_t            commitWatchdogTimeout;

    void *              hSession;
    OtaArchive_t        hOtaArchive;

    OtaCounters_t       counters;
    OtaIfType_e         type;
    unsigned int        nImageLen;
    uint32_t            nTotalRead;

    OtaIfStatus         status;
    uint8_t             progressPercent;

    union
    {
        FileServerParams_t  fileServerParams;
        TarFileParams_t     tarFileParams;
        HTTPSRV_IF_params_t httpServerParams;
        uint8_t             buff[OTA_BUFF_SIZE];
    };
} m_ota = { 0 };

//*****************************************************************************
// Static Functions Declarations
//*****************************************************************************

/* OTA Load Engine */
static int32_t WorkQCallback(void *hMsg);
static int CheckMcuImage(char *pFileName);
static int ProcessOta(uint8_t *pBuff, uint16_t bufLen);
static int TriggerLoad(OtaIfType_e loadType, void *pParams, uint32_t flags);

/* HTTP Client (Session) Callbacks */
#if CLOUD_OTA_SUPPORT
static int HTTP_CLIENT_StartSession(void **phClient, uint8_t *pbuff, uint16_t buffLen, void *pParams);
static int HTTP_CLIENT_GetChunk(void *hClient, uint8_t *pbuff, uint16_t buffLen);
static int HTTP_CLIENT_EndSession(void *hClient, int status);
#endif

#if LOCAL_OTA_SUPPORT
/* HTTP Client (Session) Callbacks */
static int HTTP_SERVER_StartSession(void **phClient, uint8_t *pbuff, uint16_t buffLen, void *pParams);
static int HTTP_SERVER_GetChunk(void *hClient, uint8_t *pbuff, uint16_t buffLen);
static int HTTP_SERVER_EndSession(void *hClient, int status);
#endif

#if INTERNAL_UPDATE_SUPPORT
/* HTTP Client (Session) Callbacks */
static int FILE_StartSession(void **phClient, uint8_t *pbuff, uint16_t buffLen, void *pParams);
static int FILE_GetChunk(void *hClient, uint8_t *pbuff, uint16_t buffLen);
static int FILE_EndSession(void *hClient, int status);
#endif

/* Table of callback per Load Type - the order must be compatible with OtaIfType_e */
static const OtaCallbacks_t m_sessionCBs[OTA_TYPES_MAX] =
{
#if CLOUD_OTA_SUPPORT
 {  HTTP_CLIENT_StartSession,   HTTP_CLIENT_GetChunk,   HTTP_CLIENT_EndSession  },
#endif
#if LOCAL_OTA_SUPPORT
 {  HTTP_SERVER_StartSession,   HTTP_SERVER_GetChunk,   HTTP_SERVER_EndSession  },
#endif
#if INTERNAL_UPDATE_SUPPORT
 {  FILE_StartSession,          FILE_GetChunk,          FILE_EndSession         },
#endif
};

//*****************************************************************************
// Static Functions - Helper function
//*****************************************************************************

static void SetStatus(OtaIfStatus status)
{
    m_ota.status = status;
    LOG_DEBUG("OTA_IF STATUS = %d", status);
}



static int CheckMcuImage(char *pFileName)
{
    int rc = 0;
    if(((strstr(pFileName, "mcuimg.bin") != NULL) && IS_FLASH_DEVICE())
            ||
       ((strstr(pFileName, "mcuflashimg.bin") != NULL) && !IS_FLASH_DEVICE()))
    {

        LOG_ERROR("CheckMcuImage:: Wrong Image for the device (%08x, %s)", m_ota.localDevType, pFileName);

        /* Stop the parsing of the archive file */
        rc = -1;
    }
    return rc;
}

static int ConfigureOtaWDT_CC32XX(int32_t TimeoutInSeconds)
{
    // TODO - write only if file doesn't exist (check before writing)
    sBootInfo_t sBootInfo;
    int rc;

    memset(&sBootInfo,0,sizeof(sBootInfo_t));
    sBootInfo.ulStartWdtTime = 40000000*TimeoutInSeconds; /* max 104 seconds */
    sBootInfo.ulStartWdtKey = APPS_WDT_START_KEY;
    rc = FILE_write("/sys/mcubootinfo.bin", sizeof(sBootInfo_t), (uint8_t*)&sBootInfo, NULL, 0);

    if(SL_ERROR_FS_FILE_IS_PENDING_COMMIT == rc)
    {
        /* "/sys/mcubootinfo.bin"  was part of the OTA image */
        LOG_INFO("OtaWatchDog: Watchdog is updated by mcubootinfo.bin from the OTA image");
        rc = 0;
    }
    else if(0 != rc)
    {
        LOG_ERROR("OtaWatchDog: Failed to store the bootinfo file (%d)", rc);
    }
    return rc;
}

static int ReadLocalDeviceInfo()
{
    int rc;
    uint16_t           macAddressLen = SL_MAC_ADDR_LEN;
    SlDeviceVersion_t ver;
    uint8_t configOpt = SL_DEVICE_GENERAL_VERSION;
    uint16_t len = sizeof(SlDeviceVersion_t);
    _u16    dhcpIsOn;

    rc = sl_DeviceGet(SL_DEVICE_GENERAL,&configOpt,&len,(uint8_t *)(&ver));
    if(rc == 0)
    {
        m_ota.localDevType = ver.ChipId & 0xFF;
        /* Update deviceType to 323XX or 3220X */
        if((HWREG(GPRCM_BASE + GPRCM_O_GPRCM_DIEID_READ_REG4) >> 24) & 0x02)
        {
            m_ota.localDevType |= (0x01 << 8);
        }

        rc = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET,NULL,&macAddressLen,(_u8 *)m_ota.localMacAddress);
    }
    if(rc == 0)
    {
       len = sizeof(SlNetCfgIpV4Args_t);
       rc = sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE, &dhcpIsOn, &len, (_u8 *)&m_ota.localIpInfo);
    }
    if(rc == 0)
    {
        SlWlanConnStatusParam_t WlanConnectInfo ;
        _u16 Len = sizeof(SlWlanConnStatusParam_t) ;
        rc = sl_WlanGet(SL_WLAN_CONNECTION_INFO, NULL, &Len, (_u8* )&WlanConnectInfo);
        strncpy(m_ota.localNetworkSSID, (const char *)WlanConnectInfo.ConnectionInfo.StaConnect.SsidName, sizeof(m_ota.localNetworkSSID));
    }
    return rc;
}

//*****************************************************************************
// Static Functions - Load Engine
//*****************************************************************************

static int OTAScheduleCmd(otaMsg_t *pOtaMsg)
{
    int status = 0;

    status = WQ_schedule(m_ota.hWQ, WorkQCallback, pOtaMsg, sizeof(otaMsg_t));

    if(status < 0){
        LOG_ERROR("msg queue send error %d", status);
    }
    return status;
}

static int OTAScheduleProcessCmd(uint8_t *pPayload, uint16_t payloadLen)
{
    otaMsg_t msg;
    msg.msgId = OTA_CMD_PROCESS;
    msg.u.process.pPayload = pPayload;
    msg.u.process.payloadLen = payloadLen;
    return OTAScheduleCmd(&msg);
}

static int OTAScheduleStartCmd(OtaIfType_e type, void *pParams, uint32_t flags)
{
    static otaMsg_t msg;

    msg.msgId = OTA_CMD_START_LOADING;
    msg.u.start.type = type;
    msg.u.start.pParams = pParams;
    msg.u.start.flags = flags;

    return OTAScheduleCmd(&msg);
}

static int TriggerLoad(OtaIfType_e loadType, void *pParams, uint32_t flags)
{
    int16_t rc = -1;
    uint32_t nRead = 0;

    if(m_ota.state != OTA_STATE_READY) {
        LOG_ERROR("library only support 1 active init call");
        return -1;
    }
    m_ota.nTotalRead = 0;

    rc = m_sessionCBs[loadType].fStartSession(&m_ota.hSession, m_ota.buff, OTA_BUFF_SIZE, pParams);
    if(rc >= 0)
    {
        m_ota.nTotalRead = rc;
        m_ota.type = loadType;
        m_ota.flags = flags;
        nRead = rc;
        rc = OtaArchive_init(&m_ota.hOtaArchive);
    }
    if(rc == 0 && nRead > 0)
    {
        rc = OTAScheduleProcessCmd(m_ota.buff, nRead);
    }
    return rc;
}

static int16_t StopLoad(int status)
{
    int16_t rc = m_sessionCBs[m_ota.type].fEndSession(m_ota.hSession, status);
    return rc;
}

int ReportError (otaNotif_e errType, int errCode)
{
    OtaEventParam_u params;
    params.err.errorCode = errCode;
    m_ota.fAppCallback(errType, &params);
    SetStatus(OTA_STATUS_ERROR);
    return 0;
}

static int32_t WorkQCallback(void *hMsg)
{
    otaMsg_t *pMsg = (otaMsg_t*)hMsg;
    int32_t status = 0;

    switch(pMsg->msgId)
    {
        case OTA_CMD_CHECK_STATE:
        {
            LOG_TRACE("OTA_CMD_CHECK_STATE: %d", m_ota.state);
            if(m_ota.state == OTA_STATE_UNINITIALIZED)
            {
                status = OtaArchive_getPendingCommit();
                if(status > 0)
                {
                    SetStatus(OTA_STATUS_PENDING_COMMIT);
                    LOG_DEBUG("...OTA image is PENDING_COMMIT");
                    m_ota.fAppCallback(OTA_NOTIF_IMAGE_PENDING_COMMIT, NULL);
                }
                else
                {
                    LOG_DEBUG("...OTA image is not PENDING_COMMIT");
                }
                m_ota.state = OTA_STATE_READY;
            }
        }
        break;

#if CLOUD_OTA_SUPPORT
        case OTA_CMD_GET_DOWNLOAD_LINK:
        {
            LOG_TRACE("OTA_CMD_GET_DOWNLOAD_LINK: %d", m_ota.state);
            status = pMsg->u.getDownloadLink.primaryVendorCB(&m_ota.fileServerParams);
            if(status < 0 && pMsg->u.getDownloadLink.backupVendorCB)
            {
                LOG_DEBUG("Primary OTA Vendor failed, Trying Backup OTA Vendor....");
                status = pMsg->u.getDownloadLink.backupVendorCB(&m_ota.fileServerParams);
            }
            if(status == 0)
            {
                LOG_DEBUG("Download Link found!");
                OTA_IF_downloadImageByFileURL(&m_ota.fileServerParams, OTA_IF_FLAG_DISABLE_DOWNGRADE_PROTECTION);
            }
            else
            {
                LOG_DEBUG("Download Link Failure!");
                ReportError(OTA_NOTIF_GETLINK_ERROR, status);
            }

        }
        break;
#endif

        case OTA_CMD_START_LOADING:
        {
            LOG_TRACE("OTA_CMD_START_LOADING: %d", m_ota.state);

            status = TriggerLoad(pMsg->u.start.type, pMsg->u.start.pParams, pMsg->u.start.flags);
            if(status == 0)
            {
                m_ota.state = OTA_STATE_IN_PROGRESS;
            }
            else
            {
                ReportError(OTA_NOTIF_DOWNLOAD_ERROR, status);
            }
        }
        break;
        case OTA_CMD_PROCESS:
        {
            LOG_TRACE("OTA_CMD_PROCESS: %d", m_ota.state);
            if(m_ota.state == OTA_STATE_IN_PROGRESS)
            {
                status = ProcessOta(pMsg->u.process.pPayload, pMsg->u.process.payloadLen);
                if(status == 0)
                {
                    int rc = FILE_write("ota.dat", VERSION_STR_SIZE, (uint8_t*)m_ota.newVersion, NULL, (SL_FS_CREATE_FAILSAFE | SL_FS_WRITE_BUNDLE_FILE));
                    if(rc < 0)
                    {
                        LOG_ERROR("Version file (ota.dat) couldn't be written: %d", rc);
                        ReportError(OTA_NOTIF_INSTALL_ERROR, rc);
                    }
                    else
                    {
                        m_ota.fAppCallback(OTA_NOTIF_IMAGE_DOWNLOADED, NULL);
                        SetStatus(OTA_STATUS_LOAD_COMPLETE);
                    }
                }
                else if(status == OTA_RUN_STATUS_CHECK_OLDER_VERSION || (status < 0))
                {
                    ReportError(OTA_NOTIF_DOWNLOAD_ERROR, status);
                }
                else
                {
                    LOG_TRACE("OTA_CMD_PROCESS: Unknown return value (%d)", status);
                    ReportError(OTA_NOTIF_DOWNLOAD_ERROR, -1);
                }

                m_ota.state = OTA_STATE_READY;

            }
        }
        break;

        default:
        {
            LOG_ERROR("WQCalback: Error (Unsupported msgId: %d)", pMsg->msgId);
            status = SL_ERROR_BSD_EINVAL;
        }
    }
    free(pMsg);
    return status;
}

static int ProcessOta(uint8_t *pBuff, uint16_t nRead)
{
    int         rc = 0;
    uint32_t    nTotalProcessed = 0;
    uint16_t    nProcessed = 0;
    uint16_t    nUnprocessed = nRead;
    uint8_t     nProgressBarPercentStep;
    uint8_t     nProgressBarPercentCount;
    uint32_t    nProgressBarStep;
    uint32_t    nProgressBarNext;
    OtaArchive_t *pOtaArchive = &m_ota.hOtaArchive;
    int         otaArchiveStatus = ARCHIVE_STATUS_CONTINUE;
    bool        bEndOfInput = false;

    if(m_ota.pVersion == NULL)
    {
        /* If pVersion is NULL - we need to test the version (timestmp that prefixes tar file)
         * Other wise, The assumption is that the user was responsible for the check and
         * his version will be used (without extra check)
         */
        if( OTA_IF_isNewVersion(pBuff))
        {
            memcpy(m_ota.newVersion, pBuff, OTA_VERSION_LEN);
        }
        else
        {
            LOG_WARNING("ProcessOta: ---- Candidate with old version");
            StopLoad(OTA_RUN_STATUS_CHECK_OLDER_VERSION);
            return OTA_RUN_STATUS_CHECK_OLDER_VERSION;
        }
    }

    /*** Initiate progress-bar params ***/
    if(m_ota.nImageLen)
    {
        nProgressBarPercentStep = (OTA_BUFF_SIZE * 100 / m_ota.nImageLen);
        if(nProgressBarPercentStep < 4)
            nProgressBarPercentStep = 4;
        nProgressBarStep = m_ota.nImageLen * nProgressBarPercentStep / 100;
        nProgressBarNext = nProgressBarStep;
        nProgressBarPercentCount = 0;
    }

    while( rc >= 0 && nUnprocessed &&
            (m_ota.nImageLen == 0 || nTotalProcessed < m_ota.nImageLen) )
    {
        /*** Call OtaArchive with unprocessed bytes ***/
        LOG_DEBUG("ProcessOta:: Ready to process (%d)", nUnprocessed);
        uint16_t byteProcessed;
        otaArchiveStatus = OtaArchive_process(pOtaArchive, &pBuff[nProcessed], (int16_t)nUnprocessed, (int16_t*)&byteProcessed);

        /*** Update processing counters ***/
        nProcessed += byteProcessed;
        nUnprocessed -= byteProcessed;
        nTotalProcessed += byteProcessed;
        LOG_DEBUG("ProcessOta:: OtaArchive status=%d, processed=%d (%d), unprocessed=%d", otaArchiveStatus, byteProcessed, nTotalProcessed, nUnprocessed);

        /*** Update (LOG) progress bar ***/
        if(m_ota.nImageLen)
        {
            if(nTotalProcessed >= nProgressBarNext)
            {
                nProgressBarNext += nProgressBarStep;
                nProgressBarPercentCount += nProgressBarPercentStep;
                m_ota.progressPercent = nProgressBarPercentCount;
                LOG_INFO("ProcessOta: ---- Download file in progress (%02d%%) %d/%d ----", nProgressBarPercentCount, nTotalProcessed, m_ota.nImageLen);
            }
        }
        else
        {
            LOG_INFO("ProcessOta: ---- Download file in progress %d ----", nTotalProcessed);
        }

        if(otaArchiveStatus == ARCHIVE_STATUS_DOWNLOAD_DONE)
        {
            /*** Terminate session upon successful download completion ***/
            nUnprocessed = 0;
            LOG_INFO("ProcessOta: ---- Download file completed");
            rc = StopLoad(0);
            return 0;
        }
        else if(otaArchiveStatus < 0)
        {
            int status;
            rc = otaArchiveStatus;
            /*** Terminate session upon OTA failure ***/
            LOG_ERROR("ProcessOta: ---- OTA failure (%d)", rc);

            nUnprocessed = 0;
            OtaArchive_abort(pOtaArchive);
            status = StopLoad(rc);
            if (status != 0)
                while(1);

            return rc;
        }


        /*** Load next chunk - if  OtaArchive require more bytes,
             or unprocessed count is less then half the buffer (and TotalRead < ImageLen) ***/

        if(     (bEndOfInput == false) &&
                (otaArchiveStatus == ARCHIVE_STATUS_FORCE_READ_MORE) ||
                ( (nUnprocessed < OTA_BUFF_SIZE/2) &&
                  (m_ota.nImageLen == 0 || m_ota.nTotalRead < m_ota.nImageLen)) )

        {
            /* The following assert verifies the OTA_BUFF_SIZE is big enough to address
               OtaArchive minimum requirements */
            if(nUnprocessed >= OTA_BUFF_SIZE)
            {
                while(1) ;
            }

            /*** Get Next Chunk ***/
            if(nProcessed)
            {
                /* First, move unprocessed bytes to buffer start */
                memcpy (m_ota.buff, &pBuff[nProcessed], nUnprocessed);
                /* in case the input buffer was not the m_ota.buff (e.g. when
                 * receiving the buffer from the HTTPSRV request) - at this point we
                 * need to update the processing pointer to m_ota.buff */
                pBuff = m_ota.buff;
                nProcessed = 0;
            }
            /* Now, fill the rest of the buffer (using GetChunk Callback) */
            rc = m_sessionCBs[m_ota.type].fGetChunk(m_ota.hSession, &m_ota.buff[nUnprocessed], OTA_BUFF_SIZE-nUnprocessed);
            if(rc > 0)
            {
                /* Update counters with the actual number of bytes read */
                m_ota.nTotalRead += rc;
                nUnprocessed += rc;
                LOG_DEBUG("ProcessOta:: read=%d (%d)", rc, m_ota.nTotalRead);
            }
            else if(rc == 0)
            {
                /* mark end of input */
                bEndOfInput = true;
                LOG_DEBUG("ProcessOta:: read=0 (%d)", m_ota.nTotalRead);
            }
            else
            {
                LOG_ERROR("ProcessOta: ---- Can't get next chunk (%d)", rc);
                return rc;
            }
        }
        if(otaArchiveStatus == ARCHIVE_STATUS_CONTINUE )
        {
            /*** Verify that MCU image fits the device type ***/
            if(OtaArchive_getStatus(pOtaArchive) == ARCHIVE_STATE_OPEN_FILE)
            {
                rc = CheckMcuImage((char *)m_ota.hOtaArchive.CurrTarObj.pFileName);
                if(rc < 0)
                    return rc;
                else
                    LOG_INFO("ProcessOta: ---- Writing file: %s", (char *)m_ota.hOtaArchive.CurrTarObj.pFileName);
            }
        }
    }
    return rc;
}



//*****************************************************************************
//                 STATIC FUNCTION - Type-Specific Implementation
//*****************************************************************************

#if CLOUD_OTA_SUPPORT
/*
    parse url into domain name and uri_name, example
    https://dl.cloud_storage_provider_1.com/1/view/3ntw3xgkgselev9/Apps/SL_OTA/IlanbSmallFile.txt
*/

/* extract domain name, extract only the domain name between // to the first / */
/* example: https://dl.cloud_storage_provider_1.com */
static char *getHostName(char *pUrl)
{
    char *pBuf, *pHost = NULL;

    pBuf = strstr(pUrl, "//");
    if (pBuf == NULL)
    {
        LOG_DEBUG("ParseUrl: double-slash not found");
    }
    else
    {
        pBuf += 2;
        /* ends with / */
        pBuf = strstr(pBuf, "/");
        if (pBuf == NULL)
        {
            LOG_DEBUG("ParseUrl: slash not found");
        }
        else
        {
            /* Replace '/' with 0 - so HostName can be used without copy */
            /* getResource() will replace this back                      */
            *pBuf = 0;
            pHost = pUrl;
        }
    }
    return pHost;
}

/* extract resource name - it can only be called following a successful getHostName() */
/* example: /1/view/3ntw3xgkgselev9/Apps/SL_OTA/IlanbSmallFile.txt */
static char *getResource(char *pUrl)
{
    char *pBuf;
    int i = 0;

    /* Find the first 0 (end of hostName) */
    while(pUrl[i])
        i++;

    /* replace the the null-terminted char of the HostName with the original '/' (see getHostName) */
    pUrl[i] = '/';

    pBuf = &pUrl[i];

    /* fix for DropBox issue - i.e. getting trimming the '#' at the end of the URL */
    pBuf = strstr(pBuf, "#");
    if(pBuf)
        *pBuf = 0;

    return &pUrl[i];
}


static int HTTP_CLIENT_StartSession(void **phClient, uint8_t *pbuff, uint16_t buffLen, void *pParams)
{
    int16_t rc = 1;

    if(pParams)
    {
        FileServerParams_t *pServerParams = (FileServerParams_t*)pParams;
        char *pHostName = getHostName(pServerParams->URL);
        if(pHostName)
        {
            rc = HTTP_setupConnection(phClient, pHostName,
                                      pServerParams->pPrivateKey, pServerParams->pClientCert,
                                      pServerParams->pRootCa1, pServerParams->pRootCa2, 0);
            if(rc == 0)
            {
                rc = HTTP_setHeaders(*phClient, USER_AGENT, NULL, NULL, HTTPClient_HFIELD_NOT_PERSISTENT);
            }
            if(rc == 0)
            {
                char *pResourcePath = getResource(pServerParams->URL);
                rc = HTTPClient_sendRequest(*phClient,HTTP_METHOD_GET,pResourcePath, NULL,0, 0);
                if(rc == 0 || rc == 200)
                {
                    char len_str[8];
                    uint32_t len = 8;
                    rc = HTTPClient_getHeader(*phClient, HTTPClient_HFIELD_RES_CONTENT_LENGTH, len_str, &len, 0);
                    if(rc == 0)
                        m_ota.nImageLen = atol(len_str);
                }
                else
                {
                    LOG_ERROR("HTTP Get Error = %d", rc);
                }
            }
        }
        while(rc == 0)
        {
            bool bMoreData;
            rc = HTTPClient_readResponseBody(*phClient, (char*)m_ota.buff, OTA_BUFF_SIZE, &bMoreData);
            SetStatus(OTA_STATUS_LOADING);
        }
        if(rc < 0)
        {
            HTTPClient_destroy(*phClient);
        }

    }
    return rc;
}

static int HTTP_CLIENT_GetChunk(void *hClient, uint8_t *pBuff, uint16_t buffLen)
{
    int rc;
    bool bMoreData;
    rc = HTTPClient_readResponseBody(hClient, (char*)pBuff, buffLen, &bMoreData);
    return rc;
}

static int HTTP_CLIENT_EndSession(void *hClient, int status)
{
    int rc = HTTPClient_disconnect(hClient);
    if(rc  == 0)
    {
        rc = HTTPClient_destroy(hClient);
    }
    return rc;
}
#endif // CLOUD_OTA_SUPPORT

#if (LOCAL_OTA_SUPPORT)
static int HttpPostCB_ota(httpHandle_t hRequest, uint8_t *pPayload, uint16_t payloadLen)
{

    m_ota.hSession = hRequest;
    SetStatus(OTA_STATUS_LOADING);
    m_ota.nImageLen = HTTPSRV_IF_getContentLength(hRequest);
    LOG_DEBUG("In HttpPostCB_ota (%d)", m_ota.nImageLen);
    OTAScheduleProcessCmd(pPayload, payloadLen);
    return 0;
}

int HttpGetCB_ota(httpHandle_t hRequest, uint8_t *pPayload, uint16_t payloadLen)
{
    char value[20];
    int rc = 0;

    //LOG_DEBUG("In HttpPostCB_ota (%s)", pPrm->pQueryStr);
    if(0 == HTTPSRV_IF_searchQueryString(hRequest, "version", value))
    {
        uint8_t version[OTA_VERSION_LEN+1];

        LOG_DEBUG("In HttpGetCB_ota (read version)");
        OTA_IF_getCurrentVersion(version);
        version[OTA_VERSION_LEN] = 0;
        LOG_DEBUG(".... found 'version' characteristic (%s)\n\r", version);
        rc = HTTPSRV_IF_setResponseQueryString(hRequest, "version", (char*)version);

    }
    if (0 == HTTPSRV_IF_searchQueryString(hRequest, "status", value))
    {
        char payload[2] = {0};
        LOG_DEBUG("In HttpGetCB_ota (read status)");
        payload[0] = '0' + m_ota.status;
        rc = HTTPSRV_IF_setResponseQueryString(hRequest, "status", (char*)payload);

        if(m_ota.status == OTA_STATUS_ERROR || m_ota.status == OTA_STATUS_COMMITTED)
        {
            SetStatus(OTA_STATUS_IDLE);
        }
    }
    if(rc == 0)
    {
        return HTTPSRV_IF_respond(hRequest, 0, NULL, 0);
    }
    else
    {
        LOG_DEBUG("In HttpGetCB_ota (unknown request)");
        return HTTPSRV_IF_respond(hRequest, -1, NULL, 0);
    }
}

int HttpGetCB_device(httpHandle_t hRequest, uint8_t *pPayload, uint16_t payloadLen)
{
    char value[20];
    LOG_DEBUG("In HttpGetCB_device");
    if(0 == HTTPSRV_IF_searchQueryString(hRequest, "macaddress", value))
    {
        char macAddrStr[6*3];

        sprintf((char *)macAddrStr,
                 "%02x:%02x:%02x:%02x:%02x:%02x",
                 m_ota.localMacAddress[0],
                 m_ota.localMacAddress[1],
                 m_ota.localMacAddress[2],
                 m_ota.localMacAddress[3],
                 m_ota.localMacAddress[4],
                 m_ota.localMacAddress[5]);
        LOG_DEBUG(".... found 'macaddress' characteristic (%s)\n\r", macAddrStr);
        HTTPSRV_IF_setResponseQueryString(hRequest, "macaddress", macAddrStr);
    }
    if(0 == HTTPSRV_IF_searchQueryString(hRequest, "ipaddress", value))
    {
        char ipaddrStr[4*4];
        sprintf((char *)ipaddrStr, "%d.%d.%d.%d",
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 3),
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 2),
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 1),
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 0));
        LOG_DEBUG(".... found 'ipaddress' characteristic (%s)\n\r", ipaddrStr);
        HTTPSRV_IF_setResponseQueryString(hRequest, "ipaddress", ipaddrStr);
    }

    if(0 == HTTPSRV_IF_searchQueryString(hRequest, "ssid", value))
    {
        //  TODO
        LOG_DEBUG(".... found 'ssid' characteristic (%s)\n\r", m_ota.localNetworkSSID);
        HTTPSRV_IF_setResponseQueryString(hRequest, "ssid", m_ota.localNetworkSSID);
    }
    return HTTPSRV_IF_respond(hRequest, 0, NULL, 0);
}



static int HTTP_SERVER_StartSession(void **phClient, uint8_t *pbuff, uint16_t buffLen, void *pParams)
{
    int rc;

    rc = HTTPSRV_IF_registerCallback(SL_NETAPP_REQUEST_HTTP_POST, "/ota", HttpPostCB_ota);
    if(rc == 0)
    {
        LOG_INFO("LOCAL OTA enabled on IP=%d.%d.%d.%d (Network: %s)",
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 3),
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 2),
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 1),
                 (int)SL_IPV4_BYTE(m_ota.localIpInfo.Ip, 0),
                 m_ota.localNetworkSSID);
    }
    SetStatus(OTA_STATUS_READY_TO_LOAD);
    return rc;
}

static int HTTP_SERVER_GetChunk(void *hRequest, uint8_t *pbuff, uint16_t readLen)
{
    int rc;
    rc = HTTPSRV_IF_receive(hRequest, pbuff, &readLen);
    if(rc == 0)
    {
        rc = readLen;
    }
    return rc;
}

static int HTTP_SERVER_EndSession(void *hRequest, int status)
{
    HTTPSRV_IF_unregisterCallback(SL_NETAPP_REQUEST_HTTP_POST, "/ota");
    if(0 == status)
    {
        status = HTTPSRV_IF_respond(hRequest, 0, NULL, 0);
    }
    if(0 != status)
    {
        status = HTTPSRV_IF_respond(hRequest, -1, NULL, 0);
    }
    return status;
}

#endif // LOCAL_OTA_SUPPORT

#if  (INTERNAL_UPDATE_SUPPORT)
static int FILE_StartSession(void **phClient, uint8_t *pbuff, uint16_t buffLen, void *pFileParams)
{
    int rc = -1;
    SlFsFileInfo_t fileInfo;
    TarFileParams_t *pTarFile = (TarFileParams_t*)pFileParams;

    // Copy Operational Image (if exist)
    rc = sl_FsGetInfo((_u8*)pTarFile->pPath, pTarFile->token, &fileInfo);
    if(rc < 0)
    {
        LOG_ERROR("OTA File (%s) - Can't be found !!!", pTarFile->pPath);
    }
    else
    {
        m_ota.nImageLen = fileInfo.Len;
       rc = sl_FsOpen((uint8_t *)pTarFile->pPath, SL_FS_READ, (unsigned long*)&pTarFile->token);
    }
    if(rc >= 0)
    {
        *phClient = (void *)rc;
        SetStatus(OTA_STATUS_LOADING);
        rc = FILE_GetChunk(*phClient, m_ota.buff, sizeof(m_ota.buff));
    }
    return rc;
}


static int FILE_GetChunk(void *hFd, uint8_t *pBuff, uint16_t buffLen)
{
    int fd = (int)hFd;
    int rc;

    rc = sl_FsRead( fd, m_ota.nTotalRead, pBuff, buffLen);
    return rc;
}

static int FILE_EndSession(void *hFd, int status)
{
    int fd = (int)hFd;
    return sl_FsClose(fd, 0, 0, 0);
}
#endif // INTERNAL_UPDATE_SUPPORT

//*****************************************************************************
//                 EXTERNAL API METHODS
//*****************************************************************************
int OTA_IF_init(HTTPSRV_IF_params_t *pLocalHttpServerParams, ota_eventCallback_f fOtaEventHndlr, uint32_t otaWatchdogTimeout, workQ_t hWQ)
{
    int16_t rc = 0;
    otaMsg_t msg;
    uint8_t otaVersion[VERSION_STR_SIZE] = {0};

    if(fOtaEventHndlr == NULL)
    {
        LOG_ERROR("Callback must be provided");
        return -1;
    }
    if(m_ota.state != OTA_STATE_UNINITIALIZED) {
        LOG_ERROR("library only support 1 active init call");
        return -1;
    }
    // Initiate OTA state and context
    m_ota.fAppCallback = fOtaEventHndlr;
    m_ota.commitWatchdogTimeout = otaWatchdogTimeout;
    if(hWQ) {
        m_ota.hWQ = hWQ;
    }
    else {
        m_ota.hWQ = WQ_create(OTA_THREAD_PRIORITY, OTA_THREAD_STACK_SIZE, 16, "OTA_IF");
    }
    if(m_ota.hWQ == NULL)
    {
        LOG_ERROR("Can't create WQ for the module");
        return -1;
    }

    ReadLocalDeviceInfo();

    // Prepare OTA Version
    rc = OTA_IF_getCurrentVersion(otaVersion);
    if(rc != 0)
    {
        LOG_ERROR("Can't read version (%d)", rc);
        return -1;
    }
    LOG_INFO("OTA_IF_INIT:: Existing OTA Version = %.*s", OTA_VERSION_LEN, otaVersion);

#if LOCAL_OTA_SUPPORT
    rc = HTTPSRV_IF_init(m_ota.hWQ);
    if(rc == 0)
        rc = HTTPSRV_IF_config(pLocalHttpServerParams);
    if(rc == 0)
        rc = HTTPSRV_IF_registerCallback(SL_NETAPP_REQUEST_HTTP_GET, "/ota", HttpGetCB_ota);
    if(rc == 0)
        rc = HTTPSRV_IF_registerCallback(SL_NETAPP_REQUEST_HTTP_GET, "/device", HttpGetCB_device);
    if(rc != 0)
    {
        LOG_ERROR("Can't init HTTPSRV_IF (%d)", rc);
        return -1;
    }
#endif

    msg.msgId = OTA_CMD_CHECK_STATE;
    rc =  OTAScheduleCmd(&msg);
    return rc;
}

#if CLOUD_OTA_SUPPORT
int OTA_IF_downloadImageByFileURL(FileServerParams_t *pFileServerParams, uint32_t flags)
{
    m_ota.pVersion = NULL;
    m_ota.fileServerParams = *pFileServerParams;
    if(pFileServerParams->pVersion)
    {
        memcpy(m_ota.newVersion, pFileServerParams->pVersion, OTA_VERSION_LEN);
        m_ota.pVersion = m_ota.newVersion;
    }
    return OTAScheduleStartCmd(OTA_TYPE_CLOUD, &m_ota.fileServerParams, flags);
}

int OTA_IF_downloadImageByCloudVendor(ota_getDownloadLink_f fPrimaryVendor, ota_getDownloadLink_f fBackupVendor, uint32_t flags)
{
    static otaMsg_t msg;

    msg.msgId = OTA_CMD_GET_DOWNLOAD_LINK;
    msg.u.getDownloadLink.primaryVendorCB = fPrimaryVendor;
    msg.u.getDownloadLink.backupVendorCB = fBackupVendor;

    return OTAScheduleCmd(&msg);
}
#endif

#if (LOCAL_OTA_SUPPORT)
int OTA_IF_uploadImage(uint32_t flags)
{
    return OTAScheduleStartCmd(OTA_TYPE_LOCAL, NULL, flags);
}
#endif

#if  (INTERNAL_UPDATE_SUPPORT)
int OTA_IF_readImage(TarFileParams_t *pFileParams, uint32_t flags)
{
    m_ota.tarFileParams = *pFileParams;
    return OTAScheduleStartCmd(OTA_TYPE_INTERNAL, &m_ota.tarFileParams, flags);
}
#endif


int OTA_IF_install()
{
    int rc = ConfigureOtaWDT_CC32XX(m_ota.commitWatchdogTimeout);

    if(rc == 0)
    {
        sl_Stop(200);

        /* Reset the MCU in order to test the bundle */
        LOG_INFO("OTA_IF_install: reseting the platform...\n\n\n\r");
        MAP_PRCMHibernateCycleTrigger();

        /* if we reach here, the platform does not support self reset */
        /* reset the NWP in order to rollback to the old image */
        LOG_ERROR("OTA_IF_install: platform does not support self reset");
        LOG_ERROR("                reset the NWP to rollback to the old image\n");
        rc = -1;
    }

    return rc;
}

int  OTA_IF_commit()
{
    int rc =  OtaArchive_commit();
    if (rc == 0)
    {
        SetStatus(OTA_STATUS_COMMITTED);
        LOG_INFO("OTA_IF_commit: OTA succeeded - new image is committed!");
        /* Stop the commit WDT */
        PowerCC32XX_reset(PowerCC32XX_PERIPH_WDT);
    }
    else
    {
        LOG_ERROR("OTA_IF_commit: OTA_set ERROR on EXTLIB_OTA_SET_OPT_IMAGE_COMMIT, Status = %d", rc);
    }
    return rc;
}

int  OTA_IF_rollback()
{
    int rc =  OtaArchive_rollback();
    if (rc == 0)
    {
        SetStatus(OTA_STATUS_ERROR);
        LOG_INFO("OTA_IF_rollback: New image failed - reverting to old image!");
        /* Stop the commit WDT */
        PowerCC32XX_reset(PowerCC32XX_PERIPH_WDT);
        /* Reset Device to revert to old image */
        MAP_PRCMHibernateCycleTrigger();
    }
    else
    {
        LOG_ERROR("OTA_IF_commit: OTA_set ERROR on EXTLIB_OTA_SET_OPT_IMAGE_COMMIT, Status = %d", rc);
    }
    return rc;
}

int OTA_IF_getCurrentVersion(uint8_t *pVersion)
{
    int16_t rc = SL_ERROR_BSD_EINVAL;

    if(pVersion)
    {
        rc = FILE_read((int8_t*)"ota.dat", OTA_VERSION_LEN, pVersion, 0);
        if(rc < 0)
        {
            memset(pVersion, '0', OTA_VERSION_LEN);
            rc = FILE_write("ota.dat", OTA_VERSION_LEN, (uint8_t*)pVersion, NULL, SL_FS_CREATE_FAILSAFE);
            if (rc < 0)
                while(1)
                    ;
        }
    }
    LOG_ERROR_IF_NEG(rc, "FILE_read failed");
    return rc;
}


bool OTA_IF_isNewVersion(uint8_t *pCandidateVersion)
{
    bool bIsNewer;
    uint8_t currVersion[OTA_VERSION_LEN];
    int cmpResult = 1;

    OTA_IF_getCurrentVersion(currVersion);
#if !OTA_IF_FLAG_DISABLE_DOWNGRADE_PROTECTION
    cmpResult = memcmp(pCandidateVersion, currVersion, OTA_VERSION_LEN);
#endif

    if(cmpResult > 0)
    {
        LOG_DEBUG("candidate version: (%.*s) is newer than current version: (%.*s)", OTA_VERSION_LEN, pCandidateVersion, OTA_VERSION_LEN, currVersion);
        bIsNewer = true;
    }
    else
    {
        LOG_DEBUG("candidate version: (%.*s) is older than current version: (%.*s)", OTA_VERSION_LEN, pCandidateVersion, OTA_VERSION_LEN, currVersion);
        bIsNewer = false;
    }
    return bIsNewer;
}

#endif
//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
