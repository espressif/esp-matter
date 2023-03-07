/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "partition.h"

#ifndef NETWORK_FLASH_STORAGE_H
#define NETWORK_FLASH_STORAGE_H

uint32_t init_flash_storage(char *filename, flash_desc_t *pFlashDesc);

uint32_t save_wifi_network(char *filename, uint8_t *network, uint32_t len);

uint32_t get_saved_wifi_network(char *filename, uint8_t *network, uint32_t *len);

uint32_t reset_saved_wifi_network(char *filename);

/* Added: */
uint32_t erase_all_params(void);

#endif
