/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 *  ======== SysMin.c ========
 */

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/SysMin.h>
#include <ti/sysbios/runtime/SystemSupport.h>
#include <ti/sysbios/runtime/Types.h>

#include <string.h>

#if defined(__TI_COMPILER_VERSION__) || defined(__ti_version__)
extern int HOSTwrite(int, const char *, unsigned);
#elif defined(__GNUC__)
extern int _write(int, char *, int);
#define HOSTwrite(x,y,z) _write((int)(x),(char *)(y),(int)(z))
#elif defined(__IAR_SYSTEMS_ICC__)
/* IAR's runtime support __write function is only a template. We should
 * either write our own or disable SysMin for IAR in the future. */
#define HOSTwrite(x,y,z) z
#else
#error unsupported target
#endif

const size_t SysMin_bufSize = SysMin_bufSize_D;

/* flushAtExit */
const bool SysMin_flushAtExit = SysMin_flushAtExit_D;

/* outputFunc */
const SysMin_OutputFxn SysMin_outputFunc = SysMin_outputFunc_D;

#if SysMin_bufSize_D
char SysMin_outbuf[SysMin_bufSize_D];
#endif

SysMin_Module_State SysMin_Module_state = {
    .bufsize = SysMin_bufSize_D,
#if SysMin_bufSize_D
    .outbuf = SysMin_outbuf,
#else
    .outbuf = NULL,
#endif
    .outidx = 0,
    .wrapped = 0
};

/*
 *  ======== SystemSupport_init ========
 */
void SystemSupport_init(void)
{
    /* REQ_TAG(SYSBIOS-919) */
    if (SysMin_bufSize_D != 0U) {
        (void)memset(SysMin_module->outbuf, 0, SysMin_bufSize_D);
    }
}

/*
 *  ======== SystemSupport_abort ========
 */
/* REQ_TAG(SYSBIOS-915) */
void SystemSupport_abort(const char * str)
{
    char ch;

    if (SysMin_bufSize_D != 0U) {
        if (str != (const char *)NULL) {
            ch = *str;
            str++;
            while (ch != '\0') {
                SystemSupport_putch(ch);
                ch = *str;
                str++;
            }
        }

        /* Only flush if configured to do so */
        /* REQ_TAG(SYSBIOS-920) */
        if (SysMin_flushAtExit != false) {
            SystemSupport_flush();
        }
    }
}

/*
 *  ======== SystemSupport_exit ========
 */
/* REQ_TAG(SYSBIOS-916) */
void SystemSupport_exit(int stat)
{
    /* REQ_TAG(SYSBIOS-920) */
    if ((SysMin_flushAtExit == true) && (SysMin_bufSize_D != 0U)) {
        SystemSupport_flush();
    }
}

/*
 *  ======== SystemSupport_putch ========
 */
/* REQ_TAG(SYSBIOS-917) */
void SystemSupport_putch(char ch)
{
    unsigned int key;

    if (SysMin_bufSize_D != 0U) {

        key = Hwi_disable();

        SysMin_module->outbuf[SysMin_module->outidx] = ch;
        SysMin_module->outidx++;
        if (SysMin_module->outidx == (unsigned int)SysMin_bufSize_D) {
            SysMin_module->outidx = 0;
            SysMin_module->wrapped = true;
        }

        Hwi_restore(key);
    }
}

/*
 *  ======== SystemSupport_ready ========
 */
/* REQ_TAG(SYSBIOS-918) */
bool SystemSupport_ready(void)
{
    return (bool)(SysMin_bufSize_D != 0U);
}

/*
 *  ======== SystemSupport_flush ========
 *  Called during SystemSupport_exit, System_exit or System_flush.
 */
void SystemSupport_flush(void)
{
    unsigned int key;

    key = Hwi_disable();

    /*
     *  If a wrap occured, we need to flush the "end" of the internal buffer
     *  first to maintain fifo character output order.
     */
    if (SysMin_module->wrapped == true) {
        SysMin_outputFunc(SysMin_module->outbuf + SysMin_module->outidx,
                          (unsigned int)(SysMin_bufSize - SysMin_module->outidx));
    }

    /* REQ_TAG(SYSBIOS-914) */
    SysMin_outputFunc(SysMin_module->outbuf, SysMin_module->outidx);
    SysMin_module->outidx = 0;
    SysMin_module->wrapped = false;

    Hwi_restore(key);
}

#if defined(__ti__)
extern int HOSTwrite(int, const char *, unsigned);
#elif (defined(gnu_targets_arm_STD_) && defined(xdc_target__os_undefined))
extern int _write(int, char *, int);
#define HOSTwrite(x,y,z) _write((int)(x),(char *)(y),(int)(z))
#elif defined(__IAR_SYSTEMS_ICC__)
/* IAR's runtime support __write function is only a template. We should
 * either write our own or disable SysMin for IAR in the future. */
#define HOSTwrite(x,y,z) z
#else
#include <stdio.h>
#endif

/*
 *  ======== SysMin_output ========
 *  HOSTWrite only writes a max of N chars at a time. The amount it writes
 *  is returned. This function loops until the entire buffer is written.
 *  Being a static function allows it to conditionally compile out.
 */
void SysMin_output(char *buf, unsigned int size)
{
    int printCount;

    while (SysMin_bufSize_D != 0 && size != 0) {
        printCount = HOSTwrite(1, buf, size);
        if ((printCount <= 0) || ((unsigned int)printCount > size)) {
            break;  /* ensure we never get stuck in an infinite loop */
        }
        size -= printCount;
        buf = buf + printCount;
    }
}
