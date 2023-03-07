/***************************************************************************//**
 * @file
 * @brief Top level application functions
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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "os.h"
#include "sl_usbd_core.h"
#include "sl_usbd_class_cdc.h"
#include "sl_usbd_class_cdc_acm.h"

#include "sl_usbd_class_cdc_acm_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Task configuration
#define TASK_STACK_SIZE      512u
#define TASK_PRIO            31u

// Menu Message and Length
#define  ACM_TERMINAL_BUF_LEN                   512u
#define  ACM_TERMINAL_SCREEN_SIZE               80u

#define  ACM_TERMINAL_CURSOR_START              "\033[H"
#define  ACM_TERMINAL_CURSOR_START_SIZE         3u

#define  ACM_TERMINAL_SCREEN_CLR                "\033[2J\033[H"
#define  ACM_TERMINAL_SCREEN_CLR_SIZE           7u

#define  ACM_TERMINAL_MSG                       "===== USB CDC ACM Serial Emulation Demo ======" \
                                                "\r\n"                                           \
                                                "\r\n"                                           \
                                                "1. Echo 1 demo.\r\n"                            \
                                                "2. Echo N demo.\r\n"                            \
                                                "Option: "

#define  ACM_TERMINAL_MSG_SIZE                  92u

#define  ACM_TERMINAL_MSG1                      "Echo 1 demo... \r\n\r\n>> "
#define  ACM_TERMINAL_MSG1_SIZE                 22u

#define  ACM_TERMINAL_MSG2                      "Echo N demo. You can send up to 512 characters at once... \r\n\r\n>> "
#define  ACM_TERMINAL_MSG2_SIZE                 65u

#define  ACM_TERMINAL_NEW_LINE                  "\n\r>> "
#define  ACM_TERMINAL_NEW_LINE_SIZE             5u

/*******************************************************************************
 ***************************  LOCAL DATA TYPE   ********************************
 ******************************************************************************/

// Terminal Menu States
SL_ENUM(terminal_state_t) {
  ACM_TERMINAL_STATE_MENU = 0u,
  ACM_TERMINAL_STATE_ECHO_1,
  ACM_TERMINAL_STATE_ECHO_N
};

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Micrium task control block
static OS_TCB tcb;

// Micrium task stack
static CPU_STK stack[TASK_STACK_SIZE];

// Universal buffer used to transmit and recevie data.
__ALIGNED(4) static uint8_t acm_terminal_buffer[ACM_TERMINAL_BUF_LEN];

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void terminal_task(void *p_arg);

/*******************************************************************************
 ***************************   HOOK FUNCTIONS  *********************************
 ******************************************************************************/

/***************************************************************************//**
 *                          sl_usbd_on_bus_event()
 *
 * @brief  USB bus events.
 ******************************************************************************/
void sl_usbd_on_bus_event(sl_usbd_bus_event_t event)
{
  switch (event) {
    case SL_USBD_EVENT_BUS_CONNECT:
      // called when usb cable is inserted in a host controller
      break;

    case SL_USBD_EVENT_BUS_DISCONNECT:
      // called when usb cable is removed from a host controller
      break;

    case SL_USBD_EVENT_BUS_RESET:
      // called when the host sends reset command
      break;

    case SL_USBD_EVENT_BUS_SUSPEND:
      // called when the host sends suspend command
      break;

    case SL_USBD_EVENT_BUS_RESUME:
      // called when the host sends wake up command
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 *                         sl_usbd_on_config_event()
 *
 * @brief  USB configuration events.
 ******************************************************************************/
void sl_usbd_on_config_event(sl_usbd_config_event_t event, uint8_t config_nbr)
{
  (void)config_nbr;

  switch (event) {
    case SL_USBD_EVENT_CONFIG_SET:
      // called when the host sets a configuration after reset
      break;

    case SL_USBD_EVENT_CONFIG_UNSET:
      // called when a configuration is unset due to reset command
      break;

    default:
      break;
  }
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS  ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void usb_device_cdc_acm_app_init(void)
{
  RTOS_ERR err;

  // Create application task
  OSTaskCreate(&tcb,
               "USB CDC ACM Termninal task",
               terminal_task,
               (void *)(uint32_t)sl_usbd_cdc_acm_acm0_number,
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

/*******************************************************************************
 **************************   LOCAL FUNCTIONS  *********************************
 ******************************************************************************/

/***************************************************************************//**
 *                          usb_terminal_task()
 *
 * @brief  USB CDC ACM terminal emulation demo task.
 *
 * @param  p_arg  Task argument pointer.
 *
 * @note   (1) This task manages the display of the terminal in according to
 *             the user's inputs.
 ******************************************************************************/
static void terminal_task(void *p_arg)
{
  bool conn = false;
  uint8_t line_state = 0;
  uint8_t ch = 0u;
  terminal_state_t state = ACM_TERMINAL_STATE_MENU;
  uint8_t cdc_acm_nbr = (uint8_t)(uint32_t)p_arg;
  uint32_t xfer_len = 0u;
  uint32_t xfer_len_dummy = 0u;
  sl_status_t status = SL_STATUS_OK;
  RTOS_ERR err;

  while (true) {
    // Wait until device is in configured state.
    status = sl_usbd_cdc_acm_is_enabled(cdc_acm_nbr, &conn);
    EFM_ASSERT(status == SL_STATUS_OK);

    status = sl_usbd_cdc_acm_get_line_control_state(cdc_acm_nbr, &line_state);
    EFM_ASSERT(status == SL_STATUS_OK);

    while ((conn != true)
           || ((line_state & SL_USBD_CDC_ACM_CTRL_DTR) == 0)) {
      OSTimeDlyHMSM(0u, 0u, 0u, 250u,
                    OS_OPT_TIME_HMSM_NON_STRICT,
                    &err);
      EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

      status = sl_usbd_cdc_acm_is_enabled(cdc_acm_nbr, &conn);
      EFM_ASSERT(status == SL_STATUS_OK);

      status = sl_usbd_cdc_acm_get_line_control_state(cdc_acm_nbr, &line_state);
      EFM_ASSERT(status == SL_STATUS_OK);
    }

    switch (state) {
      case ACM_TERMINAL_STATE_MENU:
        // Display start cursor.
        memcpy(acm_terminal_buffer,
               ACM_TERMINAL_CURSOR_START,
               ACM_TERMINAL_CURSOR_START_SIZE);

        status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                       acm_terminal_buffer,
                                       ACM_TERMINAL_CURSOR_START_SIZE,
                                       0u,
                                       &xfer_len_dummy);
        if (status != SL_STATUS_OK) {
          break;
        }

        // Display main menu.
        memcpy(acm_terminal_buffer,
               ACM_TERMINAL_MSG,
               ACM_TERMINAL_MSG_SIZE);

        status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                       acm_terminal_buffer,
                                       ACM_TERMINAL_MSG_SIZE,
                                       0u,
                                       &xfer_len_dummy);
        if (status != SL_STATUS_OK) {
          break;
        }

        // Wait for character.
        status = sl_usbd_cdc_acm_read(cdc_acm_nbr,
                                      acm_terminal_buffer,
                                      1u,
                                      0u,
                                      &xfer_len_dummy);
        if (status != SL_STATUS_OK) {
          break;
        }

        ch = acm_terminal_buffer[0u];

        // Echo back character.
        status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                       acm_terminal_buffer,
                                       1u,
                                       0u,
                                       &xfer_len_dummy);
        if (status != SL_STATUS_OK) {
          break;
        }

        // Select demo options.
        switch (ch) {
          // Echo one character.
          case '1':

            //  Clear screen.
            memcpy(acm_terminal_buffer,
                   ACM_TERMINAL_SCREEN_CLR,
                   ACM_TERMINAL_SCREEN_CLR_SIZE);

            status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                           acm_terminal_buffer,
                                           ACM_TERMINAL_SCREEN_CLR_SIZE,
                                           0u,
                                           &xfer_len_dummy);
            if (status != SL_STATUS_OK) {
              break;
            }

            // Display option 1 instructions.
            memcpy(acm_terminal_buffer,
                   ACM_TERMINAL_MSG1,
                   ACM_TERMINAL_MSG1_SIZE);

            status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                           acm_terminal_buffer,
                                           ACM_TERMINAL_MSG1_SIZE,
                                           0u,
                                           &xfer_len_dummy);
            if (status != SL_STATUS_OK) {
              break;
            }

            state = ACM_TERMINAL_STATE_ECHO_1;
            break;

          // Echon 'N' charachters.
          case '2':

            // Clear screen.
            memcpy(acm_terminal_buffer,
                   ACM_TERMINAL_SCREEN_CLR,
                   ACM_TERMINAL_SCREEN_CLR_SIZE);

            status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                           acm_terminal_buffer,
                                           ACM_TERMINAL_SCREEN_CLR_SIZE,
                                           0u,
                                           &xfer_len_dummy);
            if (status != SL_STATUS_OK) {
              break;
            }

            // Display option 2 instructions.
            memcpy(acm_terminal_buffer,
                   ACM_TERMINAL_MSG2,
                   ACM_TERMINAL_MSG2_SIZE);

            status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                           acm_terminal_buffer,
                                           ACM_TERMINAL_MSG2_SIZE,
                                           0u,
                                           &xfer_len_dummy);
            if (status != SL_STATUS_OK) {
              break;
            }

            state = ACM_TERMINAL_STATE_ECHO_N;
            break;

          default:
            break;
        }
        break;

      // 'Echo 1' state.
      case ACM_TERMINAL_STATE_ECHO_1:
        // Wait for character.
        status = sl_usbd_cdc_acm_read(cdc_acm_nbr,
                                      acm_terminal_buffer,
                                      1u,
                                      0u,
                                      &xfer_len_dummy);
        if (status != SL_STATUS_OK) {
          break;
        }

        ch = acm_terminal_buffer[0u];

        // If 'Ctrl-c' character is received, return to 'menu' state.
        if (ch == 0x03) {
          state = ACM_TERMINAL_STATE_MENU;

          // Clear screen.
          memcpy(acm_terminal_buffer,
                 ACM_TERMINAL_SCREEN_CLR,
                 ACM_TERMINAL_SCREEN_CLR_SIZE);

          status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                         acm_terminal_buffer,
                                         ACM_TERMINAL_SCREEN_CLR_SIZE,
                                         0u,
                                         &xfer_len_dummy);
          if (status != SL_STATUS_OK) {
            break;
          }
        } else {
          // Echo back character.
          status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                         acm_terminal_buffer,
                                         1u,
                                         0u,
                                         &xfer_len_dummy);
          if (status != SL_STATUS_OK) {
            break;
          }

          // Move to next line.
          memcpy(acm_terminal_buffer,
                 ACM_TERMINAL_NEW_LINE,
                 ACM_TERMINAL_NEW_LINE_SIZE);

          status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                         acm_terminal_buffer,
                                         ACM_TERMINAL_NEW_LINE_SIZE,
                                         0u,
                                         &xfer_len_dummy);
          if (status != SL_STATUS_OK) {
            break;
          }
        }
        break;

      // 'Echo N' state.
      case ACM_TERMINAL_STATE_ECHO_N:
        // Wait for N characters.
        status = sl_usbd_cdc_acm_read(cdc_acm_nbr,
                                      acm_terminal_buffer,
                                      ACM_TERMINAL_BUF_LEN,
                                      0u,
                                      &xfer_len);
        if (status != SL_STATUS_OK) {
          break;
        }

        // If 'Ctrl-c' character is received, return to 'menu' state.
        if ((xfer_len == 1u)
            && (acm_terminal_buffer[0] == 0x03)) {
          state = ACM_TERMINAL_STATE_MENU;

          // Clear screen.
          memcpy(acm_terminal_buffer,
                 ACM_TERMINAL_SCREEN_CLR,
                 ACM_TERMINAL_SCREEN_CLR_SIZE);

          status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                         acm_terminal_buffer,
                                         ACM_TERMINAL_SCREEN_CLR_SIZE,
                                         0u,
                                         &xfer_len_dummy);
          if (status != SL_STATUS_OK) {
            break;
          }
        } else {
          // Echo back characters.
          status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                         &acm_terminal_buffer[0],
                                         xfer_len,
                                         0u,
                                         &xfer_len_dummy);
          if (status != SL_STATUS_OK) {
            break;
          }

          // Move to next line.
          memcpy(acm_terminal_buffer,
                 ACM_TERMINAL_NEW_LINE,
                 ACM_TERMINAL_NEW_LINE_SIZE);

          status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                         acm_terminal_buffer,
                                         ACM_TERMINAL_NEW_LINE_SIZE,
                                         0u,
                                         &xfer_len_dummy);
          if (status != SL_STATUS_OK) {
            break;
          }
        }
        break;

      default:
        break;
    }
  }
}
