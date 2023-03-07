/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

/*
 *  ======== TransportFile.c ========
 */

/* XDC include files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <stdio.h>
#include <string.h>

/* UIA include files */
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/Transport.h>

#ifdef xdc_target__isaCompatible_64P
extern cregister volatile UInt DNUM;
#else
#define DNUM 0
#endif

#define DEFAULTFILE "C:/temp/UIADump"

/* IAR Normal dlib does not have file descriptor support */
#if defined(__IAR_SYSTEMS_ICC__) && !(_DLIB_FILE_DESCRIPTOR)
#define FILE Int
#define fopen(A, B) NULL
#define fwrite(A, B, C, D) 0
#define fflush(stdout)
#define fclose(A)
#endif


Char TransportFile_fileName[80];
Bool TransportFile_useDefaultFileName = TRUE;

/*
 *  ======== TransportFile_setFile ========
 */
Void TransportFile_setFile(String fileName)
{
    strncpy(TransportFile_fileName, fileName, 80);
    TransportFile_useDefaultFileName = FALSE;
}

/*
 *  ======== TransportFile_init ========
 */
Void TransportFile_init()
{
}

/*
 *  ======== TransportFile_start ========
 */
Ptr TransportFile_start(UIAPacket_HdrType hdrType)
{
    FILE  *TransportFile_file;
    if (hdrType != UIAPacket_HdrType_EventPkt) {
        return (NULL);
    }
    if (TransportFile_useDefaultFileName == TRUE) {
        sprintf(TransportFile_fileName, "%s%d.bin", DEFAULTFILE,
            (Int)DNUM);
    }

    System_printf("Filename = %s\n", TransportFile_fileName);
    TransportFile_file = fopen(TransportFile_fileName, "wb");

    return (TransportFile_file);
}

/*
 *  ======== TransportFile_recv ========
 */
SizeT TransportFile_recv(Ptr handle, UIAPacket_Hdr **packet, SizeT size)
{
    /* Receiving is currently not supported */
    return (0);
}

/*
 *  ======== TransportFile_send ========
 */
SizeT TransportFile_send(Ptr handle, UIAPacket_Hdr **packet)
{
    SizeT writeSize;
    SizeT eventSize;
    Bool successFlag = TRUE;

    if (UIAPacket_getHdrType(*packet) == UIAPacket_HdrType_Msg) {
        successFlag = FALSE;
    }
    else {
        eventSize = UIAPacket_getEventLength(*packet);
        writeSize = fwrite(*packet, 4, 4, handle);
        writeSize *= 4;
        writeSize += fwrite((Char *)((Char *)*packet + 16), 1, eventSize - 16,
                handle);
        if (writeSize != eventSize) {
            successFlag = FALSE;
        }
        else {
            fflush(handle);
        }
    }

    return (successFlag);
}

/*
 *  ======== TransportFile_stop ========
 */
Void TransportFile_stop(Ptr handle)
{
    if (handle != NULL) {
        fclose(handle);
    }
}

/*
 *  ======== TransportFile_exit ========
 */
Void TransportFile_exit()
{
}
