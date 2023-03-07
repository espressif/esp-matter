/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include "host_fio.h"

#ifdef XAF_HW
#define FIO_REMOTE_FS
#define REND_CAPT_HW_TEST 0
#endif

#define RD_BUF_SIZE 4096*2
#define WR_BUF_SIZE 4096*4
#define STR_PRINT_SIZE 4096

#define ARRAY_LENGTH 150
#define MIN(a,b) (((a)<(b))?(a):(b))

typedef struct xaf_fio_buf_s {
    void               *pbase;
    char               *ptr;
    size_t              remlen;
    unsigned int        flag;
} xaf_fio_buf_t;

/* */
#define PC_RESET_MAGIC_NO        0xFFFFFFFF

/* */
typedef enum {
    FIO_FOPEN_NUM_PARAMS          = 2,
    FIO_FCLOSE_NUM_PARAMS         = 1,
    FIO_FREAD_NUM_PARAMS          = 4,
    FIO_FWRITE_NUM_PARAMS         = 5,
    FIO_LOGSTR_NUM_PARAMS         = 1
} FIO_FUNCTS_NUM_PARAMS;

/* file i/o tx args */
typedef struct{
    const char      * path;
    const char      * mode;
    void            * fptr;
    void            * ptr;
    size_t            size;
    size_t            nmemb;
    void            * stream;
    int               data_len;
    char              str_print[ARRAY_LENGTH];
} xaf_fio_args_tx;

/* file i/o rx args */
typedef struct{
    void            * fptr;
    int               retval_fclose;
    size_t            items_fread;
    size_t            items_fwrite;
    void            * data_ptr;
    int               data_len;
    size_t            items_printed;
} xaf_fio_args_rx;

void  *fio_fopen(const char *path, const char *mode);
int    fio_fclose(void *fp);
size_t fio_fread(void *ptr, size_t size, size_t nmemb, void *stream);
size_t fio_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream);
size_t fio_printf(xaf_fio_args_tx *p_args_tx,xaf_fio_args_rx *p_args_rx);


#if defined(FIO_LOCAL_FS)
#define FIO_PRINTF(...) fprintf(__VA_ARGS__)
#elif defined(FIO_REMOTE_FS)
#define FIO_PRINTF(a,...) \
{\
    xaf_fio_args_tx args_tx;\
    xaf_fio_args_rx args_rx;\
    memset(args_tx.str_print, 0, sizeof(char) * ARRAY_LENGTH);\
    snprintf(args_tx.str_print,ARRAY_LENGTH,__VA_ARGS__);\
    fio_printf(&args_tx,&args_rx);\
}
#else // FIO_BUFFER
#define FIO_PRINTF(...)
#endif
