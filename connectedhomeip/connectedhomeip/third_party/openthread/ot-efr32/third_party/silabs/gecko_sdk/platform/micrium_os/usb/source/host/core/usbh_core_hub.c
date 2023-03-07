/***************************************************************************//**
 * @file
 * @brief USB Host Hub Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note             (1) The HUB class can handle up to 7 ports on each hub. If a hub with more than 7 ports
 *                       is connected, only the first 7 ports will work properly.
 *
 * @note             (2) The HUB class is in charge of keeping a linked list of the devices. It keeps one
 *                       linked list per Host Controller. The order of the devices is very important as it
 *                       is used by the HUB class when a port change is requested and must be propagated to
 *                       downstream devices (port suspend, resume or hub disconnect). Each hub function
 *                       keep a pointer to its first and last device. Each device keep a pointer to the next
 *                       and previous device.
 *                       The order is explained with the example below. Consider you have the following bus
 *                       topology:
 * @verbatim
 *                                           ---------
 *                                           |  HC   |
 *                                           |       |
 *                                           ---------
 *                                               |
 *                                           ---------
 *                                           |  RH   |
 *                                           |       |
 *                                           ---------
 *                           _________________|  |  |______________________
 *                          |                    |                         |
 *                       ---------            ---------                 ---------
 *                       |  D1   |            |  D2   |                 |  D3   |
 *                       |       |            |  H1   |                 |  H2   |
 *                       ---------            ---------                 ---------
 *                                     ________|     |_____                 |
 *                                    |                    |                |
 *                                ---------            ---------        ---------
 *                                |  D4   |            |  D5   |        |  D6   |
 *                                |       |            |  H3   |        |       |
 *                                ---------            ---------        ---------
 *                                                          |
 *                                                     ---------
 *                                                     |  D7   |
 *                                                     |       |
 *                                                     ---------
 *
 *                               ---------                          ---------
 *                     Where     |  D#   |    is a device and       |  D#   |      is a hub device
 *                               |       |    # is its address      |  H#   |      # is the index of hub.
 *                               ---------                          ---------
 *
 *                       The linked list would be organized in the following way:
 *
 *                                                   ---------
 *                                                   |  RH   |
 *                                                   |       |
 *                                                   ---------
 *                   _______________HEAD______________|   |____________TAIL_________
 *                   |                                                               |
 *                  \ /                                                             \ /
 *               --------     --------     --------     --------     --------     --------     --------
 *               |  D1  |---->|  D2  |---->|  D4  |---->|  D5  |---->|  D7  |---->|  D3  |---->|  D6  |---> -
 *        - <----|      |<----|  H1  |<----|      |<----|  H3  |<----|      |<----|  H2  |<----|      |      |
 *        |      --------     --------     ---^----     -^------     ----^---     --------     ---^----     ---
 *       ---                    |   |__HEAD___|          |   |_HEAD/TAIL_|            |_HEAD/TAIL_|         ///
 *       ///                    |_____________TAIL_______|
 * @endverbatim
 *                       This simplifies the processes that requires to be propagated. If, for instance, D2
 *                       is disconnected, the hub class can easily determine that D4, D5 and D7 must be
 *                       disconnected as well.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_USB_HOST_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define   HUB_MODULE
#define   MICRIUM_SOURCE

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <rtos_description.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_if.h>
#include  <usb/include/host/usbh_core_hub.h>
#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/hcd/usbh_hcd_hub_priv.h>
#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_ep_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>
#include  <usb/source/host/core/usbh_core_if_priv.h>
#include  <usb/source/host/core/usbh_core_hub_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (USBH, HUB)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_USBH

#define  USBH_HUB_DLY_DEV_RESET                           11u   // Guarantee at least 10ms.
#define  USBH_HUB_DLY_POST_DEV_RESET                      11u   // Guarantee at least 10ms.
#define  USBH_HUB_DLY_PORT_DEBOUNCE                      100u
#define  USBH_HUB_DLY_PORT_RESUME                  (20u + 10u)
#define  USBH_HUB_DLY_RETRY                              100u

#define  USBH_HUB_LEN_DESC_MIN                             8u
#define  USBH_HUB_DESC_LEN_MAX                          (USBH_HUB_LEN_DESC_MIN + 2u)
#define  USBH_HUB_BUF_LEN_PORT_STATUS                      4u

#define  USBH_HUB_STATUS_CHNG_BUF_LEN                      1u

/********************************************************************************************************
 *                                           HUB DESCRIPTOR TYPES
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 11.23.2, Table 11-13.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_HUB_DESC_TYPE_HUB                         0x29u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           HUB CLASS DATA TYPE
 *******************************************************************************************************/

typedef struct usbh_hub {
  KAL_TASK_HANDLE HubTaskHandle;                                // Handle to hub event task.

  KAL_SEM_HANDLE  HubEventQSemHandle;                           // Handle to hub event Q semaphore.
  USBH_HUB_EVENT  *HubEventQHead;                               // Head of hub event Q.

  MEM_DYN_POOL    HubEventPool;                                 // Dyn mem pool of USB HUB events when req'd by user.
#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  CPU_INT08U      *DescBufPtr;                                  // Ptr to desc buf.

  MEM_DYN_POOL    HubFnctPool;                                  // Pool of external hub functions.

  CPU_INT08U      *PortStatusBufPtr;                            // Ptr to    port/hub status buf.
  KAL_LOCK_HANDLE PortStatusBufLock;                            // Handle to port/hub status buf lock.
#endif
} USBH_HUB;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static CPU_BOOLEAN USBH_HUB_Probe(USBH_DEV_HANDLE dev_handle,
                                  CPU_INT08U      class_code,
                                  void            **p_class_fnct,
                                  RTOS_ERR        *p_err);

static void USBH_HUB_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_nbr,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in);

static void USBH_HUB_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_nbr);

static void USBH_HUB_Conn(void *p_class_fnct);

static void USBH_HUB_Disconn(void *p_class_fnct);

static void USBH_HUB_Suspend(void *p_class_fnct);

static void USBH_HUB_Resume(void *p_class_fnct);

#if ((USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED) \
  && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))
static void USBH_HUB_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct);
#endif

static void USBH_HUB_IntrRxCmpl(USBH_DEV_HANDLE dev_handle,
                                USBH_EP_HANDLE  ep_handle,
                                CPU_INT08U      *p_buf,
                                CPU_INT32U      buf_len,
                                CPU_INT32U      xfer_len,
                                void            *p_arg,
                                RTOS_ERR        err);
#endif

static void USBH_HUB_EventRxStart(USBH_DEV_HANDLE hub_dev_handle,
                                  USBH_HUB_FNCT   *p_hub_fnct);

static void USBH_HUB_PortReq(USBH_DEV_HANDLE    hub_dev_handle,
                             USBH_HUB_PORT_REQ  port_req,
                             CPU_INT08U         port_nbr,
                             USBH_PORT_REQ_CMPL cmpl_callback,
                             void               *p_arg,
                             RTOS_ERR           *p_err);

static void USBH_HUB_EventTaskHandler(void *p_arg);

static void USBH_HUB_PortReqProcess(USBH_DEV_HANDLE   hub_dev_handle,
                                    USBH_HUB_PORT_REQ port_req,
                                    CPU_INT08U        port_nbr,
                                    RTOS_ERR          *p_err);

static void USBH_HUB_PortConnProcess(USBH_HUB_FNCT *p_hub_fnct,
                                     CPU_INT08U    port_nbr);

static void USBH_HUB_PortDisconnProcess(USBH_HUB_FNCT *p_hub_fnct,
                                        CPU_INT08U    port_nbr);

static void USBH_HUB_StatusGet(USBH_HUB_FNCT   *p_hub_fnct,
                               USBH_HUB_STATUS *p_status,
                               RTOS_ERR        *p_err);

static void USBH_HUB_FeatureClr(USBH_HUB_FNCT *p_hub_fnct,
                                CPU_INT16U    feature,
                                RTOS_ERR      *p_err);

static void USBH_HUB_PortStatusGet(USBH_HUB_FNCT   *p_hub_fnct,
                                   CPU_INT16U      port_nbr,
                                   USBH_HUB_STATUS *p_port_status,
                                   RTOS_ERR        *p_err);

static void USBH_HUB_PortFeatureSet(USBH_HUB_FNCT *p_hub_fnct,
                                    CPU_INT16U    feature,
                                    CPU_INT08U    port_nbr,
                                    RTOS_ERR      *p_err);

static void USBH_HUB_PortFeatureClr(USBH_HUB_FNCT *p_hub_fnct,
                                    CPU_INT16U    feature,
                                    CPU_INT16U    port_nbr,
                                    RTOS_ERR      *p_err);

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_PortStatusBufLock(RTOS_ERR *p_err);

static void USBH_HUB_PortStatusBufUnlock(void);
#endif

static USBH_HC *USBH_HUB_HC_PtrGet(USBH_DEV_HANDLE root_hub_dev_handle,
                                   RTOS_ERR        *p_err);

static void USBH_HUB_RootIntEn(USBH_DEV_HANDLE root_hub_dev_handle,
                               CPU_BOOLEAN     en,
                               RTOS_ERR        *p_err);

static void USBH_HUB_DevListAdd(USBH_DEV      *p_dev,
                                USBH_HUB_FNCT *p_hub_fnct);

static void USBH_HUB_DevListRemove(USBH_DEV      *p_dev,
                                   USBH_HUB_FNCT *p_hub_fnct);

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static CPU_BOOLEAN USBH_HUB_FnctAllocCallback(MEM_DYN_POOL *p_pool,
                                              MEM_SEG      *p_seg,
                                              void         *p_blk,
                                              void         *p_arg);
#endif

static CPU_BOOLEAN USBH_HUB_EventAllocCallback(MEM_DYN_POOL *p_pool,
                                               MEM_SEG      *p_seg,
                                               void         *p_blk,
                                               void         *p_arg);

static USBH_HUB_EVENT *USBH_HUB_EventQPend(RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static USBH_HUB *USBH_HUB_Ptr = DEF_NULL;

/********************************************************************************************************
 *                                           HUB CLASS DRIVER
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
USBH_CLASS_DRV USBH_HUB_Drv = {
  USBH_HUB_Probe,
  DEF_NULL,
  DEF_NULL,
  USBH_HUB_EP_Open,
  USBH_HUB_EP_Close,
  USBH_HUB_Conn,
  DEF_NULL,
  USBH_HUB_Disconn,
  USBH_HUB_Suspend,
  USBH_HUB_Resume,
  DEF_NULL,                                                     // USBH_HUB_UnInit is used in all cases (EX HUB dis).
#ifdef RTOS_MODULE_COMMON_SHELL_AVAIL
#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  USBH_HUB_TraceDump,
#else
  DEF_NULL,
#endif
#endif
  (CPU_CHAR *)"HUB",
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HUB_TaskPrioSet()
 *
 * @brief    Assigns a new priority to the USB host HUB task.
 *
 * @param    prio    New priority of the the hub task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) This function cannot be called before the USB Host module has been initialized via
 *               the USBH_Init() function.
 *******************************************************************************************************/
void USBH_HUB_TaskPrioSet(CPU_INT08U prio,
                          RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE hub_task_handle;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((USBH_Ptr != DEF_NULL), RTOS_ERR_NOT_INIT,; );

  hub_task_handle = USBH_HUB_Ptr->HubTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(hub_task_handle,
                  prio,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortSuspendReq()
 *
 * @brief    Requests a suspend on the specified port and hub.
 *
 * @param    hub_dev_handle  Handle to the hub device.
 *
 * @param    port_nbr        Port number, can be USBH_HUB_PORT_ALL.
 *
 * @param    cmpl_callback   Callback to notify caller that the operation is complete. Can be DEF_NULL.
 *
 * @param    p_arg           Argument that will be passed to callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HUB_PortSuspendReq(USBH_DEV_HANDLE    hub_dev_handle,
                             CPU_INT08U         port_nbr,
                             USBH_PORT_REQ_CMPL cmpl_callback,
                             void               *p_arg,
                             RTOS_ERR           *p_err)
{
  USBH_HUB_PortReq(hub_dev_handle,
                   USBH_HUB_PORT_REQ_SUSPEND,
                   port_nbr,
                   cmpl_callback,
                   p_arg,
                   p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortResumeReq()
 *
 * @brief    Request resume on the specified port and hub.
 *
 * @param    hub_dev_handle  Handle to the hub device.
 *
 * @param    port_nbr        Port number, can be USBH_HUB_PORT_ALL.
 *
 * @param    cmpl_callback   Callback to notify caller that the operation is complete. Can be DEF_NULL.
 *
 * @param    p_arg           Argument that will be passed to the callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HUB_PortResumeReq(USBH_DEV_HANDLE    hub_dev_handle,
                            CPU_INT08U         port_nbr,
                            USBH_PORT_REQ_CMPL cmpl_callback,
                            void               *p_arg,
                            RTOS_ERR           *p_err)
{
  USBH_HUB_PortReq(hub_dev_handle,
                   USBH_HUB_PORT_REQ_RESUME,
                   port_nbr,
                   cmpl_callback,
                   p_arg,
                   p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortResetReq()
 *
 * @brief    Requests the reset on specified port and hub.
 *
 * @param    hub_dev_handle  Handle to the hub device.
 *
 * @param    port_nbr        Port number, can be USBH_HUB_PORT_ALL.
 *
 * @param    cmpl_callback   Callback to notify the caller that the operation is complete. Can be DEF_NULL.
 *
 * @param    p_arg           Argument that will be passed to the callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HUB_PortResetReq(USBH_DEV_HANDLE    hub_dev_handle,
                           CPU_INT08U         port_nbr,
                           USBH_PORT_REQ_CMPL cmpl_callback,
                           void               *p_arg,
                           RTOS_ERR           *p_err)
{
  USBH_HUB_PortReq(hub_dev_handle,
                   USBH_HUB_PORT_REQ_RESET,
                   port_nbr,
                   cmpl_callback,
                   p_arg,
                   p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortDisconnReq()
 *
 * @brief    Request a disconnect on the specified port and hub.
 *
 * @param    hub_dev_handle  Handle to the hub device.
 *
 * @param    port_nbr        Port number, can be USBH_HUB_PORT_ALL.
 *
 * @param    cmpl_callback   Callback to notify the caller that the operation is complete. Can be DEF_NULL.
 *
 * @param    p_arg           Argument that will be passed to the callback.
 *
 * @param    p_err           Pointer to the variable that will receive one of these return error codes
 *                           from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALLOC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *******************************************************************************************************/
void USBH_HUB_PortDisconnReq(USBH_DEV_HANDLE    hub_dev_handle,
                             CPU_INT08U         port_nbr,
                             USBH_PORT_REQ_CMPL cmpl_callback,
                             void               *p_arg,
                             RTOS_ERR           *p_err)
{
  USBH_HUB_PortReq(hub_dev_handle,
                   USBH_HUB_PORT_REQ_DISCONN,
                   port_nbr,
                   cmpl_callback,
                   p_arg,
                   p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           STACK LOCAL FUNCTION
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CORE-ONLY FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_HUB_Init()
 *
 * @brief    Initializes HUB class.
 *
 * @param    hub_fnct_qty    Quantity of hub function.
 *
 * @param    hub_event_qty   Quantity of hub event.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_Init(CPU_INT08U hub_fnct_qty,
                   CPU_INT08U hub_event_qty,
                   RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE hub_task_handle;
  USBH_HUB        *p_usbh_hub;
  CORE_DECLARE_IRQ_STATE;

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_DISABLED)
  PP_UNUSED_PARAM(hub_fnct_qty);
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  //                                                               ---------- ALLOC HUB CLASS INTERNAL DATA -----------
  //                                                               Alloc hub class root struct.
  p_usbh_hub = (USBH_HUB *)Mem_SegAlloc("USBH - HUB class root struct",
                                        USBH_InitCfg.MemSegPtr,
                                        sizeof(USBH_HUB),
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_usbh_hub->HubTaskHandle = KAL_TaskHandleNull;

  CORE_ENTER_ATOMIC();
  USBH_HUB_Ptr = p_usbh_hub;
  CORE_EXIT_ATOMIC();

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  //                                                               Alloc buffer used to retrieve HUB desc.
  USBH_HUB_Ptr->DescBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - HUB class descriptor buffer",
                                                          USBH_InitCfg.MemSegBufPtr,
                                                          USBH_HUB_DESC_LEN_MAX,
                                                          USBH_InitCfg.BufAlignOctets,
                                                          DEF_NULL,
                                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Alloc buffer used to retrieve port status.
  USBH_HUB_Ptr->PortStatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - HUB class port status buffer",
                                                                USBH_InitCfg.MemSegBufPtr,
                                                                sizeof(USBH_HUB_STATUS),
                                                                USBH_InitCfg.BufAlignOctets,
                                                                DEF_NULL,
                                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Create hub fnct pool.
  Mem_DynPoolCreatePersistent("USBH - Hub fnct pool",
                              &USBH_HUB_Ptr->HubFnctPool,
                              USBH_InitCfg.MemSegPtr,
                              sizeof(USBH_HUB_FNCT),
                              sizeof(CPU_ALIGN),
                              (hub_fnct_qty != USBH_OBJ_QTY_UNLIMITED) ? hub_fnct_qty : 0u,
                              (hub_fnct_qty != USBH_OBJ_QTY_UNLIMITED) ? hub_fnct_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                              USBH_HUB_FnctAllocCallback,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  Mem_DynPoolCreatePersistent("USBH - Hub event pool",
                              &USBH_HUB_Ptr->HubEventPool,
                              USBH_InitCfg.MemSegPtr,
                              sizeof(USBH_HUB_EVENT),
                              sizeof(CPU_ALIGN),
                              (hub_event_qty != USBH_OBJ_QTY_UNLIMITED) ? hub_event_qty : 0u,
                              (hub_event_qty != USBH_OBJ_QTY_UNLIMITED) ? hub_event_qty : LIB_MEM_BLK_QTY_UNLIMITED,
                              USBH_HUB_EventAllocCallback,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----------- CREATE HUB CLASS KERNEL RSRC -----------
  //                                                               Create hub event Q.
  USBH_HUB_Ptr->HubEventQHead = DEF_NULL;
  USBH_HUB_Ptr->HubEventQSemHandle = KAL_SemCreate("USBH - HUB event sem",
                                                   DEF_NULL,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  //                                                               Create port status buf lock.
  USBH_HUB_Ptr->PortStatusBufLock = KAL_LockCreate("USBH - HUB port status buf lock",
                                                   DEF_NULL,
                                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

  //                                                               Create hub task.
  hub_task_handle = KAL_TaskAlloc("USBH - Hub task",
                                  (CPU_STK *)USBH_InitCfg.HubTaskStkPtr,
                                  USBH_InitCfg.HubTaskStkSizeElements,
                                  DEF_NULL,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  KAL_TaskCreate(hub_task_handle,
                 USBH_HUB_EventTaskHandler,
                 DEF_NULL,
                 USBH_HUB_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();
  USBH_HUB_Ptr->HubTaskHandle = hub_task_handle;
  CORE_EXIT_ATOMIC();

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  //                                                               -------- REGISTER HUB CLASS DRIVER TO CORE ---------
  USBH_ClassDrvReg(&USBH_HUB_Drv, p_err);
#endif

  LOG_VRB(("Class driver initialized"));
}

/****************************************************************************************************//**
 *                                               USBH_HUB_UnInit()
 *
 * @brief    Uninit HUB class.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_HUB_UnInit(RTOS_ERR *p_err)
{
  KAL_TaskDel(USBH_HUB_Ptr->HubTaskHandle);
  KAL_SemDel(USBH_HUB_Ptr->HubEventQSemHandle);                 // Del HUB event q sem.
#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  KAL_LockDel(USBH_HUB_Ptr->PortStatusBufLock);                 // Del port status buf lock.
#endif

  USBH_HUB_Ptr = DEF_NULL;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_RootInit()
 *
 * @brief    Initialize root hub function structure.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    p_host      Pointer to the host.
 *
 * @param    p_hc        Pointer to the host controller.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_RootInit(USBH_HUB_FNCT *p_hub_fnct,
                       USBH_HOST     *p_host,
                       USBH_HC       *p_hc,
                       RTOS_ERR      *p_err)
{
  CPU_BOOLEAN        ok;
  USBH_HC_DRV        *p_hc_drv;
  USBH_HUB_ROOT_INFO hub_info;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT08U port_ix;
#endif

  p_hc_drv = &p_hc->Drv;                                        // Retrieve RH properties from HCD.
  ok = p_hc_drv->RH_API_Ptr->InfosGet(p_hc_drv, &hub_info);
  if (ok == DEF_FAIL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  //                                                               Init hub data.
  USBH_DEV_HANDLE_UPDATE(p_hub_fnct, p_host->Ix, p_hc->Ix, USBH_DEV_ADDR_RH);
  p_hub_fnct->NbrPort = hub_info.NbrPort;
  p_hub_fnct->Characteristics = hub_info.Characteristics;
  p_hub_fnct->PwrOn2PwrGood = hub_info.PwrOn2PwrGood;
  p_hub_fnct->DevPtr = DEF_NULL;                                // RH has no dev struct attached.
#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  p_hub_fnct->ErrCnt = 0u;
#endif
  p_hub_fnct->PortStatusChng = DEF_BIT_NONE;
  p_hub_fnct->PortBusyCnt = DEF_BIT_NONE;
  p_hub_fnct->DevListHeadPtr = DEF_NULL;
  p_hub_fnct->DevListTailPtr = DEF_NULL;

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  for (port_ix = 0u; port_ix < USBH_HUB_NBR_PORT; port_ix++) {
    p_hub_fnct->DevPtrList[port_ix] = DEF_NULL;
  }
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_RootStart()
 *
 * @brief    Starts root hub.
 *
 * @param    p_hc    Pointer to the host controller.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_RootStart(USBH_HC  *p_hc,
                        RTOS_ERR *p_err)
{
  CPU_INT08U    port_nbr;
  USBH_HUB_FNCT *p_hub_fnct = &p_hc->RH_Fnct;

  //                                                               Enable power on each root hub's port(s).
  for (port_nbr = 1u; port_nbr <= p_hub_fnct->NbrPort; port_nbr++) {
    USBH_HUB_PortFeatureSet(p_hub_fnct,
                            USBH_HUB_FEATURE_SEL_PORT_PWR,
                            port_nbr,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  USBH_HUB_RootResume(p_hc, p_err);                             // Resume root hub.
}

/****************************************************************************************************//**
 *                                           USBH_HUB_RootStop()
 *
 * @brief    Stops root hub.
 *
 * @param    p_hc    Pointer to the host controller.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_RootStop(USBH_HC  *p_hc,
                       RTOS_ERR *p_err)
{
  CPU_INT08U    port_nbr;
  USBH_HUB_FNCT *p_hub_fnct = &p_hc->RH_Fnct;

  USBH_HUB_RootSuspend(p_hc, p_err);                            // Suspend root hub.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Disable power on each root hub's port(s).
  for (port_nbr = 1u; port_nbr <= p_hub_fnct->NbrPort; port_nbr++) {
    USBH_HUB_PortFeatureClr(p_hub_fnct,
                            USBH_HUB_FEATURE_SEL_PORT_PWR,
                            port_nbr,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_HUB_RootResume()
 *
 * @brief    Resumes root hub.
 *
 * @param    p_hc    Pointer to the host controller.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_RootResume(USBH_HC  *p_hc,
                         RTOS_ERR *p_err)
{
  CPU_BOOLEAN   ok;
  USBH_HC_DRV   *p_hc_drv;
  USBH_HUB_FNCT *p_hub_fnct;

  p_hc_drv = &p_hc->Drv;
  p_hub_fnct = &p_hc->RH_Fnct;

  p_hub_fnct->State = USBH_HUB_STATE_CONN;

  ok = p_hc_drv->RH_API_Ptr->IntEn(p_hc_drv, DEF_ENABLED);
  if (ok == DEF_FAIL) {
    p_hub_fnct->State = USBH_HUB_STATE_NONE;
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_RootSuspend()
 *
 * @brief    Suspends root hub.
 *
 * @param    p_hc    Pointer to the host controller.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_RootSuspend(USBH_HC  *p_hc,
                          RTOS_ERR *p_err)
{
  CPU_BOOLEAN   ok;
  USBH_HC_DRV   *p_hc_drv;
  USBH_HUB_FNCT *p_hub_fnct;

  p_hc_drv = &p_hc->Drv;
  p_hub_fnct = &p_hc->RH_Fnct;

  ok = p_hc_drv->RH_API_Ptr->IntEn(p_hc_drv, DEF_DISABLED);
  if (ok == DEF_FAIL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }

  p_hub_fnct->State = USBH_HUB_STATE_SUSPEND;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                       USBH_HUB_PortResetProcess()
 *
 * @brief    Resets given port on hub.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    port_nbr    Port number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) This function MUST only be called by hub task. Caller MUST acquire lock on hub fnct.
 *******************************************************************************************************/
void USBH_HUB_PortResetProcess(USBH_HUB_FNCT *p_hub_fnct,
                               CPU_INT16U    port_nbr,
                               RTOS_ERR      *p_err)
{
  CPU_INT08U      retry;
  USBH_HUB_STATUS port_status;

  LOG_VRB(("Reset port #", (u)port_nbr));

  //                                                               Enable port reset on hub.
  USBH_HUB_PortFeatureSet(p_hub_fnct,
                          USBH_HUB_FEATURE_SEL_PORT_RESET,
                          port_nbr,
                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               Wait until reset is complete.
  retry = 3u;
  do {
    KAL_Dly(USBH_HUB_DLY_DEV_RESET);

    USBH_HUB_PortStatusGet(p_hub_fnct,
                           port_nbr,
                           &port_status,
                           p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    retry--;
  } while ((DEF_BIT_IS_CLR(port_status.Chng, USBH_HUB_STATUS_C_PORT_RESET) == DEF_YES)
           && (DEF_BIT_IS_SET(port_status.Status, USBH_HUB_STATUS_PORT_CONN) == DEF_YES)
           && (retry > 0u));

  if ((retry == 0u)
      && (DEF_BIT_IS_CLR(port_status.Chng, USBH_HUB_STATUS_C_PORT_RESET) == DEF_YES)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    return;
  }

  //                                                               Clr port reset change bit.
  if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_RESET) == DEF_YES) {
    USBH_HUB_PortFeatureClr(p_hub_fnct,
                            USBH_HUB_FEATURE_SEL_C_PORT_RESET,
                            port_nbr,
                            p_err);
  }

  KAL_Dly(USBH_HUB_DLY_POST_DEV_RESET);                         // Wait 10ms after reset is cmpl, as spec dictates.

  return;
}

/****************************************************************************************************//**
 *                                           USBH_HUB_EventAlloc()
 *
 * @brief    Get a HUB event from pool.
 *
 * @return   Pointer to the HUB event allocated, if NO error(s),
 *           DEF_NULL,                       otherwise.
 *******************************************************************************************************/
USBH_HUB_EVENT *USBH_HUB_EventAlloc(void)
{
  RTOS_ERR       err_lib;
  USBH_HUB_EVENT *p_hub_event;

  p_hub_event = (USBH_HUB_EVENT *)Mem_DynPoolBlkGet(&USBH_HUB_Ptr->HubEventPool,
                                                    &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    p_hub_event = DEF_NULL;
  }

  return (p_hub_event);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_EventQPost()
 *
 * @brief    Posts a HUB event to the event Q.
 *
 * @param    p_hub_event     Pointer to the HUB event to add to Q.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
void USBH_HUB_EventQPost(USBH_HUB_EVENT *p_hub_event,
                         RTOS_ERR       *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_hub_event->NextPtr = USBH_HUB_Ptr->HubEventQHead;
  USBH_HUB_Ptr->HubEventQHead = p_hub_event;
  CORE_EXIT_ATOMIC();

  KAL_SemPost(USBH_HUB_Ptr->HubEventQSemHandle,
              KAL_OPT_POST_NONE,
              p_err);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_DevAtPortGet()
 *
 * @brief    Gets device at specified port of hub.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    port_nbr    Port number.
 *
 * @note     (1) Device list must be locked when calling this function.
 *******************************************************************************************************/
USBH_DEV *USBH_HUB_DevAtPortGet(USBH_HUB_FNCT *p_hub_fnct,
                                CPU_INT08U    port_nbr)
{
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  return (p_hub_fnct->DevPtrList[port_nbr - 1u]);
#else
  {
    CPU_INT08U dev_cnt;
    USBH_DEV   *p_dev;
    USBH_DEV   *p_dev_to_rtn;

    p_dev_to_rtn = DEF_NULL;
    p_dev = p_hub_fnct->DevListHeadPtr;
    for (dev_cnt = 0u; dev_cnt < p_hub_fnct->PortBusyCnt; dev_cnt++) {
      if (p_dev->PortNbr == port_nbr) {
        p_dev_to_rtn = p_dev;
        break;
      }

      p_dev = p_dev->HubListNextPtr;
    }

    return (p_dev_to_rtn);
  }
#endif
}

/********************************************************************************************************
 *                                           HCD-RELATED FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_HUB_RootEvent()
 *
 * @brief    Queue a root hub event.
 *
 * @param    host_nbr                    Host number.
 *
 * @param    hc_nbr                      Host controller number.
 *
 * @param    port_status_chng_bitmap     Bitmap that represents the ports for which an event occurred.
 *                                       See note #1.
 *
 * @note     (1) The port_status_chng_bitmap must be filled as described below.
 *           @verbatim
 *           -+--------+--------+--------+--------+--------+--------+--------+--------+
 *            |   7    |   6    |   5    |   4    |   3    |   2    |   1    |   0    |
 *           -+--------+--------+--------+--------+--------+--------+--------+--------+
 *            | Port 7 | Port 6 | Port 5 | Port 4 | Port 3 | Port 2 | Port 1 |  Hub   |
 *            | event  | event  | event  | event  | event  | event  | event  | event  |
 *           -+--------+--------+--------+--------+--------+--------+--------+--------+
 *           @endverbatim
 * @note     (2) This function MUST NOT be called more than once per ISR.
 *******************************************************************************************************/
void USBH_HUB_RootEvent(CPU_INT08U host_nbr,
                        CPU_INT08U hc_nbr,
                        CPU_INT08U port_status_chng_bitmap)
{
  USBH_HUB_FNCT *p_hub_fnct;
  USBH_HOST     *p_host;
  USBH_HC       *p_hc;
  RTOS_ERR      err;

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (port_status_chng_bitmap == DEF_BIT_NONE) {
    LOG_ERR(("Port bitmap arg to USBH_HUB_RootEvent() -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_INVALID_ARG)));
    return;
  }
#endif

  p_host = &USBH_Ptr->HostTblPtr[host_nbr];
#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)
  if (hc_nbr >= p_host->HC_NbrNext) {
    LOG_ERR(("Host controller number arg to USBH_HUB_RootEvent() -> ", RTOS_ERR_CODE_LOG_ARG_GET(RTOS_ERR_INVALID_ARG)));
    return;
  }
#endif

  p_hc = USBH_HC_PtrGet(p_host, hc_nbr, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Posting root event, retrieving HC -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  p_hub_fnct = &p_hc->RH_Fnct;

  USBH_HUB_RootIntEn(p_hub_fnct->Handle, DEF_DISABLED, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Disabling root hub int -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }

  p_hub_fnct->PortStatusChng = (port_status_chng_bitmap & DEF_BIT_FIELD(p_hub_fnct->NbrPort + 1u, 0u));

  p_hub_fnct->PersistentHubEvent.ArgPtr = (void *)p_hub_fnct;
  p_hub_fnct->PersistentHubEvent.Type = USBH_HUB_EVENT_TYPE_PORT_STATUS_CHNG;
  USBH_HUB_EventQPost(&p_hub_fnct->PersistentHubEvent, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("HUB event post -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               USBH_HUB_Probe()
 *
 * @brief    Determines if class driver can handle connected USB function.
 *
 * @param    dev_handle      Handle to device.
 *
 * @param    class_code      Function class code.
 *
 * @param    pp_class_fnct   Pointer that will receive the internal structure of the function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if function should     be handled by this class driver.
 *           DEF_FAIL, if function should not be handled by this class driver.
 *
 * @note     (1) This function is called by the core to determine if a USB function can be supported
 *               by this class driver. At the moment of this call, the function is not ready for
 *               communication.
 *
 * @note     (2) This function will return DEF_OK as soon as the function class code is 'HUB'. If any
 *               error happens (cannot allocate hub function, ...), p_err will be set accordingly.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static CPU_BOOLEAN USBH_HUB_Probe(USBH_DEV_HANDLE dev_handle,
                                  CPU_INT08U      class_code,
                                  void            **pp_class_fnct,
                                  RTOS_ERR        *p_err)
{
  if (class_code != USBH_CLASS_CODE_HUB) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (DEF_FAIL);
  } else {
    USBH_HUB_FNCT *p_hub_fnct;
    USBH_DEV      *p_dev;
    CORE_DECLARE_IRQ_STATE;

    p_hub_fnct = (USBH_HUB_FNCT *)Mem_DynPoolBlkGet(&USBH_HUB_Ptr->HubFnctPool,
                                                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (DEF_OK);
    }

    p_dev = USBH_DevGet(dev_handle);

    //                                                             Init HUB fnct.
    p_hub_fnct->State = USBH_HUB_STATE_NONE;
    p_hub_fnct->Handle = dev_handle;
    p_hub_fnct->EventHubDevHandle = dev_handle;
    p_hub_fnct->ErrCnt = 0u;
    p_hub_fnct->NbrPort = 0u;
    p_hub_fnct->Characteristics = 0u;
    p_hub_fnct->PwrOn2PwrGood = 0u;
    p_hub_fnct->PortStatusChng = DEF_BIT_NONE;
    p_hub_fnct->PortBusyCnt = DEF_BIT_NONE;
    p_hub_fnct->DevListHeadPtr = DEF_NULL;
    p_hub_fnct->DevListTailPtr = DEF_NULL;

    CORE_ENTER_ATOMIC();
    p_hub_fnct->DevPtr = p_dev;
    CORE_EXIT_ATOMIC();

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    {
      CPU_INT08U port_ix;

      for (port_ix = 0u; port_ix < USBH_HUB_NBR_PORT; port_ix++) {
        p_hub_fnct->DevPtrList[port_ix] = DEF_NULL;
      }
    }
#endif

    *pp_class_fnct = (void *)p_hub_fnct;

    LOG_VRB(("New dev probed at addr ", (u)p_hub_fnct->DevPtr->Addr));
  }

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_EP_Open()
 *
 * @brief    Sets entry for given endpoint/function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    ep_type         Endpoint type.
 *                               - USBH_EP_TYPE_INTR
 *                               - USBH_EP_TYPE_BULK
 *                               - USBH_EP_TYPE_ISOC
 *
 * @param    ep_dir_in       Endpoint direction.
 *                               - DEF_YES     Direction IN
 *                               - DEF_NO      Direction OUT
 *
 * @note     (1) This function will be called by the core for each endpoint declared by the function
 *               via endpoint descriptors and that were successfully opened.
 *
 * @note     (2) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_EP_Open(void           *p_class_fnct,
                             USBH_EP_HANDLE ep_handle,
                             CPU_INT08U     if_nbr,
                             CPU_INT08U     ep_type,
                             CPU_BOOLEAN    ep_dir_in)
{
  USBH_HUB_FNCT *p_hub_fnct;

  (void)&if_nbr;

  p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;

  if ((ep_type == USBH_EP_TYPE_INTR)
      && (ep_dir_in == DEF_YES)) {
    p_hub_fnct->EP_IntrHandle = ep_handle;

    LOG_VRB(("Intr EP opened for hub addr", (u)p_hub_fnct->DevPtr->AddrCur));
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_EP_Close()
 *
 * @brief    Clears endpoint entry for given endpoint/function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    ep_handle       Handle to endpoint.
 *
 * @param    if_nbr          Interface number.
 *
 * @note     (1) This function will be called by the core when a endpoint is closed.
 *               - (a) When a new alternate interface is selected.
 *
 * @note     (2) This function will not be called if an endpoint is closed in the following circumstances.
 *               The Disconn() function will be called instead.
 *               - (a) When the device is disconnected.
 *               - (b) When a new configuration is selected.
 *
 * @note     (3) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_EP_Close(void           *p_class_fnct,
                              USBH_EP_HANDLE ep_handle,
                              CPU_INT08U     if_nbr)
{
  USBH_HUB_FNCT *p_hub_fnct;

  (void)&if_nbr;

  p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;

  if (p_hub_fnct->EP_IntrHandle == ep_handle) {
    p_hub_fnct->EP_IntrHandle = DEF_NULL;

    LOG_VRB(("Intr EP closed for hub addr", (u)p_hub_fnct->DevPtr->AddrCur));
  }
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HUB_Conn()
 *
 * @brief    Initializes hub communication.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will be called by the core when all the endpoints have been successfully
 *               opened for given function and after it issued the SetConfiguration request. The
 *               function is hence ready for communication.
 *
 * @note     (2) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_Conn(void *p_class_fnct)
{
  CPU_INT08U      *p_desc_buf;
  CPU_INT08U      port_nbr;
  CPU_INT16U      len_rd;
  RTOS_ERR        err;
  USBH_HUB_FNCT   *p_hub_fnct;
  USBH_HUB_STATUS port_status;
  CPU_INT32U      std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  CORE_EXIT_ATOMIC();

  p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;
  p_desc_buf = USBH_HUB_Ptr->DescBufPtr;

  //                                                               ----------- READ & PARSE HUB DESCRIPTOR ------------
  len_rd = USBH_DevDescRd(p_hub_fnct->Handle,
                          USBH_DEV_REQ_RECIPIENT_DEV,
                          USBH_DEV_REQ_TYPE_CLASS,
                          USBH_HUB_DESC_TYPE_HUB,
                          0u,
                          USBH_HUB_DESC_LEN_MAX,
                          p_desc_buf,
                          USBH_HUB_DESC_LEN_MAX,
                          &err);
  if ((RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)
      || (len_rd < USBH_HUB_LEN_DESC_MIN)) {
    LOG_ERR(("WARNING: Cannot retrieve HUB desc. Hub will be disabled. Addr = ", (u)p_hub_fnct->DevPtr->Addr));
    return;
  }

  p_hub_fnct->NbrPort = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[2u]);
  p_hub_fnct->Characteristics = MEM_VAL_GET_INT16U_LITTLE(&p_desc_buf[3u]);
  p_hub_fnct->PwrOn2PwrGood = MEM_VAL_GET_INT08U_LITTLE(&p_desc_buf[5u]);

  //                                                               Rd device status.
  len_rd = USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                            USBH_DEV_REQ_GET_STATUS,
                            (USBH_DEV_REQ_RECIPIENT_DEV | USBH_DEV_REQ_TYPE_STD | USBH_DEV_REQ_DIR_DEV_TO_HOST),
                            0u,
                            0u,
                            p_desc_buf,
                            2u,
                            2u,
                            std_req_timeout_ms,
                            &err);

  //                                                               Rd hub status.
  len_rd = USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                            USBH_DEV_REQ_GET_STATUS,
                            (USBH_DEV_REQ_RECIPIENT_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_DIR_DEV_TO_HOST),
                            0u,
                            0u,
                            p_desc_buf,
                            4u,
                            4u,
                            std_req_timeout_ms,
                            &err);

  //                                                               ----------------- ENABLE HUB PORTS -----------------
  if (p_hub_fnct->NbrPort > USBH_HUB_NBR_PORT) {
    LOG_ERR(("WARNING: Hub with more than 7 ports connected, only first 7 will work. Nbr ports = ", (u)p_hub_fnct->NbrPort));

    p_hub_fnct->NbrPort = USBH_HUB_NBR_PORT;
  }

  for (port_nbr = 1u; port_nbr <= p_hub_fnct->NbrPort; port_nbr++) {
    USBH_HUB_PortFeatureSet(p_hub_fnct,                         // Set port pwr.
                            USBH_HUB_FEATURE_SEL_PORT_PWR,
                            port_nbr,
                            &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Setting port power ", RTOS_ERR_LOG_ARG_GET(err)));
    }
  }

  KAL_Dly(p_hub_fnct->PwrOn2PwrGood * 2u);                      // See Note (1).

  //                                                               ----------------- CHK PORT STATUS ------------------
  for (port_nbr = 1u; port_nbr <= p_hub_fnct->NbrPort; port_nbr++) {
    USBH_HUB_PortStatusGet(p_hub_fnct,
                           port_nbr,
                           &port_status,
                           &err);
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_CONN) == DEF_YES) {
        DEF_BIT_SET(p_hub_fnct->PortStatusChng, DEF_BIT(port_nbr));
      }
    } else {
      LOG_ERR(("Getting port status -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }
  }

  //                                                               ----------- START EVENTS RD FROM INTR EP -----------
  p_hub_fnct->State = USBH_HUB_STATE_CONN;

  if (p_hub_fnct->PortStatusChng == DEF_BIT_NONE) {
    USBH_HUB_EventRxStart(p_hub_fnct->Handle, p_hub_fnct);
  } else {
    p_hub_fnct->PersistentHubEvent.ArgPtr = (void *)p_hub_fnct;
    p_hub_fnct->PersistentHubEvent.Type = USBH_HUB_EVENT_TYPE_PORT_STATUS_CHNG;
    USBH_HUB_EventQPost(&p_hub_fnct->PersistentHubEvent, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("HUB event post -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }
  }

  LOG_VRB(("HUB fnct connected at addr ", (u)p_hub_fnct->DevPtr->Addr));
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_Disconn()
 *
 * @brief    Disconn hub function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will be called by the core in the following circumstances.
 *               - (a) The core is unable to open an endpoint for this function.
 *               - (b) An error occurs in the core before the SetConfiguration request is issued.
 *               - (c) The device has been disconnected.
 *               - (d) A new configuration has been selected.
 *
 * @note     (2) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_Disconn(void *p_class_fnct)
{
  RTOS_ERR      err_lib;
  USBH_HUB_FNCT *p_hub_fnct;

  p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;
  p_hub_fnct->State = USBH_HUB_STATE_NONE;

  //                                                               Free HUB fnct.
  Mem_DynPoolBlkFree(&USBH_HUB_Ptr->HubFnctPool,
                     (void *)p_hub_fnct,
                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Freeing HUB function -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
  }

  LOG_VRB(("HUB fnct disconn at addr ", (u)p_hub_fnct->DevPtr->Addr));
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_Suspend()
 *
 * @brief    Puts hub function in suspend state.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_Suspend(void *p_class_fnct)
{
  USBH_HUB_FNCT *p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;

  LOG_VRB(("HUB fnct suspend at addr ", (u)p_hub_fnct->DevPtr->Addr));

  p_hub_fnct->State = USBH_HUB_STATE_SUSPEND;
}
#endif

/****************************************************************************************************//**
 *                                               USBH_HUB_Resume()
 *
 * @brief    Resumes hub function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @note     (1) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_Resume(void *p_class_fnct)
{
  CPU_INT16U      port_nbr;
  USBH_HUB_FNCT   *p_hub_fnct;
  USBH_HUB_STATUS port_status;
  RTOS_ERR        err;

  p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;

  LOG_VRB(("HUB fnct resumed at addr ", (u)p_hub_fnct->DevPtr->Addr));

  p_hub_fnct->State = USBH_HUB_STATE_CONN;

  for (port_nbr = 1u; port_nbr < p_hub_fnct->NbrPort; port_nbr++) {
    USBH_HUB_PortStatusGet(p_hub_fnct,                          // Detect dev connected during suspend state.
                           port_nbr,
                           &port_status,
                           &err);
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_CONN) == DEF_YES) {
        DEF_BIT_SET(p_hub_fnct->PortStatusChng, DEF_BIT(port_nbr));
      }
    } else {
      LOG_ERR(("Get port status -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }
  }

  if (p_hub_fnct->PortStatusChng != DEF_BIT_NONE) {
    p_hub_fnct->PersistentHubEvent.ArgPtr = (void *)p_hub_fnct;
    p_hub_fnct->PersistentHubEvent.Type = USBH_HUB_EVENT_TYPE_PORT_STATUS_CHNG;
    USBH_HUB_EventQPost(&p_hub_fnct->PersistentHubEvent, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("HUB event post -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_TraceDump()
 *
 * @brief    Output class specific debug information on function.
 *
 * @param    p_class_fnct    Pointer to the internal structure of the function.
 *
 * @param    opt             Trace option. Unused, in this case.
 *
 * @param    trace_fnct      Function used to output trace.
 *
 * @note     (1) This function will never be called for a given function if the ProbeFnct() returned
 *               DEF_FAIL and/or *p_err != RTOS_ERR_NONE.
 *******************************************************************************************************/

#if ((USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED) \
  && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))
static void USBH_HUB_TraceDump(void                *p_class_fnct,
                               CPU_INT32U          opt,
                               USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_CHAR      str[3u];
  USBH_HUB_FNCT *p_hub_fnct;

  (void)&opt;

  p_hub_fnct = (USBH_HUB_FNCT *)p_class_fnct;

  trace_fnct("    ---------- USB Host (HUB specific debug information) -----------\r\n");
  trace_fnct("    | State | Nbr port | Nbr port used | Pwr   Comp  OverCur  TT   |\r\n");
  trace_fnct("    | ");

  //                                                               Output state.
  switch (p_hub_fnct->State) {
    case USBH_HUB_STATE_CONN:
      trace_fnct("CONN  | ");
      break;

    case USBH_HUB_STATE_SUSPEND:
      trace_fnct("SUSP  | ");
      break;

    case USBH_HUB_STATE_NONE:
    default:
      trace_fnct("????  | ");
      break;
  }

  //                                                               Output nbr port.
  (void)Str_FmtNbr_Int32U(p_hub_fnct->NbrPort,
                          3u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("      | ");

  //                                                               Output nbr of used port.
  (void)Str_FmtNbr_Int32U(p_hub_fnct->PortBusyCnt,
                          3u,
                          DEF_NBR_BASE_DEC,
                          ' ',
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);
  trace_fnct(str);
  trace_fnct("           | ");

  //                                                               Output pwr type.
  switch (USBH_HUB_PWR_MODE_GET(p_hub_fnct->Characteristics)) {
    case USBH_HUB_PWR_MODE_GANGED:
      trace_fnct("GANG  ");
      break;

    case USBH_HUB_PWR_MODE_INDIVIDUAL:
      trace_fnct("INDIV ");
      break;

    default:
      trace_fnct("????? ");
      break;
  }

  //                                                               Output compound device info.
  if (DEF_BIT_IS_SET(p_hub_fnct->Characteristics, USBH_HUB_COMPOUND_MASK) == DEF_YES) {
    trace_fnct("YES   ");
  } else {
    trace_fnct("NO    ");
  }

  //                                                               Output over current protection.
  switch (USBH_HUB_OVER_CUR_GET(p_hub_fnct->Characteristics)) {
    case USBH_HUB_OVER_CUR_GLOBAL:
      trace_fnct("GLOBAL   ");
      break;

    case USBH_HUB_OVER_CUR_INDIVIDUAL:
      trace_fnct("INDIV    ");
      break;

    default:
      trace_fnct("?????    ");
      break;
  }

  //                                                               Output Think Time (TT).
  switch (USBH_HUB_TT_GET(p_hub_fnct->Characteristics)) {
    case USBH_HUB_TT_8:
      trace_fnct("8    |");
      break;

    case USBH_HUB_TT_16:
      trace_fnct("16   |");
      break;

    case USBH_HUB_TT_24:
      trace_fnct("24   |");
      break;

    case USBH_HUB_TT_32:
      trace_fnct("32   |");
      break;

    default:
      trace_fnct("??   |");
      break;
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_IntrRxCmpl()
 *
 * @brief    Handles hub status change notification.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    ep_handle   Handle to endpoint.
 *
 * @param    p_buf       Pointer to the received buffer.
 *
 * @param    buf_len     Buffer length in octets.
 *
 * @param    xfer_len    Length of data received in octets
 *
 * @param    p_arg       Pointer to the caller argument.
 *
 * @param    err         Error code.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_IntrRxCmpl(USBH_DEV_HANDLE dev_handle,
                                USBH_EP_HANDLE  ep_handle,
                                CPU_INT08U      *p_buf,
                                CPU_INT32U      buf_len,
                                CPU_INT32U      xfer_len,
                                void            *p_arg,
                                RTOS_ERR        err)
{
  USBH_HUB_FNCT *p_hub_fnct;

  (void)&dev_handle;
  (void)&ep_handle;
  (void)&buf_len;
  (void)&xfer_len;

  if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_ABORT) {
    LOG_DBG(("WARNING: HUB Intr EP URB aborted."));
    return;
  }

  p_hub_fnct = (USBH_HUB_FNCT *)p_arg;

  if ((RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE)
      && (p_hub_fnct->State == USBH_HUB_STATE_CONN)) {
    if (p_hub_fnct->ErrCnt < 10u) {
      p_hub_fnct->ErrCnt++;
      USBH_EP_Abort(p_hub_fnct->Handle,
                    p_hub_fnct->EP_IntrHandle,
                    &err);

      USBH_HUB_EventRxStart(p_hub_fnct->Handle,                 // Retry URB.
                            p_hub_fnct);
    } else {
      LOG_ERR(("HUB comm - ", RTOS_ERR_LOG_ARG_GET(err)));
    }

    return;
  }

  p_hub_fnct->ErrCnt = 0u;
  p_hub_fnct->EventHubDevHandle = dev_handle;
  p_hub_fnct->PortStatusChng = p_buf[0u];

  p_hub_fnct->PersistentHubEvent.ArgPtr = (void *)p_hub_fnct;
  p_hub_fnct->PersistentHubEvent.Type = USBH_HUB_EVENT_TYPE_PORT_STATUS_CHNG;
  USBH_HUB_EventQPost(&p_hub_fnct->PersistentHubEvent, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("HUB event post -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_EventRxStart()
 *
 * @brief    Issue an asynchronous interrupt request to receive hub events.
 *
 * @param    hub_dev_handle  Handle to hub device.
 *
 * @param    p_hub_fnct      Pointer to the the hub device structure.
 *******************************************************************************************************/
static void USBH_HUB_EventRxStart(USBH_DEV_HANDLE hub_dev_handle,
                                  USBH_HUB_FNCT   *p_hub_fnct)
{
  RTOS_ERR err;

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    USBH_HUB_RootIntEn(p_hub_fnct->Handle, DEF_ENABLED, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Enabling RH int -> ", RTOS_ERR_LOG_ARG_GET(err)));
    }

    return;
  }

    #if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  USBH_EP_IntrRxAsync(hub_dev_handle,
                      p_hub_fnct->EP_IntrHandle,
                      p_hub_fnct->StatusBufPtr,
                      USBH_HUB_STATUS_CHNG_BUF_LEN,
                      USBH_HUB_IntrRxCmpl,
                      (void *)p_hub_fnct,
                      &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Submitting status chng rd req -> ", RTOS_ERR_LOG_ARG_GET(err)));
  }
    #else
  (void)&hub_dev_handle;
    #endif
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortReq()
 *
 * @brief    Issue a port request that will be processed by the HUB task.
 *
 * @param    hub_dev_handle  Pointer to the the hub device structure.
 *
 * @param    port_req        Type of port request to perform.
 *
 * @param    port_nbr        Port number on which to perform the request.
 *
 * @param    cmpl_callback   Pointer to the callback function.
 *
 * @param    p_arg           Pointer to the argument for callback function.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBH_HUB_PortReq(USBH_DEV_HANDLE    hub_dev_handle,
                             USBH_HUB_PORT_REQ  port_req,
                             CPU_INT08U         port_nbr,
                             USBH_PORT_REQ_CMPL cmpl_callback,
                             void               *p_arg,
                             RTOS_ERR           *p_err)
{
  USBH_HUB_EVENT              *p_hub_event;
  USBH_HUB_EVENT_PORT_REQ_ARG *p_req_arg;

  //                                                               Check as much as possible, before posting event.
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_DISABLED)
  RTOS_ASSERT_DBG_ERR_SET((USBH_DEV_HANDLE_IS_RH(hub_dev_handle) == DEF_YES), *p_err, RTOS_ERR_INVALID_ARG,; );
#endif

  p_hub_event = USBH_HUB_EventAlloc();
  if (p_hub_event == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALLOC);
    return;
  }

  p_hub_event->Type = USBH_HUB_EVENT_TYPE_PORT_REQ;

  p_req_arg = (USBH_HUB_EVENT_PORT_REQ_ARG *)p_hub_event->ArgPtr;

  p_req_arg->HUB_DevHandle = hub_dev_handle;
  p_req_arg->PortReq = port_req;
  p_req_arg->PortNbr = port_nbr;
  p_req_arg->PortReqCmplCallback = cmpl_callback;
  p_req_arg->CallbackArgPtr = p_arg;

  USBH_HUB_EventQPost(p_hub_event,
                      p_err);

  return;
}

/****************************************************************************************************//**
 *                                       USBH_HUB_EventTaskHandler()
 *
 * @brief    Process hub events.
 *
 * @param    p_arg   Pointer to the task argument. Unused, in this case.
 *******************************************************************************************************/
static void USBH_HUB_EventTaskHandler(void *p_arg)
{
  USBH_HC         *p_hc;
  USBH_HUB_EVENT  *p_hub_event;
  USBH_HUB_FNCT   *p_hub_fnct;
  USBH_DEV_HANDLE hub_dev_handle;
  RTOS_ERR        err;

  (void)&p_arg;

  USBH_Ptr->HUB_TaskUUID = KAL_TaskUUID_Get();

  while (DEF_ON) {
    p_hub_event = USBH_HUB_EventQPend(&err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      LOG_ERR(("Retrieving hub from Q -> ", RTOS_ERR_LOG_ARG_GET(err)));
      continue;
    }

    if (p_hub_event->Type == USBH_HUB_EVENT_TYPE_PORT_STATUS_CHNG) {
      p_hub_fnct = (USBH_HUB_FNCT *)p_hub_event->ArgPtr;

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
      hub_dev_handle = p_hub_fnct->EventHubDevHandle;
#else
      hub_dev_handle = p_hub_fnct->Handle;
#endif

      if ((p_hub_fnct->State != USBH_HUB_STATE_CONN)
          && (p_hub_fnct->State != USBH_HUB_STATE_SUSPEND)) {
        LOG_DBG(("HUB was disconnected"));
        continue;
      }

      p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle,
                                &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        LOG_ERR(("Retrieving HC ptr from HUB -> ", RTOS_ERR_LOG_ARG_GET(err)));
        continue;
      }

      //                                                           ------------ PROCESS HUB STATUS CHANGE -------------
      if (DEF_BIT_IS_SET(p_hub_fnct->PortStatusChng, USBH_HUB_STATUS_CHNG) == DEF_YES) {
        USBH_HUB_STATUS hub_status;

        USBH_HUB_StatusGet(p_hub_fnct,
                           &hub_status,
                           &err);
        if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
          //                                                       -------------- HUB LOCAL POWER CHANGE --------------
          if (DEF_BIT_IS_SET(hub_status.Chng, USBH_HUB_STATUS_C_LOCAL_POWER) == DEF_YES) {
            USBH_HUB_FeatureClr(p_hub_fnct,
                                USBH_HUB_STATUS_C_LOCAL_POWER,
                                &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
              LOG_ERR(("Clearing hub feature local power change -> ", RTOS_ERR_LOG_ARG_GET(err)));
            }

            if (DEF_BIT_IS_CLR(hub_status.Status, USBH_HUB_STATUS_LOCAL_POWER) == DEF_YES) {
              LOG_DBG(("!! WARNING !! Local power supply good on hub at addr ", (u)USBH_HANDLE_DEV_ADDR_GET(p_hub_fnct->Handle)));
            } else {
              LOG_DBG(("!! WARNING !! Local power supply LOST on hub at addr ", (u)USBH_HANDLE_DEV_ADDR_GET(p_hub_fnct->Handle)));
            }
          }

          //                                                       ------------- HUB OVER-CURRENT CHANGE --------------
          if (DEF_BIT_IS_SET(hub_status.Chng, USBH_HUB_STATUS_C_OVER_CUR) == DEF_YES) {
            USBH_HUB_FeatureClr(p_hub_fnct,
                                USBH_HUB_STATUS_C_OVER_CUR,
                                &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
              LOG_ERR(("Clearing hub feature over current change -> ", RTOS_ERR_LOG_ARG_GET(err)));
            }

            if (DEF_BIT_IS_CLR(hub_status.Status, USBH_HUB_STATUS_OVER_CUR) == DEF_YES) {
              LOG_DBG(("!! WARNING !! No over-current condition on hub at addr ", (u)USBH_HANDLE_DEV_ADDR_GET(p_hub_fnct->Handle)));
            } else {
              LOG_DBG(("!! WARNING !! Over-current condition on hub at addr ", (u)USBH_HANDLE_DEV_ADDR_GET(p_hub_fnct->Handle)));
            }
          }
        } else {
          LOG_ERR(("Retrieving hub status -> ", RTOS_ERR_LOG_ARG_GET(err)));
        }

        DEF_BIT_CLR(p_hub_fnct->PortStatusChng, (CPU_INT08U)USBH_HUB_STATUS_CHNG);
      }

      //                                                           ---------- PROCESS HUB PORT CHNG REQUESTS ----------
      while (p_hub_fnct->PortStatusChng != DEF_BIT_NONE) {
        CPU_INT16U      port_nbr;
        USBH_HUB_STATUS port_status;

        port_nbr = CPU_CntTrailZeros08(p_hub_fnct->PortStatusChng);

        port_status.Chng = 0u;
        port_status.Status = 0u;
        USBH_HUB_PortStatusGet(p_hub_fnct,
                               port_nbr,
                               &port_status,
                               &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          LOG_ERR(("Retrieving port status -> ", RTOS_ERR_LOG_ARG_GET(err)));
          continue;
        }

        //                                                         ------------- CONNECTION STATUS CHANGE -------------
        if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_CONN) == DEF_YES) {
          USBH_HUB_PortFeatureClr(p_hub_fnct,
                                  USBH_HUB_FEATURE_SEL_C_PORT_CONN,
                                  port_nbr,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            LOG_ERR(("Clearing port feature -> ", RTOS_ERR_LOG_ARG_GET(err)));
          }

          //                                                       Dev has been connected on given port.
          if (DEF_BIT_IS_SET(port_status.Status, USBH_HUB_STATUS_PORT_CONN) == DEF_YES) {
            //                                                     Free already conn'd dev if any. Useful in case...
            //                                                     ... dev was disconnected->reconnected while suspend.
            USBH_HUB_PortDisconnProcess(p_hub_fnct, port_nbr);

            KAL_Dly(USBH_HUB_DLY_PORT_DEBOUNCE);                // Ensure port debounce.

            USBH_HUB_PortConnProcess(p_hub_fnct, port_nbr);
          } else {                                              // --------------- DEV HAS BEEN REMOVED ---------------
            USBH_HUB_PortDisconnProcess(p_hub_fnct, port_nbr);
          }
        }

        //                                                         ------------- PORT RESET STATUS CHANGE -------------
        if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_RESET) == DEF_YES) {
          USBH_HUB_PortFeatureClr(p_hub_fnct,
                                  USBH_HUB_FEATURE_SEL_C_PORT_RESET,
                                  port_nbr,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            LOG_ERR(("Clearing port feature -> ", RTOS_ERR_LOG_ARG_GET(err)));
          }
        }

        //                                                         ------------ PORT ENABLE STATUS CHANGE -------------
        if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_EN) == DEF_YES) {
          USBH_HUB_PortFeatureClr(p_hub_fnct,
                                  USBH_HUB_FEATURE_SEL_C_PORT_EN,
                                  port_nbr,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            LOG_ERR(("Clearing port feature -> ", RTOS_ERR_LOG_ARG_GET(err)));
          }
        }

        //                                                         --------- PORT OVER-CURRENT STATUS CHANGE ----------
        if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_OVER_CUR) == DEF_YES) {
          USBH_HUB_PortFeatureClr(p_hub_fnct,
                                  USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR,
                                  port_nbr,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            LOG_ERR(("Clearing port feature -> ", RTOS_ERR_LOG_ARG_GET(err)));
          }

          if (p_hub_fnct->DevPtr != DEF_NULL) {
            LOG_DBG(("!! WARNING !! Over-current state on hub addr ", (u)p_hub_fnct->DevPtr->Addr,
                     " on port #", (u)port_nbr));
          } else {
            LOG_DBG(("WARNING: Over-current state on root hub on port #", (u)port_nbr));
          }
        }

        //                                                         ------------ PORT REMOTE WAKE-UP CHANGE ------------
        //                                                         @@@@ Remote wake-up not supported.
        if (DEF_BIT_IS_SET(port_status.Chng, USBH_HUB_STATUS_C_PORT_SUSPEND) == DEF_YES) {
          USBH_HUB_PortFeatureClr(p_hub_fnct,
                                  USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND,
                                  port_nbr,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            LOG_ERR(("Clearing port feature -> ", RTOS_ERR_LOG_ARG_GET(err)));
          }
        }

        DEF_BIT_CLR(p_hub_fnct->PortStatusChng, DEF_BIT(port_nbr));
      }

      USBH_HUB_EventRxStart(hub_dev_handle, p_hub_fnct);        // Restart HUB port chng event once last port treated.

      continue;
    } else if (p_hub_event->Type == USBH_HUB_EVENT_TYPE_PORT_REQ) {
      USBH_HUB_EVENT_PORT_REQ_ARG *p_port_req_arg = (USBH_HUB_EVENT_PORT_REQ_ARG *)p_hub_event->ArgPtr;
      USBH_HUB_PORT_REQ           port_req = p_port_req_arg->PortReq;
      CPU_INT08U                  port_nbr = p_port_req_arg->PortNbr;
      USBH_PORT_REQ_CMPL          callback = p_port_req_arg->PortReqCmplCallback;
      void                        *p_req_arg = p_port_req_arg->CallbackArgPtr;
      RTOS_ERR                    err_lib;

      hub_dev_handle = p_port_req_arg->HUB_DevHandle;

      Mem_DynPoolBlkFree(&USBH_HUB_Ptr->HubEventPool,
                         (void *)p_hub_event,
                         &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing Hub Port Req hub event -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
      }

      USBH_HUB_PortReqProcess(hub_dev_handle,
                              port_req,
                              port_nbr,
                              &err);

      if (callback != DEF_NULL) {
        callback(hub_dev_handle,
                 port_req,
                 port_nbr,
                 p_req_arg,
                 err);
      }
    } else if (p_hub_event->Type == USBH_HUB_EVENT_TYPE_DEV_CONFIG_SET) {
      USBH_HUB_EVENT_DEV_CONFIG_SET_ARG *p_set_cfg_arg = (USBH_HUB_EVENT_DEV_CONFIG_SET_ARG *)p_hub_event->ArgPtr;
      USBH_DEV_HANDLE                   dev_handle = p_set_cfg_arg->DevHandle;
      CPU_INT08U                        cfg_nbr = p_set_cfg_arg->CfgNbr;
      RTOS_ERR                          err_lib;

      Mem_DynPoolBlkFree(&USBH_HUB_Ptr->HubEventPool,
                         (void *)p_hub_event,
                         &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing Dev Config Set hub event -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
      }

      USBH_DevConfigSetProcess(dev_handle,
                               cfg_nbr,
                               &err);
#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
    } else if (p_hub_event->Type == USBH_HUB_EVENT_TYPE_IF_ALT_SET) {
      USBH_HUB_EVENT_IF_ALT_SET_ARG *p_if_alt_set_arg = (USBH_HUB_EVENT_IF_ALT_SET_ARG *)p_hub_event->ArgPtr;
      USBH_IF_ALT_SET_CMPL          callback = p_if_alt_set_arg->IF_AltSetCmplCallback;
      void                          *p_req_arg = p_if_alt_set_arg->CallbackArgPtr;
      USBH_DEV_HANDLE               dev_handle = p_if_alt_set_arg->DevHandle;
      USBH_FNCT_HANDLE              fnct_handle = p_if_alt_set_arg->FnctHandle;
      CPU_INT08U                    if_ix = p_if_alt_set_arg->IF_Ix;
      CPU_INT08U                    if_alt_ix = p_if_alt_set_arg->IF_AltIx;
      CPU_INT08U                    if_alt_ix_prev;
      RTOS_ERR                      err_lib;

      Mem_DynPoolBlkFree(&USBH_HUB_Ptr->HubEventPool,
                         (void *)p_hub_event,
                         &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing IF Alt Set hub event -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
      }

      if_alt_ix_prev = USBH_IF_AltSetProcess(dev_handle,
                                             fnct_handle,
                                             if_ix,
                                             if_alt_ix,
                                             &err);

      if (callback != DEF_NULL) {
        callback(dev_handle,
                 fnct_handle,
                 if_ix,
                 if_alt_ix,
                 if_alt_ix_prev,
                 p_req_arg,
                 err);
      }
#endif
    } else if (p_hub_event->Type == USBH_HUB_EVENT_TYPE_HC_OPER) {
      USBH_HUB_EVENT_HC_OPER *p_hc_oper_arg = (USBH_HUB_EVENT_HC_OPER *)p_hub_event->ArgPtr;
      USBH_HC_OPER_TYPE      p_hc_oper_type = p_hc_oper_arg->Type;
      USBH_HC_OPER_CMPL      callback_fnct = p_hc_oper_arg->HC_OperCmplCallback;
      void                   *p_req_arg = p_hc_oper_arg->CallbackArgPtr;
      USBH_HC_DRV            *p_hc_drv;
      RTOS_ERR               err_lib;

      p_hc = p_hc_oper_arg->HC_Ptr;
      p_hc_drv = &p_hc->Drv;

      Mem_DynPoolBlkFree(&USBH_HUB_Ptr->HubEventPool,
                         (void *)p_hub_event,
                         &err_lib);
      if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        LOG_ERR(("Freeing HC Oper hub event -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
      }

      switch (p_hc_oper_type) {
        case USBH_HC_OPER_TYPE_STOP:
          USBH_HUB_PortReqProcess(p_hc->RH_Fnct.Handle,
                                  USBH_HUB_PORT_REQ_DISCONN,
                                  USBH_HUB_PORT_ALL,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          USBH_HUB_RootStop(p_hc, &err);                        // Stop root hub.
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            return;
          }

          p_hc_drv->API_Ptr->Stop(p_hc_drv, &err);              // Stop host controller driver.

                     #if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
          {
            CORE_DECLARE_IRQ_STATE;

            CORE_ENTER_ATOMIC();
            p_hc->IsStopped = DEF_YES;
            CORE_EXIT_ATOMIC();
          }
                     #endif

          LOG_VRB(("Stopped HC #", (u)p_hc->Ix, " of host #", (u)p_hc_drv->HostNbr));
          break;

        case USBH_HC_OPER_TYPE_RESUME:
          p_hc_drv->API_Ptr->Resume(p_hc_drv, &err);            // Resume HCD.
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          USBH_HUB_RootResume(p_hc, &err);                      // Resume root hub.
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }
          //                                                       Resume root hub ports.
          USBH_HUB_PortReqProcess(p_hc->RH_Fnct.Handle,
                                  USBH_HUB_PORT_REQ_RESUME,
                                  USBH_HUB_PORT_ALL,
                                  &err);

          LOG_VRB(("Resumed HC #", (u)p_hc->Ix, " of host #", (u)p_hc_drv->HostNbr));
          break;

        case USBH_HC_OPER_TYPE_SUSPEND:
          //                                                       Suspend root hub ports.
          USBH_HUB_PortReqProcess(p_hc->RH_Fnct.Handle,
                                  USBH_HUB_PORT_REQ_SUSPEND,
                                  USBH_HUB_PORT_ALL,
                                  &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          USBH_HUB_RootSuspend(p_hc, &err);                     // Suspend root hub.
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            break;
          }

          //                                                       Suspend HCD.
          p_hc_drv->API_Ptr->Suspend(p_hc_drv, &err);

          LOG_VRB(("Suspended HC #", (u)p_hc->Ix, " of host #", (u)p_hc_drv->HostNbr));
          break;

        default:
          RTOS_ERR_SET(err, RTOS_ERR_INVALID_ARG);
          break;
      }
      if (callback_fnct != DEF_NULL) {
        callback_fnct(p_hc_drv->HostNbr,
                      p_hc_drv->Nbr,
                      p_req_arg,
                      err);
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortReqProcess()
 *
 * @brief    Process request on given port on given hub.
 *
 * @param    hub_dev_handle  Handle to hub device.
 *
 * @param    port_req        Port request type.
 *
 * @param    port_nbr        Port number, can be USBH_HUB_PORT_ALL.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) This function MUST only be called from the hub task. Caller MUST NOT acquire lock.
 *******************************************************************************************************/
static void USBH_HUB_PortReqProcess(USBH_DEV_HANDLE   hub_dev_handle,
                                    USBH_HUB_PORT_REQ port_req,
                                    CPU_INT08U        port_nbr,
                                    RTOS_ERR          *p_err)
{
  CPU_INT08U    port_chng = DEF_BIT_NONE;
  USBH_HC       *p_hc;
  USBH_HUB_FNCT *p_hub_fnct;
#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  USBH_DEV *p_hub_dev = DEF_NULL;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  if (USBH_DEV_HANDLE_IS_RH(hub_dev_handle) == DEF_YES) {
    CPU_INT08U host_ix = USBH_HANDLE_HOST_IX_GET(hub_dev_handle);
    CPU_INT08U hc_ix = USBH_HANDLE_HC_IX_GET(hub_dev_handle);

    p_hc = USBH_HC_PtrGet(&USBH_Ptr->HostTblPtr[host_ix],
                          hc_ix,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    p_hub_fnct = &p_hc->RH_Fnct;
  } else {
        #if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
    p_hub_dev = USBH_DevGet(hub_dev_handle);
    if (p_hub_dev == DEF_NULL) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
      return;
    }

    if (p_hub_dev->ClassDrvPtr != &USBH_HUB_Drv) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
      return;
    }

    p_hub_fnct = (USBH_HUB_FNCT *)p_hub_dev->ClassFnctPtr;
        #else
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    return;
        #endif
  }

  RTOS_ASSERT_DBG_ERR_SET((port_nbr != 0u), *p_err, RTOS_ERR_INVALID_ARG,; );

  RTOS_ASSERT_DBG_ERR_SET(((port_nbr <= p_hub_fnct->NbrPort)
                           || (port_nbr == USBH_HUB_PORT_ALL)), *p_err, RTOS_ERR_INVALID_ARG,; );

  if (p_hub_fnct->State == USBH_HUB_STATE_SUSPEND) {            // Ensure hub is not suspended.
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    return;
  }

  if (port_nbr != USBH_HUB_PORT_ALL) {
    DEF_BIT_SET(port_chng, DEF_BIT(port_nbr));
  } else {
    port_chng = DEF_BIT_FIELD(p_hub_fnct->NbrPort, 1u);
  }

  p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_hub_fnct->DevPtr == DEF_NULL) {                         // Disable root hub interruptions.
    USBH_HUB_RootIntEn(p_hub_fnct->Handle, DEF_DISABLED, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }

  while (port_chng != DEF_BIT_NONE) {
    CPU_INT08U dev_cnt;
    CPU_INT08U port_nbr_cur;
    USBH_DEV   *p_dev;
    USBH_DEV   *p_dev_next;

    port_nbr_cur = CPU_CntTrailZeros08(port_chng);

    switch (port_req) {
      case USBH_HUB_PORT_REQ_SUSPEND:
        p_dev = USBH_HUB_DevAtPortGet(p_hub_fnct, port_nbr_cur);
        if (p_dev == DEF_NULL) {
          break;
        }

        LOG_VRB(("Suspend port #", (u)port_nbr));

        p_dev_next = p_dev;
        dev_cnt = 1u;
        while (dev_cnt > 0u) {                                  // Suspend all dev connected to dowstream hub port.
          dev_cnt--;

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
          //                                                       Chk if next dev is a hub.
          if (p_dev_next->ClassDrvPtr == &USBH_HUB_Drv) {
            dev_cnt += ((USBH_HUB_FNCT *)p_dev_next->ClassFnctPtr)->PortBusyCnt;
          }
#endif

          USBH_DevSuspend(p_dev_next->Handle);

          p_dev_next = p_dev_next->HubListNextPtr;
        }

        USBH_HUB_PortFeatureSet(p_dev->HubFnctPtr,              // Suspend port.
                                USBH_HUB_FEATURE_SEL_PORT_SUSPEND,
                                p_dev->PortNbr,
                                p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end;
        }
        break;

      case USBH_HUB_PORT_REQ_RESUME:
        p_dev = USBH_HUB_DevAtPortGet(p_hub_fnct, port_nbr_cur);
        if (p_dev == DEF_NULL) {
          break;
        }

        LOG_VRB(("Suspend port #", (u)port_nbr));

        USBH_HUB_PortFeatureClr(p_dev->HubFnctPtr,
                                USBH_HUB_FEATURE_SEL_PORT_SUSPEND,
                                p_dev->PortNbr,
                                p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto end;
        }

        KAL_Dly(USBH_HUB_DLY_PORT_RESUME);                      // Give time to hub to execute resume signaling.

        p_dev_next = p_dev;
        dev_cnt = 1u;
        while (dev_cnt > 0u) {                                  // Resume all dev on downstream hub port.
          dev_cnt--;

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
          //                                                       Chk if next dev is a hub.
          if (p_dev_next->ClassDrvPtr == &USBH_HUB_Drv) {
            //                                                     Suspend all dev connected to dowstream hub...
            dev_cnt += ((USBH_HUB_FNCT *)p_dev_next->ClassFnctPtr)->PortBusyCnt;
          }
#endif

          USBH_DevResume(p_dev_next->Handle);

          p_dev_next = p_dev_next->HubListNextPtr;
        }
        break;

      case USBH_HUB_PORT_REQ_RESET:
        USBH_HUB_PortDisconnProcess(p_hub_fnct, port_nbr_cur);

        KAL_Dly(USBH_HUB_DLY_PORT_DEBOUNCE);

        USBH_HUB_PortConnProcess(p_hub_fnct, port_nbr_cur);
        break;

      case USBH_HUB_PORT_REQ_DISCONN:
        USBH_HUB_PortDisconnProcess(p_hub_fnct, port_nbr_cur);
        break;

      default:
        break;
    }

    DEF_BIT_CLR(port_chng, DEF_BIT(port_nbr_cur));
  }

end:
  if (p_hub_fnct->DevPtr == DEF_NULL) {                         // Enable root hub interruptions.
    RTOS_ERR err_int_en;

    USBH_HUB_RootIntEn(p_hub_fnct->Handle,
                       DEF_ENABLED,
                       &err_int_en);
    if (RTOS_ERR_CODE_GET(err_int_en) != RTOS_ERR_NONE) {
      LOG_ERR(("Enabling root hub int -> ", RTOS_ERR_LOG_ARG_GET(err_int_en)));
    }
  }

  return;
}

/****************************************************************************************************//**
 *                                       USBH_HUB_PortConnProcess()
 *
 * @brief    Connects a device from a hub.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    port_nbr    Port number.
 *
 * @note     (1) This function MUST only be called from the hub task. Caller MUST acquire lock on hub.
 *******************************************************************************************************/
static void USBH_HUB_PortConnProcess(USBH_HUB_FNCT *p_hub_fnct,
                                     CPU_INT08U    port_nbr)
{
  USBH_HOST       *p_host;
  USBH_HC         *p_hc;
  USBH_DEV        *p_dev;
  USBH_DEV_SPD    dev_spd;
  USBH_HUB_STATUS port_status;
  RTOS_ERR        err = RTOS_ERR_INIT_CODE(RTOS_ERR_NONE);      // Prevent 'used before set' compiler warning.

  USBH_HUB_PortResetProcess(p_hub_fnct,                         // Execute first port reset.
                            port_nbr,
                            &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Resetting port -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  USBH_HUB_PortStatusGet(p_hub_fnct,
                         port_nbr,
                         &port_status,
                         &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Retrieving port status -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  //                                                               Ensure dev is still present.
  if (DEF_BIT_IS_CLR(port_status.Status, USBH_HUB_STATUS_PORT_CONN) == DEF_YES) {
    return;
  }

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    LOG_VRB(("Device conn'd on root hub port #", (u)port_nbr));
  } else {
    LOG_VRB(("Device conn'd on hub addr ", (u)p_hub_fnct->DevPtr->Addr, " port #", (u)port_nbr));
  }

  //                                                               Determine dev spd.
  switch (USBH_HUB_STATUS_PORT_SPD_GET(port_status.Status)) {
    case USBH_HUB_STATUS_PORT_SPD_FULL:
      dev_spd = USBH_DEV_SPD_FULL;
      break;

    case USBH_HUB_STATUS_PORT_SPD_HIGH:
      dev_spd = USBH_DEV_SPD_HIGH;
      break;

    case USBH_HUB_STATUS_PORT_SPD_LOW:
    default:
      dev_spd = USBH_DEV_SPD_LOW;
      break;
  }

  p_host = &USBH_Ptr->HostTblPtr[USBH_HANDLE_HOST_IX_GET(p_hub_fnct->Handle)];
  p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Retrieving hub HC ptr -> ", RTOS_ERR_LOG_ARG_GET(err)));
    return;
  }

  p_dev = USBH_DevConn(dev_spd,                                 // Report dev conn to usbh dev module.
                       p_host,
                       p_hc,
                       p_hub_fnct,
                       port_nbr);
  if (p_dev != DEF_NULL) {                                      // Add dev to hub's list. Null dev means conn failed.
        #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
    p_hub_fnct->DevPtrList[port_nbr - 1u] = p_dev;
        #endif
    USBH_HUB_DevListAdd(p_dev, p_hub_fnct);
  }
}

/****************************************************************************************************//**
 *                                       USBH_HUB_PortDisconnProcess()
 *
 * @brief    Disconnects a device from a hub.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    port_nbr    Port number.
 *
 * @note     (1) If the disconnected device is a hub, this function will ensure to disconnect all the
 *               devices connected to it and to any downstream hubs.
 *
 * @note     (2) This function MUST only be called from the hub task. Caller MUST acquire lock on hub.
 *******************************************************************************************************/
static void USBH_HUB_PortDisconnProcess(USBH_HUB_FNCT *p_hub_fnct,
                                        CPU_INT08U    port_nbr)
{
  CPU_INT08U      dev_cnt;
  USBH_DEV        *p_dev;
  USBH_DEV        *p_dev_to_free;
  USBH_HUB_STATUS port_status;
  RTOS_ERR        err;

  dev_cnt = 0u;
  p_dev = USBH_HUB_DevAtPortGet(p_hub_fnct, port_nbr);
  if (p_dev == DEF_NULL) {
    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  if (p_dev->ClassDrvPtr == &USBH_HUB_Drv) {                    // Chk if dev is a hub.
    dev_cnt = ((USBH_HUB_FNCT *)p_dev->ClassFnctPtr)->PortBusyCnt;
  }
#endif

  p_dev_to_free = p_dev;
  p_dev = p_dev->HubListNextPtr;

  USBH_HUB_DevListRemove(p_dev_to_free, p_hub_fnct);

  USBH_DevDisconn(p_dev_to_free->Handle);

  while (dev_cnt > 0u) {
    dev_cnt--;

    p_dev_to_free = p_dev;
    p_dev = p_dev->HubListNextPtr;

    USBH_HUB_DevListRemove(p_dev_to_free, DEF_NULL);

        #if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
    //                                                             Chk if dev is a hub.
    if (p_dev_to_free->ClassDrvPtr == &USBH_HUB_Drv) {
      dev_cnt += ((USBH_HUB_FNCT *)p_dev_to_free->ClassFnctPtr)->PortBusyCnt;
    }
        #endif

    USBH_DevDisconn(p_dev_to_free->Handle);
  }

    #if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  p_hub_fnct->DevPtrList[port_nbr - 1u] = DEF_NULL;
    #endif

  //                                                               Disable port if not already disabled.
  USBH_HUB_PortStatusGet(p_hub_fnct,
                         port_nbr,
                         &port_status,
                         &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  if (DEF_BIT_IS_SET(port_status.Status, USBH_HUB_STATUS_PORT_EN) == DEF_YES) {
    USBH_HUB_PortFeatureClr(p_hub_fnct,
                            USBH_HUB_FEATURE_SEL_PORT_EN,
                            port_nbr,
                            &err);
  }
}

/****************************************************************************************************//**
 *                                           USBH_HUB_StatusGet()
 *
 * @brief    Retrieve hub status.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    p_status    Pointer to the port status structure that will receive the port status.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) HUB class specific request GET PORT STATUS is defined in 'Universal Serial Bus
 *               specification, revision 2.0, section 11.24.2.7'.
 *******************************************************************************************************/
static void USBH_HUB_StatusGet(USBH_HUB_FNCT   *p_hub_fnct,
                               USBH_HUB_STATUS *p_status,
                               RTOS_ERR        *p_err)
{
  CPU_BOOLEAN ok;
  USBH_HC     *p_hc;

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    LOG_VRB(("Request root hub status"));

    p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    ok = p_hc->Drv.RH_API_Ptr->PortStatusGet(&p_hc->Drv,
                                             USBH_HUB_PORT_HUB_STATUS,
                                             p_status);
    RTOS_ERR_SET(*p_err, (ok == DEF_OK) ? RTOS_ERR_NONE : RTOS_ERR_IO);

    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  {
    CPU_INT08U retry;
    RTOS_ERR   ep_abort_err;
    CPU_INT32U std_req_timeout_ms;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
    CORE_EXIT_ATOMIC();

    LOG_VRB(("Request hub status at addr ", (u)p_hub_fnct->DevPtr->AddrCur));

    USBH_HUB_PortStatusBufLock(p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    retry = 3u;
    while (retry > 0u) {
      (void)USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                             USBH_DEV_REQ_GET_STATUS,
                             (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_DEV),
                             0u,
                             0u,
                             USBH_HUB_Ptr->PortStatusBufPtr,
                             USBH_HUB_BUF_LEN_PORT_STATUS,
                             USBH_HUB_BUF_LEN_PORT_STATUS,
                             std_req_timeout_ms,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        p_status->Status = MEM_VAL_GET_INT16U_LITTLE(&USBH_HUB_Ptr->PortStatusBufPtr[0u]);
        p_status->Chng = MEM_VAL_GET_INT16U_LITTLE(&USBH_HUB_Ptr->PortStatusBufPtr[2u]);
        break;
      } else {
        USBH_EP_Abort(p_hub_fnct->Handle,
                      USBH_EP_HANDLE_CTRL,
                      &ep_abort_err);

        KAL_Dly(USBH_HUB_DLY_RETRY);

        retry--;
      }
    }

    USBH_HUB_PortStatusBufUnlock();
  }
#endif
}

/****************************************************************************************************//**
 *                                           USBH_HUB_FeatureClr()
 *
 * @brief    Clear feature on given hub.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    feature     Feature to clear.
 *                           - USBH_HUB_FEATURE_SEL_C_LOCAL_POWER
 *                           - USBH_HUB_FEATURE_SEL_C_OVER_CUR
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) HUB class specific request CLEAR_FEATURE is defined in 'Universal Serial Bus
 *               specification, revision 2.0, section 11.24.2.1'.
 *******************************************************************************************************/
static void USBH_HUB_FeatureClr(USBH_HUB_FNCT *p_hub_fnct,
                                CPU_INT16U    feature,
                                RTOS_ERR      *p_err)
{
  CPU_BOOLEAN ok;
  USBH_HC     *p_hc;

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    LOG_VRB(("Root hub clr feat ", (X)feature));

    p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    ok = p_hc->Drv.RH_API_Ptr->PortReq(&p_hc->Drv,
                                       USBH_HUB_PORT_HUB_STATUS,
                                       USBH_DEV_REQ_CLR_FEATURE,
                                       feature);
    RTOS_ERR_SET(*p_err, (ok == DEF_OK) ? RTOS_ERR_NONE : RTOS_ERR_IO);

    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  {
    CPU_INT08U retry;
    RTOS_ERR   ep_abort_err;
    CPU_INT32U std_req_timeout_ms;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
    CORE_EXIT_ATOMIC();

    LOG_VRB(("Hub addr ", (u)p_hub_fnct->DevPtr->AddrCur, " clr feat ", (X)feature));

    retry = 3u;
    while (retry > 0u) {
      (void)USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                             USBH_DEV_REQ_CLR_FEATURE,
                             (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_DEV),
                             feature,
                             0u,
                             DEF_NULL,
                             0u,
                             0u,
                             std_req_timeout_ms,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        break;
      } else {
        USBH_EP_Abort(p_hub_fnct->Handle,
                      USBH_EP_HANDLE_CTRL,
                      &ep_abort_err);

        KAL_Dly(USBH_HUB_DLY_RETRY);

        retry--;
      }
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortStatusGet()
 *
 * @brief    Retrieve port status on given hub.
 *
 * @param    p_hub_fnct      Pointer to the hub function.
 *
 * @param    port_nbr        Port number.
 *
 * @param    p_port_status   Pointer to the port status structure that will receive the port status.
 *
 * @param    p_err           Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) HUB class specific request GET PORT STATUS is defined in 'Universal Serial Bus
 *               specification, revision 2.0, section 11.24.2.7'.
 *******************************************************************************************************/
static void USBH_HUB_PortStatusGet(USBH_HUB_FNCT   *p_hub_fnct,
                                   CPU_INT16U      port_nbr,
                                   USBH_HUB_STATUS *p_port_status,
                                   RTOS_ERR        *p_err)
{
  CPU_BOOLEAN ok;
  USBH_HC     *p_hc;

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    LOG_VRB(("Request root hub status port #", (u)port_nbr));

    p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    ok = p_hc->Drv.RH_API_Ptr->PortStatusGet(&p_hc->Drv,
                                             port_nbr,
                                             p_port_status);
    RTOS_ERR_SET(*p_err, (ok == DEF_OK) ? RTOS_ERR_NONE : RTOS_ERR_IO);

    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  {
    CPU_INT08U retry;
    RTOS_ERR   ep_abort_err;
    CPU_INT32U std_req_timeout_ms;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
    CORE_EXIT_ATOMIC();

    LOG_VRB(("Request hub addr ", (u)p_hub_fnct->DevPtr->AddrCur, " status port #", (u)port_nbr));

    USBH_HUB_PortStatusBufLock(p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    retry = 3u;
    while (retry > 0u) {
      (void)USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                             USBH_DEV_REQ_GET_STATUS,
                             (USBH_DEV_REQ_DIR_DEV_TO_HOST | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_OTHER),
                             0u,
                             port_nbr,
                             USBH_HUB_Ptr->PortStatusBufPtr,
                             USBH_HUB_BUF_LEN_PORT_STATUS,
                             USBH_HUB_BUF_LEN_PORT_STATUS,
                             std_req_timeout_ms,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        p_port_status->Status = MEM_VAL_GET_INT16U_LITTLE(&USBH_HUB_Ptr->PortStatusBufPtr[0u]);
        p_port_status->Chng = MEM_VAL_GET_INT16U_LITTLE(&USBH_HUB_Ptr->PortStatusBufPtr[2u]);
        break;
      } else {
        USBH_EP_Abort(p_hub_fnct->Handle,
                      USBH_EP_HANDLE_CTRL,
                      &ep_abort_err);

        KAL_Dly(USBH_HUB_DLY_RETRY);

        retry--;
      }
    }

    USBH_HUB_PortStatusBufUnlock();
  }
#endif
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortFeatureSet()
 *
 * @brief    Set feature on given port.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    feature     Feature to set.
 *                           - USBH_HUB_FEATURE_SEL_PORT_PWR
 *                           - USBH_HUB_FEATURE_SEL_PORT_CONN
 *                           - USBH_HUB_FEATURE_SEL_PORT_EN
 *                           - USBH_HUB_FEATURE_SEL_PORT_SUSPEND
 *                           - USBH_HUB_FEATURE_SEL_PORT_OVER_CUR
 *                           - USBH_HUB_FEATURE_SEL_PORT_RESET
 *                           - USBH_HUB_FEATURE_SEL_PORT_PWR
 *                           - USBH_HUB_FEATURE_SEL_PORT_LOW_SPD
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_CONN
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_EN
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_RESET
 *                           - USBH_HUB_FEATURE_SEL_PORT_TEST
 *                           - USBH_HUB_FEATURE_SEL_PORT_INDICATOR
 *
 * @param    port_nbr    Port number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) HUB class specific request SET PORT FEATURE is defined in 'Universal Serial Bus
 *               specification, revision 2.0, section 11.24.2.13'.
 *******************************************************************************************************/
static void USBH_HUB_PortFeatureSet(USBH_HUB_FNCT *p_hub_fnct,
                                    CPU_INT16U    feature,
                                    CPU_INT08U    port_nbr,
                                    RTOS_ERR      *p_err)
{
  CPU_BOOLEAN ok;
  USBH_HC     *p_hc;

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    LOG_VRB(("Root hub port set feat ", (X)feature, " port #", (u)port_nbr));

    p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    ok = p_hc->Drv.RH_API_Ptr->PortReq(&p_hc->Drv,
                                       port_nbr,
                                       USBH_DEV_REQ_SET_FEATURE,
                                       feature);
    if (ok == DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    }

    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  {
    CPU_INT08U retry;
    RTOS_ERR   ep_abort_err;
    CPU_INT32U std_req_timeout_ms;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
    CORE_EXIT_ATOMIC();

    LOG_VRB(("Hub addr ", (u)p_hub_fnct->DevPtr->AddrCur,
             " port set feat ", (X)feature, " port #", (u)port_nbr));

    retry = 3u;
    while (retry > 0u) {
      (void)USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                             USBH_DEV_REQ_SET_FEATURE,
                             (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_OTHER),
                             feature,
                             port_nbr,
                             DEF_NULL,
                             0u,
                             0u,
                             std_req_timeout_ms,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        break;
      } else {
        USBH_EP_Abort(p_hub_fnct->Handle,
                      USBH_EP_HANDLE_CTRL,
                      &ep_abort_err);

        KAL_Dly(USBH_HUB_DLY_RETRY);

        retry--;
      }
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                           USBH_HUB_PortFeatureClr()
 *
 * @brief    Clear feature on given port.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @param    feature     Feature to clear.
 *                           - USBH_HUB_FEATURE_SEL_PORT_PWR
 *                           - USBH_HUB_FEATURE_SEL_PORT_CONN
 *                           - USBH_HUB_FEATURE_SEL_PORT_EN
 *                           - USBH_HUB_FEATURE_SEL_PORT_SUSPEND
 *                           - USBH_HUB_FEATURE_SEL_PORT_OVER_CUR
 *                           - USBH_HUB_FEATURE_SEL_PORT_RESET
 *                           - USBH_HUB_FEATURE_SEL_PORT_PWR
 *                           - USBH_HUB_FEATURE_SEL_PORT_LOW_SPD
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_CONN
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_EN
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR
 *                           - USBH_HUB_FEATURE_SEL_C_PORT_RESET
 *                           - USBH_HUB_FEATURE_SEL_PORT_TEST
 *                           - USBH_HUB_FEATURE_SEL_PORT_INDICATOR
 *
 * @param    port_nbr    Port number.
 *
 * @param    p_err       Pointer to the variable that will receive the return error code from this function.
 *
 * @note     (1) HUB class specific request SET PORT FEATURE is defined in 'Universal Serial Bus
 *               specification, revision 2.0, section 11.24.2.13'.
 *******************************************************************************************************/
static void USBH_HUB_PortFeatureClr(USBH_HUB_FNCT *p_hub_fnct,
                                    CPU_INT16U    feature,
                                    CPU_INT16U    port_nbr,
                                    RTOS_ERR      *p_err)
{
  CPU_BOOLEAN ok;
  USBH_HC     *p_hc;

  if (p_hub_fnct->DevPtr == DEF_NULL) {
    LOG_VRB(("Root hub port clr feat ", (X)feature, " port #", (u)port_nbr));

    p_hc = USBH_HUB_HC_PtrGet(p_hub_fnct->Handle, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }

    ok = p_hc->Drv.RH_API_Ptr->PortReq(&p_hc->Drv,
                                       port_nbr,
                                       USBH_DEV_REQ_CLR_FEATURE,
                                       feature);
    RTOS_ERR_SET(*p_err, (ok == DEF_OK) ? RTOS_ERR_NONE : RTOS_ERR_IO);

    return;
  }

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
  {
    CPU_INT08U retry;
    RTOS_ERR   ep_abort_err;
    CPU_INT32U std_req_timeout_ms;
    CORE_DECLARE_IRQ_STATE;

    CORE_ENTER_ATOMIC();
    std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
    CORE_EXIT_ATOMIC();

    LOG_VRB(("Hub addr ", (u)p_hub_fnct->DevPtr->AddrCur,
             " port clr feat ", (X)feature,
             " port #", (u)port_nbr));

    retry = 3u;
    while (retry > 0u) {
      (void)USBH_EP_CtrlXfer(p_hub_fnct->Handle,
                             USBH_DEV_REQ_CLR_FEATURE,
                             (USBH_DEV_REQ_DIR_HOST_TO_DEV | USBH_DEV_REQ_TYPE_CLASS | USBH_DEV_REQ_RECIPIENT_OTHER),
                             feature,
                             port_nbr,
                             DEF_NULL,
                             0u,
                             0u,
                             std_req_timeout_ms,
                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
        break;
      } else {
        USBH_EP_Abort(p_hub_fnct->Handle,
                      USBH_EP_HANDLE_CTRL,
                      &ep_abort_err);

        KAL_Dly(USBH_HUB_DLY_RETRY);

        retry--;
      }
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                       USBH_HUB_PortStatusBufLock()
 *
 * @brief    Locks port status buffer.
 *
 * @param    p_err   Variable that will receive the return error code from this function.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_PortStatusBufLock(RTOS_ERR *p_err)
{
  CPU_INT32U std_req_timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  std_req_timeout_ms = USBH_Ptr->StdReqTimeout;
  CORE_EXIT_ATOMIC();

  KAL_LockAcquire(USBH_HUB_Ptr->PortStatusBufLock,
                  KAL_OPT_PEND_NONE,
                  std_req_timeout_ms,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Port status buf lock acquire -> ", RTOS_ERR_LOG_ARG_GET(*p_err)));
  }
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HUB_PortStatusBufUnlock()
 *
 * @brief    Unlocks port status buffer.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static void USBH_HUB_PortStatusBufUnlock(void)
{
  RTOS_ERR err_kal;

  KAL_LockRelease(USBH_HUB_Ptr->PortStatusBufLock, &err_kal);
  if (RTOS_ERR_CODE_GET(err_kal) != RTOS_ERR_NONE) {
    LOG_ERR(("Port status buf lock release -> ", RTOS_ERR_LOG_ARG_GET(err_kal)));
  }
}
#endif

/****************************************************************************************************//**
 *                                           USBH_HUB_HC_PtrGet()
 *
 * @brief    Gets pointer to Host Controller of given root hub.
 *
 * @param    root_hub_dev_handle     Device handle of root hub.
 *
 * @param    p_err                   Pointer to the variable that will receive the return error code from this function.
 *
 * @return   Pointer to the host controller.
 *******************************************************************************************************/
static USBH_HC *USBH_HUB_HC_PtrGet(USBH_DEV_HANDLE root_hub_dev_handle,
                                   RTOS_ERR        *p_err)
{
  USBH_HOST *p_host;
  USBH_HC   *p_hc;

  p_host = &USBH_Ptr->HostTblPtr[USBH_HANDLE_HOST_IX_GET(root_hub_dev_handle)];
  p_hc = USBH_HC_PtrGet(p_host,
                        USBH_HANDLE_HC_IX_GET(root_hub_dev_handle),
                        p_err);

  return (p_hc);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_RootIntEn()
 *
 * @brief    Enables or disables interruptions on given root hub.
 *
 * @param    root_hub_dev_handle     Device handle of root hub.
 *
 * @param    en                      Flag that indicates if interrupts must be enabled or disabled.
 *                                       - DEF_ENABLED
 *                                       - DEF_DISABLED
 *
 * @param    p_err                   Pointer to the variable that will receive the error code from this function.
 *******************************************************************************************************/
static void USBH_HUB_RootIntEn(USBH_DEV_HANDLE root_hub_dev_handle,
                               CPU_BOOLEAN     en,
                               RTOS_ERR        *p_err)
{
  CPU_BOOLEAN ok;
  USBH_HC     *p_hc;
  USBH_HC_DRV *p_hc_drv;

  p_hc = USBH_HUB_HC_PtrGet(root_hub_dev_handle, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  p_hc_drv = &p_hc->Drv;
  ok = p_hc_drv->RH_API_Ptr->IntEn(p_hc_drv, en);
  if (ok == DEF_FAIL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
    return;
  }
}

/****************************************************************************************************//**
 *                                           USBH_HUB_DevListAdd()
 *
 * @brief    Adds device to device list.
 *
 * @param    p_dev       Pointer to the device.
 *
 * @param    p_hub_fnct  Pointer to the hub function.
 *
 * @note     (1) Device list must be locked when calling this function.
 *
 * @note     (2) For more information on the device list, see Note (2) of file header.
 *******************************************************************************************************/
static void USBH_HUB_DevListAdd(USBH_DEV      *p_dev,
                                USBH_HUB_FNCT *p_hub_fnct)
{
  USBH_DEV      *p_dev_prev;
  USBH_DEV      *p_dev_next;
  USBH_HUB_FNCT *p_parent_hub_fnct;

  if ((p_hub_fnct->DevPtr == DEF_NULL)                          // Chk if first dev connected on root hub.
      && (p_hub_fnct->PortBusyCnt == 0u)) {
    p_hub_fnct->DevListHeadPtr = p_dev;
    p_hub_fnct->DevListTailPtr = p_dev;
    p_dev->HubListNextPtr = DEF_NULL;
    p_dev->HubListPrevPtr = DEF_NULL;
    p_hub_fnct->PortBusyCnt++;

    return;
  }

  if (p_hub_fnct->PortBusyCnt == 0u) {
    p_parent_hub_fnct = p_hub_fnct->DevPtr->HubFnctPtr;

    p_dev_prev = p_parent_hub_fnct->DevListTailPtr;
    p_dev_next = p_parent_hub_fnct->DevListTailPtr->HubListNextPtr;

    p_hub_fnct->DevListTailPtr = p_dev;                         // Update hub's tail dev ptr.
  } else {
    p_dev_prev = p_hub_fnct->DevListHeadPtr->HubListPrevPtr;
    p_dev_next = p_hub_fnct->DevListHeadPtr;
  }

  p_dev->HubListNextPtr = p_dev_next;                           // Update new dev next and prev ptr.
  p_dev->HubListPrevPtr = p_dev_prev;

  if (p_dev_next != DEF_NULL) {                                 // Update prev and next dev's ptrs.
    p_dev_next->HubListPrevPtr = p_dev;
  }

  if (p_dev_prev != DEF_NULL) {
    p_dev_prev->HubListNextPtr = p_dev;
  }

  p_hub_fnct->DevListHeadPtr = p_dev;                           // Update hub's head and tail dev ptr.

  p_hub_fnct->PortBusyCnt++;
}

/****************************************************************************************************//**
 *                                           USBH_HUB_DevListRemove()
 *
 * @brief    Removes device from device list.
 *
 * @param    p_dev       Pointer to the device.
 *
 * @param    p_hub_fnct  Pointer to the hub function. Can be null if hub's head and tail pointer must not
 *                       be updated.
 *
 * @note     (1) Device list must be locked when calling this function.
 *
 * @note     (2) For more information on the device list, see Note (2) of file header.
 *******************************************************************************************************/
static void USBH_HUB_DevListRemove(USBH_DEV      *p_dev,
                                   USBH_HUB_FNCT *p_hub_fnct)
{
  USBH_DEV *p_dev_prev;
  USBH_DEV *p_dev_next;

  p_dev_next = p_dev->HubListNextPtr;
  p_dev_prev = p_dev->HubListPrevPtr;

  if (p_hub_fnct != DEF_NULL) {
    if (p_hub_fnct->PortBusyCnt == 1u) {                        // Update hub's head and tail dev ptr.
      p_hub_fnct->DevListHeadPtr = DEF_NULL;
      p_hub_fnct->DevListTailPtr = DEF_NULL;
    } else if (p_hub_fnct->DevListTailPtr == p_dev) {
      p_hub_fnct->DevListTailPtr = p_dev_prev;
    } else if (p_hub_fnct->DevListHeadPtr == p_dev) {
      p_hub_fnct->DevListHeadPtr = p_dev_next;
    }

    p_hub_fnct->PortBusyCnt--;
  }

  if (p_dev_next != DEF_NULL) {                                 // Update prev and next dev's ptrs.
    p_dev_next->HubListPrevPtr = p_dev_prev;
  }

  if (p_dev_prev != DEF_NULL) {
    p_dev_prev->HubListNextPtr = p_dev_next;
  }
}

/****************************************************************************************************//**
 *                                       USBH_HUB_FnctAllocCallback()
 *
 * @brief    Function called when a new hub function is allocated.
 *
 * @param    p_pool  Pointer to the memory pool.
 *
 * @param    p_seg   Pointer to the memory pool's memory segment.
 *
 * @param    p_blk   Pointer to the memory block.
 *
 * @param    p_arg   Pointer to the argument passed at pool creation. Unused in this case.
 *
 * @return   DEF_OK,   if p_blk extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/

#if (USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED)
static CPU_BOOLEAN USBH_HUB_FnctAllocCallback(MEM_DYN_POOL *p_pool,
                                              MEM_SEG      *p_seg,
                                              void         *p_blk,
                                              void         *p_arg)
{
  RTOS_ERR      err_lib;
  USBH_HUB_FNCT *p_hub_fnct;

  (void)&p_pool;
  (void)&p_seg;
  (void)&p_arg;

  p_hub_fnct = (USBH_HUB_FNCT *)p_blk;

  //                                                               Alloc buf used to retrieve port status chng.
  p_hub_fnct->StatusBufPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - HUB Status change buf",
                                                          USBH_InitCfg.MemSegBufPtr,
                                                          USBH_HUB_STATUS_CHNG_BUF_LEN,
                                                          USBH_InitCfg.BufAlignOctets,
                                                          DEF_NULL,
                                                          &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating hub status change buf -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    return (DEF_FAIL);
  }

  return (DEF_OK);
}
#endif

/****************************************************************************************************//**
 *                                       USBH_HUB_EventAllocCallback()
 *
 * @brief    Function called when a new hub event is allocated.
 *
 * @param    p_pool  Pointer to the memory pool.
 *
 * @param    p_seg   Pointer to the memory pool's memory segment.
 *
 * @param    p_blk   Pointer to the memory block.
 *
 * @param    p_arg   Pointer to the argument passed at pool creation. Unused in this case.
 *
 * @return   DEF_OK,   if p_blk extra allocation(s) are successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_HUB_EventAllocCallback(MEM_DYN_POOL *p_pool,
                                               MEM_SEG      *p_seg,
                                               void         *p_blk,
                                               void         *p_arg)
{
  RTOS_ERR       err_lib;
  CPU_SIZE_T     size;
  USBH_HUB_EVENT *p_hub_event;

  (void)&p_pool;
  (void)&p_arg;

  p_hub_event = (USBH_HUB_EVENT *)p_blk;

  size = DEF_MAX(sizeof(USBH_HUB_EVENT_PORT_REQ_ARG), sizeof(USBH_HUB_EVENT_DEV_CONFIG_SET_ARG));
    #if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
  size = DEF_MAX(sizeof(USBH_HUB_EVENT_IF_ALT_SET_ARG), size);
    #endif

  //                                                               Alloc spaceholder used to keep arg of event.
  p_hub_event->ArgPtr = Mem_SegAlloc("USBH - HUB Event Arg Spaceholder",
                                     p_seg,
                                     size,
                                     &err_lib);
  if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
    LOG_ERR(("Allocating hub event arg spaceholder -> ", RTOS_ERR_LOG_ARG_GET(err_lib)));
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_HUB_EventQPend()
 *
 * @brief    Pends on the HUB event function Q.
 *
 * @param    p_err   Pointer to the variable that will receive the error code from this function.
 *
 * @return   Pointer to the hub event, if successful.
 *           Null pointer,             otherwise.
 *******************************************************************************************************/
static USBH_HUB_EVENT *USBH_HUB_EventQPend(RTOS_ERR *p_err)
{
  USBH_HUB_EVENT *p_hub_event;
  CORE_DECLARE_IRQ_STATE;

  KAL_SemPend(USBH_HUB_Ptr->HubEventQSemHandle,
              KAL_OPT_POST_NONE,
              0u,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  CORE_ENTER_ATOMIC();
  p_hub_event = USBH_HUB_Ptr->HubEventQHead;
  USBH_HUB_Ptr->HubEventQHead = p_hub_event->NextPtr;
  CORE_EXIT_ATOMIC();

  return (p_hub_event);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL))
