/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files *************************************************/
#include "cc_pal_types.h"
#include "cc_pal_mutex.h"
#include "cc_pal_interrupt_ctrl_plat.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cc_regs.h"
#include "dx_host.h"
#include "cc_hal.h"
/************************ Defines ********************************************/

/************************ Enums **********************************************/

/************************ Typedefs *******************************************/

/************************ Global Data ****************************************/
QueueHandle_t xQueue = NULL;

/************************ Private Functions **********************************/
eIrqReturn CC_Handler(uint32_t index, void *args);

/************************ Public Functions ***********************************/
/**
 * @brief
 *
 * @param[in]
 *
 * @param[out]
 *
 * @return - CC_SUCCESS for success, CC_FAIL for failure.
 */
CCError_t CC_PalInitIrq(void)
{
    uint32_t pxRxedMessage = 0;

    uint32_t mask = 0;

    /* clear all interrupts before starting the engine */
    CC_HAL_WRITE_REGISTER( CC_REG_OFFSET(HOST_RGF, HOST_ICR), 0xFFFFFFFFUL);

    /* unmask all interrupts except for RNG_INT */
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, SRAM_TO_DIN_MASK, mask, 0);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_SRAM_MASK, mask, 0);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, MEM_TO_DIN_MASK, mask, 0);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, DOUT_TO_MEM_MASK, mask, 0);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, AXI_ERR_MASK, mask, 0);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, PKA_EXP_MASK, mask, 0);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, RNG_INT_MASK, mask, 1);
    CC_REG_FLD_SET(HOST_RGF, HOST_IMR, SYM_DMA_COMPLETED_MASK, mask, 0);
    CC_HAL_WRITE_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IMR), mask);

    if (xQueue == NULL)
    {
        xQueue = xQueueCreate( 1, sizeof(uint32_t) );
        if (xQueue == NULL){
            return CC_FAIL;
        }
    }

    /* initialize interrupt controller */
    if(CC_PalRequestIrq( CRYPTOCELL_INTERRUPT, &CC_Handler,
                "CC", sizeof("CC"), NULL)){
        return CC_FAIL;
    }

    if(CC_PalEnableIrq(CRYPTOCELL_INTERRUPT)){
        return CC_FAIL;
    }

    /*  if there is already an interrupt raised in the system the interrupt
     *  handler will handle it and send message to the queue. in this case
     *  the queue needs to be emptied by calling xQueueReceive().
     *  return value is 0 for error and 1 for success - FreeRTOS logics*/

    if (xQueueReceive( xQueue, &( pxRxedMessage ),  10 ) )
    {
        return CC_SUCCESS;
    }
    else
    {
       return CC_FAIL;
    }

}

/**
 * @brief This function removes the interrupt handler for
 * cryptocell interrupts.
 *
 */
void CC_PalFinishIrq(void)
{
    CC_PalFreeIrq(CRYPTOCELL_INTERRUPT);

}

/**
 * @brief This function sets one of the handler function pointers that are
 * in handlerFuncPtrArr, according to given index.
 *
 * @param[in]
 * handlerIndx - Irq index.
 * funcPtr - Address of the new handler function.
 *
 * @param[out]
 *
 * @return - CC_SUCCESS for success, CC_FAIL for failure.
 */
CCError_t CC_PalRequestIrq(uint32_t irq, IrqHandlerPtr funcPtr,
            const char *name, uint8_t nameLength, void *args)
{
    CC_UNUSED_PARAM(nameLength);
    if (!RequestIrq(irq, funcPtr, name, args))
        return CC_FAIL;

    return CC_SUCCESS;
}

/**
 * @brief This function removes an interrupt handler.
 *
 * @param[in]
 * irq - Irq index.
 *
 * @param[out]
 *
 * @return
 */
void CC_PalFreeIrq(uint32_t irq)
{
    FreeIrq(irq);
}

/**
 * @brief This function enables an IRQ according to given index.
 *
 * @param[in]
 * irq - Irq index.
 *
 * @param[out]
 *
 * @return - CC_SUCCESS for success, CC_FAIL for failure.
 */
CCError_t CC_PalEnableIrq(uint32_t irq)
{
    if (!EnableIrq(irq))
        return CC_FAIL;

    return CC_SUCCESS;
}

/**
 * @brief This function disables an IRQ according to given index.
 *
 * @param[in]
 * irq - Irq index.
 *
 * @param[out]
 *
 * @return - CC_SUCCESS for success, CC_FAIL for failure.
 */
CCError_t CC_PalDisableIrq(uint32_t irq)
{
    if (!DisableIrq(irq))
        return CC_FAIL;

    return CC_SUCCESS;
}

/* ISR handler for CryptoCell interrupts.
 * Clears the IRR and sends the value of IRR to xQueue. The value is
 * received and checked by CC_PalWaitInterrupt.
 *
 * */
eIrqReturn CC_Handler(uint32_t index, void *args)
{
    uint32_t irr = CC_SUCCESS;
    uint32_t intBits;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    CC_UNUSED_PARAM(index);
    CC_UNUSED_PARAM(args);

    irr = CC_HAL_READ_REGISTER(CC_REG_OFFSET(HOST_RGF, HOST_IRR)) ;

    /* clear interrupt bits*/
    CC_HAL_WRITE_REGISTER( CC_REG_OFFSET(HOST_RGF, HOST_ICR), irr);  // IRR and ICR bit map is the same use data to clear interrupt in ICR


    /* Handle DMA interrupt */
     intBits = 0;
     CC_REG_FLD_SET(HOST_RGF, HOST_IRR, SYM_DMA_COMPLETED, intBits, 1);
     if ((irr&intBits) == intBits){

             /* Unblock the task waiting to be notified that the CryptoCell operation has ended. */
             xQueueSendFromISR( xQueue, &irr, &xHigherPriorityTaskWoken );
     }


    /* Force a context switch if needed*/
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

    return IRQ_HANDLED;
}

/*!
 * This function notifies for any ARM CryptoCell interrupt.
 * Sleep until there's an Interrupt from the cryptocell, caused when one of the
 * Interrupt Request Register (IRR) signals is raised.
 * it is the caller responsibility to verify and prompt the expected case
 * interrupt source.
 *
 * @param[in] data      - expected value of IRR
 * @return              - CC_SUCCESS for success, CC_FAIL for failure.
 */
CCError_t CC_PalWaitInterrupt( uint32_t data){
    CCError_t error = CC_SUCCESS;
    uint32_t irr = 0;

    /*Wait until an interrupt has occurred and a message is received from within the ISR*/
    if(xQueueReceive( xQueue, &irr,  10000 ) == pdFALSE){
        return CC_FAIL;
    }

    if( CC_REG_FLD_GET(0, HOST_IRR, AHB_ERR_INT, irr) == CC_TRUE) {
        error = CC_FAIL;
        /*set data for clearing bus error*/
        CC_REG_FLD_SET(HOST_RGF, HOST_ICR, AXI_ERR_CLEAR, data , 1);
    }

    if ((irr & data) == 0){
        return CC_FAIL;
    }
    return error;
}

