/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dc_fb_ssd1963.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
const dc_fb_ops_t g_dcFbOpsSSD1963 = {
    .init                  = DC_FB_SSD1963_Init,
    .deinit                = DC_FB_SSD1963_Deinit,
    .enableLayer           = DC_FB_SSD1963_EnableLayer,
    .disableLayer          = DC_FB_SSD1963_DisableLayer,
    .setLayerConfig        = DC_FB_SSD1963_SetLayerConfig,
    .getLayerDefaultConfig = DC_FB_SSD1963_GetLayerDefaultConfig,
    .setFrameBuffer        = DC_FB_SSD1963_SetFrameBuffer,
    .getProperty           = DC_FB_SSD1963_GetProperty,
    .setCallback           = DC_FB_SSD1963_SetCallback,
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void DC_FB_SSD1963_FrameDoneCallback(status_t status, void *userData)
{
    dc_fb_ssd1963_handle_t *dcHandle;
    dc_fb_ssd1963_layer_t *layer;

    dcHandle = (dc_fb_ssd1963_handle_t *)userData;

    /* Currently only support one layer, so the layer index is always 0. */
    layer = &(dcHandle->layers[0]);

    /* Frame buffer data has been sent to the panel, the frame buffer is free
     * to be used for set new data, call the callback to notify upper layer.
     * The callback is set in application or fbdev.
     */
    layer->callback(layer->cbParam, layer->frameBuffer);
}

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
status_t DC_FB_SSD1963_Init(const dc_fb_t *dc)
{
    status_t status;

    const dc_fb_ssd1963_config_t *dcConfig;

    dc_fb_ssd1963_handle_t *dcHandle = dc->prvData;

    if (0 == dcHandle->initTimes++)
    {
        dcConfig = (const dc_fb_ssd1963_config_t *)(dc->config);

        status = SSD1963_Init(&dcHandle->ssd1963, &dcConfig->ssd1963Config, dcConfig->xferOps, dcConfig->xferOpsData,
                              dcConfig->srcClock_Hz);

        if (kStatus_Success != status)
        {
            return status;
        }

        SSD1963_SetMemoryDoneCallback(&dcHandle->ssd1963, DC_FB_SSD1963_FrameDoneCallback, dcHandle);
    }

    return kStatus_Success;
}

status_t DC_FB_SSD1963_Deinit(const dc_fb_t *dc)
{
    dc_fb_ssd1963_handle_t *dcHandle = dc->prvData;

    if (dcHandle->initTimes > 0)
    {
        if (--dcHandle->initTimes == 0)
        {
            SSD1963_Deinit(&dcHandle->ssd1963);
        }
    }

    return kStatus_Success;
}

status_t DC_FB_SSD1963_EnableLayer(const dc_fb_t *dc, uint8_t layer)
{
    dc_fb_ssd1963_handle_t *dcHandle = dc->prvData;

    if (0 == dcHandle->enabledLayerCount++)
    {
        SSD1963_StartDisplay(&dcHandle->ssd1963);
        SSD1963_SetBackLight(&dcHandle->ssd1963, 128);
    }

    return kStatus_Success;
}

status_t DC_FB_SSD1963_DisableLayer(const dc_fb_t *dc, uint8_t layer)
{
    dc_fb_ssd1963_handle_t *dcHandle = dc->prvData;

    if (dcHandle->enabledLayerCount > 0)
    {
        if (--dcHandle->enabledLayerCount == 0)
        {
            SSD1963_SetBackLight(&dcHandle->ssd1963, 0);
            SSD1963_StopDisplay(&dcHandle->ssd1963);
        }
    }

    return kStatus_Success;
}

status_t DC_FB_SSD1963_SetLayerConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo)
{
    assert(layer < DC_FB_SSD1963_MAX_LAYER);

    dc_fb_ssd1963_handle_t *dcHandle = (dc_fb_ssd1963_handle_t *)(dc->prvData);

    /* The pixel format is already set by SSD1963_Init and could not be changed,
       so here don't need to set the format.
       */

    dcHandle->layers[layer].fbInfo = *fbInfo;

    return kStatus_Success;
}

status_t DC_FB_SSD1963_GetLayerDefaultConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo)
{
    assert(layer < DC_FB_SSD1963_MAX_LAYER);

    dc_fb_ssd1963_handle_t *dcHandle = (dc_fb_ssd1963_handle_t *)(dc->prvData);

    fbInfo->startX      = 0;
    fbInfo->startY      = 0;
    fbInfo->width       = dcHandle->ssd1963.panelWidth;
    fbInfo->height      = dcHandle->ssd1963.panelHeight;
    fbInfo->strideBytes = DC_FB_SSD1963_DEFAULT_PIXEL_BYTES * dcHandle->ssd1963.panelWidth;
    fbInfo->pixelFormat = DC_FB_SSD1963_DEFAULT_PIXEL_FORMAT;

    return kStatus_Success;
}

status_t DC_FB_SSD1963_SetFrameBuffer(const dc_fb_t *dc, uint8_t layer, void *frameBuffer)
{
    assert(layer < DC_FB_SSD1963_MAX_LAYER);

    dc_fb_ssd1963_handle_t *dcHandle = dc->prvData;
    dc_fb_info_t *fbInfo             = &dcHandle->layers[layer].fbInfo;

    dcHandle->layers[layer].frameBuffer = frameBuffer;
    SSD1963_SelectArea(&dcHandle->ssd1963, fbInfo->startX, fbInfo->startY, fbInfo->startX + fbInfo->width - 1,
                       fbInfo->startY + fbInfo->height - 1);
    SSD1963_WriteMemory(&dcHandle->ssd1963, frameBuffer, fbInfo->height * fbInfo->strideBytes);

    return kStatus_Success;
}

void DC_FB_SSD1963_SetCallback(const dc_fb_t *dc, uint8_t layer, dc_fb_callback_t callback, void *param)
{
    assert(layer < DC_FB_SSD1963_MAX_LAYER);
    dc_fb_ssd1963_handle_t *dcHandle = dc->prvData;

    dcHandle->layers[layer].callback = callback;
    dcHandle->layers[layer].cbParam  = param;
}

uint32_t DC_FB_SSD1963_GetProperty(const dc_fb_t *dc)
{
    return 0;
}
