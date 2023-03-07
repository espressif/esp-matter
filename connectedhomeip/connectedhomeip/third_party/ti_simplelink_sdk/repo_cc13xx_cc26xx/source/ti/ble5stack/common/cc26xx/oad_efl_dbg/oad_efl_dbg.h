/******************************************************************************

 @file  oad_efl_dbg.h

 @brief This module provides functionality for debugging external flash via UART
        for off-chip OAD applications.

 Group: WCS, BTS
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

#ifndef OAD_EFL_DBG_H
#define OAD_EFL_DBG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      OadEflDbg_displayMetadata
 *
 * @brief   Display external flash metadata
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool OadEflDbg_displayMetadata(uint8_t index);
 
/*********************************************************************
 * @fn      OadEflDbg_displayMetaList
 *
 * @brief   Displays external flash metadata list
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool OadEflDbg_displayMetaList(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_eraseAll
 *
 * @brief   Erase all images and their meta pages
 *
 * @param   index - menu item index
 *
 * @return  None
 */
extern bool  OadEflDbg_eraseAll(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_eraseSingle
 *
 * @brief   Erase selected meta pages
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool  OadEflDbg_eraseImg(uint8_t index);

/*********************************************************************
 * @fn      OadEflDbg_createFactoryImg
 *
 * @brief   Copies on-chip flash image to create factory image
 *
 * @param   index - item index
 *
 * @return  true - on success
 */
extern bool OadEflDbg_createFactoryImg(uint8_t index);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OAD_EFL_DBG_H */

