/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file provides the wrapper utilities used between openthread stack and radio Platform
 *   Abstraction Layer.
 */

#pragma once
//=================================================================================================
//  Includes
//=================================================================================================
#include <openthread/platform/radio.h>

//=================================================================================================
//  Macros and Literal Definitions
//=================================================================================================

//=================================================================================================
//  Type Definitions and Enums
//=================================================================================================

//=================================================================================================
//  Structure
//=================================================================================================

//=================================================================================================
//  Function Declaration
//=================================================================================================
/**
 * \brief       Get start index of frame auxiliary security header.
 *
 * @param[in]   otRadioFrame *aFrame (I) - pointer to the frame
 *
 * @retval      start index of the frame auxiliary security header
 *
 */
uint8_t radio_wrapper_mac_frame_security_header_idx_get(otRadioFrame *aFrame);

/**
 * \brief       Get length of frame auxiliary security header.
 *
 * @param[in]   otRadioFrame *aFrame (I) - pointer to the frame
 *
 * @retval      length of auxiliary security header in the frame
 *
 */
uint8_t radio_wrapper_mac_frame_security_header_length_get(otRadioFrame *aFrame);

/**
 * \brief       Get start index of frame payload.
 *
 * @param[in]   otRadioFrame *aFrame (I) - pointer to the frame
 *
 * @retval      start index of the frame payload
 *
 */
uint8_t radio_wrapper_mac_frame_payload_index_get(otRadioFrame *aFrame);

/**
 * \brief       Get lenght of frame payload.
 *
 * @param[in]   otRadioFrame *aFrame (I) - pointer to the frame
 *
 * @retval      length of the frame payload
 *
 */
uint16_t radio_wrapper_mac_frame_payload_length_get(otRadioFrame *aFrame);

/**
 * \brief       Get key id mode in the frame.
 *
 * @param[in]   otRadioFrame *aFrame (I) - pointer to the frame
 *
 * @retval      key id mode of the frame
 *
 */
uint8_t radio_wrapper_mac_frame_key_id_mode_get(otRadioFrame *aFrame);

//=================================================================================================
//  End of File (radio_wrapper.h)
//=================================================================================================
