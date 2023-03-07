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
#ifndef __BL_CAM_H__
#define __BL_CAM_H__

#include <sensor.h>

int bl_cam_config_get(uint8_t *quality, uint16_t *width, uint16_t *height);
int bl_cam_config_update(uint8_t quality);
int bl_cam_init(int enable_mjpeg, const rt_camera_desc *desc);
int bl_cam_restart(int enable_mjpeg);
int bl_cam_enable_24MRef(void);
int bl_cam_frame_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2);
int bl_cam_frame_fifo_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2);
int bl_cam_frame_pop(void);
int bl_cam_frame_pop_old(void);
int bl_cam_frame_wait(void);
int bl_cam_yuv_frame_wait(void);
int bl_cam_yuv_frame_get(uint32_t *frames, uint8_t **ptr1, uint32_t *len1, uint8_t **ptr2, uint32_t *len2);
int bl_cam_yuv_frame_pop(void);
int bl_cam_mjpeg_encoder(uint32_t yuv_addr, uint32_t jpeg_addr, uint32_t *jpeg_size,  uint32_t width, uint32_t height, uint32_t quality);
#endif
