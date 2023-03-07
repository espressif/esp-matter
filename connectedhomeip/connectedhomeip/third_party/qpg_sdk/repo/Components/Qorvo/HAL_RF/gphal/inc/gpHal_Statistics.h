/*
 * Copyright (c) 2009-2014, 2016, GreenPeak Technologies
 * Copyright (c) 2017-2018, Qorvo Inc
 *
 * gpHal_Statistics.h
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
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#ifndef _GP_HAL_STATISTICS_H_
#define _GP_HAL_STATISTICS_H_

/** @file gpHal_Statistics.h
 *  @brief Getters and Setters of gpHal debug statistics.
 *
 *  Counters will increment untill cleared.
 *  Therefore a typical flow can be:
 *
 *  // Initialize
 *  gpHal_StatisticsCountersClear();
 *
 *  < do some Tx/Rx activity >
 *
 *  // Dump results
 *  gpHal_StatisticsCountersGet(&macCounter, NULL);
 *  gpHal_StatisticsCountersClear();
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @struct gpHal_StatisticsCntPrio_t
 *  @brief Structure grouping coexistence signaling, counting with which priority the signal was generated.
*/
typedef struct gpHal_StatisticsCntPrio_s {
    /** @brief Amount of priority 0 signals generated/received. */
    UInt16 prio0;
    /** @brief Amount of priority 1 signals generated/received. */
    UInt16 prio1;
    /** @brief Amount of priority 2 signals generated/received. */
    UInt16 prio2;
    /** @brief Amount of priority 3 signals generated/received. */
    UInt16 prio3;
} gpHal_StatisticsCntPrio_t;

/** @struct gpHal_StatisticsCoexCounter_t
 *  @brief Structure grouping all coexistence signaling done by the RF HAL layer.
 *
 *  Note these counters will not return any usefull information if Coex is not enabled.
*/
typedef struct gpHal_StatisticsCoexCounter_s {
    /** @brief Signaling information of amount of medium requests done with their different priorities. */
    gpHal_StatisticsCntPrio_t coexReq;
    /** @brief Signaling information of amount of grants received done with their different priorities. */
    gpHal_StatisticsCntPrio_t coexGrant;
} gpHal_StatisticsCoexCounter_t;

/** @struct gpHal_StatisticsMacCounter_t
 *  @brief Structure grouping all 802.15.4 MAC related counters tracked by the RF HAL layer.
*/
typedef struct gpHal_StatisticsMacCounter_s {
    /** @brief Count of all failing Channel Clear Assessments across all packets sent. */
    UInt16 ccaFails;
    /** @brief Count of all retries across all packets sent (in case of a Acked Tx) */
    UInt16 txRetries;
    /** @brief Count of all No Acked Data requests (after all retry attempts) */
    UInt16 failTxNoAck;
    /** @brief Count of all Data requests with failing Channel Access (after all CCA attempts) */
    UInt16 failTxChannelAccess;
    /** @brief Count of all Data requests with fully successfull TX. This can be after a few CCA or retries. */
    UInt16 successTx;
    /** @brief Count of all received packets (ACKs not counted). */
    UInt16 totalRx;
    /** @brief Count of HW PBM buffers depletion when issuing a Data Request. */
    UInt16 pbmOverflow;
} gpHal_StatisticsMacCounter_t;


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Clear all debug counters */
void gpHal_StatisticsCountersClear(void);

/** @brief Get debug counters on MAC and Coex (if supported)
 *
 * @param[out] pStatisticsMacCounters Pointer to statistics struct where counters will be returned. Can be NULL if not relevant.
 * @param[out] pStatisticsCoexCounters Pointer to statistics struct where Coex counters will be returned. Can be NULL if not relevant.
*/
void gpHal_StatisticsCountersGet(gpHal_StatisticsMacCounter_t* pStatisticsMacCounters, gpHal_StatisticsCoexCounter_t* pStatisticsCoexCounters);

#ifdef __cplusplus
}
#endif

#endif //_GP_HAL_STATISTICS_H_
