/**
* @file ZW_TransportEndpoint.c
* @copyright 2018 Silicon Laboratories Inc.
* @brief Transport layer for Multi Channel endpoint functionality
*
*/

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <string.h>
#include <ZW_transport_api.h>
#include <multichannel.h>
#include <ZW_TransportLayer.h>
#include <misc.h>
//#define DEBUGPRINT
#include "DebugPrint.h"
#include <ZW_TransportEndpoint.h>
#include <ZW_application_transport_interface.h>
#include <ZAF_Common_interface.h>
#include <QueueNotifying.h>
#include "ZAF_types.h"
#include "ZAF_CC_Invoker.h"

/* Ensuring that we use the same enum definition for EZAF_EnqueueStatus_t and EQueueNotifyingStatus */
STATIC_ASSERT((ZAF_ENQUEUE_STATUS_SUCCESS == (EZAF_EnqueueStatus_t)EQUEUENOTIFYING_STATUS_SUCCESS) &&
              (ZAF_ENQUEUE_STATUS_TIMEOUT == (EZAF_EnqueueStatus_t)EQUEUENOTIFYING_STATUS_TIMEOUT),
              STATIC_ASSERT_FAILED_EZAF_EnqueueStatus_t_define_error);

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * @attention Transmissions with follow ups are able to fill the buffers configured here or in app.
 *
 * The number of transmissions we can queue with the transport service is limited by the queue size indicated with
 * CALLBACK_FUNCTION_QUEUE_SIZE and that of the QueueNotifying.h module allocated by the app. If filled, subsequent
 * transmissions are avoided and the segmentation process is abruptly halted. This leads to an disrupted/incomplete
 * segmentation that does not see a termination. The receiver will timeout and discard all previously received frames
 * for that transaction.
 *
 * CALLBACK_FUNCTION_QUEUE_SIZE must always be greater or equal to APP_EVENT_QUEUE_SIZE that is defined in the app.
 * Currently set to 5 in the example apps, but customers may have lowered this.
 */

#define BIT_FIELD_SIZE  3     /* Number can be adjusted. It should be at least big enough so that
                               * CALLBACK_FUNCTION_QUEUE_SIZE is greater than max associations in a group.
                               * It should also be big enough to store the max number of reports_to_follow + 1
                               * for transmissions that needs it. */
#define CALLBACK_FUNCTION_QUEUE_SIZE   (1 << BIT_FIELD_SIZE)

#define QUEUE_NOTIFYING_SEND_MAX_WAIT  4000  /* [ms] Due to limitations in RAM, the allocated amount of frames with
                                              * follow-ups are limited to the queue size determined by application with
                                              * APP_EVENT_QUEUE_SIZE, and this will halt certain operations if
                                              * QUEUE_NOTIFYING_SEND_MAX_WAIT is set to zero.
                                              * See the detailed description above. */

typedef struct {
  uint8_t head : BIT_FIELD_SIZE; // Head of queue index
  uint8_t tail : BIT_FIELD_SIZE; // Tail of queue index
  uint8_t numInQueue;            // Number of elements pending in queue
  void    (*pCallback[CALLBACK_FUNCTION_QUEUE_SIZE])(TRANSMISSION_RESULT *pTransmissionResult); // Queue elements
} RequestCallbackFunctionQueue_t;

typedef struct {
  uint8_t head : BIT_FIELD_SIZE; // Head of queue index
  uint8_t tail : BIT_FIELD_SIZE; // Tail of queue index
  uint8_t numInQueue;            // Number of elements pending in queue
  void    (*pCallback[CALLBACK_FUNCTION_QUEUE_SIZE])(uint8_t status); // Queue elements
} ResponseCallbackFunctionQueue_t;

RequestCallbackFunctionQueue_t  m_RequestCallbackQueue;
ResponseCallbackFunctionQueue_t m_ResponseCallbackQueue;

static void ZCB_RequestCompleted(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus);
#ifndef HOST_SECURITY_INCLUDED
static void ZCB_ResponseCompleted(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus);
#endif

typedef struct {
  uint8_t flag_supervision_encap;
  EP_FUNCTIONALITY_DATA* pFunctionality;
  uint8_t sizeList;
  EP_NIF* pList;
  AGGREGATED_GROUP_BIT_MASK* pMaskArray;
  uint8_t sizeAggGroupArray;
  CMD_CLASS_LIST emptyList;
} CTransportEndpoint_t;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/
static CTransportEndpoint_t myTransportEndpoint =
{
  false,
  (EP_FUNCTIONALITY_DATA*)NULL,
  0,
  (EP_NIF*)NULL,
  (AGGREGATED_GROUP_BIT_MASK*)NULL,
  0,
  {(uint8_t*)NULL, 0}};


// Queue for posting frames for transmission to protocol
static SQueueNotifying* m_pTxQueueNotifying;

static VOID_CALLBACKFUNC(m_cbUpdateStayAwakePeriod)();

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

void ZW_TransportEndpoint_Init(void * pTxQueueNotifying,
                               void (*updateStayAwakePeriodFunc)(void))
{
  UNUSED(pTxQueueNotifying);

  m_pTxQueueNotifying = ZAF_getZwTxQueue();
  m_cbUpdateStayAwakePeriod = updateStayAwakePeriodFunc;
  m_RequestCallbackQueue.head        = 0;
  m_RequestCallbackQueue.tail        = 0;
  m_RequestCallbackQueue.numInQueue  = 0;
  m_ResponseCallbackQueue.head       = 0;
  m_ResponseCallbackQueue.tail       = 0;
  m_ResponseCallbackQueue.numInQueue = 0;
}

/*============================ Transport_AddEndpointSupport ================
** Function description
** This function...
**
** Side effects:
**
**-------------------------------------------------------------------------*/
void
Transport_AddEndpointSupport( EP_FUNCTIONALITY_DATA* pFunctionality, EP_NIF* pList, uint8_t sizeList)
{
  myTransportEndpoint.pFunctionality = pFunctionality;
  myTransportEndpoint.pList = pList;
  myTransportEndpoint.sizeList = sizeList;
  myTransportEndpoint.pMaskArray = NULL;
  myTransportEndpoint.sizeAggGroupArray = 0;
  myTransportEndpoint.flag_supervision_encap = false;
}


/*============================ Transport_SetupAggregationGroups =============
** Function description
** This function...
**
** Side effects:
**
**-------------------------------------------------------------------------*/
void
Transport_SetupAggregationGroups(AGGREGATED_GROUP_BIT_MASK* pMaskArray, uint8_t sizeArray)
{
  if(myTransportEndpoint.pFunctionality->bits.nbrAggregatedEndpoints == sizeArray)
  {
    myTransportEndpoint.pMaskArray = pMaskArray;
    myTransportEndpoint.sizeAggGroupArray = sizeArray;
  }
}

/*============================ GetMultiChannelEndPointFunctionality ===============================
** Function description
** Read Node endpoint functionality
**
** return pointer to functionality of type EP_FUNCTIONALITY.
**
**-------------------------------------------------------------------------*/
void
GetMultiChannelEndPointFunctionality(EP_FUNCTIONALITY_DATA* pFunc)
{
  if (NULL != myTransportEndpoint.pFunctionality)
  {
    pFunc->bDdata[0] = (uint8_t)(myTransportEndpoint.pFunctionality->bits.resZero |
                                (myTransportEndpoint.pFunctionality->bits.identical << 6) |
                                (myTransportEndpoint.pFunctionality->bits.dynamic << 7));
    pFunc->bDdata[1] = (uint8_t)(myTransportEndpoint.pFunctionality->bits.nbrIndividualEndpoints |
                                (myTransportEndpoint.pFunctionality->bits.resIndZeorBit << 7));
    pFunc->bDdata[2] = (uint8_t)(myTransportEndpoint.pFunctionality->bits.nbrAggregatedEndpoints |
                                (myTransportEndpoint.pFunctionality->bits.resAggZeorBit << 7));
  }

}


uint8_t eKeyToBitKey[] =
{
  0x00, SECURITY_KEY_S2_UNAUTHENTICATED_BIT, SECURITY_KEY_S2_AUTHENTICATED_BIT, SECURITY_KEY_S2_ACCESS_BIT, SECURITY_KEY_S0_BIT
};

uint8_t ConvertEnumKeytoBitKey(enum SECURITY_KEY eKey)
{
  return eKeyToBitKey[eKey];
}

/*============================ GetEndpointDeviceClass =======================
** Function description
** This function...
**
**-------------------------------------------------------------------------*/
DEVICE_CLASS*
GetEndpointDeviceClass( uint8_t endpoint)
{
  static DEVICE_CLASS devClass;

  if( (endpoint > myTransportEndpoint.sizeList) || (0 == endpoint) )
  {
    return NULL;
  }

  devClass.genericDeviceClass = myTransportEndpoint.pList[endpoint - 1].genericDeviceClass;
  devClass.specificDeviceClass = myTransportEndpoint.pList[endpoint - 1].specificDeviceClass;

  return &devClass;
}


/*============================ GetEndpointcmdClassList ======================
**-------------------------------------------------------------------------*/
CMD_CLASS_LIST*
GetEndpointcmdClassList( bool secList, uint8_t endpoint)
{
  if( (endpoint > myTransportEndpoint.sizeList) || (0 == endpoint) )
  {
    return NULL;
  }

  if(SECURITY_KEY_NONE == GetHighestSecureLevel(ZAF_GetSecurityKeys()))
  {
    /*Non-secure included*/
    if(false == secList){
      return &(myTransportEndpoint.pList[endpoint - 1].CmdClass3List.unsecList);
    }
    else {
      return &(myTransportEndpoint.emptyList);
    }
  }
  else{
    /*Secure included*/
    if(false == secList){
      return &(myTransportEndpoint.pList[endpoint - 1].CmdClass3List.sec.unsecList);
    }
    else {
      return &(myTransportEndpoint.pList[endpoint - 1].CmdClass3List.sec.secList);
    }
  }
  return NULL;
}


/*============================ FindNextEndPoint ===============================
** Function description
** Search for next specified set of generic and specific
** device class in End Points.
** param genDeviceClass is endpoint generic device class
** param specDeviceClass is endpoint specific device class
** return endpoint. 0 if no endpoint.
**-------------------------------------------------------------------------*/
uint8_t
FindEndPoints( uint8_t genDeviceClass, uint8_t specDeviceClass, uint8_t* pEp)
{
  uint8_t i;
  uint8_t len =0;

  for(i = 0; i < myTransportEndpoint.sizeList; i++)
  {
    if( ((0xFF == genDeviceClass) && (0xFF == specDeviceClass) ) ||
        ((genDeviceClass == myTransportEndpoint.pList[i].genericDeviceClass) &&
         ((specDeviceClass == myTransportEndpoint.pList[i].specificDeviceClass) ||
         (specDeviceClass == 0xFF)))
      )
    {
      pEp[len++] = i + 1; /* Add one endpoint is from 1 to 127*/
    }
  }
  if( 0 == len)
  {
    /* deliver an endpoint list with EP1 = 0 */
    len = 1;
    pEp[0] = 0;
  }
  return len;
}


/*============================ GetAggregatedEndpointGroup ===================
** Function description
** Read members of an aggregated endpoint
**
** Side effects:
** Return number of Aggregated Members Bit Mask bytes
**-------------------------------------------------------------------------*/
uint8_t
ReadAggregatedEndpointGroup( uint8_t aggregatedEndpoint, uint8_t* pAggBitMask)
{
  uint8_t aggBitMaskSize = 0;
  uint32_t i = 0;
  /* Find aggregated Endpoint */

  for(i = 0; i < myTransportEndpoint.sizeAggGroupArray; i++)
  {
    if(aggregatedEndpoint == myTransportEndpoint.pMaskArray[i].aggregatedEndpoint)
    {
      /* Copy data bit mask*/
      memcpy(pAggBitMask, myTransportEndpoint.pMaskArray[i].pBitMask, myTransportEndpoint.pMaskArray[i].len);
      aggBitMaskSize = myTransportEndpoint.pMaskArray[i].len;
      break;
    }
  }
  return aggBitMaskSize;
}

EZAF_EnqueueStatus_t
Transport_SendRequestEP(
  uint8_t *pData,
  size_t   dataLength,
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx,
  VOID_CALLBACKFUNC(pCallback)(TRANSMISSION_RESULT * pTransmissionResult))
{
  EZAF_EnqueueStatus_t result;

  // Check for multi channel
  if(false == pTxOptionsEx->pDestNode->nodeInfo.BitMultiChannelEncap)
  {
    pTxOptionsEx->sourceEndpoint = 0;
  }

  /*
   * Save the callback function pointer to a variable in order to call it in
   * ZCB_RequestCompleted.
   * We store it in a small queue (ring buffer) to be able to have multiple
   * ongoing requests.
   */
  if (m_RequestCallbackQueue.numInQueue >= CALLBACK_FUNCTION_QUEUE_SIZE)
  {
    DPRINTF("\r\n%s: Callback function queue full!\r\n", __func__);
    ASSERT(0);  // FATAL: Raise the number of elements in array, CALLBACK_FUNCTION_QUEUE_SIZE.
    return ZAF_ENQUEUE_STATUS_TIMEOUT; // Failure to process the request
  }
  m_RequestCallbackQueue.pCallback[m_RequestCallbackQueue.head] = pCallback;

  //Safeguard against buffer overflow
  if (TX_BUFFER_SIZE < dataLength)
  {
    return ZAF_ENQUEUE_STATUS_BUFFER_OVERRUN;
  }

#ifdef HOST_SECURITY_INCLUDED
  UNUSED(pCallback);
  SZwaveTransmitPackage TransmitPackage;
  memset(&TransmitPackage, 0, sizeof(TransmitPackage));
  TransmitPackage.eTransmitType = EZWAVETRANSMITTYPE_SECURE;
  SSecureSendData *params = &TransmitPackage.uTransmitParams.SendDataParams;
  params->connection.remote.is_multicast = false;
  params->data_length = dataLength;
  memcpy(params->data, pData, params->data_length);
  params->tx_options.number_of_responses = 0;
  params->ptxCompleteCallback = (void *)ZCB_RequestCompleted;
  params->connection.remote.address.node_id = pTxOptionsEx->pDestNode->node.nodeId;
  result = (EZAF_EnqueueStatus_t)QueueNotifyingSendToBack(m_pTxQueueNotifying, (uint8_t *)&TransmitPackage, 0);
#else
  CmdClassMultiChannelEncapsulate(&pData,
                                  &dataLength,
                                  pTxOptionsEx);


  SZwaveTransmitPackage FramePackage;
  FramePackage.uTransmitParams.SendDataEx.DestNodeId = pTxOptionsEx->pDestNode->node.nodeId;
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.Handle = ZCB_RequestCompleted;
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.TransmitOptions = pTxOptionsEx->txOptions;
  memcpy(FramePackage.uTransmitParams.SendDataEx.FrameConfig.aFrame, pData, dataLength);
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.iFrameLength = (uint8_t)dataLength;
  FramePackage.uTransmitParams.SendDataEx.SourceNodeId = 0x0000;
  FramePackage.uTransmitParams.SendDataEx.TransmitOptions2 = 0x00;
  FramePackage.uTransmitParams.SendDataEx.TransmitSecurityOptions = (pTxOptionsEx->txSecOptions | S2_TXOPTION_VERIFY_DELIVERY); /**<Always set verify options on request job*/
  FramePackage.uTransmitParams.SendDataEx.eKeyType = pTxOptionsEx->pDestNode->nodeInfo.security;

  FramePackage.eTransmitType = EZWAVETRANSMITTYPE_EX;

  // Put the package on queue (and don't wait for it)
  result = (EZAF_EnqueueStatus_t)QueueNotifyingSendToBack(m_pTxQueueNotifying, (uint8_t*)&FramePackage, QUEUE_NOTIFYING_SEND_MAX_WAIT);
#endif
  if (ZAF_ENQUEUE_STATUS_SUCCESS == result)
  {
    // Success. Advance the callback function queue counters
    m_RequestCallbackQueue.head++;
    m_RequestCallbackQueue.numInQueue++;
  }
  return result;
}


EZAF_EnqueueStatus_t
Transport_SendResponseEP(
  uint8_t *pData,
  size_t   dataLength,
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX *pTxOptionsEx,
  VOID_CALLBACKFUNC(pCallback)(uint8_t bStatus))
{
  EZAF_EnqueueStatus_t result;

#ifdef HOST_SECURITY_INCLUDED
  UNUSED(pCallback);
  SZwaveTransmitPackage TransmitPackage;
  memset(&TransmitPackage, 0, sizeof(TransmitPackage));
  TransmitPackage.eTransmitType = pTxOptionsEx->pDestNode->nodeInfo.security == SECURITY_KEY_NONE ? EZWAVETRANSMITTYPE_NON_SECURE : EZWAVETRANSMITTYPE_SECURE;
  SSecureSendData *params = &TransmitPackage.uTransmitParams.SendDataParams;
  params->connection.remote.is_multicast = false;
  params->data_length = dataLength;
  memcpy(params->data, pData, params->data_length);
  params->tx_options.number_of_responses = 0;
  params->ptxCompleteCallback = NULL;
  params->connection.remote.address.node_id = pTxOptionsEx->pDestNode->node.nodeId;
  // Put the package on queue (and dont wait for it)
  result = (EZAF_EnqueueStatus_t)QueueNotifyingSendToBack(m_pTxQueueNotifying, (uint8_t *)&TransmitPackage, 0);
#else
  /*
   * Save the callback function pointer to a variable in order to call it in
   * ZCB_ResponseCompleted.
   * We store it in a small queue (ring buffer) to be able to have multiple
   * ongoing requests.
   */
  if (m_ResponseCallbackQueue.numInQueue >= CALLBACK_FUNCTION_QUEUE_SIZE)
  {
    DPRINTF("\r\n%s: Callback function queue full!\r\n", __func__);
    return ZAF_ENQUEUE_STATUS_TIMEOUT; // Failure to process the request
  }
  m_ResponseCallbackQueue.pCallback[m_ResponseCallbackQueue.head] = pCallback;


  if (IS_NULL(pTxOptionsEx))
  {
    return ZAF_ENQUEUE_STATUS_TIMEOUT;
  }

  //Safeguard against buffer overflow
  if (TX_BUFFER_SIZE < dataLength)
  {
    return ZAF_ENQUEUE_STATUS_BUFFER_OVERRUN;
  }

  CmdClassMultiChannelEncapsulate(&pData,
                                  &dataLength,
                                  pTxOptionsEx);

  SZwaveTransmitPackage FramePackage;
  FramePackage.uTransmitParams.SendDataEx.DestNodeId = pTxOptionsEx->pDestNode->node.nodeId;
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.Handle = ZCB_ResponseCompleted;
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.TransmitOptions = pTxOptionsEx->txOptions;
  memcpy(FramePackage.uTransmitParams.SendDataEx.FrameConfig.aFrame, pData, dataLength);
  FramePackage.uTransmitParams.SendDataEx.FrameConfig.iFrameLength = (uint8_t)dataLength;
  FramePackage.uTransmitParams.SendDataEx.SourceNodeId = 0x00;
  FramePackage.uTransmitParams.SendDataEx.TransmitOptions2 = 0x00;
  FramePackage.uTransmitParams.SendDataEx.TransmitSecurityOptions = pTxOptionsEx->txSecOptions;
  FramePackage.uTransmitParams.SendDataEx.eKeyType = pTxOptionsEx->pDestNode->nodeInfo.security;

  FramePackage.eTransmitType = EZWAVETRANSMITTYPE_EX;
  // Put the package on queue (and dont wait for it)
  result = (EZAF_EnqueueStatus_t)QueueNotifyingSendToBack(m_pTxQueueNotifying, (uint8_t*)&FramePackage, QUEUE_NOTIFYING_SEND_MAX_WAIT);
  if (ZAF_ENQUEUE_STATUS_SUCCESS == result)
  {
    // Success. Advance the callback function queue counters
    m_ResponseCallbackQueue.head++;
    m_ResponseCallbackQueue.numInQueue++;
  }
#endif
  return result;

}

void Transport_ApplicationCommandHandler(
    ZW_APPLICATION_TX_BUFFER * pCmd,
    uint8_t cmdLength,
    RECEIVE_OPTIONS_TYPE * rxOpt)
{
  RECEIVE_OPTIONS_TYPE_EX rxOptEx;
  rxOptEx.rxStatus = rxOpt->rxStatus;
  rxOptEx.securityKey = rxOpt->securityKey;
  rxOptEx.sourceNode.nodeId = rxOpt->sourceNode;
  rxOptEx.sourceNode.endpoint = 0;
  rxOptEx.destNode.endpoint = 0;
  rxOptEx.destNode.BitAddress = 0;
  // If applicable, supervision CC will fill in Supervision data in rxOptEx.
  rxOptEx.bSupervisionActive = 0;

  ZW_APPLICATION_TX_BUFFER bufferOut;
  uint8_t bufferOutLength = 0;

  memset((uint8_t *)&bufferOut, 0, sizeof(bufferOut));
  received_frame_status_t status = invoke_cc_handler(&rxOptEx, pCmd, cmdLength, &bufferOut, &bufferOutLength);

  if (RECEIVED_FRAME_STATUS_CC_NOT_FOUND != status) {
    if (0 < bufferOutLength) {
      // The CC handler put something into the output buffer => Transmit it.
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX * pTxOptions;
      RxToTxOptions(&rxOptEx, &pTxOptions);
      Transport_SendResponseEP((uint8_t *)&bufferOut, bufferOutLength, pTxOptions, NULL);
    }

    /*
     * The response was handled above. Hence, return here so that the frame is not parsed twice.
     */
    return;
  }

  Transport_ApplicationCommandHandlerEx(&rxOptEx, pCmd, cmdLength);
}

void
RxToTxOptions( RECEIVE_OPTIONS_TYPE_EX *rxopt,     /* IN  receive options to convert */
               TRANSMIT_OPTIONS_TYPE_SINGLE_EX** txopt)   /* OUT converted transmit options */
{
  static TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptionsEx;
  static MULTICHAN_NODE_ID destNode;
  txOptionsEx.pDestNode = &destNode;
  *txopt = &txOptionsEx;

  destNode.node.nodeId = rxopt->sourceNode.nodeId;
  destNode.node.endpoint = rxopt->sourceNode.endpoint;
  destNode.nodeInfo.BitMultiChannelEncap = (rxopt->sourceNode.endpoint == 0) ? 0 : 1;
  destNode.nodeInfo.security = rxopt->securityKey;


  txOptionsEx.txOptions = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE | ZWAVE_PLUS_TX_OPTIONS;
  if (rxopt->rxStatus & RECEIVE_STATUS_LOW_POWER)
  {
      txOptionsEx.txOptions |= TRANSMIT_OPTION_LOW_POWER;
  }
  txOptionsEx.sourceEndpoint = rxopt->destNode.endpoint;
  txOptionsEx.txSecOptions = 0;
  txOptionsEx.pDestNode->nodeInfo.security = rxopt->securityKey;
}

bool
Check_not_legal_response_job(RECEIVE_OPTIONS_TYPE_EX *rxOpt) /*rxOpt pointer of type RECEIVE_OPTIONS_TYPE_EX */
{
  bool status = false;
  /*
   * Check
   * 1: is the command received via multicast ?
   * 2: is the bit address set to 1 in the Multi Channel destination End Point ?
   * 3: Get command must not support Supervision encapsulation (CC:006C.01.00.21.003)
   */
  if (is_multicast(rxOpt) ||
     (0 != rxOpt->destNode.BitAddress) ||
     (true == myTransportEndpoint.flag_supervision_encap))
  {

    status = true;
  }
  myTransportEndpoint.flag_supervision_encap = false;
  return status;
}

bool
is_multicast(RECEIVE_OPTIONS_TYPE_EX *rxOpt) /*rxOpt pointer of type RECEIVE_OPTIONS_TYPE_EX */
{
  /*
   * Check if the frame is addressed to Broadcast NodeID or a Multicast frame
   */
  if (0 != (rxOpt->rxStatus & (RECEIVE_STATUS_TYPE_BROAD | RECEIVE_STATUS_TYPE_MULTI)))
  {
    return true;
  }
  return false;
}

/**
 * This function is used as an abstraction layer for the lower layers just to
 * parse a status. This layer will parse a pointer of the type
 * TRANSMISSION_RESULT including the given status.
 */
static void
ZCB_RequestCompleted(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus)
{
  UNUSED(extendedTxStatus);
  void (*pCallback)(TRANSMISSION_RESULT * pTransmissionResult);
  TRANSMISSION_RESULT transmissionResult;
  
  transmissionResult.nodeId = 0;
  transmissionResult.status = txStatus;
  transmissionResult.isFinished = TRANSMISSION_RESULT_FINISHED;
  // Call the callback function passed by the application in Transport_SendRequestEP (if not NULL)
  if(m_RequestCallbackQueue.numInQueue > 0)
  {
    pCallback = m_RequestCallbackQueue.pCallback[m_RequestCallbackQueue.tail];
    if (NULL != pCallback)
    {
      pCallback(&transmissionResult);
    }
    m_RequestCallbackQueue.tail++;
    m_RequestCallbackQueue.numInQueue--;
  }
  else
  {
    // Should never happen
    DPRINTF("\r\n%s: Oops, callback function queue empty!\r\n", __func__);
    ASSERT(0);
  }

  if (NULL != m_cbUpdateStayAwakePeriod)
  {
    m_cbUpdateStayAwakePeriod();
  }
}

#ifndef HOST_SECURITY_INCLUDED
static void
ZCB_ResponseCompleted(uint8_t txStatus, TX_STATUS_TYPE* extendedTxStatus)
{
  void (*pCallback)(uint8_t status);
  UNUSED(extendedTxStatus);
  // Call the callback function passed by the application in Transport_SendResponseEP (if not NULL)
  if (m_ResponseCallbackQueue.numInQueue > 0)
  {
    pCallback = m_ResponseCallbackQueue.pCallback[m_ResponseCallbackQueue.tail];
    if (NULL != pCallback)
    {
      pCallback(txStatus);
    }
    m_ResponseCallbackQueue.tail++;
    m_ResponseCallbackQueue.numInQueue--;
  }
  else
  {
    // Should never happen
    DPRINTF("\r\n%s: Oops, callback functon queue empty!\r\n", __func__);
    ASSERT(0);
  }

  if (NULL != m_cbUpdateStayAwakePeriod)
  {
    m_cbUpdateStayAwakePeriod();
  }
}
#endif

void
SetFlagSupervisionEncap(bool flag)
{
  myTransportEndpoint.flag_supervision_encap = flag;
}

ZW_WEAK received_frame_status_t
Transport_ApplicationCommandHandlerEx(
  RECEIVE_OPTIONS_TYPE_EX *rxOpt,
  ZW_APPLICATION_TX_BUFFER *pCmd,
  uint8_t cmdLength)
{
  UNUSED(rxOpt);
  UNUSED(pCmd);
  UNUSED(cmdLength);
  return RECEIVED_FRAME_STATUS_NO_SUPPORT;
}
