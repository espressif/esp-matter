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
 * xf-ipi.c
 *
 * Inter-processor interrupt handling code
 ******************************************************************************/

#define MODULE_TAG                      IPI

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include "xf-dp.h"

/*******************************************************************************
 * Interrupt handler code
 ******************************************************************************/

/* ...IPI handler on core #0 */
static void xf_ipi_0(void *arg)
{
    TRACE(1, _b("ipi-0 received"));
    xf_ipi_resume(0);
}

#if XF_CFG_CORES_NUM > 1
/* ...IPI handler on core #1 */
static void xf_ipi_1(void *arg)
{
    xf_ipi_resume(1);
}
#endif

#if XF_CFG_CORES_NUM > 2
/* ...IPI handler on core #2 */
static void xf_ipi_2(void *arg)
{
    xf_ipi_resume(2);
}
#endif

#if XF_CFG_CORES_NUM > 3
/* ...IPI handler on core #3 */
static void xf_ipi_3(void *arg)
{
    xf_ipi_resume(3);
}
#endif

#if XF_CFG_CORES_NUM > 4
#error "CPU number too high"
#endif

void (* const xf_ipi_handlers[XF_CFG_CORES_NUM])(void *arg) = 
{
    xf_ipi_0,
#if XF_CFG_CORES_NUM > 1
    xf_ipi_1,
#endif
#if XF_CFG_CORES_NUM > 2
    xf_ipi_2,
#endif
#if XF_CFG_CORES_NUM > 3
    xf_ipi_3,
#endif
};
