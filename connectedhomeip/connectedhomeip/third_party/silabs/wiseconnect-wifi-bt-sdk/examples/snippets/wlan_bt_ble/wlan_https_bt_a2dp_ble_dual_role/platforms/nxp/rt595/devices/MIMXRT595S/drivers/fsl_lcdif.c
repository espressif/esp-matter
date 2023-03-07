/*
 * Copyright (c) 2019, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_lcdif.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lcdif"
#endif

#define LCDIF_ALIGN_ADDR(addr, align) ((addr + align - 1U) & ~(align - 1U))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * brief Get the instance from the base address
 *
 * param base LCDIF peripheral base address
 *
 * return The LCDIF module instance
 */
static uint32_t LCDIF_GetInstance(LCDIF_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! brief Pointers to LCDIF bases for each instance. */
static LCDIF_Type *const s_lcdifBases[] = LCDIF_BASE_PTRS;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! brief Pointers to LCDIF clocks for each LCDIF submodule. */
static const clock_ip_name_t s_lcdifClocks[] = LCDIF_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/
static uint32_t LCDIF_GetInstance(LCDIF_Type *base)
{
    uint32_t instance;

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_lcdifBases); instance++)
    {
        if (s_lcdifBases[instance] == base)
        {
            break;
        }
    }

    assert(instance < ARRAY_SIZE(s_lcdifBases));

    return instance;
}

/*!
 * brief Initialize the LCDIF.
 *
 * This function initializes the LCDIF to work.
 *
 * param base LCDIF peripheral base address.
 *
 * retval kStatus_Success Initialize successfully.
 */
status_t LCDIF_Init(LCDIF_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = LCDIF_GetInstance(base);
    CLOCK_EnableClock(s_lcdifClocks[instance]);
#endif

    base->FRAMEBUFFERCONFIG0 = 0;
    /* Clear interrupt status and disable interrupt. */
    base->DISPLAYINTRENABLE = 0;
    (void)(base->DISPLAYINTR);

    return kStatus_Success;
}

/*!
 * brief De-initialize the LCDIF.
 *
 * This function disables the LCDIF peripheral clock.
 *
 * param base LCDIF peripheral base address.
 */
void LCDIF_Deinit(LCDIF_Type *base)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    uint32_t instance = LCDIF_GetInstance(base);
    CLOCK_DisableClock(s_lcdifClocks[instance]);
#endif
}

/*!
 * brief Get the default configuration for to initialize the LCDIF.
 *
 * The default configuration value is:
 *
 * code
 config->panelWidth = 0;
 config->panelHeight = 0;
 config->hsw = 0;
 config->hfp = 0;
 config->hbp = 0;
 config->vsw = 0;
 config->vfp = 0;
 config->vbp = 0;
 config->polarityFlags = kLCDIF_VsyncActiveLow | kLCDIF_HsyncActiveLow | kLCDIF_DataEnableActiveHigh |
 kLCDIF_DriveDataOnFallingClkEdge; config->format = kLCDIF_Output24Bit; endcode
 *
 * param config Pointer to the LCDIF configuration.
 */
void LCDIF_DpiModeGetDefaultConfig(lcdif_dpi_config_t *config)
{
    assert(config);

    config->panelWidth  = 0;
    config->panelHeight = 0;
    config->hsw         = 0;
    config->hfp         = 0;
    config->hbp         = 0;
    config->vsw         = 0;
    config->vfp         = 0;
    config->vbp         = 0;
    config->polarityFlags =
        kLCDIF_VsyncActiveLow | kLCDIF_HsyncActiveLow | kLCDIF_DataEnableActiveHigh | kLCDIF_DriveDataOnFallingClkEdge;
    config->format = kLCDIF_Output24Bit;
}

/*!
 * @brief Initialize the LCDIF to work in DPI mode.
 *
 * This function configures the LCDIF DPI display.
 *
 * param base LCDIF peripheral base address.
 * param displayIndex Display index.
 * param config Pointer to the configuration structure.
 *
 * retval kStatus_Success Initialize successfully.
 * retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t LCDIF_DpiModeSetConfig(LCDIF_Type *base, uint8_t displayIndex, const lcdif_dpi_config_t *config)
{
    assert(config);

    uint32_t regHsync;
    uint32_t regVsync;
    uint32_t regPanelConfig;

    /* Reset the frame buffer. */
    base->FRAMEBUFFERCONFIG0 = 0;

    /* Change to DPI mode. */
    base->DBICONFIG0 &= ~LCDIF_DBICONFIG0_BUS_OUTPUT_SEL_MASK;

    base->DPICONFIG0 = config->format;

    base->HDISPLAY0 = (config->panelWidth << LCDIF_HDISPLAY0_DISPLAY_END_SHIFT) |
                      ((config->panelWidth + config->hsw + config->hfp + config->hbp) << LCDIF_HDISPLAY0_TOTAL_SHIFT);

    base->VDISPLAY0 = (config->panelHeight << LCDIF_VDISPLAY0_DISPLAY_END_SHIFT) |
                      ((config->panelHeight + config->vsw + config->vfp + config->vbp) << LCDIF_VDISPLAY0_TOTAL_SHIFT);

    /* HSYNC */
    regHsync = ((config->panelWidth + config->hfp) << LCDIF_HSYNC0_START_SHIFT) |
               ((config->panelWidth + config->hfp + config->hsw) << LCDIF_HSYNC0_END_SHIFT) | LCDIF_HSYNC0_PULSE_MASK;

    if (kLCDIF_HsyncActiveHigh != (config->polarityFlags & kLCDIF_HsyncActiveHigh))
    {
        regHsync |= LCDIF_HSYNC0_POLARITY_MASK;
    }

    base->HSYNC0 = regHsync;

    /* VSYNC */
    regVsync = ((config->panelHeight + config->vfp) << LCDIF_VSYNC0_START_SHIFT) |
               ((config->panelHeight + config->vfp + config->vsw) << LCDIF_VSYNC0_END_SHIFT) | LCDIF_VSYNC0_PULSE_MASK;

    if (kLCDIF_VsyncActiveHigh != (config->polarityFlags & kLCDIF_VsyncActiveHigh))
    {
        regVsync |= LCDIF_VSYNC0_POLARITY_MASK;
    }

    base->VSYNC0 = regVsync;

    /* DE, Data, clock. */
    regPanelConfig = LCDIF_PANELCONFIG0_DE_MASK | LCDIF_PANELCONFIG0_DATA_ENABLE_MASK | LCDIF_PANELCONFIG0_CLOCK_MASK;

    if (kLCDIF_DataEnableActiveHigh != (kLCDIF_DataEnableActiveHigh & config->polarityFlags))
    {
        regPanelConfig |= LCDIF_PANELCONFIG0_DE_POLARITY_MASK;
    }

    if (kLCDIF_DriveDataOnRisingClkEdge == (kLCDIF_DriveDataOnRisingClkEdge & config->polarityFlags))
    {
        regPanelConfig |= LCDIF_PANELCONFIG0_CLOCK_POLARITY_MASK;
    }

    base->PANELCONFIG0 = regPanelConfig | LCDIF_PANELCONFIG0_SEQUENCING_MASK;

    return kStatus_Success;
}

/*!
 * @brief Get default frame buffer configuration.
 *
 * The default configuration is
 *   config->enable = true;
 *   config->enableGamma = false;
 *   config->format = kLCDIF_PixelFormatRGB565;
 *
 * @param config Pointer to the configuration structure.
 */
void LCDIF_FrameBufferGetDefaultConfig(lcdif_fb_config_t *config)
{
    assert(config != NULL);

    config->enable      = true;
    config->enableGamma = false;
    config->format      = kLCDIF_PixelFormatRGB565;
}

/*!
 * brief Configure the LCDIF frame buffer.
 *
 * param base LCDIF peripheral base address.
 * param fbIndex Frame buffer index.
 * param config Pointer to the configuration structure.
 */
void LCDIF_SetFrameBufferConfig(LCDIF_Type *base, uint8_t fbIndex, const lcdif_fb_config_t *config)
{
    assert(config);
    uint32_t reg;

    if (config->enable)
    {
        reg = LCDIF_FRAMEBUFFERCONFIG0_RESET_MASK | LCDIF_FRAMEBUFFERCONFIG0_OUTPUT_MASK | (uint32_t)config->format;
        if (config->enableGamma)
        {
            reg |= LCDIF_FRAMEBUFFERCONFIG0_GAMMA_MASK;
        }
        base->FRAMEBUFFERCONFIG0 = reg;
    }
    else
    {
        base->FRAMEBUFFERCONFIG0 = 0U;
    }
}

/*
 * @brief Set the frame buffer stride.
 *
 * @param base LCDIF peripheral base address.
 * @param fbIndex Frame buffer index.
 * @param strideBytes The stride in byte.
 */
void LCDIF_SetFrameBufferStride(LCDIF_Type *base, uint8_t fbIndex, uint32_t strideBytes)
{
    base->FRAMEBUFFERSTRIDE0 = LCDIF_ALIGN_ADDR(strideBytes, LCDIF_FB_ALIGN);
}

/*!
 * brief Set the dither configuration.
 *
 * param base LCDIF peripheral base address.
 * param displayIndex Index to configure.
 * param config Pointer to the configuration structure.
 */
void LCDIF_SetDitherConfig(LCDIF_Type *base, uint8_t displayIndex, const lcdif_dither_config_t *config)
{
    assert(config);

    if (config->enable)
    {
        base->DISPLAYDITHERTABLELOW0  = config->low;
        base->DISPLAYDITHERTABLEHIGH0 = config->high;

        base->DISPLAYDITHERCONFIG0 = ((uint32_t)config->redSize << LCDIF_DISPLAYDITHERCONFIG0_RED_SIZE_SHIFT) |
                                     ((uint32_t)config->greenSize << LCDIF_DISPLAYDITHERCONFIG0_GREEN_SIZE_SHIFT) |
                                     ((uint32_t)config->blueSize << LCDIF_DISPLAYDITHERCONFIG0_BLUE_SIZE_SHIFT) |
                                     LCDIF_DISPLAYDITHERCONFIG0_ENABLE_MASK;
    }
    else
    {
        base->DISPLAYDITHERCONFIG0    = 0U;
        base->DISPLAYDITHERTABLELOW0  = 0U;
        base->DISPLAYDITHERTABLEHIGH0 = 0U;
    }
}

/*!
 * brief Set the gamma translation values to the LCDIF gamma table.
 *
 * param base LCDIF peripheral base address.
 * param fbIndex The frame buffer index.
 * param startIndex Start index in the gamma table that the value will be set to.
 * param gamma The gamma values to set to the gamma table in LCDIF, could be defined using LCDIF_MAKE_GAMMA_VALUE.
 * param gammaLen The length of the p gamma.
 */
void LCDIF_SetGammaData(
    LCDIF_Type *base, uint8_t fbIndex, uint16_t startIndex, const uint32_t *gamma, uint16_t gammaLen)
{
    assert(startIndex + gammaLen <= LCDIF_GAMMA_INDEX_MAX);

    base->GAMMAINDEX0 = startIndex;

    while (gammaLen--)
    {
        base->GAMMADATA0 = *(gamma++);
    }
}

/*!
 * brief Get the hardware cursor default configuration
 *
 * The default configuration values are:
 *
 * code
    config->enable = true;
    config->format = kLCDIF_CursorMasked;
    config->hotspotOffsetX = 0;
    config->hotspotOffsetY = 0;
   endcode
 *
 * param config Pointer to the hardware cursor configuration structure.
 */
void LCDIF_CursorGetDefaultConfig(lcdif_cursor_config_t *config)
{
    assert(config);

    config->enable         = true;
    config->format         = kLCDIF_CursorMasked;
    config->hotspotOffsetX = 0;
    config->hotspotOffsetY = 0;
}

/*!
 * brief Configure the cursor.
 *
 * param base LCDIF peripheral base address.
 * param x X coordinate of the hotspot, range 0 ~ 8191.
 * param y Y coordinate of the hotspot, range 0 ~ 8191.
 */
void LCDIF_SetCursorConfig(LCDIF_Type *base, const lcdif_cursor_config_t *config)
{
    assert(config);

    uint32_t regConfig = 0U;

    if (config->enable)
    {
        regConfig |= (uint32_t)(config->format) << LCDIF_CURSORCONFIG_FORMAT_SHIFT;
        regConfig |= ((config->hotspotOffsetX << LCDIF_CURSORCONFIG_HOT_SPOT_X_SHIFT) |
                      (config->hotspotOffsetY << LCDIF_CURSORCONFIG_HOT_SPOT_Y_SHIFT));
    }

    base->CURSORCONFIG = regConfig;
}

/*!
 * brief Set the cursor color
 *
 * param base LCDIF peripheral base address.
 * param background  Background color, could be defined use ref LCDIF_MAKE_CURSOR_COLOR
 * param foreground  Foreground color, could be defined use ref LCDIF_MAKE_CURSOR_COLOR
 */
void LCDIF_SetCursorColor(LCDIF_Type *base, uint32_t background, uint32_t foreground)
{
    base->CURSORBACKGROUND = background;
    base->CURSORFOREGROUND = foreground;
}
