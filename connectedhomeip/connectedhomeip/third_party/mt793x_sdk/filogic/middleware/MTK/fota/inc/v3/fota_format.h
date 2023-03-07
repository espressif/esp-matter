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


#ifndef __FOTA_FORMAT_H__
#define __FOTA_FORMAT_H__


#ifdef __cplusplus
extern "C"
{
#endif


/****************************************************************************
 *
 * CONSTANTS AND MACROS
 *
 ****************************************************************************/


#define MAX_BIN_16

#define MAX_BUFFER_SIZE     (1 << 24)   // 4MB,
#ifdef MAX_BIN_16
#define BIN_NUMBER          16
#else
#define BIN_NUMBER          4
#endif


#define MAX_FILE_NAME_LEN                   ( 256 )
#define MAX_SEC_NAME_LEN                    ( BIN_NUMBER * 30 )
#define SHA1_HASH_CODE_LEN                  ( 20 )
#define MAX_OUT_BUFFER_SIZE                 ( MAX_BUFFER_SIZE * BIN_NUMBER )


#define FOTA_HEADER_MAGIC                   ( 0x004D4D4D )
#define FOTA_HEADER_GET_MAGIC(magic_ver)    ( ( magic_ver ) & 0x00FFFFFF )
#define FOTA_HEADER_GET_VER(magic_ver)      ( ( magic_ver ) >> 24 )
#define FOTA_SIGNATURE_SIZE                 ( SHA1_HASH_CODE_LEN ) /* sha1 */


/****************************************************************************
 *
 * TYPE DECLARATION
 *
 ****************************************************************************/


// FOTA pkg layout
/**
 *----------start-----------------+
 * IOT_FOTA_HEADER                |
 *--------------------------------+
 * IOT_BIN_INFO []                |
 *--------------------------------+
 * hash of                        |
 * IOT_FOTA_HEADER + IOT_BIN_INFO |
 *--------------------------------+
 * #1 BIN content                 |
 *--------------------------------+
 * #1 BIN sha1 hash               |
 *--------------------------------+
 * #2 BIN content                 |
 *--------------------------------+
 * #2 BIN sha1 hash               |
 *--------------------------------+
 *             ......             |
 *-----------end------------------+
 */


typedef struct
{
    int32_t     m_ver;              /* version of structure */
    int32_t     m_error_code;       /* update information for DM */
    int32_t     m_behavior;         /* the behavior of bootloader for this error */
    int32_t     m_is_read;          /* check if DM has read */
    char        m_marker[32];       /* marker for power loss recovery, 32 is FUNET_MARKER_REGION_SIZE */
    int32_t     reserved[4];        /* reserved & make the structure 64 bytes */
} IOT_FOTA_UPDATE_INFO;


typedef struct
{
    uint32_t    m_bin_offset;       /* bin offset from FOTA pkg head */
    uint32_t    m_bin_start_addr;   /* bin start addr in ROM */
    uint32_t    m_bin_length;       /* bin length */
    uint32_t    m_partition_length; /* partition size */
    uint32_t    m_sig_offset;       /* sig offset from FOTA pkg head*/
    uint32_t    m_sig_lenth;        /* sig length */
    uint32_t    m_is_compressed;    /* is compressed or not */
    uint8_t     m_bin_reserved[4];  /* reserved */
} IOT_BIN_INFO;


// worksaround MSVC C4200
typedef struct
{
    uint32_t        m_magic_ver;
    uint32_t        m_bin_num;
//    IOT_BIN_INFO    m_extra_info[0];
} IOT_FOTA_HEADER;


#ifdef __cplusplus
}
#endif


#endif /* __FOTA_FORMAT_H__ */
