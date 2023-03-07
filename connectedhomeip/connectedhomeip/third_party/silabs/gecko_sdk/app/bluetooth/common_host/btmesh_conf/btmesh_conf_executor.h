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

#ifndef BTMESH_CONF_EXECUTOR_H
#define BTMESH_CONF_EXECUTOR_H

#include "btmesh_conf_types.h"
#include "btmesh_conf_job.h"
#include "sl_simple_timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_executor BT Mesh Configuration Executor
 * @brief BT Mesh Configuration Executor executes configuration jobs by executing
 *   the tasks in the task tree of the configuration job.
 *
 * Each executor shall have one parent @ref btmesh_conf_distributor.
 * Parent distributor can command the executor to start configuration job
 * execution by calling @ref btmesh_conf_executor_start_job function.
 * The configuration executor reports the status of job execution by calling
 * @ref btmesh_conf_distributor_on_job_notification function of parent distributor.
 *
 * The relationship between the configuration distributor and its executors is
 * composition so the executors are owned by its parent distributor completely.
 * Only the parent distributor is allowed to start configuration jobs because
 * the executors always report its job statuses to the parent distributor.
 *
 * The robust execution of configuration tasks is guaranteed by the retry
 * mechanism of the executor.
 * @n Retry mechanism is used in the following cases:
 *   - Configuration request timeout in the BT Mesh Stack due to the following:
 *     - BT Mesh message sent to the configuration server is lost
 *     - BT Mesh status message sent by the configuration server is lost
 *   - Configuration event is not received maybe due to event lost over
 *     NCP communication (quite rare)
 *   - Configuration request is busy due to unavailable resources in the
 *     BT Mesh Stack
 * @{
 ******************************************************************************/

typedef struct btmesh_conf_distributor_t btmesh_conf_distributor_t;

/// State constants of BT Mesh Configuration Executor
typedef enum btmesh_conf_executor_state_t {
  /// Configuration Executor is ready to execute waiting configuration jobs
  BTMESH_CONF_EXEC_STATE_IDLE,
  /// Configuration Executor advanced to the next configuration task
  BTMESH_CONF_EXEC_STATE_TASK_SET,
  /// Configuration Executor forwards events to current task and waits for result
  BTMESH_CONF_EXEC_STATE_TASK_EVENT_WAIT,
  /// @brief Configuration Executor waits the retry time because the previous
  ///   task request was busy due to unavailable resources.
  ///
  /// For example the following conditions could lead to busy requests:
  ///   - Maximum number segments allowed for transmitted packets is exceeded
  ///   - Access Layer TX Queue Size
  ///
  /// See @a SL_BTMESH_CONFIG_MAX_SEND_SEGS and @a SL_BTMESH_CONFIG_APP_TXQ_SIZE
  /// BT Mesh Stack configuration defines in sl_btmesh_config.h.
  BTMESH_CONF_EXEC_STATE_TASK_REQUEST_BUSY,
  /// Number of Configuration Executor states
  BTMESH_CONF_EXEC_STATE_COUNT
} btmesh_conf_executor_state_t;

/// Current status of Configuration Executor
typedef struct btmesh_conf_executor_status_t {
  /// State of Configuration Executor
  btmesh_conf_executor_state_t state;
  /// @brief Network key index of the executed configuration job
  /// @n It is valid if the @p state is not @ref BTMESH_CONF_EXEC_STATE_IDLE.
  uint16_t enc_netkey_index;
  /// @brief Destination node primary element address of executed configuration job
  /// @n It is valid if the @p state is not @ref BTMESH_CONF_EXEC_STATE_IDLE.
  uint16_t server_address;
} btmesh_conf_executor_status_t;

/***************************************************************************//**
 * @brief BT Mesh Configuration Executor executes configuration jobs by executing
 *   the tasks in the task tree of the configuration job.
 ******************************************************************************/
typedef struct btmesh_conf_executor {
  /// BT Mesh Configuration Distributor which the executor belongs to
  btmesh_conf_distributor_t *parent;
  /// Pointer to configuration job which is executed
  btmesh_conf_job_t *current_job;
  /// @brief BT Mesh stack configuration handle which is returned by the last
  /// configuration request (BT Mesh Stack API call). Configuration executor
  /// processes those configuration events only which matches this handle.
  uint32_t handle;
  /// Current state of the configuration executor
  btmesh_conf_executor_state_t state;
  /// @brief Identifier of BT Mesh Configuration Executor instance.
  /// @n The identifier of configuration executor instance shall be unique in
  /// the context of parent configuration distributor.
  uint16_t id;
  /// @brief Number of configuration task request retries were executed due to
  /// busy BT Mesh Stack. The BT Mesh Stack might reject configuration client
  /// requests temporarily due to lack of resources. For example maximum number
  /// of parallel segmented message transmissions is reached.
  /// @n The local refers to the fact that no messages were sent to the
  /// configuration server because the BT Mesh Stack did not accept the request.
  uint16_t local_retry_counter;
  /// @brief Maximum number of configuration task request retries were executed
  /// due to busy BT Mesh Stack.
  /// @n The local refers to the fact that no messages were sent to the
  /// configuration server because the BT Mesh Stack did not accept the request.
  uint16_t local_retry_max;
  /// @brief Number of configuration task request retries were executed already
  /// due to BT Mesh Stack timeout. An ongoing BT Mesh Stack configuration client
  /// request might timeout because the configuration request message sent to
  /// the configuration server or the configuration status message sent back to
  /// the configuration client is lost.
  /// This counter is cleared at the beginning of each configuration task of the
  /// configuration job.
  uint16_t communication_retry_counter;
  /// @brief Maximum number of configuration task request retries due to BT Mesh
  /// Stack timeout which indicates lost configuration messages.
  uint16_t communication_retry_max;
  /// Timer to measure event timeout and configuration request retry interval
  sl_simple_timer_t timer;
  /// @brief Timer is active from the the moment it is started until it elapses
  /// or until it is stopped explicitly.
  bool timer_active;
} btmesh_conf_executor_t;

/***************************************************************************//**
 * Create BT Mesh Configuration Executor instance which belongs to the
 * specified configuration distributor
 *
 * @param[in] id Identifier of BT Mesh Configuration Executor instance
 * @param[in] parent BT Mesh Configuration Distributor which executor belongs to
 * @returns Created configuration executor.
 * @retval NULL If the configuration executor creation fails.
 *
 * BT Mesh Configuration Executor is allocated and initialized with the provided
 * BT Mesh Configuration Distributor parent. The initialized configuration
 * executor is in idle state and therefore it does not execute any jobs.
 * The identifier of configuration executor shall be unique in the context of
 * parent configuration distributor.
 ******************************************************************************/
btmesh_conf_executor_t *btmesh_conf_executor_create(uint16_t id,
                                                    btmesh_conf_distributor_t *const parent);

/***************************************************************************//**
 * Deallocates the BT Mesh Configuration Executor instance
 *
 * @param[in] self Pointer to the configuration executor which shall be destroyed
 *
 * If the configuration executor runs a configuration job then the job and all
 * of its tasks are deallocated as well.
 ******************************************************************************/
void btmesh_conf_executor_destroy(btmesh_conf_executor_t *const self);

/***************************************************************************//**
 * Start execution of the specified configuration job in a BT Mesh Configuration
 * Executor instance
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[in] job Configuration job which shall be started
 * @returns Status of configuration job execution start.
 * @retval SL_STATUS_OK If the configuration job is started properly.
 * @retval SL_STATUS_INVALID_STATE If the executor is busy with the execution of
 *   another configuration job.
 * @retval SL_STATUS_NOT_FOUND If the first task of configuration job could not
 *   be set.
 * @retval SL_STATUS_FAIL If one or more configuration requests are failed and
 *   no further configuration task remains in the task tree which can be executed.
 ******************************************************************************/
sl_status_t btmesh_conf_executor_start_job(btmesh_conf_executor_t *const self,
                                           btmesh_conf_job_t *job);

/***************************************************************************//**
 * Get status BT Mesh Configuration Executor instance
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[out] status Pointer where status of the executor instance is written
 ******************************************************************************/
void btmesh_conf_executor_get_status(btmesh_conf_executor_t *const self,
                                     btmesh_conf_executor_status_t *status);

/***************************************************************************//**
 * Event handler of BT Mesh Configuration Executor instance
 *
 * @param[in] self Pointer to the configuration executor instance
 * @param[in] evt BT Mesh Stack event
 *
 * The BT Mesh Configuration Executor event handler forwards events to the
 * current task of the configuration job which is executed at the moment.
 * If the event is a configuration event then it is forwarded only if the handle
 * in the event matches the handle returned by the last configuration BT Mesh
 * Stack request which belongs to the same configuration task.
 ******************************************************************************/
void btmesh_conf_executor_on_event(btmesh_conf_executor_t *const self,
                                   const sl_btmesh_msg_t *evt);

/** @} (end addtogroup btmesh_conf_executor) */
/** @} (end addtogroup btmesh_conf) */

#ifdef __cplusplus
};
#endif

#endif /* BTMESH_CONF_EXECUTOR_H */
