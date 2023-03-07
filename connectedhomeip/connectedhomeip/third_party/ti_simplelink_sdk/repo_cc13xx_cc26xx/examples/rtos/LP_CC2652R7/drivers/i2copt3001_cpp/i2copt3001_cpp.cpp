/*
 * Copyright (c) 2019-2020, Texas Instruments Incorporated
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
 *  ======== i2copt3001_cpp.cpp ========
 */

#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>

/* Threading header files */
#include <pthread.h>
#include <semaphore.h>

/* Include OPT3001 driver file */
#include "OPT3001.h"

/* Driver Configuration */
#include "ti_drivers_config.h"

/*
 * These values narrow the limits of the sensor to
 * allow those running the example to test the interrupt feature
 */
#define TEMPORARY_LOWLIMIT   30
#define TEMPORARY_HIGHLIMIT  4000

/* Number of samples to read from the sensor */
#define NUM_SAMPLES 60

/* Stack size in bytes */
#define ALARMTHREADSTACKSIZE  512

/* Range of possible I2C slave addresses */
#define MIN_ADDRESS         0x08
#define MAX_ADDRESS         0x78

static Display_Handle display;
static OPT3001 opt3001;
static sem_t semaphoreAlarm;

/* Thread function prototypes */
extern "C" {
    void  *mainThread(void *arg0);
}

/*
 *  ======== opt3001LimitInterruptFxn ========
 *  Callback function for the OPT3001 Limit interrupt
 *  on CONFIG_GPIO_OPT3001_INTERRUPT.
 */
static void opt3001LimitInterruptFxn(uint_least8_t index)
{
    /* Clear the GPIO interrupt and toggle an LED */
    GPIO_toggle(CONFIG_GPIO_LED_0);
    /* Post the alarm semaphore to unblock the alarmThread*/
    sem_post(&semaphoreAlarm);
}

/*
 *  ======== alarmThread ========
 *  Prints an alarm message when the OPT3001 sensor gets a value outside
 *  of its limits.
 *
 *  Waits for a semaphore to be posted by opt3001LimitInterruptFxn.
 */
static void *alarmThread(void *arg0)
{
    OPT3001::InterruptFlag flag;
    while(1)
    {
        /* Wait for the semaphore before unblocking the thread */
        sem_wait(&semaphoreAlarm);

        /* The flag tells us if the value is crossing the high or low limit */
        flag = opt3001.getFlag();
        switch(flag)
        {
        case OPT3001::InterruptFlag::LOW:
            Display_printf(display, 0, 0,
                (char *)"\n\tALARM: Low limit crossed!");
            Display_printf(display, 0, 0,
                (char *)"\tThe sensor detected a value of %u lux.",
                opt3001.getResult());
            Display_printf(display, 0, 0,
                (char *)"\tThe low limit is %u lux.\n",
                opt3001.getLowLimit());
            break;
        case OPT3001::InterruptFlag::HIGH:
            Display_printf(display, 0, 0,
                (char *)"\n\tALARM: High limit crossed!");
            Display_printf(display, 0, 0,
                (char *)"\tThe sensor detected a value of %u lux.",
                opt3001.getResult());
            Display_printf(display, 0, 0,
                (char *)"\tThe high limit is %u lux.\n",
                opt3001.getHighLimit());
            break;
        case OPT3001::InterruptFlag::NONE:
        default:
              break;
        }
    }
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    uint16_t sample;
    int32_t luxValue;
    uint8_t data;
    uint8_t addr;

    /* I2C structures*/
    I2C_Handle      i2c;
    I2C_Params      i2cParams;
    I2C_Transaction i2cTransaction;

    /* Alarm pthread structures */
    pthread_t           alarm;
    pthread_attr_t      attrs;
    struct sched_param  priParam;
    int                 retc;

    /* Call driver init functions */
    Display_init();
    GPIO_init();
    I2C_init();
    sem_init(&semaphoreAlarm, 0, 0);

    /* Initialize the alarm thread attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes for alarm thread */
    priParam.sched_priority = 2;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, ALARMTHREADSTACKSIZE);
    if (retc != 0) {
        /* Failed to set attributes */
        while (1) {}
    }

    /* Create the alarm thread */
    retc = pthread_create(&alarm, &attrs, alarmThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        while (1) {}
    }

    /* Open the UART display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        while (1);
    }

    /*
     * Set OPT3001 Power pin to high (give device power) and then sleep briefly
     * to allow device to power up
     */
    GPIO_setConfig(CONFIG_GPIO_OPT3001_POWER,
        GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    sleep(1);

    /* Setup GPIO Interrupt Function */
    GPIO_setCallback(CONFIG_GPIO_OPT3001_INTERRUPT, opt3001LimitInterruptFxn);

    /* Create I2C for usage */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2c = I2C_open(CONFIG_I2C_OPT3001, &i2cParams);
    if (i2c == NULL) {
        Display_printf(display, 0, 0, (char *)"Error Initializing I2C!\n");
        while (1);
    }
    else {
        Display_printf(display, 0, 0, (char *)"I2C Initialized!");
    }

    /* Setup transaction to find slave devices */
    i2cTransaction.writeBuf = &data;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = &data;
    i2cTransaction.readCount = 0;
    data = 0;

    /*
     * Attempt transaction with every possible slave address. If a transaction
     * returns successfully, then that slave address is valid and will be
     * printed to the console.
     */
    for (addr = MIN_ADDRESS; addr < MAX_ADDRESS; addr++) {
        i2cTransaction.slaveAddress = addr;
        if (I2C_transfer(i2c, &i2cTransaction)) {
            Display_printf(display, 0, 0,
                (char *)"I2C device found at address 0x%x!", addr);
        }
    }

    Display_printf(display, 0, 0, (char *)"Finished looking for I2C devices.",
        addr);

    /* Make instance of OPT3001 and then enable the interrupt pin */
    if(!opt3001.init(i2c, OPT3001::SlaveAddress::ADDRPIN_GND, display))
    {
        Display_printf(display, 0, 0,
            (char *)"Failed to communicate with OPT3001!");
        while (1);
    }
    GPIO_enableInt(CONFIG_GPIO_OPT3001_INTERRUPT);

    /* Configure the LED Pin and turn on user LED */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Print basic OPT3001 device info */
    Display_printf(display, 0, 0, (char *)"Information about "
        "the OPT3001 device:");
    Display_printf(display, 0, 0, (char *)"I2C Slave Address:\t0x%x",
        OPT3001::SlaveAddress::ADDRPIN_GND);
    Display_printf(display, 0, 0, (char *)"Device ID:\t\t0x%x",
        opt3001.getDeviceID());
    Display_printf(display, 0, 0, (char *)"Manufacturer ID:\t0x%x",
        opt3001.getManufacturerID());
    opt3001.resetConfiguration();
    Display_printf(display, 0, 0, (char *)"Configuration Register:\t0x%x",
        opt3001.getConfiguration());
    Display_printf(display, 0, 0, (char *)"Low Limit:\t\t%u lux",
        opt3001.getLowLimit());
    Display_printf(display, 0, 0, (char *)"High Limit:\t\t%u lux\n",
        opt3001.getHighLimit());

    /* Set low limit & high limits */
    if(opt3001.setLowLimit(TEMPORARY_LOWLIMIT) &&
        opt3001.setHighLimit(TEMPORARY_HIGHLIMIT))
    {
        Display_printf(display, 0, 0,
            (char *)"Set low limit to %u lux", opt3001.getLowLimit());
        Display_printf(display, 0, 0,
            (char *)"Set high limit to %u lux\n", opt3001.getHighLimit());
    }
    else
    {
        Display_printf(display, 0, 0, (char *)"Failed to set limits");
        while(1);
    }


    /* Read data in continuous conversions mode */
    if(!opt3001.setConversionMode(OPT3001::ConversionMode::CONTINUOUS_CONVERSIONS))
    {
        Display_printf(display, 0, 0,
            (char *)"Failed to Set OPT3001 to continuous conversions mode.");
        while(1);
    }

    /*
     * Read the OPT3001 sensor NUM_SAMPLES times and display the values.
     * Sleep between readings
     */
    Display_printf(display, 0, 0, (char *)"\nReading samples from OPT3001:");

    for (sample = 1; sample <= NUM_SAMPLES; sample++) {

        /* Sleep between every sample */
        sleep(1);
        luxValue = opt3001.getResult();

        /* If -1 is returned from getResult(), then the I2C transaction failed */
        if(luxValue != -1)
        {
            Display_printf(display, 0, 0, (char *)"Sample: #%u)\t%u lux",
                sample, luxValue);
        }
    }

    /* Set OPT3001 to shutdown mode */
    if(!opt3001.setConversionMode(OPT3001::ConversionMode::SHUTDOWN))
    {
        Display_printf(display, 0, 0,
            (char *)"Failed to set device to shutdown mode");
    }

    /* Reset limits back to default values */
    if(!(opt3001.resetLowLimit() && opt3001.resetHighLimit()))
    {
        Display_printf(display, 0, 0,
            (char *)"Failed to reset limits to factory default values");
    }

    /* Close the I2C connection */
    I2C_close(i2c);
    Display_printf(display, 0, 0, (char *)"\nI2C closed!");

    return (NULL);
}
