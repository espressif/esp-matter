/***************************************************************************//**
 * @file
 * @brief UARTDRV API definition.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef UARTDRV_H
#define UARTDRV_H

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif

#include "em_device.h"
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
#include "em_usart.h"
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0)
#include "em_leuart.h"
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
#include "em_eusart.h"
#endif
#include "em_gpio.h"
#include "em_cmu.h"
#include "ecode.h"
#include "uartdrv_config.h"
#include "dmadrv.h"
#include "sl_enum.h"
#include "sl_sleeptimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup uartdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup uartdrv_error_codes Error Codes
 * @{
 ******************************************************************************/
#define ECODE_EMDRV_UARTDRV_OK                (ECODE_OK)                              ///< A successful return value.
#define ECODE_EMDRV_UARTDRV_WAITING           (ECODE_EMDRV_UARTDRV_BASE | 0x00000001) ///< An operation is waiting in queue.
#define ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE    (ECODE_EMDRV_UARTDRV_BASE | 0x00000002) ///< An illegal UART handle.
#define ECODE_EMDRV_UARTDRV_PARAM_ERROR       (ECODE_EMDRV_UARTDRV_BASE | 0x00000003) ///< An illegal input parameter.
#define ECODE_EMDRV_UARTDRV_BUSY              (ECODE_EMDRV_UARTDRV_BASE | 0x00000004) ///< The UART port is busy.
#define ECODE_EMDRV_UARTDRV_ILLEGAL_OPERATION (ECODE_EMDRV_UARTDRV_BASE | 0x00000005) ///< An illegal operation on the UART port.
#define ECODE_EMDRV_UARTDRV_IDLE              (ECODE_EMDRV_UARTDRV_BASE | 0x00000008) ///< No UART transfer is in progress.
#define ECODE_EMDRV_UARTDRV_ABORTED           (ECODE_EMDRV_UARTDRV_BASE | 0x00000009) ///< A UART transfer has been aborted.
#define ECODE_EMDRV_UARTDRV_QUEUE_FULL        (ECODE_EMDRV_UARTDRV_BASE | 0x0000000A) ///< A UART operation queue is full.
#define ECODE_EMDRV_UARTDRV_QUEUE_EMPTY       (ECODE_EMDRV_UARTDRV_BASE | 0x0000000B) ///< A UART operation queue is empty.
#define ECODE_EMDRV_UARTDRV_PARITY_ERROR      (ECODE_EMDRV_UARTDRV_BASE | 0x0000000C) ///< A UART parity error frame. Data is ignored.
#define ECODE_EMDRV_UARTDRV_FRAME_ERROR       (ECODE_EMDRV_UARTDRV_BASE | 0x0000000D) ///< A UART frame error. Data is ignored.
#define ECODE_EMDRV_UARTDRV_DMA_ALLOC_ERROR   (ECODE_EMDRV_UARTDRV_BASE | 0x0000000E) ///< Unable to allocate DMA channels.
#define ECODE_EMDRV_UARTDRV_CLOCK_ERROR       (ECODE_EMDRV_UARTDRV_BASE | 0x0000000F) ///< Unable to set a desired baudrate.
/** @} (end addtogroup error codes) */

/***************************************************************************//**
 * @addtogroup uartdrv_status_codes Status Codes
 * @{
 ******************************************************************************/
#define UARTDRV_STATUS_RXEN     (1 << 0)  ///< The receiver is enabled.
#define UARTDRV_STATUS_TXEN     (1 << 1)  ///< The transmitter is enabled.
#define UARTDRV_STATUS_RXBLOCK  (1 << 3)  ///< The receiver is blocked; incoming frames will be discarded.
#define UARTDRV_STATUS_TXTRI    (1 << 4)  ///< The transmitter is tristated.
#define UARTDRV_STATUS_TXC      (1 << 5)  ///< A transmit operation is complete. No more data is available in the transmit buffer and shift register.
#define UARTDRV_STATUS_TXBL     (1 << 6)  ///< The transmit buffer is empty.
#define UARTDRV_STATUS_RXDATAV  (1 << 7)  ///< Data is available in the receive buffer.
#define UARTDRV_STATUS_RXFULL   (1 << 8)  ///< The receive buffer is full.
#define UARTDRV_STATUS_TXIDLE   (1 << 13) ///< The transmitter is idle.
#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
#define UARTDRV_STATUS_RXIDLE   (1 << 12) ///< The Receiver is idle.
#endif
/** @} (end addtogroup status codes) */

typedef uint32_t UARTDRV_Count_t;     ///< A UART transfer count
typedef uint32_t UARTDRV_Status_t;    ///< A UART status return type. Bitfield of UARTDRV_STATUS_* values.

/// Flow Control method
SL_ENUM(UARTDRV_FlowControlType_t) {
  uartdrvFlowControlNone   = 0,   ///< None
  uartdrvFlowControlSw     = 1,   ///< Software XON/XOFF
  uartdrvFlowControlHw     = 2,   ///< nRTS/nCTS hardware handshake
  uartdrvFlowControlHwUart = 3    ///< UART peripheral controls nRTS/nCTS
};

/// Flow Control state
SL_ENUM(UARTDRV_FlowControlState_t) {
  uartdrvFlowControlOn = 0,         ///< XON or nRTS/nCTS low
  uartdrvFlowControlOff = 1,        ///< XOFF or nRTS/nCTS high
  uartdrvFlowControlAuto = 2        ///< This driver controls the state.
};

/// Transfer abort type
SL_ENUM(UARTDRV_AbortType_t) {
  uartdrvAbortTransmit = 1,          ///< Abort current and queued transmit operations
  uartdrvAbortReceive = 2,           ///< Abort current and queued receive operations
  uartdrvAbortAll = 3                ///< Abort all current and queued operations
};

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/// Type of a UART peripheral
SL_ENUM(UARTDRV_UartType_t) {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
  uartdrvUartTypeUart = 0,         ///< USART/UART peripheral
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0)
  uartdrvUartTypeLeuart = 1         ///< LEUART peripheral
#elif (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
  uartdrvUartTypeEuart = 2         ///< EUART peripheral
#endif
};
/// @endcond

struct UARTDRV_HandleData;

/***************************************************************************//**
 * @brief
 *  UARTDRV transfer completion callback function.
 *
 * @details
 *  Called when a transfer is complete. An
 *  application should check the transferStatus and itemsTransferred values.
 *
 * @param[in] handle
 *   The UARTDRV device handle used to start the transfer.
 *
 * @param[in] transferStatus
 *   Completion status of the transfer operation.
 *
 * @param[in] data
 *   A pointer to the transfer data buffer.
 *
 * @param[in] transferCount
 *   A number of bytes transferred.
 ******************************************************************************/
typedef void (*UARTDRV_Callback_t)(struct UARTDRV_HandleData *handle,
                                   Ecode_t transferStatus,
                                   uint8_t *data,
                                   UARTDRV_Count_t transferCount);

/// UART transfer buffer
typedef struct {
  uint8_t *data;                           ///< Transfer data buffer
  UARTDRV_Count_t transferCount;           ///< Transfer item count
  volatile UARTDRV_Count_t itemsRemaining; ///< Transfer items remaining
  UARTDRV_Callback_t callback;             ///< Completion callback
  Ecode_t transferStatus;                  ///< Completion status of the transfer operation
} UARTDRV_Buffer_t;

/// Transfer operation FIFO queue typedef
typedef struct {
  volatile uint16_t head;                  ///< An index of the next byte to send.
  volatile uint16_t tail;                  ///< An index of the location to enqueue the next message.
  volatile uint16_t used;                  ///< A number of bytes queued.
  const uint16_t size;                     ///< FIFO size.
  UARTDRV_Buffer_t fifo[1];                ///< FIFO of queued data. Actual size varies.
} UARTDRV_Buffer_FifoQueue_t;

/// Macros to define FIFO and buffer queues. typedef can't be used because the size
/// of the FIFO array in the queues can change.
#define DEFINE_BUF_QUEUE(qSize, qName) \
  typedef struct {                     \
    uint16_t head;                     \
    uint16_t tail;                     \
    volatile uint16_t used;            \
    const uint16_t size;               \
    UARTDRV_Buffer_t fifo[qSize];      \
  } _##qName;                          \
  static volatile _##qName qName =     \
  {                                    \
    .head = 0,                         \
    .tail = 0,                         \
    .used = 0,                         \
    .size = qSize,                     \
  }

#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
/// A UART driver instance initialization structure.
/// Contains a number of UARTDRV configuration options.
/// It is required for driver instance initialization.
/// This structure is passed to @ref UARTDRV_Init() when initializing a UARTDRV
/// instance.
typedef struct {
  USART_TypeDef              *port;             ///< The peripheral used for UART
  uint32_t                   baudRate;          ///< UART baud rate
#if defined(_USART_ROUTELOC0_MASK)
  uint8_t                    portLocationTx;    ///< A location number for UART Tx pin.
  uint8_t                    portLocationRx;    ///< A location number for UART Rx pin.
#elif defined(_USART_ROUTE_MASK)
  uint8_t                    portLocation;      ///< A location number for UART pins.
#elif defined(_GPIO_USART_ROUTEEN_MASK)
  GPIO_Port_TypeDef          txPort;            ///< Port for UART Tx pin.
  GPIO_Port_TypeDef          rxPort;            ///< Port for UART Rx pin.
  uint8_t                    txPin;             ///< Pin number for UART Tx.
  uint8_t                    rxPin;             ///< Pin number for UART Rx.
  uint8_t                    uartNum;           ///< UART instance number.
#endif
  USART_Stopbits_TypeDef     stopBits;          ///< A number of stop bits.
  USART_Parity_TypeDef       parity;            ///< Parity configuration.
  USART_OVS_TypeDef          oversampling;      ///< Oversampling mode.
#if defined(USART_CTRL_MVDIS)
  bool                       mvdis;             ///< Majority Vote Disable for 16x, 8x and 6x oversampling modes.
#endif
  UARTDRV_FlowControlType_t  fcType;            ///< Flow control mode.
  GPIO_Port_TypeDef          ctsPort;           ///< A CTS pin port number.
  uint8_t                    ctsPin;            ///< A CTS pin number.
  GPIO_Port_TypeDef          rtsPort;           ///< An RTS pin port number.
  uint8_t                    rtsPin;            ///< An RTS pin number.
  UARTDRV_Buffer_FifoQueue_t *rxQueue;          ///< A receive operation queue.
  UARTDRV_Buffer_FifoQueue_t *txQueue;          ///< T transmit operation queue.
#if defined(_USART_ROUTELOC1_MASK)
  uint8_t                    portLocationCts;   ///< A location number for the UART CTS pin.
  uint8_t                    portLocationRts;   ///< A location number for the UART RTS pin.
#endif
} UARTDRV_InitUart_t;

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/// Deprecated UART driver instance initialization structure alias.
/// @deprecated This structure is deprecated. Use UARTDRV_InitUart_t instead.
typedef UARTDRV_InitUart_t UARTDRV_Init_t SL_DEPRECATED_API_SDK_4_1;
/// @endcond
#endif

#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
/// LEUART driver instance initialization structure.
/// Contains a number of UARTDRV configuration options.
/// It is required to initialize a driver instance.
/// This structure is passed to @ref UARTDRV_InitLeuart() when initializing a UARTDRV
/// instance.
typedef struct {
  LEUART_TypeDef             *port;             ///< The peripheral used for LEUART
  uint32_t                   baudRate;          ///< UART baud rate
#if defined(_LEUART_ROUTELOC0_MASK)
  uint8_t                    portLocationTx;    ///< Location number for LEUART Tx pin.
  uint8_t                    portLocationRx;    ///< Location number for LEUART Rx pin.
#else
  uint8_t                    portLocation;      ///< Location number for LEUART pins
#endif
  LEUART_Stopbits_TypeDef    stopBits;          ///< Number of stop bits
  LEUART_Parity_TypeDef      parity;            ///< Parity configuration
  UARTDRV_FlowControlType_t  fcType;            ///< Flow control mode
  GPIO_Port_TypeDef          ctsPort;           ///< CTS pin port number
  uint8_t                    ctsPin;            ///< CTS pin number
  GPIO_Port_TypeDef          rtsPort;           ///< RTS pin port number
  uint8_t                    rtsPin;            ///< RTS pin number
  UARTDRV_Buffer_FifoQueue_t *rxQueue;          ///< Receive operation queue
  UARTDRV_Buffer_FifoQueue_t *txQueue;          ///< Transmit operation queue
} UARTDRV_InitLeuart_t;
#endif

#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
/// UART driver instance initialization structure.
/// Contains a number of UARTDRV configuration options.
/// It is required to initialize a driver instance.
/// This structure is passed to @ref UARTDRV_InitEuart() when initializing a UARTDRV
typedef struct {
  EUSART_TypeDef              *port;                ///< The peripheral used for EUART
  bool                        useLowFrequencyMode;  ///< Clock configuration of the EUART
  uint32_t                    baudRate;             ///< EUART baud rate
  GPIO_Port_TypeDef           txPort;               ///< Port for UART Tx pin.
  GPIO_Port_TypeDef           rxPort;               ///< Port for UART Rx pin.
  uint8_t                     txPin;                ///< Pin number for UART Tx.
  uint8_t                     rxPin;                ///< Pin number for UART Rx.
  uint8_t                     uartNum;              ///< EUART instance number.
  EUSART_Stopbits_TypeDef     stopBits;             ///< Number of stop bits
  EUSART_Parity_TypeDef       parity;               ///< Parity configuration
  EUSART_OVS_TypeDef          oversampling;         ///< Oversampling mode.
  EUSART_MajorityVote_TypeDef mvdis;                ///< Majority Vote Disable for 16x, 8x and 6x oversampling modes.
  UARTDRV_FlowControlType_t   fcType;               ///< Flow control mode
  GPIO_Port_TypeDef           ctsPort;              ///< CTS pin port number
  uint8_t                     ctsPin;               ///< CTS pin number
  GPIO_Port_TypeDef           rtsPort;              ///< RTS pin port number
  uint8_t                     rtsPin;               ///< RTS pin number
  UARTDRV_Buffer_FifoQueue_t  *rxQueue;             ///< Receive operation queue
  UARTDRV_Buffer_FifoQueue_t  *txQueue;             ///< Transmit operation queue
} UARTDRV_InitEuart_t;
#endif

/// A UART driver instance handle data structure.
/// Allocated by the application using UARTDRV.
/// Several concurrent driver instances may exist in an application. The application must
/// not modify the contents of this handle and should not depend on its values.
typedef struct UARTDRV_HandleData{
  /// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
  union {
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
    USART_TypeDef * uart;
#endif
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
    LEUART_TypeDef * leuart;
#endif
#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
    EUSART_TypeDef * euart;
#endif
    void * __reserved_space;
  } peripheral;
  uint8_t                       uartNum;           // UART instance number
  unsigned int                  txDmaCh;           // A DMA ch assigned to Tx
  unsigned int                  rxDmaCh;           // A DMA ch assigned to Rx
  DMADRV_PeripheralSignal_t     txDmaSignal;       // A DMA Tx trigger source signal
  DMADRV_PeripheralSignal_t     rxDmaSignal;       // A DMA Rx trigger source signal
  UARTDRV_FlowControlState_t    fcSelfState;       // A current self flow control state
  UARTDRV_FlowControlState_t    fcSelfCfg;         // A self flow control override configuration
  UARTDRV_FlowControlState_t    fcPeerState;       // A current peer flow control state
  GPIO_Port_TypeDef             txPort;            // A Tx pin port number
  GPIO_Port_TypeDef             rxPort;            // An Rx pin port number
  GPIO_Port_TypeDef             ctsPort;           // A CTS pin port number
  GPIO_Port_TypeDef             rtsPort;           // An RTS pin port number
  uint8_t                       txPin;             // A Tx pin number
  uint8_t                       rxPin;             // An Tx pin number
  uint8_t                       ctsPin;            // A CTS pin number
  uint8_t                       rtsPin;            // An RTS pin number
  CMU_Clock_TypeDef             uartClock;         // A clock source select
  UARTDRV_Buffer_FifoQueue_t    *rxQueue;          // A receive operation queue
  UARTDRV_Buffer_FifoQueue_t    *txQueue;          // A transmit operation queue
  volatile bool                 rxDmaActive;       // A receive DMA is currently active
  volatile bool                 txDmaActive;       // A transmit DMA is currently active
  volatile uint8_t              txDmaPaused;       // A transmit DMA pause counter
  bool                          IgnoreRestrain;    // A transmit does not respect uartdrvFlowControlOff
  bool                          hasTransmitted;    // Indicates whether the handle has transmitted data
  UARTDRV_FlowControlType_t     fcType;            // A flow control mode
  UARTDRV_UartType_t            type;              // A type of UART
  volatile int                  em1RequestCount;   // A EM1 request count for the handle
  sl_sleeptimer_timer_handle_t  delayedTxTimer;    // A timer to wait for the last byte out
  size_t                        sleep;             // Sleep state on isr return
  /// @endcond
} UARTDRV_HandleData_t;

/// Handle pointer
typedef UARTDRV_HandleData_t * UARTDRV_Handle_t;

#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
Ecode_t UARTDRV_InitUart(UARTDRV_Handle_t handle,
                         const UARTDRV_InitUart_t * initData);
#endif

#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) && !defined(_SILICON_LABS_32B_SERIES_2)
Ecode_t UARTDRV_InitLeuart(UARTDRV_Handle_t handle,
                           const UARTDRV_InitLeuart_t * initData);
#endif

#if (defined(EUART_COUNT) && (EUART_COUNT > 0)) || (defined(EUSART_COUNT) && (EUSART_COUNT > 0))
Ecode_t UARTDRV_InitEuart(UARTDRV_Handle_t handle,
                          const UARTDRV_InitEuart_t * initData);
#endif
Ecode_t UARTDRV_DeInit(UARTDRV_Handle_t handle);

UARTDRV_Status_t UARTDRV_GetPeripheralStatus(UARTDRV_Handle_t handle);

UARTDRV_Status_t UARTDRV_GetReceiveStatus(UARTDRV_Handle_t handle,
                                          uint8_t **buffer,
                                          UARTDRV_Count_t *bytesReceived,
                                          UARTDRV_Count_t *bytesRemaining);

UARTDRV_Status_t UARTDRV_GetTransmitStatus(UARTDRV_Handle_t handle,
                                           uint8_t **buffer,
                                           UARTDRV_Count_t *bytesSent,
                                           UARTDRV_Count_t *bytesRemaining);

uint8_t UARTDRV_GetReceiveDepth(UARTDRV_Handle_t handle);

uint8_t UARTDRV_GetTransmitDepth(UARTDRV_Handle_t handle);

Ecode_t UARTDRV_Transmit(UARTDRV_Handle_t handle,
                         uint8_t *data,
                         UARTDRV_Count_t count,
                         UARTDRV_Callback_t callback);

Ecode_t UARTDRV_Receive(UARTDRV_Handle_t handle,
                        uint8_t *data,
                        UARTDRV_Count_t count,
                        UARTDRV_Callback_t callback);

Ecode_t UARTDRV_TransmitB(UARTDRV_Handle_t handle,
                          uint8_t *data,
                          UARTDRV_Count_t count);

Ecode_t UARTDRV_ReceiveB(UARTDRV_Handle_t handle,
                         uint8_t *data,
                         UARTDRV_Count_t count);

Ecode_t UARTDRV_ForceTransmit(UARTDRV_Handle_t handle,
                              uint8_t *data,
                              UARTDRV_Count_t count);

UARTDRV_Count_t UARTDRV_ForceReceive(UARTDRV_Handle_t handle,
                                     uint8_t *data,
                                     UARTDRV_Count_t maxLength);

Ecode_t UARTDRV_Abort(UARTDRV_Handle_t handle, UARTDRV_AbortType_t type);

Ecode_t UARTDRV_PauseTransmit(UARTDRV_Handle_t handle);

Ecode_t UARTDRV_ResumeTransmit(UARTDRV_Handle_t handle);

UARTDRV_FlowControlState_t UARTDRV_FlowControlGetSelfStatus(UARTDRV_Handle_t handle);

UARTDRV_FlowControlState_t UARTDRV_FlowControlGetPeerStatus(UARTDRV_Handle_t handle);

Ecode_t UARTDRV_FlowControlSet(UARTDRV_Handle_t handle, UARTDRV_FlowControlState_t state);

Ecode_t UARTDRV_FlowControlSetPeerStatus(UARTDRV_Handle_t handle, UARTDRV_FlowControlState_t state);

Ecode_t UARTDRV_FlowControlIgnoreRestrain(UARTDRV_Handle_t handle);

// --------------------------------
// Deprecated items

/***************************************************************************//**
 * @brief
 *    Initialize a U(S)ART driver instance.
 *
 * @deprecated
 *    Deprecated; Use @ref UARTDRV_InitUart() instead.
 *
 * @param[out] handle  A pointer to a UARTDRV handle, refer to @ref
 *                     UARTDRV_Handle_t.
 *
 * @param[in] initData A pointer to an initialization data structure,
 *                     refer to @ref UARTDRV_InitUart_t.
 *
 * @return
 *    @ref ECODE_EMDRV_UARTDRV_OK on success. On failure, an appropriate
 *    UARTDRV @ref Ecode_t is returned.
 ******************************************************************************/
#if (defined(UART_COUNT) && (UART_COUNT > 0)) || (defined(USART_COUNT) && (USART_COUNT > 0))
__STATIC_INLINE SL_DEPRECATED_API_SDK_4_1 Ecode_t UARTDRV_Init(UARTDRV_Handle_t handle,
                                                               UARTDRV_InitUart_t *initData)
{
  return UARTDRV_InitUart(handle, initData);
}
#endif

/// Set to 1 to include flow control support.
/// @deprecated EMDRV_UARTDRV_HW_FLOW_CONTROL_ENABLE changed to
/// to EMDRV_UARTDRV_FLOW_CONTROL_ENABLE.
/// Translate to new name if old name is in use.
#if defined(EMDRV_UARTDRV_HW_FLOW_CONTROL_ENABLE)
#undef EMDRV_UARTDRV_FLOW_CONTROL_ENABLE
#define EMDRV_UARTDRV_FLOW_CONTROL_ENABLE EMDRV_UARTDRV_HW_FLOW_CONTROL_ENABLE
#endif

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT) && !defined(SL_CATALOG_KERNEL_PRESENT)
sl_power_manager_on_isr_exit_t sl_uartdrv_sleep_on_isr_exit(void);
#endif

/** @} (end addtogroup uartdrv) */

#ifdef __cplusplus
}
#endif
#endif // UARTDRV_H
