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
 FILE:  qcbor.h

 DESCRIPTION:  This is the full public API and data structures for QCBOR

 EDIT HISTORY FOR FILE:

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when       who           what, where, why
 --------   ----          ---------------------------------------------------
 01/25/2020 llundblade    Cleaner handling of too-long encoded string input.
 01/08/2020 llundblade    Documentation corrections & improved code formatting.
 12/30/19   llundblade    Add support for decimal fractions and bigfloats.
 08/7/19    llundblade    Better handling of not well-formed encode and decode.
 07/31/19   llundblade    New error code for better end of data handling.
 7/25/19    janjongboom   Add indefinite length encoding for maps and arrays.
 05/26/19   llundblade    Add QCBOREncode_GetErrorState() and _IsBufferNULL().
 04/26/19   llundblade    Big documentation & style update. No interface change.
 02/16/19   llundblade    Redesign MemPool to fix memory access alignment bug.
 12/18/18   llundblade    Move decode malloc optional code to separate repo.
 12/13/18   llundblade    Documentatation improvements.
 11/29/18   llundblade    Rework to simpler handling of tags and labels.
 11/9/18    llundblade    Error codes are now enums.
 11/1/18    llundblade    Floating support.
 10/31/18   llundblade    Switch to one license that is almost BSD-3.
 10/15/18   llundblade    indefinite-length maps and arrays supported
 10/8/18    llundblade    indefinite-length strings supported
 09/28/18   llundblade    Added bstr wrapping feature for COSE implementation.
 07/05/17   llundbla      Add bstr wrapping of maps/arrays for COSE.
 03/01/17   llundbla      More data types; decoding improvements and fixes.
 11/13/16   llundbla      Integrate most TZ changes back into github version.
 09/30/16   gkanike       Porting to TZ.
 03/15/16   llundbla      Initial Version.

 =============================================================================*/

#ifndef __QCBOR__qcbor__
#define __QCBOR__qcbor__


/* ===========================================================================
   BEGINNING OF PRIVATE PART OF THIS FILE

   Caller of QCBOR should not reference any of the details below up until
   the start of the public part.
   ========================================================================== */

/*
 Standard integer types are used in the interface to be precise about
 sizes to be better at preventing underflow/overflow errors.
 */
#include <stdint.h>
#include <stdbool.h>
#include "UsefulBuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 The maxium nesting of arrays and maps when encoding or decoding.
 (Further down in the file there is a definition that refers to this
 that is public. This is done this way so there can be a nice
 separation of public and private parts in this file.
*/
#define QCBOR_MAX_ARRAY_NESTING1 15 // Do not increase this over 255


/* The largest offset to the start of an array or map. It is slightly
 less than UINT32_MAX so the error condition can be tests on 32-bit machines.
 UINT32_MAX comes from uStart in QCBORTrackNesting being a uin32_t.

 This will cause trouble on a machine where size_t is less than 32-bits.
 */
#define QCBOR_MAX_ARRAY_OFFSET  (UINT32_MAX - 100)

/*
 PRIVATE DATA STRUCTURE

 Holds the data for tracking array and map nesting during encoding. Pairs up
 with the Nesting_xxx functions to make an "object" to handle nesting encoding.

 uStart is a uint32_t instead of a size_t to keep the size of this
 struct down so it can be on the stack without any concern.  It would be about
 double if size_t was used instead.

 Size approximation (varies with CPU/compiler):
    64-bit machine: (15 + 1) * (4 + 2 + 1 + 1 pad) + 8 = 136 bytes
    32-bit machine: (15 + 1) * (4 + 2 + 1 + 1 pad) + 4 = 132 bytes
*/
typedef struct __QCBORTrackNesting {
   // PRIVATE DATA STRUCTURE
   struct {
      // See function QCBOREncode_OpenMapOrArray() for details on how this works
      uint32_t  uStart;   // uStart is the byte position where the array starts
      uint16_t  uCount;   // Number of items in the arrary or map; counts items
                          // in a map, not pairs of items
      uint8_t   uMajorType; // Indicates if item is a map or an array
   } pArrays[QCBOR_MAX_ARRAY_NESTING1+1], // stored state for the nesting levels
   *pCurrentNesting; // the current nesting level
} QCBORTrackNesting;


/*
 PRIVATE DATA STRUCTURE

 Context / data object for encoding some CBOR. Used by all encode functions to
 form a public "object" that does the job of encdoing.

 Size approximation (varies with CPU/compiler):
   64-bit machine: 27 + 1 (+ 4 padding) + 136 = 32 + 136 = 168 bytes
   32-bit machine: 15 + 1 + 132 = 148 bytes
*/
struct _QCBOREncodeContext {
   // PRIVATE DATA STRUCTURE
   UsefulOutBuf      OutBuf;  // Pointer to output buffer, its length and
                              // position in it
   uint8_t           uError;  // Error state, always from QCBORError enum
   QCBORTrackNesting nesting; // Keep track of array and map nesting
};


/*
 PRIVATE DATA STRUCTURE

 Holds the data for array and map nesting for decoding work. This structure
 and the DecodeNesting_xxx functions form an "object" that does the work
 for arrays and maps.

 Size approximation (varies with CPU/compiler):
   64-bit machine: 4 * 16 + 8 = 72
   32-bit machine: 4 * 16 + 4 = 68
 */
typedef struct __QCBORDecodeNesting  {
  // PRIVATE DATA STRUCTURE
   struct {
      uint16_t uCount;
      uint8_t  uMajorType;
   } pMapsAndArrays[QCBOR_MAX_ARRAY_NESTING1+1],
   *pCurrent;
} QCBORDecodeNesting;


typedef struct  {
   // PRIVATE DATA STRUCTURE
   void *pAllocateCxt;
   UsefulBuf (* pfAllocator)(void *pAllocateCxt, void *pOldMem, size_t uNewSize);
} QCORInternalAllocator;


/*
 PRIVATE DATA STRUCTURE

 The decode context. This data structure plus the public QCBORDecode_xxx
 functions form an "object" that does CBOR decoding.

 Size approximation (varies with CPU/compiler):
   64-bit machine: 32 + 1 + 1 + 6 bytes padding + 72 + 16 + 8 + 8 = 144 bytes
   32-bit machine: 16 + 1 + 1 + 2 bytes padding + 68 +  8 + 8 + 4 = 108 bytes
 */
struct _QCBORDecodeContext {
   // PRIVATE DATA STRUCTURE
   UsefulInputBuf InBuf;

   uint8_t        uDecodeMode;
   uint8_t        bStringAllocateAll;

   QCBORDecodeNesting nesting;

   // If a string allocator is configured for indefinite-length
   // strings, it is configured here.
   QCORInternalAllocator StringAllocator;

   // These are special for the internal MemPool allocator.
   // They are not used otherwise. We tried packing these
   // in the MemPool itself, but there are issues
   // with memory alignment.
   uint32_t uMemPoolSize;
   uint32_t uMemPoolFreeOffset;

   // This is NULL or points to QCBORTagList.
   // It is type void for the same reason as above.
   const void *pCallerConfiguredTagList;
};

// Used internally in the impementation here
// Must not conflict with any of the official CBOR types
#define CBOR_MAJOR_NONE_TYPE_RAW  9
#define CBOR_MAJOR_NONE_TAG_LABEL_REORDER 10
#define CBOR_MAJOR_NONE_TYPE_BSTR_LEN_ONLY 11
#define CBOR_MAJOR_NONE_TYPE_ARRAY_INDEFINITE_LEN 12
#define CBOR_MAJOR_NONE_TYPE_MAP_INDEFINITE_LEN 13


/* ==========================================================================
   END OF PRIVATE PART OF THIS FILE

   BEGINNING OF PUBLIC PART OF THIS FILE
   ========================================================================== */



/* ==========================================================================
   BEGINNING OF CONSTANTS THAT COME FROM THE CBOR STANDARD, RFC 7049

   It is not necessary to use these directly when encoding or decoding
   CBOR with this implementation.
   ========================================================================== */

/* Standard CBOR Major type for positive integers of various lengths */
#define CBOR_MAJOR_TYPE_POSITIVE_INT 0

/* Standard CBOR Major type for negative integer of various lengths */
#define CBOR_MAJOR_TYPE_NEGATIVE_INT 1

/* Standard CBOR Major type for an array of arbitrary 8-bit bytes. */
#define CBOR_MAJOR_TYPE_BYTE_STRING  2

/* Standard CBOR Major type for a UTF-8 string. Note this is true 8-bit UTF8
 with no encoding and no NULL termination */
#define CBOR_MAJOR_TYPE_TEXT_STRING  3

/* Standard CBOR Major type for an ordered array of other CBOR data items */
#define CBOR_MAJOR_TYPE_ARRAY        4

/* Standard CBOR Major type for CBOR MAP. Maps an array of pairs. The
 first item in the pair is the "label" (key, name or identfier) and the second
 item is the value.  */
#define CBOR_MAJOR_TYPE_MAP          5

/* Standard CBOR optional tagging. This tags things like dates and URLs */
#define CBOR_MAJOR_TYPE_OPTIONAL     6

/* Standard CBOR extra simple types like floats and the values true and false */
#define CBOR_MAJOR_TYPE_SIMPLE       7


/*
 These are special values for the AdditionalInfo bits that are part of
 the first byte.  Mostly they encode the length of the data item.
 */
#define LEN_IS_ONE_BYTE    24
#define LEN_IS_TWO_BYTES   25
#define LEN_IS_FOUR_BYTES  26
#define LEN_IS_EIGHT_BYTES 27
#define ADDINFO_RESERVED1  28
#define ADDINFO_RESERVED2  29
#define ADDINFO_RESERVED3  30
#define LEN_IS_INDEFINITE  31


/*
 24 is a special number for CBOR. Integers and lengths
 less than it are encoded in the same byte as the major type.
 */
#define CBOR_TWENTY_FOUR   24


/*
 Tags that are used with CBOR_MAJOR_TYPE_OPTIONAL. These
 are types defined in RFC 7049 and some additional ones
 in the IANA CBOR tags registry.
 */
/** See QCBOREncode_AddDateString(). */
#define CBOR_TAG_DATE_STRING    0
/** See QCBOREncode_AddDateEpoch(). */
#define CBOR_TAG_DATE_EPOCH     1
/** See QCBOREncode_AddPositiveBignum(). */
#define CBOR_TAG_POS_BIGNUM     2
/** See QCBOREncode_AddNegativeBignum(). */
#define CBOR_TAG_NEG_BIGNUM     3
/** CBOR tag for a two-element array representing a fraction with a
    mantissa and base-10 scaling factor. See QCBOREncode_AddDecimalFraction()
    and @ref expAndMantissa.
  */
#define CBOR_TAG_DECIMAL_FRACTION  4
/** CBOR tag for a two-element array representing a fraction with a
    mantissa and base-2 scaling factor. See QCBOREncode_AddBigFloat()
    and @ref expAndMantissa. */
#define CBOR_TAG_BIGFLOAT       5
/** Tag for COSE format encryption with no recipient
    identification. See [RFC 8152, COSE]
    (https://tools.ietf.org/html/rfc8152). No API is provided for this
    tag. */
#define CBOR_TAG_COSE_ENCRYPTO 16
/** Tag for COSE format MAC'd data with no recipient
    identification. See [RFC 8152, COSE]
    (https://tools.ietf.org/html/rfc8152). No API is provided for this
    tag.*/
#define CBOR_TAG_COSE_MAC0     17
/** Tag for COSE format single signature signing. No API is provided
    for this tag. See [RFC 8152, COSE]
    (https://tools.ietf.org/html/rfc8152). */
#define CBOR_TAG_COSE_SIGN1    18
/** A hint that the following byte string should be encoded in
    Base64URL when converting to JSON or similar text-based
    representations. Call @c
    QCBOREncode_AddTag(pCtx,CBOR_TAG_ENC_AS_B64URL) before the call to
    QCBOREncode_AddBytes(). */
#define CBOR_TAG_ENC_AS_B64URL 21
/** A hint that the following byte string should be encoded in Base64
    when converting to JSON or similar text-based
    representations. Call @c
    QCBOREncode_AddTag(pCtx,CBOR_TAG_ENC_AS_B64) before the call to
    QCBOREncode_AddBytes(). */
#define CBOR_TAG_ENC_AS_B64    22
/** A hint that the following byte string should be encoded in base-16
    format per [RFC 4648] (https://tools.ietf.org/html/rfc4648) when
    converting to JSON or similar text-based
    representations. Essentially, Base-16 encoding is the standard
    case- insensitive hex encoding and may be referred to as
    "hex". Call @c QCBOREncode_AddTag(pCtx,CBOR_TAG_ENC_AS_B16) before
    the call to QCBOREncode_AddBytes(). */
#define CBOR_TAG_ENC_AS_B16    23
/** Tag to indicate a byte string contains encoded CBOR. No API is
    provided for this tag. */
#define CBOR_TAG_CBOR          24
/** See QCBOREncode_AddURI(). */
#define CBOR_TAG_URI           32
/** See QCBOREncode_AddB64URLText(). */
#define CBOR_TAG_B64URL        33
/** See QCBOREncode_AddB64Text(). */
#define CBOR_TAG_B64           34
/** See QCBOREncode_AddRegex(). */
#define CBOR_TAG_REGEX         35
/** See QCBOREncode_AddMIMEData(). */
#define CBOR_TAG_MIME          36
/** See QCBOREncode_AddBinaryUUID(). */
#define CBOR_TAG_BIN_UUID      37
/** The data is a CBOR Web Token per [RFC 8392]
    (https://tools.ietf.org/html/rfc8932). No API is provided for this
    tag. */
#define CBOR_TAG_CWT           61
/** Tag for COSE format encryption. See [RFC 8152, COSE]
    (https://tools.ietf.org/html/rfc8152). No API is provided for this
    tag. */
#define CBOR_TAG_ENCRYPT       96
/** Tag for COSE format MAC. See [RFC 8152, COSE]
    (https://tools.ietf.org/html/rfc8152). No API is provided for this
    tag. */
#define CBOR_TAG_MAC           97
/** Tag for COSE format signed data. See [RFC 8152, COSE]
    (https://tools.ietf.org/html/rfc8152). No API is provided for this
    tag. */
#define CBOR_TAG_SIGN          98
/** World geographic coordinates. See ISO 6709, [RFC 5870]
    (https://tools.ietf.org/html/rfc5870) and WGS-84. No API is
    provided for this tag. */
#define CBOR_TAG_GEO_COORD    103
/** The magic number, self-described CBOR. No API is provided for this
    tag. */
#define CBOR_TAG_CBOR_MAGIC 55799

#define CBOR_TAG_NONE  UINT64_MAX


/*
 Values for the 5 bits for items of major type 7
 */
#define CBOR_SIMPLEV_FALSE   20
#define CBOR_SIMPLEV_TRUE    21
#define CBOR_SIMPLEV_NULL    22
#define CBOR_SIMPLEV_UNDEF   23
#define CBOR_SIMPLEV_ONEBYTE 24
#define HALF_PREC_FLOAT      25
#define SINGLE_PREC_FLOAT    26
#define DOUBLE_PREC_FLOAT    27
#define CBOR_SIMPLE_BREAK    31
#define CBOR_SIMPLEV_RESERVED_START  CBOR_SIMPLEV_ONEBYTE
#define CBOR_SIMPLEV_RESERVED_END    CBOR_SIMPLE_BREAK



/* ===========================================================================

 END OF CONSTANTS THAT COME FROM THE CBOR STANDARD, RFC 7049

 BEGINNING OF PUBLIC INTERFACE FOR QCBOR ENCODER / DECODER

 =========================================================================== */

/**

 @file qcbor.h

 Q C B O R   E n c o d e / D e c o d e

 This implements CBOR -- Concise Binary Object Representation as
 defined in [RFC 7049] (https://tools.ietf.org/html/rfc7049). More
 info is at http://cbor.io.  This is a near-complete implementation of
 the specification. Limitations are listed further down.

 CBOR is intentionally designed to be translatable to JSON, but not
 all CBOR can convert to JSON. See RFC 7049 for more info on how to
 construct CBOR that is the most JSON friendly.

 The memory model for encoding and decoding is that encoded CBOR must
 be in a contiguous buffer in memory.  During encoding the caller must
 supply an output buffer and if the encoding would go off the end of
 the buffer an error is returned.  During decoding the caller supplies
 the encoded CBOR in a contiguous buffer and the decoder returns
 pointers and lengths into that buffer for strings.

 This implementation does not require malloc. All data structures
 passed in/out of the APIs can fit on the stack.

 Decoding of indefinite-length strings is a special case that requires
 a "string allocator" to allocate memory into which the segments of
 the string are coalesced. Without this, decoding will error out if an
 indefinite-length string is encountered (indefinite-length maps and
 arrays do not require the string allocator). A simple string
 allocator called MemPool is built-in and will work if supplied with a
 block of memory to allocate. The string allocator can optionally use
 malloc() or some other custom scheme.

 Here are some terms and definitions:

 - "Item", "Data Item": An integer or string or such. The basic "thing" that
 CBOR is about. An array is an item itself that contains some items.

 - "Array": An ordered sequence of items, the same as JSON.

 - "Map": A collection of label/value pairs. Each pair is a data
 item. A JSON "object" is the same as a CBOR "map".

 - "Label": The data item in a pair in a map that names or identifies
 the pair, not the value. This implementation refers to it as a
 "label".  JSON refers to it as the "name". The CBOR RFC refers to it
 this as a "key".  This implementation chooses label instead because
 key is too easily confused with a cryptographic key. The COSE
 standard, which uses CBOR, has also chosen to use the term "label"
 rather than "key" for this same reason.

 - "Key": See "Label" above.

 - "Tag": Optional integer that can be added before each data item
 usually to indicate it is new or more specific data type. For
 example, a tag can indicate an integer is a date, or that a map is to
 be considered a type (analogous to a typedef in C).

 - "Initial Byte": The first byte of an encoded item. Encoding and
 decoding of this byte is taken care of by the implementation.

 - "Additional Info": In addition to the major type, all data items
 have some other info. This is usually the length of the data but can
 be several other things. Encoding and decoding of this is taken care
 of by the implementation.

 CBOR has two mechanisms for tagging and labeling the data values like
 integers and strings. For example, an integer that represents
 someone's birthday in epoch seconds since Jan 1, 1970 could be
 encoded like this:

 - First it is CBOR_MAJOR_TYPE_POSITIVE_INT (@ref QCBOR_TYPE_INT64),
 the primitive positive integer.

 - Next it has a "tag" @ref CBOR_TAG_DATE_EPOCH indicating the integer
 represents a date in the form of the number of seconds since Jan 1,
 1970.

 - Last it has a string "label" like "BirthDate" indicating the
 meaning of the data.

 The encoded binary looks like this:

      a1                      # Map of 1 item
         69                   # Indicates text string of 9 bytes
           426972746844617465 # The text "BirthDate"
        c1                    # Tags next integer as epoch date
           1a                 # Indicates a 4-byte integer
               580d4172       # unsigned integer date 1477263730

 Implementors using this API will primarily work with
 labels. Generally, tags are only needed for making up new data
 types. This implementation covers most of the data types defined in
 the RFC using tags. It also, allows for the use of custom tags if
 necessary.

 This implementation explicitly supports labels that are text strings
 and integers. Text strings translate nicely into JSON objects and are
 very readable.  Integer labels are much less readable but can be very
 compact. If they are in the range of 0 to 23, they take up only one
 byte.

 CBOR allows a label to be any type of data including an array or a
 map. It is possible to use this API to construct and parse such
 labels, but it is not explicitly supported.

 A common encoding usage mode is to invoke the encoding twice. First
 with no output buffer to compute the length of the needed output
 buffer. Then the correct sized output buffer is allocated. Last the
 encoder is invoked again, this time with the output buffer.

 The double invocation is not required if the maximum output buffer
 size can be predicted. This is usually possible for simple CBOR
 structures.  If the double invocation is implemented, it can be in a
 loop or function as in the example code so that the code doesn't have
 to actually be written twice, saving code size.

 If a buffer too small to hold the encoded output is given, the error
 @ref QCBOR_ERR_BUFFER_TOO_SMALL will be returned. Data will never be
 written off the end of the output buffer no matter which functions
 here are called or what parameters are passed to them.

 The encoding error handling is simple. The only possible errors are
 trying to encode structures that are too large or too complex. There
 are no internal malloc calls so there will be no failures for out of
 memory.  The error state is tracked internally, so there is no need
 to check for errors when encoding. Only the return code from
 QCBOREncode_Finish() need be checked as once an error happens, the
 encoder goes into an error state and calls to it to add more data
 will do nothing. An error check is not needed after every data item
 is added.

 Encoding generally proceeds by calling QCBOREncode_Init(), calling
 lots of @c QCBOREncode_AddXxx() functions and calling
 QCBOREncode_Finish(). There are many @c QCBOREncode_AddXxx()
 functions for various data types. The input buffers need only to be
 valid during the @c QCBOREncode_AddXxx() calls as the data is copied
 into the output buffer.

 There are three `Add` functions for each data type. The first / main
 one for the type is for adding the data item to an array.  The second
 one's name ends in `ToMap`, is used for adding data items to maps and
 takes a string argument that is its label in the map. The third one
 ends in `ToMapN`, is also used for adding data items to maps, and
 takes an integer argument that is its label in the map.

 The simplest aggregate type is an array, which is a simple ordered
 set of items without labels the same as JSON arrays. Call
 QCBOREncode_OpenArray() to open a new array, then various @c
 QCBOREncode_AddXxx() functions to put items in the array and then
 QCBOREncode_CloseArray(). Nesting to the limit @ref
 QCBOR_MAX_ARRAY_NESTING is allowed.  All opens must be matched by
 closes or an encoding error will be returned.

 The other aggregate type is a map which does use labels. The `Add`
 functions that end in `ToMap` and `ToMapN` are convenient ways to add
 labeled data items to a map. You can also call any type of `Add`
 function once to add a label of any time and then call any type of
 `Add` again to add its value.

 Note that when you nest arrays or maps in a map, the nested array or
 map has a label.

 @anchor Tags-Overview
 Any CBOR data item can be tagged to add semantics, define a new data
 type or such. Some tags are fully standardized and some are just
 registered. Others are not registered and used in a proprietary way.

 Encoding and decoding of many of the registered tags is fully
 implemented by QCBOR. It is also possible to encode and decode tags
 that are not directly supported.  For many use cases the built-in tag
 support should be adequate.

 For example, the registered epoch date tag is supported in encoding
 by QCBOREncode_AddDateEpoch() and in decoding by @ref
 QCBOR_TYPE_DATE_EPOCH and the @c epochDate member of @ref
 QCBORItem. This is typical of the built-in tag support. There is an
 API to encode data for it and a @c QCBOR_TYPE_XXX when it is decoded.

 Tags are registered in the [IANA CBOR Tags Registry]
 (https://www.iana.org/assignments/cbor-tags/cbor-tags.xhtml). There
 are roughly three options to create a new tag. First, a public
 specification can be created and the new tag registered with IANA.
 This is the most formal. Second, the new tag can be registered with
 IANA with just a short description rather than a full specification.
 These tags must be greater than 256. Third, a tag can be used without
 any IANA registration, though the registry should be checked to see
 that the new value doesn't collide with one that is registered. The
 value of these tags must be 256 or larger.

 The encoding side of tags not built-in is handled by
 QCBOREncode_AddTag() and is relatively simple. Tag decoding is more
 complex and mainly handled by QCBORDecode_GetNext(). Decoding of the
 structure of tagged data not built-in (if there is any) has to be
 implemented by the caller.

 Summary Limits of this implementation:
 - The entire encoded CBOR must fit into contiguous memory.
 - Max size of encoded / decoded CBOR data is @c UINT32_MAX (4GB).
 - Max array / map nesting level when encoding / decoding is
   @ref QCBOR_MAX_ARRAY_NESTING (this is typically 15).
 - Max items in an array or map when encoding / decoding is
   @ref QCBOR_MAX_ITEMS_IN_ARRAY (typically 65,536).
 - Does not directly support labels in maps other than text strings & integers.
 - Does not directly support integer labels greater than @c INT64_MAX.
 - Epoch dates limited to @c INT64_MAX (+/- 292 billion years).
 - Exponents for bigfloats and decimal integers are limited to @c INT64_MAX.
 - Tags on labels are ignored during decoding.
 - There is no duplicate detection of map labels (but duplicates are passed on).
 - Works only on 32- and 64-bit CPUs (modifications could make it work
   on 16-bit CPUs).

 The public interface uses @c size_t for all lengths. Internally the
 implementation uses 32-bit lengths by design to use less memory and
 fit structures on the stack. This limits the encoded CBOR it can work
 with to size @c UINT32_MAX (4GB) which should be enough.

 This implementation assumes two's compliment integer machines. @c
 <stdint.h> also requires this. It is possible to modify this
 implementation for another integer representation, but all modern
 machines seem to be two's compliment.

 */


/**
 The maximum number of items in a single array or map when encoding of
 decoding.
*/
// -1 is because the value UINT16_MAX is used to track indefinite-length arrays
#define QCBOR_MAX_ITEMS_IN_ARRAY (UINT16_MAX-1)

/**
 The maximum nesting of arrays and maps when encoding or decoding. The
 error @ref QCBOR_ERR_ARRAY_NESTING_TOO_DEEP will be returned on
 encoding of decoding if it is exceeded.
*/
#define QCBOR_MAX_ARRAY_NESTING  QCBOR_MAX_ARRAY_NESTING1

/**
 The maximum number of tags that can be in @ref QCBORTagListIn and passed to
 QCBORDecode_SetCallerConfiguredTagList()
 */
#define QCBOR_MAX_CUSTOM_TAGS    16


/**
 Error codes returned by QCBOR Encoder and Decoder.
 */
typedef enum {
   /** The encode or decode completely correctly. */
   QCBOR_SUCCESS = 0,

   /** The buffer provided for the encoded output when doing encoding
       was too small and the encoded output will not fit. Also, when
       the buffer given to QCBORDecode_SetMemPool() is too small. */
   QCBOR_ERR_BUFFER_TOO_SMALL = 1,

   /** During encoding or decoding, the array or map nesting was
       deeper than this implementation can handle. Note that in the
       interest of code size and memory use, this implementation has a
       hard limit on array nesting. The limit is defined as the
       constant @ref QCBOR_MAX_ARRAY_NESTING. */
   QCBOR_ERR_ARRAY_NESTING_TOO_DEEP = 2,

   /** During decoding or encoding, the array or map had too many
       items in it.  This limit @ref QCBOR_MAX_ITEMS_IN_ARRAY,
       typically 65,535. */
   QCBOR_ERR_ARRAY_TOO_LONG = 3,

   /** During encoding, more arrays or maps were closed than
       opened. This is a coding error on the part of the caller of the
       encoder. */
   QCBOR_ERR_TOO_MANY_CLOSES = 4,

   /** During decoding, some CBOR construct was encountered that this
       decoder doesn't support, primarily this is the reserved
       additional info values, 28 through 30. During encoding,
       an attempt to create simple value between 24 and 31. */
   QCBOR_ERR_UNSUPPORTED = 5,

   /** During decoding, hit the end of the given data to decode. For
       example, a byte string of 100 bytes was expected, but the end
       of the input was hit before finding those 100 bytes.  Corrupted
       CBOR input will often result in this error. See also @ref
       QCBOR_ERR_NO_MORE_ITEMS.
     */
   QCBOR_ERR_HIT_END = 6,

   /** During encoding, the length of the encoded CBOR exceeded @c
       UINT32_MAX. */
   QCBOR_ERR_BUFFER_TOO_LARGE = 7,

   /** During decoding, an integer smaller than INT64_MIN was received
       (CBOR can represent integers smaller than INT64_MIN, but C
       cannot). */
   QCBOR_ERR_INT_OVERFLOW = 8,

   /** During decoding, the label for a map entry is bad. What causes
       this error depends on the decoding mode. */
   QCBOR_ERR_MAP_LABEL_TYPE = 9,

   /** During encoding or decoding, the number of array or map opens
       was not matched by the number of closes. */
   QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN = 10,

   /** During decoding, a date greater than +- 292 billion years from
       Jan 1 1970 encountered during parsing. */
   QCBOR_ERR_DATE_OVERFLOW = 11,

   /** During decoding, the CBOR is not valid, primarily a simple type
      is encoded in a prohibited way. */
   QCBOR_ERR_BAD_TYPE_7 = 12,

   /** Optional tagging that doesn't make sense (an integer is tagged
       as a date string) or can't be handled. */
   QCBOR_ERR_BAD_OPT_TAG = 13,

   /** Returned by QCBORDecode_Finish() if all the inputs bytes have
       not been consumed. */
   QCBOR_ERR_EXTRA_BYTES = 14,

   /** During encoding, @c QCBOREncode_CloseXxx() called with a
       different type than is currently open. */
   QCBOR_ERR_CLOSE_MISMATCH = 15,

   /** Unable to decode an indefinite-length string because no string
       allocator was configured. See QCBORDecode_SetMemPool() or
       QCBORDecode_SetUpAllocator(). */
   QCBOR_ERR_NO_STRING_ALLOCATOR = 16,

   /** One of the chunks in an indefinite-length string is not of the
       type of the start of the string. */
   QCBOR_ERR_INDEFINITE_STRING_CHUNK = 17,

   /** Error allocating space for a string, usually for an
       indefinite-length string. */
   QCBOR_ERR_STRING_ALLOCATE = 18,

   /** During decoding, a break occurred outside an indefinite-length
       item. */
   QCBOR_ERR_BAD_BREAK = 19,

   /** During decoding, too many tags in the caller-configured tag
       list, or not enough space in @ref QCBORTagListOut. */
   QCBOR_ERR_TOO_MANY_TAGS = 20,

   /** An integer type is encoded with a bad length (an indefinite length) */
   QCBOR_ERR_BAD_INT = 21,

   /** All well-formed data items have been consumed and there are no
       more. If parsing a CBOR stream this indicates the non-error
       end of the stream. If parsing a CBOR stream / sequence, this
       probably indicates that some data items expected are not present.
       See also @ref QCBOR_ERR_HIT_END. */
   QCBOR_ERR_NO_MORE_ITEMS = 22,

   /** Something is wrong with a decimal fraction or bigfloat such as
    it not consisting of an array with two integers */
   QCBOR_ERR_BAD_EXP_AND_MANTISSA = 23,

   /** When decoding, a string's size is greater than size_t. In all but some
    very strange situations this is because of corrupt input CBOR and
    should be treated as such. The strange situation is a CPU with a very
    small size_t (e.g., a 16-bit CPU) and a large string (e.g., > 65KB).
    */
    QCBOR_ERR_STRING_TOO_LONG = 24

} QCBORError;


/**
 The decode mode options.
 */
typedef enum {
   /** See QCBORDecode_Init() */
   QCBOR_DECODE_MODE_NORMAL = 0,
   /** See QCBORDecode_Init() */
   QCBOR_DECODE_MODE_MAP_STRINGS_ONLY = 1,
   /** See QCBORDecode_Init() */
   QCBOR_DECODE_MODE_MAP_AS_ARRAY = 2
} QCBORDecodeMode;





/* Do not renumber these. Code depends on some of these values. */
/** The data type is unknown, unset or invalid. */
#define QCBOR_TYPE_NONE           0
/** Type for an integer that decoded either between @c INT64_MIN and
    @c INT32_MIN or @c INT32_MAX and @c INT64_MAX. Data is in member
    @c val.int64. */
#define QCBOR_TYPE_INT64          2
/** Type for an integer that decoded to a more than @c INT64_MAX and
     @c UINT64_MAX.  Data is in member @c val.uint64. */
#define QCBOR_TYPE_UINT64         3
/** Type for an array. The number of items in the array is in @c
    val.uCount. */
#define QCBOR_TYPE_ARRAY          4
/** Type for a map; number of items in map is in @c val.uCount. */
#define QCBOR_TYPE_MAP            5
/** Type for a buffer full of bytes. Data is in @c val.string. */
#define QCBOR_TYPE_BYTE_STRING    6
/** Type for a UTF-8 string. It is not NULL-terminated. Data is in @c
    val.string.  */
#define QCBOR_TYPE_TEXT_STRING    7
/** Type for a positive big number. Data is in @c val.bignum, a
    pointer and a length. */
#define QCBOR_TYPE_POSBIGNUM      9
/** Type for a negative big number. Data is in @c val.bignum, a
    pointer and a length. */
#define QCBOR_TYPE_NEGBIGNUM     10
/** Type for [RFC 3339] (https://tools.ietf.org/html/rfc3339) date
    string, possibly with time zone. Data is in @c val.dateString */
#define QCBOR_TYPE_DATE_STRING   11
/** Type for integer seconds since Jan 1970 + floating-point
    fraction. Data is in @c val.epochDate */
#define QCBOR_TYPE_DATE_EPOCH    12
/** A simple type that this CBOR implementation doesn't know about;
    Type is in @c val.uSimple. */
#define QCBOR_TYPE_UKNOWN_SIMPLE 13

/** A decimal fraction made of decimal exponent and integer mantissa.
    See @ref expAndMantissa and QCBOREncode_AddDecimalFraction(). */
#define QCBOR_TYPE_DECIMAL_FRACTION            14

/** A decimal fraction made of decimal exponent and positive big
    number mantissa. See @ref expAndMantissa and
    QCBOREncode_AddDecimalFractionBigNum(). */
#define QCBOR_TYPE_DECIMAL_FRACTION_POS_BIGNUM 15

/** A decimal fraction made of decimal exponent and negative big
    number mantissa. See @ref expAndMantissa and
    QCBOREncode_AddDecimalFractionBigNum(). */
#define QCBOR_TYPE_DECIMAL_FRACTION_NEG_BIGNUM 16

/** A floating-point number made of base-2 exponent and integer
    mantissa.  See @ref expAndMantissa and
    QCBOREncode_AddBigFloat(). */
#define QCBOR_TYPE_BIGFLOAT      17

/** A floating-point number made of base-2 exponent and positive big
    number mantissa.  See @ref expAndMantissa and
    QCBOREncode_AddBigFloatBigNum(). */
#define QCBOR_TYPE_BIGFLOAT_POS_BIGNUM      18

/** A floating-point number made of base-2 exponent and negative big
    number mantissa.  See @ref expAndMantissa and
    QCBOREncode_AddBigFloatBigNum(). */
#define QCBOR_TYPE_BIGFLOAT_NEG_BIGNUM      19

/** Type for the value false. */
#define QCBOR_TYPE_FALSE         20
/** Type for the value true. */
#define QCBOR_TYPE_TRUE          21
/** Type for the value null. */
#define QCBOR_TYPE_NULL          22
/** Type for the value undef. */
#define QCBOR_TYPE_UNDEF         23
/** Type for a floating-point number. Data is in @c val.float. */
#define QCBOR_TYPE_FLOAT         26
/** Type for a double floating-point number. Data is in @c val.double. */
#define QCBOR_TYPE_DOUBLE        27
/** For @ref QCBOR_DECODE_MODE_MAP_AS_ARRAY decode mode, a map that is
     being traversed as an array. See QCBORDecode_Init() */
#define QCBOR_TYPE_MAP_AS_ARRAY  32

#define QCBOR_TYPE_BREAK         31 // Used internally; never returned

#define QCBOR_TYPE_OPTTAG       254 // Used internally; never returned



/*
 Approx Size of this:
   8 + 8 + 1 + 1 + 1 + (1 padding) + (4 padding) = 24 for first part
                                                  (20 on a 32-bit machine)
   16 bytes for the val union
   16 bytes for label union
   total = 56 bytes (52 bytes on 32-bit machine)
 */

/**
 The main data structure that holds the type, value and other info for
 a decoded item returned by QCBORDecode_GetNext() and
 QCBORDecode_GetNextWithTags().
 */
typedef struct _QCBORItem {
   /** Tells what element of the @c val union to use. One of @c
       QCBOR_TYPE_XXXX */
   uint8_t  uDataType;
   /** How deep the nesting from arrays and maps are. 0 is the top
       level with no arrays or maps entered. */
   uint8_t  uNestingLevel;
    /** Tells what element of the label union to use. */
   uint8_t  uLabelType;
   /** 1 if allocated with string allocator, 0 if not. See
       QCBORDecode_SetMemPool() or QCBORDecode_SetUpAllocator() */
   uint8_t  uDataAlloc;
   /** Like @c uDataAlloc, but for label. */
   uint8_t  uLabelAlloc;
   /** If not equal to @c uNestingLevel, this item closed out at least
       one map/array */
   uint8_t  uNextNestLevel;

   /** The union holding the item's value. Select union member based
       on @c uDataType */
   union {
      /** The value for @c uDataType @ref QCBOR_TYPE_INT64. */
      int64_t     int64;
      /** The value for uDataType @ref QCBOR_TYPE_UINT64. */
      uint64_t    uint64;
      /** The value for @c uDataType @ref QCBOR_TYPE_BYTE_STRING and
          @ref QCBOR_TYPE_TEXT_STRING. */
      UsefulBufC  string;
      /** The "value" for @c uDataType @ref QCBOR_TYPE_ARRAY or @ref
          QCBOR_TYPE_MAP -- the number of items in the array or map.
          It is @c UINT16_MAX when decoding indefinite-lengths maps
          and arrays. */
      uint16_t    uCount;
      /** The value for @c uDataType @ref QCBOR_TYPE_DOUBLE. */
      double      dfnum;
      /** The value for @c uDataType @ref QCBOR_TYPE_DATE_EPOCH. */
      struct {
         int64_t  nSeconds;
         double   fSecondsFraction;
      } epochDate;
      /** The value for @c uDataType @ref QCBOR_TYPE_DATE_STRING. */
      UsefulBufC  dateString;
      /** The value for @c uDataType @ref QCBOR_TYPE_POSBIGNUM and
           @ref QCBOR_TYPE_NEGBIGNUM. */
      UsefulBufC  bigNum;
      /** The integer value for unknown simple types. */
      uint8_t     uSimple;
#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
      /** @anchor expAndMantissa

          The value for bigfloats and decimal fractions.  The use of the
          fields in this structure depend on @c uDataType.

          When @c uDataType is a @c DECIMAL_FRACTION, the exponent is
          base-10. When it is a @c BIG_FLOAT it is base-2.

          When @c uDataType is a @c POS_BIGNUM or a @c NEG_BIGNUM then the
          @c bigNum part of @c Mantissa is valid. Otherwise the
          @c nInt part of @c Mantissa is valid.

          See @ref QCBOR_TYPE_DECIMAL_FRACTION,
          @ref QCBOR_TYPE_DECIMAL_FRACTION_POS_BIGNUM,
          @ref QCBOR_TYPE_DECIMAL_FRACTION_NEG_BIGNUM,
          @ref QCBOR_TYPE_BIGFLOAT, @ref QCBOR_TYPE_BIGFLOAT_POS_BIGNUM,
          and @ref QCBOR_TYPE_BIGFLOAT_NEG_BIGNUM.

          Also see QCBOREncode_AddDecimalFraction(), QCBOREncode_AddBigFloat(),
          QCBOREncode_AddDecimalFractionBigNum() and
          QCBOREncode_AddBigFloatBigNum().
       */
      struct {
         int64_t nExponent;
         union {
            int64_t    nInt;
            UsefulBufC bigNum;
         } Mantissa;
      } expAndMantissa;
#endif
      uint64_t    uTagV;  // Used internally during decoding
   } val;

   /** Union holding the different label types selected based on @c
       uLabelType */
   union {
      /** The label for @c uLabelType @ref QCBOR_TYPE_BYTE_STRING and
          @ref QCBOR_TYPE_TEXT_STRING */
      UsefulBufC  string;
      /** The label for @c uLabelType for @ref QCBOR_TYPE_INT64 */
      int64_t     int64;
      /** The label for @c uLabelType for @ref QCBOR_TYPE_UINT64 */
      uint64_t    uint64;
   } label;

   /** Bit indicating which tags (major type 6) on this item. See
       QCBORDecode_IsTagged().  */
   uint64_t uTagBits;

} QCBORItem;



/**
  @brief The type defining what a string allocator function must do.

  @param[in] pAllocateCxt  Pointer to context for the particular
                            allocator implementation What is in the
                            context is dependent on how a particular
                            string allocator works. Typically, it
                            will contain a pointer to the memory pool
                            and some booking keeping data.
 @param[in] pOldMem         Points to some memory allocated by the
                            allocator that is either to be freed or
                            to be reallocated to be larger. It is
                            @c NULL for new allocations and when called as
                            a destructor to clean up the whole
                            allocation.
 @param[in] uNewSize        Size of memory to be allocated or new
                            size of chunk to be reallocated. Zero for
                            a new allocation or when called as a
                            destructor.

 @return   Either the allocated buffer is returned, or @ref
           NULLUsefulBufC. @ref NULLUsefulBufC is returned on a failed
           allocation and in the two cases where there is nothing to
           return.

 This is called in one of four modes:

 Allocate -- @c uNewSize is the amount to allocate. @c pOldMem is @c
 NULL.

 Free -- @c uNewSize is 0. @c pOldMem points to the memory to be
 freed.  When the decoder calls this, it will always be the most
 recent block that was either allocated or reallocated.

 Reallocate -- @c pOldMem is the block to reallocate. @c uNewSize is
 its new size.  When the decoder calls this, it will always be the
 most recent block that was either allocated or reallocated.

 Destruct -- @c pOldMem is @c NULL and @c uNewSize is 0. This is called
 when the decoding is complete by QCBORDecode_Finish(). Usually the
 strings allocated by a string allocator are in use after the decoding
 is completed so this usually will not free those strings. Many string
 allocators will not need to do anything in this mode.

 The strings allocated by this will have @c uDataAlloc set to true in
 the @ref QCBORItem when they are returned. The user of the strings
 will have to free them. How they free them, depends on the string
 allocator.

 If QCBORDecode_SetMemPool() is called, the internal MemPool will be
 used. It has its own internal implementation of this function, so
 one does not need to be implemented.
 */
typedef UsefulBuf (* QCBORStringAllocate)(void *pAllocateCxt, void *pOldMem, size_t uNewSize);


/**
 This only matters if you use the built-in string allocator by setting
 it up with QCBORDecode_SetMemPool(). This is the size of the overhead
 needed by QCBORDecode_SetMemPool(). The amount of memory available
 for decoded strings will be the size of the buffer given to
 QCBORDecode_SetMemPool() less this amount.

 If you write your own string allocator or use the separately
 available malloc based string allocator, this size will not apply.
 */
#define QCBOR_DECODE_MIN_MEM_POOL_SIZE 8


/**
 This is used by QCBORDecode_SetCallerConfiguredTagList() to set a
 list of tags beyond the built-in ones.

 See also QCBORDecode_GetNext() for general description of tag
 decoding.
 */
typedef struct {
   /** The number of tags in the @c puTags. The maximum size is @ref
       QCBOR_MAX_CUSTOM_TAGS. */
   uint8_t uNumTags;
   /** An array of tags to add to recognize in addition to the
       built-in ones. */
   const uint64_t *puTags;
} QCBORTagListIn;


/**
 This is for QCBORDecode_GetNextWithTags() to be able to return the
 full list of tags on an item. It is not needed for most CBOR protocol
 implementations. Its primary use is for pretty-printing CBOR or
 protocol conversion to another format.

 On input, @c puTags points to a buffer to be filled in and
 uNumAllocated is the number of @c uint64_t values in the buffer.

 On output the buffer contains the tags for the item.  @c uNumUsed
 tells how many there are.
 */
typedef struct {
   uint8_t uNumUsed;
   uint8_t uNumAllocated;
   uint64_t *puTags;
} QCBORTagListOut;


/**
 QCBOREncodeContext is the data type that holds context for all the
 encoding functions. It is less than 200 bytes, so it can go on the
 stack. The contents are opaque, and the caller should not access
 internal members.  A context may be re used serially as long as it is
 re initialized.
 */
typedef struct _QCBOREncodeContext QCBOREncodeContext;


/**
 Initialize the encoder to prepare to encode some CBOR.

 @param[in,out]  pCtx     The encoder context to initialize.
 @param[in]      Storage  The buffer into which this encoded result
                          will be placed.

 Call this once at the start of an encoding of a CBOR structure. Then
 call the various @c QCBOREncode_AddXxx() functions to add the data
 items. Then call QCBOREncode_Finish().

 The maximum output buffer is @c UINT32_MAX (4GB). This is not a
 practical limit in any way and reduces the memory needed by the
 implementation.  The error @ref QCBOR_ERR_BUFFER_TOO_LARGE will be
 returned by QCBOREncode_Finish() if a larger buffer length is passed
 in.

 If this is called with @c Storage.ptr as @c NULL and @c Storage.len a
 large value like @c UINT32_MAX, all the QCBOREncode_AddXxx()
 functions and QCBOREncode_Finish() can still be called. No data will
 be encoded, but the length of what would be encoded will be
 calculated. The length of the encoded structure will be handed back
 in the call to QCBOREncode_Finish(). You can then allocate a buffer
 of that size and call all the encoding again, this time to fill in
 the buffer.

 A @ref QCBOREncodeContext can be reused over and over as long as
 QCBOREncode_Init() is called.
 */
void QCBOREncode_Init(QCBOREncodeContext *pCtx, UsefulBuf Storage);


/**
 @brief  Add a signed 64-bit integer to the encoded output.

 @param[in] pCtx   The encoding context to add the integer to.
 @param[in] nNum   The integer to add.

 The integer will be encoded and added to the CBOR output.

 This function figures out the size and the sign and encodes in the
 correct minimal CBOR. Specifically, it will select CBOR major type 0
 or 1 based on sign and will encode to 1, 2, 4 or 8 bytes depending on
 the value of the integer. Values less than 24 effectively encode to
 one byte because they are encoded in with the CBOR major type.  This
 is a neat and efficient characteristic of CBOR that can be taken
 advantage of when designing CBOR-based protocols. If integers like
 tags can be kept between -23 and 23 they will be encoded in one byte
 including the major type.

 If you pass a smaller int, say an @c int16_t or a small value, say
 100, the encoding will still be CBOR's most compact that can
 represent the value.  For example, CBOR always encodes the value 0 as
 one byte, 0x00. The representation as 0x00 includes identification of
 the type as an integer too as the major type for an integer is 0. See
 [RFC 7049] (https://tools.ietf.org/html/rfc7049) Appendix A for more
 examples of CBOR encoding. This compact encoding is also canonical
 CBOR as per section 3.9 in RFC 7049.

 There are no functions to add @c int16_t or @c int32_t because they
 are not necessary because this always encodes to the smallest number
 of bytes based on the value (If this code is running on a 32-bit
 machine having a way to add 32-bit integers would reduce code size
 some).

 If the encoding context is in an error state, this will do
 nothing. If an error occurs when adding this integer, the internal
 error flag will be set, and the error will be returned when
 QCBOREncode_Finish() is called.

 See also QCBOREncode_AddUInt64().
 */
void QCBOREncode_AddInt64(QCBOREncodeContext *pCtx, int64_t nNum);

static void QCBOREncode_AddInt64ToMap(QCBOREncodeContext *pCtx, const char *szLabel, int64_t uNum);

static void QCBOREncode_AddInt64ToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, int64_t uNum);


/**
 @brief  Add an unsigned 64-bit integer to the encoded output.

 @param[in] pCtx  The encoding context to add the integer to.
 @param[in] uNum  The integer to add.

 The integer will be encoded and added to the CBOR output.

 The only reason so use this function is for integers larger than @c
 INT64_MAX and smaller than @c UINT64_MAX. Otherwise
 QCBOREncode_AddInt64() will work fine.

 Error handling is the same as for QCBOREncode_AddInt64().
 */
void QCBOREncode_AddUInt64(QCBOREncodeContext *pCtx, uint64_t uNum);

static void QCBOREncode_AddUInt64ToMap(QCBOREncodeContext *pCtx, const char *szLabel, uint64_t uNum);

static void QCBOREncode_AddUInt64ToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, uint64_t uNum);


/**
 @brief  Add a UTF-8 text string to the encoded output.

 @param[in] pCtx   The encoding context to add the text to.
 @param[in] Text   Pointer and length of text to add.

 The text passed in must be unencoded UTF-8 according to [RFC 3629]
 (https://tools.ietf.org/html/rfc3629). There is no NULL
 termination. The text is added as CBOR major type 3.

 If called with @c nBytesLen equal to 0, an empty string will be
 added. When @c nBytesLen is 0, @c pBytes may be @c NULL.

 Note that the restriction of the buffer length to a @c uint32_t is
 entirely intentional as this encoder is not capable of encoding
 lengths greater. This limit to 4GB for a text string should not be a
 problem.

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddText(QCBOREncodeContext *pCtx, UsefulBufC Text);

static void QCBOREncode_AddTextToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Text);

static void QCBOREncode_AddTextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Text);


/**
 @brief  Add a UTF-8 text string to the encoded output.

 @param[in] pCtx      The encoding context to add the text to.
 @param[in] szString  Null-terminated text to add.

 This works the same as QCBOREncode_AddText().
 */
static void QCBOREncode_AddSZString(QCBOREncodeContext *pCtx, const char *szString);

static void QCBOREncode_AddSZStringToMap(QCBOREncodeContext *pCtx, const char *szLabel, const char *szString);

static void QCBOREncode_AddSZStringToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, const char *szString);


/**
 @brief  Add a floating-point number to the encoded output.

 @param[in] pCtx  The encoding context to add the double to.
 @param[in] dNum  The double-precision number to add.

 This outputs a floating-point number with CBOR major type 7.

 This will selectively encode the double-precision floating-point
 number as either double-precision, single-precision or
 half-precision. It will always encode infinity, NaN and 0 has half
 precision. If no precision will be lost in the conversion to
 half-precision, then it will be converted and encoded. If not and no
 precision will be lost in conversion to single-precision, then it
 will be converted and encoded. If not, then no conversion is
 performed, and it encoded as a double.

 Half-precision floating-point numbers take up 2 bytes, half that of
 single-precision, one quarter of double-precision

 This automatically reduces the size of encoded messages a lot, maybe
 even by four if most of values are 0, infinity or NaN.

 On decode, these will always be returned as a double.

 Error handling is the same as QCBOREncode_AddInt64().
 */
void QCBOREncode_AddDouble(QCBOREncodeContext *pCtx, double dNum);

static void QCBOREncode_AddDoubleToMap(QCBOREncodeContext *pCtx, const char *szLabel, double dNum);

static void QCBOREncode_AddDoubleToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, double dNum);


/**
 @brief Add an optional tag.

 @param[in] pCtx  The encoding context to add the tag to.
 @param[in] uTag  The tag to add

 This outputs a CBOR major type 6 item that tags the next data item
 that is output usually to indicate it is some new data type.

 For many of the common standard tags, a function to encode data using
 it is provided and this is not needed. For example,
 QCBOREncode_AddDateEpoch() already exists to output integers
 representing dates with the right tag.

 The tag is applied to the next data item added to the encoded
 output. That data item that is to be tagged can be of any major CBOR
 type. Any number of tags can be added to a data item by calling this
 multiple times before the data item is added.

 See @ref Tags-Overview for discussion of creating new non-standard
 tags. See QCBORDecode_GetNext() for discussion of decoding custom
 tags.
*/
void QCBOREncode_AddTag(QCBOREncodeContext *pCtx,uint64_t uTag);


/**
 @brief  Add an epoch-based date.

 @param[in] pCtx  The encoding context to add the date to.
 @param[in] date  Number of seconds since 1970-01-01T00:00Z in UTC time.

 As per RFC 7049 this is similar to UNIX/Linux/POSIX dates. This is
 the most compact way to specify a date and time in CBOR. Note that
 this is always UTC and does not include the time zone.  Use
 QCBOREncode_AddDateString() if you want to include the time zone.

 The integer encoding rules apply here so the date will be encoded in
 a minimal number of bytes. Until about the year 2106 these dates will
 encode in 6 bytes -- one byte for the tag, one byte for the type and
 4 bytes for the integer. After that it will encode to 10 bytes.

 Negative values are supported for dates before 1970.

 If you care about leap-seconds and that level of accuracy, make sure
 the system you are running this code on does it correctly. This code
 just takes the value passed in.

 This implementation cannot encode fractional seconds using float or
 double even though that is allowed by CBOR, but you can encode them
 if you want to by calling QCBOREncode_AddDouble() and
 QCBOREncode_AddTag().

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddDateEpoch(QCBOREncodeContext *pCtx, int64_t date);

static void QCBOREncode_AddDateEpochToMap(QCBOREncodeContext *pCtx, const char *szLabel, int64_t date);

static  void QCBOREncode_AddDateEpochToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, int64_t date);


/**
 @brief Add a byte string to the encoded output.

 @param[in] pCtx   The encoding context to add the bytes to.
 @param[in] Bytes  Pointer and length of the input data.

 Simply adds the bytes to the encoded output as CBOR major type 2.

 If called with @c Bytes.len equal to 0, an empty string will be
 added. When @c Bytes.len is 0, @c Bytes.ptr may be @c NULL.

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddBytes(QCBOREncodeContext *pCtx, UsefulBufC Bytes);

static void QCBOREncode_AddBytesToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes);

static void QCBOREncode_AddBytesToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes);



/**
 @brief Add a binary UUID to the encoded output.

 @param[in] pCtx   The encoding context to add the UUID to.
 @param[in] Bytes  Pointer and length of the binary UUID.

 A binary UUID as defined in [RFC 4122]
 (https://tools.ietf.org/html/rfc4122) is added to the output.

 It is output as CBOR major type 2, a binary string, with tag @ref
 CBOR_TAG_BIN_UUID indicating the binary string is a UUID.
 */
static void QCBOREncode_AddBinaryUUID(QCBOREncodeContext *pCtx, UsefulBufC Bytes);

static void QCBOREncode_AddBinaryUUIDToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes);

static void QCBOREncode_AddBinaryUUIDToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes);


/**
 @brief Add a positive big number to the encoded output.

 @param[in] pCtx   The encoding context to add the big number to.
 @param[in] Bytes  Pointer and length of the big number.

 Big numbers are integers larger than 64-bits. Their format is
 described in [RFC 7049] (https://tools.ietf.org/html/rfc7049).

 It is output as CBOR major type 2, a binary string, with tag @ref
 CBOR_TAG_POS_BIGNUM indicating the binary string is a positive big
 number.

 Often big numbers are used to represent cryptographic keys, however,
 COSE which defines representations for keys chose not to use this
 particular type.
 */
static void QCBOREncode_AddPositiveBignum(QCBOREncodeContext *pCtx, UsefulBufC Bytes);

static void QCBOREncode_AddPositiveBignumToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes);

static void QCBOREncode_AddPositiveBignumToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes);


/**
 @brief Add a negative big number to the encoded output.

 @param[in] pCtx   The encoding context to add the big number to.
 @param[in] Bytes  Pointer and length of the big number.

 Big numbers are integers larger than 64-bits. Their format is
 described in [RFC 7049] (https://tools.ietf.org/html/rfc7049).

 It is output as CBOR major type 2, a binary string, with tag @ref
 CBOR_TAG_NEG_BIGNUM indicating the binary string is a negative big
 number.

 Often big numbers are used to represent cryptographic keys, however,
 COSE which defines representations for keys chose not to use this
 particular type.
 */
static void QCBOREncode_AddNegativeBignum(QCBOREncodeContext *pCtx, UsefulBufC Bytes);

static void QCBOREncode_AddNegativeBignumToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes);

static void QCBOREncode_AddNegativeBignumToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes);


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
/**
 @brief Add a decimal fraction to the encoded output.

 @param[in] pCtx            The encoding context to add the decimal fraction to.
 @param[in] nMantissa       The mantissa.
 @param[in] nBase10Exponent The exponent.

 The value is nMantissa * 10 ^ nBase10Exponent.

 A decimal fraction is good for exact representation of some values
 that can't be represented exactly with standard C (IEEE 754)
 floating-point numbers.  Much larger and much smaller numbers can
 also be represented than floating-point because of the larger number
 of bits in the exponent.

 The decimal fraction is conveyed as two integers, a mantissa and a
 base-10 scaling factor.

 For example, 273.15 is represented by the two integers 27315 and -2.

 The exponent and mantissa have the range from @c INT64_MIN to
 @c INT64_MAX for both encoding and decoding (CBOR allows @c -UINT64_MAX
 to @c UINT64_MAX, but this implementation doesn't support this range to
 reduce code size and interface complexity a little).

 CBOR Preferred encoding of the integers is used, thus they will be encoded
 in the smallest number of bytes possible.

 See also QCBOREncode_AddDecimalFractionBigNum() for a decimal
 fraction with arbitrarily large precision and QCBOREncode_AddBigFloat().

 There is no representation of positive or negative infinity or NaN
 (Not a Number). Use QCBOREncode_AddDouble() to encode them.

 See @ref expAndMantissa for decoded representation.
 */
static void QCBOREncode_AddDecimalFraction(QCBOREncodeContext *pCtx,
                                           int64_t             nMantissa,
                                           int64_t             nBase10Exponent);

static void QCBOREncode_AddDecimalFractionToMap(QCBOREncodeContext *pCtx,
                                                const char         *szLabel,
                                                int64_t             nMantissa,
                                                int64_t             nBase10Exponent);

static void QCBOREncode_AddDecimalFractionToMapN(QCBOREncodeContext *pCtx,
                                                 int64_t             nLabel,
                                                 int64_t             nMantissa,
                                                 int64_t             nBase10Exponent);

/**
 @brief Add a decimal fraction with a big number mantissa to the encoded output.

 @param[in] pCtx            The encoding context to add the decimal fraction to.
 @param[in] Mantissa        The mantissa.
 @param[in] bIsNegative     false if mantissa is positive, true if negative.
 @param[in] nBase10Exponent The exponent.

 This is the same as QCBOREncode_AddDecimalFraction() except the
 mantissa is a big number (See QCBOREncode_AddPositiveBignum())
 allowing for arbitrarily large precision.

 See @ref expAndMantissa for decoded representation.
 */
static void QCBOREncode_AddDecimalFractionBigNum(QCBOREncodeContext *pCtx,
                                                 UsefulBufC          Mantissa,
                                                 bool                bIsNegative,
                                                 int64_t             nBase10Exponent);

static void QCBOREncode_AddDecimalFractionBigNumToMap(QCBOREncodeContext *pCtx,
                                                      const char         *szLabel,
                                                      UsefulBufC          Mantissa,
                                                      bool                bIsNegative,
                                                      int64_t             nBase10Exponent);

static void QCBOREncode_AddDecimalFractionBigNumToMapN(QCBOREncodeContext *pCtx,
                                                       int64_t             nLabel,
                                                       UsefulBufC          Mantissa,
                                                       bool                bIsNegative,
                                                       int64_t             nBase10Exponent);

/**
 @brief Add a big floating-point number to the encoded output.

 @param[in] pCtx            The encoding context to add the bigfloat to.
 @param[in] nMantissa       The mantissa.
 @param[in] nBase2Exponent  The exponent.

 The value is nMantissa * 2 ^ nBase2Exponent.

 "Bigfloats", as CBOR terms them, are similar to IEEE floating-point
 numbers in having a mantissa and base-2 exponent, but they are not
 supported by hardware or encoded the same. They explicitly use two
 CBOR-encoded integers to convey the mantissa and exponent, each of which
 can be 8, 16, 32 or 64 bits. With both the mantissa and exponent
 64 bits they can express more precision and a larger range than an
 IEEE double floating-point number. See
 QCBOREncode_AddBigFloatBigNum() for even more precision.

 For example, 1.5 would be represented by a mantissa of 3 and an
 exponent of -1.

 The exponent and mantissa have the range from @c INT64_MIN to
 @c INT64_MAX for both encoding and decoding (CBOR allows @c -UINT64_MAX
 to @c UINT64_MAX, but this implementation doesn't support this range to
 reduce code size and interface complexity a little).

 CBOR Preferred encoding of the integers is used, thus they will be encoded
 in the smallest number of bytes possible.

 This can also be used to represent floating-point numbers in
 environments that don't support IEEE 754.

 See @ref expAndMantissa for decoded representation.
 */
static void QCBOREncode_AddBigFloat(QCBOREncodeContext *pCtx,
                                    int64_t             nMantissa,
                                    int64_t             nBase2Exponent);

static void QCBOREncode_AddBigFloatToMap(QCBOREncodeContext *pCtx,
                                         const char         *szLabel,
                                         int64_t             nMantissa,
                                         int64_t             nBase2Exponent);

static void QCBOREncode_AddBigFloatToMapN(QCBOREncodeContext *pCtx,
                                          int64_t             nLabel,
                                          int64_t             nMantissa,
                                          int64_t             nBase2Exponent);


/**
 @brief Add a big floating-point number with a big number mantissa to
        the encoded output.

 @param[in] pCtx            The encoding context to add the bigfloat to.
 @param[in] Mantissa        The mantissa.
 @param[in] bIsNegative     false if mantissa is positive, true if negative.
 @param[in] nBase2Exponent  The exponent.

 This is the same as QCBOREncode_AddBigFloat() except the mantissa is
 a big number (See QCBOREncode_AddPositiveBignum()) allowing for
 arbitrary precision.

 See @ref expAndMantissa for decoded representation.
 */
static void QCBOREncode_AddBigFloatBigNum(QCBOREncodeContext *pCtx,
                                          UsefulBufC          Mantissa,
                                          bool                bIsNegative,
                                          int64_t             nBase2Exponent);

static void QCBOREncode_AddBigFloatBigNumToMap(QCBOREncodeContext *pCtx,
                                               const char         *szLabel,
                                               UsefulBufC          Mantissa,
                                               bool                bIsNegative,
                                               int64_t             nBase2Exponent);

static void QCBOREncode_AddBigFloatBigNumToMapN(QCBOREncodeContext *pCtx,
                                                int64_t             nLabel,
                                                UsefulBufC          Mantissa,
                                                bool                bIsNegative,
                                                int64_t             nBase2Exponent);
#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */


/**
 @brief Add a text URI to the encoded output.

 @param[in] pCtx  The encoding context to add the URI to.
 @param[in] URI   Pointer and length of the URI.

 The format of URI must be per [RFC 3986]
 (https://tools.ietf.org/html/rfc3986).

 It is output as CBOR major type 3, a text string, with tag @ref
 CBOR_TAG_URI indicating the text string is a URI.

 A URI in a NULL-terminated string, @c szURI, can be easily added with
 this code:

      QCBOREncode_AddURI(pCtx, UsefulBuf_FromSZ(szURI));
 */
static void QCBOREncode_AddURI(QCBOREncodeContext *pCtx, UsefulBufC URI);

static void QCBOREncode_AddURIToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC URI);

static void QCBOREncode_AddURIToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC URI);


/**
 @brief Add Base64-encoded text to encoded output.

 @param[in] pCtx     The encoding context to add the base-64 text to.
 @param[in] B64Text  Pointer and length of the base-64 encoded text.

 The text content is Base64 encoded data per [RFC 4648]
 (https://tools.ietf.org/html/rfc4648).

 It is output as CBOR major type 3, a text string, with tag @ref
 CBOR_TAG_B64 indicating the text string is Base64 encoded.
 */
static void QCBOREncode_AddB64Text(QCBOREncodeContext *pCtx, UsefulBufC B64Text);

static void QCBOREncode_AddB64TextToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC B64Text);

static void QCBOREncode_AddB64TextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC B64Text);


/**
 @brief Add base64url encoded data to encoded output.

 @param[in] pCtx     The encoding context to add the base64url to.
 @param[in] B64Text  Pointer and length of the base64url encoded text.

 The text content is base64URL encoded text as per [RFC 4648]
 (https://tools.ietf.org/html/rfc4648).

 It is output as CBOR major type 3, a text string, with tag @ref
 CBOR_TAG_B64URL indicating the text string is a Base64url encoded.
 */
static void QCBOREncode_AddB64URLText(QCBOREncodeContext *pCtx, UsefulBufC B64Text);

static void QCBOREncode_AddB64URLTextToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC B64Text);

static void QCBOREncode_AddB64URLTextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC B64Text);


/**
 @brief Add Perl Compatible Regular Expression.

 @param[in] pCtx    The encoding context to add the regular expression to.
 @param[in] Regex   Pointer and length of the regular expression.

 The text content is Perl Compatible Regular
 Expressions (PCRE) / JavaScript syntax [ECMA262].

 It is output as CBOR major type 3, a text string, with tag @ref
 CBOR_TAG_REGEX indicating the text string is a regular expression.
 */
static void QCBOREncode_AddRegex(QCBOREncodeContext *pCtx, UsefulBufC Regex);

static void QCBOREncode_AddRegexToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Regex);

static void QCBOREncode_AddRegexToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Regex);


/**
 @brief MIME encoded text to the encoded output.

 @param[in] pCtx      The encoding context to add the MIME data to.
 @param[in] MIMEData  Pointer and length of the regular expression.

 The text content is in MIME format per [RFC 2045]
 (https://tools.ietf.org/html/rfc2045) including the headers. Note
 that this only supports text-format MIME. Binary MIME is not
 supported.

 It is output as CBOR major type 3, a text string, with tag
 @ref CBOR_TAG_MIME indicating the text string is MIME data.
 */
static void QCBOREncode_AddMIMEData(QCBOREncodeContext *pCtx, UsefulBufC MIMEData);

static void QCBOREncode_AddMIMEDataToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC MIMEData);

static void QCBOREncode_AddMIMEDataToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC MIMEData);


/**
 @brief  Add an RFC 3339 date string

 @param[in] pCtx    The encoding context to add the date to.
 @param[in] szDate  Null-terminated string with date to add.

 The string szDate should be in the form of [RFC 3339]
 (https://tools.ietf.org/html/rfc3339) as defined by section 3.3 in
 [RFC 4287] (https://tools.ietf.org/html/rfc4287). This is as
 described in section 2.4.1 in [RFC 7049]
 (https://tools.ietf.org/html/rfc7049).

 Note that this function doesn't validate the format of the date string
 at all. If you add an incorrect format date string, the generated
 CBOR will be incorrect and the receiver may not be able to handle it.

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddDateString(QCBOREncodeContext *pCtx, const char *szDate);

static void QCBOREncode_AddDateStringToMap(QCBOREncodeContext *pCtx, const char *szLabel, const char *szDate);

static void QCBOREncode_AddDateStringToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, const char *szDate);


/**
 @brief  Add a standard Boolean.

 @param[in] pCtx   The encoding context to add the Boolean to.
 @param[in] b      true or false from @c <stdbool.h>.

 Adds a Boolean value as CBOR major type 7.

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddBool(QCBOREncodeContext *pCtx, bool b);

static void QCBOREncode_AddBoolToMap(QCBOREncodeContext *pCtx, const char *szLabel, bool b);

static void QCBOREncode_AddBoolToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, bool b);



/**
 @brief  Add a NULL to the encoded output.

 @param[in] pCtx  The encoding context to add the NULL to.

 Adds the NULL value as CBOR major type 7.

 This NULL doesn't have any special meaning in CBOR such as a
 terminating value for a string or an empty value.

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddNULL(QCBOREncodeContext *pCtx);

static void QCBOREncode_AddNULLToMap(QCBOREncodeContext *pCtx, const char *szLabel);

static void QCBOREncode_AddNULLToMapN(QCBOREncodeContext *pCtx, int64_t nLabel);


/**
 @brief  Add an "undef" to the encoded output.

 @param[in] pCtx  The encoding context to add the "undef" to.

 Adds the undef value as CBOR major type 7.

 Note that this value will not translate to JSON.

 This Undef doesn't have any special meaning in CBOR such as a
 terminating value for a string or an empty value.

 Error handling is the same as QCBOREncode_AddInt64().
 */
static void QCBOREncode_AddUndef(QCBOREncodeContext *pCtx);

static void QCBOREncode_AddUndefToMap(QCBOREncodeContext *pCtx, const char *szLabel);

static void QCBOREncode_AddUndefToMapN(QCBOREncodeContext *pCtx, int64_t nLabel);


/**
 @brief  Indicates that the next items added are in an array.

 @param[in] pCtx The encoding context to open the array in.

 Arrays are the basic CBOR aggregate or structure type. Call this
 function to start or open an array. Then call the various @c
 QCBOREncode_AddXxx() functions to add the items that go into the
 array. Then call QCBOREncode_CloseArray() when all items have been
 added. The data items in the array can be of any type and can be of
 mixed types.

 Nesting of arrays and maps is allowed and supported just by calling
 QCBOREncode_OpenArray() again before calling
 QCBOREncode_CloseArray().  While CBOR has no limit on nesting, this
 implementation does in order to keep it smaller and simpler.  The
 limit is @ref QCBOR_MAX_ARRAY_NESTING. This is the max number of
 times this can be called without calling
 QCBOREncode_CloseArray(). QCBOREncode_Finish() will return @ref
 QCBOR_ERR_ARRAY_NESTING_TOO_DEEP when it is called as this function
 just sets an error state and returns no value when this occurs.

 If you try to add more than @ref QCBOR_MAX_ITEMS_IN_ARRAY items to a
 single array or map, @ref QCBOR_ERR_ARRAY_TOO_LONG will be returned
 when QCBOREncode_Finish() is called.

 An array itself must have a label if it is being added to a map.
 Note that array elements do not have labels (but map elements do).

 An array itself may be tagged by calling QCBOREncode_AddTag() before this call.
 */
static void QCBOREncode_OpenArray(QCBOREncodeContext *pCtx);

static void QCBOREncode_OpenArrayInMap(QCBOREncodeContext *pCtx, const char *szLabel);

static void QCBOREncode_OpenArrayInMapN(QCBOREncodeContext *pCtx,  int64_t nLabel);


/**
 @brief Close an open array.

 @param[in] pCtx The encoding context to close the array in.

 The closes an array opened by QCBOREncode_OpenArray(). It reduces
 nesting level by one. All arrays (and maps) must be closed before
 calling QCBOREncode_Finish().

 When an error occurs as a result of this call, the encoder records
 the error and enters the error state. The error will be returned when
 QCBOREncode_Finish() is called.

 If this has been called more times than QCBOREncode_OpenArray(), then
 @ref QCBOR_ERR_TOO_MANY_CLOSES will be returned when QCBOREncode_Finish()
 is called.

 If this is called and it is not an array that is currently open, @ref
 QCBOR_ERR_CLOSE_MISMATCH will be returned when QCBOREncode_Finish()
 is called.
 */
static void QCBOREncode_CloseArray(QCBOREncodeContext *pCtx);


/**
 @brief  Indicates that the next items added are in a map.

 @param[in] pCtx The encoding context to open the map in.

 See QCBOREncode_OpenArray() for more information, particularly error
 handling.

 CBOR maps are an aggregate type where each item in the map consists
 of a label and a value. They are similar to JSON objects.

 The value can be any CBOR type including another map.

 The label can also be any CBOR type, but in practice they are
 typically, integers as this gives the most compact output. They might
 also be text strings which gives readability and translation to JSON.

 Every @c QCBOREncode_AddXxx() call has one version that ends with @c
 InMap for adding items to maps with string labels and one that ends
 with @c InMapN that is for adding with integer labels.

 RFC 7049 uses the term "key" instead of "label".

 If you wish to use map labels that are neither integer labels nor
 text strings, then just call the QCBOREncode_AddXxx() function
 explicitly to add the label. Then call it again to add the value.

 See the [RFC 7049] (https://tools.ietf.org/html/rfc7049) for a lot
 more information on creating maps.
 */
static void QCBOREncode_OpenMap(QCBOREncodeContext *pCtx);

static void QCBOREncode_OpenMapInMap(QCBOREncodeContext *pCtx, const char *szLabel);

static void QCBOREncode_OpenMapInMapN(QCBOREncodeContext *pCtx, int64_t nLabel);



/**
 @brief Close an open map.

 @param[in] pCtx The encoding context to close the map in .

 This closes a map opened by QCBOREncode_OpenMap(). It reduces nesting
 level by one.

 When an error occurs as a result of this call, the encoder records
 the error and enters the error state. The error will be returned when
 QCBOREncode_Finish() is called.

 If this has been called more times than QCBOREncode_OpenMap(),
 then @ref QCBOR_ERR_TOO_MANY_CLOSES will be returned when
 QCBOREncode_Finish() is called.

 If this is called and it is not a map that is currently open, @ref
 QCBOR_ERR_CLOSE_MISMATCH will be returned when QCBOREncode_Finish()
 is called.
 */
static void QCBOREncode_CloseMap(QCBOREncodeContext *pCtx);


/**
 @brief Indicate start of encoded CBOR to be wrapped in a bstr.

 @param[in] pCtx The encoding context to open the bstr-wrapped CBOR in.

 All added encoded items between this call and a call to
 QCBOREncode_CloseBstrWrap() will be wrapped in a bstr. They will
 appear in the final output as a byte string.  That byte string will
 contain encoded CBOR.

 The typical use case is for encoded CBOR that is to be
 cryptographically hashed, as part of a [RFC 8152, COSE]
 (https://tools.ietf.org/html/rfc8152) implementation. This avoids
 having to encode the items first in one buffer (e.g., the COSE
 payload) and then add that buffer as a bstr to another encoding
 (e.g. the COSE to-be-signed bytes, the @c Sig_structure) potentially
 saving a lot of memory.

 When constructing cryptographically signed CBOR objects, maps or
 arrays, they typically are encoded normally and then wrapped as a
 byte string. The COSE standard for example does this. The wrapping is
 simply treating the encoded CBOR map as a byte string.

 The stated purpose of this wrapping is to prevent code relaying the
 signed data but not verifying it from tampering with the signed data
 thus making the signature unverifiable. It is also quite beneficial
 for the signature verification code. Standard CBOR parsers usually do
 not give access to partially parsed CBOR as would be need to check
 the signature of some CBOR. With this wrapping, standard CBOR parsers
 can be used to get to all the data needed for a signature
 verification.
 */
static void QCBOREncode_BstrWrap(QCBOREncodeContext *pCtx);

static void QCBOREncode_BstrWrapInMap(QCBOREncodeContext *pCtx, const char *szLabel);

static void QCBOREncode_BstrWrapInMapN(QCBOREncodeContext *pCtx, int64_t nLabel);


/**
 @brief Close a wrapping bstr.

 @param[in] pCtx           The encoding context to close of bstr wrapping in.
 @param[out] pWrappedCBOR  A @ref UsefulBufC containing wrapped bytes.

 The closes a wrapping bstr opened by QCBOREncode_CloseBstrWrap(). It reduces
 nesting level by one.

 A pointer and length of the enclosed encoded CBOR is returned in @c
 *pWrappedCBOR if it is not @c NULL. The main purpose of this is so
 this data can be hashed (e.g., with SHA-256) as part of a [RFC 8152,
 COSE] (https://tools.ietf.org/html/rfc8152)
 implementation. **WARNING**, this pointer and length should be used
 right away before any other calls to @c QCBOREncode_Xxx() as they
 will move data around and the pointer and length will no longer be to
 the correct encoded CBOR.

 When an error occurs as a result of this call, the encoder records
 the error and enters the error state. The error will be returned when
 QCBOREncode_Finish() is called.

 If this has been called more times than QCBOREncode_BstrWrap(),
 then @ref QCBOR_ERR_TOO_MANY_CLOSES will be returned when
 QCBOREncode_Finish() is called.

 If this is called and it is not a wrapping bstr that is currently
 open, @ref QCBOR_ERR_CLOSE_MISMATCH will be returned when
 QCBOREncode_Finish() is called.
 */
static void QCBOREncode_CloseBstrWrap(QCBOREncodeContext *pCtx, UsefulBufC *pWrappedCBOR);


/**
 @brief Add some already-encoded CBOR bytes.

 @param[in] pCtx     The encoding context to add the already-encode CBOR to.
 @param[in] Encoded  The already-encoded CBOR to add to the context.

 The encoded CBOR being added must be fully conforming CBOR. It must
 be complete with no arrays or maps that are incomplete. While this
 encoder doesn't ever produce indefinite lengths, it is OK for the
 raw CBOR added here to have indefinite lengths.

 The raw CBOR added here is not checked in anyway. If it is not
 conforming or has open arrays or such, the final encoded CBOR
 will probably be wrong or not what was intended.

 If the encoded CBOR being added here contains multiple items, they
 must be enclosed in a map or array. At the top level the raw
 CBOR must be a single data item.
 */
static void QCBOREncode_AddEncoded(QCBOREncodeContext *pCtx, UsefulBufC Encoded);

static void QCBOREncode_AddEncodedToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Encoded);

static void QCBOREncode_AddEncodedToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Encoded);


/**
 @brief Get the encoded result.

 @param[in] pCtx           The context to finish encoding with.
 @param[out] pEncodedCBOR  Pointer and length of encoded CBOR.

 @retval QCBOR_ERR_TOO_MANY_CLOSES         Nesting error

 @retval QCBOR_ERR_CLOSE_MISMATCH          Nesting error

 @retval QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN Nesting error

 @retval QCBOR_ERR_BUFFER_TOO_LARGE        Encoded output buffer size

 @retval QCBOR_ERR_BUFFER_TOO_SMALL        Encoded output buffer size

 @retval QCBOR_ERR_ARRAY_NESTING_TOO_DEEP  Implementation limit

 @retval QCBOR_ERR_ARRAY_TOO_LONG          Implementation limit

 If this returns success @ref QCBOR_SUCCESS the encoding was a success
 and the return length is correct and complete.

 If no buffer was passed to QCBOREncode_Init(), then only the length
 was computed. If a buffer was passed, then the encoded CBOR is in the
 buffer.

 Encoding errors primarily manifest here as most other encoding function
 do no return an error. They just set the error state in the encode
 context after which no encoding function does anything.

 Three types of errors manifest here. The first type are nesting
 errors where the number of @c QCBOREncode_OpenXxx() calls do not
 match the number @c QCBOREncode_CloseXxx() calls. The solution is to
 fix the calling code.

 The second type of error is because the buffer given is either too
 small or too large. The remedy is to give a correctly sized buffer.

 The third type are due to limits in this implementation.  @ref
 QCBOR_ERR_ARRAY_NESTING_TOO_DEEP can be worked around by encoding the
 CBOR in two (or more) phases and adding the CBOR from the first phase
 to the second with @c QCBOREncode_AddEncoded().

 If an error is returned, the buffer may have partially encoded
 incorrect CBOR in it and it should not be used. Likewise, the length
 may be incorrect and should not be used.

 Note that the error could have occurred in one of the many @c
 QCBOREncode_AddXxx() calls long before QCBOREncode_Finish() was
 called. This error handling reduces the CBOR implementation size but
 makes debugging harder.

 This may be called multiple times. It will always return the same. It
 can also be interleaved with calls to QCBOREncode_FinishGetSize().

 QCBOREncode_GetErrorState() can be called to get the current
 error state and abort encoding early as an optimization, but is
 is never required.
 */
QCBORError QCBOREncode_Finish(QCBOREncodeContext *pCtx, UsefulBufC *pEncodedCBOR);


/**
 @brief Get the encoded CBOR and error status.

 @param[in] pCtx          The context to finish encoding with.
 @param[out] uEncodedLen  The length of the encoded or potentially
                          encoded CBOR in bytes.

 @return The same errors as QCBOREncode_Finish().

 This functions the same as QCBOREncode_Finish(), but only returns the
 size of the encoded output.
 */
QCBORError QCBOREncode_FinishGetSize(QCBOREncodeContext *pCtx, size_t *uEncodedLen);


/**
 @brief Indicate whether output buffer is NULL or not.

 @param[in] pCtx  The encoding context.

 @return 1 if the output buffer is @c NULL.

 Sometimes a @c NULL input buffer is given to QCBOREncode_Init() so
 that the size of the generated CBOR can be calculated without
 allocating a buffer for it. This returns 1 when the output buffer is
 NULL and 0 when it is not.
*/
static int QCBOREncode_IsBufferNULL(QCBOREncodeContext *pCtx);

 /**
 @brief Get the encoding error state.

 @param[in] pCtx  The encoding context.

 @return One of \ref QCBORError. See return values from
         QCBOREncode_Finish()

 Normally encoding errors need only be handled at the end of encoding
 when QCBOREncode_Finish() is called. This can be called to get the
 error result before finish should there be a need to halt encoding
 before QCBOREncode_Finish() is called.
*/
static QCBORError QCBOREncode_GetErrorState(QCBOREncodeContext *pCtx);



/**
 QCBORDecodeContext is the data type that holds context decoding the
 data items for some received CBOR.  It is about 100 bytes, so it can
 go on the stack.  The contents are opaque, and the caller should not
 access any internal items.  A context may be re used serially as long
 as it is re initialized.
 */
typedef struct _QCBORDecodeContext QCBORDecodeContext;


/**
 Initialize the CBOR decoder context.

 @param[in] pCtx         The context to initialize.
 @param[in] EncodedCBOR  The buffer with CBOR encoded bytes to be decoded.
 @param[in] nMode        See below and @ref QCBORDecodeMode.

 Initialize context for a pre-order traversal of the encoded CBOR
 tree.

 Most CBOR decoding can be completed by calling this function to start
 and QCBORDecode_GetNext() in a loop.

 If indefinite-length strings are to be decoded, then
 QCBORDecode_SetMemPool() or QCBORDecode_SetUpAllocator() must be
 called to set up a string allocator.

 If tags other than built-in tags are to be recognized and recorded in
 @c uTagBits, then QCBORDecode_SetCallerConfiguredTagList() must be
 called. The built-in tags are those for which a macro of the form @c
 CBOR_TAG_XXX is defined.

 Three decoding modes are supported.  In normal mode, @ref
 QCBOR_DECODE_MODE_NORMAL, maps are decoded and strings and integers
 are accepted as map labels. If a label is other than these, the error
 @ref QCBOR_ERR_MAP_LABEL_TYPE is returned by QCBORDecode_GetNext().

 In strings-only mode, @ref QCBOR_DECODE_MODE_MAP_STRINGS_ONLY, only
 text strings are accepted for map labels.  This lines up with CBOR
 that converts to JSON. The error @ref QCBOR_ERR_MAP_LABEL_TYPE is
 returned by QCBORDecode_GetNext() if anything but a text string label
 is encountered.

 In @ref QCBOR_DECODE_MODE_MAP_AS_ARRAY maps are treated as special
 arrays.  They will be return with special @c uDataType @ref
 QCBOR_TYPE_MAP_AS_ARRAY and @c uCount, the number of items, will be
 double what it would be for a normal map because the labels are also
 counted. This mode is useful for decoding CBOR that has labels that
 are not integers or text strings, but the caller must manage much of
 the map decoding.
 */
void QCBORDecode_Init(QCBORDecodeContext *pCtx, UsefulBufC EncodedCBOR, QCBORDecodeMode nMode);


/**
 @brief Set up the MemPool string allocator for indefinite-length strings.

 @param[in] pCtx         The decode context.
 @param[in] MemPool      The pointer and length of the memory pool.
 @param[in] bAllStrings  If true, all strings, even of definite
                         length, will be allocated with the string
                         allocator.

 @return Error if the MemPool was less than @ref QCBOR_DECODE_MIN_MEM_POOL_SIZE.

 indefinite-length strings (text and byte) cannot be decoded unless
 there is a string allocator configured. MemPool is a simple built-in
 string allocator that allocates bytes from a memory pool handed to it
 by calling this function.  The memory pool is just a pointer and
 length for some block of memory that is to be used for string
 allocation. It can come from the stack, heap or other.

 The memory pool must be @ref QCBOR_DECODE_MIN_MEM_POOL_SIZE plus
 space for all the strings allocated.  There is no overhead per string
 allocated. A conservative way to size this buffer is to make it the
 same size as the CBOR being decoded plus @ref
 QCBOR_DECODE_MIN_MEM_POOL_SIZE.

 This memory pool is used for all indefinite-length strings that are
 text strings or byte strings, including strings used as labels.

 The pointers to strings in @ref QCBORItem will point into the memory
 pool set here. They do not need to be individually freed. Just
 discard the buffer when they are no longer needed.

 If @c bAllStrings is set, then the size will be the overhead plus the
 space to hold **all** strings, definite and indefinite-length, value
 or label. The advantage of this is that after the decode is complete,
 the original memory holding the encoded CBOR does not need to remain
 valid.

 If this function is never called because there is no need to support
 indefinite-length strings, the internal MemPool implementation should
 be dead-stripped by the loader and not add to code size.
 */
QCBORError QCBORDecode_SetMemPool(QCBORDecodeContext *pCtx, UsefulBuf MemPool, bool bAllStrings);


/**
 @brief Sets up a custom string allocator for indefinite-length strings

 @param[in] pCtx                 The decoder context to set up an
                                 allocator for.
 @param[in] pfAllocateFunction   Pointer to function that will be
                                 called by QCBOR for allocations and
                                 frees.
 @param[in] pAllocateContext     Context passed to @c
                                 pfAllocateFunction.
 @param[in] bAllStrings          If true, all strings, even of definite
                                 length, will be allocated with the
                                 string allocator.

 indefinite-length strings (text and byte) cannot be decoded unless
 there a string allocator is configured. QCBORDecode_SetUpAllocator()
 allows the caller to configure an external string allocator
 implementation if the internal string allocator is not suitable. See
 QCBORDecode_SetMemPool() to configure the internal allocator. Note
 that the internal allocator is not automatically set up.

 The string allocator configured here can be a custom one designed and
 implemented by the caller.  See @ref QCBORStringAllocate for the
 requirements for a string allocator implementation.

 A malloc-based string external allocator can be obtained by calling
 @c QCBORDecode_MakeMallocStringAllocator(). It will return a function
 and pointer that can be given here as @c pAllocatorFunction and @c
 pAllocatorContext. It uses standard @c malloc() so @c free() must be
 called on all strings marked by @c uDataAlloc @c == @c 1 or @c
 uLabelAlloc @c == @c 1 in @ref QCBORItem.

 Note that an older version of this function took an allocator
 structure, rather than single function and pointer.  The older
 version @c QCBORDecode_MakeMallocStringAllocator() also implemented
 the older interface.
 */
void QCBORDecode_SetUpAllocator(QCBORDecodeContext *pCtx,
                                QCBORStringAllocate pfAllocateFunction,
                                void *pAllocateContext,
                                bool bAllStrings);

/**
 @brief Configure list of caller-selected tags to be recognized.

 @param[in] pCtx       The decode context.
 @param[out] pTagList  Structure holding the list of tags to configure.

 This is used to tell the decoder about tags beyond those that are
 built-in that should be recognized. The built-in tags are those with
 macros of the form @c CBOR_TAG_XXX.

 The list pointed to by @c pTagList must persist during decoding.  No
 copy of it is made.

 The maximum number of tags that can be added is @ref
 QCBOR_MAX_CUSTOM_TAGS.  If a list larger than this is given, the
 error will be returned when QCBORDecode_GetNext() is called, not
 here.

 See description of @ref QCBORTagListIn.
 */
void QCBORDecode_SetCallerConfiguredTagList(QCBORDecodeContext *pCtx, const QCBORTagListIn *pTagList);


/**
 @brief Gets the next item (integer, byte string, array...) in
        preorder traversal of CBOR tree.

 @param[in]  pCtx          The decoder context.
 @param[out] pDecodedItem  Holds the CBOR item just decoded.

 @retval QCBOR_ERR_INDEFINITE_STRING_CHUNK  Not well-formed, one of the
                                            chunks in indefinite-length
                                            string is wrong type.

 @retval QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN  Not well-formed, array or map
                                            not closed.

 @retval QCBOR_ERR_UNSUPPORTED     Not well-formed, input contains
                                   unsupported CBOR.

 @retval QCBOR_ERR_HIT_END         Not well-formed, unexpectedly ran out
                                   of bytes.

 @retval QCBOR_ERR_BAD_TYPE_7      Not well-formed, bad simple type value.

 @retval QCBOR_ERR_BAD_BREAK       Not well-formed, break occurs where
                                   not allowed.

 @retval QCBOR_ERR_EXTRA_BYTES     Not well-formed, unprocessed bytes at
                                   the end.

 @retval QCBOR_ERR_BAD_INT         Not well-formed, length of integer is
                                   bad.

 @retval QCBOR_ERR_BAD_OPT_TAG     Invalid CBOR, tag on wrong type.

 @retval QCBOR_ERR_ARRAY_TOO_LONG  Implementation limit, array or map
                                   too long.

 @retval QCBOR_ERR_INT_OVERFLOW    Implementation limit, negative
                                   integer too large.

 @retval QCBOR_ERR_DATE_OVERFLOW   Implementation limit, date larger
                                   than can be handled.

 @retval QCBOR_ERR_ARRAY_NESTING_TOO_DEEP  Implementation limit, nesting
                                           too deep.

 @retval QCBOR_ERR_STRING_ALLOCATE Resource exhaustion, string allocator
                                   failed.

 @retval QCBOR_ERR_MAP_LABEL_TYPE  Configuration error / Implementation
                                   limit encountered a map label this is
                                   not a string on an integer.

 @retval QCBOR_ERR_NO_STRING_ALLOCATOR  Configuration error, encountered
                                        indefinite-length string with no
                                        allocator configured.
 @retval QCBOR_ERR_NO_MORE_ITEMS   No more bytes to decode. The previous
                                   item was successfully decoded. This
                                   is usually how the non-error end of
                                   a CBOR stream / sequence is detected.

 @c pDecodedItem is filled in with the value parsed. Generally, the
 following data is returned in the structure:

 - @c uDataType which indicates which member of the @c val union the
   data is in. This decoder figures out the type based on the CBOR
   major type, the CBOR "additionalInfo", the CBOR optional tags and
   the value of the integer.

 - The value of the item, which might be an integer, a pointer and a
   length, the count of items in an array, a floating-point number or
   other.

 - The nesting level for maps and arrays.

 - The label for an item in a map, which may be a text or byte string
   or an integer.

 - The CBOR optional tag or tags.

 See documentation on in the data type @ref _QCBORItem for all the
 details on what is returned.

 This function handles arrays and maps. When first encountered a @ref
 QCBORItem will be returned with major type @ref QCBOR_TYPE_ARRAY or
 @ref QCBOR_TYPE_MAP. @c QCBORItem.val.uCount will indicate the number
 of Items in the array or map.  Typically, an implementation will call
 QCBORDecode_GetNext() in a for loop to fetch them all. When decoding
 indefinite-length maps and arrays, @c QCBORItem.val.uCount is @c
 UINT16_MAX and @c uNextNestLevel must be used to know when the end of
 a map or array is reached.

 Nesting level 0 is the outside top-most nesting level. For example,
 in a CBOR structure with two items, an integer and a byte string
 only, both would be at nesting level 0.  A CBOR structure with an
 array open, an integer and a byte string, would have the integer and
 byte string as nesting level 1.

 Here is an example of how the nesting level is reported with no arrays
 or maps at all.

 @verbatim
 CBOR Structure           Nesting Level
 Integer                    0
 Byte String                0
 @endverbatim

 Here is an example of how the nesting level is reported with a simple
 array and some top-level items.

 @verbatim
 Integer                    0
 Array (with 2 items)       0
 Byte String                1
 Byte string                1
 Integer                    0
 @endverbatim


 Here's a more complex example
 @verbatim

 Map with 2 items           0
 Text string                1
 Array with 3 integers      1
 integer                    2
 integer                    2
 integer                    2
 text string                1
 byte string                1
 @endverbatim

 In @ref _QCBORItem, @c uNextNestLevel is the nesting level for the
 next call to QCBORDecode_GetNext(). It indicates if any maps or
 arrays were closed out during the processing of the just-fetched @ref
 QCBORItem. This processing includes a look-ahead for any breaks that
 close out indefinite-length arrays or maps. This value is needed to
 be able to understand the hierarchical structure. If @c
 uNextNestLevel is not equal to @c uNestLevel the end of the current
 map or array has been encountered. This works the same for both
 definite and indefinite-length arrays.

 This decoder support CBOR type 6 tagging. The decoding of particular
 given tag value may be supported in one of three different ways.

 First, some common tags are fully and transparently supported by
 automatically decoding them and returning them in a @ref QCBORItem.
 These tags have a @c QCBOR_TYPE_XXX associated with them and manifest
 pretty much the same as a standard CBOR type. @ref
 QCBOR_TYPE_DATE_EPOCH and the @c epochDate member of @ref QCBORItem
 is an example.

 Second are tags that are automatically recognized, but not decoded.
 These are tags that have a @c \#define of the form @c CBOR_TAG_XXX.
 These are recorded in the @c uTagBits member of @ref QCBORItem. There
 is an internal table that maps each bit to a particular tag value
 allowing up to 64 tags on an individual item to be reported (it is
 rare to have more than one or two). To find out if a particular tag
 value is set call QCBORDecode_IsTagged() on the @ref QCBORItem.  See
 also QCBORDecode_GetNextWithTags().

 Third are tags that are not automatically recognized, because they
 are proprietary, custom or more recently registered with [IANA]
 (https://www.iana.org/assignments/cbor-tags/cbor-tags.xhtml). The
 internal mapping table has to be configured to recognize these. Call
 QCBORDecode_SetCallerConfiguredTagList() to do that. Then
 QCBORDecode_IsTagged() will work with them.

 The actual decoding of tags supported in the second and third way
 must be handled by the caller. Often this is simply verifying that
 the expected tag is present on a map, byte string or such.  In other
 cases, there might a complicated map structure to decode.

 See @ref Tags-Overview for a description of how to go about creating
 custom tags.

 This tag decoding design is to be open-ended and flexible to be able
 to handle newly defined tags, while using very little memory, in
 particular keeping @ref QCBORItem as small as possible.

 If any error occurs, \c uDataType and \c uLabelType will be set
 to \ref QCBOR_TYPE_NONE. If there is no need to know the specific
 error, \ref QCBOR_TYPE_NONE can be checked for and the return value
 ignored.

 Errors fall in several categories as noted in list above:

 - Not well-formed errors are those where there is something
 syntactically and fundamentally wrong with the CBOR being
 decoded. Encoding should stop completely.

 - Invalid CBOR is well-formed, but still not correct. It is probably
 best to stop decoding, but not necessary.

 - This implementation has some size limits. They should rarely be
 encountered. If they are it may because something is wrong with the
 CBOR, for example an array size is incorrect.

 - Resource exhaustion. This only occurs when a string allocator is
 configured to handle indefinite-length strings as other than that,
 this implementation does no dynamic memory allocation.

 - There are a few CBOR constructs that are not handled without some
 extra configuration. These are indefinite length strings and maps
 with labels that are not strings or integers. See QCBORDecode_Init().

 */
QCBORError QCBORDecode_GetNext(QCBORDecodeContext *pCtx, QCBORItem *pDecodedItem);


/**
 @brief Gets the next item including full list of tags for item.

 @param[in]  pCtx          The decoder context.
 @param[out] pDecodedItem  Holds the CBOR item just decoded.
 @param[in,out] pTagList   On input array to put tags in; on output
                           the tags on this item. See
                           @ref QCBORTagListOut.

 @return See return values for QCBORDecode_GetNext().

 @retval QCBOR_ERR_TOO_MANY_TAGS  The size of @c pTagList is too small.

 This works the same as QCBORDecode_GetNext() except that it also
 returns the full list of tags for the data item. This function should
 only be needed when parsing CBOR to print it out or convert it to
 some other format. It should not be needed to implement a CBOR-based
 protocol.  See QCBORDecode_GetNext() for the main description of tag
 decoding.

 Tags will be returned here whether or not they are in the built-in or
 caller-configured tag lists.

 CBOR has no upper bound of limit on the number of tags that can be
 associated with a data item though in practice the number of tags on
 an item will usually be small, perhaps less than five. This will
 return @ref QCBOR_ERR_TOO_MANY_TAGS if the array in @c pTagList is
 too small to hold all the tags for the item.

 (This function is separate from QCBORDecode_GetNext() so as to not
 have to make @ref QCBORItem large enough to be able to hold a full
 list of tags. Even a list of five tags would nearly double its size
 because tags can be a @c uint64_t ).
 */
QCBORError QCBORDecode_GetNextWithTags(QCBORDecodeContext *pCtx, QCBORItem *pDecodedItem, QCBORTagListOut *pTagList);


/**
 @brief Determine if a CBOR item was tagged with a particular tag

 @param[in] pCtx    The decoder context.
 @param[in] pItem   The CBOR item to check.
 @param[in] uTag    The tag to check, one of @c CBOR_TAG_XXX.

 @return 1 if it was tagged, 0 if not

 See QCBORDecode_GetNext() for the main description of tag
 handling. For tags that are not fully decoded a bit corresponding to
 the tag is set in in @c uTagBits in the @ref QCBORItem. The
 particular bit depends on an internal mapping table. This function
 checks for set bits against the mapping table.

 Typically, a protocol implementation just wants to know if a
 particular tag is present. That is what this provides. To get the
 full list of tags on a data item, see QCBORDecode_GetNextWithTags().

 Also see QCBORDecode_SetCallerConfiguredTagList() for the means to
 add new tags to the internal list so they can be checked for with
 this function.
 */
int QCBORDecode_IsTagged(QCBORDecodeContext *pCtx, const QCBORItem *pItem, uint64_t uTag);


/**
 Check whether all the bytes have been decoded and maps and arrays closed.

 @param[in]  pCtx  The context to check.

 @return An error or @ref QCBOR_SUCCESS.

 This tells you if all the bytes given to QCBORDecode_Init() have been
 consumed and whether all maps and arrays were closed.  The decode is
 considered to be incorrect or incomplete if not and an error will be
 returned.
 */
QCBORError QCBORDecode_Finish(QCBORDecodeContext *pCtx);




/**
 @brief Convert int64_t to smaller integers safely.

 @param [in]  src   An @c int64_t.
 @param [out] dest  A smaller sized integer to convert to.

 @return 0 on success -1 if not

 When decoding an integer, the CBOR decoder will return the value as
 an int64_t unless the integer is in the range of @c INT64_MAX and @c
 UINT64_MAX. That is, unless the value is so large that it can only be
 represented as a @c uint64_t, it will be an @c int64_t.

 CBOR itself doesn't size the individual integers it carries at
 all. The only limits it puts on the major integer types is that they
 are 8 bytes or less in length. Then encoders like this one use the
 smallest number of 1, 2, 4 or 8 bytes to represent the integer based
 on its value. There is thus no notion that one data item in CBOR is
 a 1-byte integer and another is a 4-byte integer.

 The interface to this CBOR encoder only uses 64-bit integers. Some
 CBOR protocols or implementations of CBOR protocols may not want to
 work with something smaller than a 64-bit integer.  Perhaps an array
 of 1000 integers needs to be sent and none has a value larger than
 50,000 and are represented as @c uint16_t.

 The sending / encoding side is easy. Integers are temporarily widened
 to 64-bits as a parameter passing through QCBOREncode_AddInt64() and
 encoded in the smallest way possible for their value, possibly in
 less than an @c uint16_t.

 On the decoding side the integers will be returned at @c int64_t even if
 they are small and were represented by only 1 or 2 bytes in the
 encoded CBOR. The functions here will convert integers to a small
 representation with an overflow check.

 (The decoder could have support 8 different integer types and
 represented the integer with the smallest type automatically, but
 this would have made the decoder more complex and code calling the
 decoder more complex in most use cases.  In most use cases on 64-bit
 machines it is no burden to carry around even small integers as
 64-bit values).
 */
static inline int QCBOR_Int64ToInt32(int64_t src, int32_t *dest)
{
   if(src > INT32_MAX || src < INT32_MIN) {
      return -1;
   } else {
      *dest = (int32_t) src;
   }
   return 0;
}

static inline int QCBOR_Int64ToInt16(int64_t src, int16_t *dest)
{
   if(src > INT16_MAX || src < INT16_MIN) {
      return -1;
   } else {
      *dest = (int16_t) src;
   }
   return 0;
}

static inline int QCBOR_Int64ToInt8(int64_t src, int8_t *dest)
{
   if(src > INT8_MAX || src < INT8_MIN) {
      return -1;
   } else {
      *dest = (int8_t) src;
   }
   return 0;
}

static inline int QCBOR_Int64ToUInt32(int64_t src, uint32_t *dest)
{
   if(src > UINT32_MAX || src < 0) {
      return -1;
   } else {
      *dest = (uint32_t) src;
   }
   return 0;
}

static inline int QCBOR_Int64UToInt16(int64_t src, uint16_t *dest)
{
   if(src > UINT16_MAX || src < 0) {
      return -1;
   } else {
      *dest = (uint16_t) src;
   }
   return 0;
}

static inline int QCBOR_Int64ToUInt8(int64_t src, uint8_t *dest)
{
   if(src > UINT8_MAX || src < 0) {
      return -1;
   } else {
      *dest = (uint8_t) src;
   }
   return 0;
}

static inline int QCBOR_Int64ToUInt64(int64_t src, uint64_t *dest)
{
   if(src > 0) {
      return -1;
   } else {
      *dest = (uint64_t) src;
   }
   return 0;
}





/* ===========================================================================
 BEGINNING OF PRIVATE INLINE IMPLEMENTATION

 =========================================================================== */

/**
 @brief Semi-private method to add a buffer full of bytes to encoded output

 @param[in] pCtx       The encoding context to add the integer to.
 @param[in] uMajorType The CBOR major type of the bytes.
 @param[in] Bytes      The bytes to add.

 Use QCBOREncode_AddText() or QCBOREncode_AddBytes() or
 QCBOREncode_AddEncoded() instead. They are inline functions that call
 this and supply the correct major type. This function is public to
 make the inline functions work to keep the overall code size down and
 because the C language has no way to make it private.

 If this is called the major type should be @c
 CBOR_MAJOR_TYPE_TEXT_STRING, @c CBOR_MAJOR_TYPE_BYTE_STRING or @c
 CBOR_MAJOR_NONE_TYPE_RAW. The last one is special for adding
 already-encoded CBOR.
 */
void QCBOREncode_AddBuffer(QCBOREncodeContext *pCtx, uint8_t uMajorType, UsefulBufC Bytes);


/**
 @brief Semi-private method to open a map, array or bstr-wrapped CBOR

 @param[in] pCtx        The context to add to.
 @param[in] uMajorType  The major CBOR type to close

 Call QCBOREncode_OpenArray(), QCBOREncode_OpenMap() or
 QCBOREncode_BstrWrap() instead of this.
 */
void QCBOREncode_OpenMapOrArray(QCBOREncodeContext *pCtx, uint8_t uMajorType);


/**
 @brief Semi-private method to open a map, array with indefinite length

 @param[in] pCtx        The context to add to.
 @param[in] uMajorType  The major CBOR type to close

 Call QCBOREncode_OpenArrayIndefiniteLength() or
 QCBOREncode_OpenMapIndefiniteLength() instead of this.
 */
void QCBOREncode_OpenMapOrArrayIndefiniteLength(QCBOREncodeContext *pCtx, uint8_t uMajorType);


/**
 @brief Semi-private method to close a map, array or bstr wrapped CBOR

 @param[in] pCtx           The context to add to.
 @param[in] uMajorType     The major CBOR type to close.
 @param[out] pWrappedCBOR  Pointer to @ref UsefulBufC containing wrapped bytes.

 Call QCBOREncode_CloseArray(), QCBOREncode_CloseMap() or
 QCBOREncode_CloseBstrWrap() instead of this.
 */
void QCBOREncode_CloseMapOrArray(QCBOREncodeContext *pCtx,
                                 uint8_t uMajorType,
                                 UsefulBufC *pWrappedCBOR);

/**
 @brief Semi-private method to close a map, array with indefinite length

 @param[in] pCtx           The context to add to.
 @param[in] uMajorType     The major CBOR type to close.
 @param[out] pWrappedCBOR  Pointer to @ref UsefulBufC containing wrapped bytes.

 Call QCBOREncode_CloseArrayIndefiniteLength() or
 QCBOREncode_CloseMapIndefiniteLength() instead of this.
 */
void QCBOREncode_CloseMapOrArrayIndefiniteLength(QCBOREncodeContext *pCtx,
                                                 uint8_t uMajorType,
                                                 UsefulBufC *pWrappedCBOR);

/**
 @brief  Semi-private method to add simple types.

 @param[in] pCtx   The encoding context to add the simple value to.
 @param[in] uSize  Minimum encoding size for uNum. Usually 0.
 @param[in] uNum   One of CBOR_SIMPLEV_FALSE through _UNDEF or other.

 This is used to add simple types like true and false.

 Call QCBOREncode_AddBool(), QCBOREncode_AddNULL(),
 QCBOREncode_AddUndef() instead of this.

 This function can add simple values that are not defined by CBOR
 yet. This expansion point in CBOR should not be used unless they are
 standardized.

 Error handling is the same as QCBOREncode_AddInt64().
 */
void  QCBOREncode_AddType7(QCBOREncodeContext *pCtx, size_t uSize, uint64_t uNum);


/**
 @brief  Semi-private method to add bigfloats and decimal fractions.

 @param[in] pCtx             The encoding context to add the value to.
 @param[in] uTag             The type 6 tag indicating what this is to be
 @param[in] BigNumMantissa   Is @ref NULLUsefulBufC if mantissa is an
                             @c int64_t or the actual big number mantissa
                             if not.
 @param[in] nMantissa        The @c int64_t mantissa if it is not a big number.
 @param[in] nExponent        The exponent.

 This adds a tagged array with two members, the mantissa and exponent. The
 mantissa can be either a big number or an @c int64_t.

 Typically, QCBOREncode_AddDecimalFraction(), QCBOREncode_AddBigFloat(),
 QCBOREncode_AddDecimalFractionBigNum() or QCBOREncode_AddBigFloatBigNum()
 is called instead of this.
 */
void QCBOREncode_AddExponentAndMantissa(QCBOREncodeContext *pCtx,
                                        uint64_t            uTag,
                                        UsefulBufC          BigNumMantissa,
                                        bool                bBigNumIsNegative,
                                        int64_t             nMantissa,
                                        int64_t             nExponent);

/**
 @brief Semi-private method to add only the type and length of a byte string.

 @param[in] pCtx    The context to initialize.
 @param[in] Bytes   Pointer and length of the input data.

 This is the same as QCBOREncode_AddBytes() except it only adds the
 CBOR encoding for the type and the length. It doesn't actually add
 the bytes. You can't actually produce correct CBOR with this and the
 rest of this API. It is only used for a special case where
 the valid CBOR is created manually by putting this type and length in
 and then adding the actual bytes. In particular, when only a hash of
 the encoded CBOR is needed, where the type and header are hashed
 separately and then the bytes is hashed. This makes it possible to
 implement COSE Sign1 with only one copy of the payload in the output
 buffer, rather than two, roughly cutting memory use in half.

 This is only used for this odd case, but this is a supported
 tested function.
*/
static inline void QCBOREncode_AddBytesLenOnly(QCBOREncodeContext *pCtx, UsefulBufC Bytes);

static inline void QCBOREncode_AddBytesLenOnlyToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes);

static inline void QCBOREncode_AddBytesLenOnlyToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes);





static inline void QCBOREncode_AddInt64ToMap(QCBOREncodeContext *pCtx, const char *szLabel, int64_t uNum)
{
   // Use _AddBuffer() because _AddSZString() is defined below, not above
   QCBOREncode_AddBuffer(pCtx, CBOR_MAJOR_TYPE_TEXT_STRING, UsefulBuf_FromSZ(szLabel));
   QCBOREncode_AddInt64(pCtx, uNum);
}

static inline void QCBOREncode_AddInt64ToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, int64_t uNum)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddInt64(pCtx, uNum);
}


static inline void QCBOREncode_AddUInt64ToMap(QCBOREncodeContext *pCtx, const char *szLabel, uint64_t uNum)
{
   // Use _AddBuffer() because _AddSZString() is defined below, not above
   QCBOREncode_AddBuffer(pCtx, CBOR_MAJOR_TYPE_TEXT_STRING, UsefulBuf_FromSZ(szLabel));
   QCBOREncode_AddUInt64(pCtx, uNum);
}

static inline void QCBOREncode_AddUInt64ToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, uint64_t uNum)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddUInt64(pCtx, uNum);
}


static inline void QCBOREncode_AddText(QCBOREncodeContext *pCtx, UsefulBufC Text)
{
   QCBOREncode_AddBuffer(pCtx, CBOR_MAJOR_TYPE_TEXT_STRING, Text);
}

static inline void QCBOREncode_AddTextToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Text)
{
   // Use _AddBuffer() because _AddSZString() is defined below, not above
   QCBOREncode_AddText(pCtx, UsefulBuf_FromSZ(szLabel));
   QCBOREncode_AddText(pCtx, Text);
}

static inline void QCBOREncode_AddTextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Text)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddText(pCtx, Text);
}


inline static void QCBOREncode_AddSZString(QCBOREncodeContext *pCtx, const char *szString)
{
   QCBOREncode_AddText(pCtx, UsefulBuf_FromSZ(szString));
}

static inline void QCBOREncode_AddSZStringToMap(QCBOREncodeContext *pCtx, const char *szLabel, const char *szString)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddSZString(pCtx, szString);
}

static inline void QCBOREncode_AddSZStringToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, const char *szString)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddSZString(pCtx, szString);
}


static inline void QCBOREncode_AddDoubleToMap(QCBOREncodeContext *pCtx, const char *szLabel, double dNum)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddDouble(pCtx, dNum);
}

static inline void QCBOREncode_AddDoubleToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, double dNum)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddDouble(pCtx, dNum);
}


static inline void QCBOREncode_AddDateEpoch(QCBOREncodeContext *pCtx, int64_t date)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_DATE_EPOCH);
   QCBOREncode_AddInt64(pCtx, date);
}

static inline void QCBOREncode_AddDateEpochToMap(QCBOREncodeContext *pCtx, const char *szLabel, int64_t date)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_DATE_EPOCH);
   QCBOREncode_AddInt64(pCtx, date);
}

static inline void QCBOREncode_AddDateEpochToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, int64_t date)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_DATE_EPOCH);
   QCBOREncode_AddInt64(pCtx, date);
}


static inline void QCBOREncode_AddBytes(QCBOREncodeContext *pCtx, UsefulBufC Bytes)
{
   QCBOREncode_AddBuffer(pCtx, CBOR_MAJOR_TYPE_BYTE_STRING, Bytes);
}

static inline void QCBOREncode_AddBytesToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddBytesToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddBytesLenOnly(QCBOREncodeContext *pCtx, UsefulBufC Bytes)
{
    QCBOREncode_AddBuffer(pCtx, CBOR_MAJOR_NONE_TYPE_BSTR_LEN_ONLY, Bytes);
}

static inline void QCBOREncode_AddBytesLenOnlyToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes)
{
    QCBOREncode_AddSZString(pCtx, szLabel);
    QCBOREncode_AddBytesLenOnly(pCtx, Bytes);
}

static inline void QCBOREncode_AddBytesLenOnlyToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes)
{
    QCBOREncode_AddInt64(pCtx, nLabel);
    QCBOREncode_AddBytesLenOnly(pCtx, Bytes);
}

static inline void QCBOREncode_AddBinaryUUID(QCBOREncodeContext *pCtx, UsefulBufC Bytes)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_BIN_UUID);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddBinaryUUIDToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_BIN_UUID);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddBinaryUUIDToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_BIN_UUID);
   QCBOREncode_AddBytes(pCtx, Bytes);
}


static inline void QCBOREncode_AddPositiveBignum(QCBOREncodeContext *pCtx, UsefulBufC Bytes)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_POS_BIGNUM);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddPositiveBignumToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_POS_BIGNUM);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddPositiveBignumToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_POS_BIGNUM);
   QCBOREncode_AddBytes(pCtx, Bytes);
}


static inline void QCBOREncode_AddNegativeBignum(QCBOREncodeContext *pCtx, UsefulBufC Bytes)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_NEG_BIGNUM);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddNegativeBignumToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_NEG_BIGNUM);
   QCBOREncode_AddBytes(pCtx, Bytes);
}

static inline void QCBOREncode_AddNegativeBignumToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_NEG_BIGNUM);
   QCBOREncode_AddBytes(pCtx, Bytes);
}


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA

static inline void QCBOREncode_AddDecimalFraction(QCBOREncodeContext *pCtx,
                                                  int64_t             nMantissa,
                                                  int64_t             nBase10Exponent)
{
   QCBOREncode_AddExponentAndMantissa(pCtx,
                                      CBOR_TAG_DECIMAL_FRACTION,
                                      NULLUsefulBufC,
                                      false,
                                      nMantissa,
                                      nBase10Exponent);
}

static inline void QCBOREncode_AddDecimalFractionToMap(QCBOREncodeContext *pCtx,
                                                       const char         *szLabel,
                                                       int64_t             nMantissa,
                                                       int64_t             nBase10Exponent)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddDecimalFraction(pCtx, nMantissa, nBase10Exponent);
}

static inline void QCBOREncode_AddDecimalFractionToMapN(QCBOREncodeContext *pCtx,
                                                        int64_t             nLabel,
                                                        int64_t             nMantissa,
                                                        int64_t             nBase10Exponent)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddDecimalFraction(pCtx, nMantissa, nBase10Exponent);
}

static inline void QCBOREncode_AddDecimalFractionBigNum(QCBOREncodeContext *pCtx,
                                                        UsefulBufC          Mantissa,
                                                        bool                bIsNegative,
                                                        int64_t             nBase10Exponent)
{
   QCBOREncode_AddExponentAndMantissa(pCtx,
                                      CBOR_TAG_DECIMAL_FRACTION,
                                      Mantissa, bIsNegative,
                                      0,
                                      nBase10Exponent);
}

static inline void QCBOREncode_AddDecimalFractionBigNumToMap(QCBOREncodeContext *pCtx,
                                                             const char         *szLabel,
                                                             UsefulBufC          Mantissa,
                                                             bool                bIsNegative,
                                                             int64_t             nBase10Exponent)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddDecimalFractionBigNum(pCtx, Mantissa, bIsNegative, nBase10Exponent);
}

static inline void QCBOREncode_AddDecimalFractionBigNumToMapN(QCBOREncodeContext *pCtx,
                                                              int64_t             nLabel,
                                                              UsefulBufC          Mantissa,
                                                              bool                bIsNegative,
                                                              int64_t             nBase2Exponent)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddDecimalFractionBigNum(pCtx, Mantissa, bIsNegative, nBase2Exponent);
}

static inline void QCBOREncode_AddBigFloat(QCBOREncodeContext *pCtx,
                                           int64_t             nMantissa,
                                           int64_t             nBase2Exponent)
{
   QCBOREncode_AddExponentAndMantissa(pCtx, CBOR_TAG_BIGFLOAT, NULLUsefulBufC, false, nMantissa, nBase2Exponent);
}

static inline void QCBOREncode_AddBigFloatToMap(QCBOREncodeContext *pCtx,
                                                const char         *szLabel,
                                                int64_t             nMantissa,
                                                int64_t             nBase2Exponent)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddBigFloat(pCtx, nMantissa, nBase2Exponent);
}

static inline void QCBOREncode_AddBigFloatToMapN(QCBOREncodeContext *pCtx,
                                                 int64_t             nLabel,
                                                 int64_t             nMantissa,
                                                 int64_t             nBase2Exponent)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddBigFloat(pCtx, nMantissa, nBase2Exponent);
}

static inline void QCBOREncode_AddBigFloatBigNum(QCBOREncodeContext *pCtx,
                                                 UsefulBufC          Mantissa,
                                                 bool                bIsNegative,
                                                 int64_t             nBase2Exponent)
{
   QCBOREncode_AddExponentAndMantissa(pCtx, CBOR_TAG_BIGFLOAT, Mantissa, bIsNegative, 0, nBase2Exponent);
}

static inline void QCBOREncode_AddBigFloatBigNumToMap(QCBOREncodeContext *pCtx,
                                                      const char         *szLabel,
                                                      UsefulBufC          Mantissa,
                                                      bool                bIsNegative,
                                                      int64_t             nBase2Exponent)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddBigFloatBigNum(pCtx, Mantissa, bIsNegative, nBase2Exponent);
}

static inline void QCBOREncode_AddBigFloatBigNumToMapN(QCBOREncodeContext *pCtx,
                                                       int64_t             nLabel,
                                                       UsefulBufC          Mantissa,
                                                       bool                bIsNegative,
                                                       int64_t             nBase2Exponent)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddBigFloatBigNum(pCtx, Mantissa, bIsNegative, nBase2Exponent);
}
#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */


static inline void QCBOREncode_AddURI(QCBOREncodeContext *pCtx, UsefulBufC URI)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_URI);
   QCBOREncode_AddText(pCtx, URI);
}

static inline void QCBOREncode_AddURIToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC URI)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_URI);
   QCBOREncode_AddText(pCtx, URI);
}

static inline void QCBOREncode_AddURIToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC URI)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_URI);
   QCBOREncode_AddText(pCtx, URI);
}



static inline void QCBOREncode_AddB64Text(QCBOREncodeContext *pCtx, UsefulBufC B64Text)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_B64);
   QCBOREncode_AddText(pCtx, B64Text);
}

static inline void QCBOREncode_AddB64TextToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC B64Text)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_B64);
   QCBOREncode_AddText(pCtx, B64Text);
}

static inline void QCBOREncode_AddB64TextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC B64Text)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_B64);
   QCBOREncode_AddText(pCtx, B64Text);
}


static inline void QCBOREncode_AddB64URLText(QCBOREncodeContext *pCtx, UsefulBufC B64Text)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_B64URL);
   QCBOREncode_AddText(pCtx, B64Text);
}

static inline void QCBOREncode_AddB64URLTextToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC B64Text)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_B64URL);
   QCBOREncode_AddText(pCtx, B64Text);
}

static inline void QCBOREncode_AddB64URLTextToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC B64Text)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_B64URL);
   QCBOREncode_AddText(pCtx, B64Text);
}


static inline void QCBOREncode_AddRegex(QCBOREncodeContext *pCtx, UsefulBufC Bytes)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_REGEX);
   QCBOREncode_AddText(pCtx, Bytes);
}

static inline void QCBOREncode_AddRegexToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_REGEX);
   QCBOREncode_AddText(pCtx, Bytes);
}

static inline void QCBOREncode_AddRegexToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Bytes)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_REGEX);
   QCBOREncode_AddText(pCtx, Bytes);
}


static inline void QCBOREncode_AddMIMEData(QCBOREncodeContext *pCtx, UsefulBufC MIMEData)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_MIME);
   QCBOREncode_AddText(pCtx, MIMEData);
}

static inline void QCBOREncode_AddMIMEDataToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC MIMEData)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_MIME);
   QCBOREncode_AddText(pCtx, MIMEData);
}

static inline void QCBOREncode_AddMIMEDataToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC MIMEData)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_MIME);
   QCBOREncode_AddText(pCtx, MIMEData);
}


static inline void QCBOREncode_AddDateString(QCBOREncodeContext *pCtx, const char *szDate)
{
   QCBOREncode_AddTag(pCtx, CBOR_TAG_DATE_STRING);
   QCBOREncode_AddSZString(pCtx, szDate);
}

static inline void QCBOREncode_AddDateStringToMap(QCBOREncodeContext *pCtx, const char *szLabel, const char *szDate)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_DATE_STRING);
   QCBOREncode_AddSZString(pCtx, szDate);
}

static inline void QCBOREncode_AddDateStringToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, const char *szDate)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddTag(pCtx, CBOR_TAG_DATE_STRING);
   QCBOREncode_AddSZString(pCtx, szDate);
}


static inline void QCBOREncode_AddSimple(QCBOREncodeContext *pCtx, uint64_t uNum)
{
   QCBOREncode_AddType7(pCtx, 0, uNum);
}

static inline void QCBOREncode_AddSimpleToMap(QCBOREncodeContext *pCtx, const char *szLabel, uint8_t uSimple)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddSimple(pCtx, uSimple);
}

static inline void QCBOREncode_AddSimpleToMapN(QCBOREncodeContext *pCtx, int nLabel, uint8_t uSimple)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddSimple(pCtx, uSimple);
}


static inline void QCBOREncode_AddBool(QCBOREncodeContext *pCtx, bool b)
{
   uint8_t uSimple = CBOR_SIMPLEV_FALSE;
   if(b) {
      uSimple = CBOR_SIMPLEV_TRUE;
   }
   QCBOREncode_AddSimple(pCtx, uSimple);
}

static inline void QCBOREncode_AddBoolToMap(QCBOREncodeContext *pCtx, const char *szLabel, bool b)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddBool(pCtx, b);
}

static inline void QCBOREncode_AddBoolToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, bool b)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddBool(pCtx, b);
}


static inline void QCBOREncode_AddNULL(QCBOREncodeContext *pCtx)
{
   QCBOREncode_AddSimple(pCtx, CBOR_SIMPLEV_NULL);
}

static inline void QCBOREncode_AddNULLToMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddNULL(pCtx);
}

static inline void QCBOREncode_AddNULLToMapN(QCBOREncodeContext *pCtx, int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddNULL(pCtx);
}


static inline void QCBOREncode_AddUndef(QCBOREncodeContext *pCtx)
{
   QCBOREncode_AddSimple(pCtx, CBOR_SIMPLEV_UNDEF);
}

static inline void QCBOREncode_AddUndefToMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddUndef(pCtx);
}

static inline void QCBOREncode_AddUndefToMapN(QCBOREncodeContext *pCtx, int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddUndef(pCtx);
}


static inline void QCBOREncode_OpenArray(QCBOREncodeContext *pCtx)
{
   QCBOREncode_OpenMapOrArray(pCtx, CBOR_MAJOR_TYPE_ARRAY);
}

static inline void QCBOREncode_OpenArrayInMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_OpenArray(pCtx);
}

static inline void QCBOREncode_OpenArrayInMapN(QCBOREncodeContext *pCtx,  int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_OpenArray(pCtx);
}

static inline void QCBOREncode_CloseArray(QCBOREncodeContext *pCtx)
{
   QCBOREncode_CloseMapOrArray(pCtx, CBOR_MAJOR_TYPE_ARRAY, NULL);
}


static inline void QCBOREncode_OpenMap(QCBOREncodeContext *pCtx)
{
   QCBOREncode_OpenMapOrArray(pCtx, CBOR_MAJOR_TYPE_MAP);
}

static inline void QCBOREncode_OpenMapInMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_OpenMap(pCtx);
}

static inline void QCBOREncode_OpenMapInMapN(QCBOREncodeContext *pCtx, int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_OpenMap(pCtx);
}

static inline void QCBOREncode_CloseMap(QCBOREncodeContext *pCtx)
{
   QCBOREncode_CloseMapOrArray(pCtx, CBOR_MAJOR_TYPE_MAP, NULL);
}

static inline void QCBOREncode_OpenArrayIndefiniteLength(QCBOREncodeContext *pCtx)
{
   QCBOREncode_OpenMapOrArrayIndefiniteLength(pCtx, CBOR_MAJOR_NONE_TYPE_ARRAY_INDEFINITE_LEN);
}

static inline void QCBOREncode_OpenArrayIndefiniteLengthInMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_OpenArrayIndefiniteLength(pCtx);
}

static inline void QCBOREncode_OpenArrayIndefiniteLengthInMapN(QCBOREncodeContext *pCtx,  int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_OpenArrayIndefiniteLength(pCtx);
}

static inline void QCBOREncode_CloseArrayIndefiniteLength(QCBOREncodeContext *pCtx)
{
   QCBOREncode_CloseMapOrArrayIndefiniteLength(pCtx, CBOR_MAJOR_NONE_TYPE_ARRAY_INDEFINITE_LEN, NULL);
}


static inline void QCBOREncode_OpenMapIndefiniteLength(QCBOREncodeContext *pCtx)
{
   QCBOREncode_OpenMapOrArrayIndefiniteLength(pCtx, CBOR_MAJOR_NONE_TYPE_MAP_INDEFINITE_LEN);
}

static inline void QCBOREncode_OpenMapIndefiniteLengthInMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_OpenMapIndefiniteLength(pCtx);
}

static inline void QCBOREncode_OpenMapIndefiniteLengthInMapN(QCBOREncodeContext *pCtx, int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_OpenMapIndefiniteLength(pCtx);
}

static inline void QCBOREncode_CloseMapIndefiniteLength(QCBOREncodeContext *pCtx)
{
   QCBOREncode_CloseMapOrArrayIndefiniteLength(pCtx, CBOR_MAJOR_NONE_TYPE_MAP_INDEFINITE_LEN, NULL);
}

static inline void QCBOREncode_BstrWrap(QCBOREncodeContext *pCtx)
{
   QCBOREncode_OpenMapOrArray(pCtx, CBOR_MAJOR_TYPE_BYTE_STRING);
}

static inline void QCBOREncode_BstrWrapInMap(QCBOREncodeContext *pCtx, const char *szLabel)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_BstrWrap(pCtx);
}

static inline void QCBOREncode_BstrWrapInMapN(QCBOREncodeContext *pCtx, int64_t nLabel)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_BstrWrap(pCtx);
}

static inline void QCBOREncode_CloseBstrWrap(QCBOREncodeContext *pCtx, UsefulBufC *pWrappedCBOR)
{
   QCBOREncode_CloseMapOrArray(pCtx, CBOR_MAJOR_TYPE_BYTE_STRING, pWrappedCBOR);
}


static inline void QCBOREncode_AddEncoded(QCBOREncodeContext *pCtx, UsefulBufC Encoded)
{
   QCBOREncode_AddBuffer(pCtx, CBOR_MAJOR_NONE_TYPE_RAW, Encoded);
}

static inline void QCBOREncode_AddEncodedToMap(QCBOREncodeContext *pCtx, const char *szLabel, UsefulBufC Encoded)
{
   QCBOREncode_AddSZString(pCtx, szLabel);
   QCBOREncode_AddEncoded(pCtx, Encoded);
}

static inline void QCBOREncode_AddEncodedToMapN(QCBOREncodeContext *pCtx, int64_t nLabel, UsefulBufC Encoded)
{
   QCBOREncode_AddInt64(pCtx, nLabel);
   QCBOREncode_AddEncoded(pCtx, Encoded);
}


static inline int QCBOREncode_IsBufferNULL(QCBOREncodeContext *pCtx)
{
   return UsefulOutBuf_IsBufferNULL(&(pCtx->OutBuf));
}

static inline QCBORError QCBOREncode_GetErrorState(QCBOREncodeContext *pCtx)
{
   if(UsefulOutBuf_GetError(&(pCtx->OutBuf))) {
      // Items didn't fit in the buffer.
      // This check catches this condition for all the appends and inserts
      // so checks aren't needed when the appends and inserts are performed.
      // And of course UsefulBuf will never overrun the input buffer given
      // to it. No complex analysis of the error handling in this file is
      // needed to know that is true. Just read the UsefulBuf code.
      pCtx->uError = QCBOR_ERR_BUFFER_TOO_SMALL;
      // QCBOR_ERR_BUFFER_TOO_SMALL masks other errors, but that is
      // OK. Once the caller fixes this, they'll be unmasked.
   }

   return (QCBORError)pCtx->uError;
}


/* ===========================================================================
 END OF PRIVATE INLINE IMPLEMENTATION

 =========================================================================== */

#ifdef __cplusplus
}
#endif

#endif /* defined(__QCBOR__qcbor__) */
