/*
 * Copyright (c) 2021-2022, Qorvo Inc
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
 */

/** @file "qvCHIP_factorydata.c"
 *
 *  factory-generated data retrieval
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
/* <CodeGenerator Placeholder> Includes */

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVCHIP

#include "qvCHIP.h"

#include "gpLog.h"

/* </CodeGenerator Placeholder> Includes */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> Macro */
#define FACTORY_DATA_MAGIC (0x41444651)

typedef PACKED_PRE struct
{
    uint32_t tag_id;
    uint32_t data_length;
} PACKED_POST tag_t;
/* </CodeGenerator Placeholder> Macro */

/*****************************************************************************
 *                    Static Data
 *****************************************************************************/
extern const uint32_t _binary_factory_data_bin_start;

/*****************************************************************************
 *                    Static Component Function Definitions
 *****************************************************************************/
static qvStatus_t qvCHIP_LocateTag(uint32_t tag_id, const uint32_t** data, uint32_t* data_length)
{
    const uint32_t* cursor = &_binary_factory_data_bin_start;
    if(!data_length || !data)
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }
    if(*cursor != FACTORY_DATA_MAGIC)
    {
        GP_LOG_PRINTF("exp %x act %lx", 0, FACTORY_DATA_MAGIC, *cursor);
        return QV_STATUS_INVALID_DATA;
    }
    cursor++;

    while(((tag_t*)cursor)->tag_id != TAG_ID_END_MARKER)
    {
        GP_LOG_PRINTF("tag %lu", 0, ((tag_t*)cursor)->tag_id);
        GP_LOG_PRINTF("length %lu", 0, ((tag_t*)cursor)->data_length);
        if(((tag_t*)cursor)->tag_id == tag_id)
        {
            (*data_length) = ((tag_t*)cursor)->data_length;
            (*data) = cursor + 2; // data area
            return QV_STATUS_NO_ERROR;
        }

        // If tag length is not a multiple of 4, we need to add one more word for the alignment
        if((((tag_t*)cursor)->data_length % 4) != 0)
        {
            cursor += 2 /* tag id and tag length */ + ((tag_t*)cursor)->data_length / 4 /* tag contents */ + 1;
        }
        else
        {
            cursor += 2 /* tag id and tag length */ + ((tag_t*)cursor)->data_length / 4 /* tag contents */;
        }
    }

    return QV_STATUS_INVALID_DATA;
}

/*****************************************************************************
 *                    Public Component Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

qvStatus_t qvCHIP_FactoryDataGetValue(qvCHIP_FactoryDataTagId_t tag, uint8_t* dst, uint32_t buffer_size, uint32_t* data_length)
{
    /* <CodeGenerator Placeholder> Implementation_qvCHIP_GetData */
    qvStatus_t status;
    const uint32_t* src;
    uint32_t src_size;

    if(dst == NULL)
    {
        return QV_STATUS_INVALID_ARGUMENT;
    }

    status = qvCHIP_LocateTag(tag, &src, &src_size);
    if(status != QV_STATUS_NO_ERROR)
    {
        return status;
    }
    if(data_length != NULL)
    {
        (*data_length) = src_size;
    }
    if(buffer_size < src_size)
    {
        return QV_STATUS_BUFFER_TOO_SMALL;
    }
    MEMCPY(dst, (uint8_t*)src, src_size);

    return QV_STATUS_NO_ERROR;
    /* </CodeGenerator Placeholder> Implementation_qvCHIP_GetData */
}
