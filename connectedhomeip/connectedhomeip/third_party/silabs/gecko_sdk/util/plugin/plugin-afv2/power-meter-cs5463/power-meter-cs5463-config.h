// *****************************************************************************
// * power-meter-config.h
// *
// * Power Meter Configuration Header
// *
// * Copyright 2017 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#ifndef __POWER_METER_CS5463_CONFIG_H__
#define __POWER_METER_CS5463_CONFIG_H__
#if defined(CORTEXM3_EFR32)

#ifndef BSP_POWER_METER_USART_PORT
  #define BSP_POWER_METER_USART_PORT HAL_SPI_PORT_USART1
#endif

// Power meter USART selection
#if (BSP_POWER_METER_USART_PORT == HAL_SPI_PORT_USART1)
  #define POWER_METER_USART           USART1
  #define POWER_METER_MISO_LOC        BSP_USART1_MISO_LOC
  #define POWER_METER_MOSI_LOC        BSP_USART1_MOSI_LOC
  #define POWER_METER_CLK_LOC         BSP_USART1_CLK_LOC
  #define POWER_METER_CS_LOC          BSP_USART1_CS_LOC
  #define POWER_METER_CS_PIN          BSP_USART1_CS_PIN
  #define POWER_METER_CS_PORT         BSP_USART1_CS_PORT
#elif (BSP_POWER_METER_USART_PORT == HAL_SPI_PORT_USART2)
  #define POWER_METER_USART           USART2
  #define POWER_METER_MISO_LOC        BSP_USART2_MISO_LOC
  #define POWER_METER_MOSI_LOC        BSP_USART2_MOSI_LOC
  #define POWER_METER_CLK_LOC         BSP_USART2_CLK_LOC
  #define POWER_METER_CS_LOC          BSP_USART2_CS_LOC
  #define POWER_METER_CS_PIN          BSP_USART2_CS_PIN
  #define POWER_METER_CS_PORT         BSP_USART2_CS_PORT
#else
  #error EFR power meter not enabled for ports other than USART1 and USART2.
#endif

// Configuration data for SPI Master Init
#define SPI_CONFIG                                                     \
  {                                                                    \
    POWER_METER_USART,          /* USART port                       */ \
    POWER_METER_MOSI_LOC,       /* USART Tx/MOSI pin location number*/ \
    POWER_METER_MISO_LOC,       /* USART Rx/MISO pin location number*/ \
    POWER_METER_CLK_LOC,        /* USART Clk pin location number    */ \
    POWER_METER_CS_LOC,         /* USART Cs pin location number     */ \
    50000,                      /* Bitrate                          */ \
    8,                          /* Frame length                     */ \
    0xFFFFFFFF,                 /* Dummy tx value for rx only funcs */ \
    spidrvMaster,               /* SPI mode                         */ \
    spidrvBitOrderMsbFirst,     /* Bit order on bus                 */ \
    spidrvClockMode0,           /* SPI clock/phase mode             */ \
    spidrvCsControlApplication, /* CS controlled by aplication      */ \
    spidrvSlaveStartImmediate   /* Slave start transfers immediately*/ \
  }
#define SPI_NSEL_PORT POWER_METER_CS_PORT
#define SPI_NSEL_BIT POWER_METER_CS_PIN

#endif //defined(CORTEXM3_EFR32)
#endif // __POWER_METER_CS5463_CONFIG_H__
