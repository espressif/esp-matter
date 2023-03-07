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

/*=============================================================================
 FILE:  qcbor_encode.c

 DESCRIPTION:  This file contains the implementation of QCBOR.

 EDIT HISTORY FOR FILE:

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when       who             what, where, why
 --------   ----            ---------------------------------------------------
 01/25/2020 llundblade      Refine use of integer types to quiet static analysis.
 01/08/2020 llundblade      Documentation corrections & improved code formatting.
 12/30/19   llundblade      Add support for decimal fractions and bigfloats.
 8/7/19     llundblade      Prevent encoding simple type reserved values 24..31
 7/25/19    janjongboom     Add indefinite length encoding for maps and arrays
 4/6/19     llundblade      Wrapped bstr returned now includes the wrapping bstr
 12/30/18   llundblade      Small efficient clever encode of type & argument.
 11/29/18   llundblade      Rework to simpler handling of tags and labels.
 11/9/18    llundblade      Error codes are now enums.
 11/1/18    llundblade      Floating support.
 10/31/18   llundblade      Switch to one license that is almost BSD-3.
 09/28/18   llundblade      Added bstr wrapping feature for COSE implementation.
 02/05/18   llundbla        Works on CPUs which require integer alignment.
                            Requires new version of UsefulBuf.
 07/05/17   llundbla        Add bstr wrapping of maps/arrays for COSE
 03/01/17   llundbla        More data types
 11/13/16   llundbla        Integrate most TZ changes back into github version.
 09/30/16   gkanike         Porting to TZ.
 03/15/16   llundbla        Initial Version.

 =============================================================================*/

#include "qcbor.h"
#include "ieee754.h"



/*
 Nesting -- This tracks the nesting of maps and arrays.

 The following functions and data type QCBORTrackNesting implement the
 nesting management for encoding.

 CBOR's two nesting types, arrays and maps, are tracked here. There is
 a limit of QCBOR_MAX_ARRAY_NESTING to the number of arrays and maps
 that can be nested in one encoding so the encoding context stays
 small enough to fit on the stack.

 When an array / map is opened, pCurrentNesting points to the element
 in pArrays that records the type, start position and accumulates a
 count of the number of items added. When closed the start position is
 used to go back and fill in the type and number of items in the array
 / map.

 Encoded output can be just items like ints and strings that are not
 part of any array / map. That is, the first thing encoded does not
 have to be an array or a map.

 QCBOR has a special feature to allow constructing bstr-wrapped CBOR
 directly into the output buffer, so an extra buffer for it is not
 needed.  This is implemented as nesting with type
 CBOR_MAJOR_TYPE_BYTE_STRING and uses this code. Bstr-wrapped CBOR is
 used by COSE for data that is to be hashed.
 */
inline static void Nesting_Init(QCBORTrackNesting *pNesting)
{
   // Assumes pNesting has been zeroed
   pNesting->pCurrentNesting = &pNesting->pArrays[0];
   // Implied CBOR array at the top nesting level. This is never returned,
   // but makes the item count work correctly.
   pNesting->pCurrentNesting->uMajorType = CBOR_MAJOR_TYPE_ARRAY;
}

inline static QCBORError Nesting_Increase(QCBORTrackNesting *pNesting,
                                          uint8_t uMajorType,
                                          uint32_t uPos)
{
   QCBORError nReturn = QCBOR_SUCCESS;

   if(pNesting->pCurrentNesting == &pNesting->pArrays[QCBOR_MAX_ARRAY_NESTING]) {
      // Trying to open one too many
      nReturn = QCBOR_ERR_ARRAY_NESTING_TOO_DEEP;
   } else {
      pNesting->pCurrentNesting++;
      pNesting->pCurrentNesting->uCount     = 0;
      pNesting->pCurrentNesting->uStart     = uPos;
      pNesting->pCurrentNesting->uMajorType = uMajorType;
   }
   return nReturn;
}

inline static void Nesting_Decrease(QCBORTrackNesting *pNesting)
{
   pNesting->pCurrentNesting--;
}

inline static QCBORError Nesting_Increment(QCBORTrackNesting *pNesting)
{
   if(1 >= QCBOR_MAX_ITEMS_IN_ARRAY - pNesting->pCurrentNesting->uCount) {
      return QCBOR_ERR_ARRAY_TOO_LONG;
   }

   pNesting->pCurrentNesting->uCount += 1;

   return QCBOR_SUCCESS;
}

inline static uint16_t Nesting_GetCount(QCBORTrackNesting *pNesting)
{
   // The nesting count recorded is always the actual number of individiual
   // data items in the array or map. For arrays CBOR uses the actual item
   // count. For maps, CBOR uses the number of pairs.  This function returns
   // the number needed for the CBOR encoding, so it divides the number of
   // items by two for maps to get the number of pairs.  This implementation
   // takes advantage of the map major type being one larger the array major
   // type, hence uDivisor is either 1 or 2.

   if(pNesting->pCurrentNesting->uMajorType == CBOR_MAJOR_TYPE_MAP) {
      // Cast back to uint16_t after integer promotion for bit shift
      return (uint16_t)(pNesting->pCurrentNesting->uCount >> 1);
   } else {
      return pNesting->pCurrentNesting->uCount;
   }
}

inline static uint32_t Nesting_GetStartPos(QCBORTrackNesting *pNesting)
{
   return pNesting->pCurrentNesting->uStart;
}

inline static uint8_t Nesting_GetMajorType(QCBORTrackNesting *pNesting)
{
   return pNesting->pCurrentNesting->uMajorType;
}

inline static bool Nesting_IsInNest(QCBORTrackNesting *pNesting)
{
   return pNesting->pCurrentNesting == &pNesting->pArrays[0] ? false : true;
}




/*
 Encoding of the major CBOR types is by these functions:

 CBOR Major Type    Public Function
 0                  QCBOREncode_AddUInt64()
 0, 1               QCBOREncode_AddUInt64(), QCBOREncode_AddInt64()
 2, 3               QCBOREncode_AddBuffer(), Also QCBOREncode_OpenMapOrArray(),
                    QCBOREncode_CloseMapOrArray()
 4, 5               QCBOREncode_OpenMapOrArray(), QCBOREncode_CloseMapOrArray(),
                    QCBOREncode_OpenMapOrArrayIndefiniteLength(),
                    QCBOREncode_CloseMapOrArrayIndefiniteLength()
 6                  QCBOREncode_AddTag()
 7                  QCBOREncode_AddDouble(), QCBOREncode_AddType7()

 Additionally, encoding of decimal fractions and bigfloats is by
 QCBOREncode_AddExponentAndMantissa()
*/

/*
 Error tracking plan -- Errors are tracked internally and not returned
 until QCBOREncode_Finish is called. The CBOR errors are in me->uError.
 UsefulOutBuf also tracks whether the buffer is full or not in its
 context.  Once either of these errors is set they are never
 cleared. Only QCBOREncode_Init() resets them. Or said another way, they must
 never be cleared or we'll tell the caller all is good when it is not.

 Only one error code is reported by QCBOREncode_Finish() even if there are
 multiple errors. The last one set wins. The caller might have to fix
 one error to reveal the next one they have to fix.  This is OK.

 The buffer full error tracked by UsefulBuf is only pulled out of
 UsefulBuf in Finish() so it is the one that usually wins.  UsefulBuf
 will never go off the end of the buffer even if it is called again
 and again when full.

 It is really tempting to not check for overflow on the count in the
 number of items in an array. It would save a lot of code, it is
 extremely unlikely that any one will every put 65,000 items in an
 array, and the only bad thing that would happen is the CBOR would be
 bogus.

 Since this does not parse any input, you could in theory remove all
 error checks in this code if you knew the caller called it
 correctly. Maybe someday CDDL or some such language will be able to
 generate the code to call this and the calling code would always be
 correct. This could also automatically size some of the data
 structures like array/map nesting resulting in some stack memory
 savings.

 The 8 errors returned here fall into three categories:

 Sizes
   QCBOR_ERR_BUFFER_TOO_LARGE        -- Encoded output exceeded UINT32_MAX
   QCBOR_ERR_BUFFER_TOO_SMALL        -- Output buffer too small
   QCBOR_ERR_ARRAY_NESTING_TOO_DEEP  -- Nesting > QCBOR_MAX_ARRAY_NESTING1
   QCBOR_ERR_ARRAY_TOO_LONG          -- Too many things added to an array/map

 Nesting constructed incorrectly
   QCBOR_ERR_TOO_MANY_CLOSES         -- More close calls than opens
   QCBOR_ERR_CLOSE_MISMATCH          -- Type of close does not match open
   QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN -- Finish called without enough closes

 Would generate not-well-formed CBOR
   QCBOR_ERR_UNSUPPORTED             -- Simple type between 24 and 31
 */


/*
 Public function for initialization. See header qcbor.h
 */
void QCBOREncode_Init(QCBOREncodeContext *me, UsefulBuf Storage)
{
   memset(me, 0, sizeof(QCBOREncodeContext));
   UsefulOutBuf_Init(&(me->OutBuf), Storage);
   Nesting_Init(&(me->nesting));
}


/**
 @brief Encode a data item, the most atomic part of CBOR

 @param[in,out] me      Encoding context including output buffer
 @param[in] uMajorType  One of CBOR_MAJOR_TYPE_XX
 @param[in] nMinLen     Include zero bytes up to this length. If 0 include
                        no zero bytes. Non-zero to encode floats and doubles.
 @param[in] uNumber     The number to encode, the argument.
 @param[in] uPos        The position in the output buffer (which is inside
                        the encoding context) to insert the result. This is
                        usually at the end, an append.

 All CBOR data items have a type and an "argument". The argument is
 either the value of the item for integer types, the length of the
 content for string, byte, array and map types, a tag for major type
 6, and has several uses for major type 7.

 This function encodes the type and the argument. There are several
 encodings for the argument depending on how large it is and how it is
 used.

 Every encoding of the type and argument has at least one byte, the
 "initial byte".

 The top three bits of the initial byte are the major type for the
 CBOR data item.  The eight major types defined by the standard are
 defined as CBOR_MAJOR_TYPE_xxxx in qcbor.h.

 The remaining five bits, known as "additional information", and
 possibly more bytes encode the argument. If the argument is less than
 24, then it is encoded entirely in the five bits. This is neat
 because it allows you to encode an entire CBOR data item in 1 byte
 for many values and types (integers 0-23, true, false, and tags).

 If the argument is larger than 24, then it is encoded in 1,2,4 or 8
 additional bytes, with the number of these bytes indicated by the
 values of the 5 bits 24, 25, 25 and 27.

 It is possible to encode a particular argument in many ways with this
 representation.  This implementation always uses the smallest
 possible representation. This conforms with CBOR preferred encoding.

 This function inserts them into the output buffer at the specified
 position. AppendEncodedTypeAndNumber() appends to the end.

 This function takes care of converting to network byte order.

 This function is also used to insert floats and doubles. Before this
 function is called the float or double must be copied into a
 uint64_t. That is how they are passed in. They are then converted to
 network byte order correctly. The uMinLen parameter makes sure that
 even if all the digits of a half, float or double are 0 it is still
 correctly encoded in 2, 4 or 8 bytes.
 */
static void InsertEncodedTypeAndNumber(QCBOREncodeContext *me,
                                       uint8_t             uMajorType,
                                       int                 nMinLen,
                                       uint64_t            uNumber,
                                       size_t              uPos)
{
   /*
    This code does endian conversion without hton or knowing the
    endianness of the machine using masks and shifts. This avoids the
    dependency on hton and the mess of figuring out how to find the
    machine's endianness.

    This is a good efficient implementation on little-endian machines.
    A faster and small implementation is possible on big-endian
    machines because CBOR/network byte order is big endian. However
    big endian machines are uncommon.

    On x86, it is about 200 bytes instead of 500 bytes for the more
    formal unoptimized code.

    This also does the CBOR preferred shortest encoding for integers
    and is called to do endian conversion for floats.

    It works backwards from the LSB to the MSB as needed.

    Code Reviewers: THIS FUNCTION DOES POINTER MATH
    */
   /*
    The type int is used here for several variables because of the way
    integer promotion works in C for integer variables that are
    uint8_t or uint16_t. The basic rule is that they will always be
    promoted to int if they will fit. All of these integer variables
    need only hold values less than 255 or are promoted from uint8_t,
    so they will always fit into an int. Note that promotion is only
    to unsigned int if the value won't fit into an int even if the
    promotion is for an unsigned like uint8_t.

    By declaring them int, there are few implicit conversions and fewer
    casts needed. Code size is reduced a little. It also makes static
    analyzers happier.

    Note also that declaring them uint8_t won't stop integer wrap
    around if the code is wrong. It won't make the code more correct.

    https://stackoverflow.com/questions/46073295/implicit-type-promotion-rules
    https://stackoverflow.com/questions/589575/what-does-the-c-standard-state-the-size-of-int-long-type-to-be
    */

   // Holds up to 9 bytes of type and argument plus one extra so pointer
   // always points to valid bytes.
   uint8_t bytes[sizeof(uint64_t)+2];
   // Point to the last bytes and work backwards
   uint8_t *pByte = &bytes[sizeof(bytes)-1];
   // This is the 5 bits in the initial byte that is not the major type
   int nAdditionalInfo;

   if (uMajorType == CBOR_MAJOR_NONE_TYPE_ARRAY_INDEFINITE_LEN) {
      uMajorType = CBOR_MAJOR_TYPE_ARRAY;
      nAdditionalInfo = LEN_IS_INDEFINITE;
   } else if (uMajorType == CBOR_MAJOR_NONE_TYPE_MAP_INDEFINITE_LEN) {
      uMajorType = CBOR_MAJOR_TYPE_MAP;
      nAdditionalInfo = LEN_IS_INDEFINITE;
   } else if (uNumber < CBOR_TWENTY_FOUR && nMinLen == 0) {
      // Simple case where argument is < 24
      nAdditionalInfo = (int)uNumber;
   } else if (uMajorType == CBOR_MAJOR_TYPE_SIMPLE && uNumber == CBOR_SIMPLE_BREAK) {
      // Break statement can be encoded in single byte too (0xff)
      nAdditionalInfo = (int)uNumber;
   } else  {
      /*
       Encode argument in 1,2,4 or 8 bytes. Outer loop runs once for 1
       byte and 4 times for 8 bytes.  Inner loop runs 1, 2 or 4 times
       depending on outer loop counter. This works backwards taking 8
       bits off the argument being encoded at a time until all bits
       from uNumber have been encoded and the minimum encoding size is
       reached.  Minimum encoding size is for floating-point numbers
       with zero bytes.
       */
      static const uint8_t aIterate[] = {1,1,2,4};
      int i;
      for(i = 0; uNumber || nMinLen > 0; i++) {
         const int nIterations = aIterate[i];
         for(int j = 0; j < nIterations; j++) {
            *--pByte = (uint8_t)(uNumber & 0xff);
            uNumber = uNumber >> 8;
         }
         nMinLen -= nIterations;
      }
      // Additional info is the encoding of the number of additional
      // bytes to encode argument.
      nAdditionalInfo = LEN_IS_ONE_BYTE-1 + i;
   }

   /*
    Expression integer-promotes to type int. The code above in
    function gaurantees that uAdditionalInfo will never be larger than
    0x1f. The caller may pass in a too-large uMajor type. The
    conversion to unint8_t will cause an integer wrap around and
    incorrect CBOR will be generated, but no security issue will
    incur.
    */
   *--pByte = (uint8_t)((uMajorType << 5) + nAdditionalInfo);

   /*
    Will not go negative because the loops run for at most 8
    decrements of pByte, only one other decrement is made and the
    array is sized for this.
    */
   const size_t uHeadLen = (size_t)(&bytes[sizeof(bytes)-1] - pByte);

   UsefulOutBuf_InsertData(&(me->OutBuf), pByte, uHeadLen, uPos);
}


/*
 Append the type and number info to the end of the buffer.

 See InsertEncodedTypeAndNumber() function above for details.
*/
inline static void AppendEncodedTypeAndNumber(QCBOREncodeContext *me,
                                              uint8_t uMajorType,
                                              uint64_t uNumber)
{
   // An append is an insert at the end.
   InsertEncodedTypeAndNumber(me,
                              uMajorType,
                              0,
                              uNumber,
                              UsefulOutBuf_GetEndPosition(&(me->OutBuf)));
}




/*
 Public functions for closing arrays and maps. See qcbor.h
 */
void QCBOREncode_AddUInt64(QCBOREncodeContext *me, uint64_t uValue)
{
   if(me->uError == QCBOR_SUCCESS) {
      AppendEncodedTypeAndNumber(me, CBOR_MAJOR_TYPE_POSITIVE_INT, uValue);
      me->uError = Nesting_Increment(&(me->nesting));
   }
}


/*
 Public functions for closing arrays and maps. See qcbor.h
 */
void QCBOREncode_AddInt64(QCBOREncodeContext *me, int64_t nNum)
{
   if(me->uError == QCBOR_SUCCESS) {
      uint8_t      uMajorType;
      uint64_t     uValue;

      if(nNum < 0) {
         // In CBOR -1 encodes as 0x00 with major type negative int.
         uValue = (uint64_t)(-nNum - 1);
         uMajorType = CBOR_MAJOR_TYPE_NEGATIVE_INT;
      } else {
         uValue = (uint64_t)nNum;
         uMajorType = CBOR_MAJOR_TYPE_POSITIVE_INT;
      }

      AppendEncodedTypeAndNumber(me, uMajorType, uValue);
      me->uError = Nesting_Increment(&(me->nesting));
   }
}


/*
 Semi-private function. It is exposed to user of the interface, but
 they will usually call one of the inline wrappers rather than this.

 See qcbor.h

 Does the work of adding actual strings bytes to the CBOR output (as
 opposed to numbers and opening / closing aggregate types).

 There are four use cases:
   CBOR_MAJOR_TYPE_BYTE_STRING -- Byte strings
   CBOR_MAJOR_TYPE_TEXT_STRING -- Text strings
   CBOR_MAJOR_NONE_TYPE_RAW -- Already-encoded CBOR
   CBOR_MAJOR_NONE_TYPE_BSTR_LEN_ONLY -- Special case

 The first two add the type and length plus the actual bytes. The
 third just adds the bytes as the type and length are presumed to be
 in the bytes. The fourth just adds the type and length for the very
 special case of QCBOREncode_AddBytesLenOnly().
 */
void QCBOREncode_AddBuffer(QCBOREncodeContext *me, uint8_t uMajorType, UsefulBufC Bytes)
{
   if(me->uError == QCBOR_SUCCESS) {
      // If it is not Raw CBOR, add the type and the length
      if(uMajorType != CBOR_MAJOR_NONE_TYPE_RAW) {
         uint8_t uRealMajorType = uMajorType;
         if(uRealMajorType == CBOR_MAJOR_NONE_TYPE_BSTR_LEN_ONLY) {
            uRealMajorType = CBOR_MAJOR_TYPE_BYTE_STRING;
         }
         AppendEncodedTypeAndNumber(me, uRealMajorType, Bytes.len);
      }

      if(uMajorType != CBOR_MAJOR_NONE_TYPE_BSTR_LEN_ONLY) {
         // Actually add the bytes
         UsefulOutBuf_AppendUsefulBuf(&(me->OutBuf), Bytes);
      }

      // Update the array counting if there is any nesting at all
      me->uError = Nesting_Increment(&(me->nesting));
   }
}


/*
 Public functions for closing arrays and maps. See qcbor.h
 */
void QCBOREncode_AddTag(QCBOREncodeContext *me, uint64_t uTag)
{
   AppendEncodedTypeAndNumber(me, CBOR_MAJOR_TYPE_OPTIONAL, uTag);
}


/*
 Semi-private function. It is exposed to user of the interface,
 but they will usually call one of the inline wrappers rather than this.

 See header qcbor.h
 */
void QCBOREncode_AddType7(QCBOREncodeContext *me, size_t uSize, uint64_t uNum)
{
   if(me->uError == QCBOR_SUCCESS) {
      if(uNum >= CBOR_SIMPLEV_RESERVED_START && uNum <= CBOR_SIMPLEV_RESERVED_END) {
         me->uError = QCBOR_ERR_UNSUPPORTED;
      } else {
         // This call takes care of endian swapping for the float / double
         InsertEncodedTypeAndNumber(me,
                                    // The major type for floats and doubles
                                    CBOR_MAJOR_TYPE_SIMPLE,
                                    // Must pass size to ensure floats
                                    // with zero bytes encode correctly
                                    (int)uSize,
                                    // The floating-point number as a uint
                                    uNum,
                                    // end position because this is append
                                    UsefulOutBuf_GetEndPosition(&(me->OutBuf)));

         me->uError = Nesting_Increment(&(me->nesting));
      }
   }
}


/*
 Public functions for closing arrays and maps. See qcbor.h
 */
void QCBOREncode_AddDouble(QCBOREncodeContext *me, double dNum)
{
   const IEEE754_union uNum = IEEE754_DoubleToSmallest(dNum);

   QCBOREncode_AddType7(me, uNum.uSize, uNum.uValue);
}


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
/*
 Semi-public function. It is exposed to the user of the interface, but
 one of the inline wrappers will usually be called rather than this.

 See qcbor.h
 */
void QCBOREncode_AddExponentAndMantissa(QCBOREncodeContext *pMe,
                                        uint64_t            uTag,
                                        UsefulBufC          BigNumMantissa,
                                        bool                bBigNumIsNegative,
                                        int64_t             nMantissa,
                                        int64_t             nExponent)
{
   /*
    This is for encoding either a big float or a decimal fraction,
    both of which are an array of two items, an exponent and a
    mantissa.  The difference between the two is that the exponent is
    base-2 for big floats and base-10 for decimal fractions, but that
    has no effect on the code here.
    */
   QCBOREncode_AddTag(pMe, uTag);
   QCBOREncode_OpenArray(pMe);
   QCBOREncode_AddInt64(pMe, nExponent);
   if(!UsefulBuf_IsNULLC(BigNumMantissa)) {
      if(bBigNumIsNegative) {
         QCBOREncode_AddNegativeBignum(pMe, BigNumMantissa);
      } else {
         QCBOREncode_AddPositiveBignum(pMe, BigNumMantissa);
      }
   } else {
      QCBOREncode_AddInt64(pMe, nMantissa);
   }
   QCBOREncode_CloseArray(pMe);
}
#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */


/*
 Semi-public function. It is exposed to user of the interface,
 but they will usually call one of the inline wrappers rather than this.

 See header qcbor.h
*/
void QCBOREncode_OpenMapOrArray(QCBOREncodeContext *me, uint8_t uMajorType)
{
   // Add one item to the nesting level we are in for the new map or array
   me->uError = Nesting_Increment(&(me->nesting));
   if(me->uError == QCBOR_SUCCESS) {
      /*
       The offset where the length of an array or map will get written
       is stored in a uint32_t, not a size_t to keep stack usage
       smaller. This checks to be sure there is no wrap around when
       recording the offset.  Note that on 64-bit machines CBOR larger
       than 4GB can be encoded as long as no array / map offsets occur
       past the 4GB mark, but the public interface says that the
       maximum is 4GB to keep the discussion simpler.
      */
      size_t uEndPosition = UsefulOutBuf_GetEndPosition(&(me->OutBuf));

      /*
       QCBOR_MAX_ARRAY_OFFSET is slightly less than UINT32_MAX so this
       code can run on a 32-bit machine and tests can pass on a 32-bit
       machine. If it was exactly UINT32_MAX, then this code would not
       compile or run on a 32-bit machine and an #ifdef or some
       machine size detection would be needed reducing portability.
      */
      if(uEndPosition >= QCBOR_MAX_ARRAY_OFFSET) {
         me->uError = QCBOR_ERR_BUFFER_TOO_LARGE;

      } else {
         // Increase nesting level because this is a map or array.  Cast
         // from size_t to uin32_t is safe because of check above
         me->uError = Nesting_Increase(&(me->nesting), uMajorType, (uint32_t)uEndPosition);
      }
   }
}


/*
 Semi-public function. It is exposed to user of the interface,
 but they will usually call one of the inline wrappers rather than this.

 See qcbor.h
*/
void QCBOREncode_OpenMapOrArrayIndefiniteLength(QCBOREncodeContext *me, uint8_t uMajorType)
{
   // insert the indefinite length marker (0x9f for arrays, 0xbf for maps)
   InsertEncodedTypeAndNumber(me, uMajorType, 0, 0, UsefulOutBuf_GetEndPosition(&(me->OutBuf)));

   QCBOREncode_OpenMapOrArray(me, uMajorType);
}


/*
 Public functions for closing arrays and maps. See qcbor.h
 */
void QCBOREncode_CloseMapOrArray(QCBOREncodeContext *me,
                                 uint8_t uMajorType,
                                 UsefulBufC *pWrappedCBOR)
{
   if(me->uError == QCBOR_SUCCESS) {
      if(!Nesting_IsInNest(&(me->nesting))) {
         me->uError = QCBOR_ERR_TOO_MANY_CLOSES;
      } else if(Nesting_GetMajorType(&(me->nesting)) != uMajorType) {
         me->uError = QCBOR_ERR_CLOSE_MISMATCH;
      } else {
         /*
          When the array, map or bstr wrap was started, nothing was
          gone except note the position of the start of it. This code
          goes back and inserts the actual CBOR array, map or bstr and
          its length.  That means all the data that is in the array,
          map or wrapped needs to be slid to the right. This is done
          by UsefulOutBuf's insert function that is called from inside
          InsertEncodedTypeAndNumber()
          */
         const size_t uInsertPosition = Nesting_GetStartPos(&(me->nesting));
         const size_t uEndPosition    = UsefulOutBuf_GetEndPosition(&(me->OutBuf));
         /*
          This can't go negative because the UsefulOutBuf always only
          grows and never shrinks. UsefulOutBut itself also has
          defenses such that it won't write were it should not even if
          given hostile input lengths
          */
         const size_t uLenOfEncodedMapOrArray = uEndPosition - uInsertPosition;

         // Number of bytes for a bstr or number of items a for map & array
         const bool bIsBstr = uMajorType == CBOR_MAJOR_TYPE_BYTE_STRING;
         const size_t uLength =  bIsBstr ? uLenOfEncodedMapOrArray
                                         : Nesting_GetCount(&(me->nesting));

         // Actually insert
         InsertEncodedTypeAndNumber(me,
                                    uMajorType,       // bstr, array or map
                                    0,                // no minimum length
                                    uLength,          // either len of bstr or
                                                      // num map / array items
                                    uInsertPosition); // position in out buffer

         /*
          Return pointer and length to the enclosed encoded CBOR. The
          intended use is for it to be hashed (e.g., SHA-256) in a
          COSE implementation.  This must be used right away, as the
          pointer and length go invalid on any subsequent calls to
          this function because there might be calls to
          InsertEncodedTypeAndNumber() that slides data to the right.
          */
         if(pWrappedCBOR) {
            const UsefulBufC PartialResult = UsefulOutBuf_OutUBuf(&(me->OutBuf));
            *pWrappedCBOR = UsefulBuf_Tail(PartialResult, uInsertPosition);
         }
         Nesting_Decrease(&(me->nesting));
      }
   }
}


/*
 Public functions for closing arrays and maps. See qcbor.h
 */
void QCBOREncode_CloseMapOrArrayIndefiniteLength(QCBOREncodeContext *me,
                                                 uint8_t uMajorType,
                                                 UsefulBufC *pWrappedCBOR)
{
   if(me->uError == QCBOR_SUCCESS) {
      if(!Nesting_IsInNest(&(me->nesting))) {
         me->uError = QCBOR_ERR_TOO_MANY_CLOSES;
      } else if(Nesting_GetMajorType(&(me->nesting)) != uMajorType) {
         me->uError = QCBOR_ERR_CLOSE_MISMATCH;
      } else {
         // insert the break marker (0xff for both arrays and maps)
         InsertEncodedTypeAndNumber(me,
                                    CBOR_MAJOR_TYPE_SIMPLE,
                                    0,
                                    CBOR_SIMPLE_BREAK,
                                    UsefulOutBuf_GetEndPosition(&(me->OutBuf)));

         /*
          Return pointer and length to the enclosed encoded CBOR. The
          intended use is for it to be hashed (e.g., SHA-256) in a
          COSE implementation.  This must be used right away, as the
          pointer and length go invalid on any subsequent calls to
          this function because there might be calls to
          InsertEncodedTypeAndNumber() that slides data to the right.
          */
         if(pWrappedCBOR) {
            const UsefulBufC PartialResult = UsefulOutBuf_OutUBuf(&(me->OutBuf));
            *pWrappedCBOR = UsefulBuf_Tail(PartialResult, UsefulOutBuf_GetEndPosition(&(me->OutBuf)));
         }

         // Decrease nesting level
         Nesting_Decrease(&(me->nesting));
      }
   }
}


/*
 Public functions to finish and get the encoded result. See qcbor.h
 */
QCBORError QCBOREncode_Finish(QCBOREncodeContext *me, UsefulBufC *pEncodedCBOR)
{
   QCBORError uReturn = QCBOREncode_GetErrorState(me);

   if(uReturn != QCBOR_SUCCESS) {
      goto Done;
   }

   if (Nesting_IsInNest(&(me->nesting))) {
      uReturn = QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN;
      goto Done;
   }

   *pEncodedCBOR = UsefulOutBuf_OutUBuf(&(me->OutBuf));

Done:
   return uReturn;
}



/*
 Public functions to finish and get the encoded result. See qcbor.h
 */
QCBORError QCBOREncode_FinishGetSize(QCBOREncodeContext *me, size_t *puEncodedLen)
{
   UsefulBufC Enc;

   QCBORError nReturn = QCBOREncode_Finish(me, &Enc);

   if(nReturn == QCBOR_SUCCESS) {
      *puEncodedLen = Enc.len;
   }

   return nReturn;
}




/*
 Object code sizes on X86 with LLVM compiler and -Os (Dec 30, 2018)

 _QCBOREncode_Init   69
 _QCBOREncode_AddUInt64   76
 _QCBOREncode_AddInt64   87
 _QCBOREncode_AddBuffer   113
 _QCBOREncode_AddTag 27
 _QCBOREncode_AddType7   87
 _QCBOREncode_AddDouble 36
 _QCBOREncode_OpenMapOrArray   103
 _QCBOREncode_CloseMapOrArray   181
 _InsertEncodedTypeAndNumber   190
 _QCBOREncode_Finish   72
 _QCBOREncode_FinishGetSize  70

 Total is about 1.1KB

 _QCBOREncode_CloseMapOrArray is larger because it has a lot
 of nesting tracking to do and much of Nesting_ inlines
 into it. It probably can't be reduced much.

 If the error returned by Nesting_Increment() can be ignored
 because the limit is so high and the consequence of exceeding
 is proved to be inconsequential, then a lot of if(me->uError)
 instance can be removed, saving some code.

 */
