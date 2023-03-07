/***************************************************************************//**
 * @file
 * @brief Example Network Initialisation
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

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  "core_init/ex_net_core_init.h"

#if (defined(RTOS_MODULE_NET_HTTP_CLIENT_AVAIL) && defined(EX_HTTP_CLIENT_INIT_AVAIL))
#include  "http/client/ex_http_client.h"
#endif
#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL) && defined(EX_HTTP_SERVER_INIT_AVAIL))
#include  "http/server/ex_http_server.h"
#endif
#if (defined(RTOS_MODULE_NET_MQTT_CLIENT_AVAIL) && defined(EX_MQTT_CLIENT_INIT_AVAIL))
#include  "mqtt/ex_mqtt_client.h"
#endif
#if (defined(RTOS_MODULE_NET_TELNET_SERVER_AVAIL) && defined(EX_TELNET_SERVER_INIT_AVAIL))
#include  "telnet/ex_telnet_server.h"
#endif
#if (defined(RTOS_MODULE_NET_IPERF_AVAIL) && defined(EX_IPERF_INIT_AVAIL))
#include  "iperf/ex_iperf.h"
#endif
#if (defined(RTOS_MODULE_NET_SNTP_CLIENT_AVAIL) && defined(EX_SNTP_CLIENT_INIT_AVAIL))
#include  "sntp/ex_sntp_client.h"
#endif
#if (defined(RTOS_MODULE_NET_SMTP_CLIENT_AVAIL) && defined(EX_SMTP_CLIENT_INIT_AVAIL))
#include  "smtp/ex_smtp_client.h"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_NetworkInit()
 *
 * @brief  Initialize the Network Module including the core module and all the network applications
 *         selected.
 *******************************************************************************************************/
void Ex_NetworkInit(void)
{
  //                                                               -------------- INITIALIZE CORE MODULE --------------
  Ex_Net_CoreInit();

  //                                                               ---------- INITIALIZE HTTP CLIENT MODULE -----------
#if (defined(RTOS_MODULE_NET_HTTP_CLIENT_AVAIL) \
  && defined(EX_HTTP_CLIENT_INIT_AVAIL))
  Ex_HTTP_Client_Init();
#endif

  //                                                               ---------- INITIALIZE HTTP SERVER MODULE -----------
#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL) \
  && defined(EX_HTTP_SERVER_INIT_AVAIL))
  Ex_HTTP_Server_Init();
#endif

  //                                                               ---------- INITIALIZE MQTT CLIENT MODULE -----------
#if (defined(RTOS_MODULE_NET_MQTT_CLIENT_AVAIL) \
  && defined(EX_MQTT_CLIENT_INIT_AVAIL))
  Ex_MQTT_Client_Init();
#endif

  //                                                               --------- INITIALIZE TELNET SERVER MODULE ----------
#if (defined(RTOS_MODULE_NET_TELNET_SERVER_AVAIL) \
  && defined(EX_TELNET_SERVER_INIT_AVAIL))
  Ex_TELNET_Server_Init();
#endif

  //                                                               ------------- INITIALIZE IPERF MODULE --------------
#if (defined(RTOS_MODULE_NET_IPERF_AVAIL) \
  && defined(EX_IPERF_INIT_AVAIL))
  Ex_IPerf_Init();
#endif

  //                                                               ---------- INITIALIZE SNTP CLIENT MODULE -----------
#if (defined(RTOS_MODULE_NET_SNTP_CLIENT_AVAIL) \
  && defined(EX_SNTP_CLIENT_INIT_AVAIL))
  Ex_SNTP_Client_Init();
#endif

  //                                                               ---------- INITIALIZE SMTP CLIENT MODULE -----------
#if (defined(RTOS_MODULE_NET_SMTP_CLIENT_AVAIL) \
  && defined(EX_SMTP_CLIENT_INIT_AVAIL))
  Ex_SMTP_Client_Init();
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
