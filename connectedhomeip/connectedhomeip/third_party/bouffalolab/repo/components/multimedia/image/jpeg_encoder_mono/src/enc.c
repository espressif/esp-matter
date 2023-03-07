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

#include <enc.h>
#include <huff.h>
#include <conf.h>

#if JPEC_APT_TEST
  #include "bl_timer.h"
  #include <blog.h>
#endif

#define JPEG_ENC_DEF_QUAL   93 /* default quality factor */
#define JPEC_ENC_HEAD_SIZ  330 /* header typical size in bytes */
#define JPEC_ENC_BLOCK_SIZ  30 /* 8x8 entropy coded block typical size in bytes */

/* Private function prototypes */
static void jpec_enc_init_dqt(jpec_enc_t *e);
static void jpec_enc_open(jpec_enc_t *e);
static void jpec_enc_close(jpec_enc_t *e);
static void jpec_enc_write_soi(jpec_enc_t *e);
static void jpec_enc_write_app0(jpec_enc_t *e);
static void jpec_enc_write_dqt(jpec_enc_t *e);
static void jpec_enc_write_sof0(jpec_enc_t *e);
static void jpec_enc_write_dht(jpec_enc_t *e);
static void jpec_enc_write_sos(jpec_enc_t *e);
static int jpec_enc_next_block(jpec_enc_t *e);
static void jpec_enc_block_dct(jpec_enc_t *e);
static void jpec_enc_block_quant(jpec_enc_t *e);
static void jpec_enc_block_zz(jpec_enc_t *e);

jpec_enc_t *jpec_enc_new(const uint8_t *img, uint16_t w, uint16_t h) {
  return jpec_enc_new2(img, w, h, JPEG_ENC_DEF_QUAL);
}

jpec_enc_t *jpec_enc_new2(const uint8_t *img, uint16_t w, uint16_t h, int q) {
  assert(img && w > 0 && !(w & 0x7) && h > 0 && !(h & 0x7));
  jpec_enc_t *e = malloc(sizeof(*e));
  e->img = img;
  e->w = w;
  e->w8 = (((w-1)>>3)+1)<<3;
  e->h = h;
  e->qual = q;
  e->bmax = (((w-1)>>3)+1) * (((h-1)>>3)+1);
  e->bnum = -1;
  e->bx = -1;
  e->by = -1;
  int bsiz = JPEC_ENC_HEAD_SIZ + e->bmax * JPEC_ENC_BLOCK_SIZ;
  e->buf = jpec_buffer_new2(bsiz);
  e->hskel = malloc(sizeof(*e->hskel));
  return e;
}

void jpec_enc_del(jpec_enc_t *e) {
  assert(e);
  if (e->buf) jpec_buffer_del(e->buf);
  free(e->hskel);
  free(e);
}

const uint8_t *jpec_enc_run(jpec_enc_t *e, int *len) {
  assert(e && len);
  jpec_enc_open(e);
#if JPEC_APT_TEST
  uint32_t start_us, dct_us = 0, huff_us = 0, block_us = 0;
#endif

  while (jpec_enc_next_block(e)) {
#if JPEC_APT_TEST
    start_us = bl_timer_now_us();
    jpec_enc_block_dct(e);
    dct_us += bl_timer_now_us() - start_us;

    start_us = bl_timer_now_us();
    jpec_enc_block_quant(e);
    jpec_enc_block_zz(e);
    block_us += bl_timer_now_us() - start_us;

    start_us = bl_timer_now_us();
    e->hskel->encode_block(e->hskel->opq, &e->block, e->buf);
    huff_us += bl_timer_now_us() - start_us;

  }
  blog_info("DCT time: %d us, Block time: %d us, HUF time: %d us, count %d\r\n",
            (dct_us / e->bmax), (block_us / e->bmax), (huff_us / e->bmax), e->bmax);
#else
    jpec_enc_block_dct(e);
    jpec_enc_block_quant(e);
    jpec_enc_block_zz(e);
    e->hskel->encode_block(e->hskel->opq, &e->block, e->buf);
  }
#endif
  
  jpec_enc_close(e);
  *len = e->buf->len;
  return e->buf->stream;
}

/* Update the internal quantization matrix according to the asked quality */
static void jpec_enc_init_dqt(jpec_enc_t *e) {
  assert(e);
  float qualf = (float) e->qual;
  float scale = (e->qual < 50) ? (50/qualf) : (2 - qualf/50);
  for (int i = 0; i < 64; i++) {
    int a = (int) ((float) jpec_qzr[i]*scale + 0.5);
    a = (a < 1) ? 1 : ((a > 255) ? 255 : a);
    e->dqt[i] = a;
  }
}

static void jpec_enc_open(jpec_enc_t *e) {
  assert(e);
  jpec_huff_skel_init(e->hskel);
  jpec_enc_init_dqt(e);
  jpec_enc_write_soi(e);
  jpec_enc_write_app0(e);
  jpec_enc_write_dqt(e);
  jpec_enc_write_sof0(e);
  jpec_enc_write_dht(e);
  jpec_enc_write_sos(e);
}

static void jpec_enc_close(jpec_enc_t *e) {
  assert(e);
  e->hskel->del(e->hskel->opq);
  jpec_buffer_write_2bytes(e->buf, 0xFFD9); /* EOI marker */
}

static void jpec_enc_write_soi(jpec_enc_t *e) {
  assert(e);
  jpec_buffer_write_2bytes(e->buf, 0xFFD8); /* SOI marker */
}

static void jpec_enc_write_app0(jpec_enc_t *e) {
  assert(e);
  jpec_buffer_write_2bytes(e->buf, 0xFFE0); /* APP0 marker */
  jpec_buffer_write_2bytes(e->buf, 0x0010); /* segment length */
  jpec_buffer_write_byte(e->buf, 0x4A);     /* 'J' */
  jpec_buffer_write_byte(e->buf, 0x46);     /* 'F' */
  jpec_buffer_write_byte(e->buf, 0x49);     /* 'I' */
  jpec_buffer_write_byte(e->buf, 0x46);     /* 'F' */
  jpec_buffer_write_byte(e->buf, 0x00);     /* '\0' */
  jpec_buffer_write_2bytes(e->buf, 0x0101); /* v1.1 */
  jpec_buffer_write_byte(e->buf, 0x00);     /* no density unit */
  jpec_buffer_write_2bytes(e->buf, 0x0001); /* X density = 1 */
  jpec_buffer_write_2bytes(e->buf, 0x0001); /* Y density = 1 */
  jpec_buffer_write_byte(e->buf, 0x00);     /* thumbnail width = 0 */
  jpec_buffer_write_byte(e->buf, 0x00);     /* thumbnail height = 0 */
}

static void jpec_enc_write_dqt(jpec_enc_t *e) {
  assert(e);
  jpec_buffer_write_2bytes(e->buf, 0xFFDB); /* DQT marker */
  jpec_buffer_write_2bytes(e->buf, 0x0043); /* segment length */
  jpec_buffer_write_byte(e->buf, 0x00);     /* table 0, 8-bit precision (0) */
  for (int i = 0; i < 64; i++) {
    jpec_buffer_write_byte(e->buf, e->dqt[jpec_zz[i]]);
  }
}

static void jpec_enc_write_sof0(jpec_enc_t *e) {
  assert(e);
  jpec_buffer_write_2bytes(e->buf, 0xFFC0); /* SOF0 marker */
  jpec_buffer_write_2bytes(e->buf, 0x000B); /* segment length */
  jpec_buffer_write_byte(e->buf, 0x08);     /* 8-bit precision */
  jpec_buffer_write_2bytes(e->buf, e->h);
  jpec_buffer_write_2bytes(e->buf, e->w);
  jpec_buffer_write_byte(e->buf, 0x01);     /* 1 component only (grayscale) */
  jpec_buffer_write_byte(e->buf, 0x01);     /* component ID = 1 */
  jpec_buffer_write_byte(e->buf, 0x11);     /* no subsampling */
  jpec_buffer_write_byte(e->buf, 0x00);     /* quantization table 0 */
}

static void jpec_enc_write_dht(jpec_enc_t *e) {
  assert(e);
  jpec_buffer_write_2bytes(e->buf, 0xFFC4);          /* DHT marker */
  jpec_buffer_write_2bytes(e->buf, 19 + jpec_dc_nb_vals); /* segment length */
  jpec_buffer_write_byte(e->buf, 0x00);              /* table 0 (DC), type 0 (0 = Y, 1 = UV) */
  for (int i = 0; i < 16; i++) {
    jpec_buffer_write_byte(e->buf, jpec_dc_nodes[i+1]);
  }
  for (int i = 0; i < jpec_dc_nb_vals; i++) {
    jpec_buffer_write_byte(e->buf, jpec_dc_vals[i]);
  }
  jpec_buffer_write_2bytes(e->buf, 0xFFC4);           /* DHT marker */
  jpec_buffer_write_2bytes(e->buf, 19 + jpec_ac_nb_vals);
  jpec_buffer_write_byte(e->buf, 0x10);               /* table 1 (AC), type 0 (0 = Y, 1 = UV) */
  for (int i = 0; i < 16; i++) {
    jpec_buffer_write_byte(e->buf, jpec_ac_nodes[i+1]);
  }
  for (int i = 0; i < jpec_ac_nb_vals; i++) {
    jpec_buffer_write_byte(e->buf, jpec_ac_vals[i]);
  }
}

static void jpec_enc_write_sos(jpec_enc_t *e) {
  assert(e);
  jpec_buffer_write_2bytes(e->buf, 0xFFDA); /* SOS marker */
  jpec_buffer_write_2bytes(e->buf, 8);      /* segment length */
  jpec_buffer_write_byte(e->buf, 0x01);     /* nb. components */
  jpec_buffer_write_byte(e->buf, 0x01);     /* Y component ID */
  jpec_buffer_write_byte(e->buf, 0x00);     /* Y HT = 0 */
  /* segment end */
  jpec_buffer_write_byte(e->buf, 0x00);
  jpec_buffer_write_byte(e->buf, 0x3F);
  jpec_buffer_write_byte(e->buf, 0x00);
}

static int jpec_enc_next_block(jpec_enc_t *e) {
  assert(e);
  int rv = (++e->bnum >= e->bmax) ? 0 : 1;
  if (rv) {
    e->bx =   (e->bnum << 3) % e->w8;
    e->by = ( (e->bnum << 3) / e->w8 ) << 3;
  }
  return rv;
}

static void jpec_enc_block_dct(jpec_enc_t *e) {
  assert(e && e->bnum >= 0);
#define JPEC_BLOCK(col,row) e->img[(((e->by + row) < e->h) ? e->by + row : e->h-1) * \
                            e->w + (((e->bx + col) < e->w) ? e->bx + col : e->w-1)]
  const float* coeff = jpec_dct;
  float tmp[64];
  for (int row = 0; row < 8; row++) {
    /* NOTE: the shift by 256 allows resampling from [0 255] to [–128 127] */
    float s0 = (float) (JPEC_BLOCK(0, row) + JPEC_BLOCK(7, row) - 256);
    float s1 = (float) (JPEC_BLOCK(1, row) + JPEC_BLOCK(6, row) - 256);
    float s2 = (float) (JPEC_BLOCK(2, row) + JPEC_BLOCK(5, row) - 256);
    float s3 = (float) (JPEC_BLOCK(3, row) + JPEC_BLOCK(4, row) - 256);

    float d0 = (float) (JPEC_BLOCK(0, row) - JPEC_BLOCK(7, row));
    float d1 = (float) (JPEC_BLOCK(1, row) - JPEC_BLOCK(6, row));
    float d2 = (float) (JPEC_BLOCK(2, row) - JPEC_BLOCK(5, row));
    float d3 = (float) (JPEC_BLOCK(3, row) - JPEC_BLOCK(4, row));

    tmp[8 * row]     = coeff[3]*(s0+s1+s2+s3);
    tmp[8 * row + 1] = coeff[0]*d0+coeff[2]*d1+coeff[4]*d2+coeff[6]*d3;
    tmp[8 * row + 2] = coeff[1]*(s0-s3)+coeff[5]*(s1-s2);
    tmp[8 * row + 3] = coeff[2]*d0-coeff[6]*d1-coeff[0]*d2-coeff[4]*d3;
    tmp[8 * row + 4] = coeff[3]*(s0-s1-s2+s3);
    tmp[8 * row + 5] = coeff[4]*d0-coeff[0]*d1+coeff[6]*d2+coeff[2]*d3;
    tmp[8 * row + 6] = coeff[5]*(s0-s3)+coeff[1]*(s2-s1);
    tmp[8 * row + 7] = coeff[6]*d0-coeff[4]*d1+coeff[2]*d2-coeff[0]*d3;
  }
  for (int col = 0; col < 8; col++) {
    float s0 = tmp[     col] + tmp[56 + col];
    float s1 = tmp[ 8 + col] + tmp[48 + col];
    float s2 = tmp[16 + col] + tmp[40 + col];
    float s3 = tmp[24 + col] + tmp[32 + col];

    float d0 = tmp[     col] - tmp[56 + col];
    float d1 = tmp[ 8 + col] - tmp[48 + col];
    float d2 = tmp[16 + col] - tmp[40 + col];
    float d3 = tmp[24 + col] - tmp[32 + col];

    e->block.dct[     col] = coeff[3]*(s0+s1+s2+s3);
    e->block.dct[ 8 + col] = coeff[0]*d0+coeff[2]*d1+coeff[4]*d2+coeff[6]*d3;
    e->block.dct[16 + col] = coeff[1]*(s0-s3)+coeff[5]*(s1-s2);
    e->block.dct[24 + col] = coeff[2]*d0-coeff[6]*d1-coeff[0]*d2-coeff[4]*d3;
    e->block.dct[32 + col] = coeff[3]*(s0-s1-s2+s3);
    e->block.dct[40 + col] = coeff[4]*d0-coeff[0]*d1+coeff[6]*d2+coeff[2]*d3;
    e->block.dct[48 + col] = coeff[5]*(s0-s3)+coeff[1]*(s2-s1);
    e->block.dct[56 + col] = coeff[6]*d0-coeff[4]*d1+coeff[2]*d2-coeff[0]*d3;
  }
#undef JPEC_BLOCK
}

static void jpec_enc_block_quant(jpec_enc_t *e) {
  assert(e && e->bnum >= 0);
  for (int i = 0; i < 64; i++) {
    e->block.quant[i] = (int) (e->block.dct[i]/e->dqt[i]);
  }
}

static void jpec_enc_block_zz(jpec_enc_t *e) {
  assert(e && e->bnum >= 0);
  e->block.len = 0;
  for (int i = 0; i < 64; i++) {
    if ((e->block.zz[i] = e->block.quant[jpec_zz[i]])) e->block.len = i + 1;
  }
}
