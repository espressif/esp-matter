/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "flash_cs300.h"

mdh_flash_t *get_example_flash()
{
    return get_ram_drive_instance();
}
