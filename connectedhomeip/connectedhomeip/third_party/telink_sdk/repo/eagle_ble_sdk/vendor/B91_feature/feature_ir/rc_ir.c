/********************************************************************************************************
 * @file	rc_ir.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "tl_common.h"
#include "drivers.h"
#include "rc_ir.h"

#if (FEATURE_TEST_MODE == TEST_IR)
pwm_dma_data_t T_dmaData_buf __attribute__((aligned(4)));
ir_send_ctrl_t ir_send_ctrl;

u16 waveform_start_bit_1st;
u16 waveform_start_bit_2nd;
u16 waveform_stop_bit_1st;
u16 waveform_stop_bit_2nd;

u16 waveform_logic_0_1st;
u16 waveform_logic_0_2nd;
u16 waveform_logic_1_1st;
u16 waveform_logic_1_2nd;

u16 waveform_repeat_1st;
u16 waveform_repeat_2nd;
u16 waveform_repeat_3rd;
u16 waveform_repeat_4th;

u16 waveform_wait_to_repeat;

void ir_nec_send(u8 addr1, u8 addr2, u8 cmd)
{

	if(ir_send_ctrl.last_cmd != cmd)
	{
		DBG_CHN2_TOGGLE;
		if(ir_sending_check())
		{
			return;
		}
		ir_send_ctrl.last_cmd = cmd;

	    // set waveform input in sequence
		T_dmaData_buf.data_num = 0;

		//waveform for start bit
		T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_1st;
		T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_start_bit_2nd;


		//add data
		u32 data = (~cmd)<<24 | cmd<<16 | addr2<<8 | addr1;
		for(int i=0;i<32;i++){
			if(data & BIT(i)){
				//waveform for logic_1
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_1st;
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_1_2nd;
			}
			else{
				//waveform for logic_0
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_1st;
				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_logic_0_2nd;
			}
		}

		//waveform for stop bit
		T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_1st;
		T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_stop_bit_2nd;

		T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

		ir_send_ctrl.repeat_enable = 0;  //need repeat signal

		ir_send_ctrl.is_sending = IR_SENDING_DATA;

//dma init
		pwm_set_dma_config(PWM_DMA_CHN);

		pwm_set_dma_buf(PWM_DMA_CHN, (u32) &T_dmaData_buf ,T_dmaData_buf.dma_len);

		pwm_ir_dma_mode_start(PWM_DMA_CHN);

		pwm_set_irq_mask(FLD_PWM0_IR_DMA_FIFO_IRQ);

		pwm_clr_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ );

		core_interrupt_enable();//

		plic_interrupt_enable(IRQ16_PWM);

		ir_send_ctrl.sending_start_time = clock_time();

	}
}

void pwm_stop_dma_ir_sending(void)
{
	reg_rst0 &= ~FLD_RST0_PWM;
	sleep_us(20);  //1us <-> 4 byte
	reg_rst0 |= FLD_RST0_PWM;
}

int ir_is_sending(void)
{
	if(ir_send_ctrl.is_sending && clock_time_exceed(ir_send_ctrl.sending_start_time, 300*1000))
	{
		ir_send_ctrl.is_sending = IR_SENDING_NONE;

		pwm_stop_dma_ir_sending();
	}

	return ir_send_ctrl.is_sending;
}

int ir_sending_check(void)
{
	u32 r = irq_disable();
	if(ir_is_sending()){
		irq_restore(r);
		return 1;
	}
	irq_restore(r);
	return 0;
}

void ir_send_release(void)
{
	u32 r = irq_disable();

	ir_send_ctrl.last_cmd = 0xff;

	dma_chn_dis(PWM_DMA_CHN);

#if(!ADD_REPEAT_ONE_BY_ONE)
	if(ir_send_ctrl.is_sending != IR_SENDING_NONE){
		pwm_stop_dma_ir_sending();
	}
#endif

	ir_send_ctrl.is_sending = IR_SENDING_NONE;

	pwm_clr_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ);        //clear irq status

	pwm_clr_irq_mask(FLD_PWM0_IR_DMA_FIFO_IRQ);   //disable irq mask

	irq_restore(r);
}



_attribute_ram_code_
/**
 * @brief define system clock tick per us/ms/s.
 */
void rc_ir_irq_prc(void)
{
	if(pwm_get_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ)){

		pwm_clr_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ);

		if(ir_send_ctrl.repeat_enable){

			if(ir_send_ctrl.is_sending == IR_SENDING_DATA){

				ir_send_ctrl.is_sending = IR_SENDING_REPEAT;

				T_dmaData_buf.data_num = 0;

				u32 tick_2_repeat_sysClockTimer16M = 110*CLOCK_16M_SYS_TIMER_CLK_1MS - (clock_time() - ir_send_ctrl.sending_start_time);
				u32 tick_2_repeat_sysTimer = (tick_2_repeat_sysClockTimer16M*CLOCK_PWM_CLOCK_1US>>4);


				waveform_wait_to_repeat = pwm_cal_pwm0_ir_fifo_cfg_data(tick_2_repeat_sysTimer/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);

				T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_wait_to_repeat;

				for(int i=0;i<5;i++){
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_1st;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_2nd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_3rd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_4th;
				}

				T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

				pwm_set_dma_buf(PWM_DMA_CHN, (u32) &T_dmaData_buf ,T_dmaData_buf.dma_len);

				pwm_ir_dma_mode_start(PWM_DMA_CHN);
			}
			else if(ir_send_ctrl.is_sending == IR_SENDING_REPEAT){

				T_dmaData_buf.data_num = 0;
				for(int i=0;i<5;i++){
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_1st;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_2nd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_3rd;
					T_dmaData_buf.data[T_dmaData_buf.data_num ++] = waveform_repeat_4th;
				}

				T_dmaData_buf.dma_len = T_dmaData_buf.data_num * 2;

				pwm_set_dma_buf(PWM_DMA_CHN, (u32) &T_dmaData_buf ,T_dmaData_buf.dma_len);

				pwm_ir_dma_mode_start(PWM_DMA_CHN);
			}
		}
		else{
			ir_send_release();
		}
	}
}




void rc_ir_init(void)
{
//pwm set
	pwm_n_invert_en(PWM0_ID);

	pwm_set_clk((unsigned char) (sys_clk.pclk*1000*1000/PWM_CLK_SPEED-1));//use pclk is ok

	pwm_set_pin(PWM_PIN);

    pwm_set_pwm0_mode(PWM_IR_DMA_FIFO_MODE);

    pwm_set_tcmp(PWM_ID, PWM_CARRIER_HIGH_TICK);

    pwm_set_tmax(PWM_ID, PWM_CARRIER_CYCLE_TICK);

//logic_0, 560 us carrier,  560 us low
    waveform_logic_0_1st = pwm_cal_pwm0_ir_fifo_cfg_data(560 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 1);
    waveform_logic_0_2nd = pwm_cal_pwm0_ir_fifo_cfg_data(560 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);

//logic_1, 560 us carrier,  1690 us low
	waveform_logic_1_1st = pwm_cal_pwm0_ir_fifo_cfg_data(560 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 1);
	waveform_logic_1_2nd = pwm_cal_pwm0_ir_fifo_cfg_data(1690 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);


//start bit, 9000 us carrier,  4500 us low
	waveform_start_bit_1st = pwm_cal_pwm0_ir_fifo_cfg_data(9000 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 1);
	waveform_start_bit_2nd = pwm_cal_pwm0_ir_fifo_cfg_data(4500 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);

//stop bit,  560 us carrier, 500 us low
	waveform_stop_bit_1st = pwm_cal_pwm0_ir_fifo_cfg_data(560 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 1);
	waveform_stop_bit_2nd = pwm_cal_pwm0_ir_fifo_cfg_data(500 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);


//repeat signal  first part,  9000 us carrier, 2250 us low
	waveform_repeat_1st = pwm_cal_pwm0_ir_fifo_cfg_data(9000 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 1);
	waveform_repeat_2nd = pwm_cal_pwm0_ir_fifo_cfg_data(2250 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);

//repeat signal  second part,  560 us carrier, 99190 us low(110 ms - 9000us - 2250us - 560us = 99190 us)
	waveform_repeat_3rd = pwm_cal_pwm0_ir_fifo_cfg_data(560 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 1);
	waveform_repeat_4th = pwm_cal_pwm0_ir_fifo_cfg_data(99190 * CLOCK_PWM_CLOCK_1US/PWM_CARRIER_CYCLE_TICK, PWM0_PULSE_NORMAL, 0);

	ir_send_ctrl.last_cmd = 0xff; //must
}

#endif

