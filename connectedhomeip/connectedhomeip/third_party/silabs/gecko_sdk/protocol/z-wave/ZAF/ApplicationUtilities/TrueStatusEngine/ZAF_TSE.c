/**
 * TSE : True Status Engine.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#include "ZAF_TSE.h"
#include "zaf_tse_config.h"
#include "AppTimer.h"
#include "association_plus.h"
#include "misc.h"

//#define DEBUGPRINT
#include "DebugPrint.h"

/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/
typedef struct _s_zaf_tse_resource_t_
{
  /* Pointer to the function to call back at the end, to transmit the status */
  void (*pCallback)(TRANSMIT_OPTIONS_TYPE_SINGLE_EX, void*);
  /* Data pointer, the pointed struct must contain a RECEIVE_OPTIONS_TYPE_EX object first.
  see s_zaf_tse_data_input_template_t definition below */
  void* pData;
  /*
   * Points to the current node in a list of nodes.
   */
  destination_info_t * pCurrentNode;
  /*
   * Holds the number of remaining nodes in an active True Status session.
   */
  uint8_t remainingNodes;
}s_zaf_tse_resource_t;

// Private function prototypes
void ZAF_TSE_TimerCallback(SSwTimer* pTimer);

/* Private variables */
static s_zaf_tse_resource_t TSE_ResourceArray[ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS];
SSwTimer zaf_tse_timer;

/*
 * Points to the current trigger in the resource array.
 */
static s_zaf_tse_resource_t * pCurrentTrigger;

bool ZAF_TSE_Init()
{
  DPRINTF("%s()\r\n", __func__);

  /* Register the timer */
  if (true != AppTimerRegister(&zaf_tse_timer, false, ZAF_TSE_TimerCallback))
  {
    DPRINT("\tERROR: ZAF_TSE_Init could not register a SSwTimer\r\n");
    return false;
  }

  /* Reset the resource array */
  for (uint8_t i = 0; i < ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS; i++)
  {
    /* Reset the data in the array at the init */
    TSE_ResourceArray[i].pCallback = NULL;
    TSE_ResourceArray[i].pData = NULL;
  }

  pCurrentTrigger = NULL;

  return true;
}

bool ZAF_TSE_Trigger(zaf_tse_callback_t pCallback,
                     void* pData,
                     bool overwrite_previous_trigger)
{
  DPRINTF("\r\n%s():", __func__);
  /* Do not trigger the TSE if the callback or data is NULL */
  if (NULL == pCallback || NULL == pData)
  {
    DPRINT("TSE Callback or Data is NULL. Rejecting trigger\r\n");
    return false;
  }

  /* Extract RX_OPTIONS from the pData, assuming it is the first member in the pData struct */
  s_zaf_tse_data_input_template_t* pDataInput = (s_zaf_tse_data_input_template_t*)(pData);
  RECEIVE_OPTIONS_TYPE_EX RxOptions = pDataInput->rxOptions;

  /* Do not trigger the TSE if the frame was received via Multicast */
  if (is_multicast(&RxOptions))
  {
    DPRINT("Frame was received via multicast. Ignoring trigger\r\n");
    return true;
  }

  /* If Lifeline is identical to the RxOption Source node, just ignore the trigger and return */
  MULTICHAN_NODE_ID* pList = NULL;
  uint8_t ListLen = 0;
  handleAssociationGetnodeList(ZAF_TSE_GROUP_ID, 0, &pList, &ListLen);

  if (0 == ListLen)
  {
    DPRINT("TSE Association Group ZAF_TSE_GROUP_ID empty. Ignoring trigger\r\n");
    return true;
  }
  else if (1 == ListLen)
  {
    /* Verify here if the change was triggered by the only Association Group destinations */
    if (RxOptions.sourceNode.nodeId == pList[0].node.nodeId &&
        RxOptions.sourceNode.endpoint == pList[0].node.endpoint)
    {
      DPRINT("Association Group only destination triggered the change. Ignoring trigger\r\n");
      return true;
    }
  }

  /* Variable to search if pCallback is in the TSE Resource array */
  uint8_t resourceIndex = 0;
  bool pCallbackPresent = false;

  /* Find out if the callback is already in the array */
  if(true == overwrite_previous_trigger)
  {
    if(TimerIsActive(&zaf_tse_timer))
    {
      for (uint8_t i = 0; i < ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS; i++)
      {
        /* First look for the same callback function */
        if (TSE_ResourceArray[i].pCallback == pCallback)
        {
          /* Then also verify that it is for the same End Point */
          s_zaf_tse_data_input_template_t* pDataStored =
          (s_zaf_tse_data_input_template_t*)(TSE_ResourceArray[i].pData);
          RECEIVE_OPTIONS_TYPE_EX storedRxOptions = pDataStored->rxOptions;

          if (storedRxOptions.destNode.endpoint == RxOptions.destNode.endpoint)
          {
            pCallbackPresent = true;
            resourceIndex = i;
            break;
          }
        }
      }
    }
  }

  if (true == pCallbackPresent)
  {
    DPRINT("Trigger already in the trigger list and overwrite_previous_trigger is enabled: restarting timer\r\n");
    TimerRestart(&zaf_tse_timer);
  }
  else
  {
    /* The callback is not present in the resource array or overwrite_previous_trigger is false, find out if there is an available slot */
    bool resourceIndexAvailable = false;
    resourceIndex = 0 ;
    for (uint8_t i = 0; i < ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS; i++)
    {
      if (NULL == TSE_ResourceArray[i].pCallback)
      {
        resourceIndexAvailable = true;
        resourceIndex = i;
        DPRINTF("\r\nAssigned index: %u", resourceIndex);
        break;
      }
    }
    /* Add the pCallback in the resource array or return */
    if (false == resourceIndexAvailable)
    {
      DPRINTF("ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS (%d) already active. Rejecting new trigger\r\n",
      ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS);
      return false;
    }
    else
    {
      DPRINT("Trigger not in the trigger list: adding new callback and starting the timer\r\n");
      TSE_ResourceArray[resourceIndex].pCallback = pCallback;
      TSE_ResourceArray[resourceIndex].pData = pData;
      TSE_ResourceArray[resourceIndex].pCurrentNode = pList;
      TSE_ResourceArray[resourceIndex].remainingNodes = ListLen;
      if (NULL == pCurrentTrigger)
      {
        pCurrentTrigger = &TSE_ResourceArray[resourceIndex];
        TimerStart(&zaf_tse_timer, ZAF_TSE_DELAY_TRIGGER); /* Timer start also restarts an ongoing timer */
      }
    }
  }
  DPRINTF("\r\nListLen: %u", ListLen);
  DPRINTF("\r\nRemaining nodes: %u", TSE_ResourceArray[resourceIndex].remainingNodes);

  return true;
}

static void InvokeRegisteredCallback(void)
{
  DPRINTF("\r\n%s():", __func__);

  s_zaf_tse_data_input_template_t* pDataInput = (s_zaf_tse_data_input_template_t*)(pCurrentTrigger->pData);
  RECEIVE_OPTIONS_TYPE_EX RxOptions = pDataInput->rxOptions;

  DPRINTF("\tTSE transmit call back for dest node %d endpoint %d\r\n",
          pCurrentTrigger->pCurrentNode->node.nodeId, pCurrentTrigger->pCurrentNode->node.endpoint);

  /* Build a txOptionEx */
  TRANSMIT_OPTIONS_TYPE_SINGLE_EX txOptionsEx;

  txOptionsEx.pDestNode = pCurrentTrigger->pCurrentNode;
  txOptionsEx.sourceEndpoint = RxOptions.destNode.endpoint;
  txOptionsEx.txOptions = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_EXPLORE | ZWAVE_PLUS_TX_OPTIONS;
  if (RxOptions.rxStatus & RECEIVE_STATUS_LOW_POWER)
  {
    txOptionsEx.txOptions |= TRANSMIT_OPTION_LOW_POWER;
  }
  txOptionsEx.txSecOptions = 0;

  /*
   * Invoke the callback if it's different from NULL. We should never end up here with pCallback
   * set to NULL, but just in case.
   */
  if (NULL != pCurrentTrigger->pCallback)
  {
    pCurrentTrigger->pCallback(txOptionsEx, pCurrentTrigger->pData);
  }
}

void ZAF_TSE_TimerCallback(SSwTimer* pTimer)
{
  DPRINTF("%s():\r\n", __func__);
  /* Verify that the pTimer is actually pointing at the zaf tse timer*/
  if (&zaf_tse_timer != pTimer)
  {
    DPRINT("\tERROR: ZAF_TSE_TimerCallback called with wrong timer pointer. Ignoring callback\r\n");
    return ;
  }

  s_zaf_tse_data_input_template_t* pDataInput = (s_zaf_tse_data_input_template_t*)(pCurrentTrigger->pData);
  RECEIVE_OPTIONS_TYPE_EX RxOptions = pDataInput->rxOptions;
  if (RxOptions.sourceNode.nodeId != pCurrentTrigger->pCurrentNode->node.nodeId ||
      RxOptions.sourceNode.endpoint != pCurrentTrigger->pCurrentNode->node.endpoint)
  {
    InvokeRegisteredCallback();
  }
  else
  {
    DPRINT("\r\nSource == Destination");
    ZAF_TSE_TXCallback(NULL);
  }
}

void ZAF_TSE_TXCallback(transmission_result_t * pTransmissionResult)
{
  UNUSED(pTransmissionResult);

  DPRINTF("%s():\r\n", __func__);

  RECEIVE_OPTIONS_TYPE_EX RxOptions;

  do
  {
    pCurrentTrigger->remainingNodes--;
    DPRINTF("\r\nRemaining nodes: (%u)", pCurrentTrigger->remainingNodes);
    if (0 == pCurrentTrigger->remainingNodes)
    {
      // No more nodes left. Clear the trigger.
      pCurrentTrigger->pCallback = NULL;
      pCurrentTrigger->pData = NULL;
      pCurrentTrigger->pCurrentNode = NULL;

      // Find the current trigger in the array of triggers.
      uint32_t currentTriggerIndex = 0;
      uint32_t i;
      for (i = 0; i < ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS; i++)
      {
        if (pCurrentTrigger == &TSE_ResourceArray[i])
        {
          // Found current trigger in array.
          currentTriggerIndex = i;
          break;
        }
      }

      // Reset the current trigger so that we can check later if there was found a new current trigger.
      pCurrentTrigger = NULL;
      do
      {
        currentTriggerIndex = (currentTriggerIndex + 1) % ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS;
        if (NULL != TSE_ResourceArray[currentTriggerIndex].pCallback)
        {
          // Break if we found a trigger that is set.
          pCurrentTrigger = &TSE_ResourceArray[currentTriggerIndex];
          break;
        }
      }
      // Search the entire array until we hit the index where we started.
      while (currentTriggerIndex != i);

      if (NULL != pCurrentTrigger)
      {
        DPRINTF("\r\nNew trigger index: %u", currentTriggerIndex);
      }
    }
    else
    {
      pCurrentTrigger->pCurrentNode++;
    }
    if (NULL == pCurrentTrigger)
    {
      // pCurrentTrigger being NULL means there are no more active triggers.
      return;
    }
    s_zaf_tse_data_input_template_t* pDataInput = (s_zaf_tse_data_input_template_t*)(pCurrentTrigger->pData);
    RxOptions = pDataInput->rxOptions;
  }
  // If the destination node matches the source node, skip the node and repeat the above steps.
  while(RxOptions.sourceNode.nodeId == pCurrentTrigger->pCurrentNode->node.nodeId &&
        RxOptions.sourceNode.endpoint == pCurrentTrigger->pCurrentNode->node.endpoint);

  InvokeRegisteredCallback();
}

