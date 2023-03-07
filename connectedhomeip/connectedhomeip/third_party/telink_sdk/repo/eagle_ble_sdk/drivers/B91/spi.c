/********************************************************************************************************
 * @file	spi.c
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
#include "spi.h"

#include "timer.h"
#include "compiler.h"
static unsigned char s_hspi_tx_dma_chn;
static unsigned char s_hspi_rx_dma_chn;
static unsigned char s_pspi_tx_dma_chn;
static unsigned char s_pspi_rx_dma_chn;

dma_config_t hspi_tx_dma_config = {
    .dst_req_sel    = DMA_REQ_SPI_AHB_TX, // tx req
    .src_req_sel    = 0,
    .dst_addr_ctrl  = DMA_ADDR_FIX,
    .src_addr_ctrl  = DMA_ADDR_INCREMENT, // increment
    .dstmode        = DMA_HANDSHAKE_MODE, // handshake
    .srcmode        = DMA_NORMAL_MODE,
    .dstwidth       = DMA_CTR_WORD_WIDTH, // must word
    .srcwidth       = DMA_CTR_WORD_WIDTH, // must word
    .src_burst_size = 0,                  // must 0
    .read_num_en    = 0,
    .priority       = 0,
    .write_num_en   = 0,
    .auto_en        = 0, // must 0
};
dma_config_t hspi_rx_dma_config = {
    .dst_req_sel    = 0, // tx req
    .src_req_sel    = DMA_REQ_SPI_AHB_RX,
    .dst_addr_ctrl  = DMA_ADDR_INCREMENT,
    .src_addr_ctrl  = DMA_ADDR_FIX,
    .dstmode        = DMA_NORMAL_MODE,
    .srcmode        = DMA_HANDSHAKE_MODE,
    .dstwidth       = DMA_CTR_WORD_WIDTH, // must word
    .srcwidth       = DMA_CTR_WORD_WIDTH, ////must word
    .src_burst_size = 0,
    .read_num_en    = 0,
    .priority       = 0,
    .write_num_en   = 0,
    .auto_en        = 0, // must 0
};

dma_config_t pspi_tx_dma_config = {
    .dst_req_sel    = DMA_REQ_SPI_APB_TX, // tx req
    .src_req_sel    = 0,
    .dst_addr_ctrl  = DMA_ADDR_FIX,
    .src_addr_ctrl  = DMA_ADDR_INCREMENT, // increment
    .dstmode        = DMA_HANDSHAKE_MODE, // handshake
    .srcmode        = DMA_NORMAL_MODE,
    .dstwidth       = DMA_CTR_WORD_WIDTH, // must word
    .srcwidth       = DMA_CTR_WORD_WIDTH, // must word
    .src_burst_size = 0,                  // must 0
    .read_num_en    = 0,
    .priority       = 0,
    .write_num_en   = 0,
    .auto_en        = 0, // must 0
};

dma_config_t pspi_rx_dma_config = {
    .dst_req_sel    = 0, // tx req
    .src_req_sel    = DMA_REQ_SPI_APB_RX,
    .dst_addr_ctrl  = DMA_ADDR_INCREMENT,
    .src_addr_ctrl  = DMA_ADDR_FIX,
    .dstmode        = DMA_NORMAL_MODE,
    .srcmode        = DMA_HANDSHAKE_MODE,
    .dstwidth       = DMA_CTR_WORD_WIDTH, // must word
    .srcwidth       = DMA_CTR_WORD_WIDTH, ////must word
    .src_burst_size = 0,
    .read_num_en    = 0,
    .priority       = 0,
    .write_num_en   = 0,
    .auto_en        = 0, // must 0
};

/**
 * @brief      This function selects  pin  for hspi master or slave mode.
 * @param[in]  pin  - the selected pin.
 * @return     none
 */
void hspi_set_pin_mux(hspi_pin_def_e pin)
{
    if (pin != HSPI_NONE_PIN)
    {
        unsigned char val       = 0;
        unsigned char start_bit = (BIT_LOW_BIT(pin & 0xff) % 4) << 1;
        unsigned char mask      = (unsigned char) ~BIT_RNG(start_bit, start_bit + 1);

        if ((pin == HSPI_CLK_PB4_PIN) || (pin == HSPI_CSN_PB6_PIN) || (pin == HSPI_MOSI_IO0_PB3_PIN) ||
            (pin == HSPI_MISO_IO1_PB2_PIN) || (pin == HSPI_WP_IO2_PB1_PIN) || (pin == HSPI_HOLD_IO3_PB0_PIN))
        {
            val = 0; // function 0
        }
        else if ((pin == HSPI_CLK_PA2_PIN) || (pin == HSPI_CSN_PA1_PIN) || (pin == HSPI_MOSI_IO0_PA4_PIN) ||
                 (pin == HSPI_MISO_IO1_PA3_PIN))
        {
            val = 2 << (start_bit); // function 2
            reg_gpio_pad_mul_sel |= BIT(1);
        }
        reg_gpio_func_mux(pin) = (reg_gpio_func_mux(pin) & mask) | val;
        gpio_function_dis((gpio_pin_e) pin);
        gpio_input_en((gpio_pin_e) pin);
    }
}

/**
 * @brief     	This function enable hspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void hspi_cs_pin_en(hspi_csn_pin_def_e pin)
{
    hspi_set_pin_mux((hspi_pin_def_e) pin);
}

/**
 * @brief     	This function disable hspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void hspi_cs_pin_dis(hspi_csn_pin_def_e pin)
{
    gpio_function_en((gpio_pin_e) pin);
    gpio_set_high_level((gpio_pin_e) pin);
}

/**
 * @brief     	This function change hspi csn pin.
 * @param[in] 	next_csn_pin - the next csn pin.
 * @return 		next_csn_pin - the next csn pin.
 */
hspi_csn_pin_def_e hspi_change_csn_pin(hspi_csn_pin_def_e next_csn_pin)
{
    if (next_csn_pin == HSPI_CSN_PB6)
    {
        hspi_cs_pin_dis(HSPI_CSN_PA1);
        hspi_cs_pin_en(HSPI_CSN_PB6);
    }
    else if (next_csn_pin == HSPI_CSN_PA1)
    {
        hspi_cs_pin_dis(HSPI_CSN_PB6);
        hspi_cs_pin_en(HSPI_CSN_PA1);
    }
    return next_csn_pin;
}

/**
 * @brief      This function selects  pin  for pspi master or slave mode.
 * @param[in]  pin  - the selected pin.
 * @return     none
 */
void pspi_set_pin_mux(pspi_pin_def_e pin)
{
    if (pin != PSPI_NONE_PIN)
    {
        unsigned char val       = 0;
        unsigned char start_bit = (BIT_LOW_BIT(pin & 0xff) % 4) << 1;
        unsigned char mask      = (unsigned char) ~BIT_RNG(start_bit, start_bit + 1);
        if ((pin == PSPI_CLK_PC5_PIN) || (pin == PSPI_CSN_PC4_PIN) || (pin == PSPI_MOSI_IO0_PC7_PIN) ||
            (pin == PSPI_MISO_IO1_PC6_PIN))
        {
            val = 0; // function 0
        }

        else if ((pin == PSPI_CLK_PB5_PIN) || (pin == PSPI_CLK_PD1_PIN) || (pin == PSPI_CSN_PC0_PIN) || (pin == PSPI_CSN_PD0_PIN) ||
                 (pin == PSPI_MOSI_IO0_PB7_PIN) || (pin == PSPI_MOSI_IO0_PD3_PIN) || (pin == PSPI_MISO_IO1_PB6_PIN) ||
                 (pin == PSPI_MISO_IO1_PD2_PIN))
        {
            val = 1 << (start_bit); // function 1
        }

        reg_gpio_func_mux(pin) = (reg_gpio_func_mux(pin) & mask) | val;
        gpio_function_dis((gpio_pin_e) pin);
        gpio_input_en((gpio_pin_e) pin);
    }
}
/**
 * @brief     	This function enable pspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void pspi_cs_pin_en(pspi_csn_pin_def_e pin)
{
    pspi_set_pin_mux((pspi_pin_def_e) pin);
}

/**
 * @brief     	This function disable pspi csn pin.
 * @param[in] 	pin - the csn pin.
 * @return 		none
 */
void pspi_cs_pin_dis(pspi_csn_pin_def_e pin)
{
    gpio_output_en((gpio_pin_e) pin);
    gpio_set_high_level((gpio_pin_e) pin);
    gpio_function_en((gpio_pin_e) pin);
    gpio_input_dis((gpio_pin_e) pin);
}

/**
 * @brief     	This function change pspi csn pin.
 * @param[in] 	next_csn_pin - the next csn pin.
 * @return 		next_csn_pin - the next csn pin.
 */
pspi_csn_pin_def_e pspi_change_csn_pin(pspi_csn_pin_def_e next_csn_pin)
{
    if (next_csn_pin == PSPI_CSN_PC4)
    {
        pspi_cs_pin_dis(PSPI_CSN_PC0);
        pspi_cs_pin_dis(PSPI_CSN_PD0);
        pspi_cs_pin_en(PSPI_CSN_PC4);
    }
    else if (next_csn_pin == PSPI_CSN_PC0)
    {
        pspi_cs_pin_dis(PSPI_CSN_PC4);
        pspi_cs_pin_dis(PSPI_CSN_PD0);
        pspi_cs_pin_en(PSPI_CSN_PC0);
    }
    else if (next_csn_pin == PSPI_CSN_PD0)
    {
        pspi_cs_pin_dis(PSPI_CSN_PC4);
        pspi_cs_pin_dis(PSPI_CSN_PC0);
        pspi_cs_pin_en(PSPI_CSN_PD0);
    }
    return next_csn_pin;
}
/**
 * @brief     This function configures hspi pin.
 * @param[in] config - the pointer of pin config struct.
 * @return    none
 */
void hspi_set_pin(hspi_pin_config_t * config)
{
    hspi_set_pin_mux((hspi_pin_def_e) config->hspi_csn_pin);
    hspi_set_pin_mux((hspi_pin_def_e) config->hspi_clk_pin);
    hspi_set_pin_mux((hspi_pin_def_e) config->hspi_mosi_io0_pin);
    hspi_set_pin_mux((hspi_pin_def_e) config->hspi_miso_io1_pin);
    hspi_set_pin_mux((hspi_pin_def_e) config->hspi_wp_io2_pin);
    hspi_set_pin_mux((hspi_pin_def_e) config->hspi_hold_io3_pin);
}



/**
 * @brief     	This function configures pspi pin.
 * @param[in] 	config - the pointer of pin config struct.
 * @return 		none
 */
void pspi_set_pin(pspi_pin_config_t * config)
{
    pspi_set_pin_mux((pspi_pin_def_e) config->pspi_clk_pin);
    pspi_set_pin_mux((pspi_pin_def_e) config->pspi_csn_pin);
    pspi_set_pin_mux((pspi_pin_def_e) config->pspi_mosi_io0_pin);
    pspi_set_pin_mux((pspi_pin_def_e) config->pspi_miso_io1_pin);
}

/**
 * @brief   This function selects  pin  for hspi master or slave.
 * @return  none
 */
void spi_slave_set_pin(void)
{
    reg_gpio_pa_fuc_l = (reg_gpio_pb_fuc_l & 0x03); // set PA1 as csn,PA2 as clk,PA3 as mosi_io0,
    reg_gpio_pa_fuc_h = (reg_gpio_pb_fuc_l & 0xfc); // set PA4 slave miso_io1
    gpio_function_dis(GPIO_PA1 | GPIO_PA2 | GPIO_PA3 | GPIO_PA4);
    gpio_input_en(GPIO_PA1 | GPIO_PA2 | GPIO_PA3 | GPIO_PA4);
}

/**
 * @brief     	This function configures the clock and working mode for SPI interface.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	div_clock 	- the division factor for SPI module.
 *           	spi_clock_out = ahb_clock / ((div_clock+1)*2)
 * @param[in]	 mode 		- the selected working mode of SPI module.
 *           	bit5:CPHA-Clock Polarity ; bit6:CPOL:CPHA-Clock Phase
 *           	MODE0:  CPHA = 0, CPOL = 0;
 *           	MODE1:  CPHA = 0, CPOL = 1;
 *           	MODE2:  CPHA = 1, CPOL = 0;
 *           	MODE3:  CPHA = 1, CPOL = 1;
 * @return  	none
 */
void spi_master_init(spi_sel_e spi_sel, unsigned char div_clock, spi_mode_type_e mode)
{
    reg_spi_mode1(spi_sel) = div_clock;
    reg_spi_mode0(spi_sel) |= FLD_SPI_MASTER_MODE;       // master
    reg_spi_mode0(spi_sel) &= (~FLD_SPI_MODE_WORK_MODE); // clear spi working mode
    reg_spi_mode0(spi_sel) |= (mode << 5);               // select SPI mode, support four modes
}


/**
 * @brief     	This function configures the clock and working mode for SPI interface.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mode 	- the selected working mode of SPI module.
 *          	bit5:CPHA-Clock Polarity ; bit6:CPOL:CPHA-Clock Phase
 *           	MODE0:  CPHA = 0, CPOL = 0;
 *           	MODE1:  CPHA = 0, CPOL = 1;
 *          	MODE2:  CPHA = 1, CPOL = 0;
 *           	MODE3:  CPHA = 1,  CPOL = 1;
 * @return  	none
 * @note  		spi_clock_in  (spi_slave_clock frequency)/3
 */
void spi_slave_init(spi_sel_e spi_sel, spi_mode_type_e mode)
{
    reg_spi_mode0(spi_sel) &= (~FLD_SPI_MASTER_MODE);    // slave
    reg_spi_mode0(spi_sel) &= (~FLD_SPI_MODE_WORK_MODE); // clear spi working mode
    reg_spi_mode0(spi_sel) |= (mode << 5);               // select SPI mode, support four modes
}

/**
 * @brief     	This function servers to set dummy cycle cnt.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	dummy_cnt 	- the cnt of dummy clock.
 * @return  	none
 */
void spi_set_dummy_cnt(spi_sel_e spi_sel, unsigned char dummy_cnt)
{
    reg_spi_trans0(spi_sel) &= (~FLD_SPI_DUMMY_CNT);
    reg_spi_trans0(spi_sel) |= (dummy_cnt - 1) & FLD_SPI_DUMMY_CNT;
}

/**
 * @brief     	This function servers to set spi transfer mode.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mode 	- transfer mode.
 * @return  	none
 */
void spi_set_transmode(spi_sel_e spi_sel, spi_tans_mode_e mode)
{
    reg_spi_trans0(spi_sel) &= (~FLD_SPI_TRANSMODE);
    reg_spi_trans0(spi_sel) |= (mode & 0xf) << 4;
}

/**
 * @brief     	This function servers to set normal mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	none
 */
void spi_set_normal_mode(spi_sel_e spi_sel)
{
    spi_dual_mode_dis(spi_sel);
    spi_3line_mode_dis(spi_sel);
    if (HSPI_MODULE == spi_sel)
    {
        hspi_quad_mode_dis(spi_sel);
    }
}

/**
 * @brief     	This function servers to set dual mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	none
 */
void spi_set_dual_mode(spi_sel_e spi_sel)
{
    spi_dual_mode_en(spi_sel); // quad  precede over dual
    spi_3line_mode_dis(spi_sel);
    if (HSPI_MODULE == spi_sel)
    {
        hspi_quad_mode_dis(spi_sel);
    }
}

/**
 * @brief	This function servers to set quad mode.
 * @return	none
 */
void hspi_set_quad_mode()
{
    hspi_quad_mode_en();
    spi_dual_mode_dis(HSPI_MODULE);
    spi_3line_mode_dis(HSPI_MODULE);
}

/**
 * @brief     	This function servers to set 3line mode.
 * @param[in] 	spi_sel 	- the spi module.
 * @return  	none
 */
void spi_set_3line_mode(spi_sel_e spi_sel)
{
    /*must disable dual and quad*/
    spi_3line_mode_en(spi_sel);
    spi_dual_mode_dis(spi_sel);
    if (HSPI_MODULE == spi_sel)
    {
        hspi_quad_mode_dis(spi_sel);
    }
}

/**
 * @brief     	This function servers to set hspi io  mode.
 * @param[in] 	spi_sel - the spi module.
 * @param[in]	mode 	- single/dual/quad /3line.
 * @return  	none
 */
void spi_set_io_mode(spi_sel_e spi_sel, spi_io_mode_e mode)
{
    switch (mode)
    {
    case SPI_SINGLE_MODE:
        spi_set_normal_mode(spi_sel);
        break;
    case SPI_DUAL_MODE:
        spi_set_dual_mode(spi_sel);
        break;
    case HSPI_QUAD_MODE:
        hspi_set_quad_mode();
        break;
    case SPI_3_LINE_MODE:
        spi_set_3line_mode(spi_sel);
        break;
    }
}

/**
 * @brief     	This function servers to config normal mode.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	mode 	- nomal ,mode 3line.
 * @return  	none
 */
void spi_master_config(spi_sel_e spi_sel, spi_nomal_3line_mode_e mode)
{
    spi_cmd_dis(spi_sel);
    if (HSPI_MODULE == spi_sel)
    {
        hspi_addr_dis();
    }
    spi_set_io_mode(spi_sel, (spi_io_mode_e) mode);
}

/**
 * @brief     	This function servers to config hspi special mode.
 * @param[in] 	config 	- the pointer of pin special config struct.
 * @return  	none
 */
void hspi_master_config_plus(hspi_config_t * config)
{
    spi_set_io_mode(HSPI_MODULE, (spi_io_mode_e) config->hspi_io_mode);
    hspi_set_addr_len(config->hspi_addr_len);
    spi_set_dummy_cnt(HSPI_MODULE, config->hspi_dummy_cnt);

    if (1 == config->hspi_cmd_en)
    {
        spi_cmd_en(HSPI_MODULE);
    }
    else if (0 == config->hspi_cmd_en)
    {
        spi_cmd_dis(HSPI_MODULE);
    }

    if (1 == config->hspi_cmd_fmt_en)
    {
        hspi_cmd_fmt_en();
    }
    else if (0 == config->hspi_cmd_fmt_en)
    {
        hspi_cmd_fmt_dis();
    }

    if (1 == config->hspi_addr_en)
    {
        hspi_addr_en();
    }
    else if (0 == config->hspi_addr_en)
    {
        hspi_addr_dis();
    }

    if (1 == config->hspi_addr_fmt_en)
    {
        hspi_addr_fmt_en();
    }
    else if (0 == config->hspi_addr_fmt_en)
    {
        hspi_addr_fmt_dis();
    }
}

/**
 * @brief     	This function servers to config pspi special mode.
 * @param[in] 	config 	- the pointer of pin special config struct.
 * @return  	none
 */
void pspi_master_config_plus(pspi_config_t * config)
{
    spi_set_io_mode(PSPI_MODULE, (spi_io_mode_e) config->pspi_io_mode);
    spi_set_dummy_cnt(PSPI_MODULE, config->pspi_dummy_cnt);
    if (1 == config->pspi_cmd_en)
    {
        spi_cmd_en(PSPI_MODULE);
    }
    else if (0 == config->pspi_cmd_en)
    {
        spi_cmd_dis(PSPI_MODULE);
    }
}

/**
 * @brief     	This function servers to set slave address hspi only.
 * @param[in] 	addr - address of slave.
 * @return    	none
 */
void hspi_set_address(unsigned int addr)
{
    reg_hspi_addr_32 = addr;
}

/**
 * @brief     	This function servers to write hspi fifo.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	data 	- the pointer to the data for write.
 * @param[in] 	len 	- write length.
 * @return    	none
 */
void spi_write(spi_sel_e spi_sel, unsigned char * data, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        while (reg_spi_fifo_state(spi_sel) & FLD_SPI_TXF_FULL)
            ;
        reg_spi_wr_rd_data(spi_sel, i % 4) = data[i];
    }
}


/**
 * @brief     	This function servers to read hspi fifo.
 * @param[in] 	spi_sel	- the spi module.
 * @param[in] 	data 	- the pointer to the data for read.
 * @param[in] 	len 	- write length.
 * @return    	none
 */
void spi_read(spi_sel_e spi_sel, unsigned char * data, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        while (reg_spi_fifo_state(spi_sel) & FLD_SPI_RXF_EMPTY)
            ;
        data[i] = reg_spi_wr_rd_data(spi_sel, i % 4);
    }
}

/**
 * @brief     	This function serves to normal write data in normal.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	data 	- the pointer to the data for write.
 * @param[in] 	len 	- write length.
 * @return  	none
 */
void spi_master_write(spi_sel_e spi_sel, unsigned char * data, unsigned int len)
{
    spi_tx_fifo_clr(spi_sel);
    spi_tx_cnt(spi_sel, len);
    spi_set_transmode(spi_sel, SPI_MODE_WRITE_ONLY);
    spi_set_cmd(spi_sel, 0x00); // when  cmd  disable that  will not sent cmd,just trigger spi send .
    spi_write(spi_sel, (unsigned char *) data, len);
    while (spi_is_busy(spi_sel))
        ;
}

/**
 * @brief     	This function serves to normal write and read data.
 * @param[in] 	spi_sel - the spi module.
 * @param[in] 	wr_data - the pointer to the data for write.
 * @param[in] 	wr_len 	- write length.
 * @param[in] 	rd_data - the pointer to the data for read.
 * @param[in] 	rd_len 	- read length.
 * @return  	none
 */
void spi_master_write_read(spi_sel_e spi_sel, unsigned char * wr_data, unsigned int wr_len, unsigned char * rd_data,
                           unsigned int rd_len)
{
    spi_tx_fifo_clr(spi_sel);
    spi_rx_fifo_clr(spi_sel);
    spi_tx_cnt(spi_sel, wr_len);
    spi_rx_cnt(spi_sel, rd_len);
    spi_set_transmode(spi_sel, SPI_MODE_WRITE_READ);
    spi_set_cmd(spi_sel, 0x00); // when  cmd  disable that  will not sent cmd,just trigger spi send .
    spi_write(spi_sel, (unsigned char *) wr_data, wr_len);
    spi_read(spi_sel, (unsigned char *) rd_data, rd_len);
    while (spi_is_busy(spi_sel))
        ;
}

/**
 * @brief     	This function serves to single/dual/quad write to the SPI slave.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	cmd 		- cmd one byte will first write.
 * @param[in] 	addr 		- the address of slave.
 * @param[in] 	data 		-  pointer to the data need to write.
 * @param[in] 	data_len 	- length in byte of the data need to write.
 * @param[in] 	wr_mode 	- write mode.dummy or not dummy.
 * @return  	none
 */
void spi_master_write_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char * data, unsigned int data_len,
                           spi_wr_tans_mode_e wr_mode)
{
    spi_tx_fifo_clr(spi_sel);
    if (HSPI_MODULE == spi_sel)
    {
        hspi_set_address(addr);
    }
    spi_set_transmode(spi_sel, (spi_tans_mode_e) wr_mode);

    spi_tx_cnt(spi_sel, data_len);
    spi_set_cmd(spi_sel, cmd);
    spi_write(spi_sel, (unsigned char *) data, data_len);
    while (spi_is_busy(spi_sel))
        ;
}

/**
 * @brief     	This function serves to single/dual/quad  read from the SPI slave.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	data 		- pointer to the data need to read.
 * @param[in]  	data_len 	- the length of data.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_read_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char * data, unsigned int data_len,
                          spi_rd_tans_mode_e rd_mode)
{
    spi_rx_fifo_clr(spi_sel);
    if (HSPI_MODULE == spi_sel)
    {
        hspi_set_address(addr);
    }
    spi_set_transmode(spi_sel, (spi_tans_mode_e) rd_mode);
    spi_rx_cnt(spi_sel, data_len);
    spi_set_cmd(spi_sel, cmd);
    spi_read(spi_sel, (unsigned char *) data, data_len);
    while (spi_is_busy(spi_sel))
        ;
}

/**
 * @brief      	This function serves to write address, then read data from the SPI slave.
 * @param[in]  	spi_sel	 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addrs 		- pointer to the address of slave.
 * @param[in]  	addr_len 	- the length of address.
 * @param[in]  	data 		- the pointer to the data for read.
 * @param[in]  	data_len 	- read length.
 * @param[in] 	wr_mode 	- write mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_write_read_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned char * addrs, unsigned int addr_len,
                                unsigned char * data, unsigned int data_len, spi_rd_tans_mode_e wr_mode)
{
    spi_tx_fifo_clr(spi_sel);
    spi_rx_fifo_clr(spi_sel);
    spi_tx_cnt(spi_sel, addr_len);
    spi_rx_cnt(spi_sel, data_len);
    spi_set_transmode(spi_sel, (spi_tans_mode_e) wr_mode);

    spi_set_cmd(spi_sel, cmd);
    spi_write(spi_sel, (unsigned char *) addrs, addr_len);
    spi_read(spi_sel, (unsigned char *) data, data_len);
    while (spi_is_busy(spi_sel))
        ;
}

/**
 * @brief     	This function serves to set tx_dam channel and config dma tx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 */
void hspi_set_tx_dma_config(dma_chn_e chn)
{
    s_hspi_tx_dma_chn = chn;
    dma_config(chn, &hspi_tx_dma_config);
}

/**
 * @brief     	This function serves to set rx_dam channel and config dma rx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 */
void hspi_set_rx_dma_config(dma_chn_e chn)
{
    s_hspi_rx_dma_chn = chn;
    dma_config(chn, &hspi_rx_dma_config);
}

/**
 * @brief     	This function serves to set tx_dam channel and config dma tx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 */
void pspi_set_tx_dma_config(dma_chn_e chn)
{
    s_pspi_tx_dma_chn = chn;
    dma_config(chn, &pspi_tx_dma_config);
}

/**
 * @brief     	This function serves to set rx_dam channel and config dma rx default.
 * @param[in] 	chn 	- dma channel.
 * @return  	none
 */
void pspi_set_rx_dma_config(dma_chn_e chn)
{
    s_pspi_rx_dma_chn = chn;
    dma_config(chn, &pspi_rx_dma_config);
}

/**
 * @brief   	this  function set spi dma channel.
 * @param[in]  	spi_dma_chn - dma channel.
 * @param[in]  	src_addr 	- the address of source.
 * @param[in]  	dst_addr 	- the address of destination.
 * @param[in]  	len 		- the length of data.
 * */
void spi_set_dma(dma_chn_e spi_dma_chn, unsigned int src_addr, unsigned int dst_addr, unsigned int len)
{
    dma_set_address(spi_dma_chn, src_addr, dst_addr);
    dma_set_size(spi_dma_chn, len, DMA_WORD_WIDTH);
    dma_chn_en(spi_dma_chn);
}

/**
 * @brief   	this  function set spi tx dma channel.
 * @param[in]  	spi_sel     - the spi module.
 * @param[in]  	src_addr 	- the address of source.
 * @param[in]  	len 		- the length of data.
 * */
void spi_set_tx_dma(spi_sel_e spi_sel, unsigned char * src_addr, unsigned int len)
{
    unsigned char tx_dma_chn;
    if (HSPI_MODULE == spi_sel)
    {
        tx_dma_chn = s_hspi_tx_dma_chn;
    }
    else
    {
        tx_dma_chn = s_pspi_tx_dma_chn;
    }
    spi_rx_tx_irq_trig_cnt(spi_sel, 4); // recover trigger level to 4.
    spi_tx_cnt(spi_sel, len);
    dma_set_address(tx_dma_chn, (unsigned int) convert_ram_addr_cpu2bus(src_addr), reg_spi_data_buf_adr(spi_sel));
    dma_set_size(tx_dma_chn, len, DMA_WORD_WIDTH);
    dma_chn_en(tx_dma_chn);
}


/**
 * @brief   	this  function set spi rx dma channel.
 * @param[in]  	spi_sel     - the spi module.
 * @param[in]  	dst_addr 	- the address of destination.
 * @param[in]  	len 		- the length of data.
 * */
void spi_set_rx_dma(spi_sel_e spi_sel, unsigned char * dst_addr, unsigned int len)
{
    unsigned char rx_dma_chn;
    if (HSPI_MODULE == spi_sel)
    {
        rx_dma_chn = s_hspi_rx_dma_chn;
    }
    else
    {
        rx_dma_chn = s_pspi_rx_dma_chn;
    }
    spi_rx_tx_irq_trig_cnt(spi_sel,
                           5); // setting only for fixing the bug that slave receive number of bytes in multiples of 4 will fail.
    dma_set_address(rx_dma_chn, reg_spi_data_buf_adr(spi_sel), (unsigned int) convert_ram_addr_cpu2bus(dst_addr));
    dma_set_size(rx_dma_chn, len, DMA_WORD_WIDTH);
    dma_chn_en(rx_dma_chn);
}


/**
 * @brief     	This function serves to normal write data by dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	src_addr 	- the pointer to the data for write.
 * @param[in] 	len 		- write length.
 * @return  	none
 */
void spi_master_write_dma(spi_sel_e spi_sel, unsigned char * src_addr, unsigned int len)
{
    unsigned char tx_dma_chn;
    spi_tx_fifo_clr(spi_sel);
    spi_tx_dma_en(spi_sel);
    spi_tx_cnt(spi_sel, len);
    spi_set_transmode(spi_sel, SPI_MODE_WRITE_ONLY);
    if (HSPI_MODULE == spi_sel)
    {
        tx_dma_chn = s_hspi_tx_dma_chn;
    }
    else
    {
        tx_dma_chn = s_pspi_tx_dma_chn;
    }
    spi_set_dma(tx_dma_chn, (unsigned int) convert_ram_addr_cpu2bus(src_addr), reg_spi_data_buf_adr(spi_sel), len);
    spi_set_cmd(spi_sel, 0x00);
}

/**
 * @brief     	This function serves to normal write cmd and address, then read data by dma.
 * @param[in] 	spi_sel 	- the spi module.
 * @param[in] 	addr 		- the pointer to the cmd and address for write.
 * @param[in] 	addr_len 	- write length.
 * @param[in] 	data 		- the pointer to the data for read.
 * @param[in] 	data_len 	- read length.
 * @return  	none
 */
void spi_master_write_read_dma(spi_sel_e spi_sel, unsigned char * addr, unsigned int addr_len, unsigned char * data,
                               unsigned int data_len)
{
    unsigned char tx_dma_chn, rx_dma_chn;
    spi_tx_fifo_clr(spi_sel);
    spi_rx_fifo_clr(spi_sel);
    spi_tx_dma_en(spi_sel);
    spi_rx_dma_en(spi_sel);
    spi_tx_cnt(spi_sel, addr_len);
    spi_rx_cnt(spi_sel, data_len);
    spi_set_transmode(spi_sel, SPI_MODE_WRITE_READ);
    if (HSPI_MODULE == spi_sel)
    {
        tx_dma_chn = s_hspi_tx_dma_chn;
        rx_dma_chn = s_hspi_rx_dma_chn;
    }
    else
    {
        tx_dma_chn = s_pspi_tx_dma_chn;
        rx_dma_chn = s_pspi_rx_dma_chn;
    }
    spi_set_dma(tx_dma_chn, (unsigned int) convert_ram_addr_cpu2bus(addr), reg_spi_data_buf_adr(spi_sel), addr_len);
    spi_set_dma(rx_dma_chn, reg_spi_data_buf_adr(spi_sel), (unsigned int) convert_ram_addr_cpu2bus(data), data_len);
    spi_set_cmd(spi_sel, 0x00); // when  cmd  disable that  will not sent cmd,just trigger spi send .
}

/**
 * @brief      	This function serves to single/dual/quad  write to the SPI slave by dma.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	data 		- pointer to the data need to write.
 * @param[in]  	data_len 	- length in byte of the data need to write.
 * @param[in]  	wr_mode 	- write mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_write_dma_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char * data, unsigned int data_len,
                               spi_wr_tans_mode_e wr_mode)
{
    unsigned char tx_dma_chn;
    spi_tx_fifo_clr(spi_sel);
    spi_tx_dma_en(spi_sel);
    spi_tx_cnt(spi_sel, data_len);
    spi_set_transmode(spi_sel, (spi_tans_mode_e) wr_mode);
    if (HSPI_MODULE == spi_sel)
    {
        tx_dma_chn = s_hspi_tx_dma_chn;
        hspi_set_address(addr);
    }
    else
    {
        tx_dma_chn = s_pspi_tx_dma_chn;
    }
    spi_set_dma(tx_dma_chn, (unsigned int) convert_ram_addr_cpu2bus(data), reg_spi_data_buf_adr(spi_sel), data_len);
    spi_set_cmd(spi_sel, cmd);
}

/**
 * @brief      	This function serves to single/dual/quad  read from the SPI slave by dma.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	dst_addr 	- pointer to the buffer that will cache the reading out data.
 * @param[in]  	data_len 	- length in byte of the data need to read.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_read_dma_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned int addr, unsigned char * dst_addr,
                              unsigned int data_len, spi_rd_tans_mode_e rd_mode)
{
    unsigned char rx_dma_chn;
    spi_rx_fifo_clr(spi_sel);
    spi_rx_dma_en(spi_sel);
    spi_set_transmode(spi_sel, (spi_tans_mode_e) rd_mode);
    spi_rx_cnt(spi_sel, data_len);
    if (HSPI_MODULE == spi_sel)
    {
        rx_dma_chn = s_hspi_rx_dma_chn;
        hspi_set_address(addr);
    }
    else
    {
        rx_dma_chn = s_pspi_rx_dma_chn;
    }
    spi_set_dma(rx_dma_chn, reg_spi_data_buf_adr(spi_sel), (unsigned int) convert_ram_addr_cpu2bus(dst_addr), data_len);
    spi_set_cmd(spi_sel, cmd);
}

/**
 * @brief      	This function serves to single/dual/quad write address and read from the SPI slave by dma.
 * @param[in]  	spi_sel 	- the spi module.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr 		- the address of slave.
 * @param[in]  	addr_len 	- the length of address.
 * @param[in]  	rd_data 	- pointer to the buffer that will cache the reading out data.
 * @param[in]  	rd_len	 	- length in byte of the data need to read.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
void spi_master_write_read_dma_plus(spi_sel_e spi_sel, unsigned char cmd, unsigned char * addr, unsigned int addr_len,
                                    unsigned char * dst_addr, unsigned int rd_len, spi_rd_tans_mode_e rd_mode)
{
    unsigned char tx_dma_chn, rx_dma_chn;
    spi_tx_fifo_clr(spi_sel);
    spi_rx_fifo_clr(spi_sel);
    spi_tx_dma_en(spi_sel);
    spi_rx_dma_en(spi_sel);
    spi_tx_cnt(spi_sel, addr_len);
    spi_rx_cnt(spi_sel, rd_len);
    spi_set_transmode(spi_sel, (spi_tans_mode_e) rd_mode);
    if (HSPI_MODULE == spi_sel)
    {
        tx_dma_chn = s_hspi_tx_dma_chn;
        rx_dma_chn = s_hspi_rx_dma_chn;
    }
    else
    {
        tx_dma_chn = s_pspi_tx_dma_chn;
        rx_dma_chn = s_pspi_rx_dma_chn;
    }
    spi_set_dma(tx_dma_chn, (unsigned int) convert_ram_addr_cpu2bus(addr), reg_spi_data_buf_adr(spi_sel), addr_len);
    spi_set_dma(rx_dma_chn, reg_spi_data_buf_adr(spi_sel), (unsigned int) convert_ram_addr_cpu2bus(dst_addr), rd_len);
    spi_set_cmd(spi_sel, cmd); // when  cmd  disable that  will not sent cmd,just trigger spi send .
}

/**
 * @brief      	This function serves to single/dual (quad) write to the SPI slave by xip.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr_offset - offset of xip base address.
 * @param[in]  	data 		- pointer to the data need to write.
 * @param[in]  	data_len 	- length in byte of the data need to write.
 * @param[in]  	wr_mode 	- write mode  dummy or not dummy.
 * @return   	none
 */
_attribute_ram_code_sec_noinline_ void hspi_master_write_xip(unsigned char cmd, unsigned int addr_offset, unsigned char * data,
                                                             unsigned int data_len, spi_wr_tans_mode_e wr_mode)
{
    hspi_xip_write_transmode(wr_mode);
    hspi_xip_addr_offset(addr_offset);
    hspi_xip_set_wr_cmd(cmd);
    for (unsigned int i = 0; i < data_len; i++)
    {
        write_reg8(reg_hspi_xip_base_adr + i, data[i]);
    }
}

/**
 * @brief      	This function serves to single/dual (quad) read from the SPI slave by xip.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr_offset - offset of xip base address.
 * @param[in]  	data 		- pointer to the data need to read.
 * @param[in]  	data_len 	- length in byte of the data need to read.
 * @param[in]  	rd_mode 	- read mode.dummy or not dummy.
 * @return   	none
 */
_attribute_ram_code_sec_noinline_ void hspi_master_read_xip(unsigned char cmd, unsigned int addr_offset, unsigned char * data,
                                                            unsigned int data_len, spi_rd_tans_mode_e rd_mode)
{
    hspi_xip_read_transmode(rd_mode);
    hspi_xip_addr_offset(addr_offset);
    hspi_xip_set_rd_cmd(cmd);

    for (unsigned int i = 0; i < data_len; i++)
    {
        data[i] = read_reg8(reg_hspi_xip_base_adr + i);
    }
}

/**
 * @brief      	This function serves to a cmd and one data write to the SPI slave by xip.
 * @param[in]  	cmd 		- cmd one byte will first write.
 * @param[in]  	addr_offset - offset of xip base address.
 * @param[in]  	data_in 	- data need to write.
 * @param[in]  	wr_mode 	- write mode  dummy or not dummy.
 * @return   	none
 */
void hspi_master_write_xip_cmd_data(unsigned char cmd, unsigned int addr_offset, unsigned char data_in, spi_wr_tans_mode_e wr_mode)
{
    hspi_xip_write_transmode(wr_mode);
    hspi_xip_addr_offset(addr_offset);
    hspi_xip_set_wr_cmd(cmd);
    write_reg8(reg_hspi_xip_base_adr, data_in);
}
