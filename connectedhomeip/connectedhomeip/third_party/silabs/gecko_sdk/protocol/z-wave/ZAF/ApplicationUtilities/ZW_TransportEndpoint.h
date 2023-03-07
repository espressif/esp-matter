/**
 * @file
 * @brief Transport layer for Multi Channel endpoint functionality.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _ZW_TRANSPORTENDPOINT_H_
#define _ZW_TRANSPORTENDPOINT_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_classcmd.h>
#include <ZW_security_api.h>
#include <ZW_transport_api.h>
#include <ZAF_types.h>
/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/
#define RES_ZERO 0

/**
* Defines return values for Transport_SendResponseEP and Transport_SendRequestEP
* methods. They match the enum values defined in EQueueNotifyingStatus (i.e., 
* EQUEUENOTIFYING_STATUS_SUCCESS and EQUEUENOTIFYING_STATUS_TIMEOUT, respectively).
*/
typedef enum
{
  ZAF_ENQUEUE_STATUS_SUCCESS = 0, /**< Item successfully put on queue */
  ZAF_ENQUEUE_STATUS_TIMEOUT = 1, /**< Failed to put item on queue as it was full. Queue remained full until timeout */
  ZAF_ENQUEUE_STATUS_BUFFER_OVERRUN = 2  /**< Failed to put item on queue as it was to long to fit in tx buffer */
} EZAF_EnqueueStatus_t;

/**
 * MULTICHAN_NODE_INFO should include feature node informations as CRC, security etc.
 */
typedef struct _MULTICHAN_NODE_INFO_
{
  uint8_t BitMultiChannelEncap; /**< uint8_t 0, bit 0 */
  security_key_t security; /**< of type security_key_t*/
} MULTICHAN_NODE_INFO;

typedef struct _MULTICHAN_NODE_INFO_PACKED_
{
  uint8_t security             : 4; /**< bit 0-3 of type security_key_t: 0-NON_KEY,1-S2_UNAUTHENTICATED,
                                      2-S2_AUTHENTICATED, 3-S2_ACCESS, 4-S0 (security_key_t)*/
  uint8_t unused               : 1; /**< bit 4 */
  uint8_t BitMultiChannelEncap : 1; /**< bit 5 */
  uint8_t unused2              : 1; /**< bit 6 */
  uint8_t unused3              : 1; /**< bit 7 */
} MULTICHAN_NODE_INFO_PACKED;

/**
 * Association node data field in a association group.
 */
typedef struct _MULTICHAN_NODE_ID_
{
  MULTICHAN_DEST_NODE_ID node;
  MULTICHAN_NODE_INFO    nodeInfo;
}
destination_info_t;

typedef destination_info_t MULTICHAN_NODE_ID;

typedef struct _MULTICHAN_NODE_ID_PACKED_
{
  MULTICHAN_DEST_NODE_ID_8bit  node;    // We cannot store 16bit NodeIDs in the NVM, because of backwards compatibility!
  MULTICHAN_NODE_INFO_PACKED   nodeInfoPacked;
} MULTICHAN_NODE_ID_PACKED;

/* Transmit options for ZW_SendDataEx */
typedef struct _TRANSMIT_OPTIONS_TYPE_EX_ {
  uint8_t S2_groupID;
  uint8_t txOptions;
  uint8_t sourceEndpoint;
  MULTICHAN_NODE_ID *pList;
  uint8_t list_length;
} TRANSMIT_OPTIONS_TYPE_EX;

/* Transmit options for ZW_SendDataEx */
typedef struct _TRANSMIT_OPTIONS_TYPE_SINGLE_EX_ {
  uint8_t txOptions;
  uint8_t txSecOptions; /**< Options for enabling specific Security scheme functionality */
  uint8_t sourceEndpoint;
  MULTICHAN_NODE_ID* pDestNode;
} TRANSMIT_OPTIONS_TYPE_SINGLE_EX;

typedef enum _ENDPOINT_
{
  ENDPOINT_ROOT = 0,
  ENDPOINT_1,
  ENDPOINT_2,
  ENDPOINT_3,
  ENDPOINT_4,
  ENDPOINT_5,
  ENDPOINT_6,
  ENDPOINT_7,
  ENDPOINT_8,
  ENDPOINT_9
} ENDPOINT;


/**
 * struct includes point to list and size of list
 */
typedef struct _CMD_CLASS_LIST_
{
  uint8_t* pList;
  uint8_t size;
} CMD_CLASS_LIST;

/**
 * struct include un-secure and secure cmd class list
 */
typedef struct _CMD_CLASS_LIST_GRP_
{
  CMD_CLASS_LIST unsecList;
  CMD_CLASS_LIST secList;
}CMD_CLASS_LIST_GRP;

/**
 * struct include un-secure and secure cmd class list
 */
typedef struct _CMD_CLASS_LIST_3_LIST_
{
  CMD_CLASS_LIST unsecList;
  CMD_CLASS_LIST_GRP sec;
}CMD_CLASS_LIST_3_LIST;

/**
 * Struct for an Endpoint DEVICE_CLASS.
 */
typedef struct _DEVICE_CLASS_
{
  uint8_t genericDeviceClass;
  uint8_t specificDeviceClass;
} DEVICE_CLASS;

/**
 * Struct for an Endpoint NIF.
 */
typedef struct _EP_NIF_
{
  uint8_t genericDeviceClass;
  uint8_t specificDeviceClass;
  CMD_CLASS_LIST_3_LIST CmdClass3List;
} EP_NIF;

/**
 * Struct for setting up aggregation endpoint group bit-mask.
 */
typedef struct _AGGREGATED_GROUP_BIT_MASK_
{
  uint8_t aggregatedEndpoint;
  uint8_t len;
  uint8_t* pBitMask;
} AGGREGATED_GROUP_BIT_MASK;


/**
 * This bit MUST be set to 1 if all End Points advertise the same generic and specific
   device class and support the same optional command classes. The bit MUST be set to 0 if
   End Points do not advertise the same device class and command class information.
 */
typedef enum _ENDPOINT_IDENTICAL_DEVICE_CLASS_
{
  ENDPOINT_IDENTICAL_DEVICE_CLASS_NO = 0,
  ENDPOINT_IDENTICAL_DEVICE_CLASS_YES = 1
}ENDPOINT_IDENTICAL_DEVICE_CLASS;


/**
 * This field is used to advertise if the node implements a dynamic number of End Points.
 * The value 1 MUST be used to indicate that the number of End Points is dynamic. The value
 * 0 MUST be used to indicate that the number of End Points is static.
 */
typedef enum _ENDPOINT_DYNAMIC_
{
  ENDPOINT_DYNAMIC_NO = 0,
  ENDPOINT_DYNAMIC_YES = 1
} ENDPOINT_DYNAMIC;


/**
 * The Multi Channel End Point functionality is used to advertise the number
 * of End Points implemented by the node. See description of doc. SDS12657
 * chapter Multi Channel End Point Report Command
 */
struct _EP_FUNCTIONALITY_
{
  uint8_t nbrIndividualEndpoints : 7;  /**< B0, Number of End Points implemented by the node. The value MUST be in the range 1..127 */
  uint8_t resIndZeorBit   : 1;  /**< B0, This field MUST be set to 0*/
  uint8_t nbrAggregatedEndpoints : 7;  /**< B0, Number of End Points implemented by the node. The value MUST be in the range 1..127 */
  uint8_t resAggZeorBit   : 1;  /**< B0, This field MUST be set to 0*/
  uint8_t resZero      : 6;  /**< B1, This field MUST be set to 0*/
  ENDPOINT_IDENTICAL_DEVICE_CLASS identical : 1; /**< B1, of enum type ENDPOINT_IDENTICAL_DEVICE_CLASS*/
  ENDPOINT_DYNAMIC dynamic: 1;  /**< B1, of enum type ENDPOINT_DYNAMIC*/
};

/**
 * The Multi Channel End Point functionality encpsulation
 */
typedef union _EP_FUNCTIONALITY_DATA_
{
  struct _EP_FUNCTIONALITY_ bits;
  uint8_t   bDdata[3];
} EP_FUNCTIONALITY_DATA;



/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief Initializes the transport endpoint layer. It should be called from
 * ApplicationPoll() in the application.
 * @param pTxQueueNotifying Queue for posting frames for transmission to protocol.
 * @param updateStayAwakePeriodFunc Function that will be invoked on completion
 * of a request or response. Used by battery operated devices to set the period
 * to stay awake after those activities.
 * If mains powered, set to NULL.
 */
void ZW_TransportEndpoint_Init(
    void* pTxQueueNotifying,
    void (*updateStayAwakePeriodFunc)(void)
);

/**
 * @brief Initialization of endpoint NIF's. Each endpoint  have only one command class list
 * that is present for device security level. Framework will add non secure command class list
 * if device is secure include. Dependent ofr secure level contains the list CC security and security 2.
 * This function must be called after Transport_OnApplicationInitSW.
 * @param[in] pFunctionality of type EP_FUNCTIONALITY_DATA
 * @param[in] pList is a pointer of type EP_LIST to all endpoints NIF's.
 * @param[in] sizeList is number of endpoints NIF's pList points to.
 */
//@ [Transport_AddEndpointSupport]
void Transport_AddEndpointSupport(
    EP_FUNCTIONALITY_DATA* pFunctionality,
    EP_NIF* pList,
    uint8_t sizeList);
//@ [Transport_AddEndpointSupport]


/**
 * @brief Setup aggregation groups
 * Example including two aggregation endpoints:
 * NUMBER_OF_AGGREGATED_ENDPOINTS = 2;
 * uint8_t ep4_AGG_Group[2] = {0x03};      //bit mask for endpoints: 1 and 2
 * uint8_t ep5_AGG_Group = 0x07;           //bit mask for endpoint 1, 2 and 3
 * AGGREGATED_GROUP_BIT_MASK maskarray[NUMBER_OF_AGGREGATED_ENDPOINTS] =
 * {
 *  {ENDPOINT_4, sizeof(ep4_AGG_Group), &ep4_AGG_Group },
 *  {ENDPOINT_5, sizeof(ep5_AGG_Group), &ep5_AGG_Group }
 * };
 * Transport_SetupAggregationGroups( aggGroup, NUMBER_OF_AGGREGATED_ENDPOINTS);
 *
 * @param pMaskArray pointer to array of type AGGREGATED_GROUP_BIT_MASK
 * @param sizeArray size of array
 * @return number of Aggregated Members Bit Mask bytes
 */
void
Transport_SetupAggregationGroups(AGGREGATED_GROUP_BIT_MASK* pMaskArray, uint8_t sizeArray);


/**
 * @brief Read Node endpoint functionality
 * @param pFunc pointer of type EP_FUNCTIONALITY.
 */
void
GetMultiChannelEndPointFunctionality(EP_FUNCTIONALITY_DATA* pFunc);


/**
 * @brief Return endpoint device class.
 * @param[in] endpoint parameter
 * @return pointer of type DEVICE_CLASS!
 */
DEVICE_CLASS*
GetEndpointDeviceClass( uint8_t endpoint);


/**
 * @brief Get endpoint non-secure or secure command class list.
 * @param[in] secList true if secure list else false for non-secure list
 * @param[in] endpoint parameter
 * @return Return endpoint command class list. Return NULL if list is not present!
 */
CMD_CLASS_LIST*
GetEndpointcmdClassList( bool secList, uint8_t endpoint);

/**
 * @brief Search for next specified set of generic and specific
 * device class in End Points.
 * @param[in] genDeviceClass generic device class parameter
 * @param[in] specDeviceClass specific device class parameter
 * @param[out] pEp endpoint list pointer
 * @return endpoint. 0 if no endpoint.
 */
uint8_t FindEndPoints( uint8_t genDeviceClass, uint8_t specDeviceClass, uint8_t* pEp);


/**
 * @brief Send data request call. Encapsulate data for endpoint support and call protocol ZW_SendDataEx.
 * @param[in] pData IN Data buffer pointer.
 * @param[in] dataLength IN Data buffer length.
 * @param[in] pTxOptionsEx transmit options pointer.
 * @param[out] pCallback is a callback function-pointer returning result of the job.
 * @return status of enum type EZAF_EnqueueStatus_t
 */
EZAF_EnqueueStatus_t
Transport_SendRequestEP(
  uint8_t *pData,
  size_t   dataLength,
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult));


/**
 * @brief Send data response call. Encapsulate data for endpoint support and call protocol ZW_SendDataEx.
 * @param[in] pData IN Data buffer pointer.
 * @param[in] dataLength IN Data buffer length.
 * @param[in] pTxOptionsEx transmit options pointer.
 * @param[out] pCallback is a callback function-pointer returning result of the job. Please note that both 
 *             bStatus TRANSMIT_COMPLETE_OK and TRANSMIT_COMPLETE_VERIFIED indicates succesful transmission.
 * @return status of enum type EZAF_EnqueueStatus_t
 */
EZAF_EnqueueStatus_t
Transport_SendResponseEP(
  uint8_t *pData,
  size_t   dataLength,
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx,
  VOID_CALLBACKFUNC(pCallback)(uint8_t bStatus));

/**
 * Invokes a command class handler based on the first byte in the frame.
 *
 * Must be defined in the application.
 *
 * @deprecated If all command classes use one of the REGISTER_CC macros, this function is not
 *             required to be implemented in the application. There exists a weak definition of it.
 *             ZAF_CC_Invoker will take care of invoking the right command class handler.
 *
 * @param[in] rxOpt IN receive options of type RECEIVE_OPTIONS_TYPE_EX.
 * @param[in] pCmd IN  Payload from the received frame.
 * @param[in] cmdLength IN  Number of command bytes including command.
 * @return status of extracting frame
 */
received_frame_status_t
Transport_ApplicationCommandHandlerEx(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t   cmdLength);


/**
 * @brief Helper function to switching endpoints for SendData.
 * @param[in] rxopt Receive options to convert.
 * @param[out] txopt Converted transmit options
 */
void
RxToTxOptions( RECEIVE_OPTIONS_TYPE_EX *rxopt,
               TRANSMIT_OPTIONS_TYPE_SINGLE_EX **txopt);


/**
 * @brief Validate destination endpoint use bit-addressing. This function must only
 * be used to check jobs that return response (Get <-> Report).
 * @param[in] rxOpt pointer of type RECEIVE_OPTIONS_TYPE_EX
 * @return Boolean true if use bit-addressing else false
 */
bool
Check_not_legal_response_job(RECEIVE_OPTIONS_TYPE_EX *rxOpt);


/**
 * @brief This function indicates if the frame was received using multicast addressing
 * The Z-Wave Multicast frame, the broadcast NodeID and the Multi Channel multi-End
 * Point destination are all considered multicast addressing methods.
 * 
 * @param[in] rxOpt pointer of type RECEIVE_OPTIONS_TYPE_EX
 * @return Boolean true if use bit-addressing else false
 */
bool is_multicast(RECEIVE_OPTIONS_TYPE_EX *rxOpt);

/**
 * @brief Read members of an aggregated endpoint
 * @param[in] aggregatedEndpoint endpoint number
 * @param[out] pAggBitMask pointer to bitmask
 * @return number of Aggregated Members Bit Mask bytes
 */
uint8_t
ReadAggregatedEndpointGroup( uint8_t aggregatedEndpoint, uint8_t* pAggBitMask);

/**
 * @brief Set supervision-encapsulated-frame flag. The Command class call Check_not_legal_response_job()
 * Use the flag to discard current job if a Get<->Response job (CC:006C.01.00.13.002).
 * @param[in] flag supervision-encapsulated-frame true if active and false if deactive.
 */
void
SetFlagSupervisionEncap(bool flag);

#endif /* _ZW_TRANSPORTENDPOINT_H_ */


