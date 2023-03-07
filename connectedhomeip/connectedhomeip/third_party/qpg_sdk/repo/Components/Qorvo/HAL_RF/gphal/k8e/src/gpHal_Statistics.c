/*
 * Copyright (c) 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpHal_Statistics.c
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

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal_Statistics.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                   Data Definitions
 *****************************************************************************/
gpHal_StatisticsMacCounter_t gpHal_StatisticsMacCounters;
gpHal_StatisticsCoexCounter_t gpHal_StatisticsCoexCounters;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_StatisticsCountersGet(gpHal_StatisticsMacCounter_t* pStatisticsMacCounters, gpHal_StatisticsCoexCounter_t* pStatisticsCoexCounters)
{
    if(pStatisticsMacCounters)
    {
        MEMCPY(pStatisticsMacCounters, &gpHal_StatisticsMacCounters, sizeof(gpHal_StatisticsMacCounter_t));
    }
}

void gpHal_StatisticsCountersClear(void)
{

    MEMSET(&gpHal_StatisticsMacCounters, 0x0, sizeof(gpHal_StatisticsMacCounter_t));
}
