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

#ifndef JPEC_BUFFER_H
#define JPEC_BUFFER_H

#include <stdint.h>

/** Extensible byte buffer */
typedef struct jpec_buffer_t_ {
  uint8_t *stream;                      /* byte buffer */
  int len;                              /* current length */
  int siz;                              /* maximum size */
} jpec_buffer_t;

jpec_buffer_t *jpec_buffer_new(void);
jpec_buffer_t *jpec_buffer_new2(int siz);
void jpec_buffer_del(jpec_buffer_t *b);
void jpec_buffer_write_byte(jpec_buffer_t *b, int val);
void jpec_buffer_write_2bytes(jpec_buffer_t *b, int val);

#endif
