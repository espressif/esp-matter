/*
 * Copyright (c) 2021, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
/*
 *  ======== FlashCC26X4_s.c ========
 */
#include "FlashCC26X4_s.h"

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)

#include <assert.h>
#include "cmse.h"
#include <tfm_secure_api.h>
#include <psa_manifest/internal_storage_sp.h>

/*!
 *  @brief      Process a PSA Command
 *
 *  @pre        psa_call() was called.
 *
 *  @param[in]  msg       PSA message
 *
 *  @return     A zero status on success.
 */
__attribute__((noinline))
static psa_status_t FlashCC26X4_s_processPsaMsg(psa_msg_t *msg)
{
    struct FlashCC26X4_s_program flashProgramSecureArgs;

    uint32_t sectorAddress;
    uint32_t output;

    /* set default PSA Status */
    psa_status_t psaStatus = PSA_ERROR_PROGRAMMER_ERROR;

    switch (msg->type) {

        case FLASH_SP_MSG_TYPE_PROTECTION_GET:
            if ((msg->in_size[0] == sizeof(uint32_t)) && (msg->out_size[0] == sizeof(uint32_t)))
            {
                psa_read(msg->handle, 0, &sectorAddress, sizeof(uint32_t));

                /* Non-secure callers have negative client ID */
                /* validate the sectorAddress to be read is Non-secure */
                if ((msg->client_id < 0) &&
                   (!cmse_has_unpriv_nonsecure_read_access((void *)sectorAddress, 0x4)))
                {
                    break;
                }

                output = FlashProtectionGet(sectorAddress);
                psa_write(msg->handle, 0, &output, msg->out_size[0]);
                psaStatus = PSA_SUCCESS;
            }
            break;

        case FLASH_SP_MSG_TYPE_SECTOR_ERASE:
            if ((msg->in_size[0] == sizeof(uint32_t)) && (msg->out_size[0] == sizeof(uint32_t)))
            {
                psa_read(msg->handle, 0, &sectorAddress, sizeof(uint32_t));
                
                /* Non-secure callers have negative client ID */
                /* validate the sectorAddress to be erased is Non-secure */
                if ((msg->client_id < 0) &&
                   (!cmse_has_unpriv_nonsecure_rw_access((void *)sectorAddress, FlashSectorSizeGet())))
                {
                    break;
                }

                output = FlashSectorErase(sectorAddress);
                psa_write(msg->handle, 0, &output, msg->out_size[0]);
                psaStatus = PSA_SUCCESS;
            }
            break;

        case FLASH_SP_MSG_TYPE_PROGRAM:
            if ((msg->in_size[0] == sizeof(flashProgramSecureArgs)) && (msg->out_size[0] == sizeof(uint32_t)))
            {
                psa_read(msg->handle, 0, &flashProgramSecureArgs, sizeof(flashProgramSecureArgs));
                
                /* Non-secure callers have negative client ID */
                /* check the dataBuffer address to ensure its not secure memory */
                /* check the sectorAddress to esnure the destination is not secure memory */
                if ((msg->client_id < 0) &&
                   ((!cmse_has_unpriv_nonsecure_read_access(flashProgramSecureArgs.dataBuffer, flashProgramSecureArgs.count)) ||
                    (!cmse_has_unpriv_nonsecure_rw_access((void *)flashProgramSecureArgs.sectorAddress, flashProgramSecureArgs.count))))
                {
                    break;
                }

                output = FlashProgram((uint8_t *)flashProgramSecureArgs.dataBuffer, flashProgramSecureArgs.sectorAddress, flashProgramSecureArgs.count);
                psa_write(msg->handle, 0, &output, msg->out_size[0]);
                psaStatus = PSA_SUCCESS;
            }
            break;

        case FLASH_SP_MSG_TYPE_PROGRAM_4X:
            if ((msg->in_size[0] == sizeof(flashProgramSecureArgs)) && (msg->out_size[0] == sizeof(uint32_t)))
            {
                psa_read(msg->handle, 0, &flashProgramSecureArgs, sizeof(flashProgramSecureArgs));

                /* Non-secure callers have negative client ID */
                /* check the dataBuffer address to ensure its not secure memory */
                /* check the sectorAddress to esnure the destination is not secure memory */
                if ((msg->client_id < 0) &&
                   ((!cmse_has_unpriv_nonsecure_read_access(flashProgramSecureArgs.dataBuffer, flashProgramSecureArgs.count)) ||
                    (!cmse_has_unpriv_nonsecure_rw_access((void *)flashProgramSecureArgs.sectorAddress, flashProgramSecureArgs.count))))
                {
                    break;
                }

                output = FlashProgram4X((uint8_t *)flashProgramSecureArgs.dataBuffer, flashProgramSecureArgs.sectorAddress, flashProgramSecureArgs.count);
                psa_write(msg->handle, 0, &output, msg->out_size[0]);
                psaStatus = PSA_SUCCESS;
            }
            break;

        case FLASH_SP_MSG_TYPE_SECTOR_SIZE_GET:
            if ((msg->out_size[0] == sizeof(uint32_t)))
            {
                output = FlashSectorSizeGet();
                psa_write(msg->handle, 0, &output, msg->out_size[0]);
                psaStatus = PSA_SUCCESS;
            }
            break;
        
        case FLASH_SP_MSG_TYPE_FLASH_SIZE_GET:
            if ((msg->out_size[0] == sizeof(uint32_t)))
            {
                output = FlashSizeGet();
                psa_write(msg->handle, 0, &output, msg->out_size[0]);
                psaStatus = PSA_SUCCESS;
            }
            break;

        default:
            /* Unknown message type */
            break;
    }

    return (psaStatus);
}


/*!
 *  @brief      Main Secure Flash Service Loop
 *
 *  @pre        Secure Flash Service was started
 *
 *  @param[in]  param   Optional startup parameters
 *
 *  @return     A zero status on success.
 */
void FlashCC26X4_s_main(void *param)
{
    uint32_t signals = 0;
    psa_msg_t msg;

    while (1)
    {
        /* pend on next PSA message */
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals & FLASH_SP_SERVICE_SIGNAL)
        {
            psa_get(FLASH_SP_SERVICE_SIGNAL, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;

                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;

                default:
                    if (msg.type >= 0)
                    {
                        psa_reply(msg.handle, FlashCC26X4_s_processPsaMsg(&msg));
                    }
                    else
                    {
                        psa_reply(msg.handle, PSA_ERROR_PROGRAMMER_ERROR);
                    }
                    break;
            }
        }
    }

    return;
}