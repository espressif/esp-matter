/******************************************************************************

 @file  oad_image_header_stack.c

 @brief OAD image header definition file.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include <stddef.h>
#include "hal_types.h"
#include "icall.h"
#include "oad_image_header.h"

/*******************************************************************************
 * LOCAL VARIABLES
 */

extern void startup_entry( const ICall_RemoteTaskArg *arg0, void *arg1 );

#ifdef __TI_COMPILER_VERSION__
/* These symbols are created by the linker file */
extern uint8_t ramStartHere;
extern uint8_t entryAddr;
extern uint8_t ramEndHere;
extern uint8_t flashEndAddr;
#endif

#ifdef __IAR_SYSTEMS_ICC__
#pragma section = "ROSTK"
#pragma section = "RWDATA"
#pragma section = "ENTRY"
#ifdef OSAL_SNV
extern const uint32_t ENTRY_END;
extern const uint32_t STACK_END;
#endif
#endif

#ifdef __TI_COMPILER_VERSION__
#pragma DATA_SECTION(_imgHdr, ".image_header")
#pragma RETAIN(_imgHdr)
const imgHdr_t _imgHdr =
#elif  defined(__IAR_SYSTEMS_ICC__)
#pragma location=".image_header"
const imgHdr_t _imgHdr @ ".image_header" =
#endif
{
  .imgID = OAD_IMG_ID_VAL,
  .crc32 = 0xFFFFFFFF,
  .bimVer = 0x01,
  .metaVer = 0x01,                        //!< Metadata version */
  .techType = 0xFE,                       //!< Wireless protocol type BLE/TI-MAC/ZIGBEE etc. */
  .imgCpStat = 0xFF,                      //!< Image copy status bytes */
  .crcStat = 0xFF,                        //!< CRC status */
  .imgType =  0x2,                        //!< Image Type */
  .imgNo = 0x1,                           //!< Image number of 'image type' */
  .imgVld = 0xFFFFFFFF,                   //!< In indicates if the current image in valid 0xff - valid, 0x00 invalid image */
  .len = 0xFFFFFFFF,                      //!< Image length in bytes. */
  .prgEntry = (uint32_t)&startup_entry,   //!< Program entry address */
  .softVer = SOFTWARE_VER,                //!< Software version of the image */
  .hdrLen = offsetof(imgHdr_t, rfu2) + sizeof(((imgHdr_t){0}).rfu2),   //!< Total length of the image header */
  .rfu2 = 0xFFFF,                         //!< reserved bytes */
  .segTypeBd = 0x0,
  .wirelessTech1 = 0xFE,
  .rfu3 = 0xFF,
  .boundarySegLen = 0x18,
#ifdef __TI_COMPILER_VERSION__
  .stackStartAddr = (uint32_t)&(_imgHdr),    //!< Stack start address */
  .stackEntryAddr = (uint32)&startup_entry,
  .ram0StartAddr = (uint32_t) &ramStartHere, //!< RAM entry start address */
  .ram0EndAddr = (uint32_t) &ramEndHere,
  .imgEndAddr = (uint32)&flashEndAddr,
#elif  defined(__IAR_SYSTEMS_ICC__)
  .stackStartAddr = (uint32)(__section_begin("ROSTK")), //!< Stack start adddress */
  .stackEntryAddr = (uint32_t)&startup_entry,
  .ram0StartAddr = (uint32)(__section_begin("RWDATA")), //!< RAM entry start address */
  .ram0EndAddr = (uint32)(__section_end("RWDATA")),
#ifdef INCLUDE_SNV
  .imgEndAddr = (uint32_t)&(STACK_END),    //!< Address of the last byte of a contiguous image */
#else
  .imgEndAddr = (uint32)&(ENTRY_END),
#endif /* INCLUDE_SNV */
#endif /* __TI_COMPILER_VERSION__ */
  .segTypeImg = 1,
  .wirelessTech2  = 0xFE,
  .rfu4 =0xFF,
  .startAddr = (uint32_t)&(_imgHdr.imgID)
 };
