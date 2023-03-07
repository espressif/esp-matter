/*
 * Copyright (c) 2016-2019 Arm Limited
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

#ifndef __ARM_LTD_SSE_200_AWS_RETARGET_DEV_H__
#define __ARM_LTD_SSE_200_AWS_RETARGET_DEV_H__

#include "device_cfg.h"

/* ======= Includes generic driver headers ======= */
#include "mpc_sie_drv.h"
#include "ppc_sse200_drv.h"
#include "uart_cmsdk_drv.h"
#include "timer_cmsdk/timer_cmsdk_drv.h"

/* ======= Defines peripheral configuration structures ======= */
/* ARM UART driver structures */
#ifdef UART0_CMSDK_S
extern struct uart_cmsdk_dev_t UART0_CMSDK_DEV_S;
#endif
#ifdef UART0_CMSDK_NS
extern struct uart_cmsdk_dev_t UART0_CMSDK_DEV_NS;
#endif

/* ARM PPC driver structures */
#ifdef AHB_PPC0_S
extern struct ppc_sse200_dev_t AHB_PPC0_DEV_S;
#endif

#ifdef AHB_PPCEXP0_S
extern struct ppc_sse200_dev_t AHB_PPCEXP0_DEV_S;
#endif

#ifdef AHB_PPCEXP1_S
extern struct ppc_sie200_dev_t AHB_PPCEXP1_DEV_S;
#endif

#ifdef AHB_PPCEXP2_S
extern struct ppc_sse200_dev_t AHB_PPCEXP2_DEV_S;
#endif

#ifdef AHB_PPCEXP3_S
extern struct ppc_sse200_dev_t AHB_PPCEXP3_DEV_S;
#endif

#ifdef APB_PPC0_S
extern struct ppc_sse200_dev_t APB_PPC0_DEV_S;
#endif

#ifdef APB_PPC1_S
extern struct ppc_sse200_dev_t APB_PPC1_DEV_S;
#endif

#ifdef APB_PPCEXP0_S
extern struct ppc_sse200_dev_t APB_PPCEXP0_DEV_S;
#endif

#ifdef APB_PPCEXP1_S
extern struct ppc_sse200_dev_t APB_PPCEXP1_DEV_S;
#endif

#ifdef APB_PPCEXP2_S
extern struct ppc_sse200_dev_t APB_PPCEXP2_DEV_S;
#endif

#ifdef APB_PPCEXP3_S
extern struct ppc_sse200_dev_t APB_PPCEXP3_DEV_S;
#endif

/* CMSDK Timer driver structures */
#ifdef CMSDK_TIMER0_S
extern struct timer_cmsdk_dev_t CMSDK_TIMER0_DEV_S;
#endif
#ifdef CMSDK_TIMER0_NS
extern struct timer_cmsdk_dev_t CMSDK_TIMER0_DEV_NS;
#endif

#ifdef CMSDK_TIMER1_S
extern struct timer_cmsdk_dev_t CMSDK_TIMER1_DEV_S;
#endif
#ifdef CMSDK_TIMER1_NS
extern struct timer_cmsdk_dev_t CMSDK_TIMER1_DEV_NS;
#endif

/* ARM MPC SSE 200 driver structures */
#ifdef MPC_ISRAM0_S
extern struct mpc_sie_dev_t MPC_ISRAM0_DEV_S;
#endif

#ifdef MPC_ISRAM1_S
extern struct mpc_sie_dev_t MPC_ISRAM1_DEV_S;
#endif

#ifdef MPC_ISRAM2_S
extern struct mpc_sie_dev_t MPC_ISRAM2_DEV_S;
#endif

#ifdef MPC_ISRAM3_S
extern struct mpc_sie_dev_t MPC_ISRAM3_DEV_S;
#endif

#ifdef MPC_SSRAM1_S
extern struct mpc_sie_dev_t MPC_SSRAM1_DEV_S;
#endif

#ifdef MPC_SSRAM2_S
extern struct mpc_sie_dev_t MPC_SSRAM2_DEV_S;
#endif

#ifdef MPC_SSRAM3_S
extern struct mpc_sie_dev_t MPC_SSRAM3_DEV_S;
#endif

#endif  /* __ARM_LTD_SSE_200_AWS_RETARGET_DEV_H__ */
