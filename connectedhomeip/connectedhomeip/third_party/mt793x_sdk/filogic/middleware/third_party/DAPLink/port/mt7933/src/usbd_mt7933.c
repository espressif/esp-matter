/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * Copyright  (C) 2021  MediaTek Inc. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// mt7933 includes
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "mtu3.h"
#include "usbdcore.h"
#include "usbd.h"
#include "u_serial.h"

// daplink includes
#include "usb_def.h"
#include "usbd_hw.h"
#include "usbd_event.h"
#include "usbd_core.h"

#if 0
#define DEBUG_PRINT(x...) printf("[USBD_DEBUG] " x)
#else
#define DEBUG_PRINT(x...)
#endif
#define ERROR_PRINT(x...) printf("[USBD_ERROR] " x)
#define INFO_PRINT(x...) printf("[USBD_INFO] " x)

#define USBD_NUM_CONFIGS (1)
static struct gadget_dev mt_usb_dev[1];
static struct mt_config  mt_dap_config[USBD_NUM_CONFIGS];
#define NUM_STR_TABLE (16)
static struct string_descriptor *usb_string_table[NUM_STR_TABLE];
static int serial_online = 0;

#define EP_ADDRESS_MASK (0x7F)
#define EP_MAX_NUM ((MAX_EP_NUM * 2)+1)
/* bits used in ep interrupts etc */
#define EPT_RX(n) (1 << ((n) + 16))
#define EPT_TX(n) (1 << (n))
static udc_endpoint_t *g_udc_ep[EP_MAX_NUM];
static unsigned char g_endpoint_address_idx_map[EP_MAX_NUM];
static unsigned int  g_endpoint_map_idx = 1; // bypss EP0
#define EP0_READ_BUFFER_SIZE (512)
static uint32_t g_ep0_read_buf_cnt = 0;
static uint8_t g_ep0_read_buf[EP0_READ_BUFFER_SIZE];
static const uint32_t g_sof_ms_timeout = 1; //1ms
static uint32_t g_sof_ms_timer_id;

static void async_usb_read(udc_endpoint_t *p_ep);

static inline void* va_to_pa(void *p)
{
    int pa = (int)p;
    if(pa & 0x08000000) {
        pa = (pa & 0x00FFFFFF) | 0x80000000;
        p = (void *) pa;
    }
    return p;
}

static udc_endpoint_t* USBD_GetEp(U32 num)
{
    uint32_t i;
    for(i = 1; i < g_endpoint_map_idx; ++i) {
        if(g_endpoint_address_idx_map[i] == num) {
            return g_udc_ep[i];
        }
    }

    return NULL;
}

static int USBD_NewEp(U32 num, udc_endpoint_t *p_ep)
{
    uint32_t i;
    for(i = 1; i < g_endpoint_map_idx; ++i) {
        if(g_endpoint_address_idx_map[i] == num) {
            return -1; // already allocated
        }
    }

    if(g_endpoint_map_idx < EP_MAX_NUM) {
        g_endpoint_address_idx_map[g_endpoint_map_idx] = num;
        g_udc_ep[g_endpoint_map_idx] = p_ep;
        g_endpoint_map_idx++;
    } else {
        return -2; // full
    }

    return 0;
}

static void USBD_SetEpEvent(U32 num, U32 event)
{
#ifdef __RTX
    if (USBD_RTX_EPTask[num]) {
        isr_evt_set(event, USBD_RTX_EPTask[num]);
    }
#else
    if (USBD_P_EP[num]) {
        USBD_P_EP[num](event);
    }
#endif
}

static void sof_1ms_gpt_callback(void *user_data)
{
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;

    if(u3d->usb_online == 1) {
        if (USBD_P_SOF_Event) {
            USBD_P_SOF_Event();
        }
        /*restart gpt timer*/
        hal_gpt_sw_start_timer_ms(g_sof_ms_timer_id, g_sof_ms_timeout, sof_1ms_gpt_callback, NULL);
    }
}

static int usbd_ep0_rx_setup(struct udc_request *req)
{
    extern void USBD_EndPoint0(U32 event);
    memcpy(g_ep0_read_buf, req->buffer, req->length);
    g_ep0_read_buf_cnt = req->length;
    // run USBD_EP0Data out
    USBD_EndPoint0(USBD_EVT_OUT);
    return 0;
}

static int usbd_ep0_setup(struct udc_request *req, const struct usb_setup *usb_ctrl)
{
    extern void USBD_EndPoint0(U32 event);
    extern USBD_EP_DATA USBD_EP0Data;
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    memcpy(g_ep0_read_buf, usb_ctrl, sizeof(struct usb_setup));
    g_ep0_read_buf_cnt = sizeof(struct usb_setup);
    // init USBD_EP0Data
    USBD_EP0Data.pData = 0;
    USBD_EP0Data.Count = 0;
    // setup USBD_EP0Data
    USBD_EndPoint0(USBD_EVT_SETUP);
    DEBUG_PRINT("USBD_EP0Data: %x, %u\n", USBD_EP0Data.pData, USBD_EP0Data.Count);
    if(USBD_EP0Data.Count == 0) {
        /**
         * WARN: hack usb mtu3.c
         * If ack command, the ep0_state should be EP0_IDLE.
         */
        u3d->ep0_state = EP0_IDLE;
    } else if(u3d->ep0_state == EP0_TX){
        memcpy(req->buffer, USBD_EP0Data.pData, USBD_EP0Data.Count);
        req->length = USBD_EP0Data.Count;
    }
    return 0;
}

static int usbd_set_alt(struct udc_request *req, const struct usb_setup *usb_ctrl)
{
    // what is this for?
    return 0;
}

static void usbd_udev_notify(struct gadget_dev *gdev, unsigned int event)
{
    uint32_t i;
    switch (event) {
    case UDC_EVENT_OFFLINE:
        serial_online = 0;
        break;
    case UDC_EVENT_ONLINE:
        for(i = 1; i < g_endpoint_map_idx; ++i) {
            if(g_endpoint_address_idx_map[i] != 0 &&
              !(g_endpoint_address_idx_map[i] & USB_ENDPOINT_DIRECTION_MASK)) {
                // qeuene QMU to receive
                async_usb_read(g_udc_ep[i]);
            }
        }
        serial_online = 1;
        break;
    default:
        break;
    }
}

static void usbd_create_string_table(void)
{
    extern const char *info_get_unique_id_string_descriptor(void);
    extern U8 USBD_StringDescriptor[];
    U32 n;
    U8  *pD = (U8 *)USBD_StringDescriptor;
    for (n = 0; ((USB_STRING_DESCRIPTOR *)pD)->bLength > 0 && n < NUM_STR_TABLE; n++) {
        // unique id string descriptor
        if (n == 3) {
            usb_string_table[n] = (struct string_descriptor *) info_get_unique_id_string_descriptor();
        } else {
            usb_string_table[n] = (struct string_descriptor *) pD;
        }
        pD += ((USB_STRING_DESCRIPTOR *)pD)->bLength;
    }
}

static void usbd_assign_fs_descs(void)
{
    extern U8 USBD_DeviceQualifier[];
    extern U8 USBD_ConfigDescriptor[];

    mt_usb_dev->speed = SSUSB_SPEED_FULL;
    mt_usb_dev->dev_qualifier_desc = (struct device_qualifier_descriptor *)USBD_DeviceQualifier;
    memset(&mt_dap_config[0], 0, sizeof(struct mt_config));
    mt_dap_config[0].config_desc = (struct configuration_descriptor *) USBD_ConfigDescriptor;
    mt_dap_config[0].config_desc->bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION;
    mt_dap_config[0].raw = (char *) USBD_ConfigDescriptor;
    mt_usb_dev->configs = USBD_NUM_CONFIGS;
    mt_usb_dev->conf_array = mt_dap_config;
}

static void usbd_assign_hs_descs(void)
{
    extern U8 USBD_DeviceQualifier_HS[];
    extern U8 USBD_ConfigDescriptor_HS[];

    mt_usb_dev->speed = SSUSB_SPEED_HIGH;
    mt_usb_dev->dev_qualifier_desc = (struct device_qualifier_descriptor *)USBD_DeviceQualifier_HS;

    memset(&mt_dap_config[0], 0, sizeof(struct mt_config));
    mt_dap_config[0].config_desc = (struct configuration_descriptor *) USBD_ConfigDescriptor_HS;
    mt_dap_config[0].config_desc->bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION;
    mt_dap_config[0].raw = (char *) USBD_ConfigDescriptor_HS;
    mt_usb_dev->configs = USBD_NUM_CONFIGS;
    mt_usb_dev->conf_array = mt_dap_config;
}

static void usbd_assign_descs(USB_SPEED speed)
{
    switch (speed) {
    case SSUSB_SPEED_FULL:
        usbd_assign_fs_descs();
        USBD_HighSpeed = __FALSE;
        INFO_PRINT("SSUSB_SPEED_FULL\r\n");
        break;
    case SSUSB_SPEED_HIGH:
        usbd_assign_hs_descs();
        USBD_HighSpeed = __TRUE;
        INFO_PRINT("SSUSB_SPEED_HIGH\r\n");
        break;
    case SSUSB_SPEED_SUPER:
    case SSUSB_SPEED_SUPER_PLUS:
    default:
        INFO_PRINT("SSUSB NOT SUPPORTED SPEED\r\n");
        assert(0);
        break;
    }
}

static int usbd_bind(void)
{
    extern USB_SETUP_PACKET USBD_SetupPacket;
    extern BOOL USBD_ReqSetConfiguration(void);
    USBD_SetupPacket.bmRequestType.Recipient = 0;
    USBD_SetupPacket.bmRequestType.Type = 0;
    USBD_SetupPacket.bmRequestType.Dir = 0;
    USBD_SetupPacket.bRequest = 9;
    USBD_SetupPacket.wValue = 1;
    USBD_SetupPacket.wIndex = 0;
    USBD_SetupPacket.wLength = 0;
    // configEP by desc
    USBD_ReqSetConfiguration();
    return 0;
}

static void usbd_unbind(void)
{
}

static void usbd_device_init(void)
{
    extern U8 USBD_DeviceDescriptor[];
    extern U8 USBD_BinaryObjectStoreDescriptor[];
    U8  *pD = USBD_BinaryObjectStoreDescriptor;
    /* device instance initialization */
	mt_usb_dev->name = "dap";
    mt_usb_dev->eps = g_udc_ep;
    mt_usb_dev->num_eps = EP_MAX_NUM;
    mt_usb_dev->notify = usbd_udev_notify;
    mt_usb_dev->assign_descs = usbd_assign_descs;
    mt_usb_dev->dev_desc = (struct device_descriptor *) USBD_DeviceDescriptor;
    mt_usb_dev->bos_desc = (struct bos_descriptor *) pD;
    mt_usb_dev->ext_cap_desc = NULL;
    mt_usb_dev->ss_cap_desc = NULL;
    pD += ((USB_BINARY_OBJECT_STORE_DESCRIPTOR *)pD)->bLength;
    if(((USB_BINARY_OBJECT_STORE_DESCRIPTOR *)pD)->bLength != 0){
        mt_usb_dev->ext_cap_desc = (struct ext_cap_descriptor *)pD;
        pD += ((USB_BINARY_OBJECT_STORE_DESCRIPTOR *)pD)->bLength;
    }
    if(((USB_BINARY_OBJECT_STORE_DESCRIPTOR *)pD)->bLength != 0){
        mt_usb_dev->ss_cap_desc = (struct ss_cap_descriptor *)pD;
    }
    usbd_create_string_table();
    mt_usb_dev->string_table = usb_string_table;
    usbd_assign_fs_descs();

    mt_usb_dev->driver.setup = usbd_ep0_setup;
    mt_usb_dev->driver.rx_setup = usbd_ep0_rx_setup;
    mt_usb_dev->driver.set_alt = usbd_set_alt;
    mt_usb_dev->driver.bind = usbd_bind;
    mt_usb_dev->driver.unbind = usbd_unbind;
}

/*
 *  USB Device Initialize Function
 *   Called by the User to initialize USB
 *    Return Value:    None
 */
void USBD_Init(void)
{
    int ret;

    USBD_HighSpeed = __TRUE;

    memset((void *)g_udc_ep, 0, sizeof(g_udc_ep));

    usbd_device_init();

	ret = udc_register_gadget(mt_usb_dev, 0);
	if (ret != UDC_CLASS_ONLINE) {
		ret = udc_init(0);
        if (ret) {
            ERROR_PRINT("USBD_Init failed with error: %d.\r\n", ret);
        }
	}

    usbd_bind();
}


/*
 *  USB Device Connect Function
 *   Called by the User to Connect/Disconnect USB Device
 *    Parameters:      con:   Connect/Disconnect
 *    Return Value:    None
 */

void USBD_Connect(BOOL con)
{
    DEBUG_PRINT("USBD_Connect\r\n");
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    if(con)
    {
        udc_pullup(u3d, true);
        u3d->usb_online = 1;
        // Create SOF timer
        hal_gpt_sw_get_timer(&g_sof_ms_timer_id);
        hal_gpt_sw_start_timer_ms(g_sof_ms_timer_id, g_sof_ms_timeout, sof_1ms_gpt_callback, NULL);
    }
    else
    {
        udc_pullup(mt_usb_dev->private, false);
        u3d->usb_online = 0;
    }
}


/*
 *  USB Device Reset Function
 *   Called automatically on USB Device Reset
 *    Return Value:    None
 */

void USBD_Reset(void)
{
    // Reset by driver
}


/*
 *  USB Device Suspend Function
 *   Called automatically on USB Device Suspend
 *    Return Value:    None
 */

void USBD_Suspend(void)
{
    // Suspend by driver
}


/*
 *  USB Device Resume Function
 *   Called automatically on USB Device Resume
 *    Return Value:    None
 */

void USBD_Resume(void)
{
    DEBUG_PRINT("USBD_Resume\r\n");
    /* Performed by Hardware                                                    */
}


/*
 *  USB Device Remote Wakeup Function
 *   Called automatically on USB Device Remote Wakeup
 *    Return Value:    None
 */

void USBD_WakeUp(void)
{
    DEBUG_PRINT("USBD_WakeUp\r\n");

}


/*
 *  USB Device Remote Wakeup Configuration Function
 *    Parameters:      cfg:   Device Enable/Disable
 *    Return Value:    None
 */

void USBD_WakeUpCfg(BOOL cfg)
{
    DEBUG_PRINT("USBD_WakeUpCfg\r\n");
    /* Not needed                                                               */
}


/*
 *  USB Device Set Address Function
 *    Parameters:      adr:   USB Device Address
 *                     setup: Called in setup stage (!=0), else after status stage
 *    Return Value:    None
 */

void USBD_SetAddress(U32 adr, U32 setup)
{
    DEBUG_PRINT("USBD_SetAddress: %u %u\r\n", adr, setup);
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    u32 value;
    if(setup) {
        u3d->address = adr;
        value = ssusb_readl(u3d->mac_base, U3D_DEVICE_CONF);
        value &= ~DEV_ADDR_MSK;
        value |= DEV_ADDR(u3d->address);
        ssusb_writel(u3d->mac_base, U3D_DEVICE_CONF, value);
    }
}


/*
 *  USB Device Configure Function
 *    Parameters:      cfg:   Device Configure/Deconfigure
 *    Return Value:    None
 */

void USBD_Configure(BOOL cfg)
{
    DEBUG_PRINT("USBD_Configure\r\n");
}


/*
 *  Configure USB Device Endpoint according to Descriptor
 *    Parameters:      pEPD:  Pointer to Device Endpoint Descriptor
 *    Return Value:    None
 */

void USBD_ConfigEP(USB_ENDPOINT_DESCRIPTOR *pEPD)
{

    DEBUG_PRINT("USBD_ConfigEP\r\n");
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    unsigned int num, type;
    unsigned char inout;
    udc_endpoint_t *p_ep = NULL;

    if ((pEPD->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK)) {
        num = pEPD->bEndpointAddress & ~0x80;
        inout = USB_DIR_IN;
    } else {
        num = pEPD->bEndpointAddress;
        inout = USB_DIR_OUT;
    }

    type = pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK;
    if(num > MAX_EP_NUM) {
        ERROR_PRINT("EP %u is not supported!!.\r\n", num);
        assert(0);
        return;
    }

    p_ep = USBD_GetEp(pEPD->bEndpointAddress);
    if(p_ep != NULL) {
        ERROR_PRINT("EP(%u) is already allocated.\n", num);
        return;
    } else {
        p_ep = udc_endpoint_alloc(u3d, type, inout);
        if(p_ep == NULL) {
            ERROR_PRINT("EP(%u): allocate failed\n", num);
            return;
        } else {
            p_ep->req = udc_request_alloc(p_ep);
            if (!p_ep->req) {
                ERROR_PRINT("%s: alloc req fail\n", p_ep->name);
                udc_endpoint_free(p_ep);
            } else {
                // update ep by description
                p_ep->in = !!inout;
                p_ep->binterval = pEPD->bInterval;
                p_ep->type = pEPD->bmAttributes;
                p_ep->maxpkt = pEPD->wMaxPacketSize;
                p_ep->num = num;
                p_ep->desc = (struct endpoint_descriptor *)pEPD;

                if(USBD_NewEp(pEPD->bEndpointAddress, p_ep) < 0) {
                    ERROR_PRINT("Out of EP: EP address(%x).\n", pEPD->bEndpointAddress);
                }
            }
        }
    }
}


/*
 *  Set Direction for USB Device Control Endpoint
 *    Parameters:      dir:   Out (dir == 0), In (dir <> 0)
 *    Return Value:    None
 */

void USBD_DirCtrlEP(U32 dir)
{
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    u3d->ep0_state = (dir == 0)?EP0_RX:EP0_TX;
    DEBUG_PRINT("USBD_DirCtrlEP: %s\r\n", (dir==0)?"RX":"TX");
}

static void read_req_complete(udc_request_t *req, unsigned int actual, int status)
{
    struct mu3d_req *mreq = to_mu3d_req(req);
    udc_endpoint_t *p_ep = mreq->ept;
    unsigned int num = p_ep->num;
    req->length = actual;

    if (req->status < 0) {
        ERROR_PRINT("%s req:%p transaction failed\n", __func__, req);
    } else {
        DEBUG_PRINT("Trigger EP %u task to read data.\r\n", num);
        USBD_SetEpEvent(num, USBD_EVT_OUT);
    }
}

static void async_usb_read(udc_endpoint_t *p_ep)
{
    int ret;
    udc_request_t *req = p_ep->req;
    void *buf;
    unsigned int len = p_ep->maxpkt;
    if (len > MAX_USBFS_BULK_SIZE) {
        ERROR_PRINT("EP: request bulk size to large (%u)\n", len);
        len = MAX_USBFS_BULK_SIZE;
    }
    /* enable GDMA for read event */
    if (req->buffer == NULL) {
        DEBUG_PRINT("EP: allocate memory\n");
        buf = pvPortMallocNC(len);
        req->buffer = buf;
        if (buf == NULL) {
            ERROR_PRINT("EP: memory is not enough (%u)\n", len);
        }
    } else {
        DEBUG_PRINT("EP: memory is already allocated\n");
    }
    req->length = len;
    req->complete = read_req_complete;
    ret = udc_request_queue(p_ep, req);
    if (ret < 0) {
        ERROR_PRINT("EP: queue failed, ret:%d \r\n", ret);
    }
}

/*
 *  Enable USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */
void USBD_EnableEP(U32 EPNum)
{
    /* reserved */
#if 0
    DEBUG_PRINT("USBD_EnableEP: %x\r\n", EPNum);
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    udc_endpoint_t *p_ep = NULL;
    p_ep = USBD_GetEp(EPNum);
    if(p_ep == NULL) {
        ERROR_PRINT("EP %u is not configured.\r\n", EPNum);
        return;
    }
    udc_endpoint_enable(u3d, p_ep);

    if (!(EPNum & USB_ENDPOINT_DIRECTION_MASK)) {
        // create read event if QMU finished
        async_usb_read(p_ep);
    } // if
#endif
}


/*
 *  Disable USB Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_DisableEP(U32 EPNum)
{
    /* reserved */
#if 0
    DEBUG_PRINT("USBD_DisableEP: %x\r\n", EPNum);
    udc_endpoint_t *p_ep = NULL;
    p_ep = USBD_GetEp(EPNum);
    if(p_ep == NULL) {
        ERROR_PRINT("EP %u is not configured.\r\n", EPNum);
        return;
    }
    udc_endpoint_disable(p_ep);
#endif
}


/*
 *  Reset USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ResetEP(U32 EPNum)
{
    /* reserved */
}


/*
 *  Set Stall for USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_SetStallEP(U32 EPNum)
{
    DEBUG_PRINT("USBD_SetStallEP: %x\r\n", EPNum);
    if ( 0x00 == (EPNum & 0x7F)) {
        // EP0 stall is controlled by driver
        assert(0);
    }
}


/*
 *  Clear Stall for USB Device Endpoint
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ClrStallEP(U32 EPNum)
{
    DEBUG_PRINT("USBD_ClrStallEP: %x\r\n", EPNum);
    if ( 0x00 == (EPNum & 0x7F)) {
        // EP0 stall is controlled by driver
        assert(0);
    }
}


/*
 *  Clear USB Device Endpoint Buffer
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void USBD_ClearEPBuf(U32 EPNum)
{
    DEBUG_PRINT("USBD_ClrEPBuf: %x\r\n", EPNum);
    /* reserved */
}

static void write_req_complete(udc_request_t *req, unsigned int actual, int status)
{
    struct mu3d_req *mreq = to_mu3d_req(req);
    udc_endpoint_t *p_ep = mreq->ept;
    unsigned int num = p_ep->num;

#ifdef SUPPORT_QMU
//	portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;
//	if (pdPASS != xSemaphoreGiveFromISR(req->xSemaphore, &pxHigherPriorityTaskWoken))
//		ERROR_PRINT("%s: give semphore fail\n", __func__);
#endif

    vPortFreeNC(req->buffer);
    req->buffer = NULL;
    req->length = actual;
    if (req->status < 0) {
        ERROR_PRINT("%s req:%p transaction failed\n", __func__, req);
    } else {
        DEBUG_PRINT("Trigger EP %u task to write data.\r\n", num);
        USBD_SetEpEvent(num, USBD_EVT_IN);
    }
}

int sync_usb_write(void *_buf, unsigned int len, udc_endpoint_t *p_ep)
{
    int ret;
    udc_request_t *req = p_ep->req;
    void *buf;
    if (len > MAX_USBFS_BULK_SIZE) {
        ERROR_PRINT("EP: request bulk size to large (%u)\n", len);
        len = MAX_USBFS_BULK_SIZE;
    }
    /* enable GDMA for read event */
    buf = pvPortMallocNC(len);
    if (buf == NULL) {
        ERROR_PRINT("EP: memory is not enough (%u)\n", len);
        goto oops;
    }

    memcpy(buf, _buf, len);

    req->buffer = buf;
    req->length = len;
    req->complete = write_req_complete;
    ret = udc_request_queue(p_ep, req);
    if (ret < 0) {
        ERROR_PRINT("EP: queue failed, ret:%d \r\n", ret);
        goto oops;
    }

#ifndef SUPPORT_QMU
    vPortFreeNC(buf);
    buf = NULL;
#endif

    return req->length;

oops:
    vPortFreeNC(buf);
    buf = NULL;
    return -1;
}

/*
 *  Read USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *    Return Value:    Number of bytes read
 */

U32 USBD_ReadEP(U32 EPNum, U8 *pData, U32 bufsz)
{
    DEBUG_PRINT("USBD_ReadEP: %x\r\n", EPNum);
    U32 cnt = 0;
    udc_endpoint_t *p_ep = NULL;
    udc_request_t *req = NULL;

    if((EPNum & EP_ADDRESS_MASK) == 0) {
        assert(g_ep0_read_buf_cnt > 0);
        cnt = g_ep0_read_buf_cnt;
        memcpy(pData, g_ep0_read_buf, cnt);
        g_ep0_read_buf_cnt = 0;
    } else {
        p_ep = USBD_GetEp(EPNum);
        if(p_ep == NULL) {
            ERROR_PRINT("EP %u is not configured.\r\n", EPNum);
            return 0;
        }
        req = p_ep->req;
        if(req && req->buffer && req->length > 0) {
            cnt = req->length;
            memcpy(pData, req->buffer, cnt);
        }
    }

    if((EPNum & EP_ADDRESS_MASK) == 0) {
        ;
    } else {
        async_usb_read(p_ep);
    }

    return (cnt);
}


/*
 *  Write USB Device Endpoint Data
 *    Parameters:      EPNum: Device Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *                     pData: Pointer to Data Buffer
 *                     cnt:   Number of bytes to write
 *    Return Value:    Number of bytes written
 */

U32 USBD_WriteEP(U32 EPNum, U8 *pData, U32 cnt)
{
    DEBUG_PRINT("USBD_WriteEP: %x %u\r\n", EPNum, cnt);
    int ret = 0;

    if((EPNum & EP_ADDRESS_MASK) == 0) { // EP0
        ; // bypass, just need the buffer data
    } else {
        udc_endpoint_t *p_ep = USBD_GetEp(EPNum);
        if(p_ep == NULL) {
            ERROR_PRINT("EP %u is not configured.\r\n", EPNum);
            return 0;
        }

        if(cnt > 0) {
            ret = sync_usb_write(pData, cnt, p_ep);
            if(ret < 0) {
                ERROR_PRINT("EP %u write failed: %d.\r\n", EPNum, ret);
                ret = 0;
            }
        } else {
            ret = 0;
        }
    }

    return (U32)(ret);
}


/*
 *  Get USB Device Last Frame Number
 *    Parameters:      None
 *    Return Value:    Frame Number
 */

U32 USBD_GetFrame(void)
{
    struct mu3d *u3d = (struct mu3d *)mt_usb_dev->private;
    void *base = u3d->mac_base;
    U32 n = ssusb_readl(base, U3D_USB20_FRAME_NUM);
    DEBUG_PRINT("USBD_GetFrame %u \r\n", n);
    return n;
}

void USBD_Handler(void)
{

}

