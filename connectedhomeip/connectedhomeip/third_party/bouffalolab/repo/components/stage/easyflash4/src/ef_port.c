/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <bl_mtd.h>
#include <ef_cfg.h>

static bl_mtd_handle_t handle;
uint32_t ENV_AREA_SIZE;
uint32_t SECTOR_NUM;

/* default environment variables set for user */
static const ef_env default_env_set[] = {
        {"boot_times", "3", 1}
};

static SemaphoreHandle_t env_cache_lock = NULL;

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    int ret;
    bl_mtd_info_t info;

    ret = bl_mtd_open(BL_MTD_PARTITION_NAME_PSM, &handle, BL_MTD_OPEN_FLAG_BUSADDR);
    if (ret < 0) {
        EF_INFO("[EF] [PART] [XIP] error when get PSM partition %d\r\n", ret);
        puts("[EF] [PART] [XIP] Dead Loop. Reason: no Valid PSM partition found\r\n");
        while (1) {
        }
    }
    memset(&info, 0, sizeof(info));
    bl_mtd_info(handle, &info);
    EF_INFO("[EF] Found Valid PSM partition, XIP Addr %08x, flash addr %08x, size %d\r\n",
            info.xip_addr,
            info.offset,
            info.size
    );
    if (info.size < 8 * 1024) {
        printf("[ERROR]psm partition is less than 8k,easyflash can not work!");
        while(1);
    }
    ENV_AREA_SIZE = (info.size / EF_ERASE_MIN_SIZE) * EF_ERASE_MIN_SIZE;
    SECTOR_NUM = ENV_AREA_SIZE / EF_ERASE_MIN_SIZE;
    printf("ENV AREA SIZE %ld, SECTOR NUM %ld\r\n", ENV_AREA_SIZE, SECTOR_NUM);

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    printf("*default_env_size = 0x%08x\r\n", *default_env_size);

#if configUSE_RECURSIVE_MUTEXES
    env_cache_lock = xSemaphoreCreateRecursiveMutex();
#else
    env_cache_lock = xSemaphoreCreateMutex();
#endif
    
    return EF_NO_ERR;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    //EF_ASSERT(size % 4 == 0);

    /* You can add your code under here. */
    if (bl_mtd_read(handle, addr, size, (uint8_t*)buf) < 0) {
        result = EF_READ_ERR;
    }
    
    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;

    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */
    if (bl_mtd_erase(handle, addr, size) < 0) {
        result = EF_ERASE_ERR;
    }

    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;

    //EF_ASSERT(size % 4 == 0);
    
    /* You can add your code under here. */
    if (bl_mtd_write(handle, addr, size, (const uint8_t*)buf) < 0) {
        result = EF_WRITE_ERR;
    }

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreTakeRecursive(env_cache_lock,
                 portMAX_DELAY);
#else
    /* You can add your code under here. */
    xSemaphoreTake( env_cache_lock,
                 portMAX_DELAY );
#endif

}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    
#if configUSE_RECURSIVE_MUTEXES
    xSemaphoreGiveRecursive(env_cache_lock);
#else
    /* You can add your code under here. */
    xSemaphoreGive( env_cache_lock );
#endif
}

extern void vprint(const char *fmt, va_list argp);
/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#ifdef PRINT_DEBUG

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vprint(format, args);

    va_end(args);

#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vprint(format, args);
    
    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vprint(format, args);
    
    va_end(args);
}
