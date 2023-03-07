/*
 * Copyright (c) 2016-2019 ARM Limited
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ARM_LTD_DEVICE_CFG_H__
#define __ARM_LTD_DEVICE_CFG_H__

/**
 * \file device_cfg.h
 * \brief
 * This is the default device configuration file with all peripherals
 * defined and configured to be use via the secure and/or non-secure base
 * address. This file is an example of how to define your own configuration
 * file with the peripherals required for your application.
 */

/* ARM Memory Protection Controller (MPC) */
#define MPC_CODE_SRAM1_S
#define MPC_CODE_SRAM2_S

/* ARM Peripheral Protection Controllers (PPC) */
#define APB_PPCEXP2_S

/* ARM UART */
#define DEFAULT_UART_BAUDRATE  115200
#define ARM_UART0_S
#define ARM_UART0_NS
#define ARM_UART1_S
#define ARM_UART1_NS

/* CMSDK Timers */
#define CMSDK_TIMER0_S
#define CMSDK_TIMER0_NS
#define CMSDK_TIMER1_S
#define CMSDK_TIMER1_NS

#endif  /* __ARM_LTD_DEVICE_CFG_H__ */
