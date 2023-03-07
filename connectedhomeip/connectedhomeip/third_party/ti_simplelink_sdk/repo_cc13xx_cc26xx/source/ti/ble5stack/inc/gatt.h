/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @addtogroup ATT_GATT
 *  @{
 *  @file  gatt.h
 *  @brief      GATT layer interface
 */

#ifndef GATT_H
#define GATT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"

#include "att.h"

/*********************************************************************
 * CONSTANTS
 */

/**  @addtogroup ATT_GATT_Constants
 *   @{
 *   @defgroup GATT_Permits GATT Attribute Access Permissions Bit Fields
 *   @{
 */
#define GATT_PERMIT_READ                0x01   //!< Attribute is Readable
#define GATT_PERMIT_WRITE               0x02   //!< Attribute is Writable
#define GATT_PERMIT_AUTHEN_READ         0x04   //!< Read requires Authentication
#define GATT_PERMIT_AUTHEN_WRITE        0x08   //!< Write requires Authentication
#define GATT_PERMIT_AUTHOR_READ         0x10   //!< Read requires Authorization
#define GATT_PERMIT_AUTHOR_WRITE        0x20   //!< Write requires Authorization
#define GATT_PERMIT_ENCRYPT_READ        0x40   //!< Read requires Encryption
#define GATT_PERMIT_ENCRYPT_WRITE       0x80   //!< Write requires Encryption
/** @} End GATT_Permits */

#if !defined( GATT_MAX_NUM_PREPARE_WRITES )
  #define GATT_MAX_NUM_PREPARE_WRITES   5 //!< GATT Maximum number of attributes that Attribute Server can prepare for writing per Attribute Client
#endif

/**
 * @defgroup GATT_Key_Sizes GATT Encryption Key Size Limits
 * @{
 */
#define GATT_MIN_ENCRYPT_KEY_SIZE       7  //!< GATT Minimum Encryption Key Size
#define GATT_MAX_ENCRYPT_KEY_SIZE       16 //!< GATT Maximum Encryption Key Size
/** @} End GATT_Key_Sizes */

#define GATT_MAX_ATTR_SIZE              512 //!< GATT Maximum length of an attribute value

#define GATT_BASE_METHOD                0x40  //!< GATT Base Method

#define GATT_INVALID_HANDLE             0x0000 //!< Invalid attribute handle
#define GATT_MIN_HANDLE                 0x0001 //!< Minimum attribute handle
#define GATT_MAX_HANDLE                 0xFFFF //!< Maximum attribute handle

#define GATT_ATTR_HANDLE_SIZE           0x02 //!< Number of octets attribute handle

#define GATT_MAX_MTU                    0xFFFF //!< Maximum MTU size
/** @} */ // end of ATT_GATT_Constants

/*********************************************************************
 * VARIABLES
 */

/*********************************************************************
 * MACROS
 */

/// @cond NODOC

// Attribute Access Permissions
#define gattPermitRead( a )              ( (a) & GATT_PERMIT_READ )
#define gattPermitWrite( a )             ( (a) & GATT_PERMIT_WRITE )
#define gattPermitEncryptRead( a )       ( (a) & GATT_PERMIT_ENCRYPT_READ )
#define gattPermitEncryptWrite( a )      ( (a) & GATT_PERMIT_ENCRYPT_WRITE )
#define gattPermitAuthorRead( a )        ( (a) & GATT_PERMIT_AUTHOR_READ )
#define gattPermitAuthorWrite( a )       ( (a) & GATT_PERMIT_AUTHOR_WRITE )
#define gattPermitAuthenRead( a )        ( (a) & GATT_PERMIT_AUTHEN_READ )
#define gattPermitAuthenWrite( a )       ( (a) & GATT_PERMIT_AUTHEN_WRITE )

// Check for different UUID types
#define gattPrimaryServiceType( t )      ( MAP_ATT_CompareUUID( primaryServiceUUID, ATT_BT_UUID_SIZE, \
                                                                (t).uuid, (t).len ) )
#define gattSecondaryServiceType( t )    ( MAP_ATT_CompareUUID( secondaryServiceUUID, ATT_BT_UUID_SIZE, \
                                                                (t).uuid, (t).len ) )
#define gattCharacterType( t )           ( MAP_ATT_CompareUUID( characterUUID, ATT_BT_UUID_SIZE, \
                                                                (t).uuid, (t).len ) )
#define gattIncludeType( t )             ( MAP_ATT_CompareUUID( includeUUID, ATT_BT_UUID_SIZE, \
                                                                (t).uuid, (t).len ) )
#define gattServiceType( t )             ( gattPrimaryServiceType( (t) ) || \
                                           gattSecondaryServiceType( (t) ) )

/// @endcond // NODOC

/*********************************************************************
 * TYPEDEFS
 */

/** @} */ // end of ATT_GATT

/**
 *  @addtogroup ATT_GATT_Events
 *  @{
 */

/// @brief @brief GATT Find By Type Value Request format.
typedef struct
{
  uint16 startHandle;  //!< First requested handle number (must be first field)
  uint16 endHandle;    //!< Last requested handle number
  attAttrType_t value; //!< Primary service UUID value (2 or 16 octets)
} gattFindByTypeValueReq_t;

/// @brief GATT Read By Type Request format.
typedef struct
{
  uint8 discCharsByUUID;  //!< Whether this is a GATT Discover Characteristics by UUID sub-procedure
  attReadByTypeReq_t req; //!< Read By Type Request
} gattReadByTypeReq_t;

/// @brief GATT Write Long Request format. Do not change the order of the members.
typedef struct
{
  uint8 reliable;           //!< Whether reliable writes requested (always FALSE for Write Long)
  attPrepareWriteReq_t req; //!< ATT Prepare Write Request
  uint16 lastOffset;        //!< Offset of last Prepare Write Request sent
} gattWriteLongReq_t;

/// @brief GATT Reliable Writes Request format. Do not change the order of the members.
typedef struct
{
  uint8 reliable;              //!< Whether reliable writes requested (always TRUE for Reliable Writes)
  attPrepareWriteReq_t *pReqs; //!< Array of Prepare Write Requests (must be allocated)
  uint8 numReqs;               //!< Number of Prepare Write Requests
  uint8 index;                 //!< Index of last Prepare Write Request sent
  uint8 flags;                 //!< 0x00 - cancel all prepared writes.
                               //!< 0x01 - immediately write all pending prepared values.
} gattReliableWritesReq_t;

/**
 * @brief GATT Message format.
 *
 * This is aunion of all attribute protocol/profile messages
 * and locally-generated events used between the attribute protocol/profile and
 * upper layer application.
 */
typedef union
{
  // Request messages
  attExchangeMTUReq_t exchangeMTUReq;              //!< ATT Exchange MTU Request
  attFindInfoReq_t findInfoReq;                    //!< ATT Find Information Request
  attFindByTypeValueReq_t findByTypeValueReq;      //!< ATT Find By Type Value Request
  attReadByTypeReq_t readByTypeReq;                //!< ATT Read By Type Request
  attReadReq_t readReq;                            //!< ATT Read Request
  attReadBlobReq_t readBlobReq;                    //!< ATT Read Blob Request
  attReadMultiReq_t readMultiReq;                  //!< ATT Read Multiple Request
  attReadByGrpTypeReq_t readByGrpTypeReq;          //!< ATT Read By Group Type Request
  attWriteReq_t writeReq;                          //!< ATT Write Request
  attPrepareWriteReq_t prepareWriteReq;            //!< ATT Prepare Write Request
  attExecuteWriteReq_t executeWriteReq;            //!< ATT Execute Write Request
  gattFindByTypeValueReq_t gattFindByTypeValueReq; //!< GATT Find By Type Value Request
  gattReadByTypeReq_t gattReadByTypeReq;           //!< GATT Read By Type Request
  gattWriteLongReq_t gattWriteLongReq;             //!< GATT Long Write Request
  gattReliableWritesReq_t gattReliableWritesReq;   //!< GATT Reliable Writes Request

  // Response messages
  attErrorRsp_t errorRsp;                          //!< ATT Error Response
  attExchangeMTURsp_t exchangeMTURsp;              //!< ATT Exchange MTU Response
  attFindInfoRsp_t findInfoRsp;                    //!< ATT Find Information Response
  attFindByTypeValueRsp_t findByTypeValueRsp;      //!< ATT Find By Type Value Response
  attReadByTypeRsp_t readByTypeRsp;                //!< ATT Read By Type Response
  attReadRsp_t readRsp;                            //!< ATT Read Response
  attReadBlobRsp_t readBlobRsp;                    //!< ATT Read Blob Response
  attReadMultiRsp_t readMultiRsp;                  //!< ATT Read Multiple Response
  attReadByGrpTypeRsp_t readByGrpTypeRsp;          //!< ATT Read By Group Type Response
  attPrepareWriteRsp_t prepareWriteRsp;            //!< ATT Prepare Write Response

  // Indication and Notification messages
  attHandleValueNoti_t handleValueNoti;            //!< ATT Handle Value Notification
  attHandleValueInd_t handleValueInd;              //!< ATT Handle Value Indication

  // Locally-generated event messages
  attFlowCtrlViolatedEvt_t flowCtrlEvt;            //!< ATT Flow Control Violated Event
  attMtuUpdatedEvt_t mtuEvt;                       //!< ATT MTU Updated Event
} gattMsg_t;

/**
 * @brief GATT @ref GATT_MSG_EVENT message format.
 *
 * This message is used to forward an
 * incoming attribute protocol/profile message up to upper layer application.
 */
typedef struct
{
  osal_event_hdr_t hdr; //!< GATT_MSG_EVENT and status
  uint16 connHandle;    //!< Connection message was received on
  uint8 method;         //!< Type of message
  gattMsg_t msg;        //!< Attribute protocol/profile message
} gattMsgEvent_t;

/** @} */ // end of ATT_GATT_events

/**
 *  @addtogroup ATT_GATT_Structs
 *  @{
 */

/// @brief GATT Attribute Type format.
typedef struct
{
  uint8 len;         //!< Length of UUID (2 or 6)
  const uint8 *uuid; //!< Pointer to UUID
} gattAttrType_t;

/**
 * @brief GATT Attribute format.
 *
 * @note
 * The list must start with a Service attribute followed by
 * all attributes associated with this Service attribute.
 */
typedef struct attAttribute_t
{
  gattAttrType_t type; //!< Attribute type (2 or 16 octet UUIDs)
  uint8 permissions;    //!< Attribute permissions
  uint16 handle;       //!< Attribute handle - assigned internally by attribute server
  uint8* const pValue; //!< Attribute value - encoding of the octet array is defined in
                       //!< the applicable profile. The maximum length of an attribute
                       //!< value shall be 512 octets.
} gattAttribute_t;

/// @brief GATT Service format.
typedef struct
{
  uint16 numAttrs; //!< Number of attributes in attrs
  uint8 encKeySize;//!< Minimum encryption key size required by service (7-16 bytes)
  gattAttribute_t *attrs; //!< Array of attribute records;
} gattService_t;

/** @} */ // end of ATT_GATT_Structs

/*********************************************************************
 * VARIABLES
 */

/// @cond NODOC
extern uint8 gattNumConns;
/// @endcond // NODOC

/**
 *  @addtogroup ATT_GATT
 *  @{
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*-------------------------------------------------------------------
 * GATT Client Public APIs
 */

/**
 * @brief   Initialize the Generic Attribute Profile Client.
 *
 * @note GATT Clients must call this from the application init function
 *
 * @return  @ref SUCCESS : Client initialized successfully.
 * @return  @ref bleMemAllocError
 */
extern bStatus_t GATT_InitClient( void );

/**
 * @brief   Register to receive incoming ATT Indications or Notifications
 *          of attribute values.
 *
 * @note GATT Clients must call this from the application init function
 *
 * @param   taskId - task to forward indications or notifications to
 */
extern void GATT_RegisterForInd( uint8 taskId );

/**
 * @brief   Request the server to prepare to write the value of an attribute.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_PrepareWriteReq( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   @ref ATT_ExecuteWriteReq
 *
 * Request the server to write or cancel the write of all the prepared
 * values currently held in the prepare queue from this client.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ExecuteWriteReq( uint16 connHandle, attExecuteWriteReq_t *pReq, uint8 taskId );

/*-------------------------------------------------------------------
 * GATT Server Public APIs
 */

/**
 * @brief   Initialize the Generic Attribute Profile Server.
 *
 * @note
 * This is called by the stack initizliation functions
 *
 * @return  @ref SUCCESS : Server initialized successfully.
 * @return  @ref bleMemAllocError
 */
extern bStatus_t GATT_InitServer( void );

/**
 * @brief   Register a service attribute list with the GATT Server.
 *
 * A service is composed of characteristics or references to other services.
 *          Each characteristic contains a value and may contain optional
 *          information about the value. There are two types of services:
 *          primary service and secondary service.
 *
 *          A service definition begins with a service declaration and ends
 *          before the next service declaration or the maximum Attribute Handle.
 *
 *          A characteristic definition begins with a characteristic declaration
 *          and ends before the next characteristic or service declaration or
 *          maximum Attribute Handle.
 *
 *          The attribute server will only keep a pointer to the attribute
 *          list, so the calling application will have to maintain the code
 *          and RAM associated with this list.
 *
 * @param   pService - pointer to service attribute list to be registered
 *
 * @return  @ref SUCCESS : Service registered successfully.
 * @return  @ref INVALIDPARAMETER : Invalid service field.
 * @return  @ref FAILURE : Not enough attribute handles available.
 * @return  @ref bleMemAllocError
 * @return  @ref bleInvalidRange :Encryption key size's out of range.
 */
extern bStatus_t GATT_RegisterService( gattService_t *pService );

/**
 * @brief   Deregister a service attribute list with the GATT Server.
 *
 * @warning It's the caller's responsibility to free the service attribute
 *          list returned from this API.
 *
 * @param   handle - handle of service to be deregistered
 * @param   pService - pointer to deregistered service (to be returned)
 *
 * @return  @ref SUCCESS : Service deregistered successfully.
 * @return  @ref FAILURE : Service not found.
 */
extern bStatus_t GATT_DeregisterService( uint16 handle, gattService_t *pService );

/**
 * @brief   Register to receive incoming ATT Requests.
 *
 * @param   taskId - task to forward requests to
 */
extern void GATT_RegisterForReq( uint8 taskId );

/**
 * @brief   Verify the permissions of an attribute for reading.
 *
 * @param   connHandle - connection to use
 * @param   pAttr - pointer to attribute
 * @param   service - service handle
 *
 * @return  @ref SUCCESS : Attribute can be read.
 * @return  @ref ATT_ERR_INSUFFICIENT_ENCRYPT : Attribute cannot be read.
 * @return  @ref ATT_ERR_INSUFFICIENT_AUTHEN : Attribute requires authentication.
 * @return  @ref ATT_ERR_INSUFFICIENT_KEY_SIZE : Key Size used for encrypting is insufficient.
 * @return  @ref ATT_ERR_INSUFFICIENT_ENCRYPT : Attribute requires encryption.
 */
extern bStatus_t GATT_VerifyReadPermissions( uint16 connHandle, gattAttribute_t *pAttr,
                                             uint16 service);

/**
 * @brief   Verify the permissions of an attribute for writing.
 *
 * @param   connHandle - connection to use
 * @param   pAttr - pointer to attribute
 * @param   service - service handle
 * @param   pReq - pointer to write request
 *
 * @return  @ref SUCCESS : Attribute can be written.
 * @return  @ref ATT_ERR_INSUFFICIENT_ENCRYPT : Attribute cannot be written.
 * @return  @ref ATT_ERR_INSUFFICIENT_AUTHEN : Attribute requires authentication.
 * @return  @ref ATT_ERR_INSUFFICIENT_KEY_SIZE : Key Size used for encrypting is insufficient.
 * @return  @ref ATT_ERR_INSUFFICIENT_ENCRYPT : Attribute requires encryption.
 */
extern bStatus_t GATT_VerifyWritePermissions( uint16 connHandle, gattAttribute_t *pAttr,
                                              uint16 service, attWriteReq_t *pReq );

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
extern uint8 GATT_ServiceChangedInd( uint16 connHandle, uint8 taskId );

/**
 * @brief   Find the attribute record for a given handle and UUID.
 *
 * @param   startHandle - first handle to look for
 * @param   endHandle - last handle to look for
 * @param   pUUID - pointer to UUID to look for
 * @param   len - length of UUID
 * @param   pHandle - handle of owner of attribute (to be returned)
 *
 * @return  Pointer to attribute record.
 * @return  NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindHandleUUID( uint16 startHandle, uint16 endHandle, const uint8 *pUUID,
                                             uint16 len, uint16 *pHandle );
/**
 * @brief   Find the attribute record for a given handle
 *
 * @param   handle - handle to look for
 * @param   pHandle - handle of owner of attribute (to be returned)
 *
 * @return  Pointer to attribute record.
 * @return  NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindHandle( uint16 handle, uint16 *pHandle );

/**
 * @brief   Find the next attribute of the same type for a given attribute.
 *
 * @param   pAttr - pointer to attribute to find a next for
 * @param   endHandle - last handle to look for
 * @param   service - handle of owner service
 * @param   pLastHandle - handle of last attribute (to be returned)
 *
 * @return  Pointer to next attribute record
 * @return  NULL, otherwise.
 */
extern gattAttribute_t *GATT_FindNextAttr( gattAttribute_t *pAttr, uint16 endHandle,
                                           uint16 service, uint16 *pLastHandle );

/**
 * @brief   Get the number of attributes for a given service.
 *
 * @param   handle - service handle to look for
 *
 * @return  Number of attributes if service found. 0, otherwise.
 */
extern uint16 GATT_ServiceNumAttrs( uint16 handle );

/**
 * @brief   Get the minimum encryption key size required by a given service.
 *
 * @param   handle - service handle to look for
 *
 * @return  Encryption key size if service found.
 * @return  Default key size, otherwise.
 */
extern uint8 GATT_ServiceEncKeySize( uint16 handle );

/**
 * @brief   Send an ATT Response message out.
 *
 * @param   connHandle - connection to use
 * @param   method - type of response message
 * @param   pRsp - pointer to ATT response to be sent
 *
 * @return  @ref SUCCESS : Response was sent successfully.
 * @return  @ref INVALIDPARAMETER : Invalid response field.
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 * @return  @ref blePending : In the middle of another transmit.
 * @return  @ref bleInvalidMtuSize : Packet length is larger than connection's MTU size.
 */
extern bStatus_t GATT_SendRsp( uint16 connHandle, uint8 method, gattMsg_t *pRsp );

/*-------------------------------------------------------------------
 * GATT Server Sub-Procedure APIs
 */

/**
 * @brief   Send a GATT Indication
 *
 * This sub-procedure is used when a server is configured to
 * indicate a characteristic value to a client and expects an
 * attribute protocol layer acknowledgement that the indication
 * was successfully received.
 *
 * @ref ATT_HandleValueInd is used in this sub-procedure.
 *
 * @par Corresponding Events
 * If the return status from this function is @ref SUCCESS and the GATT client
 * succesfully sends an acknowledgement,  the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * @ref ATT_HANDLE_VALUE_CFM of type @ref attHandleValueInd_t , with
 * status @ref SUCCESS or @ref bleTimeout . At this point, the procedure
 * is complete.
 *
 * @warning The payload must be dynamically allocated using @ref GATT_bm_alloc
 *
 * @note The client must use @ref GATT_RegisterForInd in order to receive
 * Indications in the application task and use
 * @ref ATT_HandleValueCfm to return the acknowledgement to the server
 *
 * @param   connHandle - connection to use
 * @param   pInd - pointer to indication to be sent
 * @param   authenticated - whether an authenticated link is required
 *                          0x01: LE Legacy authenticated
 *                          0x02: Secure Connections authenticated
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Indication was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A confirmation is pending with this client.
 * @return  @ref bleMemAllocError
 * @return  @ref bleInvalidMtuSize : Packet length is larger than connection's MTU size.
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_Indication( uint16 connHandle, attHandleValueInd_t *pInd,
                                  uint8 authenticated, uint8 taskId );

/**
 * @brief   Send a GATT Notification
 *
 * This sub-procedure is used when a server is configured to
 * notify a characteristic value to a client without expecting
 * any attribute protocol layer acknowledgement that the
 * notification was successfully received.
 *
 * @ref ATT_HandleValueNoti is used in this sub-procedure.
 *
 * @note
 * A notification may be sent at any time.
 * No confirmation will be sent to the calling task for
 * this sub-procedure.
 *
 * @warning The payload must be dynamically allocated using @ref GATT_bm_alloc
 *
 * @note The client must use @ref GATT_RegisterForInd in order to receive
 * Notifications in the application task
 *
 * @param   connHandle - connection to use
 * @param   pNoti - pointer to notification to be sent
 * @param   authenticated - whether an authenticated link is required
 *                          0x01: LE Legacy authenticated
 *                          0x02: Secure Connections authenticated
 *
 * @return  @ref SUCCESS : Notification was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 * @return  @ref bleInvalidMtuSize : Packet length is larger than connection's MTU size.
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_Notification( uint16 connHandle, attHandleValueNoti_t *pNoti,
                                    uint8 authenticated );

/*-------------------------------------------------------------------
 * GATT Client Sub-Procedure APIs
 */

/**
 * @brief   Exchange MTU Request
 *
 * This sub-procedure is used by the client to set the ATT_MTU
 * to the maximum possible value that can be supported by both
 * devices when the client supports a value greater than the
 * default ATT_MTU for the Attribute Protocol. This sub-procedure
 * shall only be initiated once during a connection.
 *
 * @ref ATT_ExchangeMTUReq is used by this sub-procedure.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_EXCHANGE_MTU_RSP of type @ref attExchangeMTURsp_t , with
 * status @ref SUCCESS or @ref bleTimeout , if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , with
 * status @ref SUCCESS  , if an error occurred on the server
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ExchangeMTU( uint16 connHandle, attExchangeMTUReq_t *pReq, uint8 taskId );

/**
 * @brief  Discovery All Primary Services
 *
 * This sub-procedure is used by a client to discover all
 * the primary services on a server.
 *
 * @ref ATT_ReadByGrpTypeReq is used with the Attribute
 * Type parameter set to the UUID for "Primary Service". The
 * Starting Handle is set to 0x0001 and the Ending Handle is
 * set to 0xFFFF.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_READ_BY_GRP_TYPE_RSP of type @ref attReadByGrpTypeRsp_t ,
 * if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , if an error occurred on the server

 * @note This sub-procedure is complete when either @ref ATT_READ_BY_GRP_TYPE_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or
 * @ref ATT_ERROR_RSP (with @ref SUCCESS status) is received by the calling task
 *
 * @param   connHandle - connection to use
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_DiscAllPrimaryServices( uint16 connHandle, uint8 taskId );

/**
 * @brief   Discovery Primary Service by UUID
 *
 * This sub-procedure is used by a client to discover a specific
 * primary service on a server when only the Service UUID is
 * known. The primary specific service may exist multiple times
 * on a server. The primary service being discovered is identified
 * by the service UUID.
 *
 * @ref ATT_FindByTypeValueReq is used with the Attribute
 * Type parameter set to the UUID for "Primary Service" and the
 * Attribute Value set to the 16-bit Bluetooth UUID or 128-bit
 * UUID for the specific primary service. The Starting Handle shall
 * be set to 0x0001 and the Ending Handle shall be set to 0xFFFF.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_FIND_BY_TYPE_VALUE_RSP of type @ref attFindByTypeValueRsp_t ,
 * if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_FIND_BY_TYPE_VALUE_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 * (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pUUID - pointer to service UUID to look for
 * @param   len - length of value
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_DiscPrimaryServiceByUUID( uint16 connHandle, uint8 *pUUID,
                                                uint8 len, uint8 taskId );
/**
 * @brief   This sub-procedure is used by a client to find include
 * service declarations within a service definition on a
 * server. The service specified is identified by the service
 * handle range.
 *
 * @ref ATT_ReadByTypeReq is used with the Attribute
 * Type parameter set to the UUID for "Included Service". The
 * Starting Handle is set to starting handle of the specified
 * service and the Ending Handle is set to the ending handle
 * of the specified service.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_READ_BY_TYPE_RSP of type @ref attReadByTypeRsp_t ,if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_BY_TYPE_RSP
 *       (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 *       (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_FindIncludedServices( uint16 connHandle, uint16 startHandle,
                                            uint16 endHandle, uint8 taskId );
/**
 * @brief   Discover all Characteristics
 *
 * This sub-procedure is used by a client to find all the
 * characteristic declarations within a service definition on
 * a server when only the service handle range is known. The
 * service specified is identified by the service handle range.
 *
 * @ref ATT_ReadByTypeReq is used with the Attribute Type
 * parameter set to the UUID for "Characteristic". The Starting
 * Handle is set to starting handle of the specified service and
 * the Ending Handle is set to the ending handle of the specified
 * service.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_READ_BY_TYPE_RSP of type @ref attReadByTypeRsp_t , if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_BY_TYPE_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 * (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_DiscAllChars( uint16 connHandle, uint16 startHandle,
                                    uint16 endHandle, uint8 taskId );
/**
 * @brief   Discovery Characteristics by UUID
 *
 * This sub-procedure is used by a client to discover service
 * characteristics on a server when only the service handle
 * ranges are known and the characteristic UUID is known.
 * The specific service may exist multiple times on a server.
 * The characteristic being discovered is identified by the
 * characteristic UUID.
 *
 * @ref ATT_ReadByTypeReq is used with the Attribute Type
 * is set to the UUID for "Characteristic" and the Starting
 * Handle and Ending Handle parameters is set to the service
 * handle range.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_READ_BY_TYPE_RSP of type @ref attReadByTypeRsp_t , if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_BY_TYPE_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 * (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_DiscCharsByUUID( uint16 connHandle, attReadByTypeReq_t *pReq, uint8 taskId );

/**
 * @brief   Discovery All Characteristic Descriptors
 *
 * This sub-procedure is used by a client to find all the
 * characteristic descriptor's Attribute Handles and Attribute
 * Types within a characteristic definition when only the
 * characteristic handle range is known. The characteristic
 * specified is identified by the characteristic handle range.
 *
 * @ref ATT_FindInfoReq is used with the Starting
 * Handle set to starting handle of the specified characteristic
 * and the Ending Handle set to the ending handle of the specified
 * characteristic. The UUID Filter parameter is NULL (zero length).
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_FIND_INFO_RSP of type @ref attFindInfoRsp_t , if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_FIND_INFO_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 * (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   startHandle - starting handle
 * @param   endHandle - end handle
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_DiscAllCharDescs( uint16 connHandle, uint16 startHandle,
                                        uint16 endHandle, uint8 taskId );
/**
 * @brief   Read Characteristic Value
 *
 * This sub-procedure is used to read a Characteristic Value
 * from a server when the client knows the Characteristic Value
 * Handle.
 *
 * @ref ATT_ReadReq is used with the Attribute Handle
 * parameter set to the Characteristic Value Handle. The Read
 * Response returns the Characteristic Value in the Attribute
 * Value parameter.
 *
 * @note: The Read Response only contains a Characteristic Value that
 * is less than or equal to (ATT_MTU - 1) octets in length. If
 * the Characteristic Value is greater than (ATT_MTU - 1) octets
 * in length, the Read Long Characteristic Value procedure may
 * be used if the rest of the Characteristic Value is required.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_READ_RSP of type @ref attReadRsp_t , if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t , if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_RSP
 * (with @ref SUCCESS or @ref bleTimeout status) or @ref ATT_ERROR_RSP (with
 * @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReadCharValue( uint16 connHandle, attReadReq_t *pReq, uint8 taskId );

/**
 * @brief   Read Using Characteristic UUID
 *
 * This sub-procedure is used to read a Characteristic Value
 * from a server when the client only knows the characteristic
 * UUID and does not know the handle of the characteristic.
 *
 * @ref ATT_ReadByTypeReq is used to perform the sub-procedure.
 * The Attribute Type is set to the known characteristic UUID and
 * the Starting Handle and Ending Handle parameters shall be set
 * to the range over which this read is to be performed. This is
 * typically the handle range for the service in which the
 * characteristic belongs.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_READ_BY_TYPE_RSP of type @ref attReadByTypeRsp_t ,if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_BY_TYPE_RSP
 * (with @ref SUCCESS or @ref bleTimeout status) or @ref ATT_ERROR_RSP (with
 * @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReadUsingCharUUID( uint16 connHandle, attReadByTypeReq_t *pReq, uint8 taskId );
/**
 * @brief   Read Long Characteristic Value
 *
 * This sub-procedure is used to read a Characteristic Value from
 * a server when the client knows the Characteristic Value Handle
 * and the length of the Characteristic Value is longer than can
 * be sent in a single Read Response Attribute Protocol message.
 *
 * @ref ATT_ReadBlobReq is used in this sub-procedure.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive multiple @ref GATT_MSG_EVENT messages with method:
 * - @ref ATT_READ_BLOB_RSP of type @ref attReadBlobRsp_t ,if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_BLOB_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 * (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReadLongCharValue( uint16 connHandle, attReadBlobReq_t *pReq, uint8 taskId );

/**
 * @brief   Read Multiple Characteristic Values
 *
 * This sub-procedure is used to read multiple Characteristic Values
 * from a server when the client knows the Characteristic Value
 * Handles. The Attribute Protocol Read Multiple Requests is used
 * with the Set Of Handles parameter set to the Characteristic Value
 * Handles. The Read Multiple Response returns the Characteristic
 * Values in the Set Of Values parameter.
 *
 * @ref ATT_ReadMultiReq is used in this sub-procedure.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_READ_MULTI_RSP of type @ref attReadMultiRsp_t ,if the procedure was successful
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_MULTI_RSP
 * (with @ref SUCCESS or @ref bleTimeout status) or @ref ATT_ERROR_RSP (with
 * @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReadMultiCharValues( uint16 connHandle, attReadMultiReq_t *pReq, uint8 taskId );

/**
 * @brief   Write No Response
 *
 * This sub-procedure is used to write a Characteristic Value
 * to a server when the client knows the Characteristic Value
 * Handle and the client does not need an acknowledgement that
 * the write was successfully performed. This sub-procedure
 * only writes the first (ATT_MTU - 3) octets of a Characteristic
 * Value. This sub-procedure can not be used to write a long
 * characteristic; instead the Write Long Characteristic Values
 * sub-procedure should be used.
 *
 * @ref ATT_WriteReq is used for this sub-procedure. The
 * Attribute Handle parameter shall be set to the Characteristic
 * Value Handle. The Attribute Value parameter shall be set to
 * the new Characteristic Value.
 *
 * @note No response will be sent to the calling task for this
 * sub-procedure. If the Characteristic Value write request is the
 * wrong size, or has an invalid value as defined by the profile,
 * then the write will not succeed and no error will be generated
 * by the server.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to command to be sent
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_WriteNoRsp( uint16 connHandle, attWriteReq_t *pReq );

/**
 * @brief   Signed Write No Response
 *
 * This sub-procedure is used to write a Characteristic Value
 *  to a server when the client knows the Characteristic Value
 *  Handle and the ATT Bearer is not encrypted. This sub-procedure
 *  shall only be used if the Characteristic Properties authenticated
 *  bit is enabled and the client and server device share a bond as
 *  defined in the GAP.
 *
 *  This sub-procedure only writes the first (ATT_MTU - 15) octets
 *  of an Attribute Value. This sub-procedure cannot be used to
 *  write a long Attribute.
 *
 *  @ref ATT_WriteReq is used for this sub-procedure. The
 *  Attribute Handle parameter shall be set to the Characteristic
 *  Value Handle. The Attribute Value parameter shall be set to
 *  the new Characteristic Value authenticated by signing the
 *  value, as defined in the Security Manager.
 *
 *  @note No response will be sent to the calling task for this
 *  sub-procedure. If the authenticated Characteristic Value that is
 *  written is the wrong size, or has an invalid value as defined by
 *  the profile, or the signed value does not authenticate the client,
 *  then the write will not succeed and no error will be generated by
 *  the server.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to command to be sent
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 * @return  bleLinkEncrypted: Connection is already encrypted.
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_SignedWriteNoRsp( uint16 connHandle, attWriteReq_t *pReq );

/**
 * @brief   Write Characteristic Value
 *
 * This sub-procedure is used to write a characteristic value
 * to a server when the client knows the characteristic value
 * handle. This sub-procedure only writes the first (ATT_MTU-3)
 * octets of a characteristic value. This sub-procedure can not
 * be used to write a long attribute; instead the Write Long
 * Characteristic Values sub-procedure should be used.
 *
 * @ref ATT_WriteReq is used in this sub-procedure. The
 * Attribute Handle parameter shall be set to the Characteristic
 * Value Handle. The Attribute Value parameter shall be set to
 * the new characteristic.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_WRITE_RSP if the procedure is successfull
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_WRITE_RSP
 * (with @ref SUCCESS or @ref bleTimeout status) or @ref ATT_ERROR_RSP (with
 * @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_WriteCharValue( uint16 connHandle, attWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   Write Long Characteristic Value
 *
 * This sub-procedure is used to write a Characteristic Value to
 * a server when the client knows the Characteristic Value Handle
 * but the length of the Characteristic Value is longer than can
 * be sent in a single Write Request Attribute Protocol message.
 *
 * @ref ATT_PrepareWriteReq and @ref ATT_ExecuteWriteReq are
 * used to perform this sub-procedure.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_WRITE_RSP and @ref ATT_EXECUTE_WRITE_RSP if the procedure is successfull
 * - @ref ATT_EXECUTE_WRITE_RSP of type
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_PREPARE_WRITE_RSP
 * (with @ref bleTimeout status), @ref ATT_EXECUTE_WRITE_RSP (with @ref SUCCESS
 * or @ref bleTimeout status), or @ref ATT_ERROR_RSP (with @ref SUCCESS status)
 * is received by the calling task.
 *
 * @warning The 'pReq->pValue' pointer will be freed when the sub-procedure is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_WriteLongCharValue( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   Do a Reliable Write
 *
 * This sub-procedure is used to write a Characteristic Value to
 * a server when the client knows the Characteristic Value Handle,
 * and assurance is required that the correct Characteristic Value
 * is going to be written by transferring the Characteristic Value
 * to be written in both directions before the write is performed.
 * This sub-procedure can also be used when multiple values must
 * be written, in order, in a single operation.
 *
 * The sub-procedure has two phases, the first phase prepares the
 * characteristic values to be written.  Once this is complete,
 * the second phase performs the execution of all of the prepared
 * characteristic value writes on the server from this client.
 * In the first phase, @ref ATT_PrepareWriteReq is used.
 * In the second phase, @ref ATT_ExecuteWriteReq is used.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_PREPARE_WRITE_RSP of type attPrepareWriteRsp_t and
 * @ref ATT_EXECUTE_WRITE_RSP if the procedure is successfull
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_PREPARE_WRITE_RSP
 * (with @ref bleTimeout status), @ref ATT_EXECUTE_WRITE_RSP (with @ref SUCCESS
 * or @ref bleTimeout status), or @ref ATT_ERROR_RSP (with @ref SUCCESS status)
 * is received by the calling task.
 *
 * @warning The 'pReqs' pointer will be freed when the sub-procedure is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReqs - pointer to requests to be sent (must be allocated)
 * @param   numReqs - number of requests in pReq
 * @param   flags - @ref ATT_ExecuteWriteReq flags
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReliableWrites( uint16 connHandle, attPrepareWriteReq_t *pReqs,
                                      uint8 numReqs, uint8 flags, uint8 taskId );
/**
 * @brief   Read Characteristic Descriptor
 *
 * This sub-procedure is used to read a characteristic descriptor
 * from a server when the client knows the characteristic descriptor
 * declaration's Attribute handle.
 *
 * @ref ATT_ReadReq is used for this sub-procedure. with the Attribute Handle
 * parameter set to the characteristic descriptor handle.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_READ_RSP of type attReadRsp_t if the procedure is successfull
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_RSP
 * (with @ref SUCCESS or @ref bleTimeout status) or @ref ATT_ERROR_RSP (with
 * @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReadCharDesc( uint16 connHandle, attReadReq_t *pReq, uint8 taskId );

/**
 * @brief   Read Long Characteristic Descriptor
 *
 * This sub-procedure is used to read a characteristic descriptor
 * from a server when the client knows the characteristic descriptor
 * declaration's Attribute handle and the length of the characteristic
 * descriptor declaration is longer than can be sent in a single Read
 * Response attribute protocol message.
 *
 *  @ref ATT_ReadBlobReq is used to perform this sub-procedure.
 *  The Attribute Handle parameter shall be set to the characteristic
 *  descriptor handle. The Value Offset parameter shall be the offset
 *  within the characteristic descriptor to be read.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_READ_BLOB_RSP of type attReadBlobRsp_t if the procedure is successfull
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_READ_BLOB_RSP
 * (with @ref bleProcedureComplete or @ref bleTimeout status) or @ref ATT_ERROR_RSP
 * (with @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_ReadLongCharDesc( uint16 connHandle, attReadBlobReq_t *pReq, uint8 taskId );

/**
 * @brief   Write Characteristic Descriptor
 *
 * This sub-procedure is used to write a characteristic
 * descriptor value to a server when the client knows the
 * characteristic descriptor handle.
 *
 * @ref ATT_WriteReq is used for this sub-procedure. The
 * Attribute Handle parameter shall be set to the characteristic
 * descriptor handle. The Attribute Value parameter shall be
 * set to the new characteristic descriptor value.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_WRITE_RSP if the procedure is successfull
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_WRITE_RSP
 * (with @ref SUCCESS or @ref bleTimeout status) or @ref ATT_ERROR_RSP (with
 * @ref SUCCESS status) is received by the calling task.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_WriteCharDesc( uint16 connHandle, attWriteReq_t *pReq, uint8 taskId );

/**
 * @brief   Write Long Characteristic Descriptor
 *
 * This sub-procedure is used to write a Characteristic Value to
 * a server when the client knows the Characteristic Value Handle
 * but the length of the Characteristic Value is longer than can
 * be sent in a single Write Request Attribute Protocol message.
 *
 * @ref ATT_PrepareWriteReq and @ref ATT_ExecuteWriteReq are
 * used to perform this sub-procedure.
 *
 * @par Corresponding Events:
 * If the return status from this function is @ref SUCCESS, the calling
 * application task will receive a @ref GATT_MSG_EVENT message with method:
 * - @ref ATT_PREPARE_WRITE_RSP of type @ref attPrepareWriteRsp_t and
 * @ref ATT_EXECUTE_WRITE_RSP if the procedure is successfull
 * - @ref ATT_ERROR_RSP of type @ref attErrorRsp_t ,if an error occurred on the server
 *
 * @note This sub-procedure is complete when either @ref ATT_PREPARE_WRITE_RSP
 * (with @ref bleTimeout status), @ref ATT_EXECUTE_WRITE_RSP (with @ref SUCCESS
 * or @ref bleTimeout status), or @ref ATT_ERROR_RSP (with @ref SUCCESS status)
 * is received by the calling task.
 *
 * @warning The 'pReq->pValue' pointer will be freed when the sub-procedure
 * is complete.
 *
 * @param   connHandle - connection to use
 * @param   pReq - pointer to request to be sent
 * @param   taskId - task to be notified of response
 *
 * @return  @ref SUCCESS : Request was queued successfully.
 * @return  @ref INVALIDPARAMETER
 * @return  @ref MSG_BUFFER_NOT_AVAIL
 * @return  @ref bleNotConnected
 * @return  @ref blePending : A response is pending with this server.
 * @return  @ref bleMemAllocError
 * @return  @ref bleTimeout : Previous transaction timed out.
 */
extern bStatus_t GATT_WriteLongCharDesc( uint16 connHandle, attPrepareWriteReq_t *pReq, uint8 taskId );

/*-------------------------------------------------------------------
 * GATT Client and Server Common APIs
 */

/**
 * @brief   GATT Register for Messages
 *
 * Register your task ID to receive GATT local events and ATT Response
 *          messages pending for transmission. When the GATT Server fails to
 *          respond to an incoming ATT Request due to lack of HCI Tx buffers,
 *          the response will be forwarded the app for retransmission.
 *
 *          Note: A pending ATT Response message forwarded to the app should be
 *                transmitted using GATT_SendRsp(), or its payload freed using
 *                GATT_bm_free() if not transmitted.
 *
 * @param   taskID - Default task ID to send GATT events/pending ATT responses.
 */
extern void GATT_RegisterForMsgs( uint8 taskID );

/**
 * @brief   Send an event to upper layer application/protocol.
 *
 * @param   connHandle - connection event belongs to
 * @param   status - status
 * @param   method - type of message
 * @param   pMsg - pointer to message to be sent
 *
 * @return  @ref SUCCESS
 * @return  @ref INVALIDPARAMETER
 * @return  @ref bleMemAllocError
 */
extern bStatus_t GATT_NotifyEvent( uint16 connHandle, uint8 status, uint8 method, gattMsg_t *pMsg );

/**
 * Notify the stack of an updated MTU size for a given connection.
 *
 * @note It is theoretically possible for the stack to be sucesfully notified of 
 * the MTU update but have the subsequent notification event sent to the
 * registered applicaiton GATT task fail due to either a memory allocation 
 * failure or if no task was registered with @ref GATT_RegisterForMsgs. In this
 * case, SUCESSS will still be returned from this function.
 *
 * @param   connHandle - connection handle.
 * @param   mtuSize - new MTU size.
 *
 * @return SUCCESS Stack was notified of updated MTU
 * @return FAILURE invalid MTU size or connection not found
 */

extern bStatus_t GATT_UpdateMTU( uint16 connHandle, uint16 mtuSize );

/*-------------------------------------------------------------------
 * GATT Buffer Management APIs
 */

/**
 * @brief   GATT implementation of the allocator functionality.
 *
 * @note
 * This function should only be called by GATT and
 *                the upper layer protocol/application.
 *
 * @param   connHandle - connection that message is to be sent on.
 * @param   opcode - opcode of message that buffer to be allocated for.
 * @param   size - number of bytes to allocate from the heap.
 * @param   pSizeAlloc - number of bytes allocated for the caller from the heap.
 *
 * @return pointer to the heap allocation
 * @return NULL if error or failure.
 */
extern void *GATT_bm_alloc( uint16 connHandle, uint8 opcode, uint16 size, uint16 *pSizeAlloc );

/**
 * @brief   GATT implementation of the de-allocator functionality.
 *
 * @param   pMsg - pointer to GATT message containing the memory to free.
 * @param   opcode - opcode of the message
 */
extern void GATT_bm_free( gattMsg_t *pMsg, uint8 opcode );

/// @cond NODOC

/*-------------------------------------------------------------------
 * GATT Flow Control APIs
 */

/**
 * @brief   This API is used by the Application to turn flow control on
 *          or off for GATT messages sent from the Host to the Application.
 *
 *          Note: If the flow control is enabled then the Application must
 *                call the GATT_AppCompletedMsg() API when it completes
 *                processing an incoming GATT message.
 *
 * @param   heapSize - internal heap size
 * @param   flowCtrlMode - flow control mode: TRUE or FALSE
 *
 * @return  void
 */
extern void GATT_SetHostToAppFlowCtrl( uint16 heapSize, uint8 flowCtrlMode );

/**
 * @brief   This API is used by the Application to notify GATT that
 *          the processing of a message has been completed.
 *
 * @param   pMsg - pointer to the processed GATT message
 *
 * @return  void
 */
extern void GATT_AppCompletedMsg( gattMsgEvent_t *pMsg );

/*-------------------------------------------------------------------
 * Internal API - This function is only called from GATT Qualification modules.
 */

  /**
   * @internal
   *
   * @brief       Set the next available attribute handle.
   *
   * @param       handle - next attribute handle.
   */
extern void GATT_SetNextHandle( uint16 handle );

  /**
   * @internal
   *
   * @brief       Return the next available attribute handle.
   *
   * @return      next attribute handle
   */
extern uint16 GATT_GetNextHandle( void );

/*-------------------------------------------------------------------
 * Internal API - This function is called by OSAL tasks.
 */

  /**
   * @internal
   *
   * @brief       OSAL task may use this to request the next available
   *              ATT Transaction on specified connection handle.
   *
   * @param       connHandle - connection handle
   * @param       taskID     - OSAL task ID.
   *
   * @return  SUCCESS: Request was sent successfully.
   *          INVALIDPARAMETER: Invalid connection handle or request field.
   *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.
   *          bleNotConnected: Connection is down.
   *          blePending: A response is pending with this server.
   *          bleMemAllocError: Memory allocation error occurred.
   *          bleTimeout: Previous transaction timed out.
   *          bleNoResources: next transaction event already claimed.
   */
extern bStatus_t GATT_RequestNextTransaction( uint16 connHandle, uint8 taskId );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by OSAL.
 */

/**
 * @internal
 *
 * @brief   GATT Task initialization function.
 *
 * @param   taskId - GATT task ID.
 *
 * @return  void
 */
extern void GATT_Init( uint8 taskId );

/**
 * @internal
 *
 * @brief   GATT Task event processing function.
 *
 * @param   taskId - GATT task ID
 * @param   events - GATT events.
 *
 * @return  events not processed
 */
extern uint16 GATT_ProcessEvent( uint8 taskId, uint16 events );

/// @endcond // NODOC

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GATT_H */

/** @} */ // end of ATT_GATT
