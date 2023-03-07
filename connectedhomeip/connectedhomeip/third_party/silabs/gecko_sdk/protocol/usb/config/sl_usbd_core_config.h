/***************************************************************************//**
 * @file
 * @brief USBD Configuration
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

#ifndef SL_USBD_CONFIG_H
#define SL_USBD_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> USB Configuration

// <q SL_USBD_AUTO_START_USB_DEVICE> Auto-start USB device
// <i> Default: 1
// <i> If enabled, the USB device will be automatically started up,
// <i> using sl_usbd_core_start_device(), by the core task (which starts
// <i> running after kernel scheduler is ready) when the USB stack is all
// <i> initialized. You can disable this config if you want to call
// <i> sl_usbd_core_start_device() manually from your code. This might
// <i> be helpful if you do not want USB to start anytime before all
// <i> your initializations are complete, or if you want to enable/disable
// <i> USB on demand.
#define SL_USBD_AUTO_START_USB_DEVICE         1

// <q SL_USBD_MSC_SCSI_64_BIT_LBA_EN> Enable SCSI 64-Bit LBA
// <i> Default: 0
// <i> MSC SCSI Configuration for enabling 64-bit LBA support.
#define SL_USBD_MSC_SCSI_64_BIT_LBA_EN        0

// </h>

// <h> USB Core Configuration

// <h> Core Pools

// <o SL_USBD_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> The total number of configurations.
#define SL_USBD_CONFIGURATION_QUANTITY        1

// <o SL_USBD_INTERFACE_QUANTITY> Number of interfaces <1-255>
// <i> Default: 10
// <i> The total number of interfaces (for all of your USB configurations).
#define SL_USBD_INTERFACE_QUANTITY            10

// <o SL_USBD_ALT_INTERFACE_QUANTITY> Number of alternate interfaces <1-255>
// <i> Default: 10
// <i> The total number of alternate interfaces (for all of your USB configurations).
// <i> Must be equal to or bigger than SL_USBD_INTERFACE_QUANTITY
#define SL_USBD_ALT_INTERFACE_QUANTITY        10

// <o SL_USBD_INTERFACE_GROUP_QUANTITY> Number of interface groups <0-255>
// <i> Default: 20
// <i> The total number of interface groups (for all of your USB configurations).
#define SL_USBD_INTERFACE_GROUP_QUANTITY      20

// <o SL_USBD_DESCRIPTOR_QUANTITY> Number of endpoint descriptors <1-255>
// <i> Default: 20
// <i> The total number of endpoint descriptors (for all of your USB configurations).
#define SL_USBD_DESCRIPTOR_QUANTITY           20

// <o SL_USBD_STRING_QUANTITY> Number of strings <0-100>
// <i> Default: 30
// <i> The total number of strings per device.
#define SL_USBD_STRING_QUANTITY               30

// <o SL_USBD_OPEN_ENDPOINTS_QUANTITY> Number of opened endpoints <2-255>
// <i> Default: 20
// <i> The total number of opened endpoints per device.
#define SL_USBD_OPEN_ENDPOINTS_QUANTITY       20

// </h>

// <h> Core Task

// <o SL_USBD_TASK_STACK_SIZE> Stack size of USBD core task in bytes
// <i> Default: 4096
// <i> Stack size in bytes of the USBD core task.
#define SL_USBD_TASK_STACK_SIZE                      4096U

// <o SL_USBD_TASK_PRIORITY> Priority of USBD core task <8-55>
// <osPriorityLow1=> low + 1
// <osPriorityLow2=> low + 2
// <osPriorityLow3=> low + 3
// <osPriorityLow4=> low + 4
// <osPriorityLow5=> low + 5
// <osPriorityLow6=> low + 6
// <osPriorityLow7=> low + 7
// <osPriorityBelowNormal=> below normal
// <osPriorityBelowNormal1=> below normal + 1
// <osPriorityBelowNormal2=> below normal + 2
// <osPriorityBelowNormal3=> below normal + 3
// <osPriorityBelowNormal4=> below normal + 4
// <osPriorityBelowNormal5=> below normal + 5
// <osPriorityBelowNormal6=> below normal + 6
// <osPriorityBelowNormal7=> below normal + 7
// <osPriorityNormal=> normal
// <osPriorityNormal1=> normal + 1
// <osPriorityNormal2=> normal + 2
// <osPriorityNormal3=> normal + 3
// <osPriorityNormal4=> normal + 4
// <osPriorityNormal5=> normal + 5
// <osPriorityNormal6=> normal + 6
// <osPriorityNormal7=> normal + 7
// <osPriorityAboveNormal => above normal
// <osPriorityAboveNormal1=> above normal + 1
// <osPriorityAboveNormal2=> above normal + 2
// <osPriorityAboveNormal3=> above normal + 3
// <osPriorityAboveNormal4=> above normal + 4
// <osPriorityAboveNormal5=> above normal + 5
// <osPriorityAboveNormal6=> above normal + 6
// <osPriorityAboveNormal7=> above normal + 7
// <osPriorityHigh=> high
// <osPriorityHigh1=> high + 1
// <osPriorityHigh2=> high + 2
// <osPriorityHigh3=> high + 3
// <osPriorityHigh4=> high + 4
// <osPriorityHigh5=> high + 5
// <osPriorityHigh6=> high + 6
// <osPriorityHigh7=> high + 7
// <osPriorityRealtime=> realtime
// <osPriorityRealtime1=> realtime + 1
// <osPriorityRealtime2=> realtime + 2
// <osPriorityRealtime3=> realtime + 3
// <osPriorityRealtime4=> realtime + 4
// <osPriorityRealtime5=> realtime + 5
// <osPriorityRealtime6=> realtime + 6
// <osPriorityRealtime7=> realtime + 7
// <i> Default: osPriorityHigh
// <i> CMSIS-RTOS2 priority of the USBD core task.
#define SL_USBD_TASK_PRIORITY                        osPriorityHigh

// </h>

// </h>

// <h> USB CDC Configuration

// <h> CDC Pools

// <o SL_USBD_CDC_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_CDC_CLASS_INSTANCE_QUANTITY          2

// <o SL_USBD_CDC_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_CDC_CONFIGURATION_QUANTITY           1

// <o SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY> Number of subclass instances <1-255>
// <i> Default: 2
// <i> Number of subclass instances.
#define SL_USBD_CDC_ACM_SUBCLASS_INSTANCE_QUANTITY   2

// <o SL_USBD_CDC_DATA_INTERFACE_QUANTITY> Number of data interfaces <1-255>
// <i> Default: 2
// <i> Number of data interfaces.
#define SL_USBD_CDC_DATA_INTERFACE_QUANTITY          2

// </h>

// </h>

// <h> USB HID Configuration

// <h> HID Pools

// <o SL_USBD_HID_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_HID_CLASS_INSTANCE_QUANTITY          2

// <o SL_USBD_HID_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_HID_CONFIGURATION_QUANTITY           1

// <o SL_USBD_HID_REPORT_ID_QUANTITY> Number of report ids <0-255>
// <i> Default: 2
// <i> Number of report ids.
#define SL_USBD_HID_REPORT_ID_QUANTITY               2

// <o SL_USBD_HID_PUSH_POP_ITEM_QUANTITY> Number of push/pop items <0-255>
// <i> Default: 0
// <i> Number of push/pop items.
#define SL_USBD_HID_PUSH_POP_ITEM_QUANTITY           0

// </h>

// <h> HID Task

// <o SL_USBD_HID_TIMER_TASK_STACK_SIZE> Stack size of USBD HID timer task in bytes
// <i> Default: 2048
// <i> HID Timer task stack size in bytes.
#define SL_USBD_HID_TIMER_TASK_STACK_SIZE            2048

// <o SL_USBD_HID_TIMER_TASK_PRIORITY> Priority of USBD HID timer task <8-55>
// <osPriorityLow1=> low + 1
// <osPriorityLow2=> low + 2
// <osPriorityLow3=> low + 3
// <osPriorityLow4=> low + 4
// <osPriorityLow5=> low + 5
// <osPriorityLow6=> low + 6
// <osPriorityLow7=> low + 7
// <osPriorityBelowNormal=> below normal
// <osPriorityBelowNormal1=> below normal + 1
// <osPriorityBelowNormal2=> below normal + 2
// <osPriorityBelowNormal3=> below normal + 3
// <osPriorityBelowNormal4=> below normal + 4
// <osPriorityBelowNormal5=> below normal + 5
// <osPriorityBelowNormal6=> below normal + 6
// <osPriorityBelowNormal7=> below normal + 7
// <osPriorityNormal=> normal
// <osPriorityNormal1=> normal + 1
// <osPriorityNormal2=> normal + 2
// <osPriorityNormal3=> normal + 3
// <osPriorityNormal4=> normal + 4
// <osPriorityNormal5=> normal + 5
// <osPriorityNormal6=> normal + 6
// <osPriorityNormal7=> normal + 7
// <osPriorityAboveNormal => above normal
// <osPriorityAboveNormal1=> above normal + 1
// <osPriorityAboveNormal2=> above normal + 2
// <osPriorityAboveNormal3=> above normal + 3
// <osPriorityAboveNormal4=> above normal + 4
// <osPriorityAboveNormal5=> above normal + 5
// <osPriorityAboveNormal6=> above normal + 6
// <osPriorityAboveNormal7=> above normal + 7
// <osPriorityHigh=> high
// <osPriorityHigh1=> high + 1
// <osPriorityHigh2=> high + 2
// <osPriorityHigh3=> high + 3
// <osPriorityHigh4=> high + 4
// <osPriorityHigh5=> high + 5
// <osPriorityHigh6=> high + 6
// <osPriorityHigh7=> high + 7
// <osPriorityRealtime=> realtime
// <osPriorityRealtime1=> realtime + 1
// <osPriorityRealtime2=> realtime + 2
// <osPriorityRealtime3=> realtime + 3
// <osPriorityRealtime4=> realtime + 4
// <osPriorityRealtime5=> realtime + 5
// <osPriorityRealtime6=> realtime + 6
// <osPriorityRealtime7=> realtime + 7
// <i> Default: osPriorityHigh
// <i> CMSIS-RTOS2 priority of the USBD HID task.
#define SL_USBD_HID_TIMER_TASK_PRIORITY              osPriorityHigh

// </h>

// </h>

// <h> USB MSC Configuration

// <h> MSC Pools

// <o SL_USBD_MSC_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_MSC_CLASS_INSTANCE_QUANTITY          2

// <o SL_USBD_MSC_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_MSC_CONFIGURATION_QUANTITY           1

// <o SL_USBD_MSC_LUN_QUANTITY> Number of Logical Units per class instance <1-255>
// <i> Default: 2
// <i> Number of Logical Units.
#define SL_USBD_MSC_LUN_QUANTITY                     2

// <o SL_USBD_MSC_DATA_BUFFER_SIZE> Size of data buffer per class instance in bytes <1-4294967295>
// <i> Default: 512
// <i> Size of data buffer in bytes.
#define SL_USBD_MSC_DATA_BUFFER_SIZE                 512

// </h>

// </h>

// <h> USB Vendor Configuration

// <h> Vendor Pools

// <o SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY> Number of class instances <1-255>
// <i> Default: 2
// <i> Number of class instances.
#define SL_USBD_VENDOR_CLASS_INSTANCE_QUANTITY          2

// <o SL_USBD_VENDOR_CONFIGURATION_QUANTITY> Number of configurations <1-255>
// <i> Default: 1
// <i> Number of configurations.
#define SL_USBD_VENDOR_CONFIGURATION_QUANTITY           1

// </h>

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_CONFIG_H
