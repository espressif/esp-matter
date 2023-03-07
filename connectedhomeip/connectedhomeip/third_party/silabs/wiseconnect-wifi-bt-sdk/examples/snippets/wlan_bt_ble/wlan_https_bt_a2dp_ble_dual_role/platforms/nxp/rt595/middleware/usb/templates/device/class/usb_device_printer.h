/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_PRINTER_H__
#define __USB_DEVICE_PRINTER_H__

/*!
 * @addtogroup usb_device_printer_drv
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief The class code of the printer class */
#define USB_DEVICE_CONFIG_PRINTER_CLASS_CODE (0x07)

/*! @brief class-specific request GET_DEVICE_ID */
#define USB_DEVICE_PRINTER_GET_DEVICE_ID (0x00U)
/*! @brief class-specific request GET_PORT_STATUS */
#define USB_DEVICE_PRINTER_GET_PORT_STATUS (0x01U)
/*! @brief class-specific request SOFT_RESET */
#define USB_DEVICE_PRINTER_SOFT_RESET (0x02U)

/*! @brief Paper empty bit mask for GET_PORT_STATUS */
#define USB_DEVICE_PRINTER_PORT_STATUS_PAPER_EMPTRY_MASK (0x20U)
/*! @brief Select bit mask for GET_PORT_STATUS */
#define USB_DEVICE_PRINTER_PORT_STATUS_SELECT_MASK (0x10U)
/*! @brief Error bit mask for GET_PORT_STATUS */
#define USB_DEVICE_PRINTER_PORT_STATUS_NOT_ERROR_MASK (0x08U)

#define USB_DEVICE_PRINTER_PORT_STATUS_DEFAULT_VALUE \
    (USB_DEVICE_PRINTER_PORT_STATUS_SELECT_MASK | USB_DEVICE_PRINTER_PORT_STATUS_NOT_ERROR_MASK)

/*! @brief Available common EVENT types in printer class callback */
typedef enum _usb_device_printer_event
{
    kUSB_DevicePrinterEventRecvResponse = 0x01U, /*!< Data received or cancelled etc*/
    kUSB_DevicePrinterEventSendResponse,         /*!< Data send done or cancelled etc */
    kUSB_DevicePrinterEventGetDeviceId,          /*!< Get device ID request */
    kUSB_DevicePrinterEventGetPortStatus,        /*!< Get port status request */
    kUSB_DevicePrinterEventSoftReset,            /*!< Soft reset request */
} usb_device_printer_event_t;

typedef struct _usb_device_printer_class_request
{
    uint8_t *buffer;          /*!< The class specific request buffer address */
    uint32_t length;          /*!< The class specific request buffer length */
    uint8_t interface;        /*!< The class specific request interface index */
    uint8_t alternateSetting; /*!< GET_DEVICE_ID request alternate setting */
    uint8_t configIndex;      /*!< GET_DEVICE_ID request config index */
} usb_device_printer_class_request_t;

/*! @brief The printer device class instance structure */
typedef struct _usb_device_printer_struct
{
    usb_device_handle deviceHandle;                 /*!< The device handle */
    usb_device_class_config_struct_t *classConfig;  /*!< The configuration of the class. */
    usb_device_interface_struct_t *interfaceHandle; /*!< Current interface handle */
    uint8_t *bulkInPipeDataBuffer;             /*!< IN pipe data buffer backup when stall */
    uint32_t bulkInPipeDataLen;                /*!< IN pipe data length backup when stall  */
    uint8_t *bulkOutPipeDataBuffer;             /*!< OUT pipe data buffer backup when stall */
    uint32_t bulkOutPipeDataLen;                /*!< OUT pipe data length backup when stall  */
    uint8_t configuration;                          /*!< Current configuration */
    uint8_t interfaceNumber;                        /*!< Interface number in the device descriptor */
    uint8_t alternate;                              /*!< Interface alternate value */
    uint8_t bulkInBusy;                             /*!< BULK IN pipe busy flag */
    uint8_t bulkOutBusy;                            /*!< BULK OUT pipe busy flag */
    uint8_t bulkInPipeStall;                    /*!< bulk IN pipe stall flag */
    uint8_t bulkOutPipeStall;                   /*!< bulk OUT pipe stall flag */
} usb_device_printer_struct_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initializes the printer class.
 *
 * This function is used to initialize the printer class. This function only can be called by #USB_DeviceClassInit.
 *
 * @param[in] controllerId   The controller ID of the USB IP. See the enumeration #usb_controller_index_t.
 * @param[in] config         The class configuration information.
 * @param[out] handle        A parameter used to return a pointer of the printer class handle to the caller.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DevicePrinterInit(uint8_t controllerId,
                                          usb_device_class_config_struct_t *config,
                                          class_handle_t *handle);

/*!
 * @brief De-initializes the device printer class.
 *
 * The function de-initializes the device printer class. This function only can be called by #USB_DeviceClassDeinit.
 *
 * @param[in] handle The printer class handle got from usb_device_class_config_struct_t::classHandle.
 *
 * @return A USB error code or kStatus_USB_Success.
 */
extern usb_status_t USB_DevicePrinterDeinit(class_handle_t handle);

/*!
 * @brief Handles the event passed to the printer class.
 *
 * This function handles the event passed to the printer class. This function only can be called by
 * #USB_DeviceClassEvent.
 *
 * @param[in] handle          The printer class handle received from the usb_device_class_config_struct_t::classHandle.
 * @param[in] event           The event codes. See the enumeration usb_device_class_event_t.
 * @param[in,out] param       The parameter type is determined by the event code.
 *
 * @return A USB error code or kStatus_USB_Success.
 * @retval kStatus_USB_Success              Process event successfully.
 * @retval kStatus_USB_InvalidHandle        The device handle or parameter is invalid.
 * @retval kStatus_USB_InvalidRequest       The request is invalid, and the control pipe is stalled by the caller.
 */
extern usb_status_t USB_DevicePrinterEvent(void *handle, uint32_t event, void *param);

/*!
 * @name USB device printer class APIs
 * @{
 */

/*!
 * @brief Sends data through a specified endpoint.
 *
 * The function is used to send data through a specified endpoint.
 * The function calls #USB_DeviceSendRequest internally.
 *
 * @param[in] handle The printer class handle received from usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to hold the data need to be sent.
 * @param[in] length The data length to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The function can only be called in the same context. 
 *
 * @note The return value indicates whether the sending request is successful or not.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for a specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer can begin only when the previous transfer is done (a notification is received through the
 * callback).
 */
extern usb_status_t USB_DevicePrinterSend(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

/*!
 * @brief Receives data through a specified endpoint.
 *
 * The function is used to receive data through a specified endpoint.
 * The function calls #USB_DeviceSendRequest internally.
 *
 * @param[in] handle The printer class handle received from usb_device_class_config_struct_t::classHandle.
 * @param[in] ep     Endpoint index.
 * @param[in] buffer The memory address to hold the data need to be sent.
 * @param[in] length The data length to be sent.
 *
 * @return A USB error code or kStatus_USB_Success.
 *
 * @note The function can only be called in the same context. 
 *
 * @note The return value indicates whether the sending request is successful or not.
 * Currently, only one transfer request can be supported for one specific endpoint.
 * If there is a specific requirement to support multiple transfer requests for a specific endpoint, the application
 * should implement a queue in the application level.
 * The subsequent transfer can begin only when the previous transfer is done (a notification is received through the
 * callback).
 */
extern usb_status_t USB_DevicePrinterRecv(class_handle_t handle, uint8_t ep, uint8_t *buffer, uint32_t length);

/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* __USB_DEVICE_PRINTER_H__ */
