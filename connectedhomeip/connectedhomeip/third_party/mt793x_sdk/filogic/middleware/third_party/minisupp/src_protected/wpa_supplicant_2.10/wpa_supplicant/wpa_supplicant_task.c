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

#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include "wifi_os_api.h"
#include "wpa_debug.h"
#include "wpa_supplicant_task.h"
#include "task_def.h"
#include "hal_sleep_manager.h"
#ifdef CONFIG_DRIVER_INBAND
#include "wifi_inband.h"
#include "hal_lp.h"
#endif
#ifdef CONFIG_DRIVER_GEN4M
#endif

wifi_config_t g_wifi_config = {0};
wifi_config_ext_t g_wifi_config_ext = {0};
#ifdef CONFIG_DRIVER_NONE 
extern global_event_callback __process_global_event[MAX_INTERFACE_NUM];
#endif

extern int wpa_supplicant_entry(unsigned char);

static os_task_t g_supp_task_handle = NULL;

static void wpa_supplicant_task(void *args)
{
	uint8_t op_mode = g_wifi_config.opmode;
	wpa_printf(MSG_ERROR, "%s xTaskCreate Success. enter entry.", __func__);
	wpa_supplicant_entry((unsigned char)op_mode);
	wifi_os_task_delete(g_supp_task_handle);
	g_supp_task_handle = 0;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
uint8_t wifi_init_handle = 0xFF;
#endif

/* int32_t wpa_supplicant_task_init(wifi_config_t *config) */
int32_t wpa_supplicant_task_init(wifi_config_t *config, wifi_config_ext_t *config_ext)
{
	if (config != NULL)
		os_memcpy(&g_wifi_config, config, sizeof(wifi_config_t));
	else
		os_memset(&g_wifi_config, 0, sizeof(wifi_config_t));

	if (config_ext != NULL)
		os_memcpy(&g_wifi_config_ext, config_ext, sizeof(wifi_config_ext_t));
	else
		os_memset(&g_wifi_config_ext, 0, sizeof(wifi_config_ext_t));

	wpa_printf(MSG_DEBUG, "%s prepare ceate task, op_mode=%d", __func__,
		   g_wifi_config.opmode);
	BaseType_t ret_val = wifi_os_task_create(wpa_supplicant_task,
				UNIFY_WPA_SUPPLICANT_TASK_NAME,
				UNIFY_WPA_SUPPLICANT_TASK_STACKSIZE,
				NULL,
				UNIFY_WPA_SUPPLICANT_TASK_PRIO,
				&g_supp_task_handle);
	if (0 != ret_val) {
		wpa_printf(MSG_ERROR, "xTaskCreate fail.");
		return -1;
	}

	wpa_printf(MSG_ERROR, "%s task created %ld", __func__, ret_val);
#if 0 /*  def HAL_SLEEP_MANAGER_ENABLED */
	wifi_init_handle = hal_sleep_manager_set_sleep_handle("wifi_init");
	if(wifi_init_handle != 0xFF) {
		hal_sleep_manager_lock_sleep(wifi_init_handle);
	} else {
		LOG_W(minisupp, "No sleep wifi_init_handle");
	}
#endif

	return 0;
}

