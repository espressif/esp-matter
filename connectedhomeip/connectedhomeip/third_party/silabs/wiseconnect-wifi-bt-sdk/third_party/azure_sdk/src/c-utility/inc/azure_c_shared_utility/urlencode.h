// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef URLENCODE_H
#define URLENCODE_H

#include "azure_c_shared_utility/strings.h"
#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* @brief   URL Encode (aka percent encode) a string.
    * Please note that while the URL encoder accepts the full range of 8-bit extended ASCII,
    * it has unpredictable behavior beyond the 7-bit ASCII standard. This function does NOT
    * guarantee correctness of output for characters outside this range.
    *
    * @param    URL Encode can be done on a const char* or a STRING_HANDLE, use the appropriate
    * function depending on input type, they both behave the same way.
    *
    * @return   Returns a STRING_HANDLE with the encoded string, or NULL on failure.
    */
    MOCKABLE_FUNCTION(, STRING_HANDLE, URL_Encode, STRING_HANDLE, input);
    MOCKABLE_FUNCTION(, STRING_HANDLE, URL_EncodeString, const char*, textEncode);

    /* @brief   URL Decode (aka percent decode) a string.
    * Please note that the URL decoder only supports decoding characters that fall within the
    * 7-bit ASCII range. It does NOT support 8-bit extended ASCII, and will fail if you try.
    *
    * @param    URL Decode can be done on a const char* or a STRING_HANDLE, use the appropriate
    * function depending on input type, they both behave the same way.
    *
    * @return   Returns a STRING_HANDLE with the decoded string, or NULL on failure.
    */
    MOCKABLE_FUNCTION(, STRING_HANDLE, URL_Decode, STRING_HANDLE, input);
    MOCKABLE_FUNCTION(, STRING_HANDLE, URL_DecodeString, const char*, textDecode);

#ifdef __cplusplus
}
#endif

#endif /* URLENCODE_H */

