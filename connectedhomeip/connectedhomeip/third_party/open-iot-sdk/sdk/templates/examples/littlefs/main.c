/*
 * Copyright (c) 2021-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/flash_api.h"
#include "lfs.h"

#include <stdio.h>

// Application-defined context for littlefs.

struct my_lfs_context {
    mdh_flash_t *flash_obj;
    uint32_t flash_addr;
};

// littlefs callbacks.
// Note: No overflow checks are performed, because littlefs has its own
// checks based on block_size and block_count from struct lfs_config.

static uint32_t to_flash_addr(const struct lfs_config *config, lfs_block_t block, lfs_off_t off)
{
    struct my_lfs_context *context = config->context;
    return context->flash_addr + block * config->block_size + off;
}

static int lfs_bd_read(const struct lfs_config *config, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct my_lfs_context *context = config->context;
    return mdh_flash_read(context->flash_obj, to_flash_addr(config, block, off), buffer, size);
}

static int
lfs_bd_prog(const struct lfs_config *config, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct my_lfs_context *context = config->context;
    return mdh_flash_program_page(context->flash_obj, to_flash_addr(config, block, off), buffer, size);
}

static int lfs_bd_erase(const struct lfs_config *config, lfs_block_t block)
{
    struct my_lfs_context *context = config->context;
    return mdh_flash_erase_sector(context->flash_obj, to_flash_addr(config, block, 0));
}

static int lfs_bd_sync(const struct lfs_config *config)
{
    (void)config;
    return 0;
}

// Platform-specific instantiation of internal flash
extern mdh_flash_t *get_example_flash();

int main()
{
    printf("littlefs example\r\n");

    mdh_flash_t *flash_obj = get_example_flash();

    // Use the end of the internal flash for littlefs which requires at
    // least two sectors.
    // Note: On a device with non-uniform sector sizes, an application
    // should configure the littlefs block size to be multiples of all
    // sector sizes in use. Without information of any particular hardware,
    // this generic example assumes the last two sectors to be equal in
    // size.
    uint32_t flash_end = mdh_flash_get_start_address(flash_obj) + mdh_flash_get_size(flash_obj);
    uint32_t sector_size = mdh_flash_get_sector_size(flash_obj, flash_end - 1);
    if (sector_size != mdh_flash_get_sector_size(flash_obj, flash_end - sector_size - 1)) {
        printf("This example requires two equally-sized sectors at the end of the flash\r\n");
        return -1;
    }

    const uint32_t block_count = 2;
    uint32_t lfs_size = block_count * sector_size;
    uint32_t flash_addr = flash_end - lfs_size;

    struct my_lfs_context context = {
        .flash_obj = flash_obj,
        .flash_addr = flash_addr,
    };

    // See lfs.h for documentation of the parameters.
    // Here we use minimum required values of block_cycles, cache_size
    // and lookahead_size.
    // As lfs_t and struct lfs_cfg are large, we place them in the
    // static memory which cannot be initialized with non-constant
    // variables, so we assign parameters after initialization.
    static lfs_t lfs;
    static struct lfs_config lfs_cfg;
    lfs_cfg.context = &context;
    lfs_cfg.read = lfs_bd_read;
    lfs_cfg.prog = lfs_bd_prog;
    lfs_cfg.erase = lfs_bd_erase;
    lfs_cfg.sync = lfs_bd_sync;
    lfs_cfg.read_size = 1;
    lfs_cfg.prog_size = mdh_flash_get_page_size(flash_obj);
    lfs_cfg.block_size = sector_size;
    lfs_cfg.block_count = block_count;
    lfs_cfg.block_cycles = 100;
    lfs_cfg.cache_size = lfs_cfg.prog_size;
    lfs_cfg.lookahead_size = 8;

    // All variables need to be declared before "goto"
    int ret;
    lfs_file_t file;
    const char path[] = "test";
    const char fin[] = "test";
    char fout[5];
    int close_ret = 0;
    int unmount_ret = 0;

    ret = lfs_format(&lfs, &lfs_cfg);
    if (ret < 0) {
        printf("lfs_format() failed. Error %d\r\n", ret);
        return -1;
    }
    printf("littlefs formatted\r\n");

    ret = lfs_mount(&lfs, &lfs_cfg);
    if (ret < 0) {
        printf("lfs_mount() failed. Error %d\r\n", ret);
        return -1;
    }
    printf("littlefs mounted\r\n");

    ret = lfs_file_open(&lfs, &file, path, LFS_O_RDWR | LFS_O_CREAT);
    if (ret < 0) {
        printf("lfs_file_open() failed. Error %d\r\n", ret);
        goto unmount;
    }
    printf("File opened\r\n");

    ret = lfs_file_write(&lfs, &file, fin, sizeof(fin));
    if (ret < 0) {
        printf("lfs_file_write() failed. Error %d\r\n", ret);
        goto close;
    }
    printf("File written: %d bytes\r\n", ret);

    ret = lfs_file_rewind(&lfs, &file);
    if (ret < 0) {
        printf("lfs_file_rewind() failed. Error %d\r\n", ret);
        goto close;
    }
    printf("File rewound\r\n");

    ret = lfs_file_read(&lfs, &file, fout, sizeof(fout));
    if (ret < 0) {
        printf("lfs_file_read() failed. Error %d\r\n", ret);
        goto close;
    }
    printf("File read: %d bytes\r\n", ret);

close:
    close_ret = lfs_file_close(&lfs, &file);
    if (close_ret < 0) {
        printf("lfs_file_close() failed. Error %d\r\n", close_ret);
    } else {
        printf("File closed\r\n");
    }

    // Note the fallthrough here, so both close and unmount are performed
    // upon "goto close". If we fail somewhere before we open the file,
    // we can "goto unmount" and skip the close step.

unmount:
    unmount_ret = lfs_unmount(&lfs);
    if (unmount_ret < 0) {
        printf("lfs_unmount() failed. Error %d\r\n", ret);
    } else {
        printf("littlefs unmounted\r\n");
    }

    if ((ret < 0) || (close_ret < 0) || (unmount_ret < 0)) {
        return -1;
    }

    // Compare write and read buffers
    if (memcmp(fin, fout, sizeof(fin)) != 0) {
        printf("Error: file content does not match data written\r\n");
        return -1;
    }
    printf("File content matches data written\r\n");

    return 0;
}
