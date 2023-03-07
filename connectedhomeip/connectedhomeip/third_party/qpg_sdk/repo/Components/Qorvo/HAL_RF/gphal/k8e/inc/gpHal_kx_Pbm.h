/*
 * Copyright (c) 2016, GreenPeak Technologies
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

#ifndef _GPHAL_KX_PBM_H_
#define _GPHAL_KX_PBM_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

// The size and amount of the pbms that are reserved for special use (empty PDU, ACK PBM, channel change)
#define GP_HAL_RESERVED_PBM_SIZE        8

#ifndef GP_HAL_MAX_NR_OF_RESERVED_PBMS
#define GP_HAL_MAX_NR_OF_RESERVED_PBMS  3
#endif

#define GPHAL_PBM_15_4_MAX_SIZE         128
// The maximum size we allow for the BLE PBMs. Must be aligned to at least four bytes.
#define GPHAL_PBM_BLE_SIZE_MAX          268

// The number of bytes in the (BLE) PBM that cannot be used, because they are reserved for the HW/RT system
// We need to allocate three bytes for the CRC (this is only needed in the RX path)
#define GP_HAL_PBM_BLE_NR_RESERVED_BYTES    3

#ifndef GP_HAL_PBM_TYPE1_SIZE
#define GP_HAL_PBM_TYPE1_SIZE       GPHAL_PBM_15_4_MAX_SIZE
#endif //GP_HAL_PBM_TYPE1_SIZE

#ifndef GP_HAL_PBM_TYPE2_SIZE
#define GP_HAL_PBM_TYPE2_SIZE       GPHAL_PBM_BLE_SIZE_MAX
#endif //GP_HAL_PBM_TYPE2_SIZE

#ifndef GP_HAL_PBM_TYPE1_AMOUNT
#define GP_HAL_PBM_TYPE1_AMOUNT     8
#endif //GP_HAL_PBM_TYPE1_AMOUNT

#ifndef GP_HAL_PBM_TYPE2_AMOUNT
#define GP_HAL_PBM_TYPE2_AMOUNT     21
#endif //GP_HAL_PBM_TYPE1_AMOUNT

// The max size of a PBM currently maps onto the size of the type 2 pbms
#define GP_HAL_PBM_MAX_SIZE     (GP_HAL_PBM_TYPE2_SIZE)

// By default, half of the max size PBMs are used for RX
#ifndef GP_HAL_NR_OF_RX_PBMS
#define GP_HAL_NR_OF_RX_PBMS    (GP_HAL_PBM_TYPE2_AMOUNT >> 1)
#endif

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

//Ack pbm address
#define GPHAL_ACK_PBM_ADDR              GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(GP_WB_READ_RIB_ACK_PBM_PTR())

/*rap_pbe2optsbase(pbmEntry)*/
#define GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmEntry)    ((gpHal_Address_t)(GP_MM_RAM_PBM_OPTS_START + ((gpHal_Address_t)(pbmEntry))*GP_MM_RAM_PBM_OPTS_OFFSET))
/*rap_pbe2database(UInt8 pbe)*/
#define GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(pbmEntry)   ((gpHal_Address_t)(GP_MM_RAM_PBM_0_DATA_START + ((gpHal_Address_t)(pbmEntry))*GP_MM_RAM_PBM_0_DATA_SIZE))

/* Backwards compatibility - same implementation as GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(entry)*/
#define GP_HAL_PBM_ENTRY2ADDR(entry)        (GP_MM_RAM_PBM_0_DATA_START + (entry)*GP_MM_RAM_PBM_0_DATA_SIZE)
#define GP_HAL_CHECK_PBM_VALID(entry)       ((entry) < GP_MM_RAM_PBM_OPTS_NR_OF)
#define GP_HAL_IS_PBM_ALLOCATED(entry)      (GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIMED() & BM(entry))

#define GP_HAL_PBM_OFFSET_VALID(offset)     ((UInt16)(offset) < GP_HAL_PBM_MAX_SIZE)

#define GP_WB_READ_PBM_FORMAT_T_TX_RETRY_EXTENDED(offset) \
    (GP_WB_READ_U8((offset) + 0x001) & 0x07) +            \
        (GP_WB_READ_U1((offset) + 0x001, 7) << 3)

#define GP_WB_WRITE_PBM_FORMAT_T_TX_RETRY_EXTENDED(offset, val) \
    GP_WB_MWRITE_U8((offset) + 0x001, 0x87, ((val)&0x07) | (BIT_TST((val), 3) ? 0x80 : 0x00))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

#endif //_GPHAL_KX_PBM_H_
