/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *  @file       UARTCC26XX.h
 *
 *  @brief      UART driver implementation for a CC26XX UART controller
 *
 * # Driver include #
 *  The UART header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/UART.h>
 *  #include <ti/drivers/uart/UARTCC26XX.h>
 *  @endcode
 *
 *  Refer to @ref UART.h for a complete description of APIs.
 *
 * # Overview #
 * The general UART API should used in application code, i.e. UART_open()
 * is used instead of UARTCC26XX_open(). The board file will define the device
 * specific config, and casting in the general API will ensure that the correct
 * device specific functions are called.
 * This is also reflected in the example code in [Use Cases](@ref USE_CASES).
 * This driver does not support polling modes for UART_read() and UART_write().
 * Text mode (e.g., read returns on receiving a newline character) is also
 * not supported by this driver.  If polling mode or text mode are needed,
 * the UARTCC26X2 driver can be used instead.
 *
 * # General Behavior #
 * Before using the UART in CC26XX:
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
 *   - The regular callback is run in a SWI context.
 *
 * # Error handling #
 * ## Read errors ##
 * If an error occurs during read operation:
 *   - All bytes received up until an error occurs will be returned, with the
 *     error signaled in the ::UARTCC26XX_Object.status field. The RX is then turned off
 *     and all bytes will be ignored until a new read is issued. Note that only
 *     the read is cancelled when the error occurs. If a write was active
 *     while the RX error occurred, it will complete.
 *   - If a RX break error occurs, an extra 0 byte will also be returned by the
 *     UART_read().
 *   .
 *
 * ## Write errors##
 * If a timeout occurs during a write, an UART_ERROR will be returned and the
 * UART_Object.status will be set to ::UART_TIMED_OUT. All bytes that are not
 * transmitted, will be flushed.
 * If flow control is not enabled, the ::UARTCC26XX_Object.writeTimeout should
 * be kept at default value, BIOS_WAIT_FOREVER. The write call will return after
 * all bytes are transmitted.
 * If flow control is enabled, the timeout should be set by the application in
 * order to recover if the receiver gets stuck.
 *
 * ## General timeout ##
 * A timeout value can only be specified for reads and writes in ::UART_MODE_BLOCKING.
 * If a timeout occurs during a read when in ::UART_MODE_BLOCKING, the number of bytes received will be
 * returned and the UART_Object.status will be set to ::UART_TIMED_OUT.
 * After a read timeout, RX will remain on, but device is allowed to enter standby.
 * For more details see [Power Management](@ref POWER_MANAGEMENT) chapter below.
 *
 * In ::UART_MODE_CALLBACK there is no timeout and the application must call
 * UART_readCancel() or UART_writeCancel() to abort the operation.
 *
 * @note A new read or write will reset the UART_Object.status to UART_OK.
 *       Caution must be taken when doing parallel reads and writes.
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
 * # Power Management @anchor POWER_MANAGEMENT #
 * The TI-RTOS power management framework will try to put the device into the most
 * power efficient mode whenever possible. Please see the technical reference
 * manual for further details on each power mode.
 *
 * The UARTCC26XX driver is setting a power constraint during operation to keep
 * the device out of standby. When the operation has finished, the power
 * constraint is released.
 * The following statements are valid:
 *   - After UART_open(): the device is still allowed to enter standby.
 *   - During UART_read(): the device cannot enter standby.
 *   - After an RX error (overrun, break, parity, framing): device is allowed to enter standby.
 *   - After a successful UART_read():
 *     The device is allowed to enter standby, but RX remains on.
 *       - _Note_: Device might enter standby while a byte is being
 *         received if UART_read() is not called again after a successful
 *         read. This could result in corrupt data being received.
 *       - _Note_: Application thread should typically either issue another read after
 *         UART_read() completes successfully, or call
 *         UART_readCancel() to disable RX and thus assuring that no data
 *         is received while entering standby.
 *   - After UART_read() times out in ::UART_MODE_BLOCKING:
 *     The device is allowed to enter standby, but RX remains on.
 *       - _Note_: Device might enter standby while a byte is being
 *         received if UART_read() is not called again after a timeout.
 *         This could result in corrupt data being received.
 *       - _Note_: Application thread should typically either issue another read after
 *         UART_read() times out to continue reception.
 *       .
 *   - During UART_write(): the device cannot enter standby.
 *   - After UART_write() succeeds: the device can enter standby.
 *   - If UART_writeCancel() is called: the device can enter standby.
 *   - After write timeout: the device can enter standby.
 *
 * # Flow Control #
 * To enable Flow Control, the RTS and CTS pins must be assigned in the
 * ::UARTCC26XX_HWAttrsV2:
 *  @code
 *  const UARTCC26XX_HWAttrsV2 uartCC26xxHWAttrs[] = {
 *      {
 *          .baseAddr    = UART0_BASE,
 *          .powerMngrId = PERIPH_UART0,
 *          .intNum      = INT_UART0,
 *          .intPriority = ~0,
 *          .swiPriority = 0,
 *          .txPin       = CONFIG_UART_TX,
 *          .rxPin       = CONFIG_UART_RX,
 *          .ctsPin      = CONFIG_UART_CTS,
 *          .rtsPin      = CONFIG_UART_RTS
 *          .ringBufPtr  = uartCC26XXRingBuffer[0],
 *          .ringBufSize = sizeof(uartCC26XXRingBuffer[0]),
 *          .txIntFifoThr= UARTCC26XX_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr= UARTCC26XX_FIFO_THRESHOLD_4_8
 *      }
 *  };
 *  @endcode
 *
 * If the RTS and CTS pins are set to ::GPIO_INVALID_INDEX, the flow control is
 * disabled. An example is shown in the ::UARTCC26XX_HWAttrsV2 description.
 *
 * # Supported Functions #
 * | Generic API function | API function             | Description                                       |
 * |----------------------|--------------------------|------------------------
 * | UART_init()          | UARTCC26XX_init()        | Initialize UART driver                            |
 * | UART_open()          | UARTCC26XX_open()        | Initialize UART HW and set system dependencies    |
 * | UART_close()         | UARTCC26XX_close()       | Disable UART HW and release system dependencies   |
 * | UART_control()       | UARTCC26XX_control()     | Configure an already opened UART handle           |
 * | UART_read()          | UARTCC26XX_read()        | Start read from UART                              |
 * | UART_readCancel()    | UARTCC26XX_readCancel()  | Cancel ongoing read from UART                     |
 * | UART_write()         | UARTCC26XX_write()       | Start write to UART                               |
 * | UART_writeCancel()   | UARTCC26XX_writeCancel() | Cancel ongoing write to UART                      |
 *
 *  @note All calls should go through the generic API
 *
 *  # Not Supported Functionality #
 *  The CC26XX UART driver currently does not support:
 *    - ::UART_ECHO_ON
 *    - ::UART_DATA_TEXT
 *    - UART_readPolling()
 *    - UART_writePolling()
 *
 *  For polling mode or text processing, the UARTCC26X2 driver can be used.
 *
 * # Use Cases @anchor USE_CASES #
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
 *  UART_control(handle, UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE, NULL);
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
 * ## Receive Continously in ::UART_MODE_CALLBACK @anchor USE_CASE_CB #
 *  This case will configure the UART to receive and transmit continously in
 *  ::UART_MODE_CALLBACK, 16 bytes at the time and transmit them back via UART TX.
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
 *         for(size_t i = 0; i < size; i++)
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
 *  # Instrumentation #
 *  The UART driver interface produces log statements if instrumentation is
 *  enabled.
 *
 *  Diagnostics Mask | Log details |
 *  ---------------- | ----------- |
 *  Diags_USER1      | basic UART operations performed |
 *  Diags_USER2      | detailed UART operations performed |
 *
 *  ============================================================================
 */

#ifndef ti_drivers_uart_UARTCC26XX__include
#define ti_drivers_uart_UARTCC26XX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/UART.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/utils/RingBuf.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/uart.h)

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup UART_STATUS
 *  UARTCC26XX_STATUS_* macros are command codes only defined in the
 *  UARTCC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTCC26XX.h>
 *  @endcode
 *  @{
 */

/* Add UARTCC26XX_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup UART_CMD
 *  UARTCC26XX_CMD_* macros are command codes only defined in the UARTCC26XX.h
 *  driver implementation and need to:
 *  @code
 *  #include <ti/drivers/uart/UARTCC26XX.h>
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
#define UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE    (UART_CMD_RESERVED + 0)

/*!
 * @brief Command used by UART_control to disable partial return
 *
 * Disabling this command returns the UARTCC26XX to the default blocking
 * behavior where UART_read blocks until all data bytes were received. With
 * this comand @b arg is @a don't @a care and it returns UART_STATUS_SUCCESS.
 */
#define UARTCC26XX_CMD_RETURN_PARTIAL_DISABLE   (UART_CMD_RESERVED + 1)

/*!
 * @brief Command used by UART_control to flush the RX FIFO
 *
 * This control command flushes any contents in the RX FIFO. With this command
 * @b arg is @a don't @a care and it returns UART_STATUS_SUCCESS.
 */
#define UARTCC26XX_CMD_RX_FIFO_FLUSH            (UART_CMD_RESERVED + 2)
/** @}*/

/*! Size of the TX and RX FIFOs is 32 items */
#define UARTCC26XX_FIFO_SIZE 32

/*!
 *  @brief    UART TX/RX interrupt FIFO threshold select
 *
 *  Defined FIFO thresholds for generation of both TX interrupt and RX interrupt.
 *  The default value (UARTCC26XX_FIFO_THRESHOLD_DEFAULT) is defined for backward compatibility handling.
 */
typedef enum {
    UARTCC26XX_FIFO_THRESHOLD_DEFAULT = 0, /*!< Default value forces FIFO
                                                threshold of 1/8 for TX
                                                interrupt and 4/8 for RX
                                                interrupt */
    UARTCC26XX_FIFO_THRESHOLD_1_8,         /*!< FIFO threshold of 1/8 full */
    UARTCC26XX_FIFO_THRESHOLD_2_8,         /*!< FIFO threshold of 2/8 full */
    UARTCC26XX_FIFO_THRESHOLD_4_8,         /*!< FIFO threshold of 4/8 full */
    UARTCC26XX_FIFO_THRESHOLD_6_8,         /*!< FIFO threshold of 6/8 full */
    UARTCC26XX_FIFO_THRESHOLD_7_8          /*!< FIFO threshold of 7/8 full */
} UARTCC26XX_FifoThreshold;

/* BACKWARDS COMPATIBILITY */
#define UARTCC26XX_RETURN_PARTIAL_ENABLE    UARTCC26XX_CMD_RETURN_PARTIAL_ENABLE
#define UARTCC26XX_RETURN_PARTIAL_DISABLE   UARTCC26XX_CMD_RETURN_PARTIAL_DISABLE
/* END BACKWARDS COMPATIBILITY */

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
typedef void (*UARTCC26XX_ErrorCallback) (UART_Handle handle, uint32_t error);

/* UART function table pointer */
extern const UART_FxnTable UARTCC26XX_fxnTable;

/*!
 *  @brief      UARTCC26XX Hardware attributes
 *
 *  These fields, with the exception of intPriority, txIntFifoThr and
 *  rxIntFifoThr, are used by driverlib APIs and therefore must be populated
 *  by driverlib macro definitions.
 *  For CC26xxWare these definitions are found in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
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
 *
 *  A sample structure is shown below:
 *  @code
 *  const UARTCC26XX_HWAttrsV2 uartCC26xxHWAttrs[] = {
 *      {
 *          .baseAddr    = UART0_BASE,
 *          .powerMngrId = PERIPH_UART0,
 *          .intNum      = INT_UART0,
 *          .intPriority = ~0,
 *          .swiPriority = 0,
 *          .txPin       = CONFIG_UART_TX,
 *          .rxPin       = CONFIG_UART_RX,
 *          .ctsPin      = GPIO_INVALID_INDEX,
 *          .rtsPin      = GPIO_INVALID_INDEX,
 *          .ringBufPtr  = uartCC26XXRingBuffer[0],
 *          .ringBufSize = sizeof(uartCC26XXRingBuffer[0]),
 *          .txIntFifoThr= UARTCC26XX_FIFO_THRESHOLD_1_8,
 *          .rxIntFifoThr= UARTCC26XX_FIFO_THRESHOLD_4_8
 *      }
 *  };
 *  @endcode
 *
 *  The .ctsPin and .rtsPin must be assigned to enable flow control.
 */
typedef struct {
    uint32_t     baseAddr;    /*!< UART Peripheral's base address */
    uint32_t     powerMngrId; /*!< UART Peripheral's power manager ID */
    int          intNum;      /*!< UART Peripheral's interrupt vector */
    /*! @brief UART Peripheral's interrupt priority.

        The CC26xx uses three of the priority bits, meaning ~0 has the same effect as (7 << 5).

        (7 << 5) will apply the lowest priority.

        (1 << 5) will apply the highest priority.

        Setting the priority to 0 is not supported by this driver.

        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency interrupts, thus invalidating the critical sections in this driver.
     */
    uint8_t      intPriority;
    /*! @brief SPI SWI priority.
        The higher the number, the higher the priority.
        The minimum is 0 and the maximum is 15 by default.
        The maximum can be reduced to save RAM by adding or modifying Swi.numPriorities in the kernel configuration file.
    */
    uint32_t                 swiPriority;
    uint8_t                  txPin;        /*!< UART TX pin */
    uint8_t                  rxPin;        /*!< UART RX pin */
    uint8_t                  ctsPin;       /*!< UART CTS pin */
    uint8_t                  rtsPin;       /*!< UART RTS pin */
    unsigned char           *ringBufPtr;   /*!< Pointer to an application ring buffer */
    size_t                   ringBufSize;  /*!< Size of ringBufPtr */
    UARTCC26XX_FifoThreshold txIntFifoThr; /*!< UART TX interrupt FIFO threshold select */
    UARTCC26XX_FifoThreshold rxIntFifoThr; /*!< UART RX interrupt FIFO threshold select */
    /*! Application error function to be called on receive errors */
    UARTCC26XX_ErrorCallback errorFxn;
} UARTCC26XX_HWAttrsV2;

/*!
 *  @brief    UART status
 *
 *  The UART Status is used to flag the different Receive Errors.
 */
typedef enum {
    UART_TIMED_OUT     = 0x10,                 /*!< UART timed out */
    UART_PARITY_ERROR  = UART_RXERROR_PARITY,  /*!< UART Parity error */
    UART_BRAKE_ERROR   = UART_RXERROR_BREAK,   /*!< UART Break error */
    UART_OVERRUN_ERROR = UART_RXERROR_OVERRUN, /*!< UART overrun error */
    UART_FRAMING_ERROR = UART_RXERROR_FRAMING, /*!< UART Framing error */
    UART_OK            = 0x0                   /*!< UART OK */
} UART_Status;

/*!
 *  @brief      UARTCC26XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /* UART control variables */
    bool                opened;         /*!< Has the obj been opened */
    UART_Mode           readMode;       /*!< Mode for all read calls */
    UART_Mode           writeMode;      /*!< Mode for all write calls */
    unsigned int        readTimeout;    /*!< Timeout for read semaphore in BLOCKING mode*/
    unsigned int        writeTimeout;   /*!< Timeout for write semaphore in BLOCKING mode*/
    UART_Callback       readCallback;   /*!< Pointer to read callback */
    UART_Callback       writeCallback;  /*!< Pointer to write callback */
    UART_ReturnMode     readReturnMode; /*!< Receive return mode */
    UART_DataMode       readDataMode;   /*!< Type of data being read */
    UART_DataMode       writeDataMode;  /*!< Type of data being written */
    /*! @brief Baud rate for CC26xx UART
     *
     *  The CC26xx driver supports baud rates up to 3Mbaud.
     *  However, when receiving more than 32 bytes back-to-back the baud
     *  rate is limited to approx. 2Mbaud.
     *  The throughput is also dependent on the user application.
     */
    uint32_t            baudRate;
    UART_LEN            dataLength;     /*!< Data length for UART */
    UART_STOP           stopBits;       /*!< Stop bits for UART */
    UART_PAR            parityType;     /*!< Parity bit type for UART */
    UART_Status         status;         /*!< Status variable */

    /* UART write variables */
    const void         *writeBuf;       /*!< Buffer data pointer */
    size_t              writeCount;     /*!< Number of Chars sent */
    size_t              writeSize;      /*!< Chars remaining in buffer */
    bool                writeCR;        /*!< Write a return character */

    /* UART receive variables */
    bool                  readRetPartial;     /*!< Return partial RX data if timeout occurs */
    void                  *readBuf;           /*!< Buffer data pointer */
    size_t                readCount;          /*!< Number of Chars read */
    size_t                readSize;           /*!< Chars remaining in buffer */
    RingBuf_Object        ringBuffer;         /*!< local circular buffer object */

    /*! UART post-notification function pointer */
    void             *uartPostFxn;
    /*! UART post-notification object */
    Power_NotifyObj  uartPostObj;

    /* UART SYS/BIOS objects */
    HwiP_Struct hwi; /*!< Hwi object */
    SwiP_Struct           swi;                /*!< Swi object */
    SemaphoreP_Struct     writeSem;           /*!< UART write semaphore*/
    SemaphoreP_Struct     readSem;            /*!< UART read semaphore */
    ClockP_Struct         txFifoEmptyClk;     /*!< UART TX FIFO empty clock */

    bool uartRxPowerConstraint;
    bool uartTxPowerConstraint;
} UARTCC26XX_Object, *UARTCC26XX_Handle;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_uart_UARTCC26XX__include */
