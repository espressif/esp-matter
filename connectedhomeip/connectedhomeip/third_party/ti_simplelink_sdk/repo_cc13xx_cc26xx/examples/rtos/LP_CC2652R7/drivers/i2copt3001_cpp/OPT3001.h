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

/*!****************************************************************************
 *  @file    OPT3001.h
 *  @brief   Interface for reading and writing data from the OPT3001 sensor
 *
 *  # Overview
 *
 *  OPT3001.h and OPT3001.c gives the i2copt3001_cpp example access to
 *  functions which can read & write data to a OPT3001 digital ambient light
 *  sensor. This file contains very specific definitions and functions to
 *  communicate with the OPT3001.
 *
 *  This example only works when the I2C driver is used in BLOCKING mode.
 *
 *  To gain a better of understanding of these definitions and functions, see
 *  the OPT3001 data sheet at http://www.ti.com/lit/ds/symlink/opt3001.pdf
 *
 ******************************************************************************
 */

#ifndef OPT3001_H_
#define OPT3001_H_

#include <stdint.h>
#include <stdbool.h>

#include <ti/display/Display.h>
#include <ti/drivers/I2C.h>

class OPT3001
{
public:
    /*!
     * @brief Possible I2C slave addresses for the OPT3001
     */
    enum SlaveAddress
    {
        /*!
         *  Slave Address when the ADDR pin is connected to the GND pin
         *  Hex: 0x44
         */
        ADDRPIN_GND = 0b1000100,

        /*!
         *  Slave Address when the ADDR pin is connected to the VDD pin
         *  Hex: 0x45
         */
        ADDRPIN_VDD = 0b1000101,

        /*!
         *  Slave Address when the ADDR pin is connected to the SDA pin
         *  Hex: 0x46
         */
        ADDRPIN_SDA = 0b1000110,

        /*!
         *  Slave Address when the ADDR pin is connected to the SCL pin
         *  Hex: 0x47
         */
        ADDRPIN_SCL = 0b1000111
    };

    /*!
     * @brief Conversion Modes
     *
     * The values stored in this enums are associated
     * with bits 10:9 in the configuration register.
     */
    enum ConversionMode
    {
        /*! Default mode */
        SHUTDOWN = 0x0,
        /*! Reads one value before returning to shutdown mode */
        SINGLESHOT = 0x1,
        /*! Reads values to the result register continuously */
        CONTINUOUS_CONVERSIONS = 0x2
    };

    /*!
     * @brief Interrupt Flag Values
     *
     * Used by getFlag() to check if an interrupt was low or high.
     */
    enum InterruptFlag
    {
        /*! If InterruptFlag is NONE, then the value was within the limits */
        NONE = -1,
        /*! If InterruptFlag is LOW, then the value was below the low limit */
        LOW = 0,
        /*! If InterruptFlag is HIGH, then the value was above the high limit */
        HIGH = 1
    };

    /*!
     * @brief The address of all accessible registers
     *        associated with the OPT3001
     */
    enum Register
    {
        /*!
         *  RESULT stores result of the most recent light to digital conversion
         *  conversion mode cannot be in "Shutdown (default)" to get data)
         */
        RESULT = 0x00,

        /*!
         *  Controls the major operational modes of the device and the three modes
         *  of conversions: Shutdown, Single-shot, and Continuous conversions
         */
        CONFIGURATION = 0x01,

        /*! The lower comparison limit for the interrupt reporting mechanism */
        LOW_LIMIT = 0x02,

        /*! The upper comparison limit for the interrupt reporting mechanisms */
        HIGH_LIMIT = 0x03,

        /*! Intended to uniquely identify the device (hex: 5549h, ASCII: TI) */
        MANUFACTURER_ID = 0x7E,

        /*! Intended to uniquely identify the device (its value hex: 3001h) */
        DEVICE_ID = 0x7F
    };

    /*!
     *  @brief    OPT3001 Result Register Range
     *
     *  The range field selects the full-scale lux range of the device.
     *  The result register may contain a value up to that specified
     *  by the range. Greater range constitutes lower resolution while
     *  less range constitutes higher resolution. Range is set as the
     *  first 4 bits of the configuration register
     */
    enum FullRange
    {
        RANGE0 = 0x0000,  /*!< Range = 00040.95, lux per LSB = 00.01  */
        RANGE1 = 0x1000,  /*!< Range = 00081.90, lux per LSB = 00.02  */
        RANGE2 = 0x2000,  /*!< Range = 00163.80, lux per LSB = 00.04  */
        RANGE3 = 0x3000,  /*!< Range = 00327.60, lux per LSB = 00.08  */
        RANGE4 = 0x4000,  /*!< Range = 00665.20, lux per LSB = 00.16  */
        RANGE5 = 0x5000,  /*!< Range = 01310.40, lux per LSB = 00.32  */
        RANGE6 = 0x6000,  /*!< Range = 02620.80, lux per LSB = 00.64  */
        RANGE7 = 0x7000,  /*!< Range = 05241.60, lux per LSB = 01.28  */
        RANGE8 = 0x8000,  /*!< Range = 10483.20, lux per LSB = 02.56  */
        RANGE9 = 0x9000,  /*!< Range = 20966.40, lux per LSB = 05.12  */
        RANGE10 = 0xA000, /*!< Range = 41932.80, lux per LSB = 10.24  */
        RANGE11 = 0xB000, /*!< Range = 83865.60, lux per LSB = 20.48  */
        AUTO = 0xC000 /*!< Automatic Scaling */
    };

    /* Constructor */
    OPT3001();

    /* Deconstructor */
    ~OPT3001();

    /*!
     *  @brief  Returns the value stored in the configuration register
     *
     *  @return  A positive value storing 16 bits of information
     *  of the configuration register on success, or @p -1 on an error.
     */
    int32_t getConfiguration();

    /*!
     *  @brief  Returns the conversion mode stored in the
     *          configuration register.
     *
     *  @return  a conversion mode in the ConversionMode enum
     */
    ConversionMode getConversionMode();

    /*!
     *  @brief  Returns the value in the device ID register
     *          (default value: 0x3001)
     *
     *  @return  A positive device ID on success, or @p -1 on an error.
     */
    int16_t getDeviceID();

    /*!
     *  @brief  Returns the interrupt flag type stored in the
     *          configuration register.
     *
     *  @return  a @p LOW value if the value is below the value in the
     *           low limit register. Returns @p HIGH if the value is below
     *           the value in the high limit register. Returns @P NONE if the
     *           value is within the limit registers
     */
    InterruptFlag getFlag();

    /*!
     *  @brief  Returns the value in lux in the high limit register
     *
     *  @return  A positive lux value on success, or @p -1 on an error.
     */
    int32_t getHighLimit();

    /*!
     *  @brief  Returns the value in lux in the low limit register
     *
     *  @return  A positive lux value on success, or @p -1 on an error.
     */
    int32_t getLowLimit();

    /*!
     *  @brief  Returns the value in the manufacturer ID register
     *          (default value: 0x5449)
     *
     *  @return  A positive manufacturer ID on success, or @p -1 on an error.
     */
    int16_t getManufacturerID();

    /*!
     *  @brief  Get the result in lux from the result register
     *
     *  @pre    The OPT3001 must not be in Shutdown Mode to receive an accurate
     *          reading from the sensor.
     *
     *  @return  A positive lux result on success, or @p -1 on an error.
     */
    int32_t getResult();

    /*!
     *  @brief  Initialize the OPT3001 class with an I2C instance
     *
     *  @pre    I2C_init() has been called.
     *
     *  @param  i2cHandle       An #I2C_Handle returned from I2C_open()
     *
     *  @param  i2cAddress      One of the four slave addresses
     *                          in the SlaveAddress enum
     *
     *  @param  displayHandle   Optional display handle used to
     *                          print debug information.
     *
     *  @return @p true for a successful initialization; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool init(I2C_Handle i2cHandle, SlaveAddress i2cAddress,
        Display_Handle displayHandle);

    /*!
     *  @brief  Converts a lux value into a 16-bit number
     *          made up of a 4 bit exponent and 12 bit mantissa.
     *          (the data type in the low/high limit registers)
     *
     *  @note   This is called by setHighLimit() and setLowLimit()
     *          to allow users to pass in a lux value to the limit registers
     *
     *  @param  lux   A positive lux value
     *
     *  @return a 16-bit number made up of a 4 bit exponent and a 12 bit mantissa
     */
    uint16_t parseLux(uint32_t lux);

    /*!
     *  @brief  Resets the configuration register to its factory default value
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool resetConfiguration();

    /*!
     *  @brief  Resets the high limit register to its factory default value
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool resetHighLimit();

    /*!
     *  @brief  Resets the low limit register to its factory default value
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool resetLowLimit();

    /*!
     *  @brief  Sets the value stored in the configuration register to 'data'
     *
     *  @param[in]  data    a 16 bit integer
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool setConfiguration(uint16_t data);

    /*!
     *  @brief  Sets the conversion mode stored in the configuration register.
     *
     *  @param[in]  cm     a conversion mode from the ConversionMode enum
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool setConversionMode(ConversionMode cm);

    /*!
     *  @brief  Set the high limit of the OPT3001 device
     *
     *  @param[in]  limit     a high limit in lux
     *
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool setHighLimit(uint32_t limit);

    /*!
     *  @brief  Set the low limit of the OPT3001 device
     *
     *  @param[in]  limit     a low limit in lux
     *
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool setLowLimit(uint32_t limit);

private:
    I2C_Handle i2cHandle;
    I2C_Transaction i2cTransaction;
    SlaveAddress i2cAddress;
    Display_Handle displayHandle;

    uint8_t txBuffer[3];
    uint8_t txCount;
    uint8_t rxBuffer[2];
    uint8_t rxCount;

    /*!
     *  @brief  Calculates a lux value given a 16bit number that is returned
     *          from the result, low limit, and high limit registers.
     *
     *  @note   Usually called by getResult()
     *
     *  @param[in]  reg_result  a value from the result, low limit, or high
     *                          limit registers.
     *
     *  @return @p a lux value on success; @p -1
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    int32_t calculateLux(int32_t reg_result);

    /*!
     *  @brief  Retrieves a value stored in a given register on the OPT3001
     *
     *  @param[in]  reg  A register from the Register enum
     *
     *  @return @p a 16-bit value on success; @p -1
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    int32_t getRawRegisterValue(Register reg);


    /*!
     *  @brief  Prints out debug information about the I2C Transaction
     *
     *  @param[in]  transaction  A pointer to a I2C transaction structure
     *
     *  @param[in]  display  A pointer to a Display handle
     */
    void i2cErrorHandler(I2C_Transaction *transaction,
        Display_Handle display);

    /*!
     *  @brief  Puts 'data' into the register 'reg' on the OPT3001
     *
     *  @param[in]  reg     A register from the Register enum
     *  @param[in]  data    A 16-bit value to store in reg
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool setRawRegisterValue(Register reg, uint16_t data);

    /*!
     *  @brief  Calls an i2c transfer with txBuffer and rxBuffer
     *          and the txCount and rxCount variables
     *
     *  @return @p true for a successful transfer; @p false
     *          for an error (for example, an I2C bus fault (NACK)).
     */
    bool transfer();
};

#endif /* OPT3001_H_ */
