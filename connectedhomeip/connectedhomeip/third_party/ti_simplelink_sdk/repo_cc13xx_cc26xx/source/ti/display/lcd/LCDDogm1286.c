/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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

#include <string.h>

#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/display/lcd/LCDDogm1286.h>
#include <ti/display/lcd/LCDDogm1286_util.h>

/* macro to calculate minimum value */
#define MIN(a,b) (((a)<(b))?(a):(b))

/* Externs */
extern const LCD_Config LCD_config;
extern const SPI_Config SPI_config[];

/* PIN driver state object */
static PIN_State pinState;

/* PIN driver handle */
static PIN_Handle hPin;

/* Used to check status and initialization */
static int LCD_count = -1;

/* Static LCD functions */
static bool LCD_initHw();
static void LCD_sendCommand(LCD_Handle handle, const char *pcCmd,
    unsigned char ucLen);
static void LCD_sendData(LCD_Handle handle, const char *pcData,
    unsigned short usLen);
static void LCD_gotoXY(LCD_Handle handle, unsigned char ucX, unsigned char ucY);
static bool LCD_sendArray(LCD_Handle handle, const char *pcData,
    unsigned short usLen);
static void LCD_bufferLine(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucXFrom, unsigned char ucYFrom, unsigned char ucXTo,
    unsigned char ucYTo, unsigned char ucDraw);
static void LCD_doUpdate(LCD_Handle handle, unsigned int bufIndex,
    bool blocking);
static void LCD_doBufferClearPage(LCD_Handle handle, unsigned int bufIndex,
    LCD_Page iPage, bool blocking);
static void LCD_doBufferPrintString(LCD_Handle handle, unsigned int bufIndex,
    const char *pcStr, unsigned char ucX, LCD_Page iPage, bool blocking);
static void LCD_doBufferPrintInt(LCD_Handle handle, unsigned int bufIndex,
    int i32Number, unsigned char ucX, LCD_Page iPage, bool blocking);
static void LCD_doBufferSetHLine(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucXFrom, unsigned char ucXTo, unsigned char ucY,
    bool blocking);
static void LCD_doBufferClearHLine(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucXFrom, unsigned char ucXTo, unsigned char ucY,
    bool blocking);
static void LCD_doBufferSetVLine(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucX, unsigned char ucYFrom, unsigned char ucYTo,
    bool blocking);
static void LCD_doBufferClearVLine(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucX, unsigned char ucYFrom, unsigned char ucYTo,
    bool blocking);
static void LCD_doBufferSetPx(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucX, unsigned char ucY, bool blocking);
static void LCD_doBufferClearPx(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucX, unsigned char ucY, bool blocking);

/* Font data for 5x7 font */
const char LCD_alphabet[] = {
    0x00, //
    0x00, //
    0x5F, //  # #####
    0x00, //
    0x00, //

    0x00, //
    0x07, //      ###
    0x00, //
    0x07, //      ###
    0x00, //

    0x14, //    # #
    0x7F, //  #######
    0x14, //    # #
    0x7F, //  #######
    0x14, //    # #

    0x24, //   #  #
    0x2A, //   # # #
    0x7F, //  #######
    0x2A, //   # # #
    0x12, //    #  #

    0x23, //   #   ##
    0x13, //    #  ##
    0x08, //     #
    0x64, //  ##  #
    0x62, //  ##   #

    0x36, //   ## ##
    0x49, //  #  #  #
    0x55, //  # # # #
    0x22, //   #   #
    0x50, //  # #

    0x00, //
    0x05, //      # #
    0x03, //       ##
    0x00, //
    0x00, //

    0x00, //
    0x1C, //    ###
    0x22, //   #   #
    0x41, //  #     #
    0x00, //

    0x00, //
    0x41, //  #     #
    0x22, //   #   #
    0x1C, //    ###
    0x00, //

    0x08, //     #
    0x2A, //   # # #
    0x1C, //    ###
    0x2A, //   # # #
    0x08, //     #

    0x08, //     #
    0x08, //     #
    0x3E, //   #####
    0x08, //     #
    0x08, //     #

    0x00, //
    0x50, //  # #
    0x30, //   ##
    0x00, //
    0x00, //

    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x08, //     #

    0x00, //
    0x60, //  ##
    0x60, //  ##
    0x00, //
    0x00, //

    0x20, //   #
    0x10, //    #
    0x08, //     #
    0x04, //      #
    0x02, //       #

    0x3E, //   #####
    0x51, //  # #   #
    0x49, //  #  #  #
    0x45, //  #   # #
    0x3E, //   #####

    0x00, //
    0x42, //  #    #
    0x7F, //  #######
    0x40, //  #
    0x00, //

    0x42, //  #    #
    0x61, //  ##    #
    0x51, //  # #   #
    0x49, //  #  #  #
    0x46, //  #   ##

    0x21, //   #    #
    0x41, //  #     #
    0x45, //  #   # #
    0x4B, //  #  # ##
    0x31, //   ##   #

    0x18, //    ##
    0x14, //    # #
    0x12, //    #  #
    0x7F, //  #######
    0x10, //    #

    0x27, //   #  ###
    0x45, //  #   # #
    0x45, //  #   # #
    0x45, //  #   # #
    0x39, //   ###  #

    0x3C, //   ####
    0x4A, //  #  # #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x30, //   ##

    0x01, //        #
    0x71, //  ###   #
    0x09, //     #  #
    0x05, //      # #
    0x03, //       ##

    0x36, //   ## ##
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x36, //   ## ##

    0x06, //      ##
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x29, //   # #  #
    0x1E, //    ####

    0x00, //
    0x36, //   ## ##
    0x36, //   ## ##
    0x00, //
    0x00, //

    0x00, //
    0x56, //  # # ##
    0x36, //   ## ##
    0x00, //
    0x00, //

    0x00, //
    0x08, //     #
    0x14, //    # #
    0x22, //   #   #
    0x41, //  #     #

    0x14, //    # #
    0x14, //    # #
    0x14, //    # #
    0x14, //    # #
    0x14, //    # #

    0x41, //  #     #
    0x22, //   #   #
    0x14, //    # #
    0x08, //     #
    0x00, //

    0x02, //       #
    0x01, //        #
    0x51, //  # #   #
    0x09, //     #  #
    0x06, //      ##

    0x32, //   ##  #
    0x49, //  #  #  #
    0x79, //  ####  #
    0x41, //  #     #
    0x3E, //   #####

    0x7E, //  ######
    0x11, //    #   #
    0x11, //    #   #
    0x11, //    #   #
    0x7E, //  ######

    0x7F, //  #######
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x36, //   ## ##

    0x3E, //   #####
    0x41, //  #     #
    0x41, //  #     #
    0x41, //  #     #
    0x22, //   #   #

    0x7F, //  #######
    0x41, //  #     #
    0x41, //  #     #
    0x22, //   #   #
    0x1C, //    ###

    0x7F, //  #######
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x41, //  #     #

    0x7F, //  #######
    0x09, //     #  #
    0x09, //     #  #
    0x01, //        #
    0x01, //        #

    0x3E, //   #####
    0x41, //  #     #
    0x41, //  #     #
    0x51, //  # #   #
    0x32, //   ##  #

    0x7F, //  #######
    0x08, //     #
    0x08, //     #
    0x08, //     #
    0x7F, //  #######

    0x00, //
    0x41, //  #     #
    0x7F, //  #######
    0x41, //  #     #
    0x00, //

    0x20, //   #
    0x40, //  #
    0x41, //  #     #
    0x3F, //   ######
    0x01, //        #

    0x7F, //  #######
    0x08, //     #
    0x14, //    # #
    0x22, //   #   #
    0x41, //  #     #

    0x7F, //  #######
    0x40, //  #
    0x40, //  #
    0x40, //  #
    0x40, //  #

    0x7F, //  #######
    0x02, //       #
    0x04, //      #
    0x02, //       #
    0x7F, //  #######

    0x7F, //  #######
    0x04, //      #
    0x08, //     #
    0x10, //    #
    0x7F, //  #######

    0x3E, //   #####
    0x41, //  #     #
    0x41, //  #     #
    0x41, //  #     #
    0x3E, //   #####

    0x7F, //  #######
    0x09, //     #  #
    0x09, //     #  #
    0x09, //     #  #
    0x06, //      ##

    0x3E, //   #####
    0x41, //  #     #
    0x51, //  # #   #
    0x21, //   #    #
    0x5E, //  # ####

    0x7F, //  #######
    0x09, //     #  #
    0x19, //    ##  #
    0x29, //   # #  #
    0x46, //  #   ##

    0x46, //  #   ##
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x49, //  #  #  #
    0x31, //   ##   #

    0x01, //        #
    0x01, //        #
    0x7F, //  #######
    0x01, //        #
    0x01, //        #

    0x3F, //   ######
    0x40, //  #
    0x40, //  #
    0x40, //  #
    0x3F, //   ######

    0x1F, //    #####
    0x20, //   #
    0x40, //  #
    0x20, //   #
    0x1F, //    #####

    0x7F, //  #######
    0x20, //   #
    0x18, //    ##
    0x20, //   #
    0x7F, //  #######

    0x63, //  ##   ##
    0x14, //    # #
    0x08, //     #
    0x14, //    # #
    0x63, //  ##   ##

    0x03, //       ##
    0x04, //      #
    0x78, //  ####
    0x04, //      #
    0x03, //       ##

    0x61, //  ##    #
    0x51, //  # #   #
    0x49, //  #  #  #
    0x45, //  #   # #
    0x43, //  #    ##

    0x00, //
    0x00, //
    0x7F, //  #######
    0x41, //  #     #
    0x41, //  #     #

    0x02, //       #
    0x04, //      #
    0x08, //     #
    0x10, //    #
    0x20, //   #

    0x41, //  #     #
    0x41, //  #     #
    0x7F, //  #######
    0x00, //
    0x00, //

    0x04, //      #
    0x02, //       #
    0x01, //        #
    0x02, //       #
    0x04, //      #

    0x40, //  #
    0x40, //  #
    0x40, //  #
    0x40, //  #
    0x40, //  #

    0x00, //
    0x01, //        #
    0x02, //       #
    0x04, //      #
    0x00, //

    0x20, //   #
    0x54, //  # # #
    0x54, //  # # #
    0x54, //  # # #
    0x78, //  ####

    0x7F, //  #######
    0x48, //  #  #
    0x44, //  #   #
    0x44, //  #   #
    0x38, //   ###

    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x44, //  #   #
    0x20, //   #

    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x48, //  #  #
    0x7F, //  #######

    0x38, //   ###
    0x54, //  # # #
    0x54, //  # # #
    0x54, //  # # #
    0x18, //    ##

    0x08, //     #
    0x7E, //  ######
    0x09, //     #  #
    0x01, //        #
    0x02, //       #

    0x08, //     #
    0x14, //    # #
    0x54, //  # # #
    0x54, //  # # #
    0x3C, //   ####

    0x7F, //  #######
    0x08, //     #
    0x04, //      #
    0x04, //      #
    0x78, //  ####

    0x00, //
    0x44, //  #   #
    0x7D, //  ##### #
    0x40, //  #
    0x00, //

    0x20, //   #
    0x40, //  #
    0x44, //  #   #
    0x3D, //   #### #
    0x00, //

    0x00, //
    0x7F, //  #######
    0x10, //    #
    0x28, //   # #
    0x44, //  #   #

    0x00, //
    0x41, //  #     #
    0x7F, //  #######
    0x40, //  #
    0x00, //

    0x7C, //  #####
    0x04, //      #
    0x18, //    ##
    0x04, //      #
    0x78, //  ####

    0x7C, //  #####
    0x08, //     #
    0x04, //      #
    0x04, //      #
    0x78, //  ####

    0x38, //   ###
    0x44, //  #   #
    0x44, //  #   #
    0x44, //  #   #
    0x38, //   ###

    0x7C, //  #####
    0x14, //    # #
    0x14, //    # #
    0x14, //    # #
    0x08, //     #

    0x08, //     #
    0x14, //    # #
    0x14, //    # #
    0x18, //    ##
    0x7C, //  #####

    0x7C, //  #####
    0x08, //     #
    0x04, //      #
    0x04, //      #
    0x08, //     #

    0x48, //  #  #
    0x54, //  # # #
    0x54, //  # # #
    0x54, //  # # #
    0x20, //   #

    0x04, //      #
    0x3F, //   ######
    0x44, //  #   #
    0x40, //  #
    0x20, //   #

    0x3C, //   ####
    0x40, //  #
    0x40, //  #
    0x20, //   #
    0x7C, //  #####

    0x1C, //    ###
    0x20, //   #
    0x40, //  #
    0x20, //   #
    0x1C, //    ###

    0x3C, //   ####
    0x40, //  #
    0x30, //   ##
    0x40, //  #
    0x3C, //   ####

    0x44, //  #   #
    0x28, //   # #
    0x10, //    #
    0x28, //   # #
    0x44, //  #   #

    0x0C, //     ##
    0x50, //  # #
    0x50, //  # #
    0x50, //  # #
    0x3C, //   ####

    0x44, //  #   #
    0x64, //  ##  #
    0x54, //  # # #
    0x4C, //  #  ##
    0x44, //  #   #

    0x00, //
    0x08, //     #
    0x36, //   ## ##
    0x41, //  #     #
    0x00, //

    0x00, //
    0x00, //
    0x7F, //  #######
    0x00, //
    0x00, //

    0x00, //
    0x41, //  #     #
    0x36, //   ## ##
    0x08, //     #
    0x00, //

    // SPECIAL NON-ASCII SYMBOLS

    0x00, //
    0x3E, //   #####
    0x1C, //    ###
    0x08, //     #
    0x00  //
};

/*
 *  ======== LCD_close ========
 *  @pre    Function assumes that the handle is not NULL
 */
void LCD_close(LCD_Handle handle)
{
    unsigned char i = 0;

    DebugP_assert((handle != NULL) && (LCD_count != -1));

    /* Get the pointers to the LCD object and buffer */
    LCD_Object *object = handle->object;
    LCD_Buffer *buffers = object->lcdBuffers;

    /* Destroy the semaphores */
    for (i = 0; i < object->nBuffers; i++)
    {
        SemaphoreP_destruct(&(buffers[i].bufMutex));
    }

    /* Close SPI */
    SPI_close(object->spiHandle);

    /* Close pin driver and de-allocate pins */
    PIN_close(hPin);

    /* Mark object as closed*/
    object->isOpen = false;
    DebugP_log0("LCD: LCD is closed");
}

/*
 *  ======== LCD_init ========
 */
void LCD_init()
{
    /* Allow multiple calls for LCD_init */
    if (LCD_count >= 0)
    {
        return;
    }
    LCD_count++;

    LCD_Handle handle = (LCD_Handle)&(LCD_config);
    LCD_Object *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Mark the object as available */
    object->isOpen = false;
}

/*
 *  ======== LCD_Params_init ========
 */
void LCD_Params_init(LCD_Params *params)
{
    DebugP_assert(params != NULL);

    params->lcdWriteTimeout = SemaphoreP_WAIT_FOREVER;
    params->spiBitRate = 1000000;
    params->spiFrameFormat = SPI_POL0_PHA0;
}

/*
 *  ======== LCD_open ========
 */
LCD_Handle LCD_open(LCD_Buffer *buffers, uint8_t nBuffers,
    LCD_Params *lcdParams)
{
    unsigned int key;
    LCD_Params defaultParams;
    LCD_Object *lcdObject;
    LCD_HWAttrs const *lcdHwAttrs;
    SPI_Handle spiHandle;
    LCD_Handle handle = (LCD_Handle) &(LCD_config);

    /* Get the pointer to the object and hwAttrs. */
    lcdObject = handle->object;
    lcdHwAttrs = handle->hwAttrs;

    SPI_init(); /* must call SPI_init() before SPI_open() */

    /* Disable preemption while checking if the LCD is open. */
    key = HwiP_disable();

    /* Determine if the device index was already opened. */
    if (lcdObject->isOpen)
    {
        HwiP_restore(key);
        DebugP_log0("LCD already in use.");
        return (NULL);
    }

    /* Mark the handle as being used. */
    lcdObject->isOpen = true;
    HwiP_restore(key);

    /* If params are NULL use defaults. */
    if (lcdParams == NULL)
    {
        LCD_Params_init(&defaultParams);
        lcdParams = &defaultParams;
    }

    /* If buffers are NULL, or nBuffers <= 0, return */
    if((buffers == NULL) ||(nBuffers <= 0))
    {
        DebugP_log0("No buffer is availible for the LCD driver");
        return (NULL);
    }

    /* Initialize SPI parameters. Master mode, blocking mode
     * and dataSize=8 is default and should not be changed.
     * The other parameters can be set from LCD parameters.
     */
    SPI_Params spiParams;
    SPI_Params_init(&spiParams);
    spiParams.bitRate = lcdParams->spiBitRate;
    spiParams.frameFormat = lcdParams->spiFrameFormat;

    /* Try open the SPI */
    spiHandle = SPI_open(lcdHwAttrs->spiIndex, &spiParams);
    if (!spiHandle)
    {
        return (NULL);
    }

    /* Initialize the LCD object */
    lcdObject->spiHandle = spiHandle;
    lcdObject->lcdWriteTimeout = lcdParams->lcdWriteTimeout;
    lcdObject->lcdBuffers = buffers;
    lcdObject->nBuffers = nBuffers;

    /* Create a counting semaphore for each buffer */
    unsigned char i = 0;
    SemaphoreP_Params semParams;
    SemaphoreP_Params_init(&semParams);
    for (i = 0; i < nBuffers; i++)
    {
        SemaphoreP_construct(&(buffers[i].bufMutex), 1, &semParams);
    }

    /* Create a binary semaphore for the LCD */
    semParams.mode = SemaphoreP_Mode_BINARY;
    SemaphoreP_construct(&lcdObject->lcdMutex, 1, &semParams);

    /* Configure the hardware module */
    if(!LCD_initHw(handle))
    {
        // Hw initialization failed
       return (NULL);
    }

    /* Send LCD init commands */
    LCD_sendCommand(handle, (const char *) (lcdHwAttrs->LCD_initCmd),
                    sizeof(LCD_Command));

    DebugP_log0("LCD: LCD is opened");
    return (handle);
}

/*
 *  ======== LCD_writeLine ========
 *  This function writes one line of the specified buffer
 *  and sends it to the display.
 */
void LCD_writeLine(LCD_Handle handle, unsigned int bufIndex, char *str,
    unsigned int uiValue, unsigned char ucFormat, unsigned char ucLine)
{
    LCD_Page pageNo = (LCD_Page) (ucLine % LCD_PAGE_COUNT);

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible",
            object->nBuffers);
        return;
    }

    /* Pend on the LCD Mutex */
    DebugP_log0("LCD_writeLine: pending on LCD mutex.");
    if (SemaphoreP_OK != SemaphoreP_pend(&object->lcdMutex, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log0("Waiting for access to LCD timed out, exiting LCD_writeLine.");
        return;
    }

    /* Get pointers to the buffer and its semaphore. */
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    if (ucFormat)
    {

        unsigned char maxLen = 50; // max string length
        unsigned char buf[50];
        unsigned char strLength;

        /* Check that there is a null termination in the string */
        const char *end = (const char *) memchr(str, '\0', maxLen);
        if (end == NULL)
            strLength = maxLen;
        else
            strLength = end - str;

        memset(buf, ' ', maxLen);
        memcpy(buf, str, strLength);

        /* Get number of characters in string */
        unsigned char ucNumOfDigits = LCD_getIntLength(uiValue, ucFormat);

        /* String length + 2 spaces + number of digits cannot exceed one line */
        if ((strLength + 2 + ucNumOfDigits) > (LCD_COLS / LCD_CHAR_WIDTH))
        {
            DebugP_log1("LCD_writeLine does not support a string size larger than %d characters.",
                    (LCD_COLS/LCD_CHAR_WIDTH));
            SemaphoreP_post(&object->lcdMutex);
            return;
        }

        /* Copy to local buffer and send */
        _itoa(uiValue, &buf[strLength + 2], ucFormat);

        /* Pend on buffer semaphore */
        DebugP_log1("LCD_writeLine: pending on semaphore associated with buffer %p",
                        (UArg)object->lcdBuffers[bufIndex].pcBuffer);
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer %p timed out, exiting LCD_writeLine.",
                    (UArg)object->lcdBuffers[bufIndex].pcBuffer);
            SemaphoreP_post(&object->lcdMutex);
            return;
        }

        /* Clear the page */
        LCD_doBufferClearPage(handle, bufIndex, pageNo, false);

        /* write buffer*/
        LCD_doBufferPrintString(handle, bufIndex, (char*) buf, 0, pageNo,
                false);

    }
    else
    {
        /* Pend on buffer semaphore */
        DebugP_log1("LCD_writeLine: pending on semaphore associated with buffer %p",
                (UArg)object->lcdBuffers[bufIndex].pcBuffer);
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer %p timed out, exiting LCD_writeLine.",
                                (UArg)object->lcdBuffers[bufIndex].pcBuffer);
            SemaphoreP_post(&object->lcdMutex);
            return;
        }

        /* Clear the page */
        LCD_doBufferClearPage(handle, bufIndex, pageNo, false);

        /* write buffer*/
        LCD_doBufferPrintString(handle, bufIndex, str, 0, pageNo, false);

    }

    /* Update LCD */
    LCD_doUpdate(handle, bufIndex, false);

    /* Finished with buffer - post on semaphores*/
    DebugP_log1("LCD_writeLine: posting semaphore associated with buffer %p",
                    (UArg)object->lcdBuffers[bufIndex].pcBuffer);
    SemaphoreP_post(pSem);
    DebugP_log0("LCD_writeLine: posting LCD mutex.");
    SemaphoreP_post(&object->lcdMutex);
}

/*
 *  ======== LCD_update ========
 *  This function sends the specified buffer to the display.
 */
void LCD_update(LCD_Handle handle, unsigned int bufIndex)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }
    /* Call update function with use of semaphore */
    LCD_doUpdate(handle, bufIndex, true);
}

/*
 *  ======== LCD_updatePart ========
 *  This function sends the specified part of the buffer
 *  to the corresponding part on the LCD.
 */
void LCD_updatePart(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                    unsigned char ucXTo, LCD_Page iPageFrom, LCD_Page iPageTo)
{
    unsigned char ucXRange, ucY, ucYOffset, ucYRange;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Pend on the LCD Mutex */
    DebugP_log0("LCD_updatePart: pending on LCD mutex.");
    if (SemaphoreP_OK != SemaphoreP_pend(&object->lcdMutex, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log0("Waiting for access to LCD timed out, exiting LCD_updatePart.");
        return;
    }

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Assuming ucXFrom <= ucXTo */
    ucXRange = ucXTo - ucXFrom + 1;

    /* Assuming iPageFrom <= iPage To */
    ucYRange = iPageTo - iPageFrom;

    /* Pend on buffer semaphore */
    DebugP_log1("LCD_updatePart: pending on semaphore associated with buffer %p",
            bufIndex);
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Waiting for access to buffer (%p) timed out, exiting LCD_updatePart.",
                bufIndex);
        SemaphoreP_post(&object->lcdMutex);
        return;
    }

    /* For each page, send data */
    for (ucY = 0; ucY <= ucYRange; ucY++)
    {
        ucYOffset = iPageFrom + ucY;
        LCD_gotoXY(handle, ucXFrom, ucYOffset);
        LCD_sendData(handle, pcBuf + (ucYOffset * LCD_COLS) + ucXFrom,
                ucXRange);
    }

    /* Finished with buffer - post on semaphores*/
    DebugP_log1("LCD_updatePart: posting semaphore associated with buffer %p",
                    (UArg)object->lcdBuffers[bufIndex].pcBuffer);
    SemaphoreP_post(pSem);
    DebugP_log0("LCD_updatePart: posting LCD mutex.");
    SemaphoreP_post(&object->lcdMutex);
}

/*
 *  ======== LCD_bufferClear ========
 *  This function empties the specified LCD buffer
 *  by filling it with zeros.
 */
void LCD_bufferClear(LCD_Handle handle, unsigned int bufIndex)
{
    DebugP_assert(handle != NULL);

    unsigned int uiIdx;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get size of buffer */
    unsigned int bufSize = object->lcdBuffers[bufIndex].bufSize;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore */
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Waiting for access to buffer (%p) timed out, exiting LCD_bufferClear.",
                bufIndex);

        return;
    }

    for (uiIdx = 0; uiIdx < bufSize; uiIdx++)
    {
        *(pcBuf + uiIdx) = 0x00;
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);

}

/*
 * ======== LCD_bufferClearPage ========
 * This function clears the specified page of a buffer
 */
void LCD_bufferClearPage(LCD_Handle handle, unsigned int bufIndex, LCD_Page iPage)
{
    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible",
            handle->object->nBuffers);
        return;
    }

    /* Call clear page function with use of semaphore */
    LCD_doBufferClearPage(handle, bufIndex, iPage, true);
}

/*
 * ======== LCD_bufferClearPart ========
 * This function clears the pixels in a given piece of a page.
 */
void LCD_bufferClearPart(LCD_Handle handle, unsigned int bufIndex,
    unsigned char ucXFrom, unsigned char ucXTo, LCD_Page iPageFrom,
    LCD_Page iPageTo)
{
    unsigned char ucX, ucXRange, ucY, ucYRange;
    unsigned short usXFirstPos;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Assuming ucYFrom <= ucYTo */
    ucXRange = ucXTo - ucXFrom;

    /* Assuming ucYFrom <= ucYTo */
    ucYRange = iPageTo - iPageFrom;

    /* Pend on the semaphore */
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Waiting for access to buffer (%p) timed out, exiting LCD_bufferClearPart.",
                bufIndex);

        return;
    }

    /* Clear buffer part */
    for (ucY = 0; ucY <= ucYRange; ucY++)
    {
        usXFirstPos = (iPageFrom + ucY) * LCD_COLS + ucXFrom;
        for (ucX = 0; ucX <= ucXRange; ucX++)
        {
            *(pcBuf + (usXFirstPos + ucX)) = 0x00;
        }
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 * ======== LCD_bufferInvert ========
 * This function inverts the pixels (bits) in a given region of the
 * a buffer.
 */
void LCD_bufferInvert(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                          unsigned char ucYFrom, unsigned char ucXTo, unsigned char ucYTo)
{
    unsigned char ucI, ucJ, ucPow;
    unsigned char ucFirstPage, ucLastPage, ucFirstPageMask, ucLastPageMask;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Find the first and last page to invert on */
    ucFirstPage = ucYFrom / LCD_PAGE_ROWS;
    ucLastPage = ucYTo / LCD_PAGE_ROWS;

    /* Find the bitmask to invert with on first page */
    ucFirstPageMask = 0xFF;
    ucPow = 1;

    /* Generate invert bitmask for the first page */
    for (ucI = 0; ucI < LCD_PAGE_ROWS; ucI++)
    {
        if (ucYFrom - ucFirstPage * LCD_PAGE_ROWS > ucI)
        {
            ucFirstPageMask -= ucPow;
            ucPow *= 2;
        }
    }

    /* Find the bitmask to invert with on the last page */
    ucLastPageMask = 0x00;
    ucPow = 1;
    for (ucI = 0; ucI < LCD_PAGE_ROWS; ucI++)
    {
        if (ucYTo - ucLastPage * LCD_PAGE_ROWS >= ucI)
        {
            ucLastPageMask += ucPow;
            ucPow *= 2;
        }
    }

    /* Prevent error if ucFirstPage==ucLastPage */
    if (ucFirstPage == ucLastPage)
    {
        ucLastPageMask ^= 0xFF;
    }

    /* Pend on the semaphore */
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Waiting for access to buffer (%p) timed out, exiting LCD_bufferInvert.",
                bufIndex);

        return;
    }

    /* Invert the given part of the first page */
    for (ucI = ucXFrom; ucI <= ucXTo; ucI++)
    {
        *(pcBuf + (ucFirstPage * LCD_COLS + ucI)) ^= ucFirstPageMask;
    }

    /* Invert the pages between first and last in the given section */
    for (ucI = ucFirstPage + 1; ucI <= ucLastPage - 1; ucI++)
    {
        for (ucJ = ucXFrom; ucJ <= ucXTo; ucJ++)
        {
            *(pcBuf + (ucI * LCD_COLS + ucJ)) ^= 0xFF;
        }
    }

    /* Invert the given part of the last page */
    for (ucI = ucXFrom; ucI <= ucXTo; ucI++)
    {
        *(pcBuf + (ucLastPage * LCD_COLS + ucI)) ^= ucLastPageMask;
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 *  ======== LCD_bufferInvertPage ========
 *  This function inverts a range of columns in the display buffer on a
 *  specified page.
 */
void LCD_bufferInvertPage(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                             unsigned char ucXTo, LCD_Page iPage)
{
    unsigned char ucI;
    unsigned short usFirstPos = iPage * LCD_COLS + ucXFrom;
    unsigned char ucRange = ucXTo - ucXFrom;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore */
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Waiting for access to buffer (%p) timed out, exiting LCD_bufferInvertPage.",
                bufIndex);

        return;
    }

    /* Invert buffer range */
    for (ucI = 0; ucI <= ucRange; ucI++)
    {
        *(pcBuf + (usFirstPos + ucI)) ^= 0xFF;
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 *  ======== LCD_bufferPrintString ========
 *  This function writes a string to the specified buffer
 */
void LCD_bufferPrintString(LCD_Handle handle, unsigned int bufIndex, const char *pcStr,
                              unsigned char ucX, LCD_Page iPage)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call print string function with use of semaphore */
    LCD_doBufferPrintString(handle, bufIndex, pcStr, ucX, iPage, true);
}

/*
 *  ======== LCD_bufferPrintStringAligned ========
 *  This function writes an aligned string to a buffer.
 */
void LCD_bufferPrintStringAligned(LCD_Handle handle, unsigned int bufIndex, const char *pcStr,
                                   LCD_Align iAlignment, LCD_Page iPage)
{
    unsigned char ucX;
    unsigned char ucStrSize = LCD_getStringLength(pcStr);

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Calculate X offset based on alignment */
    switch (iAlignment)
    {
        case LCD_ALIGN_CENTER:
            ucX = LCD_COLS / 2 - ucStrSize * LCD_CHAR_WIDTH / 2;
            break;
        case LCD_ALIGN_RIGHT:
            ucX = LCD_COLS - ucStrSize * LCD_CHAR_WIDTH;
            break;
        case LCD_ALIGN_LEFT:
        default:
            ucX = 0;
            break;
    }

    /* Print string to buffer */
    LCD_doBufferPrintString(handle, bufIndex, pcStr, ucX, iPage, true);
}

/*
 *  ======== LCD_bufferPrintInt ========
 *  This function writes an integer to the specified buffer
 */
void LCD_bufferPrintInt(LCD_Handle handle, unsigned int bufIndex, int i32Number,
                         unsigned char ucX, LCD_Page iPage)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call print int function with use of semaphore */
    LCD_doBufferPrintInt(handle, bufIndex, i32Number, ucX, iPage, true);

}

/*
 *  ======== LCD_bufferPrintIntAligned ========
 *  This function writes an aligned integer to the specified buffer
 */
void LCD_bufferPrintIntAligned(LCD_Handle handle, unsigned int bufIndex, int i32Number,
                                       LCD_Align iAlignment, LCD_Page iPage)
{
    unsigned char ucX;
    unsigned char ucStrSize = LCD_getIntLength(i32Number, 10);

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Calculate X position based on alignment */
    switch (iAlignment)
    {
        case LCD_ALIGN_CENTER:
            ucX = LCD_COLS / 2 - ucStrSize * LCD_CHAR_WIDTH / 2;
            break;
        case LCD_ALIGN_RIGHT:
            ucX = LCD_COLS - ucStrSize * LCD_CHAR_WIDTH;
            break;
        case LCD_ALIGN_LEFT:
        default:
            ucX = 0;
            break;
    }

    /* Print number to buffer with use of semaphore */
    LCD_doBufferPrintInt(handle, bufIndex, i32Number, ucX, iPage, true);
}

/*
 *  ======== LCD_bufferPrintFloat ========
 *  This function writes a floating point number to the specified
 *  buffer.
 */
void LCD_bufferPrintFloat(LCD_Handle handle, unsigned int bufIndex, float fNumber,
    unsigned char ucDecimals, unsigned char ucX, LCD_Page iPage)
{
    unsigned char ucI, ucRoundUp, ucNumNeg;
    int8_t i8J;
    int i32intPart, i32Tmpint;
    unsigned char decimalArray[11];
    float fThreshold;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointer to the semaphore. */
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    ucNumNeg = 0;

    /* Return early if number of decimals is too high */
    if (ucDecimals > 10)
    {
        return;
    }

    /* fThreshold defines how small a Float must be to be considered negative.
     * For example, if a Float is -0.001 and the number of decimals is 2,
     * then the number will be considered as 0.00, and not -0.00.
     */
    fThreshold = -0.5;
    for (ucI = 0; ucI < ucDecimals; ucI++)
    {
        fThreshold *= 0.1;
    }

    if (fNumber <= fThreshold)
    {
        fNumber *= -1;
        ucNumNeg = 1;
    }

    /* Extract integer part */
    i32intPart = (int) fNumber;

    /* Storing (ucDecimals+1) decimals in an array */
    for (ucI = 0; ucI < ucDecimals + 1; ucI++)
    {
        fNumber = fNumber * 10;
        i32Tmpint = (int) fNumber;
        i32Tmpint = i32Tmpint % 10;
        decimalArray[ucI] = i32Tmpint;
    }

    /* Perform upwards rounding: This can correct the truncation error that
     * occurs when passing a Float argument that is generated by division. ex:
     * (59/100)*100 = 58.9999961. If printing with 2 decimals, this will give
     * 59.00. This also indicates that many decimals should not be used ...
     */
    if (decimalArray[ucDecimals] > 4)
    {
        ucRoundUp = 1;
        for (i8J = ucDecimals - 1; i8J >= 0; i8J--)
        {
            decimalArray[i8J] = decimalArray[i8J] + ucRoundUp;
            if (decimalArray[i8J] == 10)
            {
                decimalArray[i8J] = 0;
            }
            else
            {
                ucRoundUp = 0;
            }
        }
        if (ucRoundUp == 1)
        {
            i32intPart++;
        }
    }

    /* Pend on the semaphore*/
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Writing LCD buffer (%p) timed out, exiting LCD_bufferPrintFloat.",
                bufIndex);
        return;
    }

    /* Print negative sign if applicable*/
    if (ucNumNeg == 1)
    {
        LCD_doBufferPrintString(handle, bufIndex, "-", ucX, iPage, false);
        ucX += LCD_CHAR_WIDTH;
    }

    /* Print integer part */
    LCD_doBufferPrintInt(handle, bufIndex, i32intPart, ucX, iPage, false);

    /* Print integer/decimal separator */
    ucX += LCD_getIntLength(i32intPart, 10) * LCD_CHAR_WIDTH;
    LCD_doBufferPrintString(handle, bufIndex, ".", ucX, iPage, false);
    ucX += LCD_CHAR_WIDTH;

    /* Print decimals */
    for (ucI = 0; ucI < ucDecimals; ucI++)
    {
        LCD_doBufferPrintInt(handle, bufIndex, decimalArray[ucI], ucX, iPage,
                false);
        ucX += LCD_CHAR_WIDTH;
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 *  ======== LCD_bufferPrintFloat ========
 *  This function writes an aligned floating point number to the specified
 *  buffer.
 */
void LCD_bufferPrintFloatAligned(LCD_Handle handle, unsigned int bufIndex, float fNumber,
                                    unsigned char ucDecimals, LCD_Align iAlignment, LCD_Page iPage)
{
    unsigned char ucX;
    unsigned char ucStrSize = LCD_getFloatLength(fNumber, ucDecimals);

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Calculate X offset based on alignment */
    switch (iAlignment)
    {
        case LCD_ALIGN_CENTER:
            ucX = LCD_COLS / 2 - ucStrSize * LCD_CHAR_WIDTH / 2;
            break;
        case LCD_ALIGN_RIGHT:
            ucX = LCD_COLS - ucStrSize * LCD_CHAR_WIDTH;
            break;
        case LCD_ALIGN_LEFT:
        default:
            ucX = 0;
            break;
    }

    /* Print Float to buffer */
    LCD_bufferPrintFloat(handle, bufIndex, fNumber, ucDecimals, ucX, iPage);
}

/*
 *  ======== LCD_bufferSetLine ========
 *  This function draws a line into the specified buffer.
 */
void LCD_bufferSetLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                       unsigned char ucYFrom, unsigned char ucXTo, unsigned char ucYTo)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Draw line */
    LCD_bufferLine(handle, bufIndex, ucXFrom, ucYFrom, ucXTo, ucYTo, 1);
}

/*
 *  ======== LCD_bufferClearLine ========
 *  This function clears a line intoo the specified buffer.
 */
void LCD_bufferClearLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                          unsigned char ucYFrom, unsigned char ucXTo, unsigned char ucYTo)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Clear line */
    LCD_bufferLine(handle, bufIndex, ucXFrom, ucYFrom, ucXTo, ucYTo, 0);
}

/*
 *  ======== LCD_bufferSetHLine ========
 *  This function draws a horizontal line into the specified buffer.
 */
void LCD_bufferSetHLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                        unsigned char ucXTo, unsigned char ucY)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call LCD_doBufferSetHLine with use of semaphore */
    LCD_doBufferSetHLine(handle, bufIndex, ucXFrom, ucXTo, ucY, true);

}

/*
 *  ======== LCD_bufferClearHLine ========
 *  This function clears a horizontal line from the specified buffer.
 */
void LCD_bufferClearHLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                          unsigned char ucXTo, unsigned char ucY)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call LCD_doBufferClearHLine with use of semaphore */
    LCD_doBufferClearHLine(handle, bufIndex, ucXFrom, ucXTo, ucY, true);

}

/*
 *  ======== LCD_bufferSetVLine ========
 *  This function draws a vertical line into the specified buffer.
 */
void LCD_bufferSetVLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                        unsigned char ucYFrom, unsigned char ucYTo)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call LCD_doBufferSetVLine with use of semaphore */
    LCD_doBufferSetVLine(handle, bufIndex, ucX, ucYFrom, ucYTo, true);

}

/*
 *  ======== LCD_bufferClearVLine ========
 *  This function clears a vertical line from the specified buffer.
 */
void LCD_bufferClearVLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                          unsigned char ucYFrom, unsigned char ucYTo)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call LCD_doBufferClearVLine with use of semaphore */
    LCD_doBufferClearVLine(handle, bufIndex, ucX, ucYFrom, ucYTo, true);

}

/*
 *  ======== LCD_bufferHArrow ========
 *  This function draws a horizontal arrow to the specified buffer.
 */
void LCD_bufferHArrow(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                      unsigned char ucXTo, unsigned char ucY)
{

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointer to the buffer semaphore. */
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore*/
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Writing LCD buffer (%p) timed out, exiting LCD_bufferHArrow.",
                bufIndex);
        return;
    }

    if (ucXTo > ucXFrom)
    {
        /* Draw left-to-right arrow */
        LCD_doBufferSetHLine(handle, bufIndex, ucXFrom, ucXTo, ucY, false);
        LCD_doBufferSetVLine(handle, bufIndex, ucXTo - 1, ucY - 1, ucY + 1,
                false);
        LCD_doBufferSetVLine(handle, bufIndex, ucXTo - 2, ucY - 2, ucY + 2,
                false);
    }
    else if (ucXTo < ucXFrom)
    {
        /* Draw right-to-left arrow */
        LCD_doBufferSetHLine(handle, bufIndex, ucXTo, ucXFrom, ucY, false);
        LCD_doBufferSetVLine(handle, bufIndex, ucXTo + 1, ucY - 1, ucY + 1,
                false);
        LCD_doBufferSetVLine(handle, bufIndex, ucXTo + 2, ucY - 2, ucY + 2,
                false);
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 *  ======== LCD_bufferVArrow ========
 *  This function draws a vertical arrow to the specified buffer.
 */
void LCD_bufferVArrow(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                      unsigned char ucYFrom, unsigned char ucYTo)
{

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if (bufIndex >= object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointers to the buffer semaphore. */
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore*/
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Writing LCD buffer (%p) timed out, exiting LCD_bufferVArrow.",
                bufIndex);
        return;
    }

    /* Draw the line */
    LCD_doBufferSetVLine(handle, bufIndex, ucX, ucYFrom, ucYTo, false);

    /* Draw arrowhead */
    LCD_doBufferSetHLine(handle, bufIndex, ucX - 1, ucX + 1, ucYTo - 1, false);
    LCD_doBufferSetHLine(handle, bufIndex, ucX - 2, ucX + 2, ucYTo - 2, false);

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 *  ======== LCD_bufferSetPx ========
 *  This function sets a pixel in the specified buffer.
 */
void LCD_bufferSetPx(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX, unsigned char ucY)
{
   /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call LCD_doBufferSetPx with use of semaphore */
    LCD_doBufferSetPx(handle, bufIndex, ucX, ucY, true);
}

/*
 *  ======== LCD_bufferClearPx ========
 *  This function clears a pixel in the specified buffer.
 */
void LCD_bufferClearPx(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX, unsigned char ucY)
{

    /* Do a check on buffer index */
    if (bufIndex >= handle->object->nBuffers)
    {
        DebugP_log1("The LCD driver has only %d buffers availible", handle->object->nBuffers);
        return;
    }

    /* Call LCD_doBufferClearPx with use of semaphore */
    LCD_doBufferClearPx(handle, bufIndex, ucX, ucY, true);
}

/*
 *  ======== LCD_bufferCopy ========
 *  This function copies the content of one buffer to another
 */
void LCD_bufferCopy(LCD_Handle handle, unsigned int fromBufIndex, unsigned int toBufIndex)
{

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Do a check on buffer index */
    if ((fromBufIndex >= object->nBuffers) || (toBufIndex >= object->nBuffers))
    {
        DebugP_log1("The LCD driver has only %d buffers availible", object->nBuffers);
        return;
    }

    /* Get pointers to the buffer and its semaphore. */
    char *pcFromBuffer = object->lcdBuffers[fromBufIndex].pcBuffer;
    char *pcToBuffer = object->lcdBuffers[toBufIndex].pcBuffer;
    SemaphoreP_Struct *pSemFrom = &(object->lcdBuffers[fromBufIndex].bufMutex);
    SemaphoreP_Struct *pSemTo = &(object->lcdBuffers[toBufIndex].bufMutex);

    /* Get buffer sizes */
    unsigned int fromBufSize = object->lcdBuffers[fromBufIndex].bufSize;
    unsigned int toBufSize = object->lcdBuffers[toBufIndex].bufSize;

    char *pcTmpToBuf = pcToBuffer;
    char *pcTmpFromBuf = (char *) pcFromBuffer;
    register unsigned short i;

    /* If buffers are the same, do nothing */
    if (pcFromBuffer == pcToBuffer)
    {
        DebugP_log1(Diags_USER1, "Buffers are the same, nothing to be done.");
        return;
    }
    /* Return if to-buffer is smaller than from-buffer */
    if (toBufSize < fromBufSize)
    {
        DebugP_log0("The receive buffer cannot be smaller than the transmit buffer. Copy aborted.");
        return;
    }

    /* Pend on the semaphores*/
    if (SemaphoreP_OK != SemaphoreP_pend(pSemFrom, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Copying from LCD buffer (%p) timed out, exiting LCD_bufferCopy.",
                fromBufIndex);
        return;
    }
    /* Pend on the semaphore*/
    if (SemaphoreP_OK != SemaphoreP_pend(pSemTo, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Copying to LCD buffer (%p) timed out, exiting LCD_bufferCopy.",
                toBufIndex);
        return;
    }

    /* Copy */
    for (i = 0; i < fromBufSize; i++)
    {
        pcTmpToBuf[i] = pcTmpFromBuf[i];
    }

    /* Finished with buffers - post on semaphores*/
    SemaphoreP_post(pSemFrom);
    SemaphoreP_post(pSemTo);
}

/*
 * ======== LCD_setContrast ========
 * This function sets the LCD contrast.
 */
void LCD_setContrast(LCD_Handle handle, unsigned char ucContrast)
{
    char pCmd[2];

    /* Populate command array and send command */
    pCmd[0] = 0x81;
    pCmd[1] = (ucContrast & 0x3f);
    LCD_sendCommand(handle, pCmd, 2);
}

/*
 *  ======== LCD_hwInit ========
 *  This functions initializes the LCD hardware module.
 *  It returns true if initialization was successful, false otherwise
 */
static bool LCD_initHw(LCD_Handle handle)
{
    /* Locals */
    PIN_Config lcdPinTable[4];
    uint32_t i = 0;
    LCD_HWAttrs const *hwAttrs;

    /* get the pointer to the hwAttrs */
    hwAttrs = handle->hwAttrs;

    /* Populate LCD pin table and initilize pins*/
    lcdPinTable[i++] = hwAttrs->lcdModePin      | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL;
    lcdPinTable[i++] = hwAttrs->lcdCsnPin       | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL;
    lcdPinTable[i++] = hwAttrs->lcdResetPin     | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW  | PIN_PUSHPULL;
    lcdPinTable[i++] = PIN_TERMINATE;

    /* Open and assign pins through pin driver*/
    hPin = PIN_open(&pinState, lcdPinTable);
    if(!hPin)
    {
        /* Pin allocation failed, pins may already be allocated */
        return (false);
    }

    /* Delay ~100 ms for LCD to be powered up. */
    ClockP_usleep(100 * 1000);

    /* Clear reset (set high)*/
    PIN_setOutputValue(hPin, hwAttrs->lcdResetPin, 1);

    return (true);
}

/*
 *  ======== LCD_sendCommand ========
 *
 *  This function sends @e ucLen bytes of commands to the
 *  LCD controller.
 */
static void LCD_sendCommand(LCD_Handle handle, const char *pcCmd, unsigned char ucLen)
{
    /* Get the pointer to the LCD object*/
    LCD_HWAttrs const *hwAttrs = handle->hwAttrs;

    DebugP_assert(handle != NULL);

    /* Set LCD mode signal low (command) */
    PIN_setOutputValue(hPin, hwAttrs->lcdModePin, 0);
    /* Set LCD CSn low (spi active) */
    PIN_setOutputValue(hPin, hwAttrs->lcdCsnPin, 0);

    /* Do SPI transfer */
    if (LCD_sendArray(handle, pcCmd, ucLen)) {
        /* Clear CSn */
        PIN_setOutputValue(hPin, hwAttrs->lcdCsnPin, 1);
    }

}

/*
 *  ======== LCD_sendData ========
 *
 *  This function sends @e usLen bytes of data to be displayed
 *  on the LCD.
 */
static void LCD_sendData(LCD_Handle handle, const char *pcData, unsigned short usLen)
{
    /* Get the pointer to the LCD object*/
    LCD_HWAttrs const *hwAttrs = handle->hwAttrs;

    DebugP_assert(handle != NULL);

    /* Set LCD mode signal (data) */
    PIN_setOutputValue(hPin, hwAttrs->lcdModePin, 1);
    /* Set LCD CSn low (spi active) */
    PIN_setOutputValue(hPin, hwAttrs->lcdCsnPin, 0);

    /* Do SPI transfer */
    if (LCD_sendArray(handle, pcData, usLen)) {
        /* Clear CSn */
        PIN_setOutputValue(hPin, hwAttrs->lcdCsnPin, 1);
    }

}

/*
 *  ======== LCD_gotoXY ========
 *
 *  Function that sets the internal data cursor of the LCD to the
 *  location specified by @e ucX and @e ucY. When data is sent to the
 *  display, data will start printing at internal cursor location.
 */
static void LCD_gotoXY(LCD_Handle handle, unsigned char ucX, unsigned char ucY)
{

    DebugP_assert(handle != NULL);

    unsigned char cmd[] = { 0xB0, 0x10, 0x00 };

    /* Adding Y position, and X position (hi/lo nibble) to command array */
    cmd[0] = cmd[0] + ucY;
    cmd[2] = cmd[2] + (ucX & 0x0F);
    cmd[1] = cmd[1] + (ucX >> 4);

    LCD_sendCommand(handle, (char *) cmd, 3);

}

/*
 *  ======== LCD_sendArray ========
 *  This function sends @e usLen bytes from starting from address
 *  @e pcData over SPI to the LCD controller. This function only pushes
 *  data to the SPI module. It does not manipulate the LCD display's
 *  CSn signal, nor the LCD mode signal (A0).
 */
static bool LCD_sendArray(LCD_Handle handle, const char *pcData, unsigned short usLen)
{

    /* Get the pointer to the LCD object*/
    LCD_Object *object = handle->object;

    /* Do SPI transfer */
    SPI_Transaction spiTransaction;
    spiTransaction.arg = NULL;
    spiTransaction.count = usLen;
    spiTransaction.txBuf = (void *)pcData;
    spiTransaction.rxBuf = NULL;

    bool ret = SPI_transfer(object->spiHandle, &spiTransaction);
    if(ret == false){
        return false;
    }

    /* return... */
    return true;
}

/*
 *  ======== LCD_bufferLine ========
 *  Local function. Draws or clears (based on @e ucDraw) a line from
 *  @e (ucXFrom,ucYFrom) to @e (ucXTo,ucYTo). Uses Bresenham's line algorithm.
 */
static void LCD_bufferLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                           unsigned char ucYFrom, unsigned char ucXTo, unsigned char ucYTo, unsigned char ucDraw)
{
    signed char cX, cY, cDeltaY, cDeltaX, cD;
    signed char cXDir, cYDir;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer semaphore. */SemaphoreP_Struct *pSem =
            &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore*/
    if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
    {
        /* Semaphore timed out, log and return. */
        DebugP_log1("Writing LCD buffer (%p) timed out, action aborted.",
                bufIndex);
        return;
    }

    if (ucXFrom == ucXTo)
    {
        /* Vertical line */
        if (ucDraw)
        {
            LCD_doBufferSetVLine(handle, bufIndex, ucXFrom, ucYFrom, ucYTo,
                    false);
        }
        else
        {
            LCD_doBufferClearVLine(handle, bufIndex, ucXFrom, ucYFrom, ucYTo,
                    false);
        }
    }
    else if (ucYFrom == ucYTo)
    {
        /* Horizontal line */
        if (ucDraw)
        {
            LCD_doBufferSetHLine(handle, bufIndex, ucXFrom, ucXTo, ucYFrom,
                    false);
        }
        else
        {
            LCD_doBufferClearHLine(handle, bufIndex, ucXFrom, ucXTo, ucYFrom,
                    false);
        }
    }
    else
    {

        /* Diagonal Line => Bresenham's algorithm
         * Determine X and Y direction
         */
        cXDir = (ucXFrom > ucXTo) ? -1 : 1;
        cYDir = (ucYFrom > ucYTo) ? -1 : 1;

        /* Set start position and calculate X and Y delta */
        cX = ucXFrom;
        cY = ucYFrom;
        cDeltaY = ucYTo - ucYFrom;
        cDeltaX = ucXTo - ucXFrom;

        /* Take absolute value of X and Y delta */
        if (cDeltaY < 0)
        {
            cDeltaY *= -1;
        }
        if (cDeltaX < 0)
        {
            cDeltaX *= -1;
        }

        /* Determine principal direction and draw line */
        if (cDeltaX >= cDeltaY)
        {
            cD = (cDeltaY << 1) - cDeltaX;
            while (cX != ucXTo)
            {
                if (ucDraw)
                {
                    LCD_doBufferSetPx(handle, bufIndex, cX, cY, false);
                }
                else
                {
                    LCD_doBufferClearPx(handle, bufIndex, cX, cY, false);
                }

                if (cD < 0)
                {
                    cD += (cDeltaY << 1);
                }
                else
                {
                    cD += ((cDeltaY - cDeltaX) << 1);
                    cY += cYDir;
                }
                cX += cXDir;
            }
        }
        else
        {
            cD = (cDeltaX << 1) - cDeltaY;
            while (cY != ucYTo)
            {
                if (ucDraw)
                {
                    LCD_doBufferSetPx(handle, bufIndex, cX, cY, false);
                }
                else
                {
                    LCD_doBufferClearPx(handle, bufIndex, cX, cY, false);
                }
                if (cD < 0)
                {
                    cD += (cDeltaX << 1);
                }
                else
                {
                    cD += ((cDeltaX - cDeltaY) << 1);
                    cX += cXDir;
                }
                cY += cYDir;
            }
        }
    }

    /* Finished with buffer - post on semaphore*/
    SemaphoreP_post(pSem);
}

/*
 *  ======== LCD_doUpdate ========
 *  This function sends the specified buffer to the display.
 *  If blocking is set to true, the task execution will be blocked until all
 *  the data in buffer has been written to the LCD.
 */
static void LCD_doUpdate(LCD_Handle handle, unsigned int bufIndex, bool blocking)
{

    DebugP_assert(handle != NULL);

    unsigned char ucPage;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Pend on LCD mutex if blocking option is set*/
    if (blocking)
    {
        DebugP_log0("LCD_update: pending on LCD mutex.");
        if (SemaphoreP_OK != SemaphoreP_pend(&object->lcdMutex, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log0("Waiting for access to LCD timed out, exiting LCD_update.");
            return;
        }
    }

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);
    /* Pend on the semaphore if blocking option is set*/
    if(blocking)
    {
        DebugP_log1("LCD_update: pending on semaphore associated with buffer %p",
                    bufIndex);
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting LCD_update.",
                            bufIndex);
            SemaphoreP_post(&(object->lcdMutex));
            return;
        }
    }

    /* For each page */
    for(ucPage = 0; ucPage < LCD_PAGE_COUNT; ucPage++)
    {
        /* Set LCD pointer to start of the correct page and send data */
        LCD_gotoXY(handle, 0, ucPage);
        LCD_sendData(handle, pcBuf + (ucPage * LCD_COLS), LCD_COLS);
    }

    /* Finished with buffer - post on semaphore*/
    if(blocking)
    {
        DebugP_log1("LCD_update: posting semaphore associated with buffer %p", (UArg)pcBuf);
        SemaphoreP_post(pSem);
        DebugP_log0("LCD_update: posting LCD mutex.");
        SemaphoreP_post(&(object->lcdMutex));
    }
}

/*!
 *  ======== LCD_doBufferClearPage ========
 *  This function clears the page specified by @e iPage in the given buffer
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 */
static void LCD_doBufferClearPage(LCD_Handle handle, unsigned int bufIndex,
                                 LCD_Page iPage, bool blocking)
{
    unsigned char ucIdx;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore if blocking option is set*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferClearPage.", bufIndex);
            return;
        }
    }

    /* Clear page in buffer */
    for (ucIdx = 0; ucIdx < LCD_COLS; ucIdx++)
    {
        *(pcBuf + (iPage * LCD_COLS + ucIdx)) = 0x00;
    }

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferPrintString ========
 *  This function writes a string to the specified buffer
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 *
 */
static void LCD_doBufferPrintString(LCD_Handle handle, unsigned int bufIndex, const char *pcStr,
                                    unsigned char ucX, LCD_Page iPage, bool blocking)
{
    DebugP_assert(handle != NULL);

    unsigned char ucI, ucJ;
    unsigned short firstIndex;
    char *pcBuf;
    SemaphoreP_Struct *pSem;
    LCD_Object *object;

    unsigned char ucStrSize = LCD_getStringLength(pcStr);
    unsigned short usFirstPos = iPage * LCD_COLS + ucX;

    /* Get pointer to the LCD object */
    object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore if blocking option is set*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferPrintString.", bufIndex);
            return;
        }
    }

    /* Running through each letter in input string */
    for (ucI = 0; ucI < ucStrSize; ucI++)
    {
        if (pcStr[ucI] == ' ')
        {
            /* Write space character */
            for (ucJ = 0; ucJ < LCD_CHAR_WIDTH; ucJ++)
            {
                *(pcBuf + (usFirstPos + LCD_CHAR_WIDTH * ucI + ucJ)) = 0x00;
            }
        }
        else
        {
            /* Index to the beginning of the current letter in lcd_alphabet[] */
            firstIndex = ((unsigned short) (pcStr[ucI]) - 33) * LCD_FONT_WIDTH;

            /* Stores each vertical column of the current letter in the result */
            for (ucJ = 0; ucJ < LCD_FONT_WIDTH; ucJ++)
            {
                *(pcBuf + (usFirstPos + LCD_CHAR_WIDTH * ucI + ucJ)) =
                        LCD_alphabet[firstIndex + ucJ];
            }

            /* Add a single pixel spacing after each letter */
            *(pcBuf + (usFirstPos + LCD_CHAR_WIDTH * ucI + LCD_FONT_WIDTH)) =
                    0x00;
        }
    }

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferPrintInt ========
 *  This function writes an integer to the specified buffer
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 *
 */
static void LCD_doBufferPrintInt(LCD_Handle handle, unsigned int bufIndex, int i32Number,
                                 unsigned char ucX, LCD_Page iPage, bool blocking)
{
    DebugP_assert(handle != NULL);

    signed char cI;
    unsigned char ucJ, ucNumOfDigits;
    int i32Temp, i32Digit, i32FirstIdx;
    LCD_Object *object;
    unsigned short usFirstPos = iPage * LCD_COLS + ucX;

    /* Get pointer to the LCD object */
    object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* I number is negative: write a minus at the first position, increment
     position by one character and multiply number by (-1). */
    if (i32Number < 0)
    {
        for (ucJ = 0; ucJ < LCD_FONT_WIDTH; ucJ++)
        {
            *(pcBuf + (usFirstPos + ucJ)) = LCD_alphabet[12 * LCD_FONT_WIDTH
                    + ucJ];
        }
        *(pcBuf + (usFirstPos + LCD_FONT_WIDTH)) = 0x00;   // Spacing
        usFirstPos += LCD_CHAR_WIDTH;
        i32Number *= (-1);
    }

    /* Find number of digits in i32Number, decimal base (not including minus character) */
    ucNumOfDigits = LCD_getIntLength(i32Number, 10);

    /* Pend on the semaphore if blocking option is set */
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                                     "LCD_bufferPrintInt.", bufIndex);
            return;
        }
    }

    /* For each digit (most significant first), write to buffer */
    for (cI = (ucNumOfDigits - 1); cI >= 0; cI--)
    {
        i32Temp = i32Number / 10;
        i32Digit = i32Number - i32Temp * 10;
        i32FirstIdx = (i32Digit + 15) * LCD_FONT_WIDTH;
        for (ucJ = 0; ucJ < LCD_FONT_WIDTH; ucJ++)
        {
            *(pcBuf + (usFirstPos + LCD_CHAR_WIDTH * cI + ucJ)) =
                    LCD_alphabet[i32FirstIdx + ucJ];
        }

        /* character spacing */
        *(pcBuf + (usFirstPos + LCD_CHAR_WIDTH * cI + LCD_FONT_WIDTH)) = 0x00;
        i32Number = i32Temp;
    }

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }

}

/*
 *  ======== LCD_doBufferSetHLine ========
 *  This function draws a horizontal line into the specified buffer.
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 */
static void LCD_doBufferSetHLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucXFrom,
                                 unsigned char ucXTo, unsigned char ucY, bool blocking)
{
    unsigned char ucI;
    unsigned char ucPage = ucY / LCD_PAGE_ROWS;
    unsigned char bit = ucY % LCD_PAGE_ROWS;
    unsigned char bitmask = 1 << bit;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Switch draw direction if ucXTo < ucXFrom */
    if (ucXTo < ucXFrom)
    {
        unsigned char ucTemp = ucXFrom;
        ucXFrom = ucXTo;
        ucXTo = ucTemp;
    }

    /* Pend on the semaphore if blocking option is set*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferSetHLine.", bufIndex);
            return;
        }
    }

    /* Draw line */
    for (ucI = ucXFrom; ucI <= ucXTo; ucI++)
    {
        *(pcBuf + (ucPage * LCD_COLS + ucI)) |= bitmask;
    }

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferClearHLine ========
 *  This function clears a horizontal line from the specified buffer.
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 *
 */
static void LCD_doBufferClearHLine(LCD_Handle handle, unsigned int bufIndex,
                                   unsigned char ucXFrom, unsigned char ucXTo, unsigned char ucY, bool blocking)
{
    unsigned char ucI;
    unsigned char ucPage = ucY / LCD_PAGE_ROWS;
    unsigned char ucBit = ucY % LCD_PAGE_ROWS;
    unsigned char ucBitmask = 1 << ucBit;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Switch draw direction if ucXTo < ucXFrom */
    if (ucXTo < ucXFrom)
    {
        unsigned char ucTemp = ucXFrom;
        ucXFrom = ucXTo;
        ucXTo = ucTemp;
    }

    /* Pend on the semaphore if blocking option is set*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferClearHLine.", bufIndex);
            return;
        }
    }

    /* Clear line */
    for (ucI = ucXFrom; ucI <= ucXTo; ucI++)
    {
        *(pcBuf + (ucPage * LCD_COLS + ucI)) &= ~ucBitmask;
    }

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferSetVLine ========
 *  This function draws a vertical line into the specified buffer.
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 */
static void LCD_doBufferSetVLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                                 unsigned char ucYFrom, unsigned char ucYTo, bool blocking)
{
    unsigned char ucI, ucPow;
    unsigned char ucPage, ucFirstPage, ucLastPage;
    unsigned char ucFirstPageMask, ucLastPageMask;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Calculate first and last LCD page */
    ucFirstPage = ucYFrom / LCD_PAGE_ROWS;
    ucLastPage = ucYTo / LCD_PAGE_ROWS;

    /*  Find the bitmask to use with the first page */
    ucFirstPageMask = 0xFF;
    ucPow = 1;
    for (ucI = 0; ucI < LCD_PAGE_ROWS; ucI++)
    {
        if (ucYFrom - ucFirstPage * LCD_PAGE_ROWS > ucI)
        {
            ucFirstPageMask -= ucPow;
            ucPow *= 2;
        }
    }

    /* Find the bitmask to use with the last page */
    ucLastPageMask = 0x00;
    ucPow = 1;
    for (ucI = 0; ucI < LCD_PAGE_ROWS; ucI++)
    {
        if (ucYTo - ucLastPage * LCD_PAGE_ROWS >= ucI)
        {
            ucLastPageMask += ucPow;
            ucPow *= 2;
        }
    }

    /* Handle lines spanning over a single page */
    if (ucLastPage == ucFirstPage)
    {
        ucFirstPageMask &= ucLastPageMask;
        ucLastPageMask = ucFirstPageMask;
    }

    /* Pend on the semaphore if blocking option is set*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferSetVLine.", bufIndex);
            return;
        }
    }

    /* Draw line in buffer */
    *(pcBuf + (ucFirstPage * LCD_COLS + ucX)) |= ucFirstPageMask;
    for (ucPage = (ucFirstPage + 1); ucPage <= (ucLastPage - 1); ucPage++)
    {
        *(pcBuf + (ucPage * LCD_COLS + ucX)) |= 0xFF;
    }
    *(pcBuf + (ucLastPage * LCD_COLS + ucX)) |= ucLastPageMask;

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferClearVLine ========
 *  This function clears a vertical line from the specified buffer.
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 */
static void LCD_doBufferClearVLine(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                                   unsigned char ucYFrom, unsigned char ucYTo, bool blocking)
{
    unsigned char ucI, ucPow;
    unsigned char ucPage, ucFirstPage, ucLastPage;
    unsigned char ucFirstPageMask, ucLastPageMask;

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Calculate first and last LCD page */
    ucFirstPage = ucYFrom / LCD_PAGE_ROWS;
    ucLastPage = ucYTo / LCD_PAGE_ROWS;

    /* Find the bitmask to use with the first page */
    ucFirstPageMask = 0xFF;
    ucPow = 1;
    for (ucI = 0; ucI < LCD_PAGE_ROWS; ucI++)
    {
        if (ucYFrom - ucFirstPage * LCD_PAGE_ROWS > ucI)
        {
            ucFirstPageMask -= ucPow;
            ucPow *= 2;
        }
    }

    /* Find the bitmask to use with the last page */
    ucLastPageMask = 0x00;
    ucPow = 1;
    for (ucI = 0; ucI < LCD_PAGE_ROWS; ucI++)
    {
        if (ucYTo - ucLastPage * LCD_PAGE_ROWS >= ucI)
        {
            ucLastPageMask += ucPow;
            ucPow *= 2;
        }
    }

    /* Handle lines that span a single page */
    if (ucLastPage == ucFirstPage)
    {
        ucFirstPageMask &= ucLastPageMask;
        ucLastPageMask = ucFirstPageMask;
    }

    /* Pend on the semaphore if blocking option is set*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferClearVLine.", bufIndex);
            return;
        }
    }

    /* Clear line from buffer */
    *(pcBuf + (ucFirstPage * LCD_COLS + ucX)) &= ~ucFirstPageMask;
    for (ucPage = (ucFirstPage + 1); ucPage <= (ucLastPage - 1); ucPage++)
    {
        *(pcBuf + (ucPage * LCD_COLS + ucX)) &= 0x00;
    }
    *(pcBuf + (ucLastPage * LCD_COLS + ucX)) &= ~ucLastPageMask;

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferSetPx ========
 *  This function sets a pixel in the specified buffer.
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 */
static void LCD_doBufferSetPx(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                              unsigned char ucY, bool blocking)
{
    uint_fast8_t ucPage = ucY / LCD_PAGE_ROWS;
    uint_fast8_t ucBitmask = 1 << (ucY % LCD_PAGE_ROWS);

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferSetPx.", bufIndex);
            return;
        }
    }

    /* Draw pixel */
    *(pcBuf + (ucPage * LCD_COLS + ucX)) |= ucBitmask;

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}

/*
 *  ======== LCD_doBufferClearPx ========
 *  This function clears a pixel in the specified buffer.
 *  If blocking is set to true, the task execution will be blocked until all
 *  buffer modifications have finished.
 *
 */
static void LCD_doBufferClearPx(LCD_Handle handle, unsigned int bufIndex, unsigned char ucX,
                                unsigned char ucY, bool blocking)
{
    uint_fast8_t ucPage = ucY / LCD_PAGE_ROWS;
    uint_fast8_t ucBitmask = 1 << (ucY % LCD_PAGE_ROWS);

    /* Get pointer to the LCD object */
    LCD_Object *object = handle->object;

    /* Get pointers to the buffer and its semaphore. */
    char *pcBuf = object->lcdBuffers[bufIndex].pcBuffer;
    SemaphoreP_Struct *pSem = &(object->lcdBuffers[bufIndex].bufMutex);

    /* Pend on the semaphore*/
    if (blocking)
    {
        if (SemaphoreP_OK != SemaphoreP_pend(pSem, object->lcdWriteTimeout))
        {
            /* Semaphore timed out, log and return. */
            DebugP_log1("Waiting for access to buffer (%p) timed out, exiting"
                         "LCD_bufferClearPx.", bufIndex);
            return;
        }
    }

    /* Clear pixel */
    *(pcBuf + (ucPage * LCD_COLS + ucX)) &= ~ucBitmask;

    /* Finished with buffer - post on semaphore*/
    if (blocking)
    {
        SemaphoreP_post(pSem);
    }
}
