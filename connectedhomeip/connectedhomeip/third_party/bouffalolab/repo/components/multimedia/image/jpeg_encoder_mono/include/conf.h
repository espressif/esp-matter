/**
 * Copyright (c) 2012-2016 Moodstocks SAS
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef JPEC_CONF_H
#define JPEC_CONF_H

/* Common headers */
#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <FreeRTOS.h>

/** Standard JPEG quantizing table */
extern const uint8_t jpec_qzr[64];

/** DCT coefficients */
extern const float jpec_dct[7];

/** Zig-zag order */
extern const int jpec_zz[64];

/** JPEG standard Huffman tables */
/** Luminance (Y) - DC */
extern const uint8_t jpec_dc_nodes[17];
extern const int jpec_dc_nb_vals;
extern const uint8_t jpec_dc_vals[12];
/** Luminance (Y) - AC */
extern const uint8_t jpec_ac_nodes[17];
extern const int jpec_ac_nb_vals;
extern const uint8_t jpec_ac_vals[162];

/** Huffman inverted tables */
/** Luminance (Y) - DC */
extern const uint8_t jpec_dc_len[12];
extern const int jpec_dc_code[12];
/** Luminance (Y) - AC */
extern const int8_t jpec_ac_len[256];
extern const int jpec_ac_code[256];

#define malloc      pvPortMalloc

#define free        vPortFree

#define realloc     pvPortRealloc

#define JPEC_APT_TEST 1
#endif
