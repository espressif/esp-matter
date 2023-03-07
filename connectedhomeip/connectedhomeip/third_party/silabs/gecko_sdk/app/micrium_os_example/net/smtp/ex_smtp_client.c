/***************************************************************************//**
 * @file
 * @brief Example - SMTP Client Application Functions File
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

#if (defined(RTOS_MODULE_NET_SMTP_CLIENT_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/net_app.h>
#include  <net/include/smtp_client.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef EX_SMTPc_SERVER_ADDR
#define EX_SMTPc_SERVER_ADDR        "smtp.isp.com"              // TODO must be modified to specify the SMTP server to be used.
#endif
#ifndef EX_SMTPc_TO_ADDR
#define EX_SMTPc_TO_ADDR            "test_to@gmail.com"         // TODO must be modified to specify the destination address.
#endif

#ifndef EX_SMTPc_FROM_NAME
#define EX_SMTPc_FROM_NAME          "From Name"
#endif

#ifndef EX_SMTPc_FROM_ADDR
#define EX_SMTPc_FROM_ADDR          "test_from@gmail.com"
#endif

#ifndef EX_SMTPc_USERNAME
#define EX_SMTPc_USERNAME           DEF_NULL
#endif

#ifndef EX_SMTPc_PW
#define EX_SMTPc_PW                 DEF_NULL
#endif

#ifndef EX_SMTPc_MSG_SUBJECT
#define EX_SMTPc_MSG_SUBJECT        "Example Title"
#endif

#ifndef EX_SMTPc_MSG_BODY
#define EX_SMTPc_MSG_BODY           "Example email sent using Micrium OS"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_SMTP_Client_Init()
 *
 * @brief  Initialize the Micrium OS SMTP Client module for the example application.
 *******************************************************************************************************/
void Ex_SMTP_Client_Init(void)
{
  RTOS_ERR err;

  //                                                               ------------- INITIALIZE CLIENT SUITE --------------
  SMTPc_Init(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                        Ex_SMTP_Client_SendMail()
 *
 * @brief  Send an email
 *******************************************************************************************************/
void Ex_SMTP_Client_SendMail(void)
{
  SMTPc_MSG *p_msg;
  RTOS_ERR  err;

  p_msg = (SMTPc_MSG *)SMTPc_MsgAlloc(&err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_MsgSetParam(p_msg, SMTPc_FROM_ADDR, EX_SMTPc_FROM_ADDR, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_MsgSetParam(p_msg, SMTPc_FROM_DISPL_NAME, EX_SMTPc_FROM_NAME, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_MsgSetParam(p_msg, SMTPc_TO_ADDR, EX_SMTPc_TO_ADDR, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_MsgSetParam(p_msg, SMTPc_MSG_SUBJECT, EX_SMTPc_MSG_SUBJECT, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_MsgSetParam(p_msg, SMTPc_MSG_BODY, EX_SMTPc_MSG_BODY, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_SendMail(EX_SMTPc_SERVER_ADDR,
                 DEF_NULL,
                 EX_SMTPc_USERNAME,
                 EX_SMTPc_PW,
                 DEF_NULL,
                 p_msg,
                 &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  SMTPc_MsgFree(p_msg, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_SMTP_CLIENT_AVAIL
