/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
 *  ======== spislave.c ========
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define THREADSTACKSIZE (1024)

#define SPI_MSG_LENGTH  (30)
#define SLAVE_MSG       ("Hello from slave, msg#: ")

#define MAX_LOOP        (10)

static Display_Handle display;

unsigned char slaveRxBuffer[SPI_MSG_LENGTH];
unsigned char slaveTxBuffer[SPI_MSG_LENGTH];

/* Semaphore to block slave until transfer is complete */
sem_t slaveSem;

/* Status indicating whether or not SPI transfer succeeded. */
bool transferStatus;

/*
 *  ======== transferCompleteFxn ========
 *  Callback function for SPI_transfer().
 */
void transferCompleteFxn(SPI_Handle handle, SPI_Transaction *transaction)
{
    if (transaction->status != SPI_TRANSFER_COMPLETED) {
        transferStatus = false;
    }
    else {
        transferStatus = true;
    }

    sem_post(&slaveSem);
}

/*
 * ======== slaveThread ========
 *  Slave SPI sends a message to master while simultaneously receiving a
 *  message from the master.
 */
void *slaveThread(void *arg0)
{
    SPI_Handle      slaveSpi;
    SPI_Params      spiParams;
    SPI_Transaction transaction;
    uint32_t        i;
    bool            transferOK;
    int32_t         status;

    /*
     * CONFIG_SPI_MASTER_READY & CONFIG_SPI_SLAVE_READY are GPIO pins connected
     * between the master & slave.  These pins are used to synchronize
     * the master & slave applications via a small 'handshake'.  The pins
     * are later used to synchronize transfers & ensure the master will not
     * start a transfer until the slave is ready.  These pins behave
     * differently between spimaster & spislave examples:
     *
     * spislave example:
     *     * CONFIG_SPI_MASTER_READY is configured as an input pin.  During the
     *       'handshake' this pin is read & a high value will indicate the
     *       master is ready to run the application.  Afterwards, the pin is
     *       read to determine if the master has already opened its SPI pins.
     *       The master will pull this pin low when it has opened its SPI.
     *
     *     * CONFIG_SPI_SLAVE_READY is configured as an output pin.  During the
     *       'handshake' this pin is changed from low to high output.  This
     *       notifies the master the slave is ready to run the application.
     *       Afterwards, the pin is used by the slave to notify the master it
     *       is ready for a transfer.  When ready for a transfer, this pin will
     *       be pulled low.
     *
     * Below we set CONFIG_SPI_MASTER_READY & CONFIG_SPI_SLAVE_READY initial
     * conditions for the 'handshake'.
     */
    GPIO_setConfig(CONFIG_SPI_SLAVE_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_SPI_MASTER_READY, GPIO_CFG_INPUT);

    /*
     * Handshake - Set CONFIG_SPI_SLAVE_READY high to indicate slave is ready
     * to run.  Wait for CONFIG_SPI_MASTER_READY to be high.
     */
    GPIO_write(CONFIG_SPI_SLAVE_READY, 1);
    while (GPIO_read(CONFIG_SPI_MASTER_READY) == 0) {}

    /*
     * Create synchronization semaphore; this semaphore will block the slave
     * until a transfer is complete.  The slave is configured in callback mode
     * to allow us to configure the SPI transfer & then notify the master the
     * slave is ready.  However, we must still wait for the current transfer
     * to be complete before setting up the next.  Thus, we wait on slaveSem;
     * once the transfer is complete the callback function will unblock the
     * slave.
     */
    status = sem_init(&slaveSem, 0, 0);
    if (status != 0) {
        Display_printf(display, 0, 0, "Error creating slaveSem\n");

        while(1);
    }

    /*
     * Wait until master SPI is open.  When the master is configuring SPI pins
     * the clock may toggle from low to high (or high to low depending on
     * polarity).  If using 3-pin SPI & the slave has been opened before the
     * master, clock transitions may cause the slave to shift bits out assuming
     * it is an actual transfer.  We can prevent this behavior by opening the
     * master first & then opening the slave.
     */
    while (GPIO_read(CONFIG_SPI_MASTER_READY)) {}

    /*
     * Open SPI as slave in callback mode; callback mode is used to allow us to
     * configure the transfer & then set CONFIG_SPI_SLAVE_READY high.
     */
    SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.mode = SPI_SLAVE;
    spiParams.transferCallbackFxn = transferCompleteFxn;
    spiParams.transferMode = SPI_MODE_CALLBACK;
    slaveSpi = SPI_open(CONFIG_SPI_SLAVE, &spiParams);
    if (slaveSpi == NULL) {
        Display_printf(display, 0, 0, "Error initializing slave SPI\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, "Slave SPI initialized\n");
    }

    /* Copy message to transmit buffer */
    strncpy((char *) slaveTxBuffer, SLAVE_MSG, SPI_MSG_LENGTH);

    for (i = 0; i < MAX_LOOP; i++) {
        /* Initialize slave SPI transaction structure */
        slaveTxBuffer[sizeof(SLAVE_MSG) - 1] = (i % 10) + '0';
        memset((void *) slaveRxBuffer, 0, SPI_MSG_LENGTH);
        transaction.count = SPI_MSG_LENGTH;
        transaction.txBuf = (void *) slaveTxBuffer;
        transaction.rxBuf = (void *) slaveRxBuffer;

        /* Toggle on user LED, indicating a SPI transfer is in progress */
        GPIO_toggle(CONFIG_GPIO_LED_1);

        /*
         * Setup SPI transfer; CONFIG_SPI_SLAVE_READY will be set to notify
         * master the slave is ready.
         */
        transferOK = SPI_transfer(slaveSpi, &transaction);
        if (transferOK) {
            GPIO_write(CONFIG_SPI_SLAVE_READY, 0);

            /* Wait until transfer has completed */
            sem_wait(&slaveSem);

            /*
             * Drive CONFIG_SPI_SLAVE_READY high to indicate slave is not ready
             * for another transfer yet.
             */
            GPIO_write(CONFIG_SPI_SLAVE_READY, 1);

            if (transferStatus == false) {
                Display_printf(display, 0, 0, "SPI transfer failed!");
            }
            else {
                Display_printf(display, 0, 0, "Slave received: %s",
                        slaveRxBuffer);
            }
        }
        else {
            Display_printf(display, 0, 0, "Unsuccessful slave SPI transfer");
        }
    }

    SPI_close(slaveSpi);

    /* Example complete - set pins to a known state */
    GPIO_setConfig(CONFIG_SPI_MASTER_READY, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_SPI_SLAVE_READY, 0);

    Display_printf(display, 0, 0, "\nDone");

    return (NULL);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    pthread_t           thread0;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;
    int                 detachState;

    /* Call driver init functions. */
    Display_init();
    GPIO_init();
    SPI_init();

    /* Configure the LED pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        /* Failed to open display driver */
        while (1);
    }

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    Display_printf(display, 0, 0, "Starting the SPI slave example");
    Display_printf(display, 0, 0, "This example requires external wires to be "
        "connected to the header pins. Please see the Board.html for details.\n");

    /* Create application thread */
    pthread_attr_init(&attrs);

    detachState = PTHREAD_CREATE_DETACHED;
    /* Set priority and stack size attributes */
    retc = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0) {
        /* pthread_attr_setdetachstate() failed */
        while (1);
    }

    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0) {
        /* pthread_attr_setstacksize() failed */
        while (1);
    }

    /* Create slave thread */
    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&attrs, &priParam);

    retc = pthread_create(&thread0, &attrs, slaveThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1);
    }

    return (NULL);
}
