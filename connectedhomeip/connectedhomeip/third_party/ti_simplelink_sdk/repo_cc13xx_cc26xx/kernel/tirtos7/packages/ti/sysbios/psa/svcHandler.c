/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== svcHandler.c ========
 *  TF-M PSA Non-Secure Partition Management support code
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "svcHandler.h"

/*
 *  ======== ti_sysbios_family_arm_v8m_Hwi_svcHandlerHook ========
 */
uint32_t ti_sysbios_family_arm_v8m_Hwi_svcHandlerHook(uint32_t *svc_args, unsigned int lr)
{
    uint8_t svc_number = 0;

    svc_number = ((uint8_t *)svc_args[6])[-2];

    switch (svc_number) {
	case TZ_INIT_CONTEXT_SYSTEM:
	    svc_args[0] = TZ_InitContextSystem_S();
	    break;
	case TZ_ALLOC_MODULE_CONTEXT:
	    svc_args[0] = TZ_AllocModuleContext_S(svc_args[0]);
	    break;
	case TZ_FREE_MODULE_CONTEXT:
	    svc_args[0] = TZ_FreeModuleContext_S(svc_args[0]);
	    break;
	case TZ_LOAD_CONTEXT:
	    svc_args[0] = TZ_LoadContext_S(svc_args[0]);
	    break;
	case TZ_STORE_CONTEXT:
	    svc_args[0] = TZ_StoreContext_S(svc_args[0]);
	    break;
    }

    return (lr);
}

__attribute__ ((naked, aligned(4)))
uint32_t initContextSystem()
{
    __asm volatile("SVC %0           \n"
                   "BX LR            \n"
                    : : "I" (TZ_INIT_CONTEXT_SYSTEM));
}

__attribute__ ((naked))
uint32_t allocModuleContext(uint32_t modId)
{
    __asm volatile("SVC %0           \n"
                   "BX LR            \n"
                    : : "I" (TZ_ALLOC_MODULE_CONTEXT));
}

__attribute__ ((naked))
uint32_t freeModuleContext(uint32_t modId)
{
    __asm volatile("SVC %0           \n"
                   "BX LR            \n"
                    : : "I" (TZ_FREE_MODULE_CONTEXT));
}

__attribute__ ((naked))
uint32_t loadContext(uint32_t modId)
{
    __asm volatile("SVC %0           \n"
                   "BX LR            \n"
                    : : "I" (TZ_LOAD_CONTEXT));
}

__attribute__ ((naked))
uint32_t storeContext(uint32_t modId)
{
    __asm volatile("SVC %0           \n"
                   "BX LR            \n"
                    : : "I" (TZ_STORE_CONTEXT));
}

