/*******************************************************************************
* Copyright (c) 2015-2019 Cadence Design Systems, Inc.
* 
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to use this Software with Cadence processor cores only and 
* not with any other processors and platforms, subject to
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

******************************************************************************/
#include <xtensa/simcall-errno.h>
#include "xaf-api.h"

/* ...size of auxiliary pool for communication with HiFi */
#define XAF_AUX_POOL_SIZE                   32

/* ...length of auxiliary pool messages */
#define XAF_AUX_POOL_MSG_LENGTH             128
#define XAF_MAX_CONFIG_PARAMS               (XAF_AUX_POOL_MSG_LENGTH >> 3)

#ifndef EBADFD
#define EBADFD _SIMC_EBADFD
#endif

typedef enum {
    XAF_ADEV_RESET = 0,
    XAF_ADEV_INIT  = 1
} xaf_adev_state;

typedef enum {
    XAF_COMP_RESET          = 0,
    XAF_COMP_CREATE         = 1,
    XAF_COMP_SET_CONFIG     = 2,
    XAF_COMP_CONNECT        = 3,
    XAF_COMP_GET_CONFIG     = 4,
    XAF_COMP_PROCESS        = 5,
    XAF_COMP_GET_STATUS     = 6
} xaf_comp_state;

typedef struct xaf_comp_format_s {
    UWORD32             sample_rate;
    UWORD32             channels;
    UWORD32             pcm_width;
    UWORD32             input_length[XF_CFG_MAX_IN_PORTS];
    UWORD32             output_length[XF_CFG_MAX_OUT_PORTS];
} xaf_comp_format_t;

typedef struct xaf_comp xaf_comp_t;

struct xaf_comp {
    UWORD32             inp_routed;
    UWORD32             out_routed;
    UWORD32             inp_ports;
    UWORD32             out_ports;
    UWORD32             init_done;
    UWORD32             pending_resp;
    UWORD32             expect_out_cmd;
    UWORD32             input_over;

    xaf_comp_type   comp_type;
    xaf_comp_status comp_status;
    UWORD32             start_cmd_issued;
    UWORD32             exec_cmd_issued;
    void            *start_buf;

    xaf_comp_format_t    out_format;

    xf_pool_t       *inpool;
    xf_pool_t       *outpool;
    UWORD32                ninbuf;
    UWORD32             noutbuf;

    xaf_comp_t      *next;
    void            *p_adev;

    xaf_comp_state  comp_state;

    void           *comp_ptr; 

    xf_handle_t     handle;
};

typedef struct xaf_adev_s {
    xaf_comp_t  *comp_chain;

    UWORD32 n_comp;

    void *adev_ptr;
    void *p_dspMem;
    void *p_apMem;
    void *p_dspLocalBuff;
    void *p_apSharedMem;

    xaf_adev_state  adev_state;

    xf_proxy_t   proxy;
} xaf_adev_t;
