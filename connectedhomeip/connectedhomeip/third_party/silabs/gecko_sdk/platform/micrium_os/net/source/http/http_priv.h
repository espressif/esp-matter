/***************************************************************************//**
 * @file
 * @brief Network - HTTP
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _HTTP_PRIV_H_
#define  _HTTP_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/http.h"

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       HTTP DEFAUT PORT DEFINES
 *******************************************************************************************************/

#define  HTTP_DFLT_PORT_NBR                    80
#define  HTTP_DFLT_PORT_NBR_SECURE            443

/********************************************************************************************************
 *                                         URL ENCODING DEFINES
 *******************************************************************************************************/

#define  HTTP_URL_ENCODING_LEN                  3
#define  HTTP_URL_ENCODING_JUMP                 2

/********************************************************************************************************
 *                                     HEXADECIMAL STRING DEFINE
 *******************************************************************************************************/

#define  HTTP_INT_16U_HEX_STR_LEN_MAX         4u

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                             CONTENT DISPOSITION TYPE VALUES ENUMARATION
 *******************************************************************************************************/

typedef enum http_content_disposition {
  HTTP_CONTENT_DISPOSITION_FORM_DATA
} HTTP_CONTENT_DISPOSITION;

/********************************************************************************************************
 *                             HEADER FIELD CONNECTION VALUES ENUMARATION
 *******************************************************************************************************/

typedef enum http_hdr_field_conn_val {
  HTTP_HDR_FIELD_CONN_CLOSE,
  HTTP_HDR_FIELD_CONN_PERSISTENT,
  HTTP_HDR_FIELD_CONN_UPGRADE,
  HTTP_HDR_FIELD_CONN_UNKNOWN
} HTTP_HDR_FIELD_CONN_VAL;

/********************************************************************************************************
 *                             HEADER FIELD TRANSFER ENCODING VALUES ENUMARATION
 *******************************************************************************************************/

typedef enum http_hdr_field_transfer_type {
  HTTP_HDR_FIELD_TRANSFER_TYPE_NONE,
  HTTP_HDR_FIELD_TRANSFER_TYPE_CHUNCKED
} HTTP_HDR_FIELD_TRANSFER_TYPE;

/********************************************************************************************************
 *                                  HEADER FIELD UPGRADE VALUES ENUMARATION
 *******************************************************************************************************/

typedef enum http_hdr_field_upgrade_val {
  HTTP_HDR_FIELD_UPGRADE_WEBSOCKET
} HTTP_HDR_FIELD_UPGRADE_VAL;

/********************************************************************************************************
 *                                       FILE TYPES ENUMARATION
 *******************************************************************************************************/

typedef  enum http_file_type {
  HTTP_FILE_TYPE_FS,
  HTTP_FILE_TYPE_STATIC_DATA
} HTTP_FILE_TYPE;

/********************************************************************************************************
 *                                       BODY TYPES ENUMARATION
 *******************************************************************************************************/

typedef  enum http_body_type {
  HTTP_BODY_TYPE_FS,
  HTTP_BODY_TYPE_DATA,
  HTTP_BODY_TYPE_BUF
} HTTP_BODY_TYPE;

/********************************************************************************************************
 *                                     FORM DATA TYPES ENUMARATION
 *******************************************************************************************************/

typedef enum  http_form_data_type {
  HTTP_FORM_DATA_TYPE_CTRL_VAL_QUERY,
  HTTP_FORM_DATA_TYPE_CTRL_VAL_FORM,
  HTTP_FORM_DATA_TYPE_FILE
} HTTP_FORM_DATA_TYPE;

/********************************************************************************************************
 *                                            FORM DATA TYPE
 *******************************************************************************************************/

typedef  struct  http_form_data HTTP_FORM_DATA;

struct  http_form_data {
  HTTP_FORM_DATA_TYPE DataType;
  CPU_CHAR            *CtrlNamePtr;
  CPU_INT16U          CtrlNameLen;
  CPU_CHAR            *ValPtr;
  CPU_INT16U          ValLen;
  HTTP_FORM_DATA      *DataNextPtr;
};

/********************************************************************************************************
 *                                 HTTP HDR FIELD VALUE TYPE DATA TYPE
 *******************************************************************************************************/

typedef  enum  http_hdr_val_type {
  HTTP_HDR_VAL_TYPE_NONE,
  HTTP_HDR_VAL_TYPE_STR_CONST,
  HTTP_HDR_VAL_TYPE_STR_DYN
} HTTP_HDR_VAL_TYPE;

/********************************************************************************************************
 *                                     HTTP HDR FIELD BLK DATA TYPE
 *******************************************************************************************************/

typedef  struct  http_hdr_blk HTTP_HDR_BLK;

struct  http_hdr_blk {
  HTTP_HDR_FIELD    HdrField;
  HTTP_HDR_VAL_TYPE ValType;
  void              *ValPtr;
  CPU_INT32U        ValLen;
  HTTP_HDR_BLK      *NextPtr;
  HTTP_HDR_BLK      *PrevPtr;
};

/********************************************************************************************************
 *                              HTTP FORM MULTIPART CONTENT FIELD DATA TYPE
 *******************************************************************************************************/

typedef enum http_multipart_field {
  HTTP_MULTIPART_FIELD_NAME,
  HTTP_MULTIPART_FIELD_FILE_NAME,
  HTTP_MULTIPART_FIELD_UNKNOWN
} HTTP_MULTIPART_FIELD;

/********************************************************************************************************
 ********************************************************************************************************
 *                                                MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        HTTP COUNTER MACRO'S
 *
 * Description : Functionality to set and increment statistic and error counter
 *
 * Argument(s) : Various HTTP counter variable(s) & values.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : various.
 *
 *               These macro's are INTERNAL HTTP suite macro's & SHOULD NOT be called by
 *               application function(s).
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if ((HTTPs_CFG_CTR_STAT_EN == DEF_ENABLED) \
  || (HTTPc_CFG_CTR_STAT_EN == DEF_ENABLED))
#define  HTTP_SET_PTR_STATS(p_ctr_stats, p_instance)   { \
    p_ctr_stats = &p_instance->StatsCtr;                 \
}

#define  HTTP_STATS_INC(p_ctr)                         { \
    p_ctr ++;                                            \
}

#define  HTTP_STATS_OCTET_INC(p_ctr, octet)            { \
    p_ctr += octet;                                      \
}

#else
//                                                                 Prevent 'variable unused' compiler warning.
#define  HTTP_SET_PTR_STATS(p_ctr_stats, p_instance)   { \
    PP_UNUSED_PARAM(p_ctr_stats);                        \
}

#define  HTTP_STATS_INC(p_ctr)

#define  HTTP_STATS_OCTET_INC(p_ctr, octet)

#endif

#if ((HTTPs_CFG_CTR_ERR_EN == DEF_ENABLED) \
  || (HTTPc_CFG_CTR_ERR_EN == DEF_ENABLED))

#define  HTTP_SET_PTR_ERRS(p_ctr_err, p_instance)      { \
    p_ctr_err = &p_instance->ErrsCtr;                    \
}

#define  HTTP_ERR_INC(p_ctr)                           { \
    p_ctr ++;                                            \
}

#else
//                                                                 Prevent 'variable unused' compiler warning.
#define  HTTP_SET_PTR_ERRS(p_ctr_err, p_instance)      { \
    PP_UNUSED_PARAM(p_ctr_err);                          \
}

#define  HTTP_ERR_INC(p_ctr)

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_CHAR *HTTP_HdrParseFieldValueGet(CPU_CHAR   *p_field,
                                     CPU_INT16U field_len,
                                     CPU_CHAR   *p_field_end,
                                     CPU_INT16U *p_len_rem);

CPU_CHAR *HTTP_StrGraphSrchFirst(CPU_CHAR   *p_str,
                                 CPU_INT32U str_len);

CPU_BOOLEAN HTTP_URL_EncodeStr(const CPU_CHAR *p_str_src,
                               CPU_CHAR       *p_str_dest,
                               CPU_SIZE_T     *p_str_len,
                               CPU_SIZE_T     str_len_max);

CPU_BOOLEAN HTTP_URL_DecodeStr(const CPU_CHAR *p_str_src,
                               CPU_CHAR       *p_str_dest,
                               CPU_SIZE_T     *p_str_lenn);

CPU_BOOLEAN HTTP_URL_DecodeReplaceStr(CPU_CHAR   *p_str,
                                      CPU_INT16U *p_str_len);

CPU_INT16U HTTP_URL_CharEncodeNbr(const CPU_CHAR *p_str_src,
                                  CPU_SIZE_T     str_len);

CPU_CHAR *HTTP_ChunkTransferWrSize(CPU_CHAR   *p_buf_wr,
                                   CPU_SIZE_T buf_len,
                                   CPU_SIZE_T nbr_dig_max,
                                   CPU_INT16U size);

CPU_INT08U HTTP_StrSizeHexDigReq(CPU_INT32U nbr);

HTTP_CONTENT_TYPE HTTP_GetContentTypeFromFileExt(CPU_CHAR   *p_file_path,
                                                 CPU_SIZE_T path_len_max);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_PRIV_H_
