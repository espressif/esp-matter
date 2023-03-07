/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
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

/*
* This file should contain all non-hardware dependent defines from the normal mm file.
* Most of the inf page sections that are only used in software should be described here.
*/



// the length of __vector_table
#define GPHAL_IVT_SHMEMPTR_OFFSET (0xcc)

/** @brief Packet Buffer Memory entries*/
#define GPHAL_MM_PBMS_START             GP_MM_RAM_PBM_0_DATA_START
#define GPHAL_MM_PBM_NR_OF             (GP_MM_RAM_PBM_OPTS_NR_OF - 1 - 1 - 1) /*ACK buffer always claimed + extra buffer for Ack Rx + channel change PBM */
#define GPHAL_MM_PBM_OFFSET             GP_MM_RAM_PBM_0_DATA_SIZE
#define GPHAL_MM_PBM_MAX_SIZE           GP_MM_RAM_PBM_0_DATA_SIZE

// Alternative locations where a BLE device address can be found
#define GP_MM_FLASH_BLE_ADDRESS_OFFSET_IN_USER_PAGE                ( 0 )

// Alternative locations where a ZGP sourceId information can be found
#define GP_MM_FLASH_ZGP_SOURCE_ID_OFFSET_IN_USER_PAGE               (0x8) //Overlapping with MAC address
#define GP_MM_FLASH_ZGP_SOURCE_ID_OFFSET_IN_USER_PAGE_MAC_OVERLAP   (0x8 + 0x5)

// Alternative locations where a IEEE MAC address can be found
#define GP_MM_FLASH_MAC_ADDRESS_OFFSET_IN_USER_PAGE                 (0x8) //Overlapping with ZGP source ID address

#if defined(GP_DIVERSITY_LINUXKERNEL)
#define GP_WB_WRITE_MM_MRI_NVM_ADDRESS_UNPROTECTED(offset) GP_WB_WRITE_U24(GP_MM_WISHB_ADDR_FROM_COMPRESSED(0xe4c), (offset))
#endif
