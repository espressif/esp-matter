/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_DEVICE_PHDC_H_
#define _USB_DEVICE_PHDC_H_

/*!
 * @addtogroup usb_device_phdc_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief The class code of the PHDC class */
#define USB_DEVICE_CONFIG_PHDC_CLASS_CODE (0x0F)
/*! @brief The PHDC class set Meta-data message preamble feature request */
#define USB_DEVICE_PHDC_REQUEST_SET_FEATURE (0x03)
/*! @brief The PHDC class clear Meta-data message preamble feature request */
#define USB_DEVICE_PHDC_REQUEST_CLEAR_FEATURE (0x01)
/*! @brief The PHDC class get data status request */
#define USB_DEVICE_PHDC_REQUEST_GET_STATUS (0x00)
/*! @brief Available common EVENT types in PHDC class callback */
typedef enum
{
    kUSB_DevicePhdcEventInterruptInSendComplete = 0x01, /*!< Send data completed or cancelled etc*/
    kUSB_DevicePhdcEventBulkInSendComplete,             /*!< Send data completed or cancelled etc*/
    kUSB_DevicePhdcEventDataReceived,                   /*!< Data received or cancelled etc*/
    kUSB_DevicePhdcEventSetFeature,                     /*!< Set feature request */
    kUSB_DevicePhdcEventClearFeature,                   /*!< Clear feature request */
    kUSB_DevicePhdcEventGetStatus,                      /*!< Get status request */
} usb_device_phdc_event_t;

/*! @brief Definition of pipe structure. */
typedef struct _usb_device_phdc_pipe
{
    uint8_t *pipeDataBuffer;      /*!< pipe data buffer backup when stall */
    uint32_t pipeDataLen;         /*!< pipe data length backup when stall  */
    uint8_t pipeStall;            /*!< pipe is stall  */
    uint8_t ep;     /*!< The endpoint number of the pipe. */
    uint8_t isBusy; /*!< 1: The pipe is transferring packet, 0: The pipe is idle. */
} usb_device_phdc_pipe_t;

/*! @brief The PHDC device class status structure */
typedef struct _usb_device_phdc_struct
{
    usb_device_handle handle;                       /*!< The device handle */
    usb_device_class_config_struct_t *configStruct; /*!< The configuration of the class. */
    usb_device_interface_struct_t *interfaceHandle; /*!< Current interface handle */
    usb_device_phdc_pipe_t bulkIn;                  /*!< The bulk in pipe for sending data */
    usb_device_phdc_pipe_t bulkOut;                 /*!< The bulk out pipe for receiving data */
    usb_device_phdc_pipe_t interruptIn;             /*!< The interrupt in pipe for sending data */
    uint8_t configuration;                          /*!< Current configuration */
    uint8_t interfaceNumber;                        /*!< The interface number of the class */
    uint8_t alternate;                              /*!< Current alternate setting of the interface */
} usb_device_phdc_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the PHDC class.
 *
 * This function is used to initialize the PHDC class.
 *
 * @param controllerId   The controller ID of the USB IP. See the enumeration usb_controller_index_t.
 * @param config          The class configuration information.
 * @param handle          An output parameter used to return pointer of the PHDC class handle to the caller.
 *
 * @retval kStatus_USB_Success          The PHDC class is initialized successfully.
 * @retval kStatus_USB_Busy             No PHDC device handle available for allocation.
 * @retval kStatus_USB_InvalidHandle    The PHDC device handle allocation failure.
 * @retval kStatus_USB_InvalidParameter The USB device handle allocation failure.
 */
extern usb_status_t USB_DevicePhdcInit(uint8_t controllerId,
                                       usb_device_class_config_struct_t *config,
                                       class_handle_t *handle);

/*!
 * @brief Deinitializes the device PHDC class.
 *
 * The function deinitializes the device PHDC class.
 *
 * @param handle The PHDC class handle received from usb_device_class_config_struct_t::classHandle.
 *
 * @retval kStatus_USB_InvalidHandle        The device handle is not found.
 * @retval kStatus_USB_Success              The PHDC class is de-initialized successful.
 */
extern usb_status_t USB_DevicePhdcDeinit(class_handle_t handle);

/*!
 * @brief Handles the event passed to the PHDC class.
 *
 * This function handles the event passed to the PHDC class.
 *
 * @param[in] handle          The PHDC class handle received from the usb_device_class_config_struct_t::classHandle.
 * @param[in] event           The event codes. See the enumeration usb_device_class_event_t.
 * @param[in,out] param       The parameter type is determined by the event code.
 *
 * @retval kStatus_USB_Success              Free device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle is not found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid and the control pipe is stalled by the caller.
 */
extern usb_status_t USB_DevicePhdcEvent(void *handle, uint32_t event, void *param);

/*!
 * @name USB device PHDC class APIs
 * @{
 */

/*!
 * @brief Sends data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls #USB_DeviceSendRequest internally.
 *
 * @param[in] handle The PHDC class handle received from the usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to hold the data to be sent.
 * @param[in] length The data length to be sent.
 *
 * @retval kStatus_USB_InvalidHandle        The device handle is not found.
 * @retval kStatus_USB_Busy                 The previous transfer is pending.
 * @retval kStatus_USB_Success              The sending is successful.
 *
 * @note The function can only be called in the same context. 
 */
extern usb_status_t USB_DevicePhdcSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

/*!
 * @brief Receives data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls the #USB_DeviceRecvRequest internally.
 *
 * @param[in] handle The PHDC class handle received from usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to save the received data.
 * @param[in] length The data length want to be received.
 *
 * @retval kStatus_USB_InvalidHandle        The device handle is not found.
 * @retval kStatus_USB_Busy                 The previous transfer is pending.
 * @retval kStatus_USB_Success              The receiving is successful.
 *
 * @note The function can only be called in the same context. 
 */
extern usb_status_t USB_DevicePhdcRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

#if defined(__cplusplus)
}
#endif
/*! @}*/

/*! @}*/

#endif /* _USB_DEVICE_PHDC_H_ */
