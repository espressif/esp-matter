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
/* File contains constants shared between AP and DP sides */

#define XF_CFG_CORES_NUM_DSP                1

/* DSP object sizes */

#define XF_DSP_OBJ_SIZE_CORE_DATA           388
#define XF_DSP_OBJ_SIZE_DSP_LOCAL_POOL      124
#define XF_DSP_OBJ_SIZE_CORE_RO_DATA        256
#define XF_DSP_OBJ_SIZE_CORE_RW_DATA        256


/*******************************************************************************
 * Global configuration parameters (changing is to be done carefully)
 ******************************************************************************/

/* ...maximum in ports for mimo class */
#define XF_CFG_MAX_IN_PORTS             4

/* ...maximum out ports for mimo class */
#define XF_CFG_MAX_OUT_PORTS            4

