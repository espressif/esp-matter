#pragma once

#include <stdint.h>
#include <stddef.h>

#define MBEDTLS_HW_ACC_DBG 1


#define HW_MAX(x, y) ((x) > (y) ? (x) : (y))

#define HW_PORTED

#include <bl_sec.h>

#ifdef BL602
#include <bl602_sec_eng.h>
#elif defined BL616
#include <bl616_sec_eng.h>
#elif defined BL702
#include <bl702_sec_eng.h>
#elif defined BL808
#include <bl808_sec_eng.h>
#else
#error "Chip is not ported!"
#undef HW_PORTED
#endif

int mpi_words_to_reg_size(size_t words);
size_t mpi_reg_size_to_words(int reg_size);
#if MBEDTLS_HW_ACC_DBG
void dump_pka_reg(const char *tag, int s, uint8_t reg);
#else
#define dump_pka_reg(...) do {} while (0)
#endif
