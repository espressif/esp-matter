/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the UART on K8C devices.
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

#include "hal.h"
#include "hal_DMA.h"
#include "gpBsp.h"
#include "gpHal_reg.h"
#include "gpAssert.h"

#define GP_COMPONENT_ID     GP_COMPONENT_ID_HALCORTEXM4

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define HAL_UART_NR_OF_UARTS            3

#if defined(HAL_UART_DMA_MASK)
/* if DMA mask is defined that applies to both TX/RX DMA */
#if defined(HAL_UART_TX_DMA_MASK) || defined(HAL_UART_RX_DMA_MASK)
#error "Redundant definition of UART TX/RX DMA masks"
#endif
#define HAL_UART_TX_DMA_MASK HAL_UART_DMA_MASK
#define HAL_UART_RX_DMA_MASK HAL_UART_DMA_MASK
#endif

#if !defined(HAL_UART_TX_DMA_MASK)
#define HAL_UART_TX_DMA_MASK 0
#endif

#if !defined(HAL_UART_RX_DMA_MASK)
#define HAL_UART_RX_DMA_MASK 0
#endif

#define HAL_UART_COM_SYMBOL_PERIOD (((16000000L+(8*GP_BSP_UART_COM_BAUDRATE/2)) / (8*GP_BSP_UART_COM_BAUDRATE))-1)
GP_COMPILE_TIME_VERIFY(HAL_UART_COM_SYMBOL_PERIOD <=  0x0FFF);

#if defined(GP_BSP_UART_COM2_BAUDRATE)
#define HAL_UART_COM2_SYMBOL_PERIOD (((16000000L+(8*GP_BSP_UART_COM2_BAUDRATE/2)) / (8*GP_BSP_UART_COM2_BAUDRATE))-1)
GP_COMPILE_TIME_VERIFY(HAL_UART_COM2_SYMBOL_PERIOD <=  0x0FFF);
#endif

#define HAL_UART_SCOM_SYMBOL_PERIOD (((16000000L+(8*GP_BSP_UART_SCOM_BAUDRATE/2)) / (8*GP_BSP_UART_SCOM_BAUDRATE))-1)
GP_COMPILE_TIME_VERIFY(HAL_UART_SCOM_SYMBOL_PERIOD <=  0x0FFF);

#if !defined(HAL_UART_RX_BUFFER_SIZE)
#define HAL_UART_RX_BUFFER_SIZE   64U
#endif //if !defined(HAL_UART_RX_BUFFER_SIZE)

#if !defined(HAL_UART_TX_BUFFER_SIZE)
#define HAL_UART_TX_BUFFER_SIZE  (64U)
#endif

/**
  * Set the almost complete dma threshold threshold to half
  * the size of the buffer (assuming moderate system load)
  */
#if !defined(HAL_UART_TX_DMA_THRESHOLD)
#define HAL_UART_TX_DMA_THRESHOLD (HAL_UART_TX_BUFFER_SIZE / 2U)
#endif

#define UART_BASE_ADDR_FROM_NR(UartNr) \
  ((UartNr) == 0) ? GP_WB_UART_0_BASE_ADDRESS : GP_WB_UART_1_BASE_ADDRESS

/* Return 1 if the specified UART can use DMA, otherwise return 0. */
#define HAL_UART_TX_USE_DMA(uart)      ( (HAL_UART_TX_DMA_MASK >> (uart)) & 1 )
#define HAL_UART_RX_USE_DMA(uart)      ( (HAL_UART_RX_DMA_MASK >> (uart)) & 1 )

/* Number of UARTs with DMA enabled. */
#define HAL_UART_TX_NR_UARTS_WITH_DMA  ( HAL_UART_TX_USE_DMA(0) + HAL_UART_TX_USE_DMA(1) + HAL_UART_TX_USE_DMA(2) )
#define HAL_UART_RX_NR_UARTS_WITH_DMA  ( HAL_UART_RX_USE_DMA(0) + HAL_UART_RX_USE_DMA(1) + HAL_UART_RX_USE_DMA(2) )

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static UInt8 halUart_TxEnabledMask = 0;
static hal_cbUartGetTxData_t   hal_cbUartGetTxData[HAL_UART_NR_OF_UARTS] = { NULL, };
#define hal_UartStoreGetTxData(uart, cb)  { hal_cbUartGetTxData[(uart)] = (cb); }
#define hal_UartHandleGetTxData(uart)   ((hal_cbUartGetTxData[(uart)] == NULL)? -1 : hal_cbUartGetTxData[uart]())

#if (HAL_UART_RX_DMA_MASK != 0)
#if !defined(HAL_UART_NO_RX)
static hal_DmaChannel_t halUart_DmaRxChannel[HAL_UART_RX_NR_UARTS_WITH_DMA];
static UInt8            halUart_DmaRxBuffer[HAL_UART_RX_NR_UARTS_WITH_DMA][HAL_UART_RX_BUFFER_SIZE];
static hal_DmaPointer_t halUart_DmaRxReadPtr[HAL_UART_RX_NR_UARTS_WITH_DMA];
#endif /* !defined(HAL_UART_NO_RX) */
#endif /* (HAL_UART_RX_DMA_MASK != 0) */

#if (HAL_UART_TX_DMA_MASK != 0)
static hal_DmaChannel_t halUart_DmaTxChannel[HAL_UART_TX_NR_UARTS_WITH_DMA];
static UInt8            halUart_DmaTxBuffer[HAL_UART_TX_NR_UARTS_WITH_DMA][HAL_UART_TX_BUFFER_SIZE];
static hal_DmaPointer_t halUart_DmaTxWritePtr[HAL_UART_TX_NR_UARTS_WITH_DMA];
#endif /* (HAL_UART_TX_DMA_MASK != 0) */

#ifndef HAL_UART_NO_RX
static UInt8 halUart_RxEnabledMask = 0;
static hal_cbUartRx_t hal_cbUartRx[HAL_UART_NR_OF_UARTS] = { NULL, };
#define hal_UartStoreRx(uart,cb)     {hal_cbUartRx[(uart)] = (cb);}
#else
#define hal_UartStoreRx(uart,cb)     NOT_USED(cb)
#endif  /* HAL_UART_NO_RX */

static hal_cbUartEot_t  hal_cbUartOneShotEndOfTx;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/* Write interrupt enable bit INT_CTRL_MASK_INT_UART_x_INTERRUPT */
static INLINE void halUart_SetIntCtrlMaskUartInterrupt(UInt8 uart, Bool enable)
{
    switch (uart) {
        case 0: GP_WB_WRITE_INT_CTRL_MASK_INT_UART_0_INTERRUPT(enable); break;
        case 1: GP_WB_WRITE_INT_CTRL_MASK_INT_UART_1_INTERRUPT(enable); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }
}

/* Write interrupt enable bit INT_CTRL_MASK_UART_x_TX_NOT_FULL_INTERRUPT */
static INLINE void halUart_SetIntCtrlMaskUartTxNotFull(UInt8 uart, Bool enable)
{
    switch (uart) {
        case 0: GP_WB_WRITE_INT_CTRL_MASK_UART_0_TX_NOT_FULL_INTERRUPT(enable); break;
        case 1: GP_WB_WRITE_INT_CTRL_MASK_UART_1_TX_NOT_FULL_INTERRUPT(enable); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }
}

/* Write interrupt enable bit GP_WB_WRITE_INT_CTRL_MASK_UART_x_RX_NOT_EMPTY_INTERRUPT */
static INLINE void halUart_SetIntCtrlMaskUartRxNotEmpty(UInt8 uart, Bool enable)
{
    switch(uart) {
        case 0: GP_WB_WRITE_INT_CTRL_MASK_UART_0_RX_NOT_EMPTY_INTERRUPT(enable); break;
        case 1: GP_WB_WRITE_INT_CTRL_MASK_UART_1_RX_NOT_EMPTY_INTERRUPT(enable); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }
}

static INLINE void halUart_SetStandbyResetEpiUart(UInt8 uart, Bool enable)
{
    switch(uart) {
        case 0: GP_WB_WRITE_STANDBY_RESET_EPI_UART_0(enable); break;
        case 1: GP_WB_WRITE_STANDBY_RESET_EPI_UART_1(enable); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }
}

/* Enable or disable UART TX GPIO alternate function. */
static INLINE void halUart_SetUartTxGpioEnabled(UInt8 uart, Bool enable)
{
    switch (uart)
    {
        case 0: GP_BSP_UART0_TX_ENABLE(enable); break;
        case 1: GP_BSP_UART1_TX_ENABLE(enable); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }
}

/* Enable or disable UART RX GPIO alternate function. */
static INLINE void halUart_SetUartRxGpioEnabled(UInt8 uart, Bool enable)
{
    switch(uart) {
        case 0: GP_BSP_UART0_RX_ENABLE(enable); break;
        case 1: GP_BSP_UART1_RX_ENABLE(enable); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }
}

#if HAL_UART_TX_DMA_MASK != 0
/* Map UART number to index into the list of UART DMA channels. */
static INLINE UInt32 halUart_UartTxToDmaIndex(UInt8 uart)
{
    UInt32 idx = 0;
    if (uart > 0)
    {
        idx += HAL_UART_TX_USE_DMA(0);
    }
    if (uart > 1)
    {
        idx += HAL_UART_TX_USE_DMA(1);
    }
    return idx;
}
#endif

#if (HAL_UART_RX_DMA_MASK != 0) && !defined(HAL_UART_NO_RX)
/* Map UART number to index into the list of UART DMA channels. */
static INLINE UInt32 halUart_UartRxToDmaIndex(UInt8 uart)
{
    UInt32 idx = 0;
    if (uart > 0)
    {
        idx += HAL_UART_RX_USE_DMA(0);
    }
    if (uart > 1)
    {
        idx += HAL_UART_RX_USE_DMA(1);
    }
    return idx;
}
#endif

#ifndef HAL_UART_NO_RX
static void halUart_RxCheckErrors(UInt8 uart)
{
}

static void halUart_HandleIntRx(UInt8 uart)
{
    UInt32 uartBaseAddress;
    UInt8 data;

    halUart_RxCheckErrors(uart);

    uartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);
    GP_ASSERT_DEV_INT(GP_WB_READ_UART_UNMASKED_RX_NOT_EMPTY_INTERRUPT(uartBaseAddress));
    data = GP_WB_READ_UART_RX_DATA_0(uartBaseAddress);
    GP_ASSERT_DEV_EXT(hal_cbUartRx[uart] != NULL);
#if defined(HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK)
    hal_cbUartRx[uart](&data, 1);
#else
    hal_cbUartRx[uart](data);
#endif

    // Do not check or attempt to read multiple received bytes here.
    // - Reading multiple bytes is not needed. If another byte is available,
    //   uartX_handler_impl() will be called again to handle the new byte.
    // - Reading multiple bytes may leave UARTx_IRQ pending in NVIC without
    //   pending UART event. This triggers assert in uartX_handler_impl.
}

#if HAL_UART_RX_DMA_MASK != 0

static void halUart_RxHandleDma(UInt8 uart)
{
    GP_ASSERT_DEV_INT(HAL_UART_RX_USE_DMA(uart));
    UInt32 dmaIndex = halUart_UartRxToDmaIndex(uart);
    hal_DmaChannel_t dmaChannel = halUart_DmaRxChannel[dmaIndex];
    hal_DmaPointer_t readPtr;

    if (hal_cbUartRx[uart] == NULL)
    {
        return;
    }
    HAL_DISABLE_GLOBAL_INT();

    halUart_RxCheckErrors(uart);

    readPtr = halUart_DmaRxReadPtr[dmaIndex];
    do
    {
        hal_DmaPointer_t writePtr;
        UInt16 chunkSize;
        writePtr = hal_DmaGetInternalPointer(dmaChannel);
        if (HAL_DMA_POINTERS_EQUAL(writePtr,readPtr))
        {
            break;
        }
        chunkSize = hal_DmaBuffer_GetNextContinuousSize(writePtr, readPtr, HAL_UART_RX_BUFFER_SIZE);

#if defined(HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK)
        hal_cbUartRx[uart](&halUart_DmaRxBuffer[dmaIndex][readPtr.offset], chunkSize);
#else
        UInt16 i;
        for (i = 0; i < chunkSize; i++)
        {
            hal_cbUartRx[uart](halUart_DmaRxBuffer[dmaIndex][readPtr.offset + i]);
        }
#endif

        readPtr.offset += chunkSize;
        if (readPtr.offset == HAL_UART_RX_BUFFER_SIZE)
        {
            readPtr.wrap = !readPtr.wrap;
            readPtr.offset = 0;
        }
    } while (true);

    halUart_DmaRxReadPtr[dmaIndex] = readPtr;
    hal_DmaUpdatePointers(dmaChannel, readPtr);

    HAL_ENABLE_GLOBAL_INT();
}

/* Called from DMA interrupt handler when number of bytes in buffer exceeds threshold. */
static void halUart_cbDmaBufferAlmostComplete(hal_DmaChannel_t dmaChannel)
{
    UInt8 uart;
    for (uart = 0; uart < HAL_UART_NR_OF_UARTS; uart++)
    {
        if (HAL_UART_RX_USE_DMA(uart) && halUart_DmaRxChannel[halUart_UartRxToDmaIndex(uart)] == dmaChannel)
        {
            halUart_RxHandleDma(uart);
            return;
        }
    }

    // Got DMA callback for unknown DMA channel.
    GP_ASSERT_DEV_INT(false);
}

static void halUart_RxEnableDma(UInt8 uart)
{
    GP_ASSERT_DEV_INT(HAL_UART_RX_USE_DMA(uart));

    UInt32 dmaIndex = halUart_UartRxToDmaIndex(uart);

    MEMSET(&halUart_DmaRxReadPtr[dmaIndex],0,sizeof(hal_DmaPointer_t));

    hal_DmaDescriptor_t dmaDesc;
    MEMSET(&dmaDesc, 0, sizeof(dmaDesc));
    dmaDesc.channel = halUart_DmaRxChannel[dmaIndex];
    dmaDesc.cbAlmostComplete = halUart_cbDmaBufferAlmostComplete;
    dmaDesc.cbComplete = NULL;
    dmaDesc.wordMode = GP_WB_ENUM_DMA_WORD_MODE_BYTE;
    dmaDesc.bufferSize = HAL_UART_RX_BUFFER_SIZE;
    dmaDesc.circBufSel = GP_WB_ENUM_CIRCULAR_BUFFER_DEST_BUFFER;
    dmaDesc.srcAddrInRam = false;
    dmaDesc.destAddr = (UInt32) halUart_DmaRxBuffer[dmaIndex];
    dmaDesc.destAddrInRam = true;
    dmaDesc.bufCompleteIntMode = GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_ERROR_MODE;

    // Get notification as soon as at least 1 character is pending in the buffer.
    dmaDesc.threshold = 1;

    switch (uart)
    {
        case 0:
            dmaDesc.srcAddr = GP_WB_UART_0_RX_DATA_0_ADDRESS;
            dmaDesc.dmaTriggerSelect = GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_0_RX_NOT_EMPTY;
            break;
        case 1:
            dmaDesc.srcAddr = GP_WB_UART_1_RX_DATA_0_ADDRESS;
            dmaDesc.dmaTriggerSelect = GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_1_RX_NOT_EMPTY;
            break;
        default:
            GP_ASSERT_DEV_INT(false);
            break;
    }

    hal_DmaResult_t result = hal_DmaStart(&dmaDesc);
    GP_ASSERT_SYSTEM(result == HAL_DMA_RESULT_SUCCESS);
}

static void halUart_RxDisableDma(UInt8 uart)
{
    GP_ASSERT_DEV_INT(HAL_UART_RX_USE_DMA(uart));

    UInt32 dmaIndex = halUart_UartRxToDmaIndex(uart);
    hal_DmaResult_t result = hal_DmaStop(halUart_DmaRxChannel[dmaIndex]);
    GP_ASSERT_SYSTEM(result == HAL_DMA_RESULT_SUCCESS);
}


#endif // HAL_UART_DMA_MASK != 0
#endif  // HAL_UART_NO_RX

#if (HAL_UART_TX_DMA_MASK != 0)
static UInt32 halUart_TxFillDmaBuffer(UInt8 uart, UInt32 fillLevel)
{
    UInt32 dmaIndex = halUart_UartTxToDmaIndex(uart);
    hal_DmaChannel_t dmaChannel = halUart_DmaTxChannel[dmaIndex];
    hal_DmaPointer_t writePtr;
    UInt32 i;
    Int16 data;
    UInt16 bufferoffset;

    GP_ASSERT_DEV_INT(fillLevel <= HAL_UART_TX_BUFFER_SIZE);
    HAL_DISABLE_GLOBAL_INT();

    writePtr = halUart_DmaTxWritePtr[dmaIndex];

    for (i = 0U; i < fillLevel; i++) {
        data = hal_UartHandleGetTxData(uart);
        if (data != -1) {
            bufferoffset = (writePtr.offset + i) % HAL_UART_TX_BUFFER_SIZE;
            halUart_DmaTxBuffer[dmaIndex][bufferoffset] = (UInt8)data;
        } else {
            // No more data from upper layer. Break at this moment
            break;
        }
    }

    writePtr.offset += i;
    if (writePtr.offset >= HAL_UART_TX_BUFFER_SIZE) {
        writePtr.wrap = !writePtr.wrap;
        writePtr.offset = (writePtr.offset % HAL_UART_TX_BUFFER_SIZE);
    }

    halUart_DmaTxWritePtr[dmaIndex] = writePtr;
    hal_DmaUpdatePointers(dmaChannel, writePtr);

    HAL_ENABLE_GLOBAL_INT();
    return i;
}


static void halUart_TxCbAlmostDmaComplete(hal_DmaChannel_t channel)
{
    UInt8 uart;
    UInt32 bytesfilled= 0;
    UInt32 availableSpace = 0;
    UInt32 dmaIndex;
    hal_DmaPointer_t rdptr, wrptr;

    for (uart = 0U; uart < HAL_UART_NR_OF_UARTS; uart++)
    {
        if (HAL_UART_TX_USE_DMA(uart) && halUart_DmaTxChannel[halUart_UartTxToDmaIndex(uart)] == channel)
        {

            // calculate available buffer space:
            // uc_wr_ptr , dma_rd_ptr
            // available space  =
            // if uc_wr_ptr.wrap == dma_rd_ptr.wrap
            //    = (bufsize - wr_ptr.offset) + (rd_ptr.offset)
            // else
            //    = (rd_ptr.offset - wr_ptr.offset)
            // e.g.,
            // buffer is empty if wr_ptr.offset == rd_ptr.offset && wr_ptr.wrap == rd_ptr.wrap
            // buffer is full if wr_ptr.offset == rd_ptr.offset && wr_ptr.wrap != rd_ptr.wrap
            // e.g., buffer is full
            // available space = (rd_ptr.offset - wr_ptr.offset) = 0
            // e.g., buffer is empty
            // available space =  (bufsize - wr_ptr.offset) + rd_ptr.offset = bufsize
            dmaIndex = halUart_UartTxToDmaIndex(uart);
            rdptr = hal_DmaGetInternalPointer(channel);
            wrptr = halUart_DmaTxWritePtr[dmaIndex];

            if (rdptr.wrap == wrptr.wrap)
            {
              availableSpace = (HAL_UART_TX_BUFFER_SIZE - wrptr.offset) + rdptr.offset;
            }
            else
            {
              availableSpace =  rdptr.offset - wrptr.offset;
            }

            bytesfilled = halUart_TxFillDmaBuffer(uart, availableSpace);
            if (bytesfilled < availableSpace)
            {
              // no more data left in com buffer, mask the almost complete interrupt
              // the almost complete interrupt will be enabled back when gpCom has more data to transmit
              hal_DmaEnableAlmostCompleteInterruptMask(channel, false);

            }
            break;
        }
    }

    if (uart == HAL_UART_NR_OF_UARTS)
    {
        GP_ASSERT_DEV_INT(false);
    }
}

static void halUart_TxDisableDma(UInt8 uart)
{
    GP_ASSERT_DEV_INT(HAL_UART_TX_USE_DMA(uart));

    UInt32 dmaIndex = halUart_UartTxToDmaIndex(uart);
    hal_DmaResult_t result = hal_DmaStop(halUart_DmaTxChannel[dmaIndex]);
    GP_ASSERT_SYSTEM(result == HAL_DMA_RESULT_SUCCESS);
}

static void halUart_TxEnableDma(UInt8 uart)
{
    hal_DmaDescriptor_t dmaDesc;
    hal_DmaResult_t result;
    UInt32 dmaIndex;

    GP_ASSERT_DEV_INT(HAL_UART_TX_USE_DMA(uart));
    dmaIndex = halUart_UartTxToDmaIndex(uart);
    MEMSET(&halUart_DmaTxWritePtr[dmaIndex],0,sizeof(hal_DmaPointer_t));

    MEMSET(&dmaDesc, 0, sizeof(dmaDesc));
    dmaDesc.channel = halUart_DmaTxChannel[dmaIndex];
    dmaDesc.cbAlmostComplete = halUart_TxCbAlmostDmaComplete;
    dmaDesc.cbComplete = NULL;
    dmaDesc.wordMode = GP_WB_ENUM_DMA_WORD_MODE_BYTE;
    dmaDesc.circBufSel = GP_WB_ENUM_CIRCULAR_BUFFER_SRC_BUFFER;
    dmaDesc.srcAddr = (UInt32)halUart_DmaTxBuffer[dmaIndex];
    dmaDesc.srcAddrInRam = true;
    dmaDesc.bufferSize = HAL_UART_TX_BUFFER_SIZE;
    dmaDesc.threshold = HAL_UART_TX_DMA_THRESHOLD;
    dmaDesc.bufCompleteIntMode = GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_ERROR_MODE;

    switch (uart)
    {
        case 0:
            dmaDesc.destAddr = GP_WB_UART_0_TX_DATA_0_ADDRESS;
            dmaDesc.dmaTriggerSelect = GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_0_TX_NOT_FULL;
            break;
        case 1:
            dmaDesc.destAddr = GP_WB_UART_1_TX_DATA_0_ADDRESS;
            dmaDesc.dmaTriggerSelect = GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_1_TX_NOT_FULL;
            break;
        default:
            GP_ASSERT_DEV_INT(false);
            break;
    }

    /* Disabling interrupts till dma complete interrupt is disabled
    otherwise it will get stuck in interrupt handling */
    HAL_DISABLE_GLOBAL_INT();
    result = hal_DmaStart(&dmaDesc);
    GP_ASSERT_SYSTEM(result == HAL_DMA_RESULT_SUCCESS);

    // disabling buffer complete interrupts since we use the almost
    // complete interrupt to re-fill buffer
    // while flushing buffer we check the unmasked buffer complete
    // interrupt to make sure all data is emptied
    hal_DmaEnableCompleteInterruptMask(dmaDesc.channel, false);
    HAL_ENABLE_GLOBAL_INT();
}
#endif

static void halUart_HandleIntTxData(UInt8 uart)
{
    Int16 dataToTx;

    dataToTx = hal_UartHandleGetTxData(uart);

    if (dataToTx < 0)
    {
        // no more data to send
        halUart_SetIntCtrlMaskUartTxNotFull(uart, false);
    }
    else
    {
        switch(uart)
        {
            case 0: GP_WB_WRITE_UART_0_TX_DATA_0(dataToTx); break;
            case 1: GP_WB_WRITE_UART_1_TX_DATA_0(dataToTx); break;
            default: GP_ASSERT_DEV_INT(false); break;
        }
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void hal_InitUart(void)
{

#if (HAL_UART_RX_DMA_MASK != 0) && !defined(HAL_UART_NO_RX)
    MEMSET(halUart_DmaRxChannel,HAL_DMA_CHANNEL_INVALID,sizeof(halUart_DmaRxChannel));
#endif
#if (HAL_UART_TX_DMA_MASK != 0)
    MEMSET(halUart_DmaTxChannel,HAL_DMA_CHANNEL_INVALID,sizeof(halUart_DmaTxChannel));
#endif

    halUart_TxEnabledMask = 0;

#ifndef HAL_UART_NO_RX
    halUart_RxEnabledMask = 0;
#endif
}
void hal_UartStart(hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbGetTxData, UInt16 symbolPeriod, UInt16 flags, UInt8 uart)
{
    UInt8 stopbits = (flags >> 2) & 0x03;
    UInt8 parity   = (flags >> 0) & 0x03;
    UInt8 databits = (flags >> 4) & 0x0F;
    UInt32 UartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);

    GP_ASSERT_SYSTEM(uart < HAL_UART_NR_OF_UARTS);

#ifdef GP_BSP_UART_INIT
    if (uart == 0)
    {
        GP_BSP_UART_INIT();
    }
#endif

#if (HAL_UART_RX_DMA_MASK != 0) && !defined(HAL_UART_NO_RX)
    if (HAL_UART_RX_USE_DMA(uart))
    {
        if (halUart_DmaRxChannel[halUart_UartRxToDmaIndex(uart)]==HAL_DMA_CHANNEL_INVALID)
        {
            halUart_DmaRxChannel[halUart_UartRxToDmaIndex(uart)] = hal_DmaClaim();
        }
    }
#endif

#if (HAL_UART_TX_DMA_MASK != 0)
    if (HAL_UART_TX_USE_DMA(uart))
    {
        if (halUart_DmaTxChannel[halUart_UartTxToDmaIndex(uart)]==HAL_DMA_CHANNEL_INVALID)
        {
            halUart_DmaTxChannel[halUart_UartTxToDmaIndex(uart)] = hal_DmaClaim();
        }
    }
#endif

    hal_UartStoreRx(uart, cbRx);
    hal_UartStoreGetTxData(uart, cbGetTxData);
    hal_cbUartOneShotEndOfTx = (hal_cbUartEot_t) NULL;

    GP_WB_WRITE_UART_BAUD_RATE(UartBaseAddress, symbolPeriod);

    //we only support up to one byte for now.
    GP_ASSERT_DEV_EXT(databits <= 8);
    //GP_LOG_SYSTEM_PRINTF("bits: %i",2, databits);
    //GP_LOG_SYSTEM_PRINTF("parity: %i",2, parity);
    //GP_LOG_SYSTEM_PRINTF("stop: %i",2, stopbits);

    GP_WB_WRITE_UART_DATA_BITS(UartBaseAddress, databits-1);
    GP_WB_WRITE_UART_PARITY(UartBaseAddress, parity);
    GP_WB_WRITE_UART_STOP_BITS(UartBaseAddress, stopbits-1);

    hal_UartEnable(uart);
}
void hal_UartSetClockDivider(UInt8 uart, UInt16 value)
{
    UInt32 UartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);
    GP_WB_WRITE_UART_BAUD_RATE(UartBaseAddress, value);
}
UInt16 hal_UartGetClockDivider(UInt8 uart)
{
    UInt32 UartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);
    return GP_WB_READ_UART_BAUD_RATE(UartBaseAddress);
}


void hal_UartComStart( hal_cbUartRx_t cbUartRx, hal_cbUartGetTxData_t cbUartGetTxData, UInt8 uart)
{
    hal_UartStart(cbUartRx , cbUartGetTxData ,
#if defined(GP_BSP_UART_COM2_BAUDRATE)
        (uart == GP_BSP_UART_COM1) ? HAL_UART_COM_SYMBOL_PERIOD : HAL_UART_COM2_SYMBOL_PERIOD,
#else
        HAL_UART_COM_SYMBOL_PERIOD,
#endif
        (HAL_UART_OPT_8_BITS_PER_CHAR | HAL_UART_OPT_NO_PARITY | HAL_UART_OPT_ONE_STOP_BIT), uart);
}
void hal_UartSComStart( hal_cbUartRx_t cbUartRx, hal_cbUartGetTxData_t cbUartGetTxData)
{
    hal_UartStart(cbUartRx , cbUartGetTxData , HAL_UART_SCOM_SYMBOL_PERIOD, \
                   (HAL_UART_OPT_8_BITS_PER_CHAR | HAL_UART_OPT_NO_PARITY | HAL_UART_OPT_ONE_STOP_BIT), 0);
}

void hal_UartComStop(UInt8 uart)
{
    /* this function is used to deinitialize before handover between stage2 bootloader and application */
    hal_UartDisable(uart);

    hal_UartStoreRx(uart, NULL);
    hal_UartStoreGetTxData(uart, NULL);
    hal_cbUartOneShotEndOfTx = (hal_cbUartEot_t) NULL;

    /* can't call hal_Dma Release -- not implemented */
}

void hal_UartDisable(UInt8 uart)
{
    halUart_SetIntCtrlMaskUartInterrupt(uart, false);

    switch (uart) {
        case 0: NVIC_DisableIRQ(UART0_IRQn); break;
        case 1: NVIC_DisableIRQ(UART1_IRQn); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }

    //Flush remaing bytes
    hal_UartComFlush(uart);
    hal_UartRxComFlush(uart);

    //Disconnect pins and mapping
    halUart_SetIntCtrlMaskUartTxNotFull(uart, false);
    halUart_SetUartTxGpioEnabled(uart, false);
#if (HAL_UART_TX_DMA_MASK != 0)
    if (HAL_UART_TX_USE_DMA(uart) && BIT_TST(halUart_TxEnabledMask, uart))
    {
      halUart_TxDisableDma(uart);
    }
#endif
    BIT_CLR(halUart_TxEnabledMask, uart);

#ifndef HAL_UART_NO_RX
#if HAL_UART_RX_DMA_MASK != 0
    if (HAL_UART_RX_USE_DMA(uart) && BIT_TST(halUart_RxEnabledMask, uart))
    {
        halUart_RxDisableDma(uart);
    }
#endif

    halUart_SetIntCtrlMaskUartRxNotEmpty(uart, false);
    halUart_SetUartRxGpioEnabled(uart, false);

    GP_WB_WRITE_UART_RX_ENABLE(UART_BASE_ADDR_FROM_NR(uart), false);

    BIT_CLR(halUart_RxEnabledMask, uart);
#endif
}

void hal_UartEnable(UInt8 uart)
{
#ifndef HAL_UART_NO_RX
    Bool rxDefined = false;
    Bool useRxDma = false;
#endif
    Bool txDefined = false;
    UInt32 UartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);
    Bool useTxDma = false;


    GP_ASSERT_DEV_INT(uart < HAL_UART_NR_OF_UARTS);

    if (HAL_UART_TX_USE_DMA(uart))
    {
        useTxDma = true;
    }
#ifndef HAL_UART_NO_RX
    if (HAL_UART_RX_USE_DMA(uart))
    {
        useRxDma = true;
    }
#endif
    GP_WB_UART_CLR_RX_OVERRUN_INTERRUPT(UartBaseAddress);
    GP_WB_UART_CLR_RX_PARITY_ERROR_INTERRUPT(UartBaseAddress);
    GP_WB_UART_CLR_RX_FRAMING_ERROR_INTERRUPT(UartBaseAddress);
    (void) GP_WB_READ_UART_RX_DATA_0(UartBaseAddress);

    /* UART TX */
    switch (uart)
    {
        case 0: txDefined = GP_BSP_UART0_TX_DEFINED(); break;
        case 1: txDefined = GP_BSP_UART1_TX_DEFINED(); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }

    //Put UART block in reset during configuration
    halUart_SetStandbyResetEpiUart(uart, true);

    if (hal_cbUartGetTxData[uart] == NULL || !txDefined)
    {
        /* no TX will be used, why explicitly disable-> in case of reinit? */
        halUart_SetUartTxGpioEnabled(uart, false);
        halUart_SetIntCtrlMaskUartTxNotFull(uart, false);
        BIT_CLR(halUart_TxEnabledMask, uart);
    }
    else
    {
        /* TX int enable: will be enabled when data needs to be sent */

        /* configure GPIO */
        switch(uart)
        {
            case 0: GP_BSP_UART0_TX_GPIO_CFG(); break;
            case 1: GP_BSP_UART1_TX_GPIO_CFG(); break;
            default: GP_ASSERT_DEV_INT(false); break;
        }
        halUart_SetUartTxGpioEnabled(uart, true);
        BIT_SET(halUart_TxEnabledMask, uart);
    }

#if (HAL_UART_TX_DMA_MASK != 0)
    if ((hal_cbUartGetTxData[uart] != NULL) && txDefined && useTxDma)
    {
      halUart_TxEnableDma(uart);
    }
#endif

#ifndef HAL_UART_NO_RX
    /* UART RX */

    switch (uart)
    {
        case 0: rxDefined = GP_BSP_UART0_RX_DEFINED(); break;
        case 1: rxDefined = GP_BSP_UART1_RX_DEFINED(); break;
        default: GP_ASSERT_DEV_INT(false); break;
    }

    //Rx interrupt should be enabled if there is a handler, tx not yet.
    if ((hal_cbUartRx[uart] != NULL) && rxDefined)
    {

        if (useRxDma)
        {
#if HAL_UART_RX_DMA_MASK != 0
            halUart_RxEnableDma(uart);
#endif
        }

        switch (uart) {
            case 0: GP_BSP_UART0_RX_GPIO_CFG(); break;
            case 1: GP_BSP_UART1_RX_GPIO_CFG(); break;
            default: GP_ASSERT_DEV_INT(false); break;
        }
        if (!useRxDma)
        {
            halUart_SetIntCtrlMaskUartRxNotEmpty(uart, true);
        }
        halUart_SetUartRxGpioEnabled(uart, true);
        BIT_SET(halUart_RxEnabledMask, uart);
        //Enable when reset of block is lifted
    }
    else
#endif  /* HAL_UART_NO_RX */
    {

        GP_WB_WRITE_UART_RX_ENABLE(UartBaseAddress, false);

        halUart_SetIntCtrlMaskUartRxNotEmpty(uart, false);
        if (uart == 1)
        {
        }
        else
        {
            halUart_SetUartRxGpioEnabled(uart, false);
        }
    }

    /* Enable UART block mask */
    halUart_SetIntCtrlMaskUartInterrupt(uart, true);

    if ( ((hal_cbUartGetTxData[uart] != NULL) && txDefined && !useTxDma)
#ifndef HAL_UART_NO_RX
       ||((hal_cbUartRx[uart] != NULL)        && rxDefined && !useRxDma)
#endif //HAL_UART_NO_RX
       )
    {
        switch(uart) {
            case 0: NVIC_EnableIRQ(UART0_IRQn); break;
            case 1: NVIC_EnableIRQ(UART1_IRQn); break;
            default: GP_ASSERT_DEV_INT(false); break;
        }
    }

    //Release reset of block
    halUart_SetStandbyResetEpiUart(uart, false);

#ifndef HAL_UART_NO_RX
    if ((hal_cbUartRx[uart] != NULL) && rxDefined)
    {
        //Enable Rx when UART block is out of reset
        GP_WB_WRITE_UART_RX_ENABLE(UartBaseAddress, true);
    }
#endif  /* HAL_UART_NO_RX */
}

Bool hal_UartTxEnabled(UInt8 uart)
{
    GP_ASSERT_DEV_INT(uart < HAL_UART_NR_OF_UARTS);
    return (uart < HAL_UART_NR_OF_UARTS) && BIT_TST(halUart_TxEnabledMask, uart);
}

Bool hal_UartRxEnabled(UInt8 uart)
{
#ifndef HAL_UART_NO_RX
    GP_ASSERT_DEV_INT(uart < HAL_UART_NR_OF_UARTS);
    return (uart < HAL_UART_NR_OF_UARTS) && BIT_TST(halUart_RxEnabledMask, uart);
#else
    return false;
#endif //HAL_UART_NO_RX
}

void hal_UartTxNewData(UInt8 uart)
{
#if (HAL_UART_TX_DMA_MASK != 0)
    if (HAL_UART_TX_USE_DMA(uart))
    {
        UInt32 dmaIndex = halUart_UartTxToDmaIndex(uart);
        hal_DmaChannel_t dmaChannel = halUart_DmaTxChannel[dmaIndex];

        //enable almost complete interrupts to re-fill data if data in tx buffer falls below almost complete threshold
        hal_DmaEnableAlmostCompleteInterruptMask(dmaChannel, true);
    }
    else
    {
        halUart_SetIntCtrlMaskUartTxNotFull(uart, true);
    }
#else
    halUart_SetIntCtrlMaskUartTxNotFull(uart, true);
#endif
}

void hal_UartWaitEndOfTransmission(UInt8 uart)
{
    GP_ASSERT_DEV_INT(uart < HAL_UART_NR_OF_UARTS);
    if (uart < HAL_UART_NR_OF_UARTS)
    {
        UInt32 UartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);
        while (!GP_WB_READ_UART_UNMASKED_TX_NOT_BUSY_INTERRUPT(UartBaseAddress)) { }
    }
}

void hal_UartRegisterOneShotEndOfTxCb(hal_cbUartEot_t cbEot)
{
    GP_WB_WRITE_INT_CTRL_MASK_UART_0_TX_NOT_BUSY_INTERRUPT(false);
    hal_cbUartOneShotEndOfTx = cbEot;
    GP_WB_WRITE_INT_CTRL_MASK_UART_0_TX_NOT_BUSY_INTERRUPT(true);
}


void hal_UartComFlush(UInt8 uart)
{
    Bool UartBusy = false;
    UInt32 UartBaseAddress = UART_BASE_ADDR_FROM_NR(uart);

    HAL_DISABLE_GLOBAL_INT();
    if(!HAL_UART_TX_USE_DMA(uart))
    {
      do
      {
          if (GP_WB_READ_UART_UNMASKED_TX_NOT_FULL_INTERRUPT(UartBaseAddress))
          {
              halUart_HandleIntTxData(uart);
          }
          //Mask will be disabled by halUart_HandleIntTxData() when all data is sent
          switch(uart) {
              case 0: UartBusy = GP_WB_READ_INT_CTRL_MASK_UART_0_TX_NOT_FULL_INTERRUPT(); break;
              case 1: UartBusy = GP_WB_READ_INT_CTRL_MASK_UART_1_TX_NOT_FULL_INTERRUPT(); break;
              default: GP_ASSERT_DEV_INT(false); break;
          }
      } while (UartBusy);

      hal_UartWaitEndOfTransmission(uart);

      switch(uart) {
          case 0: NVIC_ClearPendingIRQ(UART0_IRQn); break;
          case 1: NVIC_ClearPendingIRQ(UART1_IRQn); break;
          default: GP_ASSERT_DEV_INT(false); break;
      }
    }
    else
    {
#if (HAL_UART_TX_DMA_MASK!=0)
      UInt32 dmaIndex = halUart_UartTxToDmaIndex(uart);
      hal_DmaChannel_t dmaChannel = halUart_DmaTxChannel[dmaIndex];

      //wait till all data is emptied from COM buffer
      while(hal_DmaIsAlmostCompleteInterruptMaskEnabled(dmaChannel))
      {
        halUart_TxCbAlmostDmaComplete(dmaChannel);
      }

      //wait till uart tx buffer is empty
      GP_DO_WHILE_TIMEOUT_ASSERT(!hal_DmaGetUnmaskedBufferCompleteInterrupt(dmaChannel), 10000);

      hal_UartWaitEndOfTransmission(uart);
#endif
    }
    HAL_ENABLE_GLOBAL_INT();

}

void hal_UartRxComFlush(UInt8 uart)
{
#if !defined(HAL_UART_NO_RX) && (HAL_UART_RX_DMA_MASK != 0)
    GP_ASSERT_DEV_INT(uart < HAL_UART_NR_OF_UARTS);
    if (HAL_UART_RX_USE_DMA(uart) && hal_UartRxEnabled(uart))
    {
        halUart_RxHandleDma(uart);
    }
#endif
}

/* Called before going to sleep to stop DMA. */
void hal_UartBeforeSleep(void)
{
    UInt8 uart;

    for (uart = 0; uart < HAL_UART_NR_OF_UARTS; uart++)
    {
#if (HAL_UART_RX_DMA_MASK != 0) && !defined(HAL_UART_NO_RX)
        if (HAL_UART_RX_USE_DMA(uart))
        {
            if (hal_UartRxEnabled(uart))
            {
                halUart_RxDisableDma(uart);
                halUart_RxHandleDma(uart);
            }
        }

#endif

#if (HAL_UART_TX_DMA_MASK != 0)
        if (HAL_UART_TX_USE_DMA(uart))
        {
            if (hal_UartTxEnabled(uart))
            {
                hal_UartComFlush(uart);
                halUart_TxDisableDma(uart);
            }
        }
#endif

    }
}

/* Called after waking up from sleep to restart DMA. */
void hal_UartAfterSleep(void)
{
    UInt8 uart;

    for (uart = 0; uart < HAL_UART_NR_OF_UARTS; uart++)
    {

#if (HAL_UART_RX_DMA_MASK != 0) && !defined(HAL_UART_NO_RX)
        if (HAL_UART_RX_USE_DMA(uart))
        {
            if (hal_UartRxEnabled(uart))
            {
                halUart_RxEnableDma(uart);
            }
        }
#endif

#if (HAL_UART_TX_DMA_MASK != 0)
        if (HAL_UART_TX_USE_DMA(uart))
        {
            if (hal_UartTxEnabled(uart))
            {
                halUart_TxEnableDma(uart);
            }
        }
#endif
    }
}

/*****************************************************************************
 *                    Global interrupt handlers
 *****************************************************************************/

void uart0_handler_impl(void)
{
#ifndef HAL_UART_NO_RX
    if (!HAL_UART_RX_USE_DMA(0) && GP_WB_READ_INT_CTRL_MASKED_UART_0_RX_NOT_EMPTY_INTERRUPT())
    {
        halUart_HandleIntRx(0);
    }
    else
#endif
    if(GP_WB_READ_INT_CTRL_MASKED_UART_0_TX_NOT_FULL_INTERRUPT())
    {
        halUart_HandleIntTxData(0);
    }
    else if(GP_WB_READ_INT_CTRL_MASKED_UART_0_TX_NOT_BUSY_INTERRUPT())
    {
        if ((hal_cbUartEot_t)NULL != hal_cbUartOneShotEndOfTx)
        {
            hal_cbUartOneShotEndOfTx();
        }
        GP_WB_WRITE_INT_CTRL_MASK_UART_0_TX_NOT_BUSY_INTERRUPT(false);
        hal_cbUartOneShotEndOfTx = (hal_cbUartEot_t)NULL;
    }
    else
    {
        GP_ASSERT_DEV_INT(false);
    }
}

void uart1_handler_impl(void)
{
#ifndef HAL_UART_NO_RX
    if (!HAL_UART_RX_USE_DMA(1) && GP_WB_READ_INT_CTRL_MASKED_UART_1_RX_NOT_EMPTY_INTERRUPT())
    {
        halUart_HandleIntRx(1);
    }
    else
#endif
    if(GP_WB_READ_INT_CTRL_MASKED_UART_1_TX_NOT_FULL_INTERRUPT())
    {
        halUart_HandleIntTxData(1);
    }
    else if(GP_WB_READ_INT_CTRL_MASKED_UART_1_TX_NOT_BUSY_INTERRUPT())
    {
        GP_WB_WRITE_INT_CTRL_MASK_UART_1_TX_NOT_BUSY_INTERRUPT(false);
    }
    else
    {
        GP_ASSERT_DEV_INT(false);
    }
}

