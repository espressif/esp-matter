/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_HSDCD_H__
#define __USB_DEVICE_HSDCD_H__
#include "fsl_common.h"
#include "fsl_device_registers.h"

/*!
 * @addtogroup usb_device_hsdcd_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/******************* Macro definitions ***************/
#include "usb_charger_detect.h"
 /*! @brief USB error code */
typedef enum _usb_hsdcd_status
{
    kStatus_hsdcd_Success = 0x00U, /*!< Success */
    kStatus_hsdcd_Error,           /*!< Failed */
} usb_hsdcd_status_t;
/*!
 * @brief Device dcd callback function typedef.
 *
 * This callback function is used to notify the upper layer that the device status has changed.
 * This callback pointer is passed by calling API dcd Init function.
 *
 * @param handle          The device handle. It equals the value returned from #USB_DCD_Init.
 * @param callbackEvent  The callback event type. See enumeration #usb_device_event_t.
 * @param eventParam     The event parameter for this callback. The parameter type is determined by the callback event.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
typedef usb_hsdcd_status_t (*usb_hsdcd_callback_t)(void *handle, uint32_t callbackEvent, void *eventParam);
typedef void *  usb_hsdcd_handle;
/*! @brief dcd configuration structure */
typedef struct _usb_hsdcd_config_struct
{
    usb_hsdcd_callback_t dcdCallback;             /*!< DCD base address*/
    void *dcdCallbackParam;                       /*!< DCD callback parameter*/
} usb_hsdcd_config_struct_t;

/*! @brief Available common EVENT types in device callback */
typedef enum _usb_hsdcd_control
{
    kUSB_DeviceHSDcdRun = 1U,    /*!< USB dcd dectect start */
    kUSB_DeviceHSDcdStop,        /*!< USB dcd module reset */
    kUSB_DeviceHSDcdEnable,      /*!< Enable USB dcd dectect module */
    kUSB_DeviceHSDcdDisable,     /*!< USB dcd module moudle */
    kUSB_HostHSDcdSetType,       /*!< set host dcd type */
} usb_hsdcd_control_t;

/*!
 * @name USB device hs dcd functions
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the USB dcd instance.
 *
 * This function initializes the USB dcd controller module specified by the base.
 *
 * @param[in] base      Pointer of the dcd register base.
 * @param[in] config    Pointer of the dcd configuration.
 * @param[out] dcdHandle   an output parameter, return the pointer of the dcd moudle handle to caller.
 *
 * @return A error code or kStatus_hsdcd_Success.
 */
usb_hsdcd_status_t USB_HSDCD_Init(USBHSDCD_Type *base, usb_hsdcd_config_struct_t *config, usb_hsdcd_handle *dcdHandle);

/*!
 * @brief Deinitializes the USB dcd instance.
 *
 * This function Deinitializes the USB dcd module specified by the dcd handle.
 *
 * @param[in] handle      The dcd peripheral handle pointer.
 *
 * @return A dcd error code or kStatus_hsdcd_Success.
 */

usb_hsdcd_status_t USB_HSDCD_Deinit(usb_hsdcd_handle handle);

/*!
 * @brief Control the status of the selected item.
 *
 * This function is used to control the status of the selected item..
 *
 * @param handle                 The dcd handle. It equals the value returned from USB_DCD_Init.
 * @param type                   The control type, please refer to the enumeration usb_dcd_event_t.
 * @param param                  The param type is determined by the selected item. Or the param is NULL pointer.
 *
 * @retval kStatus_dcd_Success              control the status successfully.
 * @retval kStatus_USB_Error                control the status failed .
 *                                          
 */
usb_hsdcd_status_t USB_HSDCD_Control(usb_hsdcd_handle handle, usb_hsdcd_control_t type, void *param);
/*!
 * @brief Device dcd ISR function.
 *
 * The function is the dcd interrupt service routine.
 *
 * @param[in] deviceHandle The device handle got from #USB_DeviceHSDCD_Init.
 */
extern void USB_HSDcdIsrFunction(usb_hsdcd_handle handle);
/*! @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* __USB_DEVICE_HSDCD_H__ */

