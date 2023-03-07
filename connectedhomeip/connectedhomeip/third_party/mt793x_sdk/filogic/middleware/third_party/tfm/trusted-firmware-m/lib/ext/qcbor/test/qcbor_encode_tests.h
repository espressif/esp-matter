/*==============================================================================
 Copyright (c) 2016-2018, The Linux Foundation.
 Copyright (c) 2018-2020, Laurence Lundblade.
 All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors, nor the name "Laurence Lundblade" may be used to
      endorse or promote products derived from this software without
      specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =============================================================================*/

#ifndef __QCBOR__qcbor_encode_tests__
#define __QCBOR__qcbor_encode_tests__

#include <stdint.h>

/*
 Notes:

 - All the functions in qcbor.h are called once in the aggregation of all
   the tests below.

 - All the types that are supported are given as input and parsed by these tests

 - There is some hostile input such as invalid lengths and CBOR too complex
   and types this parser doesn't handle

 */


/*
 Most basic test.
 */
int32_t BasicEncodeTest(void);


/*
 Encode lots of integer values, particularly around the boundary and
 make sure they Match the expected binary output. Primarily an
 encoding test.
 */
int32_t IntegerValuesTest1(void);


/*
 Create nested arrays to the max depth allowed and make sure it
 succeeds.  This is an encoding test.
 */
int32_t ArrayNestingTest1(void);


/*
 Create nested arrays to one more than the meax depth and make sure it
 fails.  This is an encoding test.
 */
int32_t ArrayNestingTest2(void);


/*
 Encoding test.  Create arrays to max depth and close one extra time
 and look for correct error code
 */
int32_t ArrayNestingTest3(void);


/*
 This tests the QCBOREncode_AddRaw() function by adding two chunks or
 RAWCBOR to an array and comparing with expected values. This is an
 encoding test.
 */
int32_t EncodeRawTest(void);


/*
 This creates a somewhat complicated CBOR MAP and verifies it against
 expected data. This is an encoding test.
 */
int32_t MapEncodeTest(void);


/*
 Encodes a goodly number of floats and doubles and checks encoding is right
 */
int32_t FloatValuesTest1(void);


/*
 Encodes true, false and the like
 */
int32_t SimpleValuesTest1(void);


/*
 Encodes basic maps and arrays with indefinite length
 */
int32_t SimpleValuesIndefiniteLengthTest1(void);


/*
 Indefinite length arrays and maps use the 'magic' number 31, verify
 that everything with length 31 still works properly
 */
int32_t EncodeLengthThirtyoneTest(void);


/*
 Encodes most data formats that are supported */
int32_t EncodeDateTest(void);


/*
 Encodes particular data structure that a particular app will need...
 */
int32_t RTICResultsTest(void);


/*
 Calls all public encode methods in qcbor.h once.
 */
int32_t AllAddMethodsTest(void);

/*
 The binary string wrapping of maps and arrays used by COSE
 */
int32_t  BstrWrapTest(void);


/*
 Test error cases for bstr wrapping encoding such as closing an open
 array with CloseBstrWrap
 */
int32_t BstrWrapErrorTest(void);


/*
 Test complicated nested bstr wrapping
 */
int32_t BstrWrapNestTest(void);


/*
 Test encoding a COSE_Sign1 with bstr wrapping
 */
int32_t CoseSign1TBSTest(void);


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
/*
 Test encoding of decimal fractions and big floats, both of which are
 made up of an exponent and mantissa
 */
int32_t ExponentAndMantissaEncodeTests(void);
#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */

/*
 Test the error cases when encoding CBOR such as buffer too large,
 buffer too small, array nesting too deep. Aims to cover the error
 codes returned when encoding CBOR
 */
int32_t EncodeErrorTests(void);


#endif /* defined(__QCBOR__qcbor_encode_tests__) */
