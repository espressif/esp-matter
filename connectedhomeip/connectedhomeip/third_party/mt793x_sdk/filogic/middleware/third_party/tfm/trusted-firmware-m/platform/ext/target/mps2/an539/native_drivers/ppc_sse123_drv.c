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

#include "ppc_sse123_drv.h"
#include "platform_regs.h"

/* PPC state definition */
#define PPC_SSE123_INITIALIZED  1U

/* Default peripheral states */
#define SECURE_AS_DEFAULT_PERIPHERAL_STATE  true
#define PRIVILEGE_ONLY_AS_DEFAULT_PERIPHERAL_STATE  true

enum ppc_sse123_error_t ppc_sse123_init(struct ppc_sse123_dev_t* dev)
{
    struct sse123_spctrl_t* p_spctrl =
                         (struct sse123_spctrl_t*)dev->cfg->spctrl_base;
    struct sse123_nspctrl_t* p_nspctrl =
                       (struct sse123_nspctrl_t*)dev->cfg->nspctrl_base;

    switch(dev->cfg->ppc_name) {
        case PPC_SSE123_AHB_EXP0:
            dev->data->spctrl_ns_ppc   = &p_spctrl->ahbnsppcexp;
            dev->data->spctrl_sp_ppc   = &p_spctrl->ahbspppcexp;
            dev->data->nspctrl_nsp_ppc = &p_nspctrl->ahbnspppcexp;
            dev->data->int_bit_mask    = AHB_PPCEXP0_INT_POS_MASK;
            break;
        case PPC_SSE123_APB:
            dev->data->spctrl_ns_ppc   = &p_spctrl->apbnsppc;
            dev->data->spctrl_sp_ppc   = &p_spctrl->apbspppc;
            dev->data->nspctrl_nsp_ppc = &p_nspctrl->apbnspppc;
            dev->data->int_bit_mask    = APB_PPC0_INT_POS_MASK;
            break;
        case PPC_SSE123_APB_EXP0:
            dev->data->spctrl_ns_ppc   = &p_spctrl->apbnsppcexp0;
            dev->data->spctrl_sp_ppc   = &p_spctrl->apbspppcexp0;
            dev->data->nspctrl_nsp_ppc = &p_nspctrl->apbnspppcexp0;
            dev->data->int_bit_mask    = APB_PPCEXP0_INT_POS_MASK;
            break;
        case PPC_SSE123_APB_EXP1:
            dev->data->spctrl_ns_ppc   = &p_spctrl->apbnsppcexp1;
            dev->data->spctrl_sp_ppc   = &p_spctrl->apbspppcexp1;
            dev->data->nspctrl_nsp_ppc = &p_nspctrl->apbnspppcexp1;
            dev->data->int_bit_mask    = APB_PPCEXP1_INT_POS_MASK;
            break;
        case PPC_SSE123_APB_EXP2:
            dev->data->spctrl_ns_ppc   = &p_spctrl->apbnsppcexp2;
            dev->data->spctrl_sp_ppc   = &p_spctrl->apbspppcexp2;
            dev->data->nspctrl_nsp_ppc = &p_nspctrl->apbnspppcexp2;
            dev->data->int_bit_mask    = APB_PPCEXP2_INT_POS_MASK;
            break;
        case PPC_SSE123_APB_EXP3:
            dev->data->spctrl_ns_ppc   = &p_spctrl->apbnsppcexp3;
            dev->data->spctrl_sp_ppc   = &p_spctrl->apbspppcexp3;
            dev->data->nspctrl_nsp_ppc = &p_nspctrl->apbnspppcexp3;
            dev->data->int_bit_mask    = APB_PPCEXP3_INT_POS_MASK;
            break;
        default:
            return PPC_SSE123_ERR_INVALID_PARAM;
        }

    dev->data->state = PPC_SSE123_INITIALIZED;

    return PPC_SSE123_ERR_NONE;
}

enum ppc_sse123_error_t
ppc_sse123_config_privilege(struct ppc_sse123_dev_t* dev, uint8_t periph,
                            enum ppc_sse123_sec_attr_t sec_attr,
                            enum ppc_sse123_priv_attr_t priv_attr)
{
    if(dev->data->state != PPC_SSE123_INITIALIZED) {
        return PPC_SSE123_ERR_NOT_INIT;
    }

    if(sec_attr == PPC_SSE123_SECURE_ACCESS) {
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
        /* Uses secure unprivileged access address (SPCTRL) to set privilege
         * attribute
         */
        if(priv_attr == PPC_SSE123_PRIV_ONLY_ACCESS) {
            *(dev->data->spctrl_sp_ppc) &= ~(1U << periph);
        } else {
            *(dev->data->spctrl_sp_ppc) |= (1U << periph);
        }
#else
        /* Configuring security from Non-Secure domain is not permitted. */
        return PPC_SSE123_ERR_NOT_PERMITTED;
#endif
    } else {
        /* Uses non-secure unprivileged access address (NSPCTRL) to set
         * privilege attribute */
        if(priv_attr == PPC_SSE123_PRIV_ONLY_ACCESS) {
            *(dev->data->nspctrl_nsp_ppc) &= ~(1U << periph);
        } else {
            *(dev->data->nspctrl_nsp_ppc) |= (1U << periph);
        }
    }

    return PPC_SSE123_ERR_NONE;
}

bool ppc_sse123_is_periph_priv_only(struct ppc_sse123_dev_t* dev,
                                        uint8_t periph)
{
    if(dev->data->state != PPC_SSE123_INITIALIZED) {
        /* Return true as the default configuration is privilege only */
        return true;
    }

#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))
    /* In secure domain either secure or non-secure privilege access is returned
     * based on the configuration */
    if ((*(dev->data->spctrl_ns_ppc) & (1U << periph)) == 0) {
        /* Returns secure unprivileged access (SPCTRL) */
        return ((*(dev->data->spctrl_sp_ppc) & (1U << periph)) == 0);
    } else {
        /* Returns non-secure unprivileged access (NSPCTRL) */
        return ((*(dev->data->nspctrl_nsp_ppc) & (1U << periph)) == 0);
    }
#else
    /* Returns non-secure unprivileged access address (NSPCTRL) */
    return ((*(dev->data->nspctrl_nsp_ppc) & (1U << periph)) == 0);
#endif
}

/* Secure only functions */
#if (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U))

enum ppc_sse123_error_t
ppc_sse123_config_security(struct ppc_sse123_dev_t* dev, uint8_t periph,
                           enum ppc_sse123_sec_attr_t sec_attr)
{
    if(dev->data->state != PPC_SSE123_INITIALIZED) {
        return PPC_SSE123_ERR_NOT_INIT;
    }

    if(sec_attr == PPC_SSE123_SECURE_ACCESS) {
        *(dev->data->spctrl_ns_ppc) &= ~(1U << periph);
    } else {
        *(dev->data->spctrl_ns_ppc) |= (1U << periph);
    }

    return PPC_SSE123_ERR_NONE;
}

bool ppc_sse123_is_periph_secure(struct ppc_sse123_dev_t* dev,
                                 uint8_t periph)
{
    if(dev->data->state != PPC_SSE123_INITIALIZED) {
        /* Return true as the default configuration is secure */
        return true;
    }

    return ((*(dev->data->spctrl_ns_ppc) & (1U << periph)) == 0);
}

enum ppc_sse123_error_t ppc_sse123_irq_enable(struct ppc_sse123_dev_t* dev)
{
    struct sse123_spctrl_t* p_spctrl =
                         (struct sse123_spctrl_t*)dev->cfg->spctrl_base;

    if(dev->data->state != PPC_SSE123_INITIALIZED) {
        return PPC_SSE123_ERR_NOT_INIT;
    }

    p_spctrl->secppcinten |= dev->data->int_bit_mask;

    return PPC_SSE123_ERR_NONE;
}

void ppc_sse123_irq_disable(struct ppc_sse123_dev_t* dev)
{
    struct sse123_spctrl_t* p_spctrl =
                         (struct sse123_spctrl_t*)dev->cfg->spctrl_base;

    if(dev->data->state == PPC_SSE123_INITIALIZED) {
        p_spctrl->secppcinten &= ~(dev->data->int_bit_mask);
    }
}

void ppc_sse123_clear_irq(struct ppc_sse123_dev_t* dev)
{
    struct sse123_spctrl_t* p_spctrl =
                         (struct sse123_spctrl_t*)dev->cfg->spctrl_base;

    if(dev->data->state == PPC_SSE123_INITIALIZED) {
        p_spctrl->secppcintclr = dev->data->int_bit_mask;
    }
}

bool ppc_sse123_irq_state(struct ppc_sse123_dev_t* dev)
{
    struct sse123_spctrl_t* p_spctrl =
                         (struct sse123_spctrl_t*)dev->cfg->spctrl_base;

    if(dev->data->state != PPC_SSE123_INITIALIZED) {
        return false;
    }

    return ((p_spctrl->secppcintstat & dev->data->int_bit_mask) != 0);
}

#endif /* (defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)) */
