/******************************************************************************

 @file graphicext.h

 @brief Graphics extension APIs used for animations.

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
#ifndef _GRAPHICEXT_H_
#define _GRAPHICEXT_H_

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

#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/grlib/grlib.h>

/******************************************************************************
 Constants and Definitions
 *****************************************************************************/
/* typedef to define a sprite with X,Y position and an image */
typedef struct
{
    uint8_t             posX;
    uint8_t             posY;
    Graphics_Image      *image;
}GraphicExt_sprite_t;

/******************************************************************************
 External functions
 *****************************************************************************/
/**
 * @brief Set up the animation timer.
 *
 * @param lcdHandle       handle for the LCD display
 * @param frameDuration   time in milliseconds per each frame.
 *
 * @return None
 */
void GraphicExt_initAnimationModule(Display_Handle *lcdHandle,
                                    uint32_t frameDuration);

/**
 * @brief Displays an animation by switching through different frames.
 *
 * @param frames      array of frames to be displayed.
 * @param startFrame  index of the initial frame to start the animation.
 * @param endFrame    index of the last frame to be displayed.
 * @param continuous  if true it will keep displaying the animation until
 *                    GraphicExt_stopAnimate is called, otherwise the animation
 *                    will be displayed once.
 * @return None
 */
void GraphicExt_animateBackground(Graphics_Image *frames, uint8_t startFrame,
                        uint8_t endFrame, bool continuous);

/**
 * @brief Stops a continuous animation.
 *
 * @return None
 */
void GraphicExt_stopAnimate(void);

/**
 * @brief Draws a list of sprites on the screen.
 *
 * @param spriteList         list of sprites to be drawn on screen.
 * @param spriteListLen      length of sprite list.
 * @param overlayBackground  if true it will draw the sprites on top of the
 *                           current background set by
 *                           GraphicExt_animateBackground, otherwise the
 *                           background will be white.
 *
 * @return None
 */
void GraphicExt_drawSprites(GraphicExt_sprite_t *spriteList,
                            uint8_t spriteListLen,
                            bool overlayBackground);

/**
 * @brief Request processing of event.
 *
 * Used to indicate to the application that @ref GraphicExt_processEvt should
 * be called. This is usually called when the frame draw timeout expires.
 */
void GraphicExt_requestProcess(void);

/**
 * @brief Process an animation event.
 *
 * @return None
 */
void GraphicExt_processEvt(void);

#ifdef __cplusplus
}
#endif

#endif /* _GRAPHICEXT_H_ */
