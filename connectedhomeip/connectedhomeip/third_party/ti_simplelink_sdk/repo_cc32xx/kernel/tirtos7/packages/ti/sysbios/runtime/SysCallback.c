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
 *  ======== SysCallback.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/SysCallback.h>
#include <ti/sysbios/runtime/SystemSupport.h>

/* 
 * These _D macros are set by SysConfig. The default values are
 * SysCallback_defaultAbort, SysCallback_defaultExit, etc. The user can
 * configure one or more of them as needed.
 */
void SysCallback_abortFxn_D(const char * str);
void SysCallback_exitFxn_D(int stat);
void SysCallback_flushFxn_D(void);
void SysCallback_initFxn_D(void);
void SysCallback_putchFxn_D(char c);
bool SysCallback_readyFxn_D(void);

/*
 *  ======== SystemSupport_init ========
 */
void SystemSupport_init(void)
{
    SysCallback_initFxn_D();
}

/*
 *  ======== SystemSupport_abort ========
 */
void SystemSupport_abort(const char * str)
{
    SysCallback_abortFxn_D(str);
}

/*
 *  ======== SystemSupport_exit ========
 */
void SystemSupport_exit(int stat)
{
    SysCallback_exitFxn_D(stat);
}

/*
 *  ======== SystemSupport_putch ========
 */
void SystemSupport_putch(char ch)
{
    SysCallback_putchFxn_D(ch);
}

/*
 *  ======== SystemSupport_ready ========
 */
bool SystemSupport_ready(void)
{
    return (SysCallback_readyFxn_D());
}

/*
 *  ======== SystemSupport_flush ========
 */
void SystemSupport_flush(void)
{
    SysCallback_flushFxn_D();
}

/*
 * These are the default implementations for each callback ...
 */

/*
 *  ======== SysCallback_defaultAbort ========
 */
void SysCallback_defaultAbort(const char * str)
{
    for (;;) {
        /* spin forever */
    }
}

/*
 *  ======== SysCallback_defaultExit ========
 */
void SysCallback_defaultExit(int stat)
{
}

/*
 *  ======== SysCallback_defaultFlush ========
 */
void SysCallback_defaultFlush(void)
{
}

/*
 *  ======== SysCallback_defaultInit ========
 */
void SysCallback_defaultInit(void)
{
}

/*
 *  ======== SysCallback_defaultPutch ========
 */
void SysCallback_defaultPutch(char c)
{
}

/*
 *  ======== SysCallback_defaultReady  ========
 */
bool SysCallback_defaultReady(void)
{
    return (true);
}
