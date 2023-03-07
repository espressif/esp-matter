/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
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
 *  ======== opt3001.c ========
 */

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Module Header */
#include <ti/common/sail/opt3001/opt3001.h>

#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define LUX_BASE_RES            00.01F
#define RANGE_BASE              00040.95F
#define RANGE_MAX               83865.60F
#define RANGE_BITS              0xF000
#define READ_BUF_SIZE           2
#define WRITE_BUF_SIZE          3
#define CONVERSION_MODE         0xC000

/* Default OPT3001 parameters structure */
const OPT3001_Params OPT3001_defaultParams = {
        OPT3001_CONTINUOUS,    /* Continuous conversions                     */
        OPT3001_CONVRD_DIS,    /* Disable Conversion Ready Interrupts        */
        OPT3001_800MS,         /* 0.8 s conversion time                      */
        OPT3001_FAULT1,        /* One fault per interrupt                    */
        OPT3001_AUTO,          /* Automatic Lux Range                        */
        OPT3001_LATCH,         /* Latch Mode                                 */
        NULL                   /* Callback pointer                           */
};

extern OPT3001_Config OPT3001_config[];

/*
 *  ======== convertToLux ========
 *  Converts register into lux
 */
static float convertToLux(uint16_t reg)
{
    float lsbSize;

    /* The register consist of data bits and exponent bits               */
    /* Shift out data bits from reg and raise two to the resulting power */
    lsbSize = (LUX_BASE_RES * (2 << (reg >> 12))) / 2;
    /* Mask out exponent bits */
    reg = reg & 0x0FFF;

    /* Return lux */
    return (lsbSize * reg);
}

/*
 *  ======== i2cTransferFxn ========
 *  Executes an I2C transfer.
 */
static bool i2cTransferFxn(I2C_Handle handle, I2C_Transaction transaction)
{
    if (I2C_transfer(handle, &transaction)) {
        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== OPT3001_close ========
 *  Closes an instance of a OPT3001 sensor.
 */
bool OPT3001_close(OPT3001_Handle handle)
{
    OPT3001_Object *obj = (OPT3001_Object *)(handle->object);

    if (obj->callback != NULL) {
       OPT3001_disableInterrupt(handle);
    }

    if (OPT3001_setConversionMode(handle, OPT3001_SHUTDOWN))
    {
        if (OPT3001_setLuxLimits(handle, RANGE_MAX, 0)) {
            obj->i2cHandle = NULL;

            return (true);
        }
    }
    return (false);
}

/*
 *  ======== OPT3001_disableInterrupt ========
 *  Disable incoming interrupts
 */
bool OPT3001_disableInterrupt(OPT3001_Handle handle)
{

    if (((OPT3001_Object *) (handle->object))->callback != NULL) {
        GPIO_disableInt(((OPT3001_HWAttrs *) (handle->hwAttrs))->gpioIndex);

        return (true);
    }

    return (false);
}

/*
 *  ======== OPT3001_enableInterrupt ========
 *  Enable incoming interrupts
 */
bool OPT3001_enableInterrupt(OPT3001_Handle handle)
{
    uint16_t reg = 0; /* Prevent warning */

    if (((OPT3001_Object *) (handle->object))->callback != NULL) {

        /* Reset INT pin */
        if (OPT3001_readRegister(handle, &reg, OPT3001_CONFIG)) {
            GPIO_enableInt(((OPT3001_HWAttrs *) (handle->hwAttrs))->gpioIndex);

            return (true);
        }
    }

    return (false);
}

/*
 *  ======== OPT3001_getLux ========
 *  Get current lux reading
 */
bool OPT3001_getLux(OPT3001_Handle handle, float *data)
{
    uint16_t reg;

    if (OPT3001_readRegister(handle, &reg, OPT3001_RESULT)) {
        *data = convertToLux(reg);

        return (true);
    }

    return (false);
}

/*
 *  ======== OPT3001_getLuxLimits ========
 */
bool OPT3001_getLuxLimits(OPT3001_Handle handle, float *high, float *low)
{
    uint16_t reg;

    if (OPT3001_readRegister(handle, &reg, OPT3001_HILIMIT)) {
        *high = convertToLux(reg);

        if (OPT3001_readRegister(handle, &reg, OPT3001_LOLIMIT)) {
            *low = convertToLux(reg);

            return (true);
        }
    }

return (false);
}

/*
 *  ======== OPT3001_init ========
 */
void OPT3001_init()
{
    static bool initFlag = false;
    unsigned char i;

    if (initFlag == false) {
        for (i = 0; OPT3001_config[i].object != NULL; i++) {
            ((OPT3001_Object *)(OPT3001_config[i].object))->i2cHandle = NULL;
        }
        initFlag = true;
    }
}

/*
 *  ======== OPT3001_open ========
 *  Setups OPT3001 sensor and returns OPT3001_Handle
 */
OPT3001_Handle OPT3001_open(unsigned int index,
        I2C_Handle i2cHandle, OPT3001_Params *params)
{
    OPT3001_Handle handle = &OPT3001_config[index];
    OPT3001_Object *obj = (OPT3001_Object*)(OPT3001_config[index].object);
    OPT3001_HWAttrs *hw = (OPT3001_HWAttrs*)(OPT3001_config[index].hwAttrs);
    uint16_t data;

    if (obj->i2cHandle != NULL) {
        return (NULL);
    }

    obj->i2cHandle = i2cHandle;

    if (params == NULL) {
        params = (OPT3001_Params *) &OPT3001_defaultParams;
    }

    data = (uint16_t)params->conversionMode | (uint16_t)params->conversionTime | (uint16_t)params->faultCount
            | (uint16_t)params->interruptMode | params->range;

    if (OPT3001_writeRegister(handle, data, OPT3001_CONFIG)) {

        if (params->callback != NULL) {
            obj->callback = params->callback;

            if (params->conversionReady) {

                if (!OPT3001_writeRegister(handle, params->conversionReady,
                        OPT3001_LOLIMIT)) {
                    obj->i2cHandle = NULL;

                    return (NULL);
                }
            }
            GPIO_setCallback(hw->gpioIndex, obj->callback);
        }

        return (handle);

    }

    obj->i2cHandle = NULL;

    return (NULL);
}

/*
 *  ======== OPT3001_Params_init ========
 *  Initialize a OPT3001_Params struct to default settings.
 */
void OPT3001_Params_init(OPT3001_Params *params)
{
    *params = OPT3001_defaultParams;
}

/*
 *  ======== OPT3001_readRegister ========
 *  Reads a specified register from a OPT3001 sensor.
 */
bool OPT3001_readRegister(OPT3001_Handle handle, uint16_t *data,
        uint8_t registerAddress)
{
    I2C_Transaction i2cTransaction;
    unsigned char writeBuffer = registerAddress;
    unsigned char readBuffer[READ_BUF_SIZE];

    i2cTransaction.writeBuf = &writeBuffer;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = readBuffer;
    i2cTransaction.readCount = READ_BUF_SIZE;
    i2cTransaction.slaveAddress =
            ((OPT3001_HWAttrs *) (handle->hwAttrs))->slaveAddress;

    if (!i2cTransferFxn(((OPT3001_Object *) (handle->object))->i2cHandle,
            i2cTransaction)) {
        return (false);
    }

    *data = (readBuffer[0] << 8) | readBuffer[1];

    return (true);
}

/*
 *  ======== OPT3001_setLuxLimits ========
 */
bool OPT3001_setLuxLimits(OPT3001_Handle handle, float high, float low)
{
    uint16_t limit;     /* Bits[11:00] */
    uint16_t exp;       /* Bits[15:12] */

    if (high != OPT3001_IGNORE) {

        if (high > RANGE_MAX) {
            high = RANGE_MAX;
        }
        /* Determine exp and limit range */
        for (exp = 0; exp <= (OPT3001_RANGE11 >> 12); exp++) {

            if (high < (RANGE_BASE * (2 << exp)) / 2) {
                break;
            }
        }

        /* Truncate and convert for Limit Register */
        limit = (int) (high / ((LUX_BASE_RES * (2 << exp)) / 2));
        exp = exp << 12;

        if (!OPT3001_writeRegister(handle, (limit | exp), OPT3001_HILIMIT)) {
            return (false);
        }
    }

    if (low != OPT3001_IGNORE) {

        if (low < RANGE_BASE) {
            low = RANGE_BASE;
        }
        /* Determine exp and limit range */
        for (exp = 0; exp <= (OPT3001_RANGE11 >> 12); exp++) {

            if (low < (RANGE_BASE * (2 << exp)) / 2) {
                break;
            }
        }

        /* Truncate and convert for Limit Register */
        limit = (int) (low / ((LUX_BASE_RES * (2 << exp)) / 2));
        exp = exp << 12;

        if (!OPT3001_writeRegister(handle, (limit | exp), OPT3001_LOLIMIT)) {
            return (false);
        }
    }

    return (true);
}

/*
 *  ======== OPT3001_setRange ========
 */
bool OPT3001_setRange(OPT3001_Handle handle, OPT3001_FullRange range)
{
    uint16_t reg;

    /* Read Configuration Register */
    if (OPT3001_readRegister(handle, &reg, OPT3001_CONFIG)) {

        /* Clear current range bits */
        reg &= ~RANGE_BITS;
        /* Write new range bits */
        reg |= range;

        if (OPT3001_writeRegister(handle, reg, OPT3001_CONFIG)) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== OPT3001_setConversionMode ========
 *  Set conversion mode.
 */
bool OPT3001_setConversionMode(OPT3001_Handle handle,
        OPT3001_ConversionMode mode)
{
    uint16_t reg;

    /* Read Configuration Register */
    if (OPT3001_readRegister(handle, &reg, OPT3001_CONFIG)) {

        /* Clear conversion mode bits */
        reg &= ~OPT3001_SINGLESHOT;
        /* Write new conversion bits */
        reg |= mode;

        if (OPT3001_writeRegister(handle, reg, OPT3001_CONFIG)) {
            return (true);
        }
    }

    return (false);
}

/*
 *  ======== OPT3001_writeRegister ========
 *  Writes data to the specified register and OPT3001 sensor.
 */
bool OPT3001_writeRegister(OPT3001_Handle handle, uint16_t data,
        uint8_t registerAddress)
{
    I2C_Transaction i2cTransaction;
    uint8_t writeBuffer[WRITE_BUF_SIZE] = {registerAddress, (data >> 8), data};

    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.writeCount = WRITE_BUF_SIZE;
    i2cTransaction.readCount = 0;
    i2cTransaction.slaveAddress = ((OPT3001_HWAttrs *)(handle->hwAttrs))
            ->slaveAddress;

    /* If transaction success */
    if (!i2cTransferFxn(((OPT3001_Object *)(handle->object))
            ->i2cHandle, i2cTransaction)) {
        return (false);
    }

    return (true);
}
