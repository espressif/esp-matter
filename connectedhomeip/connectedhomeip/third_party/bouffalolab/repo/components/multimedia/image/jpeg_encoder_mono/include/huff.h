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

#ifndef JPEC_HUFFMAN_H
#define JPEC_HUFFMAN_H

#include <stdint.h>

#include <jpec.h>
#include "enc.h"
#include "buf.h"

/** Entropy coding data that hold state along blocks */
typedef struct jpec_huff_state_t_ {
  int32_t buffer;             /* bits buffer */
  int nbits;                  /* number of bits remaining in buffer */
  int dc;                     /* DC coefficient from previous block (or 0) */
  jpec_buffer_t *buf;         /* JPEG global buffer */
} jpec_huff_state_t;

/** Type of an Huffman JPEG encoder */
typedef struct jpec_huff_t_ {
  jpec_huff_state_t state;    /* state from previous block encoding */
} jpec_huff_t;

/** Skeleton initialization */
void jpec_huff_skel_init(jpec_huff_skel_t *skel);

jpec_huff_t *jpec_huff_new(void);

void jpec_huff_del(jpec_huff_t *h);

void jpec_huff_encode_block(jpec_huff_t *h, jpec_block_t *block, jpec_buffer_t *buf);

#endif
