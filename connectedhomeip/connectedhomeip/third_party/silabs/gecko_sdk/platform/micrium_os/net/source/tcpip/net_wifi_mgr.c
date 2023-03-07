/***************************************************************************//**
 * @file
 * @brief Network Wireless Manager
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>

#ifdef  NET_IF_WIFI_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_if_wifi.h>

#include  "net_wifi_mgr_priv.h"

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <net/source/tcpip/net_if_priv.h>
#include  <net/source/tcpip/net_if_wifi_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                         (NET)
#define  RTOS_MODULE_CUR                      RTOS_CFG_MODULE_NET

#define  NET_WIFI_MGR_LOCK_OBJ_NAME          "WiFi Mgr Lock"
#define  NET_WIFI_MGR_RESP_OBJ_NAME          "WiFi Mgr MGMT Response"

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetWiFiMgr_Init(NET_IF   *p_if,
                            RTOS_ERR *p_err);

static void NetWiFiMgr_Start(NET_IF   *p_if,
                             RTOS_ERR *p_err);

static void NetWiFiMgr_Stop(NET_IF   *p_if,
                            RTOS_ERR *p_err);

static CPU_INT16U NetWiFiMgr_AP_Scan(NET_IF                 *p_if,
                                     NET_IF_WIFI_AP         *p_buf_scan,
                                     CPU_INT16U             scan_len_max,
                                     const NET_IF_WIFI_SSID *p_ssid,
                                     NET_IF_WIFI_CH         ch,
                                     RTOS_ERR               *p_err);

static void NetWiFiMgr_AP_Join(NET_IF                   *p_if,
                               const NET_IF_WIFI_AP_CFG *p_ap_cfg,
                               RTOS_ERR                 *p_err);

static void NetWiFiMgr_AP_Leave(NET_IF   *p_if,
                                RTOS_ERR *p_err);

static void NetWiFiMgr_IO_Ctrl(NET_IF     *p_if,
                               CPU_INT08U opt,
                               void       *p_data,
                               RTOS_ERR   *p_err);

static CPU_INT32U NetWiFiMgr_Mgmt(NET_IF          *p_if,
                                  NET_IF_WIFI_CMD cmd,
                                  CPU_INT08U      *p_buf_cmd,
                                  CPU_INT16U      buf_cmd_len,
                                  CPU_INT08U      *p_buf_rtn,
                                  CPU_INT16U      buf_rtn_len_max,
                                  RTOS_ERR        *p_err);

static CPU_INT32U NetWiFiMgr_MgmtHandler(NET_IF          *p_if,
                                         NET_IF_WIFI_CMD cmd,
                                         CPU_INT08U      *p_buf_cmd,
                                         CPU_INT16U      buf_cmd_len,
                                         CPU_INT08U      *p_buf_rtn,
                                         CPU_INT16U      buf_rtn_len_max,
                                         RTOS_ERR        *p_err);

static void NetWiFiMgr_Signal(NET_IF   *p_if,
                              NET_BUF  *p_buf,
                              RTOS_ERR *p_err);

static void NetWiFiMgr_LockAcquire(KAL_LOCK_HANDLE lock);

static void NetWiFiMgr_LockRelease(KAL_LOCK_HANDLE lock);

static void NetWiFiMgr_AP_Create(NET_IF                   *p_if,
                                 const NET_IF_WIFI_AP_CFG *p_ap_cfg,
                                 RTOS_ERR                 *p_err);

static CPU_INT16U NetWiFiMgr_AP_GetPeerInfo(NET_IF                 *p_if,
                                            const NET_IF_WIFI_PEER *p_buf_peer,
                                            CPU_INT16U             peer_info_len_max,
                                            RTOS_ERR               *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/
//                                                                 WiFi Mgr API fnct ptrs :
const NET_WIFI_MGR_API NetWiFiMgr_API_Generic = {
  &NetWiFiMgr_Init,                                                             // Init/add
  &NetWiFiMgr_Start,                                                            // Start
  &NetWiFiMgr_Stop,                                                             // Stop
  &NetWiFiMgr_AP_Scan,                                                          // Scan
  &NetWiFiMgr_AP_Join,                                                          // Join
  &NetWiFiMgr_AP_Leave,                                                         // Leave
  &NetWiFiMgr_IO_Ctrl,                                                          // IO Ctrl
  &NetWiFiMgr_Mgmt,                                                             // Mgmt
  &NetWiFiMgr_Signal,                                                           // Signal
  &NetWiFiMgr_AP_Create,                                                        // Create
  &NetWiFiMgr_AP_GetPeerInfo,                                                   // GetClientInfo
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetWiFiMgr_Init()
 *
 * @brief    (1) Initialize wireless manager layer.
 *               - (a) Allocate memory for manager object
 *               - (b) Initalize wireless  manager lock
 *               - (c) Initalize wireless  manager Response Signal
 *
 * @param    p_if    Pointer to interface to initialize Wifi manager.
 *
 *
 * Argument(s) : p_if    Pointer to interface to initialize Wifi manager.
 *
 *               p_err   Pointer to variable  that will receive the return error code from this function.
 *
 * Return(s)   : none
 *
 * Note(s)     : (2) Wireless manager initialization occurs only when the interface is added.
 *                       See 'net_if_wifi.c  NetIF_WiFi_IF_Add()'.
 *******************************************************************************************************/
static void NetWiFiMgr_Init(NET_IF   *p_if,
                            RTOS_ERR *p_err)
{
  NET_DEV_CFG_WIFI  *p_cfg;
  NET_WIFI_MGR_DATA *p_mgr_data;
  CPU_SIZE_T        size;

  //                                                               ----------- ALLOCATE WIFI MGR DATA AREA ------------
  size = sizeof(NET_WIFI_MGR_DATA);
  p_if->Ext_Data = Mem_SegAlloc("Net WiFi Mgr Data",
                                DEF_NULL,
                                size,
                                p_err);

  if (p_if->Ext_Data == DEF_NULL) {
    goto exit;
  }

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;

  //                                                               ---------------- INIT WIFI MGR LOCK ----------------
  p_mgr_data->MgrLock = KAL_LockCreate(NET_WIFI_MGR_LOCK_OBJ_NAME, DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_cfg = (NET_DEV_CFG_WIFI *)p_if->Dev_Cfg;

  //                                                               --------------- INIT WIFI MGR SIGNAL ---------------
  p_mgr_data->MgmtSignalResp = KAL_QCreate(NET_WIFI_MGR_RESP_OBJ_NAME,
                                           p_cfg->RxBufLargeNbr,
                                           DEF_NULL,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_delete_lock;
  }

  p_mgr_data->DevStarted = DEF_NO;
  p_mgr_data->AP_Joined = DEF_NO;
  p_mgr_data->AP_Created = DEF_NO;

  goto exit;

exit_delete_lock:
  KAL_LockDel(p_mgr_data->MgrLock);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_Start()
 *
 * @brief    Start wireless manager of the interface.
 *
 * @param    p_if    Pointer to interface to Start wireless manager.
 *
 * @param    p_err   Pointer to variable  that will receive the return error code from this function.
 *
 * @note     (1) Wireless manager start occurs each time the interface is started.
 *               See 'net_if_wifi.c  NetIF_WiFi_IF_Start()'.
 *******************************************************************************************************/
static void NetWiFiMgr_Start(NET_IF   *p_if,
                             RTOS_ERR *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;

  PP_UNUSED_PARAM(p_err);

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  p_mgr_data->DevStarted = DEF_YES;
}

/****************************************************************************************************//**
 *                                               NetWiFiMgr_Stop()
 *
 * @brief    Shutdown wireless manager of the interface.
 *
 * @param    p_if    Pointer to interface to Start Wifi manager.
 *
 * @param    p_err   Pointer to variable  that will receive the return error code from this function.
 *
 * @note     (1) Wireless manager stop occurs each time the interface is stopped.
 *               See 'net_if_wifi.c  NetIF_WiFi_IF_Stop()'.
 *******************************************************************************************************/
static void NetWiFiMgr_Stop(NET_IF   *p_if,
                            RTOS_ERR *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;

  PP_UNUSED_PARAM(p_err);

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  p_mgr_data->DevStarted = DEF_NO;
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_AP_Scan()
 *
 * @brief    - (1) Scan for available wireless network by the interface:
 *                 - (a) Release network          lock
 *                 - (b) Acquire wireless manager lock
 *                 - (c) Acquire network          lock
 *                 - (d) Send scan command and get result
 *                 - (e) Release network          lock
 *                 - (f) Release wireless manager lock
 *
 * @param    p_if            Pointer to interface to Scan with.
 *
 * @param    p_buf_scan      Pointer to table that will receive the return network found.
 *
 * @param    scan_len_max    Length of the scan buffer (i.e. Number of network that can be found).
 *
 * @param    p_ssid          Pointer to variable that contains the SSID to scan for.
 *
 * @param    ch              The wireless channel to scan.
 *                               - NET_IF_WIFI_CH_ALL         Scan Wireless network for all channel.
 *                               - NET_IF_WIFI_CH_1           Scan Wireless network on channel 1.
 *                               - NET_IF_WIFI_CH_2           Scan Wireless network on channel 2.
 *                               - NET_IF_WIFI_CH_3           Scan Wireless network on channel 3.
 *                               - NET_IF_WIFI_CH_4           Scan Wireless network on channel 4.
 *                               - NET_IF_WIFI_CH_5           Scan Wireless network on channel 5.
 *                               - NET_IF_WIFI_CH_6           Scan Wireless network on channel 6.
 *                               - NET_IF_WIFI_CH_7           Scan Wireless network on channel 7.
 *                               - NET_IF_WIFI_CH_8           Scan Wireless network on channel 8.
 *                               - NET_IF_WIFI_CH_9           Scan Wireless network on channel 9.
 *                               - NET_IF_WIFI_CH_10          Scan Wireless network on channel 10.
 *                               - NET_IF_WIFI_CH_11          Scan Wireless network on channel 11.
 *                               - NET_IF_WIFI_CH_12          Scan Wireless network on channel 12.
 *                               - NET_IF_WIFI_CH_13          Scan Wireless network on channel 13.
 *                               - NET_IF_WIFI_CH_14          Scan Wireless network on channel 14.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 *
 * @return   Number of wireless network found, if any.
 *           0, otherwise.
 *******************************************************************************************************/
static CPU_INT16U NetWiFiMgr_AP_Scan(NET_IF                 *p_if,
                                     NET_IF_WIFI_AP         *p_buf_scan,
                                     CPU_INT16U             scan_len_max,
                                     const NET_IF_WIFI_SSID *p_ssid,
                                     NET_IF_WIFI_CH         ch,
                                     RTOS_ERR               *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;
  NET_IF_WIFI_SCAN  scan;
  CPU_INT08U        *p_buf_cmd;
  CPU_INT08U        *p_buf_rtn;
  CPU_INT16U        len = 0u;
  CPU_INT32U        rtn_len;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  if (p_mgr_data->DevStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockAcquire(p_mgr_data->MgrLock);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_AP_Scan);

  //                                                               ----------- SEND SCAN CMD AND GET RESULT -----------
  Mem_Clr(&scan.SSID, sizeof(scan.SSID));
  if (p_ssid != DEF_NULL) {
    Str_Copy_N((CPU_CHAR *)&scan.SSID,
               (const CPU_CHAR *) p_ssid,
               sizeof(scan.SSID));
  }

  scan.Ch = ch;
  len = scan_len_max * sizeof(NET_IF_WIFI_AP);
  p_buf_cmd = (CPU_INT08U *)&scan;
  p_buf_rtn = (CPU_INT08U *) p_buf_scan;
  rtn_len = NetWiFiMgr_MgmtHandler(p_if,
                                   NET_IF_WIFI_CMD_SCAN,
                                   p_buf_cmd,
                                   sizeof(scan),
                                   p_buf_rtn,
                                   len,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    len = 0u;
    goto exit_release;
  }

  len = rtn_len / sizeof(NET_IF_WIFI_AP);

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return (len);
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_AP_Join()
 *
 * @brief    Join wireless network:
 *               - (a) Release network          lock
 *               - (b) Acquire wireless manager lock
 *               - (c) Acquire network          lock
 *               - (d) Send 'Join' command and get result
 *               - (e) Release network          lock
 *               - (f) Release wireless manager lock
 *
 * @param    p_if    Pointer to interface to join with.
 *
 * @param    p_join  Pointer to variable that contains the wireless network to join.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetWiFiMgr_AP_Join(NET_IF                   *p_if,
                               const NET_IF_WIFI_AP_CFG *p_ap_cfg,
                               RTOS_ERR                 *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;
  CPU_INT08U        *p_buf_cmd;
  CPU_INT16U        buf_data_len;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  if (p_mgr_data->DevStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockAcquire(p_mgr_data->MgrLock);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_AP_Join);

  //                                                               ----------- SEND JOIN CMD AND GET RESULT -----------
  buf_data_len = sizeof(NET_IF_WIFI_AP_CFG);
  p_buf_cmd = (CPU_INT08U *)p_ap_cfg;
  (void)NetWiFiMgr_MgmtHandler(p_if,
                               NET_IF_WIFI_CMD_JOIN,
                               p_buf_cmd,
                               buf_data_len,
                               0,
                               0,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_data->AP_Joined = DEF_YES;

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_AP_Leave()
 *
 * @brief    Leave wireless network.
 *               - (a) Release network          lock
 *               - (b) Acquire wireless manager lock
 *               - (c) Acquire network          lock
 *               - (d) Send 'Leave' command and get result
 *               - (e) Release network          lock
 *               - (f) Release wireless manager lock
 *
 * @param    p_if    Pointer to interface to leave wireless network.
 *
 *
 * Argument(s) : p_if            Pointer to interface to leave wireless network.
 *
 *               p_err           Pointer to variable  that will receive the return error code from this function.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void NetWiFiMgr_AP_Leave(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  if (p_mgr_data->DevStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockAcquire(p_mgr_data->MgrLock);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_AP_Leave);

  //                                                               ---------- SEND LEAVE CMD AND GET RESULT -----------
  (void)NetWiFiMgr_MgmtHandler(p_if,
                               NET_IF_WIFI_CMD_LEAVE,
                               0,
                               0,
                               0,
                               0,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_if->Link = NET_IF_LINK_DOWN;
  p_mgr_data->AP_Joined = DEF_NO;
  p_mgr_data->AP_Created = DEF_NO;

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_IO_Ctrl()
 *
 * @brief    Handle a wireless interface's (I/O) control(s).
 *
 * @param    p_if    Pointer to a Wireless network interface.
 *
 * @param    opt     Desired I/O control option code to perform; additional control options may be
 *                   defined by the device driver :
 *                   NET_IF_IO_CTRL_LINK_STATE_GET           Get    Wireless interface's link state,
 *                                                           'UP' or 'DOWN'.
 *                   NET_IF_IO_CTRL_LINK_STATE_GET_INFO      Get    Wireless interface's detailed
 *                                                           link state info.
 *                   NET_IF_IO_CTRL_LINK_STATE_UPDATE        Update Wireless interface's link state.
 *
 * @param    p_data  Pointer to variable that will receive possible I/O control data (see Note #1).
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) 'p_data' MUST point to a variable (or memory) that is sufficiently sized AND aligned
 *               to receive any return data.
 *******************************************************************************************************/
static void NetWiFiMgr_IO_Ctrl(NET_IF     *p_if,
                               CPU_INT08U opt,
                               void       *p_data,
                               RTOS_ERR   *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;
  NET_IF_LINK_STATE *p_link_state;

  RTOS_ASSERT_DBG_ERR_SET((p_data != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_link_state = (NET_IF_LINK_STATE *)p_data;
  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;

  if (p_mgr_data->DevStarted != DEF_YES) {
    *p_link_state = NET_IF_LINK_DOWN;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------

  KAL_LockAcquire(p_mgr_data->MgrLock, KAL_OPT_PEND_NON_BLOCKING, KAL_TIMEOUT_INFINITE, p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_WOULD_BLOCK:
      Net_GlobalLockAcquire((void *)NetWiFiMgr_IO_Ctrl);
      goto exit;

    default:
      RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_IO_Ctrl);

  //                                                               ------------- SEND CMD AND GET RESULT --------------
  (void)NetWiFiMgr_MgmtHandler(p_if,
                               opt,
                               (CPU_INT08U *)p_data,
                               0,
                               (CPU_INT08U *)p_data,
                               0,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               NetWiFiMgr_Mgmt()
 *
 * @brief    (1) Send driver management command:
 *               - (a) Acquire wireless manager lock
 *               - (b) Send management command and get result
 *               - (c) Release wireless manager lock
 *               - (d) Execute & process management command
 *
 * @param    p_if                Pointer to interface to manage wireless network.
 *
 * @param    cmd                 Management command to send.
 *                               See Note #2a.
 *
 * @param    p_buf_cmd           Pointer to variable that contains the data to send.
 *
 * @param    buf_cmd_len         Length of the command buffer.
 *
 * @param    p_buf_rtn           Pointer to variable that will receive the data.
 *
 * @param    buf_rtn_len_max     Length of the return buffer.
 *
 * @param    p_err               Pointer to variable  that will receive the return error code from this function.
 *
 * @note     (2) The driver can define and implement its own management commands which need a response by
 *               calling the wireless manager api (p_mgr_api->Mgmt()) to send the management command and to
 *               receive the response.
 *               - (a) Driver management command code '100' series reserved for driver.
 *
 * @note     (3) Prior calling this function, the network lock must be acquired.
 *******************************************************************************************************/
static CPU_INT32U NetWiFiMgr_Mgmt(NET_IF          *p_if,
                                  NET_IF_WIFI_CMD cmd,
                                  CPU_INT08U      *p_buf_cmd,
                                  CPU_INT16U      buf_cmd_len,
                                  CPU_INT08U      *p_buf_rtn,
                                  CPU_INT16U      buf_rtn_len_max,
                                  RTOS_ERR        *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data = DEF_NULL;
  CPU_INT32U        rtn_len = 0u;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  if (p_mgr_data->DevStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockAcquire(p_mgr_data->MgrLock);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_Mgmt);

  //                                                               ----------- SEND MGMT CMD AND GET RESULT -----------
  rtn_len = NetWiFiMgr_MgmtHandler(p_if,
                                   cmd,
                                   p_buf_cmd,
                                   buf_cmd_len,
                                   p_buf_rtn,
                                   buf_rtn_len_max,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    rtn_len = 0u;
    goto exit_release;
  }

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return (rtn_len);
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_MgmtHandler()
 *
 * @brief    (1) Send mgmt command and get result:
 *               - (a) Ask driver to transmit management command
 *               - (b) Release network lock
 *               - (b) Wait response
 *               - (c) Ask driver to decode the received management response
 *               - (d) Free received buffer
 *               - (e) Acquire network lock
 *
 * @param    p_if                Pointer to interface to leave wireless network.
 *
 * @param    cmd                 Management command to send.
 *                               NET_IF_WIFI_CMD_SCAN                    Scan  for available Wireless network.
 *                               NET_IF_WIFI_CMD_JOIN                    Join  a             Wireless network.
 *                               NET_IF_WIFI_CMD_LEAVE                   Leave the           Wireless network.
 *                               NET_IF_IO_CTRL_LINK_STATE_GET           Get    Wireless interface's link state,
 *                                                                       'UP' or 'DOWN'.
 *                               NET_IF_IO_CTRL_LINK_STATE_GET_INFO      Get    Wireless interface's detailed
 *
 * @param    p_buf_cmd           Pointer to variable that will receive the data.
 *                               link state info.
 *                               NET_IF_IO_CTRL_LINK_STATE_UPDATE        Update Wireless interface's link state.
 *                               See 'NetWiFiMgr_Mgmt()' Note 2a.
 *
 * @param    buf_cmd_len         Length of the receive buffer.
 *
 * @param    p_buf_rtn           Pointer to variable that contains the data to send.
 *
 * @param    buf_rtn_len_max     Length of the data to send.
 *
 * @param    p_err               Pointer to variable  that will receive the return error code from this function.
 *******************************************************************************************************/
static CPU_INT32U NetWiFiMgr_MgmtHandler(NET_IF          *p_if,
                                         NET_IF_WIFI_CMD cmd,
                                         CPU_INT08U      *p_buf_cmd,
                                         CPU_INT16U      buf_cmd_len,
                                         CPU_INT08U      *p_buf_rtn,
                                         CPU_INT16U      buf_rtn_len_max,
                                         RTOS_ERR        *p_err)
{
  NET_DEV_API_WIFI  *p_dev_api;
  NET_WIFI_MGR_DATA *p_mgr_data;
  NET_WIFI_MGR_CTX  ctx;
  NET_BUF           *p_buf;
  NET_BUF_HDR       *p_hdr;
  CPU_INT32U        rtn_len;
  CPU_BOOLEAN       done;

  p_dev_api = (NET_DEV_API_WIFI *)p_if->Dev_API;
  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  done = DEF_NO;
  rtn_len = 0u;

  while (done != DEF_YES) {
    //                                                             ------------- PREPARE & SEND MGMT CMD --------------
    rtn_len = p_dev_api->MgmtExecuteCmd(p_if,
                                        cmd,
                                        &ctx,
                                        p_buf_cmd,
                                        buf_cmd_len,
                                        p_buf_rtn,
                                        buf_rtn_len_max,
                                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    if (ctx.WaitResp == DEF_YES) {                              // If the cmd requires a resp.
                                                                // ----------------- RELEASE NET LOCK -----------------
      Net_GlobalLockRelease();                                  // Require to rx pkt & mgmt frame.

      //                                                           -------------------- WAIT RESP ---------------------
      p_buf = (NET_BUF *)KAL_QPend(p_mgr_data->MgmtSignalResp,
                                   KAL_OPT_PEND_NONE,
                                   ctx.WaitRespTimeout_ms,
                                   p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail_acquire_lock;
      }

      p_hdr = &p_buf->Hdr;

      //                                                           ----------------- RX & DECODE RESP -----------------
      rtn_len = p_dev_api->MgmtProcessResp(p_if,
                                           cmd,
                                           &ctx,
                                           p_buf->DataPtr,
                                           p_hdr->DataLen,
                                           p_buf_rtn,
                                           buf_rtn_len_max,
                                           p_err);

      //                                                           ------------------ FREE BUF RX'D -------------------
      NetBuf_Free(p_buf);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_acquire_lock;
      }

      //                                                           ----------------- ACQUIRE NET LOCK -----------------
      Net_GlobalLockAcquire((void *)NetWiFiMgr_MgmtHandler);
    }

    if (ctx.MgmtCompleted == DEF_YES) {
      done = DEF_YES;
    }
  }

  goto exit;

exit_fail_acquire_lock:
  rtn_len = 0u;

exit_acquire_lock:
  Net_GlobalLockAcquire((void *)NetWiFiMgr_MgmtHandler);

exit:
  return (rtn_len);
}

/****************************************************************************************************//**
 *                                           NetWiFiMgr_Signal()
 *
 * @brief    Signal reception of a management response.
 *
 * @param    p_if    Pointer to interface to leave wireless network.
 *
 * @param    p_buf   Pointer to net buffer that contains the management buffer received.
 *
 * @param    p_err   Pointer to variable   that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetWiFiMgr_Signal(NET_IF   *p_if,
                              NET_BUF  *p_buf,
                              RTOS_ERR *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;

  KAL_QPost(p_mgr_data->MgmtSignalResp,
            p_buf,
            KAL_OPT_POST_NONE,
            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NetBuf_Free(p_buf);
    RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_CODE_GET(*p_err),; );
  }
}

/****************************************************************************************************//**
 *                                           NetOS_WiFiMgr_Lock()
 *
 * @brief    Lock wireless manager.
 *
 * @param    lock    Lock Handle.
 *
 * @note     (1)
 *               - (a) Wireless manager access MUST be acquired--i.e. MUST wait for access; do NOT timeout.
 *                   - (1) Failure to acquire manager access will prevent network task(s)/operation(s)
 *                         from functioning.
 *               - (b) Wireless manager access MUST be acquired exclusively by only a single task at any one
 *                     time.
 *******************************************************************************************************/
#ifdef  NET_IF_WIFI_MODULE_EN
static void NetWiFiMgr_LockAcquire(KAL_LOCK_HANDLE lock)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_LockAcquire(lock, KAL_OPT_PEND_NONE, KAL_TIMEOUT_INFINITE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/****************************************************************************************************//**
 *                                           NetOS_WiFiMgr_Unlock()
 *
 * @brief    Unlock wireless manager.
 *
 * @param    lock    Lock Handle.
 *
 * @note     (1) Wireless manager MUST be released--i.e. MUST unlock access without failure.
 *               - (a) Failure to release Wireless manager access will prevent task(s)/operation(s) from
 *                   functioning.  Thus Wireless manager access is assumed to be successfully released
 *                   since NO Micrium OS Kernel error handling could be performed to counteract failure.
 *******************************************************************************************************/
#ifdef  NET_IF_WIFI_MODULE_EN
static void NetWiFiMgr_LockRelease(KAL_LOCK_HANDLE lock)
{
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  KAL_LockRelease(lock, &local_err);                             // Release exclusive network access.
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}
#endif

/****************************************************************************************************//**
 *                                           NetWiFiMgr_AP_Create()
 *
 * @brief    Create wireless network:
 *               - (a) Release network          lock
 *               - (b) Acquire wireless manager lock
 *               - (c) Acquire network          lock
 *               - (d) Send 'Create' command and get result
 *               - (e) Release network          lock
 *               - (f) Release wireless manager lock
 *
 * @param    p_if    Pointer to interface to create with.
 *
 * @param    p_join  Pointer to variable that contains the wireless network to join.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetWiFiMgr_AP_Create(NET_IF                   *p_if,
                                 const NET_IF_WIFI_AP_CFG *p_ap_cfg,
                                 RTOS_ERR                 *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;
  CPU_INT08U        *p_buf_cmd;
  CPU_INT16U        buf_data_len;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  if (p_mgr_data->DevStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockAcquire(p_mgr_data->MgrLock);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_AP_Create);

  //                                                               ---------- SEND CREATE CMD AND GET RESULT ----------
  buf_data_len = sizeof(NET_IF_WIFI_AP_CFG);
  p_buf_cmd = (CPU_INT08U *)p_ap_cfg;
  (void)NetWiFiMgr_MgmtHandler(p_if,
                               NET_IF_WIFI_CMD_CREATE,
                               p_buf_cmd,
                               buf_data_len,
                               0,
                               0,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  p_mgr_data->AP_Created = DEF_YES;

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetWiFiMgr_AP_GetPeerInfo()
 *
 * @brief    (1) Get the info of the peer connected to the access point created by the interface:
 *              - (a) Release network          lock
 *              - (b) Acquire wireless manager lock
 *              - (c) Acquire network          lock
 *              - (d) Send get peer info command and get result
 *              - (e) Release network          lock
 *              - (f) Release wireless manager lock
 *
 * @param    p_if            Pointer to interface to Scan with.
 *
 * @param    p_buf_peer      Pointer to table that will receive the peer info found.
 *
 * @param    peer_len_max    Length of the scan buffer (i.e. Number of network that can be found).s
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of peer found, if any.
 *           0, otherwise.
 *******************************************************************************************************/
static CPU_INT16U NetWiFiMgr_AP_GetPeerInfo(NET_IF                 *p_if,
                                            const NET_IF_WIFI_PEER *p_buf_peer,
                                            CPU_INT16U             peer_len_max,
                                            RTOS_ERR               *p_err)
{
  NET_WIFI_MGR_DATA *p_mgr_data;
  CPU_INT08U        *p_buf_rtn;
  CPU_INT16U        len = 0u;
  CPU_INT32U        rtn_len;

  p_mgr_data = (NET_WIFI_MGR_DATA *)p_if->Ext_Data;
  if (p_mgr_data->DevStarted != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    goto exit;
  }
  if (p_mgr_data->AP_Created == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    goto exit;
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  //                                                               -------------- ACQUIRE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockAcquire(p_mgr_data->MgrLock);

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetWiFiMgr_AP_Create);

  //                                                               ------ SEND GET PEER INFO CMD AND GET RESULT -------

  len = peer_len_max * sizeof(NET_IF_WIFI_PEER);
  p_buf_rtn = (CPU_INT08U *) p_buf_peer;
  rtn_len = NetWiFiMgr_MgmtHandler(p_if,
                                   NET_IF_WIFI_CMD_GET_PEER_INFO,
                                   DEF_NULL,
                                   0,
                                   p_buf_rtn,
                                   len,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    len = 0u;
    goto exit_release;
  }

  len = rtn_len / sizeof(NET_IF_WIFI_PEER);

exit_release:
  //                                                               -------------- RELEASE WIFI MGR LOCK ---------------
  NetWiFiMgr_LockRelease(p_mgr_data->MgrLock);

exit:
  return (len);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_WIFI_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
