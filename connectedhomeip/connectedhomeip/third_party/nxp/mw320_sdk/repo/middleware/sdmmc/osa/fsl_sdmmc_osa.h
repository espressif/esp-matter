/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDMMC_OSA_H_
#define _FSL_SDMMC_OSA_H_

#include "fsl_common.h"
#include "fsl_os_abstraction.h"

/*!
 * @addtogroup sdmmc_osa SDMMC OSA
 * @ingroup card
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*!@brief transfer event */
#define SDMMC_OSA_EVENT_TRANSFER_CMD_SUCCESS  (1UL << 0U)
#define SDMMC_OSA_EVENT_TRANSFER_CMD_FAIL     (1UL << 1U)
#define SDMMC_OSA_EVENT_TRANSFER_DATA_SUCCESS (1UL << 2U)
#define SDMMC_OSA_EVENT_TRANSFER_DATA_FAIL    (1UL << 3U)
#define SDMMC_OSA_EVENT_TRANSFER_DMA_COMPLETE (1UL << 4U)

/*!@brief card detect event, start from index 8 */
#define SDMMC_OSA_EVENT_CARD_INSERTED (1UL << 8U)
#define SDMMC_OSA_EVENT_CARD_REMOVED  (1UL << 9U)

/*!@brief enable semphore by default */
#ifndef SDMMC_OSA_POLLING_EVENT_BY_SEMPHORE
#define SDMMC_OSA_POLLING_EVENT_BY_SEMPHORE 1
#endif

/*!@brief sdmmc osa event */
typedef struct _sdmmc_osa_event
{
#if defined(SDMMC_OSA_POLLING_EVENT_BY_SEMPHORE) && SDMMC_OSA_POLLING_EVENT_BY_SEMPHORE
    volatile uint32_t eventFlag;
    OSA_SEMAPHORE_HANDLE_DEFINE(handle);
#else
    OSA_EVENT_HANDLE_DEFINE(handle);
#endif
} sdmmc_osa_event_t;
/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name sdmmc osa Function
 * @{
 */

/*!
 * @brief Initialize OSA.
 */
void SDMMC_OSAInit(void);

/*!
 * @brief OSA Create event.
 * @param eventHandle event handle.
 * @retval kStatus_Fail or kStatus_Success.
 */
status_t SDMMC_OSAEventCreate(void *eventHandle);

/*!
 * @brief Wait event.
 *
 * @param eventHandle The event type
 * @param eventType Timeout time in milliseconds.
 * @param timeoutMilliseconds timeout value in ms.
 * @param event event flags.
 * @retval kStatus_Fail or kStatus_Success.
 */
status_t SDMMC_OSAEventWait(void *eventHandle, uint32_t eventType, uint32_t timeoutMilliseconds, uint32_t *event);

/*!
 * @brief set event.
 * @param eventHandle event handle.
 * @param eventType The event type
 * @retval kStatus_Fail or kStatus_Success.
 */
status_t SDMMC_OSAEventSet(void *eventHandle, uint32_t eventType);

/*!
 * @brief Get event flag.
 * @param eventHandle event handle.
 * @param eventType event type.
 * @param flag pointer to store event value.
 * @retval kStatus_Fail or kStatus_Success.
 */
status_t SDMMC_OSAEventGet(void *eventHandle, uint32_t eventType, uint32_t *flag);

/*!
 * @brief clear event flag.
 * @param eventHandle event handle.
 * @param eventType The event type
 * @retval kStatus_Fail or kStatus_Success.
 */
status_t SDMMC_OSAEventClear(void *eventHandle, uint32_t eventType);

/*!
 * @brief Delete event.
 * @param eventHandle The event handle.
 */
status_t SDMMC_OSAEventDestroy(void *eventHandle);

/*!
 * @brief sdmmc delay.
 * @param milliseconds time to delay
 */
void SDMMC_OSADelay(uint32_t milliseconds);

/* @} */

#if defined(__cplusplus)
}
#endif
/* @} */
#endif /* _FSL_SDMMC_OSA_H_*/
