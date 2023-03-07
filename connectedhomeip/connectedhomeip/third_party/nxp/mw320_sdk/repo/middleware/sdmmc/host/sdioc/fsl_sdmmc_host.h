/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDMMC_HOST_H
#define _FSL_SDMMC_HOST_H

#include "fsl_common.h"
#include "fsl_sdioc.h"
#include "fsl_sdmmc_osa.h"
/*!
 * @addtogroup sdmmchost_sdioc SDIOC HOST adapter Driver
 * @ingroup sdmmchost
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief Middleware adapter version. */
#define FSL_SDMMC_HOST_ADAPTER_VERSION (MAKE_VERSION(2U, 0U, 0U)) /*2.0.0*/

/*!@brief host capability */
#define SDMMCHOST_SUPPORT_HIGH_SPEED           (1U)
#define SDMMCHOST_SUPPORT_SUSPEND_RESUME       (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_3V3          (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_3V0          (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_1V8          (1U)
#define SDMMCHOST_SUPPORT_VOLTAGE_1V2          (0U)
#define SDMMCHOST_SUPPORT_4_BIT_WIDTH          (1U)
#define SDMMCHOST_SUPPORT_8_BIT_WIDTH          (0U)
#define SDMMCHOST_SUPPORT_DDR50                (0U)
#define SDMMCHOST_SUPPORT_SDR104               (0U)
#define SDMMCHOST_SUPPORT_SDR50                (0U)
#define SDMMCHOST_SUPPORT_HS200                (0U)
#define SDMMCHOST_SUPPORT_HS400                (0U)
#define SDMMCHOST_SUPPORT_DETECT_CARD_BY_DATA3 (0U)
#define SDMMCHOST_SUPPORT_DETECT_CARD_BY_CD    (0U)
#define SDMMCHOST_SUPPORT_AUTO_CMD12           (1U)
#define SDMMCHOST_SUPPORT_MAX_BLOCK_LENGTH     (4096U)
#define SDMMCHOST_SUPPORT_MAX_BLOCK_COUNT      (SDIOC_MAX_BLOCK_COUNT)
/*! @brief sdmmc host instance capability */
#define SDMMCHOST_INSTANCE_SUPPORT_8_BIT_WIDTH(host) 0U
#define SDMMCHOST_INSTANCE_SUPPORT_HS400(host)       0U
#define SDMMCHOST_INSTANCE_SUPPORT_1V8_SIGNAL(host)  0U
#define SDMMCHOST_INSTANCE_SUPPORT_HS200(host)       0U
#define SDMMCHOST_INSTANCE_SUPPORT_SDR104(host)      0U
#define SDMMCHOST_INSTANCE_SUPPORT_SDR50(host)       0U
#define SDMMCHOST_INSTANCE_SUPPORT_DDR50(host)       0U
/*!@brief SDMMC host dma descriptor buffer address align size */
#define SDMMCHOST_DMA_DESCRIPTOR_BUFFER_ALIGN_SIZE 1U
/*! @brief host Endian mode
 * corresponding to driver define
 */
enum _sdmmchost_endian_mode
{
    kSDMMCHOST_EndianModeBig         = 0U, /*!< Big endian mode */
    kSDMMCHOST_EndianModeHalfWordBig = 1U, /*!< Half word big endian mode */
    kSDMMCHOST_EndianModeLittle      = 2U, /*!< Little endian mode */
};

/*!
 * @brief The command response type.
 *
 * Defines the command response type from card to host controller.
 * @anchor _sdioc_card_response_type
 */
enum
{
    kCARD_ResponseTypeNone = 0U, /*!< Response type: none */
    kCARD_ResponseTypeR1   = 1U, /*!< Response type: R1 */
    kCARD_ResponseTypeR1b  = 2U, /*!< Response type: R1b */
    kCARD_ResponseTypeR2   = 3U, /*!< Response type: R2 */
    kCARD_ResponseTypeR3   = 4U, /*!< Response type: R3 */
    kCARD_ResponseTypeR4   = 5U, /*!< Response type: R4 */
    kCARD_ResponseTypeR5   = 6U, /*!< Response type: R5 */
    kCARD_ResponseTypeR5b  = 7U, /*!< Response type: R5b */
    kCARD_ResponseTypeR6   = 8U, /*!< Response type: R6 */
    kCARD_ResponseTypeR7   = 9U, /*!< Response type: R7 */
};

/*! @brief sdmmc host command structure definition */
typedef struct _sdmmchost_cmd
{
    uint32_t index;              /*!< Command index. */
    uint32_t argument;           /*!< Command argument. */
    uint8_t type;                /*!< command type */
    uint32_t responseType;       /*!< Command response type. */
    uint32_t responseErrorFlags; /*!< Response error flag, which need to check
                                      the command reponse. */
    uint32_t response[4U];       /*!< pointer registered by application to receive command response */
} sdmmchost_cmd_t;

/*! @brief sdmmc host data structure definition */
typedef struct _sdmmchost_data
{
    bool enableAutoCommand12; /*!< Enable auto CMD12. */
    uint32_t blockCount;      /*!< Block count. */
    uint32_t blockSize;       /*!< Block size. */
    uint32_t *txData;         /*!< Data buffer to write. */
    uint32_t *rxData;         /*!< Buffer to save data read. */
} sdmmchost_data_t;

/*! @brief sdmmc host data structure definition */
typedef struct _sdmmchost_transfer
{
    sdmmchost_data_t *data;
    sdmmchost_cmd_t *command;
} sdmmchost_transfer_t;

/*!@brief sdmmc host handler */
typedef struct _sdmmchost_
{
    SDIOC_Type *base;            /*!< host controller base address */
    uint32_t sourceClock_HZ;     /*!< host controller source clock */
    uint32_t endianMode;         /*!< host endian mode */
    sdioc_handle_t handle;       /*!< host controller handler */
    sdmmc_osa_event_t hostEvent; /*!< host event handler */
    void *cd;                    /*!< card detect */
    void *cardInt;               /*!< call back function for card interrupt */
} sdmmchost_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name SDIOC host controller function
 * @{
 */

/*!
 * @brief set data bus width.
 * @param host host handler
 * @param dataBusWidth data bus width
 */
void SDMMCHOST_SetCardBusWidth(sdmmchost_t *host, uint32_t dataBusWidth);

/*!
 * @brief Send initilization active 80 clocks to card.
 * @param host host handler
 */
static inline void SDMMCHOST_SendCardActive(sdmmchost_t *host)
{
    /* host not support */
}

/*!
 * @brief Set card bus clock.
 * @param host host handler
 * @param targetClock target clock frequency
 * @retval actual clock frequency can be reach.
 */
uint32_t SDMMCHOST_SetCardClock(sdmmchost_t *host, uint32_t targetClock);

/*!
 * @brief check card status by DATA0.
 * @param host host handler
 * @retval true is busy, false is idle.
 */
static inline bool SDMMCHOST_IsCardBusy(sdmmchost_t *host)
{
    return (SDIOC_GetPresentStatus(host->handle.base) & kSDIOC_PresentStatusData0Level) ==
                   kSDIOC_PresentStatusData0Level ?
               false :
               true;
}

/*!
 * @brief enable card interrupt.
 * @param host host handler
 * @param enable true is enable, false is disable.
 */
static inline void SDMMCHOST_EnableCardInt(sdmmchost_t *host, bool enable)
{
    SDIOC_EnableInterruptStatus(host->handle.base, kSDIOC_InterruptCardInterruptFlag, enable);
    SDIOC_EnableInterruptSignal(host->handle.base, kSDIOC_InterruptCardInterruptFlag, enable);
}

/*!
 * @brief card interrupt function.
 * @param host host handler
 * @param sdioInt card interrupt configuration
 */
status_t SDMMCHOST_CardIntInit(sdmmchost_t *host, void *sdioInt);

/*!
 * @brief card detect init function.
 * @param host host handler
 * @param cd card detect configuration
 */
status_t SDMMCHOST_CardDetectInit(sdmmchost_t *host, void *cd);

/*!
 * @brief Detect card insert, only need for SD cases.
 * @param host host handler
 * @param waitCardStatus status which user want to wait
 * @param timeout wait time out.
 * @retval kStatus_Success detect card insert
 * @retval kStatus_Fail card insert event fail
 */
status_t SDMMCHOST_PollingCardDetectStatus(sdmmchost_t *host, uint32_t waitCardStatus, uint32_t timeout);

/*!
 * @brief card detect status.
 * @param host host handler
 * @retval kSD_Inserted, kSD_Removed
 */
uint32_t SDMMCHOST_CardDetectStatus(sdmmchost_t *host);

/*!
 * @brief Init host controller.
 * @param host host handler
 * @retval kStatus_Success host init success
 * @retval kStatus_Fail event fail
 */
status_t SDMMCHOST_Init(sdmmchost_t *host);

/*!
 * @brief Deinit host controller.
 * @param host host handler
 */
void SDMMCHOST_Deinit(sdmmchost_t *host);

/*!
 * @brief host power off card function.
 * @param host host handler
 * @param enable true is power on, false is power down.
 */
void SDMMCHOST_SetCardPower(sdmmchost_t *host, bool enable);

/*!
 * @brief host transfer function.
 * @param host host handler
 * @param content transfer content.
 */
status_t SDMMCHOST_TransferFunction(sdmmchost_t *host, sdmmchost_transfer_t *content);

/*!
 * @brief host reset function.
 *
 * @param host host handler
 */
void SDMMCHOST_Reset(sdmmchost_t *host);

/* @} */

#if defined(__cplusplus)
}
#endif
/* @} */
#endif /* _FSL_SDMMC_HOST_H */
