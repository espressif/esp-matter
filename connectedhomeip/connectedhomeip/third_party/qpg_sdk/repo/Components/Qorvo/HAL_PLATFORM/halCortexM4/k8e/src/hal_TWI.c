/*
 * Copyright (c) 2015-2016, GreenPeak Technologies
 * Copyright (c) 2017, Qorvo Inc
 *
 *   Hardware Abstraction Layer for the TWI master interface.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALCORTEXM4

#include "hal.h"
#include "gpLog.h"
#include "gpHal_reg.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#ifndef HAL_TWI_CLK_SPEED
#define HAL_TWI_CLK_SPEED       10000UL
#endif

#ifndef HAL_TWI_MAX_WAIT_FOR_INTERRUPT_IN_US_DIV_255
#define HAL_TWI_MAX_WAIT_FOR_INTERRUPT_IN_US_DIV_255   (1000000UL/HAL_TWI_CLK_SPEED) /*waiting max 255 bit clk cycles*/
#endif

#ifndef HAL_TWI_MAX_WAIT_FOR_ACK
#define HAL_TWI_MAX_WAIT_FOR_ACK   (1000000UL) /*us*/
#endif

#ifdef HAL_DIVERSITY_TWI_SLAVE
#ifndef HAL_STWI_MAX_TX_DATA
#define HAL_STWI_MAX_TX_DATA     50
#endif
#endif // HAL_DIVERSITY_TWI_SLAVE

#define TWI_PRESCALER           ((3200000UL/((HAL_TWI_CLK_SPEED)*4))-1)

#define ACK  0
#define NACK 1

#define START 1
#define STOP  1

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static Bool  halTWI_ActivityDetected = false;
static Bool  halTWI_Active           = false;

#ifdef HAL_DIVERSITY_TWI_SLAVE
// TWI Slave Transmit Buffer
UInt8 halSTWI_TxBuffer[HAL_STWI_MAX_TX_DATA];
UInt8 halSTWI_TxPtr;
UInt8 halSTWI_TxLength = 0;

// TWI Slave received data callback
static halSTWI_cbRxData_t rxDataCallback = NULL;
// TWI Slave received Stop bit indication callback
static halSTWI_cbStopIndication_t stopIndicationCallback = NULL;
#endif // HAL_DIVERSITY_TWI_SLAVE

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void halTWI_Configure(Bool enable, Bool ack, Bool clk_sync_dis)
{
    UInt8 config;

    config = 0x0;
    GP_WB_SET_I2C_M_CLK_SYNC_DISABLE_TO_CONFIG(config, clk_sync_dis);
    GP_WB_SET_I2C_M_ACK_TO_CONFIG(config, ack);
    GP_WB_SET_I2C_M_ENABLE_TO_CONFIG(config, enable);

    GP_WB_WRITE_I2C_M_CONFIG(config);

    if(enable)
    {
        GP_BSP_MTWI_SCLK_INIT();
        GP_BSP_MTWI_SDA_INIT();
    }
    else
    {
        GP_BSP_MTWI_SCLK_DEINIT();
        GP_BSP_MTWI_SDA_DEINIT();
    }

}

#ifdef HAL_DIVERSITY_TWI_SLAVE
/**
 *
 *  This function is used to Configure I2C slave register,
 *  @param deviceAddress            Parameter which defines the slave address of the TWI slave
 *  @param sda_map                  Parameter which defines the TWI slave's SDA Pin
 *  @param sclk_map                 Parameter which defines the TWI slave's SCLK Pin
 *  @param acceptGeneralCallEn      Set true to enable TWI slave to response general call address
 */
static void halSTWI_Configure(UInt16 deviceAddress, Bool acceptGeneralCallEn)
{
    // The Slave Address
    GP_WB_WRITE_I2C_SL_SLAVE_ADDRESS(deviceAddress);
    GP_LOG_SYSTEM_PRINTF("set slave address: 0x%x", 0,GP_WB_READ_I2C_SL_SLAVE_ADDRESS());

    // disable clock stretching by the slave (GP570 does not support TWI slave clock stretching)
    // Did this change for the new chips ??
    GP_WB_WRITE_I2C_SL_SCL_STRETCH_EN(false);

    // set enable/disable slave selection via the general call address
    GP_WB_WRITE_I2C_SL_ACCEPT_GENERAL_CALL(acceptGeneralCallEn);

    // Enable Slave
    GP_WB_WRITE_I2C_SL_ENABLE(1);

    // set the SDA pin and SCLK pin mapping for Slave
    GP_BSP_STWI_SCLK_INIT();
    GP_BSP_STWI_SDA_INIT();
}
#endif // HAL_DIVERSITY_TWI_SLAVE

static Bool halTWI_ExecuteAndWait(UInt8 cmd)
{
    UInt8 i = 0;

    //Add reset of interrupts
    GP_WB_SET_I2C_M_CLR_DONE_INTERRUPT_TO_CONTROL(cmd, 1);
    GP_WB_SET_I2C_M_CLR_ARB_LOST_INTERRUPT_TO_CONTROL(cmd, 1);

    GP_WB_WRITE_I2C_M_CONTROL(cmd);

    //Wait for processing of one shot
    HAL_WAIT_US(1);

    //Wait until interrupt is seen
    //We do need a max loop counter to avoid lock ups in case connectivity problems occur or slave device is broken,
    for (i=0; i<255; i++)
    {
        if (GP_WB_READ_I2C_M_UNMASKED_DONE_INTERRUPT())
        {
            return true;
        }
        if (GP_WB_READ_I2C_M_UNMASKED_ARB_LOST_INTERRUPT())
        {
            return false;
        }
        HAL_WAIT_US(HAL_TWI_MAX_WAIT_FOR_INTERRUPT_IN_US_DIV_255);
    }
    return false;
}

static Bool halTWI_TxByte(Bool start, Bool stop, UInt8 txByte)
{
    UInt8 cmd;
    Bool success;

    //Put byte in Tx register
    GP_WB_WRITE_I2C_M_TX_DATA(txByte);

    //Trigger Tx
    cmd = 0x0;
    GP_WB_SET_I2C_M_START_TO_CONTROL(cmd, start); //Start condition
    GP_WB_SET_I2C_M_STOP_TO_CONTROL(cmd, stop); //Stop condition
    GP_WB_SET_I2C_M_WRITE_TO_CONTROL(cmd, 1); //Write byte

    success = halTWI_ExecuteAndWait(cmd);

    //Expect an ack to be received
    success &= (GP_WB_READ_I2C_M_RX_ACK() == ACK);

    return success;
}

static Bool halTWI_RxByte(Bool stop, UInt8* pData)
{
    UInt8 cmd=0;
    Bool success;

    cmd = 0x0;
    GP_WB_SET_I2C_M_READ_TO_CONTROL(cmd, 1); //Read a byte
    if (stop)
    {
        GP_WB_SET_I2C_M_STOP_TO_CONTROL(cmd, 1);//Read a byte and stop
        GP_WB_WRITE_I2C_M_ACK(NACK); //No ack expected
    }
    else
    {
        GP_WB_WRITE_I2C_M_ACK(ACK); //Ack to be sent
    }
    success = halTWI_ExecuteAndWait(cmd); //Execute command
    // Expect an nack on stop=true and an ack on stop=false to be received
    success &= (GP_WB_READ_I2C_M_RX_ACK() == (stop ? NACK : ACK));
    *pData = GP_WB_READ_I2C_M_RX_DATA();

    return success;
}

static Bool halTWI_WriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer, Bool polled)
{
    UInt8 i;
    Bool success;

    halTWI_Active           = true;
    halTWI_ActivityDetected = true;

    //Enable TWI block + set mappings
    halTWI_Configure(true, ACK, 0);

    //TX
    if(txLength != 0)
    {
        //Write access CMD
        success = halTWI_TxByte(START, !STOP, deviceAddress);
        if(!success) goto hal_WriteReadTWIEnd;
    }
    else
    {
        //Only checking for ACK
        success = halTWI_TxByte(START, STOP, deviceAddress);
        goto hal_WriteReadTWIEnd;
    }

    //Send bytes - address also in these bytes
    for(i=0; i<txLength-1; i++)
    {
        success = halTWI_TxByte(!START, !STOP, txBuffer[i]);
        if(!success) goto hal_WriteReadTWIEnd;
    }

    if((rxLength == 0) || (rxBuffer == NULL))
    {
        //Send last Tx byte and stop
        success = halTWI_TxByte(!START, STOP, txBuffer[txLength-1]);
    }
    else
    {
        //Send last Tx byte
        success = halTWI_TxByte(!START, !STOP, txBuffer[txLength-1]);
    }

    if(!success) goto hal_WriteReadTWIEnd;

    if(polled)
    {
        Bool error = false;
        GP_DO_WHILE_TIMEOUT(!halTWI_TxByte(START, STOP, deviceAddress), HAL_TWI_MAX_WAIT_FOR_ACK, &error);
        if (error)
        {
            success = false;
            goto hal_WriteReadTWIEnd;
        }
    }

    //RX
    //Read out wanted bytes
    if(rxLength != 0 && rxBuffer != NULL)
    {
        //Read access CMD
        success = halTWI_TxByte(START, !STOP, deviceAddress | 0x01);
        if(!success) goto hal_WriteReadTWIEnd;

        for(i=0; i<rxLength-1; i++)
        {
            //Read out bytes
            success = halTWI_RxByte(!STOP, &(rxBuffer[i]));
            if(!success) goto hal_WriteReadTWIEnd;
        }

        //Read out last byte - stop
        success = halTWI_RxByte(STOP, &(rxBuffer[rxLength-1]));
        if(!success) goto hal_WriteReadTWIEnd;
    }

hal_WriteReadTWIEnd:

    //Disable TWI block + clr mappings
    halTWI_Configure(false, ACK, 0);

    halTWI_Active = false;

    return success;
}

static void halTWI_BusClear(void)
{
    UInt8 cmd;

    halTWI_Configure(true, NACK, 0);

    //Perform a software reset sequence to resolve situation in which the EEPROM is in a corrupt state
    cmd = 0x0;
    GP_WB_SET_I2C_M_START_TO_CONTROL(cmd, 1); //Start condition
    GP_WB_SET_I2C_M_READ_TO_CONTROL(cmd, 1);  //Read byte
    halTWI_ExecuteAndWait(cmd);               //Start + Dummy read

    GP_WB_SET_I2C_M_STOP_TO_CONTROL(cmd, 1);  //Stop condition
    halTWI_ExecuteAndWait(cmd); //Start + Stop (+ Dummy read since the hw module requires the read to be done)

    //Disable TWI block + clr mappings
    halTWI_Configure(false, ACK, 0);
}

#ifdef HAL_DIVERSITY_TWI_SLAVE
static Bool halSTWI_TxByte(void)
{
    if ((halSTWI_TxLength) && (halSTWI_TxPtr < halSTWI_TxLength))
    {
        GP_WB_WRITE_I2C_SL_TX_DATA(halSTWI_TxBuffer[halSTWI_TxPtr]);
        halSTWI_TxPtr++;

        return true;
    }
    else
    {
        return false;
    }
}
#endif // HAL_DIVERSITY_TWI_SLAVE

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#ifdef HAL_DIVERSITY_TWI_SLAVE
void hal_RegisterSTWI_RxDataCallback(halSTWI_cbRxData_t callbacks)
{
    rxDataCallback = callbacks;
}

void hal_RegisterSTWI_StopIndicationCallback(halSTWI_cbStopIndication_t callbacks)
{
    stopIndicationCallback = callbacks;
}
#endif // HAL_DIVERSITY_TWI_SLAVE

void hal_InitTWI(void)
{
    // Not enabling pullups here - expect external pullups are used

    //Do configuration with disabled core
    GP_WB_WRITE_I2C_M_ENABLE(false);
    GP_WB_WRITE_I2C_M_PRESCALER(TWI_PRESCALER);

    halTWI_BusClear();
}

Bool hal_PolledAckWriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer)
{
    return halTWI_WriteReadTWI(deviceAddress, txLength, txBuffer, rxLength, rxBuffer, true);
}

Bool hal_WriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer)
{
    return halTWI_WriteReadTWI(deviceAddress, txLength, txBuffer, rxLength, rxBuffer, false);
}

Bool hal_WasActiveTWI(void)
{
    if (halTWI_ActivityDetected)
    {
        if (!halTWI_Active) //no longer active
        {
            halTWI_ActivityDetected = false; //destructive read
        }
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef HAL_DIVERSITY_TWI_SLAVE
void hal_InitSTWI(UInt8 slaveAddress, Bool acceptGeneralCall)
{
    /* Reset Tx Buffer */
    halSTWI_TxPtr = 0;
    halSTWI_TxLength = 0;
    MEMSET(halSTWI_TxBuffer, HAL_STWI_MAX_TX_DATA, sizeof(halSTWI_TxBuffer));

    halSTWI_Configure(slaveAddress, acceptGeneralCall);
}

void hal_EnableIntSTWI(Bool en)
{
    HAL_DISABLE_GLOBAL_INT();

    GP_WB_WRITE_INT_CTRL_MASK_I2CSL_SLAD_INTERRUPT(en);
    GP_WB_WRITE_INT_CTRL_MASK_I2CSL_STOP_INTERRUPT(en);
    GP_WB_WRITE_INT_CTRL_MASK_I2CSL_RX_NOT_EMPTY_INTERRUPT(en);

    GP_WB_WRITE_INT_CTRL_MASK_INT_I2CSL_INTERRUPT(en);

    if(en)
    {
      NVIC_ClearPendingIRQ(I2CSL_IRQn);
      NVIC_EnableIRQ(I2CSL_IRQn);
    }
    else
    {
      NVIC_DisableIRQ(I2CSL_IRQn);
    }
    HAL_ENABLE_GLOBAL_INT();
}

void hal_SendDataSTWI(UInt8 length, UInt8* txData)
{
    if (length)
    {
        /* Reset Tx Pointer */
        halSTWI_TxPtr = 0;
        halSTWI_TxLength = length;

        /* Prepare the Tx Buffer to send out */
        MEMCPY(halSTWI_TxBuffer,txData,length);

        /* Push the byte from the Tx Buffer into register to send out */
        halSTWI_TxByte();

        /* Enable TX_NOT_FULL_INTERRUPT and keep sending the byte when interrupt is received */
        GP_WB_WRITE_INT_CTRL_MASK_I2CSL_TX_NOT_FULL_INTERRUPT(true);
    }
}

void i2csl_handler_impl(void)
{
    HAL_DISABLE_GLOBAL_INT();

    if (GP_WB_READ_I2C_SL_UNMASKED_SLAD_INTERRUPT())
    {
        // clear the SLAD interrupt flag
        GP_WB_I2C_SL_CLR_SLAD_INTERRUPT();
        GP_LOG_PRINTF("I2C irq: SLAD %d",0, GP_WB_READ_I2C_SL_READ_FLAG());
    }

    /* launch callback when the slave device is selected and received data in RX_DATA */
    if (GP_WB_READ_I2C_SL_UNMASKED_RX_NOT_EMPTY_INTERRUPT() && GP_WB_READ_I2C_SL_BUSY_SLAD())
    {
        if (rxDataCallback != NULL)
        {
            rxDataCallback(GP_WB_READ_I2C_SL_RX_DATA(), GP_WB_READ_I2C_SL_GENERAL_CALL_FLAG());
        }
    }

    /* When TX_DATA is detected empty and there is data pending to send,
       Push the data byte from the Tx Buffer into TX_DATA to send out */
    if (halSTWI_TxLength && GP_WB_READ_I2C_SL_UNMASKED_TX_NOT_FULL_INTERRUPT() )
    {
        Bool success = halSTWI_TxByte();
        if (!success)
        {
            GP_WB_WRITE_INT_CTRL_MASK_I2CSL_TX_NOT_FULL_INTERRUPT(false);
        }
    }

    if (GP_WB_READ_I2C_SL_UNMASKED_STOP_INTERRUPT())
    {
        GP_WB_I2C_SL_CLR_STOP_INTERRUPT();
        // check and clear Tx Buffer and pointer
        if (halSTWI_TxLength)
        {
            GP_LOG_PRINTF("TxPtr = %d, TxLen = %d",0, halSTWI_TxPtr, halSTWI_TxLength);
            MEMSET(halSTWI_TxBuffer,0, sizeof(halSTWI_TxBuffer));
            halSTWI_TxPtr = 0;
            halSTWI_TxLength = 0;
            GP_WB_WRITE_INT_CTRL_MASK_I2CSL_TX_NOT_FULL_INTERRUPT(false);
        }
        /* pass to upper layer that the transmission is completed as stop bit is received */
        if (stopIndicationCallback != NULL)
        {
            stopIndicationCallback(GP_WB_READ_I2C_SL_READ_FLAG());
        }
    }
    HAL_ENABLE_GLOBAL_INT();
}
#endif // HAL_DIVERSITY_TWI_SLAVE

