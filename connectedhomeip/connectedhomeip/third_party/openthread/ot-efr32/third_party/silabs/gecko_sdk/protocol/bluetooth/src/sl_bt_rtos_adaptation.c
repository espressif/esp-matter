/***************************************************************************//**
 * @brief Adaptation for running Bluetooth in RTOS
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <cmsis_os2.h>
#include "sl_cmsis_os2_common.h"
#include "sl_bluetooth.h"
#include "sl_bt_stack_config.h"
#include "sl_bt_rtos_config.h"
#include "sl_bt_rtos_adaptation.h"

#ifdef CONFIGURATION_HEADER
#include CONFIGURATION_HEADER
#endif // CONFIGURATION_HEADER

//Bluetooth event flag definitions
#define SL_BT_RTOS_EVENT_FLAG_STACK            0x00000001U    //Bluetooth task needs an update
#define SL_BT_RTOS_EVENT_FLAG_LL               0x00000002U    //Linklayer task needs an update
#define SL_BT_RTOS_EVENT_FLAG_CMD_WAITING      0x00000004U    //Bluetooth command is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_RSP_WAITING      0x00000008U    //Bluetooth response is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_EVT_WAITING      0x00000010U    //Bluetooth event is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED      0x00000020U    //Bluetooth event is handled
#define SL_BT_RTOS_EVENT_FLAG_START_REQUESTED  0x00000040U    //Bluetooth start has been requested

void sli_bgapi_cmd_handler_delegate(uint32_t header, sl_bgapi_handler, const void*);

static volatile sl_bt_msg_t bluetooth_evt_instance;

static volatile uint32_t command_header;
static volatile void* command_data;
static volatile sl_bgapi_handler command_handler_func = NULL;

//Bluetooth stack thread
static void bluetooth_thread(void *p_arg);
static osThreadId_t tid_thread_bluetooth;
__ALIGNED(8) static uint8_t thread_bluetooth_stk[
  SL_BT_RTOS_HOST_STACK_TASK_STACK_SIZE & 0xFFFFFFF8u];
__ALIGNED(4) static uint8_t thread_bluetooth_cb[osThreadCbSize];

static const osThreadAttr_t thread_bluetooth_attr = {
  .name = "Bluetooth stack",
  .stack_mem = thread_bluetooth_stk,
  .stack_size = sizeof(thread_bluetooth_stk),
  .cb_mem = thread_bluetooth_cb,
  .cb_size = osThreadCbSize,
  .priority = (osPriority_t) SL_BT_RTOS_HOST_STACK_TASK_PRIORITY
};

//Bluetooth linklayer thread
static void linklayer_thread(void *p_arg);
static osThreadId_t tid_thread_link_layer;
__ALIGNED(8) static uint8_t thread_link_layer_stk[
  SL_BT_RTOS_LINK_LAYER_TASK_STACK_SIZE & 0xFFFFFFF8u];
__ALIGNED(4) static uint8_t thread_link_layer_cb[osThreadCbSize];
static const osThreadAttr_t thread_Linklayer_attr = {
  .name = "Bluetooth linklayer",
  .stack_mem = thread_link_layer_stk,
  .stack_size = sizeof(thread_link_layer_stk),
  .cb_mem = thread_link_layer_cb,
  .cb_size = osThreadCbSize,
  .priority = (osPriority_t) SL_BT_RTOS_LINK_LAYER_TASK_PRIORITY
};

//Bluetooth event handler thread
#ifndef SL_BT_DISABLE_EVENT_TASK
static void event_handler_thread(void *p_arg);
static osThreadId_t tid_thread_event_handler;
__ALIGNED(8) static uint8_t thread_event_handler_stk[
  SL_BT_RTOS_EVENT_HANDLER_STACK_SIZE & 0xFFFFFFF8u];
__ALIGNED(4) static uint8_t thread_event_handler_cb[osThreadCbSize];
static const osThreadAttr_t thread_event_handler_attr = {
  .name = "Bluetooth event handler",
  .stack_mem = thread_event_handler_stk,
  .stack_size = sizeof(thread_event_handler_stk),
  .cb_mem = thread_event_handler_cb,
  .cb_size = osThreadCbSize,
  .priority = (osPriority_t) SL_BT_RTOS_EVENT_HANDLER_TASK_PRIORITY
};
#endif

static osEventFlagsId_t bluetooth_event_flags;
__ALIGNED(4) static uint8_t bluetooth_event_flags_cb[osEventFlagsCbSize];
static const osEventFlagsAttr_t bluetooth_event_flags_attr = {
  .name = "Bluetooth event flags",
  .attr_bits = 0U,
  .cb_mem = bluetooth_event_flags_cb,
  .cb_size = osEventFlagsCbSize
};

static osMutexId_t bluetooth_mutex_id;
__ALIGNED(4) static uint8_t bluetooth_mutex_cb[osMutexCbSize];
static const osMutexAttr_t bluetooth_mutex_attr = {
  .name = "Bluetooth Mutex",
  .attr_bits = osMutexRecursive | osMutexPrioInherit,
  .cb_mem = bluetooth_mutex_cb,
  .cb_size = osMutexCbSize
};

static osMutexId_t bgapi_mutex_id;
__ALIGNED(4) static uint8_t bgapi_mutex_cb[osMutexCbSize];
static const osMutexAttr_t bgapi_mutex_attr = {
  .name = "BGAPI Mutex",
  .attr_bits = osMutexRecursive | osMutexPrioInherit,
  .cb_mem = bgapi_mutex_cb,
  .cb_size = osMutexCbSize
};

static void sl_bt_rtos_deinit()
{
  (void) osEventFlagsDelete(bluetooth_event_flags);
  bluetooth_event_flags = NULL;
  (void) osMutexDelete(bluetooth_mutex_id);
  bluetooth_mutex_id = NULL;
  (void) osMutexDelete(bgapi_mutex_id);
  bgapi_mutex_id = NULL;
  (void) osThreadTerminate(tid_thread_bluetooth);
  tid_thread_bluetooth = NULL;
  (void) osThreadTerminate(tid_thread_link_layer);
  tid_thread_link_layer = NULL;
#ifndef SL_BT_DISABLE_EVENT_TASK
  (void) osThreadTerminate(tid_thread_event_handler);
  tid_thread_event_handler = NULL;
#endif
}

sl_status_t sl_bt_rtos_init()
{
  // Create event flags
  if (bluetooth_event_flags == NULL) {
    bluetooth_event_flags = osEventFlagsNew(&bluetooth_event_flags_attr);
  }
  if (bluetooth_event_flags == NULL) {
    goto failed;
  }

  // Create mutex for Bluetooth stack
  if (bluetooth_mutex_id == NULL) {
    bluetooth_mutex_id = osMutexNew(&bluetooth_mutex_attr);
  }
  if (bluetooth_mutex_id == NULL) {
    goto failed;
  }

  // Create mutex for BGAPI
  if (bgapi_mutex_id == NULL) {
    bgapi_mutex_id = osMutexNew(&bgapi_mutex_attr);
  }
  if (bgapi_mutex_id == NULL) {
    goto failed;
  }

  sli_bgapi_set_cmd_handler_delegate(sli_bt_cmd_handler_rtos_delegate);

  // Create thread for Bluetooth stack
  if (tid_thread_bluetooth == NULL) {
    tid_thread_bluetooth = osThreadNew(bluetooth_thread,
                                       NULL,
                                       &thread_bluetooth_attr);
  }
  if (tid_thread_bluetooth == NULL) {
    goto failed;
  }

  // Create thread for Linklayer
  if (tid_thread_link_layer == NULL) {
    tid_thread_link_layer = osThreadNew(linklayer_thread,
                                        NULL,
                                        &thread_Linklayer_attr);
  }
  if (tid_thread_link_layer == NULL) {
    goto failed;
  }

  // Create thread for Bluetooth event handler
#ifndef SL_BT_DISABLE_EVENT_TASK
  if (tid_thread_event_handler == NULL) {
    tid_thread_event_handler = osThreadNew(event_handler_thread,
                                           NULL,
                                           &thread_event_handler_attr);
  }
  if (tid_thread_event_handler == NULL) {
    goto failed;
  }
#endif
  return SL_STATUS_OK;
  failed:
  sl_bt_rtos_deinit();
  return SL_STATUS_FAIL;
}

//This callback is called from interrupt context (Kernel Aware)
//sets flag to trigger Link Layer Task
void sli_bt_rtos_ll_callback()
{
  osEventFlagsSet(bluetooth_event_flags,
                  SL_BT_RTOS_EVENT_FLAG_LL);
}
//This callback is called from Bluetooth stack
//Called from kernel aware interrupt context (RTCC interrupt) and from Bluetooth task
//sets flag to trigger running Bluetooth stack
void sli_bt_rtos_stack_callback()
{
  osEventFlagsSet(bluetooth_event_flags,
                  SL_BT_RTOS_EVENT_FLAG_STACK);
}

/**
 * Internal stack function to get how long the Bluetooth stack can sleep.
 *
 * @return 0 if the stack cannot sleep;
 * Maximum value of uint32_t if the stack has no task scheduled to process;
 * The ticks (in sleeptimer frequency) the stack needs to wake up to process a task
 */
extern uint32_t sli_bt_can_sleep_ticks();

/**
 * Internal stack function to start the Bluetooth stack.
 *
 * @return SL_STATUS_OK if the stack was successfully started
 */
extern sl_status_t sli_bt_system_start_bluetooth();

//Bluetooth task, it waits for events from bluetooth and handles them
void bluetooth_thread(void *p_arg)
{
  (void)p_arg;

  uint32_t flags = SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;

  sl_bt_init();

  while (1) {
    //Start the stack if that has been requested
    if ((flags & SL_BT_RTOS_EVENT_FLAG_START_REQUESTED) && (flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)) {
      flags &= ~SL_BT_RTOS_EVENT_FLAG_START_REQUESTED;
      sl_status_t status = sli_bt_system_start_bluetooth();
      if (status != SL_STATUS_OK) {
        //Starting the stack has failed. Generate a system error event to let the application know.
        uint32_t evt_len = sizeof(bluetooth_evt_instance.data.evt_system_error);
        bluetooth_evt_instance.header = sl_bt_evt_system_error_id | (evt_len << 8);
        bluetooth_evt_instance.data.evt_system_error.reason = (uint16_t) status;
        bluetooth_evt_instance.data.evt_system_error.data.len = 0;
        osEventFlagsSet(bluetooth_event_flags,
                        SL_BT_RTOS_EVENT_FLAG_EVT_WAITING);
        flags &= ~SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;
      }
    }

    //Command needs to be sent to Bluetooth stack
    if (flags & SL_BT_RTOS_EVENT_FLAG_CMD_WAITING) {
      uint32_t header = command_header;
      sl_bgapi_handler cmd_handler = command_handler_func;
      sli_bgapi_cmd_handler_delegate(header, cmd_handler, (void*)command_data);
      command_handler_func = NULL;
      flags &= ~SL_BT_RTOS_EVENT_FLAG_CMD_WAITING;
      osEventFlagsSet(bluetooth_event_flags,
                      SL_BT_RTOS_EVENT_FLAG_RSP_WAITING);
    }

    //Run Bluetooth stack. Pop the next event for application
    sl_bt_run();
    if (sl_bt_event_pending() && (flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)) {  //update bluetooth & read event
      sl_status_t status = sl_bt_pop_event((sl_bt_msg_t*) &bluetooth_evt_instance);
      if (SL_STATUS_OK != status) {
        // in case of error, continue
        continue;
      }
      osEventFlagsSet(bluetooth_event_flags,
                      SL_BT_RTOS_EVENT_FLAG_EVT_WAITING);
      flags &= ~SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;
    }

    uint32_t timeout = sli_bt_can_sleep_ticks();
    if (timeout == 0 && (flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)) {
      continue;
    }
    flags |= osEventFlagsWait(bluetooth_event_flags,
                              SL_BT_RTOS_EVENT_FLAG_STACK
                              + SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED
                              + SL_BT_RTOS_EVENT_FLAG_CMD_WAITING
                              + SL_BT_RTOS_EVENT_FLAG_START_REQUESTED,
                              osFlagsWaitAny,
                              osWaitForever);
    if ((flags & 0x80000000u) == 0x80000000u) {
      // in case of error, reset the flag and continue
      flags = SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;
      continue;
    }
    // flag_stack is used to wakeup from pend and then sl_bt_event_pending() is used to check if event is queued
    // even if event stays in queue and task again sleeps, it is woke up by evt_handled and then it can be processed.
  }
}

static void linklayer_thread(void *p_arg)
{
  (void)p_arg;

  while (1) {
    osEventFlagsWait(bluetooth_event_flags,
                     SL_BT_RTOS_EVENT_FLAG_LL,
                     osFlagsWaitAny,
                     osWaitForever);

    sl_bt_priority_handle();
  }
}

// Event task, this calls the application code
#ifndef SL_BT_DISABLE_EVENT_TASK
static void event_handler_thread(void *p_arg)
{
  (void)p_arg;

  while (1) {
    osEventFlagsWait(bluetooth_event_flags,
                     SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
                     osFlagsWaitAny,
                     osWaitForever);
    sl_bt_process_event((sl_bt_msg_t*) &bluetooth_evt_instance);

    osEventFlagsSet(bluetooth_event_flags,
                    SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED);
  }
}
#endif

//hooks for API
//called from tasks using Bluetooth API
void sli_bt_cmd_handler_rtos_delegate(uint32_t header, sl_bgapi_handler handler, const void* payload)
{
  command_header = header;
  command_handler_func = handler;
  command_data = (void*)payload;
  //Command structure is filled, notify the stack
  osEventFlagsSet(bluetooth_event_flags,
                  SL_BT_RTOS_EVENT_FLAG_CMD_WAITING);
  //wait for response
  osEventFlagsWait(bluetooth_event_flags,
                   SL_BT_RTOS_EVENT_FLAG_RSP_WAITING,
                   osFlagsWaitAny,
                   osWaitForever);
}

static sl_status_t osFlags2sl_status(uint32_t os_flags)
{
  switch (os_flags) {
    case osFlagsErrorParameter:
      return SL_STATUS_INVALID_PARAMETER;
    case osFlagsErrorTimeout:
      return SL_STATUS_TIMEOUT;
    case osFlagsErrorISR:
      return SL_STATUS_ISR;
    default:
      return SL_STATUS_FAIL;
  }
}

sl_status_t sl_bt_rtos_has_event_waiting()
{
  uint32_t flags = osEventFlagsWait(bluetooth_event_flags,
                                    SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
                                    osFlagsWaitAny | osFlagsNoClear,
                                    0);

  if ((flags & 0x80000000u) == 0x80000000u) {
    return osFlags2sl_status(flags); //some error returned
  } else {
    if ((flags & SL_BT_RTOS_EVENT_FLAG_EVT_WAITING)
        == SL_BT_RTOS_EVENT_FLAG_EVT_WAITING) {
      return SL_STATUS_OK;
    } else {
      return SL_STATUS_FAIL;
    }
  }
}

sl_status_t sl_bt_rtos_event_wait(bool blocking)
{
  uint32_t timeout = blocking ? osWaitForever : 0u;
  uint32_t flags = osEventFlagsWait(bluetooth_event_flags,
                                    SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
                                    osFlagsWaitAny | osFlagsNoClear,
                                    timeout);

  if ((flags & 0x80000000u) == 0x80000000u) {
    return osFlags2sl_status(flags); //some error returned
  } else {
    if ((flags & SL_BT_RTOS_EVENT_FLAG_EVT_WAITING)
        == SL_BT_RTOS_EVENT_FLAG_EVT_WAITING) {
      osEventFlagsClear(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_EVT_WAITING);
      return SL_STATUS_OK;
    } else {
      return SL_STATUS_FAIL;
    }
  }
}

sl_status_t sl_bt_rtos_set_event_handled()
{
  uint32_t flags = osEventFlagsSet(bluetooth_event_flags,
                                   SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED);

  if ((flags & 0x80000000u) == 0x80000000u) {
    return osFlags2sl_status(flags); //some error returned
  } else {
    if ((flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)
        == SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED) {
      return SL_STATUS_OK;
    } else {
      return SL_STATUS_FAIL;
    }
  }
}

static sl_status_t os2sl_status(osStatus_t ret)
{
  switch (ret) {
    case osOK:
      return SL_STATUS_OK;
    case osErrorTimeout:
      return SL_STATUS_TIMEOUT;
    case osErrorISR:
      return SL_STATUS_ISR;
    case osErrorParameter:
      return SL_STATUS_INVALID_PARAMETER;
    default:
      return SL_STATUS_FAIL; //return this error for codes haven't been mapped yet
  }
}

// Called by Bluetooth stack to lock BGAPI for exclusive access
sl_status_t sli_bgapi_lock()
{
  osStatus_t ret = osMutexAcquire(bgapi_mutex_id, osWaitForever);
  return os2sl_status(ret);
}

// Called by Bluetooth stack to release the lock obtained by @ref sli_bgapi_lock
void sli_bgapi_unlock()
{
  (void) osMutexRelease(bgapi_mutex_id);
}

// Request the starting of Bluetooth
sl_status_t sli_request_rtos_bluetooth_start()
{
  uint32_t flags = osEventFlagsSet(bluetooth_event_flags,
                                   SL_BT_RTOS_EVENT_FLAG_START_REQUESTED);
  if ((flags & 0x80000000u) == 0x80000000u) {
    return osFlags2sl_status(flags); // Error was returned
  }

  // Return the special status to indicate that stack will now start automatically
  return SL_STATUS_IN_PROGRESS;
}

sl_status_t sl_bt_bluetooth_pend()
{
  osStatus_t ret = osMutexAcquire(bluetooth_mutex_id, osWaitForever);
  return os2sl_status(ret);
}

sl_status_t sl_bt_bluetooth_post()
{
  osStatus_t ret = osMutexRelease(bluetooth_mutex_id);
  return os2sl_status(ret);
}

const sl_bt_msg_t* sl_bt_rtos_get_event()
{
  return (const sl_bt_msg_t*) &bluetooth_evt_instance;
}
