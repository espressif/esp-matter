/**
 * @file ZW_TransportMulticast.c
 * @brief Handles multicast frames in the Z-Wave Framework.
 * @copyright 2019 Silicon Laboratories Inc.
 */

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_TransportMulticast.h>
#include <stdint.h>
#include <string.h> // For memset and memcpy
#include <NodeMask.h>
#include <ZAF_tx_mutex.h>
#include <ZW_transport_api.h>
#include <ZW_TransportSecProtocol.h>
#include <association_plus.h>
#include <ZW_application_transport_interface.h>
#include <ZAF_Common_interface.h>

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

typedef enum
{
  MCTXRESULT_SUCCESS,
  MCTXRESULT_FAILURE,
  MCTXRESULT_NO_DESTINATIONS
}
MultiChannelTXResult_t;

typedef enum
{
  MULTICAST_TXRESULT_SUCCESS,
  MULTICAST_TXRESULT_FAILURE,
  MULTICAST_TXRESULT_NOT_ENOUGH_DESTINATIONS
}
MulticastTXResult_t;

/****************************************************************************/
/*                              PRIVATE DATA                                */
/****************************************************************************/

ZAF_TX_Callback_t p_callback_hold;

static uint8_t * p_data_hold;

static size_t data_length_hold;
static node_id_t singleCastTxDestNodeId = 0;  // this variable is used to save the current destination node ID for the singlecast follow-up frames
static TRANSMIT_OPTIONS_TYPE_EX * p_nodelist_hold;

static bool multicast_cb_called = false;
static bool gotSupervision      = false;

static uint8_t singlecast_node_count;

static uint8_t fSupervisionEnableHold;

static uint8_t secKeys;

static uint8_t txSecOptions = 0;

static SApplicationHandles * m_pAppHandle;

static bool moreMultiChannelNodes;

static uint32_t remainingNodeCount;

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/

// Nothing here.

/****************************************************************************/
/*                                   MACRO                                  */
/****************************************************************************/

/*
 * This will evaluate whether there is more transmissions left to do for the
 * currently active association group and set the isFinished flag accordingly.
 */
#define EVALUATE_TRANSMISSION_RESULT(isFinished)                    \
    do {                                                            \
      if (AssociationGetSinglecastNodeCount() == 0) {               \
        isFinished = TRANSMISSION_RESULT_FINISHED;                  \
      } else {                                                      \
        /* This will indicate to the stack and application that further transmissions are due.   \
         * Certain parameters will not be cleaned, like the buffer holding the packet for TX. */ \
         isFinished = TRANSMISSION_RESULT_NOT_FINISHED;             \
      }                                                             \
    } while(0)

/****************************************************************************/
/*                            PRIVATE FUNCTIONS                             */
/****************************************************************************/

static void ZCB_callback_wrapper(uint8_t Status, TX_STATUS_TYPE* pStatusType);
static void ZCB_multicast_callback(TRANSMISSION_RESULT * pTransmissionResult);

static MultiChannelTXResult_t TransmitMultiChannel(transmission_result_t * pTxResult);
static MulticastTXResult_t TransmitMultiCast(transmission_result_t * pTxResult);

void ZW_TransportMulticast_Init(void * pAppHandle)
{
  UNUSED(pAppHandle);
  m_pAppHandle = ZAF_getAppHandle();
}

// The application is calling this function!
void multichannel_callback(transmission_result_t * pTxResult)
{
  pTxResult->nodeId = singleCastTxDestNodeId;
  if (TRANSMIT_COMPLETE_OK != pTxResult->status) {
    /*
     * We have to check for TRANSMIT_COMPLETE_OK only because a multi channel destination node is
     * not allowed to respond with a Supervision Report. Hence, the transmission can never be
     * verified and so TRANSMIT_COMPLETE_VERIFIED is not relevant.
     *
     * In case the transmission failed, we report it to the application,
     * but we do not stop the following pending transmissions to other associations, if existing.
     */
    EVALUATE_TRANSMISSION_RESULT(pTxResult->isFinished);

    // Report the transmission result to the application.
    if (NULL != p_callback_hold) {
      p_callback_hold(pTxResult);
    }
    // Continue with the next multi-channel transmission, if any.
  }

  // Initiate Multi-channel transmissions as needed.
  if (true == moreMultiChannelNodes) {
    // There are more multi channel destinations
    MultiChannelTXResult_t MultiChannelResult = TransmitMultiChannel(NULL);
    pTxResult->nodeId = singleCastTxDestNodeId;
    if (MCTXRESULT_FAILURE == MultiChannelResult) {
      // Report the transmission result to the application.
      pTxResult->status = TRANSMIT_COMPLETE_FAIL;

      EVALUATE_TRANSMISSION_RESULT(pTxResult->isFinished);

      if (NULL != p_callback_hold) {
        p_callback_hold(pTxResult);
      }

      // Return here so that simultaneous transmissions towards different destinations are avoided.
      return;

    } else if (MCTXRESULT_NO_DESTINATIONS == MultiChannelResult) {
      // Multi-channel transmissions are done. Continue to multicast...
    } else {
      // New Multi Channel Bit Addressing frame was transmitted successfully. Wait for callback.
      return;  // This function will be called again...
    }
  }

  // Initiate multi-cast transmissions as needed.
  remainingNodeCount = AssociationGetSinglecastNodeCount();
  if (0 == remainingNodeCount) {
    // There are no other associated destinations left to transmit to.
    // Report the transmission result to the application.
    pTxResult->isFinished = TRANSMISSION_RESULT_FINISHED;

    if (NULL != p_callback_hold) {
      p_callback_hold(pTxResult);
    }
  } else {
    // There are remaining associated destinations in the association group.
    MulticastTXResult_t MulticastResult = TransmitMultiCast(NULL);
    pTxResult->nodeId = singleCastTxDestNodeId;
    if (MULTICAST_TXRESULT_FAILURE == MulticastResult) {
      // Report the transmission result to the application.
      pTxResult->status = TRANSMIT_COMPLETE_FAIL;

      EVALUATE_TRANSMISSION_RESULT(pTxResult->isFinished);

      if (NULL != p_callback_hold) {
        p_callback_hold(pTxResult);
      }
      return;
    } else if (MULTICAST_TXRESULT_NOT_ENOUGH_DESTINATIONS == MulticastResult) {
      /*
       * If there's only one non-endpoint node, we skip the multicast and call
       * the multicast callback directly.
       *
       * This callback function will initiate singlecast transmission for each of the nodes
       * included in the multicast, if there were any, or the one that was not transmitted due to
       * MULTICAST_TXRESULT_NOT_ENOUGH_DESTINATIONS in above condition.
       */
      ZCB_multicast_callback(pTxResult);
    } else {
      // Do nothing because the multicast was successful. Hence, waiting for callback.
    }
  }
}

/**
 * Checks whether there are any multi channel destinations and transmits the frame if so.
 * @return Returns true if the frame was enqueued. Otherwise false.
 */
static MultiChannelTXResult_t TransmitMultiChannel(transmission_result_t * pTxResult)
{
  UNUSED(pTxResult);
  destination_info_t node;

  do {
    memset((uint8_t *)&node, 0, sizeof(node));

    /* This function will return once every node in group of the same nodeID but with endpoint is prepared
     * in a bit-addressed list. */
    moreMultiChannelNodes = AssociationGetBitAdressingDestination(&(p_nodelist_hold->pList),
                                                                  &(p_nodelist_hold->list_length),
                                                                  &node);

    /* For each nodeID with endpoints, there will be a multi-channel transmission. */
    if (1 == node.node.BitAddress)
    {
      TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions = {
                                                   .txOptions = p_nodelist_hold->txOptions,
                                                   .txSecOptions = 0,
                                                   .sourceEndpoint = p_nodelist_hold->sourceEndpoint,
                                                   .pDestNode = &node
      };
      singleCastTxDestNodeId = node.node.nodeId;
      EZAF_EnqueueStatus_t txResult;
      txResult = Transport_SendRequestEP(
                    p_data_hold,
                    data_length_hold,
                    &txOptions,
                    multichannel_callback);

      return (ZAF_ENQUEUE_STATUS_SUCCESS == txResult) ? MCTXRESULT_SUCCESS : MCTXRESULT_FAILURE;
    }
    DPRINT("\n");
  } while (true == moreMultiChannelNodes);

  return MCTXRESULT_NO_DESTINATIONS;
}

static MulticastTXResult_t TransmitMultiCast(transmission_result_t * pTxResult)
{
  UNUSED(pTxResult);
  NODE_MASK_TYPE node_mask;
  ZW_NodeMaskClear(node_mask, sizeof(NODE_MASK_TYPE));

  singlecast_node_count = 0;
  txSecOptions = 0;
  
  //Safeguard against buffer overflow
  if (TX_BUFFER_SIZE < data_length_hold)
  {
    return MULTICAST_TXRESULT_FAILURE;
  }

  // Create transmit frame package
  SZwaveTransmitPackage FramePackage;
  memset(&FramePackage.uTransmitParams.SendDataMultiEx.FrameConfig.aFrame, 0,
      sizeof(FramePackage.uTransmitParams.SendDataMultiEx.FrameConfig.aFrame));
  memcpy(&FramePackage.uTransmitParams.SendDataMultiEx.FrameConfig.aFrame, p_data_hold, data_length_hold);
  FramePackage.uTransmitParams.SendDataMultiEx.FrameConfig.Handle = ZCB_callback_wrapper;
  FramePackage.uTransmitParams.SendDataMultiEx.FrameConfig.iFrameLength = (uint8_t)data_length_hold;
  FramePackage.uTransmitParams.SendDataMultiEx.FrameConfig.TransmitOptions = 0;

  remainingNodeCount = AssociationGetSinglecastNodeCount();

  if ((remainingNodeCount < 2) || moreMultiChannelNodes == true) {
    /* Not enough destinations for sending a multicast OR further endpoint destinations are pending TX. */
    return MULTICAST_TXRESULT_NOT_ENOUGH_DESTINATIONS;
  }
  else if (m_pAppHandle->pNetworkInfo->SecurityKeys & SECURITY_KEY_S2_MASK) {
    // We got more than one s2 node => Transmit S2 multicast.
    FramePackage.uTransmitParams.SendDataMultiEx.GroupId = p_nodelist_hold->S2_groupID;
    FramePackage.uTransmitParams.SendDataMultiEx.SourceNodeId = 0xFF;
    FramePackage.uTransmitParams.SendDataMultiEx.eKeyType = GetHighestSecureLevel(m_pAppHandle->pNetworkInfo->SecurityKeys);

    txSecOptions = S2_TXOPTION_SINGLECAST_FOLLOWUP;

    FramePackage.eTransmitType = EZWAVETRANSMITTYPE_MULTI_EX;

    // Put the package on queue (and don't wait for it)
    if (QueueNotifyingSendToBack(m_pAppHandle->pZwTxQueue, (uint8_t*)&FramePackage, 0) != EQUEUENOTIFYING_STATUS_SUCCESS)
    {
      return MULTICAST_TXRESULT_FAILURE;
    }
  } else if (!(m_pAppHandle->pNetworkInfo->SecurityKeys & SECURITY_KEY_NONE_MASK)) {
    // Fetch the nodeIDs for multicast
    for (uint32_t i = 0; i < remainingNodeCount; i++)
    {
      destination_info_t * pNode = AssociationGetNextSinglecastDestination();
      ZW_NodeMaskSetBit(node_mask, (uint8_t)pNode->node.nodeId);
    }

    FramePackage.eTransmitType = EZWAVETRANSMITTYPE_MULTI;
    memcpy(&FramePackage.uTransmitParams.SendDataMulti.NodeMask, &node_mask, sizeof(NODE_MASK_TYPE));

    // Put the package on queue (and don't wait for it)
    if (QueueNotifyingSendToBack(m_pAppHandle->pZwTxQueue, (uint8_t*)&FramePackage, 0) != EQUEUENOTIFYING_STATUS_SUCCESS)
    {
      return MULTICAST_TXRESULT_FAILURE;
    }
  }
  else
  {
    // Do nothing
  }
  return MULTICAST_TXRESULT_SUCCESS;
}

enum ETRANSPORT_MULTICAST_STATUS
ZW_TransportMulticast_SendRequest(const uint8_t * const p_data,
                                  uint8_t data_length,
                                  uint8_t fSupervisionEnable,
                                  TRANSMIT_OPTIONS_TYPE_EX * p_nodelist,
                                  ZAF_TX_Callback_t p_callback)
{
  if (IS_NULL(p_nodelist) || 0 == p_nodelist->list_length || IS_NULL(p_data) || 0 == data_length)
  {
    return ETRANSPORTMULTICAST_FAILED;
  }

  p_callback_hold = p_callback;
  p_nodelist_hold = p_nodelist;
  p_data_hold = (uint8_t *)p_data;
  data_length_hold = data_length;

  if (true != RequestBufferSetPayloadLength((ZW_APPLICATION_TX_BUFFER *)p_data, data_length))
  {
    // Failure to set payload length
    return ETRANSPORTMULTICAST_FAILED;
  }

  // Set the supervision hold variable in order to use it for singlecast
  fSupervisionEnableHold = fSupervisionEnable;

  // Get the active security keys
  secKeys = m_pAppHandle->pNetworkInfo->SecurityKeys;

  // Use supervision if security scheme is 2 and supervision is enabled.
  if (true != RequestBufferSupervisionPayloadActivate(
      (ZW_APPLICATION_TX_BUFFER**)&p_data_hold,
      &data_length_hold,
      ((0 != (SECURITY_KEY_S2_MASK & secKeys))
          && fSupervisionEnableHold)))
  {
    // Something is wrong.
    return ETRANSPORTMULTICAST_FAILED;
  }
  multicast_cb_called = false;
  gotSupervision      = false;
  // The Association Get Destination module must be initialized before getting any destinations.
  AssociationGetDestinationInit(p_nodelist_hold->pList);
  singleCastTxDestNodeId = 0;
  MultiChannelTXResult_t MultiChannelResult = TransmitMultiChannel(NULL);  // Handles bit-addressing for multi-channel

  if (MCTXRESULT_SUCCESS == MultiChannelResult) {
    return ETRANSPORTMULTICAST_ADDED_TO_QUEUE;
  } else if (MCTXRESULT_FAILURE == MultiChannelResult) {
    return ETRANSPORTMULTICAST_FAILED;
  }

  MulticastTXResult_t MulticastResult = TransmitMultiCast(NULL);

  if (MULTICAST_TXRESULT_SUCCESS == MulticastResult) {
    return ETRANSPORTMULTICAST_ADDED_TO_QUEUE;
  } else if (MULTICAST_TXRESULT_FAILURE == MulticastResult) {
    return ETRANSPORTMULTICAST_FAILED;
  } else {
    /*
     * If there's only one non-endpoint node, we skip the multicast and call
     * the multicast callback directly.
     *
     * This callback function will initiate singlecast transmission for each of the nodes
     * included in the multicast, if there were any, or the one that was not transmitted due to
     * MULTICAST_TXRESULT_NOT_ENOUGH_DESTINATIONS in above condition.
     */
    ZCB_multicast_callback(NULL);  // The argument can be ignored.
  }

  return ETRANSPORTMULTICAST_ADDED_TO_QUEUE;
}

static void
ZCB_callback_wrapper(uint8_t Status, TX_STATUS_TYPE* pStatusType)
{
  UNUSED(pStatusType);
  TRANSMISSION_RESULT  transmissionResult = {.nodeId = singleCastTxDestNodeId,
                                             .status = Status,
                                             .isFinished = TRANSMISSION_RESULT_FINISHED};
  ZCB_multicast_callback(&transmissionResult);
}

/**
 * This function will be called from two sources:
 * 1. when multicast is done OR when there's no need for multicast.
 * 2. when a transmission is done.
 *
 * It will initiate transmission of singlecast frames for each of the nodes
 * included in the multicast.
 */
static void
ZCB_multicast_callback(TRANSMISSION_RESULT * pTransmissionResult)
{
  static TRANSMISSION_RESULT transmissionResult;
  EZAF_EnqueueStatus_t txResult;

  DPRINT("\r\n ZCB_multicast_callback() \n");
  multicast_cb_called = false;
  if (gotSupervision) {
     gotSupervision = false;
    return;
  }

  // Handle the result of transmission.
  if (singlecast_node_count > 0)  // Check whether this callback is invoked due to a made transmission.
  {
    /*
     * When singlecast_node_count is higher than zero, it means that the call
     * to this function is a callback when transmission is done or failed.
     */

    // Check whether to set the finish flag.
    if (singlecast_node_count == remainingNodeCount)
    {
      transmissionResult.isFinished = TRANSMISSION_RESULT_FINISHED;
      DPRINT("\r\nTransmission done!");
    }
    else
    {
      transmissionResult.isFinished = TRANSMISSION_RESULT_NOT_FINISHED;
    }

    /*
     * If pTransmissionResult == NULL, then this call is really just a call to start transmission,
     * and not a result of a transmission having been done.
     */
     transmissionResult.nodeId = singleCastTxDestNodeId;
    if (NON_NULL(pTransmissionResult))
    {
      transmissionResult.status = pTransmissionResult->status;
    }
    else
    {
      transmissionResult.status = TRANSMIT_COMPLETE_OK;
    }

    // Report the transmission result to the application.
    if (NON_NULL(p_callback_hold))
    {
      p_callback_hold(&transmissionResult);
    }
  }

  // Initiate new transmission.
  if (singlecast_node_count < remainingNodeCount)
  {
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptions;

    txOptions.txOptions = p_nodelist_hold->txOptions;
    txOptions.txSecOptions = txSecOptions;

    if(0 == singlecast_node_count && (txSecOptions & S2_TXOPTION_SINGLECAST_FOLLOWUP) )
    {
      txOptions.txSecOptions |= S2_TXOPTION_FIRST_SINGLECAST_FOLLOWUP;
    }

    txOptions.sourceEndpoint = p_nodelist_hold->sourceEndpoint;
    txOptions.pDestNode = AssociationGetNextSinglecastDestination();
    singleCastTxDestNodeId = txOptions.pDestNode->node.nodeId;
    transmissionResult.nodeId = singleCastTxDestNodeId;

    if (true != RequestBufferSupervisionPayloadActivate(
        (ZW_APPLICATION_TX_BUFFER**)&p_data_hold,
        &data_length_hold,
        ((0 != (SECURITY_KEY_S2_MASK & secKeys))
            && fSupervisionEnableHold
            && (0 == txOptions.pDestNode->node.BitAddress))))
    {
      transmissionResult.status = TRANSMIT_COMPLETE_FAIL;
      transmissionResult.isFinished = TRANSMISSION_RESULT_FINISHED;

      // Report the transmission result to the application.
      if (NON_NULL(p_callback_hold))
      {
        p_callback_hold(&transmissionResult);
      }
      return;
    }

    txResult = Transport_SendRequestEP(
                  p_data_hold,
                  data_length_hold,
                  &txOptions,
                  ZCB_multicast_callback);

    if (ZAF_ENQUEUE_STATUS_SUCCESS != txResult)
    {
      DPRINTF("\r\nError: %d", txResult);
      transmissionResult.status = TRANSMIT_COMPLETE_FAIL;
      EVALUATE_TRANSMISSION_RESULT(transmissionResult.isFinished);

      // Report the transmission result to the application.
      if (NON_NULL(p_callback_hold))
      {
        p_callback_hold(&transmissionResult);
      }
      return;
    }
    multicast_cb_called = true;
    singlecast_node_count++;
  }
}

void
ZW_TransportMulticast_clearTimeout(void)
{
  if (multicast_cb_called) {
    return;
  }
  ZCB_multicast_callback(NULL);
  gotSupervision  = true;
}
