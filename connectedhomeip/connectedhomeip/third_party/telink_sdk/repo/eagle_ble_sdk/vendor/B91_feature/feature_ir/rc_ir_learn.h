/********************************************************************************************************
 * @file	rc_ir_learn.h
 *
 * @brief	This is the header file for BLE SDK
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
#ifndef RC_IR_LEARN_H_
#define RC_IR_LEARN_H_
#if (FEATURE_TEST_MODE == TEST_IR)

#define  GPIO_IR_OUT	                     PWM_PIN   // GPIO_PE3
#define  GPIO_IR_CONTROL	                 GPIO_PE0
#define  GPIO_IR_LEARN_IN	                 GPIO_PE1

/************************************************************************************************
* ��define��IR_LEARN_FREQUENCY_RANGE_MODE :														*
* Choose IR Learn frequency range that device supported											*
* ��define��IR_LEARN_MAX_FREQUENCY: Max carrier frequency that device support to do IR learn		*
* ��define��IR_LEARN_MIN_FREQUENCY: Min carrier frequency that device support to do IR learn		*
************************************************************************************************/

#define			IR_LEARN_MAX_FREQUENCY			40000
#define			IR_LEARN_MIN_FREQUENCY			30000

#define			IR_LEARN_CARRIER_MIN_CYCLE		16000000/IR_LEARN_MAX_FREQUENCY
#define			IR_LEARN_CARRIER_MIN_HIGH_TICK	IR_LEARN_CARRIER_MIN_CYCLE/3
#define			IR_LEARN_CARRIER_MAX_CYCLE		16000000/IR_LEARN_MIN_FREQUENCY
#define			IR_LEARN_CARRIER_MAX_HIGH_TICK	IR_LEARN_CARRIER_MAX_CYCLE/3

/************************************************************************************************
* ��define��PARAMETER SETTING :																	*
* Related parameter setting to change IR learn result											*
************************************************************************************************/

#define			IR_LEARN_INTERVAL_THRESHOLD			(IR_LEARN_CARRIER_MAX_CYCLE*3/2)
#define			IR_LEARN_END_THRESHOLD	 			(30*SYSTEM_TIMER_TICK_1MS)
#define			IR_LEARN_OVERTIME_THRESHOLD			10000000						// 10s
#define			IR_CARR_CHECK_CNT		 			10
#define			CARR_AND_NO_CARR_MIN_NUMBER			15
#define			MAX_SECTION_NUMBER					100

// Debug
#define         Debug_IR                            0
// ir_learn_states
enum {
    IR_LEARN_DISABLE = 0x00,
    IR_LEARN_WAIT_KEY,
    IR_LEARN_KEY,
    IR_LEARN_BEGIN,
    IR_LEARN_SAMPLE_END,
	IR_LEARN_SUCCESS,
    IR_LEARN_FAIL_FIRST_INTERVAL_TOO_LONG,
    IR_LEARN_FAIL_TWO_LONG_NO_CARRIER,
    IR_LEARN_FAIL_WAIT_OVER_TIME,
    IR_LEARN_FAIL_WAVE_NUM_TOO_FEW,
    IR_LEARN_FAIL_FLASH_FULL,
    IR_LEARN_FAIL,
}ir_learn_states;

// ir_learn_ctrl_t
typedef struct{
	unsigned int   last_trigger_tm_point;
	unsigned int   curr_trigger_tm_point;
	unsigned int   time_interval;
	unsigned int   carr_first_interval;

	unsigned int   carr_switch_start_tm_point;
	unsigned char  carr_check_cnt;
	unsigned char  carr_or_not;
	unsigned char  ir_learn_state;
	unsigned char  ir_learn_for_later_use;
	unsigned int   ir_learn_tick;
	unsigned int   carr_cycle_interval;

	unsigned int   ir_learn_finish_tm;
	unsigned short ir_enter_irq_cnt;
	unsigned short wave_series_cnt;
	unsigned int   wave_series_buf[MAX_SECTION_NUMBER];
}ir_learn_ctrl_t;

// ir_learn_send_t
typedef struct{
	unsigned int   ir_learn_carrier_cycle;
	unsigned short ir_learn_wave_num;
	unsigned int   ir_lenrn_send_buf[MAX_SECTION_NUMBER];
}ir_learn_send_t;

// ir_send_dma_data_t
typedef struct{
    unsigned int   dma_len;        // dma len
    unsigned short data[MAX_SECTION_NUMBER];
    unsigned int   data_num;
}ir_send_dma_data_t;

#ifdef __cplusplus
extern "C" {
#endif

void ir_learn_init(void);
void ir_learn_start(void);
void ir_learn_stop(void);
void ir_learn_copy_result(ir_learn_send_t* send_buffer);
void ir_learn_irq_handler(void);
void ir_learn_send(ir_learn_send_t* send_buffer);
void ir_learn_send_init(void);
void ir_learn_detect(void);
void ir_learn_init(void);
unsigned char get_ir_learn_state(void);

#ifdef __cplusplus
}
#endif

#endif

#endif

