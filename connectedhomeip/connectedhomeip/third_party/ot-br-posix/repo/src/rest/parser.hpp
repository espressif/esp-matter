/*
 *  Copyright (c) 2020, The OpenThread Authors.
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
 *   This file includes parser definition for RESTful HTTP server.
 */

#ifndef OTBR_REST_PARSER_HPP_
#define OTBR_REST_PARSER_HPP_

#include <memory>

#include "rest/types.hpp"

extern "C" {
#include <http_parser.h>
}

#include "rest/request.hpp"

namespace otbr {
namespace rest {

/**
 * This class implements Parser class in OTBR-REST which is used to parse the data from read buffer and form a request.
 *
 */
class Parser
{
public:
    /**
     * The constructor of a http request parser instance.
     *
     * @param[in] aRequest  A pointer to a request instance.
     *
     */
    Parser(Request *aRequest);

    /**
     * This method initializea the http-parser.
     *
     */
    void Init(void);

    /**
     * This method performs a parse process.
     *
     * @param[in] aBuf     A pointer pointing to read buffer.
     * @param[in] aLength  An integer indicates how much data is to be processed by parser.
     *
     */
    void Process(const char *aBuf, size_t aLength);

private:
    http_parser          mParser;
    http_parser_settings mSettings;
};

} // namespace rest
} // namespace otbr

#endif // OTBR_REST_PARSER_HPP_
