/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_fbdev.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void FBDEV_BufferSwitchOffCallback(void *param, void *switchOffBuffer);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t FBDEV_Open(fbdev_t *fbdev, const dc_fb_t *dc, uint8_t layer)
{
    status_t status;

    assert(fbdev);

    memset(fbdev, 0, sizeof(fbdev_t));

    fbdev->dc = dc;

    VIDEO_MEMPOOL_InitEmpty(&fbdev->fbManager);

    /* Initialize the display controller. */
    status = dc->ops->init(dc);
    if (kStatus_Success != status)
    {
        return status;
    }

    fbdev->layer = layer;

    /* Initializes the dc_fb_info_t to the display controller default setting. */
    status = dc->ops->getLayerDefaultConfig(dc, layer, &fbdev->fbInfo.bufInfo);
    if (kStatus_Success != status)
    {
        return status;
    }

    fbdev->semaFramePending = xSemaphoreCreateBinary();
    if (NULL == fbdev->semaFramePending)
    {
        return kStatus_Fail;
    }

    /* No frame pending. */
    xSemaphoreGive(fbdev->semaFramePending);

    dc->ops->setCallback(dc, layer, FBDEV_BufferSwitchOffCallback, (void *)fbdev);

    return kStatus_Success;
}

status_t FBDEV_Close(fbdev_t *fbdev)
{
    const dc_fb_t *dc = fbdev->dc;

    dc->ops->deinit(dc);

    if (NULL != fbdev->semaFbManager)
    {
        vSemaphoreDelete(fbdev->semaFbManager);
        fbdev->semaFbManager = NULL;
    }

    if (NULL != fbdev->semaFramePending)
    {
        vSemaphoreDelete(fbdev->semaFramePending);
        fbdev->semaFramePending = NULL;
    }

    return kStatus_Success;
}

status_t FBDEV_Enable(fbdev_t *fbdev)
{
    status_t status = kStatus_Success;

    const dc_fb_t *dc = fbdev->dc;

    if (!fbdev->enabled)
    {
        /* Wait for frame buffer sent to display controller video memory. */
        if ((dc->ops->getProperty(dc) & kDC_FB_ReserveFrameBuffer) == 0)
        {
            xSemaphoreTake(fbdev->semaFramePending, portMAX_DELAY);
        }

        /* No frame is pending. */
        xSemaphoreGive(fbdev->semaFramePending);

        status = dc->ops->enableLayer(dc, fbdev->layer);

        if (kStatus_Success == status)
        {
            fbdev->enabled = true;
        }
    }

    return status;
}

status_t FBDEV_Disable(fbdev_t *fbdev)
{
    const dc_fb_t *dc = fbdev->dc;

    if (!fbdev->enabled)
    {
        /* Wait until no frame pending. */
        xSemaphoreTake(fbdev->semaFramePending, portMAX_DELAY);
        xSemaphoreGive(fbdev->semaFramePending);

        dc->ops->disableLayer(dc, fbdev->layer);

        fbdev->enabled = false;
    }

    return kStatus_Success;
}

void FBDEV_GetFrameBufferInfo(fbdev_t *fbdev, fbdev_fb_info_t *info)
{
    *info = fbdev->fbInfo;
}

status_t FBDEV_SetFrameBufferInfo(fbdev_t *fbdev, fbdev_fb_info_t *info)
{
    status_t status;
    const dc_fb_t *dc = fbdev->dc;

    /* Should only change the frame buffer setting before enabling the fbdev. */
    if (fbdev->enabled)
    {
        return kStatus_Fail;
    }

    fbdev->fbInfo = *info;

    status = dc->ops->setLayerConfig(dc, fbdev->layer, &fbdev->fbInfo.bufInfo);

    if (kStatus_Success != status)
    {
        return status;
    }

    fbdev->semaFbManager = xSemaphoreCreateCounting(info->bufferCount, 0);
    if (NULL == fbdev->semaFbManager)
    {
        return kStatus_Fail;
    }

    for (uint8_t i = 0; i < info->bufferCount; i++)
    {
        /* Don't need to disable interrupt for the MEMPOOL operation, because
           the fbdev is not working, this is the only function to access MEMPOOL.
         */
        VIDEO_MEMPOOL_Put(&fbdev->fbManager, info->buffers[i]);
        xSemaphoreGive(fbdev->semaFbManager);
    }

    /* No frame pending. */
    xSemaphoreGive(fbdev->semaFramePending);

    return kStatus_Success;
}

void *FBDEV_GetFrameBuffer(fbdev_t *fbdev, uint32_t flags)
{
    TickType_t tick;
    void *fb;

    tick = ((flags & kFBDEV_NoWait) != 0) ? 0 : portMAX_DELAY;

    if (pdTRUE == xSemaphoreTake(fbdev->semaFbManager, tick))
    {
        /* Disable interrupt to protect the MEMPOOL. */
        portENTER_CRITICAL();
        fb = VIDEO_MEMPOOL_Get(&fbdev->fbManager);
        portEXIT_CRITICAL();
    }
    else
    {
        fb = NULL;
    }

    return fb;
}

status_t FBDEV_SetFrameBuffer(fbdev_t *fbdev, void *frameBuffer, uint32_t flags)
{
    TickType_t tick;
    const dc_fb_t *dc = fbdev->dc;

    tick = ((flags & kFBDEV_NoWait) != 0) ? 0 : portMAX_DELAY;

    if (xSemaphoreTake(fbdev->semaFramePending, tick))
    {
        dc->ops->setFrameBuffer(dc, fbdev->layer, frameBuffer);
        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

static void FBDEV_BufferSwitchOffCallback(void *param, void *switchOffBuffer)
{
    fbdev_t *fbdev              = (fbdev_t *)param;
    BaseType_t fbManagerWake    = pdFALSE;
    BaseType_t framePendingWake = pdFALSE;

    /* This function should only be called in ISR, so don't need to protect the MEMPOOL */
    VIDEO_MEMPOOL_Put(&fbdev->fbManager, switchOffBuffer);
    xSemaphoreGiveFromISR(fbdev->semaFbManager, &fbManagerWake);

    xSemaphoreGiveFromISR(fbdev->semaFramePending, &framePendingWake);

    if ((fbManagerWake == pdTRUE) || (framePendingWake == pdTRUE))
    {
        portYIELD_FROM_ISR(pdTRUE);
    }
}
