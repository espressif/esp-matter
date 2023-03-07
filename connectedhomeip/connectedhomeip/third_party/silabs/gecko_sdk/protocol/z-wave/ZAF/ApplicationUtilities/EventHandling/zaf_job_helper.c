/**
 * @file
 * @brief ZAF job helper source file
 * @copyright 2019 Silicon Laboratories Inc.
 */
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "zaf_job_helper.h"

/**
 * Job queue
 */
static StaticQueue_t jobQueue;

/**
 * Job queue buffer
 */
static uint8_t jobQueueBuffer[JOB_QUEUE_BUFFER_SIZE];

/**
 * Job queue handle
 */
static QueueHandle_t jobQueueHandle;

void ZAF_JobHelperInit(void)
{
  jobQueueHandle = xQueueCreateStatic(JOB_QUEUE_BUFFER_SIZE,
                                      sizeof(jobQueueBuffer[0]),
                                      jobQueueBuffer,
                                      &jobQueue);
}

bool ZAF_JobHelperJobEnqueue(uint8_t event)
{
  if(pdTRUE == xQueueSendToBack(jobQueueHandle, &event, 0))
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool ZAF_JobHelperJobDequeue(uint8_t * pEvent)
{
  bool x = false;
  if(pdTRUE == xQueueReceive(jobQueueHandle, pEvent, 0))
  {
    x = true;
  }
  return x;
}

uint8_t ZAF_JobHelperGetNumberOfRemainingJobs(void)
{
  return (uint8_t)uxQueueMessagesWaiting(jobQueueHandle);
}
