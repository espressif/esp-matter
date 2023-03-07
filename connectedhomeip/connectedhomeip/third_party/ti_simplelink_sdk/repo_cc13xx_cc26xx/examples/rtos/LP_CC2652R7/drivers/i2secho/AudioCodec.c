/*
 * Filename: AudioCodec.c
 *
 * Description: This file implements the control interface to the
 *              TLV320AIC3254 Stereo Audio Codec
 *
 *
 * Copyright (C) 2019-2021 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/I2C.h>
#include "ti_drivers_config.h"
#include "AudioCodec.h"
#include "TI3254.h"


/*********************************************************************
 * LOCAL VARIABLES
 */

// I2C driver interface
static I2C_Handle i2cHandle;
static I2C_Params i2cParams;

/*********************************************************************
 * LOCAL VARIABLES
 */
static unsigned long AudioCodec_pageSelect(unsigned char ulPageAddress);
static unsigned long AudioCodec_regWrite(unsigned char ulRegAddr,unsigned char ucRegValue);


/*********************************************************************
 * @brief   Initializes and opens I2C interface to TLV320AIC3254, resets
 *
 * @param   none
 *
 * @return  status -    AudioCodec_STATUS_SUCCESS: if succeeded
 *                      AudioCodec_STATUS_I2C_FAIL: if I2C failed to open
 */
uint8_t AudioCodec_open()
{
    // Initialize the I2C pins
    I2C_init();
    I2C_Params_init(&i2cParams);

    // Setup I2C with fast bitrate, blocking mode
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING;

    // Open the I2C and get a handle
    i2cHandle = I2C_open(CONFIG_I2C_0, &i2cParams);

    // Ensure that I2C is able to open
    if (NULL == i2cHandle)
    {
        return AudioCodec_STATUS_I2C_FAIL;
    }

    // Reset the device
    AudioCodec_reset(AudioCodec_TI_3254, NULL);


    return AudioCodec_STATUS_SUCCESS;
}


/*********************************************************************
 * @brief   Closes I2C interface to TLV320AIC3254
 *
 * @param   none
 *
 * @return  none
 */
void AudioCodec_close()
{
    if (i2cHandle != NULL)
    {
        I2C_close(i2cHandle);
    }
    return;
}

/*********************************************************************
 * @brief   Perform a soft reset of the device
 *
 * @param   none
 *
 * @return  none
 */
int AudioCodec_reset(unsigned char codecId, void *arg)
{
    if(codecId == AudioCodec_TI_3254)
    {

        //
        // Select page 0
        //
        AudioCodec_pageSelect(TI3254_PAGE_0);

        //
        // Soft RESET
        //
        AudioCodec_regWrite(TI3254_SW_RESET_REG, 0x01);
    }

    return 0;
}

/*********************************************************************
 * @brief   Configure audio codec for smaple rate, bits and number of channels
 *
 * @param[in] codecId           - Device id
 * @param[in] bitsPerSample     - Bits per sample (8, 16, 24 etc..)
 *                                  Please ref Bits per sample Macro section
 * @param[in] bitRate           - Sampling rate in Hz. (8000, 16000, 44100 etc..)
 * @param[in] noOfChannels      - Number of channels. (Mono, stereo etc..)
 *                                  Please refer Number of Channels Macro section
 * @param[in] speaker           - Audio out that need to configure. (headphone, line out, all etc..)
 *                                  Please refer Audio Out Macro section
 * @param[in] mic               - Audio in that need to configure. (line in, mono mic, all etc..)
 *                                  Please refer Audio In Macro section
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_config(unsigned char codecId, unsigned char bitsPerSample, unsigned short bitRate,
                      unsigned char noOfChannels, unsigned char speaker,  unsigned char mic)
{

    if(codecId == AudioCodec_TI_3254)
    {
        AudioCodec_pageSelect(TI3254_PAGE_0);

        if(bitsPerSample == AudioCodec_16_BIT)
        {
            // Set I2S Mode and Word Length
            AudioCodec_regWrite(TI3254_AUDIO_IF_1_REG, 0x00);    // 0x00     16bit, I2S, BCLK is input to the device
                                                                // WCLK is input to the device,
        }
        else
        {
            return -1;
        }

        /* See slaa408a for information on how to configure
         * For CC3200 Audio BoosterPack MCLK = BCLK
         */
        switch(bitRate) {

            case 8000:  // Fs = 8kHz,  BCLK = input to PLL, BCLK = 256kHz
            case 16000: // Fs = 16kHz, BCLK = input to PLL, BCLK = 512kHz
            case 32000: // Fs = 32kHz, BCLK = input to PLL, BCLK = 1024kHz
                AudioCodec_pageSelect(TI3254_PAGE_0);

                AudioCodec_regWrite(TI3254_CLK_MUX_REG, 0x13);       // PLL Clock is CODEC_CLKIN
                AudioCodec_regWrite(TI3254_CLK_PLL_P_R_REG, 0x80 | 0x10 | 0x04);    // PLL is powered up, P=1, R=4
                AudioCodec_regWrite(TI3254_CLK_PLL_J_REG, 0x30);     // J=48
                AudioCodec_regWrite(TI3254_CLK_PLL_D_MSB_REG, 0x00); // D = 0000
                AudioCodec_regWrite(TI3254_CLK_PLL_D_LSB_REG, 0x00); // D = 0000

                AudioCodec_regWrite(TI3254_CLK_NDAC_REG, 0x80 | 0x04);      // NDAC divider powered up, NDAC = 4
                AudioCodec_regWrite(TI3254_CLK_MDAC_REG, 0x80 | 0x04);      // MDAC divider powered up, MDAC = 4
                AudioCodec_regWrite(TI3254_DAC_OSR_MSB_REG, 0x01);   // DOSR = 0x0180 = 384
                AudioCodec_regWrite(TI3254_DAC_OSR_LSB_REG, 0x80);   // DOSR = 0x0180 = 384

                AudioCodec_regWrite(TI3254_CLK_NADC_REG, 0x80 | 0x0C);      // NADC divider powered up, NADC = 12
                AudioCodec_regWrite(TI3254_CLK_MADC_REG, 0x80 | 0x04);      // MADC divider powered up, MADC = 4
                AudioCodec_regWrite(TI3254_ADC_OSR_REG, 0x80);       // AOSR = 128 ((Use with PRB_R1 to PRB_R6, ADC Filter Type A)
                break;

            case 44100: // Fs = 44.1kHz, BCLK = input to PLL, BCLK = 1'411.2kHz
                AudioCodec_pageSelect(TI3254_PAGE_0);

                AudioCodec_regWrite(TI3254_CLK_MUX_REG, 0x13);       // PLL Clock is CODEC_CLKIN
                AudioCodec_regWrite(TI3254_CLK_PLL_P_R_REG, 0x80 | 0x10 | 0x03);    // PLL is powered up, P=1, R=3
                AudioCodec_regWrite(TI3254_CLK_PLL_J_REG, 0x14);     // J=20
                AudioCodec_regWrite(TI3254_CLK_PLL_D_MSB_REG, 0x00); // D = 0000
                AudioCodec_regWrite(TI3254_CLK_PLL_D_LSB_REG, 0x00); // D = 0000

                AudioCodec_regWrite(TI3254_CLK_NDAC_REG, 0x80 | 0x05);      // NDAC divider powered up, NDAC = 5
                AudioCodec_regWrite(TI3254_CLK_MDAC_REG, 0x80 | 0x03);      // MDAC divider powered up, MDAC = 3
                AudioCodec_regWrite(TI3254_DAC_OSR_MSB_REG, 0x00);   // DOSR = 0x0080 = 128
                AudioCodec_regWrite(TI3254_DAC_OSR_LSB_REG, 0x80);   // DOSR = 0x0080 = 128

                AudioCodec_regWrite(TI3254_CLK_NADC_REG, 0x80 | 0x05);      // NADC divider powered up, NADC = 5
                AudioCodec_regWrite(TI3254_CLK_MADC_REG, 0x80 | 0x03);      // MADC divider powered up, MADC = 3
                AudioCodec_regWrite(TI3254_ADC_OSR_REG, 0x80);       // AOSR = 128 ((Use with PRB_R1 to PRB_R6, ADC Filter Type A)

                break;

            default:
                return -1;
        }


        // Configure Power Supplies
        AudioCodec_pageSelect(TI3254_PAGE_1);        //Select Page 1

        AudioCodec_regWrite(TI3254_PWR_CTRL_REG, 0x08);  // Disabled weak connection of AVDD with DVDD
        AudioCodec_regWrite(TI3254_LDO_CTRL_REG, 0x01);  // Over Current detected for AVDD LDO
        AudioCodec_regWrite(TI3254_ANALOG_IP_QCHRG_CTRL_REG, 0x32); // Analog inputs power up time is 6.4 ms
        AudioCodec_regWrite(TI3254_REF_PWR_UP_CTRL_REG, 0x01);   // Reference will power up in 40ms when analog blocks are powered up


        if(speaker)
        {
            unsigned char   reg1;

            AudioCodec_pageSelect(TI3254_PAGE_0);    //Select Page 0


            // ##Configure Processing Blocks
            AudioCodec_regWrite(TI3254_DAC_SIG_P_BLK_CTRL_REG, 0x2);  // DAC Signal Processing Block PRB_P2


            AudioCodec_pageSelect(TI3254_PAGE_44);   // Select Page 44

            AudioCodec_regWrite(TI3254_DAC_ADP_FILTER_CTRL_REG, 0x04);   // Adaptive Filtering enabled for DAC


            AudioCodec_pageSelect(TI3254_PAGE_1);    // Select Page 1

            reg1 = 0x00;

            if(speaker & AudioCodec_SPEAKER_HP)
            {
                //De-pop: 5 time constants, 6k resistance
                AudioCodec_regWrite(TI3254_HP_DRV_START_UP_CTRL_REG, 0x25);  // Headphone ramps power up time is determined with 6k resistance,
                                                                            // Headphone ramps power up slowly in 5.0 time constants

                //Route LDAC/RDAC to HPL/HPR
                AudioCodec_regWrite(TI3254_HPL_ROUTING_SEL_REG, 0x08);   // Left Channel DAC reconstruction filter's positive terminal is routed to HPL
                AudioCodec_regWrite(TI3254_HPR_ROUTING_SEL_REG, 0x08);   // Left Channel DAC reconstruction filter's negative terminal is routed to HPR

                reg1 |= 0x30;   // HPL and HPR is powered up
            }

            if(speaker & AudioCodec_SPEAKER_LO)
            {
                //Route LDAC/RDAC to LOL/LOR
                AudioCodec_regWrite(TI3254_LOL_ROUTING_SEL_REG, 0x08);   // Left Channel DAC reconstruction filter output is routed to LOL
                AudioCodec_regWrite(TI3254_LOR_ROUTING_SEL_REG, 0x08);   // Right Channel DAC reconstruction filter output is routed to LOR

                reg1 |= 0x0C;   // LOL and LOR is powered up
            }

            //Power up HPL/HPR and LOL/LOR drivers
            AudioCodec_regWrite(TI3254_OP_DRV_PWR_CTRL_REG, reg1);

            if(speaker & AudioCodec_SPEAKER_HP)
            {
                //Unmute HPL/HPR driver, 0dB Gain
                AudioCodec_regWrite(TI3254_HPL_DRV_GAIN_CTRL_REG, 0x00);     // HPL driver is not muted, HPL driver gain is 0dB
                AudioCodec_regWrite(TI3254_HPR_DRV_GAIN_CTRL_REG, 0x00);     // HPR driver is not muted, HPL driver gain is 0dB
            }

            if(speaker & AudioCodec_SPEAKER_LO)
            {
                //Unmute LOL/LOR driver, 0dB Gain
                AudioCodec_regWrite(TI3254_LOL_DRV_GAIN_CTRL_REG, 0x0E); // LOL driver gain is 11dB
                AudioCodec_regWrite(TI3254_LOR_DRV_GAIN_CTRL_REG, 0x0E); // LOL driver gain is 11dB
            }



            AudioCodec_pageSelect(TI3254_PAGE_0);        //Select Page 0

            //DAC => 64dB
            AudioCodec_regWrite(TI3254_LEFT_DAC_VOL_CTRL_REG, 0x80);     // Digital Volume Control = 64.0dB silent Note: As per data sheet its reserved but on setting this value there is silent
            AudioCodec_regWrite(TI3254_RIGHT_DAC_VOL_CTRL_REG, 0x80);    // Digital Volume Control = 64.0dB silent Note: As per data sheet its reserved but on setting this value there is silent


            AudioCodec_pageSelect(TI3254_PAGE_0);        //Select Page 0

            //Power up LDAC/RDAC
            AudioCodec_regWrite(TI3254_DAC_CHANNEL_SETUP_1_REG, 0xD6);   // Left and Right DAC Channel Powered Up
            // Left DAC data Left Channel Audio Interface Data
            // Right DAC data is Left Channel Audio Interface Data
            // Soft-Stepping is disabled

            //Unmute LDAC/RDAC
            AudioCodec_regWrite(TI3254_DAC_CHANNEL_SETUP_2_REG, 0x00);   // When Right DAC Channel is powered down, the data is zero.
            // Auto Mute disabled
            // Left and Right DAC Channel not muted
            // Left and Right Channel have independent volume control
        }


        if(mic)
        {
            unsigned char reg1 = 0x00;  // TI3254_MICBIAS_CTRL_REG
            unsigned char reg2 = 0x00;  // TI3254_LEFT_MICPGA_P_CTRL_REG
            unsigned char reg3 = 0x00;  // TI3254_LEFT_MICPGA_N_CTRL_REG
            unsigned char reg4 = 0x00;  // TI3254_RIGHT_MICPGA_P_CTRL_REG
            unsigned char reg5 = 0x00;  // TI3254_RIGHT_MICPGA_N_CTRL_REG
            unsigned char reg6 = 0x00;  // TI3254_FLOAT_IP_CTRL_REG


            AudioCodec_pageSelect(TI3254_PAGE_8);    // Select Page 8

            AudioCodec_regWrite(TI3254_ADC_ADP_FILTER_CTRL_REG, 0x04);   // Adaptive Filtering enabled for ADC


            AudioCodec_pageSelect(TI3254_PAGE_0);        //Select Page 0

            AudioCodec_regWrite(TI3254_ADC_SIG_P_BLK_CTRL_REG, 0x2); // ADC Signal Processing Block PRB_P2

            if(mic & AudioCodec_MIC_LINE_IN)
            {
                reg1 |= 0x40;   // MICBIAS powered up
                reg2 |= 0x40;   // IN1L is routed to Left MICPGA with 10k resistance
                reg3 |= 0x40;   // CM is routed to Left MICPGA via CM1L with 10k resistance
                reg4 |= 0x40;   // IN1R is routed to Right MICPGA with 10k resistance
                reg5 |= 0x40;   // CM is routed to Right MICPGA via CM1R with 10k resistance
                reg6 |= 0xC0;   // IN1L input is weakly driven to common mode. Use when not routing IN1L to Left and Right MICPGA and HPL, HPR
            }

            if(mic & AudioCodec_MIC_MONO)
            {
                reg1 |= 0x40;   // MICBIAS powered up
                reg2 |= 0x00;
                reg3 |= 0x10;
                reg4 |= 0x10;   // IN2R is routed to Right MICPGA with 10k resistance
                reg5 |= 0x40;   // CM is routed to Right MICPGA via CM1R with 10k resistance
                reg6 |= 0x10;   // IN2R input is weakly driven to common mode. Use when not routing IN2R to Left and Right MICPGA
            }

            if(mic & AudioCodec_MIC_ONBOARD)
            {
                reg1 |= 0x40;   // MICBIAS powered up
                reg2 |= 0x00;
                reg3 |= 0x04;
                reg4 |= 0x04;   // IN3R is routed to Right MICPGA with 10k resistance
                reg5 |= 0x40;   // CM is routed to Right MICPGA via CM1R with 10k resistance
                reg6 |= 0x04;   // IN3R input is weakly driven to common mode. Use when not routing IN3R to Left and Right MICPGA
            }

            AudioCodec_pageSelect(TI3254_PAGE_1);     //Select Page 1

            AudioCodec_regWrite(TI3254_MICBIAS_CTRL_REG, reg1);

            //Route IN2L not routed
            AudioCodec_regWrite(TI3254_LEFT_MICPGA_P_CTRL_REG, reg2);

            //Route IN2R CM1L to LEFT_N with 10K input impedance
            AudioCodec_regWrite(TI3254_LEFT_MICPGA_N_CTRL_REG, reg3);

            //Route IN2R to RIGHT_P with 10K input impedance
            AudioCodec_regWrite(TI3254_RIGHT_MICPGA_P_CTRL_REG, reg4);

            //Route CM1R to RIGHT_M with 10K input impedance
            AudioCodec_regWrite(TI3254_RIGHT_MICPGA_N_CTRL_REG, reg5);

            AudioCodec_regWrite(TI3254_FLOAT_IP_CTRL_REG, reg6);


            //make channel gain 0dB, since 20K input
            //impedance is used single ended
            AudioCodec_regWrite(TI3254_LEFT_MICPGA_VOL_CTRL_REG, 0x00);  // 0.0dB

            //Unmute Right MICPGA, Gain selection of 6dB to
            //make channel gain 0dB, since 20K input
            //impedance is used single ended
            AudioCodec_regWrite(TI3254_RIGHT_MICPGA_VOL_CTRL_REG, 0x00); // 0.0dB

            AudioCodec_pageSelect(TI3254_PAGE_0);     //Select Page 0

            AudioCodec_regWrite(TI3254_LEFT_ADC_VOL_CTRL_REG, 0x68);     // -12dB
            AudioCodec_regWrite(TI3254_RIGHT_ADC_VOL_CTRL_REG, 0x68);    // -12dB



            AudioCodec_pageSelect(TI3254_PAGE_0);    // Select Page 0

            //Power up LADC/RADC
            AudioCodec_regWrite(TI3254_ADC_CHANNEL_SETUP_REG, 0xC0); // Left and Right Channel ADC is powered up

            //Unmute LADC/RADC
            AudioCodec_regWrite(TI3254_ADC_FINE_GAIN_ADJ_REG, 0x00); // Left and Right ADC Channel Un-muted. Left and Right ADC Channel Fine Gain = 0dB,
        }

    }

    return 0;
}

/*********************************************************************
 * @brief   Configure volume level for specific audio out on a codec device
 *
 * @param[in] codecId       - Device id
 * @param[in] speaker       - Audio out id. (headphone, line out, all etc..)
 *                              Please refer Audio out Macro section
 * @param[in] volumeLevel   -  Volume level. 0-100
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_speakerVolCtrl(unsigned char codecId, unsigned char speaker, signed char volumeLevel)
{
    short  vol = 0;

    if(volumeLevel < 4)
    {
        vol = 129;  // Register value for minimum volume (-63.5 dB)
    }
    else if (volumeLevel > 91)
    {
        vol = 48;   // Register value for maximum volume (+24.0 dB)
    }
    else
    {
        vol = 122 + (volumeLevel << 1); // Maps values in range [4,91] into [-63.0, +24] dB, in 1dB steps
    }


    AudioCodec_pageSelect(TI3254_PAGE_0);
    AudioCodec_regWrite(TI3254_LEFT_DAC_VOL_CTRL_REG, (unsigned char )(vol&0x00FF));
    AudioCodec_regWrite(TI3254_RIGHT_DAC_VOL_CTRL_REG, (unsigned char )(vol&0x00FF));
    return 0;
}

/*********************************************************************
 * @brief   Mute Audio line out
 *
 * @param[in] codecId        - Device id
 * @param[in] speaker        - Audio out id. (headphone, line out, all etc..)
 *                               Please refer Audio out Macro section
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_speakerMute(unsigned char codecId, unsigned char speaker)
{
    AudioCodec_pageSelect(TI3254_PAGE_0);

    //Unmute LDAC/RDAC
    AudioCodec_regWrite(TI3254_DAC_CHANNEL_SETUP_2_REG, 0x0C);   // Left and Right DAC Channel muted
    return 0;
}

/*********************************************************************
 * @brief   Unmute audio line out
 *
 * @param[in] codecId        - Device id
 * @param[in] speaker        - Audio out id. (headphone, line out, all etc..)
 *                               Please refer Audio out Macro section
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_speakerUnmute(unsigned char codecId, unsigned char speaker)
{

    AudioCodec_pageSelect(TI3254_PAGE_0);

    //Unmute LDAC/RDAC
    AudioCodec_regWrite(TI3254_DAC_CHANNEL_SETUP_2_REG, 0x00);   // Left and Right DAC Channel not muted
    return 0;
}

/*********************************************************************
 * @brief   Configure volume level for specific audio in on a codec device
 *
 * @param[in] codecId       - Device id
 * @param[in] mic           - Audio in id. (line in, mono mic, all etc..)
 *                              Please refer Audio In Macro section
 * @param[in] volumeLevel   - Volume level (0 - 100)
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_micVolCtrl(unsigned char codecId, unsigned char mic, signed char volumeLevel)
{
    static unsigned char vol = 0x00;

    //Note: Volume level 0 will not mute the ADC

    if(volumeLevel < 2)
    {
        vol = 104;
    }
    else
    {
        vol = 103 + (volumeLevel >> 1);
    }

    AudioCodec_pageSelect(TI3254_PAGE_0);    // Select Page 0


    //Unmute LADC/RADC
    AudioCodec_regWrite(TI3254_LEFT_ADC_VOL_CTRL_REG, (unsigned char )(vol&0x7F));
    AudioCodec_regWrite(TI3254_RIGHT_ADC_VOL_CTRL_REG, (unsigned char )(vol&0x7F));
    return 0;
}

/*********************************************************************
 * @brief   Mute Audio line in
 *
 * @param[in] codecId       - Device id
 * @param[in] mic           - Audio in id. (line in, mono mic, all etc..)
 *                          Please refer Audio In Macro section
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_micMute(unsigned char codecId, unsigned char mic)
{

    AudioCodec_pageSelect(TI3254_PAGE_0);    // Select Page 0

    //Unmute LADC/RADC
    AudioCodec_regWrite(TI3254_ADC_FINE_GAIN_ADJ_REG, 0x88); // Left and Right ADC Channel Un-muted. Left and Right ADC Channel Fine Gain = 0dB,
    return 0;
}

/*********************************************************************
 * @brief   Unmute audio line
 *
 * @param[in] codecId   - Device id
 * @param[in] mic       - Audio in id. (line in, mono mic, all etc..)
 *                      Please refer Audio In Macro section
 *
 * @return 0 on success else -ve.
 */
int AudioCodec_micUnmute(unsigned char codecId, unsigned char mic)
{
    AudioCodec_pageSelect(TI3254_PAGE_0);    // Select Page 0

    //Unmute LADC/RADC
    AudioCodec_regWrite(TI3254_ADC_FINE_GAIN_ADJ_REG, 0x00); // Left and Right ADC Channel Un-muted. Left and Right ADC Channel Fine Gain = 0dB,
    return 0;
}

/*********************************************************************
 * @brief   Select Codec page that need to configure
 *
 * @param[in] ulPageAddress - page id
 *
 * @return 0 on success else -ve.
 */
static unsigned long AudioCodec_pageSelect(unsigned char ulPageAddress)
{
    return AudioCodec_regWrite(TI3254_PAGE_SEL_REG,ulPageAddress);
}

/*********************************************************************
 * @brief   Select Codec page that need to configure
 *
 * @param[in] ulRegAddr - Register Address
 * @param[in] ucRegValue - 8 bit Register Value
 *
 * @return 0 on success else -ve.
 */
static unsigned long AudioCodec_regWrite(unsigned char ulRegAddr,unsigned char ucRegValue)
{

    // Initialize return value to success
    uint8_t stat = AudioCodec_STATUS_SUCCESS;
    uint8_t writeDataBuf[2];
    writeDataBuf[0] = (uint8_t)ulRegAddr;
    writeDataBuf[1] = ucRegValue;

    uint8_t readDataBuf[1];

    I2C_Transaction i2cTransaction;
    i2cTransaction.writeBuf = writeDataBuf;
    i2cTransaction.writeCount = 2;
    i2cTransaction.readBuf = readDataBuf;
    i2cTransaction.readCount = 0;

    i2cTransaction.slaveAddress = CODEC_I2C_SLAVE_ADDR;

    // Write to the register
    bool ret = I2C_transfer(i2cHandle, &i2cTransaction);
    if (true != ret)
    {
        stat =  AudioCodec_STATUS_I2C_FAIL;
    }

    // Setup the struct for reading back
    i2cTransaction.writeBuf = writeDataBuf;
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = readDataBuf;
    i2cTransaction.readCount = 1;

    // Read back to the register
    ret = I2C_transfer(i2cHandle, &i2cTransaction);
    if (true != ret)
    {
        stat = AudioCodec_STATUS_I2C_FAIL;
    }

    // Ensure that the write value matches the read value
    if(readDataBuf[0] != ucRegValue)
    {
      stat = AudioCodec_STATUS_I2C_FAIL;
    }

    return stat;
}
