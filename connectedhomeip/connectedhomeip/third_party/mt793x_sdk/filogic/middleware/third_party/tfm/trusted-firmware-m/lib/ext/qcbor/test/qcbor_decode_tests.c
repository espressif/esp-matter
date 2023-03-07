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

#include "qcbor_decode_tests.h"
#include "qcbor.h"
#include <string.h>
#include <math.h> // for fabs()
#include "not_well_formed_cbor.h"


#ifdef  PRINT_FUNCTIONS_FOR_DEBUGGING
#include <stdio.h>

static void PrintUsefulBufC(const char *szLabel, UsefulBufC Buf)
{
   if(szLabel) {
      printf("%s ", szLabel);
   }

   size_t i;
   for(i = 0; i < Buf.len; i++) {
      uint8_t Z = ((uint8_t *)Buf.ptr)[i];
      printf("%02x ", Z);
   }
   printf("\n");

   fflush(stdout);
}
#endif


static const uint8_t spExpectedEncodedInts[] = {
   0x98, 0x2f, 0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x3b, 0x00, 0x00, 0x00, 0x01,
   0x00, 0x00, 0x00, 0x00, 0x3a, 0xff, 0xff, 0xff,
   0xff, 0x3a, 0xff, 0xff, 0xff, 0xfe, 0x3a, 0xff,
   0xff, 0xff, 0xfd, 0x3a, 0x7f, 0xff, 0xff, 0xff,
   0x3a, 0x7f, 0xff, 0xff, 0xfe, 0x3a, 0x00, 0x01,
   0x00, 0x01, 0x3a, 0x00, 0x01, 0x00, 0x00, 0x39,
   0xff, 0xff, 0x39, 0xff, 0xfe, 0x39, 0xff, 0xfd,
   0x39, 0x01, 0x00, 0x38, 0xff, 0x38, 0xfe, 0x38,
   0xfd, 0x38, 0x18, 0x37, 0x36, 0x20, 0x00, 0x00,
   0x01, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, 0x18,
   0x1a, 0x18, 0xfe, 0x18, 0xff, 0x19, 0x01, 0x00,
   0x19, 0x01, 0x01, 0x19, 0xff, 0xfe, 0x19, 0xff,
   0xff, 0x1a, 0x00, 0x01, 0x00, 0x00, 0x1a, 0x00,
   0x01, 0x00, 0x01, 0x1a, 0x00, 0x01, 0x00, 0x02,
   0x1a, 0x7f, 0xff, 0xff, 0xff, 0x1a, 0x7f, 0xff,
   0xff, 0xff, 0x1a, 0x80, 0x00, 0x00, 0x00, 0x1a,
   0x80, 0x00, 0x00, 0x01, 0x1a, 0xff, 0xff, 0xff,
   0xfe, 0x1a, 0xff, 0xff, 0xff, 0xff, 0x1b, 0x00,
   0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x1b,
   0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
   0x1b, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff};


// return CBOR error or -1 if type of value doesn't match

static int32_t IntegerValuesParseTestInternal(QCBORDecodeContext *pDCtx)
{
   QCBORItem          Item;
   int nCBORError;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_ARRAY)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -9223372036854775807LL - 1)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -4294967297)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -4294967296)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -4294967295)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -4294967294)
      return -1;


   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -2147483648)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -2147483647)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -65538)
      return  -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -65537)
      return  -1;

   if((nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -65536)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -65535)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -65534)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -257)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -256)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -255)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -254)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -25)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -24)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -23)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -1)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 0)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 0)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 1)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 22)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 23)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 24)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 25)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 26)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 254)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 255)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 256)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 257)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 65534)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 65535)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 65536)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 65537)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 65538)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 2147483647)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 2147483647)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 2147483648)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 2147483649)
      return  -1;

   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 4294967294)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 4294967295)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 4294967296)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 4294967297)
      return  -1;



   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 9223372036854775807LL)
      return  -1;


   if((   nCBORError = QCBORDecode_GetNext(pDCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_UINT64 ||
      Item.val.uint64 != 18446744073709551615ULL)
      return  -1;


   if(QCBORDecode_Finish(pDCtx) != QCBOR_SUCCESS) {
      return -1;
   }

   return 0;
}


// One less than the smallest negative integer allowed in C. Decoding
// this should fail.
static const uint8_t spTooSmallNegative[] = {
   0x3b, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


/*
   Tests the decoding of lots of different integers sizes
   and values.
 */
int32_t IntegerValuesParseTest()
{
   int nReturn;
   QCBORDecodeContext DCtx;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spExpectedEncodedInts),
                    QCBOR_DECODE_MODE_NORMAL);

   // The really big test of all successes
   nReturn = IntegerValuesParseTestInternal(&DCtx);
   if(nReturn) {
      return nReturn;
   }

   // The one large negative integer that can be parsed
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spTooSmallNegative),
                    QCBOR_DECODE_MODE_NORMAL);

   QCBORItem item;
   if(QCBORDecode_GetNext(&DCtx, &item) != QCBOR_ERR_INT_OVERFLOW) {
      nReturn = -4000;
   }

   return(nReturn);
}


/*
   Creates a simple CBOR array and returns it in *pEncoded. The array is
   malloced and needs to be freed. This is used by several tests.

   Two of the inputs can be set. Two other items in the array are fixed.

 */

static uint8_t spSimpleArrayBuffer[50];

static int32_t CreateSimpleArray(int nInt1, int nInt2, uint8_t **pEncoded, size_t *pEncodedLen)
{
   QCBOREncodeContext ECtx;
   int nReturn = -1;

   *pEncoded = NULL;
   *pEncodedLen = INT32_MAX;

   // loop runs CBOR encoding twice. First with no buffer to
   // calculate the length so buffer can be allocated correctly,
   // and last with the buffer to do the actual encoding
   do {
      QCBOREncode_Init(&ECtx, (UsefulBuf){*pEncoded, *pEncodedLen});
      QCBOREncode_OpenArray(&ECtx);
      QCBOREncode_AddInt64(&ECtx, nInt1);
      QCBOREncode_AddInt64(&ECtx, nInt2);
      QCBOREncode_AddBytes(&ECtx, ((UsefulBufC) {"galactic", 8}));
      QCBOREncode_AddBytes(&ECtx, ((UsefulBufC) {"haven token", 11}));
      QCBOREncode_CloseArray(&ECtx);

      if(QCBOREncode_FinishGetSize(&ECtx, pEncodedLen))
         goto Done;

      if(*pEncoded != NULL) {
         nReturn = 0;
         goto Done;
      }

      // Use static buffer to avoid dependency on malloc()
      if(*pEncodedLen > sizeof(spSimpleArrayBuffer)) {
         goto Done;
      }
      *pEncoded = spSimpleArrayBuffer;

   } while(1);

Done:
   return nReturn;
}


/*
 Some basic CBOR with map and array used in a lot of tests.
 The map labels are all strings

 {"first integer": 42,
  "an array of two strings": [
      "string1", "string2"
  ],
  "map in a map": {
      "bytes 1": h'78787878',
      "bytes 2": h'79797979',
      "another int": 98,
      "text 2": "lies, damn lies and statistics"
   }
  }
 */
static const uint8_t pValidMapEncoded[] = {
   0xa3, 0x6d, 0x66, 0x69, 0x72, 0x73, 0x74, 0x20, 0x69, 0x6e,
   0x74, 0x65, 0x67, 0x65, 0x72, 0x18, 0x2a, 0x77, 0x61, 0x6e,
   0x20, 0x61, 0x72, 0x72, 0x61, 0x79, 0x20, 0x6f, 0x66, 0x20,
   0x74, 0x77, 0x6f, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67,
   0x73, 0x82, 0x67, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x31,
   0x67, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x32, 0x6c, 0x6d,
   0x61, 0x70, 0x20, 0x69, 0x6e, 0x20, 0x61, 0x20, 0x6d, 0x61,
   0x70, 0xa4, 0x67, 0x62, 0x79, 0x74, 0x65, 0x73, 0x20, 0x31,
   0x44, 0x78, 0x78, 0x78, 0x78, 0x67, 0x62, 0x79, 0x74, 0x65,
   0x73, 0x20, 0x32, 0x44, 0x79, 0x79, 0x79, 0x79, 0x6b, 0x61,
   0x6e, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x20, 0x69, 0x6e, 0x74,
   0x18, 0x62, 0x66, 0x74, 0x65, 0x78, 0x74, 0x20, 0x32, 0x78,
   0x1e, 0x6c, 0x69, 0x65, 0x73, 0x2c, 0x20, 0x64, 0x61, 0x6d,
   0x6e, 0x20, 0x6c, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64,
   0x20, 0x73, 0x74, 0x61, 0x74, 0x69, 0x73, 0x74, 0x69, 0x63,
   0x73 } ;

static int32_t ParseOrderedArray(const uint8_t *pEncoded,
                             size_t nLen,
                             int64_t *pInt1,
                             int64_t *pInt2,
                             const uint8_t **pBuf3,
                             size_t *pBuf3Len,
                             const uint8_t **pBuf4,
                             size_t *pBuf4Len)
{
   QCBORDecodeContext DCtx;
   QCBORItem          Item;
   int                nReturn = -1; // assume error until success

   QCBORDecode_Init(&DCtx,
                    (UsefulBufC){pEncoded, nLen},
                    QCBOR_DECODE_MODE_NORMAL);

   // Make sure the first thing is a map
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 || Item.uDataType != QCBOR_TYPE_ARRAY)
      goto Done;

   // First integer
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 || Item.uDataType != QCBOR_TYPE_INT64)
      goto Done;
   *pInt1 = Item.val.int64;

   // Second integer
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 || Item.uDataType != QCBOR_TYPE_INT64)
      goto Done;
   *pInt2 = Item.val.int64;

   // First string
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 || Item.uDataType != QCBOR_TYPE_BYTE_STRING)
      goto Done;
   *pBuf3 = Item.val.string.ptr;
   *pBuf3Len = Item.val.string.len;

   // Second string
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 || Item.uDataType != QCBOR_TYPE_BYTE_STRING)
      goto Done;
   *pBuf4 = Item.val.string.ptr;
   *pBuf4Len = Item.val.string.len;

   nReturn = 0;

Done:
   return(nReturn);
}




int32_t SimpleArrayTest()
{
   uint8_t *pEncoded;
   size_t  nEncodedLen;

   int64_t i1=0, i2=0;
   size_t i3=0, i4=0;
   const uint8_t *s3= (uint8_t *)"";
   const uint8_t *s4= (uint8_t *)"";


   if(CreateSimpleArray(23, 6000, &pEncoded, &nEncodedLen) < 0) {
      return(-1);
   }

   ParseOrderedArray(pEncoded, nEncodedLen, &i1, &i2, &s3, &i3, &s4, &i4);

   if(i1 != 23 ||
      i2 != 6000 ||
      i3 != 8 ||
      i4 != 11 ||
      memcmp("galactic", s3, 8) !=0 ||
      memcmp("haven token", s4, 11) !=0) {
      return(-1);
   }

   return(0);
}


/*
 [
    0,
    [],
    [
       [],
       [
          0
       ],
       {},
       {
          1: {},
          2: {},
          3: []
       }
    ]
 ]
 */
static uint8_t sEmpties[] = {0x83, 0x00, 0x80, 0x84, 0x80, 0x81, 0x00, 0xa0,
                             0xa3, 0x01, 0xa0, 0x02, 0xa0, 0x03, 0x80};

int32_t EmptyMapsAndArraysTest()
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(sEmpties),
                    QCBOR_DECODE_MODE_NORMAL);

   // Array with 3 items
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.uNestingLevel != 0 ||
      Item.uNextNestLevel != 1 ||
      Item.val.uCount != 3) {
      return -1;
   }

   // An integer 0
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.uNestingLevel != 1 ||
      Item.uNextNestLevel != 1 ||
      Item.val.uint64 != 0) {
      return -2;
   }

   // An empty array
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.uNestingLevel != 1 ||
      Item.uNextNestLevel != 1 ||
      Item.val.uCount != 0) {
      return -3;
   }

   // An array with 4 items
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.uNestingLevel != 1 ||
      Item.uNextNestLevel != 2 ||
      Item.val.uCount != 4) {
      return -4;
   }

   // An empty array
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.uNestingLevel != 2 ||
      Item.uNextNestLevel != 2 ||
      Item.val.uCount != 0) {
      return -5;
   }

   // An array with 1 item
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.uNestingLevel != 2 ||
      Item.uNextNestLevel != 3 ||
      Item.val.uCount != 1) {
      return -6;
   }

   // An integer 0
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.uNestingLevel != 3 ||
      Item.uNextNestLevel != 2 ||
      Item.val.uint64 != 0) {
      return -7;
   }

   // An empty map
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_MAP ||
      Item.uNestingLevel != 2 ||
      Item.uNextNestLevel != 2 ||
      Item.val.uCount != 0) {
      return -8;
   }

   // An map with 3 items
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_MAP ||
      Item.uNestingLevel != 2 ||
      Item.uNextNestLevel != 3 ||
      Item.val.uCount != 3) {
      return -9;
   }

   // An empty map
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_MAP ||
      Item.uNestingLevel != 3 ||
      Item.uNextNestLevel != 3 ||
      Item.val.uCount != 0) {
      return -10;
   }

   // An empty map
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_MAP ||
      Item.uNestingLevel != 3 ||
      Item.uNextNestLevel != 3 ||
      Item.val.uCount != 0) {
      return -11;
   }

   // An empty array
   if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.uNestingLevel != 3 ||
      Item.uNextNestLevel != 0 ||
      Item.val.uCount != 0) {
      return -12;
   }

   if(QCBORDecode_Finish(&DCtx) != QCBOR_SUCCESS) {
      return -13;
   }

   return 0;
}


static uint8_t spDeepArrays[] = {0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                                 0x81, 0x81, 0x81, 0x80};

int32_t ParseDeepArrayTest()
{
   QCBORDecodeContext DCtx;
   int nReturn = 0;
   int i;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spDeepArrays),
                    QCBOR_DECODE_MODE_NORMAL);

   for(i = 0; i < 10; i++) {
      QCBORItem Item;

      if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
         Item.uDataType != QCBOR_TYPE_ARRAY ||
         Item.uNestingLevel != i) {
         nReturn = -1;
         break;
      }
   }

   return(nReturn);
}

// Big enough to test nesting to the depth of 24
static uint8_t spTooDeepArrays[] = {0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                                    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                                    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
                                    0x81, 0x81, 0x81, 0x80};

int32_t ParseTooDeepArrayTest()
{
   QCBORDecodeContext DCtx;
   int nReturn = 0;
   int i;
   QCBORItem Item;


   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spTooDeepArrays),
                    QCBOR_DECODE_MODE_NORMAL);

   for(i = 0; i < QCBOR_MAX_ARRAY_NESTING1; i++) {

      if(QCBORDecode_GetNext(&DCtx, &Item) != 0 ||
         Item.uDataType != QCBOR_TYPE_ARRAY ||
         Item.uNestingLevel != i) {
         nReturn = -1;
         break;
      }
   }

   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_ARRAY_NESTING_TOO_DEEP)
      nReturn = -1;

   return(nReturn);
}




int32_t ShortBufferParseTest()
{
   int nResult = 0;

   for(size_t nNum = sizeof(spExpectedEncodedInts)-1; nNum; nNum--) {
      QCBORDecodeContext DCtx;

      QCBORDecode_Init(&DCtx,
                       (UsefulBufC){spExpectedEncodedInts, nNum},
                       QCBOR_DECODE_MODE_NORMAL);

      const int nErr = IntegerValuesParseTestInternal(&DCtx);

      if(nErr != QCBOR_ERR_HIT_END && nErr != QCBOR_ERR_NO_MORE_ITEMS) {
         nResult = -1;
         goto Done;
      }
   }
Done:
   return nResult;
}



int32_t ShortBufferParseTest2()
{
   uint8_t *pEncoded;
   int      nReturn;
   size_t   nEncodedLen;

   int64_t i1, i2;
   size_t i3, i4;
   const uint8_t *s3, *s4;

   nReturn = 0;

   if(CreateSimpleArray(23, 6000, &pEncoded, &nEncodedLen) < 0) {
      return(-1);
   }

   for(nEncodedLen--; nEncodedLen; nEncodedLen--) {
      int nResult = ParseOrderedArray(pEncoded, (uint32_t)nEncodedLen, &i1, &i2, &s3, &i3, &s4, &i4);
      if(nResult == 0) {
         nReturn = -1;
      }
   }

   return(nReturn);
}

/*
 Decode and thoroughly check a moderately complex
 set of maps. Can be run in QCBOR_DECODE_MODE_NORMAL or in
 QCBOR_DECODE_MODE_MAP_STRINGS_ONLY.
 */
static int32_t ParseMapTest1(QCBORDecodeMode nMode)
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;
   int nCBORError;

   QCBORDecode_Init(&DCtx,
                    (UsefulBufC){pValidMapEncoded, sizeof(pValidMapEncoded)},
                    nMode);

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      Item.val.uCount != 3)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 42 ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("first integer"))) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("an array of two strings")) ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.val.uCount != 2)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("string1"))) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("string2"))) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("map in a map")) ||
      Item.uDataType != QCBOR_TYPE_MAP ||
      Item.val.uCount != 4) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("bytes 1"))||
      Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("xxxx"))) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("bytes 2")) ||
      Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("yyyy"))) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("another int")) ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 98)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("text 2"))||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("lies, damn lies and statistics"))) {
      return -1;
   }

   return 0;
}


/*
 Decode and thoroughly check a moderately complex
 set of maps
 */
int32_t ParseMapAsArrayTest()
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;
   int nCBORError;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pValidMapEncoded),
                    QCBOR_DECODE_MODE_MAP_AS_ARRAY);

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP_AS_ARRAY ||
      Item.val.uCount != 6) {
      return -1;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      Item.uLabelType != QCBOR_TYPE_NONE ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("first integer"))) {
      return -2;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 42 ||
      Item.uDataAlloc ||
      Item.uLabelAlloc) {
      return -3;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("an array of two strings")) ||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING) {
      return -4;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.val.uCount != 2) {
      return -5;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.val.string.len != 7 ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("string1"))) {
      return -6;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("string2"))) {
      return -7;
   }


   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("map in a map"))) {
      return -8;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      Item.uDataType != QCBOR_TYPE_MAP_AS_ARRAY ||
      Item.val.uCount != 8) {
      return -9;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("bytes 1"))||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc) {
      return -10;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("xxxx"))) {
      return -11;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("bytes 2")) ||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc) {
      return -12;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("yyyy"))) {
      return -13;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("another int")) ||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING) {
      return -14;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 98) {
      return -15;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("text 2"))||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc) {
      return -16;
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_NONE ||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataAlloc ||
      Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("lies, damn lies and statistics"))) {
      return -17;
   }

   return 0;
}


/*
 Fully or partially decode pValidMapEncoded. When
 partially decoding check for the right error code.
 How much partial decoding depends on nLevel.

 The partial decodes test error conditions of
 incomplete encoded input.

 This could be combined with the above test
 and made prettier and maybe a little more
 thorough.
 */
static int32_t ExtraBytesTest(int nLevel)
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;
   int nCBORError;

   QCBORDecode_Init(&DCtx,
                    (UsefulBufC){pValidMapEncoded, sizeof(pValidMapEncoded)},
                    QCBOR_DECODE_MODE_NORMAL);

   if(nLevel < 1) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_EXTRA_BYTES) {
         return -1;
      } else {
         return 0;
      }
   }


   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      Item.val.uCount != 3)
      return -1;

   if(nLevel < 2) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }


   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.uCount != 42 ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("first integer"))) {
      return -1;
   }

   if(nLevel < 3) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("an array of two strings")) ||
      Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.val.uCount != 2) {
      return -1;
   }


   if(nLevel < 4) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }


   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("string1"))) {
      return -1;
   }

   if(nLevel < 5) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("string2"))) {
      return -1;
   }

   if(nLevel < 6) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("map in a map")) ||
      Item.uDataType != QCBOR_TYPE_MAP ||
      Item.val.uCount != 4)
      return -1;

   if(nLevel < 7) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("bytes 1")) ||
      Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("xxxx"))) {
      return -1;
   }

   if(nLevel < 8) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("bytes 2")) ||
      Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("yyyy"))) {
      return -1;
   }

   if(nLevel < 9) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("another int")) ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != 98)
      return -1;

   if(nLevel < 10) {
      if(QCBORDecode_Finish(&DCtx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
         return -1;
      } else {
         return 0;
      }
   }

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item))) {
      return nCBORError;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("text 2"))||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.val.string, UsefulBuf_FromSZ("lies, damn lies and statistics"))) {
      return -1;
   }

   if(QCBORDecode_Finish(&DCtx)) {
      return -1;
   }

   return 0;
}



/*
 Public function for initialization. See header qcbor.h
 */
int32_t ParseMapTest()
{
   // Parse a moderatly complex map structure very thoroughly
   int32_t nResult = ParseMapTest1(QCBOR_DECODE_MODE_NORMAL);
   if(nResult) {
      return nResult;
   }

   // Again, but in strings-only mode. It should succeed since the input
   // map has only string labels.
   nResult = ParseMapTest1(QCBOR_DECODE_MODE_MAP_STRINGS_ONLY);
   if(nResult) {
      return nResult;
   }

   // Again, but try to finish the decoding before the end of the
   // input at 10 different place and see that the right error code
   // is returned.
   for(int i = 0; i < 10; i++) {
      nResult = ExtraBytesTest(i);
      if(nResult) {
         break;
      }
   }

   return nResult;
}


static uint8_t spSimpleValues[] = {0x8a, 0xf4, 0xf5, 0xf6, 0xf7, 0xff,
                                   0xe0, 0xf3, 0xf8, 0x00, 0xf8, 0x13,
                                   0xf8, 0x1f, 0xf8, 0x20, 0xf8, 0xff};

int32_t ParseSimpleTest()
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;
   int nCBORError;


   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spSimpleValues),
                    QCBOR_DECODE_MODE_NORMAL);


   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_ARRAY ||
      Item.val.uCount != 10)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_FALSE)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_TRUE)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_NULL)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_UNDEF)
      return -1;

   // A break
   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_BAD_BREAK)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_UKNOWN_SIMPLE || Item.val.uSimple != 0)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_UKNOWN_SIMPLE || Item.val.uSimple != 19)
      return -1;

   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_BAD_TYPE_7)
      return -1;

   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_BAD_TYPE_7)
      return -1;

   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_BAD_TYPE_7)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_UKNOWN_SIMPLE || Item.val.uSimple != 32)
      return -1;

   if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
      return nCBORError;
   if(Item.uDataType != QCBOR_TYPE_UKNOWN_SIMPLE || Item.val.uSimple != 255)
      return -1;

   return 0;

}


static int IsNotWellFormedError(QCBORError nErr)
{
   switch(nErr){
      case QCBOR_ERR_INDEFINITE_STRING_CHUNK:
      case QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN:
      case QCBOR_ERR_UNSUPPORTED:
      case QCBOR_ERR_HIT_END:
      case QCBOR_ERR_BAD_TYPE_7:
      case QCBOR_ERR_BAD_BREAK:
      case QCBOR_ERR_EXTRA_BYTES:
      case QCBOR_ERR_BAD_INT:
         return 1;
      default:
         return 0;
   }
}


int32_t NotWellFormedTests()
{
   // Loop over all the not-well-formed instance of CBOR
   // that are test vectors in not_well_formed_cbor.h
   const uint16_t nArraySize = sizeof(paNotWellFormedCBOR)/sizeof(struct someBinaryBytes);
   for(uint16_t nIterate = 0; nIterate < nArraySize; nIterate++) {
      const struct someBinaryBytes *pBytes = &paNotWellFormedCBOR[nIterate];
      const UsefulBufC Input = (UsefulBufC){pBytes->p, pBytes->n};

      // Set up decoder context. String allocator needed for indefinite
      // string test cases
      QCBORDecodeContext DCtx;
      QCBORDecode_Init(&DCtx, Input, QCBOR_DECODE_MODE_NORMAL);
      UsefulBuf_MAKE_STACK_UB(Pool, 100);
      QCBORDecode_SetMemPool(&DCtx, Pool, 0);

      // Loop getting items until no more to get
      QCBORError nCBORError;
      do {
         QCBORItem Item;

         nCBORError = QCBORDecode_GetNext(&DCtx, &Item);
      } while(nCBORError == QCBOR_SUCCESS);

      // Every test vector must fail with
      // a not-well-formed error. If not
      // this test fails.
      if(!IsNotWellFormedError(nCBORError)) {
         // Return index of failure in the error code
         return 2000 + nIterate;
      }
   }
   return 0;
}


struct FailInput {
   UsefulBufC Input;
   QCBORError nError;
};


static int32_t ProcessFailures(struct FailInput *pFailInputs, size_t nNumFails)
{
   for(struct FailInput *pF = pFailInputs; pF < pFailInputs + nNumFails; pF++) {
      // Set up the decoding context including a memory pool so that
      // indefinite length items can be checked
      QCBORDecodeContext DCtx;
      QCBORDecode_Init(&DCtx, pF->Input, QCBOR_DECODE_MODE_NORMAL);
      UsefulBuf_MAKE_STACK_UB(Pool, 100);
      QCBORError nCBORError = QCBORDecode_SetMemPool(&DCtx, Pool, 0);
      if(nCBORError) {
         return -9;
      }

      // Iterate until there is an error of some sort error
      QCBORItem Item;
      do {
         // Set to something none-zero other than QCBOR_TYPE_NONE
         memset(&Item, 0x33, sizeof(Item));

         nCBORError = QCBORDecode_GetNext(&DCtx, &Item);
      } while(nCBORError == QCBOR_SUCCESS);

      // Must get the expected error or the this test fails
      // The data and label type must also be QCBOR_TYPE_NONE
      if(nCBORError != pF->nError ||
         Item.uDataType != QCBOR_TYPE_NONE ||
         Item.uLabelType != QCBOR_TYPE_NONE) {
         // return index of CBOR + 100
         const size_t nIndex = (size_t)(pF - pFailInputs)/sizeof(struct FailInput);
         return (int32_t)(nIndex * 100 + nCBORError);
      }
   }

   return 0;
}


struct FailInput  Failures[] = {
   // Most of this is copied from not_well_formed.h. Here the error code
   // returned is also checked.

   // Indefinite length strings must be closed off
   // An indefinite length byte string not closed off
   { {(uint8_t[]){0x5f, 0x41, 0x00}, 3}, QCBOR_ERR_HIT_END },
   // An indefinite length text string not closed off
   { {(uint8_t[]){0x7f, 0x61, 0x00}, 3}, QCBOR_ERR_HIT_END },


   // All the chunks in an indefinite length string must be of the type of indefinite length string
   // indefinite length byte string with text string chunk
   { {(uint8_t[]){0x5f, 0x61, 0x00, 0xff}, 4}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length text string with a byte string chunk
   { {(uint8_t[]){0x7f, 0x41, 0x00, 0xff}, 4}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length byte string with an positive integer chunk
   { {(uint8_t[]){0x5f, 0x00, 0xff}, 3}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length byte string with an negative integer chunk
   { {(uint8_t[]){0x5f, 0x21, 0xff}, 3}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length byte string with an array chunk
   { {(uint8_t[]){0x5f, 0x80, 0xff}, 3}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length byte string with an map chunk
   { {(uint8_t[]){0x5f, 0xa0, 0xff}, 3}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length byte string with tagged integer chunk
   { {(uint8_t[]){0x5f, 0xc0, 0x00, 0xff}, 4}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   // indefinite length byte string with an simple type chunk
   { {(uint8_t[]){0x5f, 0xe0, 0xff}, 3}, QCBOR_ERR_INDEFINITE_STRING_CHUNK },
   { {(uint8_t[]){0x5f, 0x5f, 0x41, 0x00, 0xff, 0xff}, 6}, QCBOR_ERR_INDEFINITE_STRING_CHUNK},
   // indefinite length text string with indefinite string inside
   { {(uint8_t[]){0x7f, 0x7f, 0x61, 0x00, 0xff, 0xff}, 6}, QCBOR_ERR_INDEFINITE_STRING_CHUNK},


   // Definte length maps and arrays must be closed by having the right number of items
   // A definte length array that is supposed to have 1 item, but has none
   { {(uint8_t[]){0x81}, 1}, QCBOR_ERR_HIT_END },
   // A definte length array that is supposed to have 2 items, but has only 1
   { {(uint8_t[]){0x82, 0x00}, 2}, QCBOR_ERR_HIT_END },
   // A definte length array that is supposed to have 511 items, but has only 1
   { {(uint8_t[]){0x9a, 0x01, 0xff, 0x00}, 4}, QCBOR_ERR_HIT_END },
   // A definte length map that is supposed to have 1 item, but has none
   { {(uint8_t[]){0xa1}, 1}, QCBOR_ERR_HIT_END },
   // A definte length map that is supposed to have s item, but has only 1
   { {(uint8_t[]){0xa2, 0x01, 0x02}, 3}, QCBOR_ERR_HIT_END },


   // Indefinte length maps and arrays must be ended by a break
   // Indefinite length array with zero items and no break
   { {(uint8_t[]){0x9f}, 1}, QCBOR_ERR_HIT_END },
   // Indefinite length array with two items and no break
   { {(uint8_t[]){0x9f, 0x01, 0x02}, 3}, QCBOR_ERR_HIT_END },
   // Indefinite length map with zero items and no break
   { {(uint8_t[]){0xbf}, 1}, QCBOR_ERR_HIT_END },
   // Indefinite length map with two items and no break
   { {(uint8_t[]){0xbf, 0x01, 0x02, 0x01, 0x02}, 5}, QCBOR_ERR_HIT_END },


   // Nested maps and arrays must be closed off (some extra nested test vectors)
   // Unclosed indefinite array containing a close definite array
   { {(uint8_t[]){0x9f, 0x80, 0x00}, 3}, QCBOR_ERR_HIT_END },
   // Definite length array containing an unclosed indefinite array
   { {(uint8_t[]){0x81, 0x9f}, 2}, QCBOR_ERR_HIT_END },
   // Deeply nested definite length arrays with deepest one unclosed
   { {(uint8_t[]){0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81}, 9}, QCBOR_ERR_HIT_END },
   // Deeply nested indefinite length arrays with deepest one unclosed
   { {(uint8_t[]){0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0xff, 0xff, 0xff, 0xff}, 9}, QCBOR_ERR_HIT_END },
   // Mixed nesting with indefinite unclosed
   // TODO: think through this one
   { {(uint8_t[]){0x9f, 0x81, 0x9f, 0x81, 0x9f, 0x9f, 0xff, 0xff, 0xff}, 9}, QCBOR_ERR_BAD_BREAK },
   // Mixed nesting with definite unclosed
   // TODO: think through this one
   { {(uint8_t[]){0x9f, 0x82, 0x9f, 0x81, 0x9f, 0x9f, 0xff, 0xff, 0xff, 0xff}, 10}, QCBOR_ERR_BAD_BREAK },


   // The "argument" for the data item is incomplete
   // Positive integer missing 1 byte argument
   { {(uint8_t[]){0x18}, 1}, QCBOR_ERR_HIT_END },
   // Positive integer missing 2 byte argument
   { {(uint8_t[]){0x19}, 1}, QCBOR_ERR_HIT_END },
   // Positive integer missing 4 byte argument
   { {(uint8_t[]){0x1a}, 1}, QCBOR_ERR_HIT_END },
   // Positive integer missing 8 byte argument
   { {(uint8_t[]){0x1b}, 1}, QCBOR_ERR_HIT_END },
   // Positive integer missing 1 byte of 2 byte argument
   { {(uint8_t[]){0x19, 0x01}, 2}, QCBOR_ERR_HIT_END },
   // Positive integer missing 2 bytes of 4 byte argument
   { {(uint8_t[]){0x1a, 0x01, 0x02}, 3}, QCBOR_ERR_HIT_END },
   // Positive integer missing 1 bytes of 7 byte argument
   { {(uint8_t[]){0x1b, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}, 8}, QCBOR_ERR_HIT_END },
   // Negative integer missing 1 byte argument
   { {(uint8_t[]){0x38}, 1}, QCBOR_ERR_HIT_END },
   // Binary string missing 1 byte argument
   { {(uint8_t[]){0x58}, 1}, QCBOR_ERR_HIT_END },
   // Text string missing 1 byte argument
   { {(uint8_t[]){0x78}, 1}, QCBOR_ERR_HIT_END },
   // Array missing 1 byte argument
   { {(uint8_t[]){0x98}, 1}, QCBOR_ERR_HIT_END },
   // Map missing 1 byte argument
   { {(uint8_t[]){0xb8}, 1}, QCBOR_ERR_HIT_END },
   // Tag missing 1 byte argument
   { {(uint8_t[]){0xd8}, 1}, QCBOR_ERR_HIT_END },
   // Simple missing 1 byte argument
   { {(uint8_t[]){0xf8}, 1}, QCBOR_ERR_HIT_END },


   // Breaks must not occur in definite length arrays and maps
   // Array of length 1 with sole member replaced by a break
   { {(uint8_t[]){0x81, 0xff}, 2}, QCBOR_ERR_BAD_BREAK },
   // Array of length 2 with 2nd member replaced by a break
   { {(uint8_t[]){0x82, 0x00, 0xff}, 3}, QCBOR_ERR_BAD_BREAK },
   // Map of length 1 with sole member label replaced by a break
   { {(uint8_t[]){0xa1, 0xff}, 2}, QCBOR_ERR_BAD_BREAK },
   // Map of length 1 with sole member label replaced by break
   // Alternate representation that some decoders handle difference
   { {(uint8_t[]){0xa1, 0xff, 0x00}, 3}, QCBOR_ERR_BAD_BREAK },
   // Array of length 1 with 2nd member value replaced by a break
   { {(uint8_t[]){0xa1, 0x00, 0xff}, 3}, QCBOR_ERR_BAD_BREAK },
   // Map of length 2 with 2nd member replaced by a break
   { {(uint8_t[]){0xa2, 0x00, 0x00, 0xff}, 4}, QCBOR_ERR_BAD_BREAK },


   // Breaks must not occur on their own out of an indefinite length data item
   // A bare break is not well formed
   { {(uint8_t[]){0xff}, 1}, QCBOR_ERR_BAD_BREAK },
   // A bare break after a zero length definite length array
   { {(uint8_t[]){0x80, 0xff}, 2}, QCBOR_ERR_BAD_BREAK },
   // A bare break after a zero length indefinite length map
   { {(uint8_t[]){0x9f, 0xff, 0xff}, 3}, QCBOR_ERR_BAD_BREAK },


   // Forbidden two byte encodings of simple types
   // Must use 0xe0 instead
   { {(uint8_t[]){0xf8, 0x00}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe1 instead
   { {(uint8_t[]){0xf8, 0x01}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe2 instead
   { {(uint8_t[]){0xf8, 0x02}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe3 instead
   { {(uint8_t[]){0xf8, 0x03}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe4 instead
   { {(uint8_t[]){0xf8, 0x04}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe5 instead
   { {(uint8_t[]){0xf8, 0x05}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe6 instead
   { {(uint8_t[]){0xf8, 0x06}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe7 instead
   { {(uint8_t[]){0xf8, 0x07}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe8 instead
   { {(uint8_t[]){0xf8, 0x08}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xe9 instead
   { {(uint8_t[]){0xf8, 0x09}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xea instead
   { {(uint8_t[]){0xf8, 0x0a}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xeb instead
   { {(uint8_t[]){0xf8, 0x0b}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xec instead
   { {(uint8_t[]){0xf8, 0x0c}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xed instead
   { {(uint8_t[]){0xf8, 0x0d}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xee instead
   { {(uint8_t[]){0xf8, 0x0e}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xef instead
   { {(uint8_t[]){0xf8, 0x0f}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xf0 instead
   { {(uint8_t[]){0xf8, 0x10}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xf1 instead
   { {(uint8_t[]){0xf8, 0x11}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Should use 0xf2 instead
   { {(uint8_t[]){0xf8, 0x12}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Must use 0xf3 instead
   { {(uint8_t[]){0xf8, 0x13}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Must use 0xf4 instead
   { {(uint8_t[]){0xf8, 0x14}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Must use 0xf5 instead
   { {(uint8_t[]){0xf8, 0x15}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Must use 0xf6 instead
   { {(uint8_t[]){0xf8, 0x16}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Must use 0xf7 instead
   { {(uint8_t[]){0xf8, 0x17}, 2}, QCBOR_ERR_BAD_TYPE_7 },
   // Must use 0xf8 instead
   { {(uint8_t[]){0xf8, 0x18}, 2}, QCBOR_ERR_BAD_TYPE_7 },


   // Integers with additional info indefinite length
   // Positive integer with additional info indefinite length
   { {(uint8_t[]){0x1f}, 1}, QCBOR_ERR_BAD_INT },
   // Negative integer with additional info indefinite length
   { {(uint8_t[]){0x3f}, 1}, QCBOR_ERR_BAD_INT },
   // CBOR tag with "argument" an indefinite length
   { {(uint8_t[]){0xdf, 0x00}, 2}, QCBOR_ERR_BAD_INT },
   // CBOR tag with "argument" an indefinite length alternate vector
   { {(uint8_t[]){0xdf}, 1}, QCBOR_ERR_BAD_INT },


   // Missing bytes from a deterministic length string
   // A byte string is of length 1 without the 1 byte
   { {(uint8_t[]){0x41}, 1}, QCBOR_ERR_HIT_END },
   // A text string is of length 1 without the 1 byte
   { {(uint8_t[]){0x61}, 1}, QCBOR_ERR_HIT_END },
   // Byte string should have 2^32-15 bytes, but has one
   { {(uint8_t[]){0x5a, 0xff, 0xff, 0xff, 0xf0, 0x00}, 6}, QCBOR_ERR_HIT_END },
   // Byte string should have 2^32-15 bytes, but has one
   { {(uint8_t[]){0x7a, 0xff, 0xff, 0xff, 0xf0, 0x00}, 6}, QCBOR_ERR_HIT_END },


   // Use of unassigned additional information values
   // Major type positive integer with reserved value 28
   { {(uint8_t[]){0x1c}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type positive integer with reserved value 29
   { {(uint8_t[]){0x1d}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type positive integer with reserved value 30
   { {(uint8_t[]){0x1e}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type negative integer with reserved value 28
   { {(uint8_t[]){0x3c}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type negative integer with reserved value 29
   { {(uint8_t[]){0x3d}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type negative integer with reserved value 30
   { {(uint8_t[]){0x3e}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type byte string with reserved value 28 length
   { {(uint8_t[]){0x5c}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type byte string with reserved value 29 length
   { {(uint8_t[]){0x5d}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type byte string with reserved value 30 length
   { {(uint8_t[]){0x5e}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type text string with reserved value 28 length
   { {(uint8_t[]){0x7c}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type text string with reserved value 29 length
   { {(uint8_t[]){0x7d}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type text string with reserved value 30 length
   { {(uint8_t[]){0x7e}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type array with reserved value 28 length
   { {(uint8_t[]){0x9c}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type array with reserved value 29 length
   { {(uint8_t[]){0x9d}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type array with reserved value 30 length
   { {(uint8_t[]){0x9e}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type map with reserved value 28 length
   { {(uint8_t[]){0xbc}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type map with reserved value 29 length
   { {(uint8_t[]){0xbd}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type map with reserved value 30 length
   { {(uint8_t[]){0xbe}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type tag with reserved value 28 length
   { {(uint8_t[]){0xdc}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type tag with reserved value 29 length
   { {(uint8_t[]){0xdd}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type tag with reserved value 30 length
   { {(uint8_t[]){0xde}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type simple with reserved value 28 length
   { {(uint8_t[]){0xfc}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type simple with reserved value 29 length
   { {(uint8_t[]){0xfd}, 1}, QCBOR_ERR_UNSUPPORTED },
   // Major type simple with reserved value 30 length
   { {(uint8_t[]){0xfe}, 1}, QCBOR_ERR_UNSUPPORTED },


   // Maps must have an even number of data items (key & value)
   // Map with 1 item when it should have 2
   { {(uint8_t[]){0xa1, 0x00}, 2}, QCBOR_ERR_HIT_END },
   // Map with 3 item when it should have 4
   { {(uint8_t[]){0xa2, 0x00, 0x00, 0x00}, 2}, QCBOR_ERR_HIT_END },
   // Map with 1 item when it should have 2
   { {(uint8_t[]){0xbf, 0x00, 0xff}, 3}, QCBOR_ERR_BAD_BREAK },
   // Map with 3 item when it should have 4
   { {(uint8_t[]){0xbf, 0x00, 0x00, 0x00, 0xff}, 5}, QCBOR_ERR_BAD_BREAK },


   // In addition to not-well-formed, some invalid CBOR
   // Text-based date, with an integer
   { {(uint8_t[]){0xc0, 0x00}, 2}, QCBOR_ERR_BAD_OPT_TAG },
   // Epoch date, with an byte string
   { {(uint8_t[]){0xc1, 0x41, 0x33}, 3}, QCBOR_ERR_BAD_OPT_TAG },
   // tagged as both epoch and string dates
   { {(uint8_t[]){0xc1, 0xc0, 0x00}, 3}, QCBOR_ERR_BAD_OPT_TAG },
   // big num tagged an int, not a byte string
   { {(uint8_t[]){0xc2, 0x00}, 2}, QCBOR_ERR_BAD_OPT_TAG },
};

int32_t DecodeFailureTests()
{
   int32_t nResult;

   nResult = ProcessFailures(Failures, sizeof(Failures)/sizeof(struct FailInput));
   if(nResult) {
      return nResult;
   }

   // Corrupt the UsefulInputBuf and see that
   // it reflected correctly for CBOR decoding
   {
      QCBORDecodeContext DCtx;
      QCBORItem          Item;
      QCBORError         nCBORError;

      QCBORDecode_Init(&DCtx,
                       UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spSimpleValues),
                       QCBOR_DECODE_MODE_NORMAL);

      if((nCBORError = QCBORDecode_GetNext(&DCtx, &Item)))
         return nCBORError;
      if(Item.uDataType != QCBOR_TYPE_ARRAY ||
         Item.val.uCount != 10) {
         // This wasn't supposed to happen
         return -1;
      }

      DCtx.InBuf.magic = 0; // Reach in and corrupt the UsefulInputBuf

      nCBORError = QCBORDecode_GetNext(&DCtx, &Item);
      if(nCBORError != QCBOR_ERR_HIT_END) {
         // Did not get back the error expected
         return -2;
      }
   }

/*
   This test is disabled until QCBOREncode_EncodeHead() is brought in so
 the size encoded can be tied to SIZE_MAX and work for all size CPUs.

 This relies on the largest string allowed being SIZE_MAX -4 rather than
 SIZE_MAX. That way the test can be performed.
   {
      QCBORDecodeContext DCtx;
      QCBORItem          Item;

      static uint8_t foo[] = {0x5b, 0xff, 0xff, 0xff, 0xff,
                                    0xff, 0xff, 0xff, 0xff};

      QCBORDecode_Init(&DCtx,
                       UsefulBuf_FROM_BYTE_ARRAY_LITERAL(foo),
                       QCBOR_DECODE_MODE_NORMAL);

      if(QCBOR_ERR_STRING_TOO_LONG != QCBORDecode_GetNext(&DCtx, &Item)) {
         return -4;
      }
   }
*/

   return 0;
}


/* Try all 256 values of the byte at nLen including recursing for
 each of the values to try values at nLen+1 ... up to nLenMax
 */
static void ComprehensiveInputRecurser(uint8_t *pBuf, size_t nLen, size_t nLenMax)
{
   if(nLen >= nLenMax) {
      return;
   }

   for(int inputByte = 0; inputByte < 256; inputByte++) {
      // Set up the input
      pBuf[nLen] = (uint8_t)inputByte;
      const UsefulBufC Input = {pBuf, nLen+1};

      // Get ready to parse
      QCBORDecodeContext DCtx;
      QCBORDecode_Init(&DCtx, Input, QCBOR_DECODE_MODE_NORMAL);

      // Parse by getting the next item until an error occurs
      // Just about every possible decoder error can occur here
      // The goal of this test is not to check for the correct
      // error since that is not really possible. It is to
      // see that there is no crash on hostile input.
      while(1) {
         QCBORItem Item;
         QCBORError nCBORError = QCBORDecode_GetNext(&DCtx, &Item);
         if(nCBORError != QCBOR_SUCCESS) {
            break;
         }
      }

      ComprehensiveInputRecurser(pBuf, nLen+1, nLenMax);
   }
}


/*
 Public function for initialization. See header qcbor.h
 */
int32_t ComprehensiveInputTest()
{
   // Size 2 tests 64K inputs and runs quickly
   uint8_t pBuf[2];

   ComprehensiveInputRecurser(pBuf, 0, sizeof(pBuf));

   return 0;
}


/*
 Public function for initialization. See header qcbor.h
 */
int32_t BigComprehensiveInputTest()
{
   // size 3 tests 16 million inputs and runs OK
   // in seconds on fast machines. Size 4 takes
   // 10+ minutes and 5 half a day on fast
   // machines. This test is kept separate from
   // the others so as to no slow down the use
   // of them as a very frequent regression.
   uint8_t pBuf[3]; //

   ComprehensiveInputRecurser(pBuf, 0, sizeof(pBuf));

   return 0;
}


static uint8_t spDateTestInput[] = {
   0xc0, // tag for string date
   0x6a, '1','9','8','5','-','0','4','-','1','2', // Date string

   0xc1, // tag for epoch date
   0x1a, 0x53, 0x72, 0x4E, 0x00, // Epoch date 1400000000; Tue, 13 May 2014 16:53:20 GMT

   // CBOR_TAG_B64
   0xc1, 0xcf, 0xd8, 0x22, // 0xee, // Epoch date with extra tags TODO: fix this test
   0x1a, 0x53, 0x72, 0x4E, 0x01,

   0xc1, // tag for epoch date
   0x1b, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // Too large integer

   0xc1, // tag for epoch date
   0xfa, 0x3f, 0x8c, 0xcc, 0xcd, // double with value 1.1

   0xc1, // tag for epoch date
   0xfa, 0x7f, 0x7f, 0xff, 0xff, // 3.4028234663852886e+38 too large

   0xc1, // tag for epoch date
   0xfb, 0x43, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 9223372036854775808.000000 just barely too large
   //0xfa, 0x7f, 0x7f, 0xff, 0xff // 3.4028234663852886e+38 too large

   0xc1, // tag for epoch date
   0xfb, 0x43, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe // 9223372036854773760 largest supported
};


// have to check float expected only to within an epsilon
int CHECK_EXPECTED_DOUBLE(double val, double expected) {

   double diff = val - expected;

   diff = fabs(diff);

   return diff > 0.0000001;
}


int32_t DateParseTest()
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spDateTestInput),
                    QCBOR_DECODE_MODE_NORMAL);

   const uint64_t uTags[] = {15};
   QCBORTagListIn TagList = {1, uTags};

   QCBORDecode_SetCallerConfiguredTagList(&DCtx, &TagList);

   // String date
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_DATE_STRING ||
      UsefulBuf_Compare(Item.val.dateString, UsefulBuf_FromSZ("1985-04-12"))){
      return -2;
   }

   // Epoch date
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -3;
   if(Item.uDataType != QCBOR_TYPE_DATE_EPOCH ||
      Item.val.epochDate.nSeconds != 1400000000 ||
      Item.val.epochDate.fSecondsFraction != 0 ) {
      return -4;
   }

   // Epoch date with extra CBOR_TAG_B64 tag that doesn't really mean anything
   // but want to be sure extra tag doesn't cause a problem
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -5;
   if(Item.uDataType != QCBOR_TYPE_DATE_EPOCH ||
      Item.val.epochDate.nSeconds != 1400000001 ||
      Item.val.epochDate.fSecondsFraction != 0 ||
      !QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_B64)) {
      return -6;
   }

   // Epoch date that is too large for our representation
   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_DATE_OVERFLOW) {
      return -7;
   }

   // Epoch date in float format with fractional seconds
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -8;
   if(Item.uDataType != QCBOR_TYPE_DATE_EPOCH ||
      Item.val.epochDate.nSeconds != 1 ||
      CHECK_EXPECTED_DOUBLE(Item.val.epochDate.fSecondsFraction, 0.1 )) {
      return -9;
   }

   // Epoch date float that is too large for our representation
   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_DATE_OVERFLOW) {
      return -10;
   }

   // Epoch date double that is just slightly too large
   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_ERR_DATE_OVERFLOW) {
      return -11;
   }

   // Largest double epoch date supported
   if(QCBORDecode_GetNext(&DCtx, &Item) != QCBOR_SUCCESS ||
      Item.uDataType != QCBOR_TYPE_DATE_EPOCH ||
      Item.val.epochDate.nSeconds != 9223372036854773760 ||
      Item.val.epochDate.nSeconds == 0) {
      return -12;
   }
   // TODO: could use a few more tests with float, double, and half precsion
   // and negative (but coverage is still pretty good)

   return 0;
}

// Really simple basic input for tagging test
static uint8_t spOptTestInput[] = {
   0xd9, 0xd9, 0xf7, // CBOR magic number
   0x81, // Array of one
   0xd8, 0x04, // non-preferred serialization of tag 4
   0x82, 0x01, 0x03}; // fraction 1/3

/*
 DB 9192939495969798 # tag(10489608748473423768)
   80               # array(0)
 */
static uint8_t spEncodedLargeTag[] = {0xdb, 0x91, 0x92, 0x93, 0x94, 0x95,
                                      0x96, 0x97, 0x98, 0x80};

/*
DB 9192939495969798 # tag(10489608748473423768)
   D8 88            # tag(136)
      C6            # tag(6)
         C7         # tag(7)
            80      # array(0)
*/
static uint8_t spLotsOfTags[] = {0xdb, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
                                 0x97, 0x98, 0xd8, 0x88, 0xc6, 0xc7, 0x80};

/*
 The cbor.me parse of this.
 55799(55799(55799({6(7(-23)): 5859837686836516696(7({7(-20): 11({17(-18): 17(17(17("Organization"))),
 9(-17): 773("SSG"), -15: 16(17(6(7(8(9(10(11(12(13(14(15("Confusion")))))))))))), 17(-16): 17("San Diego"),
 17(-14): 17("US")}), 23(-19): 19({-11: 9({-9: -7}),
 90599561(90599561(90599561(-10))): 12(h'0102030405060708090A')})})),
 16(-22): 23({11(8(7(-5))): 8(-3)})})))
 */
static uint8_t spCSRWithTags[] = {
   0xd9, 0xd9, 0xf7, 0xd9, 0xd9, 0xf7, 0xd9, 0xd9, 0xf7, 0xa2,
      0xc6, 0xc7, 0x36,
      0xdb, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0xc7, 0xa2,
         0xda, 0x00, 0x00, 0x00, 0x07, 0x33,
         0xcb, 0xa5,
            0xd1, 0x31,
            0xd1, 0xd1, 0xd1, 0x6c,
               0x4f, 0x72, 0x67, 0x61, 0x6e, 0x69, 0x7a, 0x61, 0x74, 0x69, 0x6f, 0x6e,
            0xc9, 0x30,
            0xd9, 0x03, 0x05, 0x63,
               0x53, 0x53, 0x47,
            0x2e,
            0xd0, 0xd1, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0x69,
               0x43, 0x6f, 0x6e, 0x66, 0x75, 0x73, 0x69, 0x6f, 0x6e,
            0xd1, 0x2f,
            0xd1, 0x69,
               0x53, 0x61, 0x6e, 0x20, 0x44, 0x69, 0x65, 0x67, 0x6f,
            0xd1, 0x2d,
            0xd1, 0x62,
               0x55, 0x53,
         0xd7, 0x32,
         0xd3, 0xa2,
            0x2a,
            0xc9, 0xa1,
               0x28,
               0x26,
            0xda, 0x05, 0x66, 0x70, 0x89, 0xda, 0x05, 0x66, 0x70, 0x89, 0xda, 0x05, 0x66, 0x70, 0x89, 0x29,
            0xcc, 0x4a,
               0x01, 0x02, 0x03, 0x04, 0x05, 0x06,0x07, 0x08, 0x09, 0x0a,
   0xd0, 0x35,
   0xd7, 0xa1,
      0xcb, 0xc8, 0xc7, 0x24,
      0xc8, 0x22};

static int32_t CheckCSRMaps(QCBORDecodeContext *pDC);


int32_t OptTagParseTest()
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spOptTestInput),
                    QCBOR_DECODE_MODE_NORMAL);

   //-------------------------
   // This text matches the magic number tag and the fraction tag
   if(QCBORDecode_GetNext(&DCtx, &Item)) {
      return -2;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY ||
      !QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_CBOR_MAGIC)) {
      return -3;
   }

   if(QCBORDecode_GetNext(&DCtx, &Item)) {
      return -4;
   }

#ifdef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
   if(Item.uDataType != QCBOR_TYPE_ARRAY ||
      !QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_DECIMAL_FRACTION) ||
      Item.val.uCount != 2) {
      return -5;
   }
#else
   if(Item.uDataType != QCBOR_TYPE_DECIMAL_FRACTION) {
      return -6;
   }
#endif

   // --------------------------------
   // This test decodes the very large tag, but it is not in
   // any list so it is ignored.
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spEncodedLargeTag),
                    QCBOR_DECODE_MODE_NORMAL);
   if(QCBORDecode_GetNext(&DCtx, &Item)) {
      return -6;
   }
   if(Item.uTagBits) {
      return -7;
   }

   // ----------------------------------
   // This test sets up a caller-config list that includes the very large
   // tage and then matches it.
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spEncodedLargeTag),
                    QCBOR_DECODE_MODE_NORMAL);
   const uint64_t puList[] = {0x9192939495969798, 257};
   const QCBORTagListIn TL = {2, puList};
   QCBORDecode_SetCallerConfiguredTagList(&DCtx, &TL);

   if(QCBORDecode_GetNext(&DCtx, &Item)) {
      return -8;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY ||
      !QCBORDecode_IsTagged(&DCtx, &Item, 0x9192939495969798) ||
      QCBORDecode_IsTagged(&DCtx, &Item, 257) ||
      QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_BIGFLOAT) ||
      Item.val.uCount != 0) {
      return -9;
   }

   //------------------------
   // Sets up a caller-configured list and look up something not in it
   const uint64_t puLongList[17] = {1,2,1};
   const QCBORTagListIn TLLong = {17, puLongList};
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spEncodedLargeTag),
                    QCBOR_DECODE_MODE_NORMAL);
   QCBORDecode_SetCallerConfiguredTagList(&DCtx, &TLLong);
   if(QCBORDecode_GetNext(&DCtx, &Item)) {
      return -11;
   }

   // -----------------------
   // This tests retrievel of the full tag list
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spLotsOfTags),
                    QCBOR_DECODE_MODE_NORMAL);
   uint64_t puTags[16];
   QCBORTagListOut Out = {0, 4, puTags};
   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -12;
   }
   if(puTags[0] != 0x9192939495969798 ||
      puTags[1] != 0x88 ||
      puTags[2] != 0x06 ||
      puTags[3] != 0x07) {
      return -13;
   }

   // ----------------------
   // This text if too small of an out list
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spLotsOfTags),
                    QCBOR_DECODE_MODE_NORMAL);
   QCBORTagListOut OutSmall = {0, 3, puTags};
   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &OutSmall) != QCBOR_ERR_TOO_MANY_TAGS) {
      return -14;
   }

   // ---------------
   // Parse a version of the "CSR" that has had a ton of tags randomly inserted
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCSRWithTags),
                    QCBOR_DECODE_MODE_NORMAL);
   int n = CheckCSRMaps(&DCtx);
   if(n) {
      return n-2000;
   }

   Out = (QCBORTagListOut){0, 16, puTags};
   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCSRWithTags),
                    QCBOR_DECODE_MODE_NORMAL);

   const uint64_t puTagList[] = {773, 1, 90599561};
   const QCBORTagListIn TagList = {3, puTagList};
   QCBORDecode_SetCallerConfiguredTagList(&DCtx, &TagList);


   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -100;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      !QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_CBOR_MAGIC) ||
      QCBORDecode_IsTagged(&DCtx, &Item, 90599561) ||
      QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_DATE_EPOCH) ||
      Item.val.uCount != 2 ||
      puTags[0] != CBOR_TAG_CBOR_MAGIC ||
      puTags[1] != CBOR_TAG_CBOR_MAGIC ||
      puTags[2] != CBOR_TAG_CBOR_MAGIC ||
      Out.uNumUsed != 3) {
      return -101;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -102;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_CBOR_MAGIC) ||
      QCBORDecode_IsTagged(&DCtx, &Item, 6) ||
      QCBORDecode_IsTagged(&DCtx, &Item, 7) || // item is tagged 7, but 7 is not configured to be recognized
      Item.val.uCount != 2 ||
      puTags[0] != 5859837686836516696 ||
      puTags[1] != 7 ||
      Out.uNumUsed != 2) {
      return -103;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -104;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      Item.uTagBits ||
      Item.val.uCount != 5 ||
      puTags[0] != 0x0b ||
      Out.uNumUsed != 1) {
      return -105;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -106;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      !QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_COSE_MAC0) ||
      Item.val.string.len != 12 ||
      puTags[0] != CBOR_TAG_COSE_MAC0 ||
      puTags[1] != CBOR_TAG_COSE_MAC0 ||
      puTags[2] != CBOR_TAG_COSE_MAC0 ||
      Out.uNumUsed != 3) {
      return -105;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -107;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      !QCBORDecode_IsTagged(&DCtx, &Item, 773) ||
      Item.val.string.len != 3 ||
      puTags[0] != 773 ||
      Out.uNumUsed != 1) {
      return -108;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -109;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      !QCBORDecode_IsTagged(&DCtx, &Item, 16) ||
      Item.val.string.len != 9 ||
      puTags[0] != 16 ||
      puTags[11] != 0x0f ||
      Out.uNumUsed != 12) {
      return -110;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -111;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      !QCBORDecode_IsTagged(&DCtx, &Item, 17) ||
      Item.val.string.len != 9 ||
      puTags[0] != 17 ||
      Out.uNumUsed != 1) {
      return -112;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -111;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      !QCBORDecode_IsTagged(&DCtx, &Item, 17) ||
      Item.val.string.len != 2 ||
      puTags[0] != 17 ||
      Out.uNumUsed != 1) {
      return -112;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -113;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      QCBORDecode_IsTagged(&DCtx, &Item, 19) ||
      Item.val.uCount != 2 ||
      puTags[0] != 19 ||
      Out.uNumUsed != 1) {
      return -114;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -115;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      QCBORDecode_IsTagged(&DCtx, &Item, 9) ||
      Item.uTagBits ||
      Item.val.uCount != 1 ||
      puTags[0] != 9 ||
      Out.uNumUsed != 1) {
      return -116;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -116;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.val.int64 != -7 ||
      Item.uTagBits ||
      Out.uNumUsed != 0) {
      return -117;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -118;
   }
   if(Item.uDataType != QCBOR_TYPE_BYTE_STRING ||
      Item.val.string.len != 10 ||
      Item.uTagBits ||
      puTags[0] != 12 ||
      Out.uNumUsed != 1) {
      return -119;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -120;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      !QCBORDecode_IsTagged(&DCtx, &Item, CBOR_TAG_ENC_AS_B16) ||
      Item.val.uCount != 1 ||
      puTags[0] != 0x17 ||
      Out.uNumUsed != 1) {
      return -121;
   }

   if(QCBORDecode_GetNextWithTags(&DCtx, &Item, &Out)) {
      return -122;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64 ||
      QCBORDecode_IsTagged(&DCtx, &Item, 8) ||
      Item.val.int64 != -3 ||
      puTags[0] != 8 ||
      Out.uNumUsed != 1) {
      return -123;
   }

   if(QCBORDecode_Finish(&DCtx)) {
      return -124;
   }

   return 0;
}




static uint8_t spBigNumInput[] = {
 0x83,
   0xC2, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xC3, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xA4,
     0x63, 0x42, 0x4E, 0x2B,
       0xC2, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x18, 0x40,
       0xC2, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x63, 0x42, 0x4E, 0x2D,
       0xC3, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x38, 0x3F,
       0xC3, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


static uint8_t spBigNum[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


int32_t BignumParseTest()
{
   QCBORDecodeContext DCtx;
   QCBORItem Item;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNumInput),
                    QCBOR_DECODE_MODE_NORMAL);


   //
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -1;
   }

   //
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_POSBIGNUM ||
      UsefulBuf_Compare(Item.val.bigNum, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum))){
      return -1;
   }

   //
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_NEGBIGNUM ||
      UsefulBuf_Compare(Item.val.bigNum, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum))){
      return -1;
   }

   //
   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_MAP) {
      return -1;
   }

   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_POSBIGNUM ||
      Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.val.bigNum, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum))){
      return -1;
   }

   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_POSBIGNUM ||
      Item.uLabelType != QCBOR_TYPE_INT64 ||
      Item.label.int64 != 64 ||
      UsefulBuf_Compare(Item.val.bigNum, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum))){
      return -1;
   }

   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_NEGBIGNUM ||
      Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item.val.bigNum, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum))){
      return -1;
   }

   if(QCBORDecode_GetNext(&DCtx, &Item))
      return -1;
   if(Item.uDataType != QCBOR_TYPE_NEGBIGNUM ||
      Item.uLabelType != QCBOR_TYPE_INT64 ||
      Item.label.int64 != -64 ||
      UsefulBuf_Compare(Item.val.bigNum, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum))){
      return -1;
   }

   return 0;
}



static int32_t CheckItemWithIntLabel(QCBORDecodeContext *pCtx,
                                 uint8_t uDataType,
                                 uint8_t uNestingLevel,
                                 uint8_t uNextNest,
                                 int64_t nLabel,
                                 QCBORItem *pItem)
{
   QCBORItem Item;

   if(QCBORDecode_GetNext(pCtx, &Item)) return -1;
   if(Item.uDataType != uDataType) return -1;
   if(uNestingLevel > 0) {
      if(Item.uLabelType != QCBOR_TYPE_INT64 &&
         Item.uLabelType != QCBOR_TYPE_UINT64) {
         return -1;
      }
      if(Item.uLabelType == QCBOR_TYPE_INT64) {
         if(Item.label.int64 != nLabel) return -1;
      } else  {
         if(Item.label.uint64 != (uint64_t)nLabel) return -1;
      }
   }
   if(Item.uNestingLevel != uNestingLevel) return -1;
   if(Item.uNextNestLevel != uNextNest) return -1;

   if(pItem) {
      *pItem = Item;
   }
   return 0;
}


// Same code checks definite and indefinite length versions of the map
static int32_t CheckCSRMaps(QCBORDecodeContext *pDC)
{
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_MAP, 0, 1, 0, NULL)) return -1;

   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_MAP, 1, 2, -23, NULL)) return -1;

   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_MAP, 2, 3, -20, NULL)) return -1;

   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_TEXT_STRING, 3, 3, -18, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_TEXT_STRING, 3, 3, -17, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_TEXT_STRING, 3, 3, -15, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_TEXT_STRING, 3, 3, -16, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_TEXT_STRING, 3, 2, -14, NULL)) return -1;

   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_MAP, 2, 3, -19, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_MAP, 3, 4, -11, NULL)) return -1;

   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_INT64, 4, 3, -9, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_BYTE_STRING, 3, 1, -10, NULL)) return -1;

   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_MAP, 1, 2, -22, NULL)) return -1;
   if(CheckItemWithIntLabel(pDC, QCBOR_TYPE_INT64, 2, 0, -5, NULL)) return -1;

   if(QCBORDecode_Finish(pDC)) return -2;

   return 0;
}


/*
// cbor.me decoded output
{
    -23: {
        -20: {
            -18: "Organization",
            -17: "SSG",
            -15: "Confusion",
            -16: "San Diego",
            -14: "US"
        },
        -19: {
            -11: {
                -9: -7
            },
            -10: '\u0001\u0002\u0003\u0004\u0005\u0006\a\b\t\n'
        }
    },
    -22: {
        -5: -3
    }
}
 */


static uint8_t spCSRInput[] = {
   0xa2, 0x36, 0xa2, 0x33, 0xa5, 0x31, 0x6c, 0x4f,
   0x72, 0x67, 0x61, 0x6e, 0x69, 0x7a, 0x61, 0x74,
   0x69, 0x6f, 0x6e, 0x30, 0x63, 0x53, 0x53, 0x47,
   0x2e, 0x69, 0x43, 0x6f, 0x6e, 0x66, 0x75, 0x73,
   0x69, 0x6f, 0x6e, 0x2f, 0x69, 0x53, 0x61, 0x6e,
   0x20, 0x44, 0x69, 0x65, 0x67, 0x6f, 0x2d, 0x62,
   0x55, 0x53, 0x32, 0xa2, 0x2a, 0xa1, 0x28, 0x26,
   0x29, 0x4a, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
   0x07, 0x08, 0x09, 0x0a, 0x35, 0xa1, 0x24, 0x22};

int32_t NestedMapTest()
{
   QCBORDecodeContext DCtx;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCSRInput),
                    QCBOR_DECODE_MODE_NORMAL);

   return CheckCSRMaps(&DCtx);
}



int32_t StringDecoderModeFailTest()
{
   QCBORDecodeContext DCtx;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCSRInput),
                    QCBOR_DECODE_MODE_MAP_STRINGS_ONLY);

   QCBORItem Item;
   QCBORError nCBORError;

   if(QCBORDecode_GetNext(&DCtx, &Item)) {
      return -1;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP) {
      return -2;
   }

   nCBORError = QCBORDecode_GetNext(&DCtx, &Item);
   if(nCBORError != QCBOR_ERR_MAP_LABEL_TYPE) {
      return -3;
   }

   return 0;
}


// Same map as above, but using indefinite lengths
static uint8_t spCSRInputIndefLen[] = {
   0xbf, 0x36, 0xbf, 0x33, 0xbf, 0x31, 0x6c, 0x4f,
   0x72, 0x67, 0x61, 0x6e, 0x69, 0x7a, 0x61, 0x74,
   0x69, 0x6f, 0x6e, 0x30, 0x63, 0x53, 0x53, 0x47,
   0x2e, 0x69, 0x43, 0x6f, 0x6e, 0x66, 0x75, 0x73,
   0x69, 0x6f, 0x6e, 0x2f, 0x69, 0x53, 0x61, 0x6e,
   0x20, 0x44, 0x69, 0x65, 0x67, 0x6f, 0x2d, 0x62,
   0x55, 0x53, 0xff, 0x32, 0xbf, 0x2a, 0xbf, 0x28,
   0x26, 0xff, 0x29, 0x4a, 0x01, 0x02, 0x03, 0x04,
   0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0xff, 0xff,
   0x35, 0xbf, 0x24, 0x22, 0xff, 0xff};

int32_t NestedMapTestIndefLen()
{
   QCBORDecodeContext DCtx;

   QCBORDecode_Init(&DCtx,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCSRInputIndefLen),
                    QCBOR_DECODE_MODE_NORMAL);

   return CheckCSRMaps(&DCtx);
}



static UsefulBufC make_nested_indefinite_arrays(int n, UsefulBuf Storage)
{
   UsefulOutBuf UOB;
   UsefulOutBuf_Init(&UOB, Storage);

   int i;
   for(i = 0; i < n; i++) {
      UsefulOutBuf_AppendByte(&UOB, 0x9f);
   }

   for(i = 0; i < n; i++) {
      UsefulOutBuf_AppendByte(&UOB, 0xff);
   }
   return UsefulOutBuf_OutUBuf(&UOB);
}


static int32_t parse_indeflen_nested(UsefulBufC Nested, int nNestLevel)
{
   QCBORDecodeContext DC;
   QCBORDecode_Init(&DC, Nested, QCBOR_DECODE_MODE_NORMAL);

   int j;
   for(j = 0; j < nNestLevel; j++) {
      QCBORItem Item;
      int nReturn = QCBORDecode_GetNext(&DC, &Item);
      if(j >= QCBOR_MAX_ARRAY_NESTING) {
         // Should be in error
         if(nReturn != QCBOR_ERR_ARRAY_NESTING_TOO_DEEP) {
            return -4;
         } else {
            return 0; // Decoding doesn't recover after an error
         }
      } else {
         // Should be no error
         if(nReturn) {
            return -9; // Should not have got an error
         }
      }
      if(Item.uDataType != QCBOR_TYPE_ARRAY) {
         return -7;
      }
   }
   int nReturn = QCBORDecode_Finish(&DC);
   if(nReturn) {
      return -3;
   }
   return 0;
}


int32_t IndefiniteLengthNestTest()
{
   UsefulBuf_MAKE_STACK_UB(Storage, 50);
   int i;
   for(i=1; i < QCBOR_MAX_ARRAY_NESTING+4; i++) {
      const UsefulBufC Nested = make_nested_indefinite_arrays(i, Storage);
      int nReturn = parse_indeflen_nested(Nested, i);
      if(nReturn) {
         return nReturn;
      }
   }
   return 0;
}


// [1, [2, 3]]
static const uint8_t spIndefiniteArray[]     = {0x9f, 0x01, 0x82, 0x02, 0x03, 0xff};
// No closing break
static const uint8_t spIndefiniteArrayBad1[] = {0x9f};
// Not enough closing breaks
static const uint8_t spIndefiniteArrayBad2[] = {0x9f, 0x9f, 0x02, 0xff};
// Too many closing breaks
static const uint8_t spIndefiniteArrayBad3[] = {0x9f, 0x02, 0xff, 0xff};
// Unclosed indeflen inside def len
static const uint8_t spIndefiniteArrayBad4[] = {0x81, 0x9f};
// confused tag
static const uint8_t spIndefiniteArrayBad5[] = {0x9f, 0xd1, 0xff};

int32_t IndefiniteLengthArrayMapTest()
{
   int nResult;
   // --- first test -----
    UsefulBufC IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteArray);

    // Decode it and see if it is OK
    UsefulBuf_MAKE_STACK_UB(MemPool, 150);
    QCBORDecodeContext DC;
    QCBORItem Item;
    QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

    QCBORDecode_SetMemPool(&DC, MemPool, false);

    QCBORDecode_GetNext(&DC, &Item);

    if(Item.uDataType != QCBOR_TYPE_ARRAY ||
       Item.uNestingLevel != 0 ||
       Item.uNextNestLevel != 1) {
       return -111;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_INT64 ||
       Item.uNestingLevel != 1 ||
       Item.uNextNestLevel != 1) {
        return -2;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_ARRAY ||
       Item.uNestingLevel != 1 ||
       Item.uNextNestLevel != 2) {
        return -3;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_INT64 ||
       Item.uNestingLevel != 2 ||
       Item.uNextNestLevel != 2) {
        return -4;
    }

    QCBORDecode_GetNext(&DC, &Item);
    if(Item.uDataType != QCBOR_TYPE_INT64 ||
       Item.uNestingLevel != 2 ||
       Item.uNextNestLevel != 0) {
        return -5;
    }

    if(QCBORDecode_Finish(&DC)) {
        return -6;
    }

   // --- next test -----
   IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteArrayBad1);

   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_SetMemPool(&DC, MemPool, false);

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -7;
   }

   nResult = QCBORDecode_Finish(&DC);
   if(nResult != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
      return -8;
   }


   // --- next test -----
   IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteArrayBad2);

   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_SetMemPool(&DC, MemPool, false);

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -9;
   }

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -10;
   }

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_INT64) {
      return -11;
   }

   nResult = QCBORDecode_Finish(&DC);
   if(nResult != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
      return -12;
   }


   // --- next test -----
   IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteArrayBad3);

   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_SetMemPool(&DC, MemPool, false);

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -13;
   }

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult != QCBOR_ERR_BAD_BREAK) {
      return -14;
   }


   // --- next test -----
   IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteArrayBad4);

   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_SetMemPool(&DC, MemPool, false);

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -15;
   }

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -16;
   }

   nResult = QCBORDecode_Finish(&DC);
   if(nResult != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
      return -17;
   }

   // --- next test -----
   IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteArrayBad5);

   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_SetMemPool(&DC, MemPool, false);

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult || Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -18;
   }

   nResult = QCBORDecode_GetNext(&DC, &Item);
   if(nResult != QCBOR_ERR_BAD_BREAK) {
      return -19;
   }

    return 0;
}


static const uint8_t spIndefiniteLenString[] = {
   0x81, // Array of length one
   0x7f, // text string marked with indefinite length
   0x65, 0x73, 0x74, 0x72, 0x65, 0x61, // first segment
   0x64, 0x6d, 0x69, 0x6e, 0x67, // second segment
   0xff // ending break
};

static const uint8_t spIndefiniteLenStringBad2[] = {
   0x81, // Array of length one
   0x7f, // text string marked with indefinite length
   0x65, 0x73, 0x74, 0x72, 0x65, 0x61, // first segment
   0x44, 0x6d, 0x69, 0x6e, 0x67, // second segment of wrong type
   0xff // ending break
};

static const uint8_t spIndefiniteLenStringBad3[] = {
   0x81, // Array of length one
   0x7f, // text string marked with indefinite length
   0x01, 0x02, // Not a string
   0xff // ending break
};

static const uint8_t spIndefiniteLenStringBad4[] = {
   0x81, // Array of length one
   0x7f, // text string marked with indefinite length
   0x65, 0x73, 0x74, 0x72, 0x65, 0x61, // first segment
   0x64, 0x6d, 0x69, 0x6e, 0x67, // second segment
   // missing end of string
};

static const uint8_t spIndefiniteLenStringLabel[] = {
   0xa1, // Array of length one
   0x7f, // text string marked with indefinite length
   0x65, 0x73, 0x74, 0x72, 0x75, 0x75, // first segment
   0x64, 0x6d, 0x69, 0x6e, 0x67, // second segment
   0xff, // ending break
   0x01 // integer being labeled.
};

/**
 Make an indefinite length string

 @param Storage Storage for string, must be 144 bytes in size
 @return The indefinite length string

 This makes an array with one indefinite length string that has 7 chunks
 from size of 1 byte up to 64 bytes.
 */
static UsefulBufC MakeIndefiniteBigBstr(UsefulBuf Storage)
{
   UsefulOutBuf UOB;

   UsefulOutBuf_Init(&UOB, Storage);
   UsefulOutBuf_AppendByte(&UOB, 0x81);
   UsefulOutBuf_AppendByte(&UOB, 0x5f);

   uint8_t uStringByte = 0;
   // Use of type int is intentional
   for(int uChunkSize = 1; uChunkSize <= 128; uChunkSize *= 2) {
      // Not using preferred encoding here, but that is OK.
      UsefulOutBuf_AppendByte(&UOB, 0x58);
      UsefulOutBuf_AppendByte(&UOB, (uint8_t)uChunkSize);
      for(int j = 0; j < uChunkSize; j++) {
         UsefulOutBuf_AppendByte(&UOB, uStringByte);
         uStringByte++;
      }
   }
   UsefulOutBuf_AppendByte(&UOB, 0xff);

   return UsefulOutBuf_OutUBuf(&UOB);
}

static int CheckBigString(UsefulBufC BigString)
{
   if(BigString.len != 255) {
      return 1;
   }

   for(uint8_t i = 0; i < 255; i++){
      if(((const uint8_t *)BigString.ptr)[i] != i) {
         return 1;
      }
   }
   return 0;
}


int32_t IndefiniteLengthStringTest()
{
   QCBORDecodeContext DC;
   QCBORItem Item;
   // big enough for MakeIndefiniteBigBstr() + MemPool overhead
   UsefulBuf_MAKE_STACK_UB(MemPool, 350);

   // --- Simple normal indefinite length string ------
   UsefulBufC IndefLen = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteLenString);
   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   if(QCBORDecode_SetMemPool(&DC, MemPool, false)) {
      return -1;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -2;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY || Item.uDataAlloc) {
      return -3;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -4;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING || !Item.uDataAlloc) {
      return -5;
   }
   if(QCBORDecode_Finish(&DC)) {
      return -6;
   }

   // ----- types mismatch ---
   QCBORDecode_Init(&DC,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteLenStringBad2),
                    QCBOR_DECODE_MODE_NORMAL);

   if(QCBORDecode_SetMemPool(&DC,  MemPool, false)) {
      return -7;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -8;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -9;
   }

   if(QCBORDecode_GetNext(&DC, &Item) != QCBOR_ERR_INDEFINITE_STRING_CHUNK) {
      return -10;
   }

   // ----- not a string ---
   QCBORDecode_Init(&DC,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteLenStringBad3),
                    QCBOR_DECODE_MODE_NORMAL);

   if(QCBORDecode_SetMemPool(&DC,  MemPool, false)) {
      return -11;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -12;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -13;
   }

   if(QCBORDecode_GetNext(&DC, &Item) != QCBOR_ERR_INDEFINITE_STRING_CHUNK) {
      return -14;
   }

   // ----- no end -----
   QCBORDecode_Init(&DC,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteLenStringBad4),
                    QCBOR_DECODE_MODE_NORMAL);

   if(QCBORDecode_SetMemPool(&DC,  MemPool, false)) {
      return -15;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -16;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -17;
   }

   if(QCBORDecode_GetNext(&DC, &Item) != QCBOR_ERR_HIT_END) {
      return -18;
   }

   // ------ Don't set a string allocator and see an error -----
   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -19;
   }

   if(QCBORDecode_GetNext(&DC, &Item) != QCBOR_ERR_NO_STRING_ALLOCATOR) {
      return -20;
   }

   // ----- Mempool is way too small -----
   UsefulBuf_MAKE_STACK_UB(MemPoolTooSmall, QCBOR_DECODE_MIN_MEM_POOL_SIZE-1);

   QCBORDecode_Init(&DC, IndefLen, QCBOR_DECODE_MODE_NORMAL);
   if(!QCBORDecode_SetMemPool(&DC,  MemPoolTooSmall, false)) {
      return -21;
   }

   // ----- Mempool is way too small -----
   UsefulBuf_MAKE_STACK_UB(BigIndefBStrStorage, 290);
   const UsefulBufC BigIndefBStr = MakeIndefiniteBigBstr(BigIndefBStrStorage);

   // 80 is big enough for MemPool overhead, but not BigIndefBStr
   UsefulBuf_MAKE_STACK_UB(MemPoolSmall, 80);

   QCBORDecode_Init(&DC, BigIndefBStr, QCBOR_DECODE_MODE_NORMAL);
   if(QCBORDecode_SetMemPool(&DC,  MemPoolSmall, false)) {
      return -22;
   }

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -23;
   }
   if(QCBORDecode_GetNext(&DC, &Item) != QCBOR_ERR_STRING_ALLOCATE) {
      return -24;
   }

   // ---- big bstr -----
   QCBORDecode_Init(&DC, BigIndefBStr, QCBOR_DECODE_MODE_NORMAL);

   if(QCBORDecode_SetMemPool(&DC,  MemPool, false)) {
      return -25;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -26;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY || Item.uDataAlloc) {
      return -26;
   }

   if(QCBORDecode_GetNext(&DC, &Item)) {
      return -27;
   }
   if(Item.uDataType != QCBOR_TYPE_BYTE_STRING || !Item.uDataAlloc || Item.uNestingLevel != 1) {
      return -28;
   }
   if(CheckBigString(Item.val.string)) {
      return -3;
   }
   if(QCBORDecode_Finish(&DC)) {
      return -29;
   }

   // --- label is an indefinite length string ------
   QCBORDecode_Init(&DC, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spIndefiniteLenStringLabel), QCBOR_DECODE_MODE_NORMAL);

   if(QCBORDecode_SetMemPool(&DC,  MemPool, false)) {
      return -30;
   }

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_MAP) {
      return -31;
   }

   if(QCBORDecode_GetNext(&DC, &Item)){
      return -32;
   }
   if(Item.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item.uDataType != QCBOR_TYPE_INT64 ||
      Item.uDataAlloc || !Item.uLabelAlloc ||
      UsefulBuf_Compare(Item.label.string, UsefulBuf_FromSZ("struuming"))) {
      return -33;
   }

   if(QCBORDecode_Finish(&DC)) {
      return -34;
   }

   return 0;
}


int32_t AllocAllStringsTest()
{
   QCBORDecodeContext DC;
   QCBORError nCBORError;


   // First test, use the "CSRMap" as easy input and checking
   QCBORDecode_Init(&DC,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCSRInput),
                    QCBOR_DECODE_MODE_NORMAL);

   UsefulBuf_MAKE_STACK_UB(Pool, sizeof(spCSRInput) + QCBOR_DECODE_MIN_MEM_POOL_SIZE);

   nCBORError = QCBORDecode_SetMemPool(&DC, Pool, 1); // Turn on copying.
   if(nCBORError) {
      return -1;
   }

   if(CheckCSRMaps(&DC)) {
      return -2;
   }

   // Next parse, save pointers to a few strings, destroy original and see all is OK.
   UsefulBuf_MAKE_STACK_UB(CopyOfStorage, sizeof(pValidMapEncoded) + QCBOR_DECODE_MIN_MEM_POOL_SIZE);
   const UsefulBufC CopyOf = UsefulBuf_Copy(CopyOfStorage, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pValidMapEncoded));

   QCBORDecode_Init(&DC, CopyOf, QCBOR_DECODE_MODE_NORMAL);
   UsefulBuf_Set(Pool, '/');
   QCBORDecode_SetMemPool(&DC, Pool, 1); // Turn on copying.

   QCBORItem Item1, Item2, Item3, Item4;
   if((nCBORError = QCBORDecode_GetNext(&DC, &Item1)))
      return nCBORError;
   if(Item1.uDataType != QCBOR_TYPE_MAP ||
      Item1.val.uCount != 3)
      return -3;
   if((nCBORError = QCBORDecode_GetNext(&DC, &Item1)))
      return nCBORError;
   if((nCBORError = QCBORDecode_GetNext(&DC, &Item2)))
      return nCBORError;
   if((nCBORError = QCBORDecode_GetNext(&DC, &Item3)))
      return nCBORError;
   if((nCBORError = QCBORDecode_GetNext(&DC, &Item4)))
      return nCBORError;

   UsefulBuf_Set(CopyOfStorage, '_');

   if(Item1.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      Item1.uDataType != QCBOR_TYPE_INT64 ||
      Item1.val.int64 != 42 ||
      Item1.uDataAlloc != 0 ||
      Item1.uLabelAlloc == 0 ||
      UsefulBuf_Compare(Item1.label.string, UsefulBuf_FromSZ("first integer"))) {
      return -4;
   }


   if(Item2.uLabelType != QCBOR_TYPE_TEXT_STRING ||
      UsefulBuf_Compare(Item2.label.string, UsefulBuf_FromSZ("an array of two strings")) ||
      Item2.uDataType != QCBOR_TYPE_ARRAY ||
      Item2.uDataAlloc != 0 ||
      Item2.uLabelAlloc == 0 ||
      Item2.val.uCount != 2)
      return -5;

   if(Item3.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item3.uDataAlloc == 0 ||
      Item3.uLabelAlloc != 0 ||
      UsefulBuf_Compare(Item3.val.string, UsefulBuf_FromSZ("string1"))) {
      return -6;
   }

   if(Item4.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item4.uDataAlloc == 0 ||
      Item4.uLabelAlloc != 0 ||
      UsefulBuf_Compare(Item4.val.string, UsefulBuf_FromSZ("string2"))) {
      return -7;
   }

   // Next parse with a pool that is too small
   UsefulBuf_MAKE_STACK_UB(SmallPool, QCBOR_DECODE_MIN_MEM_POOL_SIZE + 1);
   QCBORDecode_Init(&DC,
                    UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pValidMapEncoded),
                    QCBOR_DECODE_MODE_NORMAL);
   QCBORDecode_SetMemPool(&DC, SmallPool, 1); // Turn on copying.
   if((nCBORError = QCBORDecode_GetNext(&DC, &Item1)))
      return -8;
   if(Item1.uDataType != QCBOR_TYPE_MAP ||
      Item1.val.uCount != 3) {
      return -9;
   }
   if(!(nCBORError = QCBORDecode_GetNext(&DC, &Item1))){
      if(!(nCBORError = QCBORDecode_GetNext(&DC, &Item2))) {
         if(!(nCBORError = QCBORDecode_GetNext(&DC, &Item3))) {
            nCBORError = QCBORDecode_GetNext(&DC, &Item4);
         }
      }
   }
   if(nCBORError != QCBOR_ERR_STRING_ALLOCATE) {
      return -10;
   }

   return 0;
}



int32_t MemPoolTest(void)
{
   // Set up the decoder with a tiny bit of CBOR to parse because
   // nothing can be done with it unless that is set up.
   QCBORDecodeContext DC;
   const uint8_t pMinimalCBOR[] = {0xa0}; // One empty map
   QCBORDecode_Init(&DC, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pMinimalCBOR),
                    QCBOR_DECODE_MODE_NORMAL);

   // Set up an memory pool of 100 bytes
   // Then fish into the internals of the decode context
   // to get the allocator function so it can be called directly.
   // Also figure out how much pool is available for use
   // buy subtracting out the overhead.
   UsefulBuf_MAKE_STACK_UB(Pool, 100);
   QCBORError nError = QCBORDecode_SetMemPool(&DC, Pool, 0);
   if(nError) {
      return -9;
   }
   QCBORStringAllocate pAlloc = DC.StringAllocator.pfAllocator;
   void *pAllocCtx            = DC.StringAllocator.pAllocateCxt;
   size_t uAvailPool = Pool.len - QCBOR_DECODE_MIN_MEM_POOL_SIZE;

   // First test -- ask for one more byte than available and see failure
   UsefulBuf Allocated = (*pAlloc)(pAllocCtx, NULL, uAvailPool+1);
   if(!UsefulBuf_IsNULL(Allocated)) {
      return -1;
   }

   // Re do the set up for the next test that will do a successful alloc,
   // a fail, a free and then success
   QCBORDecode_SetMemPool(&DC, Pool, 0);
   pAlloc    = DC.StringAllocator.pfAllocator;
   pAllocCtx = DC.StringAllocator.pAllocateCxt;
   uAvailPool = Pool.len - QCBOR_DECODE_MIN_MEM_POOL_SIZE;

   // Allocate one byte less than available and see success
   Allocated = (pAlloc)(pAllocCtx, NULL, uAvailPool-1);
   if(UsefulBuf_IsNULL(Allocated)) { // expected to succeed
      return -2;
   }
   // Ask for some more and see failure
   UsefulBuf Allocated2 = (*pAlloc)(pAllocCtx, NULL, uAvailPool/2);
   if(!UsefulBuf_IsNULL(Allocated2)) { // expected to fail
      return -3;
   }
   // Free the first allocate, retry the second and see success
   (*pAlloc)(pAllocCtx, Allocated.ptr, 0); // Free
   Allocated = (*pAlloc)(pAllocCtx, NULL, uAvailPool/2);
   if(UsefulBuf_IsNULL(Allocated)) { // succeed because of the free
      return -4;
   }

   // Re do set up for next test that involves a successful alloc,
   // and a successful realloc and a failed realloc
   QCBORDecode_SetMemPool(&DC, Pool, 0);
   pAlloc    = DC.StringAllocator.pfAllocator;
   pAllocCtx = DC.StringAllocator.pAllocateCxt;

   // Allocate half the pool and see success
   Allocated = (*pAlloc)(pAllocCtx, NULL, uAvailPool/2);
   if(UsefulBuf_IsNULL(Allocated)) { // expected to succeed
      return -5;
   }
   // Reallocate to take up the whole pool and see success
   Allocated2 = (*pAlloc)(pAllocCtx, Allocated.ptr, uAvailPool);
   if(UsefulBuf_IsNULL(Allocated2)) {
      return -6;
   }
   // Make sure its the same pointer and the size is right
   if(Allocated2.ptr != Allocated.ptr || Allocated2.len != uAvailPool) {
      return -7;
   }
   // Try to allocate more to be sure there is failure after a realloc
   UsefulBuf Allocated3 = (*pAlloc)(pAllocCtx, Allocated.ptr, uAvailPool+1);
   if(!UsefulBuf_IsNULL(Allocated3)) {
      return -8;
   }

   return 0;
}


/* Just enough of an allocator to test configuration of one */
static UsefulBuf AllocateTestFunction(void *pCtx, void *pOldMem, size_t uNewSize)
{
   (void)pOldMem; // unused variable

   if(uNewSize) {
      // Assumes the context pointer is the buffer and
      // nothing too big will ever be asked for.
      // This is only good for this basic test!
      return (UsefulBuf) {pCtx, uNewSize};
   } else {
      return NULLUsefulBuf;
   }
}


int32_t SetUpAllocatorTest(void)
{
   // Set up the decoder with a tiny bit of CBOR to parse because
   // nothing can be done with it unless that is set up.
   QCBORDecodeContext DC;
   const uint8_t pMinimalCBOR[] = {0x62, 0x48, 0x69}; // "Hi"
   QCBORDecode_Init(&DC, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pMinimalCBOR),
                    QCBOR_DECODE_MODE_NORMAL);

   uint8_t pAllocatorBuffer[50];

   // This is really just to test that this call works.
   // The full functionality of string allocators is tested
   // elsewhere with the MemPool internal allocator.
   QCBORDecode_SetUpAllocator(&DC, AllocateTestFunction, pAllocatorBuffer, 1);

   QCBORItem Item;
   if(QCBORDecode_GetNext(&DC, &Item) != QCBOR_SUCCESS) {
      return -1;
   }

   if(Item.uDataAlloc == 0 ||
      Item.uDataType != QCBOR_TYPE_TEXT_STRING ||
      Item.val.string.ptr != pAllocatorBuffer) {
      return -2;
   }

   if(QCBORDecode_Finish(&DC) != QCBOR_SUCCESS) {
      return -3;
   }

   return 0;
}


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
/*
  [
    4([-1, 3]),
    4([-20, 4759477275222530853136]),
    4([9223372036854775807, -4759477275222530853137]),
    5([300, 100]),
    5([-20, 4759477275222530853136]),
    5([-9223372036854775807, -4759477275222530853137])
    5([9223372036854775806, -4759477275222530853137])
    5([9223372036854775806, 9223372036854775806])]
 ]
 */

static const uint8_t spExpectedExponentsAndMantissas[] = {
   0x87,
   0xC4, 0x82, 0x20,
               0x03,
   0xC4, 0x82, 0x33,
               0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
   0xC4, 0x82, 0x1B, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0xC3, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
   0xC5, 0x82, 0x19, 0x01, 0x2C,
               0x18, 0x64,
   0xC5, 0x82, 0x33,
               0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
   0xC5, 0x82, 0x3B, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
               0xC3, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
   0xC5, 0x82, 0x1B, 0x7f, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
               0x1B, 0x7f, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE
};

int32_t ExponentAndMantissaDecodeTests(void)
{
   QCBORDecodeContext DC;
   QCBORError         nCBORError;
   QCBORItem          item;

   static const uint8_t spBigNumMantissa[] = {0x01, 0x02, 0x03, 0x04, 0x05,
                                              0x06, 0x07, 0x08, 0x09, 0x010};
   UsefulBufC BN = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNumMantissa);


   QCBORDecode_Init(&DC, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spExpectedExponentsAndMantissas), QCBOR_DECODE_MODE_NORMAL);

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 1;
   }

   if(item.uDataType != QCBOR_TYPE_ARRAY) {
      return 2;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 3;
   }

   if(item.uDataType != QCBOR_TYPE_DECIMAL_FRACTION ||
      item.val.expAndMantissa.Mantissa.nInt != 3 ||
      item.val.expAndMantissa.nExponent != -1) {
      return 4;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 5;
   }

   if(item.uDataType != QCBOR_TYPE_DECIMAL_FRACTION_POS_BIGNUM ||
      item.val.expAndMantissa.nExponent != -20 ||
      UsefulBuf_Compare(item.val.expAndMantissa.Mantissa.bigNum, BN)) {
      return 6;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 7;
   }

   if(item.uDataType != QCBOR_TYPE_DECIMAL_FRACTION_NEG_BIGNUM ||
      item.val.expAndMantissa.nExponent != 9223372036854775807 ||
      UsefulBuf_Compare(item.val.expAndMantissa.Mantissa.bigNum, BN)) {
      return 8;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 9;
   }

   if(item.uDataType != QCBOR_TYPE_BIGFLOAT ||
      item.val.expAndMantissa.Mantissa.nInt != 100 ||
      item.val.expAndMantissa.nExponent != 300) {
      return 10;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 11;
   }

   if(item.uDataType != QCBOR_TYPE_BIGFLOAT_POS_BIGNUM ||
      item.val.expAndMantissa.nExponent != -20 ||
      UsefulBuf_Compare(item.val.expAndMantissa.Mantissa.bigNum, BN)) {
      return 12;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 13;
   }

   if(item.uDataType != QCBOR_TYPE_BIGFLOAT_NEG_BIGNUM ||
      item.val.expAndMantissa.nExponent != -9223372036854775807 ||
      UsefulBuf_Compare(item.val.expAndMantissa.Mantissa.bigNum, BN)) {
      return 14;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 15;
   }

   if(item.uDataType != QCBOR_TYPE_BIGFLOAT ||
      item.val.expAndMantissa.nExponent != 9223372036854775806 ||
      item.val.expAndMantissa.Mantissa.nInt!= 9223372036854775806 ) {
      return 16;
   }

   /* Now encode some stuff and then decode it */
   uint8_t pBuf[40];
   QCBOREncodeContext EC;
   UsefulBufC Encoded;

   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(pBuf));
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddDecimalFraction(&EC, 999, 1000); // 999 * (10 ^ 1000)
   QCBOREncode_AddBigFloat(&EC, 100, INT32_MIN);
   QCBOREncode_AddDecimalFractionBigNum(&EC, BN, false, INT32_MAX);
   QCBOREncode_CloseArray(&EC);
   QCBOREncode_Finish(&EC, &Encoded);


   QCBORDecode_Init(&DC, Encoded, QCBOR_DECODE_MODE_NORMAL);
   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 13;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 13;
   }

   if(item.uDataType != QCBOR_TYPE_DECIMAL_FRACTION ||
      item.val.expAndMantissa.nExponent != 1000 ||
      item.val.expAndMantissa.Mantissa.nInt != 999) {
      return 15;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 13;
   }

   if(item.uDataType != QCBOR_TYPE_BIGFLOAT ||
      item.val.expAndMantissa.nExponent != INT32_MIN ||
      item.val.expAndMantissa.Mantissa.nInt != 100) {
      return 15;
   }

   nCBORError = QCBORDecode_GetNext(&DC, &item);
   if(nCBORError != QCBOR_SUCCESS) {
      return 13;
   }

   if(item.uDataType != QCBOR_TYPE_DECIMAL_FRACTION_POS_BIGNUM ||
      item.val.expAndMantissa.nExponent != INT32_MAX ||
      UsefulBuf_Compare(item.val.expAndMantissa.Mantissa.bigNum, BN)) {
      return 12;
   }

   return 0;
}


static struct FailInput ExponentAndMantissaFailures[] = {
   // Exponent > INT64_MAX
   { {(uint8_t[]){0xC4, 0x82, 0x1B, 0x7f, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                  0xFF, 0xFF, 0x1B, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                  0xFF, 0xFF,}, 20}, QCBOR_ERR_BAD_EXP_AND_MANTISSA},
   // Mantissa > INT64_MAX
   { {(uint8_t[]){0xC4, 0x82, 0x1B, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                  0xFF, 0xFF, 0xC3, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05,
                  0x06, 0x07, 0x08, 0x09, 0x10}, 23}, QCBOR_ERR_BAD_EXP_AND_MANTISSA},
   // End of input
   { {(uint8_t[]){0xC4, 0x82}, 2}, QCBOR_ERR_HIT_END},
   // End of input
   { {(uint8_t[]){0xC4, 0x82, 0x01}, 3}, QCBOR_ERR_HIT_END},
   // bad content for big num
   { {(uint8_t[]){0xC4, 0x82, 0x01, 0xc3, 0x01}, 5}, QCBOR_ERR_BAD_OPT_TAG},
   // bad content for big num
   { {(uint8_t[]){0xC4, 0x82, 0xc2, 0x01, 0x1f}, 5}, QCBOR_ERR_BAD_INT},
   // Bad integer for exponent
   { {(uint8_t[]){0xC4, 0x82, 0x01, 0x1f}, 4}, QCBOR_ERR_BAD_INT},
   // Bad integer for mantissa
   { {(uint8_t[]){0xC4, 0x82, 0x1f, 0x01}, 4}, QCBOR_ERR_BAD_INT},
   // 3 items in array
   { {(uint8_t[]){0xC4, 0x83, 0x03, 0x01, 02}, 5}, QCBOR_ERR_BAD_EXP_AND_MANTISSA},
   // unterminated indefinite length array
   { {(uint8_t[]){0xC4, 0x9f, 0x03, 0x01, 0x02}, 5}, QCBOR_ERR_BAD_EXP_AND_MANTISSA},
   // Empty array
   { {(uint8_t[]){0xC4, 0x80}, 2}, QCBOR_ERR_NO_MORE_ITEMS},
   // Second is not an integer
   { {(uint8_t[]){0xC4, 0x82, 0x03, 0x40}, 4}, QCBOR_ERR_BAD_EXP_AND_MANTISSA},
   // First is not an integer
   { {(uint8_t[]){0xC4, 0x82, 0x40}, 3}, QCBOR_ERR_BAD_EXP_AND_MANTISSA},
   // Not an array
   { {(uint8_t[]){0xC4, 0xa2}, 2}, QCBOR_ERR_BAD_EXP_AND_MANTISSA}
};


int32_t ExponentAndMantissaDecodeFailTests()
{
   return ProcessFailures(ExponentAndMantissaFailures,
                          sizeof(ExponentAndMantissaFailures)/sizeof(struct FailInput));
}

#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */
