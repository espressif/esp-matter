/********************************************************************************************************
 * @file	myudb.h
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
#ifndef		__MYUDB_H__
#define		__MYUDB_H__
#pragma once

#include "log_def_stack.h"
#include "common/config/user_config.h"

/* Trace global enable macro */
#ifndef         VCD_EN
#define			VCD_EN												0
#endif
/* USB print log enable macro */
#ifndef			DUMP_STR_EN
#define 		DUMP_STR_EN                                   		0
#endif


#define			SL_STACK_EN											1


#if (DUMP_STR_EN)
	#define my_usb_init(id, p_print)		myudb_usb_init(id, p_print)
	#define	usb_send_str(s)					usb_send_str_data(s, 0, 0)
	#define	usb_send_data(p,n) 				usb_send_str_data(0,p,n)
	#define my_dump_str_data(en,s,p,n)		if(en){usb_send_str_data(s,(u8*)(p),n);}
	#define my_dump_str_u32s(en,s,d0,d1,d2,d3)		if(en){usb_send_str_u32s(s,(u32)(d0),(u32)(d1),(u32)(d2),(u32)(d3));}
	#define my_uart_send_str_data			usb_send_str_data
	#define	my_uart_send_str_int			usb_send_str_int
	#define	myudb_usb_handle_irq()			udb_usb_handle_irq()
#else
	#define my_usb_init(id, p_print)
	#define	usb_send_str(s)
	#define	usb_send_data(p,n)
	#define my_dump_str_data(en,s,p,n)
	#define my_dump_str_u32s(en,s,d0,d1,d2,d3)
	#define my_uart_send_str_data
	#define	my_uart_send_str_int
	#define	myudb_usb_handle_irq()
#endif



typedef int (*func_myudb_hci_cmd_cb_t) (unsigned char *, int);

#ifdef __cplusplus
extern "C" {
#endif

void 	myudb_register_hci_cb (void *p);
void    myudb_register_hci_debug_cb (void *p);

void    myudb_usb_init(u16 id, void * p_print);

void 	myudb_usb_bulkout_ready ();

void 	udb_usb_handle_irq(void);

void 	usb_send_status_pkt(u8 status, u8 buffer_num, u8 *pkt, u16 len);

void 	myudb_capture_enable (int en);

void 	usb_send_str_data (char *str, u8 *ph, int n);

void 	usb_send_str_u32s (char *str, u32 d0, u32 d1, u32 d2, u32 d3);

#ifdef __cplusplus
}
#endif

#define			my_irq_disable()		u32 rie = core_interrupt_disable ()
#define			my_irq_restore()		core_restore_interrupt(rie)
#define			LOG_EVENT_TIMESTAMP		0
#define			LOG_DATA_B1_0			0
#define			LOG_DATA_B1_1			1

//#define			get_systemtick()  	    (clock_time ()*3/2)
#define			get_systemtick()  	    stimer_get_tick()

//#define			log_uart(d)				uart_send_byte_dma(0,d)
#define			log_uart(d)				reg_usb_ep8_dat=d

#define         DEBUG_PORT				GPIO_PB2
#define			log_ref_gpio_h()		gpio_set_high_level(DEBUG_PORT)
#define			log_ref_gpio_l()		gpio_set_low_level(DEBUG_PORT)


#define	log_hw_ref()			 if(VCD_EN){my_irq_disable();log_ref_gpio_h();log_uart(0x20);int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);log_ref_gpio_l();my_irq_restore();}

// 4-byte sync word: 00 00 00 00
#define	log_sync(en)			 if(VCD_EN && en) {my_irq_disable();log_uart(0);log_uart(0);log_uart(0);log_uart(0);my_irq_restore();}
//4-byte (001_id-5bits) id0: timestamp align with hardware gpio output; id1-31: user define
#define	log_tick(en,id)			 if(VCD_EN && en) {my_irq_disable();log_uart(0x20|(id&31));int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);my_irq_restore();}

//1-byte (000_id-5bits)
#define	log_event(en,id)		 if(VCD_EN && en) {my_irq_disable();log_uart(0x00|(id&31));my_irq_restore();}

//1-byte (01x_id-5bits) 1-bit data: id0 & id1 reserved for hardware
#define	log_task(en,id,b)		 if(VCD_EN && en) {my_irq_disable();log_uart(((b)?0x60:0x40)|(id&31));int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);my_irq_restore();}

//2-byte (10-id-6bits) 8-bit data
#define	log_b8(en,id,d)			 if(VCD_EN && en) {my_irq_disable();log_uart(0x80|(id&63));log_uart(d);my_irq_restore();}

//3-byte (11-id-6bits) 16-bit data
#define	log_b16(en,id,d)		 if(VCD_EN && en) {my_irq_disable();log_uart(0xc0|(id&63));log_uart(d);log_uart((d)>>8);my_irq_restore();}




#define	log_tick_irq(en,id)		 if(VCD_EN && en) {log_uart(0x20|(id));int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);}
#define	log_tick_irq_2(en,id,t)	 if(VCD_EN && en) {log_uart(0x20|(id));log_uart(t);log_uart(t>>8);log_uart(t>>16);}


#define	log_event_irq(en,id) 	 if(VCD_EN && en) {log_uart(0x00|(id));}


#define	log_task_irq(en,id,b)	 if(VCD_EN && en) {log_uart(((b)?0x60:0x40)|(id));int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);}

#define	log_task_begin_irq(en,id)	 if(VCD_EN && en) {log_uart(0x60|(id));int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);}
#define	log_task_end_irq(en,id)		 if(VCD_EN && en) {log_uart(0x40|(id));int t=get_systemtick();log_uart(t);log_uart(t>>8);log_uart(t>>16);}

#define	log_task_begin_irq_2(en,id,t) if(VCD_EN && en) {log_uart(0x60|(id));log_uart(t);log_uart(t>>8);log_uart(t>>16);}
#define	log_task_end_irq_2(en,id,t) 	 if(VCD_EN && en) {log_uart(0x40|(id));log_uart(t);log_uart(t>>8);log_uart(t>>16);}



#define	log_b8_irq(en,id,d)		 if(VCD_EN && en) {log_uart(0x80|(id));log_uart(d);}

#define	log_b16_irq(en,id,d)	 if(VCD_EN && en) {log_uart(0xc0|(id));log_uart(d);log_uart((d)>>8);}




#endif
