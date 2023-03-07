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

#if defined(MTK_MINICLI_ENABLE)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "ff.h"

/****************************************************************************
 *
 * Static variables.
 *
 ****************************************************************************/

FATFS    FatFS[FF_VOLUMES];               /* Fatfs target */

const char* const volume_str[] = {FF_VOLUME_STRS};

/****************************************************************************
 *
 * Local functions.
 *
 ****************************************************************************/

int get_pdstr(char *pd, char *buf, int buflen)
{
#if FF_VOLUMES > 1
    int i;
#endif

    int res;
    char  mountStr[16] = {0};

    if (strlen(pd) > (unsigned int)buflen )
    {
        printf("Buffer Overflow!\n\r");
        return -1;
    }

#if FF_VOLUMES == 1
    res = sprintf(mountStr, "%s:/", volume_str[0]);
    if( res < 0 )
    {
        return -1;
    }

    if ( strncasecmp(pd, mountStr, strlen(mountStr)) == 0 )        // path like SD:/file.txt
    {
        res = sprintf(buf, "%s", pd);
        if( res < 0 )
        {
            return -1;
        }
    }
    else if ( strstr(pd, ":/") == NULL)                             // path like file.txt
    {
        res = sprintf(buf, "%s%s", mountStr, pd);
        if( res < 0 )
        {
            return -1;
        }
    }
    else
    {
        printf("Invalid Volume Str");
        return -1;
    }

    return 0;
#else
    for (i=0; i<FF_VOLUMES; i++)
    {
        res = sprintf(mountStr, "%s:/", volume_str[i]);
        if( res < 0 )
        {
            return -1;
        }
        if ( strncasecmp(pd, mountStr, strlen(mountStr)) == 0 )
        {
            res = sprintf(buf, "%s", pd);
            if( res < 0 )
            {
                return -1;
            }
            break;
        }
        memset(mountStr, 0, sizeof(mountStr));
    }
    if (i == FF_VOLUMES) {
        printf("Invalid Volumn Str!\r\n");
        return -1;
    }

    return i;
#endif
}


uint8_t _ff_cli_mount(uint8_t len, char *param[])
{
    FRESULT  res;                 /* fs status infor*/
    char     mountStr[64] = {0};
    int      vol = -1;
    int      res_sptf;

#if FF_VOLUMES == 1
    res_sptf = sprintf(mountStr, "%s:/", volume_str[0]);
    if( res_sptf < 0 )
    {
        return -1;
    }
#else
    if (!len) {
    #if FF_VOLUMES == 2
        printf("[Usage]: ff mount %s/%s\r\n", volume_str[0],volume_str[1]);
    #else
        printf("[Usage]: ff mount %s/%s/%s\r\n", volume_str[0],volume_str[1],volume_str[2]);
    #endif
        return 0;
    }
    res_sptf = sprintf(mountStr, "%s:/", param[0]);
    if( res_sptf < 0 )
    {
        return -1;
    }
#endif

    vol = get_pdstr(mountStr, mountStr, sizeof(mountStr) );
    if (vol<0)
    {
        printf("Get PD Str Fail\r\n");
        return 0;
    }
    printf("Vol(%u)", vol);

    res = f_mount(&FatFS[vol], mountStr, 1);
    if(res)
    {
        res = f_mkfs(mountStr, 0, NULL, FF_MAX_SS);
        if(res)
        {
            printf("[FS]: Create New FS Fail - res(%u)\r\n", res);
            return 0;
        }
        printf("[FS]: New FS Created (%s)\r\n", mountStr);
        res = f_mount(&FatFS[vol], mountStr, 1);
        if(res) {
            printf("[FS]: Mount Fail - res(%u)\r\n", res);
            return 0;
        }
    }

    printf("[FS]: Mount OK!\r\n");
    return 0;
}


uint8_t _ff_cli_mkdir(uint8_t len, char *param[])
{
    FRESULT  res;                 /* fs status infor*/
    int      ret = -1;
    char     path[128] = {0};

#if FF_VOLUMES == 1
    if (!len) {
        printf("[Usage]: ff mkdir new_folder\r\n");
        return 0;
    }
#else
    if (!len) {
        printf("[Usage]: ff mkdir [VolumnStr]:/new_folder\r\n");
    #if FF_VOLUMES == 2
        printf("\t\t VolumnStr == %s %s\r\n", volume_str[0],volume_str[1]);
    #else
        printf("\t\t VolumnStr == %s/%s/%s\r\n", volume_str[0],volume_str[1],volume_str[2]);
    #endif
        return 0;
    }
#endif
    ret = get_pdstr(param[0], path, sizeof(path) );
    if (ret<0)
    {
        printf("Get PD Str Fail\r\n");
        return 0;
    }

    res = f_mkdir(path);
    if (res)
    {
        printf("[FS]: Folder Create Fail - res(%u)\r\n", res);
        return 0;
    }
    printf("[FS]: Create Folder (%s)\r\n", path);

    return 0;
}

uint8_t _ff_cli_write(uint8_t len, char *param[])
{
    FRESULT  res;                 /* fs status infor*/
    FIL      fdst;                /* file target */
    int      ret = -1;
    char    *pbuff     = NULL;
    char     path[128] = {0};
    UINT     length_written;


#if FF_VOLUMES == 1
    if (len<2) {
        printf("[Usage]: ff write file_name write_context\r\n");
        return 0;
    }
#else
    if (len<2) {
        printf("[Usage]: ff write [volumn_str]:file_name write_context\r\n");
    #if FF_VOLUMES == 2
        printf("\t\t VolumnStr == %s %s\r\n", volume_str[0],volume_str[1]);
    #else
        printf("\t\t VolumnStr == %s/%s/%s\r\n", volume_str[0],volume_str[1],volume_str[2]);
    #endif
        return 0;
    }
#endif
    ret = get_pdstr(param[0], path, sizeof(path) );
    if (ret<0)
    {
        printf("Get PD Str Fail\r\n");
        return 0;
    }

    pbuff = param[1];

    res = f_open(&fdst, path, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (res)
    {
        printf("[FS]: File Open Fail - res(%u)\r\n", res);
        return 0;
    }

    res = f_write(&fdst, pbuff, strlen(pbuff), &length_written);
    if (res)
    {
        res = f_close(&fdst);
        printf("[FS_Write]: Write File Fail - res(%u)\r\n", res);
        return 0;
    }
    res = f_close(&fdst);

    printf("[FS]: Write - pfile(%s), pbuff(%s), len(%u)\r\n", path, pbuff, strlen(pbuff));

    return 0;
}


uint8_t _ff_cli_read(uint8_t len, char *param[])
{
    FRESULT  res;                 /* fs status infor*/
    FIL      fdst;                /* file target */
    int      ret = -1;
    char     buff[256] = {0};
    char     path[128] = {0};
    UINT     length_read;


#if FF_VOLUMES == 1
    if (!len) {
        printf("[Usage]: ff read file_name\r\n");
        return 0;
    }
#else
    if (!len) {
        printf("[Usage]: ff read [volumn_str]:file_name\r\n");
    #if FF_VOLUMES == 2
        printf("\t\t VolumnStr == %s %s\r\n", volume_str[0],volume_str[1]);
    #else
        printf("\t\t VolumnStr == %s/%s/%s\r\n", volume_str[0],volume_str[1],volume_str[2]);
    #endif
        return 0;
    }
#endif
    ret = get_pdstr(param[0], path, sizeof(path) );
    if (ret<0)
    {
        printf("Get PD Str Fail\r\n");
        return 0;
    }

    res = f_open(&fdst, path, FA_OPEN_EXISTING | FA_READ);
    if (res)
    {
        printf("[FS]: File Open Fail - res(%u)\r\n", res);
        return 0;
    }

    res = f_read(&fdst, buff, sizeof(buff), &length_read);
    if (res)
    {
        res = f_close(&fdst);
        printf("[FS_Read]: Read File Fail - res(%u)\r\n", res);
        return 0;
    }

    printf("[FS]: Read - Path(%s), pbuff(%s), len(%u)\r\n", path, buff, sizeof(buff));

    return 0;
}

uint8_t _ff_cli_ls(uint8_t len, char *param[])
{
    FRESULT  res;                 /* fs status infor*/
    DIR      dir;               /* Directory object */
    FILINFO  Finfo;
    int      ret = -1;
    char     path[128] = {0};


    ret = get_pdstr(len ? param[0] : "", path, sizeof(path) );
    if (ret<0)
    {
        printf("Get PD Str Fail\r\n");
        return 0;
    }

    printf("[FS]: Folder (%s)\r\n", path);
    res = f_opendir(&dir, path);
    if (res)
    {
        printf("[FS]: Folder Open Fail - res(%u)\r\n", res);
        return 0;
    }

    for(;;) {
        res = f_readdir(&dir, &Finfo);
        if (res || !Finfo.fname[0])
        {
            break;
        }
        printf("%s%s%s%s%s %u/%02u/%02u %02u:%02u %lu ",
            (Finfo.fattrib & AM_DIR) ? "DIR" : "-",
            (Finfo.fattrib & AM_RDO) ? "RDO" : "-",
            (Finfo.fattrib & AM_HID) ? "HID" : "-",
            (Finfo.fattrib & AM_SYS) ? "SYS" : "-",
            (Finfo.fattrib & AM_ARC) ? "ARC" : "-",
            (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
            (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63, (unsigned long)Finfo.fsize);
        printf("%s\n", Finfo.fname);
    }
    f_closedir(&dir);

    return 0;
}

uint8_t _ff_cli_delete(uint8_t len, char *param[])
{
    FRESULT  res;                 /* fs status infor*/
    int      ret = -1;
    char     path[128] = {0};
    ret = get_pdstr(param[0], path, sizeof(path) );
    if (ret<0)
    {
        printf("Get PD Str Fail\r\n");
        return 0;
    }

#if FF_VOLUMES == 1
    if (!len) {
        printf("[Usage]: ff delete new_folder\r\n");
        return 0;
    }
#else
    if (!len) {
        printf("[Usage]: ff delete [VolumnStr]:/new_folder\r\n");
    #if FF_VOLUMES == 2
        printf("\t\t VolumnStr == %s %s\r\n", volume_str[0],volume_str[1]);
    #else
        printf("\t\t VolumnStr == %s/%s/%s\r\n", volume_str[0],volume_str[1],volume_str[2]);
    #endif
        return 0;
    }
#endif

    res = f_unlink(path);
    if (res)
    {
        printf("[FS]: Folder Delete Fail - res(%u)\r\n", res);
        return 0;
    }
    printf("[FS]: Delete - Path(%s) \r\n", path);

    return 0;

}


/****************************************************************************
 *
 * API variable.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * API functions.
 *
 ****************************************************************************/

cmd_t ff_cli[] = {
    { "mount"    ,  "Mount FatFS System" , _ff_cli_mount,    NULL     },
    { "mkdir"    ,  "Create Folder"      , _ff_cli_mkdir,    NULL     },
    { "read"     ,  "platform ut command", _ff_cli_read,     NULL     },
    { "write"    ,  "platform ut command", _ff_cli_write,    NULL     },
    { "ls"       ,  "platform ut command", _ff_cli_ls,       NULL     },
    { "delete"   ,  "platform ut command", _ff_cli_delete,   NULL     },
    { NULL }
};

#endif /* #if defined(MTK_MINICLI_ENABLE) */