/******************************************************************************

 @file  hal_i2c.c

 @brief Layer added on top of RTOS driver for backwards compatibility with
        CC2541ST

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_i2c.h"
#include "hw_ioc.h"
#include "hw_prcm.h"
#include "interrupt.h"
#include "ioc.h"
#include "prcm.h"
#include "sys_ctrl.h"
#include "i2c.h"
#include "hal_i2c.h"

#include <ti/drivers/I2C.h>
#include "Board.h"


//*****************************************************************************
// I2C setup
#define BSP_IOD_SDA             IOID_5
#define BSP_IOD_SCL             IOID_6

#define BSP_IOD_SDA_HP          IOID_8
#define BSP_IOD_SCL_HP          IOID_9


static uint8_t slaveAddr = 0x00;
static uint8_t interface = 0xFF;

static I2C_Handle I2Chandle;
static I2C_Params I2CParams;

//
//  Burst write to an I2C device
//
bool HalI2CWrite(uint8_t *data, uint8_t len)
{
    bool fSuccess;
#ifdef TI_DRIVERS_I2C_INCLUDED

    I2C_Transaction masterTransaction;

    masterTransaction.writeCount   = len;
    masterTransaction.writeBuf     = data;
    masterTransaction.readCount    = 0;
    masterTransaction.readBuf      = NULL;
    masterTransaction.slaveAddress = slaveAddr;

    fSuccess = I2C_transfer(I2Chandle, &masterTransaction);
#endif
    return fSuccess;
}

//
//  Single write to on I2C device
//
bool HalI2CWriteSingle(uint8_t data)
{
#ifdef TI_DRIVERS_I2C_INCLUDED
    uint8_t d;

    // This is not optimal but is an implementation
    d = data;
    return (HalI2CWrite(&d, 1));
#endif
}


//
//  Burst read from an I2C device
//
bool HalI2CRead(uint8_t *data, uint8_t len)
{
    bool fSuccess;

#ifdef TI_DRIVERS_I2C_INCLUDED
    I2C_Transaction masterTransaction;

    masterTransaction.writeCount   = 0;
    masterTransaction.writeBuf     = NULL;
    masterTransaction.readCount    = len;
    masterTransaction.readBuf      = data;
    masterTransaction.slaveAddress = slaveAddr;

    fSuccess = I2C_transfer(I2Chandle, &masterTransaction);
#endif
    return fSuccess;
}

//
//  Write and read in once operation
//
bool HalI2CWriteRead(uint8_t *wdata, uint8_t wlen, uint8_t *rdata, uint8_t rlen)
{
    bool fSuccess;
#ifdef TI_DRIVERS_I2C_INCLUDED

    I2C_Transaction masterTransaction;

    masterTransaction.writeCount   = wlen;
    masterTransaction.writeBuf     = wdata;
    masterTransaction.readCount    = rlen;
    masterTransaction.readBuf      = rdata;
    masterTransaction.slaveAddress = slaveAddr;

    fSuccess = I2C_transfer(I2Chandle, &masterTransaction);
#endif

    return fSuccess;
}


void HalI2CConfig(uint8_t newInterface, uint8_t address)
{
  slaveAddr = address;
#ifdef TI_DRIVERS_I2C_INCLUDED
  // TBD: will this concept work with power management and TI RTOS style drivers?
  //      should driver be torn down and reopened??
  if (newInterface != interface)
  {
    interface = newInterface;
    if (interface == BSP_I2C_INTERFACE_0)
    {
      IOCIOPortPullSet(BSP_IOD_SDA, IOC_NO_IOPULL);
      IOCIOPortPullSet(BSP_IOD_SCL, IOC_NO_IOPULL);
      IOCPinTypeI2c(I2C0_BASE, BSP_IOD_SDA, BSP_IOD_SCL);
      IOCPinTypeGpioInput(BSP_IOD_SDA_HP);
      IOCPinTypeGpioInput(BSP_IOD_SCL_HP);
    }
    else if (interface == BSP_I2C_INTERFACE_1)
    {
      IOCIOPortPullSet(BSP_IOD_SDA_HP, IOC_NO_IOPULL);
      IOCIOPortPullSet(BSP_IOD_SCL_HP, IOC_NO_IOPULL);
      IOCPinTypeI2c(I2C0_BASE, BSP_IOD_SDA_HP, BSP_IOD_SCL_HP);
      IOCPinTypeGpioInput(BSP_IOD_SDA);
      IOCPinTypeGpioInput(BSP_IOD_SCL);
    }
  }
#endif
}


void HalI2CInit(void)
{
#ifdef TI_DRIVERS_I2C_INCLUDED
    Board_initI2C();     // Setup I2C  and initialize the driver.

    I2C_Params_init(&I2CParams);
    I2CParams.bitRate = I2C_400kHz;
    I2Chandle = I2C_open(Board_I2C, &I2CParams);
    if (I2Chandle == NULL) {
        while(1) {
            // whait here for ever
        }
    }
#endif
}

