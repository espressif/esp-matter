/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>

void *iotsdk_alloc_malloc(size_t size)
{
    return malloc(size);
}

void iotsdk_alloc_free(void *ptr)
{
    free(ptr);
}
