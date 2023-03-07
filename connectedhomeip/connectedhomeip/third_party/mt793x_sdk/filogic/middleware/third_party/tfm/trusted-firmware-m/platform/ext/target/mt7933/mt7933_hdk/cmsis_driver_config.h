/*
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CMSIS_DRIVER_CONFIG_H__
#define __CMSIS_DRIVER_CONFIG_H__

#include "platform_retarget_dev.h"
#include "RTE_Device.h"
#include "target_cfg.h"

#ifdef RTE_USART0
#define UART0_DEV       ARM_UART0_DEV_NS
#endif

#ifdef RTE_USART1
#ifdef SECURE_UART1
#define UART1_DEV       ARM_UART1_DEV_S
#else
#define UART1_DEV       ARM_UART1_DEV_NS
#endif
#endif

#endif  /* __CMSIS_DRIVER_CONFIG_H__ */
