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
/*******************************************************************************
 * xf-config.h
 *
 * Xtensa target configuration parameters
 *******************************************************************************/

/* ...number of DSP cores */
#define XF_CFG_CORES_NUM                1

/* ...size of the internal message pool (make it equal to at least ring-buffer) */
#define XF_CFG_MESSAGE_POOL_SIZE        256

/* ...local IPC is coherent (say) */
#define XF_LOCAL_IPC_NON_COHERENT       0

/* ...remote IPC is non coherent (say) */
#if XF_CFG_CORES_NUM > 1
#define XF_REMOTE_IPC_NON_COHERENT      1
#else
#define XF_REMOTE_IPC_NON_COHERENT      0
#endif

/* ...size of the local memory pool (in bytes) */
#define XF_CFG_LOCAL_POOL_SIZE          (1024 << 10)

/* ...size of the shared memory pool (in bytes) */
#define XF_CFG_REMOTE_IPC_POOL_SIZE     (256 << 10)

/* ...size of the local memory pool (in bytes) */
#define XF_CFG_LOCAL_IPC_POOL_SIZE      (1024 << 10)

/* ...maximal size of scratch memory is 56 KB */
#define XF_CFG_CODEC_SCRATCHMEM_SIZE    (56 << 10)

/* ...scratch memory is 8-bytes aligned */
#define XF_CFG_CODEC_SCRATCHMEM_ALIGN   8
