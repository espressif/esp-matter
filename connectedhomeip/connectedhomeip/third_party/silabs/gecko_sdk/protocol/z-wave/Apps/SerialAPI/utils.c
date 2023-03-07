/**
 * @file utils.c
 * @copyright 2022 Silicon Laboratories Inc.
 */

#include <string.h>
#include <utils.h>
#include <serialappl.h>
#include <ZAF_Common_interface.h>

uint8_t GetCommandResponse(SZwaveCommandStatusPackage *pCmdStatus, EZwaveCommandStatusType cmdType)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();
  TaskHandle_t m_pAppTaskHandle = ZAF_getAppTaskHandle();
  QueueHandle_t Queue = m_pAppHandles->ZwCommandStatusQueue;
  for (uint8_t delayCount = 0; delayCount < 100; delayCount++)
  {
    for (UBaseType_t QueueElmCount = uxQueueMessagesWaiting(Queue);  QueueElmCount > 0; QueueElmCount--)
    {
      if (xQueueReceive(Queue, (uint8_t*)pCmdStatus, 0))
      {
        if (pCmdStatus->eStatusType == cmdType)
        {
          if (m_pAppTaskHandle  && (0 < uxQueueMessagesWaiting(Queue)))
          {
            /* More elements in queue call xTaskNotify */
            BaseType_t Status = xTaskNotify(m_pAppTaskHandle, 1 << EAPPLICATIONEVENT_ZWCOMMANDSTATUS, eSetBits);
            ASSERT(Status == pdPASS); // We probably received a bad Task handle
          }
          return true;
        }
        else
        {
          /* Re-insert none-matching message into Queue */
          BaseType_t result = xQueueSendToBack(Queue, (uint8_t*)pCmdStatus, 0);
          ASSERT(pdTRUE == result);
        }
      }
    }
    vTaskDelay(10);
  }
  if (m_pAppTaskHandle && (0 < uxQueueMessagesWaiting(Queue)))
  {
    /* Only call xTaskNotify if still elements in queue */
    BaseType_t Status = xTaskNotify(m_pAppTaskHandle, 1 << EAPPLICATIONEVENT_ZWCOMMANDSTATUS, eSetBits);
    ASSERT(Status == pdPASS); // We probably received a bad Task handle
  }
  return false;
}

uint8_t IsPrimaryController(void)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();  
  SZwaveCommandPackage cmdPackage = {
      .eCommandType = EZWAVECOMMANDTYPE_IS_PRIMARY_CTRL};
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  SZwaveCommandStatusPackage cmdStatus;
  if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_IS_PRIMARY_CTRL))
  {
    return cmdStatus.Content.IsPrimaryCtrlStatus.result;
  }
  ASSERT(false);
  return 0;
}

uint8_t GetControllerCapabilities(void)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();  
  SZwaveCommandPackage cmdPackage = {.eCommandType = EZWAVECOMMANDTYPE_GET_CONTROLLER_CAPABILITIES};
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  SZwaveCommandStatusPackage cmdStatus;
  if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_GET_CONTROLLER_CAPABILITIES))
  {
    return cmdStatus.Content.GetControllerCapabilitiesStatus.result;
  }
  ASSERT(false);
  return 0;
}

uint8_t QueueProtocolCommand(uint8_t *pCommand)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();  
  // Put the Command on queue (and dont wait for it, queue must be empty)
  return (QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, pCommand, 0));
}

/**
* Aquire node information from protocol
*
* Method requires CommandStatus queue from protocol to be empty.
* Method requires CommandQueue to protocol to be empty.
* Method will cause assert on failure.
*
* @param[in]     NodeId       ID of node to get information about.
* @param[out]    pNodeInfo    Pointer to t_extNodeInfo struct where aquired node info can be stored.
*/
void GetNodeInfo(uint16_t NodeId, t_ExtNodeInfo* pNodeInfo)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();  
  SZwaveCommandPackage GetNodeInfoCommand = {
      .eCommandType = EZWAVECOMMANDTYPE_NODE_INFO,
      .uCommandParams.NodeInfo.NodeId = NodeId};

  // Put the Command on queue (and dont wait for it, queue must be empty)
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&GetNodeInfoCommand, 0);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  // Wait for protocol to handle command (it shouldnt take long)
  SZwaveCommandStatusPackage NodeInfo;
  if (GetCommandResponse(&NodeInfo, EZWAVECOMMANDSTATUS_NODE_INFO))
  {
    if (NodeInfo.Content.NodeInfoStatus.NodeId == NodeId)
    {
      memcpy(pNodeInfo, (uint8_t*)&NodeInfo.Content.NodeInfoStatus.extNodeInfo, sizeof(NodeInfo.Content.NodeInfoStatus.extNodeInfo));
      return;
    }
  }
  ASSERT(false);
}

/**
* Aquire a list of included nodes IDS in the network from protocol
*
* Method requires CommandStatus queue from protocol to be empty.
* Method requires CommandQueue to protocol to be empty.
* Method will cause assert on failure.
*
* @param[out]    node_id_list    Pointer to bitmask list where aquired included nodes IDs saved
*/
void Get_included_nodes(uint8_t* node_id_list)
{
  const SApplicationHandles *m_pAppHandles = ZAF_getAppHandle();  
  SZwaveCommandPackage GetIncludedNodesCommand = {
      .eCommandType = EZWAVECOMMANDTYPE_ZW_GET_INCLUDED_NODES};

  // Put the Command on queue (and dont wait for it, queue must be empty)
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&GetIncludedNodesCommand, 0);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  // Wait for protocol to handle command (it shouldnt take long)
  SZwaveCommandStatusPackage includedNodes;
  if (GetCommandResponse(&includedNodes, EZWAVECOMMANDSTATUS_ZW_GET_INCLUDED_NODES))
  {
    memcpy(node_id_list, (uint8_t*)includedNodes.Content.GetIncludedNodes.node_id_list, sizeof(NODE_MASK_TYPE));
    return;
  }
  ASSERT(false);
}


/**
* Acquire a list of included long range nodes IDS in the network from protocol.
*
* Method requires CommandStatus queue from protocol to be empty.
* Method requires CommandQueue to protocol to be empty.
* Method will cause assert on failure.
*
* @param[out]    node_id_list    Pointer to bitmask list where aquired included nodes IDs saved
*/
void Get_included_lr_nodes(uint8_t* node_id_list)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();  
  SZwaveCommandPackage GetIncludedNodesCommand = {
      .eCommandType = EZWAVECOMMANDTYPE_ZW_GET_INCLUDED_LR_NODES
  };

  // Put the Command on queue (and don't wait for it, queue is most likely empty)
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&GetIncludedNodesCommand, 0);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  // Wait for protocol to handle command (it shouldn't take long)
  SZwaveCommandStatusPackage includedNodes;
  if (GetCommandResponse(&includedNodes, EZWAVECOMMANDSTATUS_ZW_GET_INCLUDED_LR_NODES))
  {
    memcpy(node_id_list, (uint8_t*)includedNodes.Content.GetIncludedNodesLR.node_id_list, sizeof(LR_NODE_MASK_TYPE));
    return;
  }
  ASSERT(false);
}


void TriggerNotification(EApplicationEvent event)
{
  TaskHandle_t m_pAppTaskHandle =ZAF_getAppTaskHandle();  
  if (m_pAppTaskHandle)
  {
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(m_pAppTaskHandle,
                1 << event,
                eSetBits,
//                &xHigherPriorityTaskWoken
                NULL
    );
     /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
     should be performed to ensure the interrupt returns directly to the highest
     priority task.  The macro used for this purpose is dependent on the port in
     use and may be called portEND_SWITCHING_ISR(). */
//     portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }

}


uint8_t GetLongRangeChannel(void)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();
  SZwaveCommandPackage cmdPackage = {0};
  cmdPackage.eCommandType = EZWAVECOMMANDTYPE_ZW_GET_LR_CHANNEL;
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  SZwaveCommandStatusPackage cmdStatus;
  if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_ZW_GET_LR_CHANNEL))
  {
    return cmdStatus.Content.GetLRChannel.result;
  }
  ASSERT(false);
  return 0;
}

bool SetLongRangeChannel(uint8_t channel)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();
  SZwaveCommandPackage cmdPackage = {0};
  cmdPackage.eCommandType = EZWAVECOMMANDTYPE_ZW_SET_LR_CHANNEL;
  cmdPackage.uCommandParams.SetLRChannel.value = channel;
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  return (EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
}

void SetLongRangeVirtualNodes(uint8_t bitmask)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();
  SZwaveCommandPackage cmdPackage = {0};
  cmdPackage.eCommandType = EZWAVECOMMANDTYPE_ZW_SET_LR_VIRTUAL_IDS;
  cmdPackage.uCommandParams.SetLRVirtualNodeIDs.value = bitmask;
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
}

uint8_t GetPTIConfig(void)
{
  const SApplicationHandles * m_pAppHandles = ZAF_getAppHandle();
  SZwaveCommandPackage cmdPackage = {0};
  cmdPackage.eCommandType = EZWAVECOMMANDTYPE_ZW_GET_PTI_CONFIG;
  EQueueNotifyingStatus QueueStatus = QueueNotifyingSendToBack(m_pAppHandles->pZwCommandQueue, (uint8_t *)&cmdPackage, 500);
  ASSERT(EQUEUENOTIFYING_STATUS_SUCCESS == QueueStatus);
  SZwaveCommandStatusPackage cmdStatus;
  if (GetCommandResponse(&cmdStatus, EZWAVECOMMANDSTATUS_ZW_GET_PTI_CONFIG))
  {
    return cmdStatus.Content.GetPTIconfig.result;
  }
  ASSERT(false);
  return 0;
}
