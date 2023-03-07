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

#include "qcbor.h"
#include "qcbor_encode_tests.h"


/*
 This is the test set for CBOR encoding.

 This is largely complete for the implemented.

 A few more things to do include:
   - Add a test for counting the top level items and adding it back in with AddRaw()
   - Run on some different CPUs like 32-bit and maybe even 16-bit
   - Test the large array count limit
   - Add the CBOR diagnostic output for every expected

 */

//#define PRINT_FUNCTIONS_FOR_DEBUGGING

#ifdef  PRINT_FUNCTIONS_FOR_DEBUGGING
#include <stdio.h>

#if 0
// ifdef these out to not have compiler warnings
static void printencoded(const uint8_t *pEncoded, size_t nLen)
{
   size_t i;
   for(i = 0; i < nLen; i++) {
      uint8_t Z = pEncoded[i];
      printf("%02x ", Z);
   }
   printf("\n");

   fflush(stdout);
}
#endif


// Do the comparison and print out where it fails
static int UsefulBuf_Compare_Print(UsefulBufC U1, UsefulBufC U2) {
   size_t i;
   for(i = 0; i < U1.len; i++) {
      if(((uint8_t *)U1.ptr)[i] != ((uint8_t *)U2.ptr)[i]) {
         printf("Position: %d  Actual: 0x%x   Expected: 0x%x\n",
                (uint32_t)i,
                ((uint8_t *)U1.ptr)[i],
                ((uint8_t *)U2.ptr)[i]);
         return 1;
      }
   }
   return 0;

}

#define CheckResults(Enc, Expected) \
   UsefulBuf_Compare_Print(Enc, (UsefulBufC){Expected, sizeof(Expected)})

#else

#define CheckResults(Enc, Expected) \
   UsefulBuf_Compare(Enc, (UsefulBufC){Expected, sizeof(Expected)})

#endif


#ifndef QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA
/*
 Returns 0 if UsefulBufs are equal
 Returns 1000000 + offeset if they are not equal.



*/
struct UBCompareDiagnostic {
   uint8_t uActual;
   uint8_t uExpected;
   size_t  uOffset;
};

static int32_t
UsefulBuf_CompareWithDiagnostic(UsefulBufC Actual,
                                UsefulBufC Expected,
                                struct UBCompareDiagnostic *pDiag) {
   size_t i;
   for(i = 0; i < Actual.len; i++) {
      if(((uint8_t *)Actual.ptr)[i] != ((uint8_t *)Expected.ptr)[i]) {
         if(pDiag) {
            pDiag->uActual   = ((uint8_t *)Actual.ptr)[i];
            pDiag->uExpected = ((uint8_t *)Expected.ptr)[i];
            pDiag->uOffset   = i;
         }
         // Cast to int is OK as this is only a diagnostic and the sizes
         // here are never over a few KB.
         return (int32_t)i + 1000000;
      }
   }
   return 0;

}
#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */


// One big buffer that is used by all the tests to encode into
// Putting it in uninitialized data is better than using a lot
// of stack. The tests should run on small devices too.
static uint8_t spBigBuf[2200];



/*
 Some very minimal tests.
 */
int32_t BasicEncodeTest()
{
   // Very simple CBOR, a map with one boolean that is true in it
   QCBOREncodeContext EC;

   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   QCBOREncode_OpenMap(&EC);
   QCBOREncode_AddBoolToMapN(&EC, 66, true);
   QCBOREncode_CloseMap(&EC);

   UsefulBufC Encoded;
   if(QCBOREncode_Finish(&EC, &Encoded)) {
      return -1;
   }


   // Decode it and see that is right
   QCBORDecodeContext DC;
   QCBORItem Item;
   QCBORDecode_Init(&DC, Encoded, QCBOR_DECODE_MODE_NORMAL);

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_MAP) {
      return -2;
   }

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_TRUE) {
      return -3;
   }

   if(QCBORDecode_Finish(&DC)) {
      return -4;
   }


   // Make another encoded message with the CBOR from the previous
   // put into this one
   UsefulBuf_MAKE_STACK_UB(MemoryForEncoded2, 20);
   QCBOREncode_Init(&EC, MemoryForEncoded2);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddUInt64(&EC, 451);
   QCBOREncode_AddEncoded(&EC, Encoded);
   QCBOREncode_OpenMap(&EC);
   QCBOREncode_AddEncodedToMapN(&EC, -70000, Encoded);
   QCBOREncode_CloseMap(&EC);
   QCBOREncode_CloseArray(&EC);

   UsefulBufC Encoded2;
   if(QCBOREncode_Finish(&EC, &Encoded2)) {
      return -5;
   }
    /*
     [                // 0    1:3
        451,          // 1    1:2
        {             // 1    1:2   2:1
          66: true    // 2    1:1
        },
        {             // 1    1:1   2:1
          -70000: {   // 2    1:1   2:1   3:1
            66: true  // 3    XXXXXX
          }
        }
     ]



      83                # array(3)
         19 01C3        # unsigned(451)
         A1             # map(1)
            18 42       # unsigned(66)
            F5          # primitive(21)
         A1             # map(1)
            3A 0001116F # negative(69999)
            A1          # map(1)
               18 42    # unsigned(66)
               F5       # primitive(21)
     */

   // Decode it and see if it is OK
   QCBORDecode_Init(&DC, Encoded2, QCBOR_DECODE_MODE_NORMAL);

   // 0    1:3
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_ARRAY || Item.val.uCount != 3) {
      return -6;
   }

   // 1    1:2
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_INT64 || Item.val.uint64 != 451) {
      return -7;
   }

   // 1    1:2   2:1
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_MAP || Item.val.uCount != 1) {
      return -8;
   }

   // 2    1:1
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_TRUE) {
      return -9;
   }

   // 1    1:1   2:1
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_MAP || Item.val.uCount != 1) {
      return -10;
   }

   // 2    1:1   2:1   3:1
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_MAP ||
      Item.val.uCount != 1 ||
      Item.uLabelType != QCBOR_TYPE_INT64 ||
      Item.label.int64 != -70000) {
      return -11;
   }

   // 3    XXXXXX
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_TRUE || Item.uLabelType != QCBOR_TYPE_INT64 || Item.label.int64 != 66) {
      return -12;
   }

   if(QCBORDecode_Finish(&DC)) {
      return -13;
   }

   return 0;
}



static const uint8_t spExpectedEncodedAll[] = {
 0x98, 0x22, 0x66, 0x55, 0x49, 0x4e, 0x54, 0x36, 0x32, 0xd8,
 0x64, 0x1a, 0x05, 0x5d, 0x23, 0x15, 0x65, 0x49, 0x4e, 0x54,
 0x36, 0x34, 0xd8, 0x4c, 0x1b, 0x00, 0x00, 0x00, 0x12, 0x16,
 0xaf, 0x2b, 0x15, 0x00, 0x38, 0x2b, 0xa4, 0x63, 0x4c, 0x42,
 0x4c, 0x18, 0x4d, 0x23, 0x18, 0x58, 0x78, 0x1a, 0x4e, 0x45,
 0x47, 0x4c, 0x42, 0x4c, 0x54, 0x48, 0x41, 0x54, 0x20, 0x49,
 0x53, 0x20, 0x4b, 0x49, 0x4e, 0x44, 0x20, 0x4f, 0x46, 0x20,
 0x4c, 0x4f, 0x4e, 0x47, 0x3b, 0x00, 0x00, 0x02, 0x2d, 0x9a,
 0xc6, 0x94, 0x55, 0x3a, 0x05, 0xf5, 0xe0, 0xff, 0x3a, 0x2f,
 0xaf, 0x07, 0xff, 0xc1, 0x1a, 0x8e, 0x15, 0x1c, 0x8a,
 0xa3, 0x74, 0x4c, 0x6f, 0x6e, 0x67, 0x4c, 0x69, 0x76, 0x65,
 0x44, 0x65, 0x6e, 0x69, 0x73, 0x52, 0x69, 0x74, 0x63, 0x68,
 0x69, 0x65, 0xc1, 0x1a, 0x53, 0x72, 0x4e, 0x00, 0x66, 0x74,
 0x69, 0x6d, 0x65, 0x28, 0x29, 0xc1, 0x1a, 0x58, 0x0d, 0x41,
 0x72, 0x39, 0x07, 0xb0, 0xc1, 0x1a, 0x58, 0x0d, 0x3f, 0x76,
 0x42, 0xff, 0x00, 0xa3, 0x66, 0x62, 0x69, 0x6e, 0x62, 0x69,
 0x6e, 0xda, 0x00, 0x01, 0x86, 0xa0, 0x41, 0x00, 0x66, 0x62,
 0x6c, 0x61, 0x62, 0x65, 0x6c, 0x43, 0x01, 0x02, 0x03, 0x00,
 0x44, 0x04, 0x02, 0x03, 0xfe, 0x6f, 0x62, 0x61, 0x72, 0x20,
 0x62, 0x61, 0x72, 0x20, 0x66, 0x6f, 0x6f, 0x20, 0x62, 0x61,
 0x72, 0x64, 0x6f, 0x6f, 0x66, 0x0a, 0xd8, 0x20, 0x78, 0x6b,
 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x73, 0x74, 0x61,
 0x63, 0x6b, 0x6f, 0x76, 0x65, 0x72, 0x66, 0x6c, 0x6f, 0x77,
 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x71, 0x75, 0x65, 0x73, 0x74,
 0x69, 0x6f, 0x6e, 0x73, 0x2f, 0x32, 0x38, 0x30, 0x35, 0x39,
 0x36, 0x39, 0x37, 0x2f, 0x68, 0x6f, 0x77, 0x2d, 0x64, 0x6f,
 0x2d, 0x69, 0x2d, 0x74, 0x6f, 0x67, 0x67, 0x6c, 0x65, 0x2d,
 0x62, 0x65, 0x74, 0x77, 0x65, 0x65, 0x6e, 0x2d, 0x64, 0x65,
 0x62, 0x75, 0x67, 0x2d, 0x61, 0x6e, 0x64, 0x2d, 0x72, 0x65,
 0x6c, 0x65, 0x61, 0x73, 0x65, 0x2d, 0x62, 0x75, 0x69, 0x6c,
 0x64, 0x73, 0x2d, 0x69, 0x6e, 0x2d, 0x78, 0x63, 0x6f, 0x64,
 0x65, 0x2d, 0x36, 0x2d, 0x37, 0x2d, 0x38, 0xd8, 0x22, 0x78,
 0x1c, 0x59, 0x57, 0x35, 0x35, 0x49, 0x47, 0x4e, 0x68, 0x63,
 0x6d, 0x35, 0x68, 0x62, 0x43, 0x42, 0x77, 0x62, 0x47, 0x56,
 0x68, 0x63, 0x33, 0x56, 0x79, 0x5a, 0x51, 0x3d, 0x3d, 0xd8,
 0x23, 0x67, 0x5b, 0x5e, 0x61, 0x62, 0x63, 0x5d, 0x2b, 0xd8,
 0x24, 0x79, 0x01, 0x57, 0x4d, 0x49, 0x4d, 0x45, 0x2d, 0x56,
 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x31, 0x2e,
 0x30, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d,
 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x6d, 0x75, 0x6c, 0x74,
 0x69, 0x70, 0x61, 0x72, 0x74, 0x2f, 0x6d, 0x69, 0x78, 0x65,
 0x64, 0x3b, 0x0a, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72,
 0x79, 0x3d, 0x22, 0x58, 0x58, 0x58, 0x58, 0x62, 0x6f, 0x75,
 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20, 0x74, 0x65, 0x78, 0x74,
 0x22, 0x0a, 0x0a, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73,
 0x20, 0x61, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x61,
 0x72, 0x74, 0x20, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65,
 0x20, 0x69, 0x6e, 0x20, 0x4d, 0x49, 0x4d, 0x45, 0x20, 0x66,
 0x6f, 0x72, 0x6d, 0x61, 0x74, 0x2e, 0x0a, 0x0a, 0x2d, 0x2d,
 0x58, 0x58, 0x58, 0x58, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61,
 0x72, 0x79, 0x20, 0x74, 0x65, 0x78, 0x74, 0x0a, 0x43, 0x6f,
 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65,
 0x3a, 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61,
 0x69, 0x6e, 0x0a, 0x0a, 0x74, 0x68, 0x69, 0x73, 0x20, 0x69,
 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x62, 0x6f, 0x64, 0x79,
 0x20, 0x74, 0x65, 0x78, 0x74, 0x0a, 0x0a, 0x2d, 0x2d, 0x58,
 0x58, 0x58, 0x58, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72,
 0x79, 0x20, 0x74, 0x65, 0x78, 0x74, 0x0a, 0x43, 0x6f, 0x6e,
 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a,
 0x20, 0x74, 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69,
 0x6e, 0x3b, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74,
 0x2d, 0x44, 0x69, 0x73, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69,
 0x6f, 0x6e, 0x3a, 0x20, 0x61, 0x74, 0x74, 0x61, 0x63, 0x68,
 0x6d, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x66, 0x69, 0x6c, 0x65,
 0x6e, 0x61, 0x6d, 0x65, 0x3d, 0x22, 0x74, 0x65, 0x73, 0x74,
 0x2e, 0x74, 0x78, 0x74, 0x22, 0x0a, 0x0a, 0x74, 0x68, 0x69,
 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x61,
 0x74, 0x74, 0x61, 0x63, 0x68, 0x6d, 0x65, 0x6e, 0x74, 0x20,
 0x74, 0x65, 0x78, 0x74, 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58,
 0x58, 0x58, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79,
 0x20, 0x74, 0x65, 0x78, 0x74, 0x2d, 0x2d, 0xae, 0x65, 0x23,
 0x23, 0x23, 0x23, 0x23, 0x6f, 0x66, 0x6f, 0x6f, 0x20, 0x62,
 0x61, 0x72, 0x20, 0x66, 0x6f, 0x6f, 0x20, 0x66, 0x6f, 0x6f,
 0x64, 0x5f, 0x5f, 0x5f, 0x5f, 0x67, 0x66, 0x6f, 0x6f, 0x20,
 0x62, 0x61, 0x72, 0x66, 0x28, 0x29, 0x28, 0x29, 0x28, 0x29,
 0xd9, 0x03, 0xe8, 0x6b, 0x72, 0x61, 0x62, 0x20, 0x72, 0x61,
 0x62, 0x20, 0x6f, 0x6f, 0x66, 0x16, 0x6f, 0x66, 0x6f, 0x6f,
 0x20, 0x66, 0x6f, 0x6f, 0x20, 0x66, 0x6f, 0x6f, 0x20, 0x66,
 0x6f, 0x6f, 0x62, 0x5e, 0x5e, 0x69, 0x6f, 0x6f, 0x6f, 0x6f,
 0x6f, 0x6f, 0x6f, 0x6f, 0x66, 0x18, 0x63, 0x6d, 0x66, 0x66,
 0x66, 0x66, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f,
 0x66, 0x63, 0x52, 0x46, 0x43, 0xd8, 0x20, 0x78, 0x31, 0x68,
 0x74, 0x74, 0x70, 0x73, 0x3a, 0x2f, 0x2f, 0x74, 0x6f, 0x6f,
 0x6c, 0x73, 0x2e, 0x69, 0x65, 0x74, 0x66, 0x2e, 0x6f, 0x72,
 0x67, 0x2f, 0x68, 0x74, 0x6d, 0x6c, 0x2f, 0x72, 0x66, 0x63,
 0x37, 0x30, 0x34, 0x39, 0x23, 0x73, 0x65, 0x63, 0x74, 0x69,
 0x6f, 0x6e, 0x2d, 0x32, 0x2e, 0x34, 0x2e, 0x35, 0x18, 0x89,
 0xd8, 0x20, 0x6f, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f,
 0x63, 0x62, 0x6f, 0x72, 0x2e, 0x6d, 0x65, 0x2f, 0x68, 0x77,
 0x68, 0x65, 0x6e, 0x69, 0x6d, 0x36, 0x34, 0xd8, 0x22, 0x6c,
 0x63, 0x47, 0x78, 0x6c, 0x59, 0x58, 0x4e, 0x31, 0x63, 0x6d,
 0x55, 0x75, 0x18, 0x40, 0xd8, 0x22, 0x68, 0x63, 0x33, 0x56,
 0x79, 0x5a, 0x53, 0x34, 0x3d, 0x64, 0x70, 0x6f, 0x70, 0x6f,
 0xd8, 0x23, 0x68, 0x31, 0x30, 0x30, 0x5c, 0x73, 0x2a, 0x6d,
 0x6b, 0x38, 0x32, 0xd8, 0x23, 0x66, 0x70, 0x65, 0x72, 0x6c,
 0x5c, 0x42, 0x63, 0x4e, 0x65, 0x64, 0xd8, 0x24, 0x79, 0x01,
 0x57, 0x4d, 0x49, 0x4d, 0x45, 0x2d, 0x56, 0x65, 0x72, 0x73,
 0x69, 0x6f, 0x6e, 0x3a, 0x20, 0x31, 0x2e, 0x30, 0x0a, 0x43,
 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70,
 0x65, 0x3a, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x61,
 0x72, 0x74, 0x2f, 0x6d, 0x69, 0x78, 0x65, 0x64, 0x3b, 0x0a,
 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x3d, 0x22,
 0x58, 0x58, 0x58, 0x58, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61,
 0x72, 0x79, 0x20, 0x74, 0x65, 0x78, 0x74, 0x22, 0x0a, 0x0a,
 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20,
 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x61, 0x72, 0x74, 0x20,
 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x20, 0x69, 0x6e,
 0x20, 0x4d, 0x49, 0x4d, 0x45, 0x20, 0x66, 0x6f, 0x72, 0x6d,
 0x61, 0x74, 0x2e, 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58, 0x58,
 0x58, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20,
 0x74, 0x65, 0x78, 0x74, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65,
 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74,
 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0x0a,
 0x0a, 0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74,
 0x68, 0x65, 0x20, 0x62, 0x6f, 0x64, 0x79, 0x20, 0x74, 0x65,
 0x78, 0x74, 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58, 0x58, 0x58,
 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20, 0x74,
 0x65, 0x78, 0x74, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e,
 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65,
 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0x3b, 0x0a,
 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x44, 0x69,
 0x73, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3a,
 0x20, 0x61, 0x74, 0x74, 0x61, 0x63, 0x68, 0x6d, 0x65, 0x6e,
 0x74, 0x3b, 0x0a, 0x66, 0x69, 0x6c, 0x65, 0x6e, 0x61, 0x6d,
 0x65, 0x3d, 0x22, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x74, 0x78,
 0x74, 0x22, 0x0a, 0x0a, 0x74, 0x68, 0x69, 0x73, 0x20, 0x69,
 0x73, 0x20, 0x74, 0x68, 0x65, 0x20, 0x61, 0x74, 0x74, 0x61,
 0x63, 0x68, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x74, 0x65, 0x78,
 0x74, 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58, 0x58, 0x58, 0x62,
 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20, 0x74, 0x65,
 0x78, 0x74, 0x2d, 0x2d, 0x0a, 0xd8, 0x24, 0x79, 0x01, 0x57,
 0x4d, 0x49, 0x4d, 0x45, 0x2d, 0x56, 0x65, 0x72, 0x73, 0x69,
 0x6f, 0x6e, 0x3a, 0x20, 0x31, 0x2e, 0x30, 0x0a, 0x43, 0x6f,
 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65,
 0x3a, 0x20, 0x6d, 0x75, 0x6c, 0x74, 0x69, 0x70, 0x61, 0x72,
 0x74, 0x2f, 0x6d, 0x69, 0x78, 0x65, 0x64, 0x3b, 0x0a, 0x62,
 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x3d, 0x22, 0x58,
 0x58, 0x58, 0x58, 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72,
 0x79, 0x20, 0x74, 0x65, 0x78, 0x74, 0x22, 0x0a, 0x0a, 0x54,
 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x6d,
 0x75, 0x6c, 0x74, 0x69, 0x70, 0x61, 0x72, 0x74, 0x20, 0x6d,
 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x20, 0x69, 0x6e, 0x20,
 0x4d, 0x49, 0x4d, 0x45, 0x20, 0x66, 0x6f, 0x72, 0x6d, 0x61,
 0x74, 0x2e, 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58, 0x58, 0x58,
 0x62, 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20, 0x74,
 0x65, 0x78, 0x74, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e,
 0x74, 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65,
 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0x0a, 0x0a,
 0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x74, 0x68,
 0x65, 0x20, 0x62, 0x6f, 0x64, 0x79, 0x20, 0x74, 0x65, 0x78,
 0x74, 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58, 0x58, 0x58, 0x62,
 0x6f, 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20, 0x74, 0x65,
 0x78, 0x74, 0x0a, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74,
 0x2d, 0x54, 0x79, 0x70, 0x65, 0x3a, 0x20, 0x74, 0x65, 0x78,
 0x74, 0x2f, 0x70, 0x6c, 0x61, 0x69, 0x6e, 0x3b, 0x0a, 0x43,
 0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x44, 0x69, 0x73,
 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3a, 0x20,
 0x61, 0x74, 0x74, 0x61, 0x63, 0x68, 0x6d, 0x65, 0x6e, 0x74,
 0x3b, 0x0a, 0x66, 0x69, 0x6c, 0x65, 0x6e, 0x61, 0x6d, 0x65,
 0x3d, 0x22, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x74, 0x78, 0x74,
 0x22, 0x0a, 0x0a, 0x74, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73,
 0x20, 0x74, 0x68, 0x65, 0x20, 0x61, 0x74, 0x74, 0x61, 0x63,
 0x68, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x74, 0x65, 0x78, 0x74,
 0x0a, 0x0a, 0x2d, 0x2d, 0x58, 0x58, 0x58, 0x58, 0x62, 0x6f,
 0x75, 0x6e, 0x64, 0x61, 0x72, 0x79, 0x20, 0x74, 0x65, 0x78,
 0x74, 0x2d, 0x2d, 0xc0, 0x74, 0x32, 0x30, 0x30, 0x33, 0x2d,
 0x31, 0x32, 0x2d, 0x31, 0x33, 0x54, 0x31, 0x38, 0x3a, 0x33,
 0x30, 0x3a, 0x30, 0x32, 0x5a, 0xa2, 0x68, 0x42, 0x65, 0x64,
 0x20, 0x74, 0x69, 0x6d, 0x65, 0xc0, 0x78, 0x1c, 0x32, 0x30,
 0x30, 0x33, 0x2d, 0x31, 0x32, 0x2d, 0x31, 0x33, 0x54, 0x31,
 0x38, 0x3a, 0x33, 0x30, 0x3a, 0x30, 0x32, 0x2e, 0x32, 0x35,
 0x2b, 0x30, 0x31, 0x3a, 0x30, 0x30, 0x18, 0x58, 0xc0, 0x78,
 0x1c, 0x32, 0x30, 0x30, 0x33, 0x2d, 0x31, 0x32, 0x2d, 0x31,
 0x33, 0x54, 0x31, 0x38, 0x3a, 0x33, 0x30, 0x3a, 0x30, 0x32,
 0x2e, 0x32, 0x35, 0x2b, 0x30, 0x31, 0x3a, 0x30, 0x30, 0xf7,
 0xa3, 0x64, 0x64, 0x61, 0x72, 0x65, 0xd8, 0x42, 0xf5, 0x62,
 0x75, 0x75, 0xf4, 0x1a, 0x00, 0x0b, 0x41, 0x62, 0xf6, 0x80,
 0xa3, 0x78, 0x1c, 0x6c, 0x61, 0x62, 0x65, 0x6c, 0x20, 0x61,
 0x6e, 0x64, 0x20, 0x74, 0x61, 0x67, 0x67, 0x65, 0x64, 0x20,
 0x65, 0x6d, 0x70, 0x74, 0x79, 0x20, 0x61, 0x72, 0x72, 0x61,
 0x79, 0xd9, 0x04, 0x45, 0x80, 0x65, 0x61, 0x6c, 0x61, 0x62,
 0x6c, 0x80, 0x18, 0x2a, 0x80, 0xa1, 0x68, 0x69, 0x6e, 0x20,
 0x61, 0x20, 0x6d, 0x61, 0x70, 0xa1, 0x19, 0x15, 0xb4, 0xa1,
 0x6e, 0x69, 0x6e, 0x20, 0x61, 0x20, 0x69, 0x6e, 0x20, 0x61,
 0x20, 0x69, 0x6e, 0x20, 0x61, 0xd9, 0x23, 0x7f, 0xa0, 0xa5,
 0x62, 0x73, 0x31, 0xd8, 0x58, 0xf8, 0xff, 0x62, 0x73, 0x32,
 0xe0, 0x62, 0x73, 0x33, 0xd8, 0x58, 0xf8, 0x21, 0x1a, 0x05,
 0x44, 0x8c, 0x06, 0xd8, 0x58, 0xf8, 0xff, 0x18, 0x59, 0xd8,
 0x58, 0xf3, 0xd8, 0x25, 0x50, 0x53, 0x4d, 0x41, 0x52, 0x54,
 0x43, 0x53, 0x4c, 0x54, 0x54, 0x43, 0x46, 0x49, 0x43, 0x41,
 0x32, 0xa2, 0x64, 0x55, 0x55, 0x55, 0x55, 0xd8, 0x25, 0x50,
 0x53, 0x4d, 0x41, 0x52, 0x54, 0x43, 0x53, 0x4c, 0x54, 0x54,
 0x43, 0x46, 0x49, 0x43, 0x41, 0x32, 0x18, 0x63, 0xd8, 0x25,
 0x50, 0x53, 0x4d, 0x41, 0x52, 0x54, 0x43, 0x53, 0x4c, 0x54,
 0x54, 0x43, 0x46, 0x49, 0x43, 0x41, 0x32, 0xf5, 0xf4, 0xa2,
 0x71, 0x47, 0x65, 0x6f, 0x72, 0x67, 0x65, 0x20, 0x69, 0x73,
 0x20, 0x74, 0x68, 0x65, 0x20, 0x6d, 0x61, 0x6e, 0xf5, 0x19,
 0x10, 0x41, 0xf5, 0xC2, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0xC3, 0x49, 0x01, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0x63, 0x42, 0x4E, 0x2B,
 0xC2, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x18, 0x40, 0xC2, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x63, 0x42, 0x4E, 0x2D, 0xC3, 0x49,
 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38,
 0x3F, 0xC3, 0x49, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00
};


static const char *szMIME = "\
MIME-Version: 1.0\n\
Content-Type: multipart/mixed;\n\
boundary=\"XXXXboundary text\"\n\
\n\
This is a multipart message in MIME format.\n\
\n\
--XXXXboundary text\n\
Content-Type: text/plain\n\
\n\
this is the body text\n\
\n\
--XXXXboundary text\n\
Content-Type: text/plain;\n\
Content-Disposition: attachment;\n\
filename=\"test.txt\"\n\
\n\
this is the attachment text\n\
\n\
--XXXXboundary text--";


int32_t AllAddMethodsTest()
{
   // TODO: this test should be broken down into several so it is more
   // managable. Tags and labels could be more sensible
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   QCBOREncode_OpenArray(&ECtx);

   // Some ints that are tagged and have strings preceeding them
   // (not labels becase it is not a map)
   QCBOREncode_AddSZString(&ECtx, "UINT62");
   QCBOREncode_AddTag(&ECtx, 100);
   QCBOREncode_AddUInt64(&ECtx, 89989909);
   QCBOREncode_AddSZString(&ECtx, "INT64");
   QCBOREncode_AddTag(&ECtx, 76);
   QCBOREncode_AddInt64(&ECtx, 77689989909);
   QCBOREncode_AddUInt64(&ECtx,0);
   QCBOREncode_AddInt64(&ECtx, -44);

   // ints that go in maps
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddUInt64ToMap(&ECtx, "LBL", 77);
   QCBOREncode_AddUInt64ToMapN(&ECtx, -4, 88);
   QCBOREncode_AddInt64ToMap(&ECtx, "NEGLBLTHAT IS KIND OF LONG", -2394893489238);
   QCBOREncode_AddInt64ToMapN(&ECtx, -100000000, -800000000);
   QCBOREncode_CloseMap(&ECtx);

   // Epoch Date
   QCBOREncode_AddDateEpoch(&ECtx, 2383748234);

   // Epoch date with labels
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddDateEpochToMap(&ECtx, "LongLiveDenisRitchie", 1400000000);
   QCBOREncode_AddDateEpochToMap(&ECtx, "time()", 1477263730);
   QCBOREncode_AddDateEpochToMapN(&ECtx, -1969, 1477263222);
   QCBOREncode_CloseMap(&ECtx);

   // Binary blobs
   QCBOREncode_AddBytes(&ECtx, ((UsefulBufC) {(uint8_t []){0xff, 0x00}, 2}));

   // binary blobs in maps
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddSZString(&ECtx, "binbin");
   QCBOREncode_AddTag(&ECtx, 100000);
   QCBOREncode_AddBytes(&ECtx, ((UsefulBufC) {(uint8_t []){0x00}, 1}));
   QCBOREncode_AddBytesToMap(&ECtx, "blabel", ((UsefulBufC) {(uint8_t []){0x01, 0x02, 0x03}, 3}));
   QCBOREncode_AddBytesToMapN(&ECtx, 0, ((UsefulBufC){(uint8_t []){0x04, 0x02, 0x03, 0xfe}, 4}));
   QCBOREncode_CloseMap(&ECtx);

   // text blobs
   QCBOREncode_AddText(&ECtx, UsefulBuf_FROM_SZ_LITERAL("bar bar foo bar"));
   QCBOREncode_AddSZString(&ECtx, "oof\n");
   const char *szURL =
      "http://stackoverflow.com/questions/28059697/how-do-i-toggle-between-debug-and-release-builds-in-xcode-6-7-8";
   QCBOREncode_AddURI(&ECtx, UsefulBuf_FromSZ(szURL));
   QCBOREncode_AddB64Text(&ECtx, UsefulBuf_FROM_SZ_LITERAL("YW55IGNhcm5hbCBwbGVhc3VyZQ=="));
   QCBOREncode_AddRegex(&ECtx, UsefulBuf_FROM_SZ_LITERAL("[^abc]+"));
   QCBOREncode_AddMIMEData(&ECtx, UsefulBuf_FromSZ(szMIME));

   // text blobs in maps
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddTextToMap(&ECtx, "#####", UsefulBuf_FROM_SZ_LITERAL("foo bar foo foo"));
   QCBOREncode_AddTextToMap(&ECtx, "____", UsefulBuf_FROM_SZ_LITERAL("foo bar"));
   QCBOREncode_AddSZString(&ECtx, "()()()");
   QCBOREncode_AddTag(&ECtx, 1000);
   QCBOREncode_AddSZString(&ECtx, "rab rab oof");
   QCBOREncode_AddTextToMapN(&ECtx,22, UsefulBuf_FROM_SZ_LITERAL("foo foo foo foo"));
   QCBOREncode_AddSZStringToMap(&ECtx, "^^", "oooooooof");
   QCBOREncode_AddSZStringToMapN(&ECtx, 99, "ffffoooooooof");
   QCBOREncode_AddURIToMap(&ECtx,
                           "RFC",
                           UsefulBuf_FROM_SZ_LITERAL("https://tools.ietf.org/html/rfc7049#section-2.4.5"));
   QCBOREncode_AddURIToMapN(&ECtx, 0x89, UsefulBuf_FROM_SZ_LITERAL("http://cbor.me/"));
   QCBOREncode_AddB64TextToMap(&ECtx, "whenim64", UsefulBuf_FROM_SZ_LITERAL("cGxlYXN1cmUu"));
   QCBOREncode_AddB64TextToMapN(&ECtx, 64, UsefulBuf_FROM_SZ_LITERAL("c3VyZS4="));
   QCBOREncode_AddRegexToMap(&ECtx, "popo", UsefulBuf_FROM_SZ_LITERAL("100\\s*mk")); //   x code string literal bug
   QCBOREncode_AddRegexToMapN(&ECtx, -51, UsefulBuf_FROM_SZ_LITERAL("perl\\B"));  //   x code string literal bug
   QCBOREncode_AddMIMEDataToMap(&ECtx, "Ned", UsefulBuf_FromSZ(szMIME));
   QCBOREncode_AddMIMEDataToMapN(&ECtx, 10, UsefulBuf_FromSZ(szMIME));
   QCBOREncode_CloseMap(&ECtx);

   // Date strings
   QCBOREncode_AddDateString(&ECtx, "2003-12-13T18:30:02Z");
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddDateStringToMap(&ECtx, "Bed time", "2003-12-13T18:30:02.25+01:00");
   QCBOREncode_AddDateStringToMapN(&ECtx, 88, "2003-12-13T18:30:02.25+01:00");
   QCBOREncode_CloseMap(&ECtx);

   // true / false ...
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_UNDEF);
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddSZString(&ECtx, "dare");
   QCBOREncode_AddTag(&ECtx, 66);
   QCBOREncode_AddBool(&ECtx, true);
   QCBOREncode_AddBoolToMap(&ECtx, "uu", false);
   QCBOREncode_AddSimpleToMapN(&ECtx, 737634, CBOR_SIMPLEV_NULL);
   QCBOREncode_CloseMap(&ECtx);

   // opening an array
   QCBOREncode_OpenArray(&ECtx);
   QCBOREncode_CloseArray(&ECtx);

   // opening arrays in a map
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddSZString(&ECtx, "label and tagged empty array");
   QCBOREncode_AddTag(&ECtx, 1093);
   QCBOREncode_OpenArray(&ECtx);
   QCBOREncode_CloseArray(&ECtx);
   QCBOREncode_OpenArrayInMap(&ECtx, "alabl");
   QCBOREncode_CloseArray(&ECtx);
   QCBOREncode_OpenArrayInMapN(&ECtx, 42);
   QCBOREncode_CloseArray(&ECtx);
   QCBOREncode_CloseMap(&ECtx);

   // opening maps with labels and tagging
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_OpenMapInMap(&ECtx, "in a map");
   QCBOREncode_OpenMapInMapN(&ECtx, 5556);
   QCBOREncode_AddSZString(&ECtx, "in a in a in a");
   QCBOREncode_AddTag(&ECtx, 9087);
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_CloseMap(&ECtx);
   QCBOREncode_CloseMap(&ECtx);
   QCBOREncode_CloseMap(&ECtx);
   QCBOREncode_CloseMap(&ECtx);


   // Extended simple values (these are not standard...)
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddSZString(&ECtx, "s1");
   QCBOREncode_AddTag(&ECtx, 88);
   QCBOREncode_AddSimple(&ECtx, 255);
   QCBOREncode_AddSimpleToMap(&ECtx, "s2", 0);
   QCBOREncode_AddSZString(&ECtx, "s3");
   QCBOREncode_AddTag(&ECtx, 88);
   QCBOREncode_AddSimple(&ECtx, 33);
   QCBOREncode_AddInt64(&ECtx, 88378374); // label before tag
   QCBOREncode_AddTag(&ECtx, 88);
   QCBOREncode_AddSimple(&ECtx, 255);
   QCBOREncode_AddInt64(&ECtx, 89); // label before tag
   QCBOREncode_AddTag(&ECtx, 88);
   QCBOREncode_AddSimple(&ECtx, 19);
   QCBOREncode_CloseMap(&ECtx);

   // UUIDs
   static const uint8_t ppppUUID[] = {0x53, 0x4D, 0x41, 0x52, 0x54, 0x43,
                                      0x53, 0x4C, 0x54, 0x54, 0x43, 0x46,
                                      0x49, 0x43, 0x41, 0x32};
   const UsefulBufC XXUUID = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(ppppUUID);
   QCBOREncode_AddBinaryUUID(&ECtx, XXUUID);
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddBinaryUUIDToMap(&ECtx, "UUUU", XXUUID);
   QCBOREncode_AddBinaryUUIDToMapN(&ECtx, 99, XXUUID);
   QCBOREncode_CloseMap(&ECtx);

   // Bool
   QCBOREncode_AddBool(&ECtx, true);
   QCBOREncode_AddBool(&ECtx, false);
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddBoolToMap(&ECtx, "George is the man", true);
   QCBOREncode_AddBoolToMapN(&ECtx, 010101, true);
   QCBOREncode_CloseMap(&ECtx);


   static const uint8_t pBignum[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   const UsefulBufC BIGNUM = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pBignum);
   QCBOREncode_AddPositiveBignum(&ECtx, BIGNUM);
   QCBOREncode_AddNegativeBignum(&ECtx, BIGNUM);
   QCBOREncode_OpenMap(&ECtx);
   QCBOREncode_AddPositiveBignumToMap(&ECtx, "BN+", BIGNUM);
   QCBOREncode_AddPositiveBignumToMapN(&ECtx, 64, BIGNUM);
   QCBOREncode_AddNegativeBignumToMap(&ECtx, "BN-", BIGNUM);
   QCBOREncode_AddNegativeBignumToMapN(&ECtx, -64, BIGNUM);
   QCBOREncode_CloseMap(&ECtx);

   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC Enc;

   if(QCBOREncode_Finish(&ECtx, &Enc)) {
      nReturn = -1;
      goto Done;
   }

   if(CheckResults(Enc, spExpectedEncodedAll))
      nReturn = -2;

Done:
   return nReturn;
}

/*
 98 30                  # array(48)
   3B 7FFFFFFFFFFFFFFF # negative(9223372036854775807)
   3B 0000000100000000 # negative(4294967296)
   3A FFFFFFFF         # negative(4294967295)
   3A FFFFFFFE         # negative(4294967294)
   3A FFFFFFFD         # negative(4294967293)
   3A 7FFFFFFF         # negative(2147483647)
   3A 7FFFFFFE         # negative(2147483646)
   3A 00010001         # negative(65537)
   3A 00010000         # negative(65536)
   39 FFFF             # negative(65535)
   39 FFFE             # negative(65534)
   39 FFFD             # negative(65533)
   39 0100             # negative(256)
   38 FF               # negative(255)
   38 FE               # negative(254)
   38 FD               # negative(253)
   38 18               # negative(24)
   37                  # negative(23)
   36                  # negative(22)
   20                  # negative(0)
   00                  # unsigned(0)
   00                  # unsigned(0)
   01                  # unsigned(1)
   16                  # unsigned(22)
   17                  # unsigned(23)
   18 18               # unsigned(24)
   18 19               # unsigned(25)
   18 1A               # unsigned(26)
   18 1F               # unsigned(31)
   18 FE               # unsigned(254)
   18 FF               # unsigned(255)
   19 0100             # unsigned(256)
   19 0101             # unsigned(257)
   19 FFFE             # unsigned(65534)
   19 FFFF             # unsigned(65535)
   1A 00010000         # unsigned(65536)
   1A 00010001         # unsigned(65537)
   1A 00010002         # unsigned(65538)
   1A 7FFFFFFF         # unsigned(2147483647)
   1A 7FFFFFFF         # unsigned(2147483647)
   1A 80000000         # unsigned(2147483648)
   1A 80000001         # unsigned(2147483649)
   1A FFFFFFFE         # unsigned(4294967294)
   1A FFFFFFFF         # unsigned(4294967295)
   1B 0000000100000000 # unsigned(4294967296)
   1B 0000000100000001 # unsigned(4294967297)
   1B 7FFFFFFFFFFFFFFF # unsigned(9223372036854775807)
   1B FFFFFFFFFFFFFFFF # unsigned(18446744073709551615)
 */
static const uint8_t spExpectedEncodedInts[] = {
   0x98, 0x30, 0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff,
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
   0x1a, 0x18, 0x1f, 0x18, 0xfe, 0x18, 0xff, 0x19,
   0x01, 0x00, 0x19, 0x01, 0x01, 0x19, 0xff, 0xfe,
   0x19, 0xff, 0xff, 0x1a, 0x00, 0x01, 0x00, 0x00,
   0x1a, 0x00, 0x01, 0x00, 0x01, 0x1a, 0x00, 0x01,
   0x00, 0x02, 0x1a, 0x7f, 0xff, 0xff, 0xff, 0x1a,
   0x7f, 0xff, 0xff, 0xff, 0x1a, 0x80, 0x00, 0x00,
   0x00, 0x1a, 0x80, 0x00, 0x00, 0x01, 0x1a, 0xff,
   0xff, 0xff, 0xfe, 0x1a, 0xff, 0xff, 0xff, 0xff,
   0x1b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
   0x00, 0x1b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
   0x00, 0x01, 0x1b, 0x7f, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x1b, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff};

/*

  Test the generation of integers. This also ends up testing
  encoding of all the different lengths. It encodes integers
  of many lengths and values, especially around the boundaries
  for different types of integers.  It compares the output
  to expected values generated from http://cbor.me.

 */
int32_t IntegerValuesTest1()
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArray(&ECtx);

   QCBOREncode_AddInt64(&ECtx, -9223372036854775807LL - 1);
   QCBOREncode_AddInt64(&ECtx, -4294967297);
   QCBOREncode_AddInt64(&ECtx, -4294967296);
   QCBOREncode_AddInt64(&ECtx, -4294967295);
   QCBOREncode_AddInt64(&ECtx, -4294967294);
   QCBOREncode_AddInt64(&ECtx, -2147483648);
   QCBOREncode_AddInt64(&ECtx, -2147483647);
   QCBOREncode_AddInt64(&ECtx, -65538);
   QCBOREncode_AddInt64(&ECtx, -65537);
   QCBOREncode_AddInt64(&ECtx, -65536);
   QCBOREncode_AddInt64(&ECtx, -65535);
   QCBOREncode_AddInt64(&ECtx, -65534);
   QCBOREncode_AddInt64(&ECtx, -257);
   QCBOREncode_AddInt64(&ECtx, -256);
   QCBOREncode_AddInt64(&ECtx, -255);
   QCBOREncode_AddInt64(&ECtx, -254);
   QCBOREncode_AddInt64(&ECtx, -25);
   QCBOREncode_AddInt64(&ECtx, -24);
   QCBOREncode_AddInt64(&ECtx, -23);
   QCBOREncode_AddInt64(&ECtx, -1);
   QCBOREncode_AddInt64(&ECtx, 0);
   QCBOREncode_AddUInt64(&ECtx, 0ULL);
   QCBOREncode_AddInt64(&ECtx, 1);
   QCBOREncode_AddInt64(&ECtx, 22);
   QCBOREncode_AddInt64(&ECtx, 23);
   QCBOREncode_AddInt64(&ECtx, 24);
   QCBOREncode_AddInt64(&ECtx, 25);
   QCBOREncode_AddInt64(&ECtx, 26);
   QCBOREncode_AddInt64(&ECtx, 31);
   QCBOREncode_AddInt64(&ECtx, 254);
   QCBOREncode_AddInt64(&ECtx, 255);
   QCBOREncode_AddInt64(&ECtx, 256);
   QCBOREncode_AddInt64(&ECtx, 257);
   QCBOREncode_AddInt64(&ECtx, 65534);
   QCBOREncode_AddInt64(&ECtx, 65535);
   QCBOREncode_AddInt64(&ECtx, 65536);
   QCBOREncode_AddInt64(&ECtx, 65537);
   QCBOREncode_AddInt64(&ECtx, 65538);
   QCBOREncode_AddInt64(&ECtx, 2147483647);
   QCBOREncode_AddInt64(&ECtx, 2147483647);
   QCBOREncode_AddInt64(&ECtx, 2147483648);
   QCBOREncode_AddInt64(&ECtx, 2147483649);
   QCBOREncode_AddInt64(&ECtx, 4294967294);
   QCBOREncode_AddInt64(&ECtx, 4294967295);
   QCBOREncode_AddInt64(&ECtx, 4294967296);
   QCBOREncode_AddInt64(&ECtx, 4294967297);
   QCBOREncode_AddInt64(&ECtx, 9223372036854775807LL);
   QCBOREncode_AddUInt64(&ECtx, 18446744073709551615ULL);

   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC Enc;
   if(QCBOREncode_Finish(&ECtx, &Enc)) {
      nReturn = -1;
   }

   if(CheckResults(Enc, spExpectedEncodedInts))
     return -2;

   return(nReturn);
}


/*
 85                  # array(5)
   F5               # primitive(21)
   F4               # primitive(20)
   F6               # primitive(22)
   F7               # primitive(23)
   A1               # map(1)
      65            # text(5)
         554E446566 # "UNDef"
      F7            # primitive(23)
 */
static const uint8_t spExpectedEncodedSimple[] = {
   0x85, 0xf5, 0xf4, 0xf6, 0xf7, 0xa1, 0x65, 0x55, 0x4e, 0x44, 0x65, 0x66, 0xf7};

int32_t SimpleValuesTest1()
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArray(&ECtx);

   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_TRUE);
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_FALSE);
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_NULL);
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_UNDEF);

   QCBOREncode_OpenMap(&ECtx);

   QCBOREncode_AddSimpleToMap(&ECtx, "UNDef", CBOR_SIMPLEV_UNDEF);
   QCBOREncode_CloseMap(&ECtx);

   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC ECBOR;
   if(QCBOREncode_Finish(&ECtx, &ECBOR)) {
      nReturn = -1;
   }

   if(CheckResults(ECBOR, spExpectedEncodedSimple))
      return -2;

   return(nReturn);
}

/*
 9F                  # array(5)
   F5               # primitive(21)
   F4               # primitive(20)
   F6               # primitive(22)
   F7               # primitive(23)
   BF               # map(1)
      65            # text(5)
         554E446566 # "UNDef"
      F7            # primitive(23)
      FF            # break
   FF               # break
 */
static const uint8_t spExpectedEncodedSimpleIndefiniteLength[] = {
   0x9f, 0xf5, 0xf4, 0xf6, 0xf7, 0xbf, 0x65, 0x55, 0x4e, 0x44, 0x65, 0x66, 0xf7, 0xff, 0xff};

int32_t SimpleValuesIndefiniteLengthTest1()
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArrayIndefiniteLength(&ECtx);

   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_TRUE);
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_FALSE);
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_NULL);
   QCBOREncode_AddSimple(&ECtx, CBOR_SIMPLEV_UNDEF);

   QCBOREncode_OpenMapIndefiniteLength(&ECtx);

   QCBOREncode_AddSimpleToMap(&ECtx, "UNDef", CBOR_SIMPLEV_UNDEF);
   QCBOREncode_CloseMapIndefiniteLength(&ECtx);

   QCBOREncode_CloseArrayIndefiniteLength(&ECtx);

   UsefulBufC ECBOR;
   if(QCBOREncode_Finish(&ECtx, &ECBOR)) {
      nReturn = -1;
   }

   if(CheckResults(ECBOR, spExpectedEncodedSimpleIndefiniteLength))
      return -2;

   return(nReturn);
}

/*
A5                                      # map(5)
   63                                   # text(3)
      617272                            # "arr"
   98 1F                                # array(31)
      00                                # unsigned(0)
      01                                # unsigned(1)
      02                                # unsigned(2)
      03                                # unsigned(3)
      04                                # unsigned(4)
      05                                # unsigned(5)
      06                                # unsigned(6)
      07                                # unsigned(7)
      08                                # unsigned(8)
      09                                # unsigned(9)
      0A                                # unsigned(10)
      0B                                # unsigned(11)
      0C                                # unsigned(12)
      0D                                # unsigned(13)
      0E                                # unsigned(14)
      0F                                # unsigned(15)
      10                                # unsigned(16)
      11                                # unsigned(17)
      12                                # unsigned(18)
      13                                # unsigned(19)
      14                                # unsigned(20)
      15                                # unsigned(21)
      16                                # unsigned(22)
      17                                # unsigned(23)
      18 18                             # unsigned(24)
      18 19                             # unsigned(25)
      18 1A                             # unsigned(26)
      18 1B                             # unsigned(27)
      18 1C                             # unsigned(28)
      18 1D                             # unsigned(29)
      18 1E                             # unsigned(30)
   63                                   # text(3)
      6D6170                            # "map"
   B8 1F                                # map(31)
      61                                # text(1)
         61                             # "a"
      00                                # unsigned(0)
      61                                # text(1)
         62                             # "b"
      01                                # unsigned(1)
      61                                # text(1)
         63                             # "c"
      02                                # unsigned(2)
      61                                # text(1)
         64                             # "d"
      03                                # unsigned(3)
      61                                # text(1)
         65                             # "e"
      04                                # unsigned(4)
      61                                # text(1)
         66                             # "f"
      05                                # unsigned(5)
      61                                # text(1)
         67                             # "g"
      06                                # unsigned(6)
      61                                # text(1)
         68                             # "h"
      07                                # unsigned(7)
      61                                # text(1)
         69                             # "i"
      08                                # unsigned(8)
      61                                # text(1)
         6A                             # "j"
      09                                # unsigned(9)
      61                                # text(1)
         6B                             # "k"
      0A                                # unsigned(10)
      61                                # text(1)
         6C                             # "l"
      0B                                # unsigned(11)
      61                                # text(1)
         6D                             # "m"
      0C                                # unsigned(12)
      61                                # text(1)
         6E                             # "n"
      0D                                # unsigned(13)
      61                                # text(1)
         6F                             # "o"
      0E                                # unsigned(14)
      61                                # text(1)
         70                             # "p"
      0F                                # unsigned(15)
      61                                # text(1)
         71                             # "q"
      10                                # unsigned(16)
      61                                # text(1)
         72                             # "r"
      11                                # unsigned(17)
      61                                # text(1)
         73                             # "s"
      12                                # unsigned(18)
      61                                # text(1)
         74                             # "t"
      13                                # unsigned(19)
      61                                # text(1)
         75                             # "u"
      14                                # unsigned(20)
      61                                # text(1)
         76                             # "v"
      15                                # unsigned(21)
      61                                # text(1)
         77                             # "w"
      16                                # unsigned(22)
      61                                # text(1)
         78                             # "x"
      17                                # unsigned(23)
      61                                # text(1)
         79                             # "y"
      18 18                             # unsigned(24)
      61                                # text(1)
         7A                             # "z"
      18 19                             # unsigned(25)
      61                                # text(1)
         41                             # "A"
      18 1A                             # unsigned(26)
      61                                # text(1)
         42                             # "B"
      18 1B                             # unsigned(27)
      61                                # text(1)
         43                             # "C"
      18 1C                             # unsigned(28)
      61                                # text(1)
         44                             # "D"
      18 1D                             # unsigned(29)
      61                                # text(1)
         45                             # "E"
      18 1E                             # unsigned(30)
   65                                   # text(5)
      6D696E3331                        # "min31"
   38 1E                                # negative(30)
   66                                   # text(6)
      706C75733331                      # "plus31"
   18 1F                                # unsigned(31)
   63                                   # text(3)
      737472                            # "str"
   78 1F                                # text(31)
      7465737474657374746573747465737474657374746573747163626F723131 # "testtesttesttesttesttestqcbor11"
 */
static const uint8_t EncodeLengthThirtyone[] = {
   0xa5, 0x63, 0x61, 0x72, 0x72, 0x98, 0x1f, 0x00, 0x01, 0x02, 0x03, 0x04,
   0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
   0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x19, 0x18,
   0x1a, 0x18, 0x1b, 0x18, 0x1c, 0x18, 0x1d, 0x18, 0x1e, 0x63, 0x6d, 0x61,
   0x70, 0xb8, 0x1f, 0x61, 0x61, 0x00, 0x61, 0x62, 0x01, 0x61, 0x63, 0x02,
   0x61, 0x64, 0x03, 0x61, 0x65, 0x04, 0x61, 0x66, 0x05, 0x61, 0x67, 0x06,
   0x61, 0x68, 0x07, 0x61, 0x69, 0x08, 0x61, 0x6a, 0x09, 0x61, 0x6b, 0x0a,
   0x61, 0x6c, 0x0b, 0x61, 0x6d, 0x0c, 0x61, 0x6e, 0x0d, 0x61, 0x6f, 0x0e,
   0x61, 0x70, 0x0f, 0x61, 0x71, 0x10, 0x61, 0x72, 0x11, 0x61, 0x73, 0x12,
   0x61, 0x74, 0x13, 0x61, 0x75, 0x14, 0x61, 0x76, 0x15, 0x61, 0x77, 0x16,
   0x61, 0x78, 0x17, 0x61, 0x79, 0x18, 0x18, 0x61, 0x7a, 0x18, 0x19, 0x61,
   0x41, 0x18, 0x1a, 0x61, 0x42, 0x18, 0x1b, 0x61, 0x43, 0x18, 0x1c, 0x61,
   0x44, 0x18, 0x1d, 0x61, 0x45, 0x18, 0x1e, 0x65, 0x6d, 0x69, 0x6e, 0x33,
   0x31, 0x38, 0x1e, 0x66, 0x70, 0x6c, 0x75, 0x73, 0x33, 0x31, 0x18, 0x1f,
   0x63, 0x73, 0x74, 0x72, 0x78, 0x1f, 0x74, 0x65, 0x73, 0x74, 0x74, 0x65,
   0x73, 0x74, 0x74, 0x65, 0x73, 0x74, 0x74, 0x65, 0x73, 0x74, 0x74, 0x65,
   0x73, 0x74, 0x74, 0x65, 0x73, 0x74, 0x71, 0x63, 0x62, 0x6f, 0x72, 0x31,
   0x31
};

int32_t EncodeLengthThirtyoneTest()
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenMap(&ECtx);

   // add array with 31 items
   QCBOREncode_OpenArrayInMap(&ECtx, "arr");
   for (size_t ix = 0; ix < 31; ix++) {
      QCBOREncode_AddInt64(&ECtx, (int64_t)ix);
   }
   QCBOREncode_CloseArray(&ECtx);

   // add map with 31 items
   QCBOREncode_OpenMapInMap(&ECtx, "map");
   for (int ix = 0; ix < 31; ix++) {
      // make sure we have unique keys in the map (a-z then follow by A-Z)
      int c = 'a';
      if (ix < 26) c = c + ix;
      else c = 'A' + (ix - 26);
      char buffer[2] = { (char)c, 0 };
      QCBOREncode_AddInt64ToMap(&ECtx, buffer, ix);
   }
   QCBOREncode_CloseMap(&ECtx);

   // add -31 and +31
   QCBOREncode_AddInt64ToMap(&ECtx, "min31", -31);
   QCBOREncode_AddInt64ToMap(&ECtx, "plus31", 31);

   // add string with length 31
   const char *str = "testtesttesttesttesttestqcbor11";
   UsefulBufC str_b = { str, 31 };
   QCBOREncode_AddTextToMap(&ECtx, "str", str_b);

   QCBOREncode_CloseMap(&ECtx);

   UsefulBufC ECBOR;
   if(QCBOREncode_Finish(&ECtx, &ECBOR)) {
      nReturn = -1;
   }

   if(CheckResults(ECBOR, EncodeLengthThirtyone))
      return -2;

   return(nReturn);
}


/*
 83                                      # array(3)
   C0                                   # tag(0)
      74                                # text(20)
         323031332D30332D32315432303A30343A30305A # "2013-03-21T20:04:00Z"
   C1                                   # tag(1)
      1A 514B67B0                       # unsigned(1363896240)
   A2                                   # map(2)
      78 19                             # text(25)
         53616D706C6520446174652066726F6D205246432033333339 # "Sample Date from RFC 3339"
      C0                                # tag(0)
         77                             # text(23)
            313938352D30342D31325432333A32303A35302E35325A # "1985-04-12T23:20:50.52Z"
      62                                # text(2)
         5344                           # "SD"
      C1                                # tag(1)
         19 03E7                        # unsigned(999)
 */
static const uint8_t spExpectedEncodedDates[] = {
   0x83, 0xc0, 0x74, 0x32, 0x30, 0x31, 0x33, 0x2d, 0x30, 0x33,
   0x2d, 0x32, 0x31, 0x54, 0x32, 0x30, 0x3a, 0x30, 0x34, 0x3a,
   0x30, 0x30, 0x5a, 0xc1, 0x1a, 0x51, 0x4b, 0x67, 0xb0, 0xa2,
   0x78, 0x19, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x20, 0x44,
   0x61, 0x74, 0x65, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x52,
   0x46, 0x43, 0x20, 0x33, 0x33, 0x33, 0x39, 0xc0, 0x77, 0x31,
   0x39, 0x38, 0x35, 0x2d, 0x30, 0x34, 0x2d, 0x31, 0x32, 0x54,
   0x32, 0x33, 0x3a, 0x32, 0x30, 0x3a, 0x35, 0x30, 0x2e, 0x35,
   0x32, 0x5a, 0x62, 0x53, 0x44, 0xc1, 0x19, 0x03, 0xe7
};

int32_t EncodeDateTest()
{
   QCBOREncodeContext ECtx;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   QCBOREncode_OpenArray(&ECtx);


   QCBOREncode_AddDateString(&ECtx, "2013-03-21T20:04:00Z"); // from CBOR RFC
   QCBOREncode_AddDateEpoch(&ECtx, 1363896240); // from CBOR RFC


   QCBOREncode_OpenMap(&ECtx);

   QCBOREncode_AddDateStringToMap(&ECtx, "Sample Date from RFC 3339", "1985-04-12T23:20:50.52Z");

   QCBOREncode_AddDateEpochToMap(&ECtx, "SD", 999);

   QCBOREncode_CloseMap(&ECtx);

   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC ECBOR;

   if(QCBOREncode_Finish(&ECtx, &ECBOR)) {
      nReturn = -1;
   }

   if(CheckResults(ECBOR, spExpectedEncodedDates))
      return -2;

   return(nReturn);
}


int32_t ArrayNestingTest1()
{
   QCBOREncodeContext ECtx;
   int i;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   for(i = QCBOR_MAX_ARRAY_NESTING; i; i--) {
      QCBOREncode_OpenArray(&ECtx);
   }
   for(i = QCBOR_MAX_ARRAY_NESTING; i; i--) {
      QCBOREncode_CloseArray(&ECtx);
   }
   UsefulBufC Encoded;
   if(QCBOREncode_Finish(&ECtx, &Encoded)) {
      nReturn = -1;
   }

   return(nReturn);
}



int32_t ArrayNestingTest2()
{
   QCBOREncodeContext ECtx;
   int i;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   for(i = QCBOR_MAX_ARRAY_NESTING+1; i; i--) {
      QCBOREncode_OpenArray(&ECtx);
   }
   for(i = QCBOR_MAX_ARRAY_NESTING; i; i--) {
      QCBOREncode_CloseArray(&ECtx);
   }

   UsefulBufC Encoded;
   if(QCBOREncode_Finish(&ECtx, &Encoded) != QCBOR_ERR_ARRAY_NESTING_TOO_DEEP) {
      nReturn = -1;
   }

   return(nReturn);
}



int32_t ArrayNestingTest3()
{
   QCBOREncodeContext ECtx;
   int i;
   int nReturn = 0;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   for(i = QCBOR_MAX_ARRAY_NESTING; i; i--) {
      QCBOREncode_OpenArray(&ECtx);
   }
   for(i = QCBOR_MAX_ARRAY_NESTING+1 ; i; i--) {
      QCBOREncode_CloseArray(&ECtx);
   }
   UsefulBufC Encoded;
   if(QCBOREncode_Finish(&ECtx, &Encoded) != QCBOR_ERR_TOO_MANY_CLOSES) {
      nReturn = -1;
   }

   return(nReturn);
}


/*
 81             # array(1)
 81          # array(1)
 81       # array(1)
 81    # array(1)
 80 # array(0)
*/
static const uint8_t spFiveArrarys[] = {0x81, 0x81, 0x81, 0x81, 0x80};

// Validated at http://cbor.me and by manually examining its output
/*
 82                        # array(2)
 81                     # array(1)
 81                  # array(1)
 81               # array(1)
 81            # array(1)
 80         # array(0)
 98 30                  # array(48)
 3B 7FFFFFFFFFFFFFFF # negative(9223372036854775807)
 3B 0000000100000000 # negative(4294967296)
 3A FFFFFFFF         # negative(4294967295)
 3A FFFFFFFE         # negative(4294967294)
 3A FFFFFFFD         # negative(4294967293)
 3A 7FFFFFFF         # negative(2147483647)
 3A 7FFFFFFE         # negative(2147483646)
 3A 00010001         # negative(65537)
 3A 00010000         # negative(65536)
 39 FFFF             # negative(65535)
 39 FFFE             # negative(65534)
 39 FFFD             # negative(65533)
 39 0100             # negative(256)
 38 FF               # negative(255)
 38 FE               # negative(254)
 38 FD               # negative(253)
 38 18               # negative(24)
 37                  # negative(23)
 36                  # negative(22)
 20                  # negative(0)
 00                  # unsigned(0)
 00                  # unsigned(0)
 01                  # unsigned(1)
 16                  # unsigned(22)
 17                  # unsigned(23)
 18 18               # unsigned(24)
 18 19               # unsigned(25)
 18 1A               # unsigned(26)
 18 1F               # unsigned(31)
 18 FE               # unsigned(254)
 18 FF               # unsigned(255)
 19 0100             # unsigned(256)
 19 0101             # unsigned(257)
 19 FFFE             # unsigned(65534)
 19 FFFF             # unsigned(65535)
 1A 00010000         # unsigned(65536)
 1A 00010001         # unsigned(65537)
 1A 00010002         # unsigned(65538)
 1A 7FFFFFFF         # unsigned(2147483647)
 1A 7FFFFFFF         # unsigned(2147483647)
 1A 80000000         # unsigned(2147483648)
 1A 80000001         # unsigned(2147483649)
 1A FFFFFFFE         # unsigned(4294967294)
 1A FFFFFFFF         # unsigned(4294967295)
 1B 0000000100000000 # unsigned(4294967296)
 1B 0000000100000001 # unsigned(4294967297)
 1B 7FFFFFFFFFFFFFFF # unsigned(9223372036854775807)
 1B FFFFFFFFFFFFFFFF # unsigned(18446744073709551615)
 */
static const uint8_t spEncodeRawExpected[] = {
   0x82, 0x81, 0x81, 0x81, 0x81, 0x80, 0x98, 0x30,
   0x3b, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0x3b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x3a, 0xff, 0xff, 0xff, 0xff, 0x3a,
   0xff, 0xff, 0xff, 0xfe, 0x3a, 0xff, 0xff, 0xff,
   0xfd, 0x3a, 0x7f, 0xff, 0xff, 0xff, 0x3a, 0x7f,
   0xff, 0xff, 0xfe, 0x3a, 0x00, 0x01, 0x00, 0x01,
   0x3a, 0x00, 0x01, 0x00, 0x00, 0x39, 0xff, 0xff,
   0x39, 0xff, 0xfe, 0x39, 0xff, 0xfd, 0x39, 0x01,
   0x00, 0x38, 0xff, 0x38, 0xfe, 0x38, 0xfd, 0x38,
   0x18, 0x37, 0x36, 0x20, 0x00, 0x00, 0x01, 0x16,
   0x17, 0x18, 0x18, 0x18, 0x19, 0x18, 0x1a, 0x18,
   0x1f, 0x18, 0xfe, 0x18, 0xff, 0x19, 0x01, 0x00,
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


int32_t EncodeRawTest()
{
   QCBOREncodeContext ECtx;

   QCBOREncode_Init(&ECtx, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArray(&ECtx);
   QCBOREncode_AddEncoded(&ECtx, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spFiveArrarys));
   QCBOREncode_AddEncoded(&ECtx, UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spExpectedEncodedInts));
   QCBOREncode_CloseArray(&ECtx);

   UsefulBufC EncodedRawTest;

   if(QCBOREncode_Finish(&ECtx, &EncodedRawTest)) {
      return -4;
   }

   if(CheckResults(EncodedRawTest, spEncodeRawExpected)) {
      return -5;
   }

   return 0;
}

/*
 This returns a pointer to spBigBuf
 */
static int32_t CreateMap(uint8_t **pEncoded, size_t *pEncodedLen)
{
   QCBOREncodeContext ECtx;
   int nReturn = -1;

   *pEncoded = NULL;
   *pEncodedLen = INT32_MAX;
   size_t uFirstSizeEstimate = 0;

   // loop runs CBOR encoding twice. First with no buffer to
   // calucate the length so buffer can be allocated correctly,
   // and last with the buffer to do the actual encoding
   do {
      QCBOREncode_Init(&ECtx, (UsefulBuf){*pEncoded, *pEncodedLen});
      QCBOREncode_OpenMap(&ECtx);
      QCBOREncode_AddInt64ToMap(&ECtx, "first integer", 42);
      QCBOREncode_OpenArrayInMap(&ECtx, "an array of two strings");
      QCBOREncode_AddText(&ECtx, ((UsefulBufC) {"string1", 7}));
      QCBOREncode_AddText(&ECtx, ((UsefulBufC) {"string2", 7}));
      QCBOREncode_CloseArray(&ECtx);
      QCBOREncode_OpenMapInMap(&ECtx, "map in a map");
      QCBOREncode_AddBytesToMap(&ECtx,"bytes 1", ((UsefulBufC) { "xxxx", 4}));
      QCBOREncode_AddBytesToMap(&ECtx, "bytes 2",((UsefulBufC) { "yyyy", 4}));
      QCBOREncode_AddInt64ToMap(&ECtx, "another int", 98);
      QCBOREncode_AddTextToMap(&ECtx, "text 2", ((UsefulBufC) {"lies, damn lies and statistics", 30}));
      QCBOREncode_CloseMap(&ECtx);
      QCBOREncode_CloseMap(&ECtx);

      if(QCBOREncode_FinishGetSize(&ECtx, pEncodedLen))
         goto Done;
      if(*pEncoded != NULL) {
         if(uFirstSizeEstimate != *pEncodedLen) {
            nReturn = 1;
         } else {
            nReturn = 0;
         }
         goto Done;
      }
      *pEncoded = spBigBuf;
      uFirstSizeEstimate = *pEncodedLen;

   } while(1);

 Done:
   return(nReturn);
}

/*
 A3                                      # map(3)
   6D                                   # text(13)
      666972737420696E7465676572        # "first integer"
   18 2A                                # unsigned(42)
   77                                   # text(23)
      616E206172726179206F662074776F20737472696E6773 # "an array of two strings"
   82                                   # array(2)
      67                                # text(7)
         737472696E6731                 # "string1"
      67                                # text(7)
         737472696E6732                 # "string2"
   6C                                   # text(12)
      6D617020696E2061206D6170          # "map in a map"
   A4                                   # map(4)
      67                                # text(7)
         62797465732031                 # "bytes 1"
      44                                # bytes(4)
         78787878                       # "xxxx"
      67                                # text(7)
         62797465732032                 # "bytes 2"
      44                                # bytes(4)
         79797979                       # "yyyy"
      6B                                # text(11)
         616E6F7468657220696E74         # "another int"
      18 62                             # unsigned(98)
      66                                # text(6)
         746578742032                   # "text 2"
      78 1E                             # text(30)
         6C6965732C2064616D6E206C69657320616E642073746174697374696373 # "lies, damn lies and statistics"
 */
static const uint8_t spValidMapEncoded[] = {
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


int32_t MapEncodeTest()
{
   uint8_t *pEncodedMaps;
   size_t nEncodedMapLen;

   if(CreateMap(&pEncodedMaps, &nEncodedMapLen)) {
      return -1;
   }

   int nReturn = 0;
   if(memcmp(spValidMapEncoded, pEncodedMaps, sizeof(spValidMapEncoded)))
      nReturn = 2;

   return(nReturn);
}


/*
 @brief  Encode the RTIC results

 @param[in]     nRResult        CBOR_SIMPLEV_TRUE, CBOR_SIMPLEV_FALSE or
                                CBOR_SIMPLEV_NULL
 @param[in]     time            Time stamp in UNIX epoch time or 0 for none
 @param[in]     szAlexString    Diagnostic code.
 @param[in[     pOut            Buffer to put the result in
 @param[in/out] pnLen           Size of pOut buffer when called; length of data
                                output in buffer on return

 @return
 One of the CBOR encoder errors. QCBOR_SUCCESS, which is has value 0, if no error.

 The size of pOut should be 30 bytes plus the length of pnLen.  If you make it too
 short an error will be returned. This function will never write off the end
 of the buffer passed to it.

 If the result is 0, then the correct encoded CBOR is in pOut and *pnLen is the
 length of the encoded CBOR.

 */

static UsefulBufC
FormatRTICResults(uint8_t uRResult,
                  int64_t time,
                  const char *szType,
                  const char *szAlexString,
                  UsefulBuf Storage)
{
   // Buffer that the result will be written in to
   // It is fixed size and small that a stack variable will be fine
   // QCBOREncode will never write off the end of this buffer. If it won't
   // fit QCBOREncode_Finish will return an error.

   // Context for the encoder
   QCBOREncodeContext ECtx;
   QCBOREncode_Init(&ECtx, Storage);

   // All the RTIC results are grouped in a CBOR Map which will get turned into a JSON Object
   // Contents are label / value pairs
   QCBOREncode_OpenMap(&ECtx);

   { // Brace / indention just to show CBOR encoding nesting

      // The result: 0 if scan happened and found nothing; 1 if it happened and
      // found something wrong; 2 if it didn't happen
      QCBOREncode_AddSimpleToMap(&ECtx, "integrity", uRResult);

      // Add the diagnostic code
      QCBOREncode_AddSZStringToMap(&ECtx, "type", szType);

      // Add a time stamp
      if(time) {
         QCBOREncode_AddDateEpochToMap(&ECtx, "time", time);
      }

      // Add the diagnostic code
      QCBOREncode_AddSZStringToMap(&ECtx, "diag", szAlexString);

      // Open a subordinate map for telemtry data
      QCBOREncode_OpenMapInMap(&ECtx, "telemetry");

      { // Brace / indention just to show CBOR encoding nesting

         // Add a few fake integers and buffers for now.
         QCBOREncode_AddInt64ToMap(&ECtx, "Shoe Size", 12);

         // Add a few fake integers and buffers for now.
         QCBOREncode_AddInt64ToMap(&ECtx, "IQ", 0xffffffff);

         // Add a few fake integers and buffers for now.
         static const uint8_t pPV[] = {0x66, 0x67, 0x00, 0x56, 0xaa, 0xbb, 0x01, 0x01};
         const UsefulBufC WSPV = {pPV, sizeof(pPV)};

         QCBOREncode_AddBytesToMap(&ECtx, "WhaleSharkPatternVector", WSPV);
      }
   }

   // Close the telemetry map
   QCBOREncode_CloseMap(&ECtx);

   // Close the map
   QCBOREncode_CloseMap(&ECtx);

   UsefulBufC Result;

   QCBOREncode_Finish(&ECtx, &Result);

   return Result;
}


/*
 A5                                      # map(5)
   69                                   # text(9)
      696E74656772697479                # "integrity"
   F4                                   # primitive(20)
   64                                   # text(4)
      74797065                          # "type"
   66                                   # text(6)
      726563656E74                      # "recent"
   64                                   # text(4)
      74696D65                          # "time"
   C1                                   # tag(1)
      1A 580D4172                       # unsigned(1477263730)
   64                                   # text(4)
      64696167                          # "diag"
   6A                                   # text(10)
      30784131654335303031              # "0xA1eC5001"
   69                                   # text(9)
      74656C656D65747279                # "telemetry"
   A3                                   # map(3)
      69                                # text(9)
         53686F652053697A65             # "Shoe Size"
      0C                                # unsigned(12)
      62                                # text(2)
         4951                           # "IQ"
      1A FFFFFFFF                       # unsigned(4294967295)
      77                                # text(23)
         5768616C65536861726B5061747465726E566563746F72 # "WhaleSharkPatternVector"
      48                                # bytes(8)
         66670056AABB0101               # "fg\x00V\xAA\xBB\x01\x01"
 */
static const uint8_t spExpectedRTIC[] = {
   0xa5, 0x69, 0x69, 0x6e, 0x74, 0x65, 0x67, 0x72, 0x69, 0x74,
   0x79, 0xf4, 0x64, 0x74, 0x79, 0x70, 0x65, 0x66, 0x72, 0x65,
   0x63, 0x65, 0x6e, 0x74, 0x64, 0x74, 0x69, 0x6d, 0x65, 0xc1,
   0x1a, 0x58, 0x0d, 0x41, 0x72, 0x64, 0x64, 0x69, 0x61, 0x67,
   0x6a, 0x30, 0x78, 0x41, 0x31, 0x65, 0x43, 0x35, 0x30, 0x30,
   0x31, 0x69, 0x74, 0x65, 0x6c, 0x65, 0x6d, 0x65, 0x74, 0x72,
   0x79, 0xa3, 0x69, 0x53, 0x68, 0x6f, 0x65, 0x20, 0x53, 0x69,
   0x7a, 0x65, 0x0c, 0x62, 0x49, 0x51, 0x1a, 0xff, 0xff, 0xff,
   0xff, 0x77, 0x57, 0x68, 0x61, 0x6c, 0x65, 0x53, 0x68, 0x61,
   0x72, 0x6b, 0x50, 0x61, 0x74, 0x74, 0x65, 0x72, 0x6e, 0x56,
   0x65, 0x63, 0x74, 0x6f, 0x72, 0x48, 0x66, 0x67, 0x00, 0x56,
   0xaa, 0xbb, 0x01, 0x01};


int32_t RTICResultsTest()
{
   const UsefulBufC Encoded = FormatRTICResults(CBOR_SIMPLEV_FALSE, 1477263730,
                                          "recent", "0xA1eC5001",
                                          UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   if(UsefulBuf_IsNULLC(Encoded)) {
      return -1;
   }

   if(CheckResults(Encoded, spExpectedRTIC)) {
      return -2;
   }

   return 0;
}


/*
 The expected encoding for first test in BstrWrapTest()

 82           # array(2)
   19 01C3   # unsigned(451)
   43        # bytes(3)
      1901D2 # "\x19\x01\xD2"
*/
static const uint8_t spExpectedBstrWrap[] = {0x82, 0x19, 0x01, 0xC3, 0x43, 0x19, 0x01, 0xD2};

/*
 81   #array(1)
 0x58  0x25  # string of length 37 (length of "This is longer than twenty four bytes")
 */
static const uint8_t spExpectedTypeAndLen[] = {0x81, 0x58, 0x25};

/*
 Very basic bstr wrapping test
 */
int32_t BstrWrapTest()
{
   QCBOREncodeContext EC;

   // First test - make some wrapped CBOR and see that it is as expected
   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddUInt64(&EC, 451);

   QCBOREncode_BstrWrap(&EC);
   QCBOREncode_AddUInt64(&EC, 466);

   UsefulBufC Wrapped;
   QCBOREncode_CloseBstrWrap(&EC, &Wrapped);

   QCBOREncode_CloseArray(&EC);

   UsefulBufC Encoded;
   if(QCBOREncode_Finish(&EC, &Encoded)) {
      return -1;
   }

   if(CheckResults(Encoded, spExpectedBstrWrap)) {
      return -2;
   }

   // Second test - see if the length of the wrapped
   // bstr is correct. Also tests bstr wrapping
   // in length calculation only mode.
   QCBOREncode_Init(&EC, (UsefulBuf){NULL, INT32_MAX});
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_BstrWrap(&EC);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddNULL(&EC);
   QCBOREncode_CloseArray(&EC);
   UsefulBufC BStr;
   QCBOREncode_CloseBstrWrap(&EC, &BStr);
   // 3 is one byte for the wrapping bstr, 1 for an array of length 1,
   // and 1 byte for a NULL
   if(BStr.ptr != NULL || BStr.len != 3) {
      return -5;
   }

   // Third, test QCBOREncode_AddBytesLenOnly() here as it is part of the
   // bstr wrapping use cases.
   UsefulBuf_MAKE_STACK_UB(StuffBuf, 50);
   QCBOREncode_Init(&EC, StuffBuf);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddBytesLenOnly(&EC, UsefulBuf_FROM_SZ_LITERAL("This is longer than twenty four bytes"));
   QCBOREncode_CloseArray(&EC);
   if(QCBOREncode_Finish(&EC, &Encoded)) {
      return -6;
   }
   if(CheckResults(Encoded, spExpectedTypeAndLen)) {
      return -7;
   }

   return 0;
}



int32_t BstrWrapErrorTest()
{
   // ---- Test closing a bstrwrap when it is an array that is open ---------
   QCBOREncodeContext EC;

   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddUInt64(&EC, 451);

   QCBOREncode_BstrWrap(&EC);
   QCBOREncode_AddUInt64(&EC, 466);
   QCBOREncode_OpenArray(&EC);

   UsefulBufC Wrapped;
   QCBOREncode_CloseBstrWrap(&EC, &Wrapped);

   QCBOREncode_CloseArray(&EC);

   UsefulBufC Encoded2;
   if(QCBOREncode_Finish(&EC, &Encoded2) != QCBOR_ERR_CLOSE_MISMATCH) {
      return -1;
   }

   // -------- test closing a bstrwrap when nothing is open ----------------
   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_CloseBstrWrap(&EC, &Wrapped);
   if(QCBOREncode_Finish(&EC, &Encoded2) != QCBOR_ERR_TOO_MANY_CLOSES) {
      return -2;
   }

   // --------------- test nesting too deep ----------------------------------
   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   for(int i = 1; i < 18; i++) {
      QCBOREncode_BstrWrap(&EC);
   }
   QCBOREncode_AddBool(&EC, true);

   for(int i = 1; i < 18; i++) {
      QCBOREncode_CloseBstrWrap(&EC, &Wrapped);
   }

   if(QCBOREncode_Finish(&EC, &Encoded2) != QCBOR_ERR_ARRAY_NESTING_TOO_DEEP) {
      return -3;
   }

   return 0;
}



// Part of bstr_wrap_nest_test
/*
 83 array with three
 53  byte string with 19 bytes
 01  #1
 50 byte string with 16 bytes
 02
 4D byte string with 13 bytes
 03
 4A byte string with 10 bytes
 04
 47 byte string with 7 bytes
 05
 44 byte string with 4 bytes
 06
 41 byte string with 1 byte
 07
 01
 02
 03
 04
 05
 06
 07
 A2 map with two items
 18 20  label for byte string
 54 byte string of length 20
 82 Array with two items
 10  The integer value 10
 A2 map with two items
 18 21 label for byte string
 44 byte string with 4 bytes
 81 array with 1 item
 11 integer value 11
 18 30 integer value 30
 18 40 integer label 40
 65 68 65 6C 6C 6F text string hello
 18 31 integer value 31
 18 41 integer label 41
 65 68 65 6C 6C 6F text string hello


 */


/*
 83                                      # array(3)
   56                                   # bytes(22)
      00530150024D034A0447054406410700010203040506 # "\x00S\x01P\x02M\x03J\x04G\x05D\x06A\a\x00\x01\x02\x03\x04\x05\x06"
   07                                   # unsigned(7)
   A2                                   # map(2)
      18 20                             # unsigned(32)
      54                                # bytes(20)
         8210A21821448111183018406568656C6C6F1831 # "\x82\x10\xA2\x18!D\x81\x11\x180\x18@ehello\x181"
      18 41                             # unsigned(65)
      65                                # text(5)
         68656C6C6F                     # "hello"
 */
static const uint8_t spExpectedDeepBstr[] =
{
   0x83, 0x56, 0x00, 0x53, 0x01, 0x50, 0x02, 0x4D,
   0x03, 0x4A, 0x04, 0x47, 0x05, 0x44, 0x06, 0x41,
   0x07, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
   0x07, 0xA2, 0x18, 0x20, 0x54, 0x82, 0x10, 0xA2,
   0x18, 0x21, 0x44, 0x81, 0x11, 0x18, 0x30, 0x18,
   0x40, 0x65, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x18,
   0x31, 0x18, 0x41, 0x65, 0x68, 0x65, 0x6C, 0x6C,
   0x6F
};

// Part of bstr_wrap_nest_test
static int DecodeNextNested(UsefulBufC Wrapped)
{
   int nReturn;
   QCBORDecodeContext DC;
   QCBORDecode_Init(&DC, Wrapped, QCBOR_DECODE_MODE_NORMAL);

   QCBORItem Item;
   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64) {
      return -12;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn == QCBOR_ERR_HIT_END || nReturn == QCBOR_ERR_NO_MORE_ITEMS) {
      return 0;
   }
   if(Item.uDataType != QCBOR_TYPE_BYTE_STRING) {
      return -13;
   }
   nReturn =  DecodeNextNested(Item.val.string);
   if(nReturn) {
      return nReturn;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -14;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64) {
      return -15;
   }

   if(QCBORDecode_Finish(&DC)) {
      return -16;
   }

   return 0;
}

// Part of bstr_wrap_nest_test
static int32_t DecodeNextNested2(UsefulBufC Wrapped)
{
   int nReturn;
   QCBORDecodeContext DC;
   QCBORDecode_Init(&DC, Wrapped, QCBOR_DECODE_MODE_NORMAL);

   QCBORItem Item;
   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_ARRAY) {
      return -12;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64) {
      return -12;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_MAP) {
      return 0;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_BYTE_STRING) {
      return -13;
   }
   nReturn =  DecodeNextNested2(Item.val.string);
   if(nReturn) {
      return nReturn;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING) {
      return -12;
   }
   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64) {
      return -12;
   }

   if(QCBORDecode_Finish(&DC)) {
      return -16;
   }

   return 0;
}


int32_t BstrWrapNestTest()
{
   QCBOREncodeContext EC;
   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   // ---- Make a complicated nested CBOR structure ---
#define BSTR_TEST_DEPTH 10

   QCBOREncode_OpenArray(&EC);

   for(int i = 0; i < BSTR_TEST_DEPTH-2; i++) {
      QCBOREncode_BstrWrap(&EC);
      QCBOREncode_AddInt64(&EC, i);
   }

   for(int i = 0; i < BSTR_TEST_DEPTH-2; i++) {
      QCBOREncode_CloseBstrWrap(&EC, NULL);
      QCBOREncode_AddInt64(&EC, i);
   }

   for(int i = 0; i < (BSTR_TEST_DEPTH-2)/3; i++) {
      QCBOREncode_OpenMap(&EC);
      QCBOREncode_BstrWrapInMapN(&EC, i+0x20);
      QCBOREncode_OpenArray(&EC);
      QCBOREncode_AddInt64(&EC, i+0x10);
   }

   for(int i = 0; i < (BSTR_TEST_DEPTH-2)/3; i++) {
      QCBOREncode_CloseArray(&EC);
      QCBOREncode_AddInt64(&EC, i+0x30);
      QCBOREncode_CloseBstrWrap(&EC, NULL);
      QCBOREncode_AddSZStringToMapN(&EC, i+0x40, "hello");
      QCBOREncode_CloseMap(&EC);
   }
   QCBOREncode_CloseArray(&EC);

   UsefulBufC Encoded;
   if(QCBOREncode_Finish(&EC, &Encoded)) {
      return -1;
   }

   // ---Compare it to expected. Expected was hand checked with use of CBOR playground ----
   if(UsefulBuf_Compare(UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spExpectedDeepBstr), Encoded)) {
      return -25;
   }


   // ---- Decode it and see if it is OK ------
   QCBORDecodeContext DC;
   QCBORDecode_Init(&DC, Encoded, QCBOR_DECODE_MODE_NORMAL);

   QCBORItem Item;
   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_ARRAY || Item.val.uCount != 3) {
      return -2;
   }

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_BYTE_STRING) {
      return -3;
   }

   int nReturn = DecodeNextNested(Item.val.string);
   if(nReturn) {
      return nReturn;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_INT64) {
      return -12;
   }

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_MAP || Item.val.uCount != 2) {
      return -2;
   }

   QCBORDecode_GetNext(&DC, &Item);
   if(Item.uDataType != QCBOR_TYPE_BYTE_STRING) {
      return -3;
   }
   nReturn = DecodeNextNested2(Item.val.string);
   if(nReturn) {
      return nReturn;
   }

   nReturn = QCBORDecode_GetNext(&DC, &Item);
   if(nReturn) {
      return -11;
   }
   if(Item.uDataType != QCBOR_TYPE_TEXT_STRING) {
      return -12;
   }

   if(QCBORDecode_Finish(&DC)) {
      return -16;
   }

   return 0;
}


static const uint8_t spCoseSign1Signature[] = {
   0x8e, 0xb3, 0x3e, 0x4c, 0xa3, 0x1d, 0x1c, 0x46, 0x5a, 0xb0,
   0x5a, 0xac, 0x34, 0xcc, 0x6b, 0x23, 0xd5, 0x8f, 0xef, 0x5c,
   0x08, 0x31, 0x06, 0xc4, 0xd2, 0x5a, 0x91, 0xae, 0xf0, 0xb0,
   0x11, 0x7e, 0x2a, 0xf9, 0xa2, 0x91, 0xaa, 0x32, 0xe1, 0x4a,
   0xb8, 0x34, 0xdc, 0x56, 0xed, 0x2a, 0x22, 0x34, 0x44, 0x54,
   0x7e, 0x01, 0xf1, 0x1d, 0x3b, 0x09, 0x16, 0xe5, 0xa4, 0xc3,
   0x45, 0xca, 0xcb, 0x36};

/*
 D2                                      # tag(18)
   84                                   # array(4)
      43                                # bytes(3)
         A10126                         # "\xA1\x01&"
      A1                                # map(1)
         04                             # unsigned(4)
         42                             # bytes(2)
            3131                        # "11"
      54                                # bytes(20)
         546869732069732074686520636F6E74656E742E # "This is the content."
      58 40                             # bytes(64)
         8EB33E4CA31D1C465AB05AAC34CC6B23D58FEF5C083106C4D25
         A91AEF0B0117E2AF9A291AA32E14AB834DC56ED2A223444547E
         01F11D3B0916E5A4C345CACB36     # "\x8E\xB3>L\xA3\x1D\x1CFZ\xB0Z\xAC4
                                           \xCCk#\xD5\x8F\xEF\b1\x06\xC4\xD2Z
                                           \x91\xAE\xF0\xB0\x11~*\xF9\xA2\x91
                                           \xAA2\xE1J\xB84\xDCV\xED*\"4DT~\x01
                                           \xF1\x1D;\t\x16\xE5\xA4\xC3E\xCA
                                           \xCB6"
 */
static const uint8_t spCoseSign1TBSExpected[] = {
   0xD2, 0x84, 0x43, 0xA1, 0x01, 0x26, 0xA1, 0x04, 0x42, 0x31,
   0x31, 0x54, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
   0x74, 0x68, 0x65, 0x20, 0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E,
   0x74, 0x2E, 0x58, 0x40, 0x8E, 0xB3, 0x3E, 0x4C, 0xA3, 0x1D,
   0x1C, 0x46, 0x5A, 0xB0, 0x5A, 0xAC, 0x34, 0xCC, 0x6B, 0x23,
   0xD5, 0x8F, 0xEF, 0x5C, 0x08, 0x31, 0x06, 0xC4, 0xD2, 0x5A,
   0x91, 0xAE, 0xF0, 0xB0, 0x11, 0x7E, 0x2A, 0xF9, 0xA2, 0x91,
   0xAA, 0x32, 0xE1, 0x4A, 0xB8, 0x34, 0xDC, 0x56, 0xED, 0x2A,
   0x22, 0x34, 0x44, 0x54, 0x7E, 0x01, 0xF1, 0x1D, 0x3B, 0x09,
   0x16, 0xE5, 0xA4, 0xC3, 0x45, 0xCA, 0xCB, 0x36};

static const uint8_t pProtectedHeaders[] = {0xa1, 0x01, 0x26};


/*
 This corresponds exactly to the example in RFC 8152 section
 C.2.1. This doesn't actually verify the signature though that would
 be nice as it would make the test really good. That would require
 bring in ECDSA crypto to this test.
 */
int32_t CoseSign1TBSTest()
{
   // All of this is from RFC 8152 C.2.1
   const char          *szKid     = "11";
   const UsefulBufC     Kid       = UsefulBuf_FromSZ(szKid);
   const char          *szPayload = "This is the content.";
   const UsefulBufC     Payload   = UsefulBuf_FromSZ(szPayload);
   const UsefulBufC     ProtectedHeaders = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(pProtectedHeaders);
   const UsefulBufC     Signature        = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spCoseSign1Signature);

   QCBOREncodeContext EC;
   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));

   // top level array for cose sign1, 18 is the tag for COSE sign
   QCBOREncode_AddTag(&EC, CBOR_TAG_COSE_SIGN1);
   QCBOREncode_OpenArray(&EC);

   // Add protected headers
   QCBOREncode_AddBytes(&EC, ProtectedHeaders);

   // Empty map with unprotected headers
   QCBOREncode_OpenMap(&EC);
   QCBOREncode_AddBytesToMapN(&EC, 4, Kid);
   QCBOREncode_CloseMap(&EC);

   // The payload
   UsefulBufC WrappedPayload;
   QCBOREncode_BstrWrap(&EC);
   // Payload is not actually CBOR in example C.2.1 like it would be
   // for a CWT or EAT. It is just a text string.
   QCBOREncode_AddEncoded(&EC, Payload);
   QCBOREncode_CloseBstrWrap(&EC, &WrappedPayload);

   // Check we got back the actual payload expected
   // The extra "T" is 0x54, which is the initial byte a bstr of length 20.
   if(UsefulBuf_Compare(WrappedPayload,
                        UsefulBuf_FROM_SZ_LITERAL("TThis is the content."))) {
      return -1;
   }

   // The signature
   QCBOREncode_AddBytes(&EC, Signature);
   QCBOREncode_CloseArray(&EC);

   // Finish and check the results
   UsefulBufC COSE_Sign1;
   if(QCBOREncode_Finish(&EC, &COSE_Sign1)) {
      return -2;
   }

   // 98 is the size from RFC 8152 C.2.1
   if(COSE_Sign1.len != 98) {
      return -3;
   }

   // It would be good to compare this to the output from a COSE
   // implementation like COSE-C. This has been checked against the
   // CBOR playground.
   if(CheckResults(COSE_Sign1, spCoseSign1TBSExpected)) {
      return -4;
   }

   return 0;
}


int32_t EncodeErrorTests()
{
   QCBOREncodeContext EC;


   // ------ Test for QCBOR_ERR_BUFFER_TOO_LARGE ------
   // Do all of these tests with NULL buffers so no actual
   // large allocations are neccesary
   const UsefulBuf Buffer = (UsefulBuf){NULL, UINT32_MAX};

   // First verify no error from a big buffer
   QCBOREncode_Init(&EC, Buffer);
   QCBOREncode_OpenArray(&EC);
   // 6 is the CBOR overhead for opening the array and encodng the length
   // This exactly fills the buffer.
   QCBOREncode_AddBytes(&EC, (UsefulBufC){NULL, UINT32_MAX-6});
   QCBOREncode_CloseArray(&EC);
   size_t xx;
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_SUCCESS) {
      return -1;
   }

   // Second verify error from an array in encoded output too large
   // Also test fetching the error code before finish
   QCBOREncode_Init(&EC, Buffer);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddBytes(&EC, (UsefulBufC){NULL, UINT32_MAX-6});
   QCBOREncode_OpenArray(&EC); // Where QCBOR internally encounters and records error
   if(QCBOREncode_GetErrorState(&EC) != QCBOR_ERR_BUFFER_TOO_LARGE) {
      // Error fetch failed.
      return -12;
   }
   QCBOREncode_CloseArray(&EC);
   QCBOREncode_CloseArray(&EC);
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_BUFFER_TOO_LARGE) {
      return -2;
   }

   // Third, fit an array in exactly at max position allowed
   QCBOREncode_Init(&EC, Buffer);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddBytes(&EC, (UsefulBufC){NULL, QCBOR_MAX_ARRAY_OFFSET-6});
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_CloseArray(&EC);
   QCBOREncode_CloseArray(&EC);
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_SUCCESS) {
      return -10;
   }


   // ----- QCBOR_ERR_BUFFER_TOO_SMALL --------------
   // Work close to the 4GB size limit for a better test
   const uint32_t uLargeSize =  UINT32_MAX - 1024;
   const UsefulBuf Large = (UsefulBuf){NULL,uLargeSize};

   QCBOREncode_Init(&EC, Large);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddBytes(&EC, (UsefulBufC){NULL, uLargeSize/2 + 1});
   QCBOREncode_CloseArray(&EC);
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_SUCCESS) {
      // Making sure it succeeds when it should first
      return -3;
   }

   QCBOREncode_Init(&EC, Large);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddBytes(&EC, (UsefulBufC){NULL, uLargeSize/2 + 1});
   QCBOREncode_AddBytes(&EC, (UsefulBufC){NULL, uLargeSize/2});
   QCBOREncode_CloseArray(&EC);
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_BUFFER_TOO_SMALL) {
      // Now just 1 byte over, see that it fails
      return -4;
   }


   // ----- QCBOR_ERR_ARRAY_NESTING_TOO_DEEP -------
   QCBOREncode_Init(&EC, Large);
   for(int i = QCBOR_MAX_ARRAY_NESTING; i > 0; i--) {
      QCBOREncode_OpenArray(&EC);
   }
   for(int i = QCBOR_MAX_ARRAY_NESTING; i > 0; i--) {
      QCBOREncode_CloseArray(&EC);
   }
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_SUCCESS) {
      // Making sure it succeeds when it should first
      return -5;
   }

   QCBOREncode_Init(&EC, Large);
   for(int i = QCBOR_MAX_ARRAY_NESTING+1; i > 0; i--) {
      QCBOREncode_OpenArray(&EC);
   }
   for(int i = QCBOR_MAX_ARRAY_NESTING+1; i > 0; i--) {
      QCBOREncode_CloseArray(&EC);
   }
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_ARRAY_NESTING_TOO_DEEP) {
      // One more level to cause error
      return -6;
   }


   // ------ QCBOR_ERR_TOO_MANY_CLOSES --------
   QCBOREncode_Init(&EC, Large);
   for(int i = QCBOR_MAX_ARRAY_NESTING; i > 0; i--) {
      QCBOREncode_OpenArray(&EC);
   }
   for(int i = QCBOR_MAX_ARRAY_NESTING+1; i > 0; i--) {
      QCBOREncode_CloseArray(&EC);
   }
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_TOO_MANY_CLOSES) {
      // One more level to cause error
      return -7;
   }


   // ------ QCBOR_ERR_CLOSE_MISMATCH --------
   QCBOREncode_Init(&EC, Large);
   QCBOREncode_OpenArray(&EC);
   UsefulBufC Wrap;
   QCBOREncode_CloseBstrWrap(&EC, &Wrap);
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_CLOSE_MISMATCH) {
      return -8;
   }


   // ------ QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN ---------
   QCBOREncode_Init(&EC, Large);
   for(int i = QCBOR_MAX_ARRAY_NESTING; i > 0; i--) {
      QCBOREncode_OpenArray(&EC);
   }
   for(int i = QCBOR_MAX_ARRAY_NESTING-1; i > 0; i--) {
      QCBOREncode_CloseArray(&EC);
   }
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_ARRAY_OR_MAP_STILL_OPEN) {
      // One more level to cause error
      return -9;
   }

   /* QCBOR_ERR_ARRAY_TOO_LONG is not tested here as
    it would require a 64KB of RAM to test */


   // ----- Test the check for NULL buffer ------
   QCBOREncode_Init(&EC, Buffer);
   if(QCBOREncode_IsBufferNULL(&EC) == 0) {
      return -11;
   }

   // ------ QCBOR_ERR_UNSUPPORTED --------
   QCBOREncode_Init(&EC, Large);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddSimple(&EC, 24); // CBOR_SIMPLEV_RESERVED_START
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_UNSUPPORTED) {
      return -12;
   }

   QCBOREncode_Init(&EC, Large);
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddSimple(&EC, 31); // CBOR_SIMPLEV_RESERVED_END
   if(QCBOREncode_FinishGetSize(&EC, &xx) != QCBOR_ERR_UNSUPPORTED) {
      return -13;
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
      5([-9223372036854775808, -4759477275222530853137])
 ]
 */
static const uint8_t spExpectedExponentAndMantissaArray[] = {
   0x86, 0xC4, 0x82, 0x20, 0x03, 0xC4, 0x82, 0x33,
   0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
   0x07, 0x08, 0x09, 0x10, 0xC4, 0x82, 0x1B, 0x7F,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3,
   0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
   0x08, 0x09, 0x10, 0xC5, 0x82, 0x19, 0x01, 0x2C,
   0x18, 0x64, 0xC5, 0x82, 0x33, 0xC2, 0x4A, 0x01,
   0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
   0x10, 0xC5, 0x82, 0x3B, 0x7F, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0x4A, 0x01, 0x02,
   0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10};


/*
  {
    "decimal fraction": 4([-1, 3]),
    300: 4([-1, 3]),
    "decimal fraction bignum postive": 4([-200, 4759477275222530853136]),
    400: 4([2147483647, 4759477275222530853136]),
    "decimal fraction bignum negative": 4([9223372036854775807, -4759477275222530853137]),
    500: 4([9223372036854775807, -4759477275222530853137]),
    "big float": 5([300, 100]),
    600: 5([300, 100]),
    "big float bignum positive": 5([-20, 4759477275222530853136]),
    700: 5([-20, 4759477275222530853136]),
    "big float bignum negative": 5([-9223372036854775808, -4759477275222530853137]),
    800: 5([-9223372036854775808, -4759477275222530853137])
  }
 */
static const uint8_t spExpectedExponentAndMantissaMap[] = {
   0xAC, 0x70, 0x64, 0x65, 0x63, 0x69, 0x6D, 0x61,
   0x6C, 0x20, 0x66, 0x72, 0x61, 0x63, 0x74, 0x69,
   0x6F, 0x6E, 0xC4, 0x82, 0x20, 0x03, 0x19, 0x01,
   0x2C, 0xC4, 0x82, 0x20, 0x03, 0x78, 0x1F, 0x64,
   0x65, 0x63, 0x69, 0x6D, 0x61, 0x6C, 0x20, 0x66,
   0x72, 0x61, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20,
   0x62, 0x69, 0x67, 0x6E, 0x75, 0x6D, 0x20, 0x70,
   0x6F, 0x73, 0x74, 0x69, 0x76, 0x65, 0xC4, 0x82,
   0x38, 0xC7, 0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04,
   0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x19, 0x01,
   0x90, 0xC4, 0x82, 0x1A, 0x7F, 0xFF, 0xFF, 0xFF,
   0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
   0x07, 0x08, 0x09, 0x10, 0x78, 0x20, 0x64, 0x65,
   0x63, 0x69, 0x6D, 0x61, 0x6C, 0x20, 0x66, 0x72,
   0x61, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x62,
   0x69, 0x67, 0x6E, 0x75, 0x6D, 0x20, 0x6E, 0x65,
   0x67, 0x61, 0x74, 0x69, 0x76, 0x65, 0xC4, 0x82,
   0x1B, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xC3, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05,
   0x06, 0x07, 0x08, 0x09, 0x10, 0x19, 0x01, 0xF4,
   0xC4, 0x82, 0x1B, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xC3, 0x4A, 0x01, 0x02, 0x03,
   0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x69,
   0x62, 0x69, 0x67, 0x20, 0x66, 0x6C, 0x6F, 0x61,
   0x74, 0xC5, 0x82, 0x19, 0x01, 0x2C, 0x18, 0x64,
   0x19, 0x02, 0x58, 0xC5, 0x82, 0x19, 0x01, 0x2C,
   0x18, 0x64, 0x78, 0x19, 0x62, 0x69, 0x67, 0x20,
   0x66, 0x6C, 0x6F, 0x61, 0x74, 0x20, 0x62, 0x69,
   0x67, 0x6E, 0x75, 0x6D, 0x20, 0x70, 0x6F, 0x73,
   0x69, 0x74, 0x69, 0x76, 0x65, 0xC5, 0x82, 0x33,
   0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
   0x07, 0x08, 0x09, 0x10, 0x19, 0x02, 0xBC, 0xC5,
   0x82, 0x33, 0xC2, 0x4A, 0x01, 0x02, 0x03, 0x04,
   0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x78, 0x19,
   0x62, 0x69, 0x67, 0x20, 0x66, 0x6C, 0x6F, 0x61,
   0x74, 0x20, 0x62, 0x69, 0x67, 0x6E, 0x75, 0x6D,
   0x20, 0x6E, 0x65, 0x67, 0x61, 0x74, 0x69, 0x76,
   0x65, 0xC5, 0x82, 0x3B, 0x7F, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0x4A, 0x01, 0x02,
   0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
   0x19, 0x03, 0x20, 0xC5, 0x82, 0x3B, 0x7F, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0x4A,
   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
   0x09, 0x10
};


int32_t ExponentAndMantissaEncodeTests()
{
   QCBOREncodeContext EC;
   UsefulBufC         EncodedExponentAndMantissa;

   // Constant for the big number used in all the tests.
   static const uint8_t spBigNum[] = {0x01, 0x02, 0x03, 0x04, 0x05,
                                      0x06, 0x07, 0x08, 0x09, 0x010};
   const UsefulBufC   BigNum = UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spBigNum);

   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenArray(&EC);
   QCBOREncode_AddDecimalFraction(&EC, 3, -1); // 3 * (10 ^ -1)
   QCBOREncode_AddDecimalFractionBigNum(&EC, BigNum , false, -20);
   QCBOREncode_AddDecimalFractionBigNum(&EC, BigNum, true, INT64_MAX);
   QCBOREncode_AddBigFloat(&EC, 100, 300);
   QCBOREncode_AddBigFloatBigNum(&EC, BigNum, false, -20);
   QCBOREncode_AddBigFloatBigNum(&EC, BigNum, true, INT64_MIN);
   QCBOREncode_CloseArray(&EC);

   if(QCBOREncode_Finish(&EC, &EncodedExponentAndMantissa)) {
      return -2;
   }

   int nReturn = UsefulBuf_CompareWithDiagnostic(EncodedExponentAndMantissa,
                                                 UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spExpectedExponentAndMantissaArray),
                                                 NULL);
   if(nReturn) {
      return nReturn;
   }

   QCBOREncode_Init(&EC, UsefulBuf_FROM_BYTE_ARRAY(spBigBuf));
   QCBOREncode_OpenMap(&EC);

   QCBOREncode_AddDecimalFractionToMap(&EC, "decimal fraction", 3, -1);

   QCBOREncode_AddDecimalFractionToMapN(&EC, 300, 3, -1);

   QCBOREncode_AddDecimalFractionBigNumToMap(&EC,
                                             "decimal fraction bignum postive",
                                             BigNum,
                                             false,
                                             -200);

   QCBOREncode_AddDecimalFractionBigNumToMapN(&EC,
                                              400,
                                              BigNum,
                                              false,
                                              INT32_MAX);

   QCBOREncode_AddDecimalFractionBigNumToMap(&EC,
                                             "decimal fraction bignum negative",
                                             BigNum,
                                             true,
                                             INT64_MAX);

   QCBOREncode_AddDecimalFractionBigNumToMapN(&EC,
                                              500,
                                              BigNum,
                                              true,
                                              INT64_MAX);

   QCBOREncode_AddBigFloatToMap(&EC, "big float", 100, 300);

   QCBOREncode_AddBigFloatToMapN(&EC, 600, 100, 300);

   QCBOREncode_AddBigFloatBigNumToMap(&EC,
                                      "big float bignum positive",
                                      BigNum,
                                      false,
                                      -20);

   QCBOREncode_AddBigFloatBigNumToMapN(&EC,
                                       700,
                                       BigNum,
                                       false,
                                       -20);

   QCBOREncode_AddBigFloatBigNumToMap(&EC,
                                      "big float bignum negative",
                                      BigNum,
                                      true,
                                      INT64_MIN);

   QCBOREncode_AddBigFloatBigNumToMapN(&EC,
                                       800,
                                       BigNum,
                                       true,
                                       INT64_MIN);

   QCBOREncode_CloseMap(&EC);

   if(QCBOREncode_Finish(&EC, &EncodedExponentAndMantissa)) {
      return -3;
   }


   struct UBCompareDiagnostic Diag;

   nReturn = UsefulBuf_CompareWithDiagnostic(EncodedExponentAndMantissa,
                                             UsefulBuf_FROM_BYTE_ARRAY_LITERAL(spExpectedExponentAndMantissaMap),
                                             &Diag);
   if(nReturn) {
      return nReturn + 1000000; // +1000000 to distinguish from first test above
   }

   return 0;
}

#endif /* QCBOR_CONFIG_DISABLE_EXP_AND_MANTISSA */
