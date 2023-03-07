/***************************************************************************//**
 * @file
 * @brief USBD CDC ACM Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_USB_CONFIGURATION_INSTANCE_CONFIG_H
#define SL_USB_CONFIGURATION_INSTANCE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Configuration Settings

// <s SL_USB_CONFIGURATION_INSTANCE_NAME> Configuration Name
// <i> Default: "Main Configuration"
// <i> This creates a string descriptor that the USB host
// <i> can use to retrieve the name of this USB configuration descriptor.
// <i> It can be scanned with "sudo lsusb -vv" on Linux. It appears next
// <i> to iConfiguration field.
#define SL_USB_CONFIGURATION_INSTANCE_NAME              "Main Configuration"

// <o SL_USB_CONFIGURATION_INSTANCE_POWER_SOURCE> Power Source
// <0=> Bus-Powered
// <1=> Self-Powered
// <i> Default: 1
// <i> Indicates whether the device will be powered using USB bus or
// <i> or using a self-power source (like battery or a debugger)
// <i> if the host configures the device using this configuration.
#define SL_USB_CONFIGURATION_INSTANCE_POWER_SOURCE      1

// <q SL_USB_CONFIGURATION_INSTANCE_REMOTE_WAKEUP> Enable Remote Wakeup
// <i> Default: 0
// <i> Enables or disables remote wakeup feature.
#define SL_USB_CONFIGURATION_INSTANCE_REMOTE_WAKEUP    0

// <o SL_USB_CONFIGURATION_INSTANCE_MAXIMUM_POWER> Maximum Power (mA)
// <100=> 100 mA
// <500=> 500 mA
// <i> Default: 100
// <i> Specifies the maximum current that the device will draw.
// <i> Most USB devices consume 100mA at most, but the USB standard
// <i> allows the device to consume up to 500mA. When the host
// <i> operating system scans this value, it will configure the USB port
// <i> on the host controller to allow the device to consume the
// <i> configured current.
#define SL_USB_CONFIGURATION_INSTANCE_MAXIMUM_POWER     100

// </h>

// <<< end of configuration section >>>
#endif // SL_USB_CONFIGURATION_INSTANCE_CONFIG_H
