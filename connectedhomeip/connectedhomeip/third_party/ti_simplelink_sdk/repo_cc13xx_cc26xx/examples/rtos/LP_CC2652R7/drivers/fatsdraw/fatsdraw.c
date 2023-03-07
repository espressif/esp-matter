/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 *  ======== fatsdraw.c ========
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <time.h>

/* POSIX Header files */
#include <pthread.h>

#include <third_party/fatfs/ff.h>

#include <ti/display/Display.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDFatFS.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       2048
#endif

/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

/* Drive number used for FatFs */
#define DRIVE_NUM           0

const char inputfile[] = STR(DRIVE_NUM)":input.txt";
const char outputfile[] = STR(DRIVE_NUM)":output.txt";

const char textarray[] = \
"***********************************************************************\n"
"0         1         2         3         4         5         6         7\n"
"01234567890123456789012345678901234567890123456789012345678901234567890\n"
"This is some text to be inserted into the inputfile if there isn't\n"
"already an existing file located on the media.\n"
"If an inputfile already exists, or if the file was already once\n"
"generated, then the inputfile will NOT be modified.\n"
"***********************************************************************\n";

static Display_Handle display;

unsigned char cpy_buff[CPY_BUFF_SIZE + 1];

FIL src;
FIL dst;

/* Set this to the current UNIX time in seconds */
const struct timespec ts = {
    .tv_sec = 1469647026,
    .tv_nsec = 0
};

/*
 *  ======== printDrive ========
 *  Function to print drive information such as the total disk space
 *  This function was created by referencing FatFs's API documentation
 *  http://elm-chan.org/fsw/ff/en/getfree.html
 *
 *  This function call may take a while to process, depending on the size of
 *  SD Card used.
 */
void printDrive(const char *driveNumber, FATFS **fatfs)
{
    FRESULT fresult;
    DWORD   freeClusterCount;
    DWORD   totalSectorCount;
    DWORD   freeSectorCount;

    Display_printf(display, 0, 0, "Reading disk information...");

    fresult = f_getfree(driveNumber, &freeClusterCount, fatfs);
    if (fresult) {
        Display_printf(display, 0, 0,
            "Error getting the free cluster count from the FatFs object");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "done\n");

        /* Get total sectors and free sectors */
        totalSectorCount = ((*fatfs)->n_fatent - 2) * (*fatfs)->csize;
        freeSectorCount  = freeClusterCount * (*fatfs)->csize;

        /* Print the free space (assuming 512 bytes/sector) */
        Display_printf(display, 0, 0,
            "Total Disk size: %10lu KiB\n Free Disk space: %10lu KiB\n",
            totalSectorCount / 2, freeSectorCount  / 2);
    }
}

/*
 *  ======== mainThread ========
 *  Thread to perform a file copy
 *
 *  Thread tries to open an existing file inputfile[]. If the file doesn't
 *  exist, create one and write some known content into it.
 *  The contents of the inputfile[] are then copied to an output file
 *  outputfile[]. Once completed, the contents of the output file are
 *  printed onto the system console (stdout).
 */
void *mainThread(void *arg0)
{
    FRESULT fresult;

    /* Variables to keep track of the file copy progress */
    unsigned int bytesRead = 0;
    unsigned int bytesWritten = 0;
    unsigned int filesize;
    unsigned int totalBytesCopied = 0;

    SDFatFS_Handle sdfatfsHandle;

    /* Call driver init functions */
    GPIO_init();
    Display_init();
    SDFatFS_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    /* Initialize real-time clock */
    clock_settime(CLOCK_REALTIME, &ts);

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    Display_printf(display, 0, 0, "Starting the FatSD Raw example\n");
    Display_printf(display, 0, 0,
        "This example requires a FAT filesystem on the SD card.\n");
    Display_printf(display, 0, 0,
        "You will get errors if your SD card is not formatted with a filesystem.\n");

    /* Mount and register the SD Card */
    sdfatfsHandle = SDFatFS_open(CONFIG_SDFatFS_0, DRIVE_NUM);
    if (sdfatfsHandle == NULL) {
        Display_printf(display, 0, 0, "Error starting the SD card\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "Drive %u is mounted\n", DRIVE_NUM);
    }

    printDrive(STR(DRIVE_NUM), &(dst.obj.fs));

    /* Try to open the source file */
    fresult = f_open(&src, inputfile, FA_READ);
    if (fresult != FR_OK) {
        Display_printf(display, 0, 0, "Creating a new file \"%s\"...",
            inputfile);

        /* Open file for both reading and writing */
        fresult = f_open(&src, inputfile, FA_CREATE_NEW|FA_READ|FA_WRITE);
        if (fresult != FR_OK) {
            Display_printf(display, 0, 0,
                "Error: \"%s\" could not be created.\nPlease check the "
                "Board.html if additional jumpers are necessary.\n",
                inputfile);
            Display_printf(display, 0, 0, "Aborting...\n");
            while (1);
        }

        f_write(&src, textarray, strlen(textarray), &bytesWritten);
        f_sync(&src);

        /* Reset the internal file pointer */
        f_lseek(&src, 0);

        Display_printf(display, 0, 0, "done\n");
    }
    else {
        Display_printf(display, 0, 0, "Using existing copy of \"%s\"\n",
            inputfile);
    }

    /* Create a new file object for the file copy */
    fresult = f_open(&dst, outputfile, FA_CREATE_ALWAYS|FA_WRITE);
    if (fresult != FR_OK) {
        Display_printf(display, 0, 0, "Error opening \"%s\"\n", outputfile);
        Display_printf(display, 0, 0, "Aborting...\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "Starting file copy\n");
    }

    /*  Copy the contents from the src to the dst */
    while (true) {
        /*  Read from source file */
        fresult = f_read(&src, cpy_buff, CPY_BUFF_SIZE, &bytesRead);
        if (fresult || bytesRead == 0) {
            break; /* Error or EOF */
        }

        /*  Write to dst file */
        fresult = f_write(&dst, cpy_buff, bytesRead, &bytesWritten);
        if (fresult || bytesWritten < bytesRead) {
            Display_printf(display, 0, 0, "Disk Full\n");
            break; /* Error or Disk Full */
        }

        /*  Update the total number of bytes copied */
        totalBytesCopied += bytesWritten;
    }

    f_sync(&dst);

    /* Get the filesize of the source file */
    filesize = f_size(&src);

    /* Close both inputfile[] and outputfile[] */
    f_close(&src);
    f_close(&dst);

    Display_printf(display, 0, 0,
        "File \"%s\" (%u B) copied to \"%s\" (Wrote %u B)\n", inputfile,
         filesize, outputfile, totalBytesCopied);

    /* Now output the outputfile[] contents onto the console */
    fresult = f_open(&dst, outputfile, FA_READ);
    if (fresult != FR_OK) {
        Display_printf(display, 0, 0, "Error opening \"%s\"\n", outputfile);
        Display_printf(display, 0, 0, "Aborting...\n");
        while (1);
    }

    /* Print file contents */
    while (true) {
        /* Read from output file */
        fresult = f_read(&dst, cpy_buff, CPY_BUFF_SIZE, &bytesRead);
        if (fresult || bytesRead == 0) {
            break; /* Error or EOF */
        }
        cpy_buff[bytesRead] = '\0';
        /* Write output */
        Display_printf(display, 0, 0, "%s", cpy_buff);
    }

    /* Close the file */
    f_close(&dst);

    printDrive(STR(DRIVE_NUM), &(dst.obj.fs));

    /* Stopping the SDCard */
    SDFatFS_close(sdfatfsHandle);

    Display_printf(display, 0, 0, "Drive %u unmounted\n", DRIVE_NUM);

    return (NULL);
}

/*
 *  ======== fatfs_getFatTime ========
 */
int32_t fatfs_getFatTime(void)
{
    time_t seconds;
    uint32_t fatTime;
    struct tm *pTime;

    /*
     *  TI time() returns seconds elapsed since 1900, while other tools
     *  return seconds from 1970.  However, both TI and GNU localtime()
     *  sets tm tm_year to number of years since 1900.
     */
    seconds = time(NULL);

    pTime = localtime(&seconds);

    /*
     *  localtime() sets pTime->tm_year to number of years
     *  since 1900, so subtract 80 from tm_year to get FAT time
     *  offset from 1980.
     */
    fatTime = ((uint32_t)(pTime->tm_year - 80) << 25) |
        ((uint32_t)(pTime->tm_mon) << 21) |
        ((uint32_t)(pTime->tm_mday) << 16) |
        ((uint32_t)(pTime->tm_hour) << 11) |
        ((uint32_t)(pTime->tm_min) << 5) |
        ((uint32_t)(pTime->tm_sec) >> 1);

    return ((int32_t)fatTime);
}
