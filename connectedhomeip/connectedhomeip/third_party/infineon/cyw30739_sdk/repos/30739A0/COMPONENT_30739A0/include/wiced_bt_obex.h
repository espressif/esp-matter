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

/** @file
 *
 *  AIROC Bluetooth OBEX Application Programming Interface
 */
#pragma once

#include "wiced.h"
#include "wiced_bt_types.h"

/* Return values for API functions */
enum
{
    OBEX_SUCCESS,           /* Status is successful. */
    OBEX_BAD_PARAMS,        /* Bad parameter(s). */
    OBEX_NO_RESOURCES,      /* No resources (GKI buffers, control block) */
    OBEX_BAD_HANDLE         /* The OBEX handle is not valid. */
};
typedef uint8_t     wiced_bt_obex_status_t;

typedef uint16_t    wiced_bt_obex_handle_t;

#define OBEX_HANDLE_NULL                0


#define L2CAP_MIN_OFFSET            13      /* from l2c_api.h */
#define OBEX_MAX_TARGET_LEN         16

#undef  GKI_MAX_BUF_SIZE
#define GKI_MAX_BUF_SIZE            1024

/* 18 is 7/OBEX_CONN_HDRS_OFFSET + 5/conn id, 2/ssn, 2/srm 2/srm_param */
#define OBEX_HDR_OFFSET             (18 + L2CAP_MIN_OFFSET)

#define OBEX_MIN_MTU                255 /* per IrOBEX spec */
#define OBEX_MAX_MTU                (GKI_MAX_BUF_SIZE - BT_HDR_SIZE - OBEX_HDR_OFFSET)


/* OBEX Request Codes */
#define OBEX_REQ_CONNECT                        0x00
#define OBEX_REQ_DISCONNECT                     0x01
#define OBEX_REQ_PUT                            0x02
#define OBEX_REQ_GET                            0x03
#define OBEX_REQ_SETPATH                        0x05
#define OBEX_REQ_ACTION                         0x06
#define OBEX_REQ_SESSION                        0x07
#define OBEX_REQ_ABORT                          0x7F

typedef uint8_t     wiced_bt_obex_req_code_t;


/* OBEX Response Codes */
#define OBEX_RSP_DEFAULT                        0x00
#define OBEX_RSP_FAILED                         0x08    /* OBEX failed - not from spec */
#define OBEX_RSP_CONTINUE                       0x10    /* Continue */
#define OBEX_RSP_OK                             0x20    /* OK, Success */
#define OBEX_RSP_CREATED                        0x21    /* Created */
#define OBEX_RSP_ACCEPTED                       0x22    /* Accepted */
#define OBEX_RSP_NON_AUTH_INFO                  0x23    /* Non-Authoritative Information */
#define OBEX_RSP_NO_CONTENT                     0x24    /* No Content */
#define OBEX_RSP_RESET_CONTENT                  0x25    /* Reset Content */
#define OBEX_RSP_PART_CONTENT                   0x26    /* Partial Content */
#define OBEX_RSP_MULTI_CHOICES                  0x30    /* Multiple Choices */
#define OBEX_RSP_MVD_PERM                       0x31    /* Moved Permanently */
#define OBEX_RSP_MVD_TEMP                       0x32    /* Moved temporarily */
#define OBEX_RSP_SEE_OTHER                      0x33    /* See Other */
#define OBEX_RSP_NOT_MODIFIED                   0x34    /* Not modified */
#define OBEX_RSP_USE_PROXY                      0x35    /* Use Proxy */
#define OBEX_RSP_BAD_REQUEST                    0x40    /* Bad Request - server couldn't understand request */
#define OBEX_RSP_UNAUTHORIZED                   0x41    /* Unauthorized */
#define OBEX_RSP_PAYMENT_REQD                   0x42    /* Payment required */
#define OBEX_RSP_FORBIDDEN                      0x43    /* Forbidden - operation is understood but refused */
#define OBEX_RSP_NOT_FOUND                      0x44    /* Not Found */
#define OBEX_RSP_NOT_ALLOWED                    0x45    /* Method not allowed */
#define OBEX_RSP_NOT_ACCEPTABLE                 0x46    /* Not Acceptable */
#define OBEX_RSP_PROXY_AUTH_REQD                0x47    /* Proxy Authentication required */
#define OBEX_RSP_REQUEST_TIMEOUT                0x48    /* Request Time Out */
#define OBEX_RSP_CONFLICT                       0x49    /* Conflict */
#define OBEX_RSP_GONE                           0x4A    /* Gone */
#define OBEX_RSP_LENGTH_REQD                    0x4B    /* Length Required */
#define OBEX_RSP_PRECONDTN_FAILED               0x4C    /* Precondition failed */
#define OBEX_RSP_REQ_ENT_2_LARGE                0x4D    /* Requested entity too large */
#define OBEX_RSP_REQ_URL_2_LARGE                0x4E    /* Request URL too large */
#define OBEX_RSP_UNSUPTD_TYPE                   0x4F    /* Unsupported media type */
#define OBEX_RSP_INTRNL_SRVR_ERR                0x50    /* Internal Server Error */
#define OBEX_RSP_NOT_IMPLEMENTED                0x51    /* Not Implemented */
#define OBEX_RSP_BAD_GATEWAY                    0x52    /* Bad Gateway */
#define OBEX_RSP_SERVICE_UNAVL                  0x53    /* Service Unavailable */
#define OBEX_RSP_GATEWAY_TIMEOUT                0x54    /* Gateway Timeout */
#define OBEX_RSP_HTTP_VER_NOT_SUPTD             0x55    /* HTTP version not supported */
#define OBEX_RSP_DATABASE_FULL                  0x60    /* Database Full */
#define OBEX_RSP_DATABASE_LOCKED                0x61    /* Database Locked */

typedef uint8_t     wiced_bt_obex_rsp_code_t;


/* SetPath Request Flags - the following definitions can be ORed if both flags are wanted */
#define OBEX_SPF_BACKUP     0x01    /* Backup a level before applying name(equivalent to ../) */
#define OBEX_SPF_NO_CREATE  0x02    /* Don't create directory if it does not exist, return an error instead. */
typedef uint8_t     wiced_bt_obex_setpath_flag_t;

/* Actions */
#define OBEX_ACTION_COPY            0x00    /* Copy object */
#define OBEX_ACTION_MOVE            0x01    /* Move/rename object */
#define OBEX_ACTION_PERMISSION      0x02    /* Set object permission */
typedef uint8_t     wiced_bt_obex_action_t;

/* OBEX request parameter */
typedef union
{
    wiced_bt_obex_setpath_flag_t    sp_flags;   /* Set Path request */
    wiced_bool_t                    final;      /* Put and Get request */
    wiced_bt_obex_action_t          action;     /* Action request */
} wiced_bt_obex_req_param_t;


/* OBEX server and client callback events */
enum
{
    OBEX_NULL_EVT,
    /* server events */
    OBEX_CONNECT_REQ_EVT,       /* param = packet MTU */
    OBEX_SESSION_REQ_EVT,       /* A Crease Session or Resume Session request is received by the server. Call wiced_bt_obex_session_rsp(). */
    OBEX_DISCONNECT_REQ_EVT,
    OBEX_PUT_REQ_EVT,           /* could have param indicate delete or create? */
    OBEX_GET_REQ_EVT,
    OBEX_SETPATH_REQ_EVT,       /* param = flags */
    OBEX_ABORT_REQ_EVT,
    OBEX_ACTION_REQ_EVT,        /* An Action request is received by the server. Call wiced_bt_obex_action_rsp(). */

    /* client events */
    OBEX_CONNECT_RSP_EVT,       /* param = packet MTU */
    OBEX_SESSION_RSP_EVT,       /* A response for Create Session or Resume Session is received by the client. The client needs to remember the session id. The session id is to be used in calling wiced_bt_obex_resume_session, if the current session is terminated prematurely. */
    OBEX_DISCONNECT_RSP_EVT,
    OBEX_PUT_RSP_EVT,
    OBEX_GET_RSP_EVT,
    OBEX_SETPATH_RSP_EVT,
    OBEX_ABORT_RSP_EVT,
    OBEX_ACTION_RSP_EVT,        /* An Action response is received by the client. */

    /* common events */
    OBEX_SESSION_INFO_EVT,      /* the session information event to resume the session. */
    OBEX_CLOSE_IND_EVT,         /* when transport goes down; p_pkt = NULL; no response needed */
    OBEX_TIMEOUT_EVT,           /* param = wiced_bt_obex_event_t */
    OBEX_PASSWORD_EVT
};
typedef uint8_t     wiced_bt_obex_event_t;

enum
{
    OBEX_PUT_TYPE_PUT,          /* Regular Put request */
    OBEX_PUT_TYPE_DELETE,       /* Delete request - a Put request with NO Body or End-of-Body header. */
    OBEX_PUT_TYPE_CREATE        /* Create-Empty request - a Put request with an empty End-of-Body header. */
};
typedef uint8_t     wiced_bt_obex_put_type_t;

typedef struct
{
    uint8_t                     ssn;    /* session sequence number */
    wiced_bool_t                final;  /* TRUE, if this is the final packet of this PUT transaction. */
    wiced_bt_obex_put_type_t    type;   /* The type of PUT request. */
} wiced_bt_obex_put_evt_t;

typedef struct
{
    uint8_t             ssn;        /* session sequence number */
    wiced_bool_t        final;      /* TRUE, if this is the final packet of this GET transaction. */
} wiced_bt_obex_get_evt_t;

typedef struct
{
    uint8_t                     ssn;        /* session sequence number */
    wiced_bt_device_address_t   peer_addr;  /* The peer Bluetooth Address. */
    uint16_t                    mtu;        /* The peer MTU. This element is associated with OBEX_CONNECT_REQ_EVT and OBEX_CONNECT_RSP_EVT. */
    wiced_bt_obex_handle_t      handle;     /* the OBEX handle returned by OBEX_StartServer(), OBEX_CreateSession() and OBEX_ConnectReq() */
    wiced_bool_t                no_rsp;     /* TRUE, when the event is generated as a part of RESUME SESSION */
} wiced_bt_obex_conn_evt_t;

/* Session Opcode Definitions: */
#define OBEX_SESS_OP_CREATE     0x00 /* Create Session */
#define OBEX_SESS_OP_CLOSE      0x01 /* Close Session */
#define OBEX_SESS_OP_SUSPEND    0x02 /* Suspend Session */
#define OBEX_SESS_OP_RESUME     0x03 /* Resume Session */
#define OBEX_SESS_OP_SET_TIME   0x04 /* Set Timeout */
#define OBEX_SESS_OP_TRANSPORT  0xFF /* transport dropped */
typedef uint8_t     wiced_bt_obex_sess_opcode_t;

/* Session States Definitions for external use: */
enum
{
    OBEX_SESS_ST_NONE,          /* 0x00    session is not engaged/closed */
    OBEX_SESS_ST_ACTIVE,        /* 0x01    session is active. */
    OBEX_SESS_ST_SUSPENDED,     /* 0x02    session is suspended. */
    OBEX_SESS_ST_EXT_MAX
};
typedef uint8_t     wiced_bt_obex_sess_state_t;

typedef struct
{
    uint8_t                     ssn;            /* session sequence number */
    wiced_bt_obex_sess_opcode_t sess_op;        /* the session op code */
    wiced_bt_obex_sess_state_t  sess_st;        /* the session state */
    wiced_bt_device_address_t   peer_addr;      /* The peer Bluetooth Address. */
    uint8_t                     *p_sess_info;   /* The session ID and the local nonce for a reliable session, a reference to the location in OBEX control block or NULL */
    uint32_t                    timeout;        /* The number of seconds remaining in suspend. 0xffff if infinite. */
    uint32_t                    obj_offset;     /* The object offset for resume session. */
    uint8_t                     nssn;           /* The next session sequence number the server expects */
} wiced_bt_obex_sess_evt_t;

typedef struct
{
    uint8_t                 ssn;        /* session sequence number */
    wiced_bt_obex_action_t  action;     /* The action opcode. */
} wiced_bt_obex_action_evt_t;

typedef struct
{
    uint8_t                         ssn;        /* session sequence number */
    wiced_bt_obex_setpath_flag_t    flag;       /* The set path flags. */
} wiced_bt_obex_setpath_evt_t;

/* permission flags */
#define OBEX_PERMISSION_READ        0x01
#define OBEX_PERMISSION_WRITE       0x02
#define OBEX_PERMISSION_DELETE      0x04
#define OBEX_PERMISSION_MODIFY      0x80

typedef union
{
    uint8_t                         ssn;        /* session sequence number */
    wiced_bt_obex_conn_evt_t        conn;       /* This element is associated with OBEX_CONNECT_REQ_EVT and OBEX_CONNECT_RSP_EVT. */
    wiced_bt_obex_sess_evt_t        sess;       /* This element is associated with OBEX_SESSION_REQ_EVT and OBEX_SESSION_RSP_EVT. */
    wiced_bt_obex_put_evt_t         put;        /* This element is associated with OBEX_PUT_REQ_EVT. */
    wiced_bt_obex_setpath_evt_t     sp;         /* This element is associated with OBEX_SETPATH_REQ_EVT. */
    wiced_bt_obex_action_evt_t      action;     /* This element is associated with OBEX_ACTION_REQ_EVT */
    wiced_bt_obex_get_evt_t         get;        /* This element is associated with OBEX_GET_REQ_EVT. TRUE, if this is the final packet that contains the OBEX headers for this GET request. */
} wiced_bt_obex_evt_param_t;

/* Server Callback type: */
typedef void (wiced_bt_obex_server_cback_t) (wiced_bt_obex_handle_t handle, wiced_bt_obex_event_t event, wiced_bt_obex_evt_param_t param, uint8_t *p_pkt);
/* Client Callback type: */
typedef void (wiced_bt_obex_client_cback_t) (wiced_bt_obex_handle_t handle, wiced_bt_obex_event_t event, uint8_t rsp_code, wiced_bt_obex_evt_param_t param, uint8_t *p_pkt);


#define OBEX_MAX_TARGET_LEN          16

typedef struct
{
    uint16_t        len;                            /* Length of target header. */
    uint8_t         target[OBEX_MAX_TARGET_LEN];    /* The byte sequence that describes the target header. */
} wiced_bt_obex_target_t;

typedef struct
{
    wiced_bt_obex_target_t          *p_target;
    wiced_bt_obex_server_cback_t    *p_cback;
    uint16_t        mtu;
    uint8_t         scn;            /* The RFCOMM SCN number that this server listens for incoming requests. 0, if do not wish to listen to connection from RFCOMM. */
    wiced_bool_t    authenticate;
    uint8_t         auth_option;
    uint8_t         realm_charset;
    uint8_t         *p_realm;
    uint8_t         realm_len;
    uint8_t         max_sessions;
    wiced_bool_t    get_nonf;       /* report GET non-final request event. If FALSE, GET response is sent automatically */
    uint16_t        psm;            /* The L2CAP PSM number that this server listens for incoming requests. 0, if do not wish to listen to connection from L2CAP. */
    uint32_t        nonce;          /* This is converted to UINT8[16] internally before adding to the OBEX header. This value is copied to the server control block and is increased after each use. 0, if only legacy OBEX (unreliable) session is desired. */
    wiced_bool_t    srm;            /* TRUE, to support single response mode. */
    uint8_t         max_suspend;    /* Max number of suspended session. must be less than OBEX_MAX_SUSPEND_SESSIONS. ignored, if nonce is 0 */
} wiced_bt_obex_start_params_t;


/* OBEX Header Identifiers */
#define OBEX_HI_NULL                     0
#define OBEX_HI_COUNT                    0xC0
#define OBEX_HI_NAME                     0x01
#define OBEX_HI_TYPE                     0x42
#define OBEX_HI_LENGTH                   0xC3
#define OBEX_HI_TIME                     0x44
#define OBEX_HI_DESCRIPTION              0x05
#define OBEX_HI_TARGET                   0x46
#define OBEX_HI_HTTP                     0x47
#define OBEX_HI_BODY                     0x48
#define OBEX_HI_BODY_END                 0x49
#define OBEX_HI_WHO                      0x4A
#define OBEX_HI_CONN_ID                  0xCB
#define OBEX_HI_APP_PARMS                0x4C
#define OBEX_HI_CHALLENGE                0x4D
#define OBEX_HI_AUTH_RSP                 0x4E
#define OBEX_HI_CREATOR_ID               0xCF
#define OBEX_HI_WAN_UUID                 0x50
#define OBEX_HI_OBJ_CLASS                0x51
#define OBEX_HI_SESSION_PARAM            0x52
#define OBEX_HI_SESSION_SN               0x93
#define OBEX_HI_ACTION_ID                0x94
#define OBEX_HI_DEST_NAME                0x15
#define OBEX_HI_PERMISSION               0xD6
#define OBEX_HI_SRM                      0x97
#define OBEX_HI_SRM_PARAM                0x98
#define OBEX_HI_LO_USER                  0x30
#define OBEX_HI_HI_USER                  0x3F

typedef uint8_t     wiced_bt_obex_header_identifier_t;

typedef struct
{
    uint8_t     tag;
    uint8_t     len;
    uint8_t     *p_array;
} wiced_bt_obex_triplet_t;


/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup  wicedbt_obex   OBEX
 * @ingroup     wicedbt
 *
 * OBEX API Functions.
 *
 * @{
 */

/**
 * Function     wiced_bt_obex_init
 *
 *              Initialize the OBEX library
 *              This function must be called before accessing any other of OBEX APIs
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_init(void);


/****************************************************************************/
/**
 * OBEX Server Functions
 *
 *  @addtogroup  server_api_functions       Server
 *  @ingroup     wicedbt_obex
 *
 *  <b> Server API Functions </b> sub module for @b OBEX.
 *
 * @{
 */
/****************************************************************************/

/**
 * Function     wiced_bt_obex_start_server
 *
 *              Start OBEX server
 *
 *  @param[in]   p_params : Parameters for starting server
 *  @param[out]  p_handle : Pointer to return OBEX server handle
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_start_server(wiced_bt_obex_start_params_t *p_params, wiced_bt_obex_handle_t *p_handle);

/**
 * Function     wiced_bt_obex_stop_server
 *
 *              Stop OBEX server
 *
 *  @param[in]   handle : OBEX server handle
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_stop_server(wiced_bt_obex_handle_t handle);

/**
 * Function     wiced_bt_obex_send_rsp
 *
 *              Send response to a Request from an OBEX client.
 *
 *  @param[in]   handle   : OBEX server handle
 *  @param[in]   req_code : Request code
 *  @param[in]   rsp_code : Response code
 *  @param[in]   p_pkt    : Response packet
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_send_response(wiced_bt_obex_handle_t handle, wiced_bt_obex_req_code_t req_code, wiced_bt_obex_rsp_code_t rsp_code, uint8_t *p_pkt);

#ifdef OBEX_LIB_SESSION_SUPPORTED
/**
 * Function     wiced_bt_obex_session_rsp
 *
 *              Respond to a request to create a reliable session.
 *
 *  @param[in]   handle   : OBEX server handle
 *  @param[in]   rsp_code : Response code
 *  @param[in]   ssn      : Session sequence number
 *  @param[in]   offset   : Data offset
 *  @param[in]   p_pkt    : Response packet
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_session_response(wiced_bt_obex_handle_t handle, wiced_bt_obex_rsp_code_t rsp_code, uint8_t ssn, uint32_t offset, uint8_t *p_pkt);
#endif

/**
 * Function     wiced_bt_obex_get_peer_addr
 *
 *              This function is called to get the Bluetooth address of the
 *              connected device
 *
 *  @param[in]   handle   : OBEX server handle
 *  @param[out]  bd_addr  : Remote BD address
 *
 *  @return      L2CAP channel ID
 *
 */
uint16_t wiced_bt_obex_get_peer_addr(wiced_bt_obex_handle_t handle, wiced_bt_device_address_t bd_addr);


/****************************************************************************/
/**
 * OBEX Client Functions
 *
 *  @addtogroup  client_api_functions       Client
 *  @ingroup     wicedbt_obex
 *
 *  <b> Client API Functions </b> sub module for @b OBEX.
 *
 * @{
 */
/****************************************************************************/

/**
 * Function     wiced_bt_obex_connect
 *
 *              This function registers a client entity to OBEX and sends a
 *              CONNECT request to the server
 *
 *  @param[in]   bd_addr  : Remote server BD address
 *  @param[in]   scn      : Remote server SCN
 *  @param[in]   mtu      : MTU
 *  @param[in]   p_cback  : Pointer to client event callback function
 *  @param[out]  p_handle : Pointer to return client connection handle
 *  @param[in]   p_pkt    : Connect request packet
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_connect(wiced_bt_device_address_t bd_addr, uint8_t scn, uint16_t mtu,
        wiced_bt_obex_client_cback_t *p_cback, wiced_bt_obex_handle_t *p_handle, uint8_t *p_pkt);

/**
 * Function     wiced_bt_obex_disconnect
 *
 *              This function disconnects the server and unregister the
 *              client entity.
 *
 *  @param[in]   handle   : Client connection handle
 *  @param[in]   p_pkt    : Disconnect request packet
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_disconnect(wiced_bt_obex_handle_t handle, uint8_t *p_pkt);

/**
 * Function     wiced_bt_obex_send_request
 *
 *              This function sends a request to the connected server.
 *
 *  @param[in]   handle   : Client connection handle
 *  @param[in]   req_code : Request code
 *  @param[in]   p_param  : Request parameters
 *  @param[in]   p_pkt    : Request packet
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_send_request(wiced_bt_obex_handle_t handle, wiced_bt_obex_req_code_t req_code, wiced_bt_obex_req_param_t *p_param, uint8_t *p_pkt);

#ifdef OBEX_LIB_SESSION_SUPPORTED
/**
 * Function     wiced_bt_obex_alloc_session
 *
 *              This function tries to find the suspended session with session
 *              info.  If p_session_info is NULL then it uses handle to find a
 *              control block.  If it is not found then allocate a new session.
 *
 *  @param[in]      p_session_info : Session info used to search for suspended session
 *  @param[in]      scn            : RFCOMM SCN
 *  @param[in]      p_psm          : Virtual PSM for L2CAP only, set to 0 if use RFCOMM
 *  @param[in]      p_cback        : Client callback
 *  @param[in/out]  p_handle       : Return allocated handle
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_alloc_session(uint8_t *p_session_info, uint8_t scn, uint16_t *p_psm,
        wiced_bt_obex_client_cback_t *p_cback, wiced_bt_obex_handle_t *p_handle);

/**
 * Function     wiced_bt_obex_create_session
 *
 *              This function registers a client entity to OBEX and create
 *              a reliable session with server
 *
 *  @param[in]  bd_addr : Server device address
 *  @param[in]  mtu     : MTU
 *  @param[in]  srm     : TRUE - enable SRM, FALSE - no SRM
 *  @param[in]  nonce   : Create reliable session if this parameter is set
 *  @param[in]  handle  : Client handle
 *  @param[in]  p_pkt   : Request packet
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_create_session(wiced_bt_device_address_t bd_addr, uint16_t mtu,
        wiced_bool_t srm, uint32_t nonce, wiced_bt_obex_handle_t handle, uint8_t *p_pkt);

/**
 * Function     wiced_bt_obex_close_session
 *
 *              This function closes a reliable session
 *
 *  @param[in]  handle  : Client handle
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_close_session(wiced_bt_obex_handle_t handle);

/**
 * Function     wiced_bt_obex_suspend_session
 *
 *              This function suspends a reliable session
 *
 *  @param[in]  handle  : Client handle
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_suspend_session(wiced_bt_obex_handle_t handle);

/**
 * Function     wiced_bt_obex_resume_session
 *
 *              This function registers a client entity to OBEX and resumes
 *              a previously interrupted reliable session
 *
 *  @param[in]  bd_addr : Server device address
 *  @param[in]  ssn     : Session sequence number
 *  @param[in]  offset  : Data offset
 *  @param[in]  handle  : Client handle
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_resume_session(wiced_bt_device_address_t bd_addr, uint8_t ssn,
        uint32_t offset, wiced_bt_obex_handle_t handle);

/**
 * Function     wiced_bt_obex_set_session_timeout
 *
 *              This function sets timeout for a reliable session
 *
 *  @param[in]  handle   : Client handle
 *  @param[in]  timeout  : Session timeout
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_set_session_timeout(wiced_bt_obex_handle_t handle, uint32_t timeout);
#endif


/****************************************************************************/
/**
 * OBEX Header Operations
 *
 *  @addtogroup  header_api_functions       Header Operations
 *  @ingroup     wicedbt_obex
 *
 *  <b> Header Operation API Functions </b> sub module for @b OBEX.
 *
 * @{
 */
/****************************************************************************/

/**
 * Function     wiced_bt_obex_header_init
 *
 *              This function is called to initialize an OBEX packet. This
 *              function takes a GKI buffer and sets the offset in BT_HDR as
 *              OBEX_HDR_OFFSET, the len as 0. The layer_specific is set to the
 *              length still available. This function compares the given
 *              (pkt_size - sizeof(uint8_t)) with the peer MTU to get the lesser
 *              of the two and set the layer_specific to
 *              (lesser_size - OBEX_HDR_OFFSET).
 *              If composing a header for the CONNECT request (there is no
 *              client handle yet), use OBEX_HANDLE_NULL as the handle.
 *
 *  @param[in]  handle    : OBEX handle
 *  @param[in]  pkt_size  : Required packet size
 *
 *  @return     A pointer to allocated packet
 *
 */
uint8_t * wiced_bt_obex_header_init(wiced_bt_obex_handle_t handle, uint16_t pkt_size);

/**
 * Function     wiced_bt_obex_add_header
 *
 *              This function adds a header to the OBEX packet.
 *
 *              For Unicode text header, parameter p_data should point to an Ascii string
 *              and len should contain the string length.  This function will convert the
 *              input to Unicode string.  If the input string is UTF-8, call
 *              @link wiced_bt_obex_add_header_utf8 @endlink  instead.
 *              For byte sequence header, parameter p_data should point to the byte sequence
 *              and len should contain the byte sequence length.
 *              For 1 byte header, parameter p_data should point to a byte and len
 *              should equal to 1.
 *              For 4 byte header, parameter p_data should point to a uint32_t and len
 *              should equal to 4.
 *
 *  @param[in]  p_pkt   : OBEX packet
 *  @param[in]  id      : OBEX header identifier
 *  @param[in]  p_data  : Pointer to data
 *  @param[in]  len     : Data length
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_add_header(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id,
        uint8_t *p_data, uint16_t len);

/**
 * Function     wiced_bt_obex_add_header_utf8
 *
 *              This function adds a Unicode string header with UTF-8 string as input
 *
 *  @param[in]  p_pkt  : OBEX packet
 *  @param[in]  id     : OBEX header identifier
 *  @param[in]  p_str  : Pointer to UTF-8 string
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_add_header_utf8(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id, uint8_t *p_str);

/**
 * Function     wiced_bt_obex_add_triplet_header
 *
 *              This function adds a header with data in triplet format
 *
 *  @param[in]  p_pkt      : OBEX packet
 *  @param[in]  id         : OBEX header identifier
 *  @param[in]  p_triplet  : Pointer to triplets
 *  @param[in]  num        : Number of triplets
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_add_triplet_header(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id,
        wiced_bt_obex_triplet_t *p_triplet, uint8_t num);

/**
 * Function     wiced_bt_obex_add_byte_sequence_start
 *
 *              This is an alternative way to add a byte sequence header.  Call this
 *              function to get the starting point of the byte sequence header buffer,
 *              fill in byte sequence data, then call wiced_bt_obex_add_byte_sequence_end
 *              to finish.
 *
 *  @param[in]   p_pkt  : OBEX packet
 *  @param[out]  p_len  : Pointer to return available buffer length
 *
 *  @return     Starting point to the OBEX packet byte sequence header buffer
 *
 */
uint8_t * wiced_bt_obex_add_byte_sequence_start(uint8_t *p_pkt, uint16_t *p_len);

/**
 * Function     wiced_bt_obex_add_byte_sequence_end
 *
 *              This function is called to finish adding byte sequence header by adding
 *              header identifier and length.  It is assumed that the actual value of
 *              the byte sequence has been copied into the OBEX packet.
 *
 *  @param[in]   p_pkt  : OBEX packet
 *  @param[in]   id     : OBEX header identifier
 *  @param[in]   len    : Length of data
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_add_byte_sequence_end(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id,
        uint16_t len);

/**
 * Function     wiced_bt_obex_read_header
 *
 *              This function reads a header from the OBEX packet.
 *
 *              For Unicode text header, this function will convert Unicode to Ascii
 *              string.  To convert Unicode to UTF-8 string, call
 *              @link wiced_bt_obex_read_header_utf8 @endlink instead.
 *              For byte sequence header, it reads byte sequence to buffer pointed to by
 *              parameter p_data.  To avoid the data copying, call
 *              @link wiced_bt_obex_find_byte_sequence_header @endlink instead.
 *              For 1 byte and 4 byte header, it reads data to buffer pointed to by p_data.
 *
 *  @param[in]      p_pkt   : OBEX packet
 *  @param[in]      id      : OBEX header identifier
 *  @param[out]     p_data  : Pointer to buffer that receives data
 *  @param[in/out]  p_len   : Input: output buffer size, Output: read data length
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_read_header(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id,
        uint8_t *p_data, uint16_t *p_len);

/**
 * Function     wiced_bt_obex_read_header_utf8
 *
 *              This function reads a Unicode string header with UTF-8 string as output
 *
 *  @param[in]   p_pkt    : OBEX packet
 *  @param[in]   id       : OBEX header identifier
 *  @param[out]  p_data   : Pointer to buffer that receives UTF-8 string
 *  @param[in]   max_len  : Max output size
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_read_header_utf8(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id,
        uint8_t *p_data, uint16_t max_len);

/**
 * Function     wiced_bt_obex_find_byte_sequence_header
 *
 *              This function finds a specific byte sequence header, return its data pointer and length
 *
 *  @param[in]   p_pkt   : OBEX packet
 *  @param[in]   id      : OBEX header identifier
 *  @param[out]  p_data  : A pointer to return header data pointer
 *  @param[out]  p_len   : Return header length
 *
 *  @return @link wiced_bt_obex_status_e wiced_bt_obex_status_t @endlink
 *
 */
wiced_bt_obex_status_t wiced_bt_obex_find_byte_sequence_header(uint8_t *p_pkt, wiced_bt_obex_header_identifier_t id,
        uint8_t **p_data, uint16_t *p_len);

/**
 * Function     wiced_bt_obex_find_body_header
 *
 *              This function finds OBEX_HI_BODY and OBEX_HI_BODY_END headers in an
 *              OBEX packet
 *
 *  @param[in]   p_pkt   : OBEX packet
 *  @param[out]  p_body  : A pointer to return body header pointer
 *  @param[out]  p_len   : Return body header length
 *  @param[out]  p_end   : TRUE  : found a OBEX_HI_BODY_END header
 *                       : FALSE : found a OBEX_HI_BODY header or no body header was found
 *
 *  @return      0 : No body header was found
 *               1 : Found a OBEX_HI_BODY or OBEX_HI_BODY_END header
 *               2 : Found both OBEX_HI_BODY and OBEX_HI_BODY_END headers
 *
 */
uint8_t wiced_bt_obex_find_body_header(uint8_t *p_pkt, uint8_t **p_body, uint16_t *p_len, wiced_bool_t *p_end);


#ifdef __cplusplus
} /* extern "C" */
#endif
