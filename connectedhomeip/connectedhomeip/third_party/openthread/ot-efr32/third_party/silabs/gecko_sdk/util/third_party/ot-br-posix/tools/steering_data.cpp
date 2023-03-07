/*
 *    Copyright (c) 2018, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements a simple tool to compute pskc.
 */

#include <mbedtls/sha256.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

#include "common/code_utils.hpp"
#include "utils/hex.hpp"
#include "utils/steering_data.hpp"

void help(void)
{
    printf("steering-data - compute steering data\n"
           "SYNTAX:\n"
           "    steering-data [LENGTH] <JOINER_ID> ...\n"
           "EXAMPLE:\n"
           "    steering-data 18b4300000000001\n"
           "    steering-data 15 18b4300000000001\n"
           "    steering-data 18b4300000000001 18b4300000000002\n");
}

int ComputeJoinerId(const char *aEui64, uint8_t *aJoinerId)
{
    int ret = -1;

    VerifyOrExit(strlen(aEui64) == otbr::SteeringData::kSizeJoinerId * 2);
    VerifyOrExit(otbr::Utils::Hex2Bytes(aEui64, aJoinerId, otbr::SteeringData::kSizeJoinerId) ==
                 otbr::SteeringData::kSizeJoinerId);
    otbr::SteeringData::ComputeJoinerId(aJoinerId, aJoinerId);
    ret = 0;

exit:
    if (ret != 0)
    {
        fprintf(stderr, "Invalid EUI64: %s", aEui64);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    otbr::SteeringData computer;
    int                ret    = EX_USAGE;
    int                length = 16;
    int                i      = 1;

    if (argc < 2)
    {
        ExitNow(help());
    }

    if (strlen(argv[i]) != otbr::SteeringData::kSizeJoinerId * 2)
    {
        length = atoi(argv[i]);
        VerifyOrExit(length > 0 && length <= otbr::SteeringData::kMaxSizeOfBloomFilter,
                     fprintf(stderr, "Invalid bloom filter length: %d\n", length));

        ++i;
    }

    computer.Init(static_cast<uint8_t>(length));

    for (; i < argc; ++i)
    {
        uint8_t joinerId[otbr::SteeringData::kSizeJoinerId];

        VerifyOrExit(ComputeJoinerId(argv[i], joinerId) == 0, fprintf(stderr, "Invalid EUI64 : %s\n", argv[i]));
        computer.ComputeBloomFilter(joinerId);
    }

    for (i = 0; i < length; i++)
    {
        printf("%02x", computer.GetBloomFilter()[i]);
    }
    printf("\n");

    ret = EX_OK;

exit:
    return ret;
}
