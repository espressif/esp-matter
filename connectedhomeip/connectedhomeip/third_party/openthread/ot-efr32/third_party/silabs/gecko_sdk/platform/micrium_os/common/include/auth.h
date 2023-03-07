/***************************************************************************//**
 * @file
 * @brief Common - Authentication
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup COMMON_AUTH Authentication API
 * @ingroup  COMMON
 * @brief      Authentication API
 *
 * @addtogroup COMMON_AUTH
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _AUTH_H_
#define  _AUTH_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  AUTH_RIGHT_NONE         DEF_BIT_NONE
#define  AUTH_RIGHT_0            DEF_BIT_00
#define  AUTH_RIGHT_1            DEF_BIT_01
#define  AUTH_RIGHT_2            DEF_BIT_02
#define  AUTH_RIGHT_3            DEF_BIT_03
#define  AUTH_RIGHT_4            DEF_BIT_04
#define  AUTH_RIGHT_5            DEF_BIT_05
#define  AUTH_RIGHT_6            DEF_BIT_06
#define  AUTH_RIGHT_7            DEF_BIT_07
#define  AUTH_RIGHT_8            DEF_BIT_08
#define  AUTH_RIGHT_9            DEF_BIT_09
#define  AUTH_RIGHT_10           DEF_BIT_10
#define  AUTH_RIGHT_11           DEF_BIT_11
#define  AUTH_RIGHT_12           DEF_BIT_12
#define  AUTH_RIGHT_13           DEF_BIT_13
#define  AUTH_RIGHT_14           DEF_BIT_14
#define  AUTH_RIGHT_15           DEF_BIT_15
#define  AUTH_RIGHT_16           DEF_BIT_16
#define  AUTH_RIGHT_17           DEF_BIT_17
#define  AUTH_RIGHT_18           DEF_BIT_18
#define  AUTH_RIGHT_19           DEF_BIT_19
#define  AUTH_RIGHT_20           DEF_BIT_20
#define  AUTH_RIGHT_21           DEF_BIT_21
#define  AUTH_RIGHT_22           DEF_BIT_22
#define  AUTH_RIGHT_23           DEF_BIT_23
#define  AUTH_RIGHT_24           DEF_BIT_24
#define  AUTH_RIGHT_25           DEF_BIT_25
#define  AUTH_RIGHT_26           DEF_BIT_26
#define  AUTH_RIGHT_27           DEF_BIT_27

#define  AUTH_RIGHT_RSVD_1       DEF_BIT_28
#define  AUTH_RIGHT_RSVD_2       DEF_BIT_29

#define  AUTH_RIGHT_MNG          DEF_BIT_30
#define  AUTH_RIGHT_ROOT         DEF_BIT_31

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef CPU_INT32U AUTH_RIGHT;                                  ///< Auth right is a 32-bit bitmap.

/*
 ********************************************************************************************************
 *                                               USER STRUCTURE
 *******************************************************************************************************/

typedef struct auth_user {                                      // --------------------- AUTH USER --------------------
  CPU_CHAR   *Name;                                             ///< Name of the user.
  AUTH_RIGHT Rights;                                            ///< Rights associated to this user.
} AUTH_USER;

//                                                                 -------------------- AUTH HANDLE -------------------
typedef struct auth_user_handle {
  void *UserObjPtr;
} AUTH_USER_HANDLE;

/****************************************************************************************************//**
 *                                           ROOT CFG STRUCTURE
 * @brief   Root user configuration
 *******************************************************************************************************/

typedef struct auth_cfg_root_user {
  CPU_CHAR *RootUserName;                                       ///< Name of the root user. Must be NULL-terminated.
  CPU_CHAR *RootUserPwd;                                        ///< Pwd for the root user. Must be NULL-terminated.
} AUTH_CFG_ROOT_USER;

/****************************************************************************************************//**
 *                                       RESOURCE CFG STRUCTURE
 * @brief   Resource configuration
 *******************************************************************************************************/

typedef struct auth_cfg_resource {
  CPU_INT08U NbUserMax;                                         ///< Max nb  of user.
  CPU_INT08U NameLenMax;                                        ///< Max len of user name.
  CPU_INT08U PwdLenMax;                                         ///< Max len of user pwd.
} AUTH_CFG_RESOURCE;

/****************************************************************************************************//**
 *                                           INIT CFG STRUCTURE
 * @brief   Initialization configuration
 *******************************************************************************************************/

typedef struct auth_init_cfg {
  AUTH_CFG_ROOT_USER RootUserCfg;                               ///< Root user cfg struct.
  AUTH_CFG_RESOURCE  ResourceCfg;                               ///< Resource usage cfg struct.
  MEM_SEG            *MemSegPtr;                                ///< Ptr to auth mem seg.
} AUTH_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern AUTH_USER_HANDLE Auth_RootUserHandle;

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const AUTH_INIT_CFG Auth_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Auth_ConfigureRootUser(AUTH_CFG_ROOT_USER *p_cfg);

void Auth_ConfigureResource(AUTH_CFG_RESOURCE *p_cfg);

void Auth_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

void Auth_Init(RTOS_ERR *p_err);

AUTH_USER_HANDLE Auth_CreateUser(const CPU_CHAR *p_name,
                                 const CPU_CHAR *p_pwd,
                                 RTOS_ERR       *p_err);

AUTH_USER_HANDLE Auth_GetUser(const CPU_CHAR *p_name,
                              RTOS_ERR       *p_err);

AUTH_RIGHT Auth_GetUserRight(AUTH_USER_HANDLE user_handle,
                             RTOS_ERR         *p_err);

AUTH_USER_HANDLE Auth_ValidateCredentials(const CPU_CHAR *p_name,
                                          const CPU_CHAR *p_pwd,
                                          RTOS_ERR       *p_err);

void Auth_GrantRight(AUTH_RIGHT       right,
                     AUTH_USER_HANDLE user_handle,
                     AUTH_USER_HANDLE as_user_handle,
                     RTOS_ERR         *p_err);

void Auth_RevokeRight(AUTH_RIGHT       right,
                      AUTH_USER_HANDLE user_handle,
                      AUTH_USER_HANDLE as_user_handle,
                      RTOS_ERR         *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of auth module include.
