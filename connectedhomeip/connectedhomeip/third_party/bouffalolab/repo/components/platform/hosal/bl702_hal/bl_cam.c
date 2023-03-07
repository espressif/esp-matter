/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <string.h>
#include <stdio.h>
#include <bl702_glb.h>
#include <bl702_gpio.h>
#include <bl702_cam.h>
#include <bl702_mjpeg.h>
#include <cam_reg.h>
#include <mjpeg_reg.h>
#include <FreeRTOS.h>
#include <event_groups.h>

#include "bl_cam.h"

#include "utils_log.h"

#define DUMP_CAM  0

static CAM_CFG_Type camera_dvp;
static CAM_CFG_Type *context_cam;
static MJPEG_CFG_Type mjpeg_engine;
static EventGroupHandle_t camera_event;
static const rt_camera_desc *m_desc;

//static bl60x_mjpeg_t *context_mjpeg;
//uint8_t  *buffer_cam;
//uint32_t  buffer_cam_size;
uint32_t frame_pos;
uint32_t buffer_cam[10*1024];

#define CAMERA_RESOLUTION_X            m_desc->width
#define CAMERA_RESOLUTION_Y            m_desc->height
#define CAMERA_FRAME_SIZE              (CAMERA_RESOLUTION_X * 2 * CAMERA_RESOLUTION_Y)  // YUV422, 16 bits per pixel
#define MJPEG_READ_ADDR                (uint32_t)buffer_cam
#define MJPEG_READ_SIZE                2                                                // one for CAM and one for MJPEG in turn
#define CAMERA_BUFFER_SIZE_WHEN_MJPEG  (CAMERA_RESOLUTION_X * 2 * 8 * MJPEG_READ_SIZE)
#define MJPEG_WRITE_ADDR               (MJPEG_READ_ADDR + CAMERA_BUFFER_SIZE_WHEN_MJPEG)
#define MJPEG_WRITE_SIZE               (sizeof(buffer_cam) - CAMERA_BUFFER_SIZE_WHEN_MJPEG)

#define CROP_WIDTH                     120
#define CROP_HEIGHT                    120
#define CROP_FRAME_SIZE                (CROP_WIDTH * CROP_HEIGHT)          // YUV400, 8 bits per pixel
#define CROP_FRAME_CNT                 2                                   // make sure CROP_FRAME_CNT >= 2
#define CROP_MEM_ADDR                  (uint32_t)buffer_cam
#define CROP_MEM_SIZE                  (CROP_FRAME_SIZE * CROP_FRAME_CNT)  // make sure CROP_MEM_SIZE <= sizeof(buffer_cam)

#define MJPEG_BUFFER_SIZE 0x100000
#define MJPEG_EVENT_FRAME_INT (1 << 0)
#define MJPEG_EVENT_FRAME_ERROR (1 << 1)

#define CAM_REG_INT_STATUS_BITS_ERR     (0x7 << 11)
#define CAMERA_EVENT_FRAME_INT  (1 << 0)
#define CAMERA_EVENT_FRAME_ERROR (1 << 1)

#define mjpeg_printf(...) do {} while (0)

uint8_t jpeg_quality = 50;

int bl_cam_config_update(uint8_t quality)
{
    jpeg_quality = quality;
    printf("[MJPEG] Using quality %u\r\n",
            jpeg_quality
    );

    return 0;
}

int bl_cam_config_get(uint8_t *quality, uint16_t *width, uint16_t *height)
{
    if (quality){
        *quality = jpeg_quality;
    }
    if (width){
        *width = m_desc->width;
    }
    if (height){
        *height = m_desc->height;
    }

    return 0;
}

static int cam_init_gpio(void)
{
    GLB_GPIO_Cfg_Type cfg;

#if 0
    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_0;
    cfg.gpioFun = GPIO0_FUN_PIX_CLK;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);
#endif

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_1;
    cfg.gpioFun = GPIO1_FUN_FRAME_VLD;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_2;
    cfg.gpioFun = GPIO2_FUN_LINE_VLD;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_3;
    cfg.gpioFun = GPIO3_FUN_PIX_DAT0;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_4;
    cfg.gpioFun = GPIO4_FUN_PIX_DAT1;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_5;
    cfg.gpioFun = GPIO5_FUN_PIX_DAT2;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_6;
    cfg.gpioFun = GPIO6_FUN_PIX_DAT3;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_12;
    cfg.gpioFun = GPIO12_FUN_PIX_DAT4;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_29;
    cfg.gpioFun = GPIO29_FUN_PIX_DAT5;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_30;
    cfg.gpioFun = GPIO30_FUN_PIX_DAT6;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_31;
    cfg.gpioFun = GPIO31_FUN_PIX_DAT7;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);
    return 0;
}

static int dvp_init(int useMjpeg, int frm_vld_high)
{
    memset(&camera_dvp, 0, sizeof(camera_dvp));

    if (useMjpeg) {
        camera_dvp.swMode = CAM_SW_MODE_AUTO;
//        camera_dvp.swIntCnt = 0;
        camera_dvp.frameMode = CAM_INTERLEAVE_MODE;
        camera_dvp.yuvMode = CAM_YUV422;
        camera_dvp.linePol = CAM_LINE_ACTIVE_POLARITY_HIGH;
        camera_dvp.framePol = frm_vld_high;
        camera_dvp.camSensorMode = CAM_SENSOR_MODE_V_AND_H;
        camera_dvp.burstType = CAM_BURST_TYPE_INCR16;
        camera_dvp.waitCount = 0x40;
        camera_dvp.memStart0 = MJPEG_READ_ADDR;
        camera_dvp.memSize0 = CAMERA_BUFFER_SIZE_WHEN_MJPEG;
        camera_dvp.frameSize0 = CAMERA_FRAME_SIZE;
        camera_dvp.memStart1 = 0;
        camera_dvp.memSize1 = 0;
        camera_dvp.frameSize1 = 0;
    } else {
        CAM_Hsync_Crop(m_desc->width-CROP_WIDTH, m_desc->width+CROP_WIDTH);
        CAM_Vsync_Crop((m_desc->height-CROP_HEIGHT)/2, (m_desc->height+CROP_HEIGHT)/2);
        
        camera_dvp.swMode = CAM_SW_MODE_MANUAL;
//        camera_dvp.swIntCnt = 0;
        camera_dvp.frameMode = CAM_INTERLEAVE_MODE;
        camera_dvp.yuvMode = CAM_YUV400_ODD;
        camera_dvp.linePol = CAM_LINE_ACTIVE_POLARITY_HIGH;
        camera_dvp.framePol = frm_vld_high;
        camera_dvp.camSensorMode = CAM_SENSOR_MODE_V_AND_H;
        camera_dvp.burstType = CAM_BURST_TYPE_INCR16;
        camera_dvp.waitCount = 0x40;
        camera_dvp.memStart0 = CROP_MEM_ADDR;
        camera_dvp.memSize0 = CROP_MEM_SIZE;
        camera_dvp.frameSize0 = CROP_FRAME_SIZE;
        camera_dvp.memStart1 = 0;
        camera_dvp.memSize1 = 0;
        camera_dvp.frameSize1 = 0;
    }
    context_cam = &camera_dvp;
    CAM_Init(&camera_dvp);
    CAM_Enable();

    return 0;
}

int bl_cam_frame_wait(void)
{
    int ret;
    EventBits_t xResult;

    if ((ret = MJPEG_Get_Frame_Count())) {
        return ret;
    }

    xResult = xEventGroupWaitBits(
            camera_event,
            CAMERA_EVENT_FRAME_INT,
            pdTRUE,
            pdFAIL,
            10
    );
    if (xResult & CAMERA_EVENT_FRAME_INT){
        return 0;
    }

    return -1;
}

int bl_cam_yuv_frame_wait(void)
{
    int ret;
    EventBits_t xResult;

    if ((ret = CAM_Get_Frame_Count_0())) {
        return ret;
    }

    xResult = xEventGroupWaitBits(
            camera_event,
            CAMERA_EVENT_FRAME_INT,
            pdTRUE,
            pdFAIL,
            10
    );
    if (xResult & CAMERA_EVENT_FRAME_INT){
        return 0;
    }

    return -1;
}

void bl_cam_frame_edge_drop(void)
{
    // TODO
}

int bl_cam_frame_pop(void)
{
    MJPEG_Pop_Frame();

    return 0;
}

int bl_cam_yuv_frame_pop(void)
{
    CAM_Interleave_Pop_Frame();

    return 0;
}

int bl_cam_frame_pop_old(void)
{
    while (MJPEG_Get_Frame_Count() > 1) {
		bl_cam_frame_pop();
    }
    return 0;
}

int bl_cam_frame_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2)
{
    MJPEG_Frame_Info info;

    MJPEG_Get_Frame_Info(&info);
    *frames = info.validFrames;
    *ptr1 = (uint8_t *)(info.curFrameAddr);
    *len1 = info.curFrameBytes;
    if (*frames) {
        *ptr2 = NULL;
        *len2 = 0;
        return 0;
    }

    return -1;
}

int bl_cam_yuv_frame_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2)
{
    CAM_Interleave_Frame_Info info;

    CAM_Interleave_Get_Frame_Info(&info);
    *frames = info.validFrames;
    *ptr1 = (uint8_t *)(info.curFrameAddr);
    *len1 = info.curFrameBytes;
    if (*frames) {
        *ptr2 = NULL;
        *len2 = 0;
        return 0;
    }

    return -1;
}

int bl_cam_frame_fifo_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2)
{
    // TODO
    return 0;
}

static int mjpeg_init()
{
    memset(&mjpeg_engine, 0, sizeof(mjpeg_engine));

    mjpeg_engine.burst = MJPEG_BURST_INCR16;
    mjpeg_engine.quality = 50;
    mjpeg_engine.yuv = MJPEG_YUV422_INTERLEAVE;
    mjpeg_engine.waitCount = 0x400;
    mjpeg_engine.bufferMjpeg = MJPEG_WRITE_ADDR;
    mjpeg_engine.sizeMjpeg = MJPEG_WRITE_SIZE;
    mjpeg_engine.bufferCamYY = MJPEG_READ_ADDR;
    mjpeg_engine.sizeCamYY = MJPEG_READ_SIZE;
    mjpeg_engine.bufferCamUV = 0;
    mjpeg_engine.sizeCamUV = 0;
    mjpeg_engine.resolutionX = CAMERA_RESOLUTION_X;
    mjpeg_engine.resolutionY = CAMERA_RESOLUTION_Y;
    mjpeg_engine.bitOrderEnable = ENABLE;
    mjpeg_engine.evenOrderEnable = ENABLE;
    mjpeg_engine.swapModeEnable = DISABLE;
    mjpeg_engine.overStopEnable = ENABLE;
    mjpeg_engine.reflectDmy = DISABLE;
    mjpeg_engine.verticalDmy = DISABLE;
    mjpeg_engine.horizationalDmy = DISABLE;

    MJPEG_Set_YUYV_Order_Interleave(1,0,3,2);
    MJPEG_Init(&mjpeg_engine);
    MJPEG_Enable();

    return 0;
}

static int video_init(int useMjpeg)
{
#if 1
    static char video_init_flag = 0;

    if (0 == video_init_flag) {
        camera_event = xEventGroupCreate();
        video_init_flag = 1;
    }
#else
    if (NULL != camera_event) {
        vEventGroupDelete(camera_event);
    }
    camera_event = xEventGroupCreate();
#endif

    if (useMjpeg) {
        CAM_Disable();
        MJPEG_Disable();
#if !DUMP_CAM
        mjpeg_init();
#endif
        dvp_init(!DUMP_CAM, m_desc->frm_vld_high);
    } else {
        CAM_Disable();
        MJPEG_Disable();
        dvp_init(0, m_desc->frm_vld_high);
    }

    return 0;
}

static void set_camera_desc(const rt_camera_desc *desc)
{
    m_desc = desc;
}

int bl_cam_mjpeg_encoder(uint32_t yuv_addr, uint32_t jpeg_addr, uint32_t *jpeg_size,  uint32_t width, uint32_t height, uint32_t quality)
{
    // TODO
    return 0;
}

int bl_cam_init(int enable_mjpeg, const rt_camera_desc *desc)
{
    cam_init_gpio();
    set_camera_desc(desc);
    video_init(enable_mjpeg);
    return 0;
}

int bl_cam_restart(int enable_mjpeg)
{
    cam_init_gpio();
    video_init(enable_mjpeg);
    return 0;
}

int bl_cam_enable_24MRef(void)
{
    GLB_GPIO_Cfg_Type cfg;

#if 1  // configure pix_clk before cam_ref_clk
    cfg.drive=0;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_0;
    cfg.gpioFun = GPIO0_FUN_PIX_CLK;
    cfg.gpioMode = GPIO_MODE_INPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);
#endif

    cfg.drive=1;
    cfg.smtCtrl=1;
    cfg.gpioPin = GLB_GPIO_PIN_10;
    cfg.gpioFun = GPIO10_FUN_CAM_REF_CLK;
    cfg.gpioMode = GPIO_MODE_OUTPUT;
    cfg.pullType = GPIO_PULL_NONE;
    GLB_GPIO_Init(&cfg);

    GLB_AHB_Slave1_Clock_Gate(DISABLE,0x1D);  // BL_AHB_SLAVE1_CAM
    GLB_AHB_Slave1_Clock_Gate(DISABLE,0x1E);  // BL_AHB_SLAVE1_MJPEG
    GLB_Set_CAM_CLK(ENABLE,GLB_CAM_CLK_DLL96M,3);
    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
    return 0;
}
