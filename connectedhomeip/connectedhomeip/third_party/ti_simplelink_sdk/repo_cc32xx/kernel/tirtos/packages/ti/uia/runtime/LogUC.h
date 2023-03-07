/*
 * Copyright (c) 2012-2016, Texas Instruments Incorporated
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
 * */

#ifndef LOGUC_H_
#define LOGUC_H_

#include <xdc/runtime/Log.h>
/*
 *  This file extends the Log API macros that are defined in
 *  xdc.runtime.Log_epilogue.h with versions of the event logging
 *  APIs that do not test for the condition:
 *  ((Module__LOGDEF && xdc_runtime_Diags_query(evt)).
 *
 *  This removes the CPU overhead associated with
 *  checking the Diags masks associated with the current module,
 *  at the cost of no longer be able to control which events are
 *  logged via the Diags masks associated with the module.
 *
 *  Note:  This file includes <xdc/runtime/Log.h>, which includes
 *  <xdc/runtime/Log__epilogue.h>, where the pre-processor symbols:
 *
 *      xdc_runtime_Log_DISABLE_ALL
 *      xdc_runtime_Log_ENABLE_ERROR
 *      xdc_runtime_Log_ENABLE_INFO
 *      xdc_runtime_Log_ENABLE_WARNING
 *
 *  are defined, if not already defined.  The Log calls can be compiled out
 *  by defining the pre-processor symbol, xdc_runtime_Log_DISABLE_ALL.  To
 *  remove all logging except for errors, define xdc_runtime_Log_DISABLE_ALL
 *  and xdc_runtime_Log_ENABLE_ERROR.  See the cdoc for xdc.runtime.Log for
 *  further documentation on compiling out Log events.
 */

/*
 *  ======== xdc_runtime_Log_writeUC* ========
 */
#if (!xdc_runtime_Log_DISABLE_ALL)

#define Log_writeUC0(evt) \
    (xdc_runtime_Log_doPut0((evt), Module__MID))

#define Log_writeUC1(evt, a1) \
        (xdc_runtime_Log_doPut1((evt), Module__MID, (a1)))

#define Log_writeUC2(evt, a1, a2) \
        (xdc_runtime_Log_doPut2((evt), Module__MID, (a1), (a2)))

#define Log_writeUC3(evt, a1, a2, a3) \
        (xdc_runtime_Log_doPut4((evt), Module__MID, (a1), (a2), (a3), 0))

#define Log_writeUC4(evt, a1, a2, a3, a4) \
        (xdc_runtime_Log_doPut4((evt), Module__MID, (a1), (a2), (a3), (a4)))

#define Log_writeUC5(evt, a1, a2, a3, a4, a5) \
        (xdc_runtime_Log_doPut8((evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), 0, 0, 0))

#define Log_writeUC6(evt, a1, a2, a3, a4, a5, a6) \
        (xdc_runtime_Log_doPut8((evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), (a6), 0, 0))

#define Log_writeUC7(evt, a1, a2, a3, a4, a5, a6, a7) \
        (xdc_runtime_Log_doPut8((evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), (a6), (a7), 0))

#define Log_writeUC8(evt, a1, a2, a3, a4, a5, a6, a7, a8) \
        (xdc_runtime_Log_doPut8((evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8)))

#else

#define Log_writeUC0(evt)
#define Log_writeUC1(evt, a1)
#define Log_writeUC2(evt, a1, a2)
#define Log_writeUC3(evt, a1, a2, a3)
#define Log_writeUC4(evt, a1, a2, a3, a4)
#define Log_writeUC5(evt, a1, a2, a3, a4, a5)
#define Log_writeUC6(evt, a1, a2, a3, a4, a5, a6)
#define Log_writeUC7(evt, a1, a2, a3, a4, a5, a6, a7)
#define Log_writeUC8(evt, a1, a2, a3, a4, a5, a6, a7, a8)

#endif

#if (!xdc_runtime_Log_DISABLE_ALL)

#define Log_iwriteUC0(inst, evt) \
    (Module__LOGFXN0((inst), (evt), Module__MID))

#define Log_iwriteUC1(inst, evt, a1) \
        (Module__LOGFXN1((inst), (evt), Module__MID, (a1)))

#define Log_iwriteUC2(inst, evt, a1, a2) \
        (Module__LOGFXN2((inst), (evt), Module__MID, (a1), (a2)))

#define Log_iwriteUC3(inst, evt, a1, a2, a3) \
        (Module__LOGFXN4((inst), (evt), Module__MID, (a1), (a2), (a3), 0))

#define Log_iwriteUC4(inst, evt, a1, a2, a3, a4) \
        (Module__LOGFXN4((inst), (evt), Module__MID, (a1), (a2), (a3), (a4)))

#define Log_iwriteUC5(inst, evt, a1, a2, a3, a4, a5) \
        (Module__LOGFXN8((inst), (evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), 0, 0, 0))

#define Log_iwriteUC6(inst, evt, a1, a2, a3, a4, a5, a6) \
        (Module__LOGFXN8((inst), (evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), (a6), 0, 0))

#define Log_iwriteUC7(inst, evt, a1, a2, a3, a4, a5, a6, a7) \
        (Module__LOGFXN8((inst), (evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), (a6), (a7), 0))

#define Log_iwriteUC8(inst, evt, a1, a2, a3, a4, a5, a6, a7, a8) \
        (Module__LOGFXN8((inst), (evt), Module__MID, \
            (a1), (a2), (a3), (a4), (a5), (a6), (a7), (a8)))
#else

#define Log_iwriteUC0(inst, evt)
#define Log_iwriteUC1(inst, evt, a1)
#define Log_iwriteUC2(inst, evt, a1, a2)
#define Log_iwriteUC3(inst, evt, a1, a2, a3)
#define Log_iwriteUC4(inst, evt, a1, a2, a3, a4)
#define Log_iwriteUC5(inst, evt, a1, a2, a3, a4, a5)
#define Log_iwriteUC6(inst, evt, a1, a2, a3, a4, a5, a6)
#define Log_iwriteUC7(inst, evt, a1, a2, a3, a4, a5, a6, a7)
#define Log_iwriteUC8(inst, evt, a1, a2, a3, a4, a5, a6, a7, a8)

#endif


/*
 *  ======== xdc_runtime_Log_printUC* ========
 */
#if (!xdc_runtime_Log_DISABLE_ALL)

#define Log_printUC0(mask, fmt) \
    (xdc_runtime_Log_doPut1(mask, Module__MID, (IArg) fmt))

#define Log_printUC1(mask, fmt, a1) \
    (xdc_runtime_Log_doPut2(mask, Module__MID, (IArg) fmt, (a1)))

#define Log_printUC2(mask, fmt, a1, a2) \
    (xdc_runtime_Log_doPut4(mask, Module__MID, (IArg) fmt,(a1), (a2), 0))

#define Log_printUC3(mask, fmt, a1, a2, a3) \
    (xdc_runtime_Log_doPut4(mask, Module__MID, (IArg) fmt,(a1), (a2), (a3)))

#define Log_printUC4(mask, fmt, a1, a2, a3, a4) \
    (xdc_runtime_Log_doPut8(mask, Module__MID, \
            (IArg)fmt, (a1), (a2), (a3), (a4), 0, 0, 0))

#define Log_printUC5(mask, fmt, a1, a2, a3, a4, a5) \
    (xdc_runtime_Log_doPut8(mask, Module__MID, \
            (IArg)fmt, (a1), (a2), (a3), (a4), (a5), 0, 0))

#define Log_printUC6(mask, fmt, a1, a2, a3, a4, a5, a6) \
    (xdc_runtime_Log_doPut8(mask, Module__MID, \
            (IArg)fmt, (a1), (a2), (a3), (a4), (a5), (a6), 0))
#else

#define Log_printUC0(mask, fmt)
#define Log_printUC1(mask, fmt, a1)
#define Log_printUC2(mask, fmt, a1, a2)
#define Log_printUC3(mask, fmt, a1, a2, a3)
#define Log_printUC4(mask, fmt, a1, a2, a3, a4)
#define Log_printUC5(mask, fmt, a1, a2, a3, a4, a5)
#define Log_printUC6(mask, fmt, a1, a2, a3, a4, a5, a6)

#endif

/*
 *  ======== xdc_runtime_Log_iprintUC* ========
 *  Instead of passing Module__LOGOBJ to Module__LOGFXN*, the user gets
 *  to pass a Logger instance of their choice.
 *  Note that the Log_iprintUCn functions do not take a mask parameter,
 *  as the logger instance write functions do not take a mask.
 */
#if (!xdc_runtime_Log_DISABLE_ALL)

#define Log_iprintUC0(inst, fmt) \
    Module__LOGFXN1((inst), 0, Module__MID, (IArg)fmt)

#define Log_iprintUC1(inst, fmt, a1) \
    Module__LOGFXN2((inst), 0, Module__MID, (IArg)fmt, (a1))

#define Log_iprintUC2(inst, fmt, a1, a2) \
    Module__LOGFXN4((inst), 0, Module__MID, (IArg)fmt, (a1), (a2), 0)

#define Log_iprintUC3(inst, fmt, a1, a2, a3) \
    Module__LOGFXN4((inst), 0, Module__MID, (IArg)fmt, (a1), (a2), (a3))

#define Log_iprintUC4(inst, fmt, a1, a2, a3, a4) \
    Module__LOGFXN8((inst), 0, Module__MID, \
            (IArg)fmt, (a1), (a2), (a3), (a4), 0, 0, 0)

#define Log_iprintUC5(inst, fmt, a1, a2, a3, a4, a5) \
    Module__LOGFXN8((inst), 0, Module__MID, \
            (IArg)fmt, (a1), (a2), (a3), (a4), (a5), 0, 0)

#define Log_iprintUC6(inst, fmt, a1, a2, a3, a4, a5, a6) \
    Module__LOGFXN8((inst), 0, Module__MID, \
            (IArg)fmt, (a1), (a2), (a3), (a4), (a5), (a6), 0)

#else

#define Log_iprintUC0(inst, fmt)
#define Log_iprintUC1(inst, fmt, a1)
#define Log_iprintUC2(inst, fmt, a1, a2)
#define Log_iprintUC3(inst, fmt, a1, a2, a3)
#define Log_iprintUC4(inst, fmt, a1, a2, a3, a4)
#define Log_iprintUC5(inst, fmt, a1, a2, a3, a4, a5)
#define Log_iprintUC6(inst, fmt, a1, a2, a3, a4, a5, a6)

#endif

/*
 *  ======== xdc_runtime_Log_errorUC* ========
 */
#if xdc_runtime_Log_ENABLE_ERROR

#define Log_errorUC0(fmt) \
    (xdc_runtime_Log_doPut4((Log_L_error), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, 0))

#define Log_errorUC1(fmt, a1) \
    (xdc_runtime_Log_doPut4((Log_L_error), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, (a1)))

#define Log_errorUC2(fmt, a1, a2) \
    (xdc_runtime_Log_doPut8((Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), 0, 0, 0))

#define Log_errorUC3(fmt, a1, a2, a3) \
    (xdc_runtime_Log_doPut8((Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), 0, 0))

#define Log_errorUC4(fmt, a1, a2, a3, a4) \
    (xdc_runtime_Log_doPut8((Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), 0))

#define Log_errorUC5(fmt, a1, a2, a3, a4, a5) \
    (xdc_runtime_Log_doPut8((Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), \
             (a4), (a5)))

#else

#define Log_errorUC0(fmt)
#define Log_errorUC1(fmt, a1)
#define Log_errorUC2(fmt, a1, a2)
#define Log_errorUC3(fmt, a1, a2, a3)
#define Log_errorUC4(fmt, a1, a2, a3, a4)
#define Log_errorUC5(fmt, a1, a2, a3, a4, a5)

#endif

/*
 *  ======== xdc_runtime_Log_ierrorUC* ========
 */

#if xdc_runtime_Log_ENABLE_ERROR

#define Log_ierrorUC0(inst, fmt) \
    (Module__LOGFXN4((inst), (Log_L_error), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, 0))

#define Log_ierrorUC1(inst, fmt, a1) \
    (Module__LOGFXN4((inst), (Log_L_error), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, (a1)))

#define Log_ierrorUC2(inst, fmt, a1, a2) \
    (Module__LOGFXN8((inst), (Log_L_error), Module__MID,  (IArg)xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), 0, 0, 0))

#define Log_ierrorUC3(inst, fmt, a1, a2, a3) \
    (Module__LOGFXN8((inst), (Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), 0, 0))

#define Log_ierrorUC4(inst, fmt, a1, a2, a3, a4) \
    (Module__LOGFXN8((inst), (Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), 0))

#define Log_ierrorUC5(inst, fmt, a1, a2, a3, a4, a5) \
    (Module__LOGFXN8((inst), (Log_L_error), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), (a5)))

#else

#define Log_ierrorUC0(inst, fmt)
#define Log_ierrorUC1(inst, fmt, a1)
#define Log_ierrorUC2(inst, fmt, a1, a2)
#define Log_ierrorUC3(inst, fmt, a1, a2, a3)
#define Log_ierrorUC4(inst, fmt, a1, a2, a3, a4)
#define Log_ierrorUC5(inst, fmt, a1, a2, a3, a4, a5)

#endif

/*
 *  ======== xdc_runtime_Log_warningUC* ========
 */
#if xdc_runtime_Log_ENABLE_WARNING

#define Log_warningUC0(fmt) \
    (xdc_runtime_Log_doPut4((Log_L_warning), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, 0))

#define Log_warningUC1(fmt, a1) \
    (xdc_runtime_Log_doPut4((Log_L_warning), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, (a1)))

#define Log_warningUC2(fmt, a1, a2) \
    (xdc_runtime_Log_doPut8((Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), 0, 0, 0))

#define Log_warningUC3(fmt, a1, a2, a3) \
    (xdc_runtime_Log_doPut8((Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), 0, 0))

#define Log_warningUC4(fmt, a1, a2, a3, a4) \
    (xdc_runtime_Log_doPut8((Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), 0))

#define Log_warningUC5(fmt, a1, a2, a3, a4, a5) \
    (xdc_runtime_Log_doPut8((Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), \
             (a4), (a5)))

#else

#define Log_warningUC0(fmt)
#define Log_warningUC1(fmt, a1)
#define Log_warningUC2(fmt, a1, a2)
#define Log_warningUC3(fmt, a1, a2, a3)
#define Log_warningUC4(fmt, a1, a2, a3, a4)
#define Log_warningUC5(fmt, a1, a2, a3, a4, a5)

#endif

/*
 *  ======== xdc_runtime_Log_iwarningUC* ========
 */

#if xdc_runtime_Log_ENABLE_WARNING

#define Log_iwarningUC0(inst, fmt) \
    (Module__LOGFXN4((inst), (Log_L_warning), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, 0))

#define Log_iwarningUC1(inst, fmt, a1) \
    (Module__LOGFXN4((inst), (Log_L_warning), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, (a1)))

#define Log_iwarningUC2(inst, fmt, a1, a2) \
    (Module__LOGFXN8((inst), (Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), 0, 0, 0))

#define Log_iwarningUC3(inst, fmt, a1, a2, a3) \
    (Module__LOGFXN8((inst), (Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), 0, 0))

#define Log_iwarningUC4(inst, fmt, a1, a2, a3, a4) \
    (Module__LOGFXN8((inst), (Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), 0))

#define Log_iwarningUC5(inst, fmt, a1, a2, a3, a4, a5) \
    (Module__LOGFXN8((inst), (Log_L_warning), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), \
             (a4), (a5)))

#else

#define Log_iwarningUC0(inst, fmt)
#define Log_iwarningUC1(inst, fmt, a1)
#define Log_iwarningUC2(inst, fmt, a1, a2)
#define Log_iwarningUC3(inst, fmt, a1, a2, a3)
#define Log_iwarningUC4(inst, fmt, a1, a2, a3, a4)
#define Log_iwarningUC5(inst, fmt, a1, a2, a3, a4, a5)

#endif

/*
 *  ======== xdc_runtime_Log_info* ========
 */
#if xdc_runtime_Log_ENABLE_INFO

#define Log_infoUC0(fmt) \
    (xdc_runtime_Log_doPut4((Log_L_info), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, 0))

#define Log_infoUC1(fmt, a1) \
    (xdc_runtime_Log_doPut4((Log_L_info), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, (a1)))

#define Log_infoUC2(fmt, a1, a2) \
    (xdc_runtime_Log_doPut8((Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), 0, 0, 0))

#define Log_infoUC3(fmt, a1, a2, a3) \
    (xdc_runtime_Log_doPut8((Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), 0, 0))

#define Log_infoUC4(fmt, a1, a2, a3, a4) \
    (xdc_runtime_Log_doPut8((Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), 0))

#define Log_infoUC5(fmt, a1, a2, a3, a4, a5) \
    (xdc_runtime_Log_doPut8((Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), \
             (a4), (a5)))

#else

#define Log_infoUC0(fmt)
#define Log_infoUC1(fmt, a1)
#define Log_infoUC2(fmt, a1, a2)
#define Log_infoUC3(fmt, a1, a2, a3)
#define Log_infoUC4(fmt, a1, a2, a3, a4)
#define Log_infoUC5(fmt, a1, a2, a3, a4, a5)

#endif

/*
 *  ======== xdc_runtime_Log_iinfo* ========
 */

#if xdc_runtime_Log_ENABLE_INFO

#define Log_iinfoUC0(inst, fmt) \
    (Module__LOGFXN4((inst), (Log_L_info), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, 0))

#define Log_iinfoUC1(inst, fmt, a1) \
    (Module__LOGFXN4((inst), (Log_L_info), Module__MID, \
            (IArg) xdc_FILE__, (IArg)__LINE__, (IArg)fmt, (a1)))

#define Log_iinfoUC2(inst, fmt, a1, a2) \
    (Module__LOGFXN8((inst), (Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), 0, 0, 0))

#define Log_iinfoUC3(inst, fmt, a1, a2, a3) \
    (Module__LOGFXN8((inst), (Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), 0, 0))

#define Log_iinfoUC4(inst, fmt, a1, a2, a3, a4) \
    (Module__LOGFXN8((inst), (Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), (a4), 0))

#define Log_iinfoUC5(inst, fmt, a1, a2, a3, a4, a5) \
    (Module__LOGFXN8((inst), (Log_L_info), Module__MID, (IArg) xdc_FILE__, \
             (IArg)__LINE__, (IArg)fmt, (a1), (a2), (a3), \
             (a4), (a5)))

#else

#define Log_iinfoUC0(inst, fmt)
#define Log_iinfoUC1(inst, fmt, a1)
#define Log_iinfoUC2(inst, fmt, a1, a2)
#define Log_iinfoUC3(inst, fmt, a1, a2, a3)
#define Log_iinfoUC4(inst, fmt, a1, a2, a3, a4)
#define Log_iinfoUC5(inst, fmt, a1, a2, a3, a4, a5)

#endif

#endif /* LOGUC_H_ */
