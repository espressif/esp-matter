/***************************************************************************//**
 * @file
 * @brief Universal asynchronous receiver/transmitter (EUSART)
 *        driver for the Silicon Labs Bootloader.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "config/btl_config.h"

#include "btl_serial_driver.h"
#include "btl_driver_delay.h"
#include "api/btl_interface.h"
#include "btl_driver_util.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_eusart.h"
#include "em_gpio.h"
#include "em_ldma.h"

#include "debug/btl_debug.h"
#ifdef BTL_CONFIG_FILE
#include BTL_CONFIG_FILE
#else
#include "btl_euart_driver_cfg.h"
#endif

#define SL_DRIVER_EUSART_LDMA_RX_CHANNEL     0
#define SL_DRIVER_EUSART_LDMA_TX_CHANNEL     1

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Configuration validation
#if SL_SERIAL_EUART_BAUD_RATE < 100
#warning "EUART baud rate is very low, consider increasing speed"
#endif
#if SL_DRIVER_EUART_RX_BUFFER_SIZE < 8
#error "EUART RX buffer too small"
#endif
#if SL_DRIVER_EUART_TX_BUFFER_SIZE < 8
#error "EUART TX buffer too small"
#endif
#if (SL_DRIVER_EUART_RX_BUFFER_SIZE % 2) != 0
#error "EUART RX buffer size is not even"
#endif

// A btl_euart_drv driver instance initialization structure contains peripheral name
// of the euart, euart instance number and cmu_clock_type.
typedef struct {
  EUSART_TypeDef            *port;             // The peripheral used for EUART
  uint8_t                    uartNum;          // EUART instance number.
  CMU_Clock_TypeDef          euartClock;       // Clock source for the euart peripheral
} btl_euart_drv;

btl_euart_drv sl_uart_init_inst = {
  .port       = SL_SERIAL_EUART_PERIPHERAL,
  .uartNum    = SL_SERIAL_EUART_PERIPHERAL_NO,
#if defined(EUART0)
  .euartClock = cmuClock_EUART0,
#else
  .euartClock = cmuClock_EUSART0,
#endif
};

// ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Static variables

/// Flag to indicate hardware is up and running
static bool    initialized = false;
/// Receive buffer (forced to even size).
/// LDMA will pingpong between two halves of this buffer.
static uint8_t rxBuffer[SL_DRIVER_EUART_RX_BUFFER_SIZE];
/// Transmit buffer for LDMA use.
static uint8_t txBuffer[SL_DRIVER_EUART_TX_BUFFER_SIZE];
/// Amount of bytes in the current transmit operation
static size_t  txLength;

/// Index into the receive buffer indicating which byte is due to be read next.
static size_t  rxHead;

/// LDMA channel configuration triggering on free space in UART transmit FIFO
static LDMA_TransferCfg_t ldmaTxTransfer = LDMA_TRANSFER_CFG_PERIPHERAL(0);

/// LDMA channel configuration triggering on available byte in UART receive FIFO
static LDMA_TransferCfg_t ldmaRxTransfer = LDMA_TRANSFER_CFG_PERIPHERAL(0);

/// LDMA transfer for copying transmit buffer to UART TX FIFO
static LDMA_Descriptor_t ldmaTxDesc = LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(
  txBuffer,
  NULL,
  0
  );

static LDMA_Descriptor_t ldmaRxDesc[4] = {
  /// First half of receive pingpong configuration
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(
    NULL,
    &(rxBuffer[0]),
    SL_DRIVER_EUART_RX_BUFFER_SIZE / 2,
    1
    ),
  // Sync structure waiting for SYNC[1], clearing SYNC[0]
  LDMA_DESCRIPTOR_LINKREL_SYNC(
    0,
    (1 << 0),
    (1 << 1),
    (1 << 1),
    1
    ),
  /// Second half of receive pingpong configuration
  LDMA_DESCRIPTOR_LINKREL_P2M_BYTE(
    NULL,
    &(rxBuffer[SL_DRIVER_EUART_RX_BUFFER_SIZE / 2]),
    SL_DRIVER_EUART_RX_BUFFER_SIZE / 2,
    1
    ),
  /// Sync structure waiting for SYNC[0], clearing SYNC[1]
  LDMA_DESCRIPTOR_LINKREL_SYNC(
    0,
    (1 << 1),
    (1 << 0),
    (1 << 0),
    -3
    )
};
//  ‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
// Functions

/**
 * Initialize the configured EUART peripheral for UART operation. Also sets up
 *  GPIO settings for TX, RX, and, if configured, flow control.
 */
void uart_init(void)
{
#if defined(EUART0) && (SL_SERIAL_EUART_PERIPHERAL_NO == 0)
  sl_uart_init_inst.euartClock = cmuClock_EUART0;
  ldmaTxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUART0TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0);
  ldmaRxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUART0RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUART0);
#elif defined(EUSART0)  && (SL_SERIAL_EUART_PERIPHERAL_NO == 0)
  sl_uart_init_inst.euartClock = cmuClock_EUSART0;
  ldmaTxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0);
  ldmaRxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUSART0RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART0);
#elif defined(EUSART1) && (SL_SERIAL_EUART_PERIPHERAL_NO == 1)
  sl_uart_init_inst.euartClock = cmuClock_EUSART1;
  ldmaTxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART1);
  ldmaRxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUSART1RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART1);
#elif defined(EUSART2) && (SL_SERIAL_EUART_PERIPHERAL_NO == 2)
  sl_uart_init_inst.euartClock = cmuClock_EUSART2;
  ldmaTxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2TXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART2);
  ldmaRxTransfer.ldmaReqSel = (LDMAXBAR_CH_REQSEL_SIGSEL_EUSART2RXFL | LDMAXBAR_CH_REQSEL_SOURCESEL_EUSART2);
#endif

  ldmaTxDesc.xfer.dstAddr = (uint32_t)&(sl_uart_init_inst.port->TXDATA);

  ldmaRxDesc[0].xfer.srcAddr = (uint32_t)&(sl_uart_init_inst.port->RXDATA);

  ldmaRxDesc[2].xfer.srcAddr = (uint32_t)&(sl_uart_init_inst.port->RXDATA);

  CMU_ClockEnable(sl_uart_init_inst.euartClock, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Select HF clock as clock input source
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  CMU_ClockSelectSet(sl_uart_init_inst.euartClock, cmuSelect_EM01GRPACLK);
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_3) || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_4)
  CMU_ClockSelectSet(cmuClock_EM01GRPCCLK, cmuSelect_HFRCODPLL);
  if (sl_uart_init_inst.euartClock == cmuClock_EUSART0) {
    CMU_ClockSelectSet(cmuClock_EUSART0CLK, cmuSelect_EM01GRPCCLK);
  }
#endif

  EUSART_UartInit_TypeDef init = EUSART_UART_INIT_DEFAULT_HF;
  init.enable = eusartDisable;

  // Configure CTS/RTS in case of flow control
#if (SL_SERIAL_EUART_FLOW_CONTROL == 1)

  GPIO_PinModeSet(SL_SERIAL_EUART_RTS_PORT,
                  SL_SERIAL_EUART_RTS_PIN,
                  gpioModePushPull,
                  1);
  GPIO_PinModeSet(SL_SERIAL_EUART_CTS_PORT,
                  SL_SERIAL_EUART_CTS_PIN,
                  gpioModeInput,
                  1);

  // Advansed init to configure CTS and RTS
  EUSART_AdvancedInit_TypeDef advancedInit = EUSART_ADVANCED_INIT_DEFAULT;
  advancedInit.hwFlowControl = 1;
  init.advancedSettings = &advancedInit;

#if defined(EUART_PRESENT)
  GPIO->EUARTROUTE_SET->ROUTEEN = GPIO_EUART_ROUTEEN_RTSPEN;
  GPIO->EUARTROUTE_SET[0].RTSROUTE =
    (SL_SERIAL_EUART_RTS_PORT << _GPIO_EUART_RTSROUTE_PORT_SHIFT)
    | (SL_SERIAL_EUART_RTS_PIN << _GPIO_EUART_RTSROUTE_PIN_SHIFT);
  GPIO->EUARTROUTE_SET[0].CTSROUTE =
    (SL_SERIAL_EUART_CTS_PORT << _GPIO_EUART_CTSROUTE_PORT_SHIFT)
    | (SL_SERIAL_EUART_CTS_PIN << _GPIO_EUART_CTSROUTE_PIN_SHIFT);
#elif defined(EUSART_PRESENT)
  GPIO->EUSARTROUTE_SET[sl_uart_init_inst.uartNum].ROUTEEN = GPIO_EUSART_ROUTEEN_RTSPEN;
  GPIO->EUSARTROUTE_SET[sl_uart_init_inst.uartNum].RTSROUTE =
    (SL_SERIAL_EUART_RTS_PORT << _GPIO_EUSART_RTSROUTE_PORT_SHIFT)
    | (SL_SERIAL_EUART_RTS_PIN << _GPIO_EUSART_RTSROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE_SET[sl_uart_init_inst.uartNum].CTSROUTE =
    (SL_SERIAL_EUART_CTS_PORT << _GPIO_EUSART_CTSROUTE_PORT_SHIFT)
    | (SL_SERIAL_EUART_CTS_PIN << _GPIO_EUSART_CTSROUTE_PIN_SHIFT);
#endif
#endif
  init.refFreq = util_getClockFreq() / (1U + ((CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_PCLKPRESC_MASK)
                                              >> _CMU_SYSCLKCTRL_PCLKPRESC_SHIFT));
  init.baudrate = SL_SERIAL_EUART_BAUD_RATE;
  EUSART_UartInitHf(sl_uart_init_inst.port, &init);

  GPIO_PinModeSet(SL_SERIAL_EUART_TX_PORT,
                  SL_SERIAL_EUART_TX_PIN,
                  gpioModePushPull,
                  1);
  GPIO_PinModeSet(SL_SERIAL_EUART_RX_PORT,
                  SL_SERIAL_EUART_RX_PIN,
                  gpioModeInput,
                  1);
#if defined(EUART_COUNT) && (EUART_COUNT > 0)
  GPIO->EUARTROUTE->ROUTEEN = GPIO_EUART_ROUTEEN_TXPEN;
  GPIO->EUARTROUTE->TXROUTE = (SL_SERIAL_EUART_TX_PORT
                               << _GPIO_EUART_TXROUTE_PORT_SHIFT)
                              | (SL_SERIAL_EUART_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->EUARTROUTE->RXROUTE = (SL_SERIAL_EUART_RX_PORT
                               << _GPIO_EUART_RXROUTE_PORT_SHIFT)
                              | (SL_SERIAL_EUART_RX_PIN << _GPIO_EUART_RXROUTE_PIN_SHIFT);
#elif defined(EUSART_COUNT) && (EUSART_COUNT > 0)
  GPIO->EUSARTROUTE[sl_uart_init_inst.uartNum].ROUTEEN = GPIO_EUSART_ROUTEEN_TXPEN;
  GPIO->EUSARTROUTE[sl_uart_init_inst.uartNum].TXROUTE = (SL_SERIAL_EUART_TX_PORT
                                                          << _GPIO_EUSART_TXROUTE_PORT_SHIFT)
                                                         | (SL_SERIAL_EUART_TX_PIN << _GPIO_EUSART_TXROUTE_PIN_SHIFT);
  GPIO->EUSARTROUTE[sl_uart_init_inst.uartNum].RXROUTE = (SL_SERIAL_EUART_RX_PORT
                                                          << _GPIO_EUSART_RXROUTE_PORT_SHIFT)
                                                         | (SL_SERIAL_EUART_RX_PIN << _GPIO_EUSART_RXROUTE_PIN_SHIFT);
#endif

#if (SL_VCOM_ENABLE == 1) && defined(SL_VCOM_ENABLE_PORT)
  GPIO_PinModeSet(SL_VCOM_ENABLE_PORT,
                  SL_VCOM_ENABLE_PIN,
                  gpioModePushPull,
                  1);
#endif
  EUSART_Enable(sl_uart_init_inst.port, eusartEnable);

  // Configure LDMA
  CMU->CLKEN0_SET = (CMU_CLKEN0_LDMA | CMU_CLKEN0_LDMAXBAR);
  LDMA->EN = LDMA_EN_EN;
  // Reset LDMA
  LDMA->CTRL = _LDMA_CTRL_RESETVALUE;
  LDMA->CHEN = _LDMA_CHEN_RESETVALUE;
  LDMA->DBGHALT = _LDMA_DBGHALT_RESETVALUE;
  LDMA->REQDIS = _LDMA_REQDIS_RESETVALUE;
  LDMA->IEN = _LDMA_IEN_RESETVALUE;

  // Set up channel 0 as RX transfer
  LDMAXBAR->CH[SL_DRIVER_EUSART_LDMA_RX_CHANNEL].REQSEL = ldmaRxTransfer.ldmaReqSel;
  LDMA->CH[SL_DRIVER_EUSART_LDMA_RX_CHANNEL].LOOP
    = (ldmaRxTransfer.ldmaLoopCnt << _LDMA_CH_LOOP_LOOPCNT_SHIFT);
  LDMA->CH[SL_DRIVER_EUSART_LDMA_RX_CHANNEL].CFG
    = (ldmaRxTransfer.ldmaCfgArbSlots << _LDMA_CH_CFG_ARBSLOTS_SHIFT)
      | (ldmaRxTransfer.ldmaCfgSrcIncSign << _LDMA_CH_CFG_SRCINCSIGN_SHIFT)
      | (ldmaRxTransfer.ldmaCfgDstIncSign << _LDMA_CH_CFG_DSTINCSIGN_SHIFT);

  LDMA->CH[SL_DRIVER_EUSART_LDMA_RX_CHANNEL].LINK
    = (uint32_t)(&ldmaRxDesc[0]) & _LDMA_CH_LINK_LINKADDR_MASK;

  // Set up channel 1 as TX transfer
  LDMAXBAR->CH[SL_DRIVER_EUSART_LDMA_TX_CHANNEL].REQSEL = ldmaTxTransfer.ldmaReqSel;
  LDMA->CH[SL_DRIVER_EUSART_LDMA_TX_CHANNEL].LOOP
    = (ldmaTxTransfer.ldmaLoopCnt << _LDMA_CH_LOOP_LOOPCNT_SHIFT);
  LDMA->CH[SL_DRIVER_EUSART_LDMA_TX_CHANNEL].CFG
    = (ldmaTxTransfer.ldmaCfgArbSlots << _LDMA_CH_CFG_ARBSLOTS_SHIFT)
      | (ldmaTxTransfer.ldmaCfgSrcIncSign << _LDMA_CH_CFG_SRCINCSIGN_SHIFT)
      | (ldmaTxTransfer.ldmaCfgDstIncSign << _LDMA_CH_CFG_DSTINCSIGN_SHIFT);

  // Clear DONE flag on both RX and TX channels
  BUS_RegMaskedClear(&LDMA->CHDONE,
                     ((1 << SL_DRIVER_EUSART_LDMA_RX_CHANNEL)
                      | (1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL)));

  // Kick off background RX
  LDMA->LINKLOAD = (1 << SL_DRIVER_EUSART_LDMA_RX_CHANNEL);

  // Mark second half of RX buffer as ready
  LDMA->SYNCSWSET_SET = (1UL << 1);

  initialized = true;
}

/**
 * Disable the configured EUART peripheral for UART operation.
 */
void uart_deinit(void)
{
#if defined(CMU_CLKEN0_EUART0)
  CMU->CLKEN0_CLR = (CMU_CLKEN0_LDMA | CMU_CLKEN0_LDMAXBAR | CMU_CLKEN0_EUART0);
#else
  CMU->CLKEN0_CLR = (CMU_CLKEN0_LDMA | CMU_CLKEN0_LDMAXBAR);
  CMU->CLKEN1_CLR = CMU_CLKEN1_EUSART0;
#endif

  initialized = false;
}

/**
 * Write a data buffer to the UART.
 *
 * @param[in] buffer The data buffer to send
 * @param[in] length Amount of bytes in the buffer to send
 * @param[in] blocking Indicates whether this transfer can be offloaded to LDMA
 *  and return, or whether it should wait on completion before returning.
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t uart_sendBuffer(uint8_t* buffer, size_t length, bool blocking)
{
  BTL_ASSERT(initialized == true);
  BTL_ASSERT(length > 0);

  if (length >= SL_DRIVER_EUART_TX_BUFFER_SIZE) {
    return BOOTLOADER_ERROR_SPI_PERIPHERAL_ARGUMENT;
  }

  if (!uart_isTxIdle()) {
    return BOOTLOADER_ERROR_UART_BUSY;
  }

  // Copy buffer
  txLength = length;
  length = 0;
  for (; length < txLength; length++) {
    txBuffer[length] = buffer[length];
  }

  // Populate descriptor
  ldmaTxDesc.xfer.xferCnt = txLength - 1;

  // Kick off transfer
  LDMA->CH[SL_DRIVER_EUSART_LDMA_TX_CHANNEL].LINK
    = (uint32_t)(&ldmaTxDesc) & _LDMA_CH_LINK_LINKADDR_MASK;
  LDMA->LINKLOAD = (1UL << SL_DRIVER_EUSART_LDMA_TX_CHANNEL);

  // Optional wait for completion
  if (blocking) {
    while (uart_isTxIdle() == false) {
      // Do nothing
    }
    while (!(sl_uart_init_inst.port->STATUS & EUSART_STATUS_TXC)) {
      // Do nothing
    }
  }

  return BOOTLOADER_OK;
}

/**
 * Write one byte to the UART in a blocking fashion.
 *
 * @param[in] byte The byte to send
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t uart_sendByte(uint8_t byte)
{
  BTL_ASSERT(initialized == true);

  // Wait until previous LDMA transfer is done
  while (!uart_isTxIdle()) {
    // Do nothing
  }

  // Wait until there is room for one more byte
  while (!(sl_uart_init_inst.port->STATUS & EUSART_STATUS_TXFL)) {
    // Do nothing
  }

  // Send byte
  sl_uart_init_inst.port->TXDATA = byte;

  // Wait until byte has been fully sent out
  while (!(sl_uart_init_inst.port->STATUS & EUSART_STATUS_TXC)) {
    // Do nothing
  }
  return BOOTLOADER_OK;
}

/**
 * Find out whether the UART can accept more data to send.
 *
 * @return true if the UART is not currently transmitting
 */
bool uart_isTxIdle(void)
{
  BTL_ASSERT(initialized == true);
  if (LDMA->CHDONE & (1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL)) {
    LDMA->CHDIS = (1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL);
    BUS_RegMaskedClear(&LDMA->CHDONE, 1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL);
    txLength = 0;
    return true;
  } else if ((LDMA->CHSTATUS & (1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL)) == 0) {
    BUS_RegMaskedClear(&LDMA->CHDONE, 1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL);
    txLength = 0;
    return true;
  }

  return false;
}

/**
 * Gets the amount of bytes ready for reading.
 *
 * @return Amount of bytes in the receive buffer available for reading with
 *   @ref protocol_uart_recv
 */
size_t  uart_getRxAvailableBytes(void)
{
  size_t ldmaHead;
  size_t dst;

  BTL_ASSERT(initialized == true);

  // Get destination address for next transfer
  dst = LDMA->CH[SL_DRIVER_EUSART_LDMA_RX_CHANNEL].DST;

  if (dst == 0x0101) {
    // SYNC descriptor with bit 0 of MATCHEN and MATCHVAL set
    ldmaHead = 0;
  } else if (dst == 0x0202) {
    // SYNC descriptor with bit 1 of MATCHEN and MATCHVAL set
    ldmaHead = SL_DRIVER_EUART_RX_BUFFER_SIZE / 2;
  } else {
    // XFER descriptor with absolute address in buffer
    ldmaHead = dst - (uint32_t)(rxBuffer);
  }

  // Return difference between received head and LDMA head
  if (rxHead == ldmaHead) {
    return 0;
  } else if (rxHead < ldmaHead) {
    return ldmaHead - rxHead;
  } else {
    return SL_DRIVER_EUART_RX_BUFFER_SIZE - (rxHead - ldmaHead);
  }
}

/**
 * Read from the UART into a data buffer.
 *
 * @param[out] buffer The data buffer to receive into
 * @param[in] requestedLength Amount of bytes we'd like to read
 * @param[out] receivedLength Amount of bytes read
 * @param[in] blocking Indicates whether we to wait for requestedLength
 *   bytes to be available and read before returning, whether to read out
 *   data currently in the buffer and return.
 * @param[in] timeout Number of milliseconds to wait for data in blocking mode
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t uart_receiveBuffer(uint8_t  * buffer,
                           size_t   requestedLength,
                           size_t   * receivedLength,
                           bool     blocking,
                           uint32_t timeout)
{
  size_t copyBytes = 0;
  size_t copiedBytes = 0;

  BTL_ASSERT(initialized == true);
  BTL_ASSERT(requestedLength < SL_DRIVER_EUART_RX_BUFFER_SIZE);

  // Check whether we have enough data
  // Optional spin for timeout cycles
  if (blocking) {
    if (timeout != 0) {
      delay_init();
      delay_milliseconds(timeout, false);
    }

    while (uart_getRxAvailableBytes() < requestedLength) {
      if ((timeout != 0) && delay_expired()) {
        break;
      }
    }
  }

  copyBytes = uart_getRxAvailableBytes();
  if (requestedLength < copyBytes) {
    copyBytes = requestedLength;
  }

  // Copy up to requested bytes to given buffer
  while (copiedBytes < copyBytes) {
    buffer[copiedBytes] = rxBuffer[rxHead];
    copiedBytes++;
    rxHead++;

    if (rxHead == SL_DRIVER_EUART_RX_BUFFER_SIZE) {
      rxHead = 0;
      // Completed processing of second half of the buffer, mark it as available
      // for LDMA again by setting SYNC[1]
      LDMA->SYNCSWSET_SET = 1 << 1;
    } else if (rxHead == SL_DRIVER_EUART_RX_BUFFER_SIZE / 2) {
      // Completed processing of first half of the buffer, mark it as available
      // for LDMA again by setting SYNC[0]
      LDMA->SYNCSWSET_SET = 1 << 0;
    }
  }

  if ((uint32_t)receivedLength != 0UL) {
    *receivedLength = copiedBytes;
  }

  if (copiedBytes < requestedLength) {
    return BOOTLOADER_ERROR_UART_TIMEOUT;
  } else {
    return BOOTLOADER_OK;
  }
}

/**
 * Get one byte from the UART in a blocking fashion.
 *
 * @param[out] byte The byte to send
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t uart_receiveByte(uint8_t* byte)
{
  return uart_receiveBuffer(byte, 1, (size_t*)0UL, true, 0);
}

/**
 * Get one byte from the UART in a blocking fashion.
 *
 * @param[out] byte The byte to send
 * @param[in]  timeout Maximum timeout before aborting transfer
 *
 * @return BOOTLOADER_OK if successful, error code otherwise
 */
int32_t uart_receiveByteTimeout(uint8_t* byte, uint32_t timeout)
{
  return uart_receiveBuffer(byte, 1, (size_t *)0UL, true, timeout);
}

/**
 * Flushes one or both UART buffers.
 *
 * @param[in] flushTx Flushes the transmit buffer when true
 * @param[in] flushRx Flushes the receive buffer when true
 *
 * @return BOOTLOADER_OK
 */
int32_t uart_flush(bool flushTx, bool flushRx)
{
  BTL_ASSERT(initialized == true);

  if (flushTx) {
    LDMA->CHDIS = (1 << SL_DRIVER_EUSART_LDMA_TX_CHANNEL);
    txLength = 0;
  }

  if (flushRx) {
    LDMA->CHDIS = (1 << SL_DRIVER_EUSART_LDMA_RX_CHANNEL);
    BUS_RegMaskedClear(&LDMA->CHDONE, 1 << SL_DRIVER_EUSART_LDMA_RX_CHANNEL);
    LDMA->CH[SL_DRIVER_EUSART_LDMA_RX_CHANNEL].LINK
      = (uint32_t)(&ldmaRxDesc[0]) & _LDMA_CH_LINK_LINKADDR_MASK;
    rxHead = 0;
    LDMA->LINKLOAD = (1 << SL_DRIVER_EUSART_LDMA_RX_CHANNEL);

    // Mark second half of RX buffer as ready
    BUS_RegMaskedSet(&LDMA->SYNCSWSET, 1 << 1);
  }

  return BOOTLOADER_OK;
}
