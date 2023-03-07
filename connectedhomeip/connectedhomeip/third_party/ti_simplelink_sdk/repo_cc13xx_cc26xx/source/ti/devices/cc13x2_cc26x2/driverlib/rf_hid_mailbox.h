/******************************************************************************
*  Filename:       rf_hid_mailbox.h
*
*  Description:    Definitions for HID interface
*
*  Copyright (c) 2015 - 2020, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef _HID_MAILBOX_H
#define _HID_MAILBOX_H

/// \name Radio operation status
///@{
/// \name Operation finished normally
///@{
#define HID_DONE_OK            0x5400  ///< Operation ended normally
#define HID_DONE_RXTIMEOUT     0x5401  ///< Operation stopped after end trigger while waiting for sync
#define HID_DONE_NOSYNC        0x5402  ///< Timeout of subsequent Rx
#define HID_DONE_RXERR         0x5403  ///< Operation ended after CRC error
#define HID_DONE_ENDED         0x5404  ///< Operation stopped after end trigger during reception
#define HID_DONE_STOPPED       0x5405  ///< Operation stopped after stop command
#define HID_DONE_ABORT         0x5406  ///< Operation aborted by abort command
///@}
/// \name Operation finished with error
///@{
#define HID_ERROR_PAR          0x5800  ///< Illegal parameter
#define HID_ERROR_RXBUF        0x5801  ///< No available Rx buffer at the start of a packet
#define HID_ERROR_NO_SETUP     0x5802  ///< Radio was not set up in a compatible mode
#define HID_ERROR_NO_FS        0x5803  ///< Synth was not programmed when running Rx or Tx
#define HID_ERROR_RXOVF        0x5804  ///< Rx overflow observed during operation
#define HID_ERROR_TXUNF        0x5805  ///< Tx underflow observed during operation
///@}
///@}

#endif
