/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMMON_CRC_H
#define COMMON_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

// crc32 based on public domain implementation by Björn Samuelsson
// see http://home.thep.lu.se/~bjorn/crc/
void crc32(const void *data, size_t n_bytes, uint32_t *crc);

#ifdef __cplusplus
}
#endif

#endif // COMMON_CRC_H
