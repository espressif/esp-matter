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
 * \file device_definition.c
 * \brief This file defines exports the structures based on the peripheral
 * definitions from device_cfg.h.
 * This file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#include "device_definition.h"
#include "platform_base_address.h"


/* CMSDK GPIO driver structures */
#ifdef GPIO0_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO0_CMSDK_DEV_CFG_S = {
    .base = GPIO0_BASE_S};
struct gpio_cmsdk_dev_t GPIO0_CMSDK_DEV_S = {&(GPIO0_CMSDK_DEV_CFG_S)};
#endif
#ifdef GPIO0_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO0_CMSDK_DEV_CFG_NS = {
    .base = GPIO0_BASE_NS};
struct gpio_cmsdk_dev_t GPIO0_CMSDK_DEV_NS = {&(GPIO0_CMSDK_DEV_CFG_NS)};
#endif

#ifdef GPIO1_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO1_CMSDK_DEV_CFG_S = {
    .base = GPIO1_BASE_S};
struct gpio_cmsdk_dev_t GPIO1_CMSDK_DEV_S = {&(GPIO1_CMSDK_DEV_CFG_S)};
#endif
#ifdef GPIO1_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO1_CMSDK_DEV_CFG_NS = {
    .base = GPIO1_BASE_NS};
struct gpio_cmsdk_dev_t GPIO1_CMSDK_DEV_NS = {&(GPIO1_CMSDK_DEV_CFG_NS)};
#endif

#ifdef GPIO2_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO2_CMSDK_DEV_CFG_S = {
    .base = GPIO2_BASE_S};
struct gpio_cmsdk_dev_t GPIO2_CMSDK_DEV_S = {&(GPIO2_CMSDK_DEV_CFG_S)};
#endif
#ifdef GPIO2_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO2_CMSDK_DEV_CFG_NS = {
    .base = GPIO2_BASE_NS};
struct gpio_cmsdk_dev_t GPIO2_CMSDK_DEV_NS = {&(GPIO2_CMSDK_DEV_CFG_NS)};
#endif

#ifdef GPIO3_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO3_CMSDK_DEV_CFG_S = {
    .base = GPIO3_BASE_S};
struct gpio_cmsdk_dev_t GPIO3_CMSDK_DEV_S = {&(GPIO3_CMSDK_DEV_CFG_S)};
#endif
#ifdef GPIO3_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO3_CMSDK_DEV_CFG_NS = {
    .base = GPIO3_BASE_NS};
struct gpio_cmsdk_dev_t GPIO3_CMSDK_DEV_NS = {&(GPIO3_CMSDK_DEV_CFG_NS)};
#endif

/* ARM MPS2 IO FPGAIO driver structures */
#ifdef ARM_MPS2_IO_FPGAIO_S
static const struct arm_mps2_io_dev_cfg_t ARM_MPS2_IO_FPGAIO_DEV_CFG_S = {
    .base = MPS2_IO_FPGAIO_BASE_S,
    .type = ARM_MPS2_IO_TYPE_FPGAIO};
struct arm_mps2_io_dev_t ARM_MPS2_IO_FPGAIO_DEV_S =
                                              {&(ARM_MPS2_IO_FPGAIO_DEV_CFG_S)};
#endif
#ifdef ARM_MPS2_IO_FPGAIO_NS
static const struct arm_mps2_io_dev_cfg_t ARM_MPS2_IO_FPGAIO_DEV_CFG_NS = {
    .base = MPS2_IO_FPGAIO_BASE_NS,
    .type = ARM_MPS2_IO_TYPE_FPGAIO};
struct arm_mps2_io_dev_t ARM_MPS2_IO_FPGAIO_DEV_NS =
                                             {&(ARM_MPS2_IO_FPGAIO_DEV_CFG_NS)};
#endif

/* ARM MPS2 IO SCC driver structures */
#ifdef ARM_MPS2_IO_SCC_S
static const struct arm_mps2_io_dev_cfg_t ARM_MPS2_IO_SCC_DEV_CFG_S = {
    /*
     * MPS2 IO SCC and FPGAIO register addresses match with an offset of 4.
     */
    .base = MPS2_IO_SCC_BASE_S + 4,
    .type = ARM_MPS2_IO_TYPE_SCC};
struct arm_mps2_io_dev_t ARM_MPS2_IO_SCC_DEV_S = {&(ARM_MPS2_IO_SCC_DEV_CFG_S)};
#endif
#ifdef ARM_MPS2_IO_SCC_NS
static const struct arm_mps2_io_dev_cfg_t ARM_MPS2_IO_SCC_DEV_CFG_NS = {
    .base = MPS2_IO_SCC_BASE_NS + 4,
    .type = ARM_MPS2_IO_TYPE_SCC};
struct arm_mps2_io_dev_t ARM_MPS2_IO_SCC_DEV_NS =
                                                {&(ARM_MPS2_IO_SCC_DEV_CFG_NS)};
#endif

/* UART CMSDK driver structures */
#ifdef UART0_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_S = {
    .base = UART0_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART0_DEV_S = {
    &(UART0_CMSDK_DEV_CFG_S),
    &(UART0_CMSDK_DEV_DATA_S)
};
#endif
#ifdef UART0_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_NS = {
    .base = UART0_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART0_DEV_NS = {
    &(UART0_CMSDK_DEV_CFG_NS),
    &(UART0_CMSDK_DEV_DATA_NS)
};
#endif

#ifdef UART1_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART1_CMSDK_DEV_CFG_S = {
    .base = UART1_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART1_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART1_DEV_S = {
    &(UART1_CMSDK_DEV_CFG_S),
    &(UART1_CMSDK_DEV_DATA_S)
};
#endif
#ifdef UART1_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART1_CMSDK_DEV_CFG_NS = {
    .base = UART1_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART1_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART1_DEV_NS = {
    &(UART1_CMSDK_DEV_CFG_NS),
    &(UART1_CMSDK_DEV_DATA_NS)
};
#endif

#ifdef UART2_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART2_CMSDK_DEV_CFG_S = {
    .base = UART2_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART2_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART2_DEV_S = {
    &(UART2_CMSDK_DEV_CFG_S),
    &(UART2_CMSDK_DEV_DATA_S)
};
#endif
#ifdef UART2_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART2_CMSDK_DEV_CFG_NS = {
    .base = UART2_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART2_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART2_DEV_NS = {
    &(UART2_CMSDK_DEV_CFG_NS),
    &(UART2_CMSDK_DEV_DATA_NS)
};
#endif

#ifdef UART3_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART3_CMSDK_DEV_CFG_S = {
    .base = UART3_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART3_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART3_DEV_S = {
    &(UART3_CMSDK_DEV_CFG_S),
    &(UART3_CMSDK_DEV_DATA_S)
};
#endif
#ifdef UART3_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART3_CMSDK_DEV_CFG_NS = {
    .base = UART3_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART3_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART3_DEV_NS = {
    &(UART3_CMSDK_DEV_CFG_NS),
    &(UART3_CMSDK_DEV_DATA_NS)
};
#endif

#ifdef UART4_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART4_CMSDK_DEV_CFG_S = {
    .base = UART4_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART4_CMSDK_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART4_DEV_S = {
    &(UART4_CMSDK_DEV_CFG_S),
    &(UART4_CMSDK_DEV_DATA_S)
};
#endif
#ifdef UART4_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART4_CMSDK_DEV_CFG_NS = {
    .base = UART4_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART4_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t ARM_UART4_DEV_NS = {
    &(UART4_CMSDK_DEV_CFG_NS),
    &(UART4_CMSDK_DEV_DATA_NS)
};
#endif


/* SSE-123 PPC driver structures */
#ifdef PPC_SSE123_AHB_EXP0_S
static struct ppc_sse123_dev_cfg_t PPC_SSE123_AHB_EXP0_CFG_S = {
    .spctrl_base  = SSE123_SPCTRL_BASE_S,
    .nspctrl_base = SSE123_NSPCTRL_BASE_NS,
    .ppc_name = PPC_SSE123_AHB_EXP0};
static struct ppc_sse123_dev_data_t PPC_SSE123_AHB_EXP0_DATA_S = {
    .spctrl_ns_ppc  = 0,
    .spctrl_sp_ppc  = 0,
    .nspctrl_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse123_dev_t PPC_SSE123_AHB_EXP0_DEV_S = {
    &PPC_SSE123_AHB_EXP0_CFG_S, &PPC_SSE123_AHB_EXP0_DATA_S };
#endif

#ifdef PPC_SSE123_APB_S
static struct ppc_sse123_dev_cfg_t PPC_SSE123_APB_CFG_S = {
    .spctrl_base  = SSE123_SPCTRL_BASE_S,
    .nspctrl_base = SSE123_NSPCTRL_BASE_NS,
    .ppc_name = PPC_SSE123_APB};
static struct ppc_sse123_dev_data_t PPC_SSE123_APB_DATA_S = {
    .spctrl_ns_ppc  = 0,
    .spctrl_sp_ppc  = 0,
    .nspctrl_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse123_dev_t PPC_SSE123_APB_DEV_S = {
    &PPC_SSE123_APB_CFG_S, &PPC_SSE123_APB_DATA_S };
#endif


#ifdef PPC_SSE123_APB_EXP0_S
static struct ppc_sse123_dev_cfg_t PPC_SSE123_APB_EXP0_CFG_S = {
    .spctrl_base  = SSE123_SPCTRL_BASE_S,
    .nspctrl_base = SSE123_NSPCTRL_BASE_NS,
    .ppc_name = PPC_SSE123_APB_EXP0};
static struct ppc_sse123_dev_data_t PPC_SSE123_APB_EXP0_DATA_S = {
    .spctrl_ns_ppc  = 0,
    .spctrl_sp_ppc  = 0,
    .nspctrl_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse123_dev_t PPC_SSE123_APB_EXP0_DEV_S = {
    &PPC_SSE123_APB_EXP0_CFG_S, &PPC_SSE123_APB_EXP0_DATA_S };
#endif

#ifdef PPC_SSE123_APB_EXP1_S
static struct ppc_sse123_dev_cfg_t PPC_SSE123_APB_EXP1_CFG_S = {
    .spctrl_base  = SSE123_SPCTRL_BASE_S,
    .nspctrl_base = SSE123_NSPCTRL_BASE_NS,
    .ppc_name = PPC_SSE123_APB_EXP1};
static struct ppc_sse123_dev_data_t PPC_SSE123_APB_EXP1_DATA_S = {
    .spctrl_ns_ppc  = 0,
    .spctrl_sp_ppc  = 0,
    .nspctrl_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse123_dev_t PPC_SSE123_APB_EXP1_DEV_S = {
    &PPC_SSE123_APB_EXP1_CFG_S, &PPC_SSE123_APB_EXP1_DATA_S };
#endif

#ifdef PPC_SSE123_APB_EXP2_S
static struct ppc_sse123_dev_cfg_t PPC_SSE123_APB_EXP2_CFG_S = {
    .spctrl_base  = SSE123_SPCTRL_BASE_S,
    .nspctrl_base = SSE123_NSPCTRL_BASE_NS,
    .ppc_name = PPC_SSE123_APB_EXP2};
static struct ppc_sse123_dev_data_t PPC_SSE123_APB_EXP2_DATA_S = {
    .spctrl_ns_ppc  = 0,
    .spctrl_sp_ppc  = 0,
    .nspctrl_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse123_dev_t PPC_SSE123_APB_EXP2_DEV_S = {
    &PPC_SSE123_APB_EXP2_CFG_S, &PPC_SSE123_APB_EXP2_DATA_S };
#endif

#ifdef PPC_SSE123_APB_EXP3_S
static struct ppc_sse123_dev_cfg_t PPC_SSE123_APB_EXP3_CFG_S = {
    .spctrl_base  = SSE123_SPCTRL_BASE_S,
    .nspctrl_base = SSE123_NSPCTRL_BASE_NS,
    .ppc_name = PPC_SSE123_APB_EXP3};
static struct ppc_sse123_dev_data_t PPC_SSE123_APB_EXP3_DATA_S = {
    .spctrl_ns_ppc  = 0,
    .spctrl_sp_ppc  = 0,
    .nspctrl_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse123_dev_t PPC_SSE123_APB_EXP3_DEV_S = {
    &PPC_SSE123_APB_EXP3_CFG_S, &PPC_SSE123_APB_EXP3_DATA_S };
#endif

/* ARM MPC SIE 200 driver structures */
#ifdef MPC_ISRAM0_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM0_DEV_CFG_S = {
    .base = MPC_ISRAM_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM0_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM0_DEV_S = {
    &(MPC_ISRAM0_DEV_CFG_S),
    &(MPC_ISRAM0_DEV_DATA_S)};
#endif

#ifdef MPC_SSRAM1_S
static const struct mpc_sie200_dev_cfg_t MPC_SSRAM1_DEV_CFG_S = {
    .base = MPC_SSRAM1_BASE_S};
static struct mpc_sie200_dev_data_t MPC_SSRAM1_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_SSRAM1_DEV_S = {
    &(MPC_SSRAM1_DEV_CFG_S),
    &(MPC_SSRAM1_DEV_DATA_S)};
#endif

#ifdef MPC_SSRAM2_S
static const struct mpc_sie200_dev_cfg_t MPC_SSRAM2_DEV_CFG_S = {
    .base = MPC_SSRAM2_BASE_S};
static struct mpc_sie200_dev_data_t MPC_SSRAM2_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_SSRAM2_DEV_S = {
    &(MPC_SSRAM2_DEV_CFG_S),
    &(MPC_SSRAM2_DEV_DATA_S)};
#endif

#ifdef MPC_SSRAM3_S
static const struct mpc_sie200_dev_cfg_t MPC_SSRAM3_DEV_CFG_S = {
    .base = MPC_SSRAM3_BASE_S};
static struct mpc_sie200_dev_data_t MPC_SSRAM3_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_SSRAM3_DEV_S = {
    &(MPC_SSRAM3_DEV_CFG_S),
    &(MPC_SSRAM3_DEV_DATA_S)};
#endif

#ifdef SYSCOUNTER_CNTRL_ARMV8_M_S

#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT > \
    SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT is invalid.
#endif

static const struct syscounter_armv8_m_cntrl_dev_cfg_t
SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_S = {
    .base = SYSCNTR_CNTRL_BASE_S,
    .scale0.integer  = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT,
    .scale0.fixed_point_fraction = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT,
    .scale1.integer  = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT,
    .scale1.fixed_point_fraction = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT
};
static struct syscounter_armv8_m_cntrl_dev_data_t
SYSCOUNTER_CNTRL_ARMV8_M_DEV_DATA_S = {
    .is_initialized = false
};
struct syscounter_armv8_m_cntrl_dev_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_S = {
    &(SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_S),
    &(SYSCOUNTER_CNTRL_ARMV8_M_DEV_DATA_S)
};
#endif

#ifdef SYSCOUNTER_READ_ARMV8_M_S
static const struct syscounter_armv8_m_read_dev_cfg_t
SYSCOUNTER_READ_ARMV8_M_DEV_CFG_S = {
    .base = SYSCNTR_READ_BASE_S,
};
struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_S = {
    &(SYSCOUNTER_READ_ARMV8_M_DEV_CFG_S),
};
#endif
#ifdef SYSCOUNTER_READ_ARMV8_M_NS
static const struct syscounter_armv8_m_read_dev_cfg_t
SYSCOUNTER_READ_ARMV8_M_DEV_CFG_NS = {
    .base = SYSCNTR_READ_BASE_NS,
};
struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_NS = {
    &(SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_NS),
};
#endif

#ifdef SYSTIMER0_ARMV8_M_S
static const struct systimer_armv8_m_dev_cfg_t
SYSTIMER0_ARMV8_M_DEV_CFG_S = {
    .base = SYSTEM_TIMER0_BASE_S,
    .default_freq_hz = SYSTIMER0_ARMV8M_DEFAULT_FREQ_HZ
};
static struct systimer_armv8_m_dev_data_t
SYSTIMER0_ARMV8_M_DEV_DATA_S = {
    .is_initialized = false
};
struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_S = {
    &(SYSTIMER0_ARMV8_M_DEV_CFG_S),
    &(SYSTIMER0_ARMV8_M_DEV_DATA_S)
};
#endif

#ifdef SYSTIMER0_ARMV8_M_NS
static const struct systimer_armv8_m_dev_cfg_t
SYSTIMER0_ARMV8_M_DEV_CFG_NS = {
    .base = SYSTEM_TIMER0_BASE_NS,
    .default_freq_hz = SYSTIMER0_ARMV8M_DEFAULT_FREQ_HZ
};
static struct systimer_armv8_m_dev_data_t
SYSTIMER0_ARMV8_M_DEV_DATA_NS = {
    .is_initialized = false
};
struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_NS = {
    &(SYSTIMER0_ARMV8_M_DEV_CFG_NS),
    &(SYSTIMER0_ARMV8_M_DEV_DATA_NS)
};
#endif

#ifdef SYSTIMER1_ARMV8_M_S
static const struct systimer_armv8_m_dev_cfg_t
SYSTIMER1_ARMV8_M_DEV_CFG_S = {
    .base = SYSTEM_TIMER1_BASE_S,
    .default_freq_hz = SYSTIMER1_ARMV8M_DEFAULT_FREQ_HZ
};
static struct systimer_armv8_m_dev_data_t
SYSTIMER1_ARMV8_M_DEV_DATA_S = {
    .is_initialized = false
};
struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_S = {
    &(SYSTIMER1_ARMV8_M_DEV_CFG_S),
    &(SYSTIMER1_ARMV8_M_DEV_DATA_S)
};
#endif

#ifdef SYSTIMER1_ARMV8_M_NS
static const struct systimer_armv8_m_dev_cfg_t
SYSTIMER1_ARMV8_M_DEV_CFG_NS = {
    .base = SYSTEM_TIMER1_BASE_NS,
    .default_freq_hz = SYSTIMER1_ARMV8M_DEFAULT_FREQ_HZ
};
static struct systimer_armv8_m_dev_data_t
SYSTIMER1_ARMV8_M_DEV_DATA_NS = {
    .is_initialized = false
};
struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_NS = {
    &(SYSTIMER1_ARMV8_M_DEV_CFG_NS),
    &(SYSTIMER1_ARMV8_M_DEV_DATA_NS)
};
#endif
