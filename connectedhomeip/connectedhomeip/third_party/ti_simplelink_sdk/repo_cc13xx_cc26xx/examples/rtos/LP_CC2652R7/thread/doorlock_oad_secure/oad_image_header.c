/******************************************************************************

 @file  oad_image_header.c

 @brief OAD image header definition file.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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
#define OAD_SINGLE_APP  1
#include "oad_image_header.h"


#define MY_APP_SOFTWARE_VER   { '0', '0', '0', '1' }

/*******************************************************************************
 * LOCAL VARIABLES
 */

#if defined(__GNUC__)
#define FLASH_END_ADDRESS        ((uint32_t)(&flash_end_address))
#define RESET_VECT_ADDRESS       ((uint32_t)(&ti_sysbios_family_arm_m3_Hwi_resetVectors[0]))

extern const uint32_t ti_sysbios_family_arm_m3_Hwi_resetVectors[];

const uint8_t __attribute__((section( ".flash_end_address"))) __attribute__((used)) flash_end_address = 1;
const struct img_header_single_app __attribute__((section( ".oad_image_header"))) __attribute__((used)) oad_image_header =
#endif

#if defined(__TI_COMPILER_VERSION__)
#define FLASH_END_ADDRESS        ((uint32_t)(&flash_end_address))
#define RESET_VECT_ADDRESS       ((uint32_t)(&ti_sysbios_family_arm_m3_Hwi_resetVectors[0]))

extern const uint32_t ti_sysbios_family_arm_m3_Hwi_resetVectors[];

const uint8_t flash_end_address = 1;
#pragma DATA_SECTION(flash_end_address, ".flash_end_address" )
#pragma RETAIN(flash_end_address)

#pragma DATA_SECTION(oad_image_header, ".oad_image_header" )
#pragma RETAIN(oad_image_header)
const struct img_header_single_app  oad_image_header =
#endif

#if defined(__IAR_SYSTEMS_ICC__)
#define FLASH_END_ADDRESS        ((uint32_t)(&flash_end_address))
#define RESET_VECT_ADDRESS       ((uint32_t)(&__vector_table))

__root extern const unsigned int __vector_table;
__root const uint8_t flash_end_address @ ".flash_end_address" = 1;
__root const struct img_header_single_app  oad_image_header @ ".oad_image_header" =
#endif

{
    /* signature for verification */
    .h.imgID            = OAD_IMG_ID_VAL,
    /* filled in by the tool */
    .h.crc32            = 0xFFFFFFFF,

    /* Expected BIM version number */
    .h.bimVer           = BIM_V3_VER,

    /* external meta data version number */
    .h.metaVer          = 0x01,

    /* type of application */
    .h.techType         = OAD_WIRELESS_TECH_THREAD,

    /* image copy status */
    .h.imgCpStat        = DEFAULT_STATE,

    /* CRC calculation state */
    .h.crcStat          = DEFAULT_STATE,

    /* What is this for? What #define should this use */
    .h.imgType          =  OAD_IMG_TYPE_APP,

    /* What is this for? */
    .h.imgNo            = 0x0,

    /* if 0xff - image is valid, if 0x00 - image is invalid */
    .h.imgVld           = 0xFF,

    /* image length in bytes filled in by tool */
    .h.len              = 0xFFFFFFFF,

    /* Points to the CM4 vector table */
    .h.prgEntry         = RESET_VECT_ADDRESS,

    /* Software version kept in the OAD header */
    .h.softVer          = MY_APP_SOFTWARE_VER,

    /* last byte of this image */
    .h.imgEndAddr       = FLASH_END_ADDRESS,

    /* how long is this header? */
    .h.hdrLen           = sizeof(struct img_common_header),

    /* reserved */
    .h.rfu              = 0xFFFF,   //!< reserved bytes */
#if (defined(SECURITY))
    .s.segTypeSecure    = IMG_SECURITY_SEG_ID,
    .s.wirelessTech     = OAD_WIRELESS_TECH_THREAD,
    .s.verifStat        = DEFAULT_STATE,
    .s.secSegLen        = 0x55,
    .s.secVer           = SECURITY_VER,                     /* Image payload and length */
    .s.secTimestamp     = 0x0,                              /* Security timestamp */
    .s.secSignerInfo    = 0x0,
#endif
    /* our payload segment */
    .p.segType          = IMG_PAYLOAD_SEG_ID,

    .p.wirelessTech     = OAD_WIRELESS_TECH_THREAD,
    .p.rfu              = 0,
    .p.imgSegLen        = 0, /* filled in by the python script */
    .p.startAddr        = (uint32_t)(&oad_image_header) /* this should be at address 0 */
};
