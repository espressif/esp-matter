/*
 * Copyright (c) 2019, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_LCDIF_H_
#define _FSL_LCDIF_H_

#include "fsl_common.h"

/*!
 * @addtogroup lcdif_driver
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_LCDIF_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/*! @brief Construct the cursor color, every element should be in the range of 0 ~ 255. */
#define LCDIF_MAKE_CURSOR_COLOR(r, g, b) (((r) << 16U) | ((g) << 8U) | ((b) << 0U))

/*! @brief Construct the gamma value set to LCDIF gamma table, every element should be in the range of 0~255. */
#define LCDIF_MAKE_GAMMA_VALUE(r, g, b) (((r) << 16U) | ((g) << 8U) | ((b) << 0U))

/*! @brief The frame buffer should be 128 byte aligned. */
#define LCDIF_FB_ALIGN 128U

/*! @brief Gamma index max value. */
#define LCDIF_GAMMA_INDEX_MAX 256U

/*! @brief The cursor size is 32 x 32 */
#define LCDIF_CURSOR_SIZE 32U

#ifndef LCDIF_FRAMEBUFFERCONFIG0_OUTPUT_MASK
#define LCDIF_FRAMEBUFFERCONFIG0_OUTPUT_MASK (1U << 8U)
#endif

/*!
 * @brief LCDIF signal polarity flags
 */
enum _lcdif_polarity_flags
{
    kLCDIF_VsyncActiveLow            = 0U,         /*!< VSYNC active low. */
    kLCDIF_VsyncActiveHigh           = (1U << 0U), /*!< VSYNC active high. */
    kLCDIF_HsyncActiveLow            = 0U,         /*!< HSYNC active low. */
    kLCDIF_HsyncActiveHigh           = (1U << 1U), /*!< HSYNC active high. */
    kLCDIF_DataEnableActiveLow       = 0U,         /*!< Data enable line active low. */
    kLCDIF_DataEnableActiveHigh      = (1U << 2U), /*!< Data enable line active high. */
    kLCDIF_DriveDataOnFallingClkEdge = 0U, /*!< Drive data on falling clock edge, capture data on rising clock edge. */
    kLCDIF_DriveDataOnRisingClkEdge  = (1U << 3U), /*!< Drive data on falling
                                                                         clock edge, capture data
                                                                         on rising clock edge. */
};

/*! @brief LCDIF DPI output format. */
typedef enum _lcdif_output_format
{
    kLCDIF_Output16BitConfig1 = 0U, /*!< 16-bit configuration 1. RGB565: XXXXXXXX_RRRRRGGG_GGGBBBBB. */
    kLCDIF_Output16BitConfig2 = 1U, /*!< 16-bit configuration 2. RGB565: XXXRRRRR_XXGGGGGG_XXXBBBBB. */
    kLCDIF_Output16BitConfig3 = 2U, /*!< 16-bit configuration 3. RGB565: XXRRRRRX_XXGGGGGG_XXBBBBBX. */
    kLCDIF_Output18BitConfig1 = 3U, /*!< 18-bit configuration 1. RGB666: XXXXXXRR_RRRRGGGG_GGBBBBBB. */
    kLCDIF_Output18BitConfig2 = 4U, /*!< 18-bit configuration 2. RGB666: XXRRRRRR_XXGGGGGG_XXBBBBBB. */
    kLCDIF_Output24Bit        = 5U, /*!< 24-bit. */
} lcdif_output_format_t;

/*! @brief Configuration for LCDIF module to work in DBI mode. */
typedef struct _lcdif_dpi_config
{
    uint16_t panelWidth;          /*!< Display panel width, pixels per line. */
    uint16_t panelHeight;         /*!< Display panel height, how many lines per panel. */
    uint8_t hsw;                  /*!< HSYNC pulse width. */
    uint8_t hfp;                  /*!< Horizontal front porch. */
    uint8_t hbp;                  /*!< Horizontal back porch. */
    uint8_t vsw;                  /*!< VSYNC pulse width. */
    uint8_t vfp;                  /*!< Vrtical front porch. */
    uint8_t vbp;                  /*!< Vertical back porch. */
    uint32_t polarityFlags;       /*!< OR'ed value of @ref _lcdif_polarity_flags, used to contol the signal polarity. */
    lcdif_output_format_t format; /*!< DPI output format. */
} lcdif_dpi_config_t;

/*! @brief LCDIF frame buffer pixel format. */
typedef enum _lcdif_fb_format
{
    kLCDIF_PixelFormatXRGB444  = 1, /*!< XRGB444. */
    kLCDIF_PixelFormatXRGB1555 = 2, /*!< XRGB1555. */
    kLCDIF_PixelFormatRGB565   = 3, /*!< RGB565. */
    kLCDIF_PixelFormatXRGB8888 = 4, /*!< XRGB8888. */
} lcdif_fb_format_t;

/*! @brief LCDIF frame buffer configuration. */
typedef struct _lcdif_fb_config
{
    bool enable;              /*!< Enable the frame buffer output. */
    bool enableGamma;         /*!< Enable the gamma correction. */
    lcdif_fb_format_t format; /*!< Frame buffer pixel format. */
} lcdif_fb_config_t;

/*! @brief LCDIF interrupt and status. */
enum _lcdif_interrupt
{
    kLCDIF_Display0FrameDoneInterrupt =
        LCDIF_DISPLAYINTR_DISP0_MASK, /*!< The last pixel of visible area in frame is shown. */
};

/*! @brief LCDIF cursor format. */
typedef enum _lcdif_cursor_format
{
    kLCDIF_CursorMasked   = 1, /*!< Masked format. */
    kLCDIF_CursorARGB8888 = 2, /*!< ARGB8888. */
} lcdif_cursor_format_t;

/*! @brief LCDIF cursor configuration. */
typedef struct _lcdif_cursor_config
{
    bool enable;                  /*!< Enable the cursor or not. */
    lcdif_cursor_format_t format; /*!< Cursor format. */
    uint8_t hotspotOffsetX;       /*!< Offset of the hotspot to top left point, range 0 ~ 31 */
    uint8_t hotspotOffsetY;       /*!< Offset of the hotspot to top left point, range 0 ~ 31 */
} lcdif_cursor_config_t;

/*!
 * @brief LCDIF dither configuration.
 *
 * 1. Decide which bit of pixel color to enhance. This is configured by the
 * @ref redSize, @ref greenSize, and @ref blueSize. For example, setting redSize=6
 * means it is the 6th bit starting from the MSB that we want to enhance, in other words,
 * it is the RedColor[2]bit from RedColor[7:0]. greenSize and blueSize function
 * in the same way.
 *
 * 2. Create the look-up table.
 *  a. The Look-Up Table includes 16 entries, 4 bits for each.
 *  b. The Look-Up Table provides a value U[3:0] through the index X[1:0] and Y[1:0].
 *  c. The color value RedColor[3:0] is used to compare with this U[3:0].
 *  d. If RedColor[3:0] > U[3:0], and RedColor[7:2] is not 6'b111111, then the
 *     final color value is: NewRedColor = RedColor[7:2] + 1'b1.
 *  e. If RedColor[3:0] <= U[3:0], then NewRedColor = RedColor[7:2].
 */
typedef struct _lcdif_dither_config
{
    bool enable;       /*!< Enable or not. */
    uint8_t redSize;   /*!< Red color size, valid region 4-8. */
    uint8_t greenSize; /*!< Green color size, valid region 4-8. */
    uint8_t blueSize;  /*!< Blue color size, valid region 4-8. */
    uint32_t low;      /*!< Low part of the look up table. */
    uint32_t high;     /*!< High part of the look up table. */
} lcdif_dither_config_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and deinitialization
 * @{
 */

/*!
 * @brief Initialize the LCDIF.
 *
 * This function initializes the LCDIF to work.
 *
 * @param base LCDIF peripheral base address.
 *
 * @retval kStatus_Success Initialize successfully.
 */
status_t LCDIF_Init(LCDIF_Type *base);

/*!
 * @brief De-initialize the LCDIF.
 *
 * This function disables the LCDIF peripheral clock.
 *
 * @param base LCDIF peripheral base address.
 */
void LCDIF_Deinit(LCDIF_Type *base);

/* @} */

/*!
 * @brief Get the default configuration for to initialize the LCDIF.
 *
 * The default configuration value is:
 *
 * @code
    config->panelWidth = 0;
    config->panelHeight = 0;
    config->hsw = 0;
    config->hfp = 0;
    config->hbp = 0;
    config->vsw = 0;
    config->vfp = 0;
    config->vbp = 0;
    config->polarityFlags = kLCDIF_VsyncActiveLow | kLCDIF_HsyncActiveLow | kLCDIF_DataEnableActiveHigh |
 kLCDIF_DriveDataOnFallingClkEdge; config->format = kLCDIF_Output24Bit;
   @endcode
 *
 * @param config Pointer to the LCDIF configuration.
 */
void LCDIF_DpiModeGetDefaultConfig(lcdif_dpi_config_t *config);

/*!
 * @brief Initialize the LCDIF to work in DPI mode.
 *
 * This function configures the LCDIF DPI display.
 *
 * @param base LCDIF peripheral base address.
 * @param displayIndex Display index.
 * @param config Pointer to the configuration structure.
 *
 * @retval kStatus_Success Initialize successfully.
 * @retval kStatus_InvalidArgument Initialize failed because of invalid argument.
 */
status_t LCDIF_DpiModeSetConfig(LCDIF_Type *base, uint8_t displayIndex, const lcdif_dpi_config_t *config);

/*!
 * @name Frame buffer
 * @{
 */

/*!
 * @brief Configure the LCDIF frame buffer.
 *
 * @param base LCDIF peripheral base address.
 * @param fbIndex Frame buffer index.
 * @param config Pointer to the configuration structure.
 */
void LCDIF_SetFrameBufferConfig(LCDIF_Type *base, uint8_t fbIndex, const lcdif_fb_config_t *config);

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
void LCDIF_FrameBufferGetDefaultConfig(lcdif_fb_config_t *config);

/*!
 * @brief Set the frame buffer to LCDIF.
 *
 * @param base LCDIF peripheral base address.
 * @param fbIndex Frame buffer index.
 * @param address Frame buffer address.
 * @note The address must be 128 bytes aligned.
 */
static inline void LCDIF_SetFrameBufferAddr(LCDIF_Type *base, uint8_t fbIndex, uint32_t address)
{
    /* The frame buffer address and stride must be 128 bytes aligned. */
    assert(0U == (address & (LCDIF_FB_ALIGN - 1U)));

    base->FRAMEBUFFERADDRESS0 = address;
}

/*!
 * @brief Set the frame buffer stride.
 *
 * @param base LCDIF peripheral base address.
 * @param fbIndex Frame buffer index.
 * @param strideBytes The stride in byte.
 */
void LCDIF_SetFrameBufferStride(LCDIF_Type *base, uint8_t fbIndex, uint32_t strideBytes);

/* @} */

/*!
 * @name Dither
 * @{
 */

/*!
 * @brief Set the dither configuration.
 *
 * @param base LCDIF peripheral base address.
 * @param displayIndex Index to configure.
 * @param config Pointer to the configuration structure.
 */
void LCDIF_SetDitherConfig(LCDIF_Type *base, uint8_t displayIndex, const lcdif_dither_config_t *config);

/* @} */

/*!
 * @name Gamma correction
 * @{
 */

/*!
 * @brief Set the gamma translation values to the LCDIF gamma table.
 *
 * @param base LCDIF peripheral base address.
 * @param fbIndex The frame buffer index.
 * @param startIndex Start index in the gamma table that the value will be set to.
 * @param gamma The gamma values to set to the gamma table in LCDIF, could be defined using LCDIF_MAKE_GAMMA_VALUE.
 * @param gammaLen The length of the @p gamma.
 */
void LCDIF_SetGammaData(
    LCDIF_Type *base, uint8_t fbIndex, uint16_t startIndex, const uint32_t *gamma, uint16_t gammaLen);

/* @} */

/*!
 * @name Interrupts
 *
 * The interrupt must be enabled, otherwise the interrupt flags will not assert.
 *
 * @{
 */

/*!
 * @brief Enables LCDIF interrupt requests.
 *
 * @param base LCDIF peripheral base address.
 * @param mask The interrupts to enable, pass in as OR'ed value of @ref _lcdif_interrupt.
 */
static inline void LCDIF_EnableInterrupts(LCDIF_Type *base, uint32_t mask)
{
    base->DISPLAYINTRENABLE |= mask;
}

/*!
 * @brief Disable LCDIF interrupt requests.
 *
 * @param base LCDIF peripheral base address.
 * @param mask The interrupts to disable, pass in as OR'ed value of @ref _lcdif_interrupt.
 */
static inline void LCDIF_DisableInterrupts(LCDIF_Type *base, uint32_t mask)
{
    base->DISPLAYINTRENABLE &= ~mask;
}

/*!
 * @brief Get and clear LCDIF interrupt pending status.
 *
 * @param base LCDIF peripheral base address.
 * @return The interrupt pending status.
 *
 * @note The interrupt must be enabled, otherwise the interrupt flags will not assert.
 */
static inline uint32_t LCDIF_GetAndClearInterruptPendingFlags(LCDIF_Type *base)
{
    return base->DISPLAYINTR;
}

/* @} */

/*!
 * @name Cursor
 *
 * Top-left point and Hot spot are two different cursor point.
 *
 *   Top-left point is used as the base address for the cursor.
 *   Hot spot is used in a search for a corresponding screen coordinate when
 *   a selection is made, such as when the "Enter" key or left mouse button
 *   is pushed.
 *
 * @verbatim

     Top-left point
          +-------------------------------+
          |                               |
          |   Hot spot                    |
          |      +-------                 |
          |      |    \                   |
          |      | \   \                  |
          |      |  \   \                 |
          |          \   \                |
          |           \   \               |
          |            \   \              |
          |             \   \             |
          |              \                |
          |                               |
          +-------------------------------+

   @endverbatim
 *
 * For format masked, one cursor pixel is 2bits. 32x32 cursor pixels have 32
 * cursor color rows. Each cursor color row is 64bits.
 *
 * cursorColorRow_H[31:0] = colorRow[63:32]
 * cursorColorRow_L[31:0] = colorRow[31:0]
 * xorCursor = cursorColorRow_H[cursorXPos[4:0]]
 * andCursor = cursorColorRow_L[cursorXPos[4:0]]
 *
 * The output cursor color is:
 *
 *  andCursor    xorCursor          cursor color
 *      0           0           Background register color
 *      0           1           Foreground register color
 *      1           0           Frame buffer pixel color
 *      1           1           Invert frame buffer pixel color
 *
 * @{
 */

/*!
 * @brief Get the hardware cursor default configuration
 *
 * The default configuration values are:
 *
 * @code
    config->enable = true;
    config->format = kLCDIF_CursorMasked;
    config->hotspotOffsetX = 0;
    config->hotspotOffsetY = 0;
   @endcode
 *
 * @param config Pointer to the hardware cursor configuration structure.
 */
void LCDIF_CursorGetDefaultConfig(lcdif_cursor_config_t *config);

/*!
 * @brief Configure the cursor.
 *
 * @param base LCDIF peripheral base address.
 * @param x X coordinate of the hotspot, range 0 ~ 8191.
 * @param y Y coordinate of the hotspot, range 0 ~ 8191.
 */
void LCDIF_SetCursorConfig(LCDIF_Type *base, const lcdif_cursor_config_t *config);

/*!
 * @brief Set the cursor hotspot postion
 *
 * @param base LCDIF peripheral base address.
 * @param x X coordinate of the hotspot, range 0 ~ 8191.
 * @param y Y coordinate of the hotspot, range 0 ~ 8191.
 */
static inline void LCDIF_SetCursorHotspotPosition(LCDIF_Type *base, uint16_t x, uint16_t y)
{
    base->CURSORLOCATION = (y << LCDIF_CURSORLOCATION_Y_SHIFT) | (x << LCDIF_CURSORLOCATION_X_SHIFT);
}

/*!
 * @brief Set the cursor memory address.
 *
 * @param base LCDIF peripheral base address.
 * @param address Memory address.
 */
static inline void LCDIF_SetCursorBufferAddress(LCDIF_Type *base, uint32_t address)
{
    base->CURSORADDRESS = address;
}

/*!
 * @brief Set the cursor color
 *
 * @param base LCDIF peripheral base address.
 * @param background  Background color, could be defined use @ref LCDIF_MAKE_CURSOR_COLOR
 * @param foreground  Foreground color, could be defined use @ref LCDIF_MAKE_CURSOR_COLOR
 */
void LCDIF_SetCursorColor(LCDIF_Type *base, uint32_t background, uint32_t foreground);

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* _FSL_LCDIF_H_ */
