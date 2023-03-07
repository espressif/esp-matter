/*
 * Copyright (c) 2019 Arm Limited
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
 * \file device_definition.h
 * \brief The structure definitions in this file are exported based on the
 * peripheral definitions from device_cfg.h.
 * This file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#ifndef __DEVICE_DEFINITION_H__
#define __DEVICE_DEFINITION_H__

#include "device_cfg.h"

/* ======= Defines peripheral configuration structures ======= */
/* CMSDK GPIO driver structures */
#ifdef GPIO0_CMSDK_S
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO0_CMSDK_DEV_S;
#endif
#ifdef GPIO0_CMSDK_NS
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO0_CMSDK_DEV_NS;
#endif

#ifdef GPIO1_CMSDK_S
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO1_CMSDK_DEV_S;
#endif
#ifdef GPIO1_CMSDK_NS
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO1_CMSDK_DEV_NS;
#endif

#ifdef GPIO2_CMSDK_S
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO2_CMSDK_DEV_S;
#endif
#ifdef GPIO2_CMSDK_NS
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO2_CMSDK_DEV_NS;
#endif

#ifdef GPIO3_CMSDK_S
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO3_CMSDK_DEV_S;
#endif
#ifdef GPIO3_CMSDK_NS
#include "gpio_cmsdk_drv.h"
extern struct gpio_cmsdk_dev_t GPIO3_CMSDK_DEV_NS;
#endif

/* ARM MPS2 IO FPGAIO driver structures */
#ifdef ARM_MPS2_IO_FPGAIO_S
#include "arm_mps2_io_drv.h"
extern struct arm_mps2_io_dev_t ARM_MPS2_IO_FPGAIO_DEV_S;
#endif
#ifdef ARM_MPS2_IO_FPGAIO_NS
#include "arm_mps2_io_drv.h"
extern struct arm_mps2_io_dev_t ARM_MPS2_IO_FPGAIO_DEV_NS;
#endif

/* ARM MPS2 IO SCC driver structures */
#ifdef ARM_MPS2_IO_SCC_S
#include "arm_mps2_io_drv.h"
extern struct arm_mps2_io_dev_t ARM_MPS2_IO_SCC_DEV_S;
#endif
#ifdef ARM_MPS2_IO_SCC_NS
#include "arm_mps2_io_drv.h"
extern struct arm_mps2_io_dev_t ARM_MPS2_IO_SCC_DEV_NS;
#endif

/* UART CMSDK driver structures */
#ifdef UART0_CMSDK_S
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART0_DEV_S;
#endif
#ifdef UART0_CMSDK_NS
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART0_DEV_NS;
#endif

#ifdef UART1_CMSDK_S
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART1_DEV_S;
#endif
#ifdef UART1_CMSDK_NS
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART1_DEV_NS;
#endif

#ifdef UART2_CMSDK_S
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART2_DEV_S;
#endif
#ifdef UART2_CMSDK_NS
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART2_DEV_NS;
#endif

#ifdef UART3_CMSDK_S
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART3_DEV_S;
#endif
#ifdef UART3_CMSDK_NS
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART3_DEV_NS;
#endif

#ifdef UART4_CMSDK_S
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART4_DEV_S;
#endif
#ifdef UART4_CMSDK_NS
#include "uart_cmsdk_drv.h"
extern struct uart_cmsdk_dev_t ARM_UART4_DEV_NS;
#endif

/* SSE-123 PPC driver structures */
#ifdef PPC_SSE123_AHB_EXP0_S
#include "ppc_sse123_drv.h"
extern struct ppc_sse123_dev_t PPC_SSE123_AHB_EXP0_DEV_S;
#endif

#ifdef PPC_SSE123_APB_S
#include "ppc_sse123_drv.h"
extern struct ppc_sse123_dev_t PPC_SSE123_APB_DEV_S;
#endif

#ifdef PPC_SSE123_APB_EXP0_S
#include "ppc_sse123_drv.h"
extern struct ppc_sse123_dev_t PPC_SSE123_APB_EXP0_DEV_S;
#endif

#ifdef PPC_SSE123_APB_EXP1_S
#include "ppc_sse123_drv.h"
extern struct ppc_sse123_dev_t PPC_SSE123_APB_EXP1_DEV_S;
#endif

#ifdef PPC_SSE123_APB_EXP2_S
#include "ppc_sse123_drv.h"
extern struct ppc_sse123_dev_t PPC_SSE123_APB_EXP2_DEV_S;
#endif

#ifdef PPC_SSE123_APB_EXP3_S
#include "ppc_sse123_drv.h"
extern struct ppc_sse123_dev_t PPC_SSE123_APB_EXP3_DEV_S;
#endif

/* ARM MPC SIE 200 driver structures */
#ifdef MPC_ISRAM0_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_ISRAM0_DEV_S;
#endif

#ifdef MPC_SSRAM1_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_SSRAM1_DEV_S;
#endif

#ifdef MPC_SSRAM2_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_SSRAM2_DEV_S;
#endif

#ifdef MPC_SSRAM3_S
#include "mpc_sie200_drv.h"
extern struct mpc_sie200_dev_t MPC_SSRAM3_DEV_S;
#endif

#ifdef SYSCOUNTER_CNTRL_ARMV8_M_S
#include "syscounter_armv8-m_cntrl_drv.h"
extern struct syscounter_armv8_m_cntrl_dev_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_S;
#endif

#ifdef SYSCOUNTER_READ_ARMV8_M_S
#include "syscounter_armv8-m_read_drv.h"
extern struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_S;
#endif
#ifdef SYSCOUNTER_READ_ARMV8_M_NS
#include "syscounter_armv8-m_read_drv.h"
extern struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_NS;
#endif

#ifdef SYSTIMER0_ARMV8_M_S
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_S;
#endif
#ifdef SYSTIMER0_ARMV8_M_NS
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_NS;
#endif

#ifdef SYSTIMER1_ARMV8_M_S
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_S;
#endif
#ifdef SYSTIMER1_ARMV8_M_NS
#include "systimer_armv8-m_drv.h"
extern struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_NS;
#endif

#endif  /* __DEVICE_DEFINITION_H__ */
