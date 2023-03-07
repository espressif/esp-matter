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
 ============================================================================*/

#ifndef __QCBOR__qcbort_decode_tests__
#define __QCBOR__qcbort_decode_tests__

#include <stdint.h>

/*
 Notes:

 - All the functions in qcbor.h are called once in the aggregation of all the tests below.

 - All the types that are supported are given as input and parsed by these tests

 - There is some hostile input such as invalid lengths and CBOR too complex
   and types this parser doesn't handle

 */


/*
 Parse a well-known set of integers including those around the boundaries and
 make sure the expected values come out
 */
int32_t IntegerValuesParseTest(void);


/*
 Decode a simple CBOR encoded array and make sure it returns all the correct values.
 This is a decode test.
 */
int32_t SimpleArrayTest(void);


/*
 Tests with empty maps and arrays
 */
int32_t EmptyMapsAndArraysTest(void);


/*
 Make sure a maximally deep array can be parsed and that the
 reported nesting level is correct.  This uses test vector
 of CBOR encoded data with a depth of 10.  This a parse test.
 */
int32_t ParseDeepArrayTest(void);


/*
 See that the correct error is reported when parsing
 an array of depth 11, one too large.
 */
int32_t ParseTooDeepArrayTest(void);


/*
  Try to parse some legit CBOR types that this parsers
  doesn't support.
 */
int32_t UnsupportedCBORDecodeTest(void);


/*
  This takes the encoded CBOR integers used in the above test and parses
  it over and over with one more byte less each time. It should fail
  every time on incorrect CBOR input. This is a hostile input decode test.
 */
int32_t ShortBufferParseTest(void);


/*
   Same as ShortBufferParseTest, but with a different encoded CBOR input.
   It is another hostile input test
 */
int32_t ShortBufferParseTest2(void);


/*
  Parses the somewhat complicated CBOR MAP and makes sure all the correct
  values parse out.  About 15 values are tested. This is a decode test.
 */
int32_t ParseMapTest(void);


/*
Test the decoder mode where maps are treated as arrays.
 */
int32_t ParseMapAsArrayTest(void);


/*
 Test parsing of some simple values like true, false, null...
 */
int32_t ParseSimpleTest(void);


/*
 This tests all the not-well-formed CBOR from the CBOR RFC.
 (This is the CBORbis RFC which is not yet published at the
 time this test was added).
 */
int32_t NotWellFormedTests(void);


/*
 Tests a number of failure cases on bad CBOR to get the right error code
 */
int32_t DecodeFailureTests(void);


/*
 Parses all possible inputs that are two bytes long. Main point
 is that the test doesn't crash as it doesn't evaluate the
 input for correctness in any way.

 (Parsing all possible 3 byte strings takes too long on all but
  very fast machines).
 */
int32_t ComprehensiveInputTest(void);


/*
 Parses all possible inputs that are four bytes long. Main point
 is that the test doesn't crash as it doesn't evaluate the
 input for correctness in any way. This runs very slow, so it
 is only practical as a once-in-a-while regression test on
 fast machines.
 */
int32_t BigComprehensiveInputTest(void);


/*
 Thest the date types -- epoch and strings
 */
int32_t DateParseTest(void);


/*
  Test optional tags like the CBOR magic number.
 */
int32_t OptTagParseTest(void);


/*
 Parse some big numbers, positive and negative
 */
int32_t BignumParseTest(void);


/*
 Test of mode where only string labels are allowed
 */
int32_t StringDecoderModeFailTest(void);


/*
 Parse some nested maps
 */
int32_t NestedMapTest(void);


/*
 Parse maps with indefinite lengths
 */
int32_t NestedMapTestIndefLen(void);


/*
 Parse some maps and arrays with indefinite lengths.
 Includes some error cases.
 */
int32_t IndefiniteLengthArrayMapTest(void);


/*
 Parse indefinite length strings. Uses
 MemPool. Includes error cases.
 */
int32_t IndefiniteLengthStringTest(void);


/*
 Test deep nesting of indefinite length
 maps and arrays including too deep.
 */
int32_t IndefiniteLengthNestTest(void);


/*
 Test parsing strings were all strings, not
 just indefinite length strings, are
 allocated. Includes error test cases.
 */
int32_t AllocAllStringsTest(void);


/*
 Direct test of MemPool string allocator
 */
int32_t MemPoolTest(void);


/*
 Test the setting up of an external string allocator.
 */
int32_t SetUpAllocatorTest(void);


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
/*
 Test decoding of decimal fractions and big floats, both of which are
 made up of an exponent and mantissa.
 */
int32_t ExponentAndMantissaDecodeTests(void);


/*
 Hostile input tests for decimal fractions and big floats.
 */
int32_t ExponentAndMantissaDecodeFailTests(void);
#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */


#endif /* defined(__QCBOR__qcbort_decode_tests__) */
