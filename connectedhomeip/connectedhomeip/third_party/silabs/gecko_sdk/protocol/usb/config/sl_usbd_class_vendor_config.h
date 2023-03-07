/***************************************************************************//**
 * @file
 * @brief USBD Vendor Configuration
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

#ifndef SL_USBD_VENDOR_INSTANCE_CONFIG_H
#define SL_USBD_VENDOR_INSTANCE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Class Configuration

// <s SL_USBD_VENDOR_INSTANCE_CONFIGURATIONS> Configuration(s) to add this class instance to
// <i> Default: all
// <i> Comma separated list of configuration instances (like inst0, inst1)
// <i> that this vendor class instance will be attached to. You can
// <i> use "all" to attach the class to all configs, or use an empty
// <i> string if you do not want to attach the interface to any configuration.
#define SL_USBD_VENDOR_INSTANCE_CONFIGURATIONS          "all"

// </h>

// <h> Protocol Details

// <q SL_USBD_VENDOR_INSTANCE_INTERRUPT_ENDPOINTS> Add interrupt endpoints
// <i> Default: 0
// <i> Specifies whether we should add IN and OUT endpoints to this
// <i> vendor class interface.
#define SL_USBD_VENDOR_INSTANCE_INTERRUPT_ENDPOINTS     0

// <o SL_USBD_VENDOR_INSTANCE_INTERVAL> Endpoint interval
//   <1=> 1ms
//   <2=> 2ms
//   <4=> 4ms
//   <8=> 8ms
//   <16=> 16ms
//   <32=> 32ms
//   <64=> 64ms
//   <128=> 128ms
//   <256=> 256ms
//   <512=> 512ms
//   <1024=> 1024ms
//   <2048=> 2048ms
//   <4096=> 4096ms
//   <8192=> 8192ms
//   <16384=> 16384ms
//   <32768=> 32768ms
// <i> Default: 2
// <i> Polling interval for input/output transfers, in milliseconds.
// <i> It must be a power of 2.
#define SL_USBD_VENDOR_INSTANCE_INTERVAL                2

// </h>

// <<< end of configuration section >>>
#endif // SL_USBD_VENDOR_INSTANCE_CONFIG_H
