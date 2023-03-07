#ifndef __BL702_HAL_PCD_H
#define __BL702_HAL_PCD_H

#include "bl702_common.h"
#include "bl702_glb.h"
#include "bl702_usb.h"
#include "usb_def.h"
#include "usb_class_def.h"
#include "bl702_hal_def.h"


#if USE_USB_EP0_IN_BUFF
extern uint8_t ep0_in_data_buff[EP0_BUFF_LEN_MAX];
#endif
#if USE_USB_EP1_IN_BUFF
extern uint8_t ep1_in_data_buff[EP1_BUFF_LEN_MAX];
#endif
#if USE_USB_EP2_IN_BUFF
extern uint8_t ep2_in_data_buff[EP2_BUFF_LEN_MAX];
#endif
#if USE_USB_EP3_IN_BUFF
extern uint8_t ep3_in_data_buff[EP3_BUFF_LEN_MAX];
#endif
#if USE_USB_EP4_IN_BUFF
extern uint8_t ep4_in_data_buff[EP4_BUFF_LEN_MAX];
#endif
#if USE_USB_EP5_IN_BUFF
extern uint8_t ep5_in_data_buff[EP5_BUFF_LEN_MAX];
#endif
#if USE_USB_EP6_IN_BUFF
extern uint8_t ep6_in_data_buff[EP6_BUFF_LEN_MAX];
#endif
#if USE_USB_EP7_IN_BUFF
extern uint8_t ep7_in_data_buff[EP7_BUFF_LEN_MAX];
#endif
#if USE_USB_EP0_OUT_BUFF
extern uint8_t ep0_out_data_buff[EP0_BUFF_LEN_MAX];
#endif
#if USE_USB_EP1_OUT_BUFF
extern uint8_t ep1_out_data_buff[EP1_BUFF_LEN_MAX];
#endif
#if USE_USB_EP2_OUT_BUFF
extern uint8_t ep2_out_data_buff[EP2_BUFF_LEN_MAX];
#endif
#if USE_USB_EP3_OUT_BUFF
extern uint8_t ep3_out_data_buff[EP3_BUFF_LEN_MAX];
#endif
#if USE_USB_EP4_OUT_BUFF
extern uint8_t ep4_out_data_buff[EP4_BUFF_LEN_MAX];
#endif
#if USE_USB_EP5_OUT_BUFF
extern uint8_t ep5_out_data_buff[EP5_BUFF_LEN_MAX];
#endif
#if USE_USB_EP6_OUT_BUFF
extern uint8_t ep6_out_data_buff[EP6_BUFF_LEN_MAX];
#endif
#if USE_USB_EP7_OUT_BUFF
extern uint8_t ep7_out_data_buff[EP7_BUFF_LEN_MAX];
#endif


/** 
  * @brief  PCD State structure definition  
  */  
typedef enum 
{
    HAL_PCD_STATE_RESET   = 0x00U,
    HAL_PCD_STATE_READY   = 0x01U,
    HAL_PCD_STATE_ERROR   = 0x02U,
    HAL_PCD_STATE_BUSY    = 0x03U,
    HAL_PCD_STATE_TIMEOUT = 0x04U
} PCD_StateTypeDef;

/**
  * @brief  PCD double buffered endpoint direction
  */
typedef enum
{
    PCD_EP_DBUF_OUT,
    PCD_EP_DBUF_IN,
    PCD_EP_DBUF_ERR,
}PCD_EP_DBUF_DIR;

/**
  * @brief  PCD endpoint buffer number 
  */
typedef enum 
{
    PCD_EP_NOBUF,
    PCD_EP_BUF0,
    PCD_EP_BUF1
}PCD_EP_BUF_NUM;  

/** 
  * @brief  PCD Initialization Structure definition  
  */
typedef struct
{
    uint32_t dev_endpoints;               /*!< Device Endpoints number.
                                             This parameter depends on the used USB core.   
                                             This parameter must be a number between Min_Data = 1 and Max_Data = 15 */    
                                        
    uint32_t speed;                       /*!< USB Core speed.
                                             This parameter can be any value of @ref PCD_Core_Speed                 */        
                                        
    uint32_t ep0_mps;                     /*!< Set the Endpoint 0 Max Packet size. 
                                             This parameter can be any value of @ref PCD_EP0_MPS                    */              
                                        
    uint32_t phy_itface;                  /*!< Select the used PHY interface.
                                             This parameter can be any value of @ref PCD_Core_PHY                   */ 
                                        
    uint32_t Sof_enable;                  /*!< Enable or disable the output of the SOF signal.                         
                                             This parameter can be set to ENABLE or DISABLE                         */
                                        
    uint32_t low_power_enable;            /*!< Enable or disable Low Power mode                                      
                                             This parameter can be set to ENABLE or DISABLE                         */
                                        
    uint32_t lpm_enable;                  /*!< Enable or disable the Link Power Management .                                  
                                             This parameter can be set to ENABLE or DISABLE                         */
                                        
    uint32_t battery_charging_enable;     /*!< Enable or disable Battery charging.                                  
                                             This parameter can be set to ENABLE or DISABLE                         */                                    
                                        
}PCD_InitTypeDef;

typedef struct
{
    uint8_t   num;            /*!< Endpoint number
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15    */ 
    uint8_t   is_in;          /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */ 
    uint8_t   is_stall;       /*!< Endpoint stall condition
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */ 
    uint8_t   type;           /*!< Endpoint type
                                 This parameter can be any value of @ref PCD_EP_Type                      */ 
    uint32_t  pmaadress;      /*!< PMA Address
                                 This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */ 
    uint32_t  pmaaddr0;       /*!< PMA Address0
                                 This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */   
    uint32_t  pmaaddr1;        /*!< PMA Address1
                                 This parameter can be any value between Min_addr = 0 and Max_addr = 1K   */   
    uint8_t   doublebuffer;    /*!< Double buffer enable
                                 This parameter can be 0 or 1                                             */    
    uint32_t  maxpacket;      /*!< Endpoint Max packet size
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */
    uint8_t   *xfer_buff;     /*!< Pointer to transfer buffer                                               */
    uint32_t  xfer_len;       /*!< Current transfer length                                                  */
    uint32_t  xfer_count;     /*!< Partial transfer length in case of multi packet transfer                 */
}PCD_EPTypeDef;


typedef   usb_reg_t                 USB_TypeDef;
typedef   USB_TypeDef               PCD_TypeDef;


/** 
  * @brief  PCD Handle Structure definition  
  */ 
typedef struct
{
    PCD_TypeDef             *Instance;      /*!< Register base address              */ 
    PCD_InitTypeDef         Init;           /*!< PCD required parameters            */
    volatile uint8_t            USB_Address;    /*!< USB Address                        */  
    PCD_EPTypeDef           IN_ep[8];       /*!< IN endpoint parameters             */
    PCD_EPTypeDef           OUT_ep[8];      /*!< OUT endpoint parameters            */
    HAL_LockTypeDef         Lock;           /*!< PCD peripheral status              */
    volatile PCD_StateTypeDef   State;          /*!< PCD communication state            */
    uint32_t                Setup[12];      /*!< Setup packet buffer                */
    uint32_t                BLSetAddressFlag;
    void                    *pData;         /*!< Pointer to upper stack Handler     */    
} PCD_HandleTypeDef;
/*****************************************************************************/
#define PCD_SNG_BUF                                0
#define PCD_DBL_BUF                                1

#define PCD_OUT_BUF                                0
#define PCD_IN_BUF                                 1

#define PCD_SPEED_HIGH                             0 /* Not Supported */
#define PCD_SPEED_LOW                              1
#define PCD_SPEED_FULL                             2

#define PCD_PHY_EMBEDDED                           2

/* __INTERRUPT__ : bl702_usb.h - USB_INT_TYPE_SOF && ...... */
/*****************************************************************************/
#define __HAL_PCD_CLEAR_ALL_FLAG(__HANDLE__)               USB_Clr_IntStatus(USB_INT_ALL)
#define __HAL_PCD_GET_FLAG(__HANDLE__, __INTERRUPT__)      USB_Get_IntStatus(__INTERRUPT__)
#define __HAL_PCD_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    USB_Clr_IntStatus(__INTERRUPT__)

/*****************************************************************************/
#define PCD_EP_TYPE_INTR                            0x0
#define PCD_EP_TYPE_ISOC                            0x2
#define PCD_EP_TYPE_BULK                            0x4
#define PCD_EP_TYPE_CTRL                            0x5

#define PCD_SET_EPTYPE(USBx, bEpNum,wType)          USB_Set_EPx_Type(bEpNum,wType)
#define PCD_GET_EPTYPE(USBx, bEpNum)                USB_Get_EPx_Type(bEpNum)

#define PCD_EP_STATE_ACK                            0
#define PCD_EP_STATE_NACK                           1
#define PCD_EP_STATE_STALL                          2
#define PCD_EP_STATE_NSTALL                         3

#define PCD_SET_EP0_IN_NACK(USBx)                   USB_Set_EPx_IN_Busy(EP_ID0)/*;USBD_DbgLog("TX_NACK\r\n")*/
#define PCD_SET_EP0_OUT_ACK(USBx)                   USB_Set_EPx_OUT_Rdy(EP_ID0)/*;USBD_DbgLog("RX_ACK\r\n")*/
#define PCD_SET_EP0_OUT_NACK(USBx)                  USB_Set_EPx_OUT_Busy(EP_ID0)/*;USBD_DbgLog("RX_NACK\r\n")*/
#define PCD_SET_EP0_INOUT_RDY(USBx)                    USB_Set_EPx_Rdy(EP_ID0)

#define PCD_SET_EP_IN_STATUS(USBx, bEpNum, wState)  USB_Set_EPx_Status(bEpNum,wState)
#define PCD_SET_EP_OUT_STATUS(USBx, bEpNum, wState) USB_Set_EPx_Status(bEpNum,wState)
#define PCD_SET_EP_INOUT_STATUS(USBx,bEpNum,wStaterx,wStatetx)  USB_Set_EPx_Status(bEpNum,wStaterx);

#define PCD_GET_EP_RX_CNT(USBx, bEpNum)                USB_Get_EPx_RX_FIFO_CNT(bEpNum)
/*****************************************************************************/
#define BULK_MAX_PACKET_SIZE                0x00000020
#define USB_IRQ_PRIORITY_PRE                0x01
#define USB_IRQ_PRIORITY_SUB                0x01
#define LED_OFF                             0xFF
#define USART_RX_DATA_SIZE                  2048


//#define USE_EXTERNAL_TRANSCEIVER
#define USE_INTERNAL_TRANSCEIVER

#ifdef USE_EXTERNAL_TRANSCEIVER

#define USB_OEN_PIN_NUM                 GLB_GPIO_PIN_21
#define USB_SUS_PIN_NUM                 GLB_GPIO_PIN_18
#define USB_DP_PIN_NUM                  GLB_GPIO_PIN_22
#define USB_DN_PIN_NUM                  GLB_GPIO_PIN_20
#define USB_OEN_PIN_FUNC                GPIO21_FUN_USB_TX_OE
#define USB_SUS_PIN_FUNC                GPIO18_FUN_USB_SUS_EXT
#define USB_DP_PIN_FUNC                 GPIO22_FUN_USB_TX_DP_USB_RX_DP
#define USB_DN_PIN_FUNC                 GPIO20_FUN_USB_TX_DN_USB_RX_DN

#else
#ifdef USE_INTERNAL_TRANSCEIVER

#define USB_DP_PIN_NUM                  GLB_GPIO_PIN_7
#define USB_DN_PIN_NUM                  GLB_GPIO_PIN_8
#define USB_DP_PIN_FUNC                 GPIO7_FUN_USB_DP_GPIP_CH_6_GPDAC_VREF_EXT
#define USB_DN_PIN_FUNC                 GPIO8_FUN_USB_DM_GPIP_CH_0

#endif
#endif
/*****************************************************************************/
void USB_Stress_Test_Origin_Init(void);
uint8_t USB_Stress_Test_Data_Check(void);
HAL_StatusTypeDef PCD_EP_ISR_Handler(PCD_HandleTypeDef *hpcd);
void PCD_WritePMA(USB_TypeDef  *USBx, uint8_t *pbUsrBuf, uint16_t ep_id, uint16_t wNBytes);
void PCD_ReadPMA(USB_TypeDef  *USBx, uint8_t *pbUsrBuf, uint16_t ep_id, uint16_t wNBytes);
/*****************************************************************************/
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit (PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type);
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
uint16_t          HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);

PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef  HAL_PCDEx_PMAConfig(PCD_HandleTypeDef *hpcd, uint16_t ep_id, uint16_t ep_kind, uint16_t ep_dir, uint32_t pmaadress, uint32_t pmaadressbak);
/*****************************************************************************/
void USB_HAL_Power_Up(void);
void USB_HAL_Power_Off(void);
void USB_HAL_Xcvr_Config(BL_Fun_Type NewState);
void USB_HAL_GPIO_Init(BL_Fun_Type NewState);
void USB_HAL_Clock_Init(BL_Fun_Type NewState);
void USB_HAL_Interrupts_Init(BL_Fun_Type NewState);
void USB_HAL_System_Init(BL_Fun_Type NewState);
void USB_HAL_Interrupts_LPM_Init(BL_Fun_Type NewState);
/*****************************************************************************/
#endif



