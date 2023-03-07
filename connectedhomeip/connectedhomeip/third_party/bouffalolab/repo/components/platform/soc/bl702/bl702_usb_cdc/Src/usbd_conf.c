#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "bl702_hal_pcd.h"

//#define ENABLE_LPM_INT
//#define ENABLE_SOF3MS_INT
//#define ENABLE_ERROR_INT

PCD_HandleTypeDef hpcd_USB_FS = {
    .Instance = NULL,
    .Init = {
        .dev_endpoints = 0,
        .speed = 0,
        .ep0_mps = 0,
        .phy_itface = 0,
        .Sof_enable = 0,
        .low_power_enable = 0,
        .lpm_enable = 0,
        .battery_charging_enable = 0
    },
    .USB_Address = 0,
    .Lock = 0,
    .State = 0,
    .pData = NULL
};
void USB_Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
static USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);

static void SystemClockConfig_Resume(void);

extern void SystemClock_Config(void);

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/
/* MSP Init */
void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
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
    
    if(((uint32_t)pcdHandle->Instance)==USB_BASE){
        USB_HAL_Power_Off();
        BL702_Delay_MS(10);
        USB_HAL_Power_Up();
        
        /* reset USB xcvr */
        USB_HAL_Xcvr_Config(DISABLE);
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
//        USB_IntMask(USB_INT_EP2_CMD,UNMASK);           // 12
        USB_IntMask(USB_INT_EP3_DONE,UNMASK);          // 15
        USB_IntMask(USB_INT_RESET_END,UNMASK);         // 27

#ifdef ENABLE_LPM_INT
        USB_IntEn(USB_INT_LPM_PACKET,ENABLE);
        USB_IntEn(USB_INT_LPM_WAKEUP,ENABLE);
        USB_IntMask(USB_INT_LPM_PACKET,UNMASK);
        USB_IntMask(USB_INT_LPM_WAKEUP,UNMASK);

        USB_LPM_Enable();
        USB_Set_LPM_Default_Response(USB_LPM_DEFAULT_RESP_ACK);
#endif

#ifdef ENABLE_SOF3MS_INT
        /* disable sof3ms until reset_end */
        USB_IntEn(USB_INT_LOST_SOF_3_TIMES,DISABLE);
        USB_IntMask(USB_INT_LOST_SOF_3_TIMES,MASK);

        /* recommended enable sof3ms after reset_end */
        USB_IntEn(USB_INT_LOST_SOF_3_TIMES,ENABLE);
		USB_IntMask(USB_INT_LOST_SOF_3_TIMES,UNMASK);
#endif

#ifdef ENABLE_ERROR_INT
        USB_IntEn(USB_INT_ERROR,ENABLE);
        USB_IntMask(USB_INT_ERROR,UNMASK);
#endif

        if(pcdHandle->Init.low_power_enable == 1){
            /* Enable USB low power interrupt, not support */
        }
        if(pcdHandle->Init.lpm_enable == 1){
            /* Enable USB LPM interrupt */
            USB_HAL_Interrupts_LPM_Init(ENABLE);
        }

        /* Peripheral GPIO init */
        /* Peripheral clock init */
        /* Peripheral interrupt init */
        USB_HAL_System_Init(ENABLE);

        /*Clear pending interrupts*/
        USB_Clr_IntStatus(USB_INT_ALL);
        
        pcdHandle->State= HAL_PCD_STATE_READY;
        
#if 0
        /* for FPGA */
        /* usb_sus */
        *(volatile uint32_t *)(0x40000228) = *(volatile uint32_t *)(0x40000228) & ~(1<<22);
        /* usb_spd */
        *(volatile uint32_t *)(0x40000228) = *(volatile uint32_t *)(0x40000228) | (1<<21);
        /* usb_enum */
        *(volatile uint32_t *)(0x40000228) = *(volatile uint32_t *)(0x40000228) | (1<<20);
#endif

        USB_Enable();
        
        USB_Set_EPx_Busy(USB_OUT_EP);
        USB_Set_EPx_Busy(USB_IN_EP);
        USB_Set_EPx_Rdy(USB_OUT_EP);
        //USB_Set_EPx_Rdy(EP_ID0);
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{
    if(((uint32_t)pcdHandle->Instance)==USB_BASE){
        USB_HAL_Power_Off();
        USB_HAL_Xcvr_Config(DISABLE);
        
        /* Peripheral GPIO disable */
        /* Peripheral clock disable */
        /* Peripheral interrupt disable */
        USB_HAL_System_Init(DISABLE);
        
        USB_HAL_Interrupts_LPM_Init(DISABLE);
    }
}

/**
  * @brief  Setup stage callback
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SetupStage((USBD_HandleTypeDef*)hpcd->pData, (uint8_t *)hpcd->Setup);
}

/**
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataOutStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage((USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_SOF((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{ 
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    if( hpcd->Init.speed != PCD_SPEED_FULL){
        USB_Error_Handler();
    }
    /* Set Speed. */
    USBD_LL_SetSpeed((USBD_HandleTypeDef*)hpcd->pData, speed);
    hpcd->BLSetAddressFlag = 0;
    /* Reset Device. */
    USBD_LL_Reset((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    /* Inform USB library that core enters in suspend Mode. */
    USBD_LL_Suspend((USBD_HandleTypeDef*)hpcd->pData);
    /* Enter in STOP mode. */
    if (hpcd->Init.low_power_enable){
        /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
    }
}

/**
  * @brief  Resume callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    if (hpcd->Init.low_power_enable){
        /* Reset SLEEPDEEP bit of Cortex System Control Register. */
        SystemClockConfig_Resume();
    }
    
    USBD_LL_Resume((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  ISOOUTIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
  * @brief  ISOINIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete((USBD_HandleTypeDef*)hpcd->pData, epnum);
}

/**
  * @brief  Connect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevConnected((USBD_HandleTypeDef*)hpcd->pData);
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    USBD_LL_DevDisconnected((USBD_HandleTypeDef*)hpcd->pData);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
  * @brief  Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    /* Init USB Ip. */
    /* Link the driver to the stack. */
    hpcd_USB_FS.pData = pdev;
    pdev->pData = &hpcd_USB_FS;

    hpcd_USB_FS.Instance = (PCD_TypeDef *)USB_BASE;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.low_power_enable = ENABLE;
    hpcd_USB_FS.Init.lpm_enable = ENABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
    hpcd_USB_FS.BLSetAddressFlag = 0;
    USBD_DbgLog("\t\t\tHAL_PCD_Init()\r\n");
    if (HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK){
        USB_Error_Handler( );
    }

    /* Host->Device  standard  Device */
    /* Host->Device  standard  Interface */
    /* Device->Host  standard  Device */
    /* Device->Host  standard  Interface */
    /* Device->Host  standard  EndPoint */

    /* USER CODE BEGIN EndPoint_Configuration */
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData,EP_ID0,PCD_SNG_BUF,PCD_OUT_BUF,(uint32_t)ep0_out_data_buff,0);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData,EP_ID0,PCD_SNG_BUF,PCD_IN_BUF,(uint32_t)ep0_in_data_buff,0);
    /* USER CODE END EndPoint_Configuration */
    /* USER CODE BEGIN EndPoint_Configuration_CDC */
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData,EP_ID1,PCD_SNG_BUF,PCD_OUT_BUF,(uint32_t)ep1_out_data_buff,0);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData,EP_ID2,PCD_SNG_BUF,PCD_IN_BUF,(uint32_t)ep2_in_data_buff,0);
    HAL_PCDEx_PMAConfig((PCD_HandleTypeDef*)pdev->pData,EP_ID3,PCD_SNG_BUF,PCD_OUT_BUF,(uint32_t)ep3_out_data_buff,0);
    /* USER CODE END EndPoint_Configuration_CDC */
    
    return USBD_OK;
}

/**
  * @brief  De-Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_DeInit(pdev->pData);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status; 
}

/**
  * @brief  Starts the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Start(pdev->pData);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
  * @brief  Stops the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Stop(pdev->pData);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
  * @brief  Opens an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  ep_type: Endpoint type
  * @param  ep_mps: Endpoint max packet size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
  * @brief  Closes an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;  
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;  
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;  
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);  

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status; 
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*) pdev->pData;

    if((ep_addr & 0x80) == 0x80){
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall; 
    }else{
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall; 
    }
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  dev_addr: Device address
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;  
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size    
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status;    
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

    usb_status =  USBD_Get_USB_Status(hal_status);

    return usb_status; 
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

/**
  * @brief  Delays routine for the USB device library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);
}

/**
  * @brief  Static single allocation.
  * @param  size: Size of allocated memory
  * @retval None
  */
void *USBD_static_malloc(uint32_t size)
{
    static uint32_t mem[(sizeof(USBD_CDC_HandleTypeDef)/4)+1];/* On 32-bit boundary */

    return mem;
}

/**
  * @brief  Dummy memory free
  * @param  p: Pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p)
{

}

/* USER CODE BEGIN 5 */
/**
  * @brief  Configures system clock after wake-up from USB Resume CallBack:
  *         enable HSI, PLL and select PLL as system clock source.
  * @retval None
  */
static void SystemClockConfig_Resume(void)
{
    SystemClock_Config();
}
/* USER CODE END 5 */

/**
  * @brief  Retuns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
    USBD_StatusTypeDef usb_status = USBD_OK;

    switch (hal_status){
        case HAL_OK :
            usb_status = USBD_OK;
            break;
            
        case HAL_ERROR :
            usb_status = USBD_FAIL;
            break;
            
        case HAL_BUSY :
            usb_status = USBD_BUSY;
            break;
            
        case HAL_TIMEOUT :
            usb_status = USBD_FAIL;
            break;
            
        default :
            usb_status = USBD_FAIL;
            break;
    }

    return usb_status;
}



