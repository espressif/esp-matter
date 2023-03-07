/***************************************************************************//**
 * @file
 * @brief Example - Telnet Server
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_TELNET_SERVER_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/auth.h>
#include  <net/include/telnet_server.h>

#include  <common/include/rtos_prio.h>

#include  <ex_description.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_TELNET_USERNAME
#define  EX_TELNET_USERNAME         "DUT"
#endif

#ifndef  EX_TELNET_PASSWORD
#define  EX_TELNET_PASSWORD         "micrium"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if defined(EX_TELNET_SERVER_SRV_TASK_STK_SIZE) \
  || defined(EX_TELNET_SERVER_SRV_TASK_PRIO)

#ifndef  EX_TELNET_SERVER_SRV_TASK_STK_SIZE
#define  EX_TELNET_SERVER_SRV_TASK_STK_SIZE         TELNET_SERVER_TASK_CFG_STK_SIZE_ELEMENTS_DFLT
#endif

#ifndef EX_TELNET_SERVER_SRV_TASK_PRIO
#define  EX_TELNET_SERVER_SRV_TASK_PRIO             TELNET_SERVER_SRV_TASK_PRIO_DFLT
#endif

RTOS_TASK_CFG Ex_TELNET_Server_SrvTaskCfg = {
  .Prio = EX_TELNET_SERVER_SRV_TASK_PRIO,
  .StkSizeElements = EX_TELNET_SERVER_SRV_TASK_STK_SIZE,
  .StkPtr = DEF_NULL
};

RTOS_TASK_CFG *Ex_TELNET_Server_SrvTaskCfgPtr = &Ex_TELNET_Server_SrvTaskCfg;

#else
RTOS_TASK_CFG *Ex_TELNET_Server_SrvTaskCfgPtr = DEF_NULL;
#endif

#if defined(EX_TELNET_SERVER_SESSION_TASK_STK_SIZE) \
  || defined(EX_TELNET_SERVER_SESSION_TASK_PRIO)

#ifndef  EX_TELNET_SERVER_SESSION_TASK_STK_SIZE
#define  EX_TELNET_SERVER_SESSION_TASK_STK_SIZE         TELNET_SERVER_SESSION_TASK_CFG_STK_SIZE_ELEMENTS_DFLT
#endif

#ifndef EX_TELNET_SERVER_SESSION_TASK_PRIO
#define  EX_TELNET_SERVER_SESSION_TASK_PRIO             TELNET_SERVER_SESSION_TASK_PRIO_DFLT
#endif

RTOS_TASK_CFG Ex_TELNET_Server_SessionTaskCfg = {
  .Prio = EX_TELNET_SERVER_SESSION_TASK_PRIO,
  .StkSizeElements = EX_TELNET_SERVER_SESSION_TASK_STK_SIZE,
  .StkPtr = DEF_NULL
};

RTOS_TASK_CFG *Ex_TELNET_Server_SessionTaskCfgPtr = &Ex_TELNET_Server_SessionTaskCfg;

#else
RTOS_TASK_CFG *Ex_TELNET_Server_SessionTaskCfgPtr = DEF_NULL;
#endif

/****************************************************************************************************//**
 *                                         Ex_TELNET_Server_Init()
 *
 * @brief  Example of initialization of the Telnet server.
 *******************************************************************************************************/
void Ex_TELNET_Server_Init(void)
{
  RTOS_ERR err;

  TELNETs_Init(&err);                                           // Initialize the Telnet server.
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
}

/****************************************************************************************************//**
 *                                    Ex_TELNET_Server_InstanceCreate()
 *
 * @brief  Initializes and starts a default telnet server instance.
 *******************************************************************************************************/
void Ex_TELNET_Server_InstanceCreate(void)
{
  TELNETs_INSTANCE *p_instance;
  RTOS_ERR         err;

  Auth_CreateUser(EX_TELNET_USERNAME, EX_TELNET_PASSWORD, &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  p_instance = TELNETs_InstanceInit(DEF_NULL,
                                    Ex_TELNET_Server_SrvTaskCfgPtr,
                                    Ex_TELNET_Server_SessionTaskCfgPtr,
                                    &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );

  TELNETs_InstanceStart(p_instance, &err);
  APP_RTOS_ASSERT_DBG((err.Code == RTOS_ERR_NONE),; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_TELNET_SERVER_AVAIL
