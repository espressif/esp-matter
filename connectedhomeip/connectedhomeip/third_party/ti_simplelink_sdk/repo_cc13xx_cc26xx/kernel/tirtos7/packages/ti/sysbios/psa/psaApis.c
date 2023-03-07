/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== psaApis.c ========
 *  SYS/BIOS specific implementations of ARM PSA Non Secure APIs
 */

#include <stdint.h>
#include <stdbool.h>

#include <psa/client.h>
#include <tfm_api.h>
#include <tfm_ns_interface.h>

#ifdef PSA_gateType_D
PSA_GateStruct PSA_gate;
#endif

/**** API functions ****/

uint32_t psa_framework_version(void)
{
    uint32_t result;
    PSA_gateKey;

    PSA_gateEnter();
    result = tfm_psa_framework_version_veneer();
    PSA_gateLeave(key);

    return (result);
}

uint32_t psa_version(uint32_t sid)
{
    uint32_t result;
    PSA_gateKey;

    PSA_gateEnter();
    result = tfm_psa_version_veneer(sid);
    PSA_gateLeave(key);

    return (result);
}

psa_handle_t psa_connect(uint32_t sid, uint32_t minor_version)
{
    psa_handle_t handle;
    PSA_gateKey;

    PSA_gateEnter();
    handle = tfm_psa_connect_veneer(sid, minor_version);
    PSA_gateLeave(key);

    return (handle);
}

psa_status_t psa_call(psa_handle_t handle, int32_t type,
                      const psa_invec *in_vec,
                      size_t in_len,
                      psa_outvec *out_vec,
                      size_t out_len)
{
    psa_status_t result;
    PSA_gateKey;

    /*
     * FixMe: sanity check can be added to offload some NS thread checks from
     * TFM secure API
     */

    /*
     * Due to v8M restrictions, TF-M NS API needs to add another layer of
     * serialization in order for NS to pass arguments to S
     */
    const struct tfm_control_parameter_t ctrl_param = {
        .type = type,
        .in_len = in_len,
        .out_len = out_len,
    };

    PSA_gateEnter();
    result = tfm_psa_call_veneer((uint32_t)handle, &ctrl_param, in_vec, out_vec);
    PSA_gateLeave(key);

    return (result);
}

void psa_close(psa_handle_t handle)
{
    PSA_gateKey;

    PSA_gateEnter();
    tfm_psa_close_veneer(handle);
    PSA_gateLeave(key);
}

/**
 * \brief NS world, Init NS lock
 */
void PSA_init()
{
    PSA_gateConstruct(NULL);
}

/*
 * NS lock based dispatcher
 *
 * Provided for use with standardized secure function implementations
 * that route all veneer calls through this common function.
 */
int32_t tfm_ns_interface_dispatch(veneer_fn fn,
                              uint32_t arg0, uint32_t arg1,
                              uint32_t arg2, uint32_t arg3)
{
    uint32_t result;
    PSA_gateKey;

    PSA_gateEnter();
    result = fn(arg0, arg1, arg2, arg3);
    PSA_gateLeave(key);

    return result;
}
