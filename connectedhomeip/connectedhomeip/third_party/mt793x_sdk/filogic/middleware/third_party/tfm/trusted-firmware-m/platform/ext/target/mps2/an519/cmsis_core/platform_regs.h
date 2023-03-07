/*
 * Copyright (c) 2016-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ARM_LTD_AN519_REGS_H__
#define __ARM_LTD_AN519_REGS_H__

#include <stdint.h>

/* System information (SYSINFO) */
#define CMSDK_SYSINFO_BASE_NS  0x40020000
#define CMSDK_SYSINFO_BASE_S   0x50020000

/* System info memory mapped register access structure */
struct sysinfo_t {
    volatile uint32_t sys_version;      /* (R/ ) System version */
    volatile uint32_t sys_config;       /* (R/ ) System configuration */
    volatile uint32_t reserved0[1008];
    volatile uint32_t pidr4;            /* (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved1[3];
    volatile uint32_t pidr0;            /* (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;            /* (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;            /* (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;            /* (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;            /* (R/ ) Component ID 0 */
    volatile uint32_t cidr1;            /* (R/ ) Component ID 1 */
    volatile uint32_t cidr2;            /* (R/ ) Component ID 2 */
    volatile uint32_t cidr3;            /* (R/ ) Component ID 3 */
};

/* System Control (SYSCTRL) */
#define CMSDK_SYSCTRL_BASE_S   0x50021000

/* System control memory mapped register access structure */
struct sysctrl_t {
    volatile uint32_t secdbgstat;     /* (R/ ) Secure Debug Configuration
                                       *       Status Register */
    volatile uint32_t secdbgset;      /* ( /W) Secure Debug Configuration
                                       *       Set Register */
    volatile uint32_t secdbgclr;      /* ( /W) Secure Debug Configuration
                                       *       Clear Register */
    volatile uint32_t reserved0[61];
    volatile uint32_t resetsyndrome;  /* (R/W) Reset syndrome */
    volatile uint32_t resetmask;      /* (R/W) Reset MASK */
    volatile uint32_t swreset;        /* ( /W) Software Reset */
    volatile uint32_t gretreg;        /* (R/W) General Purpose Retention
                                       *       Register */
    volatile uint32_t initsvtor0;     /* (R/W) Initial Secure Reset Vector
                                       *       Register For CPU 0 */
    volatile uint32_t reserved1;
    volatile uint32_t cpuwait;        /* (R/W) CPU Boot wait control
                                       *       after reset */
    volatile uint32_t bus_wait;       /* (R/W) Bus Access wait control
                                               after reset */
    volatile uint32_t wicctrl;        /* (R/W) CPU WIC Request and
                                       *       Acknowledgement */
    volatile uint32_t reserved2[939];         /* Reserved */
    volatile uint32_t pidr4;                  /* (R/ ) Peripheral ID 0 */
    volatile uint32_t reserved3[3];           /* Reserved */
    volatile uint32_t pidr0;                  /* (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;                  /* (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;                  /* (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;                  /* (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;                  /* (R/ ) Component ID 0 */
    volatile uint32_t cidr1;                  /* (R/ ) Component ID 1 */
    volatile uint32_t cidr2;                  /* (R/ ) Component ID 2 */
    volatile uint32_t cidr3;                  /* (R/ ) Component ID 3 */
};

/* Secure Privilege Control */
#define CMSDK_SPCTRL_BASE_S  0x50080000
#define CMSDK_SPCTRL  ((struct spctrl_def*)CMSDK_SPCTRL_BASE_S)

/* SPCTRL memory mapped register access structure */
struct spctrl_def {
    volatile uint32_t reserved[4];
    volatile uint32_t secrespcfg;
    volatile uint32_t nsccfg;
    volatile uint32_t reserved2;
    volatile uint32_t secmpcintstatus;
    volatile uint32_t secppcintstat;
    volatile uint32_t secppcintclr;
    volatile uint32_t secppcinten;
    volatile uint32_t reserved3;
    volatile uint32_t secmscintstat;
    volatile uint32_t secmscintclr;
    volatile uint32_t secmscinten;
    volatile uint32_t reserved4;
    volatile uint32_t brgintstat;
    volatile uint32_t brgintclr;
    volatile uint32_t brginten;
    volatile uint32_t reserved5;
    volatile uint32_t ahbnsppc0;
    volatile uint32_t reserved6[3];
    volatile uint32_t ahbnsppcexp0;
    volatile uint32_t ahbnsppcexp1;
    volatile uint32_t ahbnsppcexp2;
    volatile uint32_t ahbnsppcexp3;
    volatile uint32_t apbnsppc0;
    volatile uint32_t apbnsppc1;
    volatile uint32_t reserved7[2];
    volatile uint32_t apbnsppcexp0;
    volatile uint32_t apbnsppcexp1;
    volatile uint32_t apbnsppcexp2;
    volatile uint32_t apbnsppcexp3;
    volatile uint32_t ahbspppc0;
    volatile uint32_t reserved8[3];
    volatile uint32_t ahbspppcexp0;
    volatile uint32_t ahbspppcexp1;
    volatile uint32_t ahbspppcexp2;
    volatile uint32_t ahbspppcexp3;
    volatile uint32_t apbspppc0;
    volatile uint32_t apbspppc1;
    volatile uint32_t reserved9[2];
    volatile uint32_t apbspppcexp0;
    volatile uint32_t apbspppcexp1;
    volatile uint32_t apbspppcexp2;
    volatile uint32_t apbspppcexp3;
    volatile uint32_t nsmscexp;
    volatile uint32_t reserved10[959];
    volatile uint32_t pid4;
    volatile uint32_t pid5;
    volatile uint32_t pid6;
    volatile uint32_t pid7;
    volatile uint32_t pid0;
    volatile uint32_t pid1;
    volatile uint32_t pid2;
    volatile uint32_t pid3;
    volatile uint32_t cid0;
    volatile uint32_t cid1;
    volatile uint32_t cid2;
    volatile uint32_t cid3;
};

/* PPC interrupt position mask */
#define CMSDK_APB_PPC0_INT_POS_MASK     (1UL << 0)
#define CMSDK_APB_PPC1_INT_POS_MASK     (1UL << 1)
#define CMSDK_APB_PPCEXP0_INT_POS_MASK  (1UL << 4)
#define CMSDK_APB_PPCEXP1_INT_POS_MASK  (1UL << 5)
#define CMSDK_APB_PPCEXP2_INT_POS_MASK  (1UL << 6)
#define CMSDK_APB_PPCEXP3_INT_POS_MASK  (1UL << 7)
#define CMSDK_AHB_PPC0_INT_POS_MASK     (1UL << 16)
#define CMSDK_AHB_PPCEXP0_INT_POS_MASK  (1UL << 20)
#define CMSDK_AHB_PPCEXP1_INT_POS_MASK  (1UL << 21)
#define CMSDK_AHB_PPCEXP2_INT_POS_MASK  (1UL << 22)
#define CMSDK_AHB_PPCEXP3_INT_POS_MASK  (1UL << 23)

/* Non-Secure Access slave PPCs register addresses */
#define CMSDK_SPCTRL_AHB_NS_PPC0     (CMSDK_SPCTRL_BASE_S + 0x050)
#define CMSDK_SPCTRL_AHB_NS_PPCEXP0  (CMSDK_SPCTRL_BASE_S + 0x060)
#define CMSDK_SPCTRL_AHB_NS_PPCEXP1  (CMSDK_SPCTRL_BASE_S + 0x064)
#define CMSDK_SPCTRL_AHB_NS_PPCEXP2  (CMSDK_SPCTRL_BASE_S + 0x068)
#define CMSDK_SPCTRL_AHB_NS_PPCEXP3  (CMSDK_SPCTRL_BASE_S + 0x06C)
#define CMSDK_SPCTRL_APB_NS_PPC0     (CMSDK_SPCTRL_BASE_S + 0x070)
#define CMSDK_SPCTRL_APB_NS_PPC1     (CMSDK_SPCTRL_BASE_S + 0x074)
#define CMSDK_SPCTRL_APB_NS_PPCEXP0  (CMSDK_SPCTRL_BASE_S + 0x080)
#define CMSDK_SPCTRL_APB_NS_PPCEXP1  (CMSDK_SPCTRL_BASE_S + 0x084)
#define CMSDK_SPCTRL_APB_NS_PPCEXP2  (CMSDK_SPCTRL_BASE_S + 0x088)
#define CMSDK_SPCTRL_APB_NS_PPCEXP3  (CMSDK_SPCTRL_BASE_S + 0x08C)

/* Secure Unprivileged (SP) Access slave PPCs register addresses */
#define CMSDK_SPCTRL_AHB_PPC0_SP     (CMSDK_SPCTRL_BASE_S + 0x090)
#define CMSDK_SPCTRL_AHB_PPCEXP0_SP  (CMSDK_SPCTRL_BASE_S + 0x0A0)
#define CMSDK_SPCTRL_AHB_PPCEXP1_SP  (CMSDK_SPCTRL_BASE_S + 0x0A4)
#define CMSDK_SPCTRL_AHB_PPCEXP2_SP  (CMSDK_SPCTRL_BASE_S + 0x0A8)
#define CMSDK_SPCTRL_AHB_PPCEXP3_SP  (CMSDK_SPCTRL_BASE_S + 0x0AC)
#define CMSDK_SPCTRL_APB_PPC0_SP     (CMSDK_SPCTRL_BASE_S + 0x0B0)
#define CMSDK_SPCTRL_APB_PPC1_SP     (CMSDK_SPCTRL_BASE_S + 0x0B4)
#define CMSDK_SPCTRL_APB_PPCEXP0_SP  (CMSDK_SPCTRL_BASE_S + 0x0C0)
#define CMSDK_SPCTRL_APB_PPCEXP1_SP  (CMSDK_SPCTRL_BASE_S + 0x0C4)
#define CMSDK_SPCTRL_APB_PPCEXP2_SP  (CMSDK_SPCTRL_BASE_S + 0x0C8)
#define CMSDK_SPCTRL_APB_PPCEXP3_SP  (CMSDK_SPCTRL_BASE_S + 0x0CC)

/* Non-Secure Privilege Control */
#define CMSDK_NSPCTRL_BASE_NS  0x40080000
#define CMSDK_NSPCTRL  ((struct nspctrl_def*)CMSDK_NSPCTRL_BASE_NS)

/* NSPCTRL memory mapped register access structure */
struct nspctrl_def {
    volatile uint32_t reserved[36];
    volatile uint32_t ahbnspppc0;
    volatile uint32_t reserved3[3];
    volatile uint32_t ahbnspppcexp0;
    volatile uint32_t ahbnspppcexp1;
    volatile uint32_t ahbnspppcexp2;
    volatile uint32_t ahbnspppcexp3;
    volatile uint32_t apbnspppc0;
    volatile uint32_t apbnspppc1;
    volatile uint32_t reserved4[2];
    volatile uint32_t apbnspppcexp0;
    volatile uint32_t apbnspppcexp1;
    volatile uint32_t apbnspppcexp2;
    volatile uint32_t apbnspppcexp3;
    volatile uint32_t reserved5[960];
    volatile uint32_t pidr4;
    volatile uint32_t reserved7; /* pidr5 */
    volatile uint32_t reserved8; /* pidr6 */
    volatile uint32_t reserved9; /* pidr7 */
    volatile uint32_t pidr0;
    volatile uint32_t pidr1;
    volatile uint32_t pidr2;
    volatile uint32_t pidr3;
    volatile uint32_t cidr0;
    volatile uint32_t cidr1;
    volatile uint32_t cidr2;
    volatile uint32_t cidr3;
};

/* Non-Secure Unprivileged Access (NSP) Access slave PPCs register addresses */
#define CMSDK_NSPCTRL_AHB_PPC0_NSP     (CMSDK_NSPCTRL_BASE_NS + 0x090)
#define CMSDK_NSPCTRL_AHB_PPCEXP0_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0A0)
#define CMSDK_NSPCTRL_AHB_PPCEXP1_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0A4)
#define CMSDK_NSPCTRL_AHB_PPCEXP2_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0A8)
#define CMSDK_NSPCTRL_AHB_PPCEXP3_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0AC)
#define CMSDK_NSPCTRL_APB_PPC0_NSP     (CMSDK_NSPCTRL_BASE_NS + 0x0B0)
#define CMSDK_NSPCTRL_APB_PPC1_NSP     (CMSDK_NSPCTRL_BASE_NS + 0x0B4)
#define CMSDK_NSPCTRL_APB_PPCEXP0_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0C0)
#define CMSDK_NSPCTRL_APB_PPCEXP1_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0C4)
#define CMSDK_NSPCTRL_APB_PPCEXP2_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0C8)
#define CMSDK_NSPCTRL_APB_PPCEXP3_NSP  (CMSDK_NSPCTRL_BASE_NS + 0x0CC)

/* ARM APB PPC0 peripherals definition */
#define CMSDK_TIMER0_APB_PPC_POS  0U
#define CMSDK_TIMER1_APB_PPC_POS  1U
#define CMSDK_DTIMER_APB_PPC_POS  2U
#define CMSDK_MHU0_APB_PPC_POS    3U
#define CMSDK_MHU1_APB_PPC_POS    4U
/* The bits 31:5 are reserved */
/* End ARM APB PPC0 peripherals definition */

/* ARM APB PPC1 peripherals definition */
#define CMSDK_S32K_TIMER_PPC_POS 0U
/* The bits 31:1 are reserved */
/* End ARM APB PPC1 peripherals definition */

/* ARM APB PPCEXP0 peripherals definition */
#define CMSDK_SSRAM1_APB_PPC_POS  0U
#define CMSDK_SSRAM2_APB_PPC_POS  1U
#define CMSDK_SSRAM3_APB_PPC_POS  2U
/* The bits 31:3 are reserved */
/* End ARM APB PPCEXP0 peripherals definition */

/* ARM APB PPCEXP1 peripherals definition */
#define CMSDK_SPI0_APB_PPC_POS   0U
#define CMSDK_SPI1_APB_PPC_POS   1U
#define CMSDK_SPI2_APB_PPC_POS   2U
#define CMSDK_SPI3_APB_PPC_POS   3U
#define CMSDK_SPI4_APB_PPC_POS   4U
#define CMSDK_UART0_APB_PPC_POS  5U
#define CMSDK_UART1_APB_PPC_POS  6U
#define CMSDK_UART2_APB_PPC_POS  7U
#define CMSDK_UART3_APB_PPC_POS  8U
#define CMSDK_UART4_APB_PPC_POS  9U
#define CMSDK_I2C0_APB_PPC_POS   10U
#define CMSDK_I2C1_APB_PPC_POS   11U
#define CMSDK_I2C2_APB_PPC_POS   12U
#define CMSDK_I2C3_APB_PPC_POS   13U
/* The bits 31:14 are reserved */
/* End ARM APB PPCEXP1 peripherals definition */

/* ARM APB PPCEXP2 peripherals definition */
#define CMSDK_FPGA_SCC_PPC_POS    0U
#define CMSDK_FPGA_AUDIO_PPC_POS  1U
#define CMSDK_FPGA_IO_PPC_POS     2U
/* The bits 31:3 are reserved */
/* End ARM APB PPCEXP2 peripherals definition */

/* ARM APB PPCEXP3 peripherals definition */
/* The bits 31:0 are reserved */
/* End ARM APB PPCEXP3 peripherals definition */

/* ARM AHB PPC0 peripherals definition */
/* The bits 31:0 are reserved */
/* End of ARM AHB PPC0 peripherals definition */

/* ARM AHB PPCEXP0 peripherals definition */
#define CMSDK_VGA_PPC_POS       0U
#define CMSDK_GPIO0_PPC_POS     1U
#define CMSDK_GPIO1_PPC_POS     2U
#define CMSDK_GPIO2_PPC_POS     3U
#define CMSDK_GPIO3_PPC_POS     4U
#define MPS2_ETHERNET_PPC_POS   5U
/* The bits 31:6 are reserved */
/* End of ARM AHB PPCEXP0 peripherals definition */

/* ARM AHB PPCEXP1 peripherals definition */
#define CMSDK_DMA0_PPC_POS  0U
#define CMSDK_DMA1_PPC_POS  1U
#define CMSDK_DMA2_PPC_POS  2U
#define CMSDK_DMA3_PPC_POS  3U
/* The bits 31:4 are reserved */
/* End of ARM AHB PPCEXP1 peripherals definition */

/* ARM AHB PPCEXP2 peripherals definition */
/* The bits 31:0 are reserved */
/* End of ARM AHB PPCEXP2 peripherals definition */

/* ARM AHB PPCEXP3 peripherals definition */
/* The bits 31:0 are reserved */

/* End of ARM AHB PPCEXP3 peripherals definition */

#endif /* __ARM_LTD_AN519_REGS_H__ */
