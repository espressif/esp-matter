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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "nvdm.h"

#include "msc_api.h"

#define MAX_BUF_LEN   10

int msc_config_set_cloud_platform(cloud_platform_e mode)
{
#ifdef MTK_NVDM_ENABLE
    int value = 0x0;
    nvdm_status_t status;
    char buf[MAX_BUF_LEN]= {0};

    if (mode & CLOUD_SMART_PLATFORM_ALINK) {
#ifdef MTK_ALINK_ENABLE
        value |= CLOUD_SMART_PLATFORM_ALINK;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_ALINK_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    if (mode & CLOUD_SMART_PLATFORM_JOYLINK) {
#ifdef MTK_JOYLINK_ENABLE
        value |= CLOUD_SMART_PLATFORM_JOYLINK;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_JOYLINK_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    if (mode & CLOUD_SMART_PLATFORM_AIRKISS) {
#ifdef MTK_AIRKISS_ENABLE
        value |= CLOUD_SMART_PLATFORM_AIRKISS;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_AIRKISS_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    sprintf(buf, "%d", value);

    status = nvdm_write_data_item(
                 "common",
                 "cloud_platform",
                 NVDM_DATA_ITEM_TYPE_STRING,
                 (uint8_t *)buf,
                 strlen(buf));

    LOG_I(multiSmnt,"%s: status:%d, buf:%s.\r\n", __FUNCTION__, status, buf);

    return (NVDM_STATUS_OK == status) ? 0 : -1;
#endif
    LOG_E(multiSmnt,"not support nvdm\r\n");
    return -1;
}

int msc_config_get_cloud_platform(cloud_platform_e *mode)
{
#ifdef MTK_NVDM_ENABLE
    int temp;
    nvdm_status_t status;
    char buf[MAX_BUF_LEN]= {0};
    uint32_t buf_sz = sizeof(buf);
    status = nvdm_read_data_item(
                 "common",
                 "cloud_platform",
                 (uint8_t *)buf,
                 &buf_sz);

    LOG_I(multiSmnt,"%s: status:%d, buf_sz:%d, buf:%s.\r\n", __FUNCTION__, status, buf_sz, buf);

    if(mode == NULL) {
        return -1;
    }

    *mode = atoi((char *)buf);
    temp= *mode;

    if (*mode & CLOUD_SMART_PLATFORM_ALINK) {
#ifndef MTK_ALINK_ENABLE
        LOG_W(multiSmnt,"warning msg: __MTK_ALINK_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~CLOUD_SMART_PLATFORM_ALINK;
#endif
    }

    if (*mode & CLOUD_SMART_PLATFORM_JOYLINK) {
#ifndef MTK_JOYLINK_ENABLE
        LOG_W(multiSmnt,"warning msg: __MTK_JOYLINK_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~CLOUD_SMART_PLATFORM_JOYLINK;
#endif
    }

    if (*mode & CLOUD_SMART_PLATFORM_AIRKISS) {
#ifndef MTK_AIRKISS_ENABLE
        LOG_W(multiSmnt,"warning msg: __MTK_AIRKISS_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~CLOUD_SMART_PLATFORM_AIRKISS;
#endif
    }

    if (temp != *mode) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", *mode);
        status = nvdm_write_data_item(
                     "common",
                     "cloud_platform",
                     NVDM_DATA_ITEM_TYPE_STRING,
                     (uint8_t *)buf,
                     strlen(buf));
    }

    return (NVDM_STATUS_OK == status) ? 0 : -1;
#endif
    LOG_E(multiSmnt,"not support nvdm\r\n");
    return -1;
}

int msc_config_set_smnt_type(smnt_type_e mode)
{
#ifdef MTK_NVDM_ENABLE
    int value = 0x0;
    nvdm_status_t status;
    char buf[MAX_BUF_LEN]= {0};

    if (mode & SUPPORT_MTK_SMNT) {
#if  defined(MTK_SMTCN_V4_ENABLE) || defined(MTK_SMTCN_V5_ENABLE)
        value |= SUPPORT_MTK_SMNT;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_SMTCN_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    if (mode & SUPPORT_JOYLINK) {
#ifdef MTK_JOYLINK_ENABLE
        value |= SUPPORT_JOYLINK;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_JOYLINK_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    if (mode & SUPPORT_AIRKISS) {
#ifdef MTK_AIRKISS_ENABLE
        value |= SUPPORT_AIRKISS;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_AIRKISS_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    if (mode & SUPPORT_AWS) {
#ifdef MTK_ALINK_ENABLE
        value |= SUPPORT_AWS;
#else
        LOG_W(multiSmnt,"warning msg: __MTK_ALINK_ENABLE__ do not define, please define it first.\r\n");
#endif
    }

    sprintf(buf, "%d", value);

    status = nvdm_write_data_item(
                 "common",
                 "smnt_type",
                 NVDM_DATA_ITEM_TYPE_STRING,
                 (uint8_t *)buf,
                 strlen(buf));

    LOG_I(multiSmnt,"%s: status:%d, buf:%s.\r\n", __FUNCTION__, status, buf);

    return (NVDM_STATUS_OK == status) ? 0 : -1;
#endif
    LOG_E(multiSmnt,"not support nvdm\r\n");
    return -1;
}

int msc_config_get_smnt_type(smnt_type_e *mode)
{
#ifdef MTK_NVDM_ENABLE
    int temp;
    nvdm_status_t status;
    char buf[MAX_BUF_LEN]= {0};
    uint32_t buf_sz = sizeof(buf);
    status = nvdm_read_data_item(
                 "common",
                 "smnt_type",
                 (uint8_t *)buf,
                 &buf_sz);

    LOG_I(multiSmnt,"%s: status:%d, buf_sz:%d, buf:%s.\r\n", __FUNCTION__, status, buf_sz, buf);

    if(mode == NULL) {
        return -1;
    }

    *mode = atoi((char *)buf);
    temp= *mode;

    if (*mode & SUPPORT_AWS) {
#ifndef MTK_ALINK_ENABLE
        LOG_W(multiSmnt,"warning msg: __MTK_ALINK_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~SUPPORT_AWS;
#endif
    }

    if (*mode & SUPPORT_JOYLINK) {
#ifndef MTK_JOYLINK_ENABLE
        LOG_W(multiSmnt,"warning msg: __MTK_JOYLINK_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~SUPPORT_JOYLINK;
#endif
    }

    if (*mode & SUPPORT_AIRKISS) {
#ifndef MTK_AIRKISS_ENABLE
        LOG_W(multiSmnt,"warning msg: __MTK_AIRKISS_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~SUPPORT_AIRKISS;
#endif
    }

    if (*mode & SUPPORT_MTK_SMNT) {
#if !defined(MTK_SMTCN_V4_ENABLE) && !defined(MTK_SMTCN_V5_ENABLE)
        LOG_W(multiSmnt,"warning msg: __MTK_SMTCN_V4_ENABLE__ do not define, clear up nvdm flag.\r\n");
        *mode &= ~SUPPORT_MTK_SMNT;
#endif
    }

    if (temp != *mode) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d", *mode);
        status = nvdm_write_data_item(
                     "common",
                     "smnt_type",
                     NVDM_DATA_ITEM_TYPE_STRING,
                     (uint8_t *)buf,
                     strlen(buf));
    }

    return (NVDM_STATUS_OK == status) ? 0 : -1;
#endif
    LOG_E(multiSmnt,"not support nvdm\r\n");
    return -1;
}

/**
 * @brief    set whether auto start the smart connection after boot up
 *
 * @param    [IN] flag
 *              0 means disable autostart,  1 means enable autostart
 *
 * @return	 0 means success,-1 means fail
 */
int msc_config_set_smnt_autostart(int flag)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t status;
    char buf[MAX_BUF_LEN]= {0};

    if(flag != 0 && flag != 1) {
        LOG_E(multiSmnt,"flag error\n");
        return -1;
    }

    sprintf(buf, "%d", flag);

    status = nvdm_write_data_item(
                 "common",
                 "smnt_autostart",
                 NVDM_DATA_ITEM_TYPE_STRING,
                 (uint8_t *)buf,
                 strlen(buf));

    LOG_I(multiSmnt,"%s: status:%d, buf:%s.\r\n", __FUNCTION__, status, buf);
    return (NVDM_STATUS_OK == status) ? 0 : -1;
#endif
    LOG_E(multiSmnt,"not support nvdm\r\n");
    return -1;
}

/**
 * @brief    get whether auto start the smart connection after boot up
 *
 * @param    [OUT] flag
 *              0 means disable autostart,  1 means enable autostart
 *
 * @return	 0 means success,-1 means fail
 */
int msc_config_get_smnt_autostart(int *flag)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t status;
    char buf[MAX_BUF_LEN]= {0};
    uint32_t buf_sz = sizeof(buf);
    status = nvdm_read_data_item(
                 "common",
                 "smnt_autostart",
                 (uint8_t *)buf,
                 &buf_sz);

    LOG_I(multiSmnt,"%s: status:%d, buf_sz:%d, buf:%s.\r\n", __FUNCTION__, status, buf_sz, buf);

    *flag = atoi(buf);
    return (NVDM_STATUS_OK == status) ? 0 : -1;
#endif
    LOG_E(multiSmnt,"not support nvdm\r\n");
    return -1;
}


