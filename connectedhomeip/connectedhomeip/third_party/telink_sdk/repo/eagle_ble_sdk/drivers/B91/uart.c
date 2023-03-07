/********************************************************************************************************
 * @file	uart.c
 *
 * @brief	This is the source file for B91
 *
 * @author	Driver Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "uart.h"

/**********************************************************************************************************************
 *                                			  local constants                                                       *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                           	local macro                                                        *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                             local data type                                                     *
 *********************************************************************************************************************/


/**********************************************************************************************************************
 *                                              global variable                                                       *
 *********************************************************************************************************************/
dma_config_t uart_tx_dma_config[2] = { {
                                           .dst_req_sel    = DMA_REQ_UART0_TX, // tx req
                                           .src_req_sel    = 0,
                                           .dst_addr_ctrl  = DMA_ADDR_FIX,
                                           .src_addr_ctrl  = DMA_ADDR_INCREMENT, // increment
                                           .dstmode        = DMA_HANDSHAKE_MODE, // handshake
                                           .srcmode        = DMA_NORMAL_MODE,
                                           .dstwidth       = DMA_CTR_WORD_WIDTH, // must be word
                                           .srcwidth       = DMA_CTR_WORD_WIDTH, // must be word
                                           .src_burst_size = 0,                  // must be 0
                                           .read_num_en    = 0,
                                           .priority       = 0,
                                           .write_num_en   = 0,
                                           .auto_en        = 0, // must be 0
                                       },
                                       {
                                           .dst_req_sel    = DMA_REQ_UART1_TX, // tx req
                                           .src_req_sel    = 0,
                                           .dst_addr_ctrl  = DMA_ADDR_FIX,
                                           .src_addr_ctrl  = DMA_ADDR_INCREMENT, // increment
                                           .dstmode        = DMA_HANDSHAKE_MODE, // handshake
                                           .srcmode        = DMA_NORMAL_MODE,
                                           .dstwidth       = DMA_CTR_WORD_WIDTH, // must be word
                                           .srcwidth       = DMA_CTR_WORD_WIDTH, // must be word
                                           .src_burst_size = 0,                  // must be 0
                                           .read_num_en    = 0,
                                           .priority       = 0,
                                           .write_num_en   = 0,
                                           .auto_en        = 0, // must be 0
                                       } };
dma_config_t uart_rx_dma_config[2] = { {
                                           .dst_req_sel    = 0, // tx req
                                           .src_req_sel    = DMA_REQ_UART0_RX,
                                           .dst_addr_ctrl  = DMA_ADDR_INCREMENT,
                                           .src_addr_ctrl  = DMA_ADDR_FIX,
                                           .dstmode        = DMA_NORMAL_MODE,
                                           .srcmode        = DMA_HANDSHAKE_MODE,
                                           .dstwidth       = DMA_CTR_WORD_WIDTH, // must be word
                                           .srcwidth       = DMA_CTR_WORD_WIDTH, ////must be word
                                           .src_burst_size = 0,
                                           .read_num_en    = 0,
                                           .priority       = 0,
                                           .write_num_en   = 0,
                                           .auto_en        = 0, // must be 0
                                       },
                                       {
                                           .dst_req_sel    = 0, // tx req
                                           .src_req_sel    = DMA_REQ_UART1_RX,
                                           .dst_addr_ctrl  = DMA_ADDR_INCREMENT,
                                           .src_addr_ctrl  = DMA_ADDR_FIX,
                                           .dstmode        = DMA_NORMAL_MODE,
                                           .srcmode        = DMA_HANDSHAKE_MODE,
                                           .dstwidth       = DMA_CTR_WORD_WIDTH, // must be word
                                           .srcwidth       = DMA_CTR_WORD_WIDTH, ////must be word
                                           .src_burst_size = 0,
                                           .read_num_en    = 0,
                                           .priority       = 0,
                                           .write_num_en   = 0,
                                           .auto_en        = 0, // must be 0
                                       } };
/**********************************************************************************************************************
 *                                              local variable                                                     *
 *********************************************************************************************************************/
static unsigned char uart_dma_tx_chn[2];
static unsigned char uart_dma_rx_chn[2];
/**********************************************************************************************************************
 *                                          local function prototype                                               *
 *********************************************************************************************************************/
/**
 * @brief     This function is used to look for the prime.if the prime is finded,it will return 1, or return 0.
 * @param[in] n - the calue to judge.
 * @return    0 or 1
 */
static unsigned char uart_is_prime(unsigned int n);

/**
 *	@brief	This function serves to set pin for UART fuction.
 *	@param  tx_pin - To set TX pin.
 *	@param  rx_pin - To set RX pin.
 *	@return	none
 */
static void uart_set_fuc_pin(uart_tx_pin_e tx_pin, uart_rx_pin_e rx_pin);

/**********************************************************************************************************************
 *                                         global function implementation                                             *
 *********************************************************************************************************************/

/**
 * @brief      This function initializes the UART module.
 * @param[in]  uart_num    - UART0 or UART1.
 * @param[in]  div         - uart clock divider.
 * @param[in]  bwpc        - bitwidth, should be set to larger than 2.
 * @param[in]  parity      - selected parity type for UART interface.
 * @param[in]  stop_bit    - selected length of stop bit for UART interface.
 * @return     none
 * @note 	   sys_clk      baudrate   g_uart_div         g_bwpc
 *
 *  	       16Mhz        9600          118   			 13
 *                          19200         118     			  6
 *          	            115200          9       		 13
 *
 * 	           24Mhz        9600          249       		  9
 *           	 	    	19200		  124                 9
 *          	 	    	115200         12    			 15
 *
 *   	       32Mhz        9600          235       		 13
 *          	 	        19200		  235                 6
 *           	 	 	    115200         17    			 13
 *
 *   	       48Mhz        9600          499       		  9
 *          	 	 	    19200		  249                 9
 *           	 	 	    115200         25    			 15
 */
void uart_init(uart_num_e uart_num, unsigned short div, unsigned char bwpc, uart_parity_e parity, uart_stop_bit_e stop_bit)
{
    reg_uart_ctrl0(uart_num) &= ~(FLD_UART_BPWC_O);
    reg_uart_ctrl0(uart_num) |= bwpc;                         // set bwpc
    reg_uart_clk_div(uart_num) = (div | FLD_UART_CLK_DIV_EN); // set div_clock

    // parity config
    if (parity)
    {
        reg_uart_ctrl1(uart_num) |= FLD_UART_PARITY_ENABLE; // enable parity function
        if (UART_PARITY_EVEN == parity)
        {
            reg_uart_ctrl1(uart_num) &= (~FLD_UART_PARITY_POLARITY); // enable even parity
        }
        else if (UART_PARITY_ODD == parity)
        {
            reg_uart_ctrl1(uart_num) |= FLD_UART_PARITY_POLARITY; // enable odd parity
        }
    }
    else
    {
        reg_uart_ctrl1(uart_num) &= (~FLD_UART_PARITY_ENABLE); // disable parity function
    }

    // stop bit config
    reg_uart_ctrl1(uart_num) &= (~FLD_UART_STOP_SEL);
    reg_uart_ctrl1(uart_num) |= stop_bit;
}

/***********************************************************
 * @brief  		This function serves to calculate the best bwpc(bit width) .i.e reg0x96.
 * @param[in]	baudrate - baut rate of UART.
 * @param[in]	pclk   - system clock.
 * @param[out]	div      - uart clock divider.
 * @param[out]	bwpc     - bitwidth, should be set to larger than 2.
 * @return 		none
 * @note        BaudRate*(div+1)*(bwpc+1) = system clock
 *  		    simplify the expression: div*bwpc =  constant(z)
 * 		        bwpc range from 3 to 15.so loop and get the minimum one decimal point
 */
void uart_cal_div_and_bwpc(unsigned int baudrate, unsigned int pclk, unsigned short * div, unsigned char * bwpc)
{
    unsigned char i = 0, j = 0;
    unsigned int primeInt  = 0;
    unsigned char primeDec = 0;
    unsigned int D_intdec[13], D_int[13];
    unsigned char D_dec[13];

    primeInt = pclk / baudrate;
    primeDec = 10 * pclk / baudrate - 10 * primeInt;

    if (uart_is_prime(primeInt))
    {                  // primeInt is prime
        primeInt += 1; //+1 must be not prime. and primeInt must be larger than 2.
    }
    else
    {
        if (primeDec > 5)
        { // >5
            primeInt += 1;
            if (uart_is_prime(primeInt))
            {
                primeInt -= 1;
            }
        }
    }

    for (i = 3; i <= 15; i++)
    {
        D_intdec[i - 3] = (10 * primeInt) / (i + 1);                     ////get the LSB
        D_dec[i - 3]    = D_intdec[i - 3] - 10 * (D_intdec[i - 3] / 10); /// get the decimal section
        D_int[i - 3]    = D_intdec[i - 3] / 10;                          /// get the integer section
    }

    // find the max and min one decimation point
    unsigned char position_min = 0, position_max = 0;
    unsigned int min = 0xffffffff, max = 0x00;
    for (j = 0; j < 13; j++)
    {
        if ((D_dec[j] <= min) && (D_int[j] != 0x01))
        {
            min          = D_dec[j];
            position_min = j;
        }
        if (D_dec[j] >= max)
        {
            max          = D_dec[j];
            position_max = j;
        }
    }

    if ((D_dec[position_min] < 5) && (D_dec[position_max] >= 5))
    {
        if (D_dec[position_min] < (10 - D_dec[position_max]))
        {
            *bwpc = position_min + 3;
            *div  = D_int[position_min] - 1;
        }
        else
        {
            *bwpc = position_max + 3;
            *div  = D_int[position_max];
        }
    }
    else if ((D_dec[position_min] < 5) && (D_dec[position_max] < 5))
    {
        *bwpc = position_min + 3;
        *div  = D_int[position_min] - 1;
    }
    else
    {
        *bwpc = position_max + 3;
        *div  = D_int[position_max];
    }
}

/**
 * @brief  		This funtion serves to set r_rxtimeout. this setting is transfer one bytes need cycles base on uart_clk.
 * 				For example, if  transfer one bytes (1start bit+8bits data+1 priority bit+2stop bits) total 12 bits,
 * 				this register setting should be (bpwc+1)*12.
 * @param[in]	uart_num - UART0 or UART1.
 * @param[in]	bwpc     - bitwidth, should be set to larger than 2.
 * @param[in]	bit_cnt  - bit number.
 * @param[in]	mul	     - mul.
 * @return 		none
 */
void uart_set_dma_rx_timeout(uart_num_e uart_num, unsigned char bwpc, unsigned char bit_cnt, uart_timeout_mul_e mul)
{
    reg_uart_rx_timeout0(uart_num) = (bwpc + 1) * bit_cnt; // one byte includes 12 bits at most
    reg_uart_rx_timeout1(uart_num) &= (~FLD_UART_TIMEOUT_MUL);
    reg_uart_rx_timeout1(uart_num) |= mul; // if over 2*(tmp_bwpc+1),one transaction end.
}

unsigned char uart_tx_byte_index[2] = { 0 };
/**
 * @brief     This function serves to send data by byte with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] tx_data  - the data to be send.
 * @return    none
 */
void uart_send_byte(uart_num_e uart_num, unsigned char tx_data)
{
    while (uart_get_txfifo_num(uart_num) > 7)
        ;

    reg_uart_data_buf(uart_num, uart_tx_byte_index[uart_num]) = tx_data;
    uart_tx_byte_index[uart_num]++;
    (uart_tx_byte_index[uart_num]) &= 0x03;
}

unsigned char uart_rx_byte_index[2] = { 0 };
/**
 * @brief     This function serves to receive uart data by byte with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @return    none
 */
unsigned char uart_read_byte(uart_num_e uart_num)
{
    unsigned char rx_data = reg_uart_data_buf(uart_num, uart_rx_byte_index[uart_num]);
    uart_rx_byte_index[uart_num]++;
    uart_rx_byte_index[uart_num] &= 0x03;
    return rx_data;
}

/**
 * @brief     This function serves to judge if the transmission of uart is done.
 * @param[in] uart_num - UART0 or UART1.
 * @return    return the tx status.
 * -          0:tx is done     1:tx isn't done
 */
unsigned char uart_tx_is_busy(uart_num_e uart_num)
{
    return ((reg_uart_status2(uart_num) & FLD_UART_TX_DONE) ? 0 : 1);
}

/**
 * @brief     This function serves to send uart0 data by halfword with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] data  - the data to be send.
 * @return    none
 */
void uart_send_hword(uart_num_e uart_num, unsigned short data)
{
    static unsigned char uart_tx_hword_index[2] = { 0 };

    while (uart_get_txfifo_num(uart_num) > 6)
        ;

    reg_uart_data_hword_buf(uart_num, uart_tx_hword_index[uart_num]) = data;
    uart_tx_hword_index[uart_num]++;
    uart_tx_hword_index[uart_num] &= 0x01;
}

/**
 * @brief     This function serves to send data by word with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] data - the data to be send.
 * @return    none
 */
void uart_send_word(uart_num_e uart_num, unsigned int data)
{
    while (uart_get_txfifo_num(uart_num) > 4)
        ;
    reg_uart_data_word_buf(uart_num) = data;
}

/**
 * @brief     This function serves to set the RTS pin's level manually.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] polarity - set the output of RTS pin(only for manual mode).
 * @return    none
 */
void uart_set_rts_level(uart_num_e uart_num, unsigned char polarity)
{
    if (polarity)
    {
        reg_uart_ctrl2(uart_num) |= FLD_UART_RTS_MANUAL_V;
    }
    else
    {
        reg_uart_ctrl2(uart_num) &= (~FLD_UART_RTS_MANUAL_V);
    }
}

/**
 *	@brief		This function serves to set pin for UART0 cts function .
 *	@param[in]  cts_pin -To set cts pin.
 *	@return		none
 */
void uart_set_cts_pin(uart_cts_pin_e cts_pin)
{
    unsigned char val  = 0;
    unsigned char mask = 0xff;
    if (cts_pin == UART0_CTS_PA1)
    {
        mask = (unsigned char) ~(BIT(2) | BIT(3));
        val  = BIT(2);
    }
    else if (cts_pin == UART0_CTS_PB6)
    {
        mask = (unsigned char) ~(BIT(4) | BIT(5));
        val  = BIT(5);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (cts_pin == UART0_CTS_PD0)
    {
        mask = (unsigned char) ~(BIT(0) | BIT(1));
        val  = 0;
    }
    else if (cts_pin == UART1_CTS_PC4)
    {
        mask = (unsigned char) ~(BIT(0) | BIT(1));
        val  = BIT(1);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (cts_pin == UART1_CTS_PD4)
    {
        mask = (unsigned char) ~(BIT(0) | BIT(1));
        val  = 0;
    }
    else if (cts_pin == UART1_CTS_PE1)
    {
        mask = (unsigned char) ~(BIT(2) | BIT(3));
        val  = BIT(2);
    }
    reg_gpio_func_mux(cts_pin) = (reg_gpio_func_mux(cts_pin) & mask) | val;
    gpio_function_dis((gpio_pin_e) cts_pin);
}

/**
 *	@brief		This function serves to set pin for UART0 rts function .
 *	@param[in]  rts_pin - To set rts pin.
 *	@return		none
 */
void uart_set_rts_pin(uart_rts_pin_e rts_pin)
{
    unsigned char val  = 0;
    unsigned char mask = 0xff;
    if (rts_pin == UART0_RTS_PA2)
    {
        mask = (unsigned char) ~(BIT(4) | BIT(5));
        val  = BIT(4);
    }
    else if (rts_pin == UART0_RTS_PB4)
    {
        mask = (unsigned char) ~(BIT(0) | BIT(1));
        val  = BIT(1);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (rts_pin == UART0_RTS_PD1)
    {
        mask = (unsigned char) ~(BIT(2) | BIT(3));
        val  = 0;
    }
    else if (rts_pin == UART1_RTS_PC5)
    {
        mask = (unsigned char) ~(BIT(2) | BIT(3));
        val  = BIT(3);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (rts_pin == UART1_RTS_PD5)
    {
        mask = (unsigned char) ~(BIT(2) | BIT(3));
        val  = 0;
    }
    else if (rts_pin == UART1_RTS_PE3)
    {
        mask = (unsigned char) ~(BIT(6) | BIT(7));
        val  = BIT(6);
    }
    reg_gpio_func_mux(rts_pin) = (reg_gpio_func_mux(rts_pin) & mask) | val;
    gpio_function_dis((gpio_pin_e) rts_pin);
}

/**
 * @brief      This function serves to select pin for UART module.
 * @param[in]  tx_pin  - the pin to send data.
 * @param[in]  rx_pin  - the pin to receive data.
 * @return     none
 */
void uart_set_pin(uart_tx_pin_e tx_pin, uart_rx_pin_e rx_pin)
{
    gpio_set_up_down_res((gpio_pin_e) tx_pin, GPIO_PIN_PULLUP_10K);
    gpio_set_up_down_res((gpio_pin_e) rx_pin, GPIO_PIN_PULLUP_10K);
    uart_set_fuc_pin(tx_pin, rx_pin); // set tx and rx pin
    gpio_input_en((gpio_pin_e) tx_pin);
    gpio_input_en((gpio_pin_e) rx_pin);
}

/**
 * @brief      This function serves to set rtx pin for UART module.
 * @param[in]  rx_pin  - the rtx pin need to set.
 * @return     none
 */
void uart_set_rtx_pin(uart_rx_pin_e rx_pin)
{
    unsigned char val  = 0;
    unsigned char mask = 0xff;
    gpio_set_up_down_res((gpio_pin_e) rx_pin, GPIO_PIN_PULLUP_10K);
    if (rx_pin == UART0_RX_PA4)
    {
        mask = (unsigned char) ~(BIT(1) | BIT(0));
        val  = BIT(0);
    }
    else if (rx_pin == UART0_RX_PB3)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = BIT(7);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (rx_pin == UART0_RX_PD3)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = 0;
    }
    else if (rx_pin == UART1_RX_PC7)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = BIT(7);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (rx_pin == UART1_RX_PD7)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = 0;
    }
    else if (rx_pin == UART1_RX_PE2)
    {
        mask = (unsigned char) ~(BIT(5) | BIT(4));
        val  = BIT(4);
    }
    reg_gpio_func_mux(rx_pin) = (reg_gpio_func_mux(rx_pin) & mask) | val;
    gpio_input_en((gpio_pin_e) rx_pin);
    gpio_function_dis((gpio_pin_e) rx_pin);
}

/**
 * @brief     This function serves to send data with not DMA method.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] addr     - pointer to the buffer containing data need to send.
 * @param[in] len      - NDMA transmission length.
 * @return    1
 */
unsigned char uart_send(uart_num_e uart_num, unsigned char * addr, unsigned char len)
{
    for (unsigned char i = 0; i < len; i++)
    {
        uart_send_byte(uart_num, addr[i]);
    }
    return 1;
}

/**
 * @brief     	This function serves to send data by DMA, this function tell the DMA to get data from the RAM and start.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in] 	addr     - pointer to the buffer containing data need to send.
 * @param[in] 	len      - DMA transmission length.The maximum transmission length of DMA is 0xFFFFFC bytes, so dont'n over this
 * length.
 * @return      1  dma start send.
 *              0  the length is error.
 */
unsigned char uart_send_dma(uart_num_e uart_num, unsigned char * addr, unsigned int len)
{
    if (len != 0)
    {
        uart_clr_tx_done(uart_num);
        dma_set_address(uart_dma_tx_chn[uart_num], (unsigned int) convert_ram_addr_cpu2bus(addr), reg_uart_data_buf_adr(uart_num));
        dma_set_size(uart_dma_tx_chn[uart_num], len, DMA_WORD_WIDTH);
        dma_chn_en(uart_dma_tx_chn[uart_num]);
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief     	This function serves to receive data function by DMA, this  function tell the DMA to get data from the uart data
 * fifo.
 * @param[in]  	uart_num - UART0 or UART1.
 * @param[in] 	addr     - pointer to the buffer  receive data.
 * @param[in]   rev_size - the receive length of DMA,The maximum transmission length of DMA is 0xFFFFFC bytes, so dont'n over this
 * length.
 * @note        The DMA version of A0 has some limitians.
 *              0:We should know the real receive length-len.
 *              1:If the data length we receive isn't the multiple of 4(the DMA carry 4-byte one time),like 5,it will carry 8 byte,
 *                while the last 3-byte data is random.
 *              2:The receive buff length sholud be equal to rec_size.The relation of the receive buff length and rec_size and
 *                the real receive data length-len : REC_BUFF_LEN=rec_size= ((len%4)==0 ? len : ((len/4)+1)*4).
 *              The DMA version of A1 can receive any length of data,the rev_size is useless.
 * @return    	none
 */
void uart_receive_dma(uart_num_e uart_num, unsigned char * addr, unsigned int rev_size)
{
    dma_chn_dis(uart_dma_rx_chn[uart_num]);
    /*In order to be able to receive data of unknown length(A0 doesn't suppport),the DMA SIZE is set to the longest value
    0xffffffff.After entering suspend and wake up, and then continue to receive, DMA will no longer move data from uart fifo,
    because DMA thinks that the last transmission was not completed and must disable dma_chn first.modified by minghai,confirmed
    qiangkai 2020.11.26.*/
    dma_set_address(uart_dma_rx_chn[uart_num], reg_uart_data_buf_adr(uart_num), (unsigned int) convert_ram_addr_cpu2bus(addr));
    if (0xff == g_chip_version)
    {
        dma_set_size(uart_dma_rx_chn[uart_num], rev_size, DMA_WORD_WIDTH);
    }
    else
    {
        reg_dma_size(uart_dma_rx_chn[uart_num]) = 0xffffffff;
    }

    dma_chn_en(uart_dma_rx_chn[uart_num]);
}

/**
 * @brief     This function serves to set uart tx_dam channel and config dma tx default.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] chn      - dma channel.
 * @return    none
 */
void uart_set_tx_dma_config(uart_num_e uart_num, dma_chn_e chn)
{
    uart_dma_tx_chn[uart_num] = chn;
    dma_config(chn, &uart_tx_dma_config[uart_num]);
}

/**
 * @brief     This function serves to set uart rx_dam channel and config dma rx default.
 * @param[in] uart_num - UART0 or UART1.
 * @param[in] chn      - dma channel.
 * @return    none
 */
void uart_set_rx_dma_config(uart_num_e uart_num, dma_chn_e chn)
{
    uart_dma_rx_chn[uart_num] = chn;
    dma_config(chn, &uart_rx_dma_config[uart_num]);
}

/**
 * @brief     UART hardware flow control configuration. Configure CTS.
 * @param[in] uart_num   - UART0 or UART1.
 * @param[in] cts_pin    - RTS pin select.
 * @param[in] cts_parity - when CTS's input equals to select, tx will be stopped.
 * @return    none
 */
void uart_cts_config(uart_num_e uart_num, uart_cts_pin_e cts_pin, unsigned char cts_parity)
{
    uart_set_cts_pin(cts_pin);

    gpio_input_en((gpio_pin_e) cts_pin); // enable input

    if (cts_parity)
    {
        reg_uart_ctrl1(uart_num) |= FLD_UART_TX_CTS_POLARITY;
    }
    else
    {
        reg_uart_ctrl1(uart_num) &= (~FLD_UART_TX_CTS_POLARITY);
    }
}

/**
 * @brief     UART hardware flow control configuration. Configure RTS.
 * @param[in] uart_num     - UART0 or UART1.
 * @param[in] rts_pin      - RTS pin select.
 * @param[in] rts_parity   - whether invert the output of RTS pin(only for auto mode)
 * @param[in] auto_mode_en - set the mode of RTS(auto or manual).
 * @return    none
 */
void uart_rts_config(uart_num_e uart_num, uart_rts_pin_e rts_pin, unsigned char rts_parity, unsigned char auto_mode_en)
{
    uart_set_rts_pin(rts_pin);

    if (auto_mode_en)
    {
        reg_uart_ctrl2(uart_num) |= FLD_UART_RTS_MANUAL_M;
    }
    else
    {
        reg_uart_ctrl2(uart_num) &= (~FLD_UART_RTS_MANUAL_M);
    }

    if (rts_parity)
    {
        reg_uart_ctrl2(uart_num) |= FLD_UART_RTS_POLARITY;
    }
    else
    {
        reg_uart_ctrl2(uart_num) &= (~FLD_UART_RTS_POLARITY);
    }
}

/**********************************************************************************************************************
 *                    						local function implementation *
 *********************************************************************************************************************/
/**
 * @brief     This function is used to look for the prime.if the prime is finded,it will return 1, or return 0.
 * @param[in] n - the calue to judge.
 * @return    0 or 1
 */
static unsigned char uart_is_prime(unsigned int n)
{
    unsigned int i = 5;
    if (n <= 3)
    {
        return 1; // althought n is prime, the bwpc must be larger than 2.
    }
    else if ((n % 2 == 0) || (n % 3 == 0))
    {
        return 0;
    }
    else
    {
        for (i = 5; i * i < n; i += 6)
        {
            if ((n % i == 0) || (n % (i + 2)) == 0)
            {
                return 0;
            }
        }
        return 1;
    }
}

/**
 *	@brief	This function serves to set pin for UART fuction.
 *	@param  tx_pin - To set TX pin.
 *	@param  rx_pin - To set RX pin.
 *	@return	none
 */
static void uart_set_fuc_pin(uart_tx_pin_e tx_pin, uart_rx_pin_e rx_pin)
{
    unsigned char val  = 0;
    unsigned char mask = 0xff;

    if (tx_pin == UART0_TX_PA3)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = BIT(6);
    }
    else if (tx_pin == UART0_TX_PB2)
    {
        mask = (unsigned char) ~(BIT(5) | BIT(4));
        val  = BIT(5);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (tx_pin == UART0_TX_PD2)
    {
        mask = (unsigned char) ~(BIT(5) | BIT(4));
        val  = 0;
    }
    else if (tx_pin == UART1_TX_PC6)
    {
        mask = (unsigned char) ~(BIT(5) | BIT(4));
        val  = BIT(5);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (tx_pin == UART1_TX_PD6)
    {
        mask = (unsigned char) ~(BIT(5) | BIT(4));
        val  = 0;
    }
    else if (tx_pin == UART1_TX_PE0)
    {
        mask = (unsigned char) ~(BIT(1) | BIT(0));
        ;
        val = BIT(0);
    }
    reg_gpio_func_mux(tx_pin) = (reg_gpio_func_mux(tx_pin) & mask) | val;

    if (rx_pin == UART0_RX_PA4)
    {
        mask = (unsigned char) ~(BIT(1) | BIT(0));
        val  = BIT(0);
    }
    else if (rx_pin == UART0_RX_PB3)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = BIT(7);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (rx_pin == UART0_RX_PD3)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = 0;
    }
    else if (rx_pin == UART1_RX_PC7)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = BIT(7);
        reg_gpio_pad_mul_sel |= BIT(0);
    }
    else if (rx_pin == UART1_RX_PD7)
    {
        mask = (unsigned char) ~(BIT(7) | BIT(6));
        val  = 0;
    }
    else if (rx_pin == UART1_RX_PE2)
    {
        mask = (unsigned char) ~(BIT(5) | BIT(4));
        val  = BIT(4);
    }
    // note:  setting pad the function  must before  setting no_gpio function, cause it will lead to uart transmit extra one byte
    // data at begin.(confirmed by minghai&sunpeng)
    reg_gpio_func_mux(rx_pin) = (reg_gpio_func_mux(rx_pin) & mask) | val;

    gpio_function_dis((gpio_pin_e) tx_pin);
    gpio_function_dis((gpio_pin_e) rx_pin);
}
