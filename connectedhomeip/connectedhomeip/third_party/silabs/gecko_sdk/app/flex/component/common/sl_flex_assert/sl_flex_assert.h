/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_FLEX_ASSERT_H
#define SL_FLEX_ASSERT_H

#include "stdint.h"
#include "sl_flex_assert_config.h"
#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_PRINTF_PRESENT)
  #include "printf.h"
#endif

#if ASSERT_ENABLE
#if defined(SL_CATALOG_PRINTF_PRESENT)
  #define APP_ASSERT(expr, ...)                                       \
  do {                                                                \
    if (!(expr)) {                                                    \
      printf("[Error]\n%s:%d: %s()\n", __FILE__, __LINE__, __func__); \
      printf("   ");                                                  \
      printf(__VA_ARGS__);                                            \
      printf("\n");                                                   \
      while (1) ;                                                     \
    }                                                                 \
  } while (0)
#else
#warning "printf component is not added to the project: APP_ASSERT() doesn't print"
 #define APP_ASSERT(expr, ...) \
  do {                         \
    if (!(expr)) {             \
      while (1) ;              \
    }                          \
  } while (0)
#endif
#else
  #define APP_ASSERT(expr, ...)
#endif

#if WARNING_ENABLE
#if defined(SL_CATALOG_PRINTF_PRESENT)
  #define APP_WARNING(expr, ...)                                           \
  do {                                                                     \
    if (!(expr)) {                                                         \
      printf("[Warning]\n   %s:%d: %s()\n", __FILE__, __LINE__, __func__); \
      printf("   ");                                                       \
      printf(__VA_ARGS__);                                                 \
      printf("\n");                                                        \
    }                                                                      \
  } while (0)
#else
  #warning "printf component is not added to the project: APP_WARNING() doesn't print"
  #define APP_WARNING(expr, ...)
#endif
#else
  #define APP_WARNING(expr, ...)
#endif

#if INFO_ENABLE
#if defined(SL_CATALOG_PRINTF_PRESENT)
  #define APP_INFO(...)         printf(__VA_ARGS__)
static inline void APP_INFO_BUFFER(const char *format, const uint8_t *buff, uint16_t len)
{
  for (uint16_t i = 0; i < len; i++) {
    printf(format, buff[i]);
  }
}
#else
  #warning "printf component is not added to the project: APP_INFO() doesn't print"
  #define APP_INFO(...)
static inline void APP_INFO_BUFFER(const char *format, const uint8_t *buff, uint16_t len)
{
}
#endif
#else
  #define APP_INFO(...)
#endif

#endif // SL_FLEX_ASSERT_H
