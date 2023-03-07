/*
 * Copyright (c) 2019, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_dc_fb_dsi_cmd.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
const dc_fb_ops_t g_dcFbOpsDsiCmd = {
    .init                  = DC_FB_DSI_CMD_Init,
    .deinit                = DC_FB_DSI_CMD_Deinit,
    .enableLayer           = DC_FB_DSI_CMD_EnableLayer,
    .disableLayer          = DC_FB_DSI_CMD_DisableLayer,
    .setLayerConfig        = DC_FB_DSI_CMD_SetLayerConfig,
    .getLayerDefaultConfig = DC_FB_DSI_CMD_GetLayerDefaultConfig,
    .setFrameBuffer        = DC_FB_DSI_CMD_SetFrameBuffer,
    .getProperty           = DC_FB_DSI_CMD_GetProperty,
    .setCallback           = DC_FB_DSI_CMD_SetCallback,
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void DC_FB_DSI_CMD_FrameDoneCallback(status_t status, void *userData)
{
    dc_fb_dsi_cmd_handle_t *dcHandle;
    dc_fb_dsi_cmd_layer_t *layer;

    dcHandle = (dc_fb_dsi_cmd_handle_t *)userData;

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
status_t DC_FB_DSI_CMD_Init(const dc_fb_t *dc)
{
    status_t status;

    const display_config_t *dcConfig;
    dc_fb_dsi_cmd_handle_t *dcHandle;
    display_handle_t *panelHandle;
    mipi_dsi_device_t *dsiDevice;

    dcHandle = (dc_fb_dsi_cmd_handle_t *)dc->prvData;

    if (0 == dcHandle->initTimes++)
    {
        panelHandle = (display_handle_t *)(dcHandle->panelHandle);
        dcConfig    = (const display_config_t *)(dc->config);
        dsiDevice   = dcHandle->dsiDevice;

        status = DISPLAY_Init(panelHandle, dcConfig);

        if (kStatus_Success != status)
        {
            return status;
        }

        MIPI_DSI_SetMemoryDoneCallback(dsiDevice, DC_FB_DSI_CMD_FrameDoneCallback, dcHandle);
    }

    return kStatus_Success;
}

status_t DC_FB_DSI_CMD_Deinit(const dc_fb_t *dc)
{
    status_t status = kStatus_Success;

    dc_fb_dsi_cmd_handle_t *dcHandle;
    display_handle_t *panelHandle;

    dcHandle = (dc_fb_dsi_cmd_handle_t *)dc->prvData;

    if (dcHandle->initTimes > 0)
    {
        if (--dcHandle->initTimes == 0)
        {
            panelHandle = (display_handle_t *)(dcHandle->panelHandle);
            status      = DISPLAY_Deinit(panelHandle);
        }
    }

    return status;
}

status_t DC_FB_DSI_CMD_EnableLayer(const dc_fb_t *dc, uint8_t layer)
{
    dc_fb_dsi_cmd_handle_t *dcHandle;
    display_handle_t *panelHandle;

    status_t status = kStatus_Success;

    dcHandle = (dc_fb_dsi_cmd_handle_t *)dc->prvData;

    if (0 == dcHandle->enabledLayerCount++)
    {
        panelHandle = (display_handle_t *)(dcHandle->panelHandle);

        status = DISPLAY_Start(panelHandle);
    }

    return status;
}

status_t DC_FB_DSI_CMD_DisableLayer(const dc_fb_t *dc, uint8_t layer)
{
    dc_fb_dsi_cmd_handle_t *dcHandle;
    display_handle_t *panelHandle;

    status_t status = kStatus_Success;

    dcHandle = (dc_fb_dsi_cmd_handle_t *)dc->prvData;

    if (dcHandle->enabledLayerCount > 0)
    {
        if (--dcHandle->enabledLayerCount == 0)
        {
            panelHandle = (display_handle_t *)(dcHandle->panelHandle);

            status = DISPLAY_Stop(panelHandle);
        }
    }

    return status;
}

status_t DC_FB_DSI_CMD_SetLayerConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo)
{
    assert(layer < DC_FB_DSI_CMD_MAX_LAYER);

    dc_fb_dsi_cmd_handle_t *dcHandle = (dc_fb_dsi_cmd_handle_t *)(dc->prvData);

    /* The pixel format is already set by DSI_CMD_Init and could not be changed,
       so here don't need to set the format.
       */

    dcHandle->layers[layer].fbInfo = *fbInfo;

    return kStatus_Success;
}

status_t DC_FB_DSI_CMD_GetLayerDefaultConfig(const dc_fb_t *dc, uint8_t layer, dc_fb_info_t *fbInfo)
{
    assert(layer < DC_FB_DSI_CMD_MAX_LAYER);

    dc_fb_dsi_cmd_handle_t *dcHandle = (dc_fb_dsi_cmd_handle_t *)(dc->prvData);
    display_handle_t *panelHandle    = (display_handle_t *)(dcHandle->panelHandle);

    fbInfo->startX      = 0;
    fbInfo->startY      = 0;
    fbInfo->width       = panelHandle->width;
    fbInfo->height      = panelHandle->height;
    fbInfo->strideBytes = VIDEO_GetPixelSizeBits(panelHandle->pixelFormat) / 8;
    fbInfo->pixelFormat = panelHandle->pixelFormat;

    return kStatus_Success;
}

status_t DC_FB_DSI_CMD_SetFrameBuffer(const dc_fb_t *dc, uint8_t layer, void *frameBuffer)
{
    assert(layer < DC_FB_DSI_CMD_MAX_LAYER);

    status_t status;

    dc_fb_dsi_cmd_handle_t *dcHandle = dc->prvData;
    dc_fb_info_t *fbInfo             = &dcHandle->layers[layer].fbInfo;
    mipi_dsi_device_t *dsiDevice     = dcHandle->dsiDevice;

    dcHandle->layers[layer].frameBuffer = frameBuffer;

    status = MIPI_DSI_SelectArea(dsiDevice, fbInfo->startX, fbInfo->startY, fbInfo->startX + fbInfo->width - 1,
                                 fbInfo->startY + fbInfo->height - 1);

    if (kStatus_Success != status)
    {
        return status;
    }

    return MIPI_DSI_WriteMemory(dsiDevice, frameBuffer, fbInfo->height * fbInfo->strideBytes);
}

void DC_FB_DSI_CMD_SetCallback(const dc_fb_t *dc, uint8_t layer, dc_fb_callback_t callback, void *param)
{
    assert(layer < DC_FB_DSI_CMD_MAX_LAYER);
    dc_fb_dsi_cmd_handle_t *dcHandle = dc->prvData;

    dcHandle->layers[layer].callback = callback;
    dcHandle->layers[layer].cbParam  = param;
}

uint32_t DC_FB_DSI_CMD_GetProperty(const dc_fb_t *dc)
{
    return 0;
}
