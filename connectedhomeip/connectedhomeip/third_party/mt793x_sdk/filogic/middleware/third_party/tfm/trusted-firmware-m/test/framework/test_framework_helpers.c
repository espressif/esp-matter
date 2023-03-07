/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_framework_helpers.h"

#include <stdio.h>

const char *asset_perms_to_str(uint8_t permissions)
{
    switch (permissions) {
    case 0:
        return "No permissions";
    case 1:
        return "SECURE_ASSET_REFERENCE";
    case 2:
        return "SECURE_ASSET_WRITE";
    case 3:
        return "SECURE_ASSET_REFERENCE | SECURE_ASSET_WRITE";
    case 4:
        return "SECURE_ASSET_READ";
    case 5:
        return "SECURE_ASSET_REFERENCE | SECURE_ASSET_READ";
    case 6:
        return "SECURE_ASSET_WRITE | SECURE_ASSET_READ";
    case 7:
        return "SECURE_ASSET_REFERENCE | SECURE_ASSET_WRITE | "
               "SECURE_ASSET_READ";
    default:
        return "Unknown permissions";
    }
}

void printf_set_color(enum serial_color_t color_id)
{
    TEST_LOG("\33[3%dm", color_id);
}
