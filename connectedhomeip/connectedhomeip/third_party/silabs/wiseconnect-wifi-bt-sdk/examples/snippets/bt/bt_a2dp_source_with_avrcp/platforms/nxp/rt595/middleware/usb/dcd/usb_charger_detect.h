/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_DEVICE_CONFIG_CHARGER_DETECT_H__
#define __USB_DEVICE_CONFIG_CHARGER_DETECT_H__

typedef enum _usb_device_charger_detect_type
{   
    kUSB_DcdTimeOut = 0x0U,        /*!< Dcd detect result is timeout */
    kUSB_DcdUnknownType,           /*!< Dcd detect result is unknown type */
    kUSB_DcdError,                 /*!< Dcd detect result is error*/
    kUSB_DcdSDP,                   /*!< The SDP facility is detected */
    kUSB_DcdCDP,                   /*!< The CDP facility is detected */
    kUSB_DcdDCP,                   /*!< The DCP facility is detected */
}usb_device_charger_detect_type_t;

#endif /* __USB_DEVICE_CONFIG_CHARGER_DETECT_H__ */

