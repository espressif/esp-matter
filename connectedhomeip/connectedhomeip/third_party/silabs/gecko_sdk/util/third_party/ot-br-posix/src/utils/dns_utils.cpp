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

#include "utils/dns_utils.hpp"

#include <assert.h>

#include "common/code_utils.hpp"

namespace otbr {

namespace DnsUtils {

std::string UnescapeInstanceName(const std::string &aName)
{
    std::string newName;
    auto        nameLen = aName.length();

    newName.reserve(nameLen);

    for (unsigned int i = 0; i < nameLen; i++)
    {
        char c = aName[i];

        if (c == '\\')
        {
            if (i + 3 < nameLen && isdigit(aName[i + 1]) && isdigit(aName[i + 2]) && isdigit(aName[i + 3]))
            {
                uint8_t b = (aName[i + 1] - '0') * 100 + (aName[i + 2] - '0') * 10 + (aName[i + 3] - '0');

                newName.push_back(b);
                i += 3;
                continue;
            }

            if (i + 1 < nameLen)
            {
                newName.push_back(aName[i + 1]);
                i += 1;
                continue;
            }
        }

        // append all not escaped characters
        newName.push_back(c);
    }

    return newName;
}

void CheckHostnameSanity(const std::string &aHostName)
{
    OTBR_UNUSED_VARIABLE(aHostName);

    assert(aHostName.length() > 0);
    assert(aHostName.back() == '.');
}

void CheckServiceNameSanity(const std::string &aServiceName)
{
    size_t dotpos;

    OTBR_UNUSED_VARIABLE(aServiceName);
    OTBR_UNUSED_VARIABLE(dotpos);

    assert(aServiceName.length() > 0);
    assert(aServiceName[aServiceName.length() - 1] != '.');
    dotpos = aServiceName.find_first_of('.');
    assert(dotpos != std::string::npos);
    assert(dotpos == aServiceName.find_last_of('.'));
}

} // namespace DnsUtils

} // namespace otbr
