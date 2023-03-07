/*
 * Copyright (c) 2019, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dc_fb_lcdif.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
const dc_fb_ops_t g_dcFbOpsLcdif = {
    .init                  = DC_FB_LCDIF_Init,
    .deinit                = DC_FB_LCDIF_Deinit,
    .enableLayer           = DC_FB_LCDIF_EnableLayer,
    .disableLayer          = DC_FB_LCDIF_DisableLayer,
    .setLayerConfig        = DC_FB_LCDIF_SetLayerConfig,
    .getLayerDefaultConfig = DC_FB_LCDIF_GetLayerDefaultConfig,
    .setFrameBuffer        = DC_FB_LCDIF_SetFrameBuffer,
    .getProperty           = DC_FB_LCDIF_GetProperty,
    .setCallback           = DC_FB_LCDIF_SetCallback,
};

typedef struct
{
    video_pixel_format_t videoFormat;
    lcdif_fb_format_t lcdifFormat;
} dc_fb_lcdif_pixel_foramt_map_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static status_t DC_FB_LCDIF_GetPixelFormat(video_pixel_format_t input, lcdif_fb_format_t *output);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static const dc_fb_lcdif_pixel_foramt_map_t s_lcdifPixelFormatMap[] = {
    {kVIDEO_PixelFormatRGB565, kLCDIF_PixelFormatRGB565},
    {
        kVIDEO_PixelFormatXRGB8888,
        kLCDIF_PixelFormatXRGB8888,
    },
};

/*******************************************************************************
 * Code
 ******************************************************************************/
static status_t DC_FB_LCDIF_GetPixelFormat(video_pixel_format_t input, lcdif_fb_format_t *output)
{
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(s_lcdifPixelFormatMap); i++)
    {
        if (s_lcdifPixelFormatMap[i].videoFormat == input)
        {
            *output = s_lcdifPixelFormatMap[i].lcdifFormat;
            return kStatus_Success;
        }
    }

    return kStatus_InvalidArgument;
}

status_t DC_FB_LCDIF_Init(const dc_fb_t *dc)
{
    status_t status = kStatus_Success;
    const dc_fb_lcdif_config_t *dcConfig;

    lcdif_dpi_config_t lcdifConfig = {0};

    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;

    if (0 == dcHandle->initTimes++)
    {
        dcConfig = (const dc_fb_lcdif_config_t *)(dc->config);

        LCDIF_DpiModeGetDefaultConfig(&lcdifConfig);

        lcdifConfig.panelWidth    = dcConfig->width;
        lcdifConfig.panelHeight   = dcConfig->height;
        lcdifConfig.hsw           = dcConfig->hsw;
        lcdifConfig.hfp           = dcConfig->hfp;
        lcdifConfig.hbp           = dcConfig->hbp;
        lcdifConfig.vsw           = dcConfig->vsw;
        lcdifConfig.vfp           = dcConfig->vfp;
        lcdifConfig.vbp           = dcConfig->vbp;
        lcdifConfig.format        = dcConfig->outputFormat;
        lcdifConfig.polarityFlags = dcConfig->polarityFlags;

        dcHandle->height = dcConfig->height;
        dcHandle->width  = dcConfig->width;
        dcHandle->lcdif  = dcConfig->lcdif;

        LCDIF_Init(dcHandle->lcdif);

        status = LCDIF_DpiModeSetConfig(dcHandle->lcdif, 0, &lcdifConfig);
    }

    return status;
}

status_t DC_FB_LCDIF_Deinit(const dc_fb_t *dc)
{
    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;

    if (dcHandle->initTimes > 0)
    {
        if (--dcHandle->initTimes == 0)
        {
            LCDIF_Deinit(dcHandle->lcdif);
        }
    }

    return kStatus_Success;
}

status_t DC_FB_LCDIF_EnableLayer(const dc_fb_t *dc, uint8_t layer)
{
    assert(layer < DC_FB_LCDIF_MAX_LAYER);

    status_t status                = kStatus_Success;
    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;

    /* If the layer is already started. */
    if (!dcHandle->layers[layer].enabled)
    {
        /* Must have valid frame buffer to show. */
        if (dcHandle->layers[layer].activeBuffer == NULL)
        {
            status = kStatus_Fail;
        }
        else
        {
            dcHandle->layers[layer].fbConfig.enable = true;
            LCDIF_SetFrameBufferConfig(dcHandle->lcdif, 0, &dcHandle->layers[layer].fbConfig);
            LCDIF_EnableInterrupts(dcHandle->lcdif, kLCDIF_Display0FrameDoneInterrupt);
            dcHandle->layers[layer].enabled = true;
        }
    }

    return status;
}

status_t DC_FB_LCDIF_DisableLayer(const dc_fb_t *dc, uint8_t layer)
{
    assert(layer < DC_FB_LCDIF_MAX_LAYER);

    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;

    if (dcHandle->layers[layer].enabled)
    {
        dcHandle->layers[layer].fbConfig.enable = false;
        LCDIF_SetFrameBufferConfig(dcHandle->lcdif, 0, &dcHandle->layers[layer].fbConfig);
        dcHandle->layers[layer].enabled = false;
        LCDIF_DisableInterrupts(dcHandle->lcdif, kLCDIF_Display0FrameDoneInterrupt);
    }

    return kStatus_Success;
}

status_t DC_FB_LCDIF_SetLayerConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo)
{
    assert(layer < DC_FB_LCDIF_MAX_LAYER);

    lcdif_fb_format_t pixelFormat;
    status_t status;

    dc_fb_lcdif_handle_t *dcHandle = (dc_fb_lcdif_handle_t *)(dc->prvData);

    assert(fbInfo->startX == 0);
    assert(fbInfo->startY == 0);
    assert(fbInfo->width == dcHandle->width);
    assert(fbInfo->height == dcHandle->height);

    status = DC_FB_LCDIF_GetPixelFormat(fbInfo->pixelFormat, &pixelFormat);
    if (kStatus_Success != status)
    {
        return status;
    }

    dcHandle->layers[layer].fbConfig.format      = pixelFormat;
    dcHandle->layers[layer].fbConfig.enableGamma = false;

    LCDIF_SetFrameBufferStride(dcHandle->lcdif, 0, fbInfo->strideBytes);

    return kStatus_Success;
}

status_t DC_FB_LCDIF_GetLayerDefaultConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo)
{
    assert(layer < DC_FB_LCDIF_MAX_LAYER);

    dc_fb_lcdif_handle_t *dcHandle = (dc_fb_lcdif_handle_t *)(dc->prvData);

    fbInfo->startX      = 0;
    fbInfo->startY      = 0;
    fbInfo->width       = dcHandle->width;
    fbInfo->height      = dcHandle->height;
    fbInfo->strideBytes = DC_FB_LCDIF_DEFAULT_BYTE_PER_PIXEL * dcHandle->width;
    fbInfo->pixelFormat = DC_FB_LCDIF_DEFAULT_PIXEL_FORMAT;

    return kStatus_Success;
}

status_t DC_FB_LCDIF_SetFrameBuffer(const dc_fb_t *dc, uint8_t layer, void *frameBuffer)
{
    assert(layer < DC_FB_LCDIF_MAX_LAYER);
    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;

    LCDIF_SetFrameBufferAddr(dcHandle->lcdif, 0, (uint32_t)frameBuffer);
    dcHandle->layers[layer].inactiveBuffer = frameBuffer;

    if (!dcHandle->layers[layer].enabled)
    {
        dcHandle->layers[layer].activeBuffer = frameBuffer;
    }
    else
    {
        dcHandle->layers[layer].framePending = true;
    }

    return kStatus_Success;
}

void DC_FB_LCDIF_SetCallback(const dc_fb_t *dc, uint8_t layer, dc_fb_callback_t callback, void *param)
{
    assert(layer < DC_FB_LCDIF_MAX_LAYER);
    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;

    dcHandle->layers[layer].callback = callback;
    dcHandle->layers[layer].cbParam  = param;
}

uint32_t DC_FB_LCDIF_GetProperty(const dc_fb_t *dc)
{
    return kDC_FB_ReserveFrameBuffer;
}

void DC_FB_LCDIF_IRQHandler(const dc_fb_t *dc)
{
    uint32_t intStatus;
    dc_fb_lcdif_handle_t *dcHandle = dc->prvData;
    dc_fb_lcdif_layer_t *layer;
    void *oldActiveBuffer;

    intStatus = LCDIF_GetAndClearInterruptPendingFlags(dcHandle->lcdif);

    if (0 == (intStatus & kLCDIF_Display0FrameDoneInterrupt))
    {
        return;
    }

    for (uint8_t i = 0; i < DC_FB_LCDIF_MAX_LAYER; i++)
    {
        if (dcHandle->layers[i].framePending)
        {
            layer = &dcHandle->layers[i];

            oldActiveBuffer                  = layer->activeBuffer;
            layer->activeBuffer              = layer->inactiveBuffer;
            dcHandle->layers[i].framePending = false;

            layer->callback(layer->cbParam, oldActiveBuffer);
        }
    }
}
