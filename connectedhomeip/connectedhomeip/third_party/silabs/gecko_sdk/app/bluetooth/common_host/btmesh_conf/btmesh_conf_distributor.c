/***************************************************************************//**
 * @file
 * @brief BT Mesh Configurator Component - Job Distributor
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdlib.h>

#include "btmesh_conf.h"
#include "btmesh_conf_distributor.h"
#include "btmesh_conf_executor.h"
#include "sl_slist.h"

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_distributor BT Mesh Configuration Distributor
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_distributor_internal \
 * BT Mesh Configuration Distributor Internal
 * @{
 ******************************************************************************/
/// BT Mesh Configuration Distributor schedule type
typedef enum {
  /// Schedule is executed immediately
  BTMESH_CONF_SCHEDULE_NORMAL,
  /// Schedule is deferred until the next @ref btmesh_conf_step
  BTMESH_CONF_SCHEDULE_DEFERRED
} btmesh_conf_schedule_t;

/***************************************************************************//**
 * Schedule one or more waiting @ref btmesh_conf_job for execution on one or
 * more idle @ref btmesh_conf_executor.
 *
 * @param[in] self Pointer to the configuration distributor instance
 * @param[in] schedule_type Normal or deferred scheduling
 *
 * If deferred scheduling type is specified then the scheduling is deferred
 * until the next @ref btmesh_conf_step.
 *
 * The scheduling algorithm does not assign a configuration job to an idle
 * executor if another configuration job is in progress with the same server
 * address (same target node) so these jobs are skipped because only one BT Mesh
 * Stack configuration request can be active with the same server address at any
 * given point of time. Therefore, configuration jobs with different server
 * addresses can be executed in parallel and configuration jobs with same server
 * address are executed in the order of @ref btmesh_conf_distributor_submit_job
 * function calls.
 ******************************************************************************/
static void btmesh_conf_distributor_schedule(btmesh_conf_distributor_t *const self,
                                             btmesh_conf_schedule_t schedule_type);

/***************************************************************************//**
 * Start a configuration job on an idle configuration executor
 *
 * @param[in] self Pointer to the configuration distributor instance
 * @param[in] job Configuration job which aggregates the configuration tasks
 * @returns
 * @retval SL_STATUS_OK Job is started on an executor
 * @retval SL_STATUS_NO_MORE_RESOURCE All executors are busy with job execution
 * @retval SL_STATUS_IN_PROGRESS Another job is ongoing with the same target node
 * @retval SL_STATUS_FAIL Job start failed unexpectedly
 ******************************************************************************/
static sl_status_t btmesh_conf_distributor_start_job(btmesh_conf_distributor_t *const self,
                                                     btmesh_conf_job_t *job);

/** @} (end addtogroup btmesh_conf_distributor_internal) */

btmesh_conf_distributor_t *btmesh_conf_distributor_create(uint16_t executor_count)
{
  btmesh_conf_distributor_t* self;
  uint16_t idx;

  // Parameter checks: executor count shall be greater than zero
  if (0 == executor_count) {
    return NULL;
  }

  self = malloc(sizeof(btmesh_conf_distributor_t));

  // If the allocation fails then NULL shall be returned
  if (NULL == self) {
    return NULL;
  }

  // Initialize configuration distributor struct members
  self->executor_count = executor_count;
  sl_slist_init(&self->waiting_jobs);
  self->deferred_schedule_request = false;

  // Allocate an array of pointers to configuration executors.
  // This makes it possible to allocate configuration executors with
  // btmesh_conf_executor_create function.
  // The executor pointer array is allocated with calloc to initialize them to
  // NULL to support the deallocation of partially created distributor with
  // btmesh_conf_distributor_destroy function if this create function fails.
  self->executors = (btmesh_conf_executor_t **)
                    calloc(self->executor_count, sizeof(btmesh_conf_executor_t *));

  if (NULL == self->executors) {
    // If the allocation of executor pointer array fails then the distributor
    // shall destroy itself.
    btmesh_conf_distributor_destroy(self);
    return NULL;
  }

  for (idx = 0; idx < self->executor_count; idx++) {
    // Create configuration executor and store the pointer in the array
    self->executors[idx] = btmesh_conf_executor_create(idx, self);
    if (NULL == self->executors[idx]) {
      break;
    }
  }

  // If at least one executor allocation fails then object creation shall fail
  if (idx != self->executor_count) {
    // If the allocation of any executor fails then the distributor shall
    // destroy itself.
    btmesh_conf_distributor_destroy(self);
    return NULL;
  }

  return self;
}

void btmesh_conf_distributor_destroy(btmesh_conf_distributor_t* self)
{
  if (NULL == self) {
    return;
  }

  // Destroy waiting jobs
  while (NULL != self->waiting_jobs) {
    btmesh_conf_job_t *job = SL_SLIST_ENTRY(sl_slist_pop(&self->waiting_jobs),
                                            btmesh_conf_job_t,
                                            list_elem);
    btmesh_conf_job_destroy(job);
  }

  if (NULL != self->executors) {
    for (uint16_t idx = 0; idx < self->executor_count; idx++) {
      // All destroy interfaces shall handle null pointers and it is guaranteed
      // by btmesh_conf_distributor_create that all non-allocated pointers in
      // the array are set to NULL.
      btmesh_conf_executor_destroy(self->executors[idx]);
    }
    // Deallocate the array of pointers
    free(self->executors);
  }
  // Deallocate the configuration distributor instance itself
  free(self);
}

sl_status_t btmesh_conf_distributor_submit_job(btmesh_conf_distributor_t *const self,
                                               btmesh_conf_job_t* job)
{
  sl_status_t submit_job_status = SL_STATUS_OK;

  if ((NULL == job) || (NULL == job->task_tree)) {
    submit_job_status = SL_STATUS_NULL_POINTER;
  } else if (BTMESH_CONF_JOB_RESULT_UNKNOWN != job->result) {
    // The job was already executed
    submit_job_status = SL_STATUS_INVALID_PARAMETER;
  } else {
    // Add configuration job to the wait queue
    sl_slist_push_back(&self->waiting_jobs,
                       &job->list_elem);
    btmesh_conf_distributor_schedule(self, BTMESH_CONF_SCHEDULE_DEFERRED);
  }
  return submit_job_status;
}

void btmesh_conf_distributor_on_job_notification(btmesh_conf_distributor_t *const self,
                                                 btmesh_conf_executor_t *const executor,
                                                 btmesh_conf_job_t *const job)
{
  // Notify the upper layer about the changed job status
  btmesh_conf_on_job_notification(job);

  // The configuration executor has just become idle so schedule is called to
  // start the execution of a waiting job
  btmesh_conf_distributor_schedule(self, BTMESH_CONF_SCHEDULE_NORMAL);
}

void btmesh_conf_distributor_on_event(btmesh_conf_distributor_t *const self,
                                      const sl_btmesh_msg_t *evt)
{
  // Forward BT Mesh stack event to configuration executors
  for (uint16_t idx = 0; idx < self->executor_count; idx++) {
    btmesh_conf_executor_on_event(self->executors[idx], evt);
  }
}

void btmesh_conf_distributor_step(btmesh_conf_distributor_t *const self)
{
  // Deferred scheduling request is executed in the cyclic step function
  if (false != self->deferred_schedule_request) {
    btmesh_conf_distributor_schedule(self, BTMESH_CONF_SCHEDULE_NORMAL);
  }
}

/***************************************************************************//**
 * @addtogroup btmesh_conf_distributor_internal \
 * BT Mesh Configuration Distributor Internal
 * @{
 ******************************************************************************/
static sl_status_t btmesh_conf_distributor_start_job(btmesh_conf_distributor_t *const self,
                                                     btmesh_conf_job_t *job)
{
  btmesh_conf_executor_t *idle_executor = NULL;
  sl_status_t sc = SL_STATUS_OK;

  for (uint16_t exec_idx = 0; exec_idx < self->executor_count; exec_idx++) {
    btmesh_conf_executor_status_t executor_status;

    // Note: self->executors is an array of pointers to struct therefore the
    //       address of (&) operator should not be used here
    btmesh_conf_executor_get_status(self->executors[exec_idx], &executor_status);

    if ((BTMESH_CONF_EXEC_STATE_IDLE == executor_status.state)
        && (NULL == idle_executor)) {
      // First idle configuration executor is found
      idle_executor = self->executors[exec_idx];
    } else {
      if ((executor_status.enc_netkey_index == job->enc_netkey_index)
          && (executor_status.server_address == job->server_address)) {
        // Only one configuration job with the same target node can be active at
        // any given point of time however others can be in waiting state.
        sc = SL_STATUS_IN_PROGRESS;
        break;
      }
    }
  }

  if (SL_STATUS_OK == sc) {
    if (NULL == idle_executor) {
      // All executors are busy with another job
      sc = SL_STATUS_NO_MORE_RESOURCE;
    } else {
      sc = btmesh_conf_executor_start_job(idle_executor, job);
    }
  }
  return sc;
}

static void btmesh_conf_distributor_schedule(btmesh_conf_distributor_t *const self,
                                             btmesh_conf_schedule_t schedule_type)
{
  if (BTMESH_CONF_SCHEDULE_DEFERRED == schedule_type) {
    self->deferred_schedule_request = true;
  } else {
    sl_status_t start_job_status;
    sl_slist_node_t **next_ptr_of_prev_list_elem = &self->waiting_jobs;

    // Deferred schedule request is accepted when normal scheduling is performed
    self->deferred_schedule_request = false;

    // The first job in the waiting jobs queue is the one which is referenced
    // by the waiting_jobs pointer (linked list head)
    btmesh_conf_job_t *job = SL_SLIST_ENTRY(self->waiting_jobs,
                                            btmesh_conf_job_t,
                                            list_elem);
    while (NULL != job) {
      // If a job is removed from the wait queue (linked list) with sl_slist_pop
      // then it is no longer possible to retrieve the next job because
      // sl_slist_pop sets the next pointer of the job to NULL and consequently
      // the next job is calculated at the beginning of each iteration.
      btmesh_conf_job_t *next_job = SL_SLIST_ENTRY(job->list_elem.node,
                                                   btmesh_conf_job_t,
                                                   list_elem);

      start_job_status = btmesh_conf_distributor_start_job(self, job);

      if (SL_STATUS_OK == start_job_status) {
        // In order to remove a job from the linked list the previous element
        // of the linked list (or the head) shall be stored because this way the
        // current job can be removed by sl_slist_pop which assigns simply the
        // pointer of the previous element to the next element of linked list.
        // Note: In case of sl_slist the elements and the head have the same type
        //       which makes the implementation more simple.
        //       The prev_list_elem is not modified because it remains the same
        //       due to the removal of the current element.
        sl_slist_pop(next_ptr_of_prev_list_elem);
      } else if (SL_STATUS_NO_MORE_RESOURCE == start_job_status) {
        break; // All executors are busy with configuration jobs so stop scheduling
      } else if (SL_STATUS_IN_PROGRESS == start_job_status) {
        // Only one configuration job with the same target node can be active at
        // any given point of time however others can be in waiting state so the
        // iteration shall be continued.
        next_ptr_of_prev_list_elem = &job->list_elem.node;
      } else {
        // The whole job is failed (all requests in job have failed in sequence)
        // so the job shall be removed from the queue and the notification shall
        // be triggered to notify the higher layers about the failure.
        // The notification is not triggered by the executor because it was not
        // possible to start the job at all consequently the ownership transfer
        // to the specific executor is not completed because the job is still in
        // the wait queue.
        // Note: In order to remove the job from the queue the previous element
        //       of the linked list is stored. Therefore the job is removed with
        //       a simple pop operation from the list. (O(1) operation)
        //       The previous list element is not modified because it remains
        //       the same due to the removal of the current element.
        sl_slist_pop(next_ptr_of_prev_list_elem);
        btmesh_conf_on_job_notification(job);
      }
      // The iteration is continued with the next job in the wait queue
      job = next_job;
    }
  }
}

/** @} (end addtogroup btmesh_conf_distributor_internal) */
/** @} (end addtogroup btmesh_conf_distributor) */
/** @} (end addtogroup btmesh_conf) */
