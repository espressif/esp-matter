/* Copyright Statement:
 *
 * (C) 2021-2021  MediaTek Inc. All rights reserved.
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


#ifndef __FOTA_DOWNLOAD_H__
#define __FOTA_DOWNLOAD_H__


/****************************************************************************
 *
 * HEADER FILES
 *
 ****************************************************************************/


// C header files
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>


// fotav3 header files
#include <v3/fota.h>
#include <v3/url.h>


/****************************************************************************
 *
 * PUBLIC FUNCTIONS
 *
 ****************************************************************************/


#ifdef MTK_FOTA_V3_HTTP_ENABLE
/**
 * Download fota update package by http client. If success, then reboot device and trigger update process.
 */
ssize_t fota_download_by_http(
    char                *address,
    uint16_t            rport,
    char                *filename,
    const uint32_t      partition,
    bool                writing,
    const fota_flash_t  *flash);
#endif


#ifdef MTK_FOTA_V3_HTTPS_ENABLE
ssize_t fota_download_by_https(
    char                *address,
    uint16_t            rport,
    char                *filename,
    const uint32_t      partition,
    bool                writing,
    const fota_flash_t  *flash);
#endif


#ifdef MTK_FOTA_V3_TFTP_ENABLE
ssize_t fota_download_by_tftp(
    char                *address,
    uint16_t            rport,
    char                *filename,
    const uint32_t      partition,
    bool                writing,
    const fota_flash_t  *flash);
#endif


fota_status_t fota_download(
    const url_t         *url,
    const fota_flash_t  *flash,
    const uint32_t      partition,
    bool                download_only);


#endif /* __FOTA_DOWNLOAD_H__ */

