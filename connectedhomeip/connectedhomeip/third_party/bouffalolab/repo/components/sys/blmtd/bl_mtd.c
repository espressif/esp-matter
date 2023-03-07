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
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>

#include <hal_boot2.h>
#include <bl_sys.h>
#include <bl_flash.h>
#include <bl_mtd.h>
#include <utils_log.h>

struct bl_mtd_handle_priv {
    char name[16];
    int id;
    unsigned int offset;
    unsigned int size;
    void *xip_addr;
};
typedef struct bl_mtd_handle_priv *bl_mtd_handle_priv_t;

static void __dump_mtd_handle(bl_mtd_handle_priv_t handle_prv)
{
    puts(  "[MTD] >>>>>> Hanlde info Dump >>>>>>\r\n");
    printf("      name %s\r\n", handle_prv->name);
    printf("      id %d\r\n", handle_prv->id);
    printf("      offset %p(%u)\r\n", (void*)handle_prv->offset, handle_prv->offset);
    printf("      size %p(%uKbytes)\r\n", (void*)handle_prv->size, handle_prv->size/1024);
    printf("      xip_addr %p\r\n", handle_prv->xip_addr);
    puts(  "[MTD] <<<<<< Hanlde info End <<<<<<\r\n");
}

static int _mtd_write_copy2ram(uint32_t addr, uint8_t *src, unsigned int len)
{
    uint8_t buf_tmp[64];
    unsigned int len_tmp;

    while (len > 0) {
        if (len >= 64) {
            len_tmp = 64;
        } else {
            len_tmp = len;
        }

        memcpy(buf_tmp, src, len_tmp);

        bl_flash_write(addr, buf_tmp, len_tmp);

        addr += len_tmp;
        src += len_tmp;
        len -= len_tmp;
    }

    return 0;
}

static int _mtd_write(uint32_t addr, uint8_t *src, unsigned int len)
{

    bl_flash_write(addr, src, len);

    return 0;
}

int bl_mtd_open(const char *name, bl_mtd_handle_t *handle, unsigned int flags)
{
    uint32_t addr = 0;
    uint32_t size = 0;
    int ret;
    bl_mtd_handle_priv_t handle_prv;

    handle_prv = (bl_mtd_handle_priv_t)pvPortMalloc(sizeof(struct bl_mtd_handle_priv));

    if (NULL == handle_prv) {
        return -1;
    }
    memset(handle_prv, 0, sizeof(struct bl_mtd_handle_priv));
    strncpy(handle_prv->name, name, sizeof(handle_prv->name));

    if (flags & BL_MTD_OPEN_FLAG_BACKUP) {
        /* open backup mtd partition*/
        if (flags & BL_MTD_OPEN_FLAG_BUSADDR) {
            if ((ret = hal_boot2_partition_bus_addr_inactive(name, &addr, &size)) || 0 == addr) {
                printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
                printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
                while (1) {
                }
            }
            handle_prv->xip_addr = (void*)addr;
        } else {
            handle_prv->xip_addr = 0;
        }
        if ((ret = hal_boot2_partition_addr_inactive(name, &addr, &size)) || 0 == addr) {
            printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
            printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
            while (1) {
            }
        }
        handle_prv->offset = addr;
        handle_prv->size = size;
    } else {
        /* open active mtd partition*/
        if (flags & BL_MTD_OPEN_FLAG_BUSADDR) {
            if ((ret = hal_boot2_partition_bus_addr_active(name, &addr, &size)) || 0 == addr) {
                printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
                printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
                while (1) {
                }
            }
            handle_prv->xip_addr = (void*)addr;
        } else {
            handle_prv->xip_addr = 0;
        }
        if ((ret = hal_boot2_partition_addr_active(name, &addr, &size)) || 0 == addr) {
            printf("[MTD] [PART] [XIP] error when get %s partition %d\r\n", name, ret);
            printf("[MTD] [PART] [XIP] Dead Loop. Reason: no Valid %s partition found\r\n", name);
            while (1) {
            }
        }
        handle_prv->offset = addr;
        handle_prv->size = size;
    }
    __dump_mtd_handle(handle_prv);
    *handle = handle_prv;

    return 0;
}

int bl_mtd_close(bl_mtd_handle_t handle)
{
    vPortFree(handle);

    return 0;
}

int bl_mtd_info(bl_mtd_handle_t handle, bl_mtd_info_t *info)
{
    bl_mtd_handle_priv_t handle_prv = (bl_mtd_handle_priv_t)handle;

    strcpy(info->name, handle_prv->name);
    info->offset = handle_prv->offset;
    info->size = handle_prv->size;
    info->xip_addr = handle_prv->xip_addr;

    return 0;
}

int bl_mtd_erase(bl_mtd_handle_t handle, unsigned int addr, unsigned int size)
{
    bl_mtd_handle_priv_t handle_prv = (bl_mtd_handle_priv_t)handle;

    bl_flash_erase(
            handle_prv->offset + addr,
            size
    );

    return 0;
}

int bl_mtd_erase_all(bl_mtd_handle_t handle)
{
    bl_mtd_handle_priv_t handle_prv = (bl_mtd_handle_priv_t)handle;

    bl_flash_erase(
            handle_prv->offset + 0,
            handle_prv->size
    );

    return 0;
}

int bl_mtd_write(bl_mtd_handle_t handle, unsigned int addr, unsigned int size, const uint8_t *data)
{
    bl_mtd_handle_priv_t handle_prv = (bl_mtd_handle_priv_t)handle;
    uint32_t real_addr = handle_prv->offset + addr;

    if (bl_sys_isxipaddr((uint32_t)data)) {
        log_warn("addr@%p is xip flash, size %d\r\n", data, size);
        _mtd_write_copy2ram(real_addr, (uint8_t*)data, size);
    } else {
        _mtd_write(real_addr, (uint8_t*)data, size);
    }

    return 0;
}

int bl_mtd_read(bl_mtd_handle_t handle,  unsigned int addr, unsigned int size, uint8_t *data)
{
    bl_mtd_handle_priv_t handle_prv = (bl_mtd_handle_priv_t)handle;

    bl_flash_read(
            handle_prv->offset + addr,
            data,
            size
    );

    return 0;
}

int bl_mtd_size(bl_mtd_handle_t handle, unsigned int *size)
{
    bl_mtd_handle_priv_t handle_prv = (bl_mtd_handle_priv_t)handle;
    if (NULL == handle) {
        return -1;
    }
    *size = handle_prv->size;

    return 0;
}
