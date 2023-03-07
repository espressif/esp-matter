/***************************************************************************/ /**
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

#ifndef BTMESH_CONF_DISTRIBUTOR_H
#define BTMESH_CONF_DISTRIBUTOR_H

#include "sl_slist.h"
#include "btmesh_conf_types.h"
#include "btmesh_conf_executor.h"
#include "btmesh_conf_job.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_distributor BT Mesh Configuration Distributor
 * @brief BT Mesh Configuration Distributor supports the parallel execution of
 *   multiple configuration jobs by distributing the jobs between executors.
 *
 * BT Mesh Configuration Distributor instance is created by calling the
 * @ref btmesh_conf_distributor_create function.
 * The specified number (@p executor_count) of @ref btmesh_conf_executor instances
 * are created which are assigned to the created distributor instance (composition).
 *
 * BT Mesh Configuration jobs can be submitted to configuration distributor
 * with @ref btmesh_conf_distributor_submit_job function which stores the job in
 * a wait queue.
 *
 * The configuration jobs are assigned to executors from the wait queue during
 * distributor scheduling process. The BT Mesh Stack does not support parallel
 * execution of two configuration job with the same server address.
 * Therefore, the scheduling process guarantees that the configuration jobs with
 * the same server addresses are executed one-by-one in the order of submission
 * however jobs with different server addresses might be executed in parallel.
 *
 * The configuration executors report the job status through
 * @ref btmesh_conf_distributor_on_job_notification function.
 *
 * @note The BT Mesh Stack does not allow the parallel execution of two
 * configuration requests with the same server address (primary element).
 * The limitation is added to the BT Mesh Stack because the configuration
 * messages don't contain any transaction identifier in accordance with BT Mesh
 * Profile specification so the status messages could not be differentiated.
 *
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief BT Mesh Configuration Distributor supports the parallel execution of
 *   multiple configuration jobs by distributing the jobs between executors.
 ******************************************************************************/
struct btmesh_conf_distributor_t {
  /// List of waiting jobs
  sl_slist_node_t *waiting_jobs;
  /// Number of @ref btmesh_conf_executor which belongs to this distributor.
  uint16_t executor_count;
  /// @brief Array of @ref btmesh_conf_executor references.
  /// @n Array of pointers are used in order to make it possible to allocate the
  /// executor instances by calling the @ref btmesh_conf_executor_create function.
  btmesh_conf_executor_t **executors;
  /// @brief Deferred schedule request flag is set when new configuration jobs
  /// are submitted in order to indicate the @ref btmesh_conf_distributor_step
  /// shall schedule the waiting jobs.
  bool deferred_schedule_request;
};

/***************************************************************************//**
 * Create BT Mesh Configuration Distributor instance with the specified number
 * of configuration executors.
 *
 * @param[in] executor_count Number of configuration executors which shall be
 *   created and added to this distributor instance.
 * @returns Created configuration distributor.
 * @retval NULL If the configuration distributor creation fails.
 ******************************************************************************/
btmesh_conf_distributor_t *btmesh_conf_distributor_create(uint16_t executor_count);

/***************************************************************************//**
 * Deallocate the BT Mesh Configuration Distributor instance
 *
 * @param[in] self Pointer to configuration distributor which shall be destroyed
 ******************************************************************************/
void btmesh_conf_distributor_destroy(btmesh_conf_distributor_t *const self);

/***************************************************************************//**
 * Submit a configuration job for execution
 *
 * @param[in] self Pointer to the configuration distributor instance
 * @param[in] job Configuration job which aggregates the configuration tasks
 * @returns Status of the job submission.
 * @retval SL_STATUS_OK If the job is submitted successfully.
 *
 * BT Mesh Configuration Distributor appends the submitted configuration job
 * to the wait queue and requests a deferred scheduling in order to assign the
 * job to an idle @ref btmesh_conf_executor. If all executors are busy then
 * the configuration job remains in the wait queue until an executor transitions
 * to idle state and there is no previously submitted configuration jobs which
 * can be started.
 *
 * It is important to note that a configuration job might be executed sooner
 * than another configuration job which was submitted earlier if the latter
 * configurator job can't be started because one executor runs a third
 * configuration job with the same server address.
 *
 * If the execution of a configuration job is finished then the configuration
 * executor calls the @ref btmesh_conf_distributor_on_job_notification to notify
 * the distributor.
 ******************************************************************************/
sl_status_t btmesh_conf_distributor_submit_job(btmesh_conf_distributor_t *const self,
                                               btmesh_conf_job_t *job);

/***************************************************************************//**
 * BT Mesh Configuration Distributor job status notification callback for
 * @ref btmesh_conf_executor
 *
 * @param[in] self Pointer to the configuration distributor instance
 * @param[in] executor Pointer to the executor which ran the configuration job
 * @param[in] job Pointer to the job with changed status
 *
 * This callback is called when the status of the job changes because its
 * execution is ended.
 ******************************************************************************/
void btmesh_conf_distributor_on_job_notification(btmesh_conf_distributor_t *const self,
                                                 btmesh_conf_executor_t *const executor,
                                                 btmesh_conf_job_t *const job);

/***************************************************************************//**
 * Event handler of BT Mesh Configuration Distributor instance
 *
 * @param[in] self Pointer to the configuration distributor instance
 * @param[in] evt BT Mesh Stack event
 *
 * BT Mesh Configuration Distributor forwards the BT Mesh Stack events to all
 * of its @ref btmesh_conf_executor instances by calling the
 * @ref btmesh_conf_executor_on_event function with each configuration executor
 * instance.
 ******************************************************************************/
void btmesh_conf_distributor_on_event(btmesh_conf_distributor_t *const self,
                                      const sl_btmesh_msg_t *evt);

/***************************************************************************//**
 * Process one step of BT Mesh Configurator
 *
 * @param[in] self Pointer to the configuration distributor instance
 *
 * The step function shall be called periodically by higher layer to perform
 * deferred scheduling.
 ******************************************************************************/
void btmesh_conf_distributor_step(btmesh_conf_distributor_t *const self);

/** @} (end addtogroup btmesh_conf_distributor) */
/** @} (end addtogroup btmesh_conf) */

#ifdef __cplusplus
};
#endif

#endif /* BTMESH_CONF_DISTRIBUTOR_H */
