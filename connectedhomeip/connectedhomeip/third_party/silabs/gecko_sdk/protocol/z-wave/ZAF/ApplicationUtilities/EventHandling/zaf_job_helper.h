/**
 * @file
 * Wraps a FreeRTOS queue to serve the application with a simpler API for jobs.
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef ZAF_APPLICATIONUTILITIES_JOB_HELPER_H_
#define ZAF_APPLICATIONUTILITIES_JOB_HELPER_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Defines the maximum number of jobs to queue for this module.
 */
#define JOB_QUEUE_BUFFER_SIZE  4

/**
 * Initializes the job helper.
 */
void ZAF_JobHelperInit(void);

/**
 * Adds a given event to the job queue.
 * @param[in] event An event.
 * @return true if the event was added to the job queue, false otherwise.
 */
bool ZAF_JobHelperJobEnqueue(uint8_t event);

/**
 * Dequeues a job from the job queue.
 * @param[out] pEvent return event from the queue.
 * @return true if a job was dequeued, false otherwise.
 */
bool ZAF_JobHelperJobDequeue(uint8_t * pEvent);

/**
 * Returns the current number of jobs on the job queue.
 * @return Number of jobs on the job queue.
 */
uint8_t ZAF_JobHelperGetNumberOfRemainingJobs(void);

#endif /* ZAF_APPLICATIONUTILITIES_JOB_HELPER_H_ */
