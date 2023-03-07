/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2021
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#ifndef __HAL_DEVAPC_H__
#define __HAL_DEVAPC_H__

#include "mt7933.h"

/********************************
 * CONST
 *******************************/
typedef enum {
    HAL_DEVAPC_STATUS_OK = 0,
    HAL_DEVAPC_STATUS_INVALID_PARAMETER,
    HAL_DEVAPC_STATUS_GENERIC_ERROR,
} hal_devapc_status_t;

typedef enum {
    HAL_DEVAPC_TYPE_INFRA = 0,
    HAL_DEVAPC_TYPE_AUD,
    HAL_DEVAPC_TYPE_MAX,
} hal_devapc_type_t;

typedef enum {
    HAL_DEVAPC_DOMAIN_0 = 0,
    HAL_DEVAPC_DOMAIN_1,
    HAL_DEVAPC_DOMAIN_2,
    HAL_DEVAPC_DOMAIN_3,
    HAL_DEVAPC_DOMAIN_4,
    HAL_DEVAPC_DOMAIN_5,
    HAL_DEVAPC_DOMAIN_6,
    HAL_DEVAPC_DOMAIN_7,
    HAL_DEVAPC_DOMAIN_MAX,
} hal_devapc_dom_t;

typedef enum {
    HAL_DEVAPC_APC_NO_PROTECTION = 0,            /* all access OK */
    HAL_DEVAPC_APC_SEC_RW_ONLY,                  /* secure RW OK, non-secure all access denied */
    HAL_DEVAPC_APC_SEC_RW_NS_R,                  /* secure RW OK, non-secure R-only */
    HAL_DEVAPC_APC_FORBIDDEN,                    /* all access denied */
    HAL_DEVAPC_APC_MAX,
} hal_devapc_apc_t;

// Use in TFM SPM to assign default value for all index
#define INFRA_DAPC_MODULE_MAX   70
#define AUD_DAPC_MODULE_MAX   34

/********************************
 * UTILS
 *******************************/
/**
 * @brief         This function sets the master domain.
 * @type[in]      The dapc type for the master to be set.
 * @master[in]    The master to be set.
 * @dom[in]       The domain to be set.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_INVALID_PARAMETER, if any of the parameter is invalid. \n
 */
hal_devapc_status_t hal_devapc_set_master_domain(hal_devapc_type_t type, unsigned int master, hal_devapc_dom_t dom);

/**
 * @brief         This function sets the module apc for a specific domain.
 * @type[in]      The dapc type for the master to be set.
 * @module[in]    The module to be set.
 * @dom[in]       The domain to be set.
 * @perm[in]      The access permission to be set.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_INVALID_PARAMETER, if any of the parameter is invalid. \n
 */
hal_devapc_status_t hal_devapc_set_module_apc(hal_devapc_type_t type, unsigned int module, hal_devapc_dom_t dom, hal_devapc_apc_t perm);

/**
 * @brief         This function sets lock for a specific module.
 * @type[in]      The dapc type for the master to be set.
 * @module[in]    The module to be set.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_INVALID_PARAMETER, if any of the parameter is invalid. \n
 */
hal_devapc_status_t hal_devapc_set_apc_lock(hal_devapc_type_t dapc_type, unsigned int module);

/**
 * @brief         This function gets the master domain.
 * @type[in]      The dapc type for the master to be set.
 * @master[in]    The master to be set.
 * @dom[out]      The domain to be get.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_INVALID_PARAMETER, if any of the parameter is invalid. \n
 */
hal_devapc_status_t hal_devapc_get_master_domain(hal_devapc_type_t type, unsigned int master, hal_devapc_dom_t *dom);

/**
 * @brief         This function gets the module apc for a specific domain.
 * @type[in]      The dapc type for the master to be set.
 * @module[in]    The module to be set.
 * @dom[in]       The domain to be set.
 * @perm[out]     The access permission to be get.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_INVALID_PARAMETER, if any of the parameter is invalid. \n
 */
hal_devapc_status_t hal_devapc_get_module_apc(hal_devapc_type_t dapc_type, unsigned int module, hal_devapc_dom_t dom, hal_devapc_apc_t *perm);

/**
 * @brief         This function will initialize infra bus devapc, and register devapc irq handlers
 *                if HAL_NVIC_MODULE_ENABLED is enabled.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_GENERIC_ERROR, if fails to register irq handler. \n
 */
hal_devapc_status_t hal_devapc_infra_devapc_init(void);

/**
 * @brief         This function will initialize audio bus devapc, and register devapc irq handlers
 *                if HAL_NVIC_MODULE_ENABLED is enabled.
 * @return        #HAL_DEVAPC_STATUS_OK, if the operation is successful.\n
 *                #HAL_DEVAPC_STATUS_GENERIC_ERROR, if fails to register irq handler. \n
 */
hal_devapc_status_t hal_devapc_aud_devapc_init(void);

/**
 * @brief         This function triggers violation for UT.
 */
void hal_devapc_trigger_ut_vio(void);

/**
 * @brief         Handler violation IRQ.
 */
void devapc_irq_handler(hal_nvic_irq_t irq_number);

#endif /* #ifndef __HAL_DEVAPC_H__ */
