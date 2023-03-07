/*
 * Copyright 2020, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_clock.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.clock"
#endif

#define FRACT_BASE_NUM (4194304U)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* External crystal MAINXTAL clock frequency. */
volatile uint32_t g_mainXtalFreq;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Enable the clock for specific IP.
 *
 * @param name  Which clock to enable, see \ref clock_ip_name_t.
 */
void CLOCK_EnableClock(clock_ip_name_t name)
{
    uint32_t pos   = CLK_OP_POS(name);
    uint32_t mask  = CLK_OP_MASK(name);
    uint32_t value = CLK_OP_VALUE(name);

    CLK_OP_REG(name) = (CLK_OP_REG(name) & ~(mask << pos)) | (value << pos);
}

/*!
 * @brief Disable the clock for specific IP.
 *
 * @param name  Which clock to disable, see \ref clock_ip_name_t.
 */
void CLOCK_DisableClock(clock_ip_name_t name)
{
    uint32_t pos   = CLK_OP_POS(name);
    uint32_t mask  = CLK_OP_MASK(name);
    uint32_t value = (~CLK_OP_VALUE(name)) & mask;

    CLK_OP_REG(name) = (CLK_OP_REG(name) & ~(mask << pos)) | (value << pos);
}

/**
 * @brief   Configure the clock selection muxes.
 * @param   connection  : Clock to be configured.
 */
void CLOCK_AttachClk(clock_attach_id_t connection)
{
    uint32_t pos   = CLK_OP_POS(connection);
    uint32_t mask  = CLK_OP_MASK(connection);
    uint32_t value = CLK_OP_VALUE(connection);

    CLK_OP_REG(connection) = (CLK_OP_REG(connection) & ~(mask << pos)) | (value << pos);
}

/**
 * @brief   Setup clock dividers.
 * @param   name        : Clock divider name
 * @param   divider     : Value to be divided.
 */
void CLOCK_SetClkDiv(clock_div_name_t name, uint32_t divider)
{
    uint32_t pos   = CLK_OP_POS(name);
    uint32_t mask  = CLK_OP_MASK(name);
    uint32_t value = CLK_OP_VALUE(name);

    if (value == 0U)
    {
        assert(divider <= mask);
        CLK_OP_REG(name) = (CLK_OP_REG(name) & ~(mask << pos)) | (divider << pos);
    }
    else
    {
        /* Special case */
        switch (name)
        {
            case kCLOCK_DivUartFast:
            case kCLOCK_DivUartSlow:
                assert(divider <= 0xFFFFFFU);
                CLK_OP_REG(name) = divider;
                break;
            case kCLOCK_DivWdt:
                assert(divider <= 0x3FU);
                CLK_OP_REG(name) &= ~(PMU_PERI2_CLK_DIV_WDT_CLK_DIV_5_3_MASK | PMU_PERI2_CLK_DIV_WDT_CLK_DIV_2_2_MASK |
                                      PMU_PERI2_CLK_DIV_WDT_CLK_DIV_1_0_MASK);
                CLK_OP_REG(name) |= ((divider >> 3U) << PMU_PERI2_CLK_DIV_WDT_CLK_DIV_5_3_SHIFT) |
                                    (((divider >> 2U) & 1U) << PMU_PERI2_CLK_DIV_WDT_CLK_DIV_2_2_SHIFT) |
                                    ((divider & 3U) << PMU_PERI2_CLK_DIV_WDT_CLK_DIV_1_0_SHIFT);
                break;
            case kCLOCK_DivGpt3:
                assert(divider <= 0x3FU);
                CLK_OP_REG(name) &=
                    ~(PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_5_3_MASK | PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_2_0_MASK);
                CLK_OP_REG(name) |= ((divider >> 3U) << PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_5_3_SHIFT) |
                                    ((divider & 7U) << PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_2_0_SHIFT);
                break;
            default:
                assert(false);
                break;
        }
    }
}

/*!
 * @brief   Setup UART clock dividers.
 *
 * @param   name        : Clock divider name
 * @param   nom         : Nominator for fraction divider.
 * @param   denom       : Denominator for fraction divider.
 */
void CLOCK_SetUartClkDiv(clock_div_name_t name, uint32_t nom, uint32_t denom)
{
    assert(nom < (1UL << 13U));
    assert(nom < (1UL << 11U));

    CLOCK_SetClkDiv(name, (nom << PMU_UART_FAST_CLK_DIV_NOMINATOR_SHIFT) | denom);
}

/*!
 * @brief Get the system clock frequency.
 *
 * @return Clock frequency in Hz.
 */
uint32_t CLOCK_GetSysClkFreq(void)
{
    uint32_t src;
    uint32_t freq;

    src = PMU->CLK_SRC & PMU_CLK_SRC_SYS_CLK_SEL_MASK;

    switch (src)
    {
        case (uint32_t)kCLOCK_SysClkSrcSFll:
            freq = CLOCK_GetSFllFreq();
            break;
        case (uint32_t)kCLOCK_SysClkSrcRC32M_1:
        case (uint32_t)kCLOCK_SysClkSrcRC32M_3:
            freq = CLOCK_GetRC32MFreq();
            break;
        case (uint32_t)kCLOCK_SysClkSrcMainXtal:
            freq = CLOCK_GetMainXtalFreq();
            break;
        default:
            assert(false);
            freq = 0U;
            break;
    }

    return freq;
}

/*!
 * @brief Set the system clock source.
 *
 * This function sets the system clock source.
 *
 * @param src System clock source to set.
 */
void CLOCK_SetSysClkSource(clock_sys_clk_src_t src)
{
    uint32_t currSrc;
    bool rc32mRdy;

    currSrc  = PMU->CLK_SRC & PMU_CLK_SRC_SYS_CLK_SEL_MASK;
    rc32mRdy = (PMU->CLK_RDY & PMU_CLK_RDY_RC32M_RDY_MASK) != 0U;

    switch (src)
    {
        case kCLOCK_SysClkSrcSFll:
            if ((currSrc == (uint32_t)kCLOCK_SysClkSrcMainXtal) && rc32mRdy)
            {
                PMU->CLK_SRC =
                    (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcRC32M_3);
                currSrc = (uint32_t)kCLOCK_SysClkSrcRC32M_3;
            }
            if (currSrc == (uint32_t)kCLOCK_SysClkSrcRC32M_3)
            {
                PMU->CLK_SRC =
                    (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcRC32M_1);
            }
            PMU->CLK_SRC =
                (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcSFll);
            break;
        case kCLOCK_SysClkSrcRC32M_1:
        case kCLOCK_SysClkSrcRC32M_3:
            if ((currSrc == (uint32_t)kCLOCK_SysClkSrcSFll) || (currSrc == (uint32_t)kCLOCK_SysClkSrcMainXtal))
            {
                PMU->CLK_SRC = (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(currSrc + 1U);
            }
            break;
        case kCLOCK_SysClkSrcMainXtal:
            if ((currSrc == (uint32_t)kCLOCK_SysClkSrcSFll) && rc32mRdy)
            {
                PMU->CLK_SRC =
                    (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcRC32M_1);
                currSrc = (uint32_t)kCLOCK_SysClkSrcRC32M_1;
            }
            if (currSrc == (uint32_t)kCLOCK_SysClkSrcRC32M_1)
            {
                PMU->CLK_SRC =
                    (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcRC32M_3);
            }
            PMU->CLK_SRC =
                (PMU->CLK_SRC & ~PMU_CLK_SRC_SYS_CLK_SEL_MASK) | PMU_CLK_SRC_SYS_CLK_SEL(kCLOCK_SysClkSrcMainXtal);
            break;
        default:
            assert(false);
            break;
    }
}

/*!
 * @brief Enable the 32K XTAL.
 *
 * @param mode   Internal or external OSC selection.
 */
void CLOCK_EnableXtal32K(clock_osc32k_mode_t mode)
{
    PMU->XTAL32K_CTRL =
        (PMU->XTAL32K_CTRL & ~PMU_XTAL32K_CTRL_X32K_EXT_OSC_EN_MASK) | PMU_XTAL32K_CTRL_X32K_EXT_OSC_EN(mode);
    PMU->XTAL32K_CTRL |= PMU_XTAL32K_CTRL_X32K_EN_MASK;
    while ((PMU->CLK_RDY & PMU_CLK_RDY_X32K_RDY_MASK) == 0U)
    {
    }
}

/*!
 * @brief Disable the 32K XTAL output.
 */
void CLOCK_DisableXtal32K(void)
{
    PMU->XTAL32K_CTRL &= ~PMU_XTAL32K_CTRL_X32K_EN_MASK;
}

/*!
 * @brief Enable the RC32M OSC.
 *
 * @param halfDiv half-divided reference clock. True for 16MHz and false for original 32MHz output.
 */
void CLOCK_EnableRC32M(bool halfDiv)
{
    RC32->CLK = (RC32->CLK & ~RC32_CLK_REF_SEL_MASK) | RC32_CLK_REF_SEL(halfDiv ? 0U : 1U);
    RC32->CTRL &= ~RC32_CTRL_PD_MASK;
    RC32->CTRL |= RC32_CTRL_EN_MASK;
    while ((PMU->CLK_RDY & PMU_CLK_RDY_RC32M_RDY_MASK) == 0U)
    {
    }
}

/*!
 * @brief Disable the RC32M OSC.
 */
void CLOCK_DisableRC32M(void)
{
    RC32->CTRL &= ~RC32_CTRL_EN_MASK;
    RC32->CTRL |= RC32_CTRL_PD_MASK;
}

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
int32_t CLOCK_CalibrateRC32M(bool autoCal, uint32_t manCalCode)
{
    volatile uint32_t localCnt;

    /* Soft reset RC32M */
    RC32->RST |= RC32_RST_SOFT_RST_MASK;

    for (localCnt = 0U; localCnt < 10U; localCnt++)
    {
        __NOP();
    }

    RC32->RST &= ~RC32_RST_SOFT_RST_MASK;

    /* Power up RC32M */
    RC32->CTRL &= ~RC32_CTRL_PD_MASK;
    RC32->CTRL |= RC32_CTRL_EN_MASK;

    localCnt = 0U;
    while (true)
    {
        /* Check RC32M_RDY status */
        if ((PMU->CLK_RDY & PMU_CLK_RDY_RC32M_RDY_MASK) != 0U)
        {
            break;
        }
        else if (localCnt > 5000000U)
        {
            return -1;
        }

        localCnt++;
    }

    if (autoCal)
    {
        /* Auto */
        RC32->CTRL &= ~RC32_CTRL_EXT_CODE_EN_MASK;
        PMU->RC32M_CTRL |= PMU_RC32M_CTRL_CAL_ALLOW_MASK;
        RC32->CTRL |= RC32_CTRL_CAL_EN_MASK;
    }
    else
    {
        /* Manual */
        RC32->CTRL = (RC32->CTRL & ~RC32_CTRL_CODE_FR_EXT_MASK) | RC32_CTRL_CODE_FR_EXT(manCalCode);
        RC32->CTRL |= RC32_CTRL_EXT_CODE_EN_MASK;
    }

    localCnt = 0;
    while (true)
    {
        /* Check CAL_DONE status */
        if (((RC32->STATUS & RC32_STATUS_CAL_DONE_MASK) != 0U) || ((RC32->CTRL & RC32_CTRL_EXT_CODE_EN_MASK) != 0U))
        {
            break;
        }
        else if (localCnt > 5000000U)
        {
            return -1;
        }

        localCnt++;
    }

    RC32->CTRL &= ~RC32_CTRL_CAL_EN_MASK;
    PMU->RC32M_CTRL &= ~PMU_RC32M_CTRL_CAL_ALLOW_MASK;

    return (int32_t)(uint32_t)((RC32->STATUS & RC32_STATUS_CODE_FR_CAL_MASK) >> RC32_STATUS_CODE_FR_CAL_SHIFT);
}

/*!
 * @brief Enable the RC32K OSC.
 */
void CLOCK_EnableRC32K(void)
{
    PMU->RC32K_CTRL0 &= ~PMU_RC32K_CTRL0_RC32K_PD_MASK;
    while ((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_RDY_MASK) == 0U)
    {
    }
}

/*!
 * @brief Disable the RC32K OSC.
 */
void CLOCK_DisableRC32K(void)
{
    PMU->RC32K_CTRL0 |= PMU_RC32K_CTRL0_RC32K_PD_MASK;
}

/*!
 * @brief RC32K clock calibration function
 *
 * @param autoCal Calibration option, true for auto and false for manual
 * @param manCalCode Select manual way, need input the calibration code
 * @return return the calibration result
 *         -1, calibration failure
 *         else, internal calibration code
 *
 * @note  Clock CLK_XTAL32K must be ready before RC32K calibration
 */
int32_t CLOCK_CalibrateRC32K(bool autoCal, uint32_t manCalCode)
{
    volatile uint32_t localCnt;

    /* Power up RC32K */
    PMU->RC32K_CTRL0 &= ~PMU_RC32K_CTRL0_RC32K_PD_MASK;

    localCnt = 0;
    while (true)
    {
        /* Check RC32K_RDY and X32K_RDY status */
        if (((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_RDY_MASK) != 0U) &&
            ((PMU->CLK_RDY & PMU_CLK_RDY_X32K_RDY_MASK) != 0U))
        {
            break;
        }
        else if (localCnt > 5000000U)
        {
            return -1;
        }

        localCnt++;
    }

    if (autoCal)
    {
        PMU->RC32K_CTRL1 &= ~PMU_RC32K_CTRL1_RC32K_EXT_CODE_EN_MASK;
        PMU->RC32K_CTRL1 |= PMU_RC32K_CTRL1_RC32K_ALLOW_CAL_MASK;
    }
    else
    {
        PMU->RC32K_CTRL0 = (PMU->RC32K_CTRL0 & ~PMU_RC32K_CTRL0_RC32K_CODE_FR_EXT_MASK) |
                           PMU_RC32K_CTRL0_RC32K_CODE_FR_EXT(manCalCode);
        PMU->RC32K_CTRL1 |= PMU_RC32K_CTRL1_RC32K_EXT_CODE_EN_MASK;
    }

    /* Start Calibration */
    PMU->RC32K_CTRL0 |= PMU_RC32K_CTRL0_RC32K_CAL_EN_MASK;

    /* Add a short delay to prevent auto cal fail */
    localCnt = 0;
    while (true)
    {
        if (((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_CAL_DONE_MASK) == 0U) ||
            ((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_EXT_CODE_EN_MASK) != 0U))
        {
            break;
        }
        else if (localCnt > 50000U)
        {
            break;
        }
        localCnt++;
    }

    localCnt = 0U;
    while (true)
    {
        /* Check CAL_DONE status */
        if (((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_CAL_DONE_MASK) != 0U) ||
            ((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_EXT_CODE_EN_MASK) != 0U))
        {
            break;
        }
        else if (localCnt > 5000000U)
        {
            return -1;
        }
        localCnt++;
    }

    localCnt = 0;
    while (localCnt <= 0x1000U)
    {
        localCnt++;
    }

    PMU->RC32K_CTRL0 &= ~PMU_RC32K_CTRL0_RC32K_CAL_EN_MASK;
    PMU->RC32K_CTRL1 &= ~PMU_RC32K_CTRL1_RC32K_ALLOW_CAL_MASK;

    return (int32_t)(uint32_t)((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_CODE_FR_CAL_MASK) >>
                               PMU_RC32K_CTRL1_RC32K_CODE_FR_CAL_SHIFT);
}

/*!
 * @brief Enable the REFCLK for SYS/AUD/USB reference.
 *
 * @param refclks Or'ed value to select SYS/AUD/USB REFCLK to disable. See \ref clock_refclk_t.
 */
void CLOCK_EnableRefClk(uint32_t refclks)
{
    assert((refclks & ~((uint32_t)kCLOCK_RefClk_ALL)) == 0U);

    /* Ensure WLAN is powered on */
    PMU->WLAN_CTRL |= PMU_WLAN_CTRL_PD_MASK;
    /* Enable reference clock request */
    PMU->WLAN_CTRL |= refclks;
    /* Wait reference clock ready */
    while ((PMU->WLAN_CTRL & (refclks << 3U)) != (refclks << 3U))
    {
    }
}

/*!
 * @brief Disable the REFCLK for SYS/AUD/USB reference.
 *
 * @param refclks Or'ed value to select SYS/AUD/USB REFCLK to disable. See \ref clock_refclk_t.
 */
void CLOCK_DisableRefClk(uint32_t refclks)
{
    assert((refclks & ~((uint32_t)kCLOCK_RefClk_ALL)) == 0U);

    /* Disable reference clock request */
    PMU->WLAN_CTRL &= ~refclks;
}

/*! @brief  Initialize the SFLL.
 *  @param  config : Configuration to set to SFLL.
 */
void CLOCK_InitSFll(const clock_sfll_config_t *config)
{
    assert(config);

    /* Accepted sfll reference divider is 2~511 */
    assert((config->refDiv >= 2U) && (config->refDiv <= 511U));

    /* Accepted sfll feedback divider is 1~511 */
    assert((config->fbDiv >= 1U) && (config->fbDiv <= 511U));

    /* Accepted sfll kvco is 0~3 */
    assert(config->kvco <= 3U);

    /* Accepted sfll postdiv is 0~3 */
    assert(config->postDiv <= 3U);

    PMU->SFLL_CTRL0 = (PMU->SFLL_CTRL0 & ~(PMU_SFLL_CTRL0_SFLL_FBDIV_MASK | PMU_SFLL_CTRL0_SFLL_KVCO_MASK |
                                           PMU_SFLL_CTRL0_SFLL_REFCLK_SEL_MASK)) |
                      PMU_SFLL_CTRL0_SFLL_FBDIV(config->fbDiv) | PMU_SFLL_CTRL0_SFLL_KVCO(config->kvco) |
                      PMU_SFLL_CTRL0_SFLL_REFCLK_SEL(config->sfllSrc);
    PMU->SFLL_CTRL1 = (PMU->SFLL_CTRL1 & ~(PMU_SFLL_CTRL1_SFLL_REFDIV_MASK | PMU_SFLL_CTRL1_SFLL_DIV_SEL_MASK)) |
                      PMU_SFLL_CTRL1_SFLL_REFDIV(config->refDiv) | PMU_SFLL_CTRL1_SFLL_DIV_SEL(config->postDiv);

    /* Power up */
    PMU->SFLL_CTRL0 |= PMU_SFLL_CTRL0_SFLL_PU_MASK;

    /* Wait PLL ready */
    while ((PMU->CLK_RDY & PMU_CLK_RDY_PLL_CLK_RDY_MASK) == 0U)
    {
    }
}

/*! @brief  Deinit the SFLL.
 */
void CLOCK_DeinitSFll(void)
{
    /* Power down */
    PMU->SFLL_CTRL0 &= ~PMU_SFLL_CTRL0_SFLL_PU_MASK;
}

/*! @brief  Initialize the AUPLL.
 *  @param  config : Configuration to set to AUPLL.
 */
void CLOCK_InitAuPll(const clock_aupll_config_t *config)
{
    assert(config);
    /* check the parameters */

    /* Accepted aupll reference divider is 3~10 */
    assert((config->refDiv >= 3U) && (config->refDiv <= 10U));

    /* Accepted aupll feedback divider is 27, 29, 31 ~ 36, 38, 39, 42, 46 */
    assert(((config->fbDiv >= 27U) && (config->fbDiv <= 39U) && (config->fbDiv != 28U) && (config->fbDiv != 30U) &&
            (config->fbDiv != 37U)) ||
           (config->fbDiv == 42U) || (config->fbDiv == 46U));

    /* Accepted aupll icp is 0~3 */
    assert(config->icp <= 3U);

    /* Accepted AUPLL_POSTDIV_PATTERN is 0~2 */
    assert(config->postDivPattern <= 2U);

    /* Accepted AUPLL_POSTDIV_MODULO is 0 ~ 2, 4, 7 */
    assert(((config->postDivModulo <= 4U) && (config->postDivModulo != 3U)) || (config->postDivModulo == 7U));

    PMU->AUPLL_CTRL1 = (PMU->AUPLL_CTRL1 & ~(PMU_AUPLL_CTRL1_DIV_MCLK_MASK | PMU_AUPLL_CTRL1_DIV_FBCCLK_MASK |
                                             PMU_AUPLL_CTRL1_ICP_MASK | PMU_AUPLL_CTRL1_DIV_OCLK_PATTERN_MASK |
                                             PMU_AUPLL_CTRL1_DIV_OCLK_MODULO_MASK | PMU_AUPLL_CTRL1_ENA_DITHER_MASK |
                                             PMU_AUPLL_CTRL1_EN_VCOX2_MASK | PMU_AUPLL_CTRL1_REFCLK_SEL_MASK)) |
                       PMU_AUPLL_CTRL1_DIV_MCLK(config->refDiv) | PMU_AUPLL_CTRL1_DIV_FBCCLK(config->fbDiv) |
                       PMU_AUPLL_CTRL1_ICP(config->icp) | PMU_AUPLL_CTRL1_DIV_OCLK_PATTERN(config->postDivPattern) |
                       PMU_AUPLL_CTRL1_DIV_OCLK_MODULO(config->postDivModulo) |
                       PMU_AUPLL_CTRL1_ENA_DITHER(config->enaDither) | PMU_AUPLL_CTRL1_EN_VCOX2(config->enaVcoClkX2) |
                       PMU_AUPLL_CTRL1_REFCLK_SEL(config->aupllSrc);

    PMU->AUPLL_CTRL0 = (PMU->AUPLL_CTRL0 & ~PMU_AUPLL_CTRL0_FRACT_MASK) | PMU_AUPLL_CTRL0_FRACT(config->fract);

    /* Power up */
    PMU->AUPLL_CTRL0 |= PMU_AUPLL_CTRL0_PU_MASK;
}

/*! @brief  Deinit the AUPLL.
 */
void CLOCK_DeinitAuPll(void)
{
    /* Power down */
    PMU->AUPLL_CTRL0 &= ~PMU_AUPLL_CTRL0_PU_MASK;
}

/*! @brief  Update the AUPLL post divider
 */
void CLOCK_UpdateAupllPostDiv(uint8_t postDivPattern, uint8_t postDivModulo)
{
    /* Accepted AUPLL_POSTDIV_PATTERN is 0~2 */
    assert(postDivPattern <= 2U);

    /* Accepted AUPLL_POSTDIV_MODULO is 0 ~ 2, 4, 7 */
    assert(((postDivModulo <= 4U) && (postDivModulo != 3U)) || (postDivModulo == 7U));

    PMU->AUPLL_CTRL1 =
        (PMU->AUPLL_CTRL1 & ~(PMU_AUPLL_CTRL1_DIV_OCLK_PATTERN_MASK | PMU_AUPLL_CTRL1_DIV_OCLK_MODULO_MASK)) |
        PMU_AUPLL_CTRL1_DIV_OCLK_PATTERN(postDivPattern) | PMU_AUPLL_CTRL1_DIV_OCLK_MODULO(postDivModulo);
}

/*! @brief  Return Frequency of MAIN XTAL Clock
 *  @return Frequency of MAIN XTAL Clock.
 */
uint32_t CLOCK_GetMainXtalFreq(void)
{
    uint32_t freq;

    if ((PMU->WLAN_CTRL & PMU_WLAN_CTRL_REFCLK_SYS_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        freq = g_mainXtalFreq;
    }

    return freq;
}

/*! @brief  Return Frequency of REFCLK_AUD Clock
 *  @return Frequency of REFCLK_AUD  Clock.
 */
uint32_t CLOCK_GetRefClkAudFreq(void)
{
    uint32_t freq;

    if ((PMU->WLAN_CTRL & PMU_WLAN_CTRL_REFCLK_AUD_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        freq = g_mainXtalFreq;
    }

    return freq;
}

/*! @brief  Return Frequency of RC32M Clock
 *  @return Frequency of RC32M Clock.
 */
uint32_t CLOCK_GetRC32MFreq(void)
{
    uint32_t freq;

    if ((PMU->CLK_RDY & PMU_CLK_RDY_RC32M_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        freq = ((RC32->CLK & RC32_CLK_REF_SEL_MASK) == 0U) ? (CLK_RC32M_CLK / 2U) : CLK_RC32M_CLK;
    }

    return freq;
}

/*! @brief  Return Frequency of XTAL32K Clock
 *  @return Frequency of XTAL32K Clock.
 */
uint32_t CLOCK_GetXtal32KFreq(void)
{
    uint32_t freq;

    if ((PMU->CLK_RDY & PMU_CLK_RDY_X32K_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        freq = ((PMU->XTAL32K_CTRL & PMU_XTAL32K_CTRL_X32K_EXT_OSC_EN_MASK) == 0U) ? CLOCK_GetRC32KFreq() :
                                                                                     CLK_XTAL32K_CLK;
    }

    return freq;
}

/*! @brief  Return Frequency of RC32K Clock
 *  @return Frequency of RC32K Clock.
 */
uint32_t CLOCK_GetRC32KFreq(void)
{
    uint32_t freq;

    if ((PMU->RC32K_CTRL1 & PMU_RC32K_CTRL1_RC32K_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        freq = CLK_RC32K_CLK;
    }

    return freq;
}

/*! @brief  Return Frequency of SFLL Clock
 *  @return Frequency of SFLL Clock.
 */
uint32_t CLOCK_GetSFllFreq(void)
{
    uint32_t pllSrcFreq;
    uint64_t freq;

    if ((PMU->CLK_RDY & PMU_CLK_RDY_PLL_CLK_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        pllSrcFreq = ((PMU->SFLL_CTRL0 & PMU_SFLL_CTRL0_SFLL_REFCLK_SEL_MASK) == 0U) ? CLOCK_GetRC32MFreq() :
                                                                                       CLOCK_GetMainXtalFreq();

        freq = (uint64_t)pllSrcFreq *
               ((PMU->SFLL_CTRL0 & PMU_SFLL_CTRL0_SFLL_FBDIV_MASK) >> PMU_SFLL_CTRL0_SFLL_FBDIV_SHIFT) * 2U;
        freq >>= ((PMU->SFLL_CTRL1 & PMU_SFLL_CTRL1_SFLL_DIV_SEL_MASK) >> PMU_SFLL_CTRL1_SFLL_DIV_SEL_SHIFT);
        freq /= ((PMU->SFLL_CTRL1 & PMU_SFLL_CTRL1_SFLL_REFDIV_MASK) >> PMU_SFLL_CTRL1_SFLL_REFDIV_SHIFT);
    }

    return (uint32_t)freq;
}

static uint32_t CLOCK_GetAuPllPostDiv(void)
{
    uint32_t mod = (PMU->AUPLL_CTRL1 & PMU_AUPLL_CTRL1_DIV_OCLK_MODULO_MASK) >> PMU_AUPLL_CTRL1_DIV_OCLK_MODULO_SHIFT;
    uint32_t pat = (PMU->AUPLL_CTRL1 & PMU_AUPLL_CTRL1_DIV_OCLK_PATTERN_MASK) >> PMU_AUPLL_CTRL1_DIV_OCLK_PATTERN_SHIFT;
    uint32_t code = (mod << 4U) | pat;
    uint32_t div;

    switch (code)
    {
        case 0x30U:
            div = 1U;
            break;
        case 0x50U:
            div = 2U;
            break;
        case 0x00U:
            div = 4U;
            break;
        case 0x11U:
            div = 6U;
            break;
        case 0x10U:
            div = 8U;
            break;
        case 0x12U:
            div = 9U;
            break;
        case 0x21U:
            div = 12U;
            break;
        case 0x20U:
            div = 16U;
            break;
        case 0x22U:
            div = 18U;
            break;
        case 0x41U:
            div = 24U;
            break;
        case 0x42U:
            div = 36U;
            break;
        case 0x61U:
            div = 48U;
            break;
        case 0x62U:
            div = 72U;
            break;
        default:
            assert(false); /* Wrong post divider value */
            div = 1U;
            break;
    }

    return div;
}

/*! @brief  Return Frequency of AUPLL Clock
 *  @return Frequency of AUPLL Clock.
 */
uint32_t CLOCK_GetAuPllFreq(void)
{
    uint32_t pllSrcFreq;
    uint64_t freq;

    if ((PMU->CLK_RDY & PMU_CLK_RDY_PLL_AUDIO_RDY_MASK) == 0U)
    {
        freq = 0U;
    }
    else
    {
        pllSrcFreq = ((PMU->AUPLL_CTRL1 & PMU_AUPLL_CTRL1_REFCLK_SEL_MASK) == 0U) ? CLOCK_GetRC32MFreq() :
                                                                                    CLOCK_GetRefClkAudFreq();

        freq = pllSrcFreq / ((PMU->AUPLL_CTRL1 & PMU_AUPLL_CTRL1_DIV_MCLK_MASK) >> PMU_AUPLL_CTRL1_DIV_MCLK_SHIFT);
        freq *=
            ((PMU->AUPLL_CTRL1 & PMU_AUPLL_CTRL1_DIV_FBCCLK_MASK) >> PMU_AUPLL_CTRL1_DIV_FBCCLK_SHIFT) * FRACT_BASE_NUM;
        freq /= (FRACT_BASE_NUM - ((PMU->AUPLL_CTRL0 & PMU_AUPLL_CTRL0_FRACT_MASK) >> PMU_AUPLL_CTRL0_FRACT_SHIFT));
        freq /= CLOCK_GetAuPllPostDiv();
    }

    return (uint32_t)freq;
}

/*! @brief  Return Frequency of M4/AHB Clock
 *  @return Frequency of M4/AHB Clock.
 */
uint32_t CLOCK_GetCoreBusFreq(void)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t div  = (PMU->MCU_CORE_CLK_DIV & PMU_MCU_CORE_CLK_DIV_FCLK_DIV_MASK) >> PMU_MCU_CORE_CLK_DIV_FCLK_DIV_SHIFT;

    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of APB Clock
 *
 *  @param instance  APB instance 0-1.
 *  @return Frequency of APB Clock.
 */
uint32_t CLOCK_GetApbFreq(uint32_t instance)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t div;

    assert(instance <= 1U);

    div = (PMU->AON_CLK_CTRL & PMU_AON_CLK_CTRL_APB0_CLK_DIV_MASK << (2U * instance)) >>
          (PMU_AON_CLK_CTRL_APB0_CLK_DIV_SHIFT + 2U * instance);

    return freq >> div;
}

/*! @brief  Return Frequency of UART Clock
 *
 *  @param instance  UART instance 0-2.
 *  @return Frequency of UART Clock.
 */
uint32_t CLOCK_GetUartClkFreq(uint32_t instance)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t sel;
    uint32_t nom, denom;

    assert(instance <= 2U);

    sel = PMU->UART_CLK_SEL & (PMU_UART_CLK_SEL_UART0_CLK_SEL_MASK << instance);

    if (sel == 0U) /* Slow */
    {
        nom = (PMU->UART_SLOW_CLK_DIV & PMU_UART_SLOW_CLK_DIV_NOMINATOR_MASK) >> PMU_UART_SLOW_CLK_DIV_NOMINATOR_SHIFT;
        denom = (PMU->UART_SLOW_CLK_DIV & PMU_UART_SLOW_CLK_DIV_DENOMINATOR_MASK) >>
                PMU_UART_SLOW_CLK_DIV_DENOMINATOR_SHIFT;
    }
    else /* Fast */
    {
        nom = (PMU->UART_FAST_CLK_DIV & PMU_UART_FAST_CLK_DIV_NOMINATOR_MASK) >> PMU_UART_FAST_CLK_DIV_NOMINATOR_SHIFT;
        denom = (PMU->UART_FAST_CLK_DIV & PMU_UART_FAST_CLK_DIV_DENOMINATOR_MASK) >>
                PMU_UART_FAST_CLK_DIV_DENOMINATOR_SHIFT;
    }

    assert(nom != 0U);

    return (uint32_t)((uint64_t)freq * denom / nom);
}

/*! @brief  Return Frequency of GPT Clock
 *
 *  @param instance  GPT instance 0-3.
 *  @return Frequency of GPT Clock.
 */
uint32_t CLOCK_GetGptClkFreq(uint32_t instance)
{
    uint32_t freq;
    uint32_t reg;
    uint32_t sel;
    uint32_t div;

    assert(instance <= 3U);

    switch (instance)
    {
        case 0:
            reg = PMU->GPT0_CTRL;
            div = (reg & PMU_GPT0_CTRL_GPT0_CLK_DIV_MASK) >> PMU_GPT0_CTRL_GPT0_CLK_DIV_SHIFT;
            break;
        case 1:
            reg = PMU->GPT1_CTRL;
            div = (reg & PMU_GPT0_CTRL_GPT0_CLK_DIV_MASK) >> PMU_GPT0_CTRL_GPT0_CLK_DIV_SHIFT;
            break;
        case 2:
            reg = PMU->GPT2_CTRL;
            div = (reg & PMU_GPT0_CTRL_GPT0_CLK_DIV_MASK) >> PMU_GPT0_CTRL_GPT0_CLK_DIV_SHIFT;
            break;
        default:
            reg = PMU->GPT3_CTRL;
            div = (((PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_5_3_MASK) >>
                    PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_5_3_SHIFT)
                   << 3U) |
                  ((PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_2_0_MASK) >>
                   PMU_PERI2_CLK_DIV_GPT3_CLK_DIV_2_0_SHIFT);
            break;
    }

    sel = (reg & (PMU_GPT0_CTRL_GPT0_CLK_SEL0_MASK | PMU_GPT0_CTRL_GPT0_CLK_SEL1_MASK)) >>
          PMU_GPT0_CTRL_GPT0_CLK_SEL1_SHIFT;
    div = div == 0U ? 1U : div;

    switch (sel)
    {
        case 0x0:
            freq = CLOCK_GetSysClkFreq() / div;
            break;
        case 0x2:
            freq = CLOCK_GetRC32KFreq();
            break;
        case 0x3:
            freq = CLOCK_GetXtal32KFreq();
            break;
        case 0x8:
            freq = CLOCK_GetRC32MFreq() / div;
            break;
        case 0xC:
            freq = CLOCK_GetMainXtalFreq() / div;
            break;
        default:
            assert(false);
            freq = 0U;
            break;
    }

    return freq;
}

/*! @brief  Return Frequency of GPT Sample Clock
 *
 *  @return Frequency of GPT Sample Clock.
 */
uint32_t CLOCK_GetGptSampleClkFreq(void)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t div =
        (PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_GPT_SAMPLE_CLK_DIV_MASK) >> PMU_PERI2_CLK_DIV_GPT_SAMPLE_CLK_DIV_SHIFT;

    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of SSP Clock
 *
 *  @param instance  SSP instance 0-2.
 *  @return Frequency of SSP Clock.
 */
uint32_t CLOCK_GetSspClkFreq(uint32_t instance)
{
    uint32_t freq;
    uint32_t sel;
    uint32_t div;

    assert(instance <= 2U);

    sel = PMU->PERI_CLK_SRC & (PMU_PERI_CLK_SRC_SSP0_AUDIO_SEL_MASK << instance);
    if (sel == 0U)
    {
        freq = CLOCK_GetSysClkFreq();
    }
    else
    {
        freq = CLOCK_GetAuPllFreq();
    }

    div = (PMU->PERI0_CLK_DIV & (PMU_PERI0_CLK_DIV_SSP0_CLK_DIV_MASK << (5U * instance))) >> (5U * instance);
    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of GAU Clock
 *  @return Frequency of GAU Clock.
 */
uint32_t CLOCK_GetGauClkFreq(void)
{
    uint32_t freq;
    uint32_t div;

    freq = CLOCK_GetAuPllFreq();

    div = (PMU->PERI3_CTRL & PMU_PERI3_CTRL_GAU_DIV_MASK) >> PMU_PERI3_CTRL_GAU_DIV_SHIFT;
    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of RTC Clock
 *  @return Frequency of RTC Clock.
 */
uint32_t CLOCK_GetRtcClkFreq(void)
{
    uint32_t freq;

    if ((PMU->AON_CLK_CTRL & PMU_AON_CLK_CTRL_RTC_INT_SEL_MASK) == 0U)
    {
        freq = CLOCK_GetRC32KFreq();
    }
    else
    {
        freq = CLOCK_GetXtal32KFreq();
    }

    return freq;
}

/*! @brief  Return Frequency of I2C Clock
 *  @return Frequency of I2C Clock.
 */
uint32_t CLOCK_GetI2cClkFreq(void)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t div  = (PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_I2C_CLK_DIV_MASK) >> PMU_PERI2_CLK_DIV_I2C_CLK_DIV_SHIFT;

    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of QSPI Clock
 *  @return Frequency of QSPI Clock.
 */
uint32_t CLOCK_GetQspiClkFreq(void)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t div  = (PMU->PERI1_CLK_DIV & (7UL << 8U)) >> 8U;

    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of FlashC Clock
 *  @return Frequency of FlashC Clock.
 */
uint32_t CLOCK_GetFlashCClkFreq(void)
{
    uint32_t freq = CLOCK_GetSysClkFreq();
    uint32_t div = (PMU->PERI1_CLK_DIV & PMU_PERI1_CLK_DIV_FLASH_CLK_DIV_MASK) >> PMU_PERI1_CLK_DIV_FLASH_CLK_DIV_SHIFT;

    div = div == 0U ? 1U : div;

    return freq / div;
}

/*! @brief  Return Frequency of WDT Clock
 *  @return Frequency of WDT Clock.
 */
uint32_t CLOCK_GetWdtClkFreq(void)
{
    uint32_t freq = CLOCK_GetApbFreq(1U);
    uint32_t div22 =
        (PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_WDT_CLK_DIV_2_2_MASK) >> PMU_PERI2_CLK_DIV_WDT_CLK_DIV_2_2_SHIFT;
    uint32_t div10 =
        (PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_WDT_CLK_DIV_1_0_MASK) >> PMU_PERI2_CLK_DIV_WDT_CLK_DIV_1_0_SHIFT;
    uint32_t div53 =
        (PMU->PERI2_CLK_DIV & PMU_PERI2_CLK_DIV_WDT_CLK_DIV_5_3_MASK) >> PMU_PERI2_CLK_DIV_WDT_CLK_DIV_5_3_SHIFT;
    uint32_t div;

    div = (div53 << 3U) | (div22 << 2U) | div10;

    return freq >> div;
}

/*! @brief  Return Frequency of SDIO Clock
 *  @return Frequency of SDIO Clock.
 */
uint32_t CLOCK_GetSdioClkFreq(void)
{
    uint32_t freq;
    uint32_t div;

    freq = CLOCK_GetSysClkFreq();

    div = (PMU->PERI0_CLK_DIV & (0xFUL << 16U)) >> 16U;
    div = div == 0U ? 1U : div;

    return freq / div;
}
