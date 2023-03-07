/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*****************************************************************************
**
**  Name:           wiced_bt_mce_api.h
**
**  Description:    This is the public interface file for the Message Client Equipment
**                  (MCE) subsystem.
**
*****************************************************************************/
#ifndef WICED_BT_MCE_API_H
#define WICED_BT_MCE_API_H

#include "wiced_bt_map_def.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
/* Extra Debug Code */
#ifndef WICED_BT_MCE_DEBUG
#define WICED_BT_MCE_DEBUG           TRUE
#endif

/* configuration related constants */
/* MCE configuration data */
#define WICED_BT_MCE_NUM_INST        4
#ifndef WICED_BT_MCE_NUM_MA
#define WICED_BT_MCE_NUM_MA          2
#endif
#define WICED_BT_MCE_NUM_MN          WICED_BT_MCE_NUM_MA
#define WICED_BT_MCE_NUM_PM          WICED_BT_MCE_NUM_MA
#define WICED_BT_MCE_MN_NUM_SESSION  WICED_BT_MCE_NUM_MA

#define WICED_BT_MNS_RFCOMM_SCN      2
#define WICED_BT_MNS_L2CAP_PSM       0x1005

/* Client callback function events */
enum
{
    WICED_BT_MCE_ENABLE_EVT            = 0,
    WICED_BT_MCE_START_EVT,
    WICED_BT_MCE_STOP_EVT,
    WICED_BT_MCE_DISCOVER_EVT,
    WICED_BT_MCE_MA_OPEN_EVT,
    WICED_BT_MCE_MA_CLOSE_EVT,
    WICED_BT_MCE_MN_OPEN_EVT,
    WICED_BT_MCE_MN_CLOSE_EVT,
    WICED_BT_MCE_NOTIF_EVT,
    WICED_BT_MCE_NOTIF_REG_EVT,
    WICED_BT_MCE_SET_MSG_STATUS_EVT,
    WICED_BT_MCE_UPDATE_INBOX_EVT,
    WICED_BT_MCE_SET_FOLDER_EVT,
    WICED_BT_MCE_FOLDER_LIST_EVT,
    WICED_BT_MCE_MSG_LIST_EVT,
    WICED_BT_MCE_GET_MSG_EVT,
    WICED_BT_MCE_GET_MAS_INS_INFO,
    WICED_BT_MCE_PUSH_MSG_EVT,
    WICED_BT_MCE_MSG_PROG_EVT,
    WICED_BT_MCE_ABORT_EVT,
    WICED_BT_MCE_OBEX_PUT_RSP_EVT,
    WICED_BT_MCE_OBEX_GET_RSP_EVT,
    WICED_BT_MCE_DISABLE_EVT,
    WICED_BT_MCE_INVALID_EVT
};
typedef UINT8 wiced_bt_mce_evt_t;

#define WICED_BT_MCE_OPER_NONE               0
#define WICED_BT_MCE_OPER_GET_MSG            1
#define WICED_BT_MCE_OPER_PUSH_MSG           2

typedef UINT8 wiced_bt_mce_oper_t;

#define WICED_BT_MCE_MAX_NAME_LEN_PER_LINE    17

typedef struct
{
    char            name[WICED_BT_MCE_MAX_NAME_LEN_PER_LINE];
    wiced_bt_ma_inst_id_t mas_inst_id;
    UINT8           supported_msg_type;
    UINT8           scn;
    UINT16          rec_version;
    UINT16          psm;
    UINT32          peer_features;
} wiced_bt_mce_mas_rec_t;

/* Structure associated with WICED_BT_MCE_DISCOVER_EVT */
typedef struct
{
    UINT16                    status;
    wiced_bt_device_address_t bd_addr;
    UINT8                     num_mas_srv;
    wiced_bt_mce_mas_rec_t    rec[WICED_BT_MCE_NUM_INST];
} wiced_bt_mce_discover_t;

/* Structure associated with WICED_BT_MCE_MA_OPEN_EVT or
   WICED_BT_MCE_MA_CLOSE_EVT                            */
typedef struct
{
    wiced_bt_ma_status_t      status;
    wiced_bt_ma_sess_handle_t session_id;
    wiced_bt_ma_inst_id_t     mas_inst_id;
    wiced_bt_remote_name_t    dev_name;    /* Name of device, "" if unknown */
    wiced_bt_device_address_t bd_addr;     /* Address of device */
} wiced_bt_mce_ma_open_close_t;


/* Structure associated with WICED_BT_MCE_MN_START_EVT or
   WICED_BT_MCE_MN_STOP_EVT                            */
typedef struct
{
    wiced_bt_ma_status_t      status;
    wiced_bt_ma_sess_handle_t session_id;
} wiced_bt_mce_mn_start_stop_t;


/* Structure associated with WICED_BT_MCE_MN_OPEN_EVT or
   WICED_BT_MCE_MN_CLOSE_EVT                            */
typedef struct
{
    wiced_bt_ma_status_t      status;
    wiced_bt_remote_name_t    dev_name;    /* Name of device, "" if unknown */
    wiced_bt_device_address_t bd_addr;     /* Address of device */
    wiced_bt_ma_sess_handle_t session_id;
    wiced_bt_obex_rsp_code_t  obx_rsp_code;/* obex response code */
} wiced_bt_mce_mn_open_close_t;

/* Structure associated with WICED_BT_MCE_NOTIF_REG_EVT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t     handle;
    wiced_bt_ma_status_t          status;
} wiced_bt_mce_set_notif_reg_t;

/* Structure associated with WICED_BT_MCE_NOTIF_EVT  */
typedef struct
{
    UINT8                       *p_object;          /* event report data */
    wiced_bt_ma_sess_handle_t   session_id;         /* MCE connection handle */
    UINT16                      len;                /* length of the event report */
    BOOLEAN                     final;
    wiced_bt_ma_status_t        status;
    UINT8                       inst_id;            /* MAS instance ID */
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_notif_t;

typedef union
{
    UINT16              fld_list_size;
    struct
    {
        UINT16          msg_list_size;
        UINT8           new_msg;
    }                   msg_list_param;
} wiced_bt_mce_list_app_param_t;

/* Structure associated with WICED_BT_MCE_GET_FOLDER_LIST_EVT  */
typedef struct
{
    UINT8                           *p_data;
    wiced_bt_mce_list_app_param_t   *p_param;
    wiced_bt_ma_sess_handle_t       session_id;
    UINT16                          len; /* 0 if object not included */
    BOOLEAN                         is_final; /* TRUE - no more pkt to come */
    wiced_bt_ma_status_t            status;
    wiced_bt_obex_rsp_code_t        obx_rsp_code;       /* obex response code */
} wiced_bt_mce_list_data_t;

/* Structure associated with WICED_BT_MCE_GET_MSG_EVT  */
typedef struct
{
    wiced_bt_ma_status_t        status;
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_frac_deliver_t  frac_deliver;
    UINT8                       *p_data;
    UINT16                      len; /* 0 if object not included */
    BOOLEAN                     is_final; /* TRUE - no more pkt to come */
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_get_msg_t;


/* Structure associated with WICED_BT_MCE_GET_MAS_INS_INFO */
typedef struct
{
    wiced_bt_ma_status_t        status;
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_inst_id_t       mas_instance_id;
    wiced_bt_ma_mas_ins_info_t  mas_ins_info;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_get_mas_ins_info_t;


/* Structure associated with WICED_BT_MCE_MSG_PROG_EVT  */
typedef struct
{
    UINT32                      read_size;
    UINT32                      obj_size;
    wiced_bt_ma_sess_handle_t   handle;
    wiced_bt_mce_oper_t         operation;
} wiced_bt_mce_msg_prog_t;

/* Structure associated with WICED_BT_MCE_PUSH_MSG_EVT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_status_t        status;
    wiced_bt_ma_msg_handle_t    msg_handle;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_push_msg_t;

/* Structure associated with WICED_BT_MCE_UPDATE_INBOX_EVT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_status_t        status;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_update_inbox_t;

/* Structure associated with WICED_BT_MCE_SET_MSG_STATUS_EVT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_status_t        status;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_set_msg_status_t;

/* Structure associated with WICED_BT_MCE_SET_MSG_FOLDER_EVT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_status_t        status;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_set_folder_t;

/* Structure associated with WICED_BT_MCE_NOTIF_REG_EVT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_status_t        status;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_notif_reg_t;

/* Structure associated with tWICED_BT_MCE_ABORT  */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_status_t        status;
    wiced_bt_obex_rsp_code_t    obx_rsp_code;       /* obex response code */
} wiced_bt_mce_abort_t;

/* Structure associated with WICED_BT_MCE_OBEX_PUT_RSP_EVT
   and WICED_BT_MCE_OBEX_GET_RSP_EVT                        */
typedef struct
{
    wiced_bt_ma_sess_handle_t   session_id;
    wiced_bt_ma_inst_id_t       mas_instance_id;
    UINT8                       rsp_code;
} wiced_bt_mce_obex_rsp_t;

typedef union
{
    wiced_bt_mce_discover_t           discover;
    wiced_bt_mce_ma_open_close_t      ma_open;
    wiced_bt_mce_ma_open_close_t      ma_close;
    wiced_bt_mce_mn_start_stop_t      mn_start;
    wiced_bt_mce_mn_start_stop_t      mn_stop;
    wiced_bt_mce_mn_open_close_t      mn_open;
    wiced_bt_mce_mn_open_close_t      mn_close;
    wiced_bt_mce_notif_t              notif;
    wiced_bt_ma_status_t              status;         /* ENABLE and DISABLE event */
    wiced_bt_mce_update_inbox_t       upd_ibx;
    wiced_bt_mce_set_msg_status_t     set_msg_sts;
    wiced_bt_mce_set_folder_t         set_folder;
    wiced_bt_mce_notif_reg_t          notif_reg;

    wiced_bt_mce_list_data_t          list_data;      /* WICED_BT_MCE_FOLDER_LIST_EVT,
                                                         WICED_BT_MCE_MSG_LIST_EVT */
    wiced_bt_mce_get_msg_t            get_msg;        /* WICED_BT_MCE_GET_MSG_EVT  */
    wiced_bt_mce_get_mas_ins_info_t   get_mas_ins_info; /* WICED_BT_MCE_GET_MAS_INS_INFO */
    wiced_bt_mce_push_msg_t           push_msg;       /* WICED_BT_MCE_PUSH_MSG_EVT */
    wiced_bt_mce_msg_prog_t           prog;           /* WICED_BT_MCE_MSG_PROG_EVT */
    wiced_bt_mce_abort_t              abort;
    wiced_bt_mce_obex_rsp_t           ma_put_rsp;
    wiced_bt_mce_obex_rsp_t           ma_get_rsp;
} wiced_bt_mce_t;

/* Client callback function */
typedef void wiced_bt_mce_cback_t(wiced_bt_mce_evt_t event, wiced_bt_mce_t *p_data);


/*****************************************************************************
**  Function Declarations
*****************************************************************************/

/**************************
**  Server Functions
***************************/

/*******************************************************************************
**
** Function         wiced_bt_mce_enable
**
** Description      Enable the MCE subsystem.  This function must be
**                  called before any other functions in the MCE API are called.
**                  When the enable operation is complete the callback function
**                  will be called with an WICED_BT_MCE_ENABLE_EVT event.
**
** Parameter        p_cback: call function registered to receive call back events.
**                  app_id: application ID.
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_enable(wiced_bt_mce_cback_t *p_cback, UINT8 app_id);

/*******************************************************************************
**
** Function         wiced_bt_mce_disable
**
** Description      Disable the MCE subsystem.  If the client is currently
**                  connected to a peer device the connection will be closed.
**
** Parameter        None
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_disable(void);

/*******************************************************************************
**
** Function         wiced_bt_mce_mn_start
**
** Description      Start the Message Notification service server.
**                  When the Start operation is complete the callback function
**                  will be called with an WICED_BT_MCE_START_EVT event.
**                  Note: Mas always enable (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
**
**  Parameters      sec_mask - The security setting for the message access server.
**                  p_service_name - The name of the Message Notification service, in SDP.
**                                   Maximum length is 35 bytes.
**                  scn - The RFCOMM SCN number where MN server listens for incoming request.
**                  psm - [MAP 1.2 and above]
**                        The L2CAP PSM number where MN server listens for incoming request.
**                  features - Local supported features
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_mn_start(UINT8 sec_mask, const char *p_service_name, const UINT8 scn,
            const UINT16 psm, wiced_bt_ma_supported_features_t features);

/*******************************************************************************
**
** Function         wiced_bt_mce_mn_stop
**
** Description      Stop the Message Access service server.  If the server is currently
**                  connected to a peer device the connection will be closed.
**
** Parameter        None
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_mn_stop(void);

/**************************
**  Client Functions
***************************/

/*******************************************************************************
**
** Function         wiced_bt_mce_discover
**
** Description      Start service discover of MAP on the peer device
**
**                  When SDP is finished, the callback function will be called
**                  with WICED_BT_MCE_DISCOVER_EVT with status.
**
** Parameter        bd_addr: MAS server bd address.
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_discover(wiced_bt_device_address_t bd_addr);

/*******************************************************************************
**
** Function         wiced_bt_mce_open
**
** Description      Open a connection to an Message Access service server
**                  based on specified mas_instance_id
**
**                  When the connection is open the callback function
**                  will be called with a WICED_BT_MCE_MA_OPEN_EVT.  If the connection
**                  fails or otherwise is closed the callback function will be
**                  called with a WICED_BT_MCE_MA_CLOSE_EVT.
**
**                  Note: MAS always enable (BTA_SEC_AUTHENTICATE | BTA_SEC_ENCRYPT)
**
** Parameter        mas_instance_id - MAS instance ID on server device.
**                  bd_addr: MAS server bd address.
**                  sec_mask: security mask used for this connection.
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_open(wiced_bt_device_address_t bd_addr, wiced_bt_ma_inst_id_t mas_instance_id,
                       UINT8 sec_mask);

/*******************************************************************************
**
** Function         wiced_bt_mce_close
**
** Description      Close the specified MAS session to the server.
**
** Parameter        session_id - MAS session ID
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_close(wiced_bt_ma_sess_handle_t session_id);

/*******************************************************************************
**
** Function         wiced_bt_mce_mn_close
**
** Description      Close a MNS session to the client.
**
** Parameters       session_id - MNS session ID
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_mn_close(wiced_bt_ma_sess_handle_t session_id);

/*******************************************************************************
**
** Function         wiced_bt_mce_close_all
**
** Description      Close all MAS session to the server.
**
** Parameter        None
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_close_all(void);

/*******************************************************************************
**
** Function         wiced_bt_mce_notif_reg
**
** Description      Set the Message Notification status to On or OFF on the MSE.
**                  When notification is registered, message notification service
**                  must be enabled by calling API wiced_bt_mce_mn_start().
**
** Parameter        status - BTA_MA_NOTIF_ON if notification required
**                           BTA_MA_NOTIF_OFF if no notification
**                  session_id - MAS session ID
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_notif_reg(wiced_bt_ma_sess_handle_t session_id,
                            wiced_bt_ma_notif_status_t status);

/*******************************************************************************
**
** Function         wiced_bt_mce_update_inbox
**
** Description      This function is used to update the inbox for the
**                  specified MAS session.
**
** Parameter        session_id - MAS session ID
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_update_inbox(wiced_bt_ma_sess_handle_t session_id);

/*******************************************************************************
**
** Function         wiced_bt_mce_set_folder
**
** Description      This function is used to navigate the folders of the MSE for
**                  the specified MAS instance
**
** Parameter        a combination of flag and p_folder specify how to navigate the
**                  folders on the MSE
**                  case 1 flag = 2 folder = empty - reset to the default directory "telecom"
**                  case 2 flag = 2 folder = name of child folder - go down 1 level into
**                  this directory name
**                  case 3 flag = 3 folder = name of child folder - go up 1 level into
**                  this directory name (same as cd ../name)
**                  case 4 flag = 3 folder = empty - go up 1 level to the parent directory
**                  (same as cd ..)
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_set_folder(wiced_bt_ma_sess_handle_t session_id,
                             wiced_bt_ma_dir_nav_t flag, char *p_folder);

/*******************************************************************************
**
** Function         wiced_bt_mce_get_folder_list
**
** Description      This function is used to retrieve the folder list object from
**                  the current folder
**
** Parameter        session_id - MAS session ID
**                  max_list_count - maximum number of folder-list objects allowed
**                            The maximum allowed value for this filed is 1024
**                  start_offset - offset of the from the first entry of the folder-list
**                                   object
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_get_folder_list(wiced_bt_ma_sess_handle_t session_id,
                                  UINT16 max_list_count, UINT16 start_offset);

/*******************************************************************************
**
** Function         wiced_bt_mce_get_msg_list
**
** Description      This function is used to retrieve the folder list object from
**                  the current folder of the MSE
**
** Parameter        session_id -  session handle
**                  p_folder        - folder name
**                  p_filter_param - message listing filter parameters
**                  is_srmp_enable - True to add SRMP header
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_get_msg_list(wiced_bt_ma_sess_handle_t session_id,
                               const char *p_folder,
                               wiced_bt_ma_msg_list_filter_param_t *p_filter_param,
			       wiced_bool_t is_srmp_enable);

/*******************************************************************************
**
** Function         wiced_bt_mce_get_msg
**
** Description      This function is used to get bMessage or bBody of the
**                  specified message handle from MSE
**
** Parameter        session_id - session ID
**                  p_param - get message parameters, it shall not be NULL.
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_get_msg(wiced_bt_ma_sess_handle_t session_id,
                          wiced_bt_ma_get_msg_param_t *p_param);

/*******************************************************************************
**
** Function         wiced_bt_mce_get_mas_instance_info
**
** Description      This function enables the MCE to get the MAS instance information
**                  from the MSE
**
** Parameters       session_id - session ID
**                  mas_instance_id - MAS Instance ID
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_get_mas_instance_info(wiced_bt_ma_sess_handle_t session_id,
                                        wiced_bt_ma_inst_id_t mas_instance_id);

/*******************************************************************************
**
** Function         wiced_bt_mce_set_msg_status
**
** Description      This function is used to set the message status of the
**                  specified message handle
**
** Parameter        session_id - MAS session ID
**                  status_indicator : read/delete message
**                  status_value : on/off
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_set_msg_status(wiced_bt_ma_sess_handle_t session_id,
                                 wiced_bt_ma_msg_handle_t msg_handle,
                                 wiced_bt_ma_sts_indctr_t status_indicator,
                                 wiced_bt_ma_sts_value_t status_value);

/*******************************************************************************
**
** Function         wiced_bt_mce_push_msg
**
** Description      This function is used to upload a message
**                  to the specified folder in MSE
**
** Parameter        session_id - MAS session ID
**                  p_param - push message parameters, it shall not be NULL.
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_push_msg(wiced_bt_ma_sess_handle_t session_id,
                           wiced_bt_ma_push_msg_param_t *p_param);

/*******************************************************************************
**
** Function         wiced_bt_mce_abort
**
** Description      This function is used to abort the current OBEX multi-packet
**                  operation
**
** Parameter        bd_addr: MAS server bd address.
**                  mas_instance_id - MAS instance ID on server device.
**
** Returns          void
**
*******************************************************************************/
void wiced_bt_mce_abort(wiced_bt_device_address_t bd_addr, wiced_bt_ma_inst_id_t mas_instance_id);

#endif
