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

#include <stdio.h>
#include <string.h>
#include "xaf_fio_test.h"
#if REND_CAPT_HW_TEST
static char gfio_wr_arr[WR_BUF_SIZE];
static xaf_fio_buf_t gfio_buf_wr;

static unsigned int offset_hw_buff =0;

short inbuff_sine_16_500hz[]= {0x0000,0x0505,0x0A08,0x0EFA,0x13E2,0x18AF,0x1D63,0x21FA,0x2664,0x2AAD,0x2EBF,0x32A5,\
  0x364D,0x39BE,0x3CEE,0x3FDB,0x4284,0x44E0,0x46F4,0x48BA,0x4A2F,0x4B53,0x4C24,0x4CA4,\
  0x4CCA,0x4CA7,0x4C21,0x4B55,0x4A2E,0x48BA,0x46F3,0x44E3,0x4282,0x3FDA,0x3CF1,0x39BA,\
  0x3652,0x32A0,0x2EC4,0x2AA8,0x2669,0x21F6,0x1D65,0x18AF,0x13E2,0x0EF9,0x0A09,0x0504,\
  0x0001,0xFAFA,0xF5F9,0xF106,0xEC1D,0xE752,0xE29B,0xDE0A,0xD998,0xD556,0xD13E,0xCD5E,\
  0xC9B2,0xC641,0xC314,0xC022,0xBD80,0xBB1C,0xB90F,0xB743,0xB5D5,0xB4A9,0xB3DF,0xB35A,\
  0xB336,0xB35B,0xB3DD,0xB4AC,0xB5D1,0xB748,0xB90A,0xBB20,0xBD7D,0xC024,0xC313,0xC641,\
  0xC9B3,0xCD5C,0xD140,0xD554,0xD99A,0xDE08,0xE29C,0xE752,0xEC1C,0xF109,0xF5F5,0xFAFE};


short inbuff_sine_16_1500hz[]={0x0000,0x0EFB,0x1D65,0x2AA9,0x3650,0x3FDB,0x46F4,0x4B53,\
   0x4CCC,0x4B55,0x46F2,0x3FDD,0x364E,0x2AAA,0x1D66,0x0EF9,\
   0x0002,0xF103,0xE29D,0xD555,0xC9B2,0xC023,0xB90E,0xB4AB,\
   0xB335,0xB4AC,0xB90B,0xC027,0xC9AF,0xD558,0xE299,0xF107,\
   0xFFFE,0x0EFD,0x1D64,0x2AA9,0x3651,0x3FD9,0x46F6,0x4B53,\
   0x4CCB,0x4B56,0x46F1,0x3FDE,0x364D,0x2AAB,0x1D65,0x0EFA,\
   0x0001,0xF104,0xE29C,0xD556,0xC9B0,0xC026,0xB90C,0xB4AB,\
   0xB337,0xB4A9,0xB90E,0xC025,0xC9B0,0xD556,0xE29D,0xF102,\
   0x0003,0x0EFA,0x1D64,0x2AAB,0x364E,0x3FDB,0x46F6,0x4B52,\
   0x4CCD,0x4B53,0x46F2,0x3FDF,0x364B,0x2AAE,0x1D62,0x0EFB,\
   0x0002,0xF102,0xE29E,0xD555,0xC9B0,0xC027,0xB90A,0xB4AE,\
   0xB333,0xB4AC,0xB90E,0xC023,0xC9B2,0xD555,0xE29C,0xF105};

#endif

#if defined(FIO_REMOTE_FS)
static char gfio_rd_arr[RD_BUF_SIZE];
static char gfio_wr_arr[WR_BUF_SIZE];
static xaf_fio_buf_t gfio_buf_rd;
static xaf_fio_buf_t gfio_buf_wr;

static int g_id = 0;

#ifdef PC_TEST_FIO

#define FILE_REQ    "..\\..\\..\\..\\test\\Request.bin"
#define FILE_RES    "..\\..\\..\\..\\test\\Response.bin"

char fio_rqst[FIO_SHMEM_SIZE];
char fio_resp[FIO_SHMEM_SIZE];

int rqst_ready_flag[64];
int resp_ready_flag[64];

#else   // #ifdef PC_TEST_FIO

char fio_rqst[FIO_SHMEM_SIZE] __attribute__ ((section(".fio.shmem")));
char fio_resp[FIO_SHMEM_SIZE] __attribute__ ((section(".fio.shmem")));

int rqst_ready_flag[64] __attribute__ ((section(".fio.shmem")));
int resp_ready_flag[64] __attribute__ ((section(".fio.shmem")));

#endif  // #ifdef PC_TEST_FIO

extern int errno ;

#define UPDATE_STR_PAYLOAD(ptr,payload,payload_size)        \
{                                                           \
	short size    = ((strlen(payload) + 4) >> 2) << 2;      \
	*ptr++        = size;                                   \
	*ptr++        = FIO_T_VOIDP;                            \
	strcpy((char *)ptr,payload);                            \
	ptr          += size >> 1;                              \
	payload_size += size + 2*sizeof(short);                 \
}

#define UPDATE_PAYLOAD(ptr,payload,size,payload_size,type)  \
{                                                           \
	*ptr++        			= size;                         \
	*ptr++        			= type;                         \
	memcpy(ptr,payload,size);                               \
	ptr					   += size >> 1;                    \
	payload_size += size + 2*sizeof(short);                 \
}

#define UPDATE_PKT_HDR(p_hdr,fu_id,size)                    \
{                                                           \
	p_hdr->delimter[0] = PC_REQUEST_MAGIC_NO;               \
	p_hdr->length      = payload_size;                      \
	p_hdr->id          = g_id++;                            \
	p_hdr->fn_id       = fu_id;                             \
	p_hdr->nparams     = fu_id ## _NUM_PARAMS;              \
}

#define PARSE_PAYLOAD(ptr,payload,payload_size)             \
{ \
	int size      = *ptr++;                                 \
	*ptr++;\
	memcpy(payload,ptr,size);                               \
	ptr          += size >> 1;                              \
	payload_size += size + 2*sizeof(short);                 \
}

#define PARSE_PKT_HDR(p_hdr,p_resp)                         \
{                                                           \
	memcpy(p_hdr,p_resp,sizeof(st_PktHdr));                 \
	p_resp += sizeof(st_PktHdr) >> 1;                       \
}

void create_requ(xaf_fio_args_tx *p_args, st_PktHdr *p_hdr,void *p_rqst, FIO_FUNCTS fn_id)
{
    short * sht_ptr        = (short *)((char *)p_rqst + sizeof(st_PktHdr));
    short payload_size     = sizeof(st_PktHdr);

    switch(fn_id)
    {
        case FIO_FOPEN:
            UPDATE_STR_PAYLOAD(sht_ptr,p_args->path,payload_size);
            UPDATE_STR_PAYLOAD(sht_ptr,p_args->mode,payload_size);
            UPDATE_PKT_HDR(p_hdr,FIO_FOPEN,payload_size);
            break;
        case FIO_FCLOSE:
            UPDATE_PAYLOAD(sht_ptr,&p_args->fptr,sizeof(void *),payload_size,FIO_T_VOIDP);
            UPDATE_PKT_HDR(p_hdr,FIO_FCLOSE,payload_size);
            break;
        case FIO_FREAD:
            UPDATE_PAYLOAD(sht_ptr,&p_args->ptr,sizeof(void *),payload_size,FIO_T_VOIDP);
            UPDATE_PAYLOAD(sht_ptr,&p_args->size,sizeof(size_t),payload_size,FIO_T_INT);
            UPDATE_PAYLOAD(sht_ptr,&p_args->nmemb,sizeof(size_t),payload_size,FIO_T_INT);
            UPDATE_PAYLOAD(sht_ptr,&p_args->stream,sizeof(void *),payload_size,FIO_T_VOIDP);
            UPDATE_PKT_HDR(p_hdr,FIO_FREAD,payload_size);
            break;
        case FIO_FWRITE:
            UPDATE_PAYLOAD(sht_ptr,&p_args->ptr,sizeof(void *),payload_size,FIO_T_VOIDP);
            UPDATE_PAYLOAD(sht_ptr,&p_args->size,sizeof(size_t),payload_size,FIO_T_INT);
            UPDATE_PAYLOAD(sht_ptr,&p_args->nmemb,sizeof(size_t),payload_size,FIO_T_INT);
            UPDATE_PAYLOAD(sht_ptr,&p_args->stream,sizeof(void *),payload_size,FIO_T_VOIDP);
            UPDATE_PAYLOAD(sht_ptr,p_args->ptr,(p_args->size*p_args->nmemb),payload_size,FIO_T_VOIDP);
            UPDATE_PKT_HDR(p_hdr,FIO_FWRITE,payload_size);
            break;
        case FIO_LOGSTR:
        	UPDATE_STR_PAYLOAD(sht_ptr,p_args->str_print,payload_size);
        	UPDATE_PKT_HDR(p_hdr,FIO_LOGSTR,payload_size);
            break;

        default:
        	FIO_PRINTF(stderr,"Invalid Request: 0x%04x\n", fn_id);
    }
}
unsigned int idx;
void confrm_requ(st_PktHdr *p_hdr,void *p_rqst)
{
	st_PktHdr rethdr;
	int result;
	memcpy(p_rqst,p_hdr,sizeof(st_PktHdr));

	memcpy(&rethdr,p_rqst,sizeof(st_PktHdr));
	result = memcmp(&rethdr,p_hdr,sizeof(st_PktHdr));

    memset(rqst_ready_flag, 0, 256);
    rqst_ready_flag[0] = PC_REQUEST_MAGIC_NO;
}

void confrm_resp(st_PktHdr *p_hdr_tx,st_PktHdr *p_hdr_rx,xaf_fio_args_rx *p_args,void *p_resp,FIO_FUNCTS fn_id)
{
    if((p_hdr_tx->id == p_hdr_rx->id) && (p_hdr_tx->fn_id == p_hdr_rx->fn_id)){
    	resp_ready_flag[0] = PC_RESET_MAGIC_NO;
    }
    else{
        switch(fn_id)
        {
            case FIO_FOPEN:
                p_args->fptr  			= NULL;
                break;
            case FIO_FCLOSE:
                p_args->retval_fclose   = EOF;
                break;
            case FIO_FREAD:
                p_args->items_fread  	= 0;
                break;
            case FIO_FWRITE:
                p_args->items_fwrite 	= 0;
                break;
            case FIO_LOGSTR:
                p_args->items_printed 	= 0;
                break;

            default:
                FIO_PRINTF(stderr,"Invalid Request: 0x%04x\n", fn_id);
        }
    }
}

void parse_resp(xaf_fio_args_tx *p_args_tx,xaf_fio_args_rx *p_args_rx,st_PktHdr *p_hdr,void *p_resp, FIO_FUNCTS fn_id)
{
    short * sht_ptr        = (short *)((char *)p_resp);
    short payload_size     = sizeof(st_PktHdr);

    switch(fn_id)
    {
        case FIO_FOPEN:
            PARSE_PKT_HDR(p_hdr,sht_ptr);
            PARSE_PAYLOAD(sht_ptr,&p_args_rx->fptr,payload_size);
            break;
        case FIO_FCLOSE:
            PARSE_PKT_HDR(p_hdr,sht_ptr);
            PARSE_PAYLOAD(sht_ptr,&p_args_rx->retval_fclose,payload_size);
            break;
        case FIO_FREAD:
            PARSE_PKT_HDR(p_hdr,sht_ptr);
            PARSE_PAYLOAD(sht_ptr,&p_args_rx->items_fread,payload_size);
            PARSE_PAYLOAD(sht_ptr,p_args_tx->ptr,payload_size);
            break;
        case FIO_FWRITE:
            PARSE_PKT_HDR(p_hdr,sht_ptr);
            PARSE_PAYLOAD(sht_ptr,&p_args_rx->items_fwrite,payload_size);
            break;
        case FIO_LOGSTR:
        	PARSE_PKT_HDR(p_hdr,sht_ptr);
        	PARSE_PAYLOAD(sht_ptr,&p_args_rx->items_printed,payload_size);
            break;

        default:
        	FIO_PRINTF(stderr,"Invalid Request: 0x%04x\n", fn_id);
    }
}

void transmit_pkt(void *p_rqst,xaf_fio_args_tx *p_args_tx,void *p_resp,xaf_fio_args_rx *p_args_rx,FIO_FUNCTS fn_id)
{
    st_PktHdr  pkthdr_tx,pkthdr_rx;

    /* Wait till the previous request is read by PC */
#ifdef PC_TEST_FIO
    {
        FILE *f_req_ptr;
        int magicWord;

        while((f_req_ptr = fopen(FILE_REQ, "rb")) != NULL)
        {
            fread(&magicWord, sizeof(char), 4, f_req_ptr);

            if(magicWord != PC_REQUEST_MAGIC_NO)
            {
                break;
            }

            fclose(f_req_ptr);
        }
    }
#else   // #ifdef PC_TEST_FIO
#endif  // #ifdef PC_TEST_FIO
    do{
    }while(rqst_ready_flag[0] == PC_REQUEST_MAGIC_NO);

    memset(fio_rqst, 0, FIO_SHMEM_SIZE);
    create_requ(p_args_tx,&pkthdr_tx,p_rqst,fn_id);
    confrm_requ(&pkthdr_tx,p_rqst);

    /* Wait till the PC response is ready */
    do{
#ifdef PC_TEST_FIO
        {
            FILE *f_req_ptr;
            FILE *f_res_ptr;

            int responsePktSize;

            f_req_ptr = fopen(FILE_REQ, "wb");
            fwrite(p_rqst, sizeof(char), pkthdr_tx.length, f_req_ptr);
            fclose(f_req_ptr);

            do {
            } while ((f_res_ptr = fopen(FILE_RES, "rb")) == NULL);

            fseek(f_res_ptr, 0, SEEK_END);
            responsePktSize = ftell(f_res_ptr);
            fseek(f_res_ptr, 0, SEEK_SET);

            if(RESPONSE_BUF_SIZE < responsePktSize)
            {
                printf("Reesponse too long...\n");
                fclose(f_res_ptr);
                return;
            }

            if(responsePktSize != fread(p_resp, 1, responsePktSize, f_res_ptr))
            {
                printf("Unable to read response...\n");
                fclose(f_res_ptr);
                return;
            }

            fclose(f_res_ptr);
        }
#else   // #ifdef PC_TEST_FIO
        {
			int i;
			do{

			}while(rqst_ready_flag[0] == PC_REQUEST_MAGIC_NO);
        }
#endif  // #ifdef PC_TEST_FIO
    }while(resp_ready_flag[0] != PC_RESPONSE_MAGIC_NO);

    parse_resp(p_args_tx,p_args_rx,&pkthdr_rx,p_resp,fn_id);
    confrm_resp(&pkthdr_tx,&pkthdr_rx,p_args_rx,p_resp,fn_id);

#ifdef PC_TEST_FIO
    {
        FILE *f_res_ptr;
        int clearFlag = 0;

        f_res_ptr = fopen(FILE_RES, "wb");
        fwrite(&clearFlag, 1, sizeof(int), f_res_ptr);
        fclose(f_res_ptr);
    }
#endif  // #ifdef PC_TEST_FIO
}
#endif /* FIO_REMOTE_FS */

void *fio_fopen(const char *path, const char *mode)
{
    void * fptr = NULL;
    /* ==== Your code here ===== */

#if defined(FIO_LOCAL_FS)
    fptr = fopen(path,mode);
#elif defined(FIO_REMOTE_FS)
    xaf_fio_args_tx args_tx;
    xaf_fio_args_rx args_rx;
    void *p_rqst, *p_resp;

    p_rqst       = (void *) fio_rqst;
    p_resp       = (void *) fio_resp;
    args_tx.path = path;
    args_tx.mode = mode;

    transmit_pkt(p_rqst,&args_tx,p_resp,&args_rx,FIO_FOPEN);

    return args_rx.fptr;

#else // FIO_BUFFER
    if(strcmp(mode,"rb") == 0){

    	fptr =  inbuff_sine_16_500hz;
    }
    else if(strcmp(mode,"wb") == 0){

        if(gfio_buf_wr.pbase != 0){
            FIO_PRINTF(stdout,"\n Output already open %s \n",path);
        }
        gfio_buf_wr.pbase  = gfio_buf_wr.ptr = (void *)gfio_wr_arr;
        gfio_buf_wr.remlen = WR_BUF_SIZE;
        gfio_buf_wr.flag   = 0;
        fptr = (void *)&gfio_buf_wr;
    }
    else{
        fptr = NULL;
        FIO_PRINTF(stdout,"\n Error opening file %s \n",path);
    }
#endif
    /* ==== Your code here ===== */

    return (void *) fptr;
}

int fio_fclose(void *fp)
{
    int status;
    /* ==== Your code here ===== */
#if defined(FIO_LOCAL_FS)
    FILE * fptr;
    fptr = (FILE *)fp;
    status = fclose(fptr);
#elif defined(FIO_REMOTE_FS)
    xaf_fio_args_tx args_tx;
    xaf_fio_args_rx args_rx;
    void *p_rqst, *p_resp;

    p_rqst       = (void *) fio_rqst;
    p_resp       = (void *) fio_resp;
    args_tx.fptr = fp;

    transmit_pkt(p_rqst,&args_tx,p_resp,&args_rx,FIO_FCLOSE);

    return args_rx.retval_fclose;

#else // FIO_BUFFER

    status=0;
#endif
    /* ==== Your code here ===== */
    return status;
}

size_t fio_fread(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t items;
    size_t size_bytes = size * nmemb;
    /* ==== Your code here ===== */
#if defined(FIO_LOCAL_FS)
    FILE * fptr;
    fptr = (FILE *)stream;
    items = fread(ptr, size, nmemb, fptr);
#elif defined(FIO_REMOTE_FS)
    xaf_fio_args_tx args_tx;
    xaf_fio_args_rx args_rx;
    void *p_rqst, *p_resp;

    p_rqst       = (void *) fio_rqst;
    p_resp       = (void *) fio_resp;

    args_tx.ptr    = ptr;
    args_tx.size   = size;
    args_tx.nmemb  = nmemb;
    args_tx.stream = stream;

    transmit_pkt(p_rqst,&args_tx,p_resp,&args_rx,FIO_FREAD);

    return args_rx.items_fread;
#else // FIO_BUFFER
#if defined(REND_CAPT_HW_TEST)
    char* inpt_buff_ptr = NULL;
    short* inpt_buff_ptr_500 = NULL;
    short* inpt_buff_ptr_1500 = NULL;
    short* dst_ptr = NULL;
    unsigned int buff_size = sizeof(inbuff_sine_16_500hz)/2;
    unsigned int rem_len = 0;
    unsigned int i =0;
    unsigned int rem_len_tmp =0;

    dst_ptr = (short*)ptr;
    inpt_buff_ptr_500 = (short*)inbuff_sine_16_500hz;
    inpt_buff_ptr_1500 = (short*)inbuff_sine_16_1500hz;
    items = size_bytes/size;
    rem_len = buff_size - offset_hw_buff;/*samples*/
    while(size_bytes > 0)
    {
        *dst_ptr = inpt_buff_ptr_500[offset_hw_buff];
        dst_ptr++;
        *dst_ptr = inpt_buff_ptr_1500[offset_hw_buff];
        dst_ptr++;
        size_bytes = size_bytes - 4;
        offset_hw_buff++;
        if(offset_hw_buff == buff_size)
            offset_hw_buff = 0;
    }

#else

    if((gfio_buf_rd.flag == 1) || (gfio_buf_rd.remlen == 0)){
        FIO_PRINTF(stdout,"\n Input over \n");
        items = 0;
        return items;
    }

    if(size_bytes > gfio_buf_rd.remlen){
        size_bytes    = gfio_buf_rd.remlen;
        gfio_buf_rd.flag   = 1;
    }
    memcpy(ptr,gfio_buf_rd.ptr, size_bytes);

    gfio_buf_rd.ptr += size_bytes;
    gfio_buf_rd.remlen -= size_bytes;

    items = size_bytes/size;

#endif
#endif
    return items;
}

size_t fio_fwrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    FILE * fptr;
    size_t items;
    size_t size_bytes = size * nmemb;

    fptr = (FILE *)stream;

#if defined(FIO_LOCAL_FS)
    fptr = (FILE *)stream;
    items = fwrite(ptr, size, nmemb, fptr);
#elif defined(FIO_REMOTE_FS)
    {
        xaf_fio_args_tx args_tx;
        xaf_fio_args_rx args_rx;
        void *p_rqst, *p_resp;

        p_rqst       = (void *) fio_rqst;
        p_resp       = (void *) fio_resp;

        args_tx.ptr    = (void *)ptr;
        args_tx.size   = size;
        args_tx.nmemb  = nmemb;
        args_tx.stream = stream;

        transmit_pkt(p_rqst,&args_tx,p_resp,&args_rx,FIO_FWRITE);

        return args_rx.items_fwrite;
    }
#else // FIO_BUFFER
    if((gfio_buf_wr.flag == 1) || (gfio_buf_wr.remlen == 0)){
        FIO_PRINTF(stdout,"\n Input over \n");
        items = 0;
        return items;
    }
    if(size_bytes > gfio_buf_wr.remlen){
        size_bytes    = gfio_buf_wr.remlen;
        gfio_buf_wr.flag   = 1;
    }

    memcpy(gfio_buf_wr.ptr,ptr, size_bytes);

    gfio_buf_wr.ptr += size_bytes;
    gfio_buf_wr.remlen -= size_bytes;

    items = size_bytes/size;
#endif

    return items;
}

#if defined(FIO_REMOTE_FS)
size_t fio_printf(xaf_fio_args_tx *p_args_tx,xaf_fio_args_rx *p_args_rx)
{
	void *p_rqst, *p_resp;
    p_rqst       = (void *) fio_rqst;
    p_resp       = (void *) fio_resp;

	transmit_pkt(p_rqst,p_args_tx,p_resp,p_args_rx,FIO_LOGSTR);
    return p_args_rx->items_printed;
}
#endif

