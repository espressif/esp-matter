/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTSDK_ALLOC_H_
#define IOTSDK_ALLOC_H_

void *iotsdk_alloc_malloc(size_t size);
void iotsdk_alloc_free(void *ptr);

#endif /* IOTSDK_ALLOC_H_ */
