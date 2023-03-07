/**
 * @file
 * @brief ZAF Event helper source file
 * @copyright 2019 Silicon Laboratories Inc.
 */
//#define DEBUGPRINT
#include "DebugPrint.h"
#include "zaf_event_helper.h"
#include "stdlib.h"

/**
 * Holds the QueueNotifying handle that the application passed by calling the ZAF_EventHelperInit
 * function.
 */
static SQueueNotifying * m_pQueueNotifyingHandle = NULL;

void ZAF_EventHelperInit(SQueueNotifying * pQueueNotifyingHandle)
{
  m_pQueueNotifyingHandle = pQueueNotifyingHandle;
}

bool ZAF_EventHelperIsInitialized(void)
{
  return m_pQueueNotifyingHandle != NULL;
}

bool ZAF_EventHelperEventEnqueue(const uint8_t event)
{
  EQueueNotifyingStatus Status;

  Status = QueueNotifyingSendToBack(m_pQueueNotifyingHandle, &event, 0);
  if (Status != EQUEUENOTIFYING_STATUS_SUCCESS)
  {
    DPRINT("Failed to queue event\r\n");
    return false;
  }
  return true;
}

bool ZAF_EventHelperEventEnqueueFromISR(const uint8_t event)
{
  EQueueNotifyingStatus Status;

  Status = QueueNotifyingSendToBackFromISR(m_pQueueNotifyingHandle, &event);
  if (Status != EQUEUENOTIFYING_STATUS_SUCCESS)
  {
    DPRINT("Failed to queue event\r\n");
    return false;
  }
  return true;
}
