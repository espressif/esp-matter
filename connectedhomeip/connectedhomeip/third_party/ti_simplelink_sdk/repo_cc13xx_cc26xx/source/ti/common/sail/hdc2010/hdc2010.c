#include <stdint.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>

/* Module Header */
#include <ti/common/sail/hdc2010/hdc2010.h>

/* POSIX Header files */
#include <unistd.h>

/* It is recommended to wait for some time(in few ms, defined by below macro)
before HDC2010 module is used after issuing a reset*/
#define WAIT_POST_RESET      3U        /* Wait time in ms after reset        */

#define MAX_CELSIUS          124.36F    /* Max degrees celsius                */
#define MIN_CELSIUS          -40        /* Min degrees celsius                */
#define CELSIUS_FAHREN_CONST 1.8F       /* Multiplier for conversion          */
#define CELSIUS_FAHREN_ADD   32U         /* Celsius to Fahrenheit added factor */
#define CELSIUS_TO_KELVIN    273.15F    /* Celsius to Kelvin addition factor  */
#define CELSIUS_PER_LSB      0.0025177F /* Degrees celsius per LSB            */
#define RH_PER_LSB           0.0015259F /* Relative Humidity celsius per LSB  */
#define MAX_CELSIUS_PER_LSB  1.5515152F
#define MAX_RH_PER_LSB       2.56F
#define READ_BUF_SIZE        2U
#define WRITE_BUF_SIZE       2U

/* HDC2010 Configuration Register Bits */
#define CMD_SW_RST    0x80U              /* Software reset                      */

/* Default HDC2010 parameters structure */
const HDC2010_Params HDC2010_defaultParams = {
            HDC2010_HT_MODE,                     /*!< Continuous Conversion Mode     */
            HDC2010_T14_BITS,                    /*!< Conversion Cycle Time 15.5ms   */
            HDC2010_H14_BITS,                    /*!< 0 Samples per conversion       */
            HDC2010_TRIGG_2_HZ,                  /*!< Alert Mode                     */
            HDC2010_HI_Z_MODE,                   /*!< Alert Pin Mode                 */
            HDC2010_NO_MASK,                     /*!< Active Low on Alert            */
            HDC2010_LEVEL_MODE,
            HDC2010_ACTIVE_HI,
            NULL,                                /*!< Pointer to GPIO callback       */
};

extern HDC2010_Config HDC2010_config[];

/*
 *  ======== i2cTransferFxn ========
 *  Executes an I2C transfer.
 */

bool HDC2010_readRegister(HDC2010_Handle handle, uint16_t *data,
                          uint8_t registerAddress)
{
    I2C_Transaction i2cTransaction;
    unsigned char writeBuffer = registerAddress;
    unsigned char readBuffer[READ_BUF_SIZE];

    i2cTransaction.writeBuf = &writeBuffer;
    i2cTransaction.writeCount = 1U;
    i2cTransaction.readBuf = readBuffer;
    i2cTransaction.readCount = READ_BUF_SIZE;
    i2cTransaction.slaveAddress = ((HDC2010_HWAttrs *)(handle->hwAttrs))->slaveAddress;

    if (!I2C_transfer(((HDC2010_Object *)(handle->object))->i2cHandle, &i2cTransaction)) {
        return (false);
    }

    *data = readBuffer[0U];

    return (true);
}

bool HDC2010_writeRegister(HDC2010_Handle handle, uint16_t data,
                           uint8_t registerAddress)
{
    I2C_Transaction i2cTransaction;
    uint8_t writeBuffer[WRITE_BUF_SIZE] = {registerAddress, data};

    i2cTransaction.writeBuf = writeBuffer;
    i2cTransaction.writeCount = WRITE_BUF_SIZE;
    i2cTransaction.readCount = 0U;
    i2cTransaction.slaveAddress = ((HDC2010_HWAttrs *)(handle->hwAttrs))->slaveAddress;

    /* If transaction success */
    if (!I2C_transfer(((HDC2010_Object *)(handle->object))->i2cHandle, &i2cTransaction)) {
        return (false);
    }

    return (true);
}

void HDC2010_init()
{
    static bool initFlag = false;
    unsigned char i;

    if (initFlag == false) {
        for (i = 0U; HDC2010_config[i].object != NULL; i++) {
            ((HDC2010_Object *)(HDC2010_config[i].object))->i2cHandle = NULL;
        }
        initFlag = true;
    }
}

void HDC2010_Params_init(HDC2010_Params *params)
{
    *params = HDC2010_defaultParams;
}

HDC2010_Handle HDC2010_open(unsigned int HDC2010Index, I2C_Handle i2cHandle,
                            HDC2010_Params *params)
{
    HDC2010_Handle handle = &HDC2010_config[HDC2010Index];
    HDC2010_Object *obj = (HDC2010_Object*)(HDC2010_config[HDC2010Index].object);
    HDC2010_HWAttrs *hw = (HDC2010_HWAttrs*)(HDC2010_config[HDC2010Index].hwAttrs);
    uint16_t data;

    if (obj->i2cHandle != NULL) {
        return (NULL);
    }

    obj->i2cHandle = i2cHandle;

    if (params == NULL) {
        params = (HDC2010_Params *) &HDC2010_defaultParams;
    }

    /* Perform a Hardware HDC2010 Reset */
    if (HDC2010_writeRegister(handle, CMD_SW_RST, HDC2010_RST_DRDY_INT_CONF_REG)) {

        usleep(WAIT_POST_RESET * 1000U);

        data = (uint8_t)params->measurementMode | (uint8_t)params->tempResolution | 
                (uint8_t)params->humResolution;

        if (HDC2010_writeRegister(handle, data, HDC2010_MEAS_CONFIG_REG)) {

            data = (uint8_t)params->outputDataRate | (uint8_t)params->interruptEn | 
                    (uint8_t)params->interruptMode | (uint8_t) params->interruptPinPolarity;

            if(HDC2010_writeRegister(handle, data, HDC2010_RST_DRDY_INT_CONF_REG)){

                data = (uint8_t) ~(params->interruptMask);

                if(HDC2010_writeRegister(handle, data, HDC2010_INT_MASK_REG)){

                    //Reset Interrupt Masks
                    if(HDC2010_readRegister(handle, (uint16_t *) &data, HDC2010_DRDY_INT_STATUS_REG)){

                        if (params->callback != NULL) {
                            obj->callback = params->callback;
                            GPIO_setCallback(hw->gpioIndex, obj->callback);
                            GPIO_enableInt(hw->gpioIndex);

                        }
                    }
                }
            }
        }
        return (handle);
    }

    obj->i2cHandle = NULL;

    return (NULL);
}

bool HDC2010_close(HDC2010_Handle handle)
{
    HDC2010_Object *obj = (HDC2010_Object *)(handle->object);

    if (obj->callback != NULL) {
        HDC2010_configInterrupt(handle, HDC2010_HI_Z_MODE, HDC2010_LEVEL_MODE, HDC2010_ACTIVE_HI,
             HDC2010_NO_MASK);
    }

    if (HDC2010_configODR(handle, HDC2010_TRIGG_ON_DEMAND)){
        obj->i2cHandle = NULL;

        return (true);
    }

    return (false);
}

bool HDC2010_getTemp(HDC2010_Handle handle, HDC2010_TempScale unit, float *data)
{
    int16_t templsb, tempmsb;

    if (HDC2010_readRegister(handle, (uint16_t *) &templsb, HDC2010_TEMPERATURE_LSB_REG)) {
        if (HDC2010_readRegister(handle, (uint16_t *) &tempmsb, HDC2010_TEMPERATURE_MSB_REG)) {
            /* Shift out first 2 don't care bits, convert to Celsius */
            *data = ((float) (((tempmsb << 8U) | (templsb)) * CELSIUS_PER_LSB) - 40U);

            switch (unit) {
            case HDC2010_KELVIN:
                *data += CELSIUS_TO_KELVIN;
                break;

            case HDC2010_FAHREN:
                *data = *data * CELSIUS_FAHREN_CONST + CELSIUS_FAHREN_ADD;
                break;

            default:
                break;
            }

            return (true);
        }
    }
    return (false);
}

bool HDC2010_getHum(HDC2010_Handle handle, float *data)
{
    int16_t humlsb, hummsb;

    if (HDC2010_readRegister(handle, (uint16_t *) &humlsb, HDC2010_HUMIDITY_LSB_REG)) {
        if (HDC2010_readRegister(handle, (uint16_t *) &hummsb, HDC2010_HUMIDITY_MSB_REG)) {
            /* Shift out first 2 don't care bits, convert to Celsius */
            *data = ( (float) ( ((hummsb << 8U) | (humlsb)) * RH_PER_LSB));

            return (true);
        }
    }
    return (false);
}

bool HDC2010_getMaxTemp(HDC2010_Handle handle, HDC2010_TempScale unit, float *data)
{
    int16_t maxtemp;

    if (HDC2010_readRegister(handle, (uint16_t *) &maxtemp, HDC2010_MAX_TEMPERATURE_REG)) {

        /* Shift out first 2 don't care bits, convert to Celsius */
        *data = ((float)((maxtemp) * MAX_CELSIUS_PER_LSB) - 40U);

        switch (unit) {
        case HDC2010_KELVIN:
            *data += CELSIUS_TO_KELVIN;
            break;

        case HDC2010_FAHREN:
            *data = *data * CELSIUS_FAHREN_CONST + CELSIUS_FAHREN_ADD;
            break;

        default:
            break;
        }

        return (true);
    }

    return (false);
}

bool HDC2010_getMaxHum(HDC2010_Handle handle, float *data)
{
    int16_t maxhum;

    if (HDC2010_readRegister(handle, (uint16_t *) &maxhum, HDC2010_MAX_HUMIDITY_REG)) {
        /* Shift out first 2 don't care bits, convert to Celsius */
        *data = ((float)((maxhum) * MAX_RH_PER_LSB));

        return (true);
    }
    return (false);
}

bool HDC2010_setTempLimit(HDC2010_Handle handle, HDC2010_TempScale scale, float high, float low)
{
    int16_t hi_temp, lo_temp;

    /* Convert from scale to Celsius */
    switch (scale) {
        case HDC2010_KELVIN:
            high = high - CELSIUS_TO_KELVIN;
            low = low - CELSIUS_TO_KELVIN;
            break;

        case HDC2010_FAHREN:
            high = (high - CELSIUS_FAHREN_ADD) / CELSIUS_FAHREN_CONST;
            low = (low - CELSIUS_FAHREN_ADD) / CELSIUS_FAHREN_CONST;
            break;

        default:
            break;
    }

    if (high > MAX_CELSIUS) {
        high = MAX_CELSIUS;
    }

    if (low > MAX_CELSIUS) {
        low = MAX_CELSIUS;
    }

    if (high < MIN_CELSIUS) {
        high = MIN_CELSIUS;
    }

    if (low < MIN_CELSIUS) {
        low = MIN_CELSIUS;
    }

    hi_temp = (int16_t)((high * MAX_CELSIUS_PER_LSB) + 40U);
    lo_temp = (int16_t)((low * MAX_CELSIUS_PER_LSB) + 40U);

    /* Write Temp Lim */
    if (!(HDC2010_writeRegister(handle, hi_temp, HDC2010_TEMPERATURE_THR_H_REG)) & 
        !(HDC2010_writeRegister(handle, lo_temp, HDC2010_TEMPERATURE_THR_L_REG))) {
        return (false);
    }

    return (true);
}

bool HDC2010_setHumLimit(HDC2010_Handle handle, float high, float low)
{
    int16_t hi_hum, lo_hum;

    hi_hum = (int16_t)((high * MAX_RH_PER_LSB));
    lo_hum = (int16_t)((low * MAX_RH_PER_LSB));

    /* Write Temp Lim */
    if (!(HDC2010_writeRegister(handle, hi_hum, HDC2010_HUMIDITY_THR_H_REG)) & 
        !(HDC2010_writeRegister(handle, lo_hum, HDC2010_HUMIDITY_THR_L_REG))) {
        return (false);
    }

    return (true);
}

bool HDC2010_configMeasurementMode(HDC2010_Handle handle, HDC2010_MeasurementMode measurementMode){

    uint16_t reg;

    if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_MEASURE_CONF__REG)) {

        reg = HDC2010_SET_BITSLICE(reg, HDC2010_MEASURE_CONF, measurementMode);

        if (HDC2010_writeRegister(handle,reg, HDC2010_MEASURE_CONF__REG)) {

            return (true);

        }
    }

    return (false);
}

bool HDC2010_configTempResolution(HDC2010_Handle handle, HDC2010_TemperatureResolution tempResolution){

    uint16_t reg;

    if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_TEMPERATURE_RES__REG)) {

        reg = HDC2010_SET_BITSLICE(reg, HDC2010_TEMPERATURE_RES, tempResolution);

        if (HDC2010_writeRegister(handle,reg, HDC2010_TEMPERATURE_RES__REG)) {

            return (true);

        }
    }

    return (false);

}

bool HDC2010_configHumResolution(HDC2010_Handle handle, HDC2010_HumidityResolution humResolution){

    uint16_t reg;

    if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_HUMIDITY_RES__REG)) {

        reg = HDC2010_SET_BITSLICE(reg, HDC2010_HUMIDITY_RES, humResolution);

        if (HDC2010_writeRegister(handle,reg, HDC2010_HUMIDITY_RES__REG)) {

            return (true);

        }
    }

    return (false);
}

bool HDC2010_configODR(HDC2010_Handle handle, HDC2010_OutputDataRate outputDataRate){

    uint16_t reg;

    if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_DATA_RATE__REG)) {

        reg = HDC2010_SET_BITSLICE(reg, HDC2010_DATA_RATE, outputDataRate);

        if (HDC2010_writeRegister(handle,reg, HDC2010_DATA_RATE__REG)) {

            return (true);

        }
    }

    return (false);

}

bool HDC2010_configInterrupt(HDC2010_Handle handle, HDC2010_InterruptEn interruptEn, HDC2010_InterruptMode interruptMode,
                             HDC2010_InterruptPinPolarity interruptPinPolarity, HDC2010_InterruptMask interruptMask){

    uint16_t reg;

    //Read Interrupt Configurations
    if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_RST_DRDY_INT_CONF_REG)) {

        reg = ((reg & ~(HDC2010_INT_MODE__MSK|HDC2010_INT_POL__MSK|HDC2010_INT_EN__MSK)) | 
                ((((uint16_t)interruptEn |(uint16_t)interruptMode |(uint16_t)interruptPinPolarity)<<HDC2010_INT__POS) & 
                (HDC2010_INT_MODE__MSK|HDC2010_INT_POL__MSK|HDC2010_INT_EN__MSK)));

        //Write new Interrupt Configurations
        if (HDC2010_writeRegister(handle, reg, HDC2010_RST_DRDY_INT_CONF_REG)) {

            //Read Interrupt Masks
            if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_INT_MASK_REG)) {

                reg = ((reg & ~(HDC2010_DRD_MASK__MSK | HDC2010_TH_MASK__MSK | HDC2010_TL_MASK__MSK | 
                    HDC2010_HH_MASK__MSK | HDC2010_HL_MASK__MSK)) | ((~(interruptMask)<<HDC2010_INT_MASK__POS) & 
                    (HDC2010_DRD_MASK__MSK | HDC2010_TH_MASK__MSK | HDC2010_TL_MASK__MSK | HDC2010_HH_MASK__MSK | 
                    HDC2010_HL_MASK__MSK)));

                //Write new Interrupt Masks
                if (HDC2010_writeRegister(handle,reg, HDC2010_INT_MASK_REG)) {

                    //Reset Interrupt Masks Satus
                    if(HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_DRDY_INT_STATUS_REG)){

                        return (true);
                    }
                }
            }
        }
    }

    return (false);
}

bool HDC2010_triggerMeasurement(HDC2010_Handle handle){
    uint16_t reg;

    if (HDC2010_readRegister(handle, (uint16_t *) &reg, HDC2010_MEASURE_TRIG__REG)) {

        reg = HDC2010_SET_BITSLICE(reg, HDC2010_MEASURE_TRIG, HDC2010_START_BIT);

        if (HDC2010_writeRegister(handle, reg, HDC2010_MEASURE_TRIG__REG)) {

            return (true);

        }
    }

    return (false);
}
