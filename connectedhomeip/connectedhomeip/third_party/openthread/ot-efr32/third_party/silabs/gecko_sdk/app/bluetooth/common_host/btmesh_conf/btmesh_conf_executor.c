/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component - Job Executor
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

#include "app_assert.h"
#include "app_log.h"

#include "btmesh_conf.h"
#include "btmesh_conf_types.h"
#include "btmesh_conf_config.h"
#include "btmesh_conf_executor.h"
#include "btmesh_conf_distributor.h"

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_executor BT Mesh Configuration Executor
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_executor_internal \
 * BT Mesh Configuration Executor Internal
 * @{
 ******************************************************************************/
/// Local log new line shortcut definition
#define NL APP_LOG_NL

/// BT Mesh Configuration Executor notification status
typedef enum {
  BTMESH_CONF_NOTIFICATION_ENABLE,  ///< Jobs status notification is enabled
  BTMESH_CONF_NOTIFICATION_DISABLE, ///< Jobs status notification is disabled
} btmesh_conf_notification_status_t;

/***************************************************************************//**
 * Send configuration request to destination node.
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[in] notification_status Job status notification is enabled or disabled
 * @returns Status of the configuration request.
 * @retval SL_STATUS_OK Configuration request is accepted.
 * @retval SL_STATUS_BUSY Request is rejected due to busy lower layers.
 * @retval SL_STATUS_FAIL Requests are rejected due to unrecoverable error.
 *   Multiple task request could fail here. If one task request fails then the
 *   next task is set until one task request is not failed or the last task fails.
 *
 * The current configuration task specific configuration request is sent to the
 * configuration server model on the primary element of destination node.
 *
 * If the btmesh_conf_task_t::conf_request is successful (SL_STATUS_OK) then
 * an event timeout timer is started to detect missing events. This is extremely
 * rare but the state machine would be stuck in case of missing events without
 * this safety net.
 *
 * If the btmesh_conf_task_t::conf_request is rejected due to busy lower layers
 * (SL_STATUS_BUSY) and the maximum number of retries is not reached then a timer
 * is started with retry interval in order to try to send the configuration
 * request again.
 * The BT Mesh Stack can be busy due to unavailable resources for example when
 * the maximum number of segmented TX messages is reached.
 * See @a SL_BTMESH_CONFIG_MAX_SEND_SEGS and @a SL_BTMESH_CONFIG_APP_TXQ_SIZE
 * BT Mesh Stack configuration defines in sl_btmesh_config.h.
 *
 * If the btmesh_conf_task_t::conf_request is rejected due unrecoverable error
 * (SL_STATUS_FAIL) then the next configuration task is selected and the
 * btmesh_conf_task_t::conf_request of next task is called and this procedure is
 * repeated until the btmesh_conf_task_t::conf_request doesn't fail or the last
 * task fails.
 ******************************************************************************/
static sl_status_t executor_conf_request(btmesh_conf_executor_t *const self,
                                         btmesh_conf_notification_status_t notification_status);

/***************************************************************************//**
 * Process the status of configuration task event handler.
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[in] task_status Status of configuration task event handler
 *
 * The status of configuration task is returned by btmesh_conf_task_t::on_event.
 *
 * If the configuration task is successful (SL_STATUS_OK) then the executor sets
 * the next configuration task and calls @ref executor_conf_request in order
 * to send the configuration request to the destination node.
 *
 * If the configuration task fails due to server side error (SL_STATUS_FAIL) or
 * due to unexpected critical error (SL_STATUS_ABORT) then the executor sets the
 * next configuration task and calls @ref executor_conf_request in order
 * to send the configuration request to the destination node.
 * In case of SL_STATUS_ABORT the configuration request is canceled first
 * because the it is still in progress.
 *
 * If configuration task timeout occurs (SL_STATUS_TIMEOUT) and the maximum
 * number of retries is not reached then configuration request is sent to the
 * destination node by @ref executor_conf_request function call.
 * If the maximum number of retries is reached then the current configuration
 * task fails and the executor sets the next configuration task and calls
 * @ref executor_conf_request in order to send the configuration request to the
 * destination node.
 *
 * If the configuration task is in progress (SL_STATUS_IN_PROGRESS) then the
 * task is waiting for additional events so no operation is required.
 *
 * @note The next configuration task is set by @ref btmesh_conf_job_set_next_task
 *   based on the result of the current configuration task. If the current task
 *   is the last task then @ref executor_finish_job is called to transition into
 *   idle state and to notify the configuration distributor.
 ******************************************************************************/
static void executor_process_task_status(btmesh_conf_executor_t *const self,
                                         sl_status_t task_status);

/***************************************************************************//**
 * Set next BT Mesh configuration task based on the current active task of a job
 *
 * @param[in] self Pointer to the configuration executor instance
 * @return Status of the next task setup
 * @retval SL_STATUS_OK If next task is set properly.
 * @retval SL_STATUS_NOT_FOUND If next is not set because this was the last task.
 * @retval SL_STATUS_FAIL If fatal error occurred.
 ******************************************************************************/
static sl_status_t executor_set_next_task(btmesh_conf_executor_t *const self);

/***************************************************************************//**
 * Transition BT Mesh Configuration Executor to idle state and notify
 * configuration distributor about the finished configuration job.
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[in] job_result Result of BT Mesh Configuration job
 * @param[in] notification_status Job status notification is enabled or disabled
 *
 * The configuration distributor is notified only if @p notification_status is
 * set to @p BTMESH_CONF_NOTIFICATION_ENABLE.
 ******************************************************************************/
static void executor_finish_job(btmesh_conf_executor_t *const self,
                                btmesh_conf_job_result_t job_result,
                                btmesh_conf_notification_status_t notification_status);

/***************************************************************************//**
 * Execute state transition of BT Mesh Configuration Executor instance
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[in] target_state Target state of the state transition
 *
 * State transition has three steps:
 *   - Exit action of source state
 *   - Transition action
 *   - Entry action of target state
 * @note If source and target state of the transition is the same then the
 *   implementation considers it a self-transition and executes the exit and
 *   entry action of that state.
 *
 * The state transition function controls timeout and retry interval measurement.
 * The lifecycle of each timer is bound to a state in order to guarantee:
 *   - Only one timer runs at any given point of time and therefore the timer
 *     structure in @ref btmesh_conf_executor_t can be shared.
 *   - Running timer is stopped when the current state is left.
 * The retry counters are cleared at the beginning of each configuration task
 * because each configuration task has the same amount of retry opportunities.
 ******************************************************************************/
static void executor_state_transition(btmesh_conf_executor_t *const self,
                                      btmesh_conf_executor_state_t target_state);

/***************************************************************************//**
 * Provide string representation of BT Mesh Configuration Executor state.
 *
 * @param[in] state Configuration executor state
 * @returns String representation of configuration executor state.
 * @retval "unknown" If the state parameter contains invalid value.
 ******************************************************************************/
const char *executor_state_to_string(btmesh_conf_executor_state_t state)
{
  switch (state) {
    case BTMESH_CONF_EXEC_STATE_IDLE:
      return "idle";
    case BTMESH_CONF_EXEC_STATE_TASK_SET:
      return "task set";
    case BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT:
      return "event wait";
    case BTMESH_CONF_EXEC_STATE_TASK_REQUEST_BUSY:
      return "request busy";
    default:
      return "invalid";
  }
}

/***************************************************************************//**
 * Callback which is called when the executor timer elapses
 *
 * @param[in] timer Pointer to the elapsed timer instance
 * @param[in] data Pointer to the configuration executor instance
 ******************************************************************************/
static void executor_on_timer_elapsed(sl_simple_timer_t *timer, void *data);

/***************************************************************************//**
 * Provide information about retry occurrence during the execution of current
 * configuration task.
 *
 * @param[in] self Pointer to the configuration executor instance
 * @returns Retry occurrence during the execution of current configuration task.
 * @retval true If at least one retry occurred during task execution.
 * @retval false If no retry occurred during task execution.
 ******************************************************************************/
static inline bool executor_retry_occurred(btmesh_conf_executor_t *const self)
{
  return (0 < self->local_retry_counter)
         || (0 < self->communication_retry_counter);
}

/** @} (end addtogroup btmesh_conf_executor_internal) */

btmesh_conf_executor_t *btmesh_conf_executor_create(uint16_t id,
                                                    btmesh_conf_distributor_t *const parent)
{
  btmesh_conf_executor_t *self;

  if (NULL == parent) {
    // If the parent parameter is NULL then the configuration executor creation
    // fails and therefore NULL is returned to the caller.
    return NULL;
  }

  self = malloc(sizeof(btmesh_conf_executor_t));

  if (NULL != self) {
    self->id = id;
    self->parent = parent;
    self->state = BTMESH_CONF_EXEC_STATE_IDLE;
    self->current_job = NULL;
    self->local_retry_counter = 0;
    self->local_retry_max = SL_BTMESH_CONF_REQUEST_BUSY_RETRY_MAX_CFG_VAL;
    self->communication_retry_counter = 0;
    self->communication_retry_max = SL_BTMESH_CONF_COMMUNICATION_RETRY_MAX_CFG_VAL;
    self->timer_active = false;
  }
  return self;
}

void btmesh_conf_executor_destroy(btmesh_conf_executor_t* self)
{
  // Destroy function shall behave as free if NULL pointer is passed
  if (NULL == self) {
    return;
  }
  if (BTMESH_CONF_EXEC_STATE_IDLE != self->state) {
    // Configuration executor shall transition to idle state in order to
    // stop the timer.
    // Note: If the timer was not stopped then the timer callback would use a
    //       pointer to an already deallocated configuration executor instance.
    executor_state_transition(self, BTMESH_CONF_EXEC_STATE_IDLE);
    btmesh_conf_job_destroy(self->current_job);
  }
  free(self);
}

sl_status_t btmesh_conf_executor_start_job(btmesh_conf_executor_t *const self,
                                           btmesh_conf_job_t *job)
{
  app_assert_s(NULL != self);
  app_assert_s(NULL != job);

  sl_status_t sc = SL_STATUS_INVALID_STATE;

  if (BTMESH_CONF_EXEC_STATE_IDLE == self->state) {
    self->current_job = job;
    sc = executor_set_next_task(self);
    if (SL_STATUS_OK == sc) {
      // Notifications are not allowed to avoid destroying the job before
      // it is removed from the distributor wait queue during scheduling
      sc = executor_conf_request(self, BTMESH_CONF_NOTIFICATION_DISABLE);
      // If the status of configuration request is busy then it means that the
      // BT Mesh stack was not able to accept the request temporarily but the
      // executor will try again when the retry time elapses.
      // Therefore, the configuration job start is considered to be successful.
      sc = (SL_STATUS_BUSY == sc) ? SL_STATUS_OK : sc;
    } else {
      // The configuration task which shall be executed first could not be set
      sc = SL_STATUS_NOT_FOUND;
    }
  }
  return sc;
}

void btmesh_conf_executor_get_status(btmesh_conf_executor_t *const self,
                                     btmesh_conf_executor_status_t *status)
{
  app_assert_s(NULL != self);
  app_assert_s(NULL != status);

  status->state = self->state;
  if (BTMESH_CONF_EXEC_STATE_IDLE == self->state) {
    status->enc_netkey_index = BTMESH_CONF_NETKEY_INDEX_UNASSIGNED;
    status->server_address = MESH_ADDR_UNASSIGNED;
  } else {
    status->enc_netkey_index = self->current_job->enc_netkey_index;
    status->server_address = self->current_job->server_address;
  }
}

void btmesh_conf_executor_on_event(btmesh_conf_executor_t *const self,
                                   const sl_btmesh_msg_t *evt)
{
  uint32_t handle;

  if (BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT == self->state) {
    sl_status_t handle_status = btmesh_conf_get_handle_from_event(evt, &handle);

    // The configuration events shall be forwarded if the handle in the event
    // is the same as the handle in the executor. The executor shall ignore
    // events which belong to parallel configuration of other nodes.
    // Note: other executors with configuration job targeting different config
    //       server might be active as well.
    // Non-configuration events are always forwareded
    bool forward_event = ((SL_STATUS_OK == handle_status)
                          && (handle == self->handle))
                         || (SL_STATUS_NOT_FOUND == handle_status);

    if (false != forward_event) {
      sl_status_t task_status;
      btmesh_conf_job_t *job = self->current_job;
      // Forward the BT Mesh Stack event with matching handle to the event
      // handler of the current configuration task.
      task_status = job->current_task->on_event(job->current_task,
                                                job->enc_netkey_index,
                                                job->server_address,
                                                evt);
      // Process the status of the current task after it handled this new
      // BT Mesh Stack event
      executor_process_task_status(self, task_status);
    }
  }
}

/***************************************************************************//**
 * @addtogroup btmesh_conf_executor_internal \
 * BT Mesh Configuration Executor Internal
 * @{
 ******************************************************************************/
static sl_status_t executor_conf_request(btmesh_conf_executor_t *const self,
                                         btmesh_conf_notification_status_t notification_status)
{
  sl_status_t conf_request_status, set_next_task_status;
  btmesh_conf_job_t *job = self->current_job;
  bool conf_request_required = true;
  bool current_task_failed = false;
  char task_str[SL_BTMESH_CONF_MAX_LOG_MSG_FRAGMENT_SIZE_CFG_VAL];
  char node_str[SL_BTMESH_CONF_MAX_LOG_MSG_FRAGMENT_SIZE_CFG_VAL];

  // Loop is necessary because it is possible that BT Mesh Stack configuration
  // request of the current task fails. If a configuration request fails with
  // unrecoverable error then there are two possibilities:
  //   - If the negative branch (next_on_failure) of the current task
  //     (in task_tree) is NULL then there is no task which could be executed
  //   - If the negative branch (next_on_failure) of the current task
  //     (in task_tree) references another configuration task then it is set
  //     as the current task and the loop body is executed again.
  // Note: In the worst case if there is a general error then multiple task in
  //       series can fail and whole job ends in this function call.
  //       For example if there is no node with given server address in device
  //       database and therefore it is not possible to retrieve the device key.
  while (false != conf_request_required) {
    // Call the configuration task specific BT Mesh Stack configuration request.
    // Handle returned by the BT Mesh Stack is stored by the executor in order
    // to forward configuration events with matching handle to the task in
    // the btmesh_conf_executor_on_event function.
    conf_request_status = job->current_task->conf_request(job->current_task,
                                                          job->enc_netkey_index,
                                                          job->server_address,
                                                          &self->handle);

    // If any log level is enabled which is used in this function then it is
    // necessary to build the log message fragments.
    if (btmesh_conf_any_severe_log_level(APP_LOG_LEVEL_INFO)) {
      int32_t str_retval;
      // Get the string representation of the current configuration task
      str_retval = job->current_task->to_string(job->current_task,
                                                &task_str[0],
                                                sizeof(task_str));
      app_assert(0 <= str_retval, "String formatting failed." NL);

      // Build log message fragment with node information
      str_retval = snprintf(node_str,
                            sizeof(node_str),
                            "node (netkey_idx=%d,addr=0x%04x,handle=0x%08lx)",
                            job->enc_netkey_index,
                            job->server_address,
                            (unsigned long)self->handle);
      app_assert(0 <= str_retval, "String formatting failed." NL);
    }

    // Defensive programming: make sure that the string is null terminated
    task_str[sizeof(task_str) - 1] = '\0';
    node_str[sizeof(node_str) - 1] = '\0';

    switch (conf_request_status) {
      case SL_STATUS_OK:
        // Configuration request is successful so the configuration task shall
        // wait for the results (BT Mesh Stack event).
        app_log_level(executor_retry_occurred(self)
                      ? APP_LOG_LEVEL_WARNING
                      : APP_LOG_LEVEL_INFO,
                      "Task %s request%s to %s is sent." NL,
                      task_str,
                      executor_retry_occurred(self) ? " (retry)" : "",
                      node_str);
        executor_state_transition(self, BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT);
        conf_request_required = false;
        break;
      case SL_STATUS_BUSY:
        // Configuration request is busy which indicates that some resources in
        // the BT Mesh Stack are not available temporarily so the configuration
        // request shall be repeated later (retry).
        if (self->local_retry_counter < self->local_retry_max) {
          app_log_warning("Task %s request%s to %s is busy (result=0x%04lx)." NL,
                          task_str,
                          executor_retry_occurred(self) ? " (retry)" : "",
                          node_str,
                          (unsigned long) job->current_task->result);
          self->local_retry_counter++;
          executor_state_transition(self, BTMESH_CONF_EXEC_STATE_TASK_REQUEST_BUSY);
          conf_request_required = false;
        } else {
          app_log_error("Task %s request%s to %s is failed (result=0x%04lx) "
                        "because too many configuration requests were busy." NL,
                        task_str,
                        executor_retry_occurred(self) ? " (retry)" : "",
                        node_str,
                        (unsigned long)job->current_task->result);
          current_task_failed = true;
        }
        break;
      default:
        // Unrecoverable task failure
        app_log_error("Task %s request%s to %s is failed (result=0x%04lx)." NL,
                      task_str,
                      executor_retry_occurred(self) ? " (retry)" : "",
                      node_str,
                      (unsigned long)job->current_task->result);
        current_task_failed = true;
        break;
    }

    if (false != current_task_failed) {
      // The negative branch (next_on_failure) of the current task shall be
      // set as the current task in the configuration job.
      set_next_task_status = executor_set_next_task(self);
      self->current_job->result = BTMESH_CONF_JOB_RESULT_FAIL;
      if (SL_STATUS_OK != set_next_task_status) {
        // If there is no further configuration task in the task_tree which
        // can be executed then the whole job fails.
        executor_finish_job(self,
                            BTMESH_CONF_JOB_RESULT_FAIL,
                            notification_status);
        conf_request_required = false;
      }
    }
  }

  return conf_request_status;
}

static void executor_process_task_status(btmesh_conf_executor_t *const self,
                                         sl_status_t task_status)
{
  bool current_task_finished = true;
  bool retry_required = false;
  btmesh_conf_job_t *job = self->current_job;
  char task_str[SL_BTMESH_CONF_MAX_LOG_MSG_FRAGMENT_SIZE_CFG_VAL];
  char node_str[SL_BTMESH_CONF_MAX_LOG_MSG_FRAGMENT_SIZE_CFG_VAL];

  // If any log level is enabled which is used in this function then it is
  // necessary to build the log message fragments.
  if (btmesh_conf_any_severe_log_level(APP_LOG_LEVEL_INFO)) {
    int32_t str_retval;
    // Get the string representation of the current configuration task
    str_retval = job->current_task->to_string(job->current_task,
                                              &task_str[0],
                                              sizeof(task_str));
    app_assert(0 < str_retval, "String formatting failed." NL);

    // Build log message fragment with node information
    str_retval = snprintf(node_str,
                          sizeof(node_str),
                          "node (netkey_idx=%d,addr=0x%04x,handle=0x%08lx)",
                          job->enc_netkey_index,
                          job->server_address,
                          (unsigned long)self->handle);
    app_assert(0 < str_retval, "String formatting failed." NL);
  }
  // Defensive programming: make sure that the string is null terminated
  task_str[sizeof(task_str) - 1] = '\0';
  node_str[sizeof(node_str) - 1] = '\0';

  switch (task_status) {
    case SL_STATUS_OK:
    {
      // The configuration task is finished successfully
      app_log_info("Task %s of %s is completed successfully." NL,
                   task_str,
                   node_str);
      current_task_finished = true;
      break;
    }

    case SL_STATUS_IN_PROGRESS:
    {
      // Task is waiting for additional events so nothing shall be done here.
      current_task_finished = false;
      break;
    }

    case SL_STATUS_TIMEOUT:
    {
      // Configuration request in the BT Mesh Stack triggers message transmission
      // to the configuration server which responds with a status message to the
      // configuration client. If any of these messages are lost then the
      // configuration request timeout occurs in the BT Mesh Stack.
      // Note: BT Mesh Stack sends the acknowledged configuration messages which
      //       is more robust but even these messages might be lost.
      if (self->communication_retry_counter < self->communication_retry_max) {
        // In order to make the configuration procedure more robust the
        // configuration requests are repeated a configurable amount of times.
        // Note: it is possible that the node is not available because it was
        //       removed or turned off so it does not make sense to retry forever.
        self->communication_retry_counter++;
        retry_required = true;
        current_task_finished = false;
      } else {
        // The maximum number of retries are exceeded so the task is failed
        app_log_error("Task %s of %s is timed out (result=0x%04lx)." NL,
                      task_str,
                      node_str,
                      (unsigned long) job->current_task->result);
        self->current_job->result = BTMESH_CONF_JOB_RESULT_FAIL;
        current_task_finished = true;
      }
      break;
    }

    case SL_STATUS_FAIL:
    {
      // Error occurred because configuration server rejected the configuration
      // request and responded to the configuration client with status message
      // with non-zero error status.
      app_log_error("Task %s of %s is failed (result=0x%04lx)." NL,
                    task_str,
                    node_str,
                    (unsigned long) job->current_task->result);
      self->current_job->result = BTMESH_CONF_JOB_RESULT_FAIL;
      current_task_finished = true;
      break;
    }

    case SL_STATUS_ABORT:
    {
      // Local error occurred in the configuration task during the BT Mesh Stack
      // event processing and the execution of the current task shall be aborted.
      app_log_error("Task %s of %s is aborted (result=0x%04lx)." NL,
                    task_str,
                    node_str,
                    (unsigned long)job->current_task->result);
      // Cancel the configuration request in the BT Mesh stack to make sure the
      // next configuration task won't be rejected because the current one is
      // considered active in the BT Mesh Stack.
      sl_status_t cancel_req_status =
        sl_btmesh_config_client_cancel_request(self->handle);
      app_log_status_warning_f(cancel_req_status,
                               "Task %s request cancellation of %s is failed." NL,
                               task_str,
                               node_str);
      self->current_job->result = BTMESH_CONF_JOB_RESULT_FAIL;
      current_task_finished = true;
      break;
    }
  }

  if (false != retry_required) {
    // It is not necessary to check the return value because the function handles
    // the errors and the error code can't be returned to any SW modules
    (void) executor_conf_request(self, BTMESH_CONF_NOTIFICATION_ENABLE);
  } else if (false != current_task_finished) {
    sl_status_t next_task_status = executor_set_next_task(self);
    if (SL_STATUS_OK == next_task_status) {
      // It is not necessary to check the return value because the function handles
      // the errors and the error code can't be returned to any SW modules
      (void) executor_conf_request(self, BTMESH_CONF_NOTIFICATION_ENABLE);
    } else if (SL_STATUS_NOT_FOUND == next_task_status) {
      btmesh_conf_job_result_t job_result = self->current_job->result;
      // The last task of job is finished. If the job result was not set to
      // failure by any task then the job shall be considered successful
      if (BTMESH_CONF_JOB_RESULT_UNKNOWN == job_result) {
        job_result = BTMESH_CONF_JOB_RESULT_SUCCESS;
      }
      executor_finish_job(self, job_result, BTMESH_CONF_NOTIFICATION_ENABLE);
    } else {
      // Critical error occurred during the setup of the next task
      executor_finish_job(self,
                          BTMESH_CONF_JOB_RESULT_CRITICAL_ERROR,
                          BTMESH_CONF_NOTIFICATION_ENABLE);
    }
  }
}

static sl_status_t executor_set_next_task(btmesh_conf_executor_t *const self)
{
  sl_status_t next_task_status = btmesh_conf_job_set_next_task(self->current_job);
  if (SL_STATUS_OK == next_task_status) {
    executor_state_transition(self, BTMESH_CONF_EXEC_STATE_TASK_SET);
  }
  return next_task_status;
}

static void executor_finish_job(btmesh_conf_executor_t *const self,
                                btmesh_conf_job_result_t job_result,
                                btmesh_conf_notification_status_t notification_status)
{
  // The configuration executor shall transition to idle state before it notifies
  // the configuration distributor in order to make it possible to start a new
  // configuration job. Configuration distributor assumes if the job execution
  // is ended then it is able to schedule new configuration job.
  btmesh_conf_job_t *finished_job = self->current_job;
  self->current_job->result = job_result;
  self->current_job = NULL;
  self->local_retry_counter = 0;
  self->communication_retry_counter = 0;
  executor_state_transition(self, BTMESH_CONF_EXEC_STATE_IDLE);
  if (BTMESH_CONF_NOTIFICATION_ENABLE == notification_status) {
    btmesh_conf_distributor_on_job_notification(self->parent,
                                                self,
                                                finished_job);
  }
}

static void executor_state_transition(btmesh_conf_executor_t *const self,
                                      btmesh_conf_executor_state_t target_state)
{
  app_assert(target_state < BTMESH_CONF_EXEC_STATE_COUNT,
             "Invalid configuration executor state." NL);
  btmesh_conf_executor_state_t source_state = self->state;

  switch (source_state) {
    case BTMESH_CONF_EXEC_STATE_TASK_REQUEST_BUSY:
      if (false != self->timer_active) {
        app_log_debug("BT Mesh Config Executor (id=%d) stops "
                      "request busy retry timer." NL,
                      self->id);
        sl_status_t sc = sl_simple_timer_stop(&self->timer);
        app_assert_status_f(sc, "Failed to stop btmesh_conf_executor timer." NL);
        self->timer_active = false;
      }
      break;

    case BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT:
      if (false != self->timer_active) {
        app_log_debug("BT Mesh Config Executor (id=%d) stops "
                      "waiting for event timeout timer." NL,
                      self->id);
        sl_status_t sc = sl_simple_timer_stop(&self->timer);
        app_assert_status_f(sc, "Failed to stop btmesh_conf_executor timer." NL);
        self->timer_active = false;
      }
      break;

    default:
      break;
  }

  self->state = target_state;
  app_log_debug("BT Mesh Config Executor (id=%d) state transition to \"%s\"." NL,
                self->id,
                executor_state_to_string(target_state));

  switch (target_state) {
    case BTMESH_CONF_EXEC_STATE_TASK_SET:
      // The retry counters are cleared because each configuration task has the
      // same number of retry opportunities.
      self->local_retry_counter = 0;
      self->communication_retry_counter = 0;
      break;

    case BTMESH_CONF_EXEC_STATE_TASK_REQUEST_BUSY:
    {
      sl_status_t sc;
      app_log_debug("BT Mesh Config Executor (id=%d) starts "
                    "request busy retry timer." NL,
                    self->id);
      sc = sl_simple_timer_start(&self->timer,
                                 SL_BTMESH_CONF_REQUEST_BUSY_RETRY_INTERVAL_MS_CFG_VAL,
                                 executor_on_timer_elapsed,
                                 self,
                                 false);
      app_assert_status_f(sc, "Failed to start btmesh_conf_executor timer." NL);
      self->timer_active = true;
      break;
    }

    case BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT:
    {
      sl_status_t sc;
      app_log_debug("BT Mesh Config Executor (id=%d) starts "
                    "waiting for event timeout timer." NL,
                    self->id);
      sc = sl_simple_timer_start(&self->timer,
                                 SL_BTMESH_CONF_EVENT_WAIT_TIMEOUT_MS_CFG_VAL,
                                 executor_on_timer_elapsed,
                                 self,
                                 false);
      app_assert_status_f(sc, "Failed to start btmesh_conf_executor timer." NL);
      self->timer_active = true;
      break;
    }

    default:
      break;
  }
}

static void executor_on_timer_elapsed(sl_simple_timer_t *timer, void *data)
{
  btmesh_conf_executor_t *const self = (btmesh_conf_executor_t *const)data;
  switch (self->state) {
    case BTMESH_CONF_EXEC_STATE_TASK_REQUEST_BUSY:
      app_log_debug("BT Mesh Config Executor (id=%d) "
                    "request busy retry timer has elapsed." NL,
                    self->id);
      self->timer_active = false;
      // The last configuration request was busy due to unavailable resources
      // in the BT Mesh Stack. The retry interval has just elapsed and therefore
      // configuration request retry is executed.
      executor_conf_request(self, BTMESH_CONF_NOTIFICATION_ENABLE);
      break;

    case BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT:
      app_log_debug("BT Mesh Config Executor (id=%d) "
                    "waiting for event timeout timer has elapsed." NL,
                    self->id);
      self->timer_active = false;
      // No BT Mesh Stack configuration event was received with the matching
      // configuration handle which can happen if the event is lost.
      // If no configuration message is received from the configuration server
      // then the BT Mesh Stack generates a configuration event with
      // SL_STATUS_TIMEOUT result so it is guaranteed that an event is generated
      // by the BT Mesh Stack under normal circumstances.
      // A configuration event might be lost due to lack of resources or due to
      // transfer errors during NCP communication.
      // This is extremely rare but the executor state machine would be stuck in
      // case of missing events without this safety net.
      // The solution is to handle the missing event as if BT Mesh Stack
      // configuration event with SL_STATUS_TIMEOUT result had been received
      // because the status of the configuration request is not known so it
      // makes sense to send the configuration request again (retry).
      executor_process_task_status(self, SL_STATUS_TIMEOUT);
      break;

    default:
      app_log_error("Configuration executor timeout occurred in unexpected state." NL);
      break;
  }
}

/** @} (end addtogroup btmesh_conf_executor_internal) */
/** @} (end addtogroup btmesh_conf_executor) */
/** @} (end addtogroup btmesh_conf) */
