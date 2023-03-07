/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component
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

#ifndef BTMESH_CONF_H
#define BTMESH_CONF_H

#include "btmesh_conf_types.h"
#include "btmesh_conf_job.h"
#include "btmesh_conf_task.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @brief Robust execution of configuration jobs over BT Mesh Stack.
 *
 * BT Mesh configuration jobs can be created from one or more configuration tasks.
 * In order to create configuration jobs and tasks the following header files
 * shall be checked:
 *   - BT Mesh Configuration Job interface can be found in btmesh_conf_job.h
 *     header file (@ref btmesh_conf_job group).
 *   - BT Mesh Configuration Task interface can be found in btmesh_conf_task.h
 *     header file (@ref btmesh_conf_task group).
 *
 * The created jobs can be submitted for execution to BT Mesh Configurator by
 * calling the @ref btmesh_conf_submit_job.
 *
 * If the application provides a callback (@ref btmesh_conf_on_job_notification_t)
 * when it creates the configuration job then the application is notified about
 * changed job status.
 * @n The btmesh_conf_job_t::result can be checked in the notification callback
 * to figure out if the configuration job was successful or not.
 * The allocation and deallocation and ownership of configuration jobs are
 * described in @ref btmesh_conf_submit_job.
 *
 * There are some common configuration use cases when the execution of job with
 * a single configuration task is necessary. Some utility functions are provided
 * in order to simplify the submission of these common configuration jobs:
 *   - @ref btmesh_conf_dcd_get
 *   - @ref btmesh_conf_reset_node
 *
 * These utility functions create configuration jobs and tasks and submit them
 * with @ref btmesh_conf_submit_job function the same way as the application
 * would do. The finished configuration job which is created by utility function
 * is deallocated automatically.
 * @{
 ******************************************************************************/

/// Information about the supported configuration operations on a model
typedef enum {
  /// No attribute flags
  BTMESH_CONF_SIG_MODEL_ATTRIBUTES_NONE = 0,
  /// Subscription supported
  BTMESH_CONF_SIG_MODEL_SUPPORTS_SUBSCRIPTION = (1 << 0),
  /// Publication supported
  BTMESH_CONF_SIG_MODEL_SUPPORTS_PUBLICATION = (1 << 1),
  /// Application key binding supported
  BTMESH_CONF_SIG_MODEL_SUPPORTS_APPKEY_BINDING = (1 << 2)
} btmesh_conf_sig_model_attr_bitmask_t;

/***************************************************************************//**
 * Initialize the BT Mesh Configurator component
 *
 * @returns Status of the initialization.
 * @retval SL_STATUS_OK If the initialization is successful.
 * @retval SL_STATUS_ALLOCATION_FAILED If the dynamic memory allocation of
 *   internal data structures is failed.
 * @retval SL_STATUS_INVALID_CONFIGURATION If the configured model information
 *   is not valid. (array elems are not arranged in ascending order by model ID)
 *
 * The initialization is performed automatically when the BT Mesh Stack triggers
 * the sl_btmesh_evt_prov_initialized_id event. The application code shall call
 * this function if it deinitialized the configurator component beforehand
 * (@ref btmesh_conf_deinit).
 ******************************************************************************/
sl_status_t btmesh_conf_init(void);

/***************************************************************************//**
 * Deinitialize the BT Mesh Configurator component
 *
 * @returns Status of the deinitialization.
 * @retval SL_STATUS_OK If the deinitialization is successful.
 * @retval SL_STATUS_INVALID_STATE If the deinitialization can't be started
 *   because it is initiated from @ref btmesh_conf_job_t::on_job_notification
 *   callback.
 *
 * Deallocates the internal data structures and stops the processing of BT Mesh
 * stack events. Deinitialization shall not be called from
 * @ref btmesh_conf_job_t::on_job_notification callback.
 *
 ******************************************************************************/
sl_status_t btmesh_conf_deinit(void);

/***************************************************************************//**
 * Submit a configuration job for execution
 *
 * @param[in] job Configuration job which aggregates the configuration tasks
 * @returns Status of the job submission.
 * @retval SL_STATUS_OK If the job is submitted successfully.
 * @retval SL_STATUS_INVALID_STATE If the BT Mesh configurator is not initialized.
 *
 * Each configuration job aggregates configuration tasks which targets the same
 * remote node (configuration server model).
 *
 * If a configuration job is submitted to the BT Mesh Configurator then it is
 * added to the end of a wait queue. Therefore it is not guaranteed that the
 * execution a job is started immediately.
 * Configuration jobs with the same server addresses are executed in the order
 * of submission however jobs with different server addresses might be executed
 * in parallel.
 *
 * If the submit operation is successful then the BT Mesh Configurator takes
 * over the ownership of the job and consequently the application is no longer
 * allowed to modify the submitted job or its tasks.
 * If the submit operation fails and @ref btmesh_conf_job_t::auto_destroy is set
 * and @ref SL_BTMESH_CONF_JOB_AUTO_DESTROY_ON_SUBMIT_FAILURE_CFG_VAL is turned on then the
 * job is deallocated automatically in the submit call and consequently the job
 * shall not be referenced when the submit returns with failure.
 *
 * If task execution path ends in the configuration job then the job ends as
 * well and @ref btmesh_conf_job_t::on_job_notification callback is called with
 * the result. The btmesh_conf_job_t::result structure member contains the
 * result of the job.
 * If the @ref btmesh_conf_job_t::auto_destroy is set to true then the job and
 * its tasks are deallocated automatically after the callback returns.
 *
 * @warning If the auto destroy feature is used then the job shall not be
 * referenced after the callback is executed.
 *
 * @note Te BT Mesh Stack does not allow the parallel execution of two
 * configuration requests with the same server address (primary element).
 * The limitation is added to the BT Mesh Stack because the configuration
 * messages don't contain any transaction identifier in accordance with BT
 * Mesh Profile specification so the status messages could not be differentiated.
 *
 ******************************************************************************/
sl_status_t btmesh_conf_submit_job(btmesh_conf_job_t *job);

/***************************************************************************//**
 * Submit a configuration job with a DCD get task
 *
 * @param[in] netkey_index Network key used to encrypt request on network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] page Composition data page to query
 * @param[in] on_job_notification Callback to notify the app about job status
 * @returns Status of the DCD get request.
 * @retval SL_STATUS_OK If the job is submitted successfully.
 * @retval SL_STATUS_INVALID_STATE If the BT Mesh configurator is not initialized.
 * @retval SL_STATUS_ALLOCATION_FAILED If allocation of job or task is failed.
 *
 * Configuration job with DCD get task is created automatically and submitted
 * for execution. The allocation and deallocation of task and job is handled
 * automatically in this function.
 *
 * @note It is a quite common usecase to get the DCD first and based on the
 * content of the DCD further configuration tasks are defined and executed.
 * @n This utility function makes it easier to get the DCD however it is
 * possible to query the DCD with the following (basic) API calls as well:
 *   - @ref btmesh_conf_task_dcd_get_create
 *   - @ref btmesh_conf_job_create
 *   - @ref btmesh_conf_submit_job
 ******************************************************************************/
sl_status_t btmesh_conf_dcd_get(uint16_t netkey_index,
                                uint16_t server_address,
                                uint8_t page,
                                btmesh_conf_on_job_notification_t on_job_notification);

/***************************************************************************//**
 * Submit a configuration job with a reset node task
 *
 * @param[in] netkey_index Network key used to encrypt request on network layer
 * @param[in] server_address Destination node primary element address
 * @param[in] on_job_notification Callback to notify the app about job status
 * @returns Status of the reset node request.
 * @retval SL_STATUS_OK If the job is submitted successfully.
 * @retval SL_STATUS_INVALID_STATE If the BT Mesh configurator is not initialized.
 * @retval SL_STATUS_ALLOCATION_FAILED If allocation of job or task is failed.
 *
 * Configuration job with reset node task is created automatically and submitted
 * for execution. The allocation and deallocation of task and job is handled
 * automatically in this function.
 *
 * @note This utility function makes it easier to reset a node however it is
 * possible to reset a node with the following (basic) API calls as well:
 *   - @ref btmesh_conf_task_reset_node_create
 *   - @ref btmesh_conf_job_create
 *   - @ref btmesh_conf_submit_job
 ******************************************************************************/
sl_status_t btmesh_conf_reset_node(uint16_t netkey_index,
                                   uint16_t server_address,
                                   btmesh_conf_on_job_notification_t on_job_notification);

/***************************************************************************//**
 * BT Mesh Configurator event handler
 *
 * @param[in] evt BT Mesh Stack event
 *
 * The BT Mesh Configurator event handler shall be called with event provided
 * sl_btmesh_pop_event or sl_btmesh_wait_event BT Mesh Stack API functions.
 ******************************************************************************/
void btmesh_conf_on_event(const sl_btmesh_msg_t *evt);

/***************************************************************************//**
 * Process one step of BT Mesh Configurator
 *
 * The step API shall be called periodically from the main loop of the application.
 ******************************************************************************/
void btmesh_conf_step(void);

/***************************************************************************//**
 * Job status notification callback for lower layers
 *
 * @param[in] job Reference of the job with changed status
 *
 * This callback is called when the status of the job changes because its
 * execution is ended. The result of the job is provided in the
 * @ref btmesh_conf_job_t::result structure member.
 * This callback calls btmesh_conf_job_t::on_job_notification with the @p job
 * to notify the application. If btmesh_conf_job_t::on_job_notification is NULL
 * then the application is not notified.
 ******************************************************************************/
void btmesh_conf_on_job_notification(btmesh_conf_job_t *const job);

/***************************************************************************//**
 * Provides the handle value from Configuration Client events of BT Mesh Stack
 *
 * @param[in] evt BT Mesh Stack event
 * @param[out] handle Handle from configuration client event is stored here
 * @returns Status of the handle retrieval from event.
 * @retval SL_STATUS_OK If config client handle is retrieved successfully.
 * @retval SL_STATUS_NOT_FOUND If the given event is not a config client event.
 * @retval SL_STATUS_NULL_POINTER If any of its parameter is a NULL pointer.
 ******************************************************************************/
sl_status_t btmesh_conf_get_handle_from_event(const sl_btmesh_msg_t *evt,
                                              uint32_t *handle);

/***************************************************************************//**
 * Provide the model attributes of the given BT Mesh SIG model ID
 *
 * @param[in] model_id Model ID for the BT Mesh SIG model
 * @param[out] attributes Attributes of the model is stored here
 * @returns Status of the get model attributes.
 * @retval SL_STATUS_OK If model attributes are provided successfully.
 * @retval SL_STATUS_NOT_FOUND If the model ID is unknown.
 * @retval SL_STATUS_NULL_POINTER If NULL pointer is given as @ref attributes.
 ******************************************************************************/
sl_status_t btmesh_conf_get_sig_model_attributes(uint16_t model_id,
                                                 btmesh_conf_sig_model_attr_bitmask_t *attributes);

/***************************************************************************//**
 * Return string representation of the given BT Mesh SIG model ID
 *
 * @param[in] model_id Model ID for the BT Mesh SIG model
 * @returns String representation of BT Mesh SIG model ID (model name)
 * @retval "UnknownSigModel" If the model ID is unknown.
 *
 * @note This function always returns a valid string which makes it easier to
 *    use this function with snprintf like functions because those expects valid
 *   (non-NULL) for %s format specifier.
 ******************************************************************************/
const char *btmesh_conf_sig_model_id_to_string(uint16_t model_id);

/***************************************************************************//**
 * Check if a given BT Mesh Stack event is a configuration client event
 *
 * @param[in] event_id Identifier of the given BT Mesh Stack event
 * @retval true If the event is a configuration client event.
 * @retval false If the event is NOT a configuration client event.
 ******************************************************************************/
bool btmesh_conf_is_configuration_event(uint32_t event_id);

/***************************************************************************//**
 * Check if a given log level or other more severe log levels are enabled
 *
 * @param[in] start_level Least severe log level checked
 * @retval true If given log level or other more severe log levels are enabled.
 * @retval false If no severe log levels are enabled.
 ******************************************************************************/
bool btmesh_conf_any_severe_log_level(uint8_t start_level);

/** @} (end addtogroup btmesh_conf) */

#ifdef __cplusplus
};
#endif

#endif /* BTMESH_CONF_H */
