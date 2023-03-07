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
 
#ifndef JPEC_H
#define JPEC_H

#include <conf.h>

/*************************************************
 * JPEG Encoder
 *************************************************/
 
/* -------------------------------------------------
 * LIMITATIONS
 * -------------------------------------------------
 * - Grayscale *only* (monochrome JPEG file): no support for color
 * - Baseline DCT-based  (SOF0), JFIF 1.01 (APP0) JPEG
 * - Block size of 8x8 pixels *only*
 * - Default quantization and Huffman tables *only*
 * - No border filling support: the input image *MUST* represent an integer
 *   number of blocks, i.e. each dimension must be a multiple of 8
 */

/** Type of a JPEG encoder object */
typedef struct jpec_enc_t_ jpec_enc_t;

/*
 * Create a JPEG encoder with default quality factor
 * `img' specifies the pointer to aligned image data.
 * `w' specifies the image width in pixels.
 * `h' specifies the image height in pixels.
 * Because the returned encoder is allocated by this function, it should be
 * released with the `jpec_enc_del' call when it is no longer useful.
 * Note: for efficiency the image data is *NOT* copied and the encoder just
 * retains a pointer to it. Thus the image data must not be deleted
 * nor change until the encoder object gets deleted.
 */
jpec_enc_t *jpec_enc_new(const uint8_t *img, uint16_t w, uint16_t h);
/*
 * `q` specifies the JPEG quality factor in 0..100
 */
jpec_enc_t *jpec_enc_new2(const uint8_t *img, uint16_t w, uint16_t h, int q);

/*
 * Release a JPEG encoder object
 * `e` specifies the encoder object
 */
void jpec_enc_del(jpec_enc_t *e);

/*
 * Run the JPEG encoding
 * `e` specifies the encoder object
 * `len` specifies the pointer to the variable into which the length of the
 * JPEG blob is assigned
 * If successful, the return value is the pointer to the JPEG blob. `NULL` is
 * returned if an error occurred.
 * Note: the caller should take care to copy or save the JPEG blob before
 * calling `jpec_enc_del` since the blob will no longer be maintained after.
 */
const uint8_t *jpec_enc_run(jpec_enc_t *e, int *len);

#endif
