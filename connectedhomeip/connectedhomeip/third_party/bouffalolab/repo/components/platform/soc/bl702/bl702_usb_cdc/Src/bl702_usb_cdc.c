#include "bl702_usb_cdc.h"
#include "usb_device.h"
#include "bl_rtc.h"
#include <yloop_types.h>
#include <vfs.h>
#include <vfs_file.h>
#include <hal/soc/soc.h>


#define USB_CDC_MONITOR_CNT        3


typedef struct vfs_uart_dev {
    hosal_uart_dev_t uart;
    void         *rx_ringbuf_handle;
    void         *tx_ringbuf_handle;
    uint32_t     rx_buf_size;
    uint32_t     tx_buf_size;
    void         *mutex;
    void         *poll_cb;
    void         *fd;
    void         *poll_data;
    void         *taskhdl;
    uint8_t      read_block_flag;
    void         *priv;
} vfs_uart_dev_t;


static volatile uint8_t usb_cdc_monitor_en = 0;
static volatile uint8_t usb_cdc_port_open = 0;
static int usb_cdc_fd = -1;


static void usb_cdc_poll_notify(void)
{
    file_t *f;
    vfs_uart_dev_t *uart;
    
    if(usb_cdc_fd < 0){
        return;
    }
    
    f = get_file(usb_cdc_fd);
    uart = (vfs_uart_dev_t *)(f->node->i_arg);
    
    if (uart->poll_cb != NULL) {
        ((struct pollfd*)uart->fd)->revents |= POLLIN;
        ((poll_notify_t)uart->poll_cb)(uart->fd, uart->poll_data);
    }
}


void usb_cdc_start(int fd_console)
{
    uint16_t pid;
    uint16_t vid;
    
    EF_Ctrl_Get_Customer_PIDVID(&pid,&vid);
    if(pid==0x0000&&vid==0x0000){
        EF_Ctrl_Get_Chip_PIDVID(&pid,&vid);
        if(pid==0x0000&&vid==0x0000){
            pid = 0xffff;
            vid = 0xffff;
        }
    }
    
    extern uint8_t USBD_FS_DeviceDesc[USB_LEN_DEV_DESC];
    USBD_FS_DeviceDesc[8] = LOBYTE(vid);
    USBD_FS_DeviceDesc[9] = HIBYTE(vid);
    USBD_FS_DeviceDesc[10] = LOBYTE(pid);
    USBD_FS_DeviceDesc[11] = HIBYTE(pid);
    
    usb_cdc_monitor_en = 0;
    usb_cdc_port_open = 0;
    
    USB_Device_Init();
    
    usb_cdc_monitor_en = 1;
    
    usb_cdc_fd = fd_console;
}

void usb_cdc_monitor(void)
{
    static int cnt = 0;
    
    if(!usb_cdc_monitor_en){
        return;
    }
    
    if(SET == USB_Get_IntStatus(USB_INT_EP2_CMD)){
        USB_Clr_IntStatus(USB_INT_EP2_CMD);
        
        usb_cdc_port_open = 1;
        cnt = USB_CDC_MONITOR_CNT;
    }else{
        if(usb_cdc_port_open){
            if(--cnt == 0){
                usb_cdc_port_open = 0;
            }
        }
    }
    
    if(USB_Get_EPx_RX_FIFO_CNT(USB_OUT_EP) > 0){
        if(usb_cdc_port_open){
            usb_cdc_poll_notify();
        }else{
            USB_Clr_EPx_RX_FIFO_Errors(USB_OUT_EP);
        }
    }
}

int usb_cdc_is_port_open(void)
{
    return usb_cdc_port_open;
}

int usb_cdc_read(uint8_t *data, uint32_t len)
{
    uint8_t rxbuf[64];
    uint16_t rxlen = 0;
    
    if(usb_cdc_port_open == 0){
        return -1;
    }
    
    if(SET == USB_Is_EPx_RDY_Free(USB_OUT_EP)){
        rxlen = USB_Get_EPx_RX_FIFO_CNT(USB_OUT_EP);
        
        if(rxlen <= len){
            USB_EPx_Read_Data_From_FIFO(USB_OUT_EP, data, rxlen);
            USB_Set_EPx_Rdy(USB_OUT_EP);
        }else{
            USB_EPx_Read_Data_From_FIFO(USB_OUT_EP, rxbuf, rxlen);
            USB_Set_EPx_Rdy(USB_OUT_EP);
        }
    }
    
    return rxlen;
}

int usb_cdc_write(const uint8_t *data, uint32_t len)
{
    uint16_t txlen = 0;
    
    if(usb_cdc_port_open == 0){
        return -1;
    }
    
    if(SET == USB_Is_EPx_RDY_Free(USB_IN_EP)){
        if(USB_Get_EPx_TX_FIFO_CNT(USB_IN_EP) >= 64){
            txlen = (len >= 64) ? 64 : len;
            
            USB_EPx_Write_Data_To_FIFO(USB_IN_EP, (uint8_t *)data, txlen);
            USB_Set_EPx_Rdy(USB_IN_EP);
        }else{
            USB_Set_EPx_Rdy(USB_IN_EP);  // fix tx fifo cnt issue
        }
    }
    
    return txlen;
}

int usb_cdc_data_send(const uint8_t *data, uint32_t len)
{
    int pos = 0;
    int ret;
    uint32_t timeout = 1000;
    uint64_t rtcRefCnt = bl_rtc_get_counter();
    
    while(pos < len){
        ret = usb_cdc_write(data + pos, len - pos);
        if(ret < 0){
            return -1;
        }
        
        if(bl_rtc_get_delta_time_ms(rtcRefCnt) > timeout){
            return -2;
        }
        
        pos += ret;
    }
    
    return 0;
}

void usb_cdc_stop(void)
{
    usb_cdc_monitor_en = 0;
    usb_cdc_port_open = 0;
    
    USB_HAL_Xcvr_Config(DISABLE);
    USB_HAL_Power_Off();
    BL702_Delay_MS(10);
}

void usb_cdc_restore(void)
{
    USB_Config_Type usbCfg = {
        .EnumOutEn = ENABLE,
        .EnumInEn = ENABLE,
        .EnumMaxPacketSize = USB_MAX_EP0_SIZE,
        .DeviceAddress = 0,
        .SoftwareCtrl = 1,
        .RomBaseDescriptorUsed = 0,
    };
    EP_Config_Type epOutCfg = {
            .EPMaxPacketSize = USB_FS_MAX_PACKET_SIZE,
            .dir = EP_OUT,
            .type = EP_BULK,
    };
    EP_Config_Type epInCfg = {
            .EPMaxPacketSize = USB_FS_MAX_PACKET_SIZE,
            .dir = EP_IN,
            .type = EP_BULK,
    };
    
    usb_cdc_monitor_en = 0;
    usb_cdc_port_open = 0;
    
    USB_HAL_Power_Up();
    USB_HAL_Xcvr_Config(ENABLE);
    
    /* Peripheral GPIO disable */
    /* Peripheral clock disable */
    /* Peripheral interrupt disable */
    USB_HAL_System_Init(DISABLE);
    
    /* Init Device */
    USB_Set_Config(DISABLE,&usbCfg);
    USB_Set_EPx_Config(USB_OUT_EP,&epOutCfg);
    USB_Set_EPx_Config(USB_IN_EP,&epInCfg);
    
    /* USB interrupt enable config */
    USB_IntEn(USB_INT_ALL,DISABLE);                // all
    USB_IntEn(USB_INT_RESET,ENABLE);               // 1
    USB_IntEn(USB_INT_EP0_SETUP_DONE,ENABLE);      // 5
    USB_IntEn(USB_INT_EP0_IN_DONE,ENABLE);         // 7
    USB_IntEn(USB_INT_EP0_OUT_DONE,ENABLE);        // 9
    USB_IntEn(USB_INT_EP1_DONE,ENABLE);            // 11
    USB_IntEn(USB_INT_EP2_CMD,ENABLE);             // 12
    USB_IntEn(USB_INT_EP3_DONE,ENABLE);            // 15
    USB_IntEn(USB_INT_RESET_END,ENABLE);           // 27
    
    /* USB interrupt mask config */
    USB_IntMask(USB_INT_ALL,MASK);                 // all
    USB_IntMask(USB_INT_RESET,UNMASK);             // 1
    USB_IntMask(USB_INT_EP0_SETUP_DONE,UNMASK);    // 5
    USB_IntMask(USB_INT_EP0_IN_DONE,UNMASK);       // 7
    USB_IntMask(USB_INT_EP0_OUT_DONE,UNMASK);      // 9
    /* USB_OUT interrupt */
    USB_IntMask(USB_INT_EP1_DONE,UNMASK);          // 11
    /* USB_IN interrupt */
//    USB_IntMask(USB_INT_EP2_CMD,UNMASK);           // 12
    USB_IntMask(USB_INT_EP3_DONE,UNMASK);          // 15
    USB_IntMask(USB_INT_RESET_END,UNMASK);         // 27
    
    /* Peripheral GPIO init */
    /* Peripheral clock init */
    /* Peripheral interrupt init */
    USB_HAL_System_Init(ENABLE);
    
    /*Clear pending interrupts*/
    USB_Clr_IntStatus(USB_INT_ALL);
    
    USB_Enable();
    
    USB_Set_EPx_Busy(USB_OUT_EP);
    USB_Set_EPx_Busy(USB_IN_EP);
    USB_Set_EPx_Rdy(USB_OUT_EP);
    
    usb_cdc_monitor_en = 1;
}

void usb_cdc_data_out_irq(uint8_t epnum)
{
    usb_cdc_poll_notify();
}
