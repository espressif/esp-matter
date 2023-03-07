/*
 * Copyright 2017, 2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_mipi_dsi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.mipi_dsi"
#endif

/* The timeout cycles to wait for DSI state machine idle. */
#ifndef FSL_MIPI_DSI_IDLE_TIMEOUT
#define FSL_MIPI_DSI_IDLE_TIMEOUT 0x1000
#endif

/* PLL CN should be in the range of 1 to 32. */
#define DSI_DPHY_PLL_CN_MIN 1U
#define DSI_DPHY_PLL_CN_MAX 32U

/* PLL refClk / CN should be in the range of 24M to 30M. */
#define DSI_DPHY_PLL_REFCLK_CN_MIN 24000000U
#define DSI_DPHY_PLL_REFCLK_CN_MAX 30000000U

/* PLL CM should be in the range of 16 to 255. */
#define DSI_DPHY_PLL_CM_MIN 16U
#define DSI_DPHY_PLL_CM_MAX 255U

/* PLL VCO output frequency max value is 1.5GHz, VCO output is (refClk / CN ) * CM. */
#define DSI_DPHY_PLL_VCO_MAX 1500000000U
#define DSI_DPHY_PLL_VCO_MIN (DSI_DPHY_PLL_REFCLK_CN_MIN * DSI_DPHY_PLL_CM_MIN)

#define DSI_HOST_PKT_CONTROL_WORD_COUNT(wc) ((uint32_t)(wc) << 0U)
#define DSI_HOST_PKT_CONTROL_VC(vc) ((uint32_t)(vc) << 16U)
#define DSI_HOST_PKT_CONTROL_HEADER_TYPE(type) ((uint32_t)(type) << 18U)
#define DSI_HOST_PKT_CONTROL_HS_MASK (1U << 24U)
#define DSI_HOST_PKT_CONTROL_BTA_MASK (1U << 25U)
#define DSI_HOST_PKT_CONTROL_BTA_ONLY_MASK (1U << 26U)

/* Macro used for D-PHY timing setting. */
#define DSI_THS_ZERO_BYTE_CLK_BASE 6U
#define DSI_TCLK_ZERO_BYTE_CLK_BASE 3U
#define DSI_THS_PREPARE_HALF_ESC_CLK_BASE 2U
#define DSI_TCLK_PREPARE_HALF_ESC_CLK_BASE 2U

/* Convert ns to byte clock. */
#define DSI_NS_TO_BYTE_CLK(ns, byte_clk_khz) ((ns) * (byte_clk_khz) / 1000000U)

/* Packet overhead for HSA, HFP, HBP */
#define DSI_HSA_OVERHEAD_BYTE 10 /* HSS + HSA header + HSA CRC. */
#define DSI_HFP_OVERHEAD_BYTE 8  /* RGB data packet CRC + HFP header + HFP CRC. */
#define DSI_HBP_OVERHEAD_BYTE 14 /* HSE + HBP header + HBP CRC + RGB data packet header */

#define DSI_INT_STATUS_TRIGGER_MASK                                                       \
    (kDSI_InterruptGroup1ResetTriggerReceived | kDSI_InterruptGroup1TearTriggerReceived | \
     kDSI_InterruptGroup1AckTriggerReceived)
#define DSI_INT_STATUS_ERROR_REPORT_MASK (0xFFFFU << 9U)

#if (defined(FSL_FEATURE_DSI_CSR_OFFSET) && FSL_FEATURE_DSI_CSR_OFFSET)
#define DSI_GET_CSR(dsi_base) (MIPI_DSI_CSR_Type *)((uint32_t)(dsi_base)-FSL_FEATURE_DSI_CSR_OFFSET)
#endif

#if defined(MIPI_DSI_HOST_DPHY_PD_TX_dphy_pd_tx_MASK)
#define DPHY_PD_REG DPHY_PD_TX
#elif defined(MIPI_DSI_HOST_DPHY_PD_DPHY_dphy_pd_dphy_MASK)
#define DPHY_PD_REG DPHY_PD_DPHY
#endif

/*! @brief Typedef for MIPI DSI interrupt handler. */
typedef void (*dsi_isr_t)(MIPI_DSI_HOST_Type *base, dsi_handle_t *handle);

/*******************************************************************************
 * Variables
 ******************************************************************************/
#if defined(MIPI_DSI_HOST_IRQS)
/* Array of DSI IRQ number. */
static const IRQn_Type s_dsiIRQ[] = MIPI_DSI_HOST_IRQS;
#endif
/*! @brief Pointers to MIPI DSI bases for each instance. */
static MIPI_DSI_HOST_Type *const s_dsiBases[] = MIPI_DSI_HOST_BASE_PTRS;
/*! @brief MIPI DSI internal handle pointer array */
static dsi_handle_t *s_dsiHandle[ARRAY_SIZE(s_dsiBases)];
/*! @brief Pointer to IRQ handler. */
static dsi_isr_t s_dsiIsr;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to MIPI DSI clocks for each instance. */
static const clock_ip_name_t s_dsiClocks[] = MIPI_DSI_HOST_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the MIPI DSI host controller instance from peripheral base address.
 *
 * @param base MIPI DSI peripheral base address.
 * @return MIPI DSI instance.
 */
uint32_t DSI_GetInstance(MIPI_DSI_HOST_Type *base);

#if !((defined(FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL) && (FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL)))
/*!
 * @brief Convert the D-PHY PLL CN to the value could be set to register.
 *
 * @param cn The CN value.
 * @return The register value.
 */
static uint8_t DSI_EncodeDphyPllCn(uint8_t cn);

/*!
 * @brief Convert the D-PHY PLL CM to the value could be set to register.
 *
 * @param cm The CM value.
 * @return The register value.
 */
static uint8_t DSI_EncodeDphyPllCm(uint8_t cm);

/*!
 * @brief Calculate the D-PHY PLL dividers to generate the desired output frequency.
 *
 * Calculate the PLL dividers to generate the most close desired output PLL frequency.
 *
 * txHsBitClk_Hz = refClkFreq_Hz * CM / (CN * CO).
 * CM: 16 ~ 255
 * CN: 1 ~ 32
 * CO: 1, 2, 4, 8
 *
 * @param cn The CN value, convert using @ref DSI_EncodeDphyPllCn before setting to register.
 * @param cm The CM value, convert using @ref DSI_EncodeDphyPllCm before setting to register.
 * @param co The CO value, could set to register directly.
 * @param refClkFreq_Hz The D-PHY input reference clock frequency (REF_CLK).
 * @param desiredOutFreq_Hz Desired PLL output frequency.
 * @return The actually output frequency using the returned dividers. If could not
 * find suitable dividers, return 0.
 */
static uint32_t DSI_DphyGetPllDivider(
    uint32_t *cn, uint32_t *cm, uint32_t *co, uint32_t refClkFreq_Hz, uint32_t desiredOutFreq_Hz);
#endif

/*!
 * @brief Clear the RX FIFO.
 *
 * @param base MIPI DSI host peripheral base address.
 */
static void DSI_ApbClearRxFifo(MIPI_DSI_HOST_Type *base);

/*!
 * @brief Handle the DSI transfer result.
 *
 * @param base MIPI DSI host peripheral base address.
 * @param xfer The transfer definition.
 * @param intFlags1 Interrupt flag group 1.
 * @param intFlags2 Interrupt flag group 2.
 * @retval kStatus_Success No error happens.
 * @retval kStatus_Timeout Hardware timeout detected.
 * @retval kStatus_DSI_RxDataError RX data error.
 * @retval kStatus_DSI_ErrorReportReceived Error Report packet received.
 * @retval kStatus_DSI_Fail Transfer failed for other reasons.
 */
static status_t DSI_HandleResult(MIPI_DSI_HOST_Type *base,
                                 uint32_t intFlags1,
                                 uint32_t intFlags2,
                                 dsi_transfer_t *xfer);

/*!
 * @brief Prepare for the DSI APB transfer.
 *
 * This function fills TX data to DSI TX FIFO and sets the packet control
 * register. Packet transfer could start using @ref DSI_SendApbPacket after
 * this function.
 *
 * @param base MIPI DSI host peripheral base address.
 * @param xfer The transfer definition.
 * @retval kStatus_Success It is ready to start transfer.
 * @retval kStatus_DSI_NotSupported The transfer format is not supported.
 */
static status_t DSI_PrepareApbTransfer(MIPI_DSI_HOST_Type *base, dsi_transfer_t *xfer);

/*******************************************************************************
 * Code
 ******************************************************************************/

uint32_t DSI_GetInstance(MIPI_DSI_HOST_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_dsiBases); instance++)
    {
        if (s_dsiBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_dsiBases));

    return instance;
}

#if !((defined(FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL) && (FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL)))
static uint8_t DSI_EncodeDphyPllCn(uint8_t cn)
{
    assert((cn >= 1) && (cn <= 32));

    if (1 == cn)
    {
        return 0x1FU;
    }
    else
    {
        return (0x65BD44E0U >> ((uint32_t)cn - 2U)) & 0x1FU;
    }
}

static uint8_t DSI_EncodeDphyPllCm(uint8_t cm)
{
    assert(cm >= 16);

    if (cm <= 31)
    {
        return 0xE0U | cm;
    }
    else if (cm <= 63)
    {
        return 0xC0U | (cm & 0x1FU);
    }
    else if (cm <= 127)
    {
        return 0x80U | (cm & 0x3FU);
    }
    else
    {
        return cm & 0xCFU;
    }
}

static uint32_t DSI_DphyGetPllDivider(
    uint32_t *cn, uint32_t *cm, uint32_t *co, uint32_t refClkFreq_Hz, uint32_t desiredOutFreq_Hz)
{
    uint32_t cnCur, cmCur, coShiftCur, pllFreqCur, diffCur, vcoFreq, refClk_CN;
    uint32_t diff             = 0xFFFFFFFFU;
    uint32_t pllFreqCandidate = 0U;

    /* CO available values are 1, 2, 4, 8, so the shift values are 0, 1, 2, 3.  */
    for (coShiftCur = 0U; coShiftCur <= 3U; coShiftCur++)
    {
        /* Desired VCO output frequency. */
        vcoFreq = desiredOutFreq_Hz << coShiftCur;

        /* If desired VCO output frequency is too small, try larger CO value. */
        if (vcoFreq < DSI_DPHY_PLL_VCO_MIN)
        {
            continue;
        }

        /* If desired VCO output frequency is too large, search finished. */
        if (vcoFreq > DSI_DPHY_PLL_VCO_MAX)
        {
            break;
        }

        /* Now search the best CN and CM to generate disired VCO output frequency. */
        for (cnCur = DSI_DPHY_PLL_CN_MIN; cnCur <= DSI_DPHY_PLL_CN_MAX; cnCur++)
        {
            /* REF_CLK / CN. */
            refClk_CN = refClkFreq_Hz / cnCur;

            /* If desired REF_CLK / CN frequency is too large, try larger CN value. */
            if (refClk_CN > DSI_DPHY_PLL_REFCLK_CN_MAX)
            {
                continue;
            }

            /* If desired REF_CLK / CN frequency is too small, stop search. */
            if (refClk_CN < DSI_DPHY_PLL_REFCLK_CN_MIN)
            {
                break;
            }

            /* Get the CM most close. */
            cmCur = (vcoFreq + (refClk_CN / 2U)) / refClk_CN;

            /* If calculated value is (DSI_DPHY_PLL_CM_MAX + 1), use DSI_DPHY_PLL_CM_MAX. */
            if ((DSI_DPHY_PLL_CM_MAX + 1U) == cmCur)
            {
                cmCur = DSI_DPHY_PLL_CM_MAX;
            }

            if ((cmCur < DSI_DPHY_PLL_CM_MIN) || (cmCur > DSI_DPHY_PLL_CM_MAX))
            {
                continue;
            }

            /* Output frequency using current dividers. */
            pllFreqCur = (refClk_CN * cmCur) >> coShiftCur;

            diffCur =
                (pllFreqCur > desiredOutFreq_Hz) ? (pllFreqCur - desiredOutFreq_Hz) : (desiredOutFreq_Hz - pllFreqCur);

            /* If the dividers is better. */
            if (diffCur < diff)
            {
                diff             = diffCur;
                *cm              = cmCur;
                *cn              = cnCur;
                *co              = coShiftCur;
                pllFreqCandidate = pllFreqCur;

                /* If the output PLL frequency is exactly the disired value, return directly. */
                if (0U == diff)
                {
                    return pllFreqCandidate;
                }
            }
        }
    }

    return pllFreqCandidate;
}
#endif

static void DSI_ApbClearRxFifo(MIPI_DSI_HOST_Type *base)
{
    volatile uint32_t dummy;
    uint32_t level = base->DSI_HOST_PKT_FIFO_RD_LEVEL;

    while (level--)
    {
        dummy = base->DSI_HOST_PKT_RX_PAYLOAD;
    }

    (void)dummy;
}

/*!
 * brief Initializes an MIPI DSI host with the user configuration.
 *
 * This function initializes the MIPI DSI host with the configuration, it should
 * be called first before other MIPI DSI driver functions.
 *
 * param base MIPI DSI host peripheral base address.
 * param config Pointer to a user-defined configuration structure.
 */
void DSI_Init(MIPI_DSI_HOST_Type *base, const dsi_config_t *config)
{
    assert(config);

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_EnableClock(s_dsiClocks[DSI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

#if (defined(FSL_FEATURE_DSI_CSR_OFFSET) && FSL_FEATURE_DSI_CSR_OFFSET)
    MIPI_DSI_CSR_Type *csr = DSI_GET_CSR(base);
    if (config->enableTxUlps)
    {
        csr->TX_ULPS_ENABLE = MIPI_DSI_TX_ULPS_ENABLE_TX_ULPS_ENABLE_MASK;
    }
    else
    {
        csr->TX_ULPS_ENABLE = 0;
    }
#endif

    base->DSI_HOST_CFG_NUM_LANES = config->numLanes - 1U;

    if (config->enableNonContinuousHsClk)
    {
        base->DSI_HOST_CFG_NONCONTINUOUS_CLK = 0x01U;
    }
    else
    {
        base->DSI_HOST_CFG_NONCONTINUOUS_CLK = 0x00U;
    }

    if (config->autoInsertEoTp)
    {
        base->DSI_HOST_CFG_AUTOINSERT_EOTP = 0x01U;
    }
    else
    {
        base->DSI_HOST_CFG_AUTOINSERT_EOTP = 0x00U;
    }

    base->DSI_HOST_CFG_EXTRA_CMDS_AFTER_EOTP = config->numExtraEoTp;
    base->DSI_HOST_CFG_HTX_TO_COUNT          = config->htxTo_ByteClk;
    base->DSI_HOST_CFG_LRX_H_TO_COUNT        = config->lrxHostTo_ByteClk;
    base->DSI_HOST_CFG_BTA_H_TO_COUNT        = config->btaTo_ByteClk;

    DSI_ApbClearRxFifo(base);

    /* Disable all interrupts by default, user could enable
     * the desired interrupts later.
     */
    base->DSI_HOST_IRQ_MASK  = 0xFFFFFFFFU;
    base->DSI_HOST_IRQ_MASK2 = 0xFFFFFFFFU;
}

/*!
 * brief Deinitializes an MIPI DSI host.
 *
 * This function should be called after all bother MIPI DSI driver functions.
 *
 * param base MIPI DSI host peripheral base address.
 */
void DSI_Deinit(MIPI_DSI_HOST_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    CLOCK_DisableClock(s_dsiClocks[DSI_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

/*!
 * brief Get the default configuration to initialize the MIPI DSI host.
 *
 * The default value is:
 * code
    config->numLanes = 4;
    config->enableNonContinuousHsClk = false;
    config->enableTxUlps = false;
    config->autoInsertEoTp = true;
    config->numExtraEoTp = 0;
    config->htxTo_ByteClk = 0;
    config->lrxHostTo_ByteClk = 0;
    config->btaTo_ByteClk = 0;
   endcode
 *
 * param config Pointer to a user-defined configuration structure.
 */
void DSI_GetDefaultConfig(dsi_config_t *config)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    config->numLanes                 = 4;
    config->enableNonContinuousHsClk = false;
    config->enableTxUlps             = false;
    config->autoInsertEoTp           = true;
    config->numExtraEoTp             = 0;
    config->htxTo_ByteClk            = 0;
    config->lrxHostTo_ByteClk        = 0;
    config->btaTo_ByteClk            = 0;
}

/*!
 * brief Configure the DPI interface core.
 *
 * This function sets the DPI interface configuration, it should be used in
 * video mode.
 *
 * param base MIPI DSI host peripheral base address.
 * param config Pointer to the DPI interface configuration.
 * param numLanes Lane number, should be same with the setting in ref dsi_dpi_config_t.
 * param dpiPixelClkFreq_Hz The DPI pixel clock frequency in Hz.
 * param dsiHsBitClkFreq_Hz The DSI high speed bit clock frequency in Hz. It is
 * the same with DPHY PLL output.
 */
void DSI_SetDpiConfig(MIPI_DSI_HOST_Type *base,
                      const dsi_dpi_config_t *config,
                      uint8_t numLanes,
                      uint32_t dpiPixelClkFreq_Hz,
                      uint32_t dsiHsBitClkFreq_Hz)
{
    assert(config);

    /* coefficient DPI event size to number of DSI bytes. */
    uint32_t coff = (numLanes * dsiHsBitClkFreq_Hz) / (dpiPixelClkFreq_Hz * 8);

#if (defined(FSL_FEATURE_DSI_CSR_OFFSET) && FSL_FEATURE_DSI_CSR_OFFSET)
    MIPI_DSI_CSR_Type *csr = DSI_GET_CSR(base);
    csr->PXL2DPI_CONFIG    = config->dpiColorCoding;
#endif

    base->DSI_HOST_CFG_DPI_PIXEL_PAYLOAD_SIZE     = config->pixelPayloadSize;
    base->DSI_HOST_CFG_DPI_INTERFACE_COLOR_CODING = config->dpiColorCoding;
    base->DSI_HOST_CFG_DPI_PIXEL_FORMAT           = config->pixelPacket;
    base->DSI_HOST_CFG_DPI_VIDEO_MODE             = config->videoMode;

    if (kDSI_DpiBllpLowPower == config->bllpMode)
    {
        base->DSI_HOST_CFG_DPI_BLLP_MODE         = 0x1U;
        base->DSI_HOST_CFG_DPI_USE_NULL_PKT_BLLP = 0x0U;
    }
    else if (kDSI_DpiBllpBlanking == config->bllpMode)
    {
        base->DSI_HOST_CFG_DPI_BLLP_MODE         = 0x0U;
        base->DSI_HOST_CFG_DPI_USE_NULL_PKT_BLLP = 0x0U;
    }
    else
    {
        base->DSI_HOST_CFG_DPI_BLLP_MODE         = 0x0U;
        base->DSI_HOST_CFG_DPI_USE_NULL_PKT_BLLP = 0x1U;
    }

    if (config->polarityFlags & kDSI_DpiVsyncActiveHigh)
    {
        base->DSI_HOST_CFG_DPI_VSYNC_POLARITY = 0x01U;
    }
    else
    {
        base->DSI_HOST_CFG_DPI_VSYNC_POLARITY = 0x00U;
    }

    if (config->polarityFlags & kDSI_DpiHsyncActiveHigh)
    {
        base->DSI_HOST_CFG_DPI_HSYNC_POLARITY = 0x01U;
    }
    else
    {
        base->DSI_HOST_CFG_DPI_HSYNC_POLARITY = 0x00U;
    }

    if (kDSI_DpiNonBurstWithSyncPulse == config->videoMode)
    {
        base->DSI_HOST_CFG_DPI_HFP                   = config->hfp - DSI_HFP_OVERHEAD_BYTE;
        base->DSI_HOST_CFG_DPI_HBP                   = config->hbp - DSI_HBP_OVERHEAD_BYTE;
        base->DSI_HOST_CFG_DPI_HSA                   = config->hsw - DSI_HSA_OVERHEAD_BYTE;
        base->DSI_HOST_CFG_DPI_PIXEL_FIFO_SEND_LEVEL = 8;
    }
    else
    {
        base->DSI_HOST_CFG_DPI_HFP                   = config->hfp * coff;
        base->DSI_HOST_CFG_DPI_HBP                   = config->hbp * coff;
        base->DSI_HOST_CFG_DPI_HSA                   = config->hsw * coff;
        base->DSI_HOST_CFG_DPI_PIXEL_FIFO_SEND_LEVEL = config->pixelPayloadSize;
    }

    base->DSI_HOST_CFG_DPI_VBP = config->vbp;
    base->DSI_HOST_CFG_DPI_VFP = config->vfp;

    base->DSI_HOST_CFG_DPI_VACTIVE = config->panelHeight - 1U;
    base->DSI_HOST_CFG_DPI_VC      = config->virtualChannel;
}

/*!
 * brief Initializes the D-PHY
 *
 * This function configures the D-PHY timing and setups the D-PHY PLL based on
 * user configuration. The configuration structure could be got by the function
 * ref DSI_GetDphyDefaultConfig.
 *
 * param base MIPI DSI host peripheral base address.
 * param config Pointer to the D-PHY configuration.
 * param refClkFreq_Hz The REFCLK frequency in Hz.
 * return The actual D-PHY PLL output frequency. If could not configure the
 * PLL to the target frequency, the return value is 0.
 */
uint32_t DSI_InitDphy(MIPI_DSI_HOST_Type *base, const dsi_dphy_config_t *config, uint32_t refClkFreq_Hz)
{
    assert(config);

#if !((defined(FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL) && (FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL)))
    uint32_t cn, cm, co, outputPllFreq;

    outputPllFreq = DSI_DphyGetPllDivider(&cn, &cm, &co, refClkFreq_Hz, config->txHsBitClk_Hz);

    /* If could not find dividers for the output PLL frequency. */
    if (!outputPllFreq)
    {
        return 0U;
    }

    /* Set the DPHY parameters. */
    base->DPHY_CN = DSI_EncodeDphyPllCn(cn);
    base->DPHY_CM = DSI_EncodeDphyPllCm(cm);
    base->DPHY_CO = co;
#endif

    /* Set the timing parameters. */
    base->DPHY_M_PRG_HS_PREPARE  = config->tHsPrepare_HalfEscClk - DSI_THS_PREPARE_HALF_ESC_CLK_BASE;
    base->DPHY_MC_PRG_HS_PREPARE = config->tClkPrepare_HalfEscClk - DSI_TCLK_PREPARE_HALF_ESC_CLK_BASE;
    base->DPHY_M_PRG_HS_ZERO     = config->tHsZero_ByteClk - DSI_THS_ZERO_BYTE_CLK_BASE;
    base->DPHY_MC_PRG_HS_ZERO    = config->tClkZero_ByteClk - DSI_TCLK_ZERO_BYTE_CLK_BASE;
    base->DPHY_M_PRG_HS_TRAIL    = config->tHsTrail_ByteClk;
    base->DPHY_MC_PRG_HS_TRAIL   = config->tClkTrail_ByteClk;

    base->DSI_HOST_CFG_T_PRE   = config->tClkPre_ByteClk;
    base->DSI_HOST_CFG_T_POST  = config->tClkPost_ByteClk;
    base->DSI_HOST_CFG_TX_GAP  = config->tHsExit_ByteClk;
    base->DSI_HOST_CFG_TWAKEUP = config->tWakeup_EscClk;

#if defined(MIPI_DSI_HOST_DPHY_RTERM_SEL_dphy_rterm_sel_MASK)
    base->DPHY_RTERM_SEL = MIPI_DSI_HOST_DPHY_RTERM_SEL_dphy_rterm_sel_MASK;
#endif
#if defined(MIPI_DSI_HOST_DPHY_TX_RCAL_dphy_tx_rcal_MASK)
    base->DPHY_TX_RCAL = 1;
#endif
    base->DPHY_RXLPRP = 1;
    base->DPHY_RXCDRP = 1;

    /* Auto power down the inactive lanes. */
    base->DPHY_AUTO_PD_EN = 0x1U;

    base->DPHY_TST = 0x25U;

#if !((defined(FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL) && (FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL)))
    /* Power up the PLL. */
    base->DPHY_PD_PLL = 0U;

    /* Wait for the PLL lock. */
    while (!base->DPHY_LOCK)
    {
    }
#endif

    /* Power up the DPHY. */
    base->DPHY_PD_REG = 0U;

#if !((defined(FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL) && (FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL)))
    return outputPllFreq;
#else
    return config->txHsBitClk_Hz;
#endif
}

/*!
 * brief Deinitializes the D-PHY
 *
 * Power down the D-PHY PLL and shut down D-PHY.
 *
 * param base MIPI DSI host peripheral base address.
 */
void DSI_DeinitDphy(MIPI_DSI_HOST_Type *base)
{
#if !((defined(FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL) && (FSL_FEATURE_MIPI_DSI_HOST_NO_DPHY_PLL)))
    /* Power down the PLL. */
    base->DPHY_PD_PLL = 1U;
#endif

    /* Power down the DPHY. */
    base->DPHY_PD_REG = 1U;
}

/*!
 * brief Get the default D-PHY configuration.
 *
 * Gets the default D-PHY configuration, the timing parameters are set according
 * to D-PHY specification. User could use the configuration directly, or change
 * some parameters according to the special device.
 *
 * param config Pointer to the D-PHY configuration.
 * param txHsBitClk_Hz High speed bit clock in Hz.
 * param txEscClk_Hz Esc clock in Hz.
 */
void DSI_GetDphyDefaultConfig(dsi_dphy_config_t *config, uint32_t txHsBitClk_Hz, uint32_t txEscClk_Hz)
{
    assert(config);

    /* Initializes the configure structure to zero. */
    memset(config, 0, sizeof(*config));

    uint32_t byteClkFreq_kHz = txHsBitClk_Hz / 8U / 1000U;

    config->txHsBitClk_Hz = txHsBitClk_Hz;

    /* TCLK-PRE in byte clock. At least 8*UI. */
    config->tClkPre_ByteClk = 1U;

    /* TCLK-POST in byte clock. At least 60ns + 52*UI. */
    config->tClkPost_ByteClk = DSI_NS_TO_BYTE_CLK(60U, byteClkFreq_kHz) + (52U / 8U) + 2U;

    /* THS-EXIT in byte clock. At least 100ns. */
    config->tHsExit_ByteClk = DSI_NS_TO_BYTE_CLK(100U, byteClkFreq_kHz) + 1U;

    /* T-WAKEUP. At least 1ms. */
    config->tWakeup_EscClk = txEscClk_Hz / 1000U + 1U;

    /* THS-PREPARE. 40ns+4*UI to 85ns+6*UI. */
    config->tHsPrepare_HalfEscClk =
        (40U * txEscClk_Hz * 2U) / 1000000000U + (4U * txEscClk_Hz * 2U / txHsBitClk_Hz) + 1U;
    if (config->tHsPrepare_HalfEscClk < DSI_THS_PREPARE_HALF_ESC_CLK_BASE)
    {
        config->tHsPrepare_HalfEscClk = DSI_THS_PREPARE_HALF_ESC_CLK_BASE;
    }

    /* TCLK-PREPARE. 38ns to 95ns. */
    config->tClkPrepare_HalfEscClk = (38U * txEscClk_Hz * 2U) / 1000000000U + 1U;
    if (config->tClkPrepare_HalfEscClk < DSI_TCLK_PREPARE_HALF_ESC_CLK_BASE)
    {
        config->tClkPrepare_HalfEscClk = DSI_TCLK_PREPARE_HALF_ESC_CLK_BASE;
    }

    /* THS-ZERO, At least 105ns+6*UI. */
    config->tHsZero_ByteClk = DSI_NS_TO_BYTE_CLK(105U, byteClkFreq_kHz) + 1U;
    if (config->tHsZero_ByteClk < DSI_THS_ZERO_BYTE_CLK_BASE + 1U)
    {
        config->tHsZero_ByteClk = DSI_THS_ZERO_BYTE_CLK_BASE + 1U;
    }

    /* TCLK-ZERO, At least 262ns. */
    config->tClkZero_ByteClk = DSI_NS_TO_BYTE_CLK(262U, byteClkFreq_kHz) + 1U;
    if (config->tClkZero_ByteClk < DSI_TCLK_ZERO_BYTE_CLK_BASE + 1U)
    {
        config->tClkZero_ByteClk = DSI_TCLK_ZERO_BYTE_CLK_BASE + 1U;
    }

    /* THS-TRAIL, 60ns+4*UI to 105ns+12UI. */
    config->tHsTrail_ByteClk = DSI_NS_TO_BYTE_CLK(60U, byteClkFreq_kHz) + 2U;

    /* TCLK-TRAIL, at least 60ns. */
    config->tClkTrail_ByteClk = DSI_NS_TO_BYTE_CLK(60U, byteClkFreq_kHz) + 1U;
}

/*!
 * brief Configure the APB packet to send.
 *
 * This function configures the next APB packet transfer. After configuration,
 * the packet transfer could be started with function ref DSI_SendApbPacket.
 * If the packet is long packet, Use ref DSI_WriteApbTxPayload to fill the payload
 * before start transfer.
 *
 * param base MIPI DSI host peripheral base address.
 * param wordCount For long packet, this is the byte count of the payload.
 * For short packet, this is (data1 << 8) | data0.
 * param virtualChannel Virtual channel.
 * param dataType The packet data type, (DI).
 * param flags The transfer control flags, see ref _dsi_transfer_flags.
 */
void DSI_SetApbPacketControl(
    MIPI_DSI_HOST_Type *base, uint16_t wordCount, uint8_t virtualChannel, dsi_tx_data_type_t dataType, uint8_t flags)
{
    uint32_t pktCtrl = DSI_HOST_PKT_CONTROL_WORD_COUNT(wordCount) | DSI_HOST_PKT_CONTROL_VC(virtualChannel) |
                       DSI_HOST_PKT_CONTROL_HEADER_TYPE(dataType);

    if (flags & kDSI_TransferUseHighSpeed)
    {
        pktCtrl |= DSI_HOST_PKT_CONTROL_HS_MASK;
    }

    if (flags & kDSI_TransferPerformBTA)
    {
        pktCtrl |= DSI_HOST_PKT_CONTROL_BTA_MASK;
    }

    base->DSI_HOST_PKT_CONTROL = pktCtrl;
}

/*!
 * brief Fill the long APB packet payload.
 *
 * Write the long packet payload to TX FIFO.
 *
 * param base MIPI DSI host peripheral base address.
 * param payload Pointer to the payload.
 * param payloadSize Payload size in byte.
 */
void DSI_WriteApbTxPayload(MIPI_DSI_HOST_Type *base, const uint8_t *payload, uint16_t payloadSize)
{
    DSI_WriteApbTxPayloadExt(base, payload, payloadSize, false, 0U);
}

void DSI_WriteApbTxPayloadExt(
    MIPI_DSI_HOST_Type *base, const uint8_t *payload, uint16_t payloadSize, bool sendDscCmd, uint8_t dscCmd)
{
    uint32_t firstWord;
    uint32_t i;

    payloadSize = sendDscCmd ? payloadSize + 1U : payloadSize;

    assert(payloadSize <= FSL_DSI_TX_MAX_PAYLOAD_BYTE);

    /* The first 4-byte. */
    if (sendDscCmd)
    {
        firstWord = dscCmd;
    }
    else
    {
        firstWord = *payload;
        payload++;
    }

    payloadSize--;

    for (i = 1; i < 4; i++)
    {
        if (payloadSize > 0)
        {
            firstWord |= ((*payload) << (i << 3U));
            payload++;
            payloadSize--;
        }
        else
        {
            break;
        }
    }

    base->DSI_HOST_TX_PAYLOAD = firstWord;

    /* Write the payload to the FIFO. */
    for (i = 0; i < payloadSize / 4U; i++)
    {
        base->DSI_HOST_TX_PAYLOAD = *(uint32_t *)payload;
        payload += 4U;
    }

    /* Write the remaining data. */
    switch (payloadSize & 0x03U)
    {
        case 3:
            base->DSI_HOST_TX_PAYLOAD = ((uint32_t)payload[2] << 16U) | ((uint32_t)payload[1] << 8U) | payload[0];
            break;
        case 2:
            base->DSI_HOST_TX_PAYLOAD = ((uint32_t)payload[1] << 8U) | payload[0];
            break;
        case 1:
            base->DSI_HOST_TX_PAYLOAD = payload[0];
            break;
        default:
            break;
    }
}

static status_t DSI_PrepareApbTransfer(MIPI_DSI_HOST_Type *base, dsi_transfer_t *xfer)
{
    /* The receive data size should be smaller than the RX FIRO. */
    assert(xfer->rxDataSize <= FSL_DSI_RX_MAX_PAYLOAD_BYTE);
    assert(xfer->txDataSize <= FSL_DSI_TX_MAX_PAYLOAD_BYTE);

    uint8_t txDataIndex;
    uint16_t wordCount;
    uint32_t intFlags1, intFlags2;
    uint32_t txDataSize;

    if (xfer->rxDataSize > 2)
    {
        return kStatus_DSI_NotSupported;
    }

    if (xfer->rxDataSize)
    {
        xfer->flags |= kDSI_TransferPerformBTA;
    }

    /* ========================== Prepare TX. ========================== */
    /* If xfer->sendDscCmd is true, then the DSC command is not included in the
       xfer->txData, but specified by xfer->dscCmd.
     */
    txDataSize = xfer->sendDscCmd ? xfer->txDataSize + 1U : xfer->txDataSize;

    /* Short packet. */
    if (txDataSize <= 2U)
    {
        if (0U == txDataSize)
        {
            wordCount = 0U;
        }
        else
        {
            txDataIndex = 0;

            if (xfer->sendDscCmd)
            {
                wordCount = xfer->dscCmd;
            }
            else
            {
                wordCount = xfer->txData[txDataIndex++];
            }

            if (2U == txDataSize)
            {
                wordCount |= ((uint32_t)xfer->txData[txDataIndex] << 8U);
            }
        }
    }
    /* Long packet. */
    else
    {
        wordCount = (uint16_t)txDataSize;
        DSI_WriteApbTxPayloadExt(base, xfer->txData, xfer->txDataSize, xfer->sendDscCmd, xfer->dscCmd);
    }

    DSI_SetApbPacketControl(base, wordCount, xfer->virtualChannel, xfer->txDataType, xfer->flags);

    /* Clear the interrupt flags set by previous transfer. */
    DSI_GetAndClearInterruptStatus(base, &intFlags1, &intFlags2);

    return kStatus_Success;
}

/*!
 * brief Read the long APB packet payload.
 *
 * Read the long packet payload from RX FIFO. This function reads directly but
 * does not check the RX FIFO status. Upper layer should make sure there are
 * available data.
 *
 * param base MIPI DSI host peripheral base address.
 * param payload Pointer to the payload.
 * param payloadSize Payload size in byte.
 */
void DSI_ReadApbRxPayload(MIPI_DSI_HOST_Type *base, uint8_t *payload, uint16_t payloadSize)
{
    uint32_t tmp;

    for (uint32_t i = 0; i < payloadSize / 4U; i++)
    {
        tmp        = base->DSI_HOST_PKT_RX_PAYLOAD;
        payload[0] = tmp & 0xFFU;
        payload[1] = (tmp >> 8U) & 0xFFU;
        payload[2] = (tmp >> 16U) & 0xFFU;
        payload[3] = (tmp >> 24U) & 0xFFU;
        payload += 4U;
    }

    /* Read out the remaining data. */
    if (payloadSize & 0x03U)
    {
        tmp = base->DSI_HOST_PKT_RX_PAYLOAD;

        for (uint32_t i = 0; i < (payloadSize & 0x3U); i++)
        {
            payload[i] = tmp & 0xFFU;
            tmp >>= 8U;
        }
    }
}

/*!
 * brief APB data transfer using blocking method.
 *
 * Perform APB data transfer using blocking method. This function waits until all
 * data send or received, or timeout happens.
 *
 * param base MIPI DSI host peripheral base address.
 * param xfer Pointer to the transfer structure.
 * retval kStatus_Success Data transfer finished with no error.
 * retval kStatus_Timeout Transfer failed because of timeout.
 * retval kStatus_DSI_RxDataError RX data error, user could use ref DSI_GetRxErrorStatus
 * to check the error details.
 * retval kStatus_DSI_ErrorReportReceived Error Report packet received, user could use
 *        ref DSI_GetAndClearHostStatus to check the error report status.
 * retval kStatus_DSI_NotSupported Transfer format not supported.
 * retval kStatus_DSI_Fail Transfer failed for other reasons.
 */
status_t DSI_TransferBlocking(MIPI_DSI_HOST_Type *base, dsi_transfer_t *xfer)
{
    status_t status;
    uint32_t intFlags1Old, intFlags2Old;
    uint32_t intFlags1New, intFlags2New;

    /* Wait for the APB state idle. */
    while (base->DSI_HOST_PKT_STATUS & kDSI_ApbNotIdle)
    {
    }

    status = DSI_PrepareApbTransfer(base, xfer);

    if (kStatus_Success != status)
    {
        return status;
    }

    DSI_SendApbPacket(base);

    /* Make sure the transfer is started. */
    while (1)
    {
        DSI_GetAndClearInterruptStatus(base, &intFlags1Old, &intFlags2Old);

        if (intFlags1Old & kDSI_InterruptGroup1ApbNotIdle)
        {
            break;
        }
    }

    /* Wait for transfer finished. */
    while (1)
    {
        /* Transfer completed. */
        if (!(base->DSI_HOST_PKT_STATUS & kDSI_ApbNotIdle))
        {
            break;
        }

        /* Time out. */
        if (base->DSI_HOST_RX_ERROR_STATUS & (kDSI_RxErrorHtxTo | kDSI_RxErrorLrxTo | kDSI_RxErrorBtaTo))
        {
            DSI_GetAndClearInterruptStatus(base, &intFlags1New, &intFlags2New);
            return kStatus_Timeout;
        }
    }

    DSI_GetAndClearInterruptStatus(base, &intFlags1New, &intFlags2New);

    return DSI_HandleResult(base, intFlags1Old | intFlags1New, intFlags2Old | intFlags2New, xfer);
}

static status_t DSI_HandleResult(MIPI_DSI_HOST_Type *base, uint32_t intFlags1, uint32_t intFlags2, dsi_transfer_t *xfer)
{
    uint32_t rxPktHeader;

    /* If hardware detect timeout. */
    if ((kDSI_InterruptGroup1HtxTo | kDSI_InterruptGroup1LrxTo | kDSI_InterruptGroup1BtaTo) & intFlags1)
    {
        return kStatus_Timeout;
    }

    /* If received data error. */
    if ((kDSI_InterruptGroup2EccMultiBit | kDSI_InterruptGroup2CrcError) & intFlags2)
    {
        return kStatus_DSI_RxDataError;
    }

    /* If BTA is performed. */
    if (xfer->flags & kDSI_TransferPerformBTA)
    {
        if (intFlags1 & DSI_INT_STATUS_ERROR_REPORT_MASK)
        {
            return kStatus_DSI_ErrorReportReceived;
        }

        if (kDSI_InterruptGroup1ApbRxHeaderReceived & intFlags1)
        {
            rxPktHeader = DSI_GetRxPacketHeader(base);

            /* If received error report. */
            if (kDSI_RxDataAckAndErrorReport == DSI_GetRxPacketType(rxPktHeader))
            {
                return kStatus_DSI_ErrorReportReceived;
            }
            else
            {
                /* Only handle short packet, long packet is not supported currently. */
                xfer->rxData[0] = rxPktHeader & 0xFFU;

                if (2U == xfer->rxDataSize)
                {
                    xfer->rxData[1] = (rxPktHeader >> 8U) & 0xFFU;
                }

                return kStatus_Success;
            }
        }

        return kStatus_Success;
    }
    else
    {
        /* Tx Done. */
        if (kDSI_InterruptGroup1ApbTxDone & intFlags1)
        {
            return kStatus_Success;
        }
    }

    return kStatus_Fail;
}

/*!
 * brief Create the MIPI DSI handle.
 *
 * This function initializes the MIPI DSI handle which can be used for other transactional APIs.
 *
 * param base MIPI DSI host peripheral base address.
 * param handle Handle pointer.
 * param callback Callback function.
 * param userData User data.
 */
status_t DSI_TransferCreateHandle(MIPI_DSI_HOST_Type *base,
                                  dsi_handle_t *handle,
                                  dsi_callback_t callback,
                                  void *userData)
{
    assert(handle);

    uint8_t instance = DSI_GetInstance(base);

    /* Zero the handle */
    memset(handle, 0, sizeof(*handle));

    /* Initialize the handle */
    s_dsiHandle[instance] = handle;
    handle->callback      = callback;
    handle->userData      = userData;
    handle->isBusy        = false;
    s_dsiIsr              = DSI_TransferHandleIRQ;

#if defined(MIPI_DSI_HOST_IRQS)
    /* Enable interrupt in NVIC. */
    EnableIRQ(s_dsiIRQ[instance]);
#endif

    return kStatus_Success;
}

/*!
 * brief APB data transfer using interrupt method.
 *
 * Perform APB data transfer using interrupt method, when transfer finished,
 * upper layer could be informed through callback function.
 *
 * param base MIPI DSI host peripheral base address.
 * param handle pointer to dsi_handle_t structure which stores the transfer state.
 * param xfer Pointer to the transfer structure.
 *
 * retval kStatus_Success Data transfer started successfully.
 * retval kStatus_DSI_Busy Failed to start transfer because DSI is busy with pervious transfer.
 * retval kStatus_DSI_NotSupported Transfer format not supported.
 */
status_t DSI_TransferNonBlocking(MIPI_DSI_HOST_Type *base, dsi_handle_t *handle, dsi_transfer_t *xfer)
{
    status_t status;

    if ((handle->isBusy) || (base->DSI_HOST_PKT_STATUS & kDSI_ApbNotIdle))
    {
        return kStatus_DSI_Busy;
    }

    handle->xfer = *xfer;

    status = DSI_PrepareApbTransfer(base, &handle->xfer);

    if (kStatus_Success != status)
    {
        return status;
    }

    DSI_SendApbPacket(base);
    handle->isBusy = true;

    /* Enable the interrupts. */
    if (handle->xfer.flags & kDSI_TransferPerformBTA)
    {
        DSI_EnableInterrupts(base,
                             DSI_INT_STATUS_TRIGGER_MASK | kDSI_InterruptGroup1ApbRxHeaderReceived |
                                 kDSI_InterruptGroup1ApbRxPacketReceived | kDSI_InterruptGroup1BtaTo |
                                 kDSI_InterruptGroup1LrxTo | kDSI_InterruptGroup1HtxTo |
                                 kDSI_InterruptGroup1AckTriggerReceived,
                             kDSI_InterruptGroup2EccMultiBit | kDSI_InterruptGroup2CrcError);
    }
    else
    {
        DSI_EnableInterrupts(base, kDSI_InterruptGroup1ApbTxDone | kDSI_InterruptGroup1HtxTo, 0U);
    }

    return kStatus_Success;
}

/*!
 * brief Abort current APB data transfer.
 *
 * param base MIPI DSI host peripheral base address.
 * param handle pointer to dsi_handle_t structure which stores the transfer state.
 */
void DSI_TransferAbort(MIPI_DSI_HOST_Type *base, dsi_handle_t *handle)
{
    assert(handle);

    if (handle->isBusy)
    {
        /* Disable the interrupts. */
        DSI_DisableInterrupts(base,
                              kDSI_InterruptGroup1ApbTxDone | DSI_INT_STATUS_TRIGGER_MASK |
                                  DSI_INT_STATUS_ERROR_REPORT_MASK | kDSI_InterruptGroup1ApbRxHeaderReceived |
                                  kDSI_InterruptGroup1ApbRxPacketReceived | kDSI_InterruptGroup1BtaTo |
                                  kDSI_InterruptGroup1LrxTo | kDSI_InterruptGroup1HtxTo,
                              kDSI_InterruptGroup2EccMultiBit | kDSI_InterruptGroup2CrcError);

        /* Reset transfer info. */
        memset(&handle->xfer, 0, sizeof(handle->xfer));

        /* Reset the state to idle. */
        handle->isBusy = false;
    }
}

/*!
 * @brief Interrupt handler for the DSI.
 *
 * @param base MIPI DSI host peripheral base address.
 * @param handle pointer to dsi_handle_t structure which stores the transfer state.
 */
/*!
 * brief Interrupt handler for the DSI.
 *
 * param base MIPI DSI host peripheral base address.
 * param handle pointer to dsi_handle_t structure which stores the transfer state.
 */
void DSI_TransferHandleIRQ(MIPI_DSI_HOST_Type *base, dsi_handle_t *handle)
{
    assert(handle);

    status_t status;
    uint32_t intFlags1, intFlags2;
    uint32_t timeout;

    /* If no transfer in progress, return directly. */
    if (!handle->isBusy)
    {
        return;
    }

    /* Make sure the transfer is completed. */
    timeout = FSL_MIPI_DSI_IDLE_TIMEOUT;
    while (timeout--)
    {
        if (!(base->DSI_HOST_PKT_STATUS & kDSI_ApbNotIdle))
        {
            break;
        }
    }

    if (!timeout)
    {
        DSI_TransferAbort(base, handle);
        status = kStatus_Timeout;
    }
    else
    {
        /* Disable the interrupts. */
        DSI_DisableInterrupts(base,
                              kDSI_InterruptGroup1ApbTxDone | DSI_INT_STATUS_TRIGGER_MASK |
                                  DSI_INT_STATUS_ERROR_REPORT_MASK | kDSI_InterruptGroup1ApbRxHeaderReceived |
                                  kDSI_InterruptGroup1ApbRxPacketReceived | kDSI_InterruptGroup1BtaTo |
                                  kDSI_InterruptGroup1LrxTo | kDSI_InterruptGroup1HtxTo,
                              kDSI_InterruptGroup2EccMultiBit | kDSI_InterruptGroup2CrcError);

        DSI_GetAndClearInterruptStatus(base, &intFlags1, &intFlags2);

        status         = DSI_HandleResult(base, intFlags1, intFlags2, &handle->xfer);
        handle->isBusy = false;
    }

    if (handle->callback)
    {
        handle->callback(base, handle, status, handle->userData);
    }
}

#if defined(MIPI_DSI_HOST)
void MIPI_IRQHandler(void)
{
    s_dsiIsr(MIPI_DSI_HOST, s_dsiHandle[0]);
}
#endif

#if defined(MIPI_DSI_HOST0)
void MIPI_DSI0_INT_OUT_IRQHandler(void)
{
    s_dsiIsr(MIPI_DSI_HOST0, s_dsiHandle[0]);
}
#endif

#if defined(MIPI_DSI_HOST1)
void MIPI_DSI1_INT_OUT_IRQHandler(void)
{
    s_dsiIsr(MIPI_DSI_HOST1, s_dsiHandle[1]);
}
#endif
