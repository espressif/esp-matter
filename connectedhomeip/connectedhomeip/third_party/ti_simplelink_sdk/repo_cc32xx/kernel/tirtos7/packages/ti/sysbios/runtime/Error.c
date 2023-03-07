/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated - http://www.ti.com
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
 * ======== Error.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/System.h>

Error_Block Error_ignore = {
    .id = NULL,
    .a0 = 0,
    .a1 = 0
};


/*
 * ======== Error_check ========
 */
bool Error_check(Error_Block *eb)
{
    /* The condition eb->id != NULL rejects Error_IGNORE */
    return (Error_policy_D == Error_UNWIND && eb != NULL && eb->id != NULL);
}

/*
 * ======== Error_init ========
 */
void Error_init(Error_Block *eb)
{
    if (eb != NULL && eb != &Error_ignore) {
        eb->id = NULL;
    }
}

/*
 * ======== Error_print ========
 */
void Error_print(Error_Block *eb)
{
    if (eb == NULL) {
        return;
    }

    if (Error_retainStrings_D && (eb->id != NULL)) {
        System_printf(eb->id, eb->a0, eb->a1);
        System_printf("\n");
    }
    else {
        System_printf("id: 0x%x a0: 0x%x a1: 0x%x\n", eb->id, eb->a0, eb->a1);
    }
}

/*
 * use an index instead of a string to avoid duplicate strings in .out
 */
static const char * prefixStrings[2] = {
    "Error raised: ",
    "Assert failed: "
};

/*
 *  ======== Error_raiseX ========
 */
void Error_raiseX(Error_Block *eb, int prefix, const char * id, intptr_t a0, intptr_t a1)
{
    Error_Block defErr;

    if (eb == Error_IGNORE) {
        return;
    }

    if (eb == NULL) {
        eb = &defErr;
    }

    /* Set up the error-block information */
    eb->id = id;
    eb->a0 = a0;
    eb->a1 = a1;

    /* call optional hook function if configured */
    if (Error_raiseHook_D) {
        Error_raiseHookFxn(eb);
    }

    if (Error_printDetails_D) {
        System_printf(prefixStrings[prefix]);
        Error_print(eb);
    }

    if (Error_policy_D == Error_SPIN) {
        for(;;) {
        }
    }
    else if (((eb == &defErr) && (Error_policy_D == Error_UNWIND)) || (Error_policy_D == Error_TERMINATE)) {
        System_abort("ti_sysbios_runtime_Error_raise: terminating execution\n");
    }
}
