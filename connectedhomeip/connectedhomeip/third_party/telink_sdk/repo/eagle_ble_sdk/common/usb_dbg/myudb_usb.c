/********************************************************************************************************
 * @file	myudb_usb.c
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
#include "drivers.h"
#include "myudb.h"
#include "myudb_usbdesc.h"
#include "common/utility.h"
#include <string.h>

#if (VCD_EN || DUMP_STR_EN)



typedef struct myudb_cfg
{
	u16		id;
	u16 	response_len;

	u8*		response;
	u32		tick_sync;

	u8 		stall;
	u8		rptr;
	u16		cmd_len;

} myudb_cfg_t;

myudb_cfg_t myudb = {0x120};

static USB_Request_Header_t control_request;



_attribute_data_retention_ my_fifo_t   *myudb_print_fifo = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//		USB device handling
//-----------------------------------------------------------------------------------------
enum {
	MYUDB_USB_IRQ_SETUP_REQ = 0,
	MYUDB_USB_IRQ_DATA_REQ,
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------
void myudb_usb_send_response(void) {
	u16 n;
	if (myudb.response_len < 8) {
		n = myudb.response_len;
	} else {
		n = 8;
	}
	myudb.response_len -= n;
	usbhw_reset_ctrl_ep_ptr();
	while (n-- > 0) {
		usbhw_write_ctrl_ep_data(*myudb.response);
		++myudb.response;
	}
}

void myudb_usb_prepare_desc_data(void) {
	u8 value_l = (control_request.wValue) & 0xff;
	u8 value_h = (control_request.wValue >> 8) & 0xff;

	myudb.response = 0;
	myudb.response_len = 0;

	switch (value_h) {
		
	case DTYPE_Device:
		myudb.response = myudb_usbdesc_get_device();
		myudb.response_len = sizeof(USB_Descriptor_Device_t);
		break;

	case DTYPE_Configuration:
		myudb.response = myudb_usbdesc_get_configuration();
		myudb.response_len = sizeof(MYUDB_USB_Descriptor_Configuration_t);
		break;

	case DTYPE_String:
		if (MYUDB_USB_STRING_LANGUAGE == value_l) {
			myudb.response = myudb_usbdesc_get_language();
			myudb.response_len = sizeof(LANGUAGE_ID_ENG);
		} else if (MYUDB_USB_STRING_VENDOR == value_l) {
			myudb.response = myudb_usbdesc_get_vendor();
			myudb.response_len = sizeof (MYUDB_STRING_VENDOR);
		} else if (MYUDB_USB_STRING_PRODUCT == value_l) {
			myudb.response = myudb_usbdesc_get_product();
			myudb.response_len = sizeof(MYUDB_STRING_PRODUCT);
		} else if (MYUDB_USB_STRING_SERIAL == value_l) {
			myudb.response = myudb_usbdesc_get_serial();
			myudb.response_len = sizeof(MYUDB_STRING_SERIAL);
		} else {
			myudb.stall = 1;
		}
		break;

	default:
		myudb.stall = 1;
		break;

	}

	if (control_request.wLength < myudb.response_len) {
		myudb.response_len = control_request.wLength;
	}

	return;
}

void myudb_usb_handle_in_class_intf_req() {
	u8 property = control_request.bRequest;
	switch (property) {
		case 0x00:
			usbhw_write_ctrl_ep_data(0x00);
			break;
		default:
			myudb.stall = 1;
			break;
	}
	
}

void myudb_usb_handle_request(u8 data_request) {
	u8 bmRequestType = control_request.bmRequestType;
	u8 bRequest = control_request.bRequest;

	usbhw_reset_ctrl_ep_ptr();
	switch (bmRequestType) {
	case (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_DEVICE):
		if (REQ_GetDescriptor == bRequest) {
			if (MYUDB_USB_IRQ_SETUP_REQ == data_request) {
				myudb_usb_prepare_desc_data();
			}
			myudb_usb_send_response();
		} 
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE):
		myudb_usb_handle_in_class_intf_req();
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_INTERFACE):
		if (MYUDB_USB_IRQ_SETUP_REQ == data_request) {
			if (0xc0 == bRequest) {					// Get board version
				usbhw_reset_ctrl_ep_ptr();
				usbhw_write_ctrl_ep_data(myudb.id);
				usbhw_write_ctrl_ep_data(myudb.id >> 8);
			}
			else
			{
				myudb.stall = 1;
			}
		}
		break;
	case (REQDIR_DEVICETOHOST | REQTYPE_VENDOR | REQREC_DEVICE):	// 0xc0
		if (MYUDB_USB_IRQ_SETUP_REQ == data_request) {
			if (0xc0 == bRequest) {					// Get board version
				usbhw_reset_ctrl_ep_ptr();
				usbhw_write_ctrl_ep_data(0x40);
				usbhw_write_ctrl_ep_data(0x25);
				usbhw_write_ctrl_ep_data(0x40);
				usbhw_write_ctrl_ep_data(0x05);
				usbhw_write_ctrl_ep_data(0x03);
				usbhw_write_ctrl_ep_data(0x00);
				usbhw_write_ctrl_ep_data(0x01);
				usbhw_write_ctrl_ep_data(0x00);
			}
			else if (0xc6 == bRequest) {			// 
				usbhw_reset_ctrl_ep_ptr();
				usbhw_write_ctrl_ep_data(0x04);
			}
			else
			{
				myudb.stall = 1;
			}
		}
		break;
	case (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE):	// 0x40
		myudb.stall = 1;
		break;
	default:
		myudb.stall = 1;
		break;
	}

	return;
}

void myudb_usb_handle_ctl_ep_setup() {
	usbhw_reset_ctrl_ep_ptr();
	control_request.bmRequestType = usbhw_read_ctrl_ep_data();
	control_request.bRequest = usbhw_read_ctrl_ep_data();
	control_request.wValue = usbhw_read_ctrl_ep_u16();
	control_request.wIndex = usbhw_read_ctrl_ep_u16();
	control_request.wLength = usbhw_read_ctrl_ep_u16();
	myudb.stall = 0;
	myudb_usb_handle_request(MYUDB_USB_IRQ_SETUP_REQ);
	if (myudb.stall)
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	else
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
}

void myudb_usb_handle_ctl_ep_data(void) {
	usbhw_reset_ctrl_ep_ptr();
	myudb.stall = 0;
	myudb_usb_handle_request(MYUDB_USB_IRQ_DATA_REQ);
	if (myudb.stall)
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_STALL);
	else
		usbhw_write_ctrl_ep_ctrl(FLD_EP_DAT_ACK);
}

void myudb_usb_handle_ctl_ep_status() {
	if (myudb.stall)
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_STALL);
	else
		usbhw_write_ctrl_ep_ctrl(FLD_EP_STA_ACK);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

_attribute_ram_code_ int  myudb_print_fifo_full (void)
{
	u8 n = myudb_print_fifo->wptr - myudb_print_fifo->rptr;
	return n >= myudb_print_fifo->num;
}

_attribute_ram_code_ void usb_send_status_pkt(u8 status, u8 buffer_num, u8 *pkt, u16 len)
{
//	if (myudb_print_fifo_full()) return;		//skip if overflow

	u8 *p = myudb_print_fifo->p + (myudb_print_fifo->wptr & (myudb_print_fifo->num - 1)) * myudb_print_fifo->size;
	if (len > 272)
	{
		len = 272;
	}
	*p++ = len + 2;
	*p++ = (len + 2) >> 8;
	 p += 2;
	*p++ = status;
	*p++ = buffer_num;
	while (len--)
	{
		*p++ = *pkt++;
	}
	myudb_print_fifo->wptr++;
}

_attribute_ram_code_ void usb_send_str_data (char *str, u8 *ph, int n)
{
//	if (myudb_print_fifo_full()) return;		//skip if overflow
	u32 rie = core_interrupt_disable ();
	u8 *ps =  myudb_print_fifo->p + (myudb_print_fifo->wptr & (myudb_print_fifo->num - 1)) * myudb_print_fifo->size;;
	u8 *pd = ps;

	int ns = str ? strlen (str) : 0;
	if (ns > myudb_print_fifo->size - 12)
	{
		ns = myudb_print_fifo->size - 12;
		n = 0;
	}
	if (n + ns > myudb_print_fifo->size - 12)
	{
		n = myudb_print_fifo->size - 12 - ns;
	}

	int len = n + ns + 2 + 3;
	*pd++ = len;
	*pd++ = len >> 8;
	*pd++ = 0;
	*pd++ = 0;
	*pd++ = 0x82;
	*pd++ = 8;

	*pd++ = 0x22;
	*pd++ = n;
	*pd++ = n >> 8;
    
	while (n--)
	{
		*pd++ = *ph++;
	}
	while (ns--)
	{
		*pd++ = *str++;
	}
	myudb_print_fifo->wptr++;
	core_restore_interrupt(rie);
}

_attribute_ram_code_ void usb_send_str_u32s (char *str, u32 d0, u32 d1, u32 d2, u32 d3)
{
	u32 d[4];
	d[0] = d0;
	d[1] = d1;
	d[2] = d2;
	d[3] = d3;
	usb_send_str_data (str, (u8*)d, 16);
}

_attribute_ram_code_ void myudb_to_usb()
{
	static u16 len = 0;
	static u8 *p = 0;

	if(usbhw_is_ep_busy(MYUDB_EDP_IN_HCI)) return;
	if (!p && (myudb_print_fifo->wptr != myudb_print_fifo->rptr))		//first packet
	{
		p =  myudb_print_fifo->p + (myudb_print_fifo->rptr++ & (myudb_print_fifo->num - 1)) * myudb_print_fifo->size;;
		//len = p[1] + 3;
		len = p[0] + p[1] * 256;
		p += 4;
	}
	if (p)
	{
		int n = len < 64 ? len : 64;
		usbhw_reset_ep_ptr(MYUDB_EDP_IN_HCI);
		for (int i=0; i<n; i++)
		{
			usbhw_write_ep_data(MYUDB_EDP_IN_HCI, *p++);
		}
		usbhw_data_ep_ack(MYUDB_EDP_IN_HCI);
		len -= n;
		if (n < 64)
		{
			p = 0;
		}
	}
}

int	usb_send_str_int (char *str,int w)
{
	if (myudb_print_fifo_full())
	{
		return -1;
	}
	int len;

	u32 rie = core_interrupt_disable ();
	char buf[13], tmp, *p;
	int u;
	char symbol = ' ';
	int  int_len=0;
	int n = strlen (str);
	if (n == 0)
	{
		return -2;
	}

	p = buf + 12;
	*p = '\0';
	u = w;
	if(w<0)
	{
		symbol = '-';
		u = -w;
	}
	do{ // at least one time..
	        tmp = u % 10;
			*--p = tmp + '0';
			u /= 10;
			int_len++;
	}while(u);



	u8 *pd = myudb_print_fifo->p + (myudb_print_fifo->wptr & (myudb_print_fifo->num-1)) * myudb_print_fifo->size;
	len = n +  2 + 1 + 1 + int_len; //str_len + '0x20' + symbol + int_len
	*pd++ = len ;
	*pd++ = len >> 8;


	*pd++ = 0;
	*pd++ = 0;
	*pd++ = 0x82;
	*pd++ = 8;

	*pd++ = 0x20;
	while (n--)
	{
		*pd++ = *str++;
	}
	*pd++ = symbol;
	while(int_len--)
	{
		*pd++ = *p++;
	}
	myudb_print_fifo->wptr++;
	core_restore_interrupt(rie);
	return len;
}


#define			USB_ENDPOINT_BULK_OUT_FLAG		(1 << (MYUDB_EDP_OUT_HCI & 7))

_attribute_ram_code_ int myudb_usb_get_packet (u8 *p)
{
	if (reg_usb_irq & USB_ENDPOINT_BULK_OUT_FLAG)
	{
		//clear interrupt flag
		reg_usb_irq = USB_ENDPOINT_BULK_OUT_FLAG;

		// read data
		int n = reg_usb_ep_ptr(MYUDB_EDP_OUT_HCI);
		reg_usb_ep_ptr(MYUDB_EDP_OUT_HCI) = 0;
		for (int i=0; i<n; i++)
		{
			p[myudb.cmd_len++] = reg_usb_ep_dat(MYUDB_EDP_OUT_HCI);
		}
		reg_usb_ep_ctrl (MYUDB_EDP_OUT_HCI) = 1;		//ACK
		if (n < 64)
		{
			n = myudb.cmd_len;
			myudb.cmd_len = 0;
			return n;
		}
	}
	return 0;
}

func_myudb_hci_cmd_cb_t	myudb_hci_cmd_cb = 0;

void myudb_register_hci_cb (void *p)
{
	myudb_hci_cmd_cb = p;
}

func_myudb_hci_cmd_cb_t	myudb_hci_debug_cb = 0;
void myudb_register_hci_debug_cb (void *p)
{
	myudb_hci_debug_cb = p;
}

#define			MYHCI_FW_DOWNLOAD			0xfe
int fw_download = 0;

_attribute_ram_code_ int myudb_mem_cmd (u8 *p, int nbyte)
{
	int len = nbyte;
	int cmd = p[0];
	u8	rsp[280];

	int ret = 0;

	//////////////////////////  Memory Read ////////////////////////////////////
	if (cmd == 0x28 && len >= 8)
	{
		usb_send_status_pkt (0x81, 8, p, 12);
		rsp[0] = 0x29;
		memcpy (rsp + 1, p + 1, 5);
		int type = p[1];
		u32 adr = p[2] | (p[3] << 8) | (p[4] << 16) | (p[5] << 24);
		int n = p[6] | (p[7] << 8);
		if (n > 256)
		{
			n = 256;
		}

		if (type == 0)
		{
			memcpy (rsp + 6, (void *)(adr | 0), n);
		}
		else if (type == 1)
		{
			for (int i=0; i<n; i++)
			{
				rsp[i + 6] = analog_read_reg8 (adr + i);
			}
		}
		else if (type == 2 || type == 3)					//flash
		{
			flash_read_page (adr, n, rsp + 6);
		}

		usb_send_status_pkt (0x82, 8, rsp, n + 6);
	}
	//////////////////////////  Memory Write ////////////////////////////////////
	else if (cmd == 0x2a && len > 6)
	{
		usb_send_status_pkt (0x81, 8, p, 12);
		rsp[0] = 0x2b;
		memcpy (rsp + 1, p + 1, 16);
		u8 type = p[1];
		u32 adr = p[2] | (p[3] << 8) | (p[4] << 16) | (p[5] << 24);
		int n = len - 6;

		if (type == 0)				//RAM
		{
			memcpy ((void *)adr, p + 6, n);
		}
		else if (type == 1)			//Analog Register
		{
			for (int i=0; i<n; i++)
			{
				analog_write_reg8 (adr + i, p[i + 6]);
			}
		}
		else if (type == 2)			// flash
		{
			if (fw_download && (adr & 0xfff) == 0)
			{
				flash_erase_sector (adr);
			}
			flash_write_page (adr, n, p + 6);
		}
		else if (type == 3)
		{
			int nb = p[6];
			if (n > 1)
				nb += p[7] << 8;
			if (n > 2)
				nb += p[8] << 16;
			if (n > 3)
				nb += p[9] << 24;

			if (nb)
			{
				for (int i=0; i<nb; i+=4096)
				{
					flash_erase_sector (adr + i);
				}
			}
			else
			{
				flash_erase_chip ();
			}
		}
		else if (type == MYHCI_FW_DOWNLOAD)
		{
			core_interrupt_disable ();

			myudb_print_fifo->rptr = myudb_print_fifo->wptr;

			//extern void hci_txfifo_set_rptr_reference (int idx);
			//hci_txfifo_set_rptr_reference (1);

			ret = MYHCI_FW_DOWNLOAD;
		}
		usb_send_status_pkt (0x82, 8, rsp, 14);
	}
	else
	{
		ret = 1;
	}
	return ret;
}

u8 buff_usb_cmd[320];
_attribute_ram_code_ int myudb_hci_cmd_from_usb (void)
{
	fw_download = 0;
	do
	{
		int n = myudb_usb_get_packet (buff_usb_cmd);
		if (n)
		{
			int r = myudb_mem_cmd(buff_usb_cmd, n);

			if (r == MYHCI_FW_DOWNLOAD)
			{
				fw_download =  MYHCI_FW_DOWNLOAD;
			}
            else if (0x11 == buff_usb_cmd[0] && myudb_hci_debug_cb && !fw_download)
            {
                myudb_hci_debug_cb (buff_usb_cmd, n);
            }
			else if (r && myudb_hci_cmd_cb && !fw_download)
			{
				myudb_hci_cmd_cb (buff_usb_cmd, n);
			}
		}
		if (fw_download)
		{
			myudb_to_usb ();
		}
	} while (fw_download);

	return 0;
}

/////////////////////////////////////////////////////////////////////////
void udb_usb_handle_irq(void) {
	if(1){		//  do nothing when in suspend. Maybe not unnecessary
		u32 irq = usbhw_get_ctrl_ep_irq();
		if (irq & FLD_CTRL_EP_IRQ_SETUP) {
			usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_SETUP);
			myudb_usb_handle_ctl_ep_setup();
			if (!myudb.tick_sync)
			{
				myudb.tick_sync = clock_time () | 1;
			}
		}
		if (irq & FLD_CTRL_EP_IRQ_DATA) {
			usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_DATA);
			myudb_usb_handle_ctl_ep_data();
		}
		if (irq & FLD_CTRL_EP_IRQ_STA) {
			usbhw_clr_ctrl_ep_irq(FLD_CTRL_EP_IRQ_STA);
			myudb_usb_handle_ctl_ep_status();
		}

		if (reg_usb_irq_mask & FLD_USB_IRQ_RESET_O)
		{
			reg_usb_irq_mask |= FLD_USB_IRQ_RESET_O; 		//Clear USB reset flag
			myudb_usb_bulkout_ready ();
	    }
		myudb.stall = 0;
	}

	myudb_to_usb ();

	myudb_hci_cmd_from_usb ();

	if (myudb.tick_sync && clock_time_exceed (myudb.tick_sync, 10000))
	{
		myudb.tick_sync = clock_time () | 1;
		log_sync (SL_STACK_EN);
	}
}

void myudb_usb_bulkout_ready ()
{
	reg_usb_ep_ctrl (MYUDB_EDP_OUT_HCI) = FLD_EP_DAT_ACK;
}

void myudb_usb_init(u16 id, void * p_print)
{
	if (!myudb_print_fifo)
	{
		myudb_print_fifo = p_print;
	}


	myudb_print_fifo->wptr = myudb_print_fifo->rptr = 0;

	memset (&myudb, 0, sizeof (myudb));

	myudb.id = id;
	//reg_usb_mask = BIT(7);			//audio in interrupt enable
	//reg_irq_mask |= FLD_IRQ_IRQ4_EN;
	reg_usb_ep_max_size = (128 >> 2);
	reg_usb_ep8_send_thre = 0x40;
	reg_usb_ep8_send_max = 128 >> 3;
	reg_usb_ep_buf_addr (MYUDB_EDP_IN_HCI) = 128;
	reg_usb_ep_buf_addr (MYUDB_EDP_OUT_HCI) = 192;
	reg_usb_ep_buf_addr (MYUDB_EDP_IN_VCD) = 0;
	reg_usb_ep8_fifo_mode = 1;
	reg_usb_mdev &= ~BIT(3);			//vendor command: bRequest[7] = 0

	usbhw_enable_manual_interrupt(FLD_CTRL_EP_AUTO_STD | FLD_CTRL_EP_AUTO_DESC);

	myudb_usb_bulkout_ready ();
	usbhw_data_ep_ack(MYUDB_EDP_IN_HCI); //add log 1st log info
}


#endif ///ending of #if(VCD_EN || DUMP_STR_EN)

