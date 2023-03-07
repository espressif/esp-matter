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
 *   This file includes the initializers for supporting Security manager.
 *
 */

#ifndef SL_PACKET_HANDLER_H
#define SL_PACKET_HANDLER_H

#include <openthread/platform/radio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This function performs AES CCM on the frame which is going to be sent.
 *
 * @param[in]  aFrame       A pointer to the MAC frame buffer that is going to be sent.
 * @param[in]  aExtAddress  A pointer to the extended address, which will be used to generate nonce
 *                          for AES CCM computation.
 *
 */
void efr32PlatProcessTransmitAesCcm(otRadioFrame *aFrame, const otExtAddress *aExtAddress);

/**
 * This function returns if the Frame Pending bit is set in any given frame.
 *
 * @param[in]  aFrame       A pointer to the MAC frame buffer.
 *
 * @retval  true    Frame Pending is set.
 * @retval  false   Frame Pending is not set.
 */
bool efr32IsFramePending(otRadioFrame *aFrame);

/**
 * This function returns the Destination PanId, if present.
 *
 * @param[in]  aFrame       A pointer to the MAC frame buffer.
 *
 * @retval  DstPanId    If present.
 * @retval  BcastPanId  If Dest PanId is compressed.
 */
otPanId efr32GetDstPanId(otRadioFrame *aFrame);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* SL_PACKET_HANDLER_H */
