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
/*!
 * @file ti/sysbios/runtime/SysCallback.h
 * @brief SystemSupport implementation for user callback functions
 *
 * This module provides a implementation of the SystemSupport.h interface
 * that simply calls back the user defined functions to support the System
 * module's functionality.
 *
 * Configuration is as shown below. Not all fields need be specfied. If a
 * field is not specified, a default (empty) implementation provided by 
 * SysCallback will be used.
 *
 * @code
 *
 *  const SysCallback = scripting.addModule("/ti/sysbios/runtime/SysCallback");
 *  SysCallback.abortFxn = "userAbort";
 *  SysCallback.exitFxn  = "userExit";
 *  SysCallback.flushFxn = "userFlush";
 *  SysCallback.abortFxn = "userInit";
 *  SysCallback.putchFxn = "userPutch";
 *  SysCallback.readyFxn = "userReady";
 *
 * @endcode
 */

#ifndef ti_sysbios_runtime_SysCallback__include
#define ti_sysbios_runtime_SysCallback__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_SysCallback_long_names
#include "SysCallback_defs.h"

extern void SysCallback_init(void);
/*! @endcond */

/*!
 * @brief Default abort function that does nothing
 *
 * This default abort function spins forever and never returns.
 */
extern void SysCallback_defaultAbort(const char * str);

/*!
 * @brief Default exit function that does nothing
 *
 * The default exit function does nothing and returns.
 */
extern void SysCallback_defaultExit(int stat);

/*!
 * @brief Default flush function that does nothing
 *
 * The default flush function does nothing and returns.
 */
extern void SysCallback_defaultFlush(void);

/*!
 * @brief Default init function that does nothing
 *
 * The default init function does nothing and returns.
 */
extern void SysCallback_defaultInit(void);

/*!
 * @brief Default putch function that does nothing
 *
 * The default putch function drops the characters.
 */
extern void SysCallback_defaultPutch(char c);
/*!
 * @brief Default ready function that does nothing
 *
 * The default ready function returns `TRUE` always.
 */
extern bool SysCallback_defaultReady(void);

#endif /* ti_sysbios_runtime_SysCallback__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_SysCallback_long_names
#include "SysCallback_defs.h"
/*! @endcond */
