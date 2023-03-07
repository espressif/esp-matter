#ifndef MX25FLASHHALCONFIG_H
#define MX25FLASHHALCONFIG_H

#include "hal-config.h"

#ifdef BSP_EXTFLASH_USART

#if BSP_EXTFLASH_USART == HAL_SPI_PORT_USART0
// USART0
  #define MX25_USART                USART0
  #define MX25_USART_CLK            cmuClock_USART0
  #define MX25_USART_ROUTE          GPIO->USARTROUTE[0]
#elif BSP_EXTFLASH_USART == HAL_SPI_PORT_USART1
// USART1
  #define MX25_USART                USART1
  #define MX25_USART_CLK            cmuClock_USART1
  #define MX25_USART_ROUTE          GPIO->USARTROUTE[1]
#elif BSP_EXTFLASH_USART == HAL_SPI_PORT_USART2
// USART2
  #define MX25_USART                USART2
  #define MX25_USART_CLK            cmuClock_USART2
  #define MX25_USART_ROUTE          GPIO->USARTROUTE[2]
#elif BSP_EXTFLASH_USART == HAL_SPI_PORT_USART3
// USART3
  #define MX25_USART                USART3
  #define MX25_USART_CLK            cmuClock_USART3
  #define MX25_USART_ROUTE          GPIO->USARTROUTE[3]
#elif BSP_EXTFLASH_USART == HAL_SPI_PORT_USART4
// USART4
  #define MX25_USART                USART4
  #define MX25_USART_CLK            cmuClock_USART4
  #define MX25_USART_ROUTE          GPIO->USARTROUTE[4]
#elif BSP_EXTFLASH_USART == HAL_SPI_PORT_USART5
// USART5
  #define MX25_USART                USART5
  #define MX25_USART_CLK            cmuClock_USART5
  #define MX25_USART_ROUTE          GPIO->USARTROUTE[5]
#else
  #error "SPI flash config: Unknown USART selection"
#endif

#define MX25_PORT_MOSI          BSP_EXTFLASH_MOSI_PORT
#define MX25_PIN_MOSI           BSP_EXTFLASH_MOSI_PIN
#define MX25_LOC_TX             BSP_EXTFLASH_MOSI_LOC
#define MX25_PORT_MISO          BSP_EXTFLASH_MISO_PORT
#define MX25_PIN_MISO           BSP_EXTFLASH_MISO_PIN
#define MX25_LOC_RX             BSP_EXTFLASH_MISO_LOC
#define MX25_PORT_SCLK          BSP_EXTFLASH_CLK_PORT
#define MX25_PIN_SCLK           BSP_EXTFLASH_CLK_PIN
#define MX25_LOC_SCLK           BSP_EXTFLASH_CLK_LOC
#define MX25_PORT_CS            BSP_EXTFLASH_CS_PORT
#define MX25_PIN_CS             BSP_EXTFLASH_CS_PIN

#define MX25_BAUDRATE           HAL_EXTFLASH_FREQUENCY

#endif //BSP_EXTFLASH_USART

#endif // MX25FLASHHALCONFIG_H
