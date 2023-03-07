/********************************************************************************************************
 * @file	pwm.c
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
#include "pwm.h"

dma_config_t pwm_tx_dma_config = {
    .dst_req_sel    = DMA_REQ_PWM_TX, // tx req
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
    .auto_en        = 0,
};


/**
 * @brief     This fuction servers to set pin as pwm0
 * @param[in] pin - selected pin
 * @return	  none.
 */
void pwm_set_pin(pwm_pin_e pin)
{
    unsigned char val       = 0;
    unsigned char start_bit = (BIT_LOW_BIT(pin & 0xff) % 4) << 1;
    unsigned char mask      = (unsigned char) ~BIT_RNG(start_bit, start_bit + 1);

    if (pin == PWM_PWM2_PB7)
    { // Pad Function Mux:0
        val = 0;
        BM_CLR(reg_gpio_pad_mul_sel, BIT(3));
    }
    else if ((pin == PWM_PWM0_PB4) || (pin == PWM_PWM4_PD7) || (pin == PWM_PWM2_N_PE6) || (pin == PWM_PWM3_N_PE7))
    { // Pad Function Mux:1
        val = 1 << (start_bit);
    }
    else
    { // Pad Function Mux:2
        val = 2 << (start_bit);
        reg_gpio_pad_mul_sel |= BIT(0);
    }

    reg_gpio_func_mux(pin) = (reg_gpio_func_mux(pin) & mask) | val;
    gpio_function_dis((gpio_pin_e) pin);
}


/**
 * @brief     This function servers to configure DMA channel and some configures.
 * @param[in] chn - to select the DMA channel.
 * @return    none
 */
void pwm_set_dma_config(dma_chn_e chn)
{
    dma_config(chn, &pwm_tx_dma_config);
}


/**
 * @brief     This function servers to configure DMA channel address and length.
 * @param[in] chn - to select the DMA channel.
 * @param[in] buf_addr - the address where DMA need to get data from SRAM.
 * @param[in] len - the length of data in SRAM.
 * @return    none
 */
void pwm_set_dma_buf(dma_chn_e chn, unsigned int buf_addr, unsigned int len)
{
    dma_set_address(chn, convert_ram_addr_cpu2bus(buf_addr), reg_pwm_data_buf_adr);
    dma_set_size(chn, len, DMA_WORD_WIDTH);
}


/**
 * @brief     This function servers to enable DMA channel.
 * @param[in] chn - to select the DMA channel.
 * @return    none
 */
void pwm_ir_dma_mode_start(dma_chn_e chn)
{
    dma_chn_en(chn);
}



/**
 * @brief     This function servers to configure DMA head node.
 * @param[in] chn - to select the DMA channel.
 * @param[in] src_addr - to configure DMA source address.
 * @param[in] data_len - to configure DMA length.
 * @param[in] head_of_list - to configure the address of the next node configure.
 * @return    none
 */
void pwm_set_dma_chain_llp(dma_chn_e chn, unsigned short * src_addr, unsigned int data_len, dma_chain_config_t * head_of_list)
{
    dma_config(chn, &pwm_tx_dma_config);
    dma_set_address(chn, convert_ram_addr_cpu2bus(src_addr), reg_pwm_data_buf_adr);
    dma_set_size(chn, data_len, DMA_WORD_WIDTH);
    reg_dma_llp(chn) = (unsigned int) convert_ram_addr_cpu2bus(head_of_list);
}


/**
 * @brief     This function servers to configure DMA cycle chain node.
 * @param[in] chn - to select the DMA channel.
 * @param[in] config_addr  - to servers to configure the address of the current node.
 * @param[in] llponit - to configure the address of the next node configure.
 * @param[in] src_addr - to configure DMA source address.
 * @param[in] data_len - to configure DMA length.
 * @return    none
 */
void pwm_set_tx_dma_add_list_element(dma_chn_e chn, dma_chain_config_t * config_addr, dma_chain_config_t * llponit,
                                     unsigned short * src_addr, unsigned int data_len)
{
    config_addr->dma_chain_ctl      = reg_dma_ctrl(chn) | BIT(0);
    config_addr->dma_chain_src_addr = (unsigned int) convert_ram_addr_cpu2bus(src_addr);
    config_addr->dma_chain_dst_addr = reg_pwm_data_buf_adr;
    config_addr->dma_chain_data_len = dma_cal_size(data_len, DMA_WORD_WIDTH);
    config_addr->dma_chain_llp_ptr  = (unsigned int) convert_ram_addr_cpu2bus(llponit);
}

