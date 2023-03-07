/***************************************************************************//**
 * @file
 * @brief MQTTc Application
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

#if (defined(RTOS_MODULE_NET_MQTT_CLIENT_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/mqtt_client.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/****************************************************************************************************//**
 *                                            Ex_MQTTc_Init()
 *
 * @brief  Initialize the application MQTT-client module.
 *
 * @return  DEF_OK,   if NO error(s),
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
void Ex_MQTT_Client_Init(void)
{
  RTOS_ERR err;

#ifdef EX_MQTT_CLIENT_TASK_STK_SIZE
  MQTTc_ConfigureTaskStk(EX_MQTT_CLIENT_TASK_STK_SIZE, DEF_NULL);
#endif

  MQTTc_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

#ifdef EX_MQTT_CLIENT_TASK_PRIO
  MQTTc_TaskPrioSet(EX_MQTT_CLIENT_TASK_PRIO, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
