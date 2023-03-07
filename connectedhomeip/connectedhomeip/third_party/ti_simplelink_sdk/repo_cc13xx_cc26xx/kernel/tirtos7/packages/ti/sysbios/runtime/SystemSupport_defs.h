/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
 */

/*
 * ======== SystemSupport_defs.h ========
 */

/* @cond LONG_NAMES */

#ifdef ti_sysbios_runtime_SystemSupport_long_names

#define SystemSupport_abort ti_sysbios_runtime_SystemSupport_abort
#define SystemSupport_exit ti_sysbios_runtime_SystemSupport_exit
#define SystemSupport_flush ti_sysbios_runtime_SystemSupport_flush
#define SystemSupport_init ti_sysbios_runtime_SystemSupport_init
#define SystemSupport_putch ti_sysbios_runtime_SystemSupport_putch
#define SystemSupport_ready ti_sysbios_runtime_SystemSupport_ready

#endif

#if defined(ti_sysbios_runtime_SystemSupport__nolocalnames) && !defined(ti_sysbios_runtime_SystemSupport_long_names)

#undef SystemSupport_abort
#undef SystemSupport_exit
#undef SystemSupport_flush
#undef SystemSupport_init
#undef SystemSupport_putch
#undef SystemSupport_ready

#endif

/* @endcond */
