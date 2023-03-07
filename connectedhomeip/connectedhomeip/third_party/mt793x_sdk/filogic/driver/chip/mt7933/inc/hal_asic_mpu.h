/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2020
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
#ifndef __HAL_ASIC_MPU_H__
#define __HAL_ASIC_MPU_H__

#include "mt7933.h"

/********************************
 * CONST
 *******************************/
#define MPU_NUM 4
#define DOMAIN_NUM 8
#define REGION_NUM 16

typedef enum {
    HAL_ASIC_MPU_STATUS_OK = 0,
    HAL_ASIC_MPU_STATUS_INVALID_PARAMETER,
    HAL_ASIC_MPU_STATUS_GENERIC_ERROR,
} hal_asic_mpu_status_t;

typedef enum {
    HAL_ASIC_MPU_TYPE_FLASH = 0,
    HAL_ASIC_MPU_TYPE_SYSRAM,
    HAL_ASIC_MPU_TYPE_PSRAM,
    HAL_ASIC_MPU_TYPE_TCM,
} hal_asic_mpu_type_t;

typedef enum {
    HAL_ASIC_MPU_APC_NO_PROTECTION = 0,            /* all access OK */
    HAL_ASIC_MPU_APC_SECURE_ACCESS_ONLY,           /* secure RW OK, non-secure all access denied */
    HAL_ASIC_MPU_APC_NORMAL_WRITE_FORBIDDEN,       /* secure RW OK, non-secure R-only */
    HAL_ASIC_MPU_APC_NORMAL_READ_FORBIDDEN,        /* secure RW OK, non-secure W-only */
    HAL_ASIC_MPU_APC_READ_ONLY,                    /* secure R-only, non-secure R-only */
    HAL_ASIC_MPU_APC_SECURE_READ_ONLY,             /* secure R-only, non-secure all access denied */
    HAL_ASIC_MPU_APC_SECURE_WRITE_FORBIDDEN,       /* secure R-only, non-secure RW OK */
    HAL_ASIC_MPU_APC_ACCESS_FORBIDDEN,             /* all access denied */
    HAL_ASIC_MPU_APC_MAX,
} hal_asic_mpu_apc_t;

/********************************
 * UTILS
 *******************************/
/**
 * @brief         This function sets the access permission for a certain region of a certain MPU.
 * @mpu_type[in]  The MPU to be set.
 * @region[in]    The region to be set.
 * @domain[in]    The domain to be set.
 * @apc[in]       The apc for the domain.
 * @return        #HAL_ASIC_MPU_STATUS_OK, if the operation is successful.\n
 *                #HAL_ASIC_MPU_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 */
hal_asic_mpu_status_t hal_asic_mpu_set_region_apc(hal_asic_mpu_type_t mpu_type, unsigned int region, int domain, unsigned int apc);

/**
 * @brief            This function sets the region boundary of a certain MPU.
 * @mpu_type[in]     The MPU to be set.
 * @region[in]       The region to be set.
 * @en[in]           If we enable the region after setting.
 * @start_addr[in]   The start_address of the region
 * @return           #HAL_ASIC_MPU_STATUS_OK, if the operation is successful.\n
 *                   #HAL_ASIC_MPU_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 */
hal_asic_mpu_status_t hal_asic_mpu_set_region_cfg(hal_asic_mpu_type_t mpu_type, unsigned int region, unsigned int en, unsigned int start_addr);

/**
 * @brief         This function sets the lock for configurations of a certain MPU.
 * @mpu_type[in]  The MPU to be set.
 * @return        #HAL_ASIC_MPU_STATUS_OK, if the operation is successful.\n
 *                #HAL_ASIC_MPU_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 */
hal_asic_mpu_status_t hal_asic_mpu_set_region_apc_lock(hal_asic_mpu_type_t mpu_type);

/**
 * @brief            This function gets the region boundary of a certain MPU.
 * @mpu_type[in]     The MPU to be set.
 * @region[in]       The region to be set.
 * @start_addr[out]  The start_address of the region
 * @return           #HAL_ASIC_MPU_STATUS_OK, if the operation is successful.\n
 *                   #HAL_ASIC_MPU_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 */
hal_asic_mpu_status_t hal_asic_mpu_get_region_cfg(hal_asic_mpu_type_t mpu_type, unsigned int region, unsigned int *start_addr);

/**
 * @brief         This function gets the access permission for a certain region of a certain MPU.
 * @mpu_type[in]  The MPU to be set.
 * @region[in]    The region to be set.
 * @domain[in]    The domain to be set.
 * @apc[out]      The apc for the domain.
 * @return        #HAL_ASIC_MPU_STATUS_OK, if the operation is successful.\n
 *                #HAL_ASIC_MPU_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 */
hal_asic_mpu_status_t hal_asic_mpu_get_region_apc(hal_asic_mpu_type_t mpu_type, unsigned int region, int domain, unsigned int *apc);

/**
 * @brief         The function is for asic_mpu violation handling.
 */
void asic_mpu_irq_handler(hal_nvic_irq_t irq_number);

/**
 * @brief         This function will initialize asic_mpu, and register asic_mpu irq handlers
 *                if HAL_NVIC_MODULE_ENABLED is enabled.
 * @return        #HAL_ASIC_MPU_STATUS_OK, if the operation is successful.\n
 *                #HAL_ASIC_MPU_STATUS_GENERIC_ERROR, if fails to register irq handler. \n
 */
hal_asic_mpu_status_t hal_asic_mpu_asic_mpu_init(void);

/**
 * @brief         This function will simple-test asic_mpu violation handler.
 */
void hal_asic_mpu_vio_ut(void);

#endif /* #ifndef __HAL_ASIC_MPU_H__ */
