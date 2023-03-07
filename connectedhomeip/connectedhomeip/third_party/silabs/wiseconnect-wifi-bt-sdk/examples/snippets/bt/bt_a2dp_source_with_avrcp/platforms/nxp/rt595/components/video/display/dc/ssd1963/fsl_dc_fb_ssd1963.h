/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_DC_FB_SSD1963_H_
#define _FSL_DC_FB_SSD1963_H_

#include "fsl_dc_fb.h"
#include "fsl_ssd1963.h"

/*!
 * @addtogroup dc_fb_ssd1963
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Only support one layer. To support multiple layers, the DBI transfer should
 * be protected. If writing to a layer is in progress, the writing to another layer
 * should be blocked until previous layer operation finished.
 */
#define DC_FB_SSD1963_MAX_LAYER 1

/* If the data bus is 16bit, only support RGB565/BGR565,
   if the data bus is 8bit, only support RGB888/BGR888.
 */
#if (16 == SSD1963_DATA_WITDH)
#define DC_FB_SSD1963_DEFAULT_PIXEL_FORMAT kVIDEO_PixelFormatRGB565
#define DC_FB_SSD1963_DEFAULT_PIXEL_FORMAT_SSD1963 kSSD1963_RGB565
#define DC_FB_SSD1963_DEFAULT_PIXEL_BYTES 2
#else
#define DC_FB_SSD1963_DEFAULT_PIXEL_FORMAT kVIDEO_PixelFormatRGB888
#define DC_FB_SSD1963_DEFAULT_PIXEL_FORMAT_SSD1963 kSSD1963_RGB888
#define DC_FB_SSD1963_DEFAULT_PIXEL_BYTES 3
#endif

/*! @brief Data for SSD1963 display controller layer. */
typedef struct _dc_fb_ssd1963_layer
{
    bool enabled;              /*!< The layer is enabled. */
    dc_fb_info_t fbInfo;       /*!< Frame buffer info. */
    void *frameBuffer;         /*!< Frame buffer sent currently. */
    dc_fb_callback_t callback; /*!< Callback for buffer switch off. */
    void *cbParam;             /*!< Callback parameter. */
} dc_fb_ssd1963_layer_t;

/*! @brief Data for SSD1963 display controller driver handle. */
typedef struct _dc_fb_ssd1963_handle
{
    ssd1963_handle_t ssd1963;                              /*!< The SSD1963 handle. */
    uint8_t initTimes;                                     /*!< How many times the DC is initialized. */
    uint8_t enabledLayerCount;                             /*!< How many times layers are enabled. */
    dc_fb_ssd1963_layer_t layers[DC_FB_SSD1963_MAX_LAYER]; /*!< Information of the layer. */
} dc_fb_ssd1963_handle_t;

/*! @brief Configuration for SSD1963 display controller driver handle. */
typedef struct _dc_fb_ssd1963_config
{
    const ssd1963_config_t ssd1963Config; /*!< SSD1963 configuration. */
    const dbi_xfer_ops_t *xferOps;        /*!< DBI bus operations used for SSD1963. */
    void *xferOpsData;                    /*!< Data used by @ref xferOps */
    uint32_t srcClock_Hz;                 /*!< The external reference clock frequency. */
} dc_fb_ssd1963_config_t;

extern const dc_fb_ops_t g_dcFbOpsSSD1963;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

status_t DC_FB_SSD1963_Init(const dc_fb_t *dc);
status_t DC_FB_SSD1963_Deinit(const dc_fb_t *dc);
status_t DC_FB_SSD1963_EnableLayer(const dc_fb_t *dc, uint8_t layer);
status_t DC_FB_SSD1963_DisableLayer(const dc_fb_t *dc, uint8_t layer);
status_t DC_FB_SSD1963_SetLayerConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo);
status_t DC_FB_SSD1963_GetLayerDefaultConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo);
status_t DC_FB_SSD1963_SetFrameBuffer(const dc_fb_t *dc, uint8_t layer, void *frameBuffer);
uint32_t DC_FB_SSD1963_GetProperty(const dc_fb_t *dc);
void DC_FB_SSD1963_SetCallback(const dc_fb_t *dc, uint8_t layer, dc_fb_callback_t callback, void *param);
void DC_FB_SSD1963_IRQHandler(const dc_fb_t *dc);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_DC_FB_SSD1963_H_ */
