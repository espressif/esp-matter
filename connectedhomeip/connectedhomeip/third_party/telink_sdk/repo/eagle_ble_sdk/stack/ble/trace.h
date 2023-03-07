/********************************************************************************************************
 * @file	trace.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
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
#ifndef TRACE_H_
#define TRACE_H_


//	event: 0 for time stamp; 1 reserved; eid2 - eid31
#define			SLEV_timestamp				0
#define			SLEV_reserved				1

#define 		SLEV_irq_rf					2
#define 		SLEV_irq_sysTimer			3

#define 		SLEV_irq_rx					5
#define 		SLEV_irq_rxCrc				6
#define 		SLEV_irq_rxTimStamp			7
#define 		SLEV_irq_rxNew				8

#define			SLEV_irq_tx					10

#define 		SLEV_irq_cmddone			12
#define 		SLEV_irq_rxTmt				13
#define 		SLEV_irq_rxFirstTmt			14
#define 		SLEV_irq_fsmTmt				15

#define 		SLEV_slave_1stRx			16

#define			SLEV_txFifo_push			17
#define			SLEV_txFifo_empty			18




#define 		SLEV_test_event				31








// 1-bit data: 0/1/2 for hardware signal
#define			SL01_irq					1
#define			SL01_sysTimer				2
#define			SL01_adv     				3
#define			SL01_brx     				4
#define			SL01_btx     				5

// 8-bit data: cid0 - cid63
#define			SL08_test_1B				0



// 16-bit data: sid0 - sid63
#define			SL16_tf_hw_push				1
#define			SL16_tf_sw_push				2
#define			SL16_tf_hw_load1			3
#define			SL16_tf_sw_load1			4
#define			SL16_tf_hw_load2			5
#define			SL16_tf_sw_load2			6
#define			SL16_tf_hw_RX				7
#define			SL16_tf_sw_RX				8
#define			SL16_tf_hw_TX				9
#define			SL16_tf_sw_TX				10

#define			SL16_seq_notify				15
#define			SL16_seq_write				16

//#define			SL01_test_task				0
//#define			SL08_test_1B				0
//#define			SL16_test_2B				0


#endif
