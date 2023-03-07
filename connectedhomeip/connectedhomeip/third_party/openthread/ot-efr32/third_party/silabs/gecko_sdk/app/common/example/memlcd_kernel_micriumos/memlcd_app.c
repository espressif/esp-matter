/***************************************************************************//**
 * @file memlcd_app.c
 * @brief Memory Liquid Crystal Display (LCD) example functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_board_control.h"
#include "sl_simple_button_instances.h"
#include "em_assert.h"
#include "glib.h"
#include "dmd.h"
#include "os.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#ifndef TASK_STACK_SIZE
#define TASK_STACK_SIZE      64
#endif

#ifndef TASK_PRIO
#define TASK_PRIO            20
#endif

#ifndef BUTTON_INSTANCE_0
#define BUTTON_INSTANCE_0   sl_button_btn0
#endif

#ifndef BUTTON_INSTANCE_1
#define BUTTON_INSTANCE_1   sl_button_btn1
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
static OS_TCB tcb;
static CPU_STK stack[TASK_STACK_SIZE];

static GLIB_Context_t glibContext;
static int currentLine = 0;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void memlcd_app_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize example.
 ******************************************************************************/
void memlcd_app_init(void)
{
  RTOS_ERR err;
  uint32_t status;

  /* Enable the memory lcd */
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the glib context */
  status = GLIB_contextInit(&glibContext);
  EFM_ASSERT(status == GLIB_OK);

  glibContext.backgroundColor = White;
  glibContext.foregroundColor = Black;

  /* Fill lcd with background color */
  GLIB_clear(&glibContext);

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *) &GLIB_FontNarrow6x8);

  /* Draw text on the memory lcd display*/
  GLIB_drawStringOnLine(&glibContext,
                        "MEMLCD Sample App",
                        currentLine++,
                        GLIB_ALIGN_LEFT,
                        5,
                        5,
                        true);
  GLIB_drawStringOnLine(&glibContext,
                        " Press BTN0 to clear",
                        currentLine++,
                        GLIB_ALIGN_LEFT,
                        5,
                        5,
                        true);
  GLIB_drawStringOnLine(&glibContext,
                        " Press BTN1 to print",
                        currentLine++,
                        GLIB_ALIGN_LEFT,
                        5,
                        5,
                        true);

  DMD_updateDisplay();

  // Create Task
  OSTaskCreate(&tcb,
               "memlcd app task",
               memlcd_app_task,
               DEF_NULL,
               TASK_PRIO,
               &stack[0],
               (TASK_STACK_SIZE / 10u),
               TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 * Task.
 ******************************************************************************/
static void memlcd_app_task(void *arg)
{
  (void) &arg;
  do {
    RTOS_ERR err;

    // Delay task to allow sleep
    OSTimeDlyHMSM(24, 0, 0, 0, OS_OPT_TIME_DLY, &err);
    EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
  } while (1);
}

/***************************************************************************//**
 * Callback on button change.
 *
 * This function overrides a weak implementation defined in the simple_button
 * module. It is triggered when the user activates one of the buttons.
 *
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&BUTTON_INSTANCE_0 == handle) {
      currentLine = 0;
      GLIB_clear(&glibContext);
    } else if (&BUTTON_INSTANCE_1 == handle) {
      GLIB_drawStringOnLine(&glibContext,
                            "Hello World!",
                            currentLine++,
                            GLIB_ALIGN_LEFT,
                            5,
                            5,
                            true);
    }
    DMD_updateDisplay();
  }
}
