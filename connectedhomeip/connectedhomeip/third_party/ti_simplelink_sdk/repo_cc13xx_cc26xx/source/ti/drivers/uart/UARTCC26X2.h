/*
 * Copyright (c) 2017-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       UARTCC26X2.h
 *
 *  @brief      UART driver implementation for a CC26X2 UART controller
 *
 * # Driver include #
 *  The UART header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/UART.h>
 *  #include <ti/drivers/uart/UARTCC26X2.h>
 *  @endcode
 *
 *  Refer to @ref UART.h for a complete description of APIs.
 *
 * # Overview #
 * The general UART API should used in application code, i.e. UART_open()
 * is used instead of UARTCC26X2_open(). The board file will define the device
 * specific config, and casting in the general API will ensure that the correct
 * device specific functions are called.
 * This is also reflected in the example code in [Use Cases](@ref USE_CASES).
 * This driver supports polling modes for UART_read() and UART_write().  Text
 * mode (e.g., UART_read() returns on receiving a newline character) is also
 * supported by this driver.  If polling mode and text mode are not needed,
 * the UARTCC26XX driver can be used instead, which will reduce code size.
 *
 * # General Behavior #
 * Before using the UART in CC26X2:
 *   - The UART driver is initialized by calling UART_init().
 *   - The UART HW is configured and flags system dependencies (e.g. IOs,
 *     power, etc.) by calling UART_open().
 *   - The RX and TX can operate independently of each other.
 *   .
 * The following is true for receive operation:
 *   - RX is enabled by calling UART_read().
 *   - All received bytes are ignored after UART_open() is called, until
 *     the first UART_read().
 *   - If an RX error occur, RX is turned off and all bytes received before the
 *     error occured are returned.
 *   - After a successful read, RX remains on. UART_read() must be called
 *     again before FIFO goes full in order to avoid overflow. It is safe to
 *     call another UART_read() from the read callback, See
 *     [Receive Continously] (@ref USE_CASE_CB) use case below.
 *   - If a read times out (in ::UART_MODE_BLOCKING mode), RX will remain on.
 *     UART_read() must be called again before FIFO goes full in order to avoid overflow.
 *   - The UART_read() supports partial return, that can be used if the
 *     receive size is unknown. See [Use Cases](@ref USE_CASES) below.
 *   - The RingBuf serves as an extension of the FIFO. If data is received when
 *     UART_read() is not called, data will be stored in the RingBuf. The
 *     functionality of the RingBuf has been tested with a size of 32. This size
 *     can be changed to suit the application.
 *   .
 * The following apply for transmit operation:
 *   - TX is enabled by calling UART_write().
 *   - If the UART_write() succeeds, the TX is disabled.
 *   .
 * If UART is no longer needed by application:
 *   - Release system dependencies for UART by calling UART_close().
 *   .
 * If the UART is configured in ::UART_MODE_CALLBACK mode:
 *   - The error handling callback is run in a HWI context.
 *   - The application's callback is run in a SWI context.
 *
 * # Error handling #
 * ## Read errors ##
 * If an error occurs during read operation:
 *   - All bytes received up until an error occurs will be returned, with the
 *     error signaled in the ::UARTCC26X2_Object.status field. The RX is then turned off
 *     and all bytes will be ignored until a new read is issued. Note that only
 *     the read is cancelled when the error occurs. If a write was active
 *     while the RX error occurred, it will complete.  A new UART_read() will reset
 *     the ::UARTCC26X2_Object.status field to 0.
 *   - If a RX break error occurs, an extra 0 byte will also be returned by the
 *     UART_read().
 *   .
 *
 * ## General timeout ##
 * A timeout value can only be specified for reads and writes in ::UART_MODE_BLOCKING.
 * If a timeout occurs during a read when in ::UART_MODE_BLOCKING, the number of bytes
 * received will be returned.
 * After a read timeout, RX will be turned off and the device allowed to enter standby.
 * For more details see [Power Management](@ref POWER_MANAGEMENT) chapter below.
 *
 * In ::UART_MODE_CALLBACK there is no timeout and the application must call
 * UART_readCancel() or UART_writeCancel() to abort the operation.
 *
 * ## Closing driver during an ongoing read/write ##
 * It's safe to call UART_close() during an ongoing UART_read() and/or UART_write(),
 * this will cancel the ongoing RX/TX immediately.
 *
 * The RX callback is alwyas called when you call UART_close() if there's an
 * ongoing read.
 * Note that if UART_close() is called during an ongoing read, the size provided
 * in the RX callback function is 0 if < 16 bytes were received before calling UART_close().
 * This is because 16 bytes is the RX watermark that triggers the ISR
 * to copy bytes from the internal UART FIFO to the software RX buffer.
 *
 * The TX callback is always called when you call UART_close() if there's an
 * ongoing write. The driver does not wait until a byte is transmitted correctly,
 * so if UART_close() is called in the middle of sending a byte,
 * this byte will be corrupted.
 *
 * # Power Management @anchor UARTCC26X2_POWER_MANAGEMENT #
 * The TI-RTOS power management framework will try to put the device into the most
 * power efficient mode whenever possible. Please see the technical reference
 * manual for further details on each power mode.
 *
 * The UARTCC26X2 driver sets a power constraint during operation to keep
 * the device out of standby. When the operation has finished, the power
 * constraint is released.
 * The following statements are valid:
 *   - After UART_open(): the device is not allowed to enter standby.
 *   - During UART_read(): the device cannot enter standby.
 *   - After an RX error (overrun, break, parity, framing): RX is disabled and the device
 *     is allowed to enter standby.
 *   - After a successful UART_read():
 *     The device is not allowed to enter standby and RX remains on.
 *       - _Note_: Application thread should typically either issue another read after
 *         UART_read() completes successfully, or call either
 *         UART_readCancel() or UART_control(uart, UART_CMD_RXDISABLE, 0),
 *         to disable RX and allow the device to enter standby.
 *   - After UART_read() times out in ::UART_MODE_BLOCKING:
 *     The device is allowed to enter standby and RX is turned off.  To prevent
 *     RX from being disabled and disallowing standby on a read timeout or read
 *     error, the application can call UART_control(uart, UART_CMD_RXENABLE, 0).
 *   - During UART_write(): the device cannot enter standby.
 *   - After UART_write() succeeds: the device can enter standby.
 *   - If UART_writeCancel() is called: the device can enter standby.
 *   - After write timeout: the device can enter standby.
 *
 * # Flow Control #
 * To enable Flow Control, the RTS and CTS pins must be assigned in the
 * ::UARTCC26X2_HWAttrs and flowControl must be set to UARTCC26X2_FLOWCTRL_HARDWARE:
 *  @code
 *  const UARTCC26X2_HWAttrs uartCC26X2HWAttrs[] = {
 *      {
 *          .baseAddr    = UART0_BASE,
 *          .powerMngrId = PERIPH_UART0,
 *          .intNum      = INT_UART0,
 *          .intPriority = ~0,
 *          .swiPriority = 0,
 *          .flowControl = UARTCC26X2_FLOWCTRL_HARDWARE,
 *          .txPin       = CONFIG_UART_TX,
 *          .rxPin       = CONFIG_UART_RX,
 *          .ctsPin      = CONFIG_UART_CTS,
 *          .rtsPin      = CONFIG_UART_RTS
 *          .ringBufPtr  = uartCC26X2RingBuffer[0],
 *          .ringBufSize = sizeof(uartCC26X2RingBuffer[0]),
 *          .txIntFifoThr= UARTCC26X2_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr= UARTCC26X2_FIFO_THRESHOLD_4_8
 *      }
 *  };
 *  @endcode
 *
 * If the RTS and CTS pins are set to ::GPIO_INVALID_INDEX, or flowControl is set
 * to UARTCC26X2_FLOWCONTROL_NONE, the flow control is disabled.
 * An example is shown in the ::UARTCC26X2_HWAttrs description.
 *
 * # Supported Functions #
 * | Generic API function | API function             | Description                                       |
 * |----------------------|--------------------------|------------------------
 * | UART_init()          | UARTCC26X2_init()        | Initialize UART driver                            |
 * | UART_open()          | UARTCC26X2_open()        | Initialize UART HW and set system dependencies    |
 * | UART_close()         | UARTCC26X2_close()       | Disable UART HW and release system dependencies   |
 * | UART_control()       | UARTCC26X2_control()     | Configure an already opened UART handle           |
 * | UART_read()          | UARTCC26X2_read()        | Start read from UART                              |
 * | UART_readCancel()    | UARTCC26X2_readCancel()  | Cancel ongoing read from UART                     |
 * | UART_readPolling()   | UARTCC26X2_readPolling() | Polling read from UART                            |
 * | UART_write()         | UARTCC26X2_write()       | Start write to UART                               |
 * | UART_writeCancel()   | UARTCC26X2_writeCancel() | Cancel ongoing write to UART                      |
 * | UART_writePolling()  | UARTCC26X2_writePolling()| Polling write to UART                      |
 *
 *  @note All calls should go through the generic API
 *
 * # Use Cases @anchor UARTCC26X2_USE_CASES #
 * ## Basic Receive #
 *  Receive 100 bytes over UART in ::UART_MODE_BLOCKING.
 *  @code
 *  UART_Handle handle;
 *  UART_Params params;
 *  uint8_t rxBuf[100];         // Receive buffer
 *  uint32_t timeoutUs = 5000;  // 5ms timeout, default timeout is no timeout (BIOS_WAIT_FOREVER)
 *
 *  // Init UART and specify non-default parameters
 *  UART_Params_init(&params);
 *  params.baudRate      = 9600;
 *  params.writeDataMode = UART_DATA_BINARY;
 *  params.readTimeout   = timeoutUs / ClockP_getSystemTickPeriod(); // Default tick period is 10us
 *
 *  // Open the UART and do the read
 *  handle = UART_open(CONFIG_UART, &params);
 *  int rxBytes = UART_read(handle, rxBuf, 100);
 *  @endcode
 *
 * ## Receive with Return Partial #
 *  This use case will read in ::UART_MODE_BLOCKING until the wanted amount of bytes is
 *  received or until a started reception is inactive for a 32-bit period.
 *  This UART_read() call can also be used when unknown amount of bytes shall
 *  be read. Note: The partial return is also possible in ::UART_MODE_CALLBACK mode.
 *  @code
 *  UART_Handle handle;
 *  UART_Params params;
 *  uint8_t rxBuf[100];      // Receive buffer
 *
 *  // Init UART and specify non-default parameters
 *  UART_Params_init(&params);
 *  params.baudRate      = 9600;
 *  params.writeDataMode = UART_DATA_BINARY;
 *
 *  // Open the UART and initiate the partial read
 *  handle = UART_open(CONFIG_UART, &params);
 *  // Enable RETURN_PARTIAL
 *  UART_control(handle, UARTCC26X2_CMD_RETURN_PARTIAL_ENABLE, NULL);
 *  // Begin read
 *  int rxBytes = UART_read(handle, rxBuf, 100));
 *  @endcode
 *
 * ## Basic Transmit #
 *  This case will configure the UART to send the data in txBuf in
 *  BLOCKING_MODE.
 *  @code
 *  UART_Handle handle;
 *  UART_Params params;
 *  uint8_t txBuf[] = "Hello World";    // Transmit buffer
 *
 *  // Init UART and specify non-default parameters
 *  UART_Params_init(&params);
 *  params.baudRate      = 9600;
 *  params.writeDataMode = UART_DATA_BINARY;
 *
 *  // Open the UART and do the write
 *  handle = UART_open(CONFIG_UART, &params);
 *  UART_write(handle, txBuf, sizeof(txBuf));
 *  @endcode
 *
 * ## Receive Continously in ::UART_MODE_CALLBACK @anchor UARTCC26X2_USE_CASE_CB #
 *  This case will configure the UART to receive and transmit continously in
 *  ::UART_MODE_CALLBACK, and transmit them back via UART TX.
 *  Note that UART_Params.readTimeout is not in use when using ::UART_MODE_CALLBACK mode.
 *  @code
 *  #define MAX_NUM_RX_BYTES    1000   // Maximum RX bytes to receive in one go
 *  #define MAX_NUM_TX_BYTES    1000   // Maximum TX bytes to send in one go
 *
 *  uint32_t wantedRxBytes;            // Number of bytes received so far
 *  uint8_t rxBuf[MAX_NUM_RX_BYTES];   // Receive buffer
 *  uint8_t txBuf[MAX_NUM_TX_BYTES];   // Transmit buffer
 *
 *  // Read callback function
 *  static void readCallback(UART_Handle handle, void *rxBuf, size_t size)
 *  {
 *      // Make sure we received all expected bytes
 *      if (size == wantedRxBytes) {
 *          // Copy bytes from RX buffer to TX buffer
 *         for (size_t i = 0; i < size; i++)
 *             txBuf[i] = ((uint8_t*)rxBuf)[i];
 *
 *         // Echo the bytes received back to transmitter
 *         UART_write(handle, txBuf, size);
 *
 *         // Start another read, with size the same as it was during first call to
 *         // UART_read()
 *         UART_read(handle, rxBuf, wantedRxBytes);
 *      }
 *      else {
 *          // Handle error or call to UART_readCancel()
 *      }
 *  }
 *
 *  // Write callback function
 *  static void writeCallback(UART_Handle handle, void *rxBuf, size_t size)
 *  {
 *      // Do nothing
 *  }
 *
 *  static void taskFxn(uintptr_t a0, uintptr_t a1)
 *  {
 *      UART_Handle handle;
 *      UART_Params params;
 *
 *      // Init UART
 *      UART_init();
 *
 *      // Specify non-default parameters
 *      UART_Params_init(&params);
 *      params.baudRate      = 9600;
 *      params.writeMode     = UART_MODE_CALLBACK;
 *      params.writeDataMode = UART_DATA_BINARY;
 *      params.writeCallback = writeCallback;
 *      params.readMode      = UART_MODE_CALLBACK;
 *      params.readDataMode  = UART_DATA_BINARY;
 *      params.readCallback  = readCallback;
 *
 *      // Open the UART and initiate the first read
 *      handle = UART_open(CONFIG_UART, &params);
 *      wantedRxBytes = 16;
 *      int rxBytes = UART_read(handle, rxBuf, wantedRxBytes);
 *
 *      while(true); // Wait forever
 *  }
 *  @endcode
 *
 *  # Baud Rate #
 *  The CC26xx driver supports baud rates up to 3Mbaud.
 *  However, when receiving more than 32 bytes back-to-back the baud
 *  rate is limited to approximately 2Mbaud.
 *  The throughput is also dependent on the user application.
 *
 *  # Stack requirements #
 *  There are no additional stack requirements for calling UART_read() within
 *  its own callback.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_uart_UARTCC26X2__include
#define ti_drivers_uart_UARTCC26X2__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/utils/RingBuf.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief No hardware flow control
 */
#define UARTCC26X2_FLOWCTRL_NONE 0

/*!
 * @brief Hardware flow control
 */
#define UARTCC26X2_FLOWCTRL_HARDWARE 1

/**
 *  @addtogroup UART_STATUS
 *  UARTCC26X2_STATUS_* macros are command codes only defined in the
 *  UARTCC26X2.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTCC26X2.h>
 *  @endcode
 *  @{
 */

/* Add UARTCC26X2_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup UART_CMD
 *  UARTCC26X2_CMD_* macros are command codes only defined in the
 *  UARTCC26X2.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTCC26X2.h>
 *  @endcode
 *  @{
 */

/*!
 * @brief Command used by UART_control to enable partial return
 *
 * Enabling this command allows UART_read to return partial data if data
 * reception is inactive for a given 32-bit period.  With this command @b arg
 * is @a don't @a care and it returns UART_STATUS_SUCCESS.
 */
#define UARTCC26X2_CMD_RETURN_PARTIAL_ENABLE    (UART_CMD_RESERVED + 0)

/*!
 * @brief Command used by UART_control to disable partial return
 *
 * Disabling this command returns the UARTCC26X2 to the default blocking
 * behavior where UART_read blocks until all data bytes were received. With
 * this comand @b arg is @a don't @a care and it returns UART_STATUS_SUCCESS.
 */
#define UARTCC26X2_CMD_RETURN_PARTIAL_DISABLE   (UART_CMD_RESERVED + 1)

/*!
 * @brief Command used by UART_control to flush the RX FIFO
 *
 * This control command flushes any contents in the RX FIFO. With this command
 * @b arg is @a don't @a care and it returns UART_STATUS_SUCCESS.
 */
#define UARTCC26X2_CMD_RX_FIFO_FLUSH            (UART_CMD_RESERVED + 2)


/** @}*/

/*! Size of the TX and RX FIFOs is 32 items */
#define UARTCC26X2_FIFO_SIZE 32

/*!
 *  @brief    UART TX/RX interrupt FIFO threshold select
 *
 *  Defined FIFO thresholds for generation of both TX interrupt and RX
 *  interrupt.  The default value (UARTCC26X2_FIFO_THRESHOLD_DEFAULT) is
 *  defined for backwards compatibility handling.  If the RX and TX FIFO
 &  thresholds are not set in the HwAttrs, or are set to
 *  UARTCC26X2_FIFO_THRESHOLD_DEFAULT, the RX interrupt FIFO threshold is
 *  set to 4/8 full, and the TX interrupt FIFO threshold is set to 1/8
 *  full.
 */
typedef enum {
    UARTCC26X2_FIFO_THRESHOLD_DEFAULT = 0, /*!< Use default FIFO threshold */
    UARTCC26X2_FIFO_THRESHOLD_1_8,         /*!< FIFO threshold of 1/8 full */
    UARTCC26X2_FIFO_THRESHOLD_2_8,         /*!< FIFO threshold of 2/8 full */
    UARTCC26X2_FIFO_THRESHOLD_4_8,         /*!< FIFO threshold of 4/8 full */
    UARTCC26X2_FIFO_THRESHOLD_6_8,         /*!< FIFO threshold of 6/8 full */
    UARTCC26X2_FIFO_THRESHOLD_7_8          /*!< FIFO threshold of 7/8 full */
} UARTCC26X2_FifoThreshold;

/*!
 *  @brief      The definition of an optional callback function used by the
 *              UART driver to notify the application when a receive error
 *              (FIFO overrun, parity error, etc) occurs.
 *
 *  @param      UART_Handle             UART_Handle
 *
 *  @param      error                   The current value of the receive
 *                                      status register.
 */
typedef void (*UARTCC26X2_ErrorCallback) (UART_Handle handle, uint32_t error);

/* UART function table pointer */
extern const UART_FxnTable UARTCC26X2_fxnTable;

/*!
 *  @brief      UARTCC26X2 Hardware attributes
 *
 *  The fields, baseAddr and intNum are used by driverlib
 *  APIs and therefore must be populated by
 *  driverlib macro definitions. These definitions are found under the
 *  device family in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *      - driverlib/uart.h
 *
 *  intPriority is the UART peripheral's interrupt priority, as defined by the
 *  underlying OS.  It is passed unmodified to the underlying OS's interrupt
 *  handler creation code, so you need to refer to the OS documentation
 *  for usage.  For example, for SYS/BIOS applications, refer to the
 *  ti.sysbios.family.arm.m3.Hwi documentation for SYS/BIOS usage of
 *  interrupt priorities.  If the driver uses the ti.dpl interface
 *  instead of making OS calls directly, then the HwiP port handles the
 *  interrupt priority in an OS specific way.  In the case of the SYS/BIOS
 *  port, intPriority is passed unmodified to Hwi_create().
 *  The CC26xx uses three of the priority bits, meaning ~0 has the same
 *  effect as (7 << 5).
 *
 *        (7 << 5) will apply the lowest priority.
 *        (1 << 5) will apply the highest priority.
 *
 *  Setting the priority to 0 is not supported by this driver.  HWI's with
 *  priority 0 ignore the HWI dispatcher to support zero-latency interrupts,
 *  thus invalidating the critical sections in this driver.
 *
 *  A sample structure is shown below:
 *  @code
 *  unsigned char uartCC26X2RingBuffer[2][32];
 *
 *  const UARTCC26X2_HWAttrs uartCC26X2HWAttrs[] = {
 *      {
 *          .baseAddr     = UARTA0_BASE,
 *          .intNum       = INT_UART0_COMB,
 *          .intPriority  = (~0),
 *          .swiPriority  = 0,
 *          .ringBufPtr   = uartCC26X2RingBuffer[0],
 *          .ringBufSize  = sizeof(uartCC26X2RingBuffer[0]),
 *          .flowControl  = UARTCC26X2_FLOWCTRL_NONE,
 *          .rxPin        = IOID_2,
 *          .txPin        = IOID_3,
 *          .ctsPin       = GPIO_INVALID_INDEX,
 *          .rtsPin       = GPIO_INVALID_INDEX,
 *          .txIntFifoThr = UARTCC26X2_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr = UARTCC26X2_FIFO_THRESHOLD_4_8,
 *          .errorFxn     = NULL
 *      },
 *      {
 *          .baseAddr     = UART1_BASE,
 *          .intNum       = INT_UART1_COMB,
 *          .intPriority  = (~0),
 *          .swiPriority  = 0,
 *          .ringBufPtr   = uartCC26X2RingBuffer[1],
 *          .ringBufSize  = sizeof(uartCC26X2RingBuffer[1]),
 *          .flowControl  = UARTCC26X2_FLOWCTRL_NONE,
 *          .rxPin        = GPIO_INVALID_INDEX,
 *          .txPin        = GPIO_INVALID_INDEX,
 *          .ctsPin       = GPIO_INVALID_INDEX,
 *          .rtsPin       = GPIO_INVALID_INDEX,
 *          .txIntFifoThr = UARTCC26X2_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr = UARTCC26X2_FIFO_THRESHOLD_4_8,
 *          .errorFxn     = NULL
 *      },
 *  };
 *  @endcode
 *
 *  The .ctsPin and .rtsPin must be assigned to enable flow control.
 */
typedef struct {
    /*! UART Peripheral's base address */
    uint32_t        baseAddr;
    /*! UART Peripheral's interrupt vector */
    int             intNum;
    /*! UART Peripheral's interrupt priority */
    uint8_t         intPriority;
    /*!
     *  @brief Swi priority.
     *  The higher the number, the higher the priority.  The minimum
     *  priority is 0 and the maximum is defined by the underlying OS.
     */
    uint32_t        swiPriority;
    /*! Hardware flow control setting */
    uint32_t        flowControl;
    /*! Pointer to an application ring buffer */
    unsigned char  *ringBufPtr;
    /*! Size of ringBufPtr */
    size_t          ringBufSize;
    /*! UART RX pin assignment */
    uint8_t         rxPin;
    /*! UART TX pin assignment */
    uint8_t         txPin;
    /*! UART clear to send (CTS) pin assignment */
    uint8_t         ctsPin;
    /*! UART request to send (RTS) pin assignment */
    uint8_t         rtsPin;
    /*! UART TX interrupt FIFO threshold select */
    UARTCC26X2_FifoThreshold txIntFifoThr;
    /*! UART RX interrupt FIFO threshold select */
    UARTCC26X2_FifoThreshold rxIntFifoThr;
    /*! Application error function to be called on receive errors */
    UARTCC26X2_ErrorCallback errorFxn;
} UARTCC26X2_HWAttrs;

/*!
 *  @brief      UARTCC26X2 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /* UART state variable */
    struct {
        bool             opened:1;         /* Has the obj been opened */
        UART_Mode        readMode:1;       /* Mode for all read calls */
        UART_Mode        writeMode:1;      /* Mode for all write calls */
        UART_DataMode    readDataMode:1;   /* Type of data being read */
        UART_DataMode    writeDataMode:1;  /* Type of data being written */
        UART_ReturnMode  readReturnMode:1; /* Receive return mode */
        UART_Echo        readEcho:1;       /* Echo received data back */
        /*
         * Flag to determine if a timeout has occurred when the user called
         * UART_read(). This flag is set by the timeoutClk clock object.
         */
        bool             bufTimeout:1;
        /*
         * Flag to determine when an ISR needs to perform a callback; in both
         * UART_MODE_BLOCKING or UART_MODE_CALLBACK
         */
        bool             callCallback:1;
        /*
         * Flag to determine if the ISR is in control draining the ring buffer
         * when in UART_MODE_CALLBACK
         */
        bool             drainByISR:1;
        /* Keep track of RX enabled state set by app with UART_control() */
        bool             ctrlRxEnabled:1;
        /* Flag to keep the state of the read Power constraints */
        bool             rxEnabled:1;
        /* Flag to keep the state of the write Power constraints */
        bool             txEnabled:1;
    } state;

    HwiP_Struct          hwi;              /* Hwi object for interrupts */
    SwiP_Struct          readSwi;          /* Swi for read callbacks */
    SwiP_Struct          writeSwi;         /* Swi for write callbacks */
    ClockP_Struct        timeoutClk;       /* Clock object to for timeouts */
    uint32_t             baudRate;         /* Baud rate for UART */
    UART_LEN             dataLength;       /* Data length for UART */
    UART_STOP            stopBits;         /* Stop bits for UART */
    UART_PAR             parityType;       /* Parity bit type for UART */
    uint32_t             status;           /* RX status */

    /* UART read variables */
    RingBuf_Object       ringBuffer;       /* local circular buffer object */
    unsigned char       *readBuf;          /* Buffer data pointer */
    size_t               readSize;         /* Desired number of bytes to read */
    size_t               readCount;        /* Number of bytes left to read */
    SemaphoreP_Struct    readSem;          /* UART read semaphore */
    unsigned int         readTimeout;      /* Timeout for read semaphore */
    UART_Callback        readCallback;     /* Pointer to read callback */
    bool                 readRetPartial;   /* Return partial RX data if timeout occurs */

    /* UART write variables */
    const unsigned char *writeBuf;         /* Buffer data pointer */
    size_t               writeSize;        /* Desired number of bytes to write*/
    size_t               writeCount;       /* Number of bytes left to write */
    SemaphoreP_Struct    writeSem;         /* UART write semaphore*/
    unsigned int         writeTimeout;     /* Timeout for write semaphore */
    UART_Callback        writeCallback;    /* Pointer to write callback */

    /* For Power management */
    Power_NotifyObj      postNotify;
    unsigned int         powerMgrId;      /* Determined from base address */
} UARTCC26X2_Object, *UARTCC26X2_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_uart_UARTCC26X2__include */
