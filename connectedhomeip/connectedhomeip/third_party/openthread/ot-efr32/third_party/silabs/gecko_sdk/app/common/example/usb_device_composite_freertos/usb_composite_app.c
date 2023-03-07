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
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_cdc.h"
#include "sl_usbd_class_cdc_acm.h"
#include "sl_usbd_class_hid.h"

#include "sl_usbd_class_cdc_acm_instances.h"
#include "sl_usbd_class_hid_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Task configuration
#define TASK_STACK_SIZE         512u
#define TASK_PRIO               24u
#define TASK_DELAY_MS           250u

// Menu Message and Length
#define  ACM_TERMINAL_BUF_LEN                   128u

#define  ACM_TERMINAL_CURSOR_START              "\033[H"
#define  ACM_TERMINAL_CURSOR_START_SIZE         3u

#define  ACM_TERMINAL_SCREEN_CLR                "\033[2J\033[H"
#define  ACM_TERMINAL_SCREEN_CLR_SIZE           7u

#define  ACM_TERMINAL_MSG                       "\r\n===== USB Composite Device Demo ======" \
                                                "\r\n"                                       \
                                                "\r\n"                                       \
                                                "To move mouse pointer, type\r\n"            \
                                                ">> mouse move x y\r\n"                      \
                                                "e. g.\r\n"                                  \
                                                ">> mouse move 100 100\r\n"                  \

#define  ACM_TERMINAL_MSG_SIZE                  strlen(ACM_TERMINAL_MSG)

#define  ACM_TERMINAL_ERROR_MESSAGE             "\r\nError\r\n"
#define  ACM_TERMINAL_ERROR_MESSAGE_SIZE        strlen(ACM_TERMINAL_ERROR_MESSAGE)

#define  ACM_TERMINAL_NEW_LINE                  "\n\r>> "
#define  ACM_TERMINAL_NEW_LINE_SIZE             5u

#define  ACM_TERMINAL_RESPONSE                  "\r\nYou moved the mouse %d %d\r\n"

#define  ACM_TERMINAL_MOUSE_COMMAND             "mouse move %d %d"

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
uint8_t mouse_report_desc_ptr[] = {
  SL_USBD_HID_GLOBAL_USAGE_PAGE           + 1, SL_USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
  SL_USBD_HID_LOCAL_USAGE                 + 1, SL_USBD_HID_CA_MOUSE,
  SL_USBD_HID_MAIN_COLLECTION             + 1, SL_USBD_HID_COLLECTION_APPLICATION,
  SL_USBD_HID_LOCAL_USAGE                 + 1, SL_USBD_HID_CP_POINTER,
  SL_USBD_HID_MAIN_COLLECTION             + 1, SL_USBD_HID_COLLECTION_PHYSICAL,
  SL_USBD_HID_GLOBAL_USAGE_PAGE           + 1, SL_USBD_HID_USAGE_PAGE_BUTTON,
  SL_USBD_HID_LOCAL_USAGE_MIN             + 1, 0x01,
  SL_USBD_HID_LOCAL_USAGE_MAX             + 1, 0x03,
  SL_USBD_HID_GLOBAL_LOG_MIN              + 1, 0x00,
  SL_USBD_HID_GLOBAL_LOG_MAX              + 1, 0x01,
  SL_USBD_HID_GLOBAL_REPORT_COUNT         + 1, 0x03,
  SL_USBD_HID_GLOBAL_REPORT_SIZE          + 1, 0x01,
  SL_USBD_HID_MAIN_INPUT                  + 1, SL_USBD_HID_MAIN_DATA | SL_USBD_HID_MAIN_VARIABLE | SL_USBD_HID_MAIN_ABSOLUTE,
  SL_USBD_HID_GLOBAL_REPORT_COUNT         + 1, 0x01,
  SL_USBD_HID_GLOBAL_REPORT_SIZE          + 1, 0x0D,
  SL_USBD_HID_MAIN_INPUT                  + 1, SL_USBD_HID_MAIN_CONSTANT,
  SL_USBD_HID_GLOBAL_USAGE_PAGE           + 1, SL_USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
  SL_USBD_HID_LOCAL_USAGE                 + 1, SL_USBD_HID_DV_X,
  SL_USBD_HID_LOCAL_USAGE                 + 1, SL_USBD_HID_DV_Y,
  SL_USBD_HID_GLOBAL_LOG_MIN              + 1, 0x81,
  SL_USBD_HID_GLOBAL_LOG_MAX              + 1, 0x7F,
  SL_USBD_HID_GLOBAL_REPORT_SIZE          + 1, 0x08,
  SL_USBD_HID_GLOBAL_REPORT_COUNT         + 1, 0x02,
  SL_USBD_HID_MAIN_INPUT                  + 1, SL_USBD_HID_MAIN_DATA | SL_USBD_HID_MAIN_VARIABLE | SL_USBD_HID_MAIN_RELATIVE,
  SL_USBD_HID_MAIN_ENDCOLLECTION,
  SL_USBD_HID_MAIN_ENDCOLLECTION
};

uint8_t *report_desc_ptr = mouse_report_desc_ptr;
uint16_t report_desc_len = sizeof(mouse_report_desc_ptr);

uint8_t *phy_desc_ptr = NULL;
uint16_t phy_desc_len = 0;

/*******************************************************************************
 ***************************  LOCAL DATA TYPE   ********************************
 ******************************************************************************/

// Terminal Menu States
SL_ENUM(terminal_state_t) {
  ACM_TERMINAL_STATE_MENU = 0u,
  ACM_TERMINAL_READ_INPUT
};

typedef struct {
  int x;
  int y;
} queue_item_t;

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// FreeRTOS handles
static TaskHandle_t terminal_task_handle;
static TaskHandle_t mouse_task_handle;
static QueueHandle_t queue_handle;

// Universal buffers used to transmit and receive data. Buffers must be aligned
__ALIGNED(4) static uint8_t acm_terminal_output_buffer[ACM_TERMINAL_BUF_LEN];
__ALIGNED(4) static uint8_t acm_terminal_input_buffer[ACM_TERMINAL_BUF_LEN];

__ALIGNED(4) static uint8_t hid_mouse_report_buffer[4];

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void terminal_task(void *p_arg);
static void mouse_task(void *p_arg);
static bool parse_input(uint8_t *input, int *x, int *y);
static void queue_mouse_command(int x, int y);

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
void usb_device_composite_app_init(void)
{
  BaseType_t xReturned = pdFAIL;

  queue_handle = xQueueCreate(2, sizeof(queue_item_t));
  EFM_ASSERT(queue_handle != NULL);

  // Create application task
  xReturned = xTaskCreate(terminal_task,
                          "USB CDC ACM Terminal task",
                          TASK_STACK_SIZE,
                          (void *)(uint32_t)sl_usbd_cdc_acm_acm0_number,
                          TASK_PRIO,
                          &terminal_task_handle);
  EFM_ASSERT(xReturned == pdPASS);

  // Create application task
  xReturned = xTaskCreate(mouse_task,
                          "USB HID Mouse task",
                          TASK_STACK_SIZE,
                          (void *)(uint32_t)sl_usbd_hid_hid0_number,
                          TASK_PRIO + 1,
                          &mouse_task_handle);
  EFM_ASSERT(xReturned == pdPASS);
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS  *********************************
 ******************************************************************************/

/***************************************************************************//**
 *                          parse_input()
 *
 * @brief  parse user input
 *
 * @param  input  User input.
 * @param  x      Pointer for parsed x-axis value.
 * @param  x      Pointer for parsed y-axis value.
 *
 * @return true if parsing succeeds, otherwise false
 *
 * @note   This function accepts user input and returns
 *         parsed values for x- and y-axes.
 ******************************************************************************/
static bool parse_input(uint8_t *input, int *x, int *y)
{
  if (sscanf((const char *)input, ACM_TERMINAL_MOUSE_COMMAND, x, y) == 2) {
    return true;
  }
  return false;
}

/***************************************************************************//**
 *                          queue_mouse_command()
 *
 * @brief  Queue user mouse command
 *
 * @param  x      Displacement in x-axis.
 * @param  x      Displacement in y-axis.
 *
 * @note   This function queues the mouse command to send to mouse_task()
 ******************************************************************************/
static void queue_mouse_command(int x, int y)
{
  BaseType_t xReturned;
  queue_item_t item;

  item.x = x;
  item.y = y;

  xReturned = xQueueSendToBack(queue_handle, &item, 0);
  EFM_ASSERT(xReturned == pdTRUE);
}

/***************************************************************************//**
 *                          terminal_task()
 *
 * @brief  USB CDC ACM terminal emulation demo task.
 *
 * @param  p_arg  Task argument pointer.
 *
 * @note   This task accepts user inputs through the ACM terminal.
 *
 ******************************************************************************/
static void terminal_task(void *p_arg)
{
  bool conn = false;
  uint8_t line_state = 0;
  terminal_state_t state = ACM_TERMINAL_STATE_MENU;
  uint8_t cdc_acm_nbr = (uint8_t)(uint32_t)p_arg;
  uint32_t xfer_len = 0u;
  uint32_t xfer_len_dummy = 0u;
  sl_status_t status = SL_STATUS_OK;
  __ALIGNED(4) uint8_t c = 0;
  uint8_t index = 0;
  int x, y;
  char response[64];

  const TickType_t xDelay = pdMS_TO_TICKS(TASK_DELAY_MS);

  while (1) {
    // Wait until device is in configured state.
    status = sl_usbd_cdc_acm_is_enabled(cdc_acm_nbr, &conn);
    EFM_ASSERT(status == SL_STATUS_OK);

    status = sl_usbd_cdc_acm_get_line_control_state(cdc_acm_nbr, &line_state);
    EFM_ASSERT(status == SL_STATUS_OK);

    while ((conn != true)
           || ((line_state & SL_USBD_CDC_ACM_CTRL_DTR) == 0)) {
      // Delay Task
      vTaskDelay(xDelay);

      status = sl_usbd_cdc_acm_is_enabled(cdc_acm_nbr, &conn);
      EFM_ASSERT(status == SL_STATUS_OK);

      status = sl_usbd_cdc_acm_get_line_control_state(cdc_acm_nbr, &line_state);
      EFM_ASSERT(status == SL_STATUS_OK);
    }

    switch (state) {
      case ACM_TERMINAL_STATE_MENU:
        // Display main menu.
        memcpy(acm_terminal_output_buffer,
               ACM_TERMINAL_MSG,
               ACM_TERMINAL_MSG_SIZE);

        status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                       acm_terminal_output_buffer,
                                       ACM_TERMINAL_MSG_SIZE,
                                       0u,
                                       &xfer_len_dummy);
        if (status != SL_STATUS_OK) {
          break;
        }
        state = ACM_TERMINAL_READ_INPUT;
        break;

      // Read mouse input
      case ACM_TERMINAL_READ_INPUT:
        memcpy(acm_terminal_output_buffer,
               ACM_TERMINAL_NEW_LINE,
               ACM_TERMINAL_NEW_LINE_SIZE);
        sl_usbd_cdc_acm_write(cdc_acm_nbr,
                              acm_terminal_output_buffer,
                              ACM_TERMINAL_NEW_LINE_SIZE,
                              0u,
                              &xfer_len_dummy);

        while (1) {
          // read one character at a time. Command is parsed
          // when end of line is reached.
          status = sl_usbd_cdc_acm_read(cdc_acm_nbr,
                                        &c,
                                        1,
                                        0u,
                                        &xfer_len);

          if (status == SL_STATUS_OK && xfer_len > 0) {
            // Wait for endline.
            if (c == '\r' || c == '\n') {
              acm_terminal_input_buffer[index] = '\0';
              index = 0;
              if (parse_input(acm_terminal_input_buffer, &x, &y)) {
                queue_mouse_command(x, y);
                snprintf(response, sizeof(acm_terminal_output_buffer),
                         ACM_TERMINAL_RESPONSE, x, y);
                memcpy(acm_terminal_output_buffer,
                       response,
                       strlen(response));
                sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                      acm_terminal_output_buffer,
                                      strlen(response),
                                      0u,
                                      &xfer_len_dummy);
                memcpy(acm_terminal_output_buffer,
                       ACM_TERMINAL_NEW_LINE,
                       ACM_TERMINAL_NEW_LINE_SIZE);
                sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                      acm_terminal_output_buffer,
                                      ACM_TERMINAL_NEW_LINE_SIZE,
                                      0u,
                                      &xfer_len_dummy);
              } else {
                memcpy(acm_terminal_output_buffer,
                       ACM_TERMINAL_ERROR_MESSAGE,
                       ACM_TERMINAL_ERROR_MESSAGE_SIZE);
                sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                      acm_terminal_output_buffer,
                                      ACM_TERMINAL_ERROR_MESSAGE_SIZE,
                                      0u,
                                      &xfer_len_dummy);
                state = ACM_TERMINAL_STATE_MENU;
                break;
              }
            } else {
              if (index < ACM_TERMINAL_BUF_LEN - 1) {
                acm_terminal_input_buffer[index] = c;
                index++;
              }
              // local echo
              status = sl_usbd_cdc_acm_write(cdc_acm_nbr,
                                             &c,
                                             1,
                                             0u,
                                             &xfer_len_dummy);
              if (status != SL_STATUS_OK) {
                break;
              }
            }
          }
        }
        break;

      default:
        break;
    }
  }
}

/***************************************************************************//**
 *                          mouse_task()
 *
 * @brief  USB HID mouse emulation demo task.
 *
 * @param  p_arg  Task argument pointer.
 *
 * @note   This task reads command from the queue and writes through the
 *         USB interface.
 ******************************************************************************/
static void mouse_task(void *p_arg)
{
  uint8_t hid_nbr = (uint8_t)(uint32_t)p_arg;
  sl_status_t status;
  queue_item_t item;
  BaseType_t xReturned;
  uint32_t xfer_len;

  while (1) {
    xReturned = xQueueReceive(queue_handle, &item, portMAX_DELAY);
    EFM_ASSERT(xReturned == pdTRUE);

    ((int8_t *)hid_mouse_report_buffer)[2u] = item.x;
    ((int8_t *)hid_mouse_report_buffer)[3u] = item.y;

    status = sl_usbd_hid_write_sync(hid_nbr,
                                    hid_mouse_report_buffer,
                                    4,
                                    0u,
                                    &xfer_len);
    EFM_ASSERT(status == SL_STATUS_OK);
  }
}
