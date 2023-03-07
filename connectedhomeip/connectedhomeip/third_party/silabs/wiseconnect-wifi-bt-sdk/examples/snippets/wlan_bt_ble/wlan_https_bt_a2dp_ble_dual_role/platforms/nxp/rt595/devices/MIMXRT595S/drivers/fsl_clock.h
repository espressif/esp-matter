/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CLOCK_H_
#define _FSL_CLOCK_H_

#include "fsl_device_registers.h"
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "fsl_reset.h"
#include "fsl_common.h"

/*! @addtogroup clock */
/*! @{ */

/*! @file */

/*******************************************************************************
 * Definitions
 *****************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief CLOCK driver version 2.2.1. */
#define FSL_CLOCK_DRIVER_VERSION (MAKE_VERSION(2, 2, 1))
/*@}*/

/* Definition for delay API in clock driver, users can redefine it to the real application. */
#ifndef SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY
#define SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY (198000000UL)
#endif

/*! @brief External XTAL (SYSOSC) clock frequency.
 *
 * The XTAL (YSOSC) clock frequency in Hz, when the clock is setup, use the
 * function CLOCK_SetXtalFreq to set the value in to clock driver. For example,
 * if XTAL is 16MHz,
 * @code
 * CLOCK_SetXtalFreq(160000000); // Set the XTALIN value to clock driver.
 * @endcode
 */
extern volatile uint32_t g_xtalFreq;
/*! @brief External MCLK IN clock frequency.
 *
 * The MCLK IN clock frequency in Hz, when the clock is setup, use the
 * function CLOCK_SetMclkFreq to set the value in to clock driver. For example,
 * if MCLK IN is 16MHz,
 * @code
 * CLOCK_SetMclkFreq(160000000); // Set the MCLK IN value to clock driver.
 * @endcode
 */
extern volatile uint32_t g_mclkFreq;

/*! @brief Clock ip name array for MIPI DSI. */
#define MIPI_DSI_HOST_CLOCKS \
    {                        \
        kCLOCK_MipiDsiCtrl   \
    }

/*! @brief Clock ip name array for LCDIF. */
#define LCDIF_CLOCKS       \
    {                      \
        kCLOCK_DisplayCtrl \
    }

/*! @brief Clock ip name array for SCT. */
#define SCT_CLOCKS \
    {              \
        kCLOCK_Sct \
    }

/*! @brief Clock ip name array for USBD. */
#define USBD_CLOCKS        \
    {                      \
        kCLOCK_UsbhsDevice \
    }

/*! @brief Clock ip name array for FlexSPI */
#define FLEXSPI_CLOCKS                   \
    {                                    \
        kCLOCK_Flexspi0, kCLOCK_Flexspi1 \
    }

/*! @brief Clock ip name array for Cache64 */
#define CACHE64_CLOCKS                   \
    {                                    \
        kCLOCK_Flexspi0, kCLOCK_Flexspi1 \
    }

/*! @brief Clock ip name array for RNG */
#define TRNG_CLOCKS \
    {               \
        kCLOCK_Rng  \
    }

/*! @brief Clock ip name array for PUF */
#define PUF_CLOCKS \
    {              \
        kCLOCK_Puf \
    }

/*! @brief Clock ip name array for HashCrypt */
#define HASHCRYPT_CLOCKS \
    {                    \
        kCLOCK_Hashcrypt \
    }

/*! @brief Clock ip name array for Casper */
#define CASPER_CLOCKS \
    {                 \
        kCLOCK_Casper \
    }

/*! @brief Clock ip name array for Powerquad */
#define POWERQUAD_CLOCKS \
    {                    \
        kCLOCK_PowerQuad \
    }

/*! @brief Clock ip name array for ADC. */
#define LPADC_CLOCKS \
    {                \
        kCLOCK_Adc0  \
    }

/*! @brief Clock ip name array for ACMP. */
#define CMP_CLOCKS   \
    {                \
        kCLOCK_Acmp0 \
    }

/*! @brief Clock ip name array for uSDHC */
#define USDHC_CLOCKS               \
    {                              \
        kCLOCK_Sdio0, kCLOCK_Sdio1 \
    }

/*! @brief Clock ip name array for WWDT. */
#define WWDT_CLOCKS                \
    {                              \
        kCLOCK_Wwdt0, kCLOCK_Wwdt1 \
    }

/*! @brief Clock ip name array for UTICK. */
#define UTICK_CLOCKS  \
    {                 \
        kCLOCK_Utick0 \
    }

/*! @brief Clock ip name array for FlexIO. */
#define FLEXIO_CLOCKS \
    {                 \
        kCLOCK_Flexio \
    }

/*! @brief Clock ip name array for OSTimer */
#define OSTIMER_CLOCKS      \
    {                       \
        kCLOCK_OsEventTimer \
    }

/*! @brief Clock ip name array for FLEXCOMM. */
#define FLEXCOMM_CLOCKS                                                                                             \
    {                                                                                                               \
        kCLOCK_Flexcomm0, kCLOCK_Flexcomm1, kCLOCK_Flexcomm2, kCLOCK_Flexcomm3, kCLOCK_Flexcomm4, kCLOCK_Flexcomm5, \
            kCLOCK_Flexcomm6, kCLOCK_Flexcomm7, kCLOCK_Flexcomm8, kCLOCK_Flexcomm9, kCLOCK_Flexcomm10,              \
            kCLOCK_Flexcomm11, kCLOCK_Flexcomm12, kCLOCK_Flexcomm13, kCLOCK_Flexcomm14, kCLOCK_Flexcomm15,          \
            kCLOCK_Flexcomm16                                                                                       \
    }

/*! @brief Clock ip name array for LPUART. */
#define USART_CLOCKS                                                                                             \
    {                                                                                                            \
        kCLOCK_Usart0, kCLOCK_Usart1, kCLOCK_Usart2, kCLOCK_Usart3, kCLOCK_Usart4, kCLOCK_Usart5, kCLOCK_Usart6, \
            kCLOCK_Usart7, kCLOCK_Usart8, kCLOCK_Usart9, kCLOCK_Usart10, kCLOCK_Usart11, kCLOCK_Usart12,         \
            kCLOCK_Usart13                                                                                       \
    }

/*! @brief Clock ip name array for I2C. */
#define I2C_CLOCKS                                                                                              \
    {                                                                                                           \
        kCLOCK_I2c0, kCLOCK_I2c1, kCLOCK_I2c2, kCLOCK_I2c3, kCLOCK_I2c4, kCLOCK_I2c5, kCLOCK_I2c6, kCLOCK_I2c7, \
            kCLOCK_I2c8, kCLOCK_I2c9, kCLOCK_I2c10, kCLOCK_I2c11, kCLOCK_I2c12, kCLOCK_I2c13, kCLOCK_I2c15      \
    }

/*! @brief Clock ip name array for SPI. */
#define SPI_CLOCKS                                                                                              \
    {                                                                                                           \
        kCLOCK_Spi0, kCLOCK_Spi1, kCLOCK_Spi2, kCLOCK_Spi3, kCLOCK_Spi4, kCLOCK_Spi5, kCLOCK_Spi6, kCLOCK_Spi7, \
            kCLOCK_Spi8, kCLOCK_Spi9, kCLOCK_Spi10, kCLOCK_Spi11, kCLOCK_Spi12, kCLOCK_Spi13, kCLOCK_Spi14,     \
            kCLOCK_Spi16                                                                                        \
    }
/*! @brief Clock ip name array for FLEXI2S. */
#define I2S_CLOCKS                                                                                              \
    {                                                                                                           \
        kCLOCK_I2s0, kCLOCK_I2s1, kCLOCK_I2s2, kCLOCK_I2s3, kCLOCK_I2s4, kCLOCK_I2s5, kCLOCK_I2s6, kCLOCK_I2s7, \
            kCLOCK_I2s8, kCLOCK_I2s9, kCLOCK_I2s10, kCLOCK_I2s11, kCLOCK_I2s12, kCLOCK_I2s13                    \
    }

/*! @brief Clock ip name array for DMIC. */
#define DMIC_CLOCKS  \
    {                \
        kCLOCK_Dmic0 \
    }

/*! @brief Clock ip name array for SEMA */
#define SEMA42_CLOCKS \
    {                 \
        kCLOCK_Sema   \
    }

/*! @brief Clock ip name array for MUA */
#define MU_CLOCKS \
    {             \
        kCLOCK_Mu \
    }

/*! @brief Clock ip name array for DMA. */
#define DMA_CLOCKS                 \
    {                              \
        kCLOCK_Dmac0, kCLOCK_Dmac1 \
    }

/*! @brief Clock ip name array for CRC. */
#define CRC_CLOCKS \
    {              \
        kCLOCK_Crc \
    }

/*! @brief Clock ip name array for GPIO. */
#define GPIO_CLOCKS                                                                                     \
    {                                                                                                   \
        kCLOCK_HsGpio0, kCLOCK_HsGpio1, kCLOCK_HsGpio2, kCLOCK_HsGpio3, kCLOCK_HsGpio4, kCLOCK_HsGpio5, \
            kCLOCK_HsGpio6, kCLOCK_HsGpio7                                                              \
    }

/*! @brief Clock ip name array for PINT. */
#define PINT_CLOCKS       \
    {                     \
        kCLOCK_GpioIntCtl \
    }

/*! @brief Clock ip name array for I3C. */
#define I3C_CLOCKS               \
    {                            \
        kCLOCK_I3c0, kCLOCK_I3c1 \
    }

/*! @brief Clock ip name array for MRT. */
#define MRT_CLOCKS  \
    {               \
        kCLOCK_Mrt0 \
    }

/*! @brief Clock ip name array for RTC. */
#define RTC_CLOCKS \
    {              \
        kCLOCK_Rtc \
    }

/*! @brief Clock ip name array for CT32B. */
#define CTIMER_CLOCKS                                                             \
    {                                                                             \
        kCLOCK_Ct32b0, kCLOCK_Ct32b1, kCLOCK_Ct32b2, kCLOCK_Ct32b3, kCLOCK_Ct32b4 \
    }

/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
/*------------------------------------------------------------------------------
 clock_ip_name_t definition:
------------------------------------------------------------------------------*/

#define CLK_GATE_REG_OFFSET_SHIFT 8U
#define CLK_GATE_REG_OFFSET_MASK 0xFF00U
#define CLK_GATE_BIT_SHIFT_SHIFT 0U
#define CLK_GATE_BIT_SHIFT_MASK 0x000000FFU

#define CLK_GATE_DEFINE(reg_offset, bit_shift)                                  \
    ((((reg_offset) << CLK_GATE_REG_OFFSET_SHIFT) & CLK_GATE_REG_OFFSET_MASK) | \
     (((bit_shift) << CLK_GATE_BIT_SHIFT_SHIFT) & CLK_GATE_BIT_SHIFT_MASK))

#define CLK_GATE_ABSTRACT_REG_OFFSET(x) (((uint32_t)(x)&CLK_GATE_REG_OFFSET_MASK) >> CLK_GATE_REG_OFFSET_SHIFT)
#define CLK_GATE_ABSTRACT_BITS_SHIFT(x) (((uint32_t)(x)&CLK_GATE_BIT_SHIFT_MASK) >> CLK_GATE_BIT_SHIFT_SHIFT)

#define CLK_CTL0_PSCCTL0 0
#define CLK_CTL0_PSCCTL1 1
#define CLK_CTL0_PSCCTL2 2
#define CLK_CTL1_PSCCTL0 3
#define CLK_CTL1_PSCCTL1 4
#define CLK_CTL1_PSCCTL2 5

/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
typedef enum _clock_ip_name
{
    kCLOCK_IpInvalid   = 0U,
    kCLOCK_Dsp         = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 1),
    kCLOCK_RomCtrlr    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 2),
    kCLOCK_AxiSwitch   = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 3),
    kCLOCK_AxiCtrl     = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 4),
    kCLOCK_PowerQuad   = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 8),
    kCLOCK_Casper      = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 9),
    kCLOCK_HashCrypt   = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 10),
    kCLOCK_Puf         = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 11),
    kCLOCK_Rng         = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 12),
    kCLOCK_Flexspi0    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 16),
    kCLOCK_OtpCtrl     = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 17),
    kCLOCK_Flexspi1    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 18),
    kCLOCK_UsbhsPhy    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 20),
    kCLOCK_UsbhsDevice = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 21),
    kCLOCK_UsbhsHost   = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 22),
    kCLOCK_UsbhsSram   = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 23),
    kCLOCK_Sct         = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 24),
    kCLOCK_Gpu         = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 26),
    kCLOCK_DisplayCtrl = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 27),
    kCLOCK_MipiDsiCtrl = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 28),
    kCLOCK_Smartdma    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL0, 30),

    kCLOCK_Sdio0    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL1, 2),
    kCLOCK_Sdio1    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL1, 3),
    kCLOCK_Acmp0    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL1, 15),
    kCLOCK_Adc0     = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL1, 16),
    kCLOCK_ShsGpio0 = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL1, 24),

    kCLOCK_Utick0 = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL2, 0),
    kCLOCK_Wwdt0  = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL2, 1),
    kCLOCK_Pmc    = CLK_GATE_DEFINE(CLK_CTL0_PSCCTL2, 29),

    kCLOCK_Flexcomm0    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 8),
    kCLOCK_Flexcomm1    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 9),
    kCLOCK_Flexcomm2    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 10),
    kCLOCK_Flexcomm3    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 11),
    kCLOCK_Flexcomm4    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 12),
    kCLOCK_Flexcomm5    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 13),
    kCLOCK_Flexcomm6    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 14),
    kCLOCK_Flexcomm7    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 15),
    kCLOCK_Flexcomm8    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 16),
    kCLOCK_Flexcomm9    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 17),
    kCLOCK_Flexcomm10   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 18),
    kCLOCK_Flexcomm11   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 19),
    kCLOCK_Flexcomm12   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 20),
    kCLOCK_Flexcomm13   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 21),
    kCLOCK_Flexcomm14   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 22),
    kCLOCK_Flexcomm15   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 23),
    kCLOCK_Flexcomm16   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 25),
    kCLOCK_Usart0       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 8),
    kCLOCK_Usart1       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 9),
    kCLOCK_Usart2       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 10),
    kCLOCK_Usart3       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 11),
    kCLOCK_Usart4       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 12),
    kCLOCK_Usart5       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 13),
    kCLOCK_Usart6       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 14),
    kCLOCK_Usart7       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 15),
    kCLOCK_Usart8       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 16),
    kCLOCK_Usart9       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 17),
    kCLOCK_Usart10      = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 18),
    kCLOCK_Usart11      = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 19),
    kCLOCK_Usart12      = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 20),
    kCLOCK_Usart13      = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 21),
    kCLOCK_I2s0         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 8),
    kCLOCK_I2s1         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 9),
    kCLOCK_I2s2         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 10),
    kCLOCK_I2s3         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 11),
    kCLOCK_I2s4         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 12),
    kCLOCK_I2s5         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 13),
    kCLOCK_I2s6         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 14),
    kCLOCK_I2s7         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 15),
    kCLOCK_I2s8         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 16),
    kCLOCK_I2s9         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 17),
    kCLOCK_I2s10        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 18),
    kCLOCK_I2s11        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 19),
    kCLOCK_I2s12        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 20),
    kCLOCK_I2s13        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 21),
    kCLOCK_I2c0         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 8),
    kCLOCK_I2c1         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 9),
    kCLOCK_I2c2         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 10),
    kCLOCK_I2c3         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 11),
    kCLOCK_I2c4         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 12),
    kCLOCK_I2c5         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 13),
    kCLOCK_I2c6         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 14),
    kCLOCK_I2c7         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 15),
    kCLOCK_I2c8         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 16),
    kCLOCK_I2c9         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 17),
    kCLOCK_I2c10        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 18),
    kCLOCK_I2c11        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 19),
    kCLOCK_I2c12        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 20),
    kCLOCK_I2c13        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 21),
    kCLOCK_I2c15        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 23),
    kCLOCK_Spi0         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 8),
    kCLOCK_Spi1         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 9),
    kCLOCK_Spi2         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 10),
    kCLOCK_Spi3         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 11),
    kCLOCK_Spi4         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 12),
    kCLOCK_Spi5         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 13),
    kCLOCK_Spi6         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 14),
    kCLOCK_Spi7         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 15),
    kCLOCK_Spi8         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 16),
    kCLOCK_Spi9         = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 17),
    kCLOCK_Spi10        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 18),
    kCLOCK_Spi11        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 19),
    kCLOCK_Spi12        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 20),
    kCLOCK_Spi13        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 21),
    kCLOCK_Spi14        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 22),
    kCLOCK_Spi16        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 25),
    kCLOCK_Dmic0        = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 24),
    kCLOCK_OsEventTimer = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 27),
    kCLOCK_Flexio       = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL0, 29),

    kCLOCK_HsGpio0 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 0),
    kCLOCK_HsGpio1 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 1),
    kCLOCK_HsGpio2 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 2),
    kCLOCK_HsGpio3 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 3),
    kCLOCK_HsGpio4 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 4),
    kCLOCK_HsGpio5 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 5),
    kCLOCK_HsGpio6 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 6),
    kCLOCK_HsGpio7 = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 7),
    kCLOCK_Crc     = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 16),
    kCLOCK_Dmac0   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 23),
    kCLOCK_Dmac1   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 24),
    kCLOCK_Mu      = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 28),
    kCLOCK_Sema    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 29),
    kCLOCK_Freqme  = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL1, 31),

    kCLOCK_Ct32b0   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 0),
    kCLOCK_Ct32b1   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 1),
    kCLOCK_Ct32b2   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 2),
    kCLOCK_Ct32b3   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 3),
    kCLOCK_Ct32b4   = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 4),
    kCLOCK_Rtc      = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 7),
    kCLOCK_Mrt0     = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 8),
    kCLOCK_Wwdt1    = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 10),
    kCLOCK_I3c0     = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 16),
    kCLOCK_I3c1     = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 17),
    kCLOCK_Pint     = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 30),
    kCLOCK_InputMux = CLK_GATE_DEFINE(CLK_CTL1_PSCCTL2, 31)
} clock_ip_name_t;

/*! @brief Clock name used to get clock frequency. */
typedef enum _clock_name
{
    kCLOCK_CoreSysClk,       /*!< Core clock  (aka HCLK)                                 */
    kCLOCK_BusClk,           /*!< Bus clock (AHB/APB clock, aka HCLK)                    */
    kCLOCK_MclkClk,          /*!< MCLK, to MCLK pin                                      */
    kCLOCK_ClockOutClk,      /*!< CLOCKOUT                                               */
    kCLOCK_AdcClk,           /*!< ADC                                                    */
    kCLOCK_Flexspi0Clk,      /*!< FlexSpi0                                               */
    kCLOCK_Flexspi1Clk,      /*!< FlexSpi1                                               */
    kCLOCK_SctClk,           /*!< SCT                                                    */
    kCLOCK_Wdt0Clk,          /*!< Watchdog0                                              */
    kCLOCK_Wdt1Clk,          /*!< Watchdog1                                              */
    kCLOCK_SystickClk,       /*!< Systick                                                */
    kCLOCK_Sdio0Clk,         /*!< SDIO0                                                  */
    kCLOCK_Sdio1Clk,         /*!< SDIO1                                                  */
    kCLOCK_I3cClk,           /*!< I3C0 and I3C1                                          */
    kCLOCK_UsbClk,           /*!< USB0                                                   */
    kCLOCK_DmicClk,          /*!< Digital Mic clock                                      */
    kCLOCK_DspCpuClk,        /*!< DSP  clock                                             */
    kCLOCK_AcmpClk,          /*!< Acmp clock                                             */
    kCLOCK_Flexcomm0Clk,     /*!< Flexcomm0Clock                                         */
    kCLOCK_Flexcomm1Clk,     /*!< Flexcomm1Clock                                         */
    kCLOCK_Flexcomm2Clk,     /*!< Flexcomm2Clock                                         */
    kCLOCK_Flexcomm3Clk,     /*!< Flexcomm3Clock                                         */
    kCLOCK_Flexcomm4Clk,     /*!< Flexcomm4Clock                                         */
    kCLOCK_Flexcomm5Clk,     /*!< Flexcomm5Clock                                         */
    kCLOCK_Flexcomm6Clk,     /*!< Flexcomm6Clock                                         */
    kCLOCK_Flexcomm7Clk,     /*!< Flexcomm7Clock                                         */
    kCLOCK_Flexcomm8Clk,     /*!< Flexcomm8Clock                                         */
    kCLOCK_Flexcomm9Clk,     /*!< Flexcomm9Clock                                         */
    kCLOCK_Flexcomm10Clk,    /*!< Flexcomm10Clock                                        */
    kCLOCK_Flexcomm11Clk,    /*!< Flexcomm11Clock                                        */
    kCLOCK_Flexcomm12Clk,    /*!< Flexcomm12Clock                                        */
    kCLOCK_Flexcomm13Clk,    /*!< Flexcomm13Clock                                        */
    kCLOCK_Flexcomm14Clk,    /*!< Flexcomm14Clock                                        */
    kCLOCK_Flexcomm15Clk,    /*!< Flexcomm15Clock                                        */
    kCLOCK_Flexcomm16Clk,    /*!< Flexcomm16Clock                                        */
    kCLOCK_FlexioClk,        /*!< FlexIO                                                 */
    kCLOCK_GpuClk,           /*!< GPU Core                                               */
    kCLOCK_DcPixelClk,       /*!< DCNano Pixel Clock                                     */
    kCLOCK_MipiDphyClk,      /*!< MIPI D-PHY Bit Clock                                   */
    kCLOCK_MipiDphyEscRxClk, /*!< MIPI D-PHY RX Clock                                    */
    kCLOCK_MipiDphyEscTxClk, /*!< MIPI D-PHY TX Clock                                    */
} clock_name_t;

/**
 * PLL PFD clock name
 */
typedef enum _clock_pfd
{
    kCLOCK_Pfd0 = 0U, /*!< PLL PFD0 */
    kCLOCK_Pfd1 = 1U, /*!< PLL PFD1 */
    kCLOCK_Pfd2 = 2U, /*!< PLL PFD2 */
    kCLOCK_Pfd3 = 3U, /*!< PLL PFD3 */
} clock_pfd_t;

/*! @brief Clock Mux Switches
 *  The encoding is as follows each connection identified is 32bits wide
 *  starting from LSB upwards
 *
 *  [    31             30              29:28        27:25         24:14        13:11          10:0    ]
 *  [CLKCTL index]:[FRODIVSEL onoff]:[FRODIVSEL]:[MUXB choice]:[MUXB offset]:[MUXA choice]:[MUXA offset]
 *  FRODIVSEL onoff '1' means need to set FRODIVSEL. MUX offset 0 means end of descriptor.
 */
/* CLKCTL0 SEL */
#define SYSPLL0CLKSEL_OFFSET 0x200
#define MAINCLKSELA_OFFSET 0x430
#define MAINCLKSELB_OFFSET 0x434
#define FLEXSPI0FCLKSEL_OFFSET 0x620
#define FLEXSPI1FCLKSEL_OFFSET 0x630
#define SCTFCLKSEL_OFFSET 0x640
#define USBHSFCLKSEL_OFFSET 0x660
#define SDIO0FCLKSEL_OFFSET 0x680
#define SDIO1FCLKSEL_OFFSET 0x690
#define ADC0FCLKSEL0_OFFSET 0x6D0
#define ADC0FCLKSEL1_OFFSET 0x6D4
#define UTICKFCLKSEL_OFFSET 0x700
#define WDT0FCLKSEL_OFFSET 0x720
#define A32KHZWAKECLKSEL_OFFSET 0x730
#define SYSTICKFCLKSEL_OFFSET 0x760
#define DPHYCLKSEL_OFFSET 0x770
#define DPHYESCCLKSEL_OFFSET 0x778
#define GPUCLKSEL_OFFSET 0x790
#define DCPIXELCLKSEL_OFFSET 0x7A0
/* CLKCTL1 SEL */
#define AUDIOPLL0CLKSEL_OFFSET 0x200
#define DSPCPUCLKSELA_OFFSET 0x430
#define DSPCPUCLKSELB_OFFSET 0x434
#define OSEVENTTFCLKSEL_OFFSET 0x480
#define FC0FCLKSEL_OFFSET 0x508
#define FC1FCLKSEL_OFFSET 0x528
#define FC2FCLKSEL_OFFSET 0x548
#define FC3FCLKSEL_OFFSET 0x568
#define FC4FCLKSEL_OFFSET 0x588
#define FC5FCLKSEL_OFFSET 0x5A8
#define FC6FCLKSEL_OFFSET 0x5C8
#define FC7FCLKSEL_OFFSET 0x5E8
#define FC8FCLKSEL_OFFSET 0x608
#define FC9FCLKSEL_OFFSET 0x628
#define FC10FCLKSEL_OFFSET 0x648
#define FC11FCLKSEL_OFFSET 0x668
#define FC12FCLKSEL_OFFSET 0x688
#define FC13FCLKSEL_OFFSET 0x6A8
#define FC14FCLKSEL_OFFSET 0x6C8
#define FC15FCLKSEL_OFFSET 0x6E8
#define FC16FCLKSEL_OFFSET 0x708
#define FLEXIOCLKSEL_OFFSET 0x728
#define DMIC0FCLKSEL_OFFSET 0x780
#define CT32BIT0FCLKSEL_OFFSET 0x7A0
#define CT32BIT1FCLKSEL_OFFSET 0x7A4
#define CT32BIT2FCLKSEL_OFFSET 0x7A8
#define CT32BIT3FCLKSEL_OFFSET 0x7AC
#define CT32BIT4FCLKSEL_OFFSET 0x7B0
#define AUDIOMCLKSEL_OFFSET 0x7C0
#define CLKOUTSEL0_OFFSET 0x7E0
#define CLKOUTSEL1_OFFSET 0x7E4
#define I3C01FCLKSEL_OFFSET 0x800
#define I3C01FCLKSTCSEL_OFFSET 0x804
#define I3C01FCLKSTSTCLKSEL_OFFSET 0x814
#define WDT1FCLKSEL_OFFSET 0x820
#define ACMP0FCLKSEL_OFFSET 0x830
/* CLKCTL0 DIV */
#define LOWFREQCLKDIV_OFFSET 0x130
#define SYSCPUAHBCLKDIV_OFFSET 0x400
#define PFC0CLKDIV_OFFSET 0x500
#define PFC1CLKDIV_OFFSET 0x504
#define FLEXSPI0FCLKDIV_OFFSET 0x624
#define FLEXSPI1FCLKDIV_OFFSET 0x634
#define SCTFCLKDIV_OFFSET 0x644
#define USBHSFCLKDIV_OFFSET 0x664
#define SDIO0FCLKDIV_OFFSET 0x684
#define SDIO1FCLKDIV_OFFSET 0x694
#define ADC0FCLKDIV_OFFSET 0x6D8
#define A32KHZWAKECLKDIV_OFFSET 0x734
#define SYSTICKFCLKDIV_OFFSET 0x764
#define DPHYCLKDIV_OFFSET 0x774
#define DPHYESCRXCLKDIV_OFFSET 0x77C
#define DPHYESCTXCLKDIV_OFFSET 0x780
#define GPUCLKDIV_OFFSET 0x794
#define DCPIXELCLKDIV_OFFSET 0x7A4
/* CLKCTL1 DIV */
#define AUDIOPLLCLKDIV_OFFSET 0x240
#define DSPCPUCLKDIV_OFFSET 0x400
#define FLEXIOCLKDIV_OFFSET 0x740
#define FRGPLLCLKDIV_OFFSET 0x760
#define DMIC0FCLKDIV_OFFSET 0x784
#define AUDIOMCLKDIV_OFFSET 0x7C4
#define CLKOUTFCLKDIV_OFFSET 0x7E8
#define I3C01FCLKSTCDIV_OFFSET 0x808
#define I3C01FCLKSDIV_OFFSET 0x80C
#define I3C01FCLKDIV_OFFSET 0x810
#define ACMP0FCLKDIV_OFFSET 0x834

#define CLKCTL0_TUPLE_MUXA(reg, choice) ((((reg) >> 2U) & 0x7FFU) | (((choice)&0x7U) << 11U))
#define CLKCTL0_TUPLE_MUXB(reg, choice) (((((reg) >> 2U) & 0x7FFU) << 14U) | (((choice)&0x7U) << 25U))
#define CLKCTL1_TUPLE_MUXA(reg, choice) (0x80000000U | ((((reg) >> 2U) & 0x7FFU) | (((choice)&0x7U) << 11U)))
#define CLKCTL1_TUPLE_MUXB(reg, choice) (0x80000000U | (((((reg) >> 2U) & 0x7FFU) << 14U) | (((choice)&0x7U) << 25U)))
#define CLKCTL_TUPLE_FRODIVSEL(choice) (0x40000000U | (((choice)&0x7U) << 28U))
#define CLKCTL_TUPLE_REG(base, tuple) ((volatile uint32_t *)(((uint32_t)(base)) + (((tuple)&0x7FFU) << 2U)))
#define CLKCTL_TUPLE_SEL(tuple) (((tuple) >> 11U) & 0x7U)

typedef enum _clock_attach_id
{
    kFRO24M_to_SYS_PLL  = CLKCTL0_TUPLE_MUXA(SYSPLL0CLKSEL_OFFSET, 0),
    kOSC_CLK_to_SYS_PLL = CLKCTL0_TUPLE_MUXA(SYSPLL0CLKSEL_OFFSET, 1),
    kNONE_to_SYS_PLL    = CLKCTL0_TUPLE_MUXA(SYSPLL0CLKSEL_OFFSET, 7),

    kFRO24M_to_AUDIO_PLL  = CLKCTL1_TUPLE_MUXA(AUDIOPLL0CLKSEL_OFFSET, 0),
    kOSC_CLK_to_AUDIO_PLL = CLKCTL1_TUPLE_MUXA(AUDIOPLL0CLKSEL_OFFSET, 1),
    kNONE_to_AUDIO_PLL    = CLKCTL1_TUPLE_MUXA(AUDIOPLL0CLKSEL_OFFSET, 7),

    kLPOSC_to_MAIN_CLK  = CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 0) | CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kFRO96M_to_MAIN_CLK = CLKCTL_TUPLE_FRODIVSEL(0) | CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 1) |
                          CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kFRO48M_to_MAIN_CLK = CLKCTL_TUPLE_FRODIVSEL(1) | CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 1) |
                          CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kFRO24M_to_MAIN_CLK = CLKCTL_TUPLE_FRODIVSEL(2) | CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 1) |
                          CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kFRO12M_to_MAIN_CLK = CLKCTL_TUPLE_FRODIVSEL(3) | CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 1) |
                          CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kOSC_CLK_to_MAIN_CLK  = CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 2) | CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kFRO192M_to_MAIN_CLK  = CLKCTL0_TUPLE_MUXA(MAINCLKSELA_OFFSET, 3) | CLKCTL0_TUPLE_MUXB(MAINCLKSELB_OFFSET, 0),
    kMAIN_PLL_to_MAIN_CLK = CLKCTL0_TUPLE_MUXA(MAINCLKSELB_OFFSET, 1),
    kOSC32K_to_MAIN_CLK   = CLKCTL0_TUPLE_MUXA(MAINCLKSELB_OFFSET, 2),

    kFRO192M_to_DSP_MAIN_CLK =
        CLKCTL1_TUPLE_MUXA(DSPCPUCLKSELA_OFFSET, 0) | CLKCTL1_TUPLE_MUXB(DSPCPUCLKSELB_OFFSET, 0),
    kOSC_CLK_to_DSP_MAIN_CLK =
        CLKCTL1_TUPLE_MUXA(DSPCPUCLKSELA_OFFSET, 1) | CLKCTL1_TUPLE_MUXB(DSPCPUCLKSELB_OFFSET, 0),
    kLPOSC_to_DSP_MAIN_CLK = CLKCTL1_TUPLE_MUXA(DSPCPUCLKSELA_OFFSET, 2) | CLKCTL1_TUPLE_MUXB(DSPCPUCLKSELB_OFFSET, 0),
    kMAIN_PLL_to_DSP_MAIN_CLK = CLKCTL1_TUPLE_MUXA(DSPCPUCLKSELB_OFFSET, 1),
    kDSP_PLL_to_DSP_MAIN_CLK  = CLKCTL1_TUPLE_MUXA(DSPCPUCLKSELB_OFFSET, 2),
    kOSC32K_to_DSP_MAIN_CLK   = CLKCTL1_TUPLE_MUXA(DSPCPUCLKSELB_OFFSET, 3),

    kLPOSC_to_UTICK_CLK = CLKCTL0_TUPLE_MUXA(UTICKFCLKSEL_OFFSET, 0),
    kNONE_to_UTICK_CLK  = CLKCTL0_TUPLE_MUXA(UTICKFCLKSEL_OFFSET, 7),

    kLPOSC_to_WDT0_CLK = CLKCTL0_TUPLE_MUXA(WDT0FCLKSEL_OFFSET, 0),
    kNONE_to_WDT0_CLK  = CLKCTL0_TUPLE_MUXA(WDT0FCLKSEL_OFFSET, 7),

    kLPOSC_to_WDT1_CLK = CLKCTL1_TUPLE_MUXA(WDT1FCLKSEL_OFFSET, 0),
    kNONE_to_WDT1_CLK  = CLKCTL1_TUPLE_MUXA(WDT1FCLKSEL_OFFSET, 7),

    kOSC32K_to_32KHZWAKE_CLK      = CLKCTL0_TUPLE_MUXA(A32KHZWAKECLKSEL_OFFSET, 0),
    kLPOSC_DIV32_to_32KHZWAKE_CLK = CLKCTL0_TUPLE_MUXA(A32KHZWAKECLKSEL_OFFSET, 1),
    kNONE_to_32KHZWAKE_CLK        = CLKCTL0_TUPLE_MUXA(A32KHZWAKECLKSEL_OFFSET, 7),

    kMAIN_CLK_DIV_to_SYSTICK_CLK = CLKCTL0_TUPLE_MUXA(SYSTICKFCLKSEL_OFFSET, 0),
    kLPOSC_to_SYSTICK_CLK        = CLKCTL0_TUPLE_MUXA(SYSTICKFCLKSEL_OFFSET, 1),
    kOSC32K_to_SYSTICK_CLK       = CLKCTL0_TUPLE_MUXA(SYSTICKFCLKSEL_OFFSET, 2),
    kNONE_to_SYSTICK_CLK         = CLKCTL0_TUPLE_MUXA(SYSTICKFCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_SDIO0_CLK = CLKCTL0_TUPLE_MUXA(SDIO0FCLKSEL_OFFSET, 0),
    kMAIN_PLL_to_SDIO0_CLK = CLKCTL0_TUPLE_MUXA(SDIO0FCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_SDIO0_CLK = CLKCTL0_TUPLE_MUXA(SDIO0FCLKSEL_OFFSET, 2),
    kFRO96M_to_SDIO0_CLK   = CLKCTL0_TUPLE_MUXA(SDIO0FCLKSEL_OFFSET, 3),
    kAUX1_PLL_to_SDIO0_CLK = CLKCTL0_TUPLE_MUXA(SDIO0FCLKSEL_OFFSET, 4),
    kNONE_to_SDIO0_CLK     = CLKCTL0_TUPLE_MUXA(SDIO0FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_SDIO1_CLK = CLKCTL0_TUPLE_MUXA(SDIO1FCLKSEL_OFFSET, 0),
    kMAIN_PLL_to_SDIO1_CLK = CLKCTL0_TUPLE_MUXA(SDIO1FCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_SDIO1_CLK = CLKCTL0_TUPLE_MUXA(SDIO1FCLKSEL_OFFSET, 2),
    kFRO96M_to_SDIO1_CLK   = CLKCTL0_TUPLE_MUXA(SDIO1FCLKSEL_OFFSET, 3),
    kAUX1_PLL_to_SDIO1_CLK = CLKCTL0_TUPLE_MUXA(SDIO1FCLKSEL_OFFSET, 4),
    kNONE_to_SDIO1_CLK     = CLKCTL0_TUPLE_MUXA(SDIO1FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_CTIMER0     = CLKCTL1_TUPLE_MUXA(CT32BIT0FCLKSEL_OFFSET, 0),
    kFRO192M_to_CTIMER0      = CLKCTL1_TUPLE_MUXA(CT32BIT0FCLKSEL_OFFSET, 1),
    kAUDIO_PLL_to_CTIMER0    = CLKCTL1_TUPLE_MUXA(CT32BIT0FCLKSEL_OFFSET, 2),
    kMASTER_CLK_to_CTIMER0   = CLKCTL1_TUPLE_MUXA(CT32BIT0FCLKSEL_OFFSET, 3),
    k32K_WAKE_CLK_to_CTIMER0 = CLKCTL1_TUPLE_MUXA(CT32BIT0FCLKSEL_OFFSET, 4),
    kNONE_to_CTIMER0         = CLKCTL1_TUPLE_MUXA(CT32BIT0FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_CTIMER1     = CLKCTL1_TUPLE_MUXA(CT32BIT1FCLKSEL_OFFSET, 0),
    kFRO192M_to_CTIMER1      = CLKCTL1_TUPLE_MUXA(CT32BIT1FCLKSEL_OFFSET, 1),
    kAUDIO_PLL_to_CTIMER1    = CLKCTL1_TUPLE_MUXA(CT32BIT1FCLKSEL_OFFSET, 2),
    kMASTER_CLK_to_CTIMER1   = CLKCTL1_TUPLE_MUXA(CT32BIT1FCLKSEL_OFFSET, 3),
    k32K_WAKE_CLK_to_CTIMER1 = CLKCTL1_TUPLE_MUXA(CT32BIT1FCLKSEL_OFFSET, 4),
    kNONE_to_CTIMER1         = CLKCTL1_TUPLE_MUXA(CT32BIT1FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_CTIMER2     = CLKCTL1_TUPLE_MUXA(CT32BIT2FCLKSEL_OFFSET, 0),
    kFRO192M_to_CTIMER2      = CLKCTL1_TUPLE_MUXA(CT32BIT2FCLKSEL_OFFSET, 1),
    kAUDIO_PLL_to_CTIMER2    = CLKCTL1_TUPLE_MUXA(CT32BIT2FCLKSEL_OFFSET, 2),
    kMASTER_CLK_to_CTIMER2   = CLKCTL1_TUPLE_MUXA(CT32BIT2FCLKSEL_OFFSET, 3),
    k32K_WAKE_CLK_to_CTIMER2 = CLKCTL1_TUPLE_MUXA(CT32BIT2FCLKSEL_OFFSET, 4),
    kNONE_to_CTIMER2         = CLKCTL1_TUPLE_MUXA(CT32BIT2FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_CTIMER3     = CLKCTL1_TUPLE_MUXA(CT32BIT3FCLKSEL_OFFSET, 0),
    kFRO192M_to_CTIMER3      = CLKCTL1_TUPLE_MUXA(CT32BIT3FCLKSEL_OFFSET, 1),
    kAUDIO_PLL_to_CTIMER3    = CLKCTL1_TUPLE_MUXA(CT32BIT3FCLKSEL_OFFSET, 2),
    kMASTER_CLK_to_CTIMER3   = CLKCTL1_TUPLE_MUXA(CT32BIT3FCLKSEL_OFFSET, 3),
    k32K_WAKE_CLK_to_CTIMER3 = CLKCTL1_TUPLE_MUXA(CT32BIT3FCLKSEL_OFFSET, 4),
    kNONE_to_CTIMER3         = CLKCTL1_TUPLE_MUXA(CT32BIT3FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_CTIMER4     = CLKCTL1_TUPLE_MUXA(CT32BIT4FCLKSEL_OFFSET, 0),
    kFRO192M_to_CTIMER4      = CLKCTL1_TUPLE_MUXA(CT32BIT4FCLKSEL_OFFSET, 1),
    kAUDIO_PLL_to_CTIMER4    = CLKCTL1_TUPLE_MUXA(CT32BIT4FCLKSEL_OFFSET, 2),
    kMASTER_CLK_to_CTIMER4   = CLKCTL1_TUPLE_MUXA(CT32BIT4FCLKSEL_OFFSET, 3),
    k32K_WAKE_CLK_to_CTIMER4 = CLKCTL1_TUPLE_MUXA(CT32BIT4FCLKSEL_OFFSET, 4),
    kNONE_to_CTIMER4         = CLKCTL1_TUPLE_MUXA(CT32BIT4FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_FLEXSPI0_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 0),
    kMAIN_PLL_to_FLEXSPI0_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_FLEXSPI0_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 2),
    kFRO192M_to_FLEXSPI0_CLK  = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 3),
    kAUX1_PLL_to_FLEXSPI0_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 4),
    kFRO48M_to_FLEXSPI0_CLK   = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 5),
    kFRO24M_to_FLEXSPI0_CLK   = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 6),
    kNONE_to_FLEXSPI0_CLK     = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_FLEXSPI1_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKSEL_OFFSET, 0),
    kMAIN_PLL_to_FLEXSPI1_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_FLEXSPI1_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKSEL_OFFSET, 2),
    kFRO192M_to_FLEXSPI1_CLK  = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKSEL_OFFSET, 3),
    kAUX1_PLL_to_FLEXSPI1_CLK = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKSEL_OFFSET, 4),
    kNONE_to_FLEXSPI1_CLK     = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKSEL_OFFSET, 7),

    kOSC_CLK_to_USB_CLK  = CLKCTL0_TUPLE_MUXA(USBHSFCLKSEL_OFFSET, 0),
    kMAIN_CLK_to_USB_CLK = CLKCTL0_TUPLE_MUXA(USBHSFCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_USB_CLK = CLKCTL0_TUPLE_MUXA(USBHSFCLKSEL_OFFSET, 3),
    kNONE_to_USB_CLK     = CLKCTL0_TUPLE_MUXA(USBHSFCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_SCT_CLK  = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 0),
    kMAIN_PLL_to_SCT_CLK  = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_SCT_CLK  = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 2),
    kFRO192M_to_SCT_CLK   = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 3),
    kAUX1_PLL_to_SCT_CLK  = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 4),
    kAUDIO_PLL_to_SCT_CLK = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 5),
    kNONE_to_SCT_CLK      = CLKCTL0_TUPLE_MUXA(SCTFCLKSEL_OFFSET, 7),

    kLPOSC_to_OSTIMER_CLK  = CLKCTL1_TUPLE_MUXA(OSEVENTTFCLKSEL_OFFSET, 0),
    kOSC32K_to_OSTIMER_CLK = CLKCTL1_TUPLE_MUXA(OSEVENTTFCLKSEL_OFFSET, 1),
    kNONE_to_OSTIMER_CLK   = CLKCTL1_TUPLE_MUXA(OSEVENTTFCLKSEL_OFFSET, 7),

    kFRO24M_to_MCLK_CLK    = CLKCTL1_TUPLE_MUXA(AUDIOMCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_MCLK_CLK = CLKCTL1_TUPLE_MUXA(AUDIOMCLKSEL_OFFSET, 1),
    kNONE_to_MCLK_CLK      = CLKCTL1_TUPLE_MUXA(AUDIOMCLKSEL_OFFSET, 7),

    kFRO48M_to_DMIC       = CLKCTL1_TUPLE_MUXA(DMIC0FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_DMIC    = CLKCTL1_TUPLE_MUXA(DMIC0FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_DMIC   = CLKCTL1_TUPLE_MUXA(DMIC0FCLKSEL_OFFSET, 2),
    kLPOSC_to_DMIC        = CLKCTL1_TUPLE_MUXA(DMIC0FCLKSEL_OFFSET, 3),
    k32K_WAKE_CLK_to_DMIC = CLKCTL1_TUPLE_MUXA(DMIC0FCLKSEL_OFFSET, 4),
    kNONE_to_DMIC         = CLKCTL1_TUPLE_MUXA(DMIC0FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM0     = CLKCTL1_TUPLE_MUXA(FC0FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM0  = CLKCTL1_TUPLE_MUXA(FC0FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM0 = CLKCTL1_TUPLE_MUXA(FC0FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM0        = CLKCTL1_TUPLE_MUXA(FC0FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM0       = CLKCTL1_TUPLE_MUXA(FC0FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM1     = CLKCTL1_TUPLE_MUXA(FC1FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM1  = CLKCTL1_TUPLE_MUXA(FC1FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM1 = CLKCTL1_TUPLE_MUXA(FC1FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM1        = CLKCTL1_TUPLE_MUXA(FC1FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM1       = CLKCTL1_TUPLE_MUXA(FC1FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM2     = CLKCTL1_TUPLE_MUXA(FC2FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM2  = CLKCTL1_TUPLE_MUXA(FC2FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM2 = CLKCTL1_TUPLE_MUXA(FC2FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM2        = CLKCTL1_TUPLE_MUXA(FC2FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM2       = CLKCTL1_TUPLE_MUXA(FC2FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM3     = CLKCTL1_TUPLE_MUXA(FC3FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM3  = CLKCTL1_TUPLE_MUXA(FC3FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM3 = CLKCTL1_TUPLE_MUXA(FC3FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM3        = CLKCTL1_TUPLE_MUXA(FC3FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM3       = CLKCTL1_TUPLE_MUXA(FC3FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM4     = CLKCTL1_TUPLE_MUXA(FC4FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM4  = CLKCTL1_TUPLE_MUXA(FC4FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM4 = CLKCTL1_TUPLE_MUXA(FC4FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM4        = CLKCTL1_TUPLE_MUXA(FC4FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM4       = CLKCTL1_TUPLE_MUXA(FC4FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM5     = CLKCTL1_TUPLE_MUXA(FC5FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM5  = CLKCTL1_TUPLE_MUXA(FC5FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM5 = CLKCTL1_TUPLE_MUXA(FC5FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM5        = CLKCTL1_TUPLE_MUXA(FC5FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM5       = CLKCTL1_TUPLE_MUXA(FC5FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM6     = CLKCTL1_TUPLE_MUXA(FC6FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM6  = CLKCTL1_TUPLE_MUXA(FC6FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM6 = CLKCTL1_TUPLE_MUXA(FC6FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM6        = CLKCTL1_TUPLE_MUXA(FC6FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM6       = CLKCTL1_TUPLE_MUXA(FC6FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM7     = CLKCTL1_TUPLE_MUXA(FC7FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM7  = CLKCTL1_TUPLE_MUXA(FC7FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM7 = CLKCTL1_TUPLE_MUXA(FC7FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM7        = CLKCTL1_TUPLE_MUXA(FC7FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM7       = CLKCTL1_TUPLE_MUXA(FC7FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM8     = CLKCTL1_TUPLE_MUXA(FC8FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM8  = CLKCTL1_TUPLE_MUXA(FC8FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM8 = CLKCTL1_TUPLE_MUXA(FC8FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM8        = CLKCTL1_TUPLE_MUXA(FC8FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM8       = CLKCTL1_TUPLE_MUXA(FC8FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM9     = CLKCTL1_TUPLE_MUXA(FC9FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM9  = CLKCTL1_TUPLE_MUXA(FC9FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM9 = CLKCTL1_TUPLE_MUXA(FC9FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM9        = CLKCTL1_TUPLE_MUXA(FC9FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM9       = CLKCTL1_TUPLE_MUXA(FC9FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM10     = CLKCTL1_TUPLE_MUXA(FC10FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM10  = CLKCTL1_TUPLE_MUXA(FC10FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM10 = CLKCTL1_TUPLE_MUXA(FC10FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM10        = CLKCTL1_TUPLE_MUXA(FC10FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM10       = CLKCTL1_TUPLE_MUXA(FC10FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM11     = CLKCTL1_TUPLE_MUXA(FC11FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM11  = CLKCTL1_TUPLE_MUXA(FC11FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM11 = CLKCTL1_TUPLE_MUXA(FC11FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM11        = CLKCTL1_TUPLE_MUXA(FC11FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM11       = CLKCTL1_TUPLE_MUXA(FC11FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM12     = CLKCTL1_TUPLE_MUXA(FC12FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM12  = CLKCTL1_TUPLE_MUXA(FC12FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM12 = CLKCTL1_TUPLE_MUXA(FC12FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM12        = CLKCTL1_TUPLE_MUXA(FC12FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM12       = CLKCTL1_TUPLE_MUXA(FC12FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM13     = CLKCTL1_TUPLE_MUXA(FC13FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM13  = CLKCTL1_TUPLE_MUXA(FC13FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM13 = CLKCTL1_TUPLE_MUXA(FC13FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM13        = CLKCTL1_TUPLE_MUXA(FC13FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM13       = CLKCTL1_TUPLE_MUXA(FC13FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM14     = CLKCTL1_TUPLE_MUXA(FC14FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM14  = CLKCTL1_TUPLE_MUXA(FC14FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM14 = CLKCTL1_TUPLE_MUXA(FC14FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM14        = CLKCTL1_TUPLE_MUXA(FC14FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM14       = CLKCTL1_TUPLE_MUXA(FC14FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM15     = CLKCTL1_TUPLE_MUXA(FC15FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM15  = CLKCTL1_TUPLE_MUXA(FC15FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM15 = CLKCTL1_TUPLE_MUXA(FC15FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM15        = CLKCTL1_TUPLE_MUXA(FC15FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM15       = CLKCTL1_TUPLE_MUXA(FC15FCLKSEL_OFFSET, 7),

    kFRO48M_to_FLEXCOMM16     = CLKCTL1_TUPLE_MUXA(FC16FCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXCOMM16  = CLKCTL1_TUPLE_MUXA(FC16FCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXCOMM16 = CLKCTL1_TUPLE_MUXA(FC16FCLKSEL_OFFSET, 2),
    kFRG_to_FLEXCOMM16        = CLKCTL1_TUPLE_MUXA(FC16FCLKSEL_OFFSET, 3),
    kNONE_to_FLEXCOMM16       = CLKCTL1_TUPLE_MUXA(FC16FCLKSEL_OFFSET, 7),

    kFRO96M_to_FLEXIO     = CLKCTL1_TUPLE_MUXA(FLEXIOCLKSEL_OFFSET, 0),
    kAUDIO_PLL_to_FLEXIO  = CLKCTL1_TUPLE_MUXA(FLEXIOCLKSEL_OFFSET, 1),
    kMASTER_CLK_to_FLEXIO = CLKCTL1_TUPLE_MUXA(FLEXIOCLKSEL_OFFSET, 2),
    kFRG_to_FLEXIO        = CLKCTL1_TUPLE_MUXA(FLEXIOCLKSEL_OFFSET, 3),
    kNONE_to_FLEXIO       = CLKCTL1_TUPLE_MUXA(FLEXIOCLKSEL_OFFSET, 7),

    kMAIN_CLK_to_I3C_CLK = CLKCTL1_TUPLE_MUXA(I3C01FCLKSEL_OFFSET, 0),
    kFRO24M_to_I3C_CLK   = CLKCTL1_TUPLE_MUXA(I3C01FCLKSEL_OFFSET, 1),
    kNONE_to_I3C_CLK     = CLKCTL1_TUPLE_MUXA(I3C01FCLKSEL_OFFSET, 7),

    kI3C_CLK_to_I3C_TC_CLK = CLKCTL1_TUPLE_MUXA(I3C01FCLKSTCSEL_OFFSET, 0),
    kLPOSC_to_I3C_TC_CLK   = CLKCTL1_TUPLE_MUXA(I3C01FCLKSTCSEL_OFFSET, 1),
    kNONE_to_I3C_TC_CLK    = CLKCTL1_TUPLE_MUXA(I3C01FCLKSTCSEL_OFFSET, 7),

    kMAIN_CLK_to_ACMP_CLK = CLKCTL1_TUPLE_MUXA(ACMP0FCLKSEL_OFFSET, 0),
    kFRO48M_to_ACMP_CLK   = CLKCTL1_TUPLE_MUXA(ACMP0FCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_ACMP_CLK = CLKCTL1_TUPLE_MUXA(ACMP0FCLKSEL_OFFSET, 2),
    kAUX1_PLL_to_ACMP_CLK = CLKCTL1_TUPLE_MUXA(ACMP0FCLKSEL_OFFSET, 3),
    kNONE_to_ACMP_CLK     = CLKCTL1_TUPLE_MUXA(ACMP0FCLKSEL_OFFSET, 7),

    kOSC_CLK_to_ADC_CLK  = CLKCTL0_TUPLE_MUXA(ADC0FCLKSEL0_OFFSET, 0) | CLKCTL0_TUPLE_MUXB(ADC0FCLKSEL1_OFFSET, 0),
    kLPOSC_to_ADC_CLK    = CLKCTL0_TUPLE_MUXA(ADC0FCLKSEL0_OFFSET, 1) | CLKCTL0_TUPLE_MUXB(ADC0FCLKSEL1_OFFSET, 0),
    kFRO48M_to_ADC_CLK   = CLKCTL0_TUPLE_MUXA(ADC0FCLKSEL0_OFFSET, 2) | CLKCTL0_TUPLE_MUXB(ADC0FCLKSEL1_OFFSET, 0),
    kMAIN_PLL_to_ADC_CLK = CLKCTL0_TUPLE_MUXA(ADC0FCLKSEL1_OFFSET, 1),
    kAUX0_PLL_to_ADC_CLK = CLKCTL0_TUPLE_MUXA(ADC0FCLKSEL1_OFFSET, 2),
    kAUX1_PLL_to_ADC_CLK = CLKCTL0_TUPLE_MUXA(ADC0FCLKSEL1_OFFSET, 3),

    kOSC_CLK_to_CLKOUT   = CLKCTL1_TUPLE_MUXA(CLKOUTSEL0_OFFSET, 0) | CLKCTL1_TUPLE_MUXB(CLKOUTSEL1_OFFSET, 0),
    kLPOSC_to_CLKOUT     = CLKCTL1_TUPLE_MUXA(CLKOUTSEL0_OFFSET, 1) | CLKCTL1_TUPLE_MUXB(CLKOUTSEL1_OFFSET, 0),
    kFRO192M_to_CLKOUT   = CLKCTL1_TUPLE_MUXA(CLKOUTSEL0_OFFSET, 2) | CLKCTL1_TUPLE_MUXB(CLKOUTSEL1_OFFSET, 0),
    kMAIN_CLK_to_CLKOUT  = CLKCTL1_TUPLE_MUXA(CLKOUTSEL0_OFFSET, 3) | CLKCTL1_TUPLE_MUXB(CLKOUTSEL1_OFFSET, 0),
    kDSP_MAIN_to_CLKOUT  = CLKCTL1_TUPLE_MUXA(CLKOUTSEL0_OFFSET, 4) | CLKCTL1_TUPLE_MUXB(CLKOUTSEL1_OFFSET, 0),
    kMAIN_PLL_to_CLKOUT  = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 1),
    kAUX0_PLL_to_CLKOUT  = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 2),
    kDSP_PLL_to_CLKOUT   = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 3),
    kAUX1_PLL_to_CLKOUT  = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 4),
    kAUDIO_PLL_to_CLKOUT = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 5),
    kOSC32K_to_CLKOUT    = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 6),
    kNONE_to_CLKOUT      = CLKCTL1_TUPLE_MUXA(CLKOUTSEL1_OFFSET, 7),

    kMAIN_CLK_to_GPU_CLK = CLKCTL0_TUPLE_MUXA(GPUCLKSEL_OFFSET, 0),
    kFRO192M_to_GPU_CLK  = CLKCTL0_TUPLE_MUXA(GPUCLKSEL_OFFSET, 1),
    kMAIN_PLL_to_GPU_CLK = CLKCTL0_TUPLE_MUXA(GPUCLKSEL_OFFSET, 2),
    kAUX0_PLL_to_GPU_CLK = CLKCTL0_TUPLE_MUXA(GPUCLKSEL_OFFSET, 3),
    kAUX1_PLL_to_GPU_CLK = CLKCTL0_TUPLE_MUXA(GPUCLKSEL_OFFSET, 4),
    kNONE_to_GPU_CLK     = CLKCTL0_TUPLE_MUXA(GPUCLKSEL_OFFSET, 7),

    kFRO192M_to_MIPI_DPHY_CLK  = CLKCTL0_TUPLE_MUXA(DPHYCLKSEL_OFFSET, 0),
    kMAIN_PLL_to_MIPI_DPHY_CLK = CLKCTL0_TUPLE_MUXA(DPHYCLKSEL_OFFSET, 1),
    kAUX0_PLL_to_MIPI_DPHY_CLK = CLKCTL0_TUPLE_MUXA(DPHYCLKSEL_OFFSET, 2),
    kAUX1_PLL_to_MIPI_DPHY_CLK = CLKCTL0_TUPLE_MUXA(DPHYCLKSEL_OFFSET, 3),
    kNONE_to_MIPI_DPHY_CLK     = CLKCTL0_TUPLE_MUXA(DPHYCLKSEL_OFFSET, 7),

    kFRO192M_to_MIPI_DPHYESC_CLK = CLKCTL0_TUPLE_MUXA(DPHYESCCLKSEL_OFFSET, 0),
    kFRO12M_to_MIPI_DPHYESC_CLK  = CLKCTL0_TUPLE_MUXA(DPHYESCCLKSEL_OFFSET, 1),

    kMIPI_DPHY_CLK_to_DCPIXEL_CLK = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 0),
    kMAIN_CLK_to_DCPIXEL_CLK      = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 1),
    kFRO192M_to_DCPIXEL_CLK       = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 2),
    kMAIN_PLL_to_DCPIXEL_CLK      = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 3),
    kAUX0_PLL_to_DCPIXEL_CLK      = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 4),
    kAUX1_PLL_to_DCPIXEL_CLK      = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 5),
    kNONE_to_DCPIXEL_CLK          = CLKCTL0_TUPLE_MUXA(DCPIXELCLKSEL_OFFSET, 7),
} clock_attach_id_t;

/*  Clock dividers */
typedef enum _clock_div_name
{
    kCLOCK_DivAudioPllClk  = CLKCTL1_TUPLE_MUXA(AUDIOPLLCLKDIV_OFFSET, 0),
    kCLOCK_DivPfc0Clk      = CLKCTL0_TUPLE_MUXA(PFC0CLKDIV_OFFSET, 0),
    kCLOCK_DivPfc1Clk      = CLKCTL0_TUPLE_MUXA(PFC1CLKDIV_OFFSET, 0),
    kCLOCK_DivSysCpuAhbClk = CLKCTL0_TUPLE_MUXA(SYSCPUAHBCLKDIV_OFFSET, 0),
    kCLOCK_Div32KhzWakeClk = CLKCTL0_TUPLE_MUXA(A32KHZWAKECLKDIV_OFFSET, 0),
    kCLOCK_DivSystickClk   = CLKCTL0_TUPLE_MUXA(SYSTICKFCLKDIV_OFFSET, 0),
    kCLOCK_DivSdio0Clk     = CLKCTL0_TUPLE_MUXA(SDIO0FCLKDIV_OFFSET, 0),
    kCLOCK_DivSdio1Clk     = CLKCTL0_TUPLE_MUXA(SDIO1FCLKDIV_OFFSET, 0),
    kCLOCK_DivFlexspi0Clk  = CLKCTL0_TUPLE_MUXA(FLEXSPI0FCLKDIV_OFFSET, 0),
    kCLOCK_DivFlexspi1Clk  = CLKCTL0_TUPLE_MUXA(FLEXSPI1FCLKDIV_OFFSET, 0),
    kCLOCK_DivUsbHsFclk    = CLKCTL0_TUPLE_MUXA(USBHSFCLKDIV_OFFSET, 0),
    kCLOCK_DivSctClk       = CLKCTL0_TUPLE_MUXA(SCTFCLKDIV_OFFSET, 0),
    kCLOCK_DivMclkClk      = CLKCTL1_TUPLE_MUXA(AUDIOMCLKDIV_OFFSET, 0),
    kCLOCK_DivDmicClk      = CLKCTL1_TUPLE_MUXA(DMIC0FCLKDIV_OFFSET, 0),
    kCLOCK_DivPLLFRGClk    = CLKCTL1_TUPLE_MUXA(FRGPLLCLKDIV_OFFSET, 0),
    kCLOCK_DivFlexioClk    = CLKCTL1_TUPLE_MUXA(FLEXIOCLKDIV_OFFSET, 0),
    kCLOCK_DivI3cClk       = CLKCTL1_TUPLE_MUXA(I3C01FCLKDIV_OFFSET, 0),
    kCLOCK_DivI3cTcClk     = CLKCTL1_TUPLE_MUXA(I3C01FCLKSTCDIV_OFFSET, 0),
    kCLOCK_DivI3cSlowClk   = CLKCTL1_TUPLE_MUXA(I3C01FCLKSDIV_OFFSET, 0),
    kCLOCK_DivDspCpuClk    = CLKCTL1_TUPLE_MUXA(DSPCPUCLKDIV_OFFSET, 0),
    kCLOCK_DivAcmpClk      = CLKCTL1_TUPLE_MUXA(ACMP0FCLKDIV_OFFSET, 0),
    kCLOCK_DivAdcClk       = CLKCTL0_TUPLE_MUXA(ADC0FCLKDIV_OFFSET, 0),
    kCLOCK_DivLowFreqClk   = CLKCTL0_TUPLE_MUXA(LOWFREQCLKDIV_OFFSET, 0),
    kCLOCK_DivClockOut     = CLKCTL1_TUPLE_MUXA(CLKOUTFCLKDIV_OFFSET, 0),
    kCLOCK_DivGpuClk       = CLKCTL0_TUPLE_MUXA(GPUCLKDIV_OFFSET, 0),
    kCLOCK_DivDcPixelClk   = CLKCTL0_TUPLE_MUXA(DCPIXELCLKDIV_OFFSET, 0),
    kCLOCK_DivDphyClk      = CLKCTL0_TUPLE_MUXA(DPHYCLKDIV_OFFSET, 0),
    kCLOCK_DivDphyEscRxClk = CLKCTL0_TUPLE_MUXA(DPHYESCRXCLKDIV_OFFSET, 0),
    kCLOCK_DivDphyEscTxClk = CLKCTL0_TUPLE_MUXA(DPHYESCTXCLKDIV_OFFSET, 0),
} clock_div_name_t;

/*! @brief PLL configuration for SYSPLL */
typedef struct _clock_sys_pll_config
{
    enum
    {
        kCLOCK_SysPllFro24MClk = 0, /*!< 24MHz FRO clock */
        kCLOCK_SysPllXtalIn    = 1, /*!< OSC clock */
        kCLOCK_SysPllNone      = 7  /*!< Gated to reduce power */
    } sys_pll_src;
    uint32_t numerator;   /*!< 30 bit numerator of fractional loop divider. */
    uint32_t denominator; /*!< 30 bit numerator of fractional loop divider. */
    enum
    {
        kCLOCK_SysPllMult16 = 0,
        kCLOCK_SysPllMult17,
        kCLOCK_SysPllMult20,
        kCLOCK_SysPllMult22,
        kCLOCK_SysPllMult27,
        kCLOCK_SysPllMult33,
    } sys_pll_mult;
} clock_sys_pll_config_t;

/*! @brief PLL configuration for SYSPLL */
typedef struct _clock_audio_pll_config
{
    enum
    {
        kCLOCK_AudioPllFro24MClk = 0, /*!< 24MHz FRO clock */
        kCLOCK_AudioPllXtalIn    = 1, /*!< OSC clock */
        kCLOCK_AudioPllNone      = 7  /*!< Gated to reduce power */
    } audio_pll_src;
    uint32_t numerator;   /*!< 30 bit numerator of fractional loop divider. */
    uint32_t denominator; /*!< 30 bit numerator of fractional loop divider. */
    enum
    {
        kCLOCK_AudioPllMult16 = 0,
        kCLOCK_AudioPllMult17,
        kCLOCK_AudioPllMult20,
        kCLOCK_AudioPllMult22,
        kCLOCK_AudioPllMult27,
        kCLOCK_AudioPllMult33,
    } audio_pll_mult;
} clock_audio_pll_config_t;

/*! @brief PLL configuration for FRG */
typedef struct _clock_frg_clk_config
{
    uint8_t num; /*!< FRG clock, [0 - 16]: Flexcomm, [17]: Flexio */
    enum
    {
        kCLOCK_FrgMainClk = 0, /*!< Main System clock */
        kCLOCK_FrgPllDiv,      /*!< Main pll clock divider*/
        kCLOCK_FrgFro48M,      /*!< 48MHz FRO */
    } sfg_clock_src;
    uint8_t divider; /*!< PLL loop divider. */
    uint8_t mult;    /*!< PLL loop divider. */
} clock_frg_clk_config_t;

/*! @brief FRO192M output enable */
typedef enum _clock_fro_output_en
{
    kCLOCK_FroDiv1OutEn  = CLKCTL0_FRODIVOEN_FRO_DIV1_O_EN_MASK,
    kCLOCK_FroDiv2OutEn  = CLKCTL0_FRODIVOEN_FRO_DIV2_O_EN_MASK,
    kCLOCK_FroDiv4OutEn  = CLKCTL0_FRODIVOEN_FRO_DIV4_O_EN_MASK,
    kCLOCK_FroDiv8OutEn  = CLKCTL0_FRODIVOEN_FRO_DIV8_O_EN_MASK,
    kCLOCK_FroDiv16OutEn = CLKCTL0_FRODIVOEN_FRO_DIV16_O_EN_MASK,
    kCLOCK_FroAllOutEn   = CLKCTL0_FRODIVOEN_FRO_DIV1_O_EN_MASK | CLKCTL0_FRODIVOEN_FRO_DIV2_O_EN_MASK |
                         CLKCTL0_FRODIVOEN_FRO_DIV4_O_EN_MASK | CLKCTL0_FRODIVOEN_FRO_DIV8_O_EN_MASK |
                         CLKCTL0_FRODIVOEN_FRO_DIV16_O_EN_MASK
} clock_fro_output_en_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

static inline void CLOCK_EnableClock(clock_ip_name_t clk)
{
    uint32_t index = CLK_GATE_ABSTRACT_REG_OFFSET(clk);

    switch (index)
    {
        case CLK_CTL0_PSCCTL0:
            CLKCTL0->PSCCTL0_SET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL0_PSCCTL1:
            CLKCTL0->PSCCTL1_SET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL0_PSCCTL2:
            CLKCTL0->PSCCTL2_SET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL1_PSCCTL0:
            CLKCTL1->PSCCTL0_SET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL1_PSCCTL1:
            CLKCTL1->PSCCTL1_SET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL1_PSCCTL2:
            CLKCTL1->PSCCTL2_SET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        default:
            break;
    }
}

static inline void CLOCK_DisableClock(clock_ip_name_t clk)
{
    uint32_t index = CLK_GATE_ABSTRACT_REG_OFFSET(clk);
    switch (index)
    {
        case CLK_CTL0_PSCCTL0:
            CLKCTL0->PSCCTL0_CLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL0_PSCCTL1:
            CLKCTL0->PSCCTL1_CLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL0_PSCCTL2:
            CLKCTL0->PSCCTL2_CLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL1_PSCCTL0:
            CLKCTL1->PSCCTL0_CLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL1_PSCCTL1:
            CLKCTL1->PSCCTL1_CLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        case CLK_CTL1_PSCCTL2:
            CLKCTL1->PSCCTL2_CLR = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(clk));
            break;
        default:
            break;
    }
}

/**
 * @brief   Configure the clock selection muxes.
 * @param   connection  : Clock to be configured.
 * @return  Nothing
 */
void CLOCK_AttachClk(clock_attach_id_t connection);

/**
 * @brief   Setup peripheral clock dividers.
 * @param   div_name    : Clock divider name
 * @param   divider     : Value to be divided.
 * @return  Nothing
 */
void CLOCK_SetClkDiv(clock_div_name_t div_name, uint32_t divider);

/*! @brief  Return Frequency of selected clock
 *  @return Frequency of selected clock
 */
uint32_t CLOCK_GetFreq(clock_name_t clockName);

/*! @brief  Return Input frequency for the Fractional baud rate generator
 *  @return Input Frequency for FRG
 */
uint32_t CLOCK_GetFRGClock(uint32_t id);

/*! @brief  Set output of the Fractional baud rate generator
 * @param   config    : Configuration to set to FRGn clock.
 */
void CLOCK_SetFRGClock(const clock_frg_clk_config_t *config);

/*! @brief  Return Frequency of SYSPLL
 *  @return Frequency of SYSPLL
 */
uint32_t CLOCK_GetSysPllFreq(void);

/*! @brief  Get current output frequency of specific System PLL PFD.
 *  @param   pfd    : pfd name to get frequency.
 *  @return  Frequency of SYSPLL PFD.
 */
uint32_t CLOCK_GetSysPfdFreq(clock_pfd_t pfd);

/*! @brief  Return Frequency of AUDIO PLL
 *  @return Frequency of AUDIO PLL
 */
uint32_t CLOCK_GetAudioPllFreq(void);

/*! @brief  Get current output frequency of specific Audio PLL PFD.
 *  @param   pfd    : pfd name to get frequency.
 *  @return  Frequency of AUDIO PLL PFD.
 */
uint32_t CLOCK_GetAudioPfdFreq(clock_pfd_t pfd);

/*! @brief  Return Frequency of main clk
 *  @return Frequency of main clk
 */
uint32_t CLOCK_GetMainClkFreq(void);

/*! @brief  Return Frequency of DSP main clk
 *  @return Frequency of DSP main clk
 */
uint32_t CLOCK_GetDspMainClkFreq(void);

/*! @brief  Return Frequency of ACMP clk
 *  @return Frequency of ACMP clk
 */
uint32_t CLOCK_GetAcmpClkFreq(void);

/*! @brief  Return Frequency of DMIC clk
 *  @return Frequency of DMIC clk
 */
uint32_t CLOCK_GetDmicClkFreq(void);

/*! @brief  Return Frequency of USB clk
 *  @return Frequency of USB clk
 */
uint32_t CLOCK_GetUsbClkFreq(void);

/*! @brief  Return Frequency of SDIO clk
 *  @param  id : SDIO index to get frequency.
 *  @return Frequency of SDIO clk
 */
uint32_t CLOCK_GetSdioClkFreq(uint32_t id);

/*! @brief  Return Frequency of I3C clk
 *  @return Frequency of I3C clk
 */
uint32_t CLOCK_GetI3cClkFreq(void);

/*! @brief  Return Frequency of systick clk
 *  @return Frequency of systick clk
 */
uint32_t CLOCK_GetSystickClkFreq(void);

/*! @brief  Return Frequency of WDT clk
 *  @param  id : WDT index to get frequency.
 *  @return Frequency of WDT clk
 */
uint32_t CLOCK_GetWdtClkFreq(uint32_t id);

/*! @brief  Return output Frequency of mclk
 *  @return Frequency of mclk output clk
 */
uint32_t CLOCK_GetMclkClkFreq(void);

/*! @brief  Return Frequency of sct
 *  @return Frequency of sct clk
 */
uint32_t CLOCK_GetSctClkFreq(void);

/*! @brief  Enable/Disable sys osc clock from external crystal clock.
 *  @param  enable : true to enable system osc clock, false to bypass system osc.
 *  @param  delay_us : Delay time after OSC power up.
 */
void CLOCK_EnableSysOscClk(bool enable, uint32_t delay_us);

/*! @brief  Enable/Disable FRO192M clock output.
 *  @param  divOutEnable : Or'ed value of clock_fro_output_en_t to enable certain clock freq output.
 */
void CLOCK_EnableFroClk(uint32_t divOutEnable);

/*! @brief  Enable LPOSC 1MHz clock.
 */
void CLOCK_EnableLpOscClk(void);

/*! @brief  Return Frequency of sys osc Clock
 *  @return Frequency of sys osc Clock. Or CLK_IN pin frequency.
 */
static inline uint32_t CLOCK_GetXtalInClkFreq(void)
{
    return (CLKCTL0->SYSOSC_BYPASS == 0U) ? g_xtalFreq : ((CLKCTL0->SYSOSC_BYPASS == 1U) ? CLK_CLK_IN : 0U);
}

/*! @brief  Return Frequency of MCLK Input Clock
 *  @return Frequency of MCLK input Clock.
 */
static inline uint32_t CLOCK_GetMclkInClkFreq(void)
{
    return g_mclkFreq;
}

/*! @brief  Return Frequency of Lower power osc
 *  @return Frequency of LPOSC
 */
static inline uint32_t CLOCK_GetLpOscFreq(void)
{
    return CLK_LPOSC_1MHZ;
}

/*! @brief  Return Frequency of 32kHz osc
 *  @return Frequency of 32kHz osc
 */
static inline uint32_t CLOCK_GetOsc32KFreq(void)
{
    return (CLKCTL0->A32KHZOSCCTL0 & CLKCTL0_A32KHZOSCCTL0_A32KHZOSCCTL0_MASK) ? CLK_RTC_32K_CLK : 0U;
}

/*! @brief  Enables and disables 32kHz osc
 *  @param  enable : true to enable 32k osc clock, false to disable clock
 */
static inline void CLOCK_EnableOsc32K(bool enable)
{
    if (enable)
    {
        CLKCTL0->A32KHZOSCCTL0 |= CLKCTL0_A32KHZOSCCTL0_A32KHZOSCCTL0_MASK;
    }
    else
    {
        CLKCTL0->A32KHZOSCCTL0 &= ~CLKCTL0_A32KHZOSCCTL0_A32KHZOSCCTL0_MASK;
    }
}

/*! @brief  Return Frequency of 32khz wake clk
 *  @return Frequency of 32kHz wake clk
 */
static inline uint32_t CLOCK_GetWakeClk32KFreq(void)
{
    return (CLKCTL0->A32KHZWAKECLKSEL & CLKCTL0_A32KHZWAKECLKSEL_SEL_MASK) ?
               CLOCK_GetLpOscFreq() / ((CLKCTL0->A32KHZWAKECLKDIV & CLKCTL0_A32KHZWAKECLKDIV_DIV_MASK) + 1U) :
               CLOCK_GetOsc32KFreq();
}

/*!
 * @brief Set the XTALIN (system OSC) frequency based on board setting.
 *
 * @param freq : The XTAL input clock frequency in Hz.
 */
static inline void CLOCK_SetXtalFreq(uint32_t freq)
{
    g_xtalFreq = freq;
}

/*!
 * @brief Set the MCLK IN frequency based on board setting.
 *
 * @param freq : The MCLK input clock frequency in Hz.
 */
static inline void CLOCK_SetMclkFreq(uint32_t freq)
{
    g_mclkFreq = freq;
}

/*! @brief  Return Frequency of Flexcomm functional Clock
 *  @param  id : flexcomm index to get frequency.
 *  @return Frequency of Flexcomm functional Clock
 */
uint32_t CLOCK_GetFlexcommClkFreq(uint32_t id);

/*! @brief  Return Frequency of Flexio functional Clock
 *  @return Frequency of Flexcomm functional Clock
 */
uint32_t CLOCK_GetFlexioClkFreq(void);

/*! @brief  Return Frequency of Ctimer Clock
 *  @param  id : ctimer index to get frequency.
 *  @return Frequency of Ctimer Clock
 */
uint32_t CLOCK_GetCtimerClkFreq(uint32_t id);
/*! @brief  Return Frequency of ClockOut
 *  @return Frequency of ClockOut
 */
uint32_t CLOCK_GetClockOutClkFreq(void);
/*! @brief  Return Frequency of Adc Clock
 *  @return Frequency of Adc Clock.
 */
uint32_t CLOCK_GetAdcClkFreq(void);
/*! @brief  Return Frequency of FLEXSPI Clock
 *  @param  id : flexspi index to get frequency.
 *  @return Frequency of Flexspi.
 */
uint32_t CLOCK_GetFlexspiClkFreq(uint32_t id);

/*! @brief  Return Frequency of GPU functional Clock
 *  @return Frequency of GPU functional Clock
 */
uint32_t CLOCK_GetGpuClkFreq(void);

/*! @brief  Return Frequency of DCNano Pixel functional Clock
 *  @return Frequency of DCNano pixel functional Clock
 */
uint32_t CLOCK_GetDcPixelClkFreq(void);

/*! @brief  Return Frequency of MIPI DPHY functional Clock
 *  @return Frequency of MIPI DPHY functional Clock
 */
uint32_t CLOCK_GetMipiDphyClkFreq(void);

/*! @brief  Return Frequency of MIPI DPHY Esc RX functional Clock
 *  @return Frequency of MIPI DPHY Esc RX functional Clock
 */
uint32_t CLOCK_GetMipiDphyEscRxClkFreq(void);

/*! @brief  Return Frequency of MIPI DPHY Esc Tx functional Clock
 *  @return Frequency of MIPI DPHY Esc Tx functional Clock
 */
uint32_t CLOCK_GetMipiDphyEscTxClkFreq(void);

#ifndef __XCC__
/*! @brief  Initialize the System PLL.
 *  @param  config    : Configuration to set to PLL.
 */
void CLOCK_InitSysPll(const clock_sys_pll_config_t *config);

/*! brief  Deinit the System PLL.
 *  param  none.
 */
static inline void CLOCK_DeinitSysPll(void)
{
    /* Set System PLL Reset & HOLDRINGOFF_ENA */
    CLKCTL0->SYSPLL0CTL0 |= CLKCTL0_SYSPLL0CTL0_HOLDRINGOFF_ENA_MASK | CLKCTL0_SYSPLL0CTL0_RESET_MASK;
    /* Power down System PLL*/
    SYSCTL0->PDRUNCFG0_SET = SYSCTL0_PDRUNCFG0_SYSPLLLDO_PD_MASK | SYSCTL0_PDRUNCFG0_SYSPLLANA_PD_MASK;
}

/*! @brief Initialize the System PLL PFD.
 *  @param pfd    : Which PFD clock to enable.
 *  @param divider    : The PFD divider value.
 *  @note It is recommended that PFD settings are kept between 12-35.
 */
void CLOCK_InitSysPfd(clock_pfd_t pfd, uint8_t divider);

/*! brief Disable the audio PLL PFD.
 *  param pfd    : Which PFD clock to disable.
 */
static inline void CLOCK_DeinitSysPfd(clock_pfd_t pfd)
{
    CLKCTL0->SYSPLL0PFD |= (CLKCTL1_AUDIOPLL0PFD_PFD0_CLKGATE_MASK << (8 * pfd));
}

/*! @brief  Initialize the audio PLL.
 *  @param  config    : Configuration to set to PLL.
 */
void CLOCK_InitAudioPll(const clock_audio_pll_config_t *config);

/*! brief  Deinit the Audio PLL.
 *  param  none.
 */
static inline void CLOCK_DeinitAudioPll(void)
{
    /* Set Audio PLL Reset & HOLDRINGOFF_ENA */
    CLKCTL1->AUDIOPLL0CTL0 |= CLKCTL1_AUDIOPLL0CTL0_HOLDRINGOFF_ENA_MASK | CLKCTL1_AUDIOPLL0CTL0_RESET_MASK;
    /* Power down Audio PLL */
    SYSCTL0->PDRUNCFG0_SET = SYSCTL0_PDRUNCFG0_AUDPLLLDO_PD_MASK | SYSCTL0_PDRUNCFG0_AUDPLLANA_PD_MASK;
}

/*! @brief Initialize the audio PLL PFD.
 *  @param pfd    : Which PFD clock to enable.
 *  @param divider    : The PFD divider value.
 *  @note It is recommended that PFD settings are kept between 12-35.
 */
void CLOCK_InitAudioPfd(clock_pfd_t pfd, uint8_t divider);

/*! brief Disable the audio PLL PFD.
 *  param pfd    : Which PFD clock to disable.
 */
static inline void CLOCK_DeinitAudioPfd(uint32_t pfd)
{
    CLKCTL1->AUDIOPLL0PFD |= (CLKCTL1_AUDIOPLL0PFD_PFD0_CLKGATE_MASK << (8 * pfd));
}

/*! @brief  Enable/Disable FRO tuning.
 *   On enable, the function will wait until FRO is close to the target frequency.
 */
void CLOCK_EnableFroTuning(bool enable);

/*! @brief Enable USB HS device clock.
 *
 * This function enables USB HS device clock.
 */
void CLOCK_EnableUsbHs0DeviceClock(clock_attach_id_t src, uint8_t divider);

/*! @brief Disable USB HS device clock.
 *
 * This function disables USB HS device clock.
 */
void CLOCK_DisableUsbHs0DeviceClock(void);

/*! @brief Enable USB HS host clock.
 *
 * This function enables USB HS host clock.
 */
void CLOCK_EnableUsbHs0HostClock(clock_attach_id_t src, uint8_t divider);

/*! @brief Disable USB HS host clock.
 *
 * This function disables USB HS host clock.
 */
void CLOCK_DisableUsbHs0HostClock(void);

/*! brief Enable USB hs0PhyPll clock.
 *
 * param src  USB HS clock source.
 * param freq The frequency specified by src.
 * retval true The clock is set successfully.
 * retval false The clock source is invalid to get proper USB HS clock.
 */
bool CLOCK_EnableUsbHs0PhyPllClock(clock_attach_id_t src, uint32_t freq);

/*! @brief Disable USB hs0PhyPll clock.
 *
 * This function disables USB hs0PhyPll clock.
 */
void CLOCK_DisableUsbHs0PhyPllClock();

/*!
 * @brief Use DWT to delay at least for some time.
 *  Please note that, this API will calculate the microsecond period with the maximum
 *  supported CPU frequency, so this API will only delay for at least the given microseconds, if precise
 *  delay count was needed, please implement a new timer count to achieve this function.
 *
 * @param delay_us  Delay time in unit of microsecond.
 */
void SDK_DelayAtLeastUs(uint32_t delay_us);
#endif /* __XCC__ */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_CLOCK_H_ */
