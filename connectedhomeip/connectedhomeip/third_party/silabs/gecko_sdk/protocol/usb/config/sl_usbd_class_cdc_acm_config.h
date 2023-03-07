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

#ifndef SL_USBD_CDC_ACM_INSTANCE_CONFIG_H
#define SL_USBD_CDC_ACM_INSTANCE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Class Configuration

// <s SL_USBD_CDC_ACM_INSTANCE_CONFIGURATIONS> Configuration(s) to add this class instance to
// <i> Default: all
// <i> Comma separated list of configuration instances (like inst0, inst1)
// <i> that this CDC ACM class instance will be attached to. You can
// <i> use "all" to attach the class to all configs, or use an empty
// <i> string if you do not want to attach the interface to any configuration.
#define SL_USBD_CDC_ACM_INSTANCE_CONFIGURATIONS            "all"

// </h>

// <h> Protocol Details

// <o SL_USBD_CDC_ACM_INSTANCE_NOTIFY_INTERVAL> Line State Notification Interval (ms)
// <i> Default: 64
// <i> Line State Notification Interval (ms).
#define SL_USBD_CDC_ACM_INSTANCE_NOTIFY_INTERVAL           64

// </h>

// <h> Call Management

// <q SL_USBD_CDC_ACM_INSTANCE_CALL_MGMT_ENABLE> Enable call management
// <i> Default: 1
// <i> If set to 1, the host is informed that this ACM instance
// <i> has call management capabilities.
#define SL_USBD_CDC_ACM_INSTANCE_CALL_MGMT_ENABLE          1

// <o SL_USBD_CDC_ACM_INSTANCE_CALL_MGMT_DCI> Call management interface
//   <1=> Over DCI
//   <0=> Over CCI
// <i> Default: 1
// <i> If set to 1 (i.e. Over DCI), a dedicated DCI interface will be created
// <i> along with the CCI interface. Otherwise, only the CCI will be created
// <i> and it will be used for both data and call management.
#define SL_USBD_CDC_ACM_INSTANCE_CALL_MGMT_DCI             1

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_CDC_ACM_INSTANCE_CONFIG_H
