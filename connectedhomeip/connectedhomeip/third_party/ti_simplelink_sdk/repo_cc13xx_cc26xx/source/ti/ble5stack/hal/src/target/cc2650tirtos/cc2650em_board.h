/******************************************************************************

 @file  cc2650em_board.h

 @brief This file is a simple gateway to include the appropriate Board.h file
        which is located in the following directories relative to this file:
        CC2650_7ID
        CC2650_5XD
        CC2650_4XS
        CC1310_7ID_7793

        The project should set the include path to Board.h to point to the
        Board.h in one of these 4 directories. This
        Board.h file will then define a symbol which is used in this file to
        include the appropriate Board.c file which is found in the same
        directory as Board.h
        This way the project can look the same (and only include this Board.c)
        file, when changing EM user only needs to update include path in the
        project options. Alternatively, the device specific board files can
        just be included directly in the project.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

#ifndef SDK_BOARD_H
#define SDK_BOARD_H

/*
*   The location of this Board.h file depends on your project include path.
*   Set it correctly to point to your CC2650DK_xxx
*/
#if defined(CC2650DK_7ID)
    #include <ti/boards/CC2650DK_7ID/Board.h>
#elif defined(CC2650DK_5XD)
    #include <ti/boards/CC2650DK_5XD/Board.h>
#elif defined(CC2650DK_4XS)
    #include <ti/boards/CC2650DK_4XS/Board.h>
#elif defined(CC1350DK_7XD)
    // Note: There currently isn't a directory that corresponds to the CC1350EM
    //       but this device is digitally identical to the CC2650EM. Until a
    //       board directory is added for CC1350DK_7XD, we'll remap to the
    //       CC2650DK_7ID board. The proper front end setup (7XD) will be
    //       correctly handled based on the board define CC2650EM_7ID.
    #include <ti/boards/CC2650DK_7ID/Board.h>
#elif defined(CC1310DK_7XD)
    #include "CC1310DK_7XD/Board.h"
#else
    #error "Must define either 'CC2650DK_7ID', 'CC2650DK_5XD', 'CC2650DK_4XS', or 'CC1310DK_7XD'. Please set include path to point to appropriate device."
#endif



#endif /* SDK_BOARD_H */
