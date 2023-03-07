/*
 * Copyright (c) 2016-2018 ARM Limited
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

/**
 * \file platform_retarget_dev.h
 * \brief The structure definitions in this file are exported based on the peripheral
 * definitions from device_cfg.h.
 * This retarget file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#ifndef __ARM_LTD_AN519_RETARGET_DEV_H__
#define __ARM_LTD_AN519_RETARGET_DEV_H__

#include "device_cfg.h"


/* ======= Defines peripheral configuration structures ======= */
/* ======= and includes generic driver headers if necessary ======= */
/* ARM UART driver structures */
#ifdef ARM_UART0_S
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART0_DEV_S;
#endif
#ifdef ARM_UART0_NS
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART0_DEV_NS;
#endif

#ifdef ARM_UART1_S
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART1_DEV_S;
#endif
#ifdef ARM_UART1_NS
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART1_DEV_NS;
#endif

#ifdef ARM_UART2_S
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART2_DEV_S;
#endif
#ifdef ARM_UART2_NS
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART2_DEV_NS;
#endif

#ifdef ARM_UART3_S
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART3_DEV_S;
#endif
#ifdef ARM_UART3_NS
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART3_DEV_NS;
#endif

#ifdef ARM_UART4_S
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART4_DEV_S;
#endif
#ifdef ARM_UART4_NS
#include "arm_uart_drv.h"
extern struct arm_uart_dev_t ARM_UART4_DEV_NS;
#endif

/* ARM PPC driver structures */
#ifdef AHB_PPC0_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t AHB_PPC0_DEV_S;
#endif

#ifdef AHB_PPCEXP0_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t AHB_PPCEXP0_DEV_S;
#endif

#ifdef AHB_PPCEXP1_S
#include "ppc_sse200_drv.h"
extern struct ppc_sie200_dev_t AHB_PPCEXP1_DEV_S;
#endif

#ifdef AHB_PPCEXP2_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t AHB_PPCEXP2_DEV_S;
#endif

#ifdef AHB_PPCEXP3_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t AHB_PPCEXP3_DEV_S;
#endif

#ifdef APB_PPC0_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t APB_PPC0_DEV_S;
#endif

#ifdef APB_PPC1_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t APB_PPC1_DEV_S;
#endif

#ifdef APB_PPCEXP0_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t APB_PPCEXP0_DEV_S;
#endif

#ifdef APB_PPCEXP1_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t APB_PPCEXP1_DEV_S;
#endif

#ifdef APB_PPCEXP2_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t APB_PPCEXP2_DEV_S;
#endif

#ifdef APB_PPCEXP3_S
#include "ppc_sse200_drv.h"
extern struct ppc_sse200_dev_t APB_PPCEXP3_DEV_S;
#endif

/* CMSDK Timer driver structures */
#ifdef CMSDK_TIMER0_S
#include "timer_cmsdk/timer_cmsdk.h"
extern struct cmsdk_timer_dev_t CMSDK_TIMER0_DEV_S;
#endif
#ifdef CMSDK_TIMER0_NS
#include "timer_cmsdk/timer_cmsdk.h"
extern struct cmsdk_timer_dev_t CMSDK_TIMER0_DEV_NS;
#endif

#ifdef CMSDK_TIMER1_S
#include "timer_cmsdk/timer_cmsdk.h"
extern struct cmsdk_timer_dev_t CMSDK_TIMER1_DEV_S;
#endif
#ifdef CMSDK_TIMER1_NS
#include "timer_cmsdk/timer_cmsdk.h"
extern struct cmsdk_timer_dev_t CMSDK_TIMER1_DEV_NS;
#endif

/* ARM MPC SSE 200 driver structures */
#ifdef MPC_ISRAM0_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_ISRAM0_DEV_S;
#endif

#ifdef MPC_ISRAM1_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_ISRAM1_DEV_S;
#endif

#ifdef MPC_ISRAM2_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_ISRAM2_DEV_S;
#endif

#ifdef MPC_ISRAM3_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_ISRAM3_DEV_S;
#endif

#ifdef MPC_CODE_SRAM1_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_CODE_SRAM1_DEV_S;
#endif

#ifdef MPC_CODE_SRAM2_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_CODE_SRAM2_DEV_S;
#endif

#ifdef MPC_CODE_SRAM3_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_CODE_SRAM3_DEV_S;
#endif

#endif  /* __ARM_LTD_AN519_RETARGET_DEV_H__ */
