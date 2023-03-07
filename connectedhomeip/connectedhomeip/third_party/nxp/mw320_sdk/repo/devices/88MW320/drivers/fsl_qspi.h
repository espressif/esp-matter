/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_QSPI_H_
#define _FSL_QSPI_H_

#include "fsl_common.h"

/*!
 * @addtogroup qspi_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief QSPI driver version 2.0.0. */
#define FSL_QSPI_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief QSPI transfer mode */
typedef enum _qspi_transfer_mode
{
    kQSPI_TransferModeSingle, /*!< Use 1 serial interface pin.  */
    kQSPI_TransferModeDual,   /*!< Use 2 serial interface pins. */
    kQSPI_TransferModeQuad,   /*!< Use 4 serial interface pins. */
} qspi_transfer_mode_t;

/*! @brief QSPI data pin */
typedef enum _qspi_data_pin
{
    kQSPI_DataPinSingle, /*!< Use 1 serial interface pin.  */
    kQSPI_DataPinDual,   /*!< Use 2 serial interface pins. */
    kQSPI_DataPinQuad,   /*!< Use 4 serial interface pins. */
} qspi_data_pin_t;

/*! @brief QSPI address pin */
typedef enum _qspi_addr_pin
{
    kQSPI_AddrPinSingle, /*!< Use 1 serial interface pin */
    kQSPI_AddrPinAsData, /*!< Use the number of pins as indicated in DATA_PIN */
} qspi_addr_pin_t;

/*! @brief QSPI clock phase */
typedef enum _qspi_clock_phase
{
    kQSPI_ClockPhaseRising,  /*!< Data is latched on the rising edge of the serial clock. */
    kQSPI_ClockPhaseFalling, /*!< Data is latched on the falling edge of the serial clock. */
} qspi_clock_phase_t;

/*! @brief QSPI clock polarity */
typedef enum _qspi_clock_polarity
{
    kQSPI_ClockPolarityLow,  /*!< Inactive state of serial clock is low. */
    kQSPI_ClockPolarityHigh, /*!< Inactive state of serial clock is high. */
} qspi_clock_polarity_t;

/*! @brief QSPI capture clock edge */
typedef enum _qspi_capture_edge
{
    kQSPI_CaptureEdgeFirst,  /*!< Input data is captured on the first edge of the serial clock  */
    kQSPI_CaptureEdgeSecond, /*!< Input data is captured on the second edge of the serial clock */
} qspi_capture_edge_t;

/*! @brief QSPI transfer length */
typedef enum _qspi_transfer_length
{
    kQSPI_TransferLength1Byte,  /*!< 1 byte in each serial interface I/O transfer  */
    kQSPI_TransferLength4Bytes, /*!< 4 bytes in each serial interface I/O transfer */
} qspi_transfer_length_t;

/*! @brief QSPI clock prescaler */
typedef enum _qspi_clock_prescaler
{
    kQSPI_ClockDiv1  = 0x1,  /*!< Serial interface clock prescaler is SPI clock / 1  */
    kQSPI_ClockDiv2  = 0x2,  /*!< Serial interface clock prescaler is SPI clock / 2  */
    kQSPI_ClockDiv3  = 0x3,  /*!< Serial interface clock prescaler is SPI clock / 3  */
    kQSPI_ClockDiv4  = 0x4,  /*!< Serial interface clock prescaler is SPI clock / 4  */
    kQSPI_ClockDiv5  = 0x5,  /*!< Serial interface clock prescaler is SPI clock / 5  */
    kQSPI_ClockDiv6  = 0x6,  /*!< Serial interface clock prescaler is SPI clock / 6  */
    kQSPI_ClockDiv7  = 0x7,  /*!< Serial interface clock prescaler is SPI clock / 7  */
    kQSPI_ClockDiv8  = 0x8,  /*!< Serial interface clock prescaler is SPI clock / 8  */
    kQSPI_ClockDiv9  = 0x9,  /*!< Serial interface clock prescaler is SPI clock / 9  */
    kQSPI_ClockDiv10 = 0xA,  /*!< Serial interface clock prescaler is SPI clock / 10 */
    kQSPI_ClockDiv11 = 0xB,  /*!< Serial interface clock prescaler is SPI clock / 11 */
    kQSPI_ClockDiv12 = 0xC,  /*!< Serial interface clock prescaler is SPI clock / 12 */
    kQSPI_ClockDiv13 = 0xD,  /*!< Serial interface clock prescaler is SPI clock / 13 */
    kQSPI_ClockDiv14 = 0xE,  /*!< Serial interface clock prescaler is SPI clock / 14 */
    kQSPI_ClockDiv15 = 0xF,  /*!< Serial interface clock prescaler is SPI clock / 15 */
    kQSPI_ClockDiv16 = 0x18, /*!< Serial interface clock prescaler is SPI clock / 16 */
    kQSPI_ClockDiv18 = 0x19, /*!< Serial interface clock prescaler is SPI clock / 18 */
    kQSPI_ClockDiv20 = 0x1A, /*!< Serial interface clock prescaler is SPI clock / 20 */
    kQSPI_ClockDiv22 = 0x1B, /*!< Serial interface clock prescaler is SPI clock / 22 */
    kQSPI_ClockDiv24 = 0x1C, /*!< Serial interface clock prescaler is SPI clock / 24 */
    kQSPI_ClockDiv26 = 0x1D, /*!< Serial interface clock prescaler is SPI clock / 26 */
    kQSPI_ClockDiv28 = 0x1E, /*!< Serial interface clock prescaler is SPI clock / 28 */
    kQSPI_ClockDiv30 = 0x1F, /*!< Serial interface clock prescaler is SPI clock / 30 */
} qspi_clock_prescaler_t;

/*! @brief QSPI read or write type */
typedef enum _qspi_transfer_direction
{
    kQSPI_Read,  /*!< Read data from the serial interface */
    kQSPI_Write, /*!< Write data to the serial interface  */
} qspi_transfer_direction_t;

/*! @brief QSPI status bit, @anchor _qspi_status_flags*/
enum
{
    kQSPI_TransferReady      = QSPI_CNTL_XFER_RDY_MASK,      /*!< Serial interface transfer ready */
    kQSPI_ReadFifoEmpty      = QSPI_CNTL_RFIFO_EMPTY_MASK,   /*!< QSPI read FIFO empty flag */
    kQSPI_ReadFifoFull       = QSPI_CNTL_RFIFO_FULL_MASK,    /*!< QSPI read FIFO full flag */
    kQSPI_WriteFifoEmpty     = QSPI_CNTL_WFIFO_EMPTY_MASK,   /*!< QSPI write FIFO empty flag */
    kQSPI_WriteFifoFull      = QSPI_CNTL_WFIFO_FULL_MASK,    /*!< QSPI write FIFO full flag */
    kQSPI_ReadFifoUnderflow  = QSPI_CNTL_RFIFO_UNDRFLW_MASK, /*!< QSPI read FIFO underflow flag */
    kQSPI_ReadFifoOverflow   = QSPI_CNTL_RFIFO_OVRFLW_MASK,  /*!< QSPI read FIFO overflow flag */
    kQSPI_WriteFifoUnderflow = QSPI_CNTL_WFIFO_UNDRFLW_MASK, /*!< QSPI write FIFO underflow flag */
    kQSPI_WriteFifoOverflow  = QSPI_CNTL_WFIFO_OVERFLW_MASK, /*!< QSPI write FIFO overflow flag */
};

/*! @brief QSPI interrupt status bit, @anchor _qspi_interrupt_status_flags*/
enum
{
    kQSPI_InterruptTransferDone     = QSPI_ISR_XFER_DONE_IS_MASK, /*!< QSPI interrupt transfer done flag */
    kQSPI_InterruptTransferReady    = QSPI_ISR_XFER_RDY_IS_MASK,  /*!< QSPI interrupt transfer ready flag */
    kQSPI_InterruptReadFifoDmaBurst = QSPI_ISR_XFER_RDY_IS_MASK,  /*!< QSPI interrupt read FIFO DMA burst flag */
    kQSPI_InterruptWriteFifoDmaBurst =
        QSPI_ISR_WFIFO_DMA_BURST_IS_MASK,                               /*!< QSPI interrupt write FIFO DMA burst flag */
    kQSPI_InterruptReadFifoEmpty      = QSPI_ISR_RFIFO_EMPTY_IS_MASK,   /*!< QSPI interrupt read FIFO empty flag */
    kQSPI_InterruptReadFifoFull       = QSPI_ISR_RFIFO_FULL_IS_MASK,    /*!< QSPI interrupt read FIFO full flag */
    kQSPI_InterruptWriteFifoEmpty     = QSPI_ISR_WFIFO_EMPTY_IS_MASK,   /*!< QSPI interrupt write FIFO empty flag */
    kQSPI_InterruptWriteFifoFull      = QSPI_ISR_WFIFO_FULL_IS_MASK,    /*!< QSPI interrupt write FIFO full flag */
    kQSPI_InterruptReadFifoUnderflow  = QSPI_ISR_RFIFO_UNDRFLW_IS_MASK, /*!< QSPI interrupt read FIFO underflow flag */
    kQSPI_InterruptReadFifoOverflow   = QSPI_ISR_RFIFO_OVRFLW_IS_MASK,  /*!< QSPI interrupt read FIFO overflow flag */
    kQSPI_InterruptWriteFifoUnderflow = QSPI_ISR_WFIFO_UNDRFLW_IS_MASK, /*!< QSPI interrupt write FIFO underflow flag */
    kQSPI_InterruptWriteFifoOverflow  = QSPI_ISR_WFIFO_OVRFLW_IS_MASK,  /*!< QSPI interrupt write FIFO overflow flag */
};

/*! @brief QSPI interrupt mask status bit, @anchor _qspi_interrupt_mask_status_flags*/
enum
{
    kQSPI_InterruptTransferDoneMask  = QSPI_IMR_XFER_DONE_IM_MASK, /*!< QSPI interrupt transfer done mask flag */
    kQSPI_InterruptTransferReadyMask = QSPI_IMR_XFER_RDY_IM_MASK,  /*!< QSPI interrupt transfer ready mask flag*/
    kQSPI_InterruptReadFifoDmaBurstMask =
        QSPI_IMR_XFER_RDY_IM_MASK, /*!< QSPI interrupt read FIFO DMA burst mask flag */
    kQSPI_InterruptWriteFifoDmaBurstMask =
        QSPI_IMR_WFIFO_DMA_BURST_IM_MASK, /*!< QSPI interrupt write FIFO DMA burst mask flag */
    kQSPI_InterruptReadFifoEmptyMask  = QSPI_IMR_RFIFO_EMPTY_IM_MASK, /*!< QSPI interrupt read FIFO empty mask flag */
    kQSPI_InterruptReadFifoFullMask   = QSPI_IMR_RFIFO_FULL_IM_MASK,  /*!< QSPI interrupt read FIFO full mask flag */
    kQSPI_InterruptWriteFifoEmptyMask = QSPI_IMR_WFIFO_EMPTY_IM_MASK, /*!< QSPI interrupt write FIFO empty mask flag */
    kQSPI_InterruptWriteFifoFullMask  = QSPI_IMR_WFIFO_FULL_IM_MASK,  /*!< QSPI interrupt write FIFO full mask flag */
    kQSPI_InterruptReadFifoUnderflowMask =
        QSPI_IMR_RFIFO_UNDRFLW_IM_MASK, /*!< QSPI interrupt read FIFO underflow mask flag */
    kQSPI_InterruptReadFifoOverflowMask =
        QSPI_IMR_RFIFO_OVRFLW_IM_MASK, /*!< QSPI interrupt read FIFO overflow mask flag */
    kQSPI_InterruptWriteFifoUnderflowMask =
        QSPI_IMR_WFIFO_UNDRFLW_IM_MASK, /*!< QSPI interrupt write FIFO underflow mask flag */
    kQSPI_InterruptWriteFifoOverflowMask =
        QSPI_IMR_WFIFO_OVRFLW_IM_MASK, /*!< QSPI interrupt write FIFO overflow mask flag */
};

/*! @brief QSPI dma read/write type  */
typedef enum _qspi_dma_transfer_direction
{
    kQSPI_DmaRead,  /*!< QSPI DMA read  */
    kQSPI_DmaWrite, /*!< QSPI DMA write */
} qspi_dma_transfer_direction_t;

/*! @brief QSPI dma read/write burst type */
typedef enum _qspi_dma_burst_len
{
    kQSPI_BurstLen1, /*!< One data(each of width BYTE_LEN) is available in the Read/Write FIFO before a receive/transmit
                        burst request is made to the DMA   */
    kQSPI_BurstLen4, /*!< Four data(each of width BYTE_LEN) is available in the Read/Write FIFO before a
                        receive/transmit burst request is made to the DMA  */
    kQSPI_BurstLen8, /*!< Eight data(each of width BYTE_LEN) is available in the Read/Write FIFO before a
                        receive/transmit burst request is made to the DMA */
    kQSPI_BurstLen0, /*!< Disable DMA read/write */
} qspi_dma_burst_len_t;

/*! @brief QSPI Instruction Count */
typedef enum _qspi_instruction_count
{
    kQSPI_InstructionCnt0Byte, /*!< Instrution count: 0 byte  */
    kQSPI_InstructionCnt1Byte, /*!< Instrution count: 1 byte  */
    kQSPI_InstructionCnt2Byte, /*!< Instrution count: 2 bytes */
} qspi_instruction_count_t;

/*! @brief QSPI Address Count */
typedef enum _qspi_address_count
{
    kQSPI_AddressCnt0Byte, /*!< Address count: 0 byte  */
    kQSPI_AddressCnt1Byte, /*!< Address count: 1 byte  */
    kQSPI_AddressCnt2Byte, /*!< Address count: 2 bytes */
    kQSPI_AddressCnt3Byte, /*!< Address count: 3 bytes */
    kQSPI_AddressCnt4Byte, /*!< Address count: 4 bytes */
} qspi_address_count_t;

/*! @brief QSPI Read Mode Count */
typedef enum _qspi_read_mode_count
{
    kQSPI_ReadModeCnt0Byte, /*!< Read mode count: 0 byte  */
    kQSPI_ReadModeCnt1Byte, /*!< Read mode count: 1 byte  */
    kQSPI_ReadModeCnt2Byte, /*!< Read mode count: 2 bytes */
} qspi_read_mode_count_t;

/*! @brief QSPI Dummy Count */
typedef enum _qspi_dummy_count
{
    kQSPI_DummyCnt0Byte, /*!< Dummy count: 0 byte  */
    kQSPI_DummyCnt1Byte, /*!< Dummy count: 1 byte  */
    kQSPI_DummyCnt2Byte, /*!< Dummy count: 2 bytes */
    kQSPI_DummyCnt3Byte, /*!< Dummy count: 3 bytes */
} qspi_dummy_count_t;

/*! @brief QSPI header count configuration structure*/
typedef struct _qspi_header_count_config
{
    qspi_instruction_count_t instructionCnt; /*!< Instruction Count, number of bytes in INSTR register to shift out. */
    qspi_address_count_t addressCnt;         /*!< Address Count, number of bytes in ADDR register to shift out. */
    qspi_read_mode_count_t readModeCnt;      /*!< Read Mode Count, number of bytes in RMODE register to shift out. */
    qspi_dummy_count_t dummyCnt;             /*!< Dummy Count, number of bytes as dummy to shift out. */
} qspi_header_count_config_t;

/*! @brief QSPI configuration structure*/
typedef struct _qspi_config
{
    qspi_clock_prescaler_t preScale;     /*!< Serial Interface Clock Prescaler. */
    qspi_transfer_length_t byteLen;      /*!< Byte Length, the number of bytes in each serial interface I/O transfer. */
    qspi_clock_phase_t clockPhase;       /*!< Serial Interface Clock Phase. */
    qspi_clock_polarity_t clockPolarity; /*!< Serial Interface Clock Polarity. */
    qspi_data_pin_t dataPinMode;         /*!< Data Transfer Pin. */
    qspi_addr_pin_t addrPinMode;         /*!< Address Transfer Pin. */
    qspi_capture_edge_t captureEdge;     /*!< Serial Interface Capture Clock Edge. */

    qspi_header_count_config_t headerCnt; /*!< Serial Interface header count. */
} qspi_config_t;

/******************************************************************************
 * API
 *****************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Get the instance number for QSPI.
 *
 * @param base QSPI base pointer.
 */
uint32_t QSPI_GetInstance(QSPI_Type *base);

/*!
 * @brief Initializes the QSPI module and internal state.
 *
 * This function enables the clock for QSPI and also configures the QSPI with the
 * input configure parameters. Users should call this function before any QSPI operations.
 *
 * @param base Pointer to QuadSPI Type.
 * @param config QSPI configure structure.
 */
void QSPI_Init(QSPI_Type *base, qspi_config_t *config);

/*!
 * @brief Gets default settings for QSPI.
 *
 * @param config QSPI configuration structure.
 */
void QSPI_GetDefaultConfig(qspi_config_t *config);

/*!
 * @brief Deinitializes the QSPI module.
 *
 * @param base Pointer to QuadSPI Type.
 */
void QSPI_Deinit(QSPI_Type *base);

/*!
 * @brief Do software reset QSPI.
 *
 * @param base Pointer to QSPI Type.
 */
void QSPI_SoftwareReset(QSPI_Type *base);

/*! @} */

/*!
 * @name Status
 * @{
 */

/*!
 * @brief Gets the state value of QSPI.
 *
 * @param base Pointer to QSPI Type.
 * @return status flag, use status flag to AND @ref _qspi_interrupt_mask_status_flags could get the related status.
 */
static inline uint32_t QSPI_GetInterruptMaskStatusFlags(QSPI_Type *base)
{
    return base->IMR;
}

/*!
 * @brief Mask/Unmask specified interrupts.
 *
 * @param base Pointer to QSPI Type.
 * @param mask interrupt mask.
 */
static inline void QSPI_MaskInterrupts(QSPI_Type *base, uint32_t mask)
{
    base->IMR = mask;
}

/*!
 * @brief Get QSPI status flag.
 *
 * @param base Pointer to QSPI Type.
 * @return status flag, use status flag to AND @ref _qspi_status_flags could get the related status.
 */
static inline uint32_t QSPI_GetStatusFlags(QSPI_Type *base)
{
    return base->CNTL;
}

/*!
 * @brief Get interrupt status.
 *
 * @param base Pointer to QSPI Type.
 * @return status flag, use status flag to AND @ref _qspi_interrupt_status_flags could get the related status.
 */
static inline uint32_t QSPI_GetInterruptStatus(QSPI_Type *base)
{
    return base->ISR;
}

/*!
 * @brief Clear transfer done interrupt.
 *
 * @param base Pointer to QSPI Type.
 */
static inline void QSPI_ClearTransferDoneStatus(QSPI_Type *base)
{
    base->ISC = QSPI_ISC_XFER_DONE_IC_MASK;
}

/*! @} */

/*!
 * @name QSPI functional interface
 * @{
 */

/*!
 * @brief Flush Write and Read FIFOs.
 *
 * @param base Pointer to QSPI Type.
 * @return return status.
 */
status_t QSPI_FlushFIFO(QSPI_Type *base);

/*!
 * @brief Set QSPI serial interface transfer mode.
 *
 * @param base Pointer to QSPI Type.
 * @param dataPin QSPI tansfer data pin.
 */
static inline void QSPI_SetDatePin(QSPI_Type *base, qspi_data_pin_t dataPin)
{
    uint32_t tempVal = 0x00U;

    tempVal    = base->CONF;
    base->CONF = (tempVal & ~QSPI_CONF_DATA_PIN_MASK) | QSPI_CONF_DATA_PIN(dataPin);
}

/*!
 * @brief Set QSPI serial interface transfer mode.
 *
 * @param base Pointer to QSPI Type.
 * @param addrPin QSPI tansfer address pin.
 */
static inline void QSPI_SetAddressPin(QSPI_Type *base, qspi_addr_pin_t addrPin)
{
    uint32_t tempVal = 0x00U;

    tempVal    = base->CONF;
    base->CONF = (tempVal & ~QSPI_CONF_ADDR_PIN_MASK) | QSPI_CONF_ADDR_PIN(addrPin);
}

/*!
 * @brief Set QSPI serial interface header count .
 *
 * @param base Pointer to QSPI Type.
 * @param config Pointer to header count configuration structure.
 */
void QSPI_SetHeaderCount(QSPI_Type *base, qspi_header_count_config_t *config);

/*!
 * @brief Set number of bytes in INSTR register to shift out to the serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param count number of bytes in INSTR register to shift out.
 */
static inline void QSPI_SetInstructionCnt(QSPI_Type *base, qspi_instruction_count_t count)
{
    uint32_t temp = 0x00U;

    temp         = base->HDRCNT;
    base->HDRCNT = (temp & ~QSPI_HDRCNT_INSTR_CNT_MASK) | QSPI_HDRCNT_INSTR_CNT(count);
}

/*!
 * @brief Set number of bytes in ADDR register to shift out to the serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param count number of bytes in ADDR register to shift out.
 */
static inline void QSPI_SetAddressCnt(QSPI_Type *base, qspi_address_count_t count)
{
    uint32_t temp = 0x00U;

    temp         = base->HDRCNT;
    base->HDRCNT = (temp & ~QSPI_HDRCNT_ADDR_CNT_MASK) | QSPI_HDRCNT_ADDR_CNT(count);
}

/*!
 * @brief Set number of bytes in RM register to shift out to the serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param count number of bytes in RM register to shift out.
 */
static inline void QSPI_SetReadModeCnt(QSPI_Type *base, qspi_read_mode_count_t count)
{
    uint32_t temp = 0x00U;

    temp         = base->HDRCNT;
    base->HDRCNT = (temp & ~QSPI_HDRCNT_RM_CNT_MASK) | QSPI_HDRCNT_RM_CNT(count);
}

/*!
 * @brief Set number of bytes as dummy to shift out to the serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param count number of bytes as dummy to shift out.
 */
static inline void QSPI_SetDummyCnt(QSPI_Type *base, qspi_dummy_count_t count)
{
    uint32_t temp = 0x00U;

    temp         = base->HDRCNT;
    base->HDRCNT = (temp & ~QSPI_HDRCNT_DUMMY_CNT_MASK) | QSPI_HDRCNT_DUMMY_CNT(count);
}

/*!
 * @brief Set QSPI serial interface instruction.
 *
 * @param base Pointer to QSPI Type.
 * @param instruction QSPI serial interface instruction.
 */
static inline void QSPI_SetInstruction(QSPI_Type *base, uint32_t instruction)
{
    base->INSTR = QSPI_INSTR_INSTR(instruction);
}

/*!
 * @brief Set QSPI serial interface address.
 *
 * @param base Pointer to QSPI Type.
 * @param address QSPI serial interface address.
 */
static inline void QSPI_SetAddress(QSPI_Type *base, uint32_t address)
{
    base->ADDR = address;
}

/*!
 * @brief Set QSPI serial interface read mode.
 *
 * @param base Pointer to QSPI Type.
 * @param mode QSPI serial interface read mode.
 */
static inline void QSPI_SetReadMode(QSPI_Type *base, uint32_t mode)
{
    base->RDMODE = QSPI_RDMODE_RMODE(mode);
}

/*!
 * @brief Set number of bytes of data to shift in from the serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param count number of bytes of data to shift in.
 */
static inline void QSPI_SetDataInCnt(QSPI_Type *base, uint32_t count)
{
    base->DINCNT = QSPI_DINCNT_DATA_IN_CNT(count);
}

/*!
 * @brief Update the number of bytes in each serial interface I/O transfer.
 *
 * @param base Pointer to QSPI Type.
 * @param length the number of bytes in each serial interface I/O transfer.
 */
static inline void QSPI_UpdateTansferLength(QSPI_Type *base, qspi_transfer_length_t length)
{
    uint32_t temp = 0x00U;

    temp       = base->CONF;
    base->CONF = (temp & ~QSPI_CONF_BYTE_LEN_MASK) | QSPI_CONF_BYTE_LEN(length);
}

/*!
 * @brief Activate or de-activate serial select output.
 *
 * @param base Pointer to QSPI Type.
 * @param enable enable or disable.
 */
void QSPI_SetSSEnable(QSPI_Type *base, bool enable);

/*!
 * @brief Start the specified QSPI transfer.
 *
 * @param base Pointer to QSPI Type.
 * @param direction QSPI tansfer direction.
 */
void QSPI_StartTransfer(QSPI_Type *base, qspi_transfer_direction_t direction);

/*!
 * @brief Stop QSPI transfer.
 *
 * @param base Pointer to QSPI Type.
 */
void QSPI_StopTransfer(QSPI_Type *base);

/*!
 * @brief Write a byte to QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param data data to be written.
 */
void QSPI_WriteByte(QSPI_Type *base, uint8_t data);

/*!
 * @brief Read a byte from QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @return byte from QSPI serial interface.
 */
uint8_t QSPI_ReadByte(QSPI_Type *base);

/*!
 * @brief Write a word to QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param word data to be written.
 */
void QSPI_WriteWord(QSPI_Type *base, uint32_t word);

/*!
 * @brief Read a word from QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @return word from QSPI serial interface.
 */
uint32_t QSPI_ReadWord(QSPI_Type *base);

/*!
 * @brief Set DMA tansfer related configuration.
 *
 * @param base Pointer to QSPI Type.
 * @param direction DMA transfer direction, read or write.
 * @param length DMA burst length, 1/4/8 bytes.
 */
void QSPI_SetDmaTransferConfig(QSPI_Type *base, qspi_dma_transfer_direction_t direction, qspi_dma_burst_len_t length);

/*!
 * @brief Sends a buffer of data bytes using a blocking method.
 * @note This function blocks via polling until all bytes have been sent.
 * @param base QSPI base pointer
 * @param buffer The data bytes to send
 * @param size The number of data bytes to send
 */
void QSPI_WriteBlocking(QSPI_Type *base, uint32_t *buffer, size_t size);

/*!
 * @brief Receives a buffer of data bytes using a blocking method.
 *
 * @param base QSPI base pointer
 * @param buffer The data bytes to send
 * @param size The number of data bytes to receive
 */
void QSPI_ReadBlocking(QSPI_Type *base, uint32_t *buffer, size_t size);

/*! @} */

#if defined(__cplusplus)
}
#endif

/* @}*/

#endif /* _FSL_QSPI_H_*/
