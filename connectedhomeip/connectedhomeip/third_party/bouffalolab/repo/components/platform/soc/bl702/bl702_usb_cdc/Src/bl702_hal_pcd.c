//#include "usb_mem.h"
#include "bl702_hal_pcd.h"
#include "bl702_usb.h"
#include "bl_irq.h"
#define USBD_DbgLog(...)

#if USE_USB_EP0_IN_BUFF
uint8_t ep0_in_data_buff[EP0_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP1_IN_BUFF
uint8_t ep1_in_data_buff[EP1_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP2_IN_BUFF
uint8_t ep2_in_data_buff[EP2_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP3_IN_BUFF
uint8_t ep3_in_data_buff[EP3_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP4_IN_BUFF
uint8_t ep4_in_data_buff[EP4_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP5_IN_BUFF
uint8_t ep5_in_data_buff[EP5_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP6_IN_BUFF
uint8_t ep6_in_data_buff[EP6_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP7_IN_BUFF
uint8_t ep7_in_data_buff[EP7_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP0_OUT_BUFF
uint8_t ep0_out_data_buff[EP0_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP1_OUT_BUFF
uint8_t ep1_out_data_buff[EP1_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP2_OUT_BUFF
uint8_t ep2_out_data_buff[EP2_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP3_OUT_BUFF
uint8_t ep3_out_data_buff[EP3_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP4_OUT_BUFF
uint8_t ep4_out_data_buff[EP4_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP5_OUT_BUFF
uint8_t ep5_out_data_buff[EP5_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP6_OUT_BUFF
uint8_t ep6_out_data_buff[EP6_BUFF_LEN_MAX] = {0};
#endif
#if USE_USB_EP7_OUT_BUFF
uint8_t ep7_out_data_buff[EP7_BUFF_LEN_MAX] = {0};
#endif


#define STRESS_DATA_LENGTH        (4*1024)
uint8_t USB_Stress_Test_Origin_Data[STRESS_DATA_LENGTH] = {0};
uint8_t USB_Stress_Test_Recv_Data[STRESS_DATA_LENGTH] = {0};
uint32_t StressTestItem = 0;
void USB_Stress_Test_Origin_Init(void)
{
    uint8_t packValue = 0;

    StressTestItem = 0;
    //USBD_DbgLog("USB_Stress_Test_Origin_Init\r\n");
    for(uint32_t i=0;i<STRESS_DATA_LENGTH;i++){
        USB_Stress_Test_Origin_Data[i] = 0x30 + packValue;
        USB_Stress_Test_Recv_Data[i] = 0;
        //USBD_DbgLog("%c",USB_Stress_Test_Origin_Data[i]);
        if((i%64==0)&&(i!=0)){
            packValue++;
        }
        if(packValue>9){
            packValue=0;
        }
    }

}

uint8_t USB_Stress_Test_Data_Check(void)
{
    for(uint32_t i=0;i<STRESS_DATA_LENGTH;i++){
        if(USB_Stress_Test_Origin_Data[i] != USB_Stress_Test_Recv_Data[i]){
            return 1;
        }
    }

    return 0;
}


#ifdef BFLB_USE_HAL_DRIVER
void USB_IRQHandler(void)
{
    extern PCD_HandleTypeDef hpcd_USB_FS;
    extern void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}
#endif


/**
  * @brief  Initializes the PCD according to the specified
  *         parameters in the PCD_InitTypeDef and create the associated handle.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{ 
    uint32_t i = 0U;

    /* Check the PCD handle allocation */
    if(hpcd == NULL)
    {
        return HAL_ERROR;
    }

    if(hpcd->State == HAL_PCD_STATE_RESET)
    {  
        /* Allocate lock resource and initialize it */
        hpcd->Lock = HAL_UNLOCKED;

        /* Init the low level hardware : GPIO, CLOCK, NVIC... */
        USBD_DbgLog("\t\t\t\tHAL_PCD_MspInit()\r\n");
        HAL_PCD_MspInit(hpcd);
    }

    hpcd->State = HAL_PCD_STATE_BUSY;

    /* Init endpoints structures */
    for (i = 0U; i < hpcd->Init.dev_endpoints ; i++)
    {
        /* Init ep structure */
        hpcd->IN_ep[i].is_in = 1U;
        hpcd->IN_ep[i].num = i;
        /* Control until ep is actvated */
        hpcd->IN_ep[i].type = PCD_EP_TYPE_CTRL;
        hpcd->IN_ep[i].maxpacket =  0U;
        hpcd->IN_ep[i].xfer_buff = 0U;
        hpcd->IN_ep[i].xfer_len = 0U;
    }

    for (i = 0U; i < hpcd->Init.dev_endpoints ; i++)
    {
        hpcd->OUT_ep[i].is_in = 0U;
        hpcd->OUT_ep[i].num = i;
        /* Control until ep is activated */
        hpcd->OUT_ep[i].type = PCD_EP_TYPE_CTRL;
        hpcd->OUT_ep[i].maxpacket = 0U;
        hpcd->OUT_ep[i].xfer_buff = 0U;
        hpcd->OUT_ep[i].xfer_len = 0U;
    }

    return HAL_OK;
}

/**
  * @brief  DeInitializes the PCD peripheral 
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
    /* Check the PCD handle allocation */
    if(hpcd == NULL)
    {
        return HAL_ERROR;
    }

    hpcd->State = HAL_PCD_STATE_BUSY;

    /* Stop Device */
    HAL_PCD_Stop(hpcd);

    /* DeInit the low level hardware */
    HAL_PCD_MspDeInit(hpcd);

    hpcd->State = HAL_PCD_STATE_RESET; 

    return HAL_OK;
}

/**
  * @brief  Initializes the PCD MSP.
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_MspInit could be implemented in the user file
    */
}
#endif
/**
  * @brief  DeInitializes PCD MSP.
  * @param  hpcd PCD handle
  * @retval None
  */
__WEAK
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_MspDeInit could be implemented in the user file
    */
}
  
/**
  * @brief  Start the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{
    hpcd->Instance->usb_config.BF.cr_usb_en = 1;

    return HAL_OK;
}

/**
  * @brief  Stop the USB device.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{ 
    __HAL_LOCK(hpcd); 

    /* disable all interrupts and force USB reset */
    USB_HAL_Interrupts_Init(DISABLE);
    USB_HAL_Interrupts_LPM_Init(DISABLE);
    hpcd->Instance->usb_config.BF.cr_usb_en = 0;
    hpcd->Instance->usb_config.BF.cr_usb_en = 1;
    
    /* clear interrupt status register */
    hpcd->Instance->usb_int_clear.WORD = 0x3FFFF;

    /* switch-off device */
    USB_HAL_Power_Off();
    
    __HAL_UNLOCK(hpcd);
    
    return HAL_OK;
}


/**
  * @brief  This function handles PCD interrupt request.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd)
{
    PCD_EPTypeDef *ep;
    
    /* SOF */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_SOF)){
        USBD_DbgLog("SOF\r\n");
        /*************************************/
        HAL_PCD_SOFCallback(hpcd);
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_SOF);
        return ;
    }

    /* reset */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_RESET)){
        USBD_DbgLog("r\r\n");
        /*************************************/
        //USB_Stress_Test_Origin_Init();
        extern void USB_Device_Init(void);
        USB_Device_Init();
        HAL_PCD_ResetCallback(hpcd);
        HAL_PCD_SetAddress(hpcd, 0U);
        //USB_INOUT_MEM_Init();
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_RESET);
        return ;
    }

    /* vbus toggle */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_VBUS_TGL)){
        USBD_DbgLog("vbus toggle\r\n");
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_VBUS_TGL);
        return ;
    }

    /* get dct cmd */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_GET_DCT_CMD)){
        USBD_DbgLog("get dct cmd\r\n");
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_GET_DCT_CMD);
        return ;
    }

    /* EP0 setup cmd */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_EP0_SETUP_CMD)){
        USBD_DbgLog("EP0 setup cmd\r\n");
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_EP0_SETUP_CMD);
        return ;
    }

    /* EP0 setup done */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_INT_EP0_SETUP_DONE)){
        USBD_DbgLog("ss\r\n");
        /*************************************/
        uint16_t count=0U;
        uint32_t timeout = 0x0000FFFF;
        do{
            count = PCD_GET_EP_RX_CNT(hpcd->Instance, 0);
        }while(--timeout&&(count<8));
        if(count){
            ep = &hpcd->OUT_ep[0];
            ep->xfer_count = count;
            PCD_ReadPMA(hpcd->Instance, (uint8_t*)(void*)hpcd->Setup , EP_ID0, count);
            HAL_PCD_SetupStageCallback(hpcd);
            USB_Set_EPx_Rdy(EP_ID0);
        }else{
            USB_Set_EPx_Rdy(EP_ID0);
        }
        USB_Set_EPx_Rdy(USB_OUT_EP);
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_EP0_SETUP_DONE);
        return ;
    }

    /* EP0 in cmd */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_EP0_IN_CMD)){
        USBD_DbgLog("EP0 in cmd\r\n");
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_EP0_IN_CMD);
        return ;
    }

    /* EP0 in done */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_INT_EP0_IN_DONE)){
        USBD_DbgLog("is\r\n");
        /*************************************/
        ep = &hpcd->IN_ep[0];
        ep->xfer_buff += ep->xfer_count;
        HAL_PCD_DataInStageCallback(hpcd, 0U);
        USB_Set_EPx_Rdy(EP_ID0);
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_EP0_IN_DONE);
        return ;
    }

    /* EP0 out cmd */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_EP0_OUT_CMD)){
        USBD_DbgLog("EP0 out cmd\r\n");
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_EP0_OUT_CMD);
        return ;
    }

    /* EP0 out done */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_INT_EP0_OUT_DONE)){
        USBD_DbgLog("os\r\n");
        /*************************************/
        ep = &hpcd->OUT_ep[0];
        uint16_t count = 0;
        count = PCD_GET_EP_RX_CNT(hpcd->Instance, ep->num);
        if (count){
            ep->xfer_count = count;
            PCD_ReadPMA(hpcd->Instance, ep->xfer_buff, 0, count);
            HAL_PCD_DataOutStageCallback(hpcd, 0U);
            ep->xfer_buff+=ep->xfer_count;
            USB_Set_EPx_Rdy(EP_ID0);

        }else{
            USB_Set_EPx_Rdy(EP_ID0);
        }
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_EP0_OUT_DONE);
        return ;
    }

    /*************************************************************************/
    /* EP1_CMD -> EP1_DONE -> ...... -> EP7_CMD -> EP7_DONE */
    uint32_t inoutFlag=1;
    USB_INT_Type epint=USB_INT_EP1_CMD;
    USB_EP_ID epnum=EP_ID0;
    for(epint=USB_INT_EP1_CMD,inoutFlag=1;epint<=USB_INT_EP7_DONE;epint++,inoutFlag++){
        if (__HAL_PCD_GET_FLAG(hpcd, epint)){
            epnum = (epint-USB_INT_EP0_OUT_CMD)>>1;
            if(inoutFlag&0x1){
                /* cmd -> in */
                HAL_PCD_DataInStageCallback(hpcd, epnum);
            }else{
                /* done -> out */
                //hpcd->OUT_ep[epnum].xfer_count = PCD_GET_EP_RX_CNT(hpcd->Instance, epnum);
                //HAL_PCD_DataOutStageCallback(hpcd, epnum);
                extern void usb_cdc_data_out_irq(uint8_t epnum);
                usb_cdc_data_out_irq(epnum);
            }
            __HAL_PCD_CLEAR_FLAG(hpcd, epint);
            return ;
        }
    }
    /*************************************************************************/

    /* reset end */
    if(__HAL_PCD_GET_FLAG(hpcd, USB_INT_RESET_END)){
        USBD_DbgLog("re\r\n");
        /*************************************/
        USB_Set_EPx_Rdy(EP_ID0);
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_RESET_END);
        return ;
    }

    /* LPM wakeup */
    /* usb wakeup interrupt, need update by reg_excel_files */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_LPM_WAKEUP)){
        /*************************************/
        /*set wInterrupt_Mask global variable*/
        //HAL_PCD_ResumeCallback(hpcd);
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_LPM_WAKEUP);
        return ;
    }

    /* LPM packet */
    /* usb suspend interrupt, need update by reg_excel_files */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_LPM_PACKET)){
        /*************************************/
        /* Force low-power mode in the macrocell */
        if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_LPM_WAKEUP) == 0)
        {
            //HAL_PCD_SuspendCallback(hpcd);
        }
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_LPM_PACKET);
        return ;
    }

    /* lost 3 SOF */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_LOST_SOF_3_TIMES)){
        USBD_DbgLog("Lost 3 SOFs\r\n");
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_LOST_SOF_3_TIMES);
        return ;
    }

    /* error */
    if (__HAL_PCD_GET_FLAG(hpcd, USB_INT_ERROR)){
        USBD_DbgLog("USB bus error 0x%08x; EP2 fifo status 0x%08x\r\n",*(volatile uint32_t *)(0x4000D81C),*(volatile uint32_t *)(0x4000D920));
        /*************************************/
        /*************************************/
        __HAL_PCD_CLEAR_FLAG(hpcd, USB_INT_ERROR);
        return ;
    }
}




/**
  * @brief  Data out stage callbacks
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataOutStageCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Data IN stage callbacks
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DataInStageCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Setup stage callback
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_SetupStageCallback could be implemented in the user file
    */
}
#endif
/**
  * @brief  USB Start Of Frame callbacks
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_SOFCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  USB Reset callbacks
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ResetCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Suspend event callbacks
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_SuspendCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Resume event callbacks
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ResumeCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Incomplete ISO OUT callbacks
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ISOOUTIncompleteCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Incomplete ISO IN  callbacks
  * @param  hpcd PCD handle
  * @param  epnum endpoint number
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);
    UNUSED(epnum);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ISOINIncompleteCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Connection event callbacks
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_ConnectCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Disconnection event callbacks
  * @param  hpcd PCD handle
  * @retval None
  */
#if 0
__WEAK
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hpcd);

    /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PCD_DisconnectCallback could be implemented in the user file
    */ 
}
#endif
/**
  * @brief  Connect the USB device 
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd)
{
    __HAL_LOCK(hpcd); 

    /* Enabling DP Pull-Down bit to Connect internal pull-up on USB DP line */
    /* D+ pull-up ENABLE, not used here */

    __HAL_UNLOCK(hpcd); 
    return HAL_OK;
}
/**
  * @brief  Disconnect the USB device 
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd)
{
    __HAL_LOCK(hpcd); 

    /* Disable DP Pull-Down bit*/
    /* D+ pull-up DISABLE, not used here */

    __HAL_UNLOCK(hpcd); 
    return HAL_OK;
}
/**
  * @brief  Set the USB Device address 
  * @param  hpcd PCD handle
  * @param  address new device address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address)
{
    __HAL_LOCK(hpcd); 

    if(address == 0U) 
    {
     /* set device address and enable function */
        hpcd->USB_Address = address;
        USB_Set_Device_Addr(address);
    }
    else /* USB Address will be applied later */
    {
         hpcd->USB_Address = address;
         USB_Set_Device_Addr(address);
    }

    __HAL_UNLOCK(hpcd);   
    return HAL_OK;
}
/**
  * @brief  Open and configure an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  ep_mps endpoint max packert size
  * @param  ep_type endpoint type   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type)
{
    HAL_StatusTypeDef  ret = HAL_OK;
    PCD_EPTypeDef *ep;

    if ((ep_addr & 0x80U) == 0x80U)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7FU];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr & 0x7FU];
    }
    ep->num   = ep_addr & 0x7FU;

    ep->is_in = (0x80U & ep_addr) != 0U;
    ep->maxpacket = ep_mps;
    ep->type = ep_type;

    __HAL_LOCK(hpcd); 

    /* initialize Endpoint */
    if(ep->num){
        /* EP1 - EP7 : set EP type */
        switch (ep->type)
        {
            case PCD_EP_TYPE_CTRL:
                PCD_SET_EPTYPE(hpcd->Instance, ep->num, PCD_EP_TYPE_CTRL);
                break;
            case PCD_EP_TYPE_BULK:
                PCD_SET_EPTYPE(hpcd->Instance, ep->num, PCD_EP_TYPE_BULK);
                break;
            case PCD_EP_TYPE_INTR:
                PCD_SET_EPTYPE(hpcd->Instance, ep->num, PCD_EP_TYPE_INTR);
                break;
            case PCD_EP_TYPE_ISOC:
                PCD_SET_EPTYPE(hpcd->Instance, ep->num, PCD_EP_TYPE_ISOC);
                break;
            default:
                break;
        }
    }else{
        /* EP0 : do nothing */
    }

    /* ST EndPoint need set EA[3:0] field ( endpoint_address ) for working, not used here */

    /* Single Buffer */
    if (ep->doublebuffer == 0U) 
    {
        if (ep->is_in)
        {
            /*Set the endpoint Transmit buffer address */
            /* this part should be used after reg_relevant_files has been updated */
            /* Configure NAK status for the Endpoint*/
            /* set ep ack for next transfer */
            //PCD_SET_EP_TX_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_ACK);
        }
        else
        {
            /*Set the endpoint Receive buffer address */
            /* this part should be used after reg_relevant_files has been updated */
            /*Set the endpoint Receive buffer counter*/
            /* this function is for initial ep maxpacket length */
            /* Configure VALID status for the Endpoint*/
            //PCD_SET_EP_RX_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_ACK);
        }
    }
    /*Double Buffer*/
    else
    {
        /* hardware designs has no double buffer function */
    }

    __HAL_UNLOCK(hpcd);   
    return ret;
}


/**
  * @brief  Deactivate an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{  
    PCD_EPTypeDef *ep;

    if ((ep_addr & 0x80U) == 0x80U)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7FU];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr & 0x7FU];
    }
    ep->num   = ep_addr & 0x7FU;

    ep->is_in = (0x80U & ep_addr) != 0U;

    __HAL_LOCK(hpcd); 

    if (ep->doublebuffer == 0U) 
    {
        if (ep->is_in)
        {
            /* Configure DISABLE status for the Endpoint*/
            PCD_SET_EP_IN_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_NACK);
        }
        else
        {
            /* Configure DISABLE status for the Endpoint*/
            PCD_SET_EP_OUT_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_NACK);
        }
    }
    /*Double Buffer*/
    else
    {
        /* hardware designs has no double buffer function */
    } 

    __HAL_UNLOCK(hpcd);   
    return HAL_OK;
}


/**
  * @brief  Receive an amount of data  
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the reception buffer   
  * @param  len amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  
    PCD_EPTypeDef *ep;

    ep = &hpcd->OUT_ep[ep_addr & 0x7FU];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;  
    ep->xfer_len = len;
    ep->xfer_count = 0U;
    ep->is_in = 0U;
    ep->num = ep_addr & 0x7FU;

    /* Multi packet transfer*/
    if (ep->xfer_len > ep->maxpacket)
    {
        len=ep->maxpacket;
        ep->xfer_len-=len; 
    }
    else
    {
        len=ep->xfer_len;
        ep->xfer_len =0U;
    }

    /* configure and validate Rx endpoint */
    if (ep->doublebuffer == 0) 
    {
        PCD_ReadPMA(hpcd->Instance, ep->xfer_buff, ep->num, len);
        /* set target_transmit_data_length for IN/TX stage */
        ep->xfer_count = len;
        /* set target_received_data_length for OUT/RX stage */
        /*Set RX buffer count*/
    }
    else
    {
        /* hardware designs has no double buffer function */
    } 

    return HAL_OK;
}

/**
  * @brief  Get Received Data Size
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval Data Size
  */
uint16_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    return hpcd->OUT_ep[ep_addr & 0x7FU].xfer_count;
}
/**
  * @brief  Send an amount of data  
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @param  pBuf pointer to the transmission buffer   
  * @param  len amount of data to be sent
  * @retval HAL status
  */
#if 0
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
    PCD_EPTypeDef *ep;
    uint16_t pmabuffer = 0U;
    UNUSED(pmabuffer);
    ep = &hpcd->IN_ep[ep_addr & 0x7FU];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;  
    ep->xfer_len = len;
    ep->xfer_count = 0U;
    ep->is_in = 1U;
    ep->num = ep_addr & 0x7FU;

    /*Multi packet transfer*/
    if (ep->xfer_len > ep->maxpacket)
    {
        len=ep->maxpacket;
        ep->xfer_len-=len; 
    }
    else
    {  
        len=ep->xfer_len;
        ep->xfer_len =0U;
    }
    /* configure and validate Tx endpoint */
    if (ep->doublebuffer == 0U) 
    {
        PCD_WritePMA(hpcd->Instance, ep->xfer_buff, ep->num, len);
        if(EP_ID0!=ep->num){
            USB_Set_EPx_Status(ep->num,USB_EP_STATUS_ACK);
        }
        /* set target_transmit_data_length for IN/TX stage */
        ep->xfer_count = len;
    }
    else
    {
        /* hardware designs has no double buffer function */
    }

    return HAL_OK;
}
#else
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
    PCD_EPTypeDef *ep;
    uint16_t pmabuffer = 0U;
    UNUSED(pmabuffer);
    ep = &hpcd->IN_ep[ep_addr & 0x7FU];

    /*setup and start the Xfer */
    ep->xfer_buff = pBuf;
    ep->xfer_len = len;
    ep->xfer_count = 0U;
    ep->is_in = 1U;
    ep->num = ep_addr & 0x7FU;

    if(ep->num){
        while(ep->xfer_len){
            //MSG("!");
            /*Multi packet transfer*/
            if (ep->xfer_len > ep->maxpacket)
            {
                len=ep->maxpacket;
                ep->xfer_len-=len;
            }
            else
            {
                len=ep->xfer_len;
                ep->xfer_len =0U;
            }

            PCD_WritePMA(hpcd->Instance, ep->xfer_buff, ep->num, len);
            if(EP_ID0!=ep->num){
                USB_Set_EPx_Status(ep->num,USB_EP_STATUS_ACK);
            }
        }
    }else{
        /*Multi packet transfer*/
        if (ep->xfer_len > ep->maxpacket)
        {
            len=ep->maxpacket;
            ep->xfer_len-=len;
        }
        else
        {
            len=ep->xfer_len;
            ep->xfer_len =0U;
        }
        /* configure and validate Tx endpoint */
        if (ep->doublebuffer == 0U)
        {
            //MSG("?");
            PCD_WritePMA(hpcd->Instance, ep->xfer_buff, ep->num, len);
            if(EP_ID0!=ep->num){
                USB_Set_EPx_Status(ep->num,USB_EP_STATUS_ACK);
            }
            /* set target_transmit_data_length for IN/TX stage */
            ep->xfer_count = len;
        }
        else
        {
            /* hardware designs has no double buffer function */
        }
    }

    return HAL_OK;
}
#endif
/**
  * @brief  Set a STALL condition over an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    PCD_EPTypeDef *ep;

    __HAL_LOCK(hpcd); 

    if ((0x80U & ep_addr) == 0x80U)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7FU];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr];
    }

    ep->is_stall = 1;
    ep->num   = ep_addr & 0x7FU;
    ep->is_in = ((ep_addr & 0x80U) == 0x80U);

    if (ep->num == 0U)
    {
        /* This macro sets STALL status for RX & TX*/ 
        PCD_SET_EP_INOUT_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_STALL, PCD_EP_STATE_STALL);
    }
    else
    {
        if (ep->is_in)
        {
            PCD_SET_EP_IN_STATUS(hpcd->Instance, ep->num , PCD_EP_STATE_STALL);
        }
        else
        {
            PCD_SET_EP_OUT_STATUS(hpcd->Instance, ep->num , PCD_EP_STATE_STALL);
        }
    }
    __HAL_UNLOCK(hpcd); 

    return HAL_OK;
}

/**
  * @brief  Clear a STALL condition over in an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{
    PCD_EPTypeDef *ep;

    if ((0x80U & ep_addr) == 0x80U)
    {
        ep = &hpcd->IN_ep[ep_addr & 0x7FU];
    }
    else
    {
        ep = &hpcd->OUT_ep[ep_addr];
    }

    ep->is_stall = 0U;
    ep->num   = ep_addr & 0x7FU;
    ep->is_in = ((ep_addr & 0x80U) == 0x80U);

    __HAL_LOCK(hpcd); 

    if (ep->is_in)
    {
        PCD_SET_EP_IN_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_NSTALL);
    }
    else
    {
        PCD_SET_EP_OUT_STATUS(hpcd->Instance, ep->num, PCD_EP_STATE_NSTALL);
    }
    __HAL_UNLOCK(hpcd); 

    return HAL_OK;
}

/**
  * @brief  Flush an endpoint
  * @param  hpcd PCD handle
  * @param  ep_addr endpoint address
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr)
{ 
    return HAL_OK;
}

/**
  * @brief  HAL_PCD_ActivateRemoteWakeup : active remote wakeup signalling
* @param  hpcd PCD handle
* @retval HAL status
*/
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
    if (hpcd->Init.lpm_enable ==1)
    {
        /* Apply L1 Resume */
        /* operate reg to send L1 signal to host */
        /* hpcd->Instance->..... */
    }
    else
    {
        /* Apply L2 Resume */
        /* operate reg to send L2 signal to host */
        /* hpcd->Instance->..... */
    }

    return (HAL_OK);
}

/**
* @brief  HAL_PCD_DeActivateRemoteWakeup : de-active remote wakeup signalling
* @param  hpcd PCD handle
* @retval HAL status
*/
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
    if (hpcd->Init.lpm_enable ==1)
    {
        /* Release L1 Resume */
        /* operate reg to release L1 signal */
        /* hpcd->Instance->..... */
    }
    else
    {
        /* Release L2 Resume */
        /* operate reg to release L2 signal */
        /* hpcd->Instance->..... */
    }

    return (HAL_OK);
}


/**
  * @brief  Return the PCD state
  * @param  hpcd PCD handle
  * @retval HAL state
  */
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd)
{
    return hpcd->State;
}

/**
  * @brief Copy a buffer from user memory area to packet memory area (PMA)
  * @param   USBx USB peripheral instance register address.
  * @param   pbUsrBuf pointer to user memory area.
  * @param   wPMABufAddr address into PMA.
  * @param   wNBytes no. of bytes to be copied.
  * @retval None
  */
void PCD_WritePMA(USB_TypeDef  *USBx, uint8_t *pbUsrBuf, uint16_t ep_id, uint16_t wNBytes)
{
    USB_EPx_Write_Data_To_FIFO(ep_id,pbUsrBuf,wNBytes);
}

/**
  * @brief Copy a buffer from user memory area to packet memory area (PMA)
  * @param   USBx USB peripheral instance register address.
  * @param   pbUsrBuf    = pointer to user memory area.
  * @param   wPMABufAddr address into PMA.
  * @param   wNBytes no. of bytes to be copied.
  * @retval None
  */
void PCD_ReadPMA(USB_TypeDef  *USBx, uint8_t *pbUsrBuf, uint16_t ep_id, uint16_t wNBytes)
{
    if(pbUsrBuf == NULL && wNBytes > 0){
        //printf("##PCD_ReadPMA Error: %p##\r\n\r\n", (void *)__builtin_return_address(0));
        return;
    }
    USB_EPx_Read_Data_From_FIFO(ep_id,pbUsrBuf,wNBytes);
}

/**
  * @brief  This function handles PCD Endpoint interrupt request.
  * @param  hpcd PCD handle
  * @retval HAL status
  */
HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd)
{
/*
    PCD_EPTypeDef *ep;
    uint16_t count=0U;
    uint8_t EPindex;
    volatile uint32_t wIstr;
    uint8_t tmpBuff[128] = {0};
*/


    /* #if 0    #endif */

    return HAL_OK;
}



HAL_StatusTypeDef  HAL_PCDEx_PMAConfig(PCD_HandleTypeDef *hpcd, uint16_t ep_id, uint16_t ep_kind, uint16_t ep_dir, uint32_t pmaadress, uint32_t pmaadressbak)
{
  PCD_EPTypeDef *ep;
  
  /* initialize ep structure*/
  if (ep_dir == PCD_IN_BUF)
  {
    ep = &hpcd->IN_ep[ep_id];
  }
  else
  {
    ep = &hpcd->OUT_ep[ep_id];
  }
  
  /* Here we check if the endpoint is single or double Buffer*/
  if (ep_kind == PCD_SNG_BUF)
  {
    /*Single Buffer*/
    ep->doublebuffer = 0U;
    /*Configure the PMA*/
    ep->pmaadress = pmaadress;

    ep->xfer_buff = (uint8_t *)pmaadress;
  }
  else /*USB_DBL_BUF*/
  {
    /*Double Buffer Endpoint*/
    ep->doublebuffer = 1U;
    /*Configure the PMA*/
    ep->pmaaddr0 =  pmaadress;
    ep->pmaaddr1 =  pmaadressbak;
  }

  return HAL_OK;
}



void USB_HAL_Power_Up(void)
{
    uint32_t tmpVal = 0;

    tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
    tmpVal=BL_SET_REG_BIT(tmpVal,GLB_PU_USB);
    BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
}
void USB_HAL_Power_Off(void)
{
    uint32_t tmpVal = 0;

    tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
    tmpVal=BL_CLR_REG_BIT(tmpVal,GLB_PU_USB);
    BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
}
void USB_HAL_Xcvr_Config(BL_Fun_Type NewState)
{
    uint32_t tmpVal = 0;
    
    if(NewState!=DISABLE) {
#ifdef USE_EXTERNAL_TRANSCEIVER
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR_CONFIG);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_REG_USB_USE_XCVR,0);  //use external tranceiver
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR_CONFIG,tmpVal);
#else
#ifdef USE_INTERNAL_TRANSCEIVER
    #if 1
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_PU_USB,1);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
        
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SUS,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SPD,1);       //0 for 1.1 ls,1 for 1.1 fs
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_DATA_CONVERT,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_OEB_SEL,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ROUT_PMOS,3);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ROUT_NMOS,3);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
        
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR_CONFIG);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_P_RISE,2);   //1 for 1.1 ls
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_P_FALL,2);   //1 for 1.1 ls
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_M_RISE,2);   //1 for 1.1 ls
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_M_FALL,2);   //1 for 1.1 ls
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_RES_PULLUP_TUNE,5);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_REG_USB_USE_XCVR,1);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_BD_VTH,1);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_V_HYS_P,2);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_V_HYS_M,2);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR_CONFIG,tmpVal);

        ///* force BD=1, not use */
        //tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        //tmpVal=BL_SET_REG_BIT(tmpVal,GLB_PU_USB_LDO);
        //BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);

        /* BD_voltage_thresdhold=2.8V */
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR_CONFIG);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_BD_VTH,7);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR_CONFIG,tmpVal);

    #else
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_PU_USB,1);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
        
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SUS,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SPD,0);       //0 for 1.1 ls,1 for 1.1 fs
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_DATA_CONVERT,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_OEB_SEL,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ROUT_PMOS,3);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ROUT_NMOS,3);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
        
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR_CONFIG);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_P_RISE,1);   //4 for 1.1 fs
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_P_FALL,1);   //3 for 1.1 fs
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_M_RISE,1);   //4 for 1.1 fs
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_SLEWRATE_M_FALL,1);   //3 for 1.1 fs
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_RES_PULLUP_TUNE,5);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_REG_USB_USE_XCVR,1);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_BD_VTH,1);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_V_HYS_P,2);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_V_HYS_M,2);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR_CONFIG,tmpVal);
    #endif

    tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
    tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ENUM,1);
    BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
#endif
#endif
    } else {
#ifdef USE_INTERNAL_TRANSCEIVER
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ENUM,0);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_PU_USB,0);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);

        ///* force BD=1, not use */
        //tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        //tmpVal=BL_SET_REG_BIT(tmpVal,GLB_PU_USB_LDO);
        //BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);

        /* BD_voltage_thresdhold=2.8V */
        tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR_CONFIG);
        tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_BD_VTH,7);
        BL_WR_REG(GLB_BASE,GLB_USB_XCVR_CONFIG,tmpVal);

#else
    tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR_CONFIG);
    tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_REG_USB_USE_XCVR,1);  //use internal tranceiver
    BL_WR_REG(GLB_BASE,GLB_USB_XCVR_CONFIG,tmpVal);
    
    tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
    tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_USB_ENUM,0);
    tmpVal=BL_SET_REG_BITS_VAL(tmpVal,GLB_PU_USB,1);
    BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);
#endif
    }
}
void USB_HAL_GPIO_Init(BL_Fun_Type NewState)
{
    GLB_GPIO_Cfg_Type cfg;
    
    if(NewState!=DISABLE){
        cfg.gpioMode=GPIO_MODE_AF;
        cfg.pullType=GPIO_PULL_NONE;
        cfg.drive=1;
        cfg.smtCtrl=1;
        
#ifdef USE_EXTERNAL_TRANSCEIVER
        cfg.gpioPin=USB_OEN_PIN_NUM;
        cfg.gpioFun=USB_OEN_PIN_FUNC;
        GLB_GPIO_Init(&cfg);
        
        cfg.gpioPin=USB_SUS_PIN_NUM;
        cfg.gpioFun=USB_SUS_PIN_FUNC;
        GLB_GPIO_Init(&cfg);
        
        cfg.gpioPin=USB_DP_PIN_NUM;
        cfg.gpioFun=USB_DP_PIN_FUNC;
        GLB_GPIO_Init(&cfg);
        
        cfg.gpioPin=USB_DN_PIN_NUM;
        cfg.gpioFun=USB_DN_PIN_FUNC;
        GLB_GPIO_Init(&cfg);
#endif
        
#ifdef USE_INTERNAL_TRANSCEIVER
        cfg.gpioPin=USB_DP_PIN_NUM;
        cfg.gpioFun=USB_DP_PIN_FUNC;
        GLB_GPIO_Init(&cfg);
        GLB_GPIO_INPUT_Disable(cfg.gpioPin);
        cfg.gpioPin=USB_DN_PIN_NUM;
        cfg.gpioFun=USB_DN_PIN_FUNC;
        GLB_GPIO_Init(&cfg);
        GLB_GPIO_INPUT_Disable(cfg.gpioPin);
#endif
    }
}
void USB_HAL_Clock_Init(BL_Fun_Type NewState)
{
    if(NewState!=DISABLE){
        GLB_Set_USB_CLK(ENABLE);
    }else{
        GLB_Set_USB_CLK(DISABLE);
    }
}
void USB_HAL_Interrupts_Init(BL_Fun_Type NewState)
{
    bl_irq_register(USB_IRQn,USB_IRQHandler);
    if(NewState!=DISABLE){
        bl_irq_enable(USB_IRQn);
    }else{
        bl_irq_disable(USB_IRQn);
    }
}
void USB_HAL_System_Init(BL_Fun_Type NewState)
{
    USB_HAL_GPIO_Init(NewState);
    USB_HAL_Clock_Init(NewState);
    USB_HAL_Interrupts_Init(NewState);
}
void USB_HAL_Interrupts_LPM_Init(BL_Fun_Type NewState)
{
    /* enable/disable LPM interrupt */
    if(NewState!=DISABLE){
        USB_IntMask(USB_INT_LPM_WAKEUP,UNMASK);
        USB_IntMask(USB_INT_LPM_PACKET,UNMASK);
    }else{
        USB_IntMask(USB_INT_LPM_WAKEUP,MASK);
        USB_IntMask(USB_INT_LPM_PACKET,MASK);
    }
}

