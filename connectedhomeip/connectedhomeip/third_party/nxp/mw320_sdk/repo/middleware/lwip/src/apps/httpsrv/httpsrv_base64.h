/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
*  Base64 encode and decode header.
*/
#ifndef __httpsrv_base64_h__
#define __httpsrv_base64_h__

#include "lwip/inet.h"
#ifdef __cplusplus
extern "C" {
#endif

char *base64_encode(char *source, char *destination);
char *base64_decode(char *dst, char *src, uint32_t dst_size);
char *base64_encode_binary(char *source, char *destination, uint32_t length);
bool isbase64(const char *string);

#ifdef __cplusplus
}
#endif

#endif
