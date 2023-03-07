#include "usb_device.h"

USBD_HandleTypeDef hUsbDeviceFS = {
    .id = 0,
    .dev_config = 0,
    .dev_default_config = 0,
    .dev_config_status = 0,
    .dev_speed = USBD_SPEED_FULL,
    .ep0_state = 0,
    .ep0_data_len = 0,
    .dev_state = 0,
    .dev_old_state = 0,
    .dev_address = 0,
    .dev_connection_status = 0,
    .dev_test_mode = 0,
    .dev_remote_wakeup = 0,
    .request = {
        .bmRequest = 0,
        .bRequest = 0,
        .wValue = 0,
        .wIndex = 0,
        .wLength = 0
    },
    .pDesc = NULL,
    .pClass = NULL,
    .pClassData = NULL,
    .pUserData = NULL,
    .pData = NULL
};

void USB_Device_Init(void)
{
    USBD_DbgLog("\tUSBD_Init()\r\n");
    if(USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK){
        USB_Error_Handler();
    }
    
    USBD_DbgLog("\tUSBD_RegisterClass()\r\n");
    if(USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK){
        USB_Error_Handler();
    }
    
    USBD_DbgLog("\tUSBD_CDC_RegisterInterface()\r\n");
    if(USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK){
        USB_Error_Handler();
    }
    
    USBD_DbgLog("\tUSBD_Start()\r\n");
    if(USBD_Start(&hUsbDeviceFS) != USBD_OK){
        USB_Error_Handler();
    }
}


void USB_Error_Handler(void)
{
    USBD_DbgLog("USB_Device_Init error.\r\n");
}


