/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_SDIOC_H_
#define _FSL_SDIOC_H_

#include "fsl_common.h"

/*!
 * @addtogroup  sdioc
 *  @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/
/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.1. */
#define FSL_SDIOC_DRIVER_VERSION (MAKE_VERSION(2U, 0U, 1U))

/*! @brief SDIOC transfer status. */
enum
{
    kStatus_SDIOC_BusyTransferring       = MAKE_STATUS(kStatusGroup_SDIOC, 0U), /*!< Transfer is on-going. */
    kStatus_SDIOC_TransferCommandSuccess = MAKE_STATUS(kStatusGroup_SDIOC, 1U), /*!< Transfer command success. */
    kStatus_SDIOC_TransferCommandFail    = MAKE_STATUS(kStatusGroup_SDIOC, 2U), /*!< Transfer command fail. */
    kStatus_SDIOC_TransferCommandResponseError =
        MAKE_STATUS(kStatusGroup_SDIOC, 3U),                                 /*!< Transfer command response error . */
    kStatus_SDIOC_TransferDataFail    = MAKE_STATUS(kStatusGroup_SDIOC, 4U), /*!< Transfer data fail. */
    kStatus_SDIOC_TransferDataSuccess = MAKE_STATUS(kStatusGroup_SDIOC, 5U), /*!< Transfer data success. */
};

/*! @brief SDIOC bus voltage level type definition */
typedef enum _sdioc_bus_voltage
{
    kSDIOC_BusVoltage_3V3 = 0x07, /*!< SDIO bus voltage select 3.3V */
    kSDIOC_BusVoltage_3V0 = 0x06, /*!< SDIO bus voltage select 3.0V */
    kSDIOC_BusVoltage_1V8 = 0x05, /*!< SDIO bus voltage select 1.8V */

    /* 0 -4 are reserved */

} sdioc_bus_voltage_t;

/*! @brief SDIOC speed mode type definition */
typedef enum _sdioc_bus_speed
{
    kSDIOC_BusSpeedNormal = 0x00, /*!< SDIO normal speed mode */
    kSDIOC_BusSpeedHigh,          /*!< SDIO high speed mode */
} sdioc_bus_speed_t;

/*! @brief SDIOC data width type definition */
typedef enum _sdioc_bus_width
{
    kSDIOC_BusWidth1Bit = 0x00, /*!< SDIO data width 1 bit mode */
    kSDIOC_BusWidth4Bit,        /*!< SDIO data width 4 bits mode */
} sdioc_bus_width_t;

/*! @brief SDIOC command type definition
 * @anchor _sdioc_command_type
 */
enum
{
    kSDIOC_CommandTypeNormal = 0x00, /*!< Normal command */
    kSDIOC_CommandTypeSuspend,       /*!< Suspend command */
    kSDIOC_CommandTypeResume,        /*!< Resume command */
    kSDIOC_CommandTypeAbort,         /*!< Abort command */
};

/*! @brief SDIOC response type definition
 * @anchor _sdioc_command_response_type
 */
enum
{
    kSDIOC_CommandNoResponse = SDIOC_MM4_CMD_XFRMD_RES_TYPE(0x00), /*!< No response */
    kSDIOC_CommandResponseLen136BitsCheckCrc =
        SDIOC_MM4_CMD_XFRMD_RES_TYPE(1U) | SDIOC_MM4_CMD_XFRMD_CRCCHKEN_MASK,        /*!< Response length 136 bits */
    kSDIOC_CommandResponseLen48Bits              = SDIOC_MM4_CMD_XFRMD_RES_TYPE(2U), /*!< Response length 48 bits*/
    kSDIOC_CommandResponseLen48BitsCheckCrcIndex = SDIOC_MM4_CMD_XFRMD_RES_TYPE(2U) |
                                                   SDIOC_MM4_CMD_XFRMD_CRCCHKEN_MASK |
                                                   SDIOC_MM4_CMD_XFRMD_IDXCHKEN_MASK, /*!< Response length 48 bits*/
    kSDIOC_CommandResponseLen48BitsWithCheckBusyCrcIndex =
        SDIOC_MM4_CMD_XFRMD_RES_TYPE(3U) | SDIOC_MM4_CMD_XFRMD_CRCCHKEN_MASK |
        SDIOC_MM4_CMD_XFRMD_IDXCHKEN_MASK, /*!< Response length 48 bits, check busy after response */
};

/*! @brief SDIOC response number definition */
typedef enum _sdioc_response_index
{
    kSDIOC_ResponseIndex0 = 0x00, /*!< Command response 0 */
    kSDIOC_ResponseIndex1,        /*!< Command response 1 */
    kSDIOC_ResponseIndex2,        /*!< Command response 2 */
    kSDIOC_ResponseIndex3,        /*!< Command response 3 */
} sdioc_response_index_t;

/*! @brief SDIOC dma buffer boundary */
typedef enum _sdioc_dma_buffer_boundary
{
    kSDIOC_DMABufferBoundary4K   = 0U, /*!< DMA buffer boundary 4K*/
    kSDIOC_DMABufferBoundary8K   = 1U, /*!< DMA buffer boundary 8K */
    kSDIOC_DMABufferBoundary16K  = 2U, /*!< DMA buffer boundary 16K */
    kSDIOC_DMABufferBoundary32K  = 3U, /*!< DMA buffer boundary 32K */
    kSDIOC_DMABufferBoundary64K  = 4U, /*!< DMA buffer boundary 64K */
    kSDIOC_DMABufferBoundary128K = 5U, /*!< DMA buffer boundary 128K */
    kSDIOC_DMABufferBoundary256K = 6U, /*!< DMA buffer boundary 256K */
    kSDIOC_DMABufferBoundary512K = 7U, /*!< DMA buffer boundary 512K */
} sdioc_dma_buffer_boundary_t;

/*! @brief sdioc data timeout value definition */
typedef enum _sdioc_data_timeout_value
{
    kSDIOC_DataTimeoutValue0  = 0U,  /*!< timeout out value is (timeout clock)*2^13 */
    kSDIOC_DataTimeoutValue1  = 1U,  /*!< timeout out value is (timeout clock)*2^14 */
    kSDIOC_DataTimeoutValue2  = 2U,  /*!< timeout out value is (timeout clock)*2^15 */
    kSDIOC_DataTimeoutValue3  = 3U,  /*!< timeout out value is (timeout clock)*2^16 */
    kSDIOC_DataTimeoutValue4  = 4U,  /*!< timeout out value is (timeout clock)*2^17 */
    kSDIOC_DataTimeoutValue5  = 5U,  /*!< timeout out value is (timeout clock)*2^18 */
    kSDIOC_DataTimeoutValue6  = 6U,  /*!< timeout out value is (timeout clock)*2^19 */
    kSDIOC_DataTimeoutValue7  = 7U,  /*!< timeout out value is (timeout clock)*2^20 */
    kSDIOC_DataTimeoutValue8  = 8U,  /*!< timeout out value is (timeout clock)*2^21 */
    kSDIOC_DataTimeoutValue9  = 9U,  /*!< timeout out value is (timeout clock)*2^22 */
    kSDIOC_DataTimeoutValue10 = 10U, /*!< timeout out value is (timeout clock)*2^23 */
    kSDIOC_DataTimeoutValue11 = 11U, /*!< timeout out value is (timeout clock)*2^24 */
    kSDIOC_DataTimeoutValue12 = 12U, /*!< timeout out value is (timeout clock)*2^25 */
    kSDIOC_DataTimeoutValue13 = 13U, /*!< timeout out value is (timeout clock)*2^26 */
    kSDIOC_DataTimeoutValue14 = 14U, /*!< timeout out value is (timeout clock)*2^27 */
} sdioc_data_timeout_value_t;

/*! @brief SDIOC interrupt flag definition
 * @anchor _sdioc_interrupt_flag
 */
enum
{
    kSDIOC_InterruptCommandCompleteFlag  = SDIOC_MM4_I_STAT_CMDCOMP_MASK,  /*!< Command complete interrupt flag */
    kSDIOC_InterruptXferCompleteFlag     = SDIOC_MM4_I_STAT_XFRCOMP_MASK,  /*!< Transfer complete interrupt flag */
    kSDIOC_InterruptBlockGapEventFlag    = SDIOC_MM4_I_STAT_BGEVNT_MASK,   /*!< Block gap event flag */
    kSDIOC_InterruptDMAInterruptFlag     = SDIOC_MM4_I_STAT_DMAINT_MASK,   /*!< DMA interrupt flag */
    kSDIOC_InterruptBufferWriteReadyFlag = SDIOC_MM4_I_STAT_BUFWRRDY_MASK, /*!< Buffer write ready interrupt flag*/
    kSDIOC_InterruptBufferReadReadyFlag  = SDIOC_MM4_I_STAT_BUFRDRDY_MASK, /*!< Buffer read ready interrupt flag */
    kSDIOC_InterruptCardInsertedFlag     = SDIOC_MM4_I_STAT_CDINS_MASK,    /*!< Card insertion interrupt flag */
    kSDIOC_InterruptCardRemovedFlag      = SDIOC_MM4_I_STAT_CDREM_MASK,    /*!< Card remove interrupt flag */
    kSDIOC_InterruptCardInterruptFlag =
        SDIOC_MM4_I_STAT_CDINT_MASK, /*!< Card interrupt flag, interrupt detected by DAT[1] */
    kSDIOC_InterruptERRInterrupt           = SDIOC_MM4_I_STAT_ERRINT_MASK,  /*!< Error interrupt flag */
    kSDIOC_InterruptCommandTimeoutFlag     = SDIOC_MM4_I_STAT_CTOERR_MASK,  /*!< Command timeout error interrupt flag */
    kSDIOC_InterruptCommandCrcErrorFlag    = SDIOC_MM4_I_STAT_CCRCERR_MASK, /*!< Command crc error interrupt flag */
    kSDIOC_InterruptCommandEntBitErrorFlag = SDIOC_MM4_I_STAT_CENDERR_MASK, /*!< Command end bit error interrupt flag */
    kSDIOC_InterruptCommandIndexErrorFlag  = SDIOC_MM4_I_STAT_CIDXERR_MASK, /*!< Command index error interrupt flag */
    kSDIOC_InterruptDataTimeoutErrorFlag   = SDIOC_MM4_I_STAT_DTOERR_MASK,  /*!< Data timeout error interrupt flag */
    kSDIOC_InterruptDataCrcErrorFlag       = SDIOC_MM4_I_STAT_DCRCERR_MASK, /*!< Data crc error interrupt flag */
    kSDIOC_InterruptDataEndBitErrorFlag    = SDIOC_MM4_I_STAT_DENDERR_MASK, /*!< Data end bit error interrupt flag */
    kSDIOC_InterruptCurrentLimitErrorFlag  = SDIOC_MM4_I_STAT_ILMTERR_MASK, /*!< Current limit error interrupt flag */
    kSDIOC_InterruptAutoCommand12ErrorFlag = SDIOC_MM4_I_STAT_AC12ERR_MASK, /*!< Current limit error interrupt flag */
    kSDIOC_InterruptAhbTargetErrorFlag     = SDIOC_MM4_I_STAT_AHBTERR_MASK, /*!< AHB Target error interrupt flag */

    kSDIOC_InterruptCardDetectFlag =
        kSDIOC_InterruptCardInsertedFlag | kSDIOC_InterruptCardRemovedFlag, /*!< card insert flag summary */
    kSDIOC_InterruptCommandErrorFlag = kSDIOC_InterruptCommandTimeoutFlag | kSDIOC_InterruptCommandCrcErrorFlag |
                                       kSDIOC_InterruptCommandEntBitErrorFlag |
                                       kSDIOC_InterruptCommandIndexErrorFlag, /*!< command error flag summary */
    kSDIOC_InterruptCommandFlag =
        kSDIOC_InterruptCommandCompleteFlag | kSDIOC_InterruptCommandErrorFlag, /*!< command flag summary */

    kSDIOC_InterruptDataErrorFlag = kSDIOC_InterruptDataTimeoutErrorFlag | kSDIOC_InterruptDataCrcErrorFlag |
                                    kSDIOC_InterruptDataEndBitErrorFlag | kSDIOC_InterruptAutoCommand12ErrorFlag |
                                    kSDIOC_InterruptDMAInterruptFlag |
                                    kSDIOC_InterruptAhbTargetErrorFlag, /*!< data error flag */

    kSDIOC_InterruptDataFlag =
        kSDIOC_InterruptXferCompleteFlag | kSDIOC_InterruptDataErrorFlag, /*!< data flag summary */

    kSDIOC_InterruptCommandAndDataFlag =
        kSDIOC_InterruptCommandFlag | kSDIOC_InterruptDataFlag, /*!< command and data flag summary */
};

/*! @brief SDIOC present status definition */
enum
{
    kSDIOC_PresentStatusCommandInhibit      = SDIOC_MM4_STATE_CCMDINHBT_MASK,        /*!< Command inhibit(CMD) status */
    kSDIOC_PresentStatusDataInhibit         = SDIOC_MM4_STATE_DCMDINHBT_MASK,        /*!< Command inhibit(DAT) status */
    kSDIOC_PresentStatusDataLineActive      = SDIOC_MM4_STATE_DATACTV_MASK,          /*!< DAT line active  */
    kSDIOC_PresentStatusWriteTransferActive = SDIOC_MM4_STATE_WRACTV_MASK,           /*!< Write transfer active */
    kSDIOC_PresentStatusReadTransferActive  = SDIOC_MM4_STATE_RDACTV_MASK,           /*!< Read transfer active */
    kSDIOC_PresentStatusBufferWriteReady    = SDIOC_MM4_STATE_BUFWREN_MASK,          /*!< Buffer write enable */
    kSDIOC_PresentStatusBufferReadReady     = SDIOC_MM4_STATE_BUFRDEN_MASK,          /*!< Buffer read enable */
    kSDIOC_PresentStatusCardInserted        = SDIOC_MM4_STATE_CDINSTD_MASK,          /*!< Card inserted */
    kSDIOC_PresentStatusData0Level          = 1U << SDIOC_MM4_STATE_LWRDATLVL_SHIFT, /*!< Data[0] line signal level */
    kSDIOC_PresentStatusData1Level   = 1U << (SDIOC_MM4_STATE_LWRDATLVL_SHIFT + 1U), /*!< Data[1] line signal level */
    kSDIOC_PresentStatusData2Level   = 1U << (SDIOC_MM4_STATE_LWRDATLVL_SHIFT + 2U), /*!< Data[2] line signal level */
    kSDIOC_PresentStatusData3Level   = 1U << (SDIOC_MM4_STATE_LWRDATLVL_SHIFT + 3U), /*!< Data[3] line signal level */
    kSDIOC_PresentStatusCommandLevel = SDIOC_MM4_STATE_CMDLVL_MASK,                  /*!< CMD line signal level */
    kSDIOC_PresentStatusData4Level   = 1U << SDIOC_MM4_STATE_UPRDATLVL_SHIFT,        /*!< DAT[4] line signal level */
    kSDIOC_PresentStatusData5Level   = 1U << (SDIOC_MM4_STATE_UPRDATLVL_SHIFT + 1U), /*!< DAT[5] line signal level */
    kSDIOC_PresentStatusData6Level   = 1U << (SDIOC_MM4_STATE_UPRDATLVL_SHIFT + 2U), /*!< DAT[6] line signal level */
    kSDIOC_PresentStatusData7Level   = 1U << (SDIOC_MM4_STATE_UPRDATLVL_SHIFT + 3U), /*!< DAT[7] line signal level */
};

/*! @brief SDIOC Configuration Structure definition */
typedef struct _sdioc_config
{
    sdioc_bus_voltage_t busVoltage;         /*!< bus voltage */
    sdioc_data_timeout_value_t dataTimeout; /*!< data timeout value */
} sdioc_config_t;

/*! @brief SDIOC command configuration structure definition */
typedef struct _sdioc_command
{
    uint32_t commandIndex;              /*!< Command index*/
    uint32_t commandArgument;           /*!< command argument which specified as bit 39-8 of command-format */
    uint8_t commandType;                /*!< Command type, @ref _sdioc_command_type */
    uint32_t commandResponseType;       /*!< Command response type, @ref _sdioc_command_response_type */
    uint32_t commandResponseErrorFlags; /*!< Command response error flags */
    uint32_t commandResponse[4];        /*!< Command response */
} sdioc_command_t;

/*! @brief SDIOC data configuration structure definition*/
typedef struct _sdioc_data
{
    bool dataEnableAutoCommand12; /*!< enable auto command 12 after data transfer complete */
    uint32_t dataBlockCount;      /*!< Block count for current transfer.
                                - 0x0000: stop count
                                - 0x0001: 1 block
                                - 0x0002: 2 blocks
                                ..................
                                ..................
                                - 0xFFFF: 65535 blocks */

    uint32_t dataBlockSize; /*!< Transfer block size.
                            - 0x000: no data transfer
                            - 0x001: 1 byte
                            - 0x002: 2 bytes
                            - 0x003: 3 bytes
                            - 0x004: 4 bytes
                            ..................
                            - 0x1FF: 511 bytes
                            - 0x200: 512 bytes
                            ..................
                            - 0x800: 2048 bytes
                            - 0x801 - 0xFFF: reserved */
    uint8_t *dataTxAddr;    /*!< tx data buffer address */
    uint8_t *dataRxAddr;    /*!< rx data buffer address */

} sdioc_data_t;

/*! @brief SDIOC dma configuration structure definition */
typedef struct _sdioc_dma_config
{
    sdioc_dma_buffer_boundary_t dmaBufferBoundary; /*!< dma buffer boundary */
} sdioc_dma_config_t;

/*! @brief SDIOC handle typedef. */
typedef struct _sdioc_handle sdioc_handle_t;

/*! @brief SDIOC callback pointer*/
typedef void (*sdioc_transfer_callback_t)(sdioc_handle_t *handle, status_t transferStatus, void *userData);
typedef void (*sdioc_card_int_callback_t)(sdioc_handle_t *handle, void *userData);
typedef void (*sdioc_card_detect_callback_t)(sdioc_handle_t *handle, bool isCardInserted, void *userData);

/*!< @brief SDIOC callback structure definition */
typedef struct _sdioc_callback
{
    sdioc_transfer_callback_t transferCallback;      /*!< transfer callback pointer */
    sdioc_card_int_callback_t cardIntCallback;       /*!< card interrupt callback pointer */
    sdioc_card_detect_callback_t cardDetectCallback; /*!< card detect callback pointer */
} sdioc_callback_t;

/*!
 * @brief SDIOC handle.
 *
 * Defines the structure to save the SDIOC state information and callback function.
 *
 */
struct _sdioc_handle
{
    SDIOC_Type *base;          /*!< host controller base address */
    sdioc_data_t *data;        /*!< transfer data pointer */
    sdioc_command_t *command;  /*!< transfer command pointer */
    sdioc_callback_t callback; /*!< Callback function. */
    void *userData;            /*!< Parameter for transfer complete callback. */
};
/*************************************************************************************************
 * API
 ************************************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name SDIOC Initialization and deinitialization
 * @{
 */
/*!
 * @brief SDIOC module initialization function.
 *
 * Configures the SDIOC according to the user configuration.
 *
 * Example:
   @code
   sdioc_config_t config;
   config.busVoltage = kSDIOC_BusVoltage_3V3;
   config.dataTimeout = kSDIOC_DataTimeoutValue14;
   SDIOC_Init(SDIOC, &config);
   @endcode
 *
 * @param base SDIOC peripheral base address.
 * @param config SDIOC configuration information.
 * @retval #kStatus_Success Operate successfully.
 */
void SDIOC_Init(SDIOC_Type *base, const sdioc_config_t *config);

/*!
 * @brief Deinitializes the SDIOC.
 *
 * @param base SDIOC peripheral base address.
 */
void SDIOC_Deinit(SDIOC_Type *base);
/* @} */

/*!
 * @name SDIOC bus functional interface
 * @{
 */
/*!
 * @brief SDIOC set bus voltage.
 *
 * @param base SDIOC peripheral base address.
 * @param busVoltage SDIO bus voltage.
 */
static inline void SDIOC_SetBusVoltage(SDIOC_Type *base, sdioc_bus_voltage_t busVoltage)
{
    base->MM4_CNTL1 = (base->MM4_CNTL1 & ~SDIOC_MM4_CNTL1_VLTGSEL_MASK) | SDIOC_MM4_CNTL1_VLTGSEL(busVoltage);
}

/*!
 * @brief SDIOC set data bus width.
 *
 * @param base SDIOC peripheral base address.
 * @param busWidth SDIO bus width.
 */
static inline void SDIOC_SetDataBusWidth(SDIOC_Type *base, sdioc_bus_width_t busWidth)
{
    base->MM4_CNTL1 = (base->MM4_CNTL1 & ~SDIOC_MM4_CNTL1__4BITMD_MASK) | SDIOC_MM4_CNTL1__4BITMD(busWidth);
}

/*!
 * @brief SDIOC set bus speed.
 *
 * @param base SDIOC peripheral base address.
 * @param busSpeed SDIO bus speed.
 */
static inline void SDIOC_SetBusSpeed(SDIOC_Type *base, sdioc_bus_speed_t busSpeed)
{
    base->MM4_CNTL1 = (base->MM4_CNTL1 & ~SDIOC_MM4_CNTL1_HISPEED_MASK) | SDIOC_MM4_CNTL1_HISPEED(busSpeed);
}

/*!
 * @brief SDIOC enable/disable bus power.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is enable power, false is disable power.
 */
static inline void SDIOC_EnableBusPower(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL1 |= SDIOC_MM4_CNTL1_BUSPWR_MASK;
    }
    else
    {
        base->MM4_CNTL1 &= ~SDIOC_MM4_CNTL1_BUSPWR_MASK;
    }
}

/*!
 * @brief SDIOC enable/disable internal clock.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is enable internal clock, false is disable internal clock.
 */
static inline void SDIOC_EnableInternalClock(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL2 |= SDIOC_MM4_CNTL2_INTCLKEN_MASK;
    }
    else
    {
        base->MM4_CNTL2 &= ~SDIOC_MM4_CNTL2_INTCLKEN_MASK;
    }
}

/*!
 * @brief SDIOC enable/disable card clock.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is enable card clock, false is disable card clock.
 */
static inline void SDIOC_EnableCardClock(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL2 |= SDIOC_MM4_CNTL2_MM4CLKEN_MASK;
    }
    else
    {
        base->MM4_CNTL2 &= ~SDIOC_MM4_CNTL2_MM4CLKEN_MASK;
    }
}

/*!
 * @brief SDIOC software reset entire host controller except for the card detection circuit register.
 *
 * @param base SDIOC peripheral base address.
 */
static inline void SDIOC_ResetAll(SDIOC_Type *base)
{
    base->MM4_CNTL2 |= SDIOC_MM4_CNTL2_MSWRST_MASK;
}

/*!
 * @brief SDIOC software reset command line.
 *
 * @param base SDIOC peripheral base address.
 */
static inline void SDIOC_ResetCommandLine(SDIOC_Type *base)
{
    base->MM4_CNTL2 |= SDIOC_MM4_CNTL2_CMDSWRST_MASK;
}

/*!
 * @brief SDIOC software reset data line.
 *
 * @param base SDIOC peripheral base address.
 */
static inline void SDIOC_ResetDataLine(SDIOC_Type *base)
{
    base->MM4_CNTL2 |= SDIOC_MM4_CNTL2_DATSWRST_MASK;
}

/*!
 * @brief SDIOC set data timeout value.
 *
 * @param base SDIOC peripheral base address.
 * @param timeout timeout value.
 */
static inline void SDIOC_SetDataTimeout(SDIOC_Type *base, sdioc_data_timeout_value_t timeout)
{
    base->MM4_CNTL2 = (base->MM4_CNTL2 & (~SDIOC_MM4_CNTL2_DTOCNTR_MASK)) | SDIOC_MM4_CNTL2_DTOCNTR(timeout);
}

/* @} */

/*!
 * @name SDIOC block gap functional interface
 * @{
 */
/*!
 * @brief SDIOC enable stop at block gap request.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is enable stop executing read and write transaction at next block gap, false is disable.
 */
static inline void SDIOC_EnableStopAtBlockGap(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL1 |= SDIOC_MM4_CNTL1_BGREQSTP_MASK;
    }
    else
    {
        base->MM4_CNTL1 &= ~SDIOC_MM4_CNTL1_BGREQSTP_MASK;
    }
}

/*!
 * @brief SDIOC enable continue request.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is restart a transaction which was stopped using stop at block gap request, false not affect.
 */
static inline void SDIOC_EnableContinueRequest(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL1 |= SDIOC_MM4_CNTL1_CONTREQ_MASK;
    }
    else
    {
        base->MM4_CNTL1 &= ~SDIOC_MM4_CNTL1_CONTREQ_MASK;
    }
}

/*!
 * @brief SDIOC enable/disable read wait control.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is enable read wait function, false is disable.
 */
static inline void SDIOC_EnableReadWaitControl(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL1 |= SDIOC_MM4_CNTL1_RDWTCNTL_MASK;
    }
    else
    {
        base->MM4_CNTL1 &= ~SDIOC_MM4_CNTL1_RDWTCNTL_MASK;
    }
}

/*!
 * @brief SDIOC enable/disable stop at block gap interrupt.
 *
 * @param base SDIOC peripheral base address.
 * @param enable true is enable interrupt at block gap, false is disable.
 */
static inline void SDIOC_EnableStopAtBlockGapInterrupt(SDIOC_Type *base, bool enable)
{
    if (enable)
    {
        base->MM4_CNTL1 |= SDIOC_MM4_CNTL1_BGIRQEN_MASK;
    }
    else
    {
        base->MM4_CNTL1 &= ~SDIOC_MM4_CNTL1_BGIRQEN_MASK;
    }
}

/*!
 * @brief SDIOC read command response.
 *
 * @param base SDIOC peripheral base address.
 * @param responseIndex response index.
 * @retval response read from sdioc.
 */
static inline uint32_t SDIOC_ReadCommandResponse(SDIOC_Type *base, sdioc_response_index_t responseIndex)
{
    return ((uint32_t *)&(base->MM4_RESP0))[responseIndex];
}

/*!
 * @brief SDIOC read data port.
 *
 * @param base SDIOC peripheral base address.
 * @retval data read from SDIOC fifo.
 */
static inline uint32_t SDIOC_ReadData(SDIOC_Type *base)
{
    return base->MM4_DP;
}

/*!
 * @brief SDIOC write data port.
 *
 * @param base SDIOC peripheral base address.
 * @param data data to be written to SDIOC fifo.
 */
static inline void SDIOC_WriteData(SDIOC_Type *base, uint32_t data)
{
    base->MM4_DP = data;
}

/* @} */

/*!
 * @name SDIOC dma functional interface
 * @{
 */

/*!
 * @brief SDIOC set dma buffer boundary.
 *
 * The large contiguous memory space may not be available in the virtual memory
 * system. To perform long DMA transfer, DMA System Address register shall be
 * updated at every system memory boundary during DMA transfer.
 * These bits specify the size of contiguous buffer in the system memory. The
 * SDMA transfer shall wait at the every boundary specified by these fields and the
 * Host Controller generates the DMA Interrupt to request the Host Driver to
 * update the SDMA System Address register.
 *
 * @param base SDIOC peripheral base address.
 * @param bufferBoundary dma buffer boundary.
 */
static inline void SDIOC_SetDmaBufferBoundary(SDIOC_Type *base, sdioc_dma_buffer_boundary_t bufferBoundary)
{
    base->MM4_BLK_CNTL =
        (base->MM4_BLK_CNTL & ~SDIOC_MM4_BLK_CNTL_DMA_BUFSZ_MASK) | SDIOC_MM4_BLK_CNTL_DMA_BUFSZ(bufferBoundary);
}

/*!
 * @brief SDIOC set dma buffer address.
 *
 * @param base SDIOC peripheral base address.
 * @param bufferAddress dma buffer address.
 */
static inline void SDIOC_SetDmaBufferAddress(SDIOC_Type *base, void *bufferAddress)
{
    base->MM4_SYSADDR = (uint32_t)bufferAddress;
}

/* @} */

/*!
 * @name SDIOC interrupt and status interface
 * @{
 */

/*!
 * @brief SDIOC enable/disable interrupt status.
 *
 * @param base SDIOC peripheral base address.
 * @param interruptFlags status flag to enable/disable, @ref _sdioc_interrupt_flag.
 * @param enable true is enable the corresponding interrupt status, false is disable.
 */
static inline void SDIOC_EnableInterruptStatus(SDIOC_Type *base, uint32_t interruptFlags, bool enable)
{
    if (enable)
    {
        base->MM4_I_STAT_EN |= interruptFlags;
    }
    else
    {
        base->MM4_I_STAT_EN &= ~interruptFlags;
    }
}

/*!
 * @brief SDIOC enable/disable interrupt signal.
 *
 * @param base SDIOC peripheral base address.
 * @param interruptFlags status flag to enable/disable, @ref _sdioc_interrupt_flag.
 * @param enable true is enable the corresponding interrupt status, false is disable.
 */
static inline void SDIOC_EnableInterruptSignal(SDIOC_Type *base, uint32_t interruptFlags, bool enable)
{
    if (enable)
    {
        base->MM4_I_SIG_EN |= interruptFlags;
    }
    else
    {
        base->MM4_I_SIG_EN &= ~interruptFlags;
    }
}

/*!
 * @brief SDIOC get interrupt status.
 *
 * @param base SDIOC peripheral base address.
 * @retval interrupt status flag.
 */
static inline uint32_t SDIOC_GetInterruptStatus(SDIOC_Type *base)
{
    return base->MM4_I_STAT;
}

/*!
 * @brief Gets the enabled interrupt status.
 *
 * @param base USDHC peripheral base address.
 * @retval Current interrupt status flags mask @ref _sdioc_interrupt_flag.
 */
static inline uint32_t SDIOC_GetEnabledInterruptStatus(SDIOC_Type *base)
{
    uint32_t intStatus = base->MM4_I_STAT;

    return intStatus & base->MM4_I_SIG_EN;
}

/*!
 * @brief SDIOC clear interrupt status.
 *
 * @param base SDIOC peripheral base address.
 * @param interruptFlags interrupt status flag to clear, @ref _sdioc_interrupt_flag..
 */
static inline void SDIOC_ClearInterruptStatus(SDIOC_Type *base, uint32_t interruptFlags)
{
    base->MM4_I_STAT = interruptFlags;
}

/*!
 * @brief SDIOC get host controller present status.
 *
 * @param base SDIOC peripheral base address.
 * @retval present status flag.
 */
static inline uint32_t SDIOC_GetPresentStatus(SDIOC_Type *base)
{
    return base->MM4_STATE;
}

/*!
 * @brief Transfer parameters command/data/dma configurations.
 *
 * @param base SDIOC base address.
 * @param command Transfer command pointer, application should not pass NULL pointer to this parameter.
 * @param data Transfer data pointer, application can pass NULL when send command only.
 * @param dmaConfig Internal dma configurations, application can pass NULL when dma is not used.
 *
 * @retval #kStatus_InvalidArgument Argument is invalid.
 * @retval #kStatus_Success Operate successfully.
 */
status_t SDIOC_SetTransferConfig(SDIOC_Type *base,
                                 sdioc_command_t *command,
                                 sdioc_data_t *data,
                                 sdioc_dma_config_t *dmaConfig);
/* @} */

/*!
 * @name SDIOC transactional interface
 * @{
 */
/*!
 * @brief Creates the SDIOC handle.
 *
 * @param base SDIOC peripheral base address.
 * @param handle SDIOC handle pointer.
 * @param callback Structure pointer to contain all callback functions.
 * @param userData Callback function parameter.
 */
void SDIOC_TransferCreateHandle(SDIOC_Type *base, sdioc_handle_t *handle, sdioc_callback_t *callback, void *userData);

/*!
 * @brief Transfers the command/data using an interrupt and an asynchronous method.
 *
 * This function sends a command and data and returns immediately. It doesn't wait for the transfer to complete or
 * to encounter an error. The application must not call this API in multiple threads at the same time. Because of that
 * this API doesn't support the re-entry mechanism.
 *
 * This function transfer data through SDIOC internal DMA.
 *
 * @note Call API @ref SDIOC_TransferCreateHandle when calling this API.
 *
 * @param handle SDIOC handle.
 * @param command Transfer command pointer should not be NULL.
 * @param data Transfer data pointer can be NULL if send command only.
 * @param dmaConfig Internal dma configurations, NULL pointer is not allowed.
 *
 * @retval #kStatus_InvalidArgument Argument is invalid.
 * @retval #kStatus_SDIOC_BusyTransferring Busy transferring.
 * @retval #kStatus_Success Operate successfully.
 */
status_t SDIOC_TransferDMANonBlocking(sdioc_handle_t *handle,
                                      sdioc_command_t *command,
                                      sdioc_data_t *data,
                                      sdioc_dma_config_t *dmaConfig);

/*!
 * @brief IRQ handler for the SDIOC.
 *
 * This function deals with the IRQs on the given host controller.
 *
 * @param handle SDIOC handle.
 */
void SDIOC_TransferHandleIRQ(sdioc_handle_t *handle);

/* @} */
#if defined(__cplusplus)
}
#endif
/*! @} */

#endif /* _FSL_SDIOC_H_ */
