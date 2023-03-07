#ifndef __MX25_CONFIG_H
#define __MX25_CONFIG_H

#include "em_device.h"
#include "em_gpio.h"

#define MX25_PORT_MOSI         gpioPortC
#define MX25_PIN_MOSI          6
#define MX25_PORT_MISO         gpioPortC
#define MX25_PIN_MISO          7
#define MX25_PORT_SCLK         gpioPortC
#define MX25_PIN_SCLK          8
#define MX25_PORT_CS           gpioPortA
#define MX25_PIN_CS            4

#define MX25_USART             USART1
#define MX25_USART_CLK         cmuClock_USART1

#endif
