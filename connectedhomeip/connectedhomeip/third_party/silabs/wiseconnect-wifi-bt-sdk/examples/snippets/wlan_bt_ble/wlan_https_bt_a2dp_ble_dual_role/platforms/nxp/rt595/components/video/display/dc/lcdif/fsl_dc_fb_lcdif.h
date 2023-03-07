/*
 * Copyright (c) 2019, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DC_FB_LCDIF_H_
#define _FSL_DC_FB_LCDIF_H_

#include "fsl_dc_fb.h"
#include "fsl_lcdif.h"

/*!
 * @addtogroup dc_fb_lcdif
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define DC_FB_LCDIF_MAX_LAYER 1 /* Only support one layer currently. */
#define DC_FB_LCDIF_DEFAULT_PIXEL_FORMAT kVIDEO_PixelFormatRGB565
#define DC_FB_LCDIF_DEFAULT_PIXEL_FORMAT_LCDIF kLCDIF_PixelFormatRGB565
#define DC_FB_LCDIF_DEFAULT_BYTE_PER_PIXEL 2

/*! @brief Data for LCDIF display controller layer. */
typedef struct _dc_fb_lcdif_layer
{
    bool enabled;               /*!< The layer is enabled. */
    volatile bool framePending; /*!< New frame pending. */
    void *activeBuffer;         /*!< The frame buffer which is shown. */
    void *inactiveBuffer;       /*!< The frame buffer which will be shown. */
    lcdif_fb_config_t fbConfig; /*!< Frame buffer configuration. */
    dc_fb_callback_t callback;  /*!< Callback for buffer switch off. */
    void *cbParam;              /*!< Callback parameter. */
} dc_fb_lcdif_layer_t;

/*! @brief Data for LCDIF display controller driver handle. */
typedef struct _dc_fb_lcdif_handle
{
    LCDIF_Type *lcdif;                                 /*!< eLCDIF peripheral. */
    uint8_t initTimes;                                 /*!< How many times the DC is initialized. */
    uint16_t height;                                   /*!< Panel height. */
    uint16_t width;                                    /*!< Panel width. */
    dc_fb_lcdif_layer_t layers[DC_FB_LCDIF_MAX_LAYER]; /*!< Information of the layer. */
} dc_fb_lcdif_handle_t;

/*! @brief Configuration for LCDIF display controller driver handle. */
typedef struct _dc_fb_lcdif_config
{
    LCDIF_Type *lcdif;                  /*!< LCDIF peripheral. */
    uint16_t width;                     /*!< Width of the panel. */
    uint16_t height;                    /*!< Height of the panel. */
    uint16_t hsw;                       /*!< HSYNC pulse width. */
    uint16_t hfp;                       /*!< Horizontal front porch. */
    uint16_t hbp;                       /*!< Horizontal back porch. */
    uint16_t vsw;                       /*!< VSYNC pulse width. */
    uint16_t vfp;                       /*!< Vertical front porch. */
    uint16_t vbp;                       /*!< Vertical back porch. */
    uint32_t polarityFlags;             /*!< Control flags, OR'ed value of @ref _lcdif_polarity_flags. */
    lcdif_output_format_t outputFormat; /*!< DPI output format. */
} dc_fb_lcdif_config_t;

extern const dc_fb_ops_t g_dcFbOpsLcdif;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

status_t DC_FB_LCDIF_Init(const dc_fb_t *dc);
status_t DC_FB_LCDIF_Deinit(const dc_fb_t *dc);
status_t DC_FB_LCDIF_EnableLayer(const dc_fb_t *dc, uint8_t layer);
status_t DC_FB_LCDIF_DisableLayer(const dc_fb_t *dc, uint8_t layer);
status_t DC_FB_LCDIF_SetLayerConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo);
status_t DC_FB_LCDIF_GetLayerDefaultConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo);
status_t DC_FB_LCDIF_SetFrameBuffer(const dc_fb_t *dc, uint8_t layer, void *frameBuffer);
uint32_t DC_FB_LCDIF_GetProperty(const dc_fb_t *dc);
void DC_FB_LCDIF_SetCallback(const dc_fb_t *dc, uint8_t layer, dc_fb_callback_t callback, void *param);
void DC_FB_LCDIF_IRQHandler(const dc_fb_t *dc);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_DC_FB_LCDIF_H_ */
