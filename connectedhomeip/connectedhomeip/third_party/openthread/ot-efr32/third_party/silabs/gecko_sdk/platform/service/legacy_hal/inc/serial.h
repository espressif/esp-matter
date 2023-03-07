/***************************************************************************//**
 * @file
 * @brief Serial hardware abstraction layer interfaces.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SERIAL_H
#define SERIAL_H

#include <stdarg.h>

#ifdef CORTEXM3_EFM32_MICRO
  #include "em_usart.h"
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Assign numerical values for variables that hold Baud Rate
 * parameters.
 */
enum SerialBaudRate
#else
  #ifndef DEFINE_BAUD
  #define DEFINE_BAUD(num) BAUD_##num
  #endif
  #ifdef CORTEXM3_EFM32_MICRO
typedef uint32_t SerialBaudRate;
  #else
typedef uint8_t SerialBaudRate;
  #endif
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef CORTEXM3_EFM32_MICRO
{
  DEFINE_BAUD(300) = 300,  // BAUD_300
  DEFINE_BAUD(600) = 600,  // BAUD_600
  DEFINE_BAUD(900) = 900,  // etc...
  DEFINE_BAUD(1200) = 1200,
  DEFINE_BAUD(2400) = 2400,
  DEFINE_BAUD(4800) = 4800,
  DEFINE_BAUD(9600) = 9600,
  DEFINE_BAUD(14400) = 14400,
  DEFINE_BAUD(19200) = 19200,
  DEFINE_BAUD(28800) = 28800,
  DEFINE_BAUD(38400) = 38400,
  DEFINE_BAUD(50000) = 50000,
  DEFINE_BAUD(57600) = 57600,
  DEFINE_BAUD(76800) = 76800,
  DEFINE_BAUD(100000) = 100000,
  DEFINE_BAUD(115200) = 115200
};
#else //CORTEXM3_EFM32_MICRO
{
  DEFINE_BAUD(300) = 0,  // BAUD_300
  DEFINE_BAUD(600) = 1,  // BAUD_600
  DEFINE_BAUD(900) = 2,  // etc...
  DEFINE_BAUD(1200) = 3,
  DEFINE_BAUD(2400) = 4,
  DEFINE_BAUD(4800) = 5,
  DEFINE_BAUD(9600) = 6,
  DEFINE_BAUD(14400) = 7,
  DEFINE_BAUD(19200) = 8,
  DEFINE_BAUD(28800) = 9,
  DEFINE_BAUD(38400) = 10,
  DEFINE_BAUD(50000) = 11,
  DEFINE_BAUD(57600) = 12,
  DEFINE_BAUD(76800) = 13,
  DEFINE_BAUD(100000) = 14,
  DEFINE_BAUD(115200) = 15,
  DEFINE_BAUD(230400) = 16,   /*<! define higher baud rates for the EM2XX and EM3XX */
  DEFINE_BAUD(460800) = 17,   /*<! Note: receiving data at baud rates > 115200 */
  DEFINE_BAUD(CUSTOM) = 18    /*<! may not be reliable due to interrupt latency */
};
#endif //CORTEXM3_EFM32_MICRO

#ifdef  CORTEXM3_EFM32_MICRO

typedef enum COM_Port {
  // Legacy COM port defines
  COM_VCP = 0,
  COM_USART0 = 1,
  COM_USART1 = 2,
  COM_USART2 = 3,

  // VCP
  comPortVcp = 0x10,

  // USARTs
  comPortUsart0 = 0x20,
  comPortUsart1 = 0x21,
  comPortUsart2 = 0x22,
  comPortUsart3 = 0x23,
} COM_Port_t;

typedef USART_Parity_TypeDef SerialParity;
#define PARITY_NONE usartNoParity
#define PARITY_ODD  usartOddParity
#define PARITY_EVEN usartEvenParity

#else//!CORTEXM3_EFM32_MICRO

#ifdef DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Assign numerical values for the types of parity.
 * Use for variables that hold Parity parameters.
 */
enum SerialParity
#else
  #ifndef DEFINE_PARITY
  #define DEFINE_PARITY(val) PARITY_##val
  #endif
typedef uint8_t SerialParity;
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  DEFINE_PARITY(NONE) = 0U,  // PARITY_NONE
  DEFINE_PARITY(ODD) = 1U,   // PARITY_ODD
  DEFINE_PARITY(EVEN) = 2U   // PARITY_EVEN
};

#endif//CORTEXM3_EFM32_MICRO
#include "ember-types.h" // needed to define EmberStatus

EmberStatus emberSerialInit(uint8_t port,
                            SerialBaudRate rate,
                            SerialParity parity,
                            uint8_t stopBits);

EmberStatus emberSerialReadByte(uint8_t port, uint8_t *dataByte);

EmberStatus emberSerialReadData(uint8_t port,
                                uint8_t *data,
                                uint16_t length,
                                uint16_t *bytesRead);

EmberStatus emberSerialWriteByte(uint8_t port, uint8_t dataByte);

EmberStatus emberSerialPrintf(uint8_t port, const char * formatString, ...);

EmberStatus emberSerialPrintfLine(uint8_t port, const char * formatString, ...);

EmberStatus emberSerialPrintCarriageReturn(uint8_t port);

EmberStatus emberSerialPrintfVarArg(uint8_t port, const char * formatString, va_list ap);

EmberStatus emberSerialWaitSend(uint8_t port);

EmberStatus emberSerialGuaranteedPrintf(uint8_t port, const char * formatString, ...);

bool halInternalUartXonRefreshDone(uint8_t port);

bool halInternalUartTxIsIdle(uint8_t port);

bool halInternalUartFlowControlRxIsEnabled(uint8_t port);

#endif // SERIAL_H
