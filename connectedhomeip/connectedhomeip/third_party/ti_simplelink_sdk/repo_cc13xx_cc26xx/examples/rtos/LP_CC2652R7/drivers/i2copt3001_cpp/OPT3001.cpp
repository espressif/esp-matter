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

#include "OPT3001.h"

/*
 * A mask of the 16-bit configuration register where
 * there are only 1's on the two flag bits. (bits 6:5)
 */
#define FLAG_MASK    0x60

/*
 * A mask of the 16-bit result register where
 * there are only 1's on the exponent bits.
 * The exponent bits are the 4 most significant bits.
 */
#define EXP_MASK    0xF000

/*
 * A mask of the 16-bit result register where
 * there are only 1's on the mantissa bits.
 * The mantissa bits are the 12 least significant bits
 */
#define MANTISSA_MASK    0x0FFF

/*
 * A mask of the 16-bit configuration register where
 * there are only 1's on the bits explaining the conversion mode.
 * The mantissa bits are the 12 least significant bits. (bits 10:9)
 */
#define CONVERSION_MODE_MASK    0x0600

/*
 * A mask of the 16-bit configuration register where
 * there are only 1's on the bits explaining the conversion mode.
 * The mantissa bits are the 12 least significant bits. (bits 10:9)
 */
#define FLAG_LOW_VALUE    0x1

/*
 * A mask of the 16-bit configuration register where
 * there are only 1's on the bits explaining the conversion mode.
 * The mantissa bits are the 12 least significant bits. (bits 10:9)
 */
#define FLAG_HIGH_VALUE    0x2

/* Min/max values of the limit registers as seen in datasheet */
#define MIN_LIMIT    0x0000
#define MAX_LIMIT    0xBFFF

/* Lowest LSB Size in milliLux per LSB */
#define LUX_BASE_RES    1

/* Lowest full-scale range in milliLux */
#define RANGE_BASE    4095

/* Default */
#define DEFAULT_CONFIGURATION    0xC810

/*
 *  ======== OPT3001 ========
 */
OPT3001::OPT3001()
{
}

/*
 *  ======== ~OPT3001 ========
 */
OPT3001::~OPT3001()
{
    setConversionMode(ConversionMode::SHUTDOWN);
    i2cHandle = NULL;
    displayHandle = NULL;
}

/*
 *  ======== init ========
 */
bool OPT3001::init(I2C_Handle i2cHandle, SlaveAddress i2cAddress,
    Display_Handle displayHandle)
{
    this->displayHandle = displayHandle;
    this->i2cHandle = i2cHandle;
    this->i2cAddress = i2cAddress;
    i2cTransaction.slaveAddress = this->i2cAddress;

    txCount = 0;
    rxCount = 0;

    /* if getConfiguration returns -1, then we failed to
     * communicate with the OPT3001 sensor. Return false
     * to indicate that initialization failed. */
    if(getConfiguration() == -1)
    {
        return (false);
    }
    else
    {
        return (true);
    }
}

/*
 *  ======== calculateLux ========
 * Calculates the lux value from the result register.
 * We are calculating lux = (2^exp) * mantissa / 100
 *
 * Since we are performing integer division to read the lux value,
 * there will be small rounding errors.
 *
 * Returns -1 if I2C transfer fails
 * See datasheet for more information */
int32_t OPT3001::calculateLux(int32_t reg_value)
{
    /* Return -1 if I2C transfer failed */
    if(reg_value == -1)
    {
        return (-1);
    }

    /* Calculate lux = (2^exp) * mantissa / 100 */
    uint32_t exp = ((reg_value & EXP_MASK) >> 12);
    uint32_t mantissa = (reg_value & MANTISSA_MASK);

    uint32_t twoToTheExp = (1 << exp);

    return ((twoToTheExp * mantissa) / 100);
}

/*
 *  ======== getConfiguration ========
 */
int32_t OPT3001::getConfiguration()
{
    return (getRawRegisterValue(Register::CONFIGURATION));
}

/*
 *  ======== getConversionMode ========
 */
OPT3001::ConversionMode OPT3001::getConversionMode()
{
    /* Get conversion mode from configuration register without
     * modifying other bits */
    uint16_t data = getConfiguration();
    data &= CONVERSION_MODE_MASK;
    data >>= 9;

    return ((ConversionMode) data);
}

/*
 *  ======== getDeviceID ========
 */
int16_t OPT3001::getDeviceID()
{
    return (getRawRegisterValue(Register::DEVICE_ID));
}

/*
 *  ======== getFlag ========
 */
OPT3001::InterruptFlag OPT3001::getFlag()
{
    uint16_t configVal = getConfiguration();
    /* grab the two flag bits and make them the least significant bits */
    configVal &= FLAG_MASK;
    configVal = configVal >> 5;

    switch(configVal)
    {
    case FLAG_LOW_VALUE:
        return (InterruptFlag::LOW);
    case FLAG_HIGH_VALUE:
        return (InterruptFlag::HIGH);
    default:
        return (InterruptFlag::NONE);
    }
}

/*
 *  ======== getHighLimit ========
 */
int32_t OPT3001::getHighLimit()
{
    return (calculateLux(getRawRegisterValue(Register::HIGH_LIMIT)));
}

/*
 *  ======== getLowLimit ========
 */
int32_t OPT3001::getLowLimit()
{
    return (calculateLux(getRawRegisterValue(Register::LOW_LIMIT)));
}

/*
 *  ======== getManufacturerID ========
 */
int16_t OPT3001::getManufacturerID()
{
    return (getRawRegisterValue(Register::MANUFACTURER_ID));
}

/*
 *  ======== getRawRegisterValue ========
 */
int32_t OPT3001::getRawRegisterValue(Register reg)
{
    txBuffer[0] = reg;
    txCount = 1;
    rxCount = 2;

    /* if transfer fails, return -1*/
    if (!transfer())
    {
        return (-1);
    }
    else
    {
        return ( (rxBuffer[0] << 8) | (rxBuffer[1]) );
    }
}

/*
 *  ======== getResult ========
 */
int32_t OPT3001::getResult()
{
    return (calculateLux(getRawRegisterValue(Register::RESULT)));
}

/*
 *  ======== parseLux ========
 */
uint16_t OPT3001::parseLux(uint32_t lux)
{
    /* convert lux to centi-lux to avoid floating point math */
    lux = lux * 100;

    uint16_t mantissa;  /* Bits[11:00] */
    uint16_t exp;       /* Bits[15:12] */

    for (exp = 0; exp <= (RANGE11 >> 12); exp++)
    {
        if (lux < (unsigned)(RANGE_BASE * (2 << exp)) / 2)
        {
            break;
        }
    }

    mantissa = (int) (lux / ((LUX_BASE_RES * (2 << exp)) / 2));
    exp = exp << 12;

    return (exp | mantissa);
}

/*
 *  ======== resetConfiguration ========
 */
bool OPT3001::resetConfiguration()
{
    return (setConfiguration(DEFAULT_CONFIGURATION));
}

/*
 *  ======== resetHighLimit ========
 */
bool OPT3001::resetHighLimit()
{
    return (setRawRegisterValue(Register::HIGH_LIMIT, MAX_LIMIT));
}

/*
 *  ======== resetLowLimit ========
 */
bool OPT3001::resetLowLimit()
{
    return (setRawRegisterValue(Register::LOW_LIMIT, MIN_LIMIT));
}

/*
 *  ======== setConfiguration ========
 */
bool OPT3001::setConfiguration(uint16_t data)
{
    return (setRawRegisterValue(Register::CONFIGURATION, data));
}

/*
 *  ======== setConversionMode ========
 */
bool OPT3001::setConversionMode(ConversionMode cmode)
{
    /* Write conversion mode to configuration register without
     * modifying other bits */
    uint16_t output;

    /* Clear conversion mode bits */
    output = getConfiguration() & ~CONVERSION_MODE_MASK;

    /* Write new conversion mode bits */
    output |= (cmode << 9);

    return (setConfiguration(output));
}

/*
 *  ======== setHighLimit ========
 */
bool OPT3001::setHighLimit(uint32_t lux)
{
    uint16_t limit = parseLux(lux);

    /* Verify the value is within min/max range*/
    if(limit <= MIN_LIMIT)
    {
        limit = MIN_LIMIT;
    }
    else if (limit >= MAX_LIMIT)
    {
        limit = MAX_LIMIT;
    }

    return (setRawRegisterValue(Register::HIGH_LIMIT, limit));
}

/*
 *  ======== setLowLimit ========
 */
bool OPT3001::setLowLimit(uint32_t lux)
{

    uint16_t limit = parseLux(lux);

    /* Verify the value is within min/max range*/
    if(limit <= MIN_LIMIT)
    {
        limit = MIN_LIMIT;
    }
    else if (limit >= MAX_LIMIT)
    {
        limit = MAX_LIMIT;
    }

    return (setRawRegisterValue(Register::LOW_LIMIT, limit));
}

/*
 *  ======== setRawRegisterValue ========
 */
bool OPT3001::setRawRegisterValue(Register reg, uint16_t data)
{
    txBuffer[0] = reg;
    txBuffer[1] = (uint8_t)((data & 0xFF00) >> 8);
    txBuffer[2] = (uint8_t)(data & 0x00FF);
    txCount = 3;
    rxCount = 0;

    return (transfer());
}

/*
 *  ======== transfer ========
 */
bool OPT3001::transfer()
{
    bool ret;

    /* Before performing any i2c transfer, verify we called init() first*/
    if (i2cHandle == NULL)
    {
        return (false);
    }

    i2cTransaction.writeBuf = txBuffer;
    i2cTransaction.writeCount = txCount;
    i2cTransaction.readBuf = rxBuffer;
    i2cTransaction.readCount = rxCount;

    ret = I2C_transfer(i2cHandle, &i2cTransaction);

    if (!ret && displayHandle != NULL) {
        i2cErrorHandler(&i2cTransaction, displayHandle);
    }

    return (ret);
}

/*
 *  ======== i2cErrorHandler ========
 */
void OPT3001::i2cErrorHandler(I2C_Transaction *transaction,
    Display_Handle display)
{
    switch (transaction->status) {
    case I2C_STATUS_TIMEOUT:
        Display_printf(display, 0, 0, (char *) "%s: I2C transaction timed"
            "out!", __FILE__);
        break;
    case I2C_STATUS_CLOCK_TIMEOUT:
        Display_printf(display, 0, 0, (char *) "%s: I2C serial clock line"
            "timed out!", __FILE__);
        break;
    case I2C_STATUS_ADDR_NACK:
        Display_printf(display, 0, 0, (char *) "%s: I2C slave address 0x%x not"
            " acknowledged!", __FILE__, transaction->slaveAddress);
        break;
    case I2C_STATUS_DATA_NACK:
        Display_printf(display, 0, 0, (char *) "%s: I2C data byte not"
            "acknowledged!", __FILE__);
        break;
    case I2C_STATUS_ARB_LOST:
        Display_printf(display, 0, 0, (char *) "%s: I2C arbitration to another"
            "master!", __FILE__);
        break;
    case I2C_STATUS_INCOMPLETE:
        Display_printf(display, 0, 0, (char *) "%s: I2C transaction returned"
            "before completion!", __FILE__);
        break;
    case I2C_STATUS_BUS_BUSY:
        Display_printf(display, 0, 0, (char *) "%s: I2C bus is already"
            "in use!", __FILE__);
        break;
    case I2C_STATUS_CANCEL:
        Display_printf(display, 0, 0, (char *) "%s: I2C transaction"
            "cancelled!", __FILE__);
        break;
    case I2C_STATUS_INVALID_TRANS:
        Display_printf(display, 0, 0, (char *) "%s: I2C transaction"
            "invalid!", __FILE__);
        break;
    case I2C_STATUS_ERROR:
        Display_printf(display, 0, 0, (char *) "%s: I2C generic error!",
            __FILE__);
        break;
    default:
        Display_printf(display, 0, 0, (char *)"%s: I2C undefined error case!",
            __FILE__);
        break;
    }
}
