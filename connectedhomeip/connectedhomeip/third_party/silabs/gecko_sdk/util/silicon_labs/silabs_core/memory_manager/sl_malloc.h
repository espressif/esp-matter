/***************************************************************************//**
 * @file
 * @brief This is a simple wrapper for the stdlib memory management functions
 *   like malloc, calloc, realloc and free in order to make them
 *   thread safe.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_MALLOC_H
#define SL_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

void *sl_malloc(size_t size);
void *sl_calloc(size_t nmemb, size_t size);
void *sl_realloc(void * ptr, size_t size);
void sl_free(void * ptr);

#ifdef __cplusplus
}
#endif

#endif // SL_MALLOC_H
