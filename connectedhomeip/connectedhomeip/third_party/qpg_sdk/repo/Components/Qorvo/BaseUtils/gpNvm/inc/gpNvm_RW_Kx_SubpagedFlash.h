
/*
 * Copyright (c) 2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 * gpNvm.h
 *
 * This file defines the Non Volatilie Memory component api
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPNVM_RW_KX_SUBPAGEDFLASH_H_
#define _GPNVM_RW_KX_SUBPAGEDFLASH_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_DATA_SECTION_SIZE_NVM
#error define GP_DATA_SECTION_SIZE_NVM when building
#endif //GP_DATA_SECTION_SIZE_NVM

#define NVM_NUMBER_PAGES_PER_SECTOR (2)
#ifndef GP_NVM_NBR_OF_REDUNDANT_SECTORS
#define GP_NVM_NBR_OF_REDUNDANT_SECTORS (1)
#endif //GP_NVM_NBR_OF_REDUNDANT_SECTORS


#if GP_NVM_NBR_OF_POOLS == 1
#if ! defined(GP_NVM_POOL_1_NBR_OF_PHY_SECTORS)
#define GP_NVM_POOL_1_NBR_OF_PHY_SECTORS ((GP_DATA_SECTION_SIZE_NVM / FLASH_PAGE_SIZE) / NVM_NUMBER_PAGES_PER_SECTOR)
#endif // ! defined(GP_NVM_POOL_1_NBR_OF_PHY_SECTORS)
#endif //GP_NVM_NBR_OF_POOLS == 1

#if GP_NVM_NBR_OF_POOLS > 1
#if ! defined(GP_NVM_POOL_2_NBR_OF_PHY_SECTORS)
#error You need to define GP_NVM_POOL_2_NBR_OF_PHY_SECTORS
#endif // ! defined(GP_NVM_POOL_2_NBR_OF_PHY_SECTORS)
#endif //GP_NVM_NBR_OF_POOLS > 1

#if GP_NVM_NBR_OF_POOLS > 2
#if ! defined(GP_NVM_POOL_3_NBR_OF_PHY_SECTORS)
#error You need to define GP_NVM_POOL_3_NBR_OF_PHY_SECTORS
#endif // !  defined(GP_NVM_POOL_3_NBR_OF_PHY_SECTORS)
#endif //GP_NVM_NBR_OF_POOLS > 2

#if GP_NVM_NBR_OF_POOLS > 3
#if ! defined(GP_NVM_POOL_4_NBR_OF_PHY_SECTORS)
#error You need to define GP_NVM_POOL_4_NBR_OF_PHY_SECTORS
#endif // ! defined(GP_NVM_POOL_4_NBR_OF_PHY_SECTORS)
#endif // GP_NVM_NBR_OF_POOLS > 3

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#ifdef GP_DIVERSITY_NVM
void Nvm_DumpInfo(void);
#endif


#ifdef __cplusplus
}
#endif

#endif //_GPNVM_RW_KX_SUBPAGEDFLASH_H_
