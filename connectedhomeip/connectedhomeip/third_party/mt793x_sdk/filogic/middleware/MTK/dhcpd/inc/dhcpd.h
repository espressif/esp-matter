/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __DHCPD_H__
#define __DHCPD_H__
#include "lwip/ip4_addr.h"
#include "syslog.h"


/**
 * @addtogroup DHCPD
 * @{
 * This section introduces the DHCP daemon (DHCPD) APIs including details on how to use this module, structures and functions.
 * It's a simple DHCP daemon to operate in soft AP mode for assigning IP addresses to connected Wi-Fi station nodes.
 *
 * @section DHCPD_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DHCP                       | Dynamic Host Configuration Protocol. |
 * |\b Wi-Fi                      | Wi-Fi is a local area wireless computer networking technology, https://en.wikipedia.org/wiki/Wi-Fi |
 *
 * @section DHCPD_Usage_Chapter How to use this module
 *
 * - Step1: Set DHCPD parameters.
 *              If you want to use all default DHCPD parameters (DHPCD_DEFAULT_X, such as DHPCD_DEFAULT_SERVER_IP),
 *              skip this step.
 * - Step2: Call #dhcpd_start() to start the service.
 *              If you want to use all default DHCPD parameters (DHPCD_DEFAULT_X, such as DHPCD_DEFAULT_SERVER_IP),
 *              just set the input parameter dhcpd_settings of #dhcpd_start() to be NULL.
 *  - Sample code:
 *     @code
 *        xTaskHandle dhcpd_task;
 *        int32_t process_softap(void)
 *        {
 *            LOG_I(common, "Now start DHCPD:");
 *            dhcpd_settings_t dhcpd_settings;
 *
 *            memset(&dhcpd_settings, 0, sizeof(dhcpd_settings_t));
 *            strncpy(dhcpd_settings.dhcpd_server_address, "10.10.10.2" , IP4ADDR_STRLEN_MAX - 1);
 *            strncpy(dhcpd_settings.dhcpd_gateway, "10.10.10.1" , IP4ADDR_STRLEN_MAX - 1);
 *            strncpy(dhcpd_settings.dhcpd_netmask, "255.255.255.0" , IP4ADDR_STRLEN_MAX - 1);
 *            strncpy(dhcpd_settings.dhcpd_primary_dns, "8.8.8.8" , IP4ADDR_STRLEN_MAX - 1);
 *            strncpy(dhcpd_settings.dhcpd_secondary_dns, "8.8.4.4" , IP4ADDR_STRLEN_MAX - 1);
 *            strncpy(dhcpd_settings.dhcpd_ip_pool_start, "10.10.10.3" , IP4ADDR_STRLEN_MAX - 1);
 *            strncpy(dhcpd_settings.dhcpd_ip_pool_end, "10.10.10.11" , IP4ADDR_STRLEN_MAX - 1);
 *
 *            dhcpd_start(&dhcpd_settings);
 *
 *            return 0;
 *        }
 *
 *     @endcode
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup dhcpd_define Define
  * @{
  */


/** @brief  This macro enables the feature of saving the client configuration when AP is keeping power on.
  * Client configuration will not be stored when AP is power off.
  */
#define DHCPD_SAVE_CLIENT_CONFIG_ON_LINE


/** @brief This macro defines default IP lease time in seconds.
 */
#define DHCPD_DEFAULT_LEASE_TIME (64800)


/** @brief This macro defines default server IP for AP.
 */
#define DHPCD_DEFAULT_SERVER_IP            ("10.10.10.1")


/** @brief This macro defines default gateway IP for AP.
 */
#define DHPCD_DEFAULT_GATEWAY              ("10.10.10.1")


/** @brief This macro defines default netmask for AP.
 */
#define DHPCD_DEFAULT_NETMASK              ("255.255.255.0")


/** @brief This macro defines default primary DNS IP for AP.
 */
#define DHPCD_DEFAULT_PRIMARY_DNS          ("8.8.8.8")


/** @brief This macro defines default secondary DNS IP for AP.
 */
#define DHPCD_DEFAULT_SECONDARY_DNS        ("8.8.4.4")


/** @brief This macro defines default starting IP for IP pool.
 */
#define DHPCD_DEFAULT_IP_POOL_START        ("10.10.10.2")


/** @brief This macro defines default ending IP for IP pool.
 */
#define DHPCD_DEFAULT_IP_POOL_END          ("10.10.10.10")

/**
  * @}
  */


/** @defgroup dhcpd_struct Struct
  * @{
  */

/** @brief This structure defines the DHCPD configuration structure. For more information, please refer to #dhcpd_start() */
typedef struct
{
    char dhcpd_server_address[IP4ADDR_STRLEN_MAX];  /**< Specify server IP for AP. */
    char dhcpd_gateway[IP4ADDR_STRLEN_MAX];         /**< Specify gateway IP for AP. */
    char dhcpd_netmask[IP4ADDR_STRLEN_MAX];         /**< Specify netmask for AP. */
    char dhcpd_primary_dns[IP4ADDR_STRLEN_MAX];     /**< Specify primary DNS IP for AP. */
    char dhcpd_secondary_dns[IP4ADDR_STRLEN_MAX];   /**< Specify secondary DNS IP for AP. */
    char dhcpd_ip_pool_start[IP4ADDR_STRLEN_MAX];   /**< Specify starting IP for IP pool. */
    char dhcpd_ip_pool_end[IP4ADDR_STRLEN_MAX];     /**< Specify ending IP for IP pool. */
} dhcpd_settings_t;

/**
  * @}
  */


/**
 * @brief This function is used to start the DHCPD service. It creates a new task for the DHCPD, then returns. The DHCPD task runs until dhcpd_stop() is called.
 * @param[in] dhcpd_settings specifies the settings for DHCPD, such as DHCP server IP, gateway, netmask, DNS and IP pool. These settings can be set individually.
 *            If the setting is not set, default value will be used. Please beware that if both default values and customer configured value are used, they should
 *            match to each other.
 * @return
 * @sa #dhcpd_stop();
 */
int dhcpd_start(dhcpd_settings_t *dhcpd_settings);


/**
 * @brief This function is used to stop the DHCPD service. It kills the DHCPD task and stops the DHCPD service immediately.
 * @return
 * @sa #dhcpd_start();
 */
void dhcpd_stop(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __DHCPD_H__ */
