/*
 * Copyright (c) 2014-2019 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell
 * ("Utilize") this software subject to the terms herein. With respect to the
 * foregoing patent license, such license is granted solely to the extent that
 * any such patent is necessary to Utilize the software alone. The patent
 * license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI
 * ("TI Devices"). No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce
 * this license (including the above copyright notice and the disclaimer and
 * (if applicable) source code license limitations below) in the documentation
 * and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted
 * provided that the following conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is
 *   permitted with respect to any software provided in binary form.
 *
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * * Nothing shall obligate TI to provide you with source code for the software
 *   licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution
 * of the source code are permitted provided that the following conditions are
 * met:
 *
 * * any redistribution and use of the source code, including any resulting
 *   derivative works, are licensed by TI for use only with TI Devices.
 *
 * * any redistribution and use of any object code compiled from the source
 *   code and any resulting derivative works, are licensed by TI for use only
 *   with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its
 * suppliers may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** ============================================================================
 *  @file       Codec1.h
 *
 *  @brief      Implementation of TI Codec Type 1 (IMA ADPCM) encoding and decoding.
 *
 *  This module provides functions for encoding and decoding data using ADPCM encoding.
 *
 *  ============================================================================
 */

#ifndef ti_drivers_pdm_Codec1__include
#define ti_drivers_pdm_Codec1__include

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  This routine encodes one int16 sample with TI Codec Type 1.
 *
 *  @param  audSample  The audtion sample to encode
 *
 *  @return The encoded result as a 4-bit nibble
 */
extern uint8_t Codec1_encodeSingle(int16_t audSample);


/*!
 *  @brief  This routine decode a 4-bit nibble sample to a uint16 PCM audio sample.
 *
 *  @param  nibble_4bits  A 4-bit nibble to decode
 *
 *  @return The decoded value as a 16-bit PCM sample
 */
extern int16_t Codec1_decodeSingle(uint8_t nibble_4bits);

/*!
 *  @brief  This routine encode a buffer with TI codec Type 1.
 *
 *  @param  dst Pointer to the buffer where encoding result will be written to
 *
 *  @param  src Pointer to the buffer that should be encoded.
 *              Must be a multiple of 4 bytes
 *
 *  @param  srcSize The number of samples (int16) in the src buffer.
 *                  Must be a multiple of 2.
 *
 *  @param  si Pointer to the current step index
 *
 *  @param  pv Pointer to the current predicted-value
 *
 *  @return Number of bytes written to the destination buffer
 */
extern uint8_t Codec1_encodeBuff(uint8_t* dst, int16_t* src, int16_t srcSize, int8_t *si, int16_t *pv);

/*!
 *  @brief  This routine decodes a buffer with TI codec Type 1.
 *
 *  @param  dst Pointer to the buffer where decoded result will be written to
 *
 *  @param  src Pointer to the buffer that should be decoded.
 *              Must be a multiple of 4 bytes
 *
 *  @param  srcSize Number of byte that will be generated by
 *                  the encoder (4* (src buffer size in byte))
 *
 *  @param  si Pointer to the current step index
 *
 *  @param  pv Pointer to the current predicted-value
 */
extern void Codec1_decodeBuff(int16_t* dst, uint8_t* src, unsigned srcSize,  int8_t *si, int16_t *pv);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_pdm_Codec1__include */
