/*
 * Copyright 2020, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_CLOCK_H_
#define _FSL_CLOCK_H_

#include "fsl_common.h"

/*! @addtogroup clock */
/*! @{ */

/*! @file */

/*******************************************************************************
 * Configurations
 ******************************************************************************/

/*! @brief Configure whether driver controls clock
 *
 * When set to 0, peripheral drivers will enable clock in initialize function
 * and disable clock in de-initialize function. When set to 1, peripheral
 * driver will not control the clock, application could control the clock out of
 * the driver.
 *
 * @note All drivers share this feature switcher. If it is set to 1, application
 * should handle clock enable and disable for all drivers.
 */
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL))
#define FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL 0
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief CLOCK driver version 2.0.1. */
#define FSL_CLOCK_DRIVER_VERSION (MAKE_VERSION(2, 0, 1))
/*@}*/

/*! @brief External crystal MAINXTAL clock frequency.
 *
 * The MAINXTAL clock frequency in Hz. When the clock is set up, use the
 * function CLOCK_SetMainXtalFreq to set the value in the clock driver. For example,
 * if MAINXTAL is 38.4 MHz:
 * @code
 * CLOCK_SetMainXtalFreq(384000000);
 * @endcode
 */
extern volatile uint32_t g_mainXtalFreq;

/* Definition for delay API in clock driver, users can redefine it to the real application. */
#ifndef SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY
#define SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY (200000000UL)
#endif

/*! @brief Clock ip name array for GPIO. */
#define GPIO_CLOCKS \
    {               \
        kCLOCK_Gpio \
    }

/*! @brief Clock ip name array for I2C. */
#define I2C_CLOCKS               \
    {                            \
        kCLOCK_I2c0, kCLOCK_I2c1 \
    }

/*! @brief Clock ip name array for QSPI. */
#define QSPI_CLOCKS \
    {               \
        kCLOCK_Qspi \
    }

/*! @brief Clock ip name array for UART. */
#define UART_CLOCKS                              \
    {                                            \
        kCLOCK_Uart0, kCLOCK_Uart1, kCLOCK_Uart2 \
    }

/*! @brief Clock ip name array for SSP. */
#define SSP_CLOCKS                            \
    {                                         \
        kCLOCK_Ssp0, kCLOCK_Ssp1, kCLOCK_Ssp2 \
    }

/*! @brief Clock ip name array for GPT. */
#define GPT_CLOCKS                                         \
    {                                                      \
        kCLOCK_Gpt0, kCLOCK_Gpt1, kCLOCK_Gpt2, kCLOCK_Gpt3 \
    }

/*! @brief Clock ip name array for WDT. */
#define WDT_CLOCKS \
    {              \
        kCLOCK_Wdt \
    }

/*! @brief Clock ip name array for RTC. */
#define RTC_CLOCKS \
    {              \
        kCLOCK_Rtc \
    }

/*! @brief Clock ip name array for ACOMP. */
#define ACOMP_CLOCKS     \
    {                    \
        kCLOCK_AcompMclk \
    }

/*! @brief Clock ip name array for ADC. */
#define ADC_CLOCKS     \
    {                  \
        kCLOCK_AdcMclk \
    }

/*! @brief Clock ip name array for DAC. */
#define DAC_CLOCKS     \
    {                  \
        kCLOCK_DacMclk \
    }

/*! @brief Clock ip name array for DMAC. */
#define DMAC_CLOCKS \
    {               \
        kCLOCK_Dma  \
    }

/*! @brief Clock operation code.
 *  [31:16] Register offset from PMU base.
 *  [15:11] Register bit position.
 *  [10: 8] Register bits width to operate.
 *  [ 7: 0] Value to set to the bits.
 */
#define CLK_OP_CODE_DEFINE(regOff, bitPos, bitWid, val) \
    (((regOff) << 16U) | ((bitPos) << 11U) | ((bitWid) << 8U) | (val))
#define CLK_OP_REG(code)   (*((volatile uint32_t *)(PMU_BASE + (((uint32_t)(code)) >> 16U))))
#define CLK_OP_POS(code)   ((((uint32_t)(code)) >> 11U) & 0x1FU)
#define CLK_OP_MASK(code)  ((1UL << ((((uint32_t)(code)) >> 8U) & 0x7U)) - 1U)
#define CLK_OP_VALUE(code) (((uint32_t)(code)) & 0xFFU)

/*!
 * @brief Peripheral clock name difinition used for
 * clock gate.
 */
typedef enum _clock_ip_name
{
    kCLOCK_IpInvalid = 0U,

    kCLOCK_Qspi    = CLK_OP_CODE_DEFINE(0x7CU, 1U, 1U, 0U),
    kCLOCK_Gpio    = CLK_OP_CODE_DEFINE(0x7CU, 4U, 1U, 0U),
    kCLOCK_Uart0   = CLK_OP_CODE_DEFINE(0x7CU, 5U, 1U, 0U),
    kCLOCK_Uart1   = CLK_OP_CODE_DEFINE(0x7CU, 6U, 1U, 0U),
    kCLOCK_Uart2   = CLK_OP_CODE_DEFINE(0x7CU, 15U, 1U, 0U),
    kCLOCK_I2c0    = CLK_OP_CODE_DEFINE(0x7CU, 7U, 1U, 0U),
    kCLOCK_I2c1    = CLK_OP_CODE_DEFINE(0x7CU, 19U, 1U, 0U),
    kCLOCK_I2c2    = CLK_OP_CODE_DEFINE(0x7CU, 20U, 1U, 0U),
    kCLOCK_Ssp0    = CLK_OP_CODE_DEFINE(0x7CU, 8U, 1U, 0U),
    kCLOCK_Ssp1    = CLK_OP_CODE_DEFINE(0x7CU, 9U, 1U, 0U),
    kCLOCK_Ssp2    = CLK_OP_CODE_DEFINE(0x7CU, 17U, 1U, 0U),
    kCLOCK_Gpt0    = CLK_OP_CODE_DEFINE(0x7CU, 10U, 1U, 0U),
    kCLOCK_Gpt1    = CLK_OP_CODE_DEFINE(0x7CU, 11U, 1U, 0U),
    kCLOCK_Gpt2    = CLK_OP_CODE_DEFINE(0x7CU, 21U, 1U, 0U),
    kCLOCK_Gpt3    = CLK_OP_CODE_DEFINE(0x7CU, 22U, 1U, 0U),
    kCLOCK_Wdt     = CLK_OP_CODE_DEFINE(0x7CU, 23U, 1U, 0U),
    kCLOCK_Sdio    = CLK_OP_CODE_DEFINE(0x7CU, 25U, 1U, 0U),
    kCLOCK_SdioAhb = CLK_OP_CODE_DEFINE(0x7CU, 31U, 1U, 0U),
    kCLOCK_Usbc    = CLK_OP_CODE_DEFINE(0x7CU, 27U, 1U, 0U),
    kCLOCK_UsbcAhb = CLK_OP_CODE_DEFINE(0x7CU, 30U, 1U, 0U),
    kCLOCK_Adc     = CLK_OP_CODE_DEFINE(0x7CU, 26U, 1U, 0U),

    kCLOCK_Gau = CLK_OP_CODE_DEFINE(0x9CU, 2U, 1U, 0U),

    kCLOCK_AcompMclk = CLK_OP_CODE_DEFINE(0xB4U, 0U, 1U, 1U),
    kCLOCK_DacMclk   = CLK_OP_CODE_DEFINE(0xB4U, 1U, 1U, 1U),
    kCLOCK_AdcMclk   = CLK_OP_CODE_DEFINE(0xB4U, 3U, 1U, 1U),
    kCLOCK_BgMclk    = CLK_OP_CODE_DEFINE(0xB4U, 4U, 1U, 1U),

    kCLOCK_Rtc = CLK_OP_CODE_DEFINE(0xF8U, 4U, 1U, 0U),
    kCLOCK_Dma = CLK_OP_CODE_DEFINE(0xF8U, 6U, 1U, 0U),

    kCLOCK_Rc32m = CLK_OP_CODE_DEFINE(0xFCU, 18U, 1U, 0U),
} clock_ip_name_t;

/*!
 * @brief Peripheral clock source selection definition.
 */
typedef enum _clock_attach_id
{
    kSYS_CLK_to_SLOW_UART0 = CLK_OP_CODE_DEFINE(0x88U, 0U, 1U, 0x0U),
    kSYS_CLK_to_FAST_UART0 = CLK_OP_CODE_DEFINE(0x88U, 0U, 1U, 0x1U),

    kSYS_CLK_to_SLOW_UART1 = CLK_OP_CODE_DEFINE(0x88U, 1U, 1U, 0x0U),
    kSYS_CLK_to_FAST_UART1 = CLK_OP_CODE_DEFINE(0x88U, 1U, 1U, 0x1U),

    kSYS_CLK_to_SLOW_UART2 = CLK_OP_CODE_DEFINE(0x88U, 2U, 1U, 0x0U),
    kSYS_CLK_to_FAST_UART2 = CLK_OP_CODE_DEFINE(0x88U, 2U, 1U, 0x1U),

    kSYS_CLK_to_GPT0  = CLK_OP_CODE_DEFINE(0xE4U, 7U, 4U, 0x0U),
    kRC32K_to_GPT0    = CLK_OP_CODE_DEFINE(0xE4U, 7U, 4U, 0x2U),
    kXTAL32K_to_GPT0  = CLK_OP_CODE_DEFINE(0xE4U, 7U, 4U, 0x3U),
    kRC32M_to_GPT0    = CLK_OP_CODE_DEFINE(0xE4U, 7U, 4U, 0x8U),
    kMAINXTAL_to_GPT0 = CLK_OP_CODE_DEFINE(0xE4U, 7U, 4U, 0xCU),

    kSYS_CLK_to_GPT1  = CLK_OP_CODE_DEFINE(0xE8U, 7U, 4U, 0x0U),
    kRC32K_to_GPT1    = CLK_OP_CODE_DEFINE(0xE8U, 7U, 4U, 0x2U),
    kXTAL32K_to_GPT1  = CLK_OP_CODE_DEFINE(0xE8U, 7U, 4U, 0x3U),
    kRC32M_to_GPT1    = CLK_OP_CODE_DEFINE(0xE8U, 7U, 4U, 0x8U),
    kMAINXTAL_to_GPT1 = CLK_OP_CODE_DEFINE(0xE8U, 7U, 4U, 0xCU),

    kSYS_CLK_to_GPT2  = CLK_OP_CODE_DEFINE(0xECU, 7U, 4U, 0x0U),
    kRC32K_to_GPT2    = CLK_OP_CODE_DEFINE(0xECU, 7U, 4U, 0x2U),
    kXTAL32K_to_GPT2  = CLK_OP_CODE_DEFINE(0xECU, 7U, 4U, 0x3U),
    kRC32M_to_GPT2    = CLK_OP_CODE_DEFINE(0xECU, 7U, 4U, 0x8U),
    kMAINXTAL_to_GPT2 = CLK_OP_CODE_DEFINE(0xECU, 7U, 4U, 0xCU),

    kSYS_CLK_to_GPT3  = CLK_OP_CODE_DEFINE(0xF0U, 7U, 4U, 0x0U),
    kRC32K_to_GPT3    = CLK_OP_CODE_DEFINE(0xF0U, 7U, 4U, 0x2U),
    kXTAL32K_to_GPT3  = CLK_OP_CODE_DEFINE(0xF0U, 7U, 4U, 0x3U),
    kRC32M_to_GPT3    = CLK_OP_CODE_DEFINE(0xF0U, 7U, 4U, 0x8U),
    kMAINXTAL_to_GPT3 = CLK_OP_CODE_DEFINE(0xF0U, 7U, 4U, 0xCU),

    kSYS_CLK_to_SSP0 = CLK_OP_CODE_DEFINE(0xDCU, 0U, 1U, 0x0U),
    kAUPLL_to_SSP0   = CLK_OP_CODE_DEFINE(0xDCU, 0U, 1U, 0x1U),

    kSYS_CLK_to_SSP1 = CLK_OP_CODE_DEFINE(0xDCU, 1U, 1U, 0x0U),
    kAUPLL_to_SSP1   = CLK_OP_CODE_DEFINE(0xDCU, 1U, 1U, 0x1U),

    kSYS_CLK_to_SSP2 = CLK_OP_CODE_DEFINE(0xDCU, 2U, 1U, 0x0U),
    kAUPLL_to_SSP2   = CLK_OP_CODE_DEFINE(0xDCU, 2U, 1U, 0x1U),

    kSYS_CLK_to_GAU  = CLK_OP_CODE_DEFINE(0x9CU, 0U, 2U, 0x0U),
    kRC32M_to_GAU    = CLK_OP_CODE_DEFINE(0x9CU, 0U, 2U, 0x1U),
    kMAINXTAL_to_GAU = CLK_OP_CODE_DEFINE(0x9CU, 0U, 2U, 0x2U),
    kAUPLL_to_GAU    = CLK_OP_CODE_DEFINE(0x9CU, 0U, 2U, 0x3U),

    kRC32K_to_RTC   = CLK_OP_CODE_DEFINE(0xF8U, 5U, 1U, 0x0U),
    kXTAL32K_to_RTC = CLK_OP_CODE_DEFINE(0xF8U, 5U, 1U, 0x1U),
} clock_attach_id_t;

/*!
 * @brief Clock divider definition.
 */
typedef enum _clock_div_name
{
    kCLOCK_DivUartFast = CLK_OP_CODE_DEFINE(0x80U, 0U, 0U, 1U),
    kCLOCK_DivUartSlow = CLK_OP_CODE_DEFINE(0x84U, 0U, 0U, 1U),
    kCLOCK_DivCore     = CLK_OP_CODE_DEFINE(0x8CU, 0U, 6U, 0U),

    kCLOCK_DivSsp0 = CLK_OP_CODE_DEFINE(0x90U, 0U, 5U, 0U),
    kCLOCK_DivSsp1 = CLK_OP_CODE_DEFINE(0x90U, 5U, 5U, 0U),
    kCLOCK_DivSsp2 = CLK_OP_CODE_DEFINE(0x90U, 10U, 5U, 0U),
    kCLOCK_DivSdio = CLK_OP_CODE_DEFINE(0x90U, 16U, 4U, 0U),

    kCLOCK_DivFlashC = CLK_OP_CODE_DEFINE(0x94U, 4U, 3U, 0U),
    kCLOCK_DivQspi   = CLK_OP_CODE_DEFINE(0x94U, 8U, 3U, 0U),

    kCLOCK_DivI2c     = CLK_OP_CODE_DEFINE(0x98U, 20U, 2U, 0U),
    kCLOCK_DivGptSamp = CLK_OP_CODE_DEFINE(0x98U, 0U, 3U, 0U),
    kCLOCK_DivWdt     = CLK_OP_CODE_DEFINE(0x98U, 24U, 2U, 1U),

    kCLOCK_DivGpt0 = CLK_OP_CODE_DEFINE(0xE4U, 0U, 6U, 0U),
    kCLOCK_DivGpt1 = CLK_OP_CODE_DEFINE(0xE8U, 0U, 6U, 0U),
    kCLOCK_DivGpt2 = CLK_OP_CODE_DEFINE(0xECU, 0U, 6U, 0U),
    kCLOCK_DivGpt3 = CLK_OP_CODE_DEFINE(0x98U, 8U, 3U, 1U),

    kCLOCK_DivApb0 = CLK_OP_CODE_DEFINE(0xF8U, 7U, 2U, 0U),
    kCLOCK_DivApb1 = CLK_OP_CODE_DEFINE(0xF8U, 9U, 2U, 0U),
    kCLOCK_DivPmu  = CLK_OP_CODE_DEFINE(0xF8U, 0U, 4U, 0U),

    kCLOCK_DivGau      = CLK_OP_CODE_DEFINE(0xFCU, 8U, 5U, 0U),
    kCLOCK_DivRc32mRef = CLK_OP_CODE_DEFINE(0xFCU, 13U, 5U, 0U),
} clock_div_name_t;

/*!
 * @brief CLOCK system clock source.
 */
typedef enum _clock_sys_clk_src
{
    kCLOCK_SysClkSrcSFll     = 0U, /*!< SFLL. */
    kCLOCK_SysClkSrcRC32M_1  = 1U, /*!< RC 32MHz clock.   */
    kCLOCK_SysClkSrcMainXtal = 2U, /*!< XTAL 38.4MHz clock.   */
    kCLOCK_SysClkSrcRC32M_3  = 3U, /*!< RC 32MHz clock. */
} clock_sys_clk_src_t;

/*!
 * @brief System reference clock option
 */
typedef enum _clock_refclk
{
    kCLOCK_RefClk_SYS = (1U << PMU_WLAN_CTRL_REFCLK_SYS_REQ_SHIFT), /*!< REFCLK_SYS */
    kCLOCK_RefClk_AUD = (1U << PMU_WLAN_CTRL_REFCLK_AUD_REQ_SHIFT), /*!< REFCLK_AUD */
    kCLOCK_RefClk_USB = (1U << PMU_WLAN_CTRL_REFCLK_USB_REQ_SHIFT), /*!< REFCLK_USB */
    kCLOCK_RefClk_ALL = (kCLOCK_RefClk_SYS | kCLOCK_RefClk_AUD | kCLOCK_RefClk_USB),
} clock_refclk_t;

/*!
 * @brief XTAL32K mode option
 */
typedef enum _clock_osc32k_mode
{
    kCLOCK_Osc32k_Internal = 0, /*!< Internal oscillator mode */
    kCLOCK_Osc32k_External = 1, /*!< External oscillator mode */
} clock_osc32k_mode_t;

/*! @brief SFLL Reference Input Clock Source */
typedef enum _clock_sfll_src
{
    kCLOCK_SFllSrcRC32M    = 0U, /*!< RC32M clock */
    kCLOCK_SFllSrcMainXtal = 1U, /*!< Main XTAL clock */
} clock_sfll_src_t;

/*! @brief SFLL configuration. */
typedef struct _clock_sfll_config
{
    clock_sfll_src_t sfllSrc; /*!< Reference Input Clock Source */
    uint16_t refDiv;          /*!< Divider for reference clock, 9-bit */
                              /*!< 0.2 MHz <= Fref (frequency of Reference clock / refDiv) <= 0.4 MHz */
    uint16_t fbDiv;           /*!< Divider for feedback clock, 9-bit */
    uint8_t kvco;             /*!< VCO setting, 2-bit */
                              /*!< Select KVCO per VCO target frequency */
    uint8_t postDiv;          /*!< Post divider, 2-bit */
                              /*!< 2'b01, Fout = Fvco/2 */
                              /*!< 2'b10, Fout = Fvco/4 */
                              /*!< 2'b11, Fout = Fvco/8 */
} clock_sfll_config_t;

/*! @brief AUPLL Reference Input Clock Source */
typedef enum _clock_aupll_src
{
    kCLOCK_AuPllSrcRC32M  = 0U, /*!< RC32M clock */
    kCLOCK_AuPllSrcRefClk = 1U, /*!< REFCLK_AUD clock */
} clock_aupll_src_t;

/*! @brief AUPLL configuration. */
typedef struct _clock_aupll_config
{
    clock_aupll_src_t aupllSrc; /*!< Reference Input Clock Source */
    uint8_t refDiv;             /*!< Divider for reference clock, 4-bit, range: 3 ~ 10 */
    uint8_t fbDiv;              /*!< Divider for feedback clock, 6-bit, range: 27, 29, 31 ~ 36, 38, 39, 42, 46 */
    uint8_t icp;                /*!< charge pump current control, 2-bit, reserved now */
                                /*!< 2'b00, 2.5 uA */
                                /*!< 2'b01, 5 uA */
                                /*!< 2'b10, 7.5 uA */
                                /*!< 2'b11, 10 uA */
    uint8_t postDivPattern;     /*!< Pattern divider for ouput clock, 2-bit, range: 0 ~ 2 */
                                /*!< 2'b00, pattern = 4.0 */
                                /*!< 2'b01, pattern = 3.0 */
                                /*!< 2'b10, pattern = 4.5 */
    uint8_t postDivModulo;      /*!< Modulo divider for ouput clock, 3-bit, range: 0 ~ 2, 4, 7 */
                                /*!< 3'b000, modulo = 1 */
                                /*!< 3'b001, modulo = 2 */
                                /*!< 3'b010, modulo = 4 */
                                /*!< 3'b100, modulo = 8 */
                                /*!< 3'b111, modulo = 16 */
    bool enaDither;             /* ENA_DITHER status */
                                /*!< 1'b0, Integration only */
                                /*!< 1'b1, Integration + MASH */
    bool enaVcoClkX2;           /* Enable or disable VCOCLK_X2 */
                                /*!< 1'b0, Disable VCOCLK_X2 */
                                /*!< 1'b1, Enable VCOCLK_X2 */
    uint32_t fract;             /*!< Fractional PLL feedback divider 20-bits, range: 0 ~ 0x10000 */
} clock_aupll_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 * @brief Enable the clock for specific IP.
 *
 * @param name  Which clock to enable, see \ref clock_ip_name_t.
 */
void CLOCK_EnableClock(clock_ip_name_t name);

/*!
 * @brief Disable the clock for specific IP.
 *
 * @param name  Which clock to disable, see \ref clock_ip_name_t.
 */
void CLOCK_DisableClock(clock_ip_name_t name);

/**
 * @brief   Configure the clock selection muxes.
 * @param   connection  : Clock to be configured.
 */
void CLOCK_AttachClk(clock_attach_id_t connection);

/**
 * @brief   Setup clock dividers.
 * @param   name        : Clock divider name
 * @param   divider     : Value to be divided.
 */
void CLOCK_SetClkDiv(clock_div_name_t name, uint32_t divider);

/*!
 * @brief   Setup UART clock dividers.
 *
 * @param   name        : Clock divider name
 * @param   nom         : Nominator for fraction divider.
 * @param   denom       : Denominator for fraction divider.
 */
void CLOCK_SetUartClkDiv(clock_div_name_t name, uint32_t nom, uint32_t denom);

/*!
 * @brief Get the system clock frequency.
 *
 * @return Clock frequency in Hz.
 */
uint32_t CLOCK_GetSysClkFreq(void);

/*!
 * @brief Set the system clock source.
 *
 * This function sets the system clock source.
 *
 * @param src System clock source to set.
 */
void CLOCK_SetSysClkSource(clock_sys_clk_src_t src);

/*!
 * @brief Enable the 32K XTAL.
 *
 * @param mode   Internal or external OSC selection.
 */
void CLOCK_EnableXtal32K(clock_osc32k_mode_t mode);

/*!
 * @brief Disable the 32K XTAL output.
 */
void CLOCK_DisableXtal32K(void);

/*!
 * @brief Enable the RC32M OSC.
 *
 * @param halfDiv half-divided reference clock. True for 16MHz and false for original 32MHz output.
 */
void CLOCK_EnableRC32M(bool halfDiv);

/*!
 * @brief Disable the RC32M OSC.
 */
void CLOCK_DisableRC32M(void);

/*!
 * @brief RC32M clock calibration function
 *
 * @param autoCal Calibration option, true for auto and false for manual
 * @param manCalCode Select manual way, need input the calibration code
 * @return return the calibration result
 *         -1, calibration failure
 *         else, internal calibration code
 *
 * @note  Clock CLK_AUPLL_CAU must be ready before RC32M calibration
 */
int32_t CLOCK_CalibrateRC32M(bool autoCal, uint32_t manCalCode);

/*!
 * @brief Enable the RC32K OSC.
 */
void CLOCK_EnableRC32K(void);

/*!
 * @brief Disable the RC32K OSC.
 */
void CLOCK_DisableRC32K(void);

/*!
 * @brief RC32K clock calibration function
 *
 * @param autoCal Calibration option, true for auto and false for manual
 * @param manCalCode Select manual way, need input the calibration code
 * @return return the calibration result
 *         -1, calibration failure
 *         else, internal calibration code
 *
 * @note  Clock XTAL32K must be ready before RC32K calibration
 */
int32_t CLOCK_CalibrateRC32K(bool autoCal, uint32_t manCalCode);

/*!
 * @brief Enable the REFCLK for SYS/AUD/USB reference.
 *
 * @param refclks Or'ed value to select SYS/AUD/USB REFCLK to disable. See \ref clock_refclk_t.
 */
void CLOCK_EnableRefClk(uint32_t refclks);

/*!
 * @brief Disable the REFCLK for SYS/AUD/USB reference.
 *
 * @param refclks Or'ed value to select SYS/AUD/USB REFCLK to disable. See \ref clock_refclk_t.
 */
void CLOCK_DisableRefClk(uint32_t refclks);

/*! @brief  Initialize the SFLL.
 *  @param  config : Configuration to set to SFLL.
 */
void CLOCK_InitSFll(const clock_sfll_config_t *config);

/*! @brief  Deinit the SFLL.
 */
void CLOCK_DeinitSFll(void);

/*! @brief  Initialize the AUPLL.
 *  @param  config : Configuration to set to AUPLL.
 */
void CLOCK_InitAuPll(const clock_aupll_config_t *config);

/*! @brief  Deinit the AUPLL.
 */
void CLOCK_DeinitAuPll(void);

/*! @brief  Update the AUPLL post divider
 */
void CLOCK_UpdateAupllPostDiv(uint8_t postDivPattern, uint8_t postDivModulo);

/*!
 * @brief Set the MAIN XTAL frequency based on board setting.
 *
 * @param freq : The XTAL input clock frequency in Hz.
 */
__STATIC_INLINE void CLOCK_SetMainXtalFreq(uint32_t freq)
{
    g_mainXtalFreq = freq;
}

/*! @brief  Return Frequency of MAIN XTAL Clock
 *  @return Frequency of MAIN XTAL Clock.
 */
uint32_t CLOCK_GetMainXtalFreq(void);

/*! @brief  Return Frequency of REFCLK_AUD Clock
 *  @return Frequency of REFCLK_AUD  Clock.
 */
uint32_t CLOCK_GetRefClkAudFreq(void);

/*! @brief  Return Frequency of RC32M Clock
 *  @return Frequency of RC32M Clock.
 */
uint32_t CLOCK_GetRC32MFreq(void);

/*! @brief  Return Frequency of XTAL32K Clock
 *  @return Frequency of XTAL32K Clock.
 */
uint32_t CLOCK_GetXtal32KFreq(void);

/*! @brief  Return Frequency of RC32K Clock
 *  @return Frequency of RC32K Clock.
 */
uint32_t CLOCK_GetRC32KFreq(void);

/*! @brief  Return Frequency of SFLL Clock
 *  @return Frequency of SFLL Clock.
 */
uint32_t CLOCK_GetSFllFreq(void);

/*! @brief  Return Frequency of AUPLL Clock
 *  @return Frequency of AUPLL Clock.
 */
uint32_t CLOCK_GetAuPllFreq(void);

/*! @brief  Return Frequency of M4/AHB Clock
 *  @return Frequency of M4/AHB Clock.
 */
uint32_t CLOCK_GetCoreBusFreq(void);

/*! @brief  Return Frequency of APB Clock
 *
 *  @param instance  APB instance 0-1.
 *  @return Frequency of APB Clock.
 */
uint32_t CLOCK_GetApbFreq(uint32_t instance);

/*! @brief  Return Frequency of UART Clock
 *
 *  @param instance  UART instance 0-2.
 *  @return Frequency of UART Clock.
 */
uint32_t CLOCK_GetUartClkFreq(uint32_t instance);

/*! @brief  Return Frequency of GPT Clock
 *
 *  @param instance  GPT instance 0-3.
 *  @return Frequency of GPT Clock.
 */
uint32_t CLOCK_GetGptClkFreq(uint32_t instance);

/*! @brief  Return Frequency of GPT Sample Clock
 *
 *  @return Frequency of GPT Sample Clock.
 */
uint32_t CLOCK_GetGptSampleClkFreq(void);

/*! @brief  Return Frequency of SSP Clock
 *
 *  @param instance  SSP instance 0-2.
 *  @return Frequency of SSP Clock.
 */
uint32_t CLOCK_GetSspClkFreq(uint32_t instance);

/*! @brief  Return Frequency of GAU Clock
 *  @return Frequency of GAU Clock.
 */
uint32_t CLOCK_GetGauClkFreq(void);

/*! @brief  Return Frequency of RTC Clock
 *  @return Frequency of RTC Clock.
 */
uint32_t CLOCK_GetRtcClkFreq(void);

/*! @brief  Return Frequency of I2C Clock
 *  @return Frequency of I2C Clock.
 */
uint32_t CLOCK_GetI2cClkFreq(void);

/*! @brief  Return Frequency of QSPI Clock
 *  @return Frequency of QSPI Clock.
 */
uint32_t CLOCK_GetQspiClkFreq(void);

/*! @brief  Return Frequency of FlashC Clock
 *  @return Frequency of FlashC Clock.
 */
uint32_t CLOCK_GetFlashCClkFreq(void);

/*! @brief  Return Frequency of WDT Clock
 *  @return Frequency of WDT Clock.
 */
uint32_t CLOCK_GetWdtClkFreq(void);

/*! @brief  Return Frequency of SDIO Clock
 *  @return Frequency of SDIO Clock.
 */
uint32_t CLOCK_GetSdioClkFreq(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

/*! @} */

#endif /* _FSL_CLOCK_H_ */
