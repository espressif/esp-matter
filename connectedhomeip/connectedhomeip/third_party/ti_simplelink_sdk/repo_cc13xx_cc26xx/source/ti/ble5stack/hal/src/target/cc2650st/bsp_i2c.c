/******************************************************************************

 @file  bsp_i2c.c

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

#include "bsp_i2c.h"

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "Board.h"


//*****************************************************************************
// I2C setup

static uint8_t slaveAddr = 0x00;
static uint8_t interface = 0xFF;
static I2C_Handle I2Chandle;
static I2C_Params I2CParams;
static Semaphore_Struct mutex;

//
//  Burst write to an I2C device
//
bool bspI2cWrite(uint8_t *data, uint8_t len)
{
  I2C_Transaction masterTransaction;

  masterTransaction.writeCount   = len;
  masterTransaction.writeBuf     = data;
  masterTransaction.readCount    = 0;
  masterTransaction.readBuf      = NULL;
  masterTransaction.slaveAddress = slaveAddr;

  return I2C_transfer(I2Chandle, &masterTransaction);
}

//
//  Single write to an I2C device
//
bool bspI2cWriteSingle(uint8_t data)
{
  uint8_t d;

  // This is not optimal but is an implementation
  d = data;
  return (bspI2cWrite(&d, 1));
}


//
//  Burst read from an I2C device
//
bool bspI2cRead(uint8_t *data, uint8_t len)
{
  I2C_Transaction masterTransaction;

  masterTransaction.writeCount   = 0;
  masterTransaction.writeBuf     = NULL;
  masterTransaction.readCount    = len;
  masterTransaction.readBuf      = data;
  masterTransaction.slaveAddress = slaveAddr;

  return I2C_transfer(I2Chandle, &masterTransaction);
}

//
//  Write and read in one operation
//
bool bspI2cWriteRead(uint8_t *wdata, uint8_t wlen, uint8_t *rdata, uint8_t rlen)
{
  bool f;

  I2C_Transaction masterTransaction;

  masterTransaction.writeCount   = wlen;
  masterTransaction.writeBuf     = wdata;
  masterTransaction.readCount    = rlen;
  masterTransaction.readBuf      = rdata;
  masterTransaction.slaveAddress = slaveAddr;

  f =  I2C_transfer(I2Chandle, &masterTransaction);
  /*
  if (!f)
    Board_initI2C();
  */
  return f;
}


void bspI2cAcquire(uint8_t newInterface, uint8_t address)
{
  Semaphore_pend(Semaphore_handle(&mutex),BIOS_WAIT_FOREVER);

  slaveAddr = address;

  // TBD: will this concept work with power management and TI RTOS style drivers?
  //      should driver be torn down and reopened??
  if (newInterface != interface)
  {
    interface = newInterface;

    if (interface == BSP_I2C_INTERFACE_0)
    {
      IOCPinTypeI2c(I2C0_BASE, BSP_IOID_SDA, BSP_IOID_SCL);
      IOCPinTypeGpioInput(BSP_IOID_SDA_HP);
      IOCPinTypeGpioInput(BSP_IOID_SCL_HP);
    }
    else if (interface == BSP_I2C_INTERFACE_1)
    {
      IOCPinTypeI2c(I2C0_BASE, BSP_IOID_SDA_HP, BSP_IOID_SCL_HP);
      IOCPinTypeGpioInput(BSP_IOID_SDA);
      IOCPinTypeGpioInput(BSP_IOID_SCL);
    }

  }
}

void bspI2cRelease(void)
{
  Semaphore_post(Semaphore_handle(&mutex));
}


void bspI2cInit(void)
{
  Semaphore_Params semParamsMutex;

  Board_initI2C();     // Setup I2C  and initialize the driver.

  // Create protection semaphore
  Semaphore_Params_init(&semParamsMutex);
  semParamsMutex.mode = Semaphore_Mode_BINARY;
  Semaphore_construct(&mutex, 1, &semParamsMutex);

  I2C_Params_init(&I2CParams);
  I2CParams.bitRate = I2C_400kHz;
  I2Chandle = I2C_open(Board_I2C, &I2CParams);

  if (I2Chandle == NULL) {
    while(1) {
      // whait here for ever
    }
  }
}

