#ifndef RSI_BOARD_CONFIG_H
#define RSI_BOARD_CONFIG_H

#define PIN(port_id, pin_id) port_id, pin_id


//#define RSI_HAL_RESET_PIN    PIN(C, 5)

#define SPI_BUS_PERIPHERAL   SPI1
//#define SPI1_CLOCK_PIN     PIN(A,  5)
//#define SPI1_MOSI_PIN      PIN(A,  7)
//#define SPI1_MISO_PIN      PIN(A,  6)
//#define SPI1_CS_PIN        PIN(B,  6)

#define COM_PORT_PERIPHERAL  USART2
//#define UART1_TX           PIN(A,  9)
//#define UART1_RX           PIN(A, 10)


// platforms/stm32/hal/rsi_hal_mcu_platform_init.c
//#define RSI_HAL_WAKEUP_INDICATION_PIN     PIN(C, 6)
//#define RSI_HAL_SLEEP_CONFIRM_PIN         PIN(C, 8)
//#define RSI_HAL_LP_SLEEP_CONFIRM_PIN      PIN(C, 8)

#ifndef RSI_BUS_INTERFACE_DEFINED
#define RSI_SPI_INTERFACE
#define RSI_SPI_HIGH_SPEED_ENABLE
#endif

// Declaration of MCU type
#define RSI_ARM_CM4F

#endif
