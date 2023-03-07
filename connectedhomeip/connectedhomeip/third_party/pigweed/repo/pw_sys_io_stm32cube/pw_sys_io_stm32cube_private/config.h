// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#pragma once

// Defaults to USART1 on the STM32F429xx, but can be overridden.

// The USART peripheral number to use. (1 for USART1, 2 for USART2, etc.)
#ifndef PW_SYS_IO_STM32CUBE_USART_NUM
#define PW_SYS_IO_STM32CUBE_USART_NUM 1
#endif  // PW_SYS_IO_STM32CUBE_USART_NUM

// The port that the USART peripheral TX/RX pins are on. (e.g. to use A9/A10
// pins for TX and RX, respectively, set this to A)
#ifndef PW_SYS_IO_STM32CUBE_GPIO_PORT
#define PW_SYS_IO_STM32CUBE_GPIO_PORT A
#endif  // PW_SYS_IO_STM32CUBE_GPIO_PORT

// The ports the USART peripheral TX and RX pins are on (if different ports).
#ifndef PW_SYS_IO_STM32CUBE_GPIO_TX_PORT
#define PW_SYS_IO_STM32CUBE_GPIO_TX_PORT PW_SYS_IO_STM32CUBE_GPIO_PORT
#endif  // PW_SYS_IO_STM32CUBE_GPIO_TX_PORT

#ifndef PW_SYS_IO_STM32CUBE_GPIO_RX_PORT
#define PW_SYS_IO_STM32CUBE_GPIO_RX_PORT PW_SYS_IO_STM32CUBE_GPIO_PORT
#endif  // PW_SYS_IO_STM32CUBE_GPIO_RX_PORT

// The pin index to use for USART transmission within the port set by
// PW_SYS_IO_STM32CUBE_GPIO_PORT.
#ifndef PW_SYS_IO_STM32CUBE_GPIO_TX_PIN
#define PW_SYS_IO_STM32CUBE_GPIO_TX_PIN 9
#endif  // PW_SYS_IO_STM32CUBE_GPIO_TX_PIN

// The pin index to use for USART reception within the port set by
// PW_SYS_IO_STM32CUBE_GPIO_PORT.
#ifndef PW_SYS_IO_STM32CUBE_GPIO_RX_PIN
#define PW_SYS_IO_STM32CUBE_GPIO_RX_PIN 10
#endif  // PW_SYS_IO_STM32CUBE_GPIO_RX_PIN

// The Alternate Function to use for configuring USART pins.
#ifndef PW_SYS_IO_STM32CUBE_GPIO_AF
#define PW_SYS_IO_STM32CUBE_GPIO_AF 7
#endif  // PW_SYS_IO_STM32CUBE_GPIO_AF

// The type of this peripheral. "USART" or "UART".
#ifndef PW_SYS_IO_STM32CUBE_USART_PREFIX
#define PW_SYS_IO_STM32CUBE_USART_PREFIX USART
#endif  // PW_SYS_IO_STM32CUBE_GPIO_AF
