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

/**
 * \file platform_base_address.h
 * \brief This file defines all the peripheral base addresses for MPS3 SSE-200
 *        AN524 platform.
 */

#ifndef __PLATFORM_BASE_ADDRESS_H_
#define __PLATFORM_BASE_ADDRESS_H_

/* ======= Defines peripherals memory map addresses ======= */
/* Non-secure memory map addresses */
#define CMSDK_TIMER0_BASE_NS    0x40000000
#define CMSDK_TIMER1_BASE_NS    0x40001000
#define CMSDK_DUALTIMER_BASE_NS 0x40002000
#define CMSDK_SYSINFO_BASE_NS   0x40020000
#define S32K_TIMER_BASE_NS      0x4002F000
#define CMSDK_NSPCTRL_BASE_NS   0x40080000
#define APB_WATCHDOG_BASE_NS    0x40081000
#define GPIO0_BASE_NS           0x41100000
#define GPIO1_BASE_NS           0x41101000
#define GPIO2_BASE_NS           0x41102000
#define GPIO3_BASE_NS           0x41103000
#define I2C0_SBCON_BASE_NS      0x41200000 /* Touchscreen I2C Base Address */
#define I2C1_SBCON_BASE_NS      0x41201000 /* Audio I2C Base Address */
#define SSP0_BASE_NS            0x41202000 /* ADC SPI PL022 Base Address */
#define SSP1_BASE_NS            0x41203000 /* Shield 0 SPI PL022 Base Address */
#define SSP2_BASE_NS            0x41204000 /* Shield 1 SPI PL022 Base Address */
#define I2C2_SBCON_BASE_NS      0x41205000 /* Shield 0 SBCon Base Address */
#define I2C3_SBCON_BASE_NS      0x41206000 /* Shield 1 SBCon Base Address */
#define I2C4_SBCON_BASE_NS      0x41208000 /* DDR4 EEPROM I2C Base Address */
#define MPS3_IO_SCC_BASE_NS     0x41300000
#define I2S_BASE_NS             0x41301000 /* Audio I2S Base Address */
#define MPS3_IO_FPGAIO_BASE_NS  0x41302000
#define UART0_BASE_NS           0x41303000
#define UART1_BASE_NS           0x41304000
#define UART2_BASE_NS           0x41305000
#define UART3_BASE_NS           0x41306000
#define UART4_BASE_NS           0x41307000
#define UART5_BASE_NS           0x41308000
#define CLCD_CONFIG_BASE_NS     0x4130A000
#define RTC_BASE_NS             0x4130B000
#define ETHERNET_BASE_NS        0x41400000

/* Secure memory map addresses */
#define CMSDK_TIMER0_BASE_S     0x50000000
#define CMSDK_TIMER1_BASE_S     0x50001000
#define CMSDK_DUALTIMER_BASE_S  0x50002000
#define CMSDK_SYSINFO_BASE_S    0x50020000
#define CMSDK_SYSCTRL_BASE_S    0x50021000
#define S32K_WATCHDOG_BASE_S    0x5002E000
#define S32K_TIMER_BASE_S       0x5002F000
#define CMSDK_SPCTRL_BASE_S     0x50080000
#define APB_WATCHDOG_BASE_S     0x50081000
#define GPIO0_BASE_S            0x51100000
#define GPIO1_BASE_S            0x51101000
#define GPIO2_BASE_S            0x51102000
#define GPIO3_BASE_S            0x51103000
#define I2C0_SBCON_BASE_S       0x51200000
#define I2C1_SBCON_BASE_S       0x51201000
#define SSP0_BASE_S             0x51202000
#define SSP1_BASE_S             0x51203000
#define SSP2_BASE_S             0x51204000
#define I2C2_SBCON_BASE_S       0x51205000
#define I2C3_SBCON_BASE_S       0x51206000
#define I2C4_SBCON_BASE_S       0x51208000
#define MPS3_IO_SCC_BASE_S      0x51300000
#define I2S_BASE_S              0x51301000
#define MPS3_IO_FPGAIO_BASE_S   0x51302000
#define UART0_BASE_S            0x51303000
#define UART1_BASE_S            0x51304000
#define UART2_BASE_S            0x51305000
#define UART3_BASE_S            0x51306000
#define UART4_BASE_S            0x51307000
#define UART5_BASE_S            0x51308000
#define CLCD_CONFIG_BASE_S      0x5130A000
#define RTC_BASE_S              0x5130B000
#define ETHERNET_BASE_S         0x51400000
#define MPC_ISRAM0_BASE_S       0x50083000
#define MPC_ISRAM1_BASE_S       0x50084000
#define MPC_ISRAM2_BASE_S       0x50085000
#define MPC_ISRAM3_BASE_S       0x50086000
#define MPC_BRAM_BASE_S         0x58007000
#define MPC_QSPI_BASE_S         0x58008000
#define MPC_DDR4_BASE_S         0x58009000

/* SRAM MPC ranges and limits */
/* Internal memory */
#define MPC_ISRAM0_RANGE_BASE_NS   0x20000000
#define MPC_ISRAM0_RANGE_LIMIT_NS  0x20007FFF
#define MPC_ISRAM0_RANGE_BASE_S    0x30000000
#define MPC_ISRAM0_RANGE_LIMIT_S   0x30007FFF

#define MPC_ISRAM1_RANGE_BASE_NS   0x20008000
#define MPC_ISRAM1_RANGE_LIMIT_NS  0x2000FFFF
#define MPC_ISRAM1_RANGE_BASE_S    0x30008000
#define MPC_ISRAM1_RANGE_LIMIT_S   0x3000FFFF

#define MPC_ISRAM2_RANGE_BASE_NS   0x20010000
#define MPC_ISRAM2_RANGE_LIMIT_NS  0x20017FFF
#define MPC_ISRAM2_RANGE_BASE_S    0x30010000
#define MPC_ISRAM2_RANGE_LIMIT_S   0x30017FFF

#define MPC_ISRAM3_RANGE_BASE_NS   0x20018000
#define MPC_ISRAM3_RANGE_LIMIT_NS  0x2001FFFF
#define MPC_ISRAM3_RANGE_BASE_S    0x30018000
#define MPC_ISRAM3_RANGE_LIMIT_S   0x3001FFFF

#ifdef REMAP_QSPI
/* REMAP: QSPI memory map */
/* FPGA BRAM memory */
#define MPC_BRAM_RANGE_BASE_NS     0x28000000
#define MPC_BRAM_RANGE_LIMIT_NS    0x2807FFFF
#define MPC_BRAM_RANGE_BASE_S      0x38000000
#define MPC_BRAM_RANGE_LIMIT_S     0x3807FFFF

/* QSPI flash memory */
#define MPC_QSPI_RANGE_BASE_NS     0x00000000
#define MPC_QSPI_RANGE_LIMIT_NS    0x007FFFFF
#define MPC_QSPI_RANGE_BASE_S      0x10000000
#define MPC_QSPI_RANGE_LIMIT_S     0x107FFFFF

#else
/* Default BRAM remap memory map*/
/*  BRAM memory */
#define MPC_BRAM_RANGE_BASE_NS     0x00000000
#define MPC_BRAM_RANGE_LIMIT_NS    0x0007FFFF
#define MPC_BRAM_RANGE_BASE_S      0x10000000
#define MPC_BRAM_RANGE_LIMIT_S     0x1007FFFF

/* QSPI flash memory */
#define MPC_QSPI_RANGE_BASE_NS     0x28000000
#define MPC_QSPI_RANGE_LIMIT_NS    0x287FFFFF
#define MPC_QSPI_RANGE_BASE_S      0x38000000
#define MPC_QSPI_RANGE_LIMIT_S     0x387FFFFF

#endif /* REMAP_QSPI */

/* DDR4 memory */
#define MPC_DDR4_RANGE0_BASE_NS    0x60000000
#define MPC_DDR4_RANGE0_LIMIT_NS   0x6FFFFFFF
#define MPC_DDR4_RANGE0_BASE_S     0x70000000
#define MPC_DDR4_RANGE0_LIMIT_S    0x7FFFFFFF

#endif  /* __PLATFORM_BASE_ADDRESS_H_ */
