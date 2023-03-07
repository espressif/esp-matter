/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
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
 *  ======== LoggerStopMode__epilogue.h ========
 */

#include <ti/uia/runtime/UIAPacket.h>

#ifdef xdc_target__bigEndian
#define ti_uia_loggers_LoggerStopMode_ENDIANNESS \
        ti_uia_runtime_UIAPacket_PayloadEndian_BIG
#else
#define ti_uia_loggers_LoggerStopMode_ENDIANNESS \
        ti_uia_runtime_UIAPacket_PayloadEndian_LITTLE
#endif

/*
 *  Length and sequence number are initialized to 0xFFFF since
 *  they will be set by the module when the buffer is full.
 *  Priority of LoggerStopMode is 0 and the instance ID is 1
 *  because it is a module based logger.
 */
#define ti_uia_loggers_LoggerStopMode_DUMMY_LEN 0
#define ti_uia_loggers_LoggerStopMode_DUMMY_SEQ 0
#define ti_uia_loggers_LoggerStopMode_PRIORITY 0
#define ti_uia_loggers_LoggerStopMode_INSTANCE_ID 1
