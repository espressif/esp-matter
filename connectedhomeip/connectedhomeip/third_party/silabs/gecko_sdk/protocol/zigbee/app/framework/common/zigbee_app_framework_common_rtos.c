/***************************************************************************//**
 * @brief ZigBee CMSIS RTOS support code.
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

#include PLATFORM_HEADER
#include "sl_component_catalog.h"

#include "ember.h"

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#if defined(EMBER_AF_NCP) && defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
#include "sl_iostream.h"
#endif

#include "zigbee_app_framework_common.h"
#include "zigbee_rtos_task_config.h"

//------------------------------------------------------------------------------
// Tasks variables and defines

#define ZIGBEE_STACK_TASK_PRIORITY SL_ZIGBEE_OS_STACK_TASK_PRIORITY
#define ZIGBEE_TASK_YIELD 0x0001U
#define ZIGBEE_TASK_SEMAPHORE_MAX_COUNT 255
#define ZIGBEE_TASK_SEMAPHORE_INITIAL_COUNT 0

//Zigbee stack size is specified in "word-increments". Multiply by size of void pointer
static osThreadId_t zigbee_task_id;
__ALIGNED(8) static uint8_t zigbee_task_stack[SL_ZIGBEE_OS_STACK_TASK_SIZE * sizeof(void *)];
__ALIGNED(4) static uint8_t zigbee_task_cb[osThreadCbSize];
static osThreadAttr_t zigbee_task_attr;

static osSemaphoreId_t zigbee_task_semaphore_id;
__ALIGNED(4) static uint8_t zigbee_task_semaphore_cb[osSemaphoreCbSize];
static osSemaphoreAttr_t zigbee_task_semaphore_attr = {
  .name = "Zigbee task semphore",
  .cb_mem = zigbee_task_semaphore_cb,
  .cb_size = osSemaphoreCbSize,
  .attr_bits = 0
};

//------------------------------------------------------------------------------
// Forward and external declarations.

extern void App_OS_SetAllHooks(void);

static void zigbee_task(void *p_arg);
static void zigbee_stack_task_yield(void);

//------------------------------------------------------------------------------
// APIs and callbacks.

void sl_zigbee_common_rtos_wakeup_stack_task(void)
{
  osStatus_t retVal = osSemaphoreRelease(zigbee_task_semaphore_id);
  assert(retVal != osErrorParameter);
}

void sli_zigbee_common_rtos_init_callback(void)
{
  // Create ZigBee task.
  zigbee_task_attr.name = "Zigbee task";
  zigbee_task_attr.stack_mem = &zigbee_task_stack[0];
  zigbee_task_attr.stack_size = sizeof(zigbee_task_stack);
  zigbee_task_attr.cb_mem = zigbee_task_cb;
  zigbee_task_attr.cb_size = osThreadCbSize;
  zigbee_task_attr.priority = ZIGBEE_STACK_TASK_PRIORITY;
  zigbee_task_attr.attr_bits = 0;
  zigbee_task_attr.tz_module = 0;

  zigbee_task_id = osThreadNew(zigbee_task,
                               NULL,
                               &zigbee_task_attr);
  assert(zigbee_task_id != NULL);

  zigbee_task_semaphore_id =   osSemaphoreNew(ZIGBEE_TASK_SEMAPHORE_MAX_COUNT,
                                              ZIGBEE_TASK_SEMAPHORE_INITIAL_COUNT,
                                              &zigbee_task_semaphore_attr);
  assert(zigbee_task_semaphore_id != NULL);
}

void sli_zigbee_common_rtos_wakeup_isr_callback(void)
{
  sl_zigbee_common_rtos_wakeup_stack_task();
}

//------------------------------------------------------------------------------
// Static functions.
#if defined(EMBER_AF_NCP) && defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
void sli_iostream_on_uart_rx(sl_iostream_t *handle)
{
  sli_zigbee_common_rtos_wakeup_isr_callback();
}
#endif

static void zigbee_task(void *p_arg)
{
  (void)p_arg;

  sli_zigbee_stack_init_callback();
  sli_zigbee_app_framework_init_callback();
#ifdef EMBER_AF_NCP
  sli_zigbee_ncp_init_callback();
#endif

  while (true) {
    sli_zigbee_stack_tick_callback();
    sli_zigbee_app_framework_tick_callback();
#ifdef EMBER_AF_NCP
    sli_zigbee_ncp_tick_callback();
#endif

    // Yield the ZigBee stack task if possible.
    zigbee_stack_task_yield();
  }
}

static void zigbee_stack_task_yield(void)
{
  uint32_t yield_time_ms = 0;
  uint32_t yield_time_ticks = 0;

  // Function below adds or removes EM1 requirement
  // and also checks stack and app events to see how long
  // the zigbee task can pend for
  yield_time_ms = sli_zigbee_app_framework_set_pm_requirements_and_get_ms_to_next_wakeup();

  // Convert time in ms to ticks
  if (yield_time_ms != osWaitForever) {
    yield_time_ticks = (osKernelGetTickFreq() * yield_time_ms) / 1000;
  } else {
    yield_time_ticks = osWaitForever;
  }

  if (yield_time_ticks > 0) {
    osStatus_t retVal = osSemaphoreAcquire(zigbee_task_semaphore_id, yield_time_ms);
    assert((retVal != osErrorParameter) && (retVal != osErrorResource));
  }
}
