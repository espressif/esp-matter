/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>
#include <hal_boot2.h>
#include <hal_sys.h>
#include <utils_sha256.h>
#include <hosal_ota.h>
#include <bl_mtd.h>
#include <blog.h>

typedef struct ota_parm_s 
{
    bl_mtd_handle_t mtd_handle;
    uint32_t file_size;
    uint32_t sector_erased[((HOSAL_OTA_FILE_SIZE_MAX + 4095)/4096 + 31)/32];
}hosal_ota_parm_t;

static hosal_ota_parm_t *ota_parm = NULL; 

static int hosal_ota_erase(uint32_t offset, uint32_t len)
{
    uint32_t start_index, end_index, index;
    int ret;
    
    start_index = offset / 4096;
    end_index = (offset + len - 1) / 4096;
    
    for (index = start_index; index <= end_index; index++) {
        if ((ota_parm->sector_erased[index / 32] & (1U << (index % 32))) == 0) {
            ret = bl_mtd_erase(ota_parm->mtd_handle, index * 4096, 4096);
            if (ret) {
                blog_error("mtd erase failed\r\n");
                return -1;
            }
            ota_parm->sector_erased[index / 32] |= (1U << (index % 32));
        }
    }
    
    return 0;
}

int hosal_ota_start(uint32_t file_size)
{
    int ret;
    uint32_t bin_size, ota_addr;
    HALPartition_Entry_Config ptEntry;
   
    if (ota_parm != NULL) {
        blog_info("ota had start\r\n");
        return 0;
    }
    
    ota_parm = (hosal_ota_parm_t *)aos_malloc(sizeof(hosal_ota_parm_t));
    if (NULL == ota_parm) {
        blog_error("have not enough memory\r\n");
        return -1;
    }
    memset(ota_parm, 0, sizeof(hosal_ota_parm_t));

    ota_parm->file_size = file_size;

    ret = bl_mtd_open(BL_MTD_PARTITION_NAME_FW_DEFAULT, &ota_parm->mtd_handle, BL_MTD_OPEN_FLAG_BACKUP);
    if (ret) {
        aos_free(ota_parm);
        ota_parm = NULL;
        blog_error("Open Default FW partition failed\r\n");
        return -1;
    }

    if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &ptEntry)) {
        bl_mtd_close(ota_parm->mtd_handle);
        aos_free(ota_parm);
        ota_parm = NULL;
        
        blog_error("PtTable_Get_Active_Entries fail\r\n");
        return -1;
    }
    ota_addr = ptEntry.Address[!ptEntry.activeIndex];
    bin_size = ptEntry.maxLen[!ptEntry.activeIndex];
    
    if (file_size > bin_size) {
        blog_error("file size is more than partition size\r\n");
        bl_mtd_close(ota_parm->mtd_handle);
        aos_free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    blog_info("Starting OTA test. OTA size is %lu\r\n", bin_size);
    blog_info("[OTA] [TEST] activeIndex is %u, use OTA address=%08x\r\n", ptEntry.activeIndex, (unsigned int)ota_addr);
#if 0
    blog_info("[OTA] [TEST] Erase flash with size %lu...", bin_size);
 
    hal_update_mfg_ptable();
    ret = bl_mtd_erase_all(ota_parm->mtd_handle);
    if (ret) {
        bl_mtd_close(ota_parm->mtd_handle);
        aos_free(ota_parm);
        ota_parm = NULL;
        blog_error("erase flash error\r\n");
        return -1;
    }
    blog_info("Done\r\n");
#endif
    return ret;
}

int hosal_ota_update(uint32_t filesize, uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    uint32_t file_size, ret;
    if (ota_parm == NULL) {
        blog_error("please start ota first\r\n");
        return -1;
    }
    
    file_size = ota_parm->file_size;
    if ((filesize > file_size) || (NULL == buf) || ((offset + buf_len) > file_size) || (buf_len == 0)) {
        blog_error("parm is error!\r\n");
        return -1;
    }
    
    ret = hosal_ota_erase(offset, buf_len);
    if (ret) {
        blog_error("erase failed\r\n");
        return -1;
    }
    
    ret = bl_mtd_write(ota_parm->mtd_handle, offset, buf_len, buf);
    if (ret) {
        blog_error("mtd write failed\r\n");
        return -1;
    }
    
    return ret;
}

int hosal_ota_finish(uint8_t check_hash, uint8_t auto_reset)
{
    if (ota_parm == NULL) {
        blog_error("please start ota first\r\n");
        return -1;
    }
    uint32_t bin_size; 
    HALPartition_Entry_Config ptEntry;
   
    if (ota_parm->file_size <= 32) {
        bl_mtd_close(ota_parm->mtd_handle);
        aos_free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    bin_size = ota_parm->file_size - 32;

    if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &ptEntry)) {
        blog_error("PtTable_Get_Active_Entries fail\r\n");
        bl_mtd_close(ota_parm->mtd_handle);
        aos_free(ota_parm);
        ota_parm = NULL;
        return -1;
    }
    blog_info("[OTA] prepare OTA partition info\r\n");

    if (check_hash) {
#define CHECK_IMG_BUF_SIZE   512
        uint8_t sha_check[32] = {0};
        uint8_t dst_sha[32] = {0};
        uint32_t read_size;
        iot_sha256_context sha256_ctx;
        int i, offset = 0;
        uint8_t *r_buf;

        r_buf = aos_malloc(CHECK_IMG_BUF_SIZE);
        if (r_buf == NULL) {
            bl_mtd_close(ota_parm->mtd_handle);
            aos_free(ota_parm);
            ota_parm = NULL;
            blog_error("malloc error\r\n");
            return -1;
        }

        utils_sha256_init(&sha256_ctx);
        utils_sha256_starts(&sha256_ctx);
    
        memset(sha_check, 0, 32);
        memset(dst_sha , 0, 32);
        offset = 0;
        while (offset < bin_size) {
            (bin_size - offset >= CHECK_IMG_BUF_SIZE) ? (read_size = CHECK_IMG_BUF_SIZE):(read_size = bin_size - offset);
            if (bl_mtd_read(ota_parm->mtd_handle, offset, read_size, r_buf)) {
                blog_error("mtd read failed\r\n");
                bl_mtd_close(ota_parm->mtd_handle);
                aos_free(ota_parm);
                ota_parm = NULL;
                aos_free(r_buf);
                return -1;
            }
            
            utils_sha256_update(&sha256_ctx, (const uint8_t *)r_buf, read_size);
            offset += read_size;
        }

        utils_sha256_finish(&sha256_ctx, sha_check);
        aos_free(r_buf);

        bl_mtd_read(ota_parm->mtd_handle, offset, 32, dst_sha);
        for (i = 0; i < 32; i++) {
            blog_info("%02X", dst_sha[i]);
        }
        puts("\r\nHeader SET SHA256 Checksum:");
        for (i = 0; i < 32; i++) {
            blog_info("%02X", sha_check[i]);
        }

        if (memcmp(sha_check, (const void *)dst_sha, 32) != 0) {
            blog_error("sha256 check error\r\n");
            bl_mtd_close(ota_parm->mtd_handle);
            aos_free(ota_parm);
            ota_parm = NULL;
            utils_sha256_free(&sha256_ctx);
            return -1;
        }

        utils_sha256_free(&sha256_ctx);
    } 
    
    ptEntry.len = bin_size;
    blog_info("[OTA] [TCP] Update PARTITION, partition len is %lu\r\n", ptEntry.len);
    hal_boot2_update_ptable(&ptEntry);
    bl_mtd_close(ota_parm->mtd_handle);
    aos_free(ota_parm);
    ota_parm = NULL;
    
    if (auto_reset) {
        hal_reboot();
    }
    
    return 0;
}

int hosal_ota_read(uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    if (ota_parm == NULL) {
        printf("please start ota first\r\n");
        return -1;
    }

    if ((NULL == buf) || ((offset + buf_len) > ota_parm->file_size) || (buf_len == 0)) {
        printf("parm is error!\r\n");
        return -1;
    }

    return bl_mtd_read(ota_parm->mtd_handle, offset, buf_len, buf);
}

