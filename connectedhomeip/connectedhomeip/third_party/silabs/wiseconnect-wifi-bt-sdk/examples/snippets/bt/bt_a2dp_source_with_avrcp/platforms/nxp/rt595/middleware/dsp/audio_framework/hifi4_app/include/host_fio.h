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

#ifndef __HOST_FIO_H__
#define __HOST_FIO_H__

/* */
#define PC_REQUEST_MAGIC_NO     0xAABBCCDD
#define PC_RESPONSE_MAGIC_NO     0xABCDABCD

#define FIO_SHMEM_SIZE      5120
#define FIO_SPI_BURST_SIZE  256

#define DSP_REQ_ADDR	0x5ffdd200
#define DSP_RESP_ADDR	0x5ffde600

#define DSP_REQ_FLAG_ADDR	0x5ffdd000
#define DSP_RESP_FLAG_ADDR	0x5ffdd100

/* */
typedef enum {
    FIO_FOPEN          = 1,
    FIO_FCLOSE         = 2,
    FIO_FREAD          = 3,
    FIO_FWRITE         = 4,
    FIO_LOGSTR         = 5
} FIO_FUNCTS;

/* */
typedef enum {
    FIO_T_INT          = 1,
    FIO_T_VOIDP        = 2
} FIO_PARAM_TYPE;

typedef struct{
    int         delimter[2];
    short       length;
    short       id;
    short       fn_id;
    short       nparams;
} st_PktHdr;

typedef struct{
    short   size;
    short   type;
} st_Params;

#endif /* __HOST_FIO_H__ */
