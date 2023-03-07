/******************************************************************************

 @file  oad.h

 @brief This file contains OAD Profile header file.

 Group: WCS, BTS
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
/** ============================================================================
 *  @defgroup OAD_PROFILE OAD BLE Profile
 *  @brief BLE Profile implementation of the TI over the air download  solution.
 *         This file is responsible for processing data from the BLE-Stack,
 *         passing this data to the application, and processing queued OAD
 *         events as well as the OAD state machine
 *
 *  @{
 *
 *  # Header Include #
 *  The OAD Profile header should be included in an OAD enabled application
 *  as follows:
 *  @code
 *  #include <profiles/oad/cc26xx/oad.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  This module is intended to handle OAD related functionality for the
 *  application. At a high level it is responsible for receiving and parsing
 *  OAD profile data and commands. This includes:
 *      - Receiving image blocks and commands
 *      - Executing commands and writing blocks to flash
 *      - Reporting status to both local application and peer device
 *      - Metadata management
 *      - Checking the received image for integrity (CRC, etc)
 *
 * ## General Behavior #
 * For code examples, see the use cases below.
 *
 *  ### Initialzation
 *  - The application must plug a callback function that allows the OAD profile
 *    to queue an event or packet up for processing in the app thread context
 *  @code
 *  static oadTargetCBs_t MyApp_oadCBs =
 *  {
 *      MyApp_processOadWriteCB // Write Callback.
 *  };
 *  @endcode
 *  - Inside the @ref oadWriteCB_t, the application should do the following:
 *    @code Event_post(appEvent, event); @endcode
 *
 *  - The application thread should additionally pend on the following OAD
 *    events
 *      - @ref OAD_QUEUE_EVT
 *      - @ref OAD_DL_COMPLETE_EVT
 *  - In the _init function of the a BLE-Stack enabled application thread,
 *    Do the following calls:
 *  @code
 *  // Open the OAD module and add the OAD service to the application
 *  if(OAD_SUCCESS != OAD_open(OAD_DEFAULT_INACTIVITY_TIME))
 *  {
 *      // Display or log the error
 *  }
 *  else
 *  {
 *      // Register the OAD callback with the application
 *      OAD_register(&MyApp_oadCBs);
 *  }
 *  @endcode
 *
 *  ### Useage
 *  After plugging the callbacks shown above, and initializing the OAD module
 *  the application will begin receive callbacks related to @ref OAD_QUEUE_EVT
 *  and @ref OAD_DL_COMPLETE_EVT.
 *
 *
 *  #### Processing OAD_QUEUE_EVT
 *  Inside the application task function, the queue event should be processed:
 *  @code
 *  // OAD events
 *  if(events & OAD_QUEUE_EVT)
 *  {
 *      // Process the OAD Message Queue
 *      uint8_t status = OAD_processQueue();
 *
 *      // If the OAD state machine encountered an error, print it
 *      if(status == OAD_DL_COMPLETE)
 *      {
 *          //Display or print the message
 *      }
 *      else if(status == OAD_IMG_ID_TIMEOUT)
 *      {
 *          // Display the error and terminate the link
 *          MAP_GAP_TerminateLinkReq(OAD_getactiveCxnHandle(),
 *                                      HCI_DISCONNECT_REMOTE_USER_TERM);
 *      }
 *      else if(status != OAD_SUCCESS)
 *      {
 *          // Display the error
 *      }
 *  }
 *  @endcode
 *
 *  #### Processing OAD_DL_COMPLETE_EVT
 *  At this time, the OAD is entirely complete and the system should be reset.
 *  The reset behavior is defined by the application.
 *  \warning The application should ensure the BLE-Stack has no pending items
 *           in its transmit buffer (i.e. final OAD response messages) before
 *           rebooting
 *
 *  @file  oad.h
 *
 *  @brief This file contains OAD Profile header file.
 */

#ifndef OAD_H
#define OAD_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include <ti/sysbios/knl/Queue.h>

#include <common/cc26xx/oad/oad_image_header.h>

/*********************************************************************
 * CONSTANTS
 */

/*!
 * This define controls whether the OAD profile requires BLE security
 * By default, this is off.
 */
#ifdef OAD_BLE_SECURITY
    #define OAD_WRITE_PERMIT     GATT_PERMIT_AUTHEN_WRITE
#else
    #define OAD_WRITE_PERMIT     GATT_PERMIT_WRITE
#endif //OAD_SECURITY

/*!
 * Number of failed image IDs are allowed before OAD terminates the cxn
 */
#define OAD_IMG_ID_RETRIES                  3

// Events the OAD process will post to the app

/*!
 * @defgroup OAD_EVTS OAD Events
 * @{
 */
/*!
 * The queue event is posted whenever there is a new message in the OAD message
 * queue from the BLE-Stack. Messages in the OAD queue must be processed in the
 * application context
 */
#define OAD_QUEUE_EVT                       Event_Id_01

/*!
 * The download complete signifies that the OAD has completed and the candidate
 * image has passed verification. When this event is received, the application
 * should reset the device so that the BIM can perform further processing.
 */
#define OAD_DL_COMPLETE_EVT                 Event_Id_02

/*!
 * The out of memory event signifies that the OAD has run out of memory and 
 * could not allocate memory for a message to notify the application 
 * (for example in the case of a timeout).
 */
#define OAD_OUT_OF_MEM_EVT                  Event_Id_03
/** @} End OAD_EVTS */

/*!
 * @defgroup OAD_SVC_UUIDS OAD Service UUIDs
 * @{
 * OAD Service UUIDs:
 * Warning! The image count and image status chars are deprecated
 * their functionality has been moved to the control point
 */

/*!
 * 16-bit root of the OAD Service UUID.
 * \note the service and chars will use the 128-bit TI base UUID
 */
#define OAD_SERVICE_UUID                    0xFFC0

/*!
 * 16-bit root of the OAD Image Identify Characteristic UUID.
 */
#define OAD_IMG_IDENTIFY_UUID               0xFFC1

/*!
 * 16-bit root of the OAD Image Block Characteristic UUID.
 */
#define OAD_IMG_BLOCK_UUID                  0xFFC2

/*!
 * 16-bit root of the OAD Image Count Characteristic UUID.
 * \warning This characteristic is deprecated
 */
#define OAD_IMG_COUNT_UUID                  0xFFC3

/*!
 * 16-bit root of the OAD Image Status Characteristic UUID.
 * \warning This characteristic is deprecated. Status info is now reported
 *          through the extended control characteristic.
 */
#define OAD_IMG_STATUS_UUID                 0xFFC4

/*!
 * 16-bit root of the OAD Extended Control Point Characteristic UUID.
 */
#define OAD_EXT_CTRL_UUID                   0xFFC5

/** @} End OAD_SVC_UUIDS */

/*!
 * @defgroup OAD_SVC_INDEXES OAD Service Indexes
 * @{
 */

/*!
 * Index of the image identify characteristic within the OAD service array
 */
#define OAD_IDX_IMG_IDENTIFY                0

/*!
 * Index of the image block characteristic within the OAD service array
 */
#define OAD_IDX_IMG_BLOCK                   1

/*!
 * Index of the extended control characteristic within the OAD service array
 */
#define OAD_IDX_EXT_CTRL                    2

/** @} End OAD_SVC_INDEXES */

/*!
 * The ATT header overhead required to send a GATT_notifiation()
 * This is used in calculating acceptable block sizes
 */
#define OAD_ATT_OVERHEAD                    3

/*!
 * The size of the OAD block number in a block write or block request
 */
#define OAD_BLK_NUM_HDR_SZ                  4

/*!
 * Minimum OAD block size
 * OAD Block size can range from [OAD_DEFAULT_BLOCK_SIZE,OAD_MAX_BLOCK_SIZE]
 * \note OAD block size must be 4 Octet aligned
 */
#define OAD_DEFAULT_BLOCK_SIZE              20

/*!
 * Maximum OAD block size
 * OAD Block size can range from [OAD_DEFAULT_BLOCK_SIZE,OAD_MAX_BLOCK_SIZE]
 * \note OAD block size must be 4 Octet aligned
 */
#define OAD_MAX_BLOCK_SIZE                  244

/*!
 * Size of the payload in an image identify response.
 */
#define OAD_IMAGE_ID_RSP_LEN                0x01

/*!
 * @defgroup OAD_EXT_CTRL_OPCODES OAD Extended Control Command op codes
 * @{
 */

/*!
 * Get Block Size external control command op-code
 * This command is used by a peer to determine what is the largest block
 * size the target can support
 */
#define OAD_EXT_CTRL_GET_BLK_SZ             0x01

/*!
 * Get image count external control command op-code
 * This command is used by a peer to instruct the target that multiple images
 * are to be downloaded during this session
 */
#define OAD_EXT_CTRL_IMG_CNT                0x02

/*!
 * Start OAD external control command op-code
 * This command is used to tell the target device that the configuration stage
 * has completed and it is time to start sending block requests
 */
#define OAD_EXT_CTRL_START_OAD              0x03

/*!
 * Enable image external control command op-code
 * This command is used to enable an image after download, instructing the
 * target to prepare the image to run and then reboot
 */
#define OAD_EXT_CTRL_ENABLE_IMG             0x04

/*!
 * Cancel OAD external control command op-code
 * This command is used to cancel the OAD process
 */
#define OAD_EXT_CTRL_CANCEL_OAD             0x05

/*!
 * Disable block notification external control command op-code
 * This command is used to disable the image block request notifications
 */
#define OAD_EXT_CTRL_DISABLE_BLK_NOTIF      0x06

/*!
 * Get software version external control command op-code
 * This command is used to query the OAD target device for its software version
 */
#define OAD_EXT_CTRL_GET_SW_VER             0x07

/*!
 * Get status external control command op-code
 * This command is used to query the status of the OAD process.
 */
#define OAD_EXT_CTRL_GET_IMG_STAT           0x08

/*!
 * Get profile version external control command op-code
 * This command is used to query the version of the OAD profile
 */
#define OAD_EXT_CTRL_GET_PROF_VER           0x09

/*!
 * Get device type external control command op-code
 * This command is used to query type of the device the profile is running on
 */
#define OAD_EXT_CTRL_GET_DEV_TYPE           0x10

/*!
 * Get image info external control command op-code
 * This command is used to get the image info structure corresponding to the
 * the image asked for
 */
#define OAD_EXT_CTRL_GET_IMG_INFO           0x11

/*!
 * Send block request external control command op-code
 * This command is used to send a block request notification to the peer device
 */
#define OAD_EXT_CTRL_BLK_RSP_NOTIF          0x12

/*!
 * Erase bonds external control command op-code
 * This command is used to erase all BLE bonding info on the device
 */
#define OAD_EXT_CTRL_ERASE_BONDS            0x13

/** @} End OAD_EXT_CTRL_OPCODES */


/*!
 * Error code returned when an external control command is received with an
 * invalid opcode.
 */
#define OAD_EXT_CTRL_CMD_NOT_SUPPORTED      0xFF

/*!
 * There are two flavors of the get image info command:
 * - A short get info command: returns the image info for the image on the chip
 * - A long get info command: return an array of image info structures from
 *   external flash
 */
#define OAD_IMG_INFO_ONCHIP                 0x01

/*!
 * Default OAD inactivity timeout, the OAD state machine will cancel OAD
 * if no activity is observed during this time.
 *
 * The inactivity time is configurable via a parameter to @ref OAD_open
 */
#define OAD_DEFAULT_INACTIVITY_TIME         10000

/*!
 * Minimum OAD inactivity timeout, the OAD state machine will cancel OAD
 * if no activity is observed during this time.
 */
#define OAD_MIN_INACTIVITY_TIME             4000

/*!
 * Number of characteristics in the OAD service
 */
#define OAD_CHAR_CNT                        3

/*!
 * Number of CCCDs in the OAD service
 */
#define OAD_CCCD_CNT                        OAD_CHAR_CNT


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

/** @} End OAD_STATUS_CODES */

/// @cond NODOC
#if defined(DeviceFamily_CC26X2) || defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC26X1)
#define BIM_START                           0x56000
#elif defined(DeviceFamily_CC26X2X7) || defined(DeviceFamily_CC13X2X7)
#define BIM_START                           0xAE000
#else
#define BIM_START                           0x1F000
#endif
/// @endcond // NODOC

/*!
 * Invalid image page (outside of device flash)
 */
#define OAD_IMG_PG_INVALID                  0xFF

//!< OAD Security Types
#define OAD_SEC_ECDSA_P256                  0x01  //!< OAD Security Type for ECDSA P256
#define OAD_SEC_AES_128_CBC                 0x02  //!< OAD Security Type for AES 128

/*!
 * Maximum number of external flash image header entries
 * \note this only applies to off-chip OAD
 */
#define OAD_EFL_MAX_META                    4

/*********************************************************************
 * MACROS
 */

/*!
 * Macro to parse op-code from external control command
 */
#define EXT_CTRL_OP_CODE(pData)(pData[0])

/*********************************************************************
 * TYPEDEFS
 */

/*!
 * Image identify Payload
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   imgID[8];         //!< User-defined Image Identification bytes
    uint8_t   bimVer;           //!< BIM version
    uint8_t   metaVer;          //!< Metadata version
    uint8_t   imgCpStat;        //!< Image copy status bytes
    uint8_t   crcStat;          //!< CRC status
    uint8_t   imgType;          //!< Image Type
    uint8_t   imgNo;            //!< Image number of 'image type'
    uint32_t  len;              //!< Image length in octets
    uint8_t   softVer[4];       //!< Software version of the image
} imgIdentifyPld_t;

/*!
 * Block request payload
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   cmdID;            //!< External control op-code
    uint8_t   prevBlkStat;      //!< Status of previous block write
    uint32_t  requestedBlk;     //!< Requested block number
}blockReqPld_t;

/*!
 * Image info structure
 * This structure is a subset of the image info field in @ref imgFixedHdr_t
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   imgCpStat;        //!< Image copy flag
    uint8_t   crcStat;          //!< Image CRC status flag
    uint8_t   imgType;          //!< Image type flag
    uint8_t   imgNo;            //!< Image number flag
} imageInfo_t;

PACKED_TYPEDEF_STRUCT
{
    uint8_t       securityVersion;
    uint32_t      timeStamp;
}securityHdr_t;

/*!
 * Event internal to the OAD module, use in state machine processing
 */
typedef enum
{
    OAD_WRITE_IDENTIFY_REQ,     //!< Image identify received
    OAD_WRITE_BLOCK_REQ,        //!< Image block request/response event
    OAD_IMAGE_COMPLETE,         //!< OAD complete
    OAD_EXT_CTRL_WRITE_CMD,     //!< External Control Command received
    OAD_TIMEOUT                 //!< OAD peer timed out
}oadEvent_e;

/*!
 * Stores information related to OAD write event
 */
typedef struct
{
    Queue_Elem _elem;           //!< New queue element to contain write evt info
    oadEvent_e  event;          //!< Event that occurred
    uint16_t connHandle;        //!< Connection event was received on
    uint16_t len;               //!< Length of data received
    uint16_t offset;            //!< GATT offset into blob
    uint8_t  *pData;            //!< Pointer to data received
} oadTargetWrite_t;

/*!
 * OAD state types
 */
typedef enum
{
    OAD_IDLE,                   //!< OAD module is not performing any action
    OAD_VALIDATE_IMG,           //!< OAD module is checking an image
    OAD_CONFIG,                 //!< OAD module is configuring for a download
    OAD_DOWNLOAD,               //!< OAD module is receiving an image
    OAD_CHECK_DL,               //!< OAD module is validating an received image
    OAD_COMPLETE                //!< OAD module has completed a download
}oadState_e;


/*********************************************************************
 * Ext Control Command Payloads
 */

/*!
 * A generic external control response command
 * Most commands simply return a status, this will cover those commands
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   cmdID;            //!< Ext Ctrl Op-code
    uint8_t   status;           //!< Status of command
} genericExtCtrlRsp_t;

/*!
 * Response to a @ref OAD_EXT_CTRL_GET_PROF_VER command
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   cmdID;            //!< Ext Ctrl Op-code
    uint8_t   profVer;          //!< OAD profile version
} profileVerRsp_t;

/*!
 * Response to a @ref OAD_EXT_CTRL_GET_BLK_SZ command
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t     cmdID;          //!< Ext Ctrl Op-code
    uint16_t    oadBlkSz;       //!< OAD block size
} blockSizeRspPld_t;

/*!
 * Response to a @ref OAD_EXT_CTRL_GET_SW_VER command
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t       cmdID;                    //!< Ext Ctrl Op-code
    uint8_t       swVer[OAD_SW_VER_LEN];    //!< App and stack software version
} swVersionPld_t;

/*!
 * Response to a @ref OAD_EXT_CTRL_GET_DEV_TYPE command
 *
 * For information on how to interpret the fields below, see the ChipInfo
 * module within the device's driverlib documentation
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   cmdID;            //!< Ext Ctrl Op-code
    uint8_t   chipType;         //!< Chip Type Field from ChipInfo module
    uint8_t   chipFamily;       //!< Chip Family Field from ChipInfo module
    uint8_t   hardwareRev;      //!< Hardware revision Field from ChipInfo module
    uint8_t   rsvd;             //!< RFU
} deviceTypeRspPld_t;

/*!
 * Response to a @ref OAD_EXT_CTRL_GET_IMG_INFO command
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t   cmdID;            //!< Ext Ctrl Op-code
    uint8_t   numImages;        //!< Number of images
    uint8_t   imgCpStat;        //!< Image copy status
    uint8_t   crcStat;          //!< Image CRC status
    uint8_t   imgType;          //!< Image Type
    uint8_t   imgNo;            //!< Image Number
} imageInfoRspPld_t;

/*!
 * The extended payload of an @ref OAD_EXT_CTRL_ENABLE_IMG command
 * \note This is the extended version of the command, there is a short version
 *       with just the op-code that will enable the last downloaded image.
 */
PACKED_TYPEDEF_STRUCT
{
    uint8_t     cmdID;          //!< Ext Ctrl Op-code
    uint8_t     imgType;        //!< Image Type
    uint8_t     imgNo;          //!< Image Number
    uint16_t    techType;       //!< Wireless technology type
}extImgEnableReq_t;

#ifdef DMM_OAD
typedef struct
{
    uint32_t totBlocks;
    uint32_t currBlock;
}imgProgressInfo_t;
#endif

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * Profile Callbacks
 */

/**
 * OAD Module Write Callback Function
 *
 * This callback is used to signal to the application that the OAD profile has
 * received data over the air or needs to process an event in its internal queue
 *
 * @param event The event that the OAD module is posting in the application
                Can either @ref OAD_QUEUE_EVT or @ref OAD_DL_COMPLETE_EVT
 * @param arg This is the bim_var if performing on-chip OAD, otherwise unused
*/
typedef void (*oadWriteCB_t)(uint8_t event, uint16_t arg);

/**
 * OAD Module Callback structure
 */
typedef struct
{
    oadWriteCB_t       pfnOadWrite; //!< The peer has written to the OAD service
} oadTargetCBs_t;

/*********************************************************************
 * FUNCTIONS
 */

/*!
 * Open the OAD module
 *
 * Initializes the OAD Service by registering GATT attributes
 * with the GATT server. Only call this function once.
 *
 * @param   oadTimeout - the timeout on for a pending OAD operation (in ms)
 *
 * @return  Success or Failure
 */
extern uint8_t OAD_open(uint32_t oadTimeout);

/*!
 * Cancel and active OAD and reset the state
 */
extern void OAD_cancel(void);


/*!
 * Close the OAD and its flash_interface, reset the state
 */
extern void OAD_close(void);

/*!
 * Returns whether or not the user application is valid (on-chip OAD only)
 *
 * @return  user app status - true if the user app hasn't been erased.
 */
extern bool OAD_isUsrAppValid(void);

/*!
 * Register a callback function with the OAD Target Profile.
 *
 * @param   pfnOadCBs - struct holding function pointers to OAD application
 *                      callbacks @ref oadTargetCBs_t
 */
extern void OAD_register(oadTargetCBs_t *pfnOadCBs);

/*!
 * Returns the currently running SW version field
 *
 * @param   swVer - pointer to array to copy version into
 * @param   len - number of bytes to copy
 *
 * @return  status - true if copy succeeded
 *                 - false if copy failed (invalid parameter)
 */
extern bool OAD_getSWVersion(uint8_t *swVer, uint8_t len);

/*!
 * Gets the block size currently used by the OAD module
 *
 * @return  The block size being used.
 */
extern uint16_t OAD_getBlockSize(void);

/*!
 * Sets the block size to be used during OAD based on a given MTU
 *
 * @param   mtuSize - The negotiated MTU size
 *
 * @return  TRUE if operation is successful.
 */
extern uint8_t OAD_setBlockSize(uint16_t mtuSize);

/*!
 * Process the OAD message queue/state machine
 *
 * @return  status of operation - @ref OAD_STATUS_CODES
 */
extern uint8_t OAD_processQueue(void);

/*!
 * Returns the active connection handle of the OAD session
 *
 * @return  handle - the 16-bit connection handle of the OAD session
 */
extern uint16_t OAD_getactiveCxnHandle();

/*!
 * This function creates factory image backup of current running image
 *
 * @return  @ref OAD_SUCCESS on successful erasure else
 *          @ref OAD_FLASH_ERR
 */
extern uint8_t oadCreateFactoryImageBackup(void);

/*!
 * This function checks if the value field has even number of 1's.
 *
 * @param   value  Value field to check for even number of 1's.
 *
 * @return  TRUE if even number of 1's otherwise false.
 */
extern bool OAD_evenBitCount(uint32_t value);

#ifdef DMM_OAD
extern void OAD_getProgressInfo(imgProgressInfo_t* pImgInfo);
#endif

/*********************************************************************
*********************************************************************/

/** @} End OAD_PROFILE */

#ifdef __cplusplus
}
#endif

#endif /* OAD_H */
