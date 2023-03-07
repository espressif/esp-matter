/***************************************************************************//**
 * @file
 * @brief Common - RTOS Prio
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _RTOS_PRIO_H_
#define  _RTOS_PRIO_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           TASK PRIORITIES
 *******************************************************************************************************/

//                                                                 ------------- VERY HIGH PRIORITY TASKS -------------
#ifndef  KERNEL_TICK_TASK_PRIO_DFLT
#define  KERNEL_TICK_TASK_PRIO_DFLT                            4u
#endif

#ifndef  KERNEL_TMR_TASK_PRIO_DFLT
#define  KERNEL_TMR_TASK_PRIO_DFLT                             5u
#endif

#ifndef  KERNEL_STAT_TASK_PRIO_DFLT
#define  KERNEL_STAT_TASK_PRIO_DFLT                            6u
#endif

//                                                                 --------------- HIGH PRIORITY TASKS ----------------
#ifndef  CANOPEN_CORE_TASK_PRIO_DFLT
#define  CANOPEN_CORE_TASK_PRIO_DFLT                           9u
#endif

#ifndef  USBH_HUB_TASK_PRIO_DFLT
#define  USBH_HUB_TASK_PRIO_DFLT                              10u
#endif

#ifndef  USBH_PBHCI_TASK_PRIO_DFLT
#define  USBH_PBHCI_TASK_PRIO_DFLT                            11u
#endif

#ifndef  IO_SD_CORE_TASK_PRIO_DFLT
#define  IO_SD_CORE_TASK_PRIO_DFLT                            14u
#endif

//                                                                 -------------- MEDIUM PRIORITY TASKS ---------------
#ifndef  NET_CORE_TASK_CFG_PRIO_DFLT
#define  NET_CORE_TASK_CFG_PRIO_DFLT                          16u
#endif

#ifndef  NET_CORE_IF_WIFI_TASK_PRIO_DFLT
#define  NET_CORE_IF_WIFI_TASK_PRIO_DFLT                      17u
#endif

#ifndef  HTTP_CLIENT_TASK_PRIO_DFLT
#define  HTTP_CLIENT_TASK_PRIO_DFLT                           18u
#endif

#ifndef  MQTT_CLIENT_TASK_PRIO_DFLT
#define  MQTT_CLIENT_TASK_PRIO_DFLT                           19u
#endif

#ifndef  IPERF_TASK_PRIO_DFLT
#define  IPERF_TASK_PRIO_DFLT                                 20u
#endif

#ifndef  TELNET_SERVER_SESSION_TASK_PRIO_DFLT
#define  TELNET_SERVER_SESSION_TASK_PRIO_DFLT                 21u
#endif

#ifndef  TELNET_SERVER_SRV_TASK_PRIO_DFLT
#define  TELNET_SERVER_SRV_TASK_PRIO_DFLT                     22u
#endif

#ifndef  TFTP_SERVER_TASK_PRIO_DFLT
#define  TFTP_SERVER_TASK_PRIO_DFLT                           23u
#endif

#ifndef  USBH_ASYNC_TASK_PRIO_DFLT
#define  USBH_ASYNC_TASK_PRIO_DFLT                            25u
#endif

#ifndef  IO_SD_ASYNC_TASK_PRIO_DFLT
#define  IO_SD_ASYNC_TASK_PRIO_DFLT                           26u
#endif

//                                                                 ---------------- LOW PRIORITY TASKS ----------------
#ifndef  FS_MEDIA_POLL_TASK_PRIO_DFLT
#define  FS_MEDIA_POLL_TASK_PRIO_DFLT                         27u
#endif

#ifndef  NET_CORE_SVC_TASK_CFG_PRIO_DFLT
#define  NET_CORE_SVC_TASK_CFG_PRIO_DFLT                      28u
#endif

#ifndef  USBD_HID_TMR_TASK_PRIO_DFLT
#define  USBD_HID_TMR_TASK_PRIO_DFLT                          29u
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of rtos path module include.
