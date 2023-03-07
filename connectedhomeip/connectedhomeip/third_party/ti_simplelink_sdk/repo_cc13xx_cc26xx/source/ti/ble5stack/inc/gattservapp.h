/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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

/**
 *  @defgroup GATTServApp GATTServApp
 *  @brief This module implements the GATT Server App
 *  @{
 *  @file  gattservapp.h
 *  @brief      GATTServApp layer interface
 */

#ifndef GATTSERVAPP_H
#define GATTSERVAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"

/*********************************************************************
 * CONSTANTS
 */

/**
 * @defgroup GATT_SERV_OTHER_DEFINES GATT Server App Constants
 * @{
 */

/**
 * @defgroup GATT_SERV_MSG_EVENT_DEFINES GATT Server Message IDs
 * @{
 */

#define GATT_CLIENT_CHAR_CFG_UPDATED_EVENT  0x00 //!< Sent when a Client Characteristic Configuration is updated.  This event is sent as an OSAL message defined as gattCharCfgUpdatedEvent_t.

/** @} End GATT_SERV_MSG_EVENT_DEFINES */


/**
 * @defgroup GATT_PROP_BITMAPS_DEFINES GATT Characteristic Properties Bit Fields
 * @{
 */

#define GATT_PROP_BCAST                  0x01 //!< Permits broadcasts of the Characteristic Value
#define GATT_PROP_READ                   0x02 //!< Permits reads of the Characteristic Value
#define GATT_PROP_WRITE_NO_RSP           0x04 //!< Permits writes of the Characteristic Value without response
#define GATT_PROP_WRITE                  0x08 //!< Permits writes of the Characteristic Value with response
#define GATT_PROP_NOTIFY                 0x10 //!< Permits notifications of a Characteristic Value without acknowledgement
#define GATT_PROP_INDICATE               0x20 //!< Permits indications of a Characteristic Value with acknowledgement
#define GATT_PROP_AUTHEN                 0x40 //!< Permits signed writes to the Characteristic Value
#define GATT_PROP_EXTENDED               0x80 //!< Additional characteristic properties are defined in the Characteristic Extended Properties Descriptor

/** @} End GATT_PROP_BITMAPS_DEFINES */

/**
 * @defgroup GATT_EXT_PROP_BITMAPS_DEFINES GATT Characteristic Extended Properties Bit Fields
 * @{
 */

#define GATT_EXT_PROP_RELIABLE_WRITE     0x0001 //!< Permits reliable writes of the Characteristic Value
#define GATT_EXT_PROP_WRITABLE_AUX       0x0002 //!< Permits writes to the characteristic descriptor

/** @} End GATT_EXT_PROP_BITMAPS_DEFINES */

/**
 * @defgroup GATT_CLIENT_CFG_BITMAPS_DEFINES GATT Client Characteristic Configuration Bit Fields
 * @{
 */

#define GATT_CLIENT_CFG_NOTIFY           0x0001 //!< The Characteristic Value shall be notified
#define GATT_CLIENT_CFG_INDICATE         0x0002 //!< The Characteristic Value shall be indicated

/** @} End GATT_CLIENT_CFG_BITMAPS_DEFINES */

/**
 * @defgroup GATT_SERV_CFG_BITMAPS_DEFINES GATT Server Characteristic Configuration Bit Fields
 * @{
 */

#define GATT_SERV_CFG_BCAST              0x0001 //!< The Characteristic Value shall be broadcast when the server is in the broadcast procedure if advertising data resources are available

/** @} End GATT_SERV_CFG_BITMAPS_DEFINES */

#define GATT_CFG_NO_OPERATION            0x0000 //!< No operation

/**
 * @defgroup GATT_FORMAT_TYPES_DEFINES GATT Characteristic Format Types
 * @{
 */

#define GATT_FORMAT_BOOL                 0x01 //!< Unsigned 1 bit; 0 = false, 1 = true
#define GATT_FORMAT_2BIT                 0x02 //!< Unsigned 2 bit integer
#define GATT_FORMAT_NIBBLE               0x03 //!< Unsigned 4 bit integer
#define GATT_FORMAT_UINT8                0x04 //!< Unsigned 8 bit integer
#define GATT_FORMAT_UINT12               0x05 //!< Unsigned 12 bit integer
#define GATT_FORMAT_UINT16               0x06 //!< Unsigned 16 bit integer
#define GATT_FORMAT_UINT24               0x07 //!< Unsigned 24 bit integer
#define GATT_FORMAT_UINT32               0x08 //!< Unsigned 32 bit integer
#define GATT_FORMAT_UINT48               0x09 //!< Unsigned 48 bit integer
#define GATT_FORMAT_UINT64               0x0a //!< Unsigned 64 bit integer
#define GATT_FORMAT_UINT128              0x0b //!< Unsigned 128 bit integer
#define GATT_FORMAT_SINT8                0x0c //!< Signed 8 bit integer
#define GATT_FORMAT_SINT12               0x0d //!< Signed 12 bit integer
#define GATT_FORMAT_SINT16               0x0e //!< Signed 16 bit integer
#define GATT_FORMAT_SINT24               0x0f //!< Signed 24 bit integer
#define GATT_FORMAT_SINT32               0x10 //!< Signed 32 bit integer
#define GATT_FORMAT_SINT48               0x11 //!< Signed 48 bit integer
#define GATT_FORMAT_SINT64               0x12 //!< Signed 64 bit integer
#define GATT_FORMAT_SINT128              0x13 //!< Signed 128 bit integer
#define GATT_FORMAT_FLOAT32              0x14 //!< IEEE-754 32 bit floating point
#define GATT_FORMAT_FLOAT64              0x15 //!< IEEE-754 64 bit floating point
#define GATT_FORMAT_SFLOAT               0x16 //!< IEEE-11073 16 bit SFLOAT
#define GATT_FORMAT_FLOAT                0x17 //!< IEEE-11073 32 bit FLOAT
#define GATT_FORMAT_DUINT16              0x18 //!< IEEE-20601 format
#define GATT_FORMAT_UTF8S                0x19 //!< UTF-8 string
#define GATT_FORMAT_UTF16S               0x1a //!< UTF-16 string
#define GATT_FORMAT_STRUCT               0x1b //!< Opaque structure

/** @} End GATT_FORMAT_TYPES_DEFINES */

/**
 * @defgroup GATT_NS_TYPES_DEFINES GATT Namespace Types
 * @{
 */
#define GATT_NS_NONE                     0x00 //!< No namespace
#define GATT_NS_BT_SIG                   0x01 //!< Bluetooth SIG namespace
#define GATT_NS_BT_DESC_UNKNOWN          0x0000 //!< The description is unknown
/** @} End GATT_NS_TYPES_DEFINES */

/// @brief All profile services bit fields
#define GATT_ALL_SERVICES                0xFFFFFFFF

/// @brief GATT Services bit fields
#define GATT_SERVICE                     0x00000001

/// @cond NODOC
#if defined ( TESTMODES )
  // GATT Test Modes
  #define GATT_TESTMODE_OFF              0 // Test mode off
  #define GATT_TESTMODE_NO_RSP           1 // Ignore incoming request
  #define GATT_TESTMODE_PREPARE_WRITE    2 // Forward Prepare Write Request right away
  #define GATT_TESTMODE_MAX_MTU_SIZE     3 // Use Max ATT MTU size with Exchange MTU Rsp
  #define GATT_TESTMODE_CORRUPT_PW_DATA  4 // Corrupt incoming Prepare Write Request data
#endif
/// @endcond // NODOC

/**
 * @defgroup GATT_Serv_Params GATT Server Parameters
 * @{
 */
#define GATT_PARAM_NUM_PREPARE_WRITES    0 //!< RW  uint8
/** @} End GATT_Serv_Params */

/// @cond NODOC
/// @brief To make the size of the pointer type be platform/compiler independent
#define PTR_TYPE                         unsigned int *
/// @endcond // NODOC

/// @brief GATT local read
#define GATT_LOCAL_READ                  0xFF
/// @brief GATT local write
#define GATT_LOCAL_WRITE                 0xFE

/** @} End GATT_SERV_OTHER_DEFINES */

/*********************************************************************
 * VARIABLES
 */

/*********************************************************************
 * MACROS
 */

/// @brief The number of attribute records in a given attribute table
#define GATT_NUM_ATTRS( attrs )          ( sizeof( attrs ) / sizeof( gattAttribute_t ) )

/// @brief The handle of a service is the handle of the first attribute
#define GATT_SERVICE_HANDLE( attrs )     ( (attrs)[0].handle )

/// @brief The handle of the first included service (i = 1) is the value of the second attribute
#define GATT_INCLUDED_HANDLE( attrs, i ) ( *((uint16 *)((attrs)[(i)].pValue)) )

/// @brief Client Characteristic Configuration table (from CCC attribute value pointer)
#define GATT_CCC_TBL( pValue )           ( (gattCharCfg_t *)(*((PTR_TYPE)(pValue))) )

/*********************************************************************
 * TYPEDEFS
 */

/**
 * @defgroup GATT_Serv_CBs GATT Server App Callbacks
 * @{
 */

/**
 * @brief   Callback function prototype to read an attribute value.
 *
 * @note
 * blePending can be returned ONLY for the following
 * read operations:
 * - @ref ATT_READ_REQ
 * - @ref ATT_READ_BLOB_REQ
 *
 * @note
 * If blePending is returned then it's the responsibility
 * of the application to respond to @ref ATT_READ_REQ and
 * @ref ATT_READ_BLOB_REQ message with @ref ATT_READ_RSP and
 * @ref ATT_READ_BLOB_RSP message respectively.
 *
 * @note
 * Payload 'pValue' used with @ref ATT_READ_RSP and @ref ATT_READ_BLOB_RSP
 * must be allocated using @ref GATT_bm_alloc().
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be read (to be returned)
 * @param   pLen - length of data (to be returned)
 * @param   offset - offset of the first octet to be read
 * @param   maxLen - maximum length of data to be read
 * @param   method - type of read message
 *
 * @return  @ref SUCCESS : Read was successfully
 * @return  @ref blePending : A response is pending for client.
 * @return  Error, otherwise: @ref ATT_ERR_CODE_DEFINES.
 */
typedef bStatus_t (*pfnGATTReadAttrCB_t)( uint16 connHandle, gattAttribute_t *pAttr,
                                          uint8 *pValue, uint16 *pLen, uint16 offset,
                                          uint16 maxLen, uint8 method );

/**
 * @brief   Callback function prototype to write an attribute value.
 *
 * @note
 * blePending can be returned ONLY for the following write operations:
 * - @ref ATT_WRITE_REQ
 * - @ref ATT_WRITE_CMD
 * - @ref ATT_EXECUTE_WRITE_REQ
 * - Multiple @ref ATT_PREPARE_WRITE_REQ followed by one final @ref ATT_EXECUTE_WRITE_REQ
 *
 * @note
 * If blePending is returned then it's the responsibility
 * of the application to:
 * 1. respond to @ref ATT_WRITE_REQ and @ref ATT_EXECUTE_WRITE_REQ message with
 * @ref ATT_WRITE_RSP and @ref ATT_EXECUTE_WRITE_RSP message respectively
 * 2. free each request payload 'pValue' using BM_free().
 *
 * @note
 * @ref ATT_WRITE_CMD does NOT require a response message.
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   pLen - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  @ref SUCCESS : Write was successfully.
 * @return  @ref blePending : A response is pending for this client.
 * @return  Error, otherwise: @ref ATT_ERR_CODE_DEFINES.
 */
typedef bStatus_t (*pfnGATTWriteAttrCB_t)( uint16 connHandle, gattAttribute_t *pAttr,
                                           uint8 *pValue, uint16 len, uint16 offset,
                                           uint8 method );
/**
 * @brief   Callback function prototype to authorize a Read or Write operation
 *          on a given attribute.
 *
 * @param   connHandle - connection request was received on
 * @param   pAttr - pointer to attribute
 * @param   opcode - request opcode (@ref ATT_READ_REQ or @ref ATT_WRITE_REQ)
 *
 * @return  @ref SUCCESS : Operation authorized.
 * @return  @ref ATT_ERR_INSUFFICIENT_AUTHOR
 */
typedef bStatus_t (*pfnGATTAuthorizeAttrCB_t)( uint16 connHandle, gattAttribute_t *pAttr,
                                               uint8 opcode );

/** @} End GATT_Serv_CBs */

/**
 * @defgroup GATT_Serv_Structs GATT Server App Structures
 * @{
 */

/// @brief GATT Structure for Characteristic Presentation Format Value.
typedef struct
{
  uint8 format;    //!< Format of the value of this characteristic
  int8 exponent;   //!< A sign integer which represents the exponent of an integer
  uint16 unit;     //!< Unit of this attribute as defined in the data dictionary
  uint8 nameSpace; //!< Name space of the description
  uint16 desc;     //!< Description of this attribute as defined in a higher layer profile
} gattCharFormat_t;

/// @brief Structure for Client Characteristic Configuration.
typedef struct
{
  uint16 connHandle; //!< Client connection handle
  uint8  value;      //!< Characteristic configuration value for this client
} gattCharCfg_t;

/**
 * @brief GATT Structure for service callback functions
 *
 *        Must be setup by the application and used when
 *        @ref GATTServApp_RegisterService is called.
 */
typedef struct
{
  pfnGATTReadAttrCB_t pfnReadAttrCB;           //!< Read callback function pointer
  pfnGATTWriteAttrCB_t pfnWriteAttrCB;         //!< Write callback function pointer
  pfnGATTAuthorizeAttrCB_t pfnAuthorizeAttrCB; //!< Authorization callback function pointer
} gattServiceCBs_t;

/// @brief GATT Server App event header format.
typedef struct
{
  osal_event_hdr_t  hdr;           //!< GATT_SERV_MSG_EVENT and status
  uint16 connHandle;               //!< Connection message was received on
  uint8 method;                    //!< GATT type of command. Ref: @ref GATT_SERV_MSG_EVENT_DEFINES
} gattEventHdr_t;

/**
 * @brief    @ref GATT_CLIENT_CHAR_CFG_UPDATED_EVENT message format.
 *
 *           This message is sent to the app when a Client Characteristic
 *           Configuration is updated.
 */
typedef struct
{
  osal_event_hdr_t hdr; //!< GATT_SERV_MSG_EVENT and status
  uint16 connHandle;    //!< Connection message was received on
  uint8 method;         //!< GATT_CLIENT_CHAR_CFG_UPDATED_EVENT
  uint16 attrHandle;    //!< attribute handle
  uint16 value;         //!< attribute new value
} gattClientCharCfgUpdatedEvent_t;


/// @brief Structure to keep Prepare Write Requests for each Client
typedef struct
{
  uint16 connHandle;                    //!< connection message was received on
  attPrepareWriteReq_t *pPrepareWriteQ; //!< Prepare Write Request queue
} prepareWrites_t;

/// @brief GATT Structure to keep CBs information for each service being registered
typedef struct
{
  uint16 handle;                //!< Service handle - assigned internally by GATT Server
  CONST gattServiceCBs_t *pCBs; //!< Service callback function pointers
} gattServiceCBsInfo_t;

/// @brief Service callbacks list item
typedef struct _serviceCBsList
{
  struct _serviceCBsList *next;     //!< pointer to next service callbacks record
  gattServiceCBsInfo_t serviceInfo; //!< service handle/callbacks
} serviceCBsList_t;


/*********************************************************************
 * VARIABLES
 */

/// @brief Server Prepare Write table (one entry per each physical link)
extern prepareWrites_t          *prepareWritesTbl;

/// @brief Callbacks for services
extern serviceCBsList_t         *serviceCBsList;

/** @} End GATT_Serv_Structs */

/*********************************************************************
 * API FUNCTIONS
 */

/**
 * @brief   Register your task ID to receive event messages
 *          from the GATT Server Application.
 *
 * @param   taskID - Default task ID to send events.
 */
extern void GATTServApp_RegisterForMsg( uint8 taskID );

/**
 * @brief   Register a service's attribute list and callback functions with
 *          the GATT Server Application.
 *
 * @param   pAttrs - Array of attribute records to be registered
 * @param   numAttrs - Number of attributes in array
 * @param   encKeySize - Minimum encryption key size required by service (7-16 bytes)
 * @param   pServiceCBs - Service callback function pointers
 *
 * @return  @ref SUCCESS : Service registered successfully.
 * @return  @ref INVALIDPARAMETER : Invalid service fields.
 * @return  @ref FAILURE : Not enough attribute handles available.
 * @return  @ref bleMemAllocError
 * @return  @ref bleInvalidRange :Encryption key size's out of range.
 */
extern bStatus_t GATTServApp_RegisterService( gattAttribute_t *pAttrs,
                                              uint16 numAttrs, uint8 encKeySize,
                                              CONST gattServiceCBs_t *pServiceCBs );

/**
 * @brief   Deregister a service's attribute list and callback functions from
 *          the GATT Server Application.
 *
 * @note
 * It's the caller's responsibility to free the service attribute
 * list returned from this API.
 *
 * @param   handle - handle of service to be deregistered
 * @param   p2pAttrs - pointer to array of attribute records (to be returned)
 *
 * @return  @ref SUCCESS : Service deregistered successfully.
 * @return  @ref FAILURE : Service not found.
 */
bStatus_t GATTServApp_DeregisterService( uint16 handle, gattAttribute_t **p2pAttrs );

/**
 * @brief       Find the attribute record within a service attribute
 *              table for a given attribute value pointer.
 *
 * @param       pAttrTbl - pointer to attribute table
 * @param       numAttrs - number of attributes in attribute table
 * @param       pValue - pointer to attribute value
 *
 * @return      Pointer to attribute record
 * @return      NULL, if not found
 */
extern gattAttribute_t *GATTServApp_FindAttr( gattAttribute_t *pAttrTbl,
                                              uint16 numAttrs, uint8 *pValue );
/**
 * @brief   Add function for the GATT Service.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  @ref SUCCESS : Service added successfully.
 * @return  @ref INVALIDPARAMETER : Invalid service field.
 * @return  @ref FAILURE : Not enough attribute handles available.
 * @return  @ref bleMemAllocError
 */
extern bStatus_t GATTServApp_AddService( uint32 services );

/**
 * @brief   Delete function for the GATT Service.
 *
 * @param   services - services to delete. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  @ref SUCCESS : Service deleted successfully.
 * @return  @ref FAILURE : Service not found.
 */
extern bStatus_t GATTServApp_DelService( uint32 services );

/**
 * @brief   Set a GATT Server parameter.
 *
 * @param   param - @ref GATT_Serv_Params
 * @param   len - length of data to right
 * @param   pValue - pointer to data to write. This is dependent on the
 *                   parameter ID and WILL be cast to the appropriate
 *                   data type (example: data type of uint16 will be cast
 *                   to uint16 pointer).
 *
 * @return  @ref SUCCESS : Parameter set successful
 * @return  @ref FAILURE : Parameter in use
 * @return  @ref INVALIDPARAMETER : Invalid parameter
 * @return  @ref bleInvalidRange :Invalid value
 * @return  @ref bleMemAllocError : Memory allocation failed
 */
extern bStatus_t GATTServApp_SetParameter( uint8 param, uint8 len, void *pValue );

/**
 * @brief   Get a GATT Server parameter.
 *
 * @param   param - @ref GATT_Serv_Params
 * @param   pValue - pointer to data to put. This is dependent on the
 *                   parameter ID and WILL be cast to the appropriate
 *                   data type (example: data type of uint16 will be
 *                   cast to uint16 pointer).
 *
 * @return  @ref SUCCESS : Parameter get successful
 * @return  @ref INVALIDPARAMETER : Invalid parameter
 */
extern bStatus_t GATTServApp_GetParameter( uint8 param, void *pValue );

/**
 * @brief   Update the Client Characteristic Configuration for a given
 *          Client.
 *
 * @par Note
 * This API should only be called from the Bond Manager.
 *
 * @param   connHandle - connection handle.
 * @param   attrHandle - attribute handle.
 * @param   value - characteristic configuration value.
 *
 * @return  @ref SUCCESS : Parameter get successful
 * @return  @ref INVALIDPARAMETER : Invalid parameter
 */
extern bStatus_t GATTServApp_UpdateCharCfg( uint16 connHandle, uint16 attrHandle, uint16 value );

/**
 * @brief   Initialize the client characteristic configuration table.
 *
 * @note
 * Each client has its own instantiation of the Client
 *                Characteristic Configuration. Reads/Writes of the Client
 *                Characteristic Configuration only only affect the
 *                configuration of that client.
 *
 * @param   connHandle - connection handle (0xFFFF for all connections).
 * @param   charCfgTbl - client characteristic configuration table.
 */
extern void GATTServApp_InitCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl );

/**
 * @brief   Read the client characteristic configuration for a given
 *          client.
 *
 * @note
 * Each client has its own instantiation of the Client
 *                Characteristic Configuration. Reads of the Client
 *                Characteristic Configuration only shows the configuration
 *                for that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  attribute value
 */
extern uint16 GATTServApp_ReadCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl );

/**
 * @brief   Write the client characteristic configuration for a given
 *          client.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Writes of the Client
 *                Characteristic Configuration only only affect the
 *                configuration of that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 * @param   value - attribute new value.
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 */
extern uint8 GATTServApp_WriteCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl, uint16 value );

/**
 * @brief   Process the client characteristic configuration
 *          write request for a given client.
 *
 * @param   connHandle - connection message was received on.
 * @param   pAttr - pointer to attribute.
 * @param   pValue - pointer to data to be written.
 * @param   len - length of data.
 * @param   offset - offset of the first octet to be written.
 * @param   validCfg - valid configuration.
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref ATT_ERR_INVALID_VALUE : an invalid value for a CCC
 * @return @ref ATT_ERR_INVALID_VALUE_SIZE : an invalid size for a CCC
 * @return @ref ATT_ERR_ATTR_NOT_LONG : offset needs to be 0
 * @return @ref ATT_ERR_INSUFFICIENT_RESOURCES : CCC not found
 */
extern bStatus_t GATTServApp_ProcessCCCWriteReq( uint16 connHandle, gattAttribute_t *pAttr,
                                          uint8 *pValue, uint16 len, uint16 offset,
                                          uint16 validCfg );

/**
 * @brief   Process Client Characteristic Configuration change.
 *
 * @param   charCfgTbl - characteristic configuration table.
 * @param   pValue - pointer to attribute value.
 * @param   authenticated - whether an authenticated link is required.
 * @param   attrTbl - attribute table.
 * @param   numAttrs - number of attributes in attribute table.
 * @param   taskId - task to be notified of confirmation.
 * @param   pfnReadAttrCB - read callback function pointer.
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 * @return @ref bleTimeout : ATT timeout occurred
 * @return @ref blePending : another ATT request is pending
 * @return @ref LINKDB_ERR_INSUFFICIENT_AUTHEN : authentication is required but link is not authenticated
 * @return @ref bleMemAllocError : memory allocation failure occurred when allocating buffer
 */
extern bStatus_t GATTServApp_ProcessCharCfg( gattCharCfg_t *charCfgTbl, uint8 *pValue,
                                        uint8 authenticated, gattAttribute_t *attrTbl,
                                        uint16 numAttrs, uint8 taskId,
                                        pfnGATTReadAttrCB_t pfnReadAttrCB );

/**
 * @brief   Build and send the @ref GATT_CLIENT_CHAR_CFG_UPDATED_EVENT to
 *          the application.
 *
 * @param   connHandle - connection handle
 * @param   attrHandle - attribute handle
 * @param   value - attribute new value
 */
extern void GATTServApp_SendCCCUpdatedEvent( uint16 connHandle, uint16 attrHandle, uint16 value );

/**
 * @brief   Send out a Service Changed Indication.
 *
 * @param   connHandle - connection to use
 * @param   taskId - task to be notified of confirmation
 *
 * @return  @ref SUCCESS : Indication was sent successfully.
 * @return  @ref FAILURE : Service Changed attribute not found.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A confirmation is pending with this client.
 */
extern bStatus_t GATTServApp_SendServiceChangedInd( uint16 connHandle, uint8 taskId );

/**
 * @brief       Read an attribute. If the format of the attribute value
 *              is unknown to GATT Server, use the callback function
 *              provided by the Service.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       service - handle of owner service
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read
 *
 * @return      @ref SUCCESS
 * @return      @ref FAILURE
 */
extern uint8 GATTServApp_ReadAttr( uint16 connHandle, gattAttribute_t *pAttr,
                                   uint16 service, uint8 *pValue, uint16 *pLen,
                                   uint16 offset, uint16 maxLen, uint8 method );

/**
 * @brief   Write attribute data
 *
 * @param   connHandle - connection message was received on
 * @param   handle - attribute handle
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE
 */
extern uint8 GATTServApp_WriteAttr( uint16 connHandle, uint16 handle,
                                    uint8 *pValue, uint16 len, uint16 offset,
                                    uint8 method );

/**
 * @brief   If a service returns blePending to the read attribute call back
 *          invoked from GATTServApp, the service can later respond to with the
 *          value to be read using this API
 *
 * @param   connHandle - connection read request was received on
 * @param   pValue - pointer to data to be read (to be returned)
 * @param   pLen - length of data (to be returned)
 * @param   offset - offset of the first octet to be read
 *
 * @return  @ref SUCCESS : Read was successfully added to response.
 * @return  @ref bleNotConnected : Connection associated with read req is down.
 */
extern bStatus_t GATTServApp_ReadRsp( uint16 connHandle, uint8 *pValue,
                                      uint16 pLen, uint16 offset );

/**
 * @brief   Set a GATT Server Application Parameter value. Use this
 *          function to change the default GATT parameter values.
 *
 * @param   value - new param value
 */
extern void GATTServApp_SetParamValue( uint16 value );

/**
 * @brief   Get a GATT Server Application Parameter value.
 *
 * @return  GATT Parameter value
 */
extern uint16 GATTServApp_GetParamValue( void );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by OSAL.
 */

/// @cond NODOC

/**
 * @internal
 *
 * @brief   Initialize the GATT Server Test Application.
 *
 * @param   taskId - Task identifier for the desired task
 *
 * @return  void
 *
 */
extern void GATTServApp_Init( uint8 taskId, uint8_t cfg_GATTServApp_att_delayed_req, uint8_t cfg_gapBond_gatt_no_service_changed );

/**
 * @internal
 *
 * @brief   GATT Server Application Task event processor. This function
 *          is called to process all events for the task. Events include
 *          timers, messages and any other user defined events.
 *
 * @param   task_id - The OSAL assigned task ID.
 * @param   events - events to process. This is a bit map and can
 *                   contain more than one event.
 */
extern uint16 GATTServApp_ProcessEvent( uint8 taskId, uint16 events );

/**
 * @internal
 *
 * @brief   Find the Authorize Attribute CB function pointer for a given service.
 *
 * @param   handle - service attribute handle
 *
 * @return  pointer to the found CB. NULL, otherwise.
 */
extern pfnGATTAuthorizeAttrCB_t  gattServApp_FindAuthorizeAttrCB( uint16 handle );

/**
 * @internal
 *
 * @brief       Enqueue Prepare Write Request.
 *
 * @param       connHandle - connection packet was received on
 * @param       pReq - pointer to request
 *
 * @return      @ref SUCCESS or Failure
 */
extern bStatus_t gattServApp_EnqueuePrepareWriteReq( uint16 connHandle, attPrepareWriteReq_t *pReq );

/**
 * @internal
 *
 * @brief       Process a Write Long operation.
 *
 * @param       pMsg - pointer to received message
 * @param       pQueue - pointer to client prepare write queue
 * @param       pErrHandle - attribute handle that generates an error
 *
 * @return      @ref SUCCESS or Failure
 */
extern bStatus_t gattServApp_ProcessWriteLong( gattMsgEvent_t *pMsg, prepareWrites_t *pQueue, uint16 *pErrHandle );

/**
 * @internal
 *
 * @brief       Process a Reliable Writes operation.
 *
 * @param       pMsg - pointer to received message
 * @param       pQueue - pointer to client prepare write queue
 * @param       pErrHandle - attribute handle that generates an error
 *
 * @return      @ref SUCCESS or Failure
 */
extern bStatus_t gattServApp_ProcessReliableWrites( gattMsgEvent_t *pMsg, prepareWrites_t *pQueue, uint16 *pErrHandle );

/**
 * @internal
 *
 * @brief       Process attribute value received from service to build
 *              Read by Type Response message.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttrValue - pointer to data read
 * @param       attrLen - length of data read
 * @param       attrHandle - handle of attribute read
 *
 * @return      @ref SUCCESS, Failure, or ATT_ERR_INSUFFICIENT_RESOURCES
 */
extern bStatus_t gattServApp_buildReadByTypeRsp( uint16 connHandle, uint8 *pAttrValue, uint16 attrLen, uint16 attrHandle );

/**
 * @internal
 *
 * @brief       Find client's queue.
 *
 * @param       connHandle - connection used by client
 *
 * @return      Pointer to queue. NULL, otherwise.
 */
extern prepareWrites_t *gattServApp_FindPrepareWriteQ( uint16 connHandle );

/**
 * @internal
 *
 * @brief       Check for a Write Long operation.
 *
 * @param       pMsg - pointer to received message
 * @param       pQueue - pointer to client prepare write queue
 *
 * @return      TRUE or FALSE
 */
extern uint8 gattServApp_IsWriteLong( attExecuteWriteReq_t *pReq, prepareWrites_t *pQueue );

/**
 * @internal
 *
 * @brief       Clear the prepare write queue for a GATT Client.
 *
 * @param       pQueue - pointer to client's queue
 */
extern void gattServApp_ClearPrepareWriteQ( prepareWrites_t *pQueue );

/**
 * @brief   Enqueue an ATT response retransmission
 *
 * Enable L2CAP signaling to GATT Serv App if it is not already enabled.
 *
 * @param       connHandle - connection event belongs to
 * @param       method - type of message
 * @param       pMsg - pointer to message to be sent
 *
 * @return      SUCCESS
 * @return      @ref bleMemAllocError
 */
extern bStatus_t gattServApp_EnqueueReTx( uint16 connHandle, uint8 method,
                                          gattMsg_t *pMsg );

/**
 * @brief  Dequeue and try to send an ATT response retransmission
 *
 * Disable L2CAP signaling to GATT Serv App if the queue is empty
 *
 * @param       connHandle - connection event belongs to
 * @param       method - type of message
 * @param       pMsg - pointer to message to be sent
 */
extern void gattServApp_DequeueReTx( void );

/// @endcond // NODOC

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GATTSERVAPP_H */

/** @} End GATTServApp */
