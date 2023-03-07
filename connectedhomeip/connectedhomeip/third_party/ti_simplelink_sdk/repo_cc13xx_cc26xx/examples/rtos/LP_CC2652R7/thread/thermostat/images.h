/******************************************************************************

 @file images.h

 @brief Graphics image data definition.

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef _IMAGES_H_
#define _IMAGES_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 Includes
 *****************************************************************************/

/* grlib header defines `NDEBUG`, undefine here to avoid a compile warning */
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <ti/grlib/grlib.h>

/******************************************************************************
 Constants and Definitions
 *****************************************************************************/
/* number of sprites to be displayed on the sprite list */
#define SPRITE_COUNT 8
/* setpoint font sprite defines */
#define SETPT_FONT_Y 102
#define SETPT_FONT_X 48
#define SETPT_FONT_W 16
#define SETPT_FONT_H 19
#define SETPT_FONT_OFFSET (SETPT_FONT_W/8)*SETPT_FONT_H
/* temperature font sprite defines */
#define TEMP_FONT_Y 25
#define TEMP_FONT_X 24
#define TEMP_FONT_W 40
#define TEMP_FONT_H 51
#define TEMP_FONT_OFFSET (TEMP_FONT_W/8)*TEMP_FONT_H
/* mode icon sprite defines */
#define MODE_ICON_W 32
#define MODE_ICON_H 33
#define MODE_ICON_OFFSET (MODE_ICON_W/8)*MODE_ICON_H
/* animation end frames defines */
#define COOL_ANIMATION_END 3
#define HEAT_ANIMATION_END 6

/* font buffer for the temperature display */
extern const uint8_t thermostatTempFontPix[];
/* font buffer for the setpoint display */
extern const uint8_t thermostatSetPtFontPix[];
/* font buffer for the mode icon display */
extern const uint8_t thermostatModePix[];

/* Thermostat sprite list, for use by graphicext  */
extern GraphicExt_sprite_t thermostatSpriteList[SPRITE_COUNT];
/* Thermostat background frames, for use by graphicext  */
extern const Graphics_Image thermostatBackgroundImage[];

#ifdef __cplusplus
}
#endif

#endif /* _IMAGEG_H_ */
