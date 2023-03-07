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

#include <huff.h>
#include <conf.h>

#ifdef WORD_BIT
#define JPEC_INT_WIDTH WORD_BIT
#else
#define JPEC_INT_WIDTH (int)(sizeof(int) * CHAR_BIT)
#endif

#if __GNUC__
#define JPEC_HUFF_NBITS(JPEC_nbits, JPEC_val) \
  JPEC_nbits = (!JPEC_val) ? 0 : JPEC_INT_WIDTH - __builtin_clz(JPEC_val)
#else
#define JPEC_HUFF_NBITS(JPEC_nbits, JPEC_val) \
  JPEC_nbits = 0; \
  while (val) { \
    JPEC_nbits++; \
    val >>= 1; \
  }
#endif

/* Private function prototypes */
static void jpec_huff_encode_block_impl(jpec_block_t *block, jpec_huff_state_t *s);
static void jpec_huff_write_bits(jpec_huff_state_t *s, unsigned int bits, int n);

void jpec_huff_skel_init(jpec_huff_skel_t *skel) {
  assert(skel);
  memset(skel, 0, sizeof(*skel));
  skel->opq = jpec_huff_new();
  skel->del = (void (*)(void *))jpec_huff_del;
  skel->encode_block = (void (*)(void *, jpec_block_t *, jpec_buffer_t *))jpec_huff_encode_block;
}

jpec_huff_t *jpec_huff_new(void) {
  jpec_huff_t *h = malloc(sizeof(*h));
  h->state.buffer = 0;
  h->state.nbits = 0;
  h->state.dc = 0;
  h->state.buf = NULL;
  return h;
}

void jpec_huff_del(jpec_huff_t *h) {
  assert(h);
  /* Flush any remaining bits and fill in the incomple byte (if any) with 1-s */
  jpec_huff_write_bits(&h->state, 0x7F, 7);
  free(h);
}

void jpec_huff_encode_block(jpec_huff_t *h, jpec_block_t *block, jpec_buffer_t *buf) {
  assert(h && block && buf);
  jpec_huff_state_t state;
  state.buffer = h->state.buffer;
  state.nbits = h->state.nbits;
  state.dc = h->state.dc;
  state.buf = buf;
  jpec_huff_encode_block_impl(block, &state);
  h->state.buffer = state.buffer;
  h->state.nbits = state.nbits;
  h->state.dc = state.dc;
  h->state.buf = state.buf;
}

static void jpec_huff_encode_block_impl(jpec_block_t *block, jpec_huff_state_t *s) {
  assert(block && s);
  int val, bits, nbits;
  /* DC coefficient encoding */
  if (block->len > 0) {
    val = block->zz[0] - s->dc;
    s->dc = block->zz[0];
  }
  else {
    val = -s->dc;
    s->dc = 0;
  }
  bits = val;
  if (val < 0) {
    val = -val;
    bits = ~val;
  }
  JPEC_HUFF_NBITS(nbits, val);  
  jpec_huff_write_bits(s, jpec_dc_code[nbits], jpec_dc_len[nbits]);
  if (nbits) jpec_huff_write_bits(s, (unsigned int) bits, nbits);
  /* AC coefficients encoding (w/ RLE of zeros) */
  int nz = 0;
  for (int i = 1; i < block->len; i++) {
    if ((val = block->zz[i]) == 0) nz++;
    else {
      while (nz >= 16) {
        jpec_huff_write_bits(s, jpec_ac_code[0xF0], jpec_ac_len[0xF0]); /* ZRL code */
        nz -= 16;
      }
      bits = val;
      if (val < 0) {
        val = -val;
        bits = ~val;
      }
      JPEC_HUFF_NBITS(nbits, val);
      int j = (nz << 4) + nbits;
      jpec_huff_write_bits(s, jpec_ac_code[j], jpec_ac_len[j]);
      if (nbits) jpec_huff_write_bits(s, (unsigned int) bits, nbits);
      nz = 0;
    }
  }
  if (block->len < 64) {
    jpec_huff_write_bits(s, jpec_ac_code[0x00], jpec_ac_len[0x00]); /* EOB marker */
  }
}

/* Write n bits into the JPEG buffer, with 0 < n <= 16.
 *
 * == Details
 * - 16 bits are large enough to hold any zig-zag coeff or the longest AC code
 * - bits are chunked into bytes before being written into the JPEG buffer
 * - any remaining bits are kept in memory by the Huffman state
 * - at most 7 bits can be kept in memory
 * - a 32-bit integer buffer is used internally
 * - only the right 24 bits part of this buffer are used
 * - the input bits and remaining bits (if any) are left-justified on this part
 * - a mask is used to mask off any extra bits: useful when the input value has been
 *   first transformed by bitwise complement(|initial value|)
 * - if an 0xFF byte is detected a 0x00 stuff byte is automatically written right after
 */
static void jpec_huff_write_bits(jpec_huff_state_t *s, unsigned int bits, int n) {
  assert(s && n > 0 && n <= 16);
  int32_t mask = (((int32_t) 1) << n) - 1;
  int32_t buffer = (int32_t) bits;
  int nbits = s->nbits + n;
  buffer &= mask;
  buffer <<= 24 - nbits;
  buffer |= s->buffer;
  while (nbits >= 8) {
   int chunk = (int) ((buffer >> 16) & 0xFF);
   jpec_buffer_write_byte(s->buf, chunk);
   if (chunk == 0xFF) jpec_buffer_write_byte(s->buf, 0x00);
   buffer <<= 8;
   nbits -= 8;
  }
  s->buffer = buffer;
  s->nbits = nbits;
}
