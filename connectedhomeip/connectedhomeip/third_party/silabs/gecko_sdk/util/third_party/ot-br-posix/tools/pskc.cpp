/*
 *    Copyright (c) 2017-2018, The OpenThread Authors.
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

#include <stdio.h>
#include <sysexits.h>

#include "common/code_utils.hpp"
#include "utils/hex.hpp"
#include "utils/pskc.hpp"

/**
 * Constants.
 */
enum
{
    kMaxNetworkName = 16,
    kMaxPassphrase  = 255,
    kSizeExtPanId   = 8,
};

void help(void)
{
    printf("pskc - compute PSKc\n"
           "SYNTAX:\n"
           "    pskc <PASSPHRASE> <EXTPANID> <NETWORK_NAME>\n"
           "EXAMPLE:\n"
           "    pskc 654321 1122334455667788 OpenThread\n");
}

int printPSKc(const char *aPassphrase, const char *aExtPanId, const char *aNetworkName)
{
    uint8_t extpanid[kSizeExtPanId];
    size_t  length;
    int     ret = -1;

    otbr::Psk::Pskc pskcComputer;
    const uint8_t * pskc;

    length = strlen(aPassphrase);
    VerifyOrExit(length > 0, printf("PASSPHRASE must not be empty.\n"));
    VerifyOrExit(length <= kMaxPassphrase,
                 printf("PASSPHRASE Passphrase must be no more than %d bytes.\n", kMaxPassphrase));

    length = strlen(aExtPanId);
    VerifyOrExit(length == kSizeExtPanId * 2, printf("EXTPANID length must be %d bytes.\n", kSizeExtPanId));
    for (size_t i = 0; i < length; i++)
    {
        VerifyOrExit((aExtPanId[i] <= '9' && aExtPanId[i] >= '0') || (aExtPanId[i] <= 'f' && aExtPanId[i] >= 'a') ||
                         (aExtPanId[i] <= 'F' && aExtPanId[i] >= 'A'),
                     printf("EXTPANID must be encoded in hex.\n"));
    }
    otbr::Utils::Hex2Bytes(aExtPanId, extpanid, sizeof(extpanid));

    length = strlen(aNetworkName);
    VerifyOrExit(length > 0, printf("NETWORK_NAME must not be empty.\n"));
    VerifyOrExit(length <= kMaxNetworkName,
                 printf("NETWOR_KNAME length must be no more than %d bytes.\n", kMaxNetworkName));

    pskc = pskcComputer.ComputePskc(extpanid, aNetworkName, aPassphrase);
    for (int i = 0; i < 16; i++)
    {
        printf("%02x", pskc[i]);
    }
    printf("\n");
    ret = 0;

exit:
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    VerifyOrExit(argc == 4, help(), ret = EX_USAGE);
    ret = printPSKc(argv[1], argv[2], argv[3]);

exit:
    return ret;
}
