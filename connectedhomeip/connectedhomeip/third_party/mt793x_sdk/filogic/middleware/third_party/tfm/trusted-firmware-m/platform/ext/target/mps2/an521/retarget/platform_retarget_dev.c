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
 * \file platform_retarget_dev.c
 * \brief This file defines exports the structures based on the peripheral
 * definitions from device_cfg.h.
 * This retarget file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#include "platform_retarget_dev.h"
#include "platform_retarget.h"
#include "system_cmsdk_mps2_an521.h"
#include "platform/include/tfm_plat_defs.h"

/* ARM UART driver structures */
#ifdef ARM_UART0_S
static const struct arm_uart_dev_cfg_t ARM_UART0_DEV_CFG_S = {
    .base = UART0_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART0_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART0_DEV_S = {&(ARM_UART0_DEV_CFG_S),
                                         &(ARM_UART0_DEV_DATA_S)};
#endif
#ifdef ARM_UART0_NS
static const struct arm_uart_dev_cfg_t ARM_UART0_DEV_CFG_NS = {
    .base = UART0_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART0_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART0_DEV_NS = {&(ARM_UART0_DEV_CFG_NS),
                                          &(ARM_UART0_DEV_DATA_NS)};
#endif

#ifdef ARM_UART1_S
static const struct arm_uart_dev_cfg_t ARM_UART1_DEV_CFG_S = {
    .base = UART1_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART1_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART1_DEV_S = {&(ARM_UART1_DEV_CFG_S),
                                         &(ARM_UART1_DEV_DATA_S)};
#endif
#ifdef ARM_UART1_NS
static const struct arm_uart_dev_cfg_t ARM_UART1_DEV_CFG_NS = {
    .base = UART1_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART1_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART1_DEV_NS = {&(ARM_UART1_DEV_CFG_NS),
                                          &(ARM_UART1_DEV_DATA_NS)};
#endif

#ifdef ARM_UART2_S
static const struct arm_uart_dev_cfg_t ARM_UART2_DEV_CFG_S = {
    .base = UART2_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART2_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART2_DEV_S = {&(ARM_UART2_DEV_CFG_S),
                                         &(ARM_UART2_DEV_DATA_S)};
#endif
#ifdef ARM_UART2_NS
static const struct arm_uart_dev_cfg_t ARM_UART2_DEV_CFG_NS = {
    .base = UART2_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART2_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART2_DEV_NS = {&(ARM_UART2_DEV_CFG_NS),
                                          &(ARM_UART2_DEV_DATA_NS)};
#endif

#ifdef ARM_UART3_S
static const struct arm_uart_dev_cfg_t ARM_UART3_DEV_CFG_S = {
    .base = UART3_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART3_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART3_DEV_S = {&(ARM_UART3_DEV_CFG_S),
                                         &(ARM_UART3_DEV_DATA_S)};
#endif
#ifdef ARM_UART3_NS
static const struct arm_uart_dev_cfg_t ARM_UART3_DEV_CFG_NS = {
    .base = UART3_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART3_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART3_DEV_NS = {&(ARM_UART3_DEV_CFG_NS),
                                          &(ARM_UART3_DEV_DATA_NS)};
#endif

#ifdef ARM_UART4_S
static const struct arm_uart_dev_cfg_t ARM_UART4_DEV_CFG_S = {
    .base = UART4_BASE_S,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART4_DEV_DATA_S = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART4_DEV_S = {&(ARM_UART4_DEV_CFG_S),
                                         &(ARM_UART4_DEV_DATA_S)};
#endif
#ifdef ARM_UART4_NS
static const struct arm_uart_dev_cfg_t ARM_UART4_DEV_CFG_NS = {
    .base = UART4_BASE_NS,
    .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct arm_uart_dev_data_t ARM_UART4_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0};
struct arm_uart_dev_t ARM_UART4_DEV_NS = {&(ARM_UART4_DEV_CFG_NS),
                                          &(ARM_UART4_DEV_DATA_NS)};
#endif

/* ARM PPC SIE 200 driver structures */
#ifdef AHB_PPC0_S
static struct ppc_sse200_dev_cfg_t AHB_PPC0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPC0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPC0_DEV_S = {
    &AHB_PPC0_DEV_CFG_S, &AHB_PPC0_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP0_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPCEXP0_DEV_S = {
    &AHB_PPCEXP0_DEV_CFG_S, &AHB_PPCEXP0_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP1_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP1_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP1_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sie200_dev_t AHB_PPCEXP1_DEV_S = {
    &AHB_PPCEXP1_DEV_CFG_S, &AHB_PPCEXP1_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP2_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP2_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP2_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPCEXP2_DEV_S = {
    &AHB_PPCEXP2_DEV_CFG_S, &AHB_PPCEXP2_DEV_DATA_S };
#endif

#ifdef AHB_PPCEXP3_S
static struct ppc_sse200_dev_cfg_t AHB_PPCEXP3_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t AHB_PPCEXP3_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t AHB_PPCEXP3_DEV_S = {
    &AHB_PPCEXP3_DEV_CFG_S, &AHB_PPCEXP3_DEV_DATA_S };
#endif

#ifdef APB_PPC0_S
static struct ppc_sse200_dev_cfg_t APB_PPC0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPC0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPC0_DEV_S = {
    &APB_PPC0_DEV_CFG_S, &APB_PPC0_DEV_DATA_S };
#endif

#ifdef APB_PPC1_S
static struct ppc_sse200_dev_cfg_t APB_PPC1_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPC1_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPC1_DEV_S = {
    &APB_PPC1_DEV_CFG_S, &APB_PPC1_DEV_DATA_S};
#endif

#ifdef APB_PPCEXP0_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP0_DEV_CFG_S = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP0_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP0_DEV_S = {
    &APB_PPCEXP0_DEV_CFG_S, &APB_PPCEXP0_DEV_DATA_S };
#endif

#ifdef APB_PPCEXP1_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP1_DEV_CFG = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP1_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP1_DEV_S = {
    &APB_PPCEXP1_DEV_CFG, &APB_PPCEXP1_DEV_DATA_S };
#endif

#ifdef APB_PPCEXP2_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP2_DEV_CFG = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP2_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP2_DEV_S = {
    &APB_PPCEXP2_DEV_CFG, &APB_PPCEXP2_DEV_DATA_S };
#endif

#ifdef APB_PPCEXP3_S
static struct ppc_sse200_dev_cfg_t APB_PPCEXP3_DEV_CFG = {
    .spctrl_base  = CMSDK_SPCTRL_BASE_S,
    .nspctrl_base = CMSDK_NSPCTRL_BASE_NS };
static struct ppc_sse200_dev_data_t APB_PPCEXP3_DEV_DATA_S = {
    .p_ns_ppc  = 0,
    .p_sp_ppc  = 0,
    .p_nsp_ppc = 0,
    .int_bit_mask = 0,
    .state = 0 };
struct ppc_sse200_dev_t APB_PPCEXP3_DEV_S = {
    &APB_PPCEXP3_DEV_CFG, &APB_PPCEXP3_DEV_DATA_S };
#endif

/* CMSDK Timer driver structures */
#ifdef CMSDK_TIMER0_S
static const struct cmsdk_timer_dev_cfg_t CMSDK_TIMER0_DEV_CFG_S
    TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_IRQ_TEST_1")
    = {.base = CMSDK_TIMER0_BASE_S};
static struct cmsdk_timer_dev_data_t CMSDK_TIMER0_DEV_DATA_S
    TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_IRQ_TEST_1")
    = {.is_initialized = 0};

struct cmsdk_timer_dev_t CMSDK_TIMER0_DEV_S
       TFM_LINK_SET_RW_IN_PARTITION_SECTION("TFM_IRQ_TEST_1")
       = {&(CMSDK_TIMER0_DEV_CFG_S), &(CMSDK_TIMER0_DEV_DATA_S)};
#endif
#ifdef CMSDK_TIMER0_NS
static const struct cmsdk_timer_dev_cfg_t CMSDK_TIMER0_DEV_CFG_NS = {
    .base = CMSDK_TIMER0_BASE_NS};
static struct cmsdk_timer_dev_data_t CMSDK_TIMER0_DEV_DATA_NS = {
    .is_initialized = 0};
struct cmsdk_timer_dev_t CMSDK_TIMER0_DEV_NS = {&(CMSDK_TIMER0_DEV_CFG_NS),
                                                &(CMSDK_TIMER0_DEV_DATA_NS)};
#endif

#ifdef CMSDK_TIMER1_S
static const struct cmsdk_timer_dev_cfg_t CMSDK_TIMER1_DEV_CFG_S = {
    .base = CMSDK_TIMER1_BASE_S};
static struct cmsdk_timer_dev_data_t CMSDK_TIMER1_DEV_DATA_S = {
    .is_initialized = 0};
struct cmsdk_timer_dev_t CMSDK_TIMER1_DEV_S = {&(CMSDK_TIMER1_DEV_CFG_S),
                                               &(CMSDK_TIMER1_DEV_DATA_S)};
#endif
#ifdef CMSDK_TIMER1_NS
static const struct cmsdk_timer_dev_cfg_t CMSDK_TIMER1_DEV_CFG_NS = {
    .base = CMSDK_TIMER1_BASE_NS};
static struct cmsdk_timer_dev_data_t CMSDK_TIMER1_DEV_DATA_NS = {
    .is_initialized = 0};
struct cmsdk_timer_dev_t CMSDK_TIMER1_DEV_NS = {&(CMSDK_TIMER1_DEV_CFG_NS),
                                                &(CMSDK_TIMER1_DEV_DATA_NS)};
#endif

/* ARM MPC SSE 200 driver structures */
#ifdef MPC_ISRAM0_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM0_DEV_CFG_S = {
    .base = MPC_ISRAM0_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM0_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM0_DEV_S = {
    &(MPC_ISRAM0_DEV_CFG_S),
    &(MPC_ISRAM0_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM1_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM1_DEV_CFG_S = {
    .base = MPC_ISRAM1_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM1_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM1_DEV_S = {
    &(MPC_ISRAM1_DEV_CFG_S),
    &(MPC_ISRAM1_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM2_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM2_DEV_CFG_S = {
    .base = MPC_ISRAM2_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM2_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM2_DEV_S = {
    &(MPC_ISRAM2_DEV_CFG_S),
    &(MPC_ISRAM2_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM3_S
static const struct mpc_sie200_dev_cfg_t MPC_ISRAM3_DEV_CFG_S = {
    .base = MPC_ISRAM3_BASE_S};
static struct mpc_sie200_dev_data_t MPC_ISRAM3_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_ISRAM3_DEV_S = {
    &(MPC_ISRAM3_DEV_CFG_S),
    &(MPC_ISRAM3_DEV_DATA_S)};
#endif

#ifdef MPC_CODE_SRAM1_S
static const struct mpc_sie200_dev_cfg_t MPC_CODE_SRAM1_DEV_CFG_S = {
    .base = MPC_CODE_SRAM1_BASE_S};
static struct mpc_sie200_dev_data_t MPC_CODE_SRAM1_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_CODE_SRAM1_DEV_S = {
    &(MPC_CODE_SRAM1_DEV_CFG_S),
    &(MPC_CODE_SRAM1_DEV_DATA_S)};
#endif

#ifdef MPC_CODE_SRAM2_S
static const struct mpc_sie200_dev_cfg_t MPC_CODE_SRAM2_DEV_CFG_S = {
    .base = MPC_CODE_SRAM2_BASE_S};
static struct mpc_sie200_dev_data_t MPC_CODE_SRAM2_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_CODE_SRAM2_DEV_S = {
    &(MPC_CODE_SRAM2_DEV_CFG_S),
    &(MPC_CODE_SRAM2_DEV_DATA_S)};
#endif

#ifdef MPC_CODE_SRAM3_S
static const struct mpc_sie200_dev_cfg_t MPC_CODE_SRAM3_DEV_CFG_S = {
    .base = MPC_CODE_SRAM3_BASE_S};
static struct mpc_sie200_dev_data_t MPC_CODE_SRAM3_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .state = 0,
    .reserved = 0};
struct mpc_sie200_dev_t MPC_CODE_SRAM3_DEV_S = {
    &(MPC_CODE_SRAM3_DEV_CFG_S),
    &(MPC_CODE_SRAM3_DEV_DATA_S)};
#endif
