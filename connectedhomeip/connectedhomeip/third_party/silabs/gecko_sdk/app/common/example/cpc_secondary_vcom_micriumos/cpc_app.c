/***************************************************************************//**
 * @file cpc_app.c
 * @brief Secondary firmware for the CPC sample application
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>
#include <string.h>

#include "sl_cpc.h"
#include "sl_assert.h"
#include "os.h"

#include "cpc_app.h"

/*******************************************************************************
 *******************************  DEFINES   ************************************
 ******************************************************************************/
#ifndef TASK_STACK_SIZE
#define TASK_STACK_SIZE       128
#endif

#ifndef TASK_PRIO
#define TASK_PRIO             30
#endif

#define ENDPOINT_0_CONNECTED  0x1
#define ENDPOINT_1_CONNECTED  0x2

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static uint8_t static_write_array[SL_CPC_RX_PAYLOAD_MAX_LENGTH];
static sl_cpc_endpoint_handle_t user_endpoint_handle_0;
static sl_cpc_endpoint_handle_t user_endpoint_handle_1;

static OS_TCB tcb_0;
static OS_TCB tcb_1;
static OS_TCB tcb_connections;

static CPU_STK stack_task_0[TASK_STACK_SIZE];
static CPU_STK stack_task_1[TASK_STACK_SIZE];
static CPU_STK stack_task_connections[TASK_STACK_SIZE];

static OS_FLAG_GRP endpoint_connected_flags;
static OS_SEM      tx_complete_semaphore;

/*******************************************************************************
 *************************  LOCAL FUNCTIONS   **********************************
 ******************************************************************************/

// Callback triggered on error
static void on_endpoint_error(uint8_t endpoint_id,
                              void *arg)
{
  (void) arg;
  RTOS_ERR err;

  if ((endpoint_id == SL_CPC_ENDPOINT_USER_ID_0)
      && (sl_cpc_get_endpoint_state(&user_endpoint_handle_0) == SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE)) {
    // clear connected flag
    OSFlagPost(&endpoint_connected_flags,
               ENDPOINT_0_CONNECTED,
               OS_OPT_POST_FLAG_CLR,
               &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  } else if ((endpoint_id == SL_CPC_ENDPOINT_USER_ID_1)
             && (sl_cpc_get_endpoint_state(&user_endpoint_handle_1) == SL_CPC_STATE_ERROR_DESTINATION_UNREACHABLE)) {
    // clear connected flag
    OSFlagPost(&endpoint_connected_flags,
               ENDPOINT_1_CONNECTED,
               OS_OPT_POST_FLAG_CLR,
               &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  } else {
    EFM_ASSERT(0);
  }
}

// Callback triggered on write complete
static void on_write_completed(sl_cpc_user_endpoint_id_t endpoint_id,
                               void *buffer,
                               void *arg,
                               sl_status_t status)
{
  (void) arg;
  (void) buffer;
  RTOS_ERR err;
  OS_SEM_CTR sem;

  EFM_ASSERT(status == SL_STATUS_OK);

  if (endpoint_id == SL_CPC_ENDPOINT_USER_ID_0) {
    // user endpoint 0 uses a dynamically allocated buffer
    // which can be safely freed in the on_write_completed callback.
    free(buffer);
  } else if (endpoint_id == SL_CPC_ENDPOINT_USER_ID_1) {
    // user endpoint 1 uses a static buffer, so a semaphore is used
    // to ensure that the buffer is available for the next write
    sem = OSSemPost(&tx_complete_semaphore,
                    OS_OPT_POST_ALL,
                    &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
    EFM_ASSERT(sem != 0);
  } else {
    EFM_ASSERT(0);
  }
}

/***************************************************************************//**
 * Open and configure endpoint
 ******************************************************************************/
static void open_endpoint(sl_cpc_endpoint_handle_t *ep, uint8_t endpoint_id)
{
  sl_status_t status;

  status = sl_cpc_open_user_endpoint(ep, endpoint_id, 0, 1);
  EFM_ASSERT(status == SL_STATUS_OK);

  // the same callback can be used for multiple endpoints, as the endpoint_id is passed
  // to it
  status = sl_cpc_set_endpoint_option(ep, SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED,
                                      (void *)on_write_completed);
  EFM_ASSERT(status == SL_STATUS_OK);

  status = sl_cpc_set_endpoint_option(ep, SL_CPC_ENDPOINT_ON_ERROR,
                                      (void *)on_endpoint_error);
  EFM_ASSERT(status == SL_STATUS_OK);
}

/***************************************************************************//**
 * CPC task for user endpoint 0
 ******************************************************************************/
static void cpc_app_task_endpoint_0(void *arg)
{
  sl_status_t status;
  void *read_array;
  uint16_t size;
  uint8_t *dynamic_write_array;
  RTOS_ERR err;
  OS_FLAGS flags;
  (void) &arg;

  while (1) {
    flags = OSFlagPend(&endpoint_connected_flags,
                       ENDPOINT_0_CONNECTED,
                       0u,
                       OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_BLOCKING,
                       (CPU_TS *)NULL,
                       &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    if (flags & ENDPOINT_0_CONNECTED) {
      // read is blocking
      status = sl_cpc_read(&user_endpoint_handle_0, &read_array, &size, 0, 0u);
      // status may be SL_STATUS_EMPTY if the endpoint is closed
      EFM_ASSERT(status == SL_STATUS_OK || status == SL_STATUS_EMPTY);
      if (size > 0) {
        // allocate a buffer which will be freed in the on_write_completed callback
        dynamic_write_array = (uint8_t *)malloc(size);
        EFM_ASSERT(dynamic_write_array != NULL);
        memcpy(dynamic_write_array, read_array, size);
        // return the buffer to CPC
        sl_cpc_free_rx_buffer(read_array);
        // echo the data back to the host
        status = sl_cpc_write(&user_endpoint_handle_0, dynamic_write_array, size, 0u, NULL);
        EFM_ASSERT(status == SL_STATUS_OK);
      }
    }
  }
}

/***************************************************************************//**
 * CPC task for user endpoint 1
 ******************************************************************************/
static void cpc_app_task_endpoint_1(void *arg)
{
  sl_status_t status;
  void *read_array;
  uint16_t size;
  RTOS_ERR err;
  OS_FLAGS flags;
  (void) &arg;

  while (1) {
    flags = OSFlagPend(&endpoint_connected_flags,
                       ENDPOINT_1_CONNECTED,
                       0u,
                       OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_BLOCKING,
                       (CPU_TS *)NULL,
                       &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    if (flags & ENDPOINT_1_CONNECTED) {
      // read is blocking
      status = sl_cpc_read(&user_endpoint_handle_1, &read_array, &size, 0u, 0u);
      // status may be SL_STATUS_EMPTY if the endpoint is closed
      EFM_ASSERT(status == SL_STATUS_OK || status == SL_STATUS_EMPTY);
      if (size > 0) {
        EFM_ASSERT(size <= sizeof(static_write_array));
        memcpy(static_write_array, read_array, size);
        // return the buffer to CPC
        sl_cpc_free_rx_buffer(read_array);
        // echo the data back to the host
        status = sl_cpc_write(&user_endpoint_handle_1, static_write_array, size, 0, NULL);
        EFM_ASSERT(status == SL_STATUS_OK);
        // wait for tx to complete
        OSSemPend(&tx_complete_semaphore,
                  1000,
                  OS_OPT_PEND_BLOCKING,
                  (CPU_TS *)NULL,
                  &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
      }
    }
  }
}

/***************************************************************************//**
 * CPC task for monitoring connections
 ******************************************************************************/
static void cpc_app_task_connections(void *arg)
{
  RTOS_ERR err;
  OS_FLAGS flags;
  sl_status_t status;
  (void) &arg;

  // Open user endpoints and set flags
  open_endpoint(&user_endpoint_handle_0, SL_CPC_ENDPOINT_USER_ID_0);
  // set connected flag
  OSFlagPost(&endpoint_connected_flags,
             ENDPOINT_0_CONNECTED,
             OS_OPT_POST_FLAG_SET,
             &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  open_endpoint(&user_endpoint_handle_1, SL_CPC_ENDPOINT_USER_ID_1);
  // set connected flag
  OSFlagPost(&endpoint_connected_flags,
             ENDPOINT_1_CONNECTED,
             OS_OPT_POST_FLAG_SET,
             &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // enter the main loop of the task
  while (1) {
    // Pend on the endpoint connected flag group. If any flag is cleared,
    // the corresponding endpoint is in error and the endpoint
    // must be reopened
    flags = OSFlagPend(&endpoint_connected_flags,
                       ENDPOINT_0_CONNECTED | ENDPOINT_1_CONNECTED,
                       0u,
                       OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_BLOCKING,
                       (CPU_TS *)NULL,
                       &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

    if (flags & ENDPOINT_0_CONNECTED) {
      // endpoint 0 has disconnected
      status = sl_cpc_close_endpoint(&user_endpoint_handle_0);
      // status will be SL_STATUS_BUSY if the endpoint is in the process of closing
      EFM_ASSERT(status == SL_STATUS_OK || status == SL_STATUS_BUSY);

      if (sl_cpc_get_endpoint_state(&user_endpoint_handle_0) == SL_CPC_STATE_FREED) {
        // User endpoint ids are defined in sl_cpc.h
        open_endpoint(&user_endpoint_handle_0, SL_CPC_ENDPOINT_USER_ID_0);
        // set connected flag
        OSFlagPost(&endpoint_connected_flags,
                   ENDPOINT_0_CONNECTED,
                   OS_OPT_POST_FLAG_SET,
                   &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
      } else {
        // allow core thread to free endpoint
        OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_DLY, &err);
      }
    }

    if (flags & ENDPOINT_1_CONNECTED) {
      // endpoint 1 has disconnected
      status = sl_cpc_close_endpoint(&user_endpoint_handle_1);
      // status will be SL_STATUS_BUSY if the endpoint is in the process of closing
      EFM_ASSERT(status == SL_STATUS_OK || status == SL_STATUS_BUSY);

      if (sl_cpc_get_endpoint_state(&user_endpoint_handle_1) == SL_CPC_STATE_FREED) {
        // User endpoint ids are defined in sl_cpc.h
        open_endpoint(&user_endpoint_handle_1, SL_CPC_ENDPOINT_USER_ID_1);
        // set connected flag
        OSFlagPost(&endpoint_connected_flags,
                   ENDPOINT_1_CONNECTED,
                   OS_OPT_POST_FLAG_SET,
                   &err);
        EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
      } else {
        // allow core thread to free endpoint
        OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_DLY, &err);
      }
    }
  }
}
/*******************************************************************************
 ************************  GLOBAL FUNCTIONS   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize CPC application
 ******************************************************************************/
void cpc_app_init(void)
{
  RTOS_ERR err;

  // Create semaphore for transmission complete
  OSSemCreate(&tx_complete_semaphore,
              "tx complete semaphore",
              0u,
              &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create flag group for endpoint disconnect events
  OSFlagCreate(&endpoint_connected_flags,
               "Endpoint connected flag group",
               0u,
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  // Create Tasks
  OSTaskCreate(&tcb_0,
               "cpc task endpoint 0",
               cpc_app_task_endpoint_0,
               DEF_NULL,
               TASK_PRIO,
               &stack_task_0[0],
               (TASK_STACK_SIZE / 10u),
               TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTaskCreate(&tcb_1,
               "cpc task endpoint 1",
               cpc_app_task_endpoint_1,
               DEF_NULL,
               TASK_PRIO,
               &stack_task_1[0],
               (TASK_STACK_SIZE / 10u),
               TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  OSTaskCreate(&tcb_connections,
               "cpc task connection monitor",
               cpc_app_task_connections,
               DEF_NULL,
               TASK_PRIO,
               &stack_task_connections[0],
               (TASK_STACK_SIZE / 10u),
               TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}
