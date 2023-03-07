#ifndef __GMAC_TEST_H__
#define __GMAC_TEST_H__

#include <stddef.h>
#include <stdint.h>
#include <bl_sec.h>
#include <bl602_sec_eng.h>
#include <mbedtls/gcm.h>
#include "malloc_hw.h"

#ifdef __cplusplus
extern "C" {
#endif

void gmac_mult (const uint8_t *input, uint8_t output[16], uint32_t len);

void gmac_test (uint32_t length, uint32_t count, ram_type_t type);

#ifdef __cplusplus
}
#endif

#endif
