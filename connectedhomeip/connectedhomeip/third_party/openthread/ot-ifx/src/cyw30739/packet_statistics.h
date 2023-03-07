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
 *   This file defines the related parameters used for packet statistics.
 */

#pragma once
//=================================================================================================
//  Includes
//=================================================================================================
#include <stdio.h>

//=================================================================================================
//  Macros and Literal Definitions
//=================================================================================================

//=================================================================================================
//  Type Definitions and Enums
//=================================================================================================

//=================================================================================================
//  Structure
//=================================================================================================
typedef struct
{
    struct
    {
        uint32_t num;
        struct
        {
            uint32_t success;
            uint32_t no_ack;
            uint32_t channel_access_failure;
        } status;
    } tx;

    struct
    {
        uint32_t num;
    } rx;
} PACKET_STATISTICS_t;

//=================================================================================================
//  Function Declaration
//=================================================================================================
/**
 * \brief           Get current packet statistics.
 *
 * @param[in/out]   p_statistics - memory block to write current statistics.
 *
 * @retval          None
 *
 */
void otPlatRadioPacketStatisticsGet(PACKET_STATISTICS_t *p_statistics);

//=================================================================================================
//	End of File (packet_statistics.h)
//=================================================================================================
