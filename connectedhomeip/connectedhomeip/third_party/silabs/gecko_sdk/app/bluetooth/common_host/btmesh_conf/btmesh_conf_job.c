/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component - Job
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

#include "sl_common.h"
#include "btmesh_conf_job.h"
#include "btmesh_conf_config.h"

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_job BT Mesh Configuration Job
 * @{
 ******************************************************************************/
btmesh_conf_job_t *btmesh_conf_job_create_default(uint16_t enc_netkey_index,
                                                  uint16_t server_address,
                                                  btmesh_conf_task_t *task_tree,
                                                  btmesh_conf_on_job_notification_t on_job_notification)
{
  // Call general btmesh_conf_job_create with default parameters
  return btmesh_conf_job_create(enc_netkey_index,
                                server_address,
                                task_tree,
                                on_job_notification,
                                BTMESH_CONF_VARG_NULL,
                                SL_BTMESH_CONF_JOB_AUTO_DESTROY_CFG_VAL,
                                NULL);
}

btmesh_conf_job_t *btmesh_conf_job_create(uint16_t enc_netkey_index,
                                          uint16_t server_address,
                                          btmesh_conf_task_t *task_tree,
                                          btmesh_conf_on_job_notification_t on_job_notification,
                                          btmesh_conf_varg_t job_status_param,
                                          bool auto_destroy,
                                          uint32_t *const job_id)
{
  btmesh_conf_job_t *self;
  self = malloc(sizeof(btmesh_conf_job_t));

  if (NULL != self) {
    // Generate configuration job identifier to differentiate jobs easily
    self->job_id = btmesh_conf_job_id_generator();
    self->list_elem.node = NULL;
    self->current_task = NULL;
    self->enc_netkey_index = enc_netkey_index;
    self->server_address = server_address;
    self->task_tree = task_tree;
    self->result = BTMESH_CONF_JOB_RESULT_UNKNOWN;
    self->on_job_notification = on_job_notification;
    self->job_status_param = job_status_param;
    self->auto_destroy = auto_destroy;

    if (NULL != job_id) {
      // It is not mandatory to store the job identifier in the application
      *job_id = self->job_id;
    }
  }
  return self;
}

void btmesh_conf_job_destroy(btmesh_conf_job_t *const self)
{
  // Destroy function shall behave as free if NULL pointer is passed
  if (NULL == self) {
    return;
  }
  // Destroy all configuration tasks in task_tree
  btmesh_conf_task_destroy(self->task_tree);
  free(self);
}

sl_status_t btmesh_conf_job_set_next_task(btmesh_conf_job_t *const self)
{
  sl_status_t sc;
  if (NULL == self->current_task) {
    // First task of the configuration job shall be set
    if (self->task_tree == NULL) {
      sc = SL_STATUS_FAIL;
    } else {
      self->current_task = self->task_tree;
      sc = SL_STATUS_OK;
    }
  } else {
    // Set the next configuration task based on the result of the current task
    if (SL_STATUS_OK == self->current_task->result) {
      self->current_task = self->current_task->next_on_success;
    } else {
      self->current_task = self->current_task->next_on_failure;
    }

    if (NULL == self->current_task) {
      // There is no additional task in the task_tree of the configuration job
      sc = SL_STATUS_NOT_FOUND;
    } else {
      sc = SL_STATUS_OK;
    }
  }
  return sc;
}

SL_WEAK uint32_t btmesh_conf_job_id_generator(void)
{
  // Job identifiers are generated by incrementing the previous one
  static uint32_t job_id = 0;
  return job_id++;
}

/** @} (end addtogroup btmesh_conf_job) */
/** @} (end addtogroup btmesh_conf) */
