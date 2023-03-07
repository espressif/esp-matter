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

#ifdef __XCC__
#ifdef __TOOLS_RF2__
#include <xtensa/xos/xos.h>
#else   // #ifdef __TOOLS_RF2__
#include <xtensa/xos.h>
#endif  // #ifdef __TOOLS_RF2__
#endif  // #ifdef __XCC__

#include "xa_type_def.h"

/* ...debugging facility */
#include "xf-debug.h"

#include "xaf_test.h"
#include "xaf-api.h"
#include "xaf_mem.h"

#ifdef __XCC__
#include <xtensa/hal.h>

#ifdef __TOOLS_RF2__
#define TOOLS_SUFFIX    "_RF2"
#else
#define TOOLS_SUFFIX    "_RF3"
#endif

#if XCHAL_HAVE_HIFI4
#define BUILD_STRING "XTENSA_HIFI4" TOOLS_SUFFIX
#elif XCHAL_HAVE_HIFI3
#define BUILD_STRING "XTENSA_HIFI3" TOOLS_SUFFIX
#elif XCHAL_HAVE_HIFI_MINI
#define BUILD_STRING "XTENSA_HIFI_MINI" TOOLS_SUFFIX
#elif XCHAL_HAVE_HIFI2EP
#ifdef XAF_HW
#define BUILD_STRING "XTENSA_HIFIEP_RTK_HW" TOOLS_SUFFIX
#else
#define BUILD_STRING "XTENSA_HIFIEP_RTK" TOOLS_SUFFIX
#endif
#elif XCHAL_HAVE_HIFI2
#define BUILD_STRING "XTENSA_HIFI2" TOOLS_SUFFIX
#else
#define BUILD_STRING "XTENSA_NON_HIFI" TOOLS_SUFFIX
#endif

#else
#define BUILD_STRING "NON_XTENSA"
#endif


#ifdef XAF_PROFILE
#include <xtensa/sim.h>
#include "xaf_clk_test.h"
#endif

#ifndef STACK_SIZE
#define STACK_SIZE          8192
#endif

#define DEVICE_ID    (0x180201FC) //Vender ID register, NOT USED
#define VENDER_ID    (0x10EC)     //Vender ID value, NOT USED

/* global IO attributes for shared memory R/W */
#ifdef XAF_HW_LPCNEXT0
extern int audio_frmwk_buf_size;
extern int audio_comp_buf_size;

extern unsigned char *readBuffer;
extern unsigned int readBufferPtr;
extern unsigned int readBufferSize;
extern unsigned char *readBuffer2;
extern unsigned int readBufferPtr2;
extern unsigned int readBufferSize2;
extern unsigned char *writeBuffer;
extern unsigned int writeBufferSize;
extern unsigned int writeBufferPtr;

extern unsigned char TESTBUFFER[];
extern unsigned char dsp_wb[204800];
#endif

void set_wbna(int *argc, const char **argv);
int print_verinfo(pUWORD8 ver_info[],pUWORD8 app_name);
int read_input(void *p_buf, int buf_length, int *read_length, void *p_input, xaf_comp_type comp_type);
double compute_comp_mcps(unsigned int num_bytes, int comp_cycles, xaf_format_t comp_format, double *strm_duration);
int print_mem_mcps_info(mem_obj_t* mem_handle, int num_comp);
int comp_process_entry(void *arg, int wake_value);
int comp_process_entry_recorder(void *arg, int wake_value);
unsigned short start_xos();
void srtm_reset_io_attr();
