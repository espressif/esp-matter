/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __SRTM_CONFIG_H__
#define __SRTM_CONFIG_H__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/*!
 * @addtogroup srtm
 * @{
 */

/*******************************************************************************
 * Definitions, version for initial SRTM on RT600
 ******************************************************************************/
/*! @brief Defines SRTM major version */
#define SRTM_VERSION_MAJOR (0x01U)
/*! @brief Defines SRTM minor version */
#define SRTM_VERSION_MINOR (0x02U)
/*! @brief Defines SRTM bugfix version */
#define SRTM_VERSION_BUGFIX (0x00U)

/*! @brief SRTM version definition */
#define SRTM_MAKE_VERSION(major, minor, bugfix) (((major) << 16) | ((minor) << 8) | (bugfix))

/* There is no specific packing definition here for cross-compiler compatibility*/
/* There is no specific DEBUG MESSAGE Function here */

/**
* @brief Timeout definition: infinite wait that never timeout
*/
#define SRTM_WAIT_FOR_EVER       (0xFFFFFFFFU)

/**
* @brief Timeout definition: no wait that return immediately
*/
#define SRTM_NO_WAIT             (0x0U)

/*! @brief SRTM error code */
typedef enum _srtm_status
{
    SRTM_Status_Success = 0x00U,  /*!< Success */
    SRTM_Status_Error,            /*!< Failed */

    SRTM_Status_InvalidParameter, /*!< Invalid parameter */
    SRTM_Status_InvalidMessage,   /*!< Invalid message */
    SRTM_Status_InvalidState,     /*!< Operate in invalid state */
    SRTM_Status_OutOfMemory,      /*!< Memory allocation failed */
    SRTM_Status_Timeout,          /*!< Timeout when waiting for an event */
    SRTM_Status_ListAddFailed,    /*!< Cannot add to list as node already in another list */
    SRTM_Status_ListRemoveFailed, /*!< Cannot remove from list as node not in list */

    SRTM_Status_TransferTimeout,  /*!< Transfer timeout */
    SRTM_Status_TransferNotAvail, /*!< Transfer failed due to peer core not ready */
    SRTM_Status_TransferFailed,   /*!< Transfer failed due to communication failure */

    SRTM_Status_ServiceNotFound,  /*!< Cannot find service for a request/notification */
    SRTM_Status_ServiceVerMismatch, /*!< Service version cannot support the request/notification */
} srtm_status_t;

/**
* @brief SRTM message type fields
*/
typedef enum _srtm_message_type
{
    SRTM_MessageTypeRequest = 0x00U,   /*!< Request message */
    SRTM_MessageTypeResponse,          /*!< Response message for certain Request */
    SRTM_MessageTypeNotification,      /*!< Notification message that doesn't require response */
    SRTM_MessageTypeCommLast,          /*!< Last value of communication message */

    SRTM_MessageTypeProcedure = 0x40,  /*!< Local procedure */
    SRTM_MessageTypeRawData = 0x41,    /*!< Raw data message */
} srtm_message_type_t;

/**
* @brief SRTM message direction fields
*/
typedef enum _srtm_message_direct
{
    SRTM_MessageDirectNone = 0x00U, /*!< Local procedure message has no direction */
    SRTM_MessageDirectRx,           /*!< Received message */
    SRTM_MessageDirectTx,           /*!< Transfer message */
} srtm_message_direct_t;

/**
* @brief SRTM communication packet head
* Do NOT use any typedef enums for any shared structure, the size will be different cross different platforms!
* ONLY use basic data types for consistant structure size!
*/
typedef struct _srtm_packet_head
{
    unsigned char category;
    unsigned char majorVersion;
    unsigned char minorVersion;
    unsigned char type;
    unsigned char command;
    unsigned char priority;
    unsigned char reserved[4U];
} srtm_packet_head_t;

/**
* @brief SRTM list fields
*/
typedef struct _srtm_list
{
    struct _srtm_list *prev;  /*!< previous list node */
    struct _srtm_list *next;  /*!< next list node */
} srtm_list_t;

/**
* @brief SRTM message structure
* Do NOT use any typedef enums for any shared structure, the size will be different cross different platforms!
* ONLY use basic data types for consistant structure size!
*/
#define SRTM_CMD_PARAMS_MAX 32
typedef struct _srtm_message
{
    srtm_list_t node;                         /*!< SRTM message list node to link to a list */
    srtm_packet_head_t head;                  /*!< SRTM raw data, including header and payload for CommMessage */
    unsigned int error;                       /*!< SRTM message error status */
    unsigned int param[SRTM_CMD_PARAMS_MAX];                    /*!< SRTM user defined message params */
} srtm_message;

/**
* @brief SRTM command fields
*/
typedef enum _srtm_rt600_command
{
    SRTM_Command_ECHO = 0x00U,   /*!< ECHO */
    SRTM_Command_MP3,            /*!< MP3 decoder */
    SRTM_Command_AAC,            /*!< AAC+ decoder */
    SRTM_Command_MIX,            /*!< Mix of two channels */
    SRTM_Command_SRC,            /*!< Sampling rate converter */
    SRTM_Command_GAIN,           /*!< PCM Gain control */
    SRTM_Command_REC,           /*!< PCM Gain control */

    SRTM_Command_SYST,           /*!< For System Test ONLY */
    SRTM_Command_MAX,            /*!< Request message */
} srtm_command_t;

#define SRTM_CMD_STRLEN_MAX 8
static const char srtm_command_alias[SRTM_Command_MAX][SRTM_CMD_STRLEN_MAX] = {
    "echo",  /*MSG command text has to match above enums in order*/
    "mp3",
    "aac",
    "mix",
    "src",
    "gain",
    "rec",
    "syst"};

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @brief Get SRTM version.
 *
 * @return SRTM version.
 */
static inline uint32_t SRTM_GetVersion(void)
{
    return SRTM_MAKE_VERSION(SRTM_VERSION_MAJOR, SRTM_VERSION_MINOR, SRTM_VERSION_BUGFIX);
}

/*! @} */

#endif /* __SRTM_CONFIG_H__ */
