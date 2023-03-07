/*
 * Copyright (c) 2019 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_SSE123_PPC_Common.h"

#include "cmsis.h"
#include "cmsis_driver_config.h"
#include "RTE_Device.h"

#if (RTE_PPC_SSE123_APB_EXP0)

static SSE123_PPC_Resources APB_EXP0_PPC_DEV = {
    .dev = &PPC_SSE123_APB_EXP0_DEV,
};

/* APB PPCEXP0 Driver wrapper functions */
static int32_t PPC_SSE123_APB_EXP0_Initialize(void)
{
    return PPC_SSE123_Initialize(&APB_EXP0_PPC_DEV);
}

static int32_t PPC_SSE123_APB_EXP0_Uninitialize(void)
{
    return PPC_SSE123_Uninitialize();
}

static int32_t
PPC_SSE123_APB_EXP0_ConfigPrivilege(uint32_t periph,
                                    PPC_SSE123_SecAttr sec_attr,
                                    PPC_SSE123_PrivAttr priv_attr)
{
    return PPC_SSE123_ConfigPrivilege(&APB_EXP0_PPC_DEV, periph, sec_attr,
                                      priv_attr);
}

static bool PPC_SSE123_APB_EXP0_IsPeriphPrivOnly(uint32_t periph)
{
    return PPC_SSE123_IsPeriphPrivOnly(&APB_EXP0_PPC_DEV, periph);
}

/* Secure only functions */
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

static int32_t
PPC_SSE123_APB_EXP0_ConfigSecurity(uint32_t periph,
                                   PPC_SSE123_SecAttr sec_attr)
{
    return PPC_SSE123_ConfigSecurity(&APB_EXP0_PPC_DEV, periph, sec_attr);
}

static bool PPC_SSE123_APB_EXP0_IsPeriphSecure(uint32_t periph)
{
    return PPC_SSE123_IsPeriphSecure(&APB_EXP0_PPC_DEV, periph);
}

static int32_t PPC_SSE123_APB_EXP0_EnableInterrupt(void)
{
    return PPC_SSE123_EnableInterrupt(&APB_EXP0_PPC_DEV);
}

static void PPC_SSE123_APB_EXP0_DisableInterrupt(void)
{
    PPC_SSE123_DisableInterrupt(&APB_EXP0_PPC_DEV);
}

static void PPC_SSE123_APB_EXP0_ClearInterrupt(void)
{
    PPC_SSE123_ClearInterrupt(&APB_EXP0_PPC_DEV);
}

static bool PPC_SSE123_APB_EXP0_InterruptState(void)
{
    return PPC_SSE123_InterruptState(&APB_EXP0_PPC_DEV);
}

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */

/* PPC SSE-123 APB EXP0 Driver CMSIS access structure */
DRIVER_PPC_SSE123 Driver_PPC_SSE123_APB_EXP0 = {
    .GetVersion        = PPC_SSE123_GetVersion,
    .Initialize        = PPC_SSE123_APB_EXP0_Initialize,
    .Uninitialize      = PPC_SSE123_APB_EXP0_Uninitialize,
    .ConfigPrivilege   = PPC_SSE123_APB_EXP0_ConfigPrivilege,
    .IsPeriphPrivOnly  = PPC_SSE123_APB_EXP0_IsPeriphPrivOnly,
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    .ConfigSecurity    = PPC_SSE123_APB_EXP0_ConfigSecurity,
    .IsPeriphSecure    = PPC_SSE123_APB_EXP0_IsPeriphSecure,
    .EnableInterrupt   = PPC_SSE123_APB_EXP0_EnableInterrupt,
    .DisableInterrupt  = PPC_SSE123_APB_EXP0_DisableInterrupt,
    .ClearInterrupt    = PPC_SSE123_APB_EXP0_ClearInterrupt,
    .InterruptState    = PPC_SSE123_APB_EXP0_InterruptState
#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */
};
#endif /* RTE_PPC_SSE123_APB_EXP0 */

