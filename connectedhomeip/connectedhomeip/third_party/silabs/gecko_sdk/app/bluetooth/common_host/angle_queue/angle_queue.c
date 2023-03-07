/***************************************************************************//**
 * @file
 * @brief Angle queue
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "angle_queue.h"
#include "aoa_util.h"

// -----------------------------------------------------------------------------
// Defines
#define ROUND_DIV(num, den) (((num) + ((den) / 2)) / (den))

// -----------------------------------------------------------------------------
// Type definitions.

// Forward declarations
typedef struct angle_queue angle_queue_t;
typedef struct angle_data angle_data_t;

// Angle queue
struct angle_queue{
  aoa_id_t tag_id;
  angle_data_t *data;
  angle_queue_t *next;
};

// Angle data
struct angle_data{
  int32_t sequence;
  uint32_t angle_count;
  aoa_id_t *locators;
  aoa_angle_t *angles;
};

// -----------------------------------------------------------------------------
// Private function declarations.
static sl_status_t new_queue(aoa_id_t tag_id, angle_queue_t **queue);
static sl_status_t get_queue_by_id(aoa_id_t tag_id, angle_queue_t **queue);
static void add_data(aoa_id_t locator_id,
                     aoa_angle_t *angle,
                     angle_queue_t *queue);
static uint32_t get_data_index(int32_t sequence, angle_queue_t *queue);
static void check_data(aoa_id_t tag_id,
                       aoa_id_t locator_id,
                       angle_queue_t *queue);
static void remove_old_data(int32_t sequence, angle_queue_t *queue);
static void cleanup_data(int32_t index, angle_queue_t *queue);
static uint32_t get_expected_angles(int32_t index);
// -----------------------------------------------------------------------------
// Module variables.

// Queue head
static angle_queue_t *head_queue = NULL;

// Configuration
static angle_queue_config_t angle_queue_config = ANGLE_QUEUE_DEFAULT_CONFIG;

// Initialization status.
static bool initialized = false;

// -----------------------------------------------------------------------------
// Public function definitions.

/**************************************************************************//**
 * Initializes the module.
 *****************************************************************************/
sl_status_t angle_queue_init(angle_queue_config_t *config)
{
  if (initialized) {
    return SL_STATUS_ALREADY_INITIALIZED;
  }

  angle_queue_config.locator_count = config->locator_count;
  angle_queue_config.max_sequence_diff = config->max_sequence_diff;
  angle_queue_config.sequence_ids = config->sequence_ids;
  angle_queue_config.on_angles_ready = config->on_angles_ready;

  if (2 > angle_queue_config.sequence_ids) {
    angle_queue_config.sequence_ids = 2;
  }

  // Validate configuration.
  if ((0 == angle_queue_config.locator_count)
      || (angle_queue_config.sequence_ids > angle_queue_config.max_sequence_diff)) {
    return SL_STATUS_INVALID_CONFIGURATION;
  }
  initialized = true;

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Pushes a new angle data to the queue.
 *****************************************************************************/
sl_status_t angle_queue_push(aoa_id_t tag_id,
                             aoa_id_t locator_id,
                             aoa_angle_t *angle)
{
  angle_queue_t *queue = NULL;

  if (!initialized) {
    return SL_STATUS_NOT_INITIALIZED;
  }

  if (SL_STATUS_OK != get_queue_by_id(tag_id, &queue)) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  remove_old_data(angle->sequence, queue);
  add_data(locator_id, angle, queue);
  check_data(tag_id, locator_id, queue);

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Deinitializes the queue.
 *****************************************************************************/
void angle_queue_deinit(void)
{
  angle_queue_t *current = NULL;
  angle_queue_t *next = NULL;

  for (current = head_queue; current != NULL; current = next) {
    next = current->next;
    for (int32_t i = 0; i < angle_queue_config.sequence_ids; i++) {
      free(current->data[i].locators);
      free(current->data[i].angles);
    }
    free(current->data);
    free(current);
  }
  head_queue = NULL;
  initialized = false;
}

// -----------------------------------------------------------------------------
// Private function definitions.

/**************************************************************************//**
 * Creates a new queue
 *****************************************************************************/
static sl_status_t new_queue(aoa_id_t tag_id,
                             angle_queue_t **queue)
{
  angle_queue_t *new = (angle_queue_t *)malloc(sizeof(angle_queue_t));

  if (NULL == new) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  new->data = (angle_data_t *)malloc(angle_queue_config.sequence_ids
                                     * sizeof(angle_data_t));

  if (NULL ==  new->data) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  aoa_id_copy(new->tag_id, tag_id);
  new->next = head_queue;
  head_queue = new;
  *queue = new;

  for (int32_t i = 0; i < angle_queue_config.sequence_ids; i++) {
    new->data[i].angles = (aoa_angle_t *)malloc(angle_queue_config.locator_count
                                                * sizeof(aoa_angle_t));

    new->data[i].locators = (aoa_id_t *)malloc(angle_queue_config.locator_count
                                               * sizeof(aoa_id_t));

    new->data[i].angle_count = 0;
    new->data[i].sequence = -1;

    if ((NULL == new->data[i].angles) || (NULL == new->data[i].locators)) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  return SL_STATUS_OK;
}

/**************************************************************************//**
 * Returns a queue by its id.
 *****************************************************************************/
static sl_status_t get_queue_by_id(aoa_id_t tag_id, angle_queue_t **queue)
{
  angle_queue_t *current = head_queue;
  while (NULL != current) {
    if (0 == aoa_id_compare(current->tag_id, tag_id)) {
      *queue = current;
      return SL_STATUS_OK;
    }
    current = current->next;
  }

  return new_queue(tag_id, queue);
}

/**************************************************************************//**
 * Adds a new angle data to the queue.
 *****************************************************************************/
static void add_data(aoa_id_t locator_id,
                     aoa_angle_t *angle,
                     angle_queue_t *queue)
{
  int32_t index = get_data_index(angle->sequence, queue);

  if (queue->data[index].sequence != angle->sequence ) {
    for (int32_t i = angle_queue_config.sequence_ids - 1; i > index; i--) {
      queue->data[i].sequence = queue->data[i - 1].sequence;
      queue->data[i].angle_count = queue->data[i - 1].angle_count;
      memcpy(queue->data[i].locators,
             queue->data[i - 1].locators,
             angle_queue_config.locator_count * sizeof(aoa_id_t));
      memcpy(queue->data[i].angles,
             queue->data[i - 1].angles,
             angle_queue_config.locator_count * sizeof(aoa_angle_t));
    }
    queue->data[index].sequence = angle->sequence;
    queue->data[index].angle_count = 0;
  }

  aoa_id_copy(queue->data[index].locators[queue->data[index].angle_count],
              locator_id);

  memcpy(&queue->data[index].angles[queue->data[index].angle_count],
         angle,
         sizeof(aoa_angle_t));

  queue->data[index].angle_count++;
}

/**************************************************************************//**
 * Returns an index which corresponds to a sequence group.
 *****************************************************************************/
static uint32_t get_data_index(int32_t sequence,
                               angle_queue_t *queue)
{
  for (uint32_t i = 0; i < angle_queue_config.sequence_ids; i++) {
    if (queue->data[i].sequence <= sequence) {
      return i;
    }
  }

  return 0;
}

/**************************************************************************//**
 * Checks if the data is ready.
 *****************************************************************************/
static void check_data(aoa_id_t tag_id,
                       aoa_id_t locator_id,
                       angle_queue_t *queue)
{
  int32_t cleanup_start = -1;

  // Starting from the oldest sequence id,
  // check if we have enough data.
  for (int32_t i = angle_queue_config.sequence_ids - 1; i >= 0; i--) {
    if (queue->data[i].angle_count >= get_expected_angles(i)) {
      if (NULL != angle_queue_config.on_angles_ready) {
        angle_queue_config.on_angles_ready(queue->tag_id,
                                           queue->data[i].angle_count,
                                           queue->data[i].angles,
                                           queue->data[i].locators);
      }
      cleanup_start = i;
    }
  }

  if (0 <= cleanup_start) {
    // Remove already processed and older data
    cleanup_data(cleanup_start, queue);
  }
}

/**************************************************************************//**
 * Removes old data from the queue.
 *****************************************************************************/
static void remove_old_data(int32_t sequence, angle_queue_t *queue)
{
  for (uint32_t i = 0; i < angle_queue_config.sequence_ids; i++) {
    if (aoa_sequence_compare(queue->data[i].sequence, sequence) > angle_queue_config.max_sequence_diff) {
      cleanup_data(i, queue);
      return;
    }
  }
}

/**************************************************************************//**
 * Calculates the expected angle number for a given sequence group.
 *****************************************************************************/
static uint32_t get_expected_angles(int32_t index)
{
  uint32_t coeff;

  // The expected number of angles depends on the index. A smaller
  // index (i.e. with more recent sequence number) expects more angles, while a
  // higher index (i.e. with older sequence number) requires less angles.
  // For the newest data we expect an angle from all locators,
  // while for the oldest needs only 2 locators.
  coeff = (angle_queue_config.locator_count < 2) ? 0 : (angle_queue_config.locator_count - 2);
  return (uint32_t)(angle_queue_config.locator_count
                    - ROUND_DIV(index * coeff, angle_queue_config.sequence_ids - 1));
}

/**************************************************************************//**
 * Cleanup the data in the queue from a start index.
 *****************************************************************************/
static void cleanup_data(int32_t index, angle_queue_t *queue)
{
  while (index < angle_queue_config.sequence_ids) {
    queue->data[index].angle_count = 0;
    queue->data[index].sequence = -1;
    index++;
  }
}
