/*
 * Copyright (c) 2018-2019, Texas Instruments Incorporated
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
 *  ======== spiffsinternal.c ========
 */
#include <stdint.h>

#include <third_party/spiffs/spiffs.h>
#include <third_party/spiffs/SPIFFSNVS.h>

#include <ti/display/Display.h>

#include "ti_drivers_config.h"

/* SPIFFS configuration parameters */
#define SPIFFS_LOGICAL_BLOCK_SIZE    (8192)
#define SPIFFS_LOGICAL_PAGE_SIZE     (256)
#define SPIFFS_FILE_DESCRIPTOR_SIZE  (44)

/*
 * SPIFFS needs RAM to perform operations on files.  It requires a work buffer
 * which MUST be (2 * LOGICAL_PAGE_SIZE) bytes.
 */
static uint8_t spiffsWorkBuffer[SPIFFS_LOGICAL_PAGE_SIZE * 2];

/* The array below will be used by SPIFFS as a file descriptor cache. */
static uint8_t spiffsFileDescriptorCache[SPIFFS_FILE_DESCRIPTOR_SIZE * 4];

/* The array below will be used by SPIFFS as a read/write cache. */
static uint8_t spiffsReadWriteCache[SPIFFS_LOGICAL_PAGE_SIZE * 2];

#define MESSAGE_LENGTH    (22)
const char message[MESSAGE_LENGTH] = "Hello from SPIFFS!!!\n";
char readBuffer[MESSAGE_LENGTH];

spiffs fs;
SPIFFSNVS_Data spiffsnvsData;

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    Display_Handle displayHandle;
    spiffs_file    fd;
    spiffs_config  fsConfig;
    int32_t        status;

    Display_init();

    displayHandle = Display_open(Display_Type_UART, NULL);
    if (displayHandle == NULL) {
        /* Display_open() failed */
        while (1);
    }

    /* Initialize spiffs, spiffs_config & spiffsnvsdata structures */
    status = SPIFFSNVS_config(&spiffsnvsData, CONFIG_NVSINTERNAL, &fs, &fsConfig,
        SPIFFS_LOGICAL_BLOCK_SIZE, SPIFFS_LOGICAL_PAGE_SIZE);
    if (status != SPIFFSNVS_STATUS_SUCCESS) {
        Display_printf(displayHandle, 0, 0,
            "Error with SPIFFS configuration.\n");

        while (1);
    }

    Display_printf(displayHandle, 0, 0, "Mounting file system...");

    status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
        spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
        spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
    if (status != SPIFFS_OK) {
        /*
         * If SPIFFS_ERR_NOT_A_FS is returned; it means there is no existing
         * file system in memory.  In this case we must unmount, format &
         * re-mount the new file system.
         */
        if (status == SPIFFS_ERR_NOT_A_FS) {
            Display_printf(displayHandle, 0, 0,
                "File system not found; creating new SPIFFS fs...");

            SPIFFS_unmount(&fs);
            status = SPIFFS_format(&fs);
            if (status != SPIFFS_OK) {
                Display_printf(displayHandle, 0, 0,
                    "Error formatting memory.\n");

                while (1);
            }

            status = SPIFFS_mount(&fs, &fsConfig, spiffsWorkBuffer,
                spiffsFileDescriptorCache, sizeof(spiffsFileDescriptorCache),
                spiffsReadWriteCache, sizeof(spiffsReadWriteCache), NULL);
            if (status != SPIFFS_OK) {
                Display_printf(displayHandle, 0, 0,
                    "Error mounting file system.\n");

                while (1);
            }
        }
        else {
            /* Received an unexpected error when mounting file system  */
            Display_printf(displayHandle, 0, 0,
                "Error mounting file system: %d.\n", status);

            while (1);
        }
    }

    /* Open a file */
    fd = SPIFFS_open(&fs, "spiffsFile", SPIFFS_RDWR, 0);
    if (fd < 0) {
        /* File not found; create a new file & write message to it */
        Display_printf(displayHandle, 0, 0, "Creating spiffsFile...");

        fd = SPIFFS_open(&fs, "spiffsFile", SPIFFS_CREAT | SPIFFS_RDWR, 0);
        if (fd < 0) {
            Display_printf(displayHandle, 0, 0,
                "Error creating spiffsFile.\n");

            while (1);
        }

        Display_printf(displayHandle, 0, 0, "Writing to spiffsFile...");

        if (SPIFFS_write(&fs, fd, (void *) &message, MESSAGE_LENGTH) < 0) {
            Display_printf(displayHandle, 0, 0, "Error writing spiffsFile.\n");

            while (1) ;
        }

        SPIFFS_close(&fs, fd);
    }
    else {
        Display_printf(displayHandle, 0, 0, "Reading spiffsFile...\n");

        /* spiffsFile exists; read its contents & delete the file */
        if (SPIFFS_read(&fs, fd, readBuffer, MESSAGE_LENGTH) < 0) {
            Display_printf(displayHandle, 0, 0, "Error reading spiffsFile.\n");

            while (1) ;
        }

        Display_printf(displayHandle, 0, 0, "spiffsFile: %s", readBuffer);
        Display_printf(displayHandle, 0, 0, "Erasing spiffsFile...");

        status = SPIFFS_fremove(&fs, fd);
        if (status != SPIFFS_OK) {
            Display_printf(displayHandle, 0, 0, "Error removing spiffsFile.\n");

            while (1);
        }

        SPIFFS_close(&fs, fd);
    }

    SPIFFS_unmount(&fs);

    Display_printf(displayHandle, 0, 0, "Reset the device.");
    Display_printf(displayHandle, 0, 0,
        "==================================================\n\n");

    return (NULL);
}
