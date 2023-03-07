/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
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
 *  ======== sdraw.c ========
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SD.h>
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/*
 * Set WRITEENABLE to 1 to enable writes to the SD card.
 *
 * WARNING: Running this example with writes enabled will cause any file
 * system present on the SD card to be corrupted!
 */
#define WRITEENABLE 0

/* Size of data buffers */
#define BUFFER_SIZE 1024

/* Starting sector to write/read to */
#define STARTINGSECTOR 0

/* Bytes per Megabyte */
#define BYTESPERMEGABYTE 1048576

static uint8_t writeBuffer[BUFFER_SIZE];
static uint8_t readBuffer[BUFFER_SIZE];
#if (WRITEENABLE)
static uint8_t emptyBuffer[BUFFER_SIZE] = {0};
#endif

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    SD_Handle sdHandle;
    Display_Handle display;
    uint32_t cardCapacity;
    uint32_t totalSectors;
    uint32_t sectorSize;
    uint32_t sectors;
    uint32_t i;
    int16_t result;

    Display_init();
    GPIO_init();
    SD_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    /* Initialize the writeBuffer with data */
    for (i = 0; i < BUFFER_SIZE; i++) {
        writeBuffer[i] = i & 0xFF;
    }

    /* Mount and register the SD Card */
    sdHandle = SD_open(CONFIG_SD_0, NULL);
    if (sdHandle == NULL) {
        Display_printf(display, 0, 0, "Error starting the SD card!");
        while (1);
    }

    result = SD_initialize(sdHandle);
    if (result != SD_STATUS_SUCCESS) {
        Display_printf(display, 0, 0, "Error initializing the SD card!");
        while (1);
    }

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    Display_printf(display, 0, 0, "\n\rStarting the SD example...\n");

    /* Read capacity information from the SD card */
    totalSectors = SD_getNumSectors(sdHandle);
    sectorSize = SD_getSectorSize(sdHandle);
    cardCapacity = (totalSectors / BYTESPERMEGABYTE) * sectorSize;

    /* Display capacity information */
    Display_printf(display, 0, 0, "======== SD Card Information ========");
    Display_printf(display, 0, 0, "Sectors:\t\t%u", totalSectors);
    Display_printf(display, 0, 0, "Sector Size:\t\t%u bytes", sectorSize);
    Display_printf(display, 0, 0, "Card Capacity:\t\t%u MB", cardCapacity);
    Display_printf(display, 0, 0, "=====================================\n");

    /* Calculate number of sectors taken up by the array by rounding up */
    sectors = (BUFFER_SIZE + sectorSize - 1) / sectorSize;

#if (WRITEENABLE)
    Display_printf(display, 0, 0, "Writing %u bytes...", BUFFER_SIZE);

    result = SD_write(sdHandle, writeBuffer, STARTINGSECTOR, sectors);
    if (result != SD_STATUS_SUCCESS) {
        Display_printf(display, 0, 0, "Error writing to the SD card!");
        while (1);
    }
#else
    Display_printf(display, 0, 0, "Run the example with WRITEENABLE "
                "= 1 to enable writes to the SD card!");
#endif

    Display_printf(display, 0, 0, "Reading %u bytes...", BUFFER_SIZE);

    result = SD_read(sdHandle, readBuffer, STARTINGSECTOR, sectors);
    if (result != SD_STATUS_SUCCESS) {
        Display_printf(display, 0, 0, "Error reading from the SD card!");
        while (1);
    }

    /* Compare data read from the SD card with expected values */
    Display_printf(display, 0, 0, "\nComparing read and write data...");
    if (memcmp(readBuffer, writeBuffer, BUFFER_SIZE)) {
#if (WRITEENABLE)
        Display_printf(display, 0, 0,
            "Data read does not match write data!");
        while (1);
#else
        Display_printf(display, 0, 0,
         "WRITEENABLE == 0, therefore data mismatch occurred!");
#endif
    }
    else {
        Display_printf(display, 0, 0,
           "Data matches!");
    }

#if (WRITEENABLE)
    /* Clear any previously written data overwriting data with 0x0 */
    Display_printf(display, 0, 0, "Erasing data written...");
    result = SD_write(sdHandle, emptyBuffer, STARTINGSECTOR, sectors);
    if (result != SD_STATUS_SUCCESS) {
        Display_printf(display, 0, 0, "Error erasing the SD card!");
        while (1);
    }
#endif

    Display_printf(display, 0, 0, "Done!");
    SD_close(sdHandle);

    return (NULL);
}
