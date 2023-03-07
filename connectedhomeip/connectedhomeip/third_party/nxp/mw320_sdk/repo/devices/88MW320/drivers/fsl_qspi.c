/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_qspi.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.qspi"
#endif

/*******************************************************************************
 * Definitations
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void QSPI_MemSet(void *src, uint8_t value, size_t length);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Base pointer array */
static QSPI_Type *const s_qspiBases[] = QSPI_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Array of QSPI clock name. */
static const clock_ip_name_t s_qspiClock[] = QSPI_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static void QSPI_MemSet(void *src, uint8_t value, size_t length)
{
    assert(src != NULL);
    uint8_t *p = src;

    for (volatile uint32_t i = 0U; i < length; i++)
    {
        *p = value;
        p++;
    }
}

/*!
 * brief Get the instance number for QSPI.
 *
 * param base QSPI base pointer.
 */
uint32_t QSPI_GetInstance(QSPI_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_qspiBases); instance++)
    {
        if (s_qspiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_qspiBases));

    return instance;
}

/*!
 * brief Initializes QSPI.
 *
 * param config QSPI configuration structure.
 */
void QSPI_Init(QSPI_Type *base, qspi_config_t *config)
{
    uint32_t val = 0;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Enable qspi clock */
    CLOCK_EnableClock(s_qspiClock[QSPI_GetInstance(base)]);
#endif

    /* Set QSPI clock prescaler */
    val |= QSPI_CONF_CLK_PRESCALE(config->preScale);
    /* Set data length mode */
    val |= QSPI_CONF_BYTE_LEN(config->byteLen);
    /* Set QSPI clock phase */
    val |= QSPI_CONF_CLK_PHA(config->clockPhase);
    /* Set QSPI clock polarity */
    val |= QSPI_CONF_CLK_POL(config->clockPolarity);
    /* Set data pin number */
    val |= QSPI_CONF_DATA_PIN(config->dataPinMode);
    /* Set address pin number */
    val |= QSPI_CONF_ADDR_PIN(config->addrPinMode);

    base->CONF = val;

    val = base->TIMING;
    /* Set QSPI capture clock edge */
    val |= QSPI_TIMING_CLK_CAPT_EDGE(config->captureEdge);
    base->TIMING = val;

    /* Set header count. */
    QSPI_SetHeaderCount(base, &config->headerCnt);

    /* Unmask all interrupts */
    QSPI_MaskInterrupts(base, 0x00U);
}

/*!
 * brief Gets default settings for QSPI.
 *
 * param config QSPI configuration structure.
 */
void QSPI_GetDefaultConfig(qspi_config_t *config)
{
    /* Initializes the configure structure to zero. */
    QSPI_MemSet(config, 0x00U, sizeof(*config));

    config->preScale      = kQSPI_ClockDiv1;
    config->byteLen       = kQSPI_TransferLength1Byte;
    config->clockPhase    = kQSPI_ClockPhaseRising;
    config->clockPolarity = kQSPI_ClockPolarityLow;
    config->dataPinMode   = kQSPI_DataPinSingle;
    config->addrPinMode   = kQSPI_AddrPinSingle;
    config->captureEdge   = kQSPI_CaptureEdgeFirst;

    /* Configure header count. */
    config->headerCnt.instructionCnt = kQSPI_InstructionCnt0Byte;
    config->headerCnt.addressCnt     = kQSPI_AddressCnt0Byte;
    config->headerCnt.readModeCnt    = kQSPI_ReadModeCnt0Byte;
    config->headerCnt.dummyCnt       = kQSPI_DummyCnt0Byte;
}

/*!
 * brief Deinitializes the QSPI module.
 *
 * param base Pointer to QSPI Type.
 */
void QSPI_Deinit(QSPI_Type *base)
{
}

/*!
 * brief Do software reset QSPI.
 *
 * param base Pointer to QSPI Type.
 */
void QSPI_SoftwareReset(QSPI_Type *base)
{
    base->CONF2 |= QSPI_CONF2_SRST_MASK;

    /* Delay */
    for (uint32_t i = 0x00U; i < 10U; i++)
    {
        __asm("nop");
    }

    base->CONF2 &= ~QSPI_CONF2_SRST_MASK;
}

/*!
 * brief Flush Write and Read FIFOs.
 *
 * @param base Pointer to QSPI Type.
 * @return return status.
 */
status_t QSPI_FlushFIFO(QSPI_Type *base)
{
    status_t status            = kStatus_InvalidArgument;
    volatile uint32_t localCnt = 0U;

    /* Flush write and read FIFO. */
    base->CONF |= QSPI_CONF_FIFO_FLUSH_MASK;

    /* Wait until Write and Read FIFOs are flushed. */
    while (localCnt++ < 0xFFFFFFFU)
    {
        if ((base->CONF & QSPI_CONF_FIFO_FLUSH_MASK) == 0x00U)
        {
            status = kStatus_Success;
            break;
        }
    }

    return status;
}

/*!
 * brief Set QSPI serial interface header count.
 *
 * @param base Pointer to QSPI Type.
 * @param config Pointer to header count configuration structure.
 */
void QSPI_SetHeaderCount(QSPI_Type *base, qspi_header_count_config_t *config)
{
    assert(config != NULL);

    uint32_t tempVal = 0x00U;

    tempVal = QSPI_HDRCNT_INSTR_CNT(config->instructionCnt) | QSPI_HDRCNT_ADDR_CNT(config->addressCnt) |
              QSPI_HDRCNT_RM_CNT(config->readModeCnt) | QSPI_HDRCNT_DUMMY_CNT(config->dummyCnt);

    base->HDRCNT = tempVal;
}

/*!
 * @brief Activate or de-activate serial select output.
 *
 * @param base Pointer to QSPI Type.
 * @param enable enable or disable.
 */
void QSPI_SetSSEnable(QSPI_Type *base, bool enable)
{
    if (enable == true)
    {
        base->CNTL |= QSPI_CNTL_SS_EN_MASK;
    }
    else
    {
        base->CNTL &= ~QSPI_CNTL_SS_EN_MASK;
    }

    while (0x00U == (QSPI_GetStatusFlags(base) & (uint32_t)kQSPI_TransferReady))
    {
    }
}

/*!
 * @brief Start the specified QSPI transfer.
 *
 * @param base Pointer to QSPI Type.
 * @param direction QSPI tansfer direction.
 */
void QSPI_StartTransfer(QSPI_Type *base, qspi_transfer_direction_t direction)
{
    uint32_t tempVal = 0x00U;

    /* Assert QSPI SS */
    base->CNTL |= QSPI_CNTL_SS_EN_MASK;
    /* Wait until QSPI ready */
    while ((base->CNTL & QSPI_CNTL_XFER_RDY_MASK) == 0x00U)
    {
    }

    tempVal = base->CONF;
    /* Set read/write mode */
    base->CONF = (tempVal & ~QSPI_CONF_RW_EN_MASK) | QSPI_CONF_RW_EN(direction);

    /* Start QSPI */
    base->CONF |= QSPI_CONF_XFER_START_MASK;
}

/*!
 * @brief Stop QSPI transfer.
 *
 * @param base Pointer to QSPI Type.
 */
void QSPI_StopTransfer(QSPI_Type *base)
{
    /* Wait until QSPI ready */
    while ((base->CNTL & QSPI_CNTL_XFER_RDY_MASK) == 0x00U)
    {
    }

    /* Wait until wfifo empty */
    while ((base->CNTL & QSPI_CNTL_WFIFO_EMPTY_MASK) == 0x00U)
    {
    }

    /* Stop QSPI */
    base->CONF |= QSPI_CONF_XFER_STOP_MASK;

    /* Wait until QSPI release start signal */
    while ((base->CONF & QSPI_CONF_XFER_START_MASK) == QSPI_CONF_XFER_START_MASK)
    {
    }

    /* De-assert QSPI SS */
    base->CNTL &= ~QSPI_CNTL_SS_EN_MASK;
    /* Wait until QSPI ready */
    while ((base->CNTL & QSPI_CNTL_XFER_RDY_MASK) == 0x00U)
    {
    }
}

/*!
 * @brief Write a byte to QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param data data to be written.
 */
void QSPI_WriteByte(QSPI_Type *base, uint8_t data)
{
    /* Wait unitl WFIFO is not full*/
    while ((base->CNTL & QSPI_CNTL_WFIFO_FULL_MASK) == QSPI_CNTL_WFIFO_FULL_MASK)
    {
    }

    base->DOUT = (data & 0xFFU);
}

/*!
 * @brief Read a byte from QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @return byte from QSPI serial interface.
 */
uint8_t QSPI_ReadByte(QSPI_Type *base)
{
    uint8_t data;

    /* Wait if RFIFO is empty*/
    while ((base->CNTL & QSPI_CNTL_RFIFO_EMPTY_MASK) == QSPI_CNTL_RFIFO_EMPTY_MASK)
    {
    }

    data = (base->DIN & 0xFFU);

    return data;
}

/*!
 * @brief Write a word to QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @param word data to be written.
 */
void QSPI_WriteWord(QSPI_Type *base, uint32_t data)
{
    /* Wait unitl WFIFO is not full*/
    while ((base->CNTL & QSPI_CNTL_RFIFO_FULL_MASK) == QSPI_CNTL_RFIFO_FULL_MASK)
    {
    }

    base->DOUT = data;
}

/*!
 * @brief Read a word from QSPI serial interface.
 *
 * @param base Pointer to QSPI Type.
 * @return word from QSPI serial interface.
 */
uint32_t QSPI_ReadWord(QSPI_Type *base)
{
    /* Wait if RFIFO is empty*/
    while ((base->CNTL & QSPI_CNTL_RFIFO_EMPTY_MASK) == QSPI_CNTL_RFIFO_EMPTY_MASK)
    {
    }

    return base->DIN;
}

/*!
 * @brief Set DMA tansfer related configuration.
 *
 * @param base Pointer to QSPI Type.
 * @param direction DMA transfer direction, read or write.
 * @param length DMA burst length, 1/4/8 bytes.
 */
void QSPI_SetDmaTransferConfig(QSPI_Type *base, qspi_dma_transfer_direction_t direction, qspi_dma_burst_len_t length)
{
    if (direction == kQSPI_DmaRead)
    {
        base->CONF2 |= QSPI_CONF2_DMA_RD_EN_MASK;
        base->CONF2 |= QSPI_CONF2_DMA_RD_BURST(length);
    }
    else
    {
        base->CONF2 |= QSPI_CONF2_DMA_WR_EN_MASK;
        base->CONF2 |= QSPI_CONF2_DMA_WR_BURST(length);
    }
}

/*!
 * brief Sends a buffer of data bytes using a blocking method.
 * note This function blocks via polling until all bytes have been sent.
 * param base QSPI base pointer
 * param buffer The data bytes to send
 * param size The number of data bytes to send
 */
void QSPI_WriteBlocking(QSPI_Type *base, uint32_t *buffer, size_t size)
{
    assert(size >= 4U);

    uint32_t i = 0;

    for (i = 0; i < size / 4U; i++)
    {
        /* Wait unitl WFIFO is not full*/
        while (0U != (QSPI_GetStatusFlags(base) & (uint32_t)kQSPI_WriteFifoFull))
        {
        }

        base->DOUT = *buffer++;
    }
}

/*!
 * brief Receives a buffer of data bytes using a blocking method.
 *
 * param base QSPI base pointer
 * param buffer The data bytes to send
 * param size The number of data bytes to receive
 */
void QSPI_ReadBlocking(QSPI_Type *base, uint32_t *buffer, size_t size)
{
    assert(size >= 4U);

    uint32_t i = 0;

    for (i = 0; i < size / 4U; i++)
    {
        /* Wait if RFIFO is empty*/
        while (0U != (QSPI_GetStatusFlags(base) & (uint32_t)kQSPI_ReadFifoEmpty))
        {
        }

        buffer[i] = base->DIN;
    }
}
