/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_FILE__
#define __MFLASH_FILE__

#include "mflash_drv.h"
#include "mflash_params.h"

typedef struct
{
    uint32_t flash_addr;
    uint32_t max_size;
    char path[64];
} mflash_file_t;

bool mflash_is_initialized(void);

int mflash_init(mflash_file_t *user_file_table, bool init_drv);

int mflash_read_file(char *pcFileName, uint8_t **ppucData, uint32_t *pulDataSize);

int mflash_save_file(char *pcFileName, uint8_t *pucData, uint32_t ulDataSize);

#endif
