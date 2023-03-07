/*
 * Copyright 2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "usb_hsdcd.h"
#include "usb_hsdcd_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if !(defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U))
/*! @brief The sequence initiation time for the dcd module. */
#define USB_HSDCD_TSEQ_INIT_TIME (1000U)
/*! @brief Time Period to Debounce D+ Signal. */
#define USB_HSDCD_TDCD_DBNC (10U)
/*! @brief The time period comparator enabled. */
#define USB_HSDCD_VDPSRC_ON (200U)
/*! @brief The amount of time that the module waits after primary detection before start to secondary detection. */
#define USB_HSDCD_TWAIT_AFTER_PRD (40U)
/*! @brief The amount of time the module enable the Vdm_src. */
#define USB_HSDCD_TVDMSRC_ON (10U)
#endif
typedef enum _usb_dcd_detection_sequence_results
{
    kUSB_DcdDetectionNoResults = 0x0U,
    kUSB_DcdDetectionStandardHost = 0x01U,
    kUSB_DcdDetectionChargingPort = 0x02U,
    kUSB_DcdDetectionDedicatedCharger = 0x03U,
} usb_dcd_detection_sequence_results_t;
typedef enum _usb_dcd_detection_sequence_status
{
    kUSB_DcdDetectionNotEnabled = 0x0U,
    kUSB_DcdDataPinDetectionCompleted = 0x01U,
    kUSB_DcdChargingPortDetectionCompleted = 0x02U,
    kUSB_DcdChargerTypeDetectionCompleted = 0x03U,
} usb_dcd_detection_sequence_status_t;
typedef struct _usb_hsdcd_state_struct
{
    USBHSDCD_Type *dcdRegisterBase;          /*!< The base address of the dcd module */
    usb_hsdcd_callback_t dcdCallback; /*!< DCD callback function*/
    void *dcdCallbackParam;                  /*!< DCD callback parameter*/
#if (defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U))
    void *phyBase;                           /*!< dcd phy base address, if no phy control needed, set to NULL*/
#endif
    uint8_t dcdDisable;                      /*!< whether enable dcd function or not*/
    uint8_t detectResult;                    /*!< dcd detect result*/
} usb_hsdcd_state_struct_t;
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*******************************************************************************
 * Variables
 ******************************************************************************/

static const uint32_t s_hsdcdBaseAddrs[FSL_FEATURE_SOC_USBHSDCD_COUNT] = USBHSDCD_BASE_ADDRS;

 /* Apply for device dcd state structure */
static usb_hsdcd_state_struct_t s_UsbDeviceDcdHSState[FSL_FEATURE_SOC_USBHSDCD_COUNT];
/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t USB_HSDCD_GetInstance(USBHSDCD_Type *base)
{
    int i;

    for (i = 0; i < FSL_FEATURE_SOC_USBHSDCD_COUNT; i++)
    {
        if ((uint32_t)base == s_hsdcdBaseAddrs[i])
        {
            return i;
        }
    }
    return 0xFF;
}

usb_hsdcd_status_t USB_HSDCD_Init(USBHSDCD_Type *base, usb_hsdcd_config_struct_t *config, usb_hsdcd_handle *dcdHandle)
{

    usb_hsdcd_state_struct_t *dcdHSState;
    uint32_t speed;
    uint32_t index;
#if (defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U))
    uint32_t phyBase[] = USBPHY_BASE_ADDRS;
#endif  
    if(NULL == base)
    {
        return kStatus_hsdcd_Error;
    }
    index = USB_HSDCD_GetInstance(base);
    if(0xFF == index)
    {
        return kStatus_hsdcd_Error;
    }

    dcdHSState = &s_UsbDeviceDcdHSState[index];
    *dcdHandle = dcdHSState;
    dcdHSState->dcdRegisterBase = base;
    if ((NULL == config) || (NULL == config->dcdCallback))
    {
        /* don't need init */
    }
    else
    {
#if (defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U))
        dcdHSState->phyBase = (void *)phyBase[index];
#endif
        dcdHSState->dcdCallbackParam = config->dcdCallbackParam;
        /*initialize the dcd controller*/
        dcdHSState->dcdCallback = config->dcdCallback;
    }
    dcdHSState->dcdDisable = 0U;
    if(USB_HSDCD_CLOCK_SPEED)
    {
        /*clock speed unit is MHz*/
        if(USB_HSDCD_CLOCK_SPEED/1000000U)
        {
            speed = USB_HSDCD_CLOCK_SPEED/1000000U;
        }
        else
        {
            speed = USB_HSDCD_CLOCK_SPEED/1000U;
            dcdHSState->dcdRegisterBase->CLOCK &= ~(USBHSDCD_CLOCK_CLOCK_UNIT_MASK);
        }
        dcdHSState->dcdRegisterBase->CLOCK &= ~USBHSDCD_CLOCK_CLOCK_SPEED_MASK;
        dcdHSState->dcdRegisterBase->CLOCK |= USBHSDCD_CLOCK_CLOCK_SPEED(speed);
    }

#if !(defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U))
    /*Valid values are 0-1023*/
    dcdHSState->dcdRegisterBase->TIMER0 &= ~(USBHSDCD_TIMER0_TSEQ_INIT_MASK);
    dcdHSState->dcdRegisterBase->TIMER0 |= USBHSDCD_TIMER0_TSEQ_INIT(USB_HSDCD_TSEQ_INIT_TIME);
    /*Valid values are 1-1023*/
    if(USB_HSDCD_VDPSRC_ON)
    {
        dcdHSState->dcdRegisterBase->TIMER1 &= ~(USBHSDCD_TIMER1_TVDPSRC_ON_MASK);
        dcdHSState->dcdRegisterBase->TIMER1 |= USBHSDCD_TIMER1_TVDPSRC_ON(USB_HSDCD_VDPSRC_ON);
    }
    /*Valid values are 1-1023*/
    if(USB_HSDCD_TDCD_DBNC)
    {
        dcdHSState->dcdRegisterBase->TIMER1 &= ~(USBHSDCD_TIMER1_TDCD_DBNC_MASK);
        dcdHSState->dcdRegisterBase->TIMER1 |= USBHSDCD_TIMER1_TDCD_DBNC(USB_HSDCD_TDCD_DBNC);
    }
    /*Valid values are 0-40ms*/
    dcdHSState->dcdRegisterBase->TIMER2_BC12 &= ~(USBHSDCD_TIMER2_BC12_TVDMSRC_ON_MASK);
    dcdHSState->dcdRegisterBase->TIMER2_BC12 |= USBHSDCD_TIMER2_BC12_TVDMSRC_ON(USB_HSDCD_TVDMSRC_ON);
 
    /*Valid values are 1-1023ms*/
    if(USB_HSDCD_TWAIT_AFTER_PRD)
    {
        dcdHSState->dcdRegisterBase->TIMER2_BC12 &= ~(USBHSDCD_TIMER2_BC12_TWAIT_AFTER_PRD_MASK);
        dcdHSState->dcdRegisterBase->TIMER2_BC12 |= USBHSDCD_TIMER2_BC12_TWAIT_AFTER_PRD(USB_HSDCD_TWAIT_AFTER_PRD);
    }
#endif
    dcdHSState->dcdRegisterBase->CONTROL |= USBHSDCD_CONTROL_BC12_MASK;
   
    return kStatus_hsdcd_Success;
}            
usb_hsdcd_status_t USB_HSDCD_Deinit(usb_hsdcd_handle handle)
{
    usb_hsdcd_state_struct_t *dcdHSState;
    dcdHSState = (usb_hsdcd_state_struct_t *)handle;
    if(NULL == handle)
    {
        return kStatus_hsdcd_Error;
    }
    dcdHSState->dcdRegisterBase->CONTROL |= USBHSDCD_CONTROL_SR_MASK;
    return kStatus_hsdcd_Success;
}
#if (defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U))
/*The USB PHY is shared between DCD function and USB function, but different configurations should be used 
in these two functions. If DCD function is desired, USB_HSDCDResetEHCIPhy should be called to set the USB PHY 
to DCD function; If USB function is desired, USB_HSDCDSetPHYToUSBMode should be called to set the USB PHY to USB function*/
static usb_hsdcd_status_t USB_HSDCDSetPHYtoDCDMode(void *phyBase)
{

    USBPHY_Type *usbPhyBase = (USBPHY_Type *)phyBase;

    if (NULL == usbPhyBase)
    {
        return kStatus_hsdcd_Error;
    }
    usbPhyBase->PWD_SET |= USBPHY_PWD_TXPWDFS_MASK;
    usbPhyBase->ANACTRL &= ~(USBPHY_ANACTRL_DEV_PULLDOWN_MASK);
    usbPhyBase->PLL_SIC |= (USBPHY_PLL_SIC_REFBIAS_PWD_SEL_MASK);
    usbPhyBase->PLL_SIC &= ~(USBPHY_PLL_SIC_REFBIAS_PWD_MASK);
    usbPhyBase->USB1_CHRG_DETECT_SET = USBPHY_USB1_CHRG_DETECT_CLR_BGR_IBIAS_MASK;
    return kStatus_hsdcd_Success;
}
static usb_hsdcd_status_t USB_HSDCDSetPHYtoUSBMode(void *phyBase)
{

    USBPHY_Type *usbPhyBase = (USBPHY_Type *)phyBase;

    if (NULL == usbPhyBase)
    {
        return kStatus_hsdcd_Error;
    }
    /*de-initialize phy for dcd detect*/
    usbPhyBase->PWD &= ~(USBPHY_PWD_TXPWDFS_MASK);
    usbPhyBase->ANACTRL |= USBPHY_ANACTRL_DEV_PULLDOWN_MASK;
    usbPhyBase->PLL_SIC |= USBPHY_PLL_SIC_REFBIAS_PWD_MASK;
    usbPhyBase->PLL_SIC &= ~(USBPHY_PLL_SIC_REFBIAS_PWD_SEL_MASK);
    return kStatus_hsdcd_Success;
}
#endif
usb_hsdcd_status_t USB_HSDCD_Control(usb_hsdcd_handle handle, usb_hsdcd_control_t type, void *param)
{
    usb_hsdcd_state_struct_t *dcdHSState;
    dcdHSState = (usb_hsdcd_state_struct_t *)handle;
    usb_hsdcd_status_t dcdError = kStatus_hsdcd_Success;
    if(NULL == handle)
    {
        return kStatus_hsdcd_Error;
    }
    switch (type)
    {
        case kUSB_DeviceHSDcdRun:
            if(0U == dcdHSState->dcdDisable)
            {
#if (defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U)) && \
    ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
                dcdError = USB_HSDCDSetPHYtoDCDMode(dcdHSState->phyBase);
#endif
                dcdHSState->dcdRegisterBase->CONTROL |= USBHSDCD_CONTROL_START_MASK;            
            }
            break;
        case kUSB_DeviceHSDcdStop:
            if(0U == dcdHSState->dcdDisable)
            {
#if (defined(FSL_FEATURE_USBPHY_HAS_DCD_ANALOG) && (FSL_FEATURE_USBPHY_HAS_DCD_ANALOG > 0U)) && \
    ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
                dcdError = USB_HSDCDSetPHYtoUSBMode(dcdHSState->phyBase);
#endif
                dcdHSState->dcdRegisterBase->CONTROL |= USBHSDCD_CONTROL_SR_MASK;
            }
            break;
        case kUSB_DeviceHSDcdEnable:
            dcdHSState->dcdDisable = 0U;
            break;
        case kUSB_DeviceHSDcdDisable:
            dcdHSState->dcdDisable = 1U;
            break;
        case kUSB_HostHSDcdSetType:
            if ((*((uint8_t *)param)) == kUSB_DcdCDP)
            {
                dcdHSState->dcdRegisterBase->SIGNAL_OVERRIDE = ((dcdHSState->dcdRegisterBase->SIGNAL_OVERRIDE & (~(USBHSDCD_SIGNAL_OVERRIDE_PS_MASK << USBHSDCD_SIGNAL_OVERRIDE_PS_SHIFT))) | (USBHSDCD_SIGNAL_OVERRIDE_PS(3u)));
            }
            else
            {
                dcdHSState->dcdRegisterBase->SIGNAL_OVERRIDE = (dcdHSState->dcdRegisterBase->SIGNAL_OVERRIDE & (~(USBHSDCD_SIGNAL_OVERRIDE_PS_MASK << USBHSDCD_SIGNAL_OVERRIDE_PS_SHIFT)));
            }
            break;
        default:
            break;      
    }
    return dcdError;
}
void USB_HSDcdIsrFunction(usb_hsdcd_handle handle)
{

    uint32_t status;
    uint32_t chargerType;
    usb_hsdcd_state_struct_t *dcdHSState;
    dcdHSState = (usb_hsdcd_state_struct_t *)handle;
    usb_device_charger_detect_type_t event;
    if (NULL == handle)
    {
        return;
    }
    
    event = kUSB_DcdError;
    status = dcdHSState->dcdRegisterBase->STATUS;

    dcdHSState->dcdRegisterBase->CONTROL |= USBHSDCD_CONTROL_IACK_MASK;

    if (status & USBHSDCD_STATUS_ERR_MASK)
    {
        if (status & USBHSDCD_STATUS_TO_MASK)
        {
            event = kUSB_DcdTimeOut;
        }
        else
        {
            event = kUSB_DcdError;
        }
    }
    else
    {
        switch (status & USBHSDCD_STATUS_SEQ_STAT_MASK)
        {
            case USBHSDCD_STATUS_SEQ_STAT(kUSB_DcdChargingPortDetectionCompleted):
                chargerType = status & USBHSDCD_STATUS_SEQ_RES_MASK;
                if (chargerType == USBHSDCD_STATUS_SEQ_RES(kUSB_DcdDetectionStandardHost))
                {
                    event = kUSB_DcdSDP;
                }
                else if (chargerType == USBHSDCD_STATUS_SEQ_RES(kUSB_DcdDetectionChargingPort))
                {
                    event = kUSB_DcdError;
                }
                break;
            case USBHSDCD_STATUS_SEQ_STAT(kUSB_DcdChargerTypeDetectionCompleted):
                chargerType = status & USBHSDCD_STATUS_SEQ_RES_MASK;
                if (chargerType == USBHSDCD_STATUS_SEQ_RES(kUSB_DcdDetectionChargingPort))
                {
                    event = kUSB_DcdCDP;
                }
                else if (chargerType == USBHSDCD_STATUS_SEQ_RES(kUSB_DcdDetectionDedicatedCharger))
                {
                    event = kUSB_DcdDCP;            
                }
                break;
            default:
                break;
        }
    }
    dcdHSState->detectResult = event;
    dcdHSState->dcdRegisterBase->CONTROL |= USBHSDCD_CONTROL_SR_MASK;
    dcdHSState->dcdCallback(dcdHSState->dcdCallbackParam, event, (void *)&dcdHSState->detectResult);
    USB_HSDCD_Control(dcdHSState, kUSB_DeviceHSDcdStop, NULL);
}



