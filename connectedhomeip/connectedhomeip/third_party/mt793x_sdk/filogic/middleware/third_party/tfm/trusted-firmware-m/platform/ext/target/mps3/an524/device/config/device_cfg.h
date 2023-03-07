/*
 * Copyright (c) 2019 Arm Limited. All rights reserved.
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
 * This is the device configuration file with only used peripherals
 * defined and configured via the secure and/or non-secure base address.
 */

/* ARM Memory Protection Controller (MPC) */
#define MPC_ISRAM0_S
#define MPC_ISRAM1_S
#define MPC_ISRAM2_S
#define MPC_ISRAM3_S
#define MPC_QSPI_S

/* ARM Peripheral Protection Controllers (PPC) */
#define AHB_PPCEXP0_S
#define APB_PPC0_S
#define APB_PPC1_S
#define APB_PPCEXP0_S
#define APB_PPCEXP1_S
#define APB_PPCEXP2_S

/* ARM UART CMSDK */
#define DEFAULT_UART_BAUDRATE  115200
#define UART0_CMSDK_S
#define UART0_CMSDK_NS
#define UART1_CMSDK_S
#define UART1_CMSDK_NS

/* CMSDK Timers */
#define CMSDK_TIMER0_S
#define CMSDK_TIMER0_NS
#define CMSDK_TIMER1_S
#define CMSDK_TIMER1_NS

#endif  /* __ARM_LTD_DEVICE_CFG_H__ */
