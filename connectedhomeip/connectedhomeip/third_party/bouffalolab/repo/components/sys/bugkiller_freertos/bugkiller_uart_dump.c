/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>

#ifdef BL602
#include "bl602.h"
#elif defined BL808
#include "bl808.h"
#include "bl808_common.h"
#elif defined BL616
#include "common.h"
#endif

#include "uart_reg.h"
void uart_dump_printf(uint32_t addr);

void bugkiller_uart_dump(void)
{
    printf("start uart dump\r\n");
    //uart_dump_printf( UART0_BASE );
    //printf("*********UART0_BASE dump end********\r\n");
    uart_dump_printf( UART1_BASE );
    printf("*********UART1_BASE dump end********\r\n");
}

void uart_dump_printf(uint32_t addr)
{
    uint32_t tmpVal;
    int val;
    if(addr == UART0_BASE){
        printf("*****dump UART0_BASE para*******\r\n");
    }
    else if(addr == UART1_BASE){
        printf("*****dump UART1_BASE para*******\r\n");
    }
    else{
        printf("*****error:this para should be UART BASE*******\r\n");
        return;
    }
    tmpVal = BL_RD_REG(addr,UART_UTX_CONFIG);
    printf("UART_UTX_CONFIG tmpval = 0x%lx\r\n",tmpVal);
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_EN)){
        printf("Enable signal of UART TX function\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_CTS_EN)){
        printf("Enable signal of UART TX CTS flow control function\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_FRM_EN)){
        printf("Enable signal of UART TX freerun mode (utx_end_int will be disabled)\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_PRT_EN)){
        printf("Enable signal of UART TX parity bit\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_PRT_SEL)){
        printf("Select signal of UART TX parity bit ***odd parity\r\n");
    }
    else{
         printf("Select signal of UART TX parity bit ***even parity\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_IR_EN)){
        printf("UART_CR_UTX_IR_EN\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_IR_INV)){
        printf("UART_CR_UTX_IR_INV\r\n");
    }
    val = tmpVal>>8 & 0x07;
    printf("RX DATA bit count for each character is %d\r\n",val);
    val = tmpVal>>12 & 0x03;
    printf("UART TX STOP bit count is %d\r\n",val);
    val = tmpVal>>16 & 0xffff;
    printf("Length of UART RX data transfer is %d\r\n",val);

    tmpVal = BL_RD_REG(addr,UART_URX_CONFIG);
    printf("UART_URX_CONFIG tmpval = 0x%lx\r\n",tmpVal);
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_EN)){
        printf("Enable signal of UART RX function\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_RTS_SW_MODE)){
        printf("ENABLE UART RX RTS output SW control mode\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_RTS_SW_VAL)){
        printf("UART RX RTS output SW control value is 1\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_ABR_EN)){
        printf("Enable signal of UART RX Auto Baud Rate detection function\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_PRT_EN)){
        printf("Enable signal of UART RX parity bit\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_PRT_SEL)){
        printf("Select signal of UART RX parity bit ***odd parity\r\n");
    }
    else{
         printf("Select signal of UART TX parity bit ***even parity\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_IR_EN)){
        printf("Enable signal of UART RX IR mode\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_IR_INV)){
        printf("Inverse signal of UART RX input in IR mode\r\n");
    }
    val = tmpVal>>8 & 0x07;
    printf("UART RX DATA bit count for each character is %d\r\n",val);
    val = tmpVal>>12 & 0xf;
    printf("De-glitch function cycle count is %d\r\n",val); 
    val = tmpVal>>16 & 0xffff;
    printf("Length of UART RX data transfer  is %d\r\n",val);

    tmpVal = BL_RD_REG(addr,UART_INT_EN);
    printf("UART_INT_EN tmpval = 0x%lx\r\n",tmpVal);
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_END_EN)){
        printf("Interrupt enable of utx_end_int\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_END_EN)){
        printf("Interrupt enable of urx_end_int\r\n");
    }
#ifdef BL602
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_FIFO_EN)){
        printf("Interrupt enable of utx_fifo_int\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_FIFO_EN)){
        printf("Interrupt enable of urx_fifo_int\r\n");
    }
#endif
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_RTO_EN)){
        printf("Interrupt enable of urx_rto_int\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_PCE_EN)){
        printf("Interrupt enable of urx_pce_int\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_UTX_FER_EN)){
        printf("Interrupt enable of utx_fer_int\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_CR_URX_FER_EN)){
        printf("Interrupt enable of urx_fer_int\r\n");
    }

    tmpVal = BL_RD_REG(addr,UART_INT_STS);
    printf("UART_INT_STS tmpval = 0x%lx\r\n",tmpVal);
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_UTX_END_INT)){
        printf("UART TX transfer end interrupt (set according to cr_utx_len)\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_URX_END_INT)){
        printf("UART RX transfer end interrupt (set according to cr_urx_len)\r\n");
    }
#ifdef BL602
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_UTX_FIFO_INT)){
        printf("UART TX FIFO ready (tx_fifo_cnt > tx_fifo_th) interrupt, auto-cleared when data is pushed\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_URX_FIFO_INT)){
        printf("UART RX FIFO ready (rx_fifo_cnt > rx_fifo_th) interrupt, auto-cleared when data is popped\r\n");
    }
#endif
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_URX_RTO_INT)){
        printf("UART RX Time-out interrupt\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_URX_PCE_INT)){
        printf("UART RX parity check error interrupt\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_UTX_FER_INT)){
        printf("UART TX FIFO error interrupt, auto-cleared when FIFO overflow/underflow error flag is cleared\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_URX_FER_INT)){
        printf("UART RX FIFO error interrupt, auto-cleared when FIFO overflow/underflow error flag is cleared\r\n");
    }

    tmpVal = BL_RD_REG(addr,UART_STATUS);
    printf("UART_STATUS tmpval = 0x%lx\r\n",tmpVal);
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_STS_UTX_BUS_BUSY)){
        printf("Indicator of UART TX bus busy\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_STS_URX_BUS_BUSY)){
        printf("Indicator of UART RX bus busy\r\n");
    }

    tmpVal = BL_RD_REG(addr,UART_FIFO_CONFIG_0);
    printf("UART_FIFO_CONFIG_0 tmpval = 0x%lx\r\n",tmpVal);
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_DMA_TX_EN)){
        printf("Enable signal of dma_tx_req/ack interface\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_DMA_RX_EN)){
        printf("Enable signal of dma_rx_req/ack interface\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_TX_FIFO_OVERFLOW)){
        printf("Overflow flag of TX FIFO, can be cleared by tx_fifo_clr\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_TX_FIFO_UNDERFLOW)){
        printf("Underflow flag of TX FIFO, can be cleared by tx_fifo_clr\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_RX_FIFO_OVERFLOW)){
        printf("Overflow flag of RX FIFO, can be cleared by rx_fifo_clr\r\n");
    }
    if(BL_GET_REG_BITS_VAL(tmpVal,UART_RX_FIFO_UNDERFLOW)){
        printf("Underflow flag of RX FIFO, can be cleared by rx_fifo_clr\r\n");
    }

    tmpVal = BL_RD_REG(addr,UART_FIFO_CONFIG_1);
    printf("UART_FIFO_CONFIG_1 tmpval = 0x%lx\r\n",tmpVal);
    val = tmpVal & 0x3f;
    printf("TX FIFO available count is %d\r\n",val);
    val = tmpVal>>8 & 0x3f;
    printf("RX FIFO available count is %d\r\n",val);
    val = tmpVal>>16 & 0x1f;
    printf("TX FIFO threshold, dma_tx_req will not be asserted if tx_fifo_cnt is less than this value is %d\r\n",val);
    val = tmpVal>>24 & 0x1f;
    printf("RX FIFO threshold, dma_rx_req will not be asserted if tx_fifo_cnt is less than this value is %d\r\n",val);
}
