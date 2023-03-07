/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _USB_HOST_HUB_APP_H_
#define _USB_HOST_HUB_APP_H_

#if ((defined USB_HOST_CONFIG_HUB) && (USB_HOST_CONFIG_HUB))

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief HUB reset times*/
#define USB_HOST_HUB_PORT_RESET_TIMES (1)

#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
/*! @brief HUB Control tansaction retry times for remote wakeup*/
#define USB_HOST_HUB_REMOTE_WAKEUP_TIMES (3U)
#endif

/*! @brief HUB application global structure */
typedef struct _usb_host_hub_global
{
    usb_host_handle hostHandle;          /*!< This HUB list belong to this host*/
    usb_host_hub_instance_t *hubProcess; /*!< HUB in processing*/
    usb_host_hub_instance_t *hubList;    /*!< host's HUB list*/
    usb_osa_mutex_handle hubMutex;       /*!< HUB mutex*/
} usb_host_hub_global_t;

/*! @brief HUB application status */
typedef enum _usb_host_hub_app_status
{
    kHubRunIdle = 0,         /*!< Idle */
    kHubRunInvalid,          /*!< Invalid state */
    kHubRunWaitSetInterface, /*!< Wait callback of set interface */
    kHubRunGetDescriptor7,   /*!< Get 7 bytes HUB descriptor */
    kHubRunGetDescriptor,    /*!< Get all HUB descriptor */
    kHubRunSetPortPower,     /*!< Set HUB's port power */
    kHubRunGetStatusDone,    /*!< HUB status changed */
    kHubRunClearDone,        /*!< clear HUB feature callback */
} usb_host_hub_app_status_t;

/*! @brief HUB port application status */
typedef enum _usb_host_port_app_status
{
    kPortRunIdle = 0,             /*!< Idle */
    kPortRunInvalid,              /*!< Invalid state */
    kPortRunWaitPortChange,       /*!< Wait port status change */
    kPortRunCheckCPortConnection, /*!< Check C_PORT_CONNECTION */
    kPortRunGetPortConnection,    /*!< Get port status data */
    kPortRunCheckPortConnection,  /*!< Check PORT_CONNECTION   */
    kPortRunWaitPortResetDone,    /*!< Wait port reset transfer done */
    kPortRunWaitCPortReset,       /*!< Wait C_PORT_RESET */
    KPortRunCheckCPortReset,      /*!< Check C_PORT_RESET */
    kPortRunResetAgain,           /*!< Reset port again */
    kPortRunPortAttached,         /*!< Device is attached on the port */
    kPortRunCheckPortDetach,      /*!< Check port is detached */
    kPortRunGetConnectionBit,     /*!< Get the port status data */
    kPortRunCheckConnectionBit,   /*!< Check PORT_CONNECTION */
#if ((defined(USB_HOST_CONFIG_LOW_POWER_MODE)) && (USB_HOST_CONFIG_LOW_POWER_MODE > 0U))
    kPortRunClearCPortSuspend, /*!< Clear C_PORT_SUSPEND */
    kPortRunCheckPortSuspend,  /*!< Check PORT_SUSPEND */
    kPortRunPortSuspended,     /*!< Port is suspended */
#endif
} usb_host_port_app_status_t;

/*! @brief HUB data prime status */
typedef enum _usb_host_hub_prime_status
{
    kPrimeNone = 0,    /*!< Don't prime data*/
    kPrimeHubControl,  /*!< Prime HUB control transfer*/
    kPrimePortControl, /*!< Prime port control transfer*/
    kPrimeInterrupt,   /*!< Prime interrupt transfer*/
} usb_host_hub_prime_status_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#endif /* USB_HOST_CONFIG_HUB */

#endif /* _USB_HOST_HUB_APP_H_ */
