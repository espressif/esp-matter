/***************************************************************************//**
 * @file
 * @brief USBD MSC Configuration
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

#ifndef SL_USBD_MSC_SCSI_INSTANCE_CONFIG_H
#define SL_USBD_MSC_SCSI_INSTANCE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Class Configuration

// <s SL_USBD_MSC_SCSI_INSTANCE_CONFIGURATIONS> Configuration(s) to add this class instance to
// <i> Default: all
// <i> Comma separated list of configuration instances (like inst0, inst1)
// <i> that this MSC SCSI class instance will be attached to. You can
// <i> use "all" to attach the class to all configs, or use an empty
// <i> string if you do not want to attach the interface to any configuration.
#define SL_USBD_MSC_SCSI_INSTANCE_CONFIGURATIONS        "all"

// </h>

// <h> MSC task

// <o SL_USBD_MSC_SCSI_INSTANCE_TASK_STACK_SIZE> MSC task stack size in bytes
// <i> Default: 2048
// <i> MSC task stack size in bytes.
#define SL_USBD_MSC_SCSI_INSTANCE_TASK_STACK_SIZE       2048

// <o SL_USBD_MSC_SCSI_INSTANCE_TASK_PRIORITY> Priority of USBD MSC task <8-55>
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
// <i> CMSIS-RTOS2 priority of the USBD MSC task.
#define SL_USBD_MSC_SCSI_INSTANCE_TASK_PRIORITY         osPriorityHigh

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_MSC_SCSI_INSTANCE_CONFIG_H
