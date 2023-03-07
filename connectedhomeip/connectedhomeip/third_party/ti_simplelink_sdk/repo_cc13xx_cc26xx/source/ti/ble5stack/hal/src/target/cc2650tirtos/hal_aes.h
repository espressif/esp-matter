/******************************************************************************

 @file  hal_aes.h

 @brief Support for HW/SW AES encryption.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2007-2022, Texas Instruments Incorporated
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

#ifndef     HAL_AES_H_
#define     HAL_AES_H_

#include "zcomdef.h"
#include <inc/hw_types.h>
#include "hal_board.h"
#include <driverlib/crypto.h>

/* AES and Keystore functions */
extern void HalAesInit( void );
extern void ssp_HW_KeyInit (uint8 *);
extern void sspAesDecryptHW (uint8 *, uint8 *);
extern void sspAesEncryptHW_keylocation( uint8 *, uint8 *, uint8 );
extern void sspAesDecryptHW_keylocation( uint8 *, uint8 *, uint8 );
extern void (*pSspAesEncrypt)( uint8 *, uint8 * );
extern void sspAesEncryptHW (uint8 *, uint8 *);

#define     STATE_BLENGTH   16      // Number of bytes in State
#define     KEY_BLENGTH     16      // Number of bytes in Key
#define     KEY_EXP_LENGTH  176     // Nb * (Nr+1) * 4

/* AES Engine is default to hardware AES. To turn on software AES, #define one of the followings:
 * #define SOFTWARE_AES TRUE, uses software aes  ( slowest setting )
 * #define SW_AES_AND_KEY_EXP TRUE, enables software aes with key expansion ( improves speed at the cost of 176 bytes of data (RAM) )
 */
#if ((defined SOFTWARE_AES) && (SOFTWARE_AES == TRUE)) && ((defined SW_AES_AND_KEY_EXP) && (SW_AES_AND_KEY_EXP == TRUE))
#error "SOFTWARE_AES and SW_AES_AND_KEY_EXP cannot be both defined."
#endif

extern void HalAesInit( void );
extern uint8 AesLoadKey( uint8 * );

extern void (*pSspAesEncrypt)( uint8 *, uint8 * );
extern void ssp_HW_KeyInit (uint8 *);


extern void sspAesEncryptHW (uint8 *, uint8 *);

#define AES_BUSY    0x08
#define ENCRYPT     0x00
#define DECRYPT     0x01

// _mode_ is one of
#define CBC         0x00000020
#define CTR         0x00000040
#define ECB         0x1FFFFFE0
#define CBC_MAC     0x00008000
#define CCM         0x00040000

// Macro for setting the mode of the AES operation
#define AES_SETMODE_ECB do { HWREG(AES_AES_CTRL) &= ~ECB; } while (0)
#define AES_SETMODE(mode) do { HWREG(AES_AES_CTRL) &= ~mode; HW_REG(AES_AES_CTRL) |= mode} while (0)



#endif  // HAL_AES_H_
