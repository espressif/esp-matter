/*
 * Copyright (c) 2015-2021 Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file       SPICC26XXDMA.h
 *
 *  @brief      SPI driver implementation for a CC26XX SPI controller using
 *              the UDMA controller.
 *
 * # Driver include #
 *  The SPI header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/SPI.h>
 *  #include <ti/drivers/spi/SPICC26XXDMA.h>
 *  #include <ti/drivers/dma/UDMACC26XX.h>
 *  @endcode
 *
 *  Refer to @ref SPI.h for a complete description of APIs.
 *
 * Note that the user also needs to include the UDMACC26XX.h driver since the
 * SPI uses uDMA in order to improve throughput.
 *
 * # Overview #
 * The general SPI API should be used in application code, i.e. SPI_open()
 * should be used instead of SPICC26XXDMA_open(). The board file will define the device
 * specific config, and casting in the general API will ensure that the correct
 * device specific functions are called.
 * This is also reflected in the example code in [Use Cases](@ref USE_CASES_SPI).
 *
 * # General Behavior #
 * Before using SPI on CC26XX:
 *   - The SPI driver is initialized by calling SPI_init().
 *   - The SPI HW is configured and flags system dependencies (e.g. IOs,
 *     power, etc.) by calling SPI_open().
 *   - The SPI driver makes use of DMA in order to optimize throughput.
 *     This is handled directly by the SPI driver, so the application should never
 *     to make any calls directly to the UDMACC26XX.h driver.
 *
 * The following is true for slave operation:
 *   - RX overrun IRQ, SPI and UDMA modules are enabled by calling SPI_transfer().
 *   - All received bytes are ignored after SPI_open() is called, until
 *     the first SPI_transfer().
 *   - If an RX overrun occur or if SPI_transferCancel() is called, RX overrun IRQ, SPI and UDMA
 *     modules are disabled, TX and RX FIFOs are flushed and all bytes are ignored.
 *   - After a successful transfer, RX overrun IRQ and SPI module remains enabled and UDMA module is disabled.
 *     SPI_transfer() must be called again before RX FIFO goes full in order to
 *     avoid overflow. If the TX buffer is underflowed, zeros will be output.
 *     It is safe to call another SPI_transfer() from the transfer callback,
 *     see [Continuous Slave Transfer] (@ref USE_CASE_CST) use case below.
 *   - The SPI driver supports partial return, that can be used if the
 *     passed to SPI_control(), the transfer will end when chip select is
 *     deasserted. The #SPI_Transaction.status and the #SPI_Transaction.count
 *     will be updated to indicate whether the transfer ended due to a chip
 *     select deassertion and how many bytes were transferred. See
 *     [Slave Mode With Return Partial] (@ref USE_CASE_RP_X2) use case below.
 *
 * @warning The SPI modules on the CC13x0, CC26x0, and CC26x0R2 devices have a
 *     bug which may result in TX data being lost when operating in SPI slave
 *     mode. Please refer to the device errata sheet for full details. The SPI
 *     protocol should therefore include a data integrity check, such as
 *     appending a CRC to the payload to ensure all the data was transmitted
 *     correctly by the SPI slave.
 *
 * @warning This driver does not support queueing multiple SPI transactions.
 *
 * The following apply for master operation:
 *   - SPI and UDMA modules are enabled by calling SPI_transfer().
 *   - If the SPI_transfer() succeeds, SPI module is enabled and UDMA module is disabled.
 *   - If SPI_transferCancel() is called, SPI and UDMA modules are disabled and
 *     TX and RX FIFOs are flushed.
 *   .
 * After SPI operation has ended:
 *   - Release system dependencies for SPI by calling SPI_close().
 *   .
 * The callback function is called in the following context:
 *   - When an error occurs, the callback function is called in a HWI context.
 *   - When no error occurs, the callback function is called in a SWI context.
 *
 * @warning     The application should avoid transmitting data stored in flash via SPI if the application
 *              might switch to the XOSC_HF, the high frequency external oscillator, during this transfer.
 *
 * # Error handling #
 * If an RX overrun occurs during slave operation:
 *   - If a transfer is ongoing, all bytes received up until the error occurs will be returned, with the
 *     error signaled in the #SPI_Transaction.status field. RX overrun IRQ, SPI and UDMA modules are then disabled,
 *     TX and RX FIFOs are flushed and all bytes will be ignored until a new transfer is issued.
 *   - If a transfer is not ongoing, RX overrun IRQ, SPI and UDMA modules are disabled,
 *     TX and RX FIFOs are flushed and all bytes will be ignored until a new transfer is issued.
 *
 * # Timeout #
 * Timeout can occur in #SPI_MODE_BLOCKING, there's no timeout in #SPI_MODE_CALLBACK.
 * When in #SPI_MODE_CALLBACK, the transfer must be cancelled by calling SPI_transferCancel().\n
 * If a timeout happens in either  #SPI_SLAVE or #SPI_MASTER mode,
 * the receive buffer will contain the bytes received up until the timeout occurred.
 * The SPI transaction status will be set to #SPI_TRANSFER_FAILED.
 * The SPI transaction count will be set to the number of bytes sent/received before timeout.
 * The remaining bytes will be flushed from the TX FIFO so that the subsequent transfer
 * can be executed correctly. Note that specifying a timeout prevents the
 * driver from performing a polling transfer when in slave mode.
 *
 * # Power Management #
 * The TI-RTOS power management framework will try to put the device into the most
 * power efficient mode whenever possible. Please see the technical reference
 * manual for further details on each power mode.
 *
 *  The SPICC26XXDMA.h driver is setting a power constraint during transfers to keep
 *  the device out of standby. When the transfer has finished, the power
 *  constraint is released.
 *  The following statements are valid:
 *    - After SPI_open(): the device is still allowed to enter standby.
 *    - In slave mode:
 *        - During SPI_transfer(): the device cannot enter standby, only idle.
 *        - After an RX overflow: device is allowed to enter standby.
 *        - After a successful SPI_transfer(): the device is allowed
 *          to enter standby, but SPI module remains enabled.
 *            - _Note_: In slave mode, the device might enter standby while a byte is being
 *               transferred if SPI_transfer() is not called again after a successful
 *               transfer. This could result in corrupt data being transferred.
 *        - Application thread should typically either issue another transfer after
 *          SPI_transfer() completes successfully, or call
 *          SPI_transferCancel() to disable the SPI module and thus assuring that no data
 *          is received while entering standby.
 *        .
 *    - In master mode:
 *        - During SPI_transfer(): the device cannot enter standby, only idle.
 *        - After SPI_transfer() succeeds: the device can enter standby.
 *        - If SPI_transferCancel() is called: the device can enter standby.
 *
 *  @note The external hardware connected to the SPI might have some pull configured on the
 *        SPI lines. When the SPI is inactive, this might cause leakage on the IO and the
 *        current consumption to increase. The application must configure a pull configuration
 *        that aligns with the external hardware.
 *        See [Ensure low power during inactive periods] (@ref USE_CASE_LPWR) for code example.
 *
 *  # SPI details #
 *  ## Chip Select #
 *  This SPI controller supports a hardware chip select pin. Refer to the
 *  user manual on how this hardware chip select pin behaves in regards
 *  to the SPI frame format.
 *
 *  <table>
 *  <tr>
 *  <th>Chip select type</th>
 *  <th>SPI_MASTER mode</th>
 *  <th>SPI_SLAVE mode</th>
 *  </tr>
 *  <tr>
 *  <td>Hardware chip select</td>
 *  <td>No action is needed by the application to select the peripheral.</td>
 *  <td>See the device documentation on it's chip select requirements.</td>
 *  </tr>
 *  <tr>
 *  <td>Software chip select</td>
 *  <td>The application is responsible to ensure that correct SPI slave is
 *      selected before performing a SPI_transfer().</td>
 *  <td>See the device documentation on it's chip select requirements.</td>
 *  </tr>
 *  </table>
 *
 *  ### Multiple slaves when operating in master mode #
 *  In a scenario where the SPI module is operating in master mode with multiple
 *  SPI slaves, the chip select pin can be reallocated at runtime to select the
 *  appropriate slave device. See [Master Mode With Multiple Slaves](@ref USE_CASE_MMMS) use case below.
 *  This is only relevant when chip select is a hardware chip select. Otherwise the application
 *  can control the chip select pins directly using the PIN driver.
 *
 *  ## Data Frames #
 *
 *  SPI data frames can be any size from 4-bits to 16-bits. If the dataSize in
 *  #SPI_Params is greater that 8-bits, then the SPICC26XXDMA driver
 *  implementation will assume that the #SPI_Transaction txBuf and rxBuf
 *  point to an array of 16-bit uint16_t elements.
 *
 *  dataSize  | buffer element size |
 *  --------  | ------------------- |
 *  4-8 bits  | uint8_t             |
 *  9-16 bits | uint16_t            |
 *
 *  ## Bit Rate ##
 *  When the SPI is configured as SPI slave, the maximum bit rate is 4MHz.
 *
 *  When the SPI is configured as SPI master, the maximum bit rate is 12MHz.
 *
 *
 *  ## UDMA #
 *  ### Interrupts #
 *  The UDMA module generates IRQs on the SPI interrupt vector. This driver automatically
 *  installs a UDMA aware Hwi (interrupt) to service the assigned UDMA channels.
 *
 *  ### Transfer Size Limit #
 *
 *  The UDMA controller only supports data transfers of up to 1024 data frames.
 *  A transfer with more than 1024 frames will be transmitted/received in
 *  multiple 1024 sized portions until all data has been transmitted/received.
 *  A data frame can be 4 to 16 bits in length.
 *
 *  ### Scratch Buffers #
 *  A uint16_t scratch buffer is used to allow SPI_transfers where txBuf or rxBuf
 *  are NULL. Rather than requiring txBuf or rxBuf to have a dummy buffer of size
 *  of the transfer count, a single-word UDMA accessible uint16_t scratch buffer is used.
 *  When rxBuf is NULL, the UDMA will transfer all the received SPI data into the
 *  scratch buffer as a "bit-bucket".
 *  When txBuf is NULL, the scratch buffer is initialized to defaultTxBufValue
 *  so the uDMA will send some known value.
 *  Each SPI driver instance uses its own scratch buffer.
 *
 *  ### TX and RX buffers #
 *  Before SPI_transfer, txBuf should be filled with the outgoing SPI data. These
 *  data are sent out during the transfer, while the incoming data are received
 *  into rxBuf. To save memory space, txBuf and rxBuf can be assigned to the same
 *  buffer location. At the beginning of the transfer, this buffer holds outgoing
 *  data. At the end of the transfer, the outgoing data are overwritten and
 *  the buffer holds the received SPI data.
 *
 *  ## Polling SPI transfers #
 *  When used in blocking mode small SPI transfers are can be done by polling
 *  the peripheral & sending data frame-by-frame. A master device can perform
 *  the transfer immediately and return, but a slave will block until it
 *  receives the number of frames specified in the SPI_Transfer() call.
 *  The minDmaTransferSize field in the hardware attributes is
 *  the threshold; if the transaction count is below the threshold a polling
 *  transfer is performed; otherwise a DMA transfer is done.  This is intended
 *  to reduce the overhead of setting up a DMA transfer to only send a few
 *  data frames.
 *
 *  Notes:
 *  - Specifying a timeout prevents slave devices from using polling transfers.
 *  - Keep in mind that during polling transfers the current task
 *  is still being executed; there is no context switch to another task.

 *
 * # Supported Functions #
 * | Generic API function  | API function                   | Description                                                 |
 * |-----------------------|------------------------------- |-------------------------------------------------------------|
 * | SPI_init()            | SPICC26XXDMA_init()            | Initialize SPI driver                                       |
 * | SPI_open()            | SPICC26XXDMA_open()            | Initialize SPI HW and set system dependencies               |
 * | SPI_close()           | SPICC26XXDMA_close()           | Disable SPI and UDMA HW and release system dependencies     |
 * | SPI_control()         | SPICC26XXDMA_control()         | Configure an already opened SPI handle                      |
 * | SPI_transfer()        | SPICC26XXDMA_transfer()        | Start transfer from SPI                                     |
 * | SPI_transferCancel()  | SPICC26XXDMA_transferCancel()  | Cancel ongoing transfer from SPI                            |
 *
 *  @note All calls should go through the generic API
 *
 *  ## Unsupported Functionality #
 *  The CC26XX SPI driver does not support:
 *    - SPICC26XXDMA_serviceISR()
 *
 * ## Use Cases @anchor USE_CASES_SPI ##
 * ### Basic Slave Mode #
 *  Receive 100 bytes over SPI in #SPI_MODE_BLOCKING.
 *  @code
 *  SPI_Handle handle;
 *  SPI_Params params;
 *  SPI_Transaction transaction;
 *  uint8_t rxBuf[100];     // Receive buffer
 *
 *  // Init SPI and specify non-default parameters
 *  SPI_Params_init(&params);
 *  params.bitRate     = 1000000;
 *  params.frameFormat = SPI_POL1_PHA1;
 *  params.mode        = SPI_SLAVE;
 *
 *  // Configure the transaction
 *  transaction.count = 100;
 *  transaction.txBuf = NULL;
 *  transaction.rxBuf = rxBuf;
 *
 *  // Open the SPI and perform the transfer
 *  handle = SPI_open(CONFIG_SPI, &params);
 *  SPI_transfer(handle, &transaction);
 *  @endcode
 *
 * ### Slave Mode With Return Partial @anchor USE_CASE_RP #
 *  This use case will perform a transfer in #SPI_MODE_BLOCKING until the wanted amount of bytes is
 *  transferred or until chip select is deasserted by the SPI master.
 *  This SPI_transfer() call can be used when unknown amount of bytes shall
 *  be transferred.
 *  Note: The partial return is also possible in #SPI_MODE_CALLBACK mode.
 *  Note: Polling transfers are not available when using return partial mode.
 *  @code
 *  SPI_Handle handle;
 *  SPI_Params params;
 *  SPI_Transaction transaction;
 *  uint8_t rxBuf[100];     // Receive buffer
 *
 *  // Init SPI and specify non-default parameters
 *  SPI_Params_init(&params);
 *  params.bitRate     = 1000000;
 *  params.frameFormat = SPI_POL1_PHA1;
 *  params.mode        = SPI_SLAVE;
 *
 *  // Configure the transaction
 *  transaction.count = 100;
 *  transaction.txBuf = NULL;
 *  transaction.rxBuf = rxBuf;
 *
 *  // Open the SPI and initiate the partial read
 *  handle = SPI_open(CONFIG_SPI, &params);
 *
 *  // Enable RETURN_PARTIAL
 *  SPI_control(handle, SPICC26XXDMA_RETURN_PARTIAL_ENABLE, NULL);
 *
 *  // Begin transfer
 *  SPI_transfer(handle, &transaction);
 *  @endcode
 *
 * ### Continuous Slave Transfer In #SPI_MODE_CALLBACK @anchor USE_CASE_CST #
 *  This use case will configure the SPI driver to transfer continuously in
 *  #SPI_MODE_CALLBACK, 16 bytes at the time and echoing received data after every
 *  16 bytes.
 *  @code
 *  // Callback function
 *  static void transferCallback(SPI_Handle handle, SPI_Transaction *transaction)
 *  {
 *      // Start another transfer
 *      SPI_transfer(handle, transaction);
 *  }
 *
 *  static void taskFxn(uintptr_t a0, uintptr_t a1)
 *  {
 *      SPI_Handle handle;
 *      SPI_Params params;
 *      SPI_Transaction transaction;
 *      uint8_t buf[16];                  // Receive and transmit buffer
 *
 *      // Init SPI and specify non-default parameters
 *      SPI_Params_init(&params);
 *      params.bitRate             = 1000000;
 *      params.frameFormat         = SPI_POL1_PHA1;
 *      params.mode                = SPI_SLAVE;
 *      params.transferMode        = SPI_MODE_CALLBACK;
 *      params.transferCallbackFxn = transferCallback;
 *
 *      // Configure the transaction
 *      transaction.count = 16;
 *      transaction.txBuf = buf;
 *      transaction.rxBuf = buf;
 *
 *      // Open the SPI and initiate the first transfer
 *      handle = SPI_open(CONFIG_SPI, &params);
 *      SPI_transfer(handle, &transaction);
 *
 *      // Wait forever
 *      while(true);
 *  }
 *  @endcode
 *
 * ### Basic Master Mode #
 *  This use case will configure a SPI master to send the data in txBuf while receiving data to rxBuf in
 *  BLOCKING_MODE.
 *  @code
 *  SPI_Handle handle;
 *  SPI_Params params;
 *  SPI_Transaction transaction;
 *  uint8_t txBuf[] = "Hello World";    // Transmit buffer
 *  uint8_t rxBuf[11];                  // Receive buffer
 *
 *  // Init SPI and specify non-default parameters
 *  SPI_Params_init(&params);
 *  params.bitRate     = 1000000;
 *  params.frameFormat = SPI_POL1_PHA1;
 *  params.mode        = SPI_MASTER;
 *
 *  // Configure the transaction
 *  transaction.count = sizeof(txBuf);
 *  transaction.txBuf = txBuf;
 *  transaction.rxBuf = rxBuf;
 *
 *  // Open the SPI and perform the transfer
 *  handle = SPI_open(CONFIG_SPI, &params);
 *  SPI_transfer(handle, &transaction);
 *  @endcode
 *
 *  ### Master Mode With Multiple Slaves @anchor USE_CASE_MMMS #
 *  This use case will configure a SPI master to send data to one slave and then to another in
 *  BLOCKING_MODE. It is assumed that the board file is configured so that the two chip select
 *  pins have a default setting of a high output and that the #SPICC26XXDMA_HWAttrsV1 used points
 *  to one of them since the SPI driver will revert to this default setting when switching the
 *  chip select pin.
 *
 *  @code
 *  // From ti_Drivers_config.c
 *  PIN_Config BoardGpioInitTable[] = {
 *      CONFIG_CSN_0   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH   | PIN_PUSHPULL,   // Ensure SPI slave 0 is not selected
 *      CONFIG_CSN_1   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH   | PIN_PUSHPULL    // Ensure SPI slave 1 is not selected
 *  }
 *
 *  const SPICC26XXDMA_HWAttrsV1 spiCC26XXDMAHWAttrs[CC2650_SPICOUNT] = {
 *  {   // Use SPI0 module with default chip select on CONFIG_CSN_0
 *      .baseAddr = SSI0_BASE,
 *      .intNum = INT_SSI0,
 *      .intPriority = ~0,
 *      .swiPriority = 0,
 *      .defaultTxBufValue = 0,
 *      .powerMngrId = PERIPH_SSI0,
 *      .rxChannelIndex = UDMA_CHAN_SSI0_RX,
 *      .txChannelIndex = UDMA_CHAN_SSI0_TX,
 *      .mosiPin = CONFIG_SPI0_MOSI,
 *      .misoPin = CONFIG_SPI0_MISO,
 *      .clkPin = CONFIG_SPI0_CLK,
 *      .csnPin = CONFIG_CSN_0
 *  }
 *
 *  // From your_application.c
 *  static void taskFxn(uintptr_t a0, uintptr_t a1)
 *  {
 *      SPI_Handle handle;
 *      SPI_Params params;
 *      SPI_Transaction transaction;
 *      PIN_Id csnPin1  = PIN_ID(CONFIG_CSN_1);
 *      uint8_t txBuf[] = "Hello World";    // Transmit buffer
 *
 *      // Init SPI and specify non-default parameters
 *      SPI_Params_init(&params);
 *      params.bitRate     = 1000000;
 *      params.frameFormat = SPI_POL1_PHA1;
 *      params.mode        = SPI_MASTER;
 *
 *      // Configure the transaction
 *      transaction.count = sizeof(txBuf);
 *      transaction.txBuf = txBuf;
 *      transaction.rxBuf = NULL;
 *
 *      // Open the SPI and perform transfer to the first slave
 *      handle = SPI_open(CONFIG_SPI, &params);
 *      SPI_transfer(handle, &transaction);
 *
 *      // Then switch chip select pin and perform transfer to the second slave
 *      SPI_control(handle, SPICC26XXDMA_SET_CSN_PIN, &csnPin1);
 *      SPI_transfer(handle, &transaction);
 *  }
 *  @endcode
 *
 *  ### Ensure low power during inactive periods @anchor USE_CASE_LPWR #
 *  External hardware connected on the SPI, i.e. SPI host/slave, might have configured
 *  a pull on one or more of the SPI lines. Dependent on the hardware, it might conflict
 *  with the pull used for the CC26XX SPI. To avoid increased leakage and ensure the lowest
 *  possible power consumption when the SPI is inactive, the application must configure a
 *  matching pull on the SPI IOs. An example of how this can be done is shown below.
 *
 *  @code
 *  PIN_Handle pinHandle;
 *  SPI_Handle handle;
 *  SPI_Params params;
 *  SPI_Transaction transaction;
 *  uint8_t txBuf[] = "Heartbeat";    // Transmit buffer
 *  uint8_t rxBuf[9];                 // Receive buffer
 *  PIN_Id misoPinId;
 *  uint32_t standbyDurationMs = 100;
 *
 *  // Init SPI and specify non-default parameters
 *  SPI_Params_init(&params);
 *  params.bitRate     = 1000000;
 *  params.frameFormat = SPI_POL1_PHA1;
 *  params.mode        = SPI_MASTER;
 *
 *  // Configure the transaction
 *  transaction.count = sizeof(txBuf);
 *  transaction.txBuf = txBuf;
 *  transaction.rxBuf = rxBuf;
 *
 *  // Open the SPI and perform the transfer
 *  handle = SPI_open(CONFIG_SPI, &params);
 *  // Get pinHandle
 *  pinHandle = ((SPICC26XXDMA_Object *)spiHandle->object)->pinHandle;
 *  // Get miso pin id
 *  misoPinId = ((SPICC26XXDMA_HWAttrsV1 *)spiHandle->hwAttrs)->misoPin;
 *
 *  // Apply low power sleep pull config for MISO
 *  PIN_setConfig(pinHandle, PIN_BM_PULLING, PIN_PULLUP | misoPinId);
 *
 *  // Do forever
 *  while(1) {
 *    // Transfer data
 *    SPI_transfer(handle, &transaction);
 *    // Sleep
 *    Task_sleep(standbyDurationMs*100);
 *  }
 *  @endcode
 *
 *  ### Wake Up On Chip Select Deassertion In Slave Mode Using #SPI_MODE_CALLBACK #
 *  To wake the SPI slave device up on deassertion of the chip select, the chip select
 *  pin must be controled outside of the SPI driver in between SPI transfers.
 *  The example below show how this can be implemented by registering the chip select pin
 *  with the PIN driver and configuring a callback on a falling edge.
 *  In the PIN callback, the chip select pin is released from the PIN driver,
 *  the SPI driver is opened, and a transaction started. During the SPI callback, the SPI
 *  driver is closed again and the chip select pin is reconfigured to trigger a callback on
 *  a falling edge again.
 *
 *  *Note: The SPI master must allow enough time between deasserting the chip select and the
 *  start of the transaction for the SPI slave to wake up and open up the SPI driver.
 *
 *  @code
 *  // Global variables
 *  SPI_Handle spiHandle
 *  SPI_Params spiParams;
 *  SPI_Transaction spiTransaction;
 *  const uint8_t transferSize = 8;
 *  uint8_t txBuf[8];
 *  PIN_Handle pinHandle;
 *  PIN_Config pinConfig[] = {
 *              PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE | CS_PIN_ID,
 *              PIN_TERMINATE  // Terminate list
 *  };
 *
 *  // Chip select callback
 *  static void chipSelectCallback(PIN_Handle handle, PIN_Id pinId)
 *  {
 *      // Release the chip select pin
 *      PIN_remove(handle, pinId);
 *
 *      // Open SPI driver
 *      spiHandle = SPI_open(CONFIG_SPI, &spiParams);
 *
 *      // Issue echo transfer
 *      SPI_transfer(spiHandle, &spiTransaction);
 *  }
 *
 *  // SPI transfer callback
 *  static void transferCallback(SPI_Handle handle, SPI_Transaction *transaction)
 *  {
 *      // Close the SPI driver
 *      SPI_close(handle);
 *
 *      // Add chip select back to the PIN driver
 *      PIN_add(pinHandle, pinConfig[0]);
 *
 *      // Register chip select callback
 *      PIN_registerIntCb(pinHandle, chipSelectCallback);
 *  }
 *
 *  // From your_application.c
 *  static void taskFxn(uintptr_t a0, uintptr_t a1)
 *  {
 *      uint8_t i;
 *      PIN_State   pinState;
 *
 *      // Setup SPI params
 *      SPI_Params_init(&spiParams);
 *      spiParams.bitRate     = 1000000;
 *      spiParams.frameFormat = SPI_POL1_PHA1;
 *      spiParams.mode        = SPI_SLAVE;
 *      spiParams.dataSize    = transferSize;
 *      spiParams.transferMode = SPI_MODE_CALLBACK;
 *      spiParams.transferCallbackFxn = transferCallback;
 *
 *      // Setup SPI transaction
 *      spiTransaction.arg = NULL;
 *      spiTransaction.count = transferSize;
 *      spiTransaction.txBuf = txBuf;
 *      spiTransaction.rxBuf = txBuf;
 *
 *      // First echo message
 *      for (i = 0; i < transferSize; i++) {
 *          txBuf[i] = i;
 *      }
 *
 *      // Open PIN driver and configure chip select pin callback
 *      pinHandle = PIN_open(&pinState, pinConfig);
 *      PIN_registerIntCb(pinHandle, chipSelectCallback);
 *
 *      // Wait forever
 *      while(true);
 *  }
 *  @endcode
 *
 *  # Instrumentation #
 *  The SPI driver interface produces log statements if instrumentation is
 *  enabled.
 *
 *  Diagnostics Mask | Log details |
 *  ---------------- | ----------- |
 *  Diags_USER1      | basic SPI operations performed |
 *  Diags_USER2      | detailed SPI operations performed |
 *
 *  ============================================================================
 */

#ifndef ti_drivers_spi_SPICC26XXDMA__include
#define ti_drivers_spi_SPICC26XXDMA__include

#include <stdint.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup SPI_STATUS
 *  SPICC26XXDMA_STATUS_* macros are command codes only defined in the
 *  SPICC26XXDMA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/sdspi/SPICC26XXDMA.h>
 *  @endcode
 *  @{
 */

/* Add SPICC26XXDMA_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup SPI_CMD
 *  SPICC26XXDMA_CMD_* macros are command codes only defined in the
 *  SPICC26XXDMA.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/sdspi/SPICC26XXDMA.h>
 *  @endcode
 *  @{
 */

/*!
 * @brief Command used by SPI_control to enable partial return
 *
 * Enabling this command allows SPI_transfer to return partial data if the
 * master de-asserts the CS line before the expected number of frames were
 * recieved. This command @b arg is of type @a don't @a care and it returns
 * SPI_STATUS_SUCCESS or SPI_STATUS_ERROR.
 */
#define SPICC26XXDMA_CMD_RETURN_PARTIAL_ENABLE  (SPI_CMD_RESERVED + 0)

/*!
 * @brief Command used by SPI_control to disable partial return
 *
 * Disabling this command returns the SPICC26XXDMA to the default blocking
 * behavior where SPI_transfer blocks until all data bytes were received. With
 * this comand @b arg is @a don't @a care and it returns SPI_STATUS_SUCCESS.
 */
#define SPICC26XXDMA_CMD_RETURN_PARTIAL_DISABLE (SPI_CMD_RESERVED + 1)

/*!
 * @brief Command used by SPI_control to re-configure chip select pin
 *
 * This command specifies a chip select pin
 * With this command @b arg is of type @c PIN_Id and it return SPI_STATUS_SUCCESS
 */
#define SPICC26XXDMA_CMD_SET_CSN_PIN            (SPI_CMD_RESERVED + 2)
/** @}*/

/* BACKWARDS COMPATIBILITY */
#define SPICC26XXDMA_RETURN_PARTIAL_ENABLE      SPICC26XXDMA_CMD_RETURN_PARTIAL_ENABLE
#define SPICC26XXDMA_RETURN_PARTIAL_DISABLE     SPICC26XXDMA_CMD_RETURN_PARTIAL_DISABLE
#define SPICC26XXDMA_SET_CSN_PIN                SPICC26XXDMA_CMD_SET_CSN_PIN
/* END BACKWARDS COMPATIBILITY */

/*!
 *  @internal
 *  @brief
 *  SPI function table pointer
 */
extern const SPI_FxnTable SPICC26XXDMA_fxnTable;

/*!
 *  @internal
 *  @brief
 *  SPICC26XXDMA data frame size is used to determine how to configure the
 *  UDMA data transfers. This field is to be only used internally.
 *
 *  - SPICC26XXDMA_8bit:  txBuf and rxBuf are arrays of uint8_t elements
 *  - SPICC26XXDMA_16bit: txBuf and rxBuf are arrays of uint16_t elements
 */
typedef enum {
    SPICC26XXDMA_8bit  = 0,
    SPICC26XXDMA_16bit = 1
} SPICC26XXDMA_FrameSize;

/*!
 *  @brief  SPICC26XXDMA Hardware attributes
 *
 *  These fields, with the exception of intPriority,
 *  are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For CC26xxWare these definitions are found in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *      - driverlib/udma.h
 *
 *  intPriority is the SPI peripheral's interrupt priority, as defined by the
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
 *  const SPICC26XXDMA_HWAttrsV1 spiCC26XXDMAobjects[] = {
 *      {
 *          .baseAddr = SSI0_BASE,
 *          .intNum = INT_SPI0,
 *          .intPriority = ~0,
 *          .swiPriority = 0,
 *          .powerMngrId = PERIPH_SPI0,
 *          .defaultTxBufValue = 0,
 *          .rxChannelBitMask = UDMA_CHAN_SPI0_RX,
 *          .txChannelBitMask = UDMA_CHAN_SPI0_TX,
 *          .mosiPin = CONFIG_SPI0_MISO,
 *          .misoPin = CONFIG_SPI0_MOSI,
 *          .clkPin = CONFIG_SPI0_CLK,
 *          .csnPin = CONFIG_SPI0_CSN
 *      },
 *      {
 *          .baseAddr = SSI1_BASE,
 *          .intNum = INT_SPI1,
 *          .intPriority = ~0,
 *          .swiPriority = 0,
 *          .powerMngrId = PERIPH_SPI1,
 *          .defaultTxBufValue = 0,
 *          .rxChannelBitMask = UDMA_CHAN_SPI1_RX,
 *          .txChannelBitMask = UDMA_CHAN_SPI1_TX,
 *          .mosiPin = CONFIG_SPI1_MISO,
 *          .misoPin = CONFIG_SPI1_MOSI,
 *          .clkPin = CONFIG_SPI1_CLK,
 *          .csnPin = CONFIG_SPI1_CSN
 *      },
 *  };
 *  @endcode
 */
typedef struct {
    /*! SPI Peripheral's base address */
    uint32_t         baseAddr;
    /*! SPI CC26XXDMA Peripheral's interrupt vector */
    uint8_t          intNum;
    /*! @brief SPI CC26XXDMA Peripheral's interrupt priority.

        The CC26xx uses three of the priority bits,
        meaning ~0 has the same effect as (7 << 5).

        (7 << 5) will apply the lowest priority.

        (1 << 5) will apply the highest priority.

        Setting the priority to 0 is not supported by this driver.

        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency interrupts, thus invalidating the critical sections in this driver.
    */
    uint8_t          intPriority;
    /*! @brief SPI SWI priority.
        The higher the number, the higher the priority.
        The minimum is 0 and the maximum is 15 by default.
        The maximum can be reduced to save RAM by adding or modifying Swi.numPriorities in the kernel configuration file.
    */
    uint32_t         swiPriority;
    /*! SPI Peripheral's power manager ID */
    PowerCC26XX_Resource   powerMngrId;
    /*! Default TX value if txBuf == NULL */
    uint16_t         defaultTxBufValue;
    /*! uDMA controlTable channel index */
    uint32_t         rxChannelBitMask;
    /*! uDMA controlTable channel index */
    uint32_t         txChannelBitMask;
    /*! SPI MOSI pin */
    PIN_Id           mosiPin;
    /*! SPI MISO pin */
    PIN_Id           misoPin;
    /*! SPI CLK pin */
    PIN_Id           clkPin;
    /*! SPI CSN pin */
    PIN_Id           csnPin;

    /*! Minimum transfer size for DMA based transfer */
    uint32_t minDmaTransferSize;
} SPICC26XXDMA_HWAttrsV1;

/*!
 *  @brief  SPICC26XXDMA Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /* SPI control variables */
    SPI_TransferMode       transferMode;        /*!< Blocking or Callback mode */
    unsigned int           transferTimeout;     /*!< Timeout for the transfer when in blocking mode */
    SPI_CallbackFxn        transferCallbackFxn; /*!< Callback function pointer */
    SPI_Mode               mode;                /*!< Master or Slave mode */
    /*! @brief SPI bit rate in Hz.
     *
     *  When the SPI is configured as SPI slave, the maximum bitrate is 4MHz.
     *
     *  When the SPI is configured as SPI master, the maximum bitrate is 12MHz.
     */
    unsigned int           bitRate;
    unsigned int           dataSize;            /*!< SPI data frame size in bits */
    SPI_FrameFormat        frameFormat;         /*!< SPI frame format */

    /* SPI SYS/BIOS objects */
    HwiP_Struct hwi;    /*!< Hwi object handle */
    SwiP_Struct             swi;                 /*!< Swi object */
    SemaphoreP_Struct       transferComplete;    /*!< Notify finished SPICC26XXDMA transfer */

    /* SPI current transaction */
    SPI_Transaction        *currentTransaction; /*!< Ptr to the current transaction*/
    size_t                 amtDataXferred;      /*!< Number of frames transferred */
    size_t                 currentXferAmt;      /*!< Size of current DMA transfer */
    SPICC26XXDMA_FrameSize frameSize;           /*!< Data frame size variable */

    /* Support for dynamic CSN pin allocation */
    PIN_Id                 csnPin;              /*!< SPI CSN pin */

    /* PIN driver state object and handle */
    PIN_State              pinState;
    PIN_Handle             pinHandle;

    /* UDMA driver handle */
    UDMACC26XX_Handle      udmaHandle;

    /* Optional slave mode features */
    bool                   returnPartial;      /*!< Optional slave mode return partial on CSN deassert */

    /* Scratch buffer of size uint16_t */
    uint16_t               scratchBuf;

    /* SPI pre- and post notification functions */
    void                   *spiPreFxn;         /*!< SPI pre-notification function pointer */
    void                   *spiPostFxn;        /*!< SPI post-notification function pointer */
    Power_NotifyObj        spiPreObj;          /*!< SPI pre-notification object */
    Power_NotifyObj        spiPostObj;         /*!< SPI post-notification object */

    volatile bool          spiPowerConstraint; /*!< SPI power constraint flag, guard to avoid power constraints getting out of sync */

    bool                   isOpen;             /*!< Has the object been opened */
} SPICC26XXDMA_Object, *SPICC26XXDMA_Handle;




#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_spi_SPICC26XXDMA__include */
