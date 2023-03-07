/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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
 */
/*
 *  ======== FlashCC26X4_ns.c ========
 */
#include "FlashCC26X4_ns.h"

#include "FlashCC26X4_s.h"

#include <psa/client.h>
#include <psa_manifest/sid.h>

#include <xdc/std.h>

static psa_handle_t spHandle = 0;

static struct psa_invec invecs[1];
static struct psa_outvec outvecs[1];

static struct FlashCC26X4_s_program flashProgramArgs;

/*
 *  ======== FlashOpen ========
 */
int_fast16_t FlashOpen(void)
{
    int_fast16_t status = 0;

    /* if no connection exists, connect */
    if (spHandle <= 0)
    {
        /* Connect to the Flash Service */
        spHandle = psa_connect(FLASH_SP_SERVICE_SID, psa_version(FLASH_SP_SERVICE_SID));

        if (spHandle <= 0)
        {
            /* error connecting */
            status = (int_fast16_t)spHandle;
        }
    }

    return (status);
}

/*
 *  ======== FlashClose ========
 */
void FlashClose(void)
{
    /* if connection handle exists, close handle */
    if (spHandle > 0)
    {
        psa_close(spHandle);
        spHandle = 0;
    }
}

/*
 *  ======== FlashProtectionGet ========
 */
uint32_t FlashProtectionGet(uint32_t ui32SectorAddress)
{
    /* if there is no PSA Connection, connect */
    if (spHandle <= 0)
    {
        FlashOpen();
        if (spHandle <= 0)
        {
            return (spHandle);
        }
    }

    /* set the input vector */
    invecs[0].base = &ui32SectorAddress;
    invecs[0].len = sizeof(uint32_t);

    /* set the output vector */
    uint32_t ui32SectorProtect = 0;
    outvecs[0].base = &ui32SectorProtect;
    outvecs[0].len = sizeof(uint32_t);

    /* call FlashProtectionGet command */
    psa_status_t err = psa_call(spHandle, FLASH_SP_MSG_TYPE_PROTECTION_GET, invecs, 1, outvecs, 1);

    if (err)
    {
        /* return PSA error code */
        return (err);
    }
    else
    {
        /* return FlashProtectionGet status code */
        return (ui32SectorProtect);
    }
}


/*
 *  ======== FlashSectorErase ========
 */
uint32_t FlashSectorErase(uint32_t ui32SectorAddress)
{
    /* if there is no PSA Connection, connect */
    if (spHandle <= 0)
    {
        FlashOpen();
        if (spHandle <= 0)
        {
            return (spHandle);
        }
    }

    /* set the input vector */
    invecs[0].base = &ui32SectorAddress;
    invecs[0].len = sizeof(uint32_t);

    /* set the output vector */
    uint32_t status = 0;
    outvecs[0].base = &status;
    outvecs[0].len = sizeof(uint32_t);

    /* call FlashSectorErase command */
    psa_status_t err = psa_call(spHandle, FLASH_SP_MSG_TYPE_SECTOR_ERASE, invecs, 1, outvecs, 1);

    if (err)
    {
        /* return PSA error code */
        return (err);
    }
    else
    {
        /* return FlashSectorErase status code */
        return (status);
    }
}

/*
 *  ======== FlashProgram ========
 */
uint32_t FlashProgram(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count)
{
    /* if there is no PSA Connection, connect */
    if (spHandle <= 0)
    {
        FlashOpen();
        if (spHandle <= 0)
        {
            return (spHandle);
        }
    }

    /* construct structure containing all input arguments */
    flashProgramArgs.dataBuffer = pui8DataBuffer;
    flashProgramArgs.sectorAddress = ui32Address;
    flashProgramArgs.count = ui32Count;

    /* set the input vector */
    invecs[0].base = &flashProgramArgs;
    invecs[0].len = sizeof(flashProgramArgs);

    /* set the output vector */
    uint32_t status = 0;
    outvecs[0].base = &status;
    outvecs[0].len = sizeof(uint32_t);

    /* call FlashProgram command */
    psa_status_t err = psa_call(spHandle, FLASH_SP_MSG_TYPE_PROGRAM, invecs, 1, outvecs, 1);

    if (err)
    {
        /* return PSA error code */
        return (err);
    }
    else
    {
        /* return FlashProgram status code */
        return (status);
    }
}

/*
 *  ======== FlashProgram4X ========
 */
uint32_t FlashProgram4X(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count)
{
    /* if there is no PSA Connection, connect */
    if (spHandle <= 0)
    {
        FlashOpen();
        if (spHandle <= 0)
        {
            return (spHandle);
        }
    }

    /* construct structure containing all input arguments */
    flashProgramArgs.dataBuffer = pui8DataBuffer;
    flashProgramArgs.sectorAddress = ui32Address;
    flashProgramArgs.count = ui32Count;
    
    /* set the input vector */
    invecs[0].base = &flashProgramArgs;
    invecs[0].len = sizeof(flashProgramArgs);

    /* set the output vector */
    uint32_t status = 0;
    outvecs[0].base = &status;
    outvecs[0].len = sizeof(uint32_t);

    /* call FlashProgram4X command */
    psa_status_t err = psa_call(spHandle, FLASH_SP_MSG_TYPE_PROGRAM_4X, invecs, 1, outvecs, 1);

    if (err)
    {
        /* return PSA error code */
        return (err);
    }
    else
    {
        /* return FlashProgram4X status code */
        return (status);
    }
}


/*
 *  ======== FlashSectorSizeGet ========
 */
uint32_t FlashSectorSizeGet(void)
{
    /* if there is no PSA Connection, connect */
    if (spHandle <= 0)
    {
        FlashOpen();
        if (spHandle <= 0)
        {
            return (spHandle);
        }
    }

    /* set the output vector */
    uint32_t status = 0;
    outvecs[0].base = &status;
    outvecs[0].len = sizeof(uint32_t);

    /* call FlashSectorSizeGet command */
    psa_status_t err = psa_call(spHandle, FLASH_SP_MSG_TYPE_SECTOR_SIZE_GET, invecs, 0, outvecs, 1);

    if (err)
    {
        /* return PSA error code */
        return (err);
    }
    else
    {
        /* return FlashSectorSizeGet status code */
        return (status);
    }
}

/*
 *  ======== FlashSizeGet ========
 */
uint32_t FlashSizeGet(void)
{
    /* if there is no PSA Connection, connect */
    if (spHandle <= 0)
    {
        FlashOpen();
        if (spHandle <= 0)
        {
            return (spHandle);
        }
    }

    /* set the output vector */
    uint32_t status = 0;
    outvecs[0].base = &status;
    outvecs[0].len = sizeof(uint32_t);

    /* call FlashSizeGet command */
    psa_status_t err = psa_call(spHandle, FLASH_SP_MSG_TYPE_FLASH_SIZE_GET, invecs, 0, outvecs, 1);

    if (err)
    {
        /* return PSA error code */
        return (err);
    }
    else
    {
        /* return FlashSizeGet status code */
        return (status);
    }
}