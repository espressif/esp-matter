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
 * \file platform_base_address.h
 * \brief This file defines all the peripheral base addresses for MPS2 SSE-123
 *        AN539 platform.
 */

#ifndef __PLATFORM_BASE_ADDRESS_H_
#define __PLATFORM_BASE_ADDRESS_H_

/* ======= Defines peripherals memory map addresses ======= */
/* Non-secure memory map addresses */
#define ZBT_SRAM1_BASE_NS        0x00000000 /* ZBT SRAM1 Non-Secure base address */
#define ZBT_SRAM2_BASE_NS        0x28000000 /* ZBT SRAM2 Non-Secure base address */
#define ZBT_SRAM3_BASE_NS        0x28200000 /* ZBT SRAM3 Non-Secure base address */
#define SYSTEM_TIMER0_BASE_NS    0x40000000 /* System Timer 0 Non-Secure base address */
#define SYSTEM_TIMER1_BASE_NS    0x40001000 /* System Timer 1 Non-Secure base address */
#define SSE123_SYSINFO_BASE_NS   0x40020000 /* SSE-123 System info Block Non-Secure base address */
#define SSE123_NSPCTRL_BASE_NS   0x40080000 /* SSE-123 Non-Secure Priviledge Control Block Non-Secure base address */
#define SYSTEM_WATCHDOG_BASE_NS  0x40081000 /* System Watchdog Non-Secure base address */
#define CRYPTOCELL_APBS_BASE_NS  0x40088000 /* CryptoCell APBS I/F Non-Secure base address */
#define CPU_IO_BASE_NS           0x400F0000 /* CPU IO Non-Secure base address */
#define GPIO0_BASE_NS            0x40100000 /* GPIO0 Non-Secure base address */
#define GPIO1_BASE_NS            0x40101000 /* GPIO1 Non-Secure base address */
#define GPIO2_BASE_NS            0x40102000 /* GPIO2 Non-Secure base address */
#define GPIO3_BASE_NS            0x40103000 /* GPIO3 Non-Secure base address */
#define DMA0_PL081_BASE_NS       0x40110000 /* DMA0 PL081 Non-Secure base address */
#define DMA1_PL081_BASE_NS       0x40111000 /* DMA1 PL081 Non-Secure base address */
#define UART0_BASE_NS            0x40200000 /* UART0 (J10) Non-Secure base address */
#define UART1_BASE_NS            0x40201000 /* UART1 (XBEE) Non-Secure base address */
#define UART2_BASE_NS            0x40202000 /* UART2 (Reserved) Non-Secure base address */
#define UART3_BASE_NS            0x40203000 /* UART3 (Shield 0) Non-Secure base address */
#define UART4_BASE_NS            0x40204000 /* UART4 (Shield 1) Non-Secure base address */
#define SPI0_PL022_BASE_NS       0x40205000 /* SPI0 PL022 (J21) Non-Secure base address */
#define SPI1_PL022_BASE_NS       0x40206000 /* SPI2 PL022 (CLCD) Non-Secure base address */
#define I2C0_SBCON_BASE_NS       0x40207000 /* I2C0 (Touchscreen) Non-Secure Base Address */
#define I2C1_SBCON_BASE_NS       0x40208000 /* I2C1 (Audio) Non-Secure base address */
#define SPI2_PL022_BASE_NS       0x40209000 /* SPI2 PL022 (ADC) Non-Secure base address */
#define SPI3_PL022_BASE_NS       0x4020A000 /* SPI3 PL022 (Shield 0) Non-Secure base address */
#define SPI4_PL022_BASE_NS       0x4020B000 /* SPI4 PL022 (Shield 1) Non-Secure base address */
#define I2C2_SBCON_BASE_NS       0x4020C000 /* I2C2 (Shield 0) Non-Secure base address */
#define I2C3_SBCON_BASE_NS       0x4020D000 /* I2C3 (Shield 1) Non-Secure base address */
#define MPS2_IO_SCC_BASE_NS      0x40300000 /* MPS2 SCC IO Non-Secure base address */
#define I2S_FPGA_BASE_NS         0x40301000 /* MPS2 FPGA I2S Non-Secure base address */
#define MPS2_IO_FPGAIO_BASE_NS   0x40302000 /* MPS2 FPGA IO Non-Secure base address */
#define VGA_CONSOLE_BASE_NS      0x41000000 /* VGA Console Non-Secure base address */
#define VGA_IMAGE_BASE_NS        0x41100000 /* VGA Image Non-Secure base address */
#define ETH_SMSC9220_BASE_NS     0x42000000 /* Ethernet SMSC 9220 Non-Secure base address */
#define FPGA_NSPCTRL_BASE_NS     0x48007000 /* FPGA Non-Secure Priviledge Control Block Non-Secure base address */
#define SYSCNTR_READ_BASE_NS     0x48104000 /* System Counter Read Non-Secure base address */
#define PSRAM_BASE_NS            0x80000000 /* Parallel SRAM Non-Secure base address */

/* Secure memory map addresses */
#define ZBT_SRAM1_BASE_S         0x10000000 /* ZBT SRAM1 Secure base address */
#define ZBT_SRAM2_BASE_S         0x38000000 /* ZBT SRAM2 Secure base address */
#define ZBT_SRAM3_BASE_S         0x38200000 /* ZBT SRAM3 Secure base address */
#define SYSTEM_TIMER0_BASE_S     0x50000000 /* System Timer 0 Secure base address */
#define SYSTEM_TIMER1_BASE_S     0x50001000 /* System Timer 1 Secure base address */
#define SSE123_SYSINFO_BASE_S    0x50020000 /* SSE-123 System info Block Secure base address */
#define SSE123_SYSCTRL_BASE_S    0x50021000 /* SSE-123 System control Block Secure base address */
#define SSE123_SYSPPU_BASE_S     0x50022000 /* SSE-123 System Power Policy Unit Secure base address */
#define SSE123_SPCTRL_BASE_S     0x50080000 /* SSE-123 Secure Priviledge Control Block Secure base address */
#define SYSTEM_WATCHDOG_BASE_S   0x50081000 /* System Watchdog Secure base address */
#define MPC_ISRAM_BASE_S         0x50083000 /* Internal SRAM MPC Secure base address */
#define CRYPTOCELL_APBS_BASE_S   0x50088000 /* CryptoCell APBS I/F Secure base address */
#define CPU_IO_BASE_S            0x500F0000 /* CPU IO Secure base address */
#define GPIO0_BASE_S             0x50100000 /* GPIO0 Secure base address */
#define GPIO1_BASE_S             0x50101000 /* GPIO1 Secure base address */
#define GPIO2_BASE_S             0x50102000 /* GPIO2 Secure base address */
#define GPIO3_BASE_S             0x50103000 /* GPIO3 Secure base address */
#define DMA0_PL081_BASE_S        0x50110000 /* DMA0 PL081 Secure base address */
#define DMA1_PL081_BASE_S        0x50111000 /* DMA1 PL081 Secure base address */
#define UART0_BASE_S             0x50200000 /* UART0 (J10) Secure base address */
#define UART1_BASE_S             0x50201000 /* UART1 (XBEE) Secure base address */
#define UART2_BASE_S             0x50202000 /* UART2 (Reserved) Secure base address */
#define UART3_BASE_S             0x50203000 /* UART3 (Shield 0) Secure base address */
#define UART4_BASE_S             0x50204000 /* UART4 (Shield 1) Secure base address */
#define SPI0_PL022_BASE_S        0x50205000 /* SPI0 PL022 (J21) Secure base address */
#define SPI1_PL022_BASE_S        0x50206000 /* SPI2 PL022 (CLCD) Secure base address */
#define I2C0_SBCON_BASE_S        0x50207000 /* I2C0 (Touchscreen) Secure Base Address */
#define I2C1_SBCON_BASE_S        0x50208000 /* I2C1 (Audio) -Secure base address */
#define SPI2_PL022_BASE_S        0x50209000 /* SPI2 PL022 (ADC) Secure base address */
#define SPI3_PL022_BASE_S        0x5020A000 /* SPI3 PL022 (Shield 0) Secure base address */
#define SPI4_PL022_BASE_S        0x5020B000 /* SPI4 PL022 (Shield 1) Secure base address */
#define I2C2_SBCON_BASE_S        0x5020C000 /* I2C2 (Shield 0) Secure base address */
#define I2C3_SBCON_BASE_S        0x5020D000 /* I2C3 (Shield 1) Secure base address */
#define MPS2_IO_SCC_BASE_S       0x50300000 /* MPS2 SCC IO Secure base address */
#define I2S_FPGA_BASE_S          0x50301000 /* MPS2 FPGA I2S Secure base address */
#define MPS2_IO_FPGAIO_BASE_S    0x50302000 /* MPS2 FPGA IO Secure base address */
#define VGA_CONSOLE_BASE_S       0x51000000 /* VGA Console Non-Secure base address */
#define VGA_IMAGE_BASE_S         0x51100000 /* VGA Image Non-Secure base address */
#define ETH_SMSC9220_BASE_S      0x52000000 /* Ethernet SMSC 9220 Secure base address */
#define MPC_SSRAM1_BASE_S        0x58100000 /* SSRAM1 MPC Secure base address */
#define MPC_SSRAM2_BASE_S        0x58008000 /* SSRAM2 MPC Secure base address */
#define MPC_SSRAM3_BASE_S        0x58009000 /* SSRAM3 MPC Secure base address */
#define SYSCNTR_READ_BASE_S      0x58104000 /* System Counter Read Secure base address */
#define SYSCNTR_CNTRL_BASE_S     0x58105000 /* System Counter Control Secure base address */

/* Internal SRAM memory */
/*  Internal SRAM - 64 KiB */
#define MPC_ISRAM0_RANGE_BASE_NS   0x20000000
#define MPC_ISRAM0_RANGE_LIMIT_NS  0x2000FFFF
#define MPC_ISRAM0_RANGE_BASE_S    0x30000000
#define MPC_ISRAM0_RANGE_LIMIT_S   0x3000FFFF

/* External SSRAM memory */
/* SSRAM1 - 4 MiB */
#define MPC_SSRAM1_RANGE_BASE_NS   0x00000000
#define MPC_SSRAM1_RANGE_LIMIT_NS  0x003FFFFF
#define MPC_SSRAM1_RANGE_BASE_S    0x10000000
#define MPC_SSRAM1_RANGE_LIMIT_S   0x103FFFFF

/* SSRAM2 - 2 MiB */
#define MPC_SSRAM2_RANGE_BASE_NS   0x28000000
#define MPC_SSRAM2_RANGE_LIMIT_NS  0x281FFFFF
#define MPC_SSRAM2_RANGE_BASE_S    0x38000000
#define MPC_SSRAM2_RANGE_LIMIT_S   0x381FFFFF

/* SSRAM3 - 2 MiB */
#define MPC_SSRAM3_RANGE_BASE_NS   0x28200000
#define MPC_SSRAM3_RANGE_LIMIT_NS  0x283FFFFF
#define MPC_SSRAM3_RANGE_BASE_S    0x38200000
#define MPC_SSRAM3_RANGE_LIMIT_S   0x383FFFFF


#endif  /* __PLATFORM_BASE_ADDRESS_H_ */
