/*
 * Copyright (c) 2019 Arm Limited
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

#ifndef __PLATFORM_REGS_H__
#define __PLATFORM_REGS_H__

#include <stdint.h>

/* System info memory mapped register access structure */
struct sse123_sysinfo_t {
    volatile uint32_t sys_version;      /* 0x000 (R/ ) System version */
    volatile uint32_t sys_config;       /* 0x004 (R/ ) System configuration */
    volatile uint32_t reserved0[1010];
    volatile uint32_t pidr4;            /* 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved1[3];
    volatile uint32_t pidr0;            /* 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;            /* 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;            /* 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;            /* 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;            /* 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;            /* 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;            /* 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;            /* 0xFFC (R/ ) Component ID 3 */
};

/* System control memory mapped register access structure */
struct sse123_sysctrl_t {
    volatile uint32_t secdbgstat;     /* 0x000 (R/ ) Secure Debug Configuration
                                       *             Status Register */
    volatile uint32_t secdbgset;      /* 0x004 ( /W) Secure Debug Configuration
                                       *             Set Register */
    volatile uint32_t secdbgclr;      /* 0x008 ( /W) Secure Debug Configuration
                                       *             Clear Register */
    volatile uint32_t scsecctrl;      /* 0x00C (R/W) System Control Security
                                       *             Control Register */
    volatile uint32_t reserved0[2];
    volatile uint32_t clock_ctrl;     /* 0x018 (R/W) Clock Control */
    volatile uint32_t reserved1[57];
    volatile uint32_t resetsyndrome;  /* 0x100 (R/W) Reset syndrome */
    volatile uint32_t resetmask;      /* 0x104 (R/W) Reset MASK */
    volatile uint32_t swreset;        /* 0x108 ( /W) Software Reset */
    volatile uint32_t gretreg;        /* 0x10C (R/W) General Purpose Retention
                                       *       Register */
    volatile uint32_t initsvtor;      /* 0x110 (R/W) Initial Secure Reset Vector
                                       *             Register For CPU */
    volatile uint32_t reserved2;
    volatile uint32_t cpuwait;        /* 0x118 (R/W) CPU Boot wait control
                                       *             after reset */
    volatile uint32_t nmi_enable;     /* 0x11C (R/W) NMI Enable Register */
    volatile uint32_t reserved3;
    volatile uint32_t wicbrgctrl;     /* 0x124 (R/W) WIC-Bridge Control */
    volatile uint32_t reserved4[938];
    volatile uint32_t pidr4;          /* 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved5[3];
    volatile uint32_t pidr0;          /* 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;          /* 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;          /* 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;          /* 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;          /* 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;          /* 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;          /* 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;          /* 0xFFC (R/ ) Component ID 3 */

};

/* SPCTRL memory mapped register access structure */
struct sse123_spctrl_t {
    volatile uint32_t spcsecctrl;     /* 0x000 (R/W) Secure Configuration
                                       *            Control Register */
    volatile uint32_t buswait;        /* 0x004 (R/W) Bus Access wait control */
    volatile uint32_t reserved1[2];
    volatile uint32_t secrespcfg;     /* 0x010 (R/W) Security Violation Response
                                       *             Configuration register.*/
    volatile uint32_t nsccfg;         /* 0x014 (R/W) Non Secure Callable
                                       *             Configuration for IDAU. */
    volatile uint32_t reserved2;
    volatile uint32_t secmpcintstat;  /* 0x01C (R/ ) Secure MPC IRQ Status. */
    volatile uint32_t secppcintstat;  /* 0x020 (R/ ) Secure PPC IRQ Status. */
    volatile uint32_t secppcintclr;   /* 0x024 ( /W) Secure PPC IRQ Clear. */
    volatile uint32_t secppcinten;    /* 0x028 (R/W) Secure PPC IRQ Enable. */
    volatile uint32_t reserved3;
    volatile uint32_t secmscintstat;  /* 0x030 (R/ ) Secure MSC IRQ Status. */
    volatile uint32_t secmscintclr;   /* 0x034 ( /W) Secure MSC IRQ Clear. */
    volatile uint32_t secmscinten;    /* 0x038 (R/W) Secure MSC IRQ Enable. */
    volatile uint32_t reserved4;
    volatile uint32_t brgintstat;     /* 0x040 (R/ ) Bridge Buffer Error IRQ
                                       *             Status */
    volatile uint32_t brgintclr;      /* 0x044 ( /W) Bridge Buffer Error IRQ
                                       *             Clear */
    volatile uint32_t brginten;       /* 0x048 (R/W) Bridge Buffer Error IRQ
                                       *             Enable */
    volatile uint32_t reserved5[5];
    volatile uint32_t ahbnsppcexp;    /* 0x060 (R/W) Expansion Non-Secure
                                                     Access AHB slave Peripheral
                                                     Protection Control */
    volatile uint32_t reserved6[3];
    volatile uint32_t apbnsppc;       /* 0x070 (R/W) Non-secure Access APB
                                                     slave Peripheral Protection
                                                     Control */
    volatile uint32_t reserved7[3];
    volatile uint32_t apbnsppcexp0;   /* 0x080 (R/W) Expansion 0 NS Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t apbnsppcexp1;   /* 0x084 (R/W) Expansion 1 NS Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t apbnsppcexp2;   /* 0x088 (R/W) Expansion 2 NS Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t apbnsppcexp3;   /* 0x08C (R/W) Expansion 3 NS Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t reserved8[4];
    volatile uint32_t ahbspppcexp;    /* 0x0A0 (R/W) Expansion Secure
                                       *             Unprivileged Access AHB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t reserved9[3];
    volatile uint32_t apbspppc;       /* 0x0B0 (R/W) Secure Unprivileged Access
                                       *             APB slave Peripheral
                                       *             Protection Control */
    volatile uint32_t reserved10[3];
    volatile uint32_t apbspppcexp0;    /* 0x0C0 (R/W) Expansion 0 Secure
                                       *             Unprivileged Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t apbspppcexp1;    /* 0x0C4 (R/W) Expansion 1 Secure
                                       *             Unprivileged Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t apbspppcexp2;    /* 0x0C8 (R/W) Expansion 2 Secure
                                       *             Unprivileged Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t apbspppcexp3;    /* 0x0CC (R/W) Expansion 3 Secure
                                       *             Unprivileged Access APB
                                       *             slave Peripheral Protection
                                       *             Control */
    volatile uint32_t nsmscexp;       /* 0x0D0 (R/W) Expansion MSC Non-Secure
                                       *             Configuration */
    volatile uint32_t reserved11[959];
    volatile uint32_t pid4;           /* 0xFD0 (R/W) Peripheral ID 4 */
    volatile uint32_t reserved12[3];
    volatile uint32_t pidr0;          /* 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;          /* 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;          /* 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;          /* 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;          /* 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;          /* 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;          /* 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;          /* 0xFFC (R/ ) Component ID 3 */
};

/* PPC interrupt position mask */
#define APB_PPC0_INT_POS_MASK     (1UL << 0)
#define APB_PPCEXP0_INT_POS_MASK  (1UL << 4)
#define APB_PPCEXP1_INT_POS_MASK  (1UL << 5)
#define APB_PPCEXP2_INT_POS_MASK  (1UL << 6)
#define APB_PPCEXP3_INT_POS_MASK  (1UL << 7)
#define AHB_PPCEXP0_INT_POS_MASK  (1UL << 20)


/* NSPCTRL memory mapped register access structure */
struct sse123_nspctrl_t {
    volatile uint32_t reserved1[40];
    volatile uint32_t ahbnspppcexp;  /* 0x0A0 (R/W) Expansion Non-Secure
                                      *             Unprivileged Access AHB
                                      *             slave Peripheral Protection
                                      *             Control */
    volatile uint32_t reserved2[3];
    volatile uint32_t apbnspppc;     /* 0x0B0 (R/W) Non-secure Unprivileged
                                      *             Access APB slave Peripheral
                                      *             Protection Control */
    volatile uint32_t reserved3[3];
    volatile uint32_t apbnspppcexp0; /* 0x0C0 (R/W) Expansion 0 Non-Secure
                                      *             Unprivileged Access APB
                                      *             slave Peripheral Protection
                                      *             Control */
    volatile uint32_t apbnspppcexp1; /* 0x0C4 (R/W) Expansion 1 Non-Secure
                                      *             Unprivileged Access APB
                                      *             slave Peripheral Protection
                                      *             Control */
    volatile uint32_t apbnspppcexp2; /* 0x0C8 (R/W) Expansion 2 Non-Secure
                                      *             Unprivileged Access APB
                                      *             slave Peripheral Protection
                                      *             Control */
    volatile uint32_t apbnspppcexp3; /* 0x0CC (R/W) Expansion 3 Non-Secure
                                      *             Unprivileged Access APB
                                      *             slave Peripheral Protection
                                      *             Control */
    volatile uint32_t reserved4[960];
    volatile uint32_t pid4;          /* 0xFD0 (R/W) Peripheral ID 4 */
    volatile uint32_t reserved5[3];
    volatile uint32_t pidr0;         /* 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;         /* 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;         /* 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;         /* 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;         /* 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;         /* 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;         /* 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;         /* 0xFFC (R/ ) Component ID 3 */
};

/* APB PPC peripherals definition */
#define SYSTEM_TIMER0_APB_PPC_POS     0U
#define SYSTEM_TIMER1_APB_PPC_POS     1U
#define SYSTEM_WATCHDOG_APB_PPC_POS   6U
/* End APB PPC peripherals definition */

/* ARM APB PPCEXP0 peripherals definition */
/* End ARM APB PPCEXP0 peripherals definition */

/* APB PPCEXP1 peripherals definition */
#define SSRAM2_MPC_APB_PPCEXP1_POS    1U
#define SSRAM3_MPC_APB_PPCEXP1_POS    2U
/* End APB PPCEXP1 peripherals definition */

/* APB PPCEXP2 peripherals definition */
#define SPI0_APB_PPCEXP2_POS          0U
#define SPI1_APB_PPCEXP2_POS          1U
#define SPI2_APB_PPCEXP2_POS          2U
#define SPI3_APB_PPCEXP2_POS          3U
#define SPI4_APB_PPCEXP2_POS          4U
#define UART0_APB_PPCEXP2_POS         5U
#define UART1_APB_PPCEXP2_POS         6U
#define UART2_APB_PPCEXP2_POS         7U
#define UART3_APB_PPCEXP2_POS         8U
#define UART4_APB_PPCEXP2_POS         9U
#define I2C0_APB_PPCEXP2_POS          10U
#define I2C1_APB_PPCEXP2_POS          11U
#define I2C2_APB_PPCEXP2_POS          12U
#define I2C3_APB_PPCEXP2_POS          13U
/* End APB PPCEXP2 peripherals definition */

/* APB PPCEXP3 peripherals definition */
#define FPGA_SCC_APB_PPCEXP3_POS      0U
#define FPGA_AUDIO_APB_PPCEXP3_POS    1U
#define FPGA_IO_APB_PPCEXP3_POS       2U
/* End APB PPCEXP3 peripherals definition */

/* AHB PPCEXP peripherals definition */
#define VGA_AHB_PPCEXP_POS            0U
#define GPIO0_AHB_PPCEXP_POS          1U
#define GPIO1_AHB_PPCEXP_POS          2U
#define GPIO2_AHB_PPCEXP_POS          3U
#define GPIO3_AHB_PPCEXP_POS          4U
#define PSRAM_ETH_AHB_PPCEXP_POS      5U
#define DMA0_AHB_PPCEXP_POS           8U
#define DMA1_AHB_PPCEXP_POS           9U
/* End AHB PPCEXP peripherals definition */

#endif /* __PLATFORM_REGS_H__ */
