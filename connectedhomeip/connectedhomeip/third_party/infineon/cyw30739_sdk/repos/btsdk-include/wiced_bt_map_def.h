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
**  Name:           wiced_bt_map_def.h
**
**  Description:    This file contains the common definitions for the Message Access
**                  profile (MA) related SW modules
**
*****************************************************************************/
#ifndef WICED_BT_MA_DEF_H
#define WICED_BT_MA_DEF_H

#include "wiced_bt_obex.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/


#define WICED_BT_MA_HANDLE_SIZE          17

typedef UINT8  wiced_bt_ma_msg_handle_t[WICED_BT_MA_HANDLE_SIZE];

#define WICED_BT_MA_DEFAULT_SUPPORTED_FEATURES   0x0000005F

typedef UINT32 wiced_bt_ma_supported_features_t;


#define WICED_BT_MA_DATETIME_SIZE         19   /* In the format of YEAR MONTH DATE T HOURS MINUTES SECONDS "<20120822T100000>" */
#define WICED_BT_MA_SUBJECT_SIZE          255  /* 255 according to MAP 1.2*/
#define WICED_BT_MA_PRIORITY_SIZE         7    /* Either "<Yes>" or "<No>"*/
#define WICED_BT_MA_NAME_LEN              255  /* 255 according to MAP 1.2*/


typedef wiced_bt_obex_handle_t wiced_bt_ma_sess_handle_t;

#ifndef WICED_BT_MA_INS_INFO_MAX_LEN
#define WICED_BT_MA_INS_INFO_MAX_LEN    200  /* Instance info cannot be longer than 200 according to spec (including NULL termination) */
#endif
typedef char wiced_bt_ma_mas_ins_info_t[WICED_BT_MA_INS_INFO_MAX_LEN];

typedef UINT8 wiced_bt_ma_inst_id_t;

#define WICED_BT_MA_SUP_FEA_NOTIF_REG        0x00000001
#define WICED_BT_MA_SUP_FEA_NOTIF            0x00000002
#define WICED_BT_MA_SUP_FEA_BROWSING         0x00000004
#define WICED_BT_MA_SUP_FEA_UPLOADING        0x00000008
#define WICED_BT_MA_SUP_FEA_DELETE           0x00000010
#define WICED_BT_MA_SUP_FEA_INST_INFO        0x00000020
#define WICED_BT_MA_SUP_FEA_EXT_EVENT_REP    0x00000040

typedef UINT32 wiced_bt_ma_sup_fea_mask_t;


#define WICED_BT_MA_STATUS_OK                0
#define WICED_BT_MA_STATUS_FAIL              1   /* Used to pass all other errors */
#define WICED_BT_MA_STATUS_ABORTED           2
#define WICED_BT_MA_STATUS_NO_RESOURCE       3
#define WICED_BT_MA_STATUS_EACCES            4
#define WICED_BT_MA_STATUS_ENOTEMPTY         5
#define WICED_BT_MA_STATUS_EOF               6
#define WICED_BT_MA_STATUS_EODIR             7
#define WICED_BT_MA_STATUS_ENOSPACE          8   /* Returned in bta_fs_ci_open if no room */
#define WICED_BT_MA_STATUS_DUPLICATE_ID      9
#define WICED_BT_MA_STATUS_ID_NOT_FOUND      10
#define WICED_BT_MA_STATUS_FULL              11  /* reach the max packet size */
#define WICED_BT_MA_STATUS_UNSUP_FEATURES    12  /* feature is not supported on the peer device */

typedef UINT8 wiced_bt_ma_status_t;

#define WICED_BT_MA_OPER_NONE                0
#define WICED_BT_MA_OPER_GET_MSG             1
#define WICED_BT_MA_OPER_PUSH_MSG            2

typedef UINT8 wiced_bt_ma_oper_t;

/* mode field in tBTA_MSE_CO_FOLDER_ENTRY (OR'd together) */
#define WICED_BT_MA_A_RDONLY         1
#define WICED_BT_MA_A_DIR            2      /* Entry is a sub directory */


/* message status indicator */
#define WICED_BT_MA_STS_INDTR_READ       0
#define WICED_BT_MA_STS_INDTR_DELETE     1
typedef UINT8 wiced_bt_ma_sts_indctr_t;

/* message status value */
#define WICED_BT_MA_STS_VALUE_NO         0
#define WICED_BT_MA_STS_VALUE_YES        1
typedef UINT8 wiced_bt_ma_sts_value_t;

/* notification status */
enum
{
    WICED_BT_MA_NOTIF_OFF = 0,
    WICED_BT_MA_NOTIF_ON,
    WICED_BT_MA_NOTIF_MAX

};
typedef UINT8 wiced_bt_ma_notif_status_t;


/* Access response types */
enum
{
    WICED_BT_MA_ACCESS_TYPE_ALLOW = 0,    /* Allow the requested operation */
    WICED_BT_MA_ACCESS_TYPE_FORBID        /* Disallow the requested operation */
};

typedef UINT8 wiced_bt_ma_access_type_t;

/* Structure for in progress related event*/
typedef struct
{
    UINT32                      obj_size;   /* Total size of object 0 if unknow*/
    wiced_bt_ma_sess_handle_t   mas_session_id;
    UINT16                      bytes;      /* Number of bytes read or written since last progress event */
} wiced_bt_ma_in_prog_t;


/* Message type see SDP supported message type */
#define WICED_BT_MA_MSG_TYPE_EMAIL                (1<<0)
#define WICED_BT_MA_MSG_TYPE_SMS_GSM              (1<<1)
#define WICED_BT_MA_MSG_TYPE_SMS_CDMA             (1<<2)
#define WICED_BT_MA_MSG_TYPE_MMS                  (1<<3)

typedef UINT8 wiced_bt_ma_msg_type_t;

#define WICED_BT_MA_MAX_FILTER_TEXT_SIZE 255

/* Message type mask for FilterMessageType in Application parameter */
#define WICED_BT_MA_MSG_TYPE_MASK_SMS_GSM         (1<<0)
#define WICED_BT_MA_MSG_TYPE_MASK_SMS_CDMA        (1<<1)
#define WICED_BT_MA_MSG_TYPE_MASK_EMAIL           (1<<2)
#define WICED_BT_MA_MSG_TYPE_MASK_MMS             (1<<3)

typedef UINT8 wiced_bt_ma_msg_type_mask_t;


/* Parameter Mask for Messages-Listing   */
#define WICED_BT_MA_ML_MASK_SUBJECT               (1<<0)
#define WICED_BT_MA_ML_MASK_DATETIME              (1<<1)
#define WICED_BT_MA_ML_MASK_SENDER_NAME           (1<<2)
#define WICED_BT_MA_ML_MASK_SENDER_ADDRESSING     (1<<3)
#define WICED_BT_MA_ML_MASK_RECIPIENT_NAME        (1<<4)
#define WICED_BT_MA_ML_MASK_RECIPIENT_ADDRESSING  (1<<5)
#define WICED_BT_MA_ML_MASK_TYPE                  (1<<6)
#define WICED_BT_MA_ML_MASK_SIZE                  (1<<7)
#define WICED_BT_MA_ML_MASK_RECEPTION_STATUS      (1<<8)
#define WICED_BT_MA_ML_MASK_TEXT                  (1<<9)
#define WICED_BT_MA_ML_MASK_ATTACHMENT_SIZE       (1<<10)
#define WICED_BT_MA_ML_MASK_PRIORITY              (1<<11)
#define WICED_BT_MA_ML_MASK_READ                  (1<<12)
#define WICED_BT_MA_ML_MASK_SENT                  (1<<13)
#define WICED_BT_MA_ML_MASK_PROTECTED             (1<<14)
#define WICED_BT_MA_ML_MASK_REPLYTO_ADDRESSING    (1<<15)

typedef UINT32 wiced_bt_ma_ml_mask_t;

/* Read status used for  message list */
enum
{
    WICED_BT_MA_READ_STATUS_NO_FILTERING = 0,
    WICED_BT_MA_READ_STATUS_UNREAD       = 1,
    WICED_BT_MA_READ_STATUS_READ         = 2
};
typedef UINT8 wiced_bt_ma_read_status_t;

/* Priority status used for filtering message list */
enum
{
    WICED_BT_MA_PRI_STATUS_NO_FILTERING = 0,
    WICED_BT_MA_PRI_STATUS_HIGH         = 1,
    WICED_BT_MA_PRI_STATUS_NON_HIGH     = 2
};
typedef UINT8 wiced_bt_ma_pri_status_t;

#define WICED_BT_MA_LTIME_LEN 15
typedef struct
{
    wiced_bt_ma_ml_mask_t       parameter_mask;
    UINT16                      max_list_cnt;
    UINT16                      list_start_offset;
    UINT8                       subject_length;  /* valid range 1...255 */
    wiced_bt_ma_msg_type_mask_t msg_mask;
    char                        period_begin[WICED_BT_MA_LTIME_LEN+1]; /* "yyyymmddTHHMMSS", or "" if none */
    char                        period_end[WICED_BT_MA_LTIME_LEN+1]; /* "yyyymmddTHHMMSS", or "" if none */
    wiced_bt_ma_read_status_t   read_status;
    char                        recipient[WICED_BT_MA_MAX_FILTER_TEXT_SIZE+1]; /* "" if none */
    char                        originator[WICED_BT_MA_MAX_FILTER_TEXT_SIZE+1];/* "" if none */
    wiced_bt_ma_pri_status_t    pri_status;
} wiced_bt_ma_msg_list_filter_param_t;

/* enum for charset used in GetMEssage */
enum
{
    WICED_BT_MA_CHARSET_NATIVE = 0,
    WICED_BT_MA_CHARSET_UTF_8  = 1,
    WICED_BT_MA_CHARSET_UNKNOWN,
    WICED_BT_MA_CHARSET_MAX
};
typedef UINT8 wiced_bt_ma_charset_t;

/* enum for fraction request used in GetMEssage */
enum
{
    WICED_BT_MA_FRAC_REQ_FIRST = 0,
    WICED_BT_MA_FRAC_REQ_NEXT  = 1,
    WICED_BT_MA_FRAC_REQ_NO,        /* this is not a fraction request */
    WICED_BT_MA_FRAC_REQ_MAX
};
typedef UINT8 wiced_bt_ma_frac_req_t;

/* enum for fraction delivery used in GetMEssage */
enum
{
    WICED_BT_MA_FRAC_DELIVER_MORE  = 0,
    WICED_BT_MA_FRAC_DELIVER_LAST  = 1,
    WICED_BT_MA_FRAC_DELIVER_NO,          /* this is not a fraction deliver*/
    WICED_BT_MA_FRAC_DELIVER_MAX
};
typedef UINT8 wiced_bt_ma_frac_deliver_t;


typedef struct
{
    BOOLEAN                  attachment;
    wiced_bt_ma_msg_handle_t handle;
    wiced_bt_ma_charset_t    charset;
    wiced_bt_ma_frac_req_t   fraction_request;
} wiced_bt_ma_get_msg_param_t;

#define WICED_BT_MA_RETRY_OFF        0
#define WICED_BT_MA_RETRY_ON         1
#define WICED_BT_MA_RETRY_UNKNOWN    0xff
typedef UINT8   wiced_bt_ma_retry_type_t;


#define WICED_BT_MA_TRANSP_OFF        0
#define WICED_BT_MA_TRANSP_ON         1
#define WICED_BT_MA_TRANSP_UNKNOWN    0xff
typedef UINT8   wiced_bt_ma_transp_type_t;

typedef struct
{
    char                        *p_folder;  /* current or child folder
                                               for current folder set *p_folder = "" */
    uint8_t                     *p_msg;     /* message body */
    uint16_t                    len;
    wiced_bool_t                is_final;
    wiced_bt_ma_transp_type_t   transparent;
    wiced_bt_ma_retry_type_t    retry;
    wiced_bt_ma_charset_t       charset;
} wiced_bt_ma_push_msg_param_t;

/* get or push message multi-packet status */
enum
{
    WICED_BT_MA_MPKT_STATUS_MORE = 0,
    WICED_BT_MA_MPKT_STATUS_LAST,
    WICED_BT_MA_MPKT_MAX
};

typedef UINT8 wiced_bt_ma_mpkt_status_t;

/* definitions for directory navigation */
#define WICED_BT_MA_DIR_NAV_ROOT_OR_DOWN_ONE_LVL     2
#define WICED_BT_MA_DIR_NAV_UP_ONE_LVL               3

typedef UINT8 wiced_bt_ma_dir_nav_t;

enum
{
    WICED_BT_MA_ATTACH_OFF = 0,
    WICED_BT_MA_ATTACH_ON
};
typedef UINT8 wiced_bt_ma_attach_type_t;

/* MAS tag ID in application parameters header definition */
#define WICED_BT_MA_APH_MAX_LIST_COUNT   0x01    /* MaxListCount     2 bytes     0x0000 to 0xFFFF */
#define WICED_BT_MA_APH_START_STOFF      0x02    /* StartOffset      2 bytes     0x0000 to 0xFFFF */
#define WICED_BT_MA_APH_FILTER_MSG_TYPE  0x03    /* SearchAttribute  1 byte      1,2,4 */
#define WICED_BT_MA_APH_FILTER_PRD_BEGIN 0x04    /* Filter Period Begin  variable */
#define WICED_BT_MA_APH_FILTER_PRD_END   0x05    /* Filter Period End    variable */
#define WICED_BT_MA_APH_FILTER_READ_STS  0x06    /* Filter read status 1 byte      0, 1, 2 */
#define WICED_BT_MA_APH_FILTER_RECEIP    0x07    /* FilterRecipient variable */
#define WICED_BT_MA_APH_FILTER_ORIGIN    0x08    /* FilterOriginator variable */
#define WICED_BT_MA_APH_FILTER_PRIORITY  0x09    /* FilterPriority 1 byte */
#define WICED_BT_MA_APH_ATTACH           0x0a    /* Attachment 1 byte */
#define WICED_BT_MA_APH_TRANSPARENT      0x0b    /* transparent 1 byte */
#define WICED_BT_MA_APH_RETRY            0x0c    /* retry 1 byte */
#define WICED_BT_MA_APH_NEW_MSG          0x0d    /* NewMessage 1 byte */
#define WICED_BT_MA_APH_NOTIF_STATUS     0x0e    /* Notification Status 1 byte */
#define WICED_BT_MA_APH_MAS_INST_ID      0x0f    /* MAS instance ID 1 byte  0 ... 255 */
#define WICED_BT_MA_APH_PARAM_MASK       0x10    /* Parameter mask    2 bytes  */
#define WICED_BT_MA_APH_FOLDER_LST_SIZE  0x11    /* Folder Listing Size    2 bytes  */
#define WICED_BT_MA_APH_MSG_LST_SIZE     0x12    /* Message Listing Size    2 bytes  */
#define WICED_BT_MA_APH_SUBJ_LEN         0x13    /* Subject Length    1 byte  */
#define WICED_BT_MA_APH_CHARSET          0x14    /* Character Set    1 byte :0, 1 */
#define WICED_BT_MA_APH_FRAC_REQ         0x15    /* Fraction request    1 byte :0, 1 */
#define WICED_BT_MA_APH_FRAC_DELVR       0x16    /* Fraction delivery    1 byte :0, 1 */
#define WICED_BT_MA_APH_STS_INDCTR       0x17    /* Status Indicator    1 byte  */
#define WICED_BT_MA_APH_STS_VALUE        0x18    /* Status Value    1 byte: 0, 1  */
#define WICED_BT_MA_APH_MSE_TIME         0x19    /* MSETime variable  */
#define WICED_BT_MA_BODY_FILLER_BYTE     0x30    /* Used for SetMessageStatus */

/* MAS type header */
#define WICED_BT_MA_HDR_TYPE_NOTIF_REG       "x-bt/MAP-NotificationRegistration"
#define WICED_BT_MA_HDR_TYPE_MSG_UPDATE      "x-bt/MAP-messageUpdate"
#define WICED_BT_MA_HDR_TYPE_EVENT_RPT       "x-bt/MAP-event-report"
#define WICED_BT_MA_HDR_TYPE_MSG_LIST        "x-bt/MAP-msg-listing"
#define WICED_BT_MA_HDR_TYPE_MSG             "x-bt/message"
#define WICED_BT_MA_HDR_TYPE_MSG_STATUS      "x-bt/messageStatus"
#define WICED_BT_MA_HDR_TYPE_FOLDER_LIST     "x-obex/folder-listing"
#define WICED_BT_MA_HDR_TYPE_MAS_INS_INFO    "x-bt/MASInstanceInformation"

#define WICED_BT_MAS_MESSAGE_ACCESS_TARGET_UUID       "\xBB\x58\x2B\x40\x42\x0C\x11\xDB\xB0\xDE\x08\x00\x20\x0C\x9A\x66"
#define WICED_BT_MAS_MESSAGE_NOTIFICATION_TARGET_UUID "\xBB\x58\x2B\x41\x42\x0C\x11\xDB\xB0\xDE\x08\x00\x20\x0C\x9A\x66"
#define WICED_BT_MAS_UUID_LENGTH                 16

#define WICED_BT_MA_VERSION_1_0          0x0100      /* MAP 1.0 */
#define WICED_BT_MA_VERSION_1_1          0x0101      /* MAP 1.1 */
#define WICED_BT_MA_VERSION_1_2          0x0102      /* MAP 1.2 */


#define WICED_BT_MA_NOTIF_STS_TAG_ID     0x0E
#define WICED_BT_MA_NOTIF_STS_LEN        0x01
#define WICED_BT_MA_NOTIF_STS_ON         0x01
#define WICED_BT_MA_NOTIF_STS_OFF        0x00

#define WICED_BT_MA_NAS_INST_ID_TAG_ID     0x0F
#define WICED_BT_MA_NAS_INST_ID_LEN        0x01

#define WICED_BT_MA_DEFAULT_MAX_LIST_CNT   1024


#define WICED_BT_MA_64BIT_HEX_STR_SIZE               (16+1)
#define WICED_BT_MA_32BIT_HEX_STR_SIZE               (8+1)

#define WICED_BT_MA_MAX_PATH_LEN            32


/*******************************************************************************
**
** bMessage types
**
** Description      The following types are designed to hold data in memory.
**                  The internal structure of these types are implementation
**                  specific.
**
*******************************************************************************/

enum
{
    WICED_BT_MA_BMSG_ENC_8BIT = 0,      /* Used for Email/MMS */

    WICED_BT_MA_BMSG_ENC_G7BIT,         /* Used for GSM-SMS */
    WICED_BT_MA_BMSG_ENC_G7BITEXT,
    WICED_BT_MA_BMSG_ENC_GUCS2,
    WICED_BT_MA_BMSG_ENC_G8BIT,

    WICED_BT_MA_BMSG_ENC_C8BIT,         /* Used for CDMA-SMS */
    WICED_BT_MA_BMSG_ENC_CEPM,
    WICED_BT_MA_BMSG_ENC_C7ASCII,
    WICED_BT_MA_BMSG_ENC_CIA5,
    WICED_BT_MA_BMSG_ENC_CUNICODE,
    WICED_BT_MA_BMSG_ENC_CSJIS,
    WICED_BT_MA_BMSG_ENC_CKOREAN,
    WICED_BT_MA_BMSG_ENC_CLATINHEB,
    WICED_BT_MA_BMSG_ENC_CLATIN,

    WICED_BT_MA_BMSG_ENC_UNKNOWN
};
typedef UINT8 wiced_bt_ma_bmsg_encoding_t;

enum
{
    WICED_BT_MA_BMSG_LANG_UNSPECIFIED = 0,

    WICED_BT_MA_BMSG_LANG_UNKNOWN,
    WICED_BT_MA_BMSG_LANG_SPANISH,      /* GSM-SMS or CDMA-SMS */

    WICED_BT_MA_BMSG_LANG_TURKISH,      /* GSM-SMS only */
    WICED_BT_MA_BMSG_LANG_PORTUGUESE,

    WICED_BT_MA_BMSG_LANG_ENGLISH,      /* CDMA-SMS only */
    WICED_BT_MA_BMSG_LANG_FRENCH,
    WICED_BT_MA_BMSG_LANG_JAPANESE,
    WICED_BT_MA_BMSG_LANG_KOREAN,
    WICED_BT_MA_BMSG_LANG_CHINESE,
    WICED_BT_MA_BMSG_LANG_HEBREW
};
typedef UINT8 wiced_bt_ma_bmsg_language_t;


enum
{
    WICED_BT_MA_VCARD_VERSION_21=0,
    WICED_BT_MA_VCARD_VERSION_30
};
typedef UINT8 wiced_bt_ma_vcard_version_t;

enum
{
    WICED_BT_MA_VCARD_PROP_N,
    WICED_BT_MA_VCARD_PROP_FN,
    WICED_BT_MA_VCARD_PROP_TEL,
    WICED_BT_MA_VCARD_PROP_EMAIL,

    WICED_BT_MA_VCARD_PROP_MAX
};
typedef UINT8 wiced_bt_ma_vcard_prop_t;

typedef struct
{
    char *                      p_param;
    char *                      p_value;

    /* link to the next property (if any) */
    void *                      p_next;

} wiced_bt_ma_vcard_property_t;

typedef struct
{
    wiced_bt_ma_vcard_version_t         vcard_version;

    wiced_bt_ma_vcard_property_t *      p_prop[WICED_BT_MA_VCARD_PROP_MAX];

    /* link to the next vCard (if any) */
    void *                              p_next;

} wiced_bt_ma_bmsg_vcard_t;

typedef struct BMSG_MESSAGE_struct
{
    char *                      p_text;

    /* link to the next chunk of message text (if any) */
    void *                      p_next;

} wiced_bt_ma_bmsg_message_t;

typedef struct BMSG_CONTENT_struct
{
    /* this is the first bit of message text */
    wiced_bt_ma_bmsg_message_t *    p_message;

    /* this points to the last entered text -or-
    ** it is the last that we returned back to
    **
    */
    wiced_bt_ma_bmsg_message_t *    p_last;

    /* link to the next chunk of content (if any) */
    void *                          p_next;

} wiced_bt_ma_bmsg_content_t;


typedef struct
{
    /* Part ID */
    UINT16                          part_id;
    BOOLEAN                         is_multipart;

    /* Properties */
    wiced_bt_ma_bmsg_encoding_t     encoding;
    wiced_bt_ma_bmsg_language_t     language;       /* optional */
    wiced_bt_ma_charset_t           charset;
    /* One or more body content */
    wiced_bt_ma_bmsg_content_t *    p_content;

} wiced_bt_ma_bmsg_body_t;

typedef struct BMSG_ENVELOPE_struct
{
    /* One or more Recipient (vCards) */
    wiced_bt_ma_bmsg_vcard_t *      p_recip;

    /* There will be either another envelope or the body */
    void *                          p_next;
    wiced_bt_ma_bmsg_body_t *       p_body;

} wiced_bt_ma_bmsg_envelope_t;

typedef struct
{
    /* Property values */
    BOOLEAN                         read_sts;
    wiced_bt_ma_msg_type_t          msg_type;
    char *                          p_folder;

    /* One or more Originator (vCards) */
    wiced_bt_ma_bmsg_vcard_t *      p_orig;

    /* Envelope */
    wiced_bt_ma_bmsg_envelope_t *   p_envelope;

} wiced_bt_ma_bmsg_t;


#endif
