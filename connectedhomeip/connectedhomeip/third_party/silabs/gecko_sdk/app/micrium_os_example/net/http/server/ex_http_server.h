/***************************************************************************//**
 * @file
 * @brief Example HTTP Server
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

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  EX_HTTP_SERVER_H
#define  EX_HTTP_SERVER_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

extern CPU_CHAR Ex_HTTPs_Name[];

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if  defined(RTOS_MODULE_NET_AVAIL) \
  && defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL)

void Ex_HTTP_Server_Init(void);

void Ex_HTTP_Server_InstanceCreateStaticFS(void);

void Ex_HTTP_Server_InstanceCreateNoFS(void);

void Ex_HTTP_Server_InstanceCreateSecure(void);

#ifdef  RTOS_MODULE_FS_AVAIL
void Ex_HTTP_Server_InstanceCreateBasic(void);
#endif

void Ex_HTTP_Server_InstanceCreateREST(void);

void Ex_HTTP_Server_InstanceCreateCtrlLayer(void);

#endif

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
