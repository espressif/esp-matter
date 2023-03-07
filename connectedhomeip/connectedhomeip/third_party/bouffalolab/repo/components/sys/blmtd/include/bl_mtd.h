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
#ifndef __BL_MTD_H__
#define __BL_MTD_H__
#include <stdint.h>

typedef void *bl_mtd_handle_t;
typedef struct {
    char name[16];
    unsigned int offset;
    unsigned int size;
    void *xip_addr;
} bl_mtd_info_t;

#define BL_MTD_OPEN_FLAG_NONE           (0)
/* open backup partition */
#define BL_MTD_OPEN_FLAG_BACKUP        (1 << 0)
#define BL_MTD_OPEN_FLAG_BUSADDR       (1 << 1)
int bl_mtd_open(const char *name, bl_mtd_handle_t *handle, unsigned int flags);
int bl_mtd_close(bl_mtd_handle_t handle);
int bl_mtd_info(bl_mtd_handle_t handle, bl_mtd_info_t *info);
int bl_mtd_erase(bl_mtd_handle_t handle, unsigned int addr, unsigned int size);
int bl_mtd_erase_all(bl_mtd_handle_t handle);
int bl_mtd_write(bl_mtd_handle_t handle, unsigned int addr, unsigned int size, const uint8_t *data);
int bl_mtd_read(bl_mtd_handle_t handle,  unsigned int addr, unsigned int size, uint8_t *data);
int bl_mtd_size(bl_mtd_handle_t handle, unsigned int *size);

#define BL_MTD_PARTITION_NAME_PSM               "PSM"
#define BL_MTD_PARTITION_NAME_FW_DEFAULT        "FW"
#define BL_MTD_PARTITION_NAME_ROMFS             "media"

#endif
