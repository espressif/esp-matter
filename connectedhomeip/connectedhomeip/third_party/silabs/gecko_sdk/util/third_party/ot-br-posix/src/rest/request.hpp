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
 *   This file includes request definition for RESTful HTTP server.
 */

#ifndef OTBR_REST_REQUEST_HPP_
#define OTBR_REST_REQUEST_HPP_

#include <string>
#include <vector>

#include "common/code_utils.hpp"
#include "rest/types.hpp"

namespace otbr {
namespace rest {

/**
 * This class implements an instance to host services used by border router.
 *
 */
class Request
{
public:
    /**
     * The constructor is to initialize Request instance.
     *
     */
    Request(void);

    /**
     * This method sets the Url field of a request.
     *
     * @param[in] aString  A pointer points to url string.
     * @param[in] aLength  Length of the url string
     *
     */
    void SetUrl(const char *aString, size_t aLength);

    /**
     * This method sets the body field of a request.
     *
     * @param[in] aString  A pointer points to body string.
     * @param[in] aLength  Length of the body string
     *
     */
    void SetBody(const char *aString, size_t aLength);

    /**
     * This method sets the content-length field of a request.
     *
     * @param[in] aContentLength  An unsigned integer representing content-length.
     *
     */
    void SetContentLength(size_t aContentLength);

    /**
     * This method sets the method of the parsed request.
     *
     * @param[in] aMethod  An integer representing request method.
     *
     */
    void SetMethod(int32_t aMethod);

    /**
     * This method labels the request as complete which means it no longer need to be parsed one more time .
     *
     */
    void SetReadComplete(void);

    /**
     * This method resets the request then it could be set by parser from start.
     *
     */
    void ResetReadComplete(void);

    /**
     * This method returns the HTTP method of this request.
     *
     * @returns A integer representing HTTP method.
     */
    HttpMethod GetMethod() const;

    /**
     * This method returns the HTTP method of this request.
     *
     * @returns A HttpMethod enum representing HTTP method of this request.
     */
    std::string GetBody() const;

    /**
     * This method returns the url for this request.
     *
     * @returns A string contains the url of this request.
     */
    std::string GetUrl(void) const;

    /**
     * This method indicates whether this request is parsed completely.
     *
     *
     */
    bool IsComplete(void) const;

private:
    int32_t     mMethod;
    size_t      mContentLength;
    std::string mUrl;
    std::string mBody;
    bool        mComplete;
};

} // namespace rest
} // namespace otbr

#endif // OTBR_REST_REQUEST_HPP_
