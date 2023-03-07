#ifndef __MX25_CONFIG_H
#define __MX25_CONFIG_H

#include "em_device.h"
#include "em_gpio.h"

#define MX25_PORT_MOSI         gpioPortB
#define MX25_PIN_MOSI          11
#define MX25_PORT_MISO         gpioPortB
#define MX25_PIN_MISO          12
#define MX25_PORT_SCLK         gpioPortB
#define MX25_PIN_SCLK          13
#define MX25_PORT_CS           gpioPortD
#define MX25_PIN_CS            13

#define MX25_USART             USART1
#define MX25_USART_CLK         cmuClock_USART1

#endif
