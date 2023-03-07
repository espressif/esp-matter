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

#include "FreeRTOS.h"
#include "task.h"

#include "sl_usbd_core.h"
#include "sl_usbd_class_hid.h"

#include "sl_usbd_class_hid_instances.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

// Task configuration
#define TASK_STACK_SIZE         512u
#define TASK_PRIO               24u
#define TASK_DELAY_MS           100u

#define USB_HID_MOUSE_REPORT_LEN        4u

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// FreeRTOS Task handle
static TaskHandle_t task_handle;

// Current protocol selected by host.
static uint8_t usb_hid_mouse_protocol;

// Mouse report buffer.
__ALIGNED(4) static uint8_t usb_hid_mouse_report_buffer[USB_HID_MOUSE_REPORT_LEN];

// HID mouse report
// The report buffer is sent to the host when it requests the report descriptor. The report
// descriptor describes the device itself (axis, quantity of button, scroll wheel, etc) and
// how the data for each element is reported in the report.
static const uint8_t usb_hid_mouse_report_desc[] = {
  SL_USBD_HID_GLOBAL_USAGE_PAGE   + 1, SL_USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
  SL_USBD_HID_LOCAL_USAGE         + 1, SL_USBD_HID_CA_MOUSE,
  SL_USBD_HID_MAIN_COLLECTION     + 1, SL_USBD_HID_COLLECTION_APPLICATION,
  SL_USBD_HID_LOCAL_USAGE         + 1, SL_USBD_HID_CP_POINTER,
  SL_USBD_HID_MAIN_COLLECTION     + 1, SL_USBD_HID_COLLECTION_PHYSICAL,
  SL_USBD_HID_GLOBAL_USAGE_PAGE   + 1, SL_USBD_HID_USAGE_PAGE_BUTTON,
  SL_USBD_HID_LOCAL_USAGE_MIN     + 1, 0x01,
  SL_USBD_HID_LOCAL_USAGE_MAX     + 1, 0x03,
  SL_USBD_HID_GLOBAL_LOG_MIN      + 1, 0x00,
  SL_USBD_HID_GLOBAL_LOG_MAX      + 1, 0x01,
  SL_USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x03,
  SL_USBD_HID_GLOBAL_REPORT_SIZE  + 1, 0x01,
  SL_USBD_HID_MAIN_INPUT          + 1, SL_USBD_HID_MAIN_DATA | SL_USBD_HID_MAIN_VARIABLE | SL_USBD_HID_MAIN_ABSOLUTE,
  SL_USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x01,
  SL_USBD_HID_GLOBAL_REPORT_SIZE  + 1, 0x0D,
  SL_USBD_HID_MAIN_INPUT          + 1, SL_USBD_HID_MAIN_CONSTANT,
  SL_USBD_HID_GLOBAL_USAGE_PAGE   + 1, SL_USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS,
  SL_USBD_HID_LOCAL_USAGE         + 1, SL_USBD_HID_DV_X,
  SL_USBD_HID_LOCAL_USAGE         + 1, SL_USBD_HID_DV_Y,
  SL_USBD_HID_GLOBAL_LOG_MIN      + 1, 0x81,
  SL_USBD_HID_GLOBAL_LOG_MAX      + 1, 0x7F,
  SL_USBD_HID_GLOBAL_REPORT_SIZE  + 1, 0x08,
  SL_USBD_HID_GLOBAL_REPORT_COUNT + 1, 0x02,
  SL_USBD_HID_MAIN_INPUT          + 1, SL_USBD_HID_MAIN_DATA | SL_USBD_HID_MAIN_VARIABLE | SL_USBD_HID_MAIN_RELATIVE,
  SL_USBD_HID_MAIN_ENDCOLLECTION,
  SL_USBD_HID_MAIN_ENDCOLLECTION
};

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void hid_mouse_task(void *p_arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS  ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void usb_device_hid_app_init(void)
{
  BaseType_t xReturned = pdFAIL;

  // Create application task
  xReturned = xTaskCreate(hid_mouse_task,
                          "USB HID Mouse task",
                          TASK_STACK_SIZE,
                          (void *)(uint32_t)sl_usbd_hid_mouse0_number,
                          TASK_PRIO,
                          &task_handle);
  EFM_ASSERT(xReturned == pdPASS);
}

/*******************************************************************************
 **************************   LOCAL FUNCTIONS  *********************************
 ******************************************************************************/

/***************************************************************************//**
 *                          hid_mouse_task()
 *
 * @brief  Perform HID writes to host.
 *         The HID writes simulate the movement of a mouse.
 *
 * @param  p_arg  Task argument pointer. Class number in this case.
 ******************************************************************************/
static void hid_mouse_task(void *p_arg)
{
  uint8_t class_nbr = (uint8_t)(uint32_t)p_arg;
  bool x_is_pos = true;
  bool y_is_pos = true;
  bool conn;
  sl_status_t status;
  uint32_t xfer_len = 0;
  const TickType_t xDelay = pdMS_TO_TICKS(TASK_DELAY_MS);

  usb_hid_mouse_report_buffer[0u] = 0u;
  usb_hid_mouse_report_buffer[1u] = 0u;

  while (true) {
    // Wait for device connection.
    status = sl_usbd_hid_is_enabled(class_nbr, &conn);
    EFM_ASSERT(status == SL_STATUS_OK);
    while (conn != true) {
      vTaskDelay(xDelay);
      status = sl_usbd_hid_is_enabled(class_nbr, &conn);
      EFM_ASSERT(status == SL_STATUS_OK);
    }

    // Emulates back and fourth movement.
    ((int8_t *)usb_hid_mouse_report_buffer)[2u] = (x_is_pos) ? 50 : -50;
    ((int8_t *)usb_hid_mouse_report_buffer)[3u] = (y_is_pos) ? 50 : -50;

    x_is_pos = !x_is_pos;
    y_is_pos = !y_is_pos;

    // Send report.
    status = sl_usbd_hid_write_sync(class_nbr,
                                    usb_hid_mouse_report_buffer,
                                    USB_HID_MOUSE_REPORT_LEN,
                                    0u,
                                    &xfer_len);

    // Delay Task
    vTaskDelay(xDelay);
  }
}

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

/***************************************************************************//**
 *                    sl_usbd_hid_mouse0_on_enable_event()
 *
 * @brief  HID mouse0 instance Enable event.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_enable_event(void)
{
  // Called when the HID device is connected to the USB host and a
  // RESET transfer succeeded.
}

/***************************************************************************//**
 *                    sl_usbd_hid_mouse0_on_enable_event()
 *
 * @brief  HID mouse0 instance Disable event.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_disable_event(void)
{
  // Called when the HID device is disconnected to the USB host (cable removed).
}

/***************************************************************************//**
 *                sl_usbd_hid_mouse0_on_get_report_desc_event()
 *
 * @brief  Hook function to pass the HID descriptor of the mouse0 instance.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_get_report_desc_event(const uint8_t **p_report_ptr,
                                                 uint16_t       *p_report_len)
{
  // Called during the HID mouse0 instance initialization so the USB stack
  // can retrieve its HID descriptor.

  *p_report_ptr = usb_hid_mouse_report_desc;
  *p_report_len = sizeof(usb_hid_mouse_report_desc);
}

/***************************************************************************//**
 *                  sl_usbd_hid_mouse0_on_get_phy_desc_event()
 *
 * @brief  Hook function to pass the HID PHY descriptor.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_get_phy_desc_event(const uint8_t **p_report_ptr,
                                              uint16_t       *p_report_len)
{
  // Called during the HID mouse0 instance initialization so the USB stack
  // can retrieve the its HID physical descriptor.

  *p_report_ptr = NULL;
  *p_report_len = 0;
}

/***************************************************************************//**
 *                sl_usbd_hid_mouse0_on_set_output_report_event()
 *
 * @brief  Notification of a new set report received on control endpoint.
 *
 * @param  report_id     Report ID.
 *
 * @param  p_report_buf  Pointer to report buffer.
 *
 * @param  report_len    Length of report, in octets.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_set_output_report_event(uint8_t   report_id,
                                                   uint8_t  *p_report_buf,
                                                   uint16_t  report_len)
{
  // This function is called when host issues a SetReport request.
  // The application can take action in function of the report content.

  (void)report_id;
  (void)p_report_buf;
  (void)report_len;
}

/***************************************************************************//**
 *             sl_usbd_hid_mouse0_on_get_feature_report_event()
 *
 * @brief  Get HID feature report corresponding to report ID.
 *
 * @param  report_id     Report ID.
 *
 * @param  p_report_buf  Pointer to feature report buffer.
 *
 * @param  report_len    Length of report, in octets.
 *
 * @note   (1) Report ID must not be written into the feature report buffer.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_get_feature_report_event(uint8_t   report_id,
                                                    uint8_t  *p_report_buf,
                                                    uint16_t  report_len)
{
  // This function is called when host issues a GetReport(feature) request.
  // The application can provide the report to send by copying it in p_report_buf.

  (void)report_id;

  memset(p_report_buf, 0, report_len);
}

/***************************************************************************//**
 *                  sl_usbd_hid_mouse0_on_set_feature_report_event()
 *
 * @brief  Set HID feature report corresponding to report ID.
 *
 * @param  report_id     Report ID.
 *
 * @param  p_report_buf  Pointer to feature report buffer.
 *
 * @param  report_len    Length of report, in octets.
 *
 * @note   (1) Report ID is not present in the feature report buffer.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_set_feature_report_event(uint8_t   report_id,
                                                    uint8_t  *p_report_buf,
                                                    uint16_t  report_len)
{
  // This function is called when host issues a SetReport(Feature) request.
  // The application can take action in function of the provided report in p_report_buf.

  (void)report_id;
  (void)p_report_buf;
  (void)report_len;
}

/***************************************************************************//**
 *                    sl_usbd_hid_mouse0_on_get_protocol_event()
 *
 * @brief  Retrieve active protocol: BOOT or REPORT protocol.
 *
 * @param  p_protocol Pointer to variable that will receive the protocol type.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_get_protocol_event(uint8_t *p_protocol)
{
  // This function is called when host issues a GetProtocol request.
  // The application should return the current protocol.

  *p_protocol = usb_hid_mouse_protocol;
}

/***************************************************************************//**
 *                  sl_usbd_hid_mouse0_on_set_protocol_event()
 *
 * @brief  Store active protocol: BOOT or REPORT protocol.
 *
 * @param  protocol   Protocol.
 ******************************************************************************/
void sl_usbd_hid_mouse0_on_set_protocol_event(uint8_t  protocol)
{
  // This function is called when host issues a SetProtocol request.
  // The application should apply the new protocol.

  usb_hid_mouse_protocol = protocol;
}
