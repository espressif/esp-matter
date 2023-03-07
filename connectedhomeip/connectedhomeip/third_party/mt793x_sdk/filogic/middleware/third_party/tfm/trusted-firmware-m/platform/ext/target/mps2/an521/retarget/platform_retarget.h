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
 * \file platform_retarget.h
 * \brief This file defines all the peripheral base addresses for MPS2/SSE-200 platform.
 */

#ifndef __ARM_LTD_AN521_RETARGET_H__
#define __ARM_LTD_AN521_RETARGET_H__

#include "platform_regs.h"           /* Platform registers */
#include "platform_irq.h"            /* IRQ numbers */
#include "platform_retarget_pins.h"  /* Platform pin names */

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
#define GPIO0_BASE_NS          0x40100000
#define GPIO1_BASE_NS          0x40101000
#define GPIO2_BASE_NS          0x40102000
#define GPIO3_BASE_NS          0x40103000
#define UART0_BASE_NS          0x40200000
#define UART1_BASE_NS          0x40201000
#define UART2_BASE_NS          0x40202000
#define UART3_BASE_NS          0x40203000
#define UART4_BASE_NS          0x40204000
#define I2C0_SBCON_BASE_NS     0x40207000  /* Touchscreen I2C Base Address */
#define I2C1_SBCON_BASE_NS     0x40208000  /* Audio I2C Base Address */
#define I2C2_SBCON_BASE_NS     0x4020C000  /* Shield 0 SBCon Base Address */
#define I2C3_SBCON_BASE_NS     0x4020D000  /* Shield 1 SBCon Base Address */
#define SSP0_BASE_NS           0x40206000  /* CLCD SSP PL022 Base Address */
#define SSP1_BASE_NS           0x40205000  /* User SSP PL022 Base Address */
#define SSP2_BASE_NS           0x40209000  /* ADC SPI PL022 Base Address */
#define SSP3_BASE_NS           0x4020A000  /* Shield 0 SPI PL022 Base Address */
#define SSP4_BASE_NS           0x4020B000  /* Shield 1 SPI PL022 Base Address */
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
#define GPIO0_BASE_S           0x50100000
#define GPIO1_BASE_S           0x50101000
#define GPIO2_BASE_S           0x50102000
#define GPIO3_BASE_S           0x50103000
#define UART0_BASE_S           0x50200000
#define UART1_BASE_S           0x50201000
#define UART2_BASE_S           0x50202000
#define UART3_BASE_S           0x50203000
#define UART4_BASE_S           0x50204000
#define I2C0_SBCON_BASE_S      0x50207000
#define I2C1_SBCON_BASE_S      0x50208000
#define I2C2_SBCON_BASE_S      0x5020C000
#define I2C3_SBCON_BASE_S      0x5020D000
#define SSP0_BASE_S            0x50206000
#define SSP1_BASE_S            0x50205000
#define SSP2_BASE_S            0x50209000
#define SSP3_BASE_S            0x5020A000
#define SSP4_BASE_S            0x5020B000
#define MPS2_IO_SCC_BASE_S     0x50300000
#define MPS2_IO_FPGAIO_BASE_S  0x50302000
#define MPC_ISRAM0_BASE_S      0x50083000
#define MPC_ISRAM1_BASE_S      0x50084000
#define MPC_ISRAM2_BASE_S      0x50085000
#define MPC_ISRAM3_BASE_S      0x50086000
#define MPC_CODE_SRAM1_BASE_S  0x58007000
#define MPC_CODE_SRAM2_BASE_S  0x58008000
#define MPC_CODE_SRAM3_BASE_S  0x58009000

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
#define MPC_CODE_SRAM1_RANGE_BASE_NS   0x00000000
#define MPC_CODE_SRAM1_RANGE_LIMIT_NS  0x003FFFFF
#define MPC_CODE_SRAM1_RANGE_BASE_S    0x10000000
#define MPC_CODE_SRAM1_RANGE_LIMIT_S   0x103FFFFF

#define MPC_CODE_SRAM2_RANGE_BASE_NS   0x28000000
#define MPC_CODE_SRAM2_RANGE_LIMIT_NS  0x281FFFFF
#define MPC_CODE_SRAM2_RANGE_BASE_S    0x38000000
#define MPC_CODE_SRAM2_RANGE_LIMIT_S   0x381FFFFF

#define MPC_CODE_SRAM3_RANGE_BASE_NS   0x28200000
#define MPC_CODE_SRAM3_RANGE_LIMIT_NS  0x283FFFFF
#define MPC_CODE_SRAM3_RANGE_BASE_S    0x38200000
#define MPC_CODE_SRAM3_RANGE_LIMIT_S   0x383FFFFF

/* Flash memory emulated over external SSRAM memory */
#define FLASH0_BASE_S                  0x10000000
#define FLASH0_BASE_NS                 0x00000000
#define FLASH0_SIZE                    0x00400000  /* 4 MB */
#define FLASH0_SECTOR_SIZE             0x00001000  /* 4 kB */
#define FLASH0_PAGE_SIZE               0x00001000  /* 4 kB */
#define FLASH0_PROGRAM_UNIT            0x1         /* Minimum write size */

#endif  /* __ARM_LTD_AN521_RETARGET_H__ */
