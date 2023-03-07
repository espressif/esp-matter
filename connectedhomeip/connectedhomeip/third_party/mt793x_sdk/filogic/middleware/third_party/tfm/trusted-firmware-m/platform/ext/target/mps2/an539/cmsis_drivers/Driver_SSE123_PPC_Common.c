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

/* Driver version */
#define ARM_PPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_PPC_API_VERSION,
    ARM_PPC_DRV_VERSION
};

ARM_DRIVER_VERSION PPC_SSE123_GetVersion(void)
{
    return DriverVersion;
}

int32_t PPC_SSE123_Initialize(SSE123_PPC_Resources *ppc_dev)
{
    ppc_sse123_init(ppc_dev->dev);

    return ARM_DRIVER_OK;
}

int32_t PPC_SSE123_Uninitialize(void)
{
    /* Nothing to do */
    return ARM_DRIVER_OK;
}

int32_t PPC_SSE123_ConfigPrivilege(SSE123_PPC_Resources *ppc_dev,
                                   uint32_t periph,
                                   PPC_SSE123_SecAttr sec_attr,
                                   PPC_SSE123_PrivAttr priv_attr)
{
    enum ppc_sse123_error_t ret;

    ret = ppc_sse123_config_privilege(ppc_dev->dev, periph,
                                      (enum ppc_sse123_sec_attr_t)sec_attr,
                                      (enum ppc_sse123_priv_attr_t)priv_attr);

    if( ret != PPC_SSE123_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

bool PPC_SSE123_IsPeriphPrivOnly(SSE123_PPC_Resources *ppc_dev,
                                 uint32_t periph)
{
    return ppc_sse123_is_periph_priv_only(ppc_dev->dev, periph);
}

/* Secure only functions */
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

int32_t PPC_SSE123_ConfigSecurity(SSE123_PPC_Resources *ppc_dev,
                                  uint32_t periph,
                                  PPC_SSE123_SecAttr sec_attr)
{
    enum ppc_sse123_error_t ret;

    ret = ppc_sse123_config_security(ppc_dev->dev, periph,
                                     (enum ppc_sse123_sec_attr_t)sec_attr);

    if( ret != PPC_SSE123_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

bool PPC_SSE123_IsPeriphSecure(SSE123_PPC_Resources *ppc_dev, uint32_t periph)
{
    return ppc_sse123_is_periph_secure(ppc_dev->dev, periph);
}

int32_t PPC_SSE123_EnableInterrupt(SSE123_PPC_Resources *ppc_dev)
{
    enum ppc_sse123_error_t ret;

    ret = ppc_sse123_irq_enable(ppc_dev->dev);

    if( ret != PPC_SSE123_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

void PPC_SSE123_DisableInterrupt(SSE123_PPC_Resources *ppc_dev)
{
    ppc_sse123_irq_disable(ppc_dev->dev);
}

void PPC_SSE123_ClearInterrupt(SSE123_PPC_Resources *ppc_dev)
{
    ppc_sse123_clear_irq(ppc_dev->dev);
}

bool PPC_SSE123_InterruptState(SSE123_PPC_Resources *ppc_dev)
{
    return ppc_sse123_irq_state(ppc_dev->dev);
}

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */


