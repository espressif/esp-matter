/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_HID_H__
#define __USB_DEVICE_HID_H__

/*!
 * @addtogroup usb_device_hid_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief The class code of the HID class */
#define USB_DEVICE_CONFIG_HID_CLASS_CODE (0x03U)

/*! @brief Request code to get report of HID class. */
#define USB_DEVICE_HID_REQUEST_GET_REPORT (0x01U)
#define USB_DEVICE_HID_REQUEST_GET_REPORT_TYPE_INPUT (0x01U)
#define USB_DEVICE_HID_REQUEST_GET_REPORT_TYPE_OUPUT (0x02U)
#define USB_DEVICE_HID_REQUEST_GET_REPORT_TYPE_FEATURE (0x03U)
/*! @brief Request code to get idle of HID class. */
#define USB_DEVICE_HID_REQUEST_GET_IDLE (0x02U)
/*! @brief Request code to get protocol of HID class. */
#define USB_DEVICE_HID_REQUEST_GET_PROTOCOL (0x03U)
/*! @brief Request code to set report of HID class. */
#define USB_DEVICE_HID_REQUEST_SET_REPORT (0x09U)
/*! @brief Request code to set idle of HID class. */
#define USB_DEVICE_HID_REQUEST_SET_IDLE (0x0AU)
/*! @brief Request code to set protocol of HID class. */
#define USB_DEVICE_HID_REQUEST_SET_PROTOCOL (0x0BU)


/*! @brief Available common EVENT types in HID class callback */
typedef enum _usb_device_hid_event
{
    kUSB_DeviceHidEventSendResponse = 0x01U, /*!< Send data completed or cancelled etc*/
    kUSB_DeviceHidEventRecvResponse,         /*!< Data received or cancelled etc*/
    kUSB_DeviceHidEventGetReport,            /*!< Get report request */
    kUSB_DeviceHidEventGetIdle,              /*!< Get idle request */
    kUSB_DeviceHidEventGetProtocol,          /*!< Get protocol request */
    kUSB_DeviceHidEventSetReport,            /*!< Set report request */
    kUSB_DeviceHidEventSetIdle,              /*!< Set idle request */
    kUSB_DeviceHidEventSetProtocol,          /*!< Set protocol request */
    kUSB_DeviceHidEventRequestReportBuffer,  /*!< Get buffer to save the data of the set report request. */
} usb_device_hid_event_t;

/*!
 * @brief The device HID GET/SET report structure.
 *
 * This structure is used to pass data when the event type is kUSB_DeviceHidEventGetReport,
 * kUSB_DeviceHidEventSetReport, and kUSB_DeviceHidEventRequestReportBuffer.
 * 1. kUSB_DeviceHidEventGetReport
 *    The structure is used to save the report buffer and report length got from the application.
 *    The reportBuffer is the report data buffer address filled by the application.
 *    The reportLength is the report length.
 *    The reportType is the requested report type.
 *    The reportId is the requested report ID.
 *
 * 2. kUSB_DeviceHidEventSetReport
 *    The structure is used to pass the report data received from the host to the application.
 *    The reportBuffer is buffer address of the report data received from the host.
 *    The reportLength is the report data length.
 *    The reportType is the requested report type.
 *    The reportId is the requested report ID.
 *
 * 3. kUSB_DeviceHidEventRequestReportBuffer
 *    The structure is used to get the buffer to save the report data sent by the host.
 *    The reportBuffer is buffer address to receive to report data. It is filled by the application.
 *    The reportLength is the requested report data buffer length.
 *    The reportType is the requested report type.
 *    The reportId is the requested report ID.
 */
typedef struct _usb_device_hid_report_struct
{
    uint8_t *reportBuffer; /*!< The report buffer address */
    uint32_t reportLength; /*!< The report data length */
    uint8_t reportType;    /*!< The report type */
    uint8_t reportId;      /*!< The report ID */
} usb_device_hid_report_struct_t;

/*! @brief The HID device class status structure */
typedef struct _usb_device_hid_struct
{
    usb_device_handle handle;                       /*!< The device handle */
    usb_device_class_config_struct_t *configStruct; /*!< The configuration of the class. */
    usb_device_interface_struct_t *interfaceHandle; /*!< Current interface handle */
    uint8_t *interruptInPipeDataBuffer;             /*!< IN pipe data buffer backup when stall */
    uint32_t interruptInPipeDataLen;                /*!< IN pipe data length backup when stall  */
    uint8_t *interruptOutPipeDataBuffer;             /*!< OUT pipe data buffer backup when stall */
    uint32_t interruptOutPipeDataLen;                /*!< OUT pipe data length backup when stall  */
    uint8_t configuration;                          /*!< Current configuration */
    uint8_t interfaceNumber;                        /*!< The interface number of the class */
    uint8_t alternate;                              /*!< Current alternate setting of the interface */
    uint8_t idleRate;                               /*!< The idle rate of the HID device */
    uint8_t protocol;                               /*!< Current protocol */
    uint8_t interruptInPipeBusy;                    /*!< Interrupt IN pipe busy flag */
    uint8_t interruptOutPipeBusy;                   /*!< Interrupt OUT pipe busy flag */
    uint8_t interruptInPipeStall;                    /*!< Interrupt IN pipe stall flag */
    uint8_t interruptOutPipeStall;                   /*!< Interrupt OUT pipe stall flag */
} usb_device_hid_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the HID class.
 *
 * This function is used to initialize the HID class. This function only can be called by #USB_DeviceClassInit.
 *
 * @param[in] controllerId   The controller ID of the USB IP. See the enumeration #usb_controller_index_t.
 * @param[in] config          The class configuration information.
 * @param[out] handle          An parameter used to return pointer of the HID class handle to the caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceHidInit(uint8_t controllerId,
                                      usb_device_class_config_struct_t *config,
                                      class_handle_t *handle);

/*!
 * @brief Deinitializes the device HID class.
 *
 * The function deinitializes the device HID class. This function only can be called by #USB_DeviceClassDeinit.
 *
 * @param[in] handle The HID class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DeviceHidDeinit(class_handle_t handle);

/*!
 * @brief Handles the event passed to the HID class.
 *
 * This function handles the event passed to the HID class. This function only can be called by #USB_DeviceClassEvent.
 *
 * @param[in] handle          The HID class handle received from the usb_device_class_config_struct_t::classHandle.
 * @param[in] event           The event codes. See the enumeration usb_device_class_event_t.
 * @param[in,out] param           The parameter type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success              Free device handle successfully.
 * @retval kStatus_USB_InvalidParameter     The device handle not be found.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe is stalled by the caller.
 */
extern usb_status_t USB_DeviceHidEvent(void *handle, uint32_t event, void *param);

/*!
 * @name USB device HID class APIs
 * @{
 */

/*!
 * @brief Sends data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls #USB_DeviceSendRequest internally.
 *
 * @param[in] handle The HID class handle received from usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to hold the data need to be sent.
 * @param[in] length The data length to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The function can only be called in the same context.
 * 
 * @note The return value indicates whether the sending request is successful or not. The transfer done is notified by
 * usb_device_hid_interrupt_in.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for a specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer can begin only when the previous transfer is done (a notification is received through the
 * endpoint
 * callback).
 */
extern usb_status_t USB_DeviceHidSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

/*!
 * @brief Receives data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls #USB_DeviceRecvRequest internally.
 *
 * @param[in] handle The HID class handle received from the usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to save the received data.
 * @param[in] length The data length to be received.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The function can only be called in the same context. 
 *
 * @note The return value indicates whether the receiving request is successful or not. The transfer done is notified by
 * usb_device_hid_interrupt_out.
 * Currently, only one transfer request can be supported for a specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for a specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer can begin only when the previous transfer is done (a notification is received through the
 * endpoint
 * callback).
 */
extern usb_status_t USB_DeviceHidRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __USB_DEVICE_HID_H__ */
