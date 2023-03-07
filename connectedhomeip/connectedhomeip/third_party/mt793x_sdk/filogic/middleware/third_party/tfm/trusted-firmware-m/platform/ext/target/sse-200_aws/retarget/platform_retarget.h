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
 * \file platform_retarget.h
 * \brief This file defines all the peripheral base addresses for MPS2/SSE-200 platform.
 */

#ifndef __ARM_LTD_SSE_200_AWS_RETARGET_H__
#define __ARM_LTD_SSE_200_AWS_RETARGET_H__

#include "platform_regs.h"           /* Platform registers */
#include "platform_irq.h"            /* IRQ numbers */

/* ======= Defines peripherals memory map addresses ======= */
/* Non-secure memory map addresses */
#define CMSDK_TIMER0_BASE_NS   0x40000000
#define CMSDK_TIMER1_BASE_NS   0x40001000
#define APB_DUALTIMER_BASE_NS  0x40002000
#define MHU0_BASE_NS           0x40003000
#define MHU1_BASE_NS           0x40004000
#define S32K_TIMER_BASE_NS     0x4002F000
#define S32K_WATCHDOG_BASE_NS  0x4002E000
#define APB_WATCHDOG_BASE_NS   0x40081000
#define UART0_BASE_NS          0x40200000
#define MPS2_IO_SCC_BASE_NS    0x40300000
#define MPS2_IO_FPGAIO_BASE_NS 0x40302000

/* Secure memory map addresses */
#define CMSDK_TIMER0_BASE_S    0x50000000
#define CMSDK_TIMER1_BASE_S    0x50001000
#define APB_DUALTIMER_BASE_S   0x50002000
#define MHU0_BASE_S            0x50003000
#define MHU1_BASE_S            0x50004000
#define S32K_TIMER_BASE_S      0x5002F000
#define S32K_WATCHDOG_BASE_S   0x5002E000
#define APB_WATCHDOG_BASE_S    0x50081000
#define UART0_BASE_S           0x50200000
#define MPS2_IO_SCC_BASE_S     0x50300000
#define MPS2_IO_FPGAIO_BASE_S  0x50302000
#define MPC_ISRAM0_BASE_S      0x50083000
#define MPC_ISRAM1_BASE_S      0x50084000
#define MPC_ISRAM2_BASE_S      0x50085000
#define MPC_ISRAM3_BASE_S      0x50086000
#define MPC_SSRAM1_BASE_S      0x58007000
#define MPC_SSRAM2_BASE_S      0x58008000
#define MPC_SSRAM3_BASE_S      0x58009000

/* SRAM MPC ranges and limits */
/* Internal memory */
#define MPC_ISRAM0_RANGE_BASE_NS       0x20000000
#define MPC_ISRAM0_RANGE_LIMIT_NS      0x20007FFF
#define MPC_ISRAM0_RANGE_BASE_S        0x30000000
#define MPC_ISRAM0_RANGE_LIMIT_S       0x30007FFF

#define MPC_ISRAM1_RANGE_BASE_NS       0x20008000
#define MPC_ISRAM1_RANGE_LIMIT_NS      0x2000FFFF
#define MPC_ISRAM1_RANGE_BASE_S        0x30008000
#define MPC_ISRAM1_RANGE_LIMIT_S       0x3000FFFF

#define MPC_ISRAM2_RANGE_BASE_NS       0x20010000
#define MPC_ISRAM2_RANGE_LIMIT_NS      0x20017FFF
#define MPC_ISRAM2_RANGE_BASE_S        0x30010000
#define MPC_ISRAM2_RANGE_LIMIT_S       0x30017FFF

#define MPC_ISRAM3_RANGE_BASE_NS       0x20018000
#define MPC_ISRAM3_RANGE_LIMIT_NS      0x2001FFFF
#define MPC_ISRAM3_RANGE_BASE_S        0x30018000
#define MPC_ISRAM3_RANGE_LIMIT_S       0x3001FFFF

/* External SSRAM memory */
#define MPC_SSRAM1_RANGE_BASE_NS       0x00000000
#define MPC_SSRAM1_RANGE_LIMIT_NS      0x003FFFFF
#define MPC_SSRAM1_RANGE_BASE_S        0x10000000
#define MPC_SSRAM1_RANGE_LIMIT_S       0x103FFFFF

#define MPC_SSRAM2_RANGE_BASE_NS       0x28000000
#define MPC_SSRAM2_RANGE_LIMIT_NS      0x281FFFFF
#define MPC_SSRAM2_RANGE_BASE_S        0x38000000
#define MPC_SSRAM2_RANGE_LIMIT_S       0x381FFFFF

#define MPC_SSRAM3_RANGE_BASE_NS       0x28200000
#define MPC_SSRAM3_RANGE_LIMIT_NS      0x283FFFFF
#define MPC_SSRAM3_RANGE_BASE_S        0x38200000
#define MPC_SSRAM3_RANGE_LIMIT_S       0x383FFFFF

/* Flash memory emulated over external SSRAM memory */
#define FLASH0_BASE_S                  0x10000000
#define FLASH0_BASE_NS                 0x00000000
#define FLASH0_SIZE                    0x00400000  /* 4 MB */
#define FLASH0_SECTOR_SIZE             0x00001000  /* 4 kB */
#define FLASH0_PAGE_SIZE               0x00001000  /* 4 kB */
#define FLASH0_PROGRAM_UNIT            0x1         /* Minimum write size */

#endif  /* __ARM_LTD_SSE_200_AWS_RETARGET_H__ */
