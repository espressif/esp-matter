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

#ifndef __PPC_SSE123_COMMON_H__
#define __PPC_SSE123_COMMON_H__

#include "Driver_Common.h"
#include "Driver_SSE123_PPC.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SSE123_PPC_Resources {
    struct ppc_sse123_dev_t* dev;       /* PPC device structure */
} SSE123_PPC_Resources;

/**
 * \brief       Get driver version.
 *
 * \return      \ref ARM_DRIVER_VERSION
 */
ARM_DRIVER_VERSION PPC_SSE123_GetVersion(void);

/**
 * \brief     Initializes PPC Interface.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 *
 * \return    Returns SSE-123 PPC error code.
 */
int32_t PPC_SSE123_Initialize(SSE123_PPC_Resources *ppc_dev);

/**
 * \brief     De-initializes PPC Interface.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 *
 * \return    Returns SSE-123 PPC error code.
 */
int32_t PPC_SSE123_Uninitialize(void);

/**
 * \brief     Configures a peripheral controlled by the given PPC
 *            in the given security domain with privileged and
 *            unprivileged access or privileged access only.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 * \param[in] periph     Peripheral position in SPCTRL and NSPCTRL registers.
 * \param[in] sec_attr   Secure attribute value.
 * \param[in] priv_attr  Privilege attribute value.
 *
 * \return    Returns SSE-123 PPC error code.
 */
int32_t PPC_SSE123_ConfigPrivilege(SSE123_PPC_Resources *ppc_dev,
                                   uint32_t periph,
                                   PPC_SSE123_SecAttr sec_attr,
                                   PPC_SSE123_PrivAttr priv_attr);

/**
 * \brief     Checks if the peripheral is configured to be privilege only
 *                     - with non-secure caller in the non-secure domain
 *                     - with secure caller in the configured security domain
 * \param[in] ppc_dev  PPC device \ref SSE123_PPC_Resources
 * \param[in] periph   Peripheral position in SPCTRL and NSPCTRL registers.
 *
 * \return    Returns true if the peripheral is configured as privilege access
 *            only, false for privilege and unprivilege access mode.
 */
bool PPC_SSE123_IsPeriphPrivOnly(SSE123_PPC_Resources *ppc_dev,
                                 uint32_t periph);

/* Secure only functions */
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

/**
 * \brief     Configures a peripheral controlled by the given PPC with secure
 *            or non-secure access only
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 * \param[in] periph     Peripheral position in SPCTRL and NSPCTRL registers.
 * \param[in] sec_attr   Secure attribute value.
 *
 * \return    Returns SSE-123 PPC error code.
 */
int32_t PPC_SSE123_ConfigSecurity(SSE123_PPC_Resources *ppc_dev,
                                  uint32_t periph,
                                  PPC_SSE123_SecAttr sec_attr);

/**
 * \brief     Checks if the peripheral is configured to be secure.
 *
 * \param[in] ppc_dev  PPC device \ref SSE123_PPC_Resources
 * \param[in] periph   Peripheral position in SPCTRL and NSPCTRL registers.
 *
 * \return    Returns true if the peripheral is configured as secure,
 *            false for non-secure.
 */
bool PPC_SSE123_IsPeriphSecure(SSE123_PPC_Resources *ppc_dev, uint32_t periph);

/**
 * \brief     Enables PPC interrupt.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 *
 * \return    Returns SSE-123 PPC error code.
 */
int32_t PPC_SSE123_EnableInterrupt(SSE123_PPC_Resources *ppc_dev);

/**
 * \brief     Disables PPC interrupt.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 */
void PPC_SSE123_DisableInterrupt(SSE123_PPC_Resources *ppc_dev);

/**
 * \brief     Clears PPC interrupt.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 */
void PPC_SSE123_ClearInterrupt(SSE123_PPC_Resources *ppc_dev);

/**
 * \brief     Gets PPC interrupt state.
 *
 * \param[in] ppc_dev    PPC device \ref SSE123_PPC_Resources
 *
 * \return    Returns true if the interrupt is active, false otherwise
 */
bool PPC_SSE123_InterruptState(SSE123_PPC_Resources *ppc_dev);

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */

#ifdef __cplusplus
}
#endif
#endif /* __PPC_SSE123_COMMON_H__ */
