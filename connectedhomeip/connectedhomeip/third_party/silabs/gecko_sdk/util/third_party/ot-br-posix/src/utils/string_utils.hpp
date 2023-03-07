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
 * This file includes definition for string utilities.
 */

#ifndef OTBR_UTILS_STRING_UTILS_HPP_
#define OTBR_UTILS_STRING_UTILS_HPP_

#include "openthread-br/config.h"

#include <string.h>
#include <string>

namespace otbr {

namespace StringUtils {

/**
 * This function compares two strings in a case-insensitive manner.
 *
 * @param[in] aString1 The first string.
 * @param[in] aString2 The second string.
 *
 * @returns  Whether the two strings are equal in a case-insensitive manner.
 *
 */
bool EqualCaseInsensitive(const std::string &aString1, const std::string &aString2);

/**
 * This function converts a given string to lowercase.
 *
 * @param[in] aString The string to convert.
 *
 * @returns  A copy of @p aString with all letters converted to lowercase.
 *
 */
std::string ToLowercase(const std::string &aString);

} // namespace StringUtils

} // namespace otbr

#endif // OTBR_UTILS_STRING_UTILS_HPP_
