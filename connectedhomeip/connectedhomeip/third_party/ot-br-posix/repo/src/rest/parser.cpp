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

#include "rest/parser.hpp"

#include <string>
#include <vector>

namespace otbr {
namespace rest {

static int OnUrl(http_parser *parser, const char *at, size_t len)
{
    Request *request = reinterpret_cast<Request *>(parser->data);

    if (len > 0)
    {
        request->SetUrl(at, len);
    }

    return 0;
}

static int OnBody(http_parser *parser, const char *at, size_t len)
{
    Request *request = reinterpret_cast<Request *>(parser->data);

    if (len > 0)
    {
        request->SetBody(at, len);
    }

    return 0;
}

static int OnMessageComplete(http_parser *parser)
{
    Request *request = reinterpret_cast<Request *>(parser->data);

    request->SetReadComplete();

    return 0;
}

static int OnMessageBegin(http_parser *parser)
{
    Request *request = reinterpret_cast<Request *>(parser->data);
    request->ResetReadComplete();

    return 0;
}

static int OnHeaderComplete(http_parser *parser)
{
    Request *request = reinterpret_cast<Request *>(parser->data);
    request->SetMethod(parser->method);
    return 0;
}

static int OnHandlerData(http_parser *, const char *, size_t)
{
    return 0;
}

Parser::Parser(Request *aRequest)
{
    mParser.data = aRequest;
}

void Parser::Init(void)
{
    mSettings.on_message_begin    = OnMessageBegin;
    mSettings.on_url              = OnUrl;
    mSettings.on_status           = OnHandlerData;
    mSettings.on_header_field     = OnHandlerData;
    mSettings.on_header_value     = OnHandlerData;
    mSettings.on_body             = OnBody;
    mSettings.on_headers_complete = OnHeaderComplete;
    mSettings.on_message_complete = OnMessageComplete;
    http_parser_init(&mParser, HTTP_REQUEST);
}

void Parser::Process(const char *aBuf, size_t aLength)
{
    http_parser_execute(&mParser, &mSettings, aBuf, aLength);
}

} // namespace rest
} // namespace otbr
