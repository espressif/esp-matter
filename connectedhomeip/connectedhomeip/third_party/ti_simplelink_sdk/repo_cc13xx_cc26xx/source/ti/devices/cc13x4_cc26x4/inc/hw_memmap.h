/******************************************************************************
*  Filename:       hw_memmap_h
*  Revised:        $Date$
*  Revision:       $Revision$
*
* Copyright (c) 2015 - 2017, Texas Instruments Incorporated
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1) Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2) Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3) Neither the name of the ORGANIZATION nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef __HW_MEMMAP_H__
#define __HW_MEMMAP_H__

//*****************************************************************************
//
// The following are defines for the base address of the memories and
// peripherals on the CPU_MMAP interface
//
//*****************************************************************************
#define FLASHMEM_BASE           0x00000000 // FLASHMEM
#define BROM_BASE               0x10000000 // BROM
#define GPRAM_BASE              0x11000000 // GPRAM
#define SRAM_BASE               0x20000000 // SRAM
#define RFC_RAM_BASE            0x21000000 // RFC_RAM
#define RFC_ULLRAM_BASE         0x21004000 // RFC_ULLRAM
#define SPI0_BASE               0x40000000 // SPI
#define UART0_BASE              0x40001000 // UART
#define I2C0_BASE               0x40002000 // I2C
#define SPI1_BASE               0x40008000 // SPI
#define SPI2_BASE               0x40009000 // SPI
#define SPI3_BASE               0x4000A000 // SPI
#define UART1_BASE              0x4000B000 // UART
#define UART2_BASE              0x4000C000 // UART
#define UART3_BASE              0x4000D000 // UART
#define GPT0_BASE               0x40010000 // GPT
#define GPT1_BASE               0x40011000 // GPT
#define GPT2_BASE               0x40012000 // GPT
#define GPT3_BASE               0x40013000 // GPT
#define UDMA0_BASE              0x40020000 // UDMA
#define I2S0_BASE               0x40021000 // I2S
#define GPIO_BASE               0x40022000 // GPIO
#define I2C1_BASE               0x4002A000 // I2C
#define VIMS_BASE               0x40034000 // VIMS
#define RFC_PWR_BASE            0x40040000 // RFC_PWR
#define RFC_DBELL_BASE          0x40041000 // RFC_DBELL
#define RFC_RAT_BASE            0x40043000 // RFC_RAT
#define RFC_FSCA_BASE           0x40044000 // RFC_FSCA
#define WDT_BASE                0x40080000 // WDT
#define IOC_BASE                0x40081000 // IOC
#define EVENT_BASE              0x40083000 // EVENT
#define SMPH_BASE               0x40084000 // SMPH
#define AON_EVENT_BASE          0x40093000 // AON_EVENT
#define AON_IOC_BASE            0x40094000 // AON_IOC
#define AON_BATMON_BASE         0x40095000 // AON_BATMON
#define AUX_SPIM_BASE           0x400C1000 // AUX_SPIM
#define AUX_MAC_BASE            0x400C2000 // AUX_MAC
#define AUX_TIMER2_BASE         0x400C3000 // AUX_TIMER2
#define AUX_TDC_BASE            0x400C4000 // AUX_TDC
#define AUX_EVCTL_BASE          0x400C5000 // AUX_EVCTL
#define AUX_TIMER01_BASE        0x400C7000 // AUX_TIMER01
#define AUX_SMPH_BASE           0x400C8000 // AUX_SMPH
#define AUX_ANAIF_BASE          0x400C9000 // AUX_ANAIF
#define AUX_ADI4_BASE           0x400CB000 // ADI
#define AUX_AIODIO0_BASE        0x400CC000 // AUX_AIODIO
#define AUX_AIODIO1_BASE        0x400CD000 // AUX_AIODIO
#define AUX_AIODIO2_BASE        0x400CE000 // AUX_AIODIO
#define AUX_AIODIO3_BASE        0x400CF000 // AUX_AIODIO
#define AUX_RAM_BASE            0x400E0000 // AUX_RAM
#ifndef CCFG_BASE
#define CCFG_BASE               0x50000000 // CCFG
#endif
#define CCFG_BASE_DEFAULT       0x50000000 // CCFG
#define FCFG1_BASE              0x50000800 // FCFG1
#define FCFG2_BASE              0x50002800 // FCFG2
#define CRYPTO_BASE             0x58024000 // CRYPTO
#define PKA_BASE                0x58025000 // PKA
#define PKA_RAM_BASE            0x58026000 // PKA_RAM
#define PKA_INT_BASE            0x58027000 // PKA_INT
#define TRNG_BASE               0x58028000 // TRNG
#define FLASH_BASE              0x58030000 // FLASH
#define NVMNW_BASE              0x58032000 //
#define SRAM_MMR_BASE           0x58035000 // SRAM_MMR
#define PRCM_BASE               0x58082000 // PRCM
#define ADI2_BASE               0x58086000 // ADI
#define ADI3_BASE               0x58086200 // ADI
#define AON_PMCTL_BASE          0x58090000 // AON_PMCTL
#define AON_RTC_BASE            0x58092000 // AON_RTC
#define AUX_SYSIF_BASE          0x580C6000 // AUX_SYSIF
#define AUX_DDI0_OSC_BASE       0x580CA000 // DDI
#define AUX_SCE_BASE            0x580E1000 // AUX_SCE
#define SPI0_NONBUF_BASE        0x60000000 // SPI CPU nonbuf base
#define UART0_NONBUF_BASE       0x60001000 // UART CPU nonbuf base
#define I2C0_NONBUF_BASE        0x60002000 // I2C CPU nonbuf base
#define SPI1_NONBUF_BASE        0x60008000 // SPI CPU nonbuf base
#define SPI2_NONBUF_BASE        0x60009000 // SPI CPU nonbuf base
#define SPI3_NONBUF_BASE        0x6000A000 // SPI CPU nonbuf base
#define UART1_NONBUF_BASE       0x6000B000 // UART CPU nonbuf base
#define UART2_NONBUF_BASE       0x6000C000 // UART CPU nonbuf base
#define UART3_NONBUF_BASE       0x6000D000 // UART CPU nonbuf base
#define GPT0_NONBUF_BASE        0x60010000 // GPT CPU nonbuf base
#define GPT1_NONBUF_BASE        0x60011000 // GPT CPU nonbuf base
#define GPT2_NONBUF_BASE        0x60012000 // GPT CPU nonbuf base
#define GPT3_NONBUF_BASE        0x60013000 // GPT CPU nonbuf base
#define UDMA0_NONBUF_BASE       0x60020000 // UDMA CPU nonbuf base
#define I2S0_NONBUF_BASE        0x60021000 // I2S CPU nonbuf base
#define GPIO_NONBUF_BASE        0x60022000 // GPIO CPU nonbuf base
#define I2C1_NONBUF_BASE        0x6002A000 // I2C CPU nonbuf base
#define VIMS_NONBUF_BASE        0x60034000 // VIMS CPU nonbuf base
#define RFC_PWR_NONBUF_BASE     0x60040000 // RFC_PWR CPU nonbuf base
#define RFC_DBELL_NONBUF_BASE   0x60041000 // RFC_DBELL CPU nonbuf base
#define RFC_RAT_NONBUF_BASE     0x60043000 // RFC_RAT CPU nonbuf base
#define RFC_FSCA_NONBUF_BASE    0x60044000 // RFC_FSCA CPU nonbuf base
#define WDT_NONBUF_BASE         0x60080000 // WDT CPU nonbuf base
#define IOC_NONBUF_BASE         0x60081000 // IOC CPU nonbuf base
#define EVENT_NONBUF_BASE       0x60083000 // EVENT CPU nonbuf base
#define SMPH_NONBUF_BASE        0x60084000 // SMPH CPU nonbuf base
#define AON_EVENT_NONBUF_BASE   0x60093000 // AON_EVENT CPU nonbuf base
#define AON_IOC_NONBUF_BASE     0x60094000 // AON_IOC CPU nonbuf base
#define AON_BATMON_NONBUF_BASE  0x60095000 // AON_BATMON CPU nonbuf base
#define AUX_SPIM_NONBUF_BASE    0x600C1000 // AUX_SPIM CPU nonbuf base
#define AUX_MAC_NONBUF_BASE     0x600C2000 // AUX_MAC CPU nonbuf base
#define AUX_TIMER2_NONBUF_BASE  0x600C3000 // AUX_TIMER2 CPU nonbuf base
#define AUX_TDC_NONBUF_BASE     0x600C4000 // AUX_TDC CPU nonbuf base
#define AUX_EVCTL_NONBUF_BASE   0x600C5000 // AUX_EVCTL CPU nonbuf base
#define AUX_TIMER01_NONBUF_BASE \
                                0x600C7000 // AUX_TIMER01 CPU nonbuf base
#define AUX_SMPH_NONBUF_BASE    0x600C8000 // AUX_SMPH CPU nonbuf base
#define AUX_ANAIF_NONBUF_BASE   0x600C9000 // AUX_ANAIF CPU nonbuf base
#define AUX_ADI4_NONBUF_BASE    0x600CB000 // ADI CPU nonbuf base
#define AUX_AIODIO0_NONBUF_BASE \
                                0x600CC000 // AUX_AIODIO CPU nonbuf base
#define AUX_AIODIO1_NONBUF_BASE \
                                0x600CD000 // AUX_AIODIO CPU nonbuf base
#define AUX_AIODIO2_NONBUF_BASE \
                                0x600CE000 // AUX_AIODIO CPU nonbuf base
#define AUX_AIODIO3_NONBUF_BASE \
                                0x600CF000 // AUX_AIODIO CPU nonbuf base
#define AUX_RAM_NONBUF_BASE     0x600E0000 // AUX_RAM CPU nonbuf base
#define CRYPTO_NONBUF_BASE      0x78024000 // CRYPTO CPU secure nonbuf base
#define PKA_NONBUF_BASE         0x78025000 // PKA CPU secure nonbuf base
#define PKA_RAM_NONBUF_BASE     0x78026000 // PKA_RAM CPU secure nonbuf base
#define PKA_INT_NONBUF_BASE     0x78027000 // PKA_INT CPU secure nonbuf base
#define TRNG_NONBUF_BASE        0x78028000 // TRNG CPU secure nonbuf base
#define FLASH_NONBUF_BASE       0x78030000 // FLASH CPU secure nonbuf base
#define NVMNW_NONBUF_BASE       0x78032000 //  CPU secure nonbuf base
#define SRAM_MMR_NONBUF_BASE    0x78035000 // SRAM_MMR CPU secure nonbuf base
#define PRCM_NONBUF_BASE        0x78082000 // PRCM CPU secure nonbuf base
#define ADI2_NONBUF_BASE        0x78086000 // ADI CPU secure nonbuf base
#define ADI3_NONBUF_BASE        0x78086200 // ADI CPU secure nonbuf base
#define AON_PMCTL_NONBUF_BASE   0x78090000 // AON_PMCTL CPU secure nonbuf base
#define AON_RTC_NONBUF_BASE     0x78092000 // AON_RTC CPU secure nonbuf base
#define AUX_SYSIF_NONBUF_BASE   0x780C6000 // AUX_SYSIF CPU secure nonbuf base
#define AUX_DDI0_OSC_NONBUF_BASE \
                                0x780CA000 // DDI CPU secure nonbuf base
#define AUX_SCE_NONBUF_BASE     0x780E1000 // AUX_SCE CPU secure nonbuf base
#define SPI0_NONBUF_NONBUF_BASE \
                                0x80000000 // SPI CPU nonbuf base CPU secure nonbuf base
#define UART0_NONBUF_NONBUF_BASE \
                                0x80001000 // UART CPU nonbuf base CPU secure nonbuf base
#define I2C0_NONBUF_NONBUF_BASE \
                                0x80002000 // I2C CPU nonbuf base CPU secure nonbuf base
#define SPI1_NONBUF_NONBUF_BASE \
                                0x80008000 // SPI CPU nonbuf base CPU secure nonbuf base
#define SPI2_NONBUF_NONBUF_BASE \
                                0x80009000 // SPI CPU nonbuf base CPU secure nonbuf base
#define SPI3_NONBUF_NONBUF_BASE \
                                0x8000A000 // SPI CPU nonbuf base CPU secure nonbuf base
#define UART1_NONBUF_NONBUF_BASE \
                                0x8000B000 // UART CPU nonbuf base CPU secure nonbuf base
#define UART2_NONBUF_NONBUF_BASE \
                                0x8000C000 // UART CPU nonbuf base CPU secure nonbuf base
#define UART3_NONBUF_NONBUF_BASE \
                                0x8000D000 // UART CPU nonbuf base CPU secure nonbuf base
#define GPT0_NONBUF_NONBUF_BASE \
                                0x80010000 // GPT CPU nonbuf base CPU secure nonbuf base
#define GPT1_NONBUF_NONBUF_BASE \
                                0x80011000 // GPT CPU nonbuf base CPU secure nonbuf base
#define GPT2_NONBUF_NONBUF_BASE \
                                0x80012000 // GPT CPU nonbuf base CPU secure nonbuf base
#define GPT3_NONBUF_NONBUF_BASE \
                                0x80013000 // GPT CPU nonbuf base CPU secure nonbuf base
#define UDMA0_NONBUF_NONBUF_BASE \
                                0x80020000 // UDMA CPU nonbuf base CPU secure nonbuf base
#define I2S0_NONBUF_NONBUF_BASE \
                                0x80021000 // I2S CPU nonbuf base CPU secure nonbuf base
#define GPIO_NONBUF_NONBUF_BASE \
                                0x80022000 // GPIO CPU nonbuf base CPU secure nonbuf base
#define I2C1_NONBUF_NONBUF_BASE \
                                0x8002A000 // I2C CPU nonbuf base CPU secure nonbuf base
#define VIMS_NONBUF_NONBUF_BASE \
                                0x80034000 // VIMS CPU nonbuf base CPU secure nonbuf base
#define RFC_PWR_NONBUF_NONBUF_BASE \
                                0x80040000 // RFC_PWR CPU nonbuf base CPU secure nonbuf base
#define RFC_DBELL_NONBUF_NONBUF_BASE \
                                0x80041000 // RFC_DBELL CPU nonbuf base CPU secure nonbuf base
#define RFC_RAT_NONBUF_NONBUF_BASE \
                                0x80043000 // RFC_RAT CPU nonbuf base CPU secure nonbuf base
#define RFC_FSCA_NONBUF_NONBUF_BASE \
                                0x80044000 // RFC_FSCA CPU nonbuf base CPU secure nonbuf base
#define WDT_NONBUF_NONBUF_BASE  0x80080000 // WDT CPU nonbuf base CPU secure nonbuf base
#define IOC_NONBUF_NONBUF_BASE  0x80081000 // IOC CPU nonbuf base CPU secure nonbuf base
#define EVENT_NONBUF_NONBUF_BASE \
                                0x80083000 // EVENT CPU nonbuf base CPU secure nonbuf base
#define SMPH_NONBUF_NONBUF_BASE \
                                0x80084000 // SMPH CPU nonbuf base CPU secure nonbuf base
#define AON_EVENT_NONBUF_NONBUF_BASE \
                                0x80093000 // AON_EVENT CPU nonbuf base CPU secure nonbuf base
#define AON_IOC_NONBUF_NONBUF_BASE \
                                0x80094000 // AON_IOC CPU nonbuf base CPU secure nonbuf base
#define AON_BATMON_NONBUF_NONBUF_BASE \
                                0x80095000 // AON_BATMON CPU nonbuf base CPU secure nonbuf base
#define AUX_SPIM_NONBUF_NONBUF_BASE \
                                0x800C1000 // AUX_SPIM CPU nonbuf base CPU secure nonbuf base
#define AUX_MAC_NONBUF_NONBUF_BASE \
                                0x800C2000 // AUX_MAC CPU nonbuf base CPU secure nonbuf base
#define AUX_TIMER2_NONBUF_NONBUF_BASE \
                                0x800C3000 // AUX_TIMER2 CPU nonbuf base CPU secure nonbuf base
#define AUX_TDC_NONBUF_NONBUF_BASE \
                                0x800C4000 // AUX_TDC CPU nonbuf base CPU secure nonbuf base
#define AUX_EVCTL_NONBUF_NONBUF_BASE \
                                0x800C5000 // AUX_EVCTL CPU nonbuf base CPU secure nonbuf base
#define AUX_SMPH_NONBUF_NONBUF_BASE \
                                0x800C8000 // AUX_SMPH CPU nonbuf base CPU secure nonbuf base
#define AUX_ANAIF_NONBUF_NONBUF_BASE \
                                0x800C9000 // AUX_ANAIF CPU nonbuf base CPU secure nonbuf base
#define AUX_ADI4_NONBUF_NONBUF_BASE \
                                0x800CB000 // ADI CPU nonbuf base CPU secure nonbuf base
#define AUX_RAM_NONBUF_NONBUF_BASE \
                                0x800E0000 // AUX_RAM CPU nonbuf base CPU secure nonbuf base
#define FLASHMEM_ALIAS_BASE     0xA0000000 // FLASHMEM Alias base
#define CPU_ITM_BASE            0xE0000000 // CPU_ITM
#define CPU_DWT_BASE            0xE0001000 // CPU_DWT
#define CPU_FPB_BASE            0xE0002000 // CPU_FPB
#define CPU_ICB_BASE            0xE000E000 // CPU_ICB
#define CPU_SYSTICK_BASE        0xE000E010 // CPU_SYSTICK
#define CPU_NVIC_BASE           0xE000E100 // CPU_NVIC
#define CPU_MPU_BASE            0xE000EDC0 // CPU_MPU
#define CPU_SAU_BASE            0xE000EDD0 // CPU_SAU
#define CPU_DCB_BASE            0xE000EDE0 // CPU_DCB
#define CPU_SIG_BASE            0xE000EF00 // CPU_SIG
#define CPU_FPU_BASE            0xE000EF30 // CPU_FPU
#define CPU_ETM_BASE            0xE0041000 // CPU_ETM
#define CPU_CTI_BASE            0xE0042000 // CPU_CTI
#define CPU_MTB_BASE            0xE0043000 // CPU_MTB
#define CPU_ROM_TABLE_BASE      0xE00FF000 // CPU_ROM_TABLE

#endif // __HW_MEMMAP__
