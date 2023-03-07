/***************************************************************************//**
 * @file
 * @brief Internal SPI Protocol Device Header for USART and Pin Configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef __SPI_PROTOCOL_DEVICE_H__
#define __SPI_PROTOCOL_DEVICE_H__

#include "legacy_ncp_spi_config.h"

// Size of SPIP VERSION and ALIVE commands (not including the frame terminator).
#define SPIP_COMMAND_SIZE            1

// Size of all error responses (not including the frame terminator).
#define SPIP_ERROR_RESPONSE_SIZE     2

// The maximum supported SPIP payload length. This should be enough to fit a
// max EZSP or bootloader message.
#define MAX_PAYLOAD_FRAME_LENGTH     133

// The index and size of the prefix.
#define SPIP_PREFIX_INDEX            0
#define SPIP_PREFIX_SIZE             1

// The index and size of the length (for commands that have it).
#define SPIP_LENGTH_INDEX            1
#define SPIP_LENGTH_SIZE             1

// Legacy name for the above.
#define EZSP_LENGTH_INDEX            (SPIP_LENGTH_INDEX)

// The size of the frame terminator (the index may vary based on the length).
#define SPIP_FRAME_TERMINATOR_SIZE   1

// The SPIP overhead, in bytes, for EZSP and bootloader payload messages
#define SPIP_OVERHEAD                (SPIP_PREFIX_SIZE   \
                                      + SPIP_LENGTH_SIZE \
                                      + SPIP_FRAME_TERMINATOR_SIZE)

// The maximum SPI Protocol message size.
#define SPIP_MAX_MESSAGE_SIZE        (MAX_PAYLOAD_FRAME_LENGTH + SPIP_OVERHEAD)

// We add an extra byte to the size for padding with 0xFF at the end of the
// response transmission to keep MISO high.
#define SPIP_MAX_WITH_PADDING        (SPIP_MAX_MESSAGE_SIZE + 1)

// We define the buffer size to be twice the maximum message size so that, in
// the worst case, it can hold a maximally sized command and response at the
// same time. This way we can start the receive for the next command at the same
// time as we start the transmit of the current response, making the SPIP code
// more resilient against unexpectedly long delays in the processing of ticks
// and interrupts (during, for instance, flash page erasure).
#define SPIP_BUFFER_SIZE             (2 * SPIP_MAX_WITH_PADDING)

// SPI transfer timeout (ms), 0 disables the feature (it will never timeout).
#ifndef SPI_NCP_TIMEOUT
#define SPI_NCP_TIMEOUT 0
#endif

#ifndef SPI_NCP_RX_BUFFERS
#define SPI_NCP_RX_BUFFERS 1
#endif
// SPI NCP USART selection

#if (LEGACY_NCP_SPI_PERIPHERAL_NO == 0)
  #define SPI_NCP_USART           USART0
  #define SPI_NCP_USART_IRQn      USART0_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME  USART0_RX_IRQHandler
  #define SPI_NCP_USART_CLOCK     cmuClock_USART0
#elif (LEGACY_NCP_SPI_PERIPHERAL_NO == 1)
  #define SPI_NCP_USART           USART1
  #define SPI_NCP_USART_IRQn      USART1_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME  USART1_RX_IRQHandler
  #define SPI_NCP_USART_CLOCK     cmuClock_USART1
#elif (LEGACY_NCP_SPI_PERIPHERAL_NO == 2)
  #define SPI_NCP_USART           USART2
  #define SPI_NCP_USART_IRQn      USART2_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME  USART2_RX_IRQHandler
  #define SPI_NCP_USART_CLOCK     cmuClock_USART2
#elif (LEGACY_NCP_SPI_PERIPHERAL_NO == 3)
  #define SPI_NCP_USART           USART3
  #define SPI_NCP_USART_IRQn      USART3_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME  USART3_RX_IRQHandler
  #define SPI_NCP_USART_CLOCK     cmuClock_USART3
#elif (LEGACY_NCP_SPI_PERIPHERAL_NO == 4)
  #define SPI_NCP_USART           USART4
  #define SPI_NCP_USART_IRQn      USART4_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME  USART4_RX_IRQHandler
  #define SPI_NCP_USART_CLOCK     cmuClock_USART4
#elif (LEGACY_NCP_SPI_PERIPHERAL_NO == 5)
  #define SPI_NCP_USART           USART5
  #define SPI_NCP_USART_IRQn      USART5_RX_IRQn
  #define SPI_NCP_USART_IRQ_NAME  USART5_RX_IRQHandler
  #define SPI_NCP_USART_CLOCK     cmuClock_USART5
#else
  #error Invalid USART selected for SPI NCP
#endif

#if defined(LEGACY_NCP_SPI_TX_LOC)
  #define SPI_NCP_MOSI_LOC        LEGACY_NCP_SPI_TX_LOC
  #define SPI_NCP_MISO_LOC        LEGACY_NCP_SPI_RX_LOC
  #define SPI_NCP_CLK_LOC         LEGACY_NCP_SPI_CLK_LOC
  #define SPI_NCP_CS_LOC          LEGACY_NCP_SPI_CS_LOC
#endif

#define SPI_NCP_MOSI_PORT         LEGACY_NCP_SPI_TX_PORT
#define SPI_NCP_MOSI_PIN          LEGACY_NCP_SPI_TX_PIN
#define SPI_NCP_MISO_PORT         LEGACY_NCP_SPI_RX_PORT
#define SPI_NCP_MISO_PIN          LEGACY_NCP_SPI_RX_PIN
#define SPI_NCP_CLK_PORT          LEGACY_NCP_SPI_CLK_PORT
#define SPI_NCP_CLK_PIN           LEGACY_NCP_SPI_CLK_PIN
#define SPI_NCP_CS_PORT           LEGACY_NCP_SPI_CS_PORT
#define SPI_NCP_CS_PIN            LEGACY_NCP_SPI_CS_PIN

#define BSP_SPINCP_NWAKE_PORT        LEGACY_NCP_SPI_WAKE_INT_PORT
#define BSP_SPINCP_NWAKE_PIN         LEGACY_NCP_SPI_WAKE_INT_PIN

#define BSP_SPINCP_NHOSTINT_PORT     LEGACY_NCP_SPI_HOST_INT_PORT
#define BSP_SPINCP_NHOSTINT_PIN      LEGACY_NCP_SPI_HOST_INT_PIN

// Configuration data for SPI NCP slave.
#if defined(SPI_NCP_MOSI_LOC)
#define SPI_NCP_USART_INIT                                             \
  {                                                                    \
    SPI_NCP_USART,              /* USART port                       */ \
    SPI_NCP_MOSI_LOC,           /* USART Tx/MOSI pin location number*/ \
    SPI_NCP_MISO_LOC,           /* USART Rx/MISO pin location number*/ \
    SPI_NCP_CLK_LOC,            /* USART Clk pin location number    */ \
    SPI_NCP_CS_LOC,             /* USART Cs pin location number     */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0xFF,                       /* Dummy tx value for rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#else
#define SPI_NCP_USART_INIT                                             \
  {                                                                    \
    SPI_NCP_USART,              /* USART port                       */ \
    SPI_NCP_MOSI_PORT,          /* USART Tx/MOSI port               */ \
    SPI_NCP_MISO_PORT,          /* USART Rx/MISO port               */ \
    SPI_NCP_CLK_PORT,           /* USART Clk port                   */ \
    SPI_NCP_CS_PORT,            /* USART CS port                    */ \
    SPI_NCP_MOSI_PIN,           /* USART Tx/MOSI pin                */ \
    SPI_NCP_MISO_PIN,           /* USART Rx/MISO pin                */ \
    SPI_NCP_CLK_PIN,            /* USART Clk pin                    */ \
    SPI_NCP_CS_PIN,             /* USART CS pin                     */ \
    0,                          /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0xFF,                       /* Dummy tx value for rx only funcs */ \
    spidrvSlave,                /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlAuto,        /* CS controlled by the driver      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#endif

#ifndef SPI_BTL_USART_INIT
#define SPI_BTL_USART_INIT                                                \
  {                                                                       \
    usartEnable,   /*enable*/                                             \
    0,             /*refFreq*/                                            \
    1000,          /*baudrate*/                                           \
    usartDatabits8, /*databits*/                                          \
    false,         /*master:  false = slave mode*/                        \
    true,          /*msfb: true = msb first  false = lsb first*/          \
    usartClockMode0, /*clockMode: clock idle low, sample on rising edge*/ \
  }
#endif

#if (!defined(DISABLE_NWAKE)) && (!defined(HAL_CONFIG) || defined(BSP_SPINCP_NWAKE_PIN))
static inline bool nWAKE_IS_NEGATED(void)
{
  return (GPIO_PinInGet(BSP_SPINCP_NWAKE_PORT, BSP_SPINCP_NWAKE_PIN) != 0);
}

static inline bool nWAKE_IS_ASSERTED(void)
{
  return (GPIO_PinInGet(BSP_SPINCP_NWAKE_PORT, BSP_SPINCP_NWAKE_PIN) == 0);
}
#else
#define nWAKE_IS_NEGATED() true
#define nWAKE_IS_ASSERTED() false
#endif

static inline bool nSSEL_IS_NEGATED(void)
{
  return (GPIO_PinInGet(SPI_NCP_CS_PORT, SPI_NCP_CS_PIN) != 0);
}

static inline bool nSSEL_IS_ASSERTED(void)
{
  return (GPIO_PinInGet(SPI_NCP_CS_PORT, SPI_NCP_CS_PIN) == 0);
}

static inline void SET_nHOST_INT(void)
{
  GPIO_PinOutSet(BSP_SPINCP_NHOSTINT_PORT, BSP_SPINCP_NHOSTINT_PIN);
}

static inline void CLR_nHOST_INT(void)
{
  GPIO_PinOutClear(BSP_SPINCP_NHOSTINT_PORT, BSP_SPINCP_NHOSTINT_PIN);
}

#endif // __SPI_PROTOCOL_DEVICE_H__
