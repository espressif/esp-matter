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

 /** @file memextract.h
 *
 *  memory dump to flash and extract with memory extracter
 *
 */

#ifndef __MEMEXTRACT_H__
#define __MEMEXTRACT_H__

#include "syslog.h"

#define MEMORY_EXTRACT_MAX_FILES               (6)

/****************************************************************************
 * Header
 ****************************************************************************/
#define PREDEFINED_SECTION       (0)
#define USER_SECTION             (1)

#define HEADER_SIZE              (26)
#define FILENAME_SIZE            (18)

#define SWLA_HEADER_SIZE         (20)
#define SWLA_FILENAME_SIZE       (12)
/****************************************************************************/

/****************************************************************************
 * Address
 ****************************************************************************/
#define SWLA_BASE                (LOG_BASE + 4 + 8 + SWLA_HEADER_SIZE)
#define SWLA_LENGTH              ( 4096 - 4 - 8 - HEADER_SIZE - SWLA_HEADER_SIZE )//(4096-(4+8+HEADER_SIZE*2))

#define USER_BASE                (NORMAL_SYSLOG_BASE+NORMAL_SYSLOG_LENGTH)
/****************************************************************************/


typedef struct{
    uint32_t filename_size;
    char* filename;
    uint32_t file_size;
    char *content;
} extract_file_t;

typedef struct{
    char* start_addr;
    uint32_t len;
} region_list_t;

typedef enum{
    MEMORY_EXTRACT_SWLA,
    MEMORY_EXTRACT_OTHER
} extract_type;


/* What the callback function does is just to set the region_list, num and total_size.
   In this way, memextract is able to know how and what to save */
typedef void (*memextract_cb)(region_list_t** region_list, int* num, uint32_t* total_size);

typedef struct{
    extract_type   type;
    memextract_cb  cb;
} memextract_entry;

void memextract_init(void);

void memextract_register_callbacks(memextract_entry* cb);
#endif//__MEMEXTRACT_H__
