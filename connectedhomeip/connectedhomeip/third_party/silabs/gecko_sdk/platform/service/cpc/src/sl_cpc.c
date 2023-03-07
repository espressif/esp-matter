/***************************************************************************/ /**
 * @file
 * @brief CPC API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_cpc.h"
#include "sl_sleeptimer.h"
#include "sli_cpc.h"
#include "sl_atomic.h"
#include "sl_status.h"
#include "sl_assert.h"
#include "em_core.h"
#include <stddef.h>
#include <stdbool.h>

#include "sli_cpc_hdlc.h"
#include "sli_cpc_drv.h"
#include "sli_cpc_crc.h"
#include "sli_cpc_debug.h"
#include "sl_cpc_config.h"
#include "sli_cpc_system_common.h"

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_cpc_kernel_config.h"
#endif

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/
#if defined(SL_CATALOG_KERNEL_PRESENT)
#define LOCK_ENDPOINTS_LIST()            if (osKernelGetState() == osKernelRunning) \
  { EFM_ASSERT(osMutexAcquire(endpoints_list_lock, osWaitForever) == osOK); }

#define RELEASE_ENDPOINTS_LIST()         if (osKernelGetState() == osKernelRunning) \
  { EFM_ASSERT(osMutexRelease(endpoints_list_lock) == osOK); }

#define LOCK_ENDPOINT(ep)                EFM_ASSERT(ep->state != SL_CPC_STATE_FREED); \
  if (osKernelGetState() == osKernelRunning)                                          \
  { EFM_ASSERT(osMutexAcquire(ep->lock, osWaitForever) == osOK); }

#define RELEASE_ENDPOINT(ep)             EFM_ASSERT(ep->state != SL_CPC_STATE_FREED); \
  if (osKernelGetState() == osKernelRunning)                                          \
  { EFM_ASSERT(osMutexRelease(ep->lock) == osOK); }
#else
#define LOCK_ENDPOINTS_LIST()
#define RELEASE_ENDPOINTS_LIST()
#define LOCK_ENDPOINT(ep)
#define RELEASE_ENDPOINT(ep)
#endif

#define ABS(a)  ((a) < 0 ? -(a) : (a))

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static sl_slist_node_t *endpoints;
static sl_slist_node_t *closed_endpoint_list;
static sl_slist_node_t *transmit_queue;
static sl_slist_node_t *pending_on_security_ready_queue;

static sl_cpc_dispatcher_handle_t dispatcher_handle;

#if defined(SL_CATALOG_KERNEL_PRESENT)
#define THREAD_STACK_SIZE   (SL_CPC_TASK_STACK_SIZE * sizeof(void *)) & 0xFFFFFFF8u

__ALIGNED(4) static uint8_t endpoints_list_mutex_cb[osMutexCbSize];
__ALIGNED(4) static uint8_t event_semaphore_cb[osSemaphoreCbSize];
__ALIGNED(4) static uint8_t thread_cb[osThreadCbSize];
__ALIGNED(8) static uint8_t thread_stack[THREAD_STACK_SIZE];

static osSemaphoreId_t event_signal;     ///< event signal
static osMutexId_t endpoints_list_lock;  ///< endpoints list lock
static osThreadId_t thread_id;
#else
static uint8_t rx_process_flag = 0;
#endif

sli_cpc_drv_capabilities_t sli_cpc_driver_capabilities;

#if ((SL_CPC_DEBUG_CORE_EVENT_COUNTERS == 1) \
  || (SL_CPC_DEBUG_MEMORY_ALLOCATOR_COUNTERS == 1))
sl_cpc_core_debug_t sl_cpc_core_debug;
#endif

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
__WEAK void sli_cpc_system_process(void);

#if defined(SL_CATALOG_KERNEL_PRESENT)
static void task(void *arg);
#endif

static sl_status_t open_endpoint(sl_cpc_endpoint_handle_t *endpoint,
                                 uint8_t id,
                                 uint8_t flags,
                                 uint8_t tx_window_size);

static sl_cpc_endpoint_t *find_endpoint(uint8_t id);

static bool sort_endpoints(sl_slist_node_t *item_l,
                           sl_slist_node_t *item_r);

static sl_status_t write(sl_cpc_endpoint_t *ep,
                         void* data,
                         uint16_t data_length,
                         uint8_t flags,
                         void *on_write_completed_arg);

static void decode_packet(void);

static void receive_ack(sl_cpc_endpoint_t *endpoint,
                        uint8_t ack);

static void receive_iframe(sl_cpc_endpoint_t *endpoint,
                           sl_cpc_buffer_handle_t *rx_handle,
                           uint8_t address,
                           uint8_t control,
                           uint8_t seq);

static void receive_sframe(sl_cpc_endpoint_t *endpoint,
                           sl_cpc_buffer_handle_t *rx_handle,
                           uint8_t control,
                           uint16_t data_length);

static void receive_uframe(sl_cpc_endpoint_t *endpoint,
                           sl_cpc_buffer_handle_t *rx_handle,
                           uint8_t control,
                           uint16_t data_length);

static sl_status_t transmit_ack(sl_cpc_endpoint_t *endpoint);

static sl_status_t re_transmit_frame(sl_cpc_endpoint_t *endpoint);

static void transmit_reject(sl_cpc_endpoint_t *endpoint,
                            uint8_t address,
                            uint8_t ack,
                            sl_cpc_reject_reason_t reason);

static void queue_for_transmission(sl_cpc_endpoint_t *ep,
                                   sl_cpc_transmit_queue_item_t *item,
                                   sl_cpc_buffer_handle_t *handle);

static sl_status_t process_tx_queue(void);

static void process_close(void);

static void defer_endpoint_free(sl_cpc_endpoint_t *ep);

static void process_deferred_on_write_completed(void *data);

static bool free_closed_endpoint_if_empty(sl_cpc_endpoint_t *ep);

static void clean_tx_queues(sl_cpc_endpoint_t *endpoint);

static void re_transmit_timeout_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

static void endpoint_close_timeout_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

static void notify_error(sl_cpc_endpoint_t *endpoint);

static bool is_seq_valid(uint8_t seq, uint8_t ack);

static bool cpc_enter_api(sl_cpc_endpoint_handle_t *endpoint_handle);

static void cpc_exit_api(sl_cpc_endpoint_handle_t *endpoint_handle);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize CPC module.
 ******************************************************************************/
sl_status_t sl_cpc_init(void)
{
#if defined(SL_CATALOG_KERNEL_PRESENT)
  osThreadAttr_t task_attr = { 0 };
  osMutexAttr_t mutex_attr = { 0 };
  osSemaphoreAttr_t semaphore_attr = { 0 };

  SLI_CPC_DEBUG_CORE_INIT();

  mutex_attr.attr_bits = osMutexRecursive;
  mutex_attr.name = "CPC Endpoints List Lock";
  mutex_attr.cb_mem = endpoints_list_mutex_cb;
  mutex_attr.cb_size = osMutexCbSize;
  endpoints_list_lock = osMutexNew(&mutex_attr);
  EFM_ASSERT(endpoints_list_lock != 0);

  semaphore_attr.cb_mem = event_semaphore_cb;
  semaphore_attr.cb_size = osSemaphoreCbSize;
  event_signal = osSemaphoreNew(EVENT_SIGNAL_MAX_COUNT, 0u, &semaphore_attr);
  EFM_ASSERT(event_signal != NULL);

  task_attr.name = "CPC Core";
  task_attr.priority = SL_CPC_TASK_PRIORITY;
  task_attr.stack_mem = &thread_stack[0];
  task_attr.stack_size = THREAD_STACK_SIZE;
  task_attr.cb_mem = &thread_cb[0];
  task_attr.cb_size = osThreadCbSize;

  thread_id = osThreadNew(&task, NULL, &task_attr);
  EFM_ASSERT(thread_id != NULL);
#endif

  sl_slist_init(&endpoints);
  sl_slist_init(&closed_endpoint_list);
  sl_slist_init(&transmit_queue);
  sl_slist_init(&pending_on_security_ready_queue);

  sli_cpc_init_buffers();

  sli_cpc_drv_get_capabilities(&sli_cpc_driver_capabilities);
  EFM_ASSERT(sli_cpc_drv_init() == SL_STATUS_OK);

#if !defined(SL_CATALOG_KERNEL_PRESENT)
  EFM_ASSERT(sli_cpc_system_init() == SL_STATUS_OK);
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  EFM_ASSERT(sli_cpc_security_init() == SL_STATUS_OK);
#endif
#endif

  sli_cpc_dispatcher_init_handle(&dispatcher_handle);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Open a user endpoint
 ******************************************************************************/
sl_status_t sl_cpc_open_user_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle,
                                      sl_cpc_user_endpoint_id_t id,
                                      uint8_t flags,
                                      uint8_t tx_window_size)
{
  EFM_ASSERT((uint8_t)id >= SL_CPC_USER_ENDPOINT_ID_START);
  EFM_ASSERT((uint8_t)id <= SL_CPC_USER_ENDPOINT_ID_END);
  return open_endpoint(endpoint_handle, (uint8_t)id, flags, tx_window_size);
}

/***************************************************************************//**
 * Open a service endpoint
 ******************************************************************************/
sl_status_t sli_cpc_open_service_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle,
                                          sl_cpc_service_endpoint_id_t id,
                                          uint8_t flags,
                                          uint8_t tx_window_size)
{
  EFM_ASSERT((uint8_t)id <= SLI_CPC_SERVICE_ENDPOINT_ID_END);
  return open_endpoint(endpoint_handle, (uint8_t)id, flags, tx_window_size);
}

/***************************************************************************//**
 * Open a temporary endpoint
 ******************************************************************************/
sl_status_t sli_cpc_open_temporary_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle,
                                            uint8_t *id,
                                            uint8_t flags,
                                            uint8_t tx_window_size)
{
  uint8_t id_free = 0;
  for (uint8_t i = SLI_CPC_TEMPORARY_ENDPOINT_ID_START; i <= SLI_CPC_TEMPORARY_ENDPOINT_ID_END; i++) {
    sl_cpc_endpoint_t *endpoint = find_endpoint(i);
    if (endpoint == NULL) {
      id_free = i;
      break;
    }
  }

  if (id_free == 0) {
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  EFM_ASSERT(id_free >= SLI_CPC_TEMPORARY_ENDPOINT_ID_START);
  EFM_ASSERT(id_free <= SLI_CPC_TEMPORARY_ENDPOINT_ID_END);
  *id = id_free;
  return open_endpoint(endpoint_handle, id_free, flags, tx_window_size);
}

/***************************************************************************//**
 * Set endpoint option
 ******************************************************************************/
sl_status_t sl_cpc_set_endpoint_option(sl_cpc_endpoint_handle_t *endpoint_handle,
                                       sl_cpc_endpoint_option_t option,
                                       void *value)
{
  sl_cpc_endpoint_t *ep;
  sl_status_t status = SL_STATUS_OK;

  if (endpoint_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (!cpc_enter_api(endpoint_handle)) {
    return SL_STATUS_INVALID_STATE;
  }

  sl_atomic_load(ep, (sl_cpc_endpoint_t *)endpoint_handle->ep);

  if (ep == NULL) {
    // Close has already been called, we are in the process of closing the endpoint
    ep = find_endpoint(endpoint_handle->id);
    if (ep == NULL) {
      status = SL_STATUS_INVALID_STATE;
    } else if (ep->state == SL_CPC_STATE_CLOSING) {
      status = SL_STATUS_INVALID_STATE;
    } else {
      status = SL_STATUS_OWNERSHIP;
    }
    cpc_exit_api(endpoint_handle);
    return status;
  }

  LOCK_ENDPOINT(ep);
  switch (option) {
    case SL_CPC_ENDPOINT_ON_IFRAME_RECEIVE:
      ep->on_iframe_data_reception = (sl_cpc_on_data_reception_t)value;
      break;
    case SL_CPC_ENDPOINT_ON_IFRAME_RECEIVE_ARG:
      ep->on_iframe_data_reception_arg = value;
      break;
    case SL_CPC_ENDPOINT_ON_UFRAME_RECEIVE:
      ep->on_uframe_data_reception = (sl_cpc_on_data_reception_t)value;
      break;
    case SL_CPC_ENDPOINT_ON_UFRAME_RECEIVE_ARG:
      ep->on_uframe_data_reception_arg = value;
      break;
    case SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED:
      ep->on_iframe_write_completed = (sl_cpc_on_write_completed_t)value;
      break;
    case SL_CPC_ENDPOINT_ON_UFRAME_WRITE_COMPLETED:
      ep->on_uframe_write_completed = (sl_cpc_on_write_completed_t)value;
      break;
    case SL_CPC_ENDPOINT_ON_FINAL:
#if defined(SL_CPC_ON_FINAL_PRESENT)
      ep->poll_final.on_final = (sl_cpc_on_final_t)value;
#else
      status = SL_STATUS_INVALID_PARAMETER;
#endif
      break;
    case SL_CPC_ENDPOINT_ON_POLL:
#if defined(SL_CPC_ON_POLL_PRESENT)
      ep->poll_final.on_poll = (sl_cpc_on_poll_t)value;
#else
      status = SL_STATUS_INVALID_PARAMETER;
#endif
      break;
    case SL_CPC_ENDPOINT_ON_POLL_ARG:
    case SL_CPC_ENDPOINT_ON_FINAL_ARG:
      ep->poll_final.on_fnct_arg = value;
      break;
    case SL_CPC_ENDPOINT_ON_ERROR:
      ep->on_error = (sl_cpc_on_error_callback_t)value;
      break;
    case SL_CPC_ENDPOINT_ON_ERROR_ARG:
      ep->on_error_arg = value;
      break;
    default:
      status = SL_STATUS_INVALID_PARAMETER;
      break;
  }

  RELEASE_ENDPOINT(ep);

  cpc_exit_api(endpoint_handle);
  return status;
}

/***************************************************************************//**
 * Close an endpoint
 ******************************************************************************/
sl_status_t sl_cpc_close_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle)
{
  sl_status_t status;
  sl_cpc_endpoint_t *ep;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (endpoint_handle->ref_count > 1) {
    CORE_EXIT_ATOMIC();
    return SL_STATUS_BUSY;
  }
  endpoint_handle->ref_count = 0u;

  sl_atomic_load(ep, (sl_cpc_endpoint_t *)endpoint_handle->ep);

  if (ep == NULL) {
    // Close has already been called, we are in the process of closing the endpoint
    ep = find_endpoint(endpoint_handle->id);
    if (ep == NULL) {
      status = SL_STATUS_OK;
    } else if (ep->state == SL_CPC_STATE_CLOSING || ep->state == SL_CPC_STATE_CLOSED) {
      status = SL_STATUS_OK;
    } else {
      status = SL_STATUS_OWNERSHIP;
    }

    CORE_EXIT_ATOMIC();
    return status;
  }

  CORE_EXIT_ATOMIC();
  LOCK_ENDPOINT(ep);

  // Notify the host that we want to close an endpoint
  if (endpoint_handle->id != SL_CPC_ENDPOINT_SYSTEM) {
    status = sli_cpc_send_disconnection_notification(ep->id);
    if (status != SL_STATUS_OK) {
      RELEASE_ENDPOINT(ep);
      CORE_EXIT_ATOMIC();
      return SL_STATUS_BUSY;
    }
  }

  while (ep->iframe_receive_queue != NULL) {
    // Drop the data from the receive Queue;
    //   Data reception is not allowed when the endpoint is in closing state
    //   Not possible to read anymore data once the endpoint is closed (or in closing state)
    sl_slist_node_t *node;
    sl_cpc_receive_queue_item_t *item;
    node = sl_slist_pop(&ep->iframe_receive_queue);
    item = SL_SLIST_ENTRY(node, sl_cpc_receive_queue_item_t, node);

    sli_cpc_drop_receive_queue_item(item);
  }

  while (ep->uframe_receive_queue != NULL) {
    // Drop the data from the receive Queue;
    //   Data reception is not allowed when the endpoint is in closing state
    //   Not possible to read anymore data once the endpoint is closed (or in closing state)
    sl_slist_node_t *node;
    sl_cpc_receive_queue_item_t *item;
    node = sl_slist_pop(&ep->uframe_receive_queue);
    item = SL_SLIST_ENTRY(node, sl_cpc_receive_queue_item_t, node);

    sli_cpc_drop_receive_queue_item(item);
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (ep->receive_signal != NULL) {
    EFM_ASSERT(osSemaphoreRelease(ep->receive_signal) == osOK);
  }
#endif

  switch (ep->state) {
    case SL_CPC_STATE_OPEN:
      break;
    case SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE:
    case SL_CPC_STATE_ERROR_SECURITY_INCIDENT:
    case SL_CPC_STATE_ERROR_FAULT:
      // Fatal error; must clean everything and free endpoint
      clean_tx_queues(ep);
      break;

    case SL_CPC_STATE_CLOSING:
    case SL_CPC_STATE_CLOSED:
    default:
      EFM_ASSERT(false);  // Should not reach this case
      return SL_STATUS_FAIL;
  }

  if ( endpoint_handle->id == SL_CPC_ENDPOINT_SYSTEM) {
    ep->state = SL_CPC_STATE_CLOSED;
    defer_endpoint_free(ep);
  } else {
    ep->state = SL_CPC_STATE_CLOSING;

    // We expect the host to close the endpoint in a reasonable time, start a timer
    status = sl_sleeptimer_restart_timer(&ep->close_timer,
                                         sl_sleeptimer_ms_to_tick(SLI_CPC_DISCONNECTION_NOTIFICATION_TIMEOUT_MS),
                                         endpoint_close_timeout_callback,
                                         ep,
                                         0u,
                                         0u);
    EFM_ASSERT(status == SL_STATUS_OK);
  }

  RELEASE_ENDPOINT(ep);

  // Set endpoint to null, so we cannot read and send data anymore or
  // closing the endpoint again
  endpoint_handle->ep = NULL;

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Abort read from an endpoint
 ******************************************************************************/
#if defined(SL_CATALOG_KERNEL_PRESENT)
sl_status_t sl_cpc_abort_read(sl_cpc_endpoint_handle_t *endpoint_handle)
{
  osSemaphoreId_t receive_signal;
  sl_cpc_endpoint_t *ep;

  if (endpoint_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (osKernelGetState() != osKernelRunning) {
    return SL_STATUS_INVALID_STATE;
  }

  if (!cpc_enter_api(endpoint_handle)) {
    return SL_STATUS_INVALID_STATE;
  }

  sl_atomic_load(ep, (sl_cpc_endpoint_t *)endpoint_handle->ep);

  if (ep == NULL) {
    cpc_exit_api(endpoint_handle);
    return SL_STATUS_NOT_INITIALIZED;
  }

  sl_atomic_load(receive_signal, ep->receive_signal);

  if (receive_signal != NULL) {
    sl_atomic_store(ep->read_aborted, true);
    cpc_exit_api(endpoint_handle);
    EFM_ASSERT(osSemaphoreRelease(receive_signal) == osOK);
    return SL_STATUS_OK;
  }

  cpc_exit_api(endpoint_handle);
  return SL_STATUS_NONE_WAITING;
}
#endif

/***************************************************************************//**
 * Read data from an endpoint
 ******************************************************************************/
sl_status_t sl_cpc_read(sl_cpc_endpoint_handle_t *endpoint_handle,
                        void **data,
                        uint16_t *data_length,
                        uint32_t timeout,
                        uint8_t flags)
{
  sl_slist_node_t **receive_queue = NULL;
  sl_cpc_endpoint_t *ep = NULL;
  sl_cpc_receive_queue_item_t *item = NULL;
  sl_slist_node_t *node = NULL;
  sl_status_t status = SL_STATUS_EMPTY;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  osStatus_t  sem_status = osOK;
  osSemaphoreId_t receive_signal;

  if (osKernelGetState() != osKernelRunning) {
    return SL_STATUS_INVALID_STATE;
  }
#endif

  if (endpoint_handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (!cpc_enter_api(endpoint_handle)) {
    return SL_STATUS_INVALID_STATE;
  }

  sl_atomic_load(ep, (sl_cpc_endpoint_t *)endpoint_handle->ep);
  if (ep == NULL) {
    ep = find_endpoint(endpoint_handle->id);
    if (ep != NULL) {
      status = SL_STATUS_OWNERSHIP;
    } else {
      status = SL_STATUS_INVALID_STATE;
    }
    cpc_exit_api(endpoint_handle);
    return status;
  }

  LOCK_ENDPOINT(ep);

  if (flags & SL_CPC_FLAG_UNNUMBERED_INFORMATION) {
    receive_queue = &ep->uframe_receive_queue;
  } else {
    receive_queue = &ep->iframe_receive_queue;
  }

  if (*receive_queue == NULL) {
    if (ep->state != SL_CPC_STATE_OPEN) {
      // Return error only when the receive queue is empty
      RELEASE_ENDPOINT(ep);
      cpc_exit_api(endpoint_handle);
      return SL_STATUS_INVALID_STATE;
    }

#if defined(SL_CATALOG_KERNEL_PRESENT)
    if ((flags & SL_CPC_FLAG_NO_BLOCK) > 0) {
      RELEASE_ENDPOINT(ep);
      cpc_exit_api(endpoint_handle);
      return status;
    } else {
      // Valid state, block until data is received
      osSemaphoreAttr_t semaphore_attr;
      uint32_t semaphore_cb[osSemaphoreCbSize / 4];

      if (ep->receive_signal != NULL) {
        // A task is already pending on this endpoint
        RELEASE_ENDPOINT(ep);
        cpc_exit_api(endpoint_handle);
        return SL_STATUS_IS_WAITING;
      }

      semaphore_attr.cb_mem = semaphore_cb;
      semaphore_attr.cb_size = osSemaphoreCbSize;
      ep->receive_signal = osSemaphoreNew(255, 0u, &semaphore_attr);
      EFM_ASSERT(ep->receive_signal != NULL);
      if (ep->receive_signal == NULL) {
        RELEASE_ENDPOINT(ep);
        cpc_exit_api(endpoint_handle);
        return SL_STATUS_ALLOCATION_FAILED;
      }

      RELEASE_ENDPOINT(ep);
      if (timeout == 0) {
        timeout = osWaitForever;
      }
      sem_status = osSemaphoreAcquire(ep->receive_signal, timeout);
      EFM_ASSERT(sem_status == osOK || sem_status == osErrorTimeout);
      LOCK_ENDPOINT(ep);

      receive_signal = ep->receive_signal;
      sl_atomic_store(ep->receive_signal, NULL);
      EFM_ASSERT(osSemaphoreDelete(receive_signal) == osOK);

      if (sem_status == osErrorTimeout) {
        RELEASE_ENDPOINT(ep);
        cpc_exit_api(endpoint_handle);
        return SL_STATUS_TIMEOUT;
      }

      if (ep->read_aborted) {
        ep->read_aborted = false;
        RELEASE_ENDPOINT(ep);
        cpc_exit_api(endpoint_handle);
        return SL_STATUS_ABORT;
      }
    }
#else
    (void)timeout;
    (void)flags;
    RELEASE_ENDPOINT(ep);
    cpc_exit_api(endpoint_handle);
    return status;
#endif
  }

  // Allow read even if the state is "error".
  // Error will be returned only when the queue is empty.
  node = sl_slist_pop(receive_queue);
  if (node != NULL) {
    item = SL_SLIST_ENTRY(node, sl_cpc_receive_queue_item_t, node);
    sli_cpc_free_receive_queue_item(item, data, data_length);
  } else {
    *data = NULL;
    *data_length = 0;
    RELEASE_ENDPOINT(ep);
    cpc_exit_api(endpoint_handle);
    return SL_STATUS_EMPTY;
  }

  RELEASE_ENDPOINT(ep);
  cpc_exit_api(endpoint_handle);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Write data from an endpoint
 ******************************************************************************/
sl_status_t sl_cpc_write(sl_cpc_endpoint_handle_t *endpoint_handle,
                         void* data,
                         uint16_t data_length,
                         uint8_t flags,
                         void *on_write_completed_arg)
{
  sl_cpc_endpoint_t *ep;
  sl_status_t status;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (osKernelGetState() != osKernelRunning) {
    return SL_STATUS_INVALID_STATE;
  }
#endif

  if (endpoint_handle == NULL || data == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (data_length == 0) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Payload must be 4087 or less
  EFM_ASSERT(data_length <= SL_CPC_APP_DATA_MAX_LENGTH);

  if (!cpc_enter_api(endpoint_handle)) {
    return SL_STATUS_INVALID_STATE;
  }
  sl_atomic_load(ep, (sl_cpc_endpoint_t *)endpoint_handle->ep);

  if (ep == NULL) {
    ep = find_endpoint(endpoint_handle->id);
    if (ep != NULL) {
      status = SL_STATUS_OWNERSHIP;
    } else {
      status = SL_STATUS_INVALID_STATE;
    }
    cpc_exit_api(endpoint_handle);
    return status;
  }

#if defined(SL_CATALOG_CPC_SECONDARY_PRESENT)
  // Secondary cannot send poll message
  // Can answer only using the on_poll callback
  EFM_ASSERT((flags & SL_CPC_FLAG_UNNUMBERED_POLL) == 0);
#endif
  status = write(ep, data, data_length, flags, on_write_completed_arg);
  cpc_exit_api(endpoint_handle);

  return status;
}

/***************************************************************************//**
 * Get endpoint status
 ******************************************************************************/
sl_cpc_endpoint_state_t sl_cpc_get_endpoint_state(sl_cpc_endpoint_handle_t *endpoint_handle)
{
  sl_cpc_endpoint_t *ep;
  sl_cpc_endpoint_state_t state;

  sl_atomic_load(ep, (sl_cpc_endpoint_t *)endpoint_handle->ep);

  if (ep == NULL) {
    ep = find_endpoint(endpoint_handle->id);
    if (ep == NULL) {
      state = SL_CPC_STATE_FREED;
    } else {
      sl_atomic_load(state, ep->state);
    }
  } else {
    sl_atomic_load(state, ep->state);
  }

  return state;
}

void sli_cpc_remote_disconnected(uint8_t endpoint_id)
{
  sl_cpc_endpoint_t *ep;

  ep = find_endpoint(endpoint_id);

  if (ep == NULL) {
    return; // Endpoint is not being used
  }

  if (ep->state == SL_CPC_STATE_CLOSING) {
    // Stop the close timer
    sl_sleeptimer_stop_timer(&ep->close_timer);
    ep->state = SL_CPC_STATE_CLOSED;
    defer_endpoint_free(ep);
    return;
  }

  // Check if endpoint is in error
  if (ep->state == SL_CPC_STATE_OPEN) {
    ep->state = SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE;

    // Stop re-transmit timeout
    sl_sleeptimer_stop_timer(&ep->re_transmit_timer);
    clean_tx_queues(ep);
    notify_error(ep);
  }
}

/***************************************************************************//**
 * Calculate the re transmit timeout
 * Implemented using Karn’s algorithm
 * Based off of RFC 2988 Computing TCP's Retransmission Timer
 ******************************************************************************/
static void sli_compute_re_transmit_timeout(sl_cpc_endpoint_t *endpoint)
{
  // Implemented using Karn’s algorithm
  // Based off of RFC 2988 Computing TCP's Retransmission Timer
  static bool first_rtt_measurement = true;

  uint64_t round_trip_time = 0;
  uint64_t rto = 0;
  int64_t delta = 0;

  const long k = 4; // This value is recommended by the Karn’s algorithm

  EFM_ASSERT(endpoint != NULL);

  round_trip_time = sl_sleeptimer_get_tick_count64() - endpoint->last_iframe_sent_timestamp;

  if (first_rtt_measurement) {
    endpoint->smoothed_rtt = round_trip_time;
    endpoint->rtt_variation = round_trip_time / 2;
    first_rtt_measurement = false;
  } else {
    // RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R'| where beta is 0.25
    delta = ABS((int64_t)endpoint->smoothed_rtt - (int64_t)round_trip_time);
    endpoint->rtt_variation = 3 * (endpoint->rtt_variation / 4) +  delta / 4;

    //SRTT <- (1 - alpha) * SRTT + alpha * R' where alpha is 0.125
    endpoint->smoothed_rtt = 7 * (endpoint->smoothed_rtt / 8) + round_trip_time / 8;
  }

  // Impose a lowerbound on the variation, we don't want the RTO to converge too close to the RTT
  if (endpoint->rtt_variation < sl_sleeptimer_ms_to_tick(SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MINIMUM_VARIATION_MS)) {
    endpoint->rtt_variation = sl_sleeptimer_ms_to_tick(SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MINIMUM_VARIATION_MS);
  }

  rto = endpoint->smoothed_rtt + k * endpoint->rtt_variation;

  if (rto > sl_sleeptimer_ms_to_tick(SLI_CPC_MAX_RE_TRANSMIT_TIMEOUT_MS)) {
    rto = sl_sleeptimer_ms_to_tick(SLI_CPC_MAX_RE_TRANSMIT_TIMEOUT_MS);
  } else if (rto < sl_sleeptimer_ms_to_tick(SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MS)) {
    rto = sl_sleeptimer_ms_to_tick(SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MS);
  }

  endpoint->re_transmit_timeout = rto;
}

/***************************************************************************//**
 * Signal processing is required
 ******************************************************************************/
void sli_cpc_signal_event(sl_cpc_signal_type_t signal_type)
{
  (void)signal_type;

#if defined(SL_CATALOG_KERNEL_PRESENT)
  EFM_ASSERT(osSemaphoreRelease(event_signal) == osOK);
#else
  if (signal_type == SL_CPC_SIGNAL_RX) {
    rx_process_flag++;
  }
#endif
}

/***************************************************************************//**
 * Notify Transmit completed
 ******************************************************************************/
void sli_cpc_drv_notify_tx_complete(sl_cpc_buffer_handle_t *buffer_handle)
{
  uint8_t control_byte;
  uint8_t frame_type;
  sl_status_t status = SL_STATUS_OK;
  sl_cpc_endpoint_t *endpoint;
  bool can_be_freed = false;
  CORE_DECLARE_IRQ_STATE;

#if defined(CPC_DEBUG_TRACE)
  if (buffer_handle->endpoint != NULL) {
    SLI_CPC_DEBUG_TRACE_ENDPOINT_FRAME_TRANSMIT_COMPLETED(buffer_handle->endpoint);
    SLI_CPC_DEBUG_TRACE_CORE_TXD_TRANSMIT_COMPLETED();
  }
#endif

  // Recover what type is the frame
  control_byte = sli_cpc_hdlc_get_control(buffer_handle->hdlc_header);
  frame_type = sli_cpc_hdlc_get_frame_type(control_byte);

  CORE_ENTER_ATOMIC();
  endpoint = buffer_handle->endpoint;

  if ((endpoint == NULL)                 // Endpoint already closed
      || ((frame_type != SLI_CPC_HDLC_FRAME_TYPE_DATA)
          && (frame_type != SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED))) {
    if ((frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA)
        || (frame_type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED)) {
      // Data associated with a closed endpoint,
      // but buffer was still referenced in the driver
      sl_cpc_endpoint_closed_arg_t *arg = buffer_handle->arg;

      EFM_ASSERT(arg != NULL);
      if ((frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA)
          && (arg->on_iframe_write_completed != NULL)) {
        // Notify caller that it can free the tx buffer now
        arg->on_iframe_write_completed(arg->id, buffer_handle->data, arg->arg, SL_STATUS_TRANSMIT_INCOMPLETE);
      } else if ((frame_type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED)
                 && (arg->on_uframe_write_completed != NULL)) {
        arg->on_uframe_write_completed(arg->id, buffer_handle->data, arg->arg, SL_STATUS_TRANSMIT_INCOMPLETE);
      }
      buffer_handle->data = NULL;
      sli_cpc_free_closed_arg(arg);
    }
    // Free handle and data buffer
    sli_cpc_drop_buffer_handle(buffer_handle);
  } else {
    if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA) {
      if (buffer_handle->on_write_complete_pending) {
        // Push to the dispatcher queue in order to call on_write_completed outside of IRQ context
        sli_cpc_dispatcher_push(&dispatcher_handle, process_deferred_on_write_completed, buffer_handle);
        buffer_handle->on_write_complete_pending = false;
      }

      // Drop the buffer restart re_transmit_timer if it was not acknowledged (still referenced)
      if (sli_cpc_drop_buffer_handle(buffer_handle) == SL_STATUS_BUSY) {
        SLI_CPC_DEBUG_TRACE_ENDPOINT_DATA_FRAME_TRANSMIT_COMPLETED(endpoint);
        status = sl_sleeptimer_restart_timer(&endpoint->re_transmit_timer,
                                             endpoint->re_transmit_timeout,
                                             re_transmit_timeout_callback,
                                             buffer_handle,
                                             0u,
                                             0u);
        EFM_ASSERT(status == SL_STATUS_OK);
      }
    } else if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED) {
      if (endpoint->on_uframe_write_completed != NULL) {
        // Notify caller that it can free the tx buffer now
        endpoint->on_uframe_write_completed(endpoint->id, buffer_handle->data, buffer_handle->arg, SL_STATUS_OK);
      }

      // Free handle
      buffer_handle->data = NULL;
      sli_cpc_drop_buffer_handle(buffer_handle);

      // Increase tx window
      endpoint->current_tx_window_space++;

      if (endpoint->holding_list != NULL) {
        // Put data frames hold in the endpoint in the tx queue if space in transmit window

        while (endpoint->holding_list != NULL
               && endpoint->current_tx_window_space > 0) {
          sl_cpc_transmit_queue_item_t *qitem;
          uint8_t type;

          sl_slist_node_t *item_node = SLI_CPC_POP_BUFFER_HANDLE_LIST(&endpoint->holding_list, sl_cpc_transmit_queue_item_t);
          qitem = SL_SLIST_ENTRY(item_node, sl_cpc_transmit_queue_item_t, node);
          type = sli_cpc_hdlc_get_frame_type(qitem->handle->control);
          if ((type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED)
              || (endpoint->frames_count_re_transmit_queue == 0)) {
            // If next frame in holding list is an unnumbered or if no frame to retx
            // Transmit uframe or iframe
            sli_cpc_push_back_buffer_handle(&transmit_queue, item_node, qitem->handle);
            endpoint->current_tx_window_space--;
          } else {
            // if next is iframe; wait ACK before transmitting
            sli_cpc_push_buffer_handle(&endpoint->holding_list, item_node, qitem->handle);
            break;
          }
        }
      } else if (endpoint->state == SL_CPC_STATE_CLOSED) {
        can_be_freed = true;
      }
    } else {
      SLI_CPC_DEBUG_TRACE_ENDPOINT_SUPERVISORY_FRAME_TRANSMIT_COMPLETED(endpoint);
    }
  }

  // Notify task if transmit frames are still queued in endpoints
  if (transmit_queue != NULL) {
    EFM_ASSERT(can_be_freed == false);
    sli_cpc_signal_event(SL_CPC_SIGNAL_TX);
  }

  if (can_be_freed) {
    defer_endpoint_free(endpoint);
  }
  CORE_EXIT_ATOMIC();
}

/***************************************************************************//**
 * Notify Packet has been received and it is ready to be processed
 ******************************************************************************/
void sli_cpc_drv_notify_rx_data(void)
{
  sli_cpc_signal_event(SL_CPC_SIGNAL_RX);
}

/***************************************************************************/ /**
 * Determines if CPC is ok to enter sleep mode.
 ******************************************************************************/
#if !defined(SL_CATALOG_KERNEL_PRESENT) && defined(SL_CATALOG_POWER_MANAGER_PRESENT)
bool sl_cpc_is_ok_to_sleep(void)
{
  return (rx_process_flag == 0 && (transmit_queue == NULL || !sli_cpc_drv_is_transmit_ready()));
}
#endif

/***************************************************************************//**
 * Determines if CPC is ok to return to sleep mode on ISR exit.
 ******************************************************************************/
#if !defined(SL_CATALOG_KERNEL_PRESENT) && defined(SL_CATALOG_POWER_MANAGER_PRESENT)
sl_power_manager_on_isr_exit_t sl_cpc_sleep_on_isr_exit(void)
{
  return (sl_cpc_is_ok_to_sleep() ? SL_POWER_MANAGER_IGNORE : SL_POWER_MANAGER_WAKEUP);
}
#endif

/***************************************************************************//**
 * CPC Task
 ******************************************************************************/
#if defined(SL_CATALOG_KERNEL_PRESENT)
static void task(void *arg)
{
  EFM_ASSERT(sli_cpc_system_init() == SL_STATUS_OK);
#if (defined(SL_CATALOG_CPC_SECURITY_PRESENT))
  EFM_ASSERT(sli_cpc_security_init() == SL_STATUS_OK);
#endif
  (void)arg;
  while (1) {
    // Wait for signal indicating frame to receive or frame to transmit
    EFM_ASSERT(osSemaphoreAcquire(event_signal, osWaitForever) == osOK);

    // Check for frame to receive and demux it
    decode_packet();

    sli_cpc_system_process();

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
    sli_cpc_security_process();
#endif

    // Check for frame to transmit
    process_tx_queue();

    process_close();

    sli_cpc_dispatcher_process();
  }
}
#endif

/***************************************************************************//**
 * Tick step function
 ******************************************************************************/
#if !defined(SL_CATALOG_KERNEL_PRESENT)
void sl_cpc_process_action(void)
{
  uint8_t flag;
  uint8_t processed;
  CORE_DECLARE_IRQ_STATE;

  sl_atomic_load(flag, rx_process_flag);
  processed = flag;

  // Reception frame process action
  while (flag > 0) {
    decode_packet();
    flag--;
  }

  CORE_ENTER_ATOMIC();
  rx_process_flag -= processed;  // Do not process packet received during processing
  CORE_EXIT_ATOMIC();

  sli_cpc_system_process();
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  sli_cpc_security_process();
#endif

  // Transmit frame process action
  while (transmit_queue != NULL || pending_on_security_ready_queue != NULL) {
    sl_status_t status;

    status = process_tx_queue();
    if (status != SL_STATUS_OK) {
      break;
    }
  }

  process_close();

  sli_cpc_dispatcher_process();
}
#endif

/***************************************************************************//**
 * Open a specialized endpoint
 ******************************************************************************/
static sl_status_t open_endpoint(sl_cpc_endpoint_handle_t *endpoint_handle,
                                 uint8_t id,
                                 uint8_t flags,
                                 uint8_t tx_window_size)
{
  sl_cpc_endpoint_t *ep;
  sl_status_t status;

  EFM_ASSERT(tx_window_size >= SL_CPC_TRANSMIT_WINDOW_MIN_SIZE);
  EFM_ASSERT(tx_window_size <= SL_CPC_TRANSMIT_WINDOW_MAX_SIZE);

  ep = find_endpoint(id);

  if (ep != NULL) {
    if (ep->state == SL_CPC_STATE_OPEN) {
      return SL_STATUS_ALREADY_EXISTS;
    }
    return SL_STATUS_BUSY; // Can't open right away
  }

  status = sli_cpc_get_endpoint(&ep);
  if (status != SL_STATUS_OK) {
    return status;
  }

  SLI_CPC_DEBUG_ENDPOINT_INIT(ep);
  ep->flags = flags;
  ep->id = id;
  ep->seq = 0;
  ep->ack = 0;
  ep->configured_tx_window_size = tx_window_size;
  ep->current_tx_window_space = ep->configured_tx_window_size;
  ep->frames_count_re_transmit_queue = 0;
  ep->state = SL_CPC_STATE_OPEN;
  ep->node.node = NULL;
  ep->re_transmit_timeout = sl_sleeptimer_ms_to_tick(SLI_CPC_MIN_RE_TRANSMIT_TIMEOUT_MS);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  osMutexAttr_t mutex_attr;
  mutex_attr.attr_bits = osMutexRecursive;
  mutex_attr.name = "CPC Endpoint Lock";
  mutex_attr.cb_mem = ep->lock_cb;
  mutex_attr.cb_size = osMutexCbSize;
  ep->lock = osMutexNew(&mutex_attr);
  EFM_ASSERT(ep->lock != NULL);
#endif

  sl_slist_init(&ep->iframe_receive_queue);
  sl_slist_init(&ep->uframe_receive_queue);
  sl_slist_init(&ep->re_transmit_queue);
  sl_slist_init(&ep->holding_list);

  LOCK_ENDPOINTS_LIST();
  sl_slist_push(&endpoints, &ep->node);
  sl_slist_sort(&endpoints, sort_endpoints);
  RELEASE_ENDPOINTS_LIST();

#if defined(SL_CATALOG_KERNEL_PRESENT)
  ep->read_aborted = false;
  ep->receive_signal = NULL;
#endif

  endpoint_handle->id = id;
  endpoint_handle->ref_count = 1u;
  endpoint_handle->ep = ep;

  SLI_CPC_DEBUG_TRACE_CORE_OPEN_ENDPOINT();
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Check if endpoint ID is already in use
 ******************************************************************************/
static sl_cpc_endpoint_t *find_endpoint(uint8_t id)
{
  sl_cpc_endpoint_t *endpoint;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  SL_SLIST_FOR_EACH_ENTRY(endpoints, endpoint, sl_cpc_endpoint_t, node) {
    if (endpoint->id == id) {
      EFM_ASSERT(endpoint->state != SL_CPC_STATE_FREED); // Should not be in the endpoint list if freed
      CORE_EXIT_ATOMIC();
      return endpoint;
    }
  }

  CORE_EXIT_ATOMIC();
  return NULL;
}

/***************************************************************************//**
 * Endpoint list sorting function
 ******************************************************************************/
static bool sort_endpoints(sl_slist_node_t *item_l,
                           sl_slist_node_t *item_r)
{
  sl_cpc_endpoint_t *ep_left = SL_SLIST_ENTRY(item_l, sl_cpc_endpoint_t, node);
  sl_cpc_endpoint_t *ep_right = SL_SLIST_ENTRY(item_r, sl_cpc_endpoint_t, node);

  if (ep_left->id < ep_right->id) {
    return true;
  } else {
    return false;
  }
}

/***************************************************************************//**
 * Queue a buffer_handle for transmission. This function will either put the
 * buffer in the transmit queue if there is enough room in the endpoint's TX
 * window, or in the endpoint holding list otherwise.
 * This function doesn't lock the endpoint, it's caller's responsibility.
 ******************************************************************************/
static void queue_for_transmission(sl_cpc_endpoint_t *ep,
                                   sl_cpc_transmit_queue_item_t *item,
                                   sl_cpc_buffer_handle_t *handle)
{
  // Check if there is still place in the transmit window
  if (ep->current_tx_window_space > 0) {
    ep->current_tx_window_space--;

    // Put frame in Tx Q so that it can be transmitted by CPC Core later
    CORE_ATOMIC_SECTION(sli_cpc_push_back_buffer_handle(&transmit_queue, &item->node, handle); );

    // Signal task/process_action that frame is in Tx Queue
    sli_cpc_signal_event(SL_CPC_SIGNAL_TX);
  } else {
    // Put frame in endpoint holding list to wait for more space in the transmit window
    sli_cpc_push_back_buffer_handle(&ep->holding_list, &item->node, handle);
  }
}

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
sl_status_t endpoint_secure_tx_is_allowed(sl_cpc_endpoint_t *ep, bool *encrypt)
{
  sl_cpc_security_state_t security_state = sl_cpc_security_get_state();
  uint8_t endpoint_id = ep->id;

  if (security_state == SL_CPC_SECURITY_STATE_NOT_READY) {
    /*
     * The security subsystem is not ready, communication is only allowed
     * on the system endpoint, unencrypted for now.
     */
    if (endpoint_id != SL_CPC_ENDPOINT_SYSTEM) {
      return SL_STATUS_NOT_READY;
    } else {
      *encrypt = false;
      return SL_STATUS_OK;
    }
  } else if (security_state == SL_CPC_SECURITY_STATE_DISABLED) {
    /*
     * Security is not used, plaintext I-Frame
     */
    *encrypt = false;
    return SL_STATUS_OK;
  } else if (security_state == SL_CPC_SECURITY_STATE_INITIALIZING
             || security_state == SL_CPC_SECURITY_STATE_WAITING_ON_TX_COMPLETE) {
    /*
     * If the security is being initialized, I-Frames are only allowed
     * on the security endpoint and they are not encrypted at this point.
     */
    if (endpoint_id != SL_CPC_ENDPOINT_SECURITY
        && endpoint_id != SL_CPC_ENDPOINT_SYSTEM) {
      return SL_STATUS_PERMISSION;
    }

    *encrypt = false;
    return SL_STATUS_OK;
  } else if (security_state == SL_CPC_SECURITY_STATE_RESETTING) {
    if (endpoint_id != SL_CPC_ENDPOINT_SECURITY) {
      return SL_STATUS_PERMISSION;
    }

    *encrypt = false;
    return SL_STATUS_OK;
  } else if (security_state == SL_CPC_SECURITY_STATE_INITIALIZED) {
    /*
     * If initialized, encrypt all I-Frames.
     */
    *encrypt = true;
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FAIL;
  }
}

static sl_status_t should_encrypt_frame(sl_cpc_buffer_handle_t *frame, bool *encrypt)
{
  uint16_t data_length = frame->data_length;
  uint8_t control = frame->control;
  bool type = sli_cpc_hdlc_get_frame_type(control);

  if (type != SLI_CPC_HDLC_FRAME_TYPE_DATA) {
    *encrypt = false;
    return SL_STATUS_OK;
  }

  if (data_length == 0) {
    *encrypt = false;
    return SL_STATUS_OK;
  }

  /* security buffer already exists, frame already encrypted */
  if (frame->security_tag != NULL) {
    *encrypt = false;
    return SL_STATUS_OK;
  }

  return endpoint_secure_tx_is_allowed(frame->endpoint, encrypt);
}

/***************************************************************************//**
 * Return true if security is ready to process packets on regular endpoints.
 ******************************************************************************/
static bool security_is_ready(void)
{
  sl_cpc_security_state_t security_state = sl_cpc_security_get_state();

  return security_state == SL_CPC_SECURITY_STATE_INITIALIZED
         || security_state == SL_CPC_SECURITY_STATE_DISABLED;
}
#endif

/***************************************************************************//**
 * Write data from an endpoint
 ******************************************************************************/
static sl_status_t write(sl_cpc_endpoint_t *ep,
                         void* data,
                         uint16_t data_length,
                         uint8_t flags,
                         void *on_write_completed_arg)
{
  sl_cpc_buffer_handle_t *frame_handle;
  sl_cpc_transmit_queue_item_t *item;
  sl_status_t status;
  bool is_final = flags & SL_CPC_FLAG_INFORMATION_FINAL;
  bool iframe = true;
  uint16_t fcs;
  uint8_t type = 0;

  LOCK_ENDPOINT(ep);

  if ((flags & SL_CPC_FLAG_UNNUMBERED_INFORMATION)
      || (flags & SL_CPC_FLAG_UNNUMBERED_ACKNOWLEDGE)
      || (flags & SL_CPC_FLAG_UNNUMBERED_POLL)) {
    if (!(ep->flags & SL_CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE)) {
      RELEASE_ENDPOINT(ep);
      return SL_STATUS_INVALID_PARAMETER;
    }

    iframe = false;

    if (flags & SL_CPC_FLAG_UNNUMBERED_INFORMATION) {
      type = SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_INFORMATION;
    } else if ((flags & SL_CPC_FLAG_UNNUMBERED_POLL)) {
      type = SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_POLL_FINAL;
    } else if ((flags & SL_CPC_FLAG_UNNUMBERED_ACKNOWLEDGE)) {
      type = SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_ACKNOWLEDGE;
    }
  } else if (ep->flags & SL_CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE) {
    RELEASE_ENDPOINT(ep);
    return SL_STATUS_INVALID_PARAMETER;
  }

  if (ep->state != SL_CPC_STATE_OPEN) {
    RELEASE_ENDPOINT(ep);
    return SL_STATUS_INVALID_STATE;
  }

  // Get item to queue frame
  status = sli_cpc_get_transmit_queue_item(&item);
  if (status != SL_STATUS_OK) {
    RELEASE_ENDPOINT(ep);
    return status;
  }

  // Get new frame handler
  status = sli_cpc_get_buffer_handle(&frame_handle);
  if (status != SL_STATUS_OK) {
    EFM_ASSERT(sli_cpc_free_transmit_queue_item(item) == SL_STATUS_OK);
    RELEASE_ENDPOINT(ep);
    return status;
  }

  // Link the data buffer inside the frame buffer
  frame_handle->data = data;
  frame_handle->data_length = data_length;
  frame_handle->endpoint = ep;
  frame_handle->address = ep->id;
  frame_handle->arg = on_write_completed_arg;

  // Add buffer to tx_queue_item
  item->handle = frame_handle;

  if (iframe) {
    // Set the SEQ number and ACK number in the control byte
    frame_handle->control = sli_cpc_hdlc_create_control_data(ep->seq, ep->ack, is_final);
    // Update endpoint sequence number
    ep->seq++;
    ep->seq %= 8;
  } else {
    frame_handle->control = sli_cpc_hdlc_create_control_unumbered(type);
  }

  // Compute payload CRC
  fcs = sli_cpc_get_crc_sw(data, data_length);
  frame_handle->fcs[0] = (uint8_t)fcs;
  frame_handle->fcs[1] = (uint8_t)(fcs >> 8);

  queue_for_transmission(ep, item, frame_handle);

  RELEASE_ENDPOINT(ep);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * De-multiplex receive frame and put it in right endpoint queue.
 ******************************************************************************/
static void decode_packet(void)
{
  uint8_t address;
  uint8_t type;
  uint8_t control;
  uint8_t seq;
  uint8_t ack;
  uint16_t hcs;
  uint16_t data_length;
  sl_cpc_buffer_handle_t *rx_handle;
  sl_cpc_endpoint_t *endpoint;
  uint16_t rx_buffer_payload_len;

  if (sli_cpc_drv_read_data(&rx_handle, &rx_buffer_payload_len) != SL_STATUS_OK) {
    SLI_CPC_DEBUG_TRACE_CORE_DRIVER_READ_ERROR();
    return;
  }

  SLI_CPC_DEBUG_TRACE_CORE_RXD_FRAME();

  if (!sli_cpc_driver_capabilities.preprocess_hdlc_header) {
    // Validate header checksum. In case it is invalid, drop the packet.
    hcs = sli_cpc_hdlc_get_hcs(rx_handle->hdlc_header);
    if (!sli_cpc_validate_crc_sw(rx_handle->hdlc_header, SLI_CPC_HDLC_HEADER_SIZE, hcs)) {
      // If HCS is invalid, we drop the packet as we cannot NAK it
      sli_cpc_drop_buffer_handle(rx_handle);
      SLI_CPC_DEBUG_TRACE_CORE_INVALID_HEADER_CHECKSUM();
      return;
    }
  }

  address = sli_cpc_hdlc_get_address(rx_handle->hdlc_header);
  control = sli_cpc_hdlc_get_control(rx_handle->hdlc_header);
  data_length = sli_cpc_hdlc_get_length(rx_handle->hdlc_header);
  seq = sli_cpc_hdlc_get_seq(control);
  ack = sli_cpc_hdlc_get_ack(control);
  type = sli_cpc_hdlc_get_frame_type(control);

  if (data_length < rx_buffer_payload_len) {
    // If driver return worst case; set true data size
    rx_handle->data_length = data_length;
  } else {
    rx_handle->data_length = rx_buffer_payload_len;
  }

  endpoint = find_endpoint(address);

  if (endpoint != NULL) {
    SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_FRAME(endpoint)
    LOCK_ENDPOINT(endpoint);

    if ((type == SLI_CPC_HDLC_FRAME_TYPE_DATA
         || type == SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY)
        && rx_handle->reason == SL_CPC_REJECT_NO_ERROR) {
      // Clean Tx queue
      receive_ack(endpoint, ack);
    }

    if (rx_handle->reason != SL_CPC_REJECT_NO_ERROR) {
      transmit_reject(endpoint, address, endpoint->ack, rx_handle->reason);
      rx_handle->reason = SL_CPC_REJECT_NO_ERROR;
      sli_cpc_drop_buffer_handle(rx_handle);
      SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_SUPERVISORY_DROPPED(endpoint);
    } else if (type == SLI_CPC_HDLC_FRAME_TYPE_DATA) {
      SLI_CPC_DEBUG_TRACE_CORE_RXD_VALID_IFRAME();
      receive_iframe(endpoint, rx_handle, address, control, seq);
    } else if (type == SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY) {
      receive_sframe(endpoint, rx_handle, control, data_length);
      SLI_CPC_DEBUG_TRACE_CORE_RXD_VALID_SFRAME();
    } else if (type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED) {
      receive_uframe(endpoint, rx_handle, control, data_length);
      SLI_CPC_DEBUG_TRACE_CORE_RXD_VALID_UFRAME();
    } else {
      transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_ERROR);
      sli_cpc_drop_buffer_handle(rx_handle);
      SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_SUPERVISORY_DROPPED(endpoint);
    }

    if (endpoint->state == SL_CPC_STATE_CLOSED) {
      if (!free_closed_endpoint_if_empty(endpoint)) {
        RELEASE_ENDPOINT(endpoint);
      }
    } else {
      RELEASE_ENDPOINT(endpoint);
    }
  } else {
    if (type != SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY) {
      transmit_reject(NULL, address, 0, SL_CPC_REJECT_UNREACHABLE_ENDPOINT);
    }
    sli_cpc_drop_buffer_handle(rx_handle);
  }
}

/***************************************************************************//**
 * Process received ACK frame
 ******************************************************************************/
static void receive_ack(sl_cpc_endpoint_t * endpoint,
                        uint8_t ack)
{
  sl_cpc_transmit_queue_item_t *item;
  sl_slist_node_t *item_node;
  sl_cpc_buffer_handle_t *frame;
  uint8_t control_byte;
  uint8_t seq_number;
  uint8_t ack_range_min;
  uint8_t ack_range_max;
  uint8_t frames_count_ack = 0;
  CORE_DECLARE_IRQ_STATE;

  // Protect the re_transmit_queue from being popped in a re_transmit timeout
  CORE_ENTER_ATOMIC();

  // Return if no frame to acknowledge
  if (endpoint->re_transmit_queue == NULL) {
    CORE_EXIT_ATOMIC();
    return;
  }

  // Get the sequence number of the first frame in the re-transmission queue
  item = SL_SLIST_ENTRY(endpoint->re_transmit_queue, sl_cpc_transmit_queue_item_t, node);
  frame = item->handle;

  control_byte = sli_cpc_hdlc_get_control(frame->hdlc_header);
  seq_number = sli_cpc_hdlc_get_seq(control_byte);

  // Calculate the acceptable ACK number range
  ack_range_min = seq_number + 1;
  ack_range_min %= 8;
  ack_range_max = seq_number + endpoint->frames_count_re_transmit_queue;
  ack_range_max %= 8;

  // Check that received ACK number is in range
  if (ack_range_max >= ack_range_min) {
    if (ack < ack_range_min
        || ack > ack_range_max) {
      // Invalid ack number
      CORE_EXIT_ATOMIC();
      return;
    }
  } else {
    if (ack > ack_range_max
        && ack < ack_range_min) {
      // Invalid ack number
      CORE_EXIT_ATOMIC();
      return;
    }
  }

  // Find number of frames acknowledged with ACK number
  if (ack > seq_number) {
    frames_count_ack = ack - seq_number;
  } else {
    frames_count_ack = 8 - seq_number;
    frames_count_ack += ack;
  }

  // Reset re-transmit counter
  endpoint->packet_re_transmit_count = 0u;

  // Calculate re_transmit_timeout
  sli_compute_re_transmit_timeout(endpoint);

  // Stop incomming re-transmit timeout
  (void)sl_sleeptimer_stop_timer(&endpoint->re_transmit_timer);

  // Remove all acknowledged frames in re-transmit queue
  for (uint8_t i = 0; i < frames_count_ack; i++) {
    item_node = SLI_CPC_POP_BUFFER_HANDLE_LIST(&endpoint->re_transmit_queue, sl_cpc_transmit_queue_item_t);
    EFM_ASSERT(item_node != NULL);

    item = SL_SLIST_ENTRY(item_node, sl_cpc_transmit_queue_item_t, node);
    frame = item->handle;

    if (frame->ref_count == 0) {
      // Only iframe can be acked
      if (endpoint->on_iframe_write_completed != NULL) {
        endpoint->on_iframe_write_completed(endpoint->id, frame->data, frame->arg, SL_STATUS_OK);
      }

      // Free the header buffer, the buffer handle and queue item
      frame->data = NULL;
      sli_cpc_drop_buffer_handle(frame);
      frame = NULL;
    } else {
      frame->on_write_complete_pending = true;
    }

    sli_cpc_free_transmit_queue_item(item);

    // Update transmit window
    endpoint->current_tx_window_space++;

    // Update number of frames in re-transmit queue
    endpoint->frames_count_re_transmit_queue--;
  }

  CORE_EXIT_ATOMIC();

  // Put data frames hold in the endpoint in the tx queue if space in transmit window
  while (endpoint->holding_list != NULL
         && endpoint->current_tx_window_space > 0) {
    CORE_ENTER_ATOMIC();
    item_node = SLI_CPC_POP_BUFFER_HANDLE_LIST(&endpoint->holding_list, sl_cpc_transmit_queue_item_t);
    item = SL_SLIST_ENTRY(item_node, sl_cpc_transmit_queue_item_t, node);
    frame = item->handle;
    sli_cpc_push_back_buffer_handle(&transmit_queue, item_node, frame);
    endpoint->current_tx_window_space--;
    CORE_EXIT_ATOMIC();
  }

  SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_ACK(endpoint);
}

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
/***************************************************************************//**
 * Decrypt a buffer
 *
 * This function just wraps the call to the security API.
 ******************************************************************************/
static sl_status_t decrypt_buffer_handle(sl_cpc_buffer_handle_t *rx_handle,
                                         uint16_t *payload_len)
{
  size_t output_len;
  sl_status_t ret;

  ret = sli_cpc_security_decrypt(rx_handle->hdlc_header, SLI_CPC_HDLC_HEADER_RAW_SIZE,
                                 rx_handle->data, *payload_len, *payload_len, &output_len);
  if (ret == SL_STATUS_OK) {
    EFM_ASSERT(output_len < UINT16_MAX);
    *payload_len = output_len;
  }

  return ret;
}

/***************************************************************************//**
 * Decrypt an I-Frame.
 *
 * Takes into consideration the security setup to handle buffer accordingly:
 *  - if security is not used, this does nothing
 *  - if security is being setup (messages are being exchanged between primary
 *    and secondary), only allow messages on the security endpoint. All other
 *    endpoints cannot send I-frames.
 *  - if security is setup, assume payload is encrypted.
 ******************************************************************************/
static sl_status_t decrypt_iframe(sl_cpc_endpoint_t *endpoint,
                                  sl_cpc_buffer_handle_t *rx_handle,
                                  uint16_t *payload_len)
{
  sl_cpc_security_state_t security_state = sl_cpc_security_get_state();

  if (security_state == SL_CPC_SECURITY_STATE_NOT_READY) {
    /*
     * Security is not initialized yet. At this point we don't know if the
     * payload is encrypted or if plaintext payload on non-security endpoints
     * are allowed. Reject.
     */
    return SL_STATUS_NOT_READY;
  } else if (security_state == SL_CPC_SECURITY_STATE_INITIALIZING) {
    /*
     * During initialization, I-frames are only allowed on the security
     * endpoint. The content is still cleartext, so there is no need
     * for decryption just yet.
     */
    if (endpoint->id != SL_CPC_ENDPOINT_SECURITY
        && endpoint->id != SL_CPC_ENDPOINT_SYSTEM) {
      return SL_STATUS_PERMISSION;
    } else {
      return SL_STATUS_OK;
    }
  } else if (security_state == SL_CPC_SECURITY_STATE_DISABLED) {
    /* nothing to do here, just for completeness */
    return SL_STATUS_OK;
  } else if (security_state == SL_CPC_SECURITY_STATE_RESETTING
             || security_state == SL_CPC_SECURITY_STATE_WAITING_ON_TX_COMPLETE) {
    /*
     * During a reset, we might still receive frames encrypted with the previous
     * session key. In that case, decrypt them. If something is received on the
     * security endpoint, it should not be decrypted.
     */
    if (endpoint->id == SL_CPC_ENDPOINT_SECURITY) {
      return SL_STATUS_OK;
    }
  } else if (security_state != SL_CPC_SECURITY_STATE_INITIALIZED) {
    /* we should not be here */
    EFM_ASSERT(0);
  }

  return decrypt_buffer_handle(rx_handle, payload_len);
}
#endif

/***************************************************************************//**
 * Process received iframe
 ******************************************************************************/
static void receive_iframe(sl_cpc_endpoint_t *endpoint,
                           sl_cpc_buffer_handle_t *rx_handle,
                           uint8_t address,
                           uint8_t control,
                           uint8_t seq)
{
  sl_status_t status;
  uint16_t rx_buffer_payload_len;
  uint16_t fcs;
  uint32_t reply_data_length = 0;
  void *reply_data = NULL;
  void *on_write_completed_arg = NULL;

  SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_DATA_FRAME(endpoint);
  if (endpoint->state == SL_CPC_STATE_CLOSING) {
    // Close endpoint has been called. The Receive side is closed (rx queue cleaned)
    // Endpoint is not yet removed from the list because we need to complete the transmission(s)
    transmit_reject(endpoint, address, 0, SL_CPC_REJECT_UNREACHABLE_ENDPOINT);
    sli_cpc_drop_buffer_handle(rx_handle);
    return;
  }

  rx_buffer_payload_len = sli_cpc_hdlc_get_length(rx_handle->hdlc_header) - 2;

  // Validate payload checksum. In case it is invalid, NAK the packet.
  if (rx_buffer_payload_len > 0) {
    fcs = sli_cpc_hdlc_get_fcs(rx_handle->data, rx_buffer_payload_len);
    if (!sli_cpc_validate_crc_sw(rx_handle->data, rx_buffer_payload_len, fcs)) {
      transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_CHECKSUM_MISMATCH);
      SLI_CPC_DEBUG_TRACE_CORE_INVALID_PAYLOAD_CHECKSUM();
      sli_cpc_drop_buffer_handle(rx_handle);
      return;
    }
  }

  if (endpoint->flags & SL_CPC_OPEN_ENDPOINT_FLAG_IFRAME_DISABLE) {
    // iframe disable; drop packet and send reject
    transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_ERROR);
    sli_cpc_drop_buffer_handle(rx_handle);
    return;
  }

  // data received, Push in Rx Queue and send Ack
  if (seq == endpoint->ack) {
#if defined(SL_CPC_ON_POLL_PRESENT)
    if (sli_cpc_hdlc_is_poll_final(control)) {
#if (!defined(SLI_CPC_DEVICE_UNDER_TEST))
      // Only system endpoint can use poll/final
      if (endpoint->id != 0) {
        transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_ERROR);
        sli_cpc_drop_buffer_handle(rx_handle);
        return;
      }
#endif

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
      sl_status_t ret = decrypt_iframe(endpoint, rx_handle, &rx_buffer_payload_len);
      if (ret != SL_STATUS_OK) {
        transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_SECURITY_ISSUE);
        sli_cpc_drop_buffer_handle(rx_handle);
        return;
      }
#endif

      if (endpoint->poll_final.on_poll != NULL) {
        endpoint->poll_final.on_poll(endpoint->id, (void *)SLI_CPC_HDLC_FRAME_TYPE_DATA,
                                     rx_handle->data, rx_buffer_payload_len,
                                     &reply_data, &reply_data_length, &on_write_completed_arg);
      }
      sl_cpc_free_rx_buffer(rx_handle->data);
      rx_handle->data = NULL;
      sli_cpc_drop_buffer_handle(rx_handle);
    }
#endif
    else {
      sl_status_t ret;

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
      ret = decrypt_iframe(endpoint, rx_handle, &rx_buffer_payload_len);
      if (ret != SL_STATUS_OK) {
        transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_SECURITY_ISSUE);
        sli_cpc_drop_buffer_handle(rx_handle);
        return;
      }
#endif

      ret = sli_cpc_push_back_rx_data_in_receive_queue(rx_handle, &endpoint->iframe_receive_queue, rx_buffer_payload_len);

      if (ret != SL_STATUS_OK) {
        transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_OUT_OF_MEMORY);
        sli_cpc_drop_buffer_handle(rx_handle);
        return;
      }
    }
    SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_DATA_FRAME_QUEUED(endpoint);

    #if defined(SL_CATALOG_KERNEL_PRESENT)
    if (endpoint->receive_signal != NULL) {
      EFM_ASSERT(osSemaphoreRelease(endpoint->receive_signal) == osOK);
    }
    #endif

    // Notify the user if a callback is registered
    // We expect the users to not call sl_cpc_read from there
    if (endpoint->on_iframe_data_reception != NULL) {
      endpoint->on_iframe_data_reception(endpoint->id, endpoint->on_iframe_data_reception_arg);
    }

    // Update endpoint acknowledge number
    endpoint->ack++;
    endpoint->ack %= 8;

    // Send ack
    transmit_ack(endpoint);
#if defined(SL_CPC_ON_POLL_PRESENT)
    // Send poll reply (final) if required
    if (reply_data != NULL && reply_data_length > 0) {
      status = write(endpoint, reply_data, reply_data_length, SL_CPC_FLAG_INFORMATION_FINAL, on_write_completed_arg);
      EFM_ASSERT(status == SL_STATUS_OK);
    } else {
      sli_cpc_free_command_buffer(reply_data);
    }
#endif
  } else if (is_seq_valid(seq, endpoint->ack)) {
    // The packet was already received. We must re-send a ACK because the other side missed it the first time
    transmit_ack(endpoint);
    sli_cpc_drop_buffer_handle(rx_handle);
    SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_DUPLICATE_DATA_FRAME(endpoint);
  } else {
    transmit_reject(endpoint, address, endpoint->ack, SL_CPC_REJECT_SEQUENCE_MISMATCH);
    sli_cpc_drop_buffer_handle(rx_handle);
    return;
  }
}

/***************************************************************************//**
 * Process received sframe
 ******************************************************************************/
static void receive_sframe(sl_cpc_endpoint_t *endpoint,
                           sl_cpc_buffer_handle_t *rx_handle,
                           uint8_t control,
                           uint16_t data_length)
{
  SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_SUPERVISORY_FRAME(endpoint);
  // Supervisory packet received
  bool fatal_error = false;
  bool notify = false;
  sl_cpc_endpoint_state_t new_state = endpoint->state;
  uint8_t supervisory_function = sli_cpc_hdlc_get_supervisory_function(control);

  switch (supervisory_function) {
    case SLI_CPC_HDLC_ACK_SUPERVISORY_FUNCTION:
      SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_SUPERVISORY_PROCESSED(endpoint);
      // ACK; already processed previously by receive_ack(), so nothing to do
      break;

    case SLI_CPC_HDLC_REJECT_SUPERVISORY_FUNCTION:
      SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_SUPERVISORY_PROCESSED(endpoint);
      EFM_ASSERT((data_length - 2) == SLI_CPC_HDLC_REJECT_PAYLOAD_SIZE);
      switch (*((sl_cpc_reject_reason_t *)rx_handle->data)) {
        case SL_CPC_REJECT_SEQUENCE_MISMATCH:
          // This is not a fatal error when the tx window is > 1
          fatal_error = true;
          new_state = SL_CPC_STATE_ERROR_FAULT;
          SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_REJECT_SEQ_MISMATCH(endpoint);
          break;
        case SL_CPC_REJECT_CHECKSUM_MISMATCH:
          if (endpoint->re_transmit_queue != NULL) {
            re_transmit_frame(endpoint);
          }
          SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_REJECT_CHECSUM_MISMATCH(endpoint);
          break;
        case SL_CPC_REJECT_OUT_OF_MEMORY:
          if (endpoint->re_transmit_queue != NULL) {
            re_transmit_frame(endpoint);
          }
          SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_REJECT_OUT_OF_MEMORY(endpoint);
          break;
        case SL_CPC_REJECT_SECURITY_ISSUE:
          fatal_error = true;
          new_state = SL_CPC_STATE_ERROR_SECURITY_INCIDENT;
          SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_REJECT_SECURITY_ISSUE(endpoint);
          notify = true;
          break;
        case SL_CPC_REJECT_UNREACHABLE_ENDPOINT:
          fatal_error = true;
          new_state = SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE;
          SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_REJECT_DESTINATION_UNREACHABLE(endpoint);
          notify = true;
          break;
        case SL_CPC_REJECT_ERROR:
        default:
          fatal_error = true;
          new_state = SL_CPC_STATE_ERROR_FAULT;
          SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_REJECT_FAULT(endpoint);
          notify = true;
          break;
      }
      break;
    default:
      // Should not reach this case
      EFM_ASSERT(false);
      break;
  }

  // Free buffers
  sli_cpc_drop_buffer_handle(rx_handle);

  if ((fatal_error)
      && (endpoint->state != SL_CPC_STATE_CLOSING)) {
    // Stop incomming re-transmit timeout
    sl_sleeptimer_stop_timer(&endpoint->re_transmit_timer);
    endpoint->state = new_state;
    if (notify) {
      notify_error(endpoint);
    }
  } else if ((fatal_error)
             && (endpoint->state == SL_CPC_STATE_CLOSING)) {
    // Force shutdown
    clean_tx_queues(endpoint);
    // Free is completed in free_closed_endpoint_if_empty
  }
}

/***************************************************************************//**
 * Process received uframe
 ******************************************************************************/
static void receive_uframe(sl_cpc_endpoint_t *endpoint,
                           sl_cpc_buffer_handle_t *rx_handle,
                           uint8_t control,
                           uint16_t data_length)
{
  uint16_t payload_len = 0;
  uint8_t type;
  uint16_t fcs;

  if (data_length > SLI_CPC_HDLC_FCS_SIZE) {
    payload_len = data_length - SLI_CPC_HDLC_FCS_SIZE;
  }

  SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_FRAME(endpoint);

  if (payload_len > 0) {
    EFM_ASSERT(rx_handle->data != NULL);
    fcs = sli_cpc_hdlc_get_fcs(rx_handle->data, payload_len);
  }

  type = sli_cpc_hdlc_get_unumbered_type(control);
  if (payload_len > 0 && !sli_cpc_validate_crc_sw(rx_handle->data, payload_len, fcs)) {
    sli_cpc_drop_buffer_handle(rx_handle);
    SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_DROPPED(endpoint);
    SLI_CPC_DEBUG_TRACE_CORE_INVALID_PAYLOAD_CHECKSUM();
    return;
  }

  if (!(endpoint->flags & SL_CPC_OPEN_ENDPOINT_FLAG_UFRAME_ENABLE)) {
    SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_DROPPED(endpoint);
    sli_cpc_drop_buffer_handle(rx_handle);
    return;
  }

  if ((type == SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_INFORMATION)
      && !(endpoint->flags & SL_CPC_OPEN_ENDPOINT_FLAG_UFRAME_INFORMATION_DISABLE)) {
    if (sli_cpc_push_back_rx_data_in_receive_queue(rx_handle, &endpoint->uframe_receive_queue, payload_len) != SL_STATUS_OK) {
      SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_DROPPED(endpoint);
      sli_cpc_drop_buffer_handle(rx_handle);
      return;
    }

    // Notify the user if a callback is registered
    // We expect the users to not call sl_cpc_read from there
    if (endpoint->on_uframe_data_reception != NULL) {
      endpoint->on_uframe_data_reception(endpoint->id, endpoint->on_uframe_data_reception_arg);
    }

#if defined(SL_CATALOG_KERNEL_PRESENT)
    if (endpoint->receive_signal != NULL) {
      EFM_ASSERT(osSemaphoreRelease(endpoint->receive_signal) == osOK);
    }
#endif
  } else if (type == SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_POLL_FINAL) {
    if (endpoint->poll_final.on_poll != NULL) {
      void *reply_data;
      uint32_t reply_data_length = 0;

      void *on_write_completed_arg;
      endpoint->poll_final.on_poll(endpoint->id, (void *)SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED,
                                   rx_handle->data, payload_len,
                                   &reply_data, &reply_data_length, &on_write_completed_arg);
      if (reply_data != NULL && reply_data_length > 0) {
        sl_status_t status = write(endpoint, reply_data, reply_data_length, SL_CPC_FLAG_UNNUMBERED_POLL, on_write_completed_arg);
        if ((status != SL_STATUS_OK)
            && (endpoint->on_uframe_write_completed != NULL)) {
          endpoint->on_uframe_write_completed(endpoint->id, reply_data, on_write_completed_arg, status);
        }
      } else {
        sli_cpc_free_command_buffer(reply_data);
      }
    }
    sl_cpc_free_rx_buffer(rx_handle->data);
    rx_handle->data = NULL;
    sli_cpc_drop_buffer_handle(rx_handle);
  } else if (type == SLI_CPC_HDLC_CONTROL_UNNUMBERED_TYPE_RESET_SEQ) {
    sli_cpc_drop_buffer_handle(rx_handle);
    if (endpoint->id != 0) {
      // Can only reset sequence numbers on the system endpoint, drop the packet
      SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_DROPPED(endpoint);
      return;
    } else {
      // Reset sequence numbers on the system endpoint
      endpoint->seq = 0;
      endpoint->ack = 0;
      // Send an unnumbered acknowledgement
      write(endpoint, NULL, 0, SL_CPC_FLAG_UNNUMBERED_ACKNOWLEDGE, NULL);
    }
  } else {
    SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_DROPPED(endpoint);
    sli_cpc_drop_buffer_handle(rx_handle);
    return;
  }

  SLI_CPC_DEBUG_TRACE_ENDPOINT_RXD_UNNUMBERED_PROCESSED(endpoint);
}

/***************************************************************************//**
 * Transmit ACK frame
 ******************************************************************************/
static sl_status_t transmit_ack(sl_cpc_endpoint_t * endpoint)
{
  sl_status_t status;
  sl_cpc_buffer_handle_t *frame_handle;
  sl_cpc_transmit_queue_item_t *item;

  // Get tx queue item
  status = sli_cpc_get_sframe_transmit_queue_item(&item);
  if (status != SL_STATUS_OK) {
    return status;
  }

  // Get new frame handler
  status = sli_cpc_get_buffer_handle(&frame_handle);
  if (status != SL_STATUS_OK) {
    EFM_ASSERT(sli_cpc_free_sframe_transmit_queue_item(item) == SL_STATUS_OK);
    return status;
  }

  frame_handle->endpoint = endpoint;
  frame_handle->address = endpoint->id;

  // Set ACK number in the supervisory control byte
  frame_handle->control = sli_cpc_hdlc_create_control_supervisory(endpoint->ack, 0);

  // Put frame in Tx Q so that it can be transmitted by CPC Core later
  item->handle = frame_handle;

  CORE_ATOMIC_SECTION(sli_cpc_push_back_buffer_handle(&transmit_queue, &item->node, item->handle); );

  SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_ACK(endpoint);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Called on re-transmition of frame
 ******************************************************************************/
__WEAK void sli_cpc_on_frame_retransmit(sl_cpc_transmit_queue_item_t *item)
{
  (void)item;
}

/***************************************************************************//**
 * Re-transmit frame
 ******************************************************************************/
static sl_status_t re_transmit_frame(sl_cpc_endpoint_t* endpoint)
{
  sl_cpc_transmit_queue_item_t *item;
  sl_slist_node_t *item_node;

  CORE_ATOMIC_SECTION(item_node = SLI_CPC_POP_BUFFER_HANDLE_LIST(&endpoint->re_transmit_queue, sl_cpc_transmit_queue_item_t); );
  if (item_node == NULL) {
    return SL_STATUS_NOT_AVAILABLE;
  }

  item = SL_SLIST_ENTRY(item_node, sl_cpc_transmit_queue_item_t, node);
  sli_cpc_on_frame_retransmit(item);

  // Free the header buffer
  sli_cpc_free_hdlc_header(item->handle->hdlc_header);

  //Put frame in Tx Q so that it can be transmitted by CPC Core later
  CORE_ATOMIC_SECTION(sli_cpc_push_buffer_handle(&transmit_queue, &item->node, item->handle); );

  endpoint->packet_re_transmit_count++;
  endpoint->frames_count_re_transmit_queue--;

  // Signal task/process_action that frame is in Tx Queue
  sli_cpc_signal_event(SL_CPC_SIGNAL_TX);

  SLI_CPC_DEBUG_TRACE_ENDPOINT_RETXD_DATA_FRAME(endpoint);
  SLI_CPC_DEBUG_TRACE_CORE_RE_TRANSMIT_FRAME();

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Transmit REJECT frame
 ******************************************************************************/
static void transmit_reject(sl_cpc_endpoint_t * endpoint,
                            uint8_t address,
                            uint8_t ack,
                            sl_cpc_reject_reason_t reason)
{
  uint16_t fcs;
  sl_cpc_buffer_handle_t *handle;
  sl_status_t status;
  sl_cpc_transmit_queue_item_t *item;

  // Get tx queue item
  status = sli_cpc_get_sframe_transmit_queue_item(&item);
  if (status != SL_STATUS_OK) {
    return; // Try again when the primary will re-transmit
  }

  status = sli_cpc_get_reject_buffer(&handle);
  if (status != SL_STATUS_OK) {
    EFM_ASSERT(sli_cpc_free_sframe_transmit_queue_item(item) == SL_STATUS_OK);
    return; // Try again when the primary will re-transmit
  }

  handle->endpoint = endpoint;
  handle->address = address;

  // Set the SEQ number and ACK number in the control byte
  handle->control = sli_cpc_hdlc_create_control_supervisory(ack, SLI_CPC_HDLC_REJECT_SUPERVISORY_FUNCTION);

  // Set in reason
  *((uint8_t *)handle->data) = (uint8_t)reason;

  // Compute payload CRC
  fcs = sli_cpc_get_crc_sw(handle->data, 1);
  handle->fcs[0] = (uint8_t)fcs;
  handle->fcs[1] = (uint8_t)(fcs >> 8);

  // Put frame in Tx Q so that it can be transmitted by CPC Core later
  item->handle = handle;

  CORE_ATOMIC_SECTION(sli_cpc_push_back_buffer_handle(&transmit_queue, &item->node, handle); );

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  if (reason == SL_CPC_REJECT_SEQUENCE_MISMATCH
      || reason == SL_CPC_REJECT_SECURITY_ISSUE
      || reason == SL_CPC_REJECT_UNREACHABLE_ENDPOINT
      || reason == SL_CPC_REJECT_ERROR) {
    sli_cpc_security_drop_incoming_packet();
  }
#endif

#if defined(CPC_DEBUG_TRACE)
  if (endpoint != NULL) {
    switch (reason) {
      case SL_CPC_REJECT_CHECKSUM_MISMATCH:
        SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_REJECT_CHECKSUM_MISMATCH(endpoint);
        break;
      case SL_CPC_REJECT_SEQUENCE_MISMATCH:
        SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_REJECT_SEQ_MISMATCH(endpoint);
        break;
      case SL_CPC_REJECT_OUT_OF_MEMORY:
        SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_REJECT_OUT_OF_MEMORY(endpoint);
        break;
      case SL_CPC_REJECT_SECURITY_ISSUE:
        SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_REJECT_SECURITY_ISSUE(endpoint);
        break;
      case SL_CPC_REJECT_UNREACHABLE_ENDPOINT:
        SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_REJECT_DESTINATION_UNREACHABLE(endpoint);
        break;
      case SL_CPC_REJECT_ERROR:
      default:
        SLI_CPC_DEBUG_TRACE_ENDPOINT_TXD_REJECT_FAULT(endpoint);
        break;
    }
  } else {
    if (reason == SL_CPC_REJECT_UNREACHABLE_ENDPOINT) {
    }
  }
#else
  (void)endpoint;
#endif
}

/***************************************************************************//**
 * Transmit the next data frame queued in a endpoint's transmit queue.
 ******************************************************************************/
static sl_status_t process_tx_queue(void)
{
  sl_status_t status;
  sl_slist_node_t *node = NULL;
  sl_cpc_transmit_queue_item_t *item;
  sl_cpc_buffer_handle_t *frame;
  uint8_t frame_type;
  uint16_t data_length;
  sl_slist_node_t *tx_queue;
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  bool encrypt;
#endif

  CORE_DECLARE_IRQ_STATE;

  // Check if driver is ready or not
  if (sli_cpc_drv_is_transmit_ready() == false) {
    return SL_STATUS_TRANSMIT_BUSY;
  }

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  // If there are packets queued up because they were about to be sent but
  // the security sub-system was not ready, send these packets first.
  // Otherwise, process the regular transmit_queue.
  sl_atomic_load(tx_queue, pending_on_security_ready_queue);
  if (tx_queue != NULL && security_is_ready()) {
    CORE_ATOMIC_SECTION(node = SLI_CPC_POP_BUFFER_HANDLE_LIST(&pending_on_security_ready_queue,
                                                              sl_cpc_transmit_queue_item_t); );
  }
#endif

  // This condition is always true when security is not used.
  // It's just to keep the code a bit more clean.
  if (node == NULL) {
    // Return if nothing to transmit
    sl_atomic_load(tx_queue, transmit_queue);
    if (tx_queue == NULL) {
      return SL_STATUS_EMPTY;
    }

    // Get first queued frame for transmission
    CORE_ATOMIC_SECTION(node = SLI_CPC_POP_BUFFER_HANDLE_LIST(&transmit_queue, sl_cpc_transmit_queue_item_t); );
  }

  item = SL_SLIST_ENTRY(node, sl_cpc_transmit_queue_item_t, node);
  frame = item->handle;

  // Get buffer for HDLC header
  status = sli_cpc_get_hdlc_header_buffer(&frame->hdlc_header);

  if (status != SL_STATUS_OK) {
    // Retry later on
    CORE_ATOMIC_SECTION(sli_cpc_push_buffer_handle(&transmit_queue, &item->node, frame); );
    return SL_STATUS_NO_MORE_RESOURCE;
  }

  // Form the HDLC header
  data_length = (frame->data_length != 0) ? frame->data_length + 2 : 0;

  frame_type = sli_cpc_hdlc_get_frame_type(frame->control);

  if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA) {
    // Update ACK cnt with latest
    LOCK_ENDPOINT(frame->endpoint);
    sli_cpc_hdlc_set_control_ack(&frame->control, frame->endpoint->ack);
    RELEASE_ENDPOINT(frame->endpoint);
  }

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  status = should_encrypt_frame(frame, &encrypt);
  if (status != SL_STATUS_OK) {
    sli_cpc_free_hdlc_header(frame->hdlc_header);
    frame->hdlc_header = NULL;

    // If should_encrypt_frame returns an error, it means the security is not
    // yet ready to process such frame, push it back to an hold back queue
    // for latter processing.
    CORE_ATOMIC_SECTION(sli_cpc_push_buffer_handle(&pending_on_security_ready_queue, &item->node, frame); );

    return SL_STATUS_OK;
  }

  if (encrypt || frame->security_tag) {
    data_length += SLI_SECURITY_TAG_LENGTH_BYTES;
  }
#endif

  /*
   * header must be created after it is known if the frame should be encrypted
   * or not, as it has an impact on the total length of the message.
   */
  sli_cpc_hdlc_create_header(frame->hdlc_header, frame->address, data_length, frame->control, true);

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  if (encrypt) {
    uint16_t fcs;

    status = sli_cpc_get_security_tag_buffer(&frame->security_tag);
    if (status != SL_STATUS_OK) {
      sli_cpc_free_hdlc_header(frame->hdlc_header);
      frame->hdlc_header = NULL;

      return status;
    }

    status = sli_cpc_security_encrypt(frame->endpoint->id,
                                      frame->hdlc_header, SLI_CPC_HDLC_HEADER_RAW_SIZE,
                                      frame->data, frame->data_length,
                                      frame->security_tag, SLI_SECURITY_TAG_LENGTH_BYTES);
    if (status != SL_STATUS_OK) {
      sli_cpc_free_security_tag_buffer(frame->security_tag);
      frame->security_tag = NULL;

      sli_cpc_free_hdlc_header(frame->hdlc_header);
      frame->hdlc_header = NULL;
      return status;
    }

    /*
     * Recompute FCS as the payload is now encrypted and there is
     * an additional security tag to take into account.
     */
    fcs = sli_cpc_get_crc_sw_with_security(frame->data,
                                           frame->data_length,
                                           frame->security_tag,
                                           SLI_SECURITY_TAG_LENGTH_BYTES);

    frame->fcs[0] = (uint8_t)fcs;
    frame->fcs[1] = (uint8_t)(fcs >> 8);
  }
#endif

  // Pass frame to driver for transmission
  CORE_ENTER_ATOMIC();
  status = sli_cpc_drv_transmit_data(frame, frame->data_length);
  if (status != SL_STATUS_OK) {
    // Retry later on
    sli_cpc_push_buffer_handle(&transmit_queue, &item->node, frame);

    // In case the driver returns an error we will wait for driver
    // notification before resuming transmission
    // free HDLC header
    sli_cpc_free_hdlc_header(frame->hdlc_header);
    frame->hdlc_header = NULL;

    CORE_EXIT_ATOMIC();
    return status;
  }

  SLI_CPC_DEBUG_TRACE_ENDPOINT_FRAME_TRANSMIT_SUBMITTED(frame->endpoint);

  // Put frame in re-transmission queue if it's a I-frame type (with data)
  if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_DATA) {
    sli_cpc_push_back_buffer_handle(&frame->endpoint->re_transmit_queue, &item->node, frame);
    frame->endpoint->frames_count_re_transmit_queue++;

    // Remember when we sent this i-frame in order to calculate round trip time
    // Only do so if this is not a re_transmit
    if (frame->endpoint->packet_re_transmit_count == 0u) {
      frame->endpoint->last_iframe_sent_timestamp = sl_sleeptimer_get_tick_count64();
    }

    CORE_EXIT_ATOMIC();
  } else {
    CORE_EXIT_ATOMIC();

    if (frame_type == SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY) {
      sli_cpc_free_sframe_transmit_queue_item(item);
    } else {
      sli_cpc_free_transmit_queue_item(item);
    }
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Process endpoint that need to be closed
 ******************************************************************************/
static void process_close(void)
{
  bool freed;

  if (closed_endpoint_list != NULL) {
    sl_slist_node_t *node;
    CORE_ATOMIC_SECTION(node = sl_slist_pop(&closed_endpoint_list); );
    do {
      sl_cpc_endpoint_t *endpoint = SL_SLIST_ENTRY(node, sl_cpc_endpoint_t, node_closing);
      LOCK_ENDPOINT(endpoint);
      freed = free_closed_endpoint_if_empty(endpoint);
      CORE_ATOMIC_SECTION(node = sl_slist_pop(&closed_endpoint_list); );
      if (!freed) {
        // Something is preventing us from freeing the endpoint
        // process_close will be called again when freeing is possible
        RELEASE_ENDPOINT(endpoint);
      }
    } while (node != NULL);
  }
}

/***************************************************************************//**
 * Add endpoint to closing list
 ******************************************************************************/
static void defer_endpoint_free(sl_cpc_endpoint_t * ep)
{
  CORE_ATOMIC_SECTION(sl_slist_push(&closed_endpoint_list, &ep->node_closing); );
  sli_cpc_signal_event(SL_CPC_SIGNAL_CLOSED);
}

/***************************************************************************//**
 * Called by the dispatcher when the driver completes TX on an i-frame that was acked
 ******************************************************************************/
static void process_deferred_on_write_completed(void *data)
{
  sl_cpc_endpoint_t *endpoint;
  sl_cpc_buffer_handle_t *buffer_handle;

  EFM_ASSERT(data != NULL);
  buffer_handle = (sl_cpc_buffer_handle_t *)data;

  endpoint = find_endpoint(buffer_handle->address);

  if (endpoint->on_iframe_write_completed != NULL) {
    endpoint->on_iframe_write_completed(endpoint->id, buffer_handle->data, buffer_handle->arg, SL_STATUS_OK);
  }
}

/***************************************************************************//**
 * Try to free endpoint in closed state (Must be called with the endpoint locked)
 ******************************************************************************/
static bool free_closed_endpoint_if_empty(sl_cpc_endpoint_t *ep)
{
  bool wait = false;
  bool freed = false;

  // This function must not be called if the endpoint is not in the closed state
  EFM_ASSERT(ep->state == SL_CPC_STATE_CLOSED);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (ep->receive_signal != NULL) {
    // Pending task has not yet been unblocked
    wait = true;
  }
#endif

  if (ep->current_tx_window_space != ep->configured_tx_window_size) {
    wait = true;
  }

  // Don't need to check `holding_list` because `receive_ack()` fills the
  // `tx_queue` and reduce `current_tx_window_space`. So it is impossible
  // to have something in the holding list and have `current_tx_window_space`
  // equal to `configured_tx_window_size`

  if (!wait) {
    // Stop incoming re-transmit timeout
    (void)sl_sleeptimer_stop_timer(&ep->re_transmit_timer);
    ep->state = SL_CPC_STATE_FREED;
    LOCK_ENDPOINTS_LIST();
    sl_slist_remove(&endpoints, &ep->node);
    RELEASE_ENDPOINTS_LIST();
#if defined(SL_CATALOG_KERNEL_PRESENT)
    osMutexDelete(ep->lock);
#endif
    sli_cpc_free_endpoint(ep);
    SLI_CPC_DEBUG_TRACE_CORE_CLOSE_ENDPOINT();
    freed = true;
  }

  return freed;
}

/***************************************************************************//**
 * Clean queue item
 ******************************************************************************/
static void clean_single_queue_item(sl_cpc_endpoint_t *endpoint,
                                    sl_cpc_transmit_queue_item_t *queue_item,
                                    sl_slist_node_t **queue)
{
  uint8_t type = sli_cpc_hdlc_get_frame_type(queue_item->handle->control);

  if ((type == SLI_CPC_HDLC_FRAME_TYPE_DATA)
      && (endpoint->on_iframe_write_completed != NULL)) {
    endpoint->on_iframe_write_completed(endpoint->id,
                                        queue_item->handle->data,
                                        queue_item->handle->arg,
                                        SL_STATUS_TRANSMIT_INCOMPLETE);
  } else if ((type == SLI_CPC_HDLC_FRAME_TYPE_UNNUMBERED)
             && (endpoint->on_uframe_write_completed != NULL)) {
    endpoint->on_uframe_write_completed(endpoint->id,
                                        queue_item->handle->data,
                                        queue_item->handle->arg,
                                        SL_STATUS_TRANSMIT_INCOMPLETE);
  }

  SLI_CPC_REMOVE_BUFFER_HANDLE_FROM_LIST(queue, &queue_item, sl_cpc_transmit_queue_item_t);
  queue_item->handle->data = NULL;
  sli_cpc_drop_buffer_handle(queue_item->handle);

  if (type == SLI_CPC_HDLC_FRAME_TYPE_SUPERVISORY) {
    sli_cpc_free_sframe_transmit_queue_item(queue_item);
  } else {
    sli_cpc_free_transmit_queue_item(queue_item);
  }
}

/***************************************************************************//**
 * Function for freeing items in tx queues
 ******************************************************************************/
static void clean_tx_queues(sl_cpc_endpoint_t * endpoint)
{
  sl_slist_node_t *node;
  CORE_DECLARE_IRQ_STATE;

  LOCK_ENDPOINT(endpoint);

  // Stop incoming re-transmit timeout
  (void)sl_sleeptimer_stop_timer(&endpoint->re_transmit_timer);

  endpoint->packet_re_transmit_count = 0u;

  CORE_ENTER_ATOMIC();

  node = transmit_queue;
  while (node != NULL) {
    sl_cpc_transmit_queue_item_t *queue_item = SL_SLIST_ENTRY(node, sl_cpc_transmit_queue_item_t, node);

    node = node->node;
    if (queue_item->handle->endpoint == endpoint) {
      clean_single_queue_item(endpoint, queue_item, &transmit_queue);
      endpoint->current_tx_window_space++;
    }
  }

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
  node = pending_on_security_ready_queue;
  while (node != NULL) {
    sl_cpc_transmit_queue_item_t *queue_item = SL_SLIST_ENTRY(node, sl_cpc_transmit_queue_item_t, node);

    node = node->node;
    if (queue_item->handle->endpoint == endpoint) {
      clean_single_queue_item(endpoint, queue_item, &pending_on_security_ready_queue);
      // Must be done in atomic context as we clean this flag on the first packet
      // that matches the condition, but there might be other packets queued up
      // for this endpoint
      endpoint->packets_held_for_security = false;
    }
  }
#endif

  CORE_EXIT_ATOMIC();

  node = endpoint->holding_list;
  while (node != NULL) {
    sl_cpc_transmit_queue_item_t *queue_item = SL_SLIST_ENTRY(node, sl_cpc_transmit_queue_item_t, node);

    node = node->node;
    clean_single_queue_item(endpoint, queue_item, &endpoint->holding_list);
  }

  node = endpoint->re_transmit_queue;
  while (node != NULL) {
    sl_cpc_transmit_queue_item_t *queue_item = SL_SLIST_ENTRY(node, sl_cpc_transmit_queue_item_t, node);

    node = node->node;
    SLI_CPC_REMOVE_BUFFER_HANDLE_FROM_LIST(&endpoint->re_transmit_queue, &queue_item, sl_cpc_transmit_queue_item_t);
    if (queue_item->handle->ref_count == 0) {
      // Can only be iframe in re_transmit_queue
      if (endpoint->on_iframe_write_completed != NULL) {
        endpoint->on_iframe_write_completed(endpoint->id, queue_item->handle->data, queue_item->handle->arg, SL_STATUS_TRANSMIT_INCOMPLETE);
      }
      queue_item->handle->data = NULL;
      sli_cpc_drop_buffer_handle(queue_item->handle);
    } else {
      // Will be freed when driver will notify it has completed the transmit
      sl_cpc_endpoint_closed_arg_t *arg;

      EFM_ASSERT(sli_cpc_get_closed_arg(&arg) == SL_STATUS_OK);
      CORE_ENTER_ATOMIC();
      arg->id = endpoint->id;
      arg->on_iframe_write_completed = endpoint->on_iframe_write_completed;
      arg->on_uframe_write_completed = endpoint->on_uframe_write_completed;
      arg->arg = queue_item->handle->arg;
      queue_item->handle->endpoint = NULL;
      queue_item->handle->arg = arg;
      CORE_EXIT_ATOMIC();
    }
    sli_cpc_free_transmit_queue_item(queue_item);
    endpoint->frames_count_re_transmit_queue--;
  }

  endpoint->current_tx_window_space = endpoint->configured_tx_window_size;

  RELEASE_ENDPOINT(endpoint);
}

/***************************************************************************//**
 * Callback for endpoint close timeout
 ******************************************************************************/
static void endpoint_close_timeout_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sl_cpc_endpoint_t *ep = (sl_cpc_endpoint_t *)data;

  (void)handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();

  // If true, the host took too long to reply, close the endpoint anyway
  if (ep->state == SL_CPC_STATE_CLOSING) {
    ep->state = SL_CPC_STATE_CLOSED;
    defer_endpoint_free(ep);
  }

  CORE_EXIT_ATOMIC();
}

/***************************************************************************//**
 * Callback for re-transmit frame
 ******************************************************************************/
static void re_transmit_timeout_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sl_cpc_buffer_handle_t *buffer_handle = (sl_cpc_buffer_handle_t *)data;
  (void)handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (buffer_handle->endpoint->packet_re_transmit_count >= SLI_CPC_RE_TRANSMIT) {
    if (buffer_handle->endpoint->id != SL_CPC_ENDPOINT_SYSTEM) {
      buffer_handle->endpoint->state = SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE;
      sl_sleeptimer_stop_timer(&buffer_handle->endpoint->re_transmit_timer);
      notify_error(buffer_handle->endpoint);
    }
  } else {
    buffer_handle->endpoint->re_transmit_timeout *= 2; // RTO(new) = RTO(before retransmission) *2 )
                                                       // this is explained in Karn’s Algorithm
    if (buffer_handle->endpoint->re_transmit_timeout > sl_sleeptimer_ms_to_tick(SLI_CPC_MAX_RE_TRANSMIT_TIMEOUT_MS)) {
      buffer_handle->endpoint->re_transmit_timeout = sl_sleeptimer_ms_to_tick(SLI_CPC_MAX_RE_TRANSMIT_TIMEOUT_MS);
    }
    re_transmit_frame(buffer_handle->endpoint);
  }
  CORE_EXIT_ATOMIC();
}

/***************************************************************************//**
 * Notify app about endpoint error
 ******************************************************************************/
static void notify_error(sl_cpc_endpoint_t * endpoint)
{
  if (endpoint->on_error != NULL) {
    endpoint->on_error(endpoint->id, endpoint->on_error_arg);
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  if (endpoint->receive_signal != NULL) {
    EFM_ASSERT(osSemaphoreRelease(endpoint->receive_signal) == osOK);
  }
#endif
}

/***************************************************************************//**
 * Check if seq equal ack minus one
 ******************************************************************************/
static bool is_seq_valid(uint8_t seq, uint8_t ack)
{
  bool result = false;

  if (seq == (ack - 1u)) {
    result = true;
  } else if (ack == 0u && seq == 7u) {
    result = true;
  }

  return result;
}

/***************************************************************************//**
 * Increment the endpoint reference counter, returns false if it could not be incremented
 ******************************************************************************/
static bool cpc_enter_api(sl_cpc_endpoint_handle_t * endpoint_handle)
{
  CORE_DECLARE_IRQ_STATE;

  if (endpoint_handle == NULL) {
    return false;
  }

  CORE_ENTER_ATOMIC();

  if (endpoint_handle->ref_count == 0) {
    CORE_EXIT_ATOMIC();
    return false;
  }

  endpoint_handle->ref_count++;

  CORE_EXIT_ATOMIC();
  return true;
}

/***************************************************************************//**
 * Decrement the endpoint reference counter
 ******************************************************************************/
static void cpc_exit_api(sl_cpc_endpoint_handle_t * endpoint_handle)
{
  CORE_ATOMIC_SECTION(endpoint_handle->ref_count--; );
}

/***************************************************************************//**
 * Function for white testing
 ******************************************************************************/
#if (defined(SLI_CPC_DEVICE_UNDER_TEST))
sl_slist_node_t **sli_cpc_dut_get_endpoints_head(void)
{
  return &endpoints->node; // Give the next node since we want to skip the system endpoint
}
#endif

/***************************************************************************//**
 * In case sl_cpc_primary.slcc or sl_cpc_secondary.slcc is not included (for unity tests
 * for example), the definition of sli_cpc_system_init is missing. This is
 * allows to skip compile error
 ******************************************************************************/
__WEAK sl_status_t sli_cpc_system_init(void)
{
  return SL_STATUS_OK;
}
