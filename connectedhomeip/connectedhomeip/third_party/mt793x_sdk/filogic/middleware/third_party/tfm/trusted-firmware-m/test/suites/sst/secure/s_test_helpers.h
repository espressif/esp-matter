/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __S_TEST_HELPERS_H__
#define __S_TEST_HELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Several tests use a buffer to read back data from an asset. This buffer is
 * larger than the size of the asset data by PADDING_SIZE bytes. This allows
 * us to ensure that the only the expected data is read back and that it is read
 * back correctly.
 *
 * For example if the buffer and asset are as follows:
 * Buffer - "XXXXXXXXXXXX", Asset data - "AAAA"
 *
 * Then a correct and successful read would give this result: "XXXXAAAAXXXX"
 * (Assuming a PADDING_SIZE of 8)
 */
#define BUFFER_SIZE 24
#define PADDING_SIZE 8
#define HALF_PADDING_SIZE 4

#define BUFFER_PLUS_PADDING_SIZE (BUFFER_SIZE + PADDING_SIZE)
#define BUFFER_PLUS_HALF_PADDING_SIZE (BUFFER_SIZE + HALF_PADDING_SIZE)

#ifdef __cplusplus
}
#endif

#endif /* __S_TEST_HELPERS_H__ */
