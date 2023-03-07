/***************************************************************************//**
 * @file
 * @brief Network - DNS Client Task Module
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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

#include  <net/include/net_cfg_net.h>

#ifdef  NET_DNS_CLIENT_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "dns_client_task_priv.h"
#include  "dns_client_cache_priv.h"

#include  <net/include/dns_client.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (NET, DNS)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_NET_APP

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern DNSc_DATA *DNSc_DataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if 0
#ifdef  DNSc_TASK_MODULE_EN
static void DNScTask(void *p_arg);
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            DNScTask_Init()
 *
 * @brief    Initialize DNSc task module.
 *
 * @param    p_cfg   Pointer to the DNSc configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function:
 *******************************************************************************************************/
#if 0
void DNScTask_Init(DNSc_DATA    *p_data,
                   CPU_INT32U   task_prio,
                   CPU_STK_SIZE stk_size_elements,
                   void         *p_stk,
                   RTOS_ERR     *p_err)
{
#ifdef  DNSc_TASK_MODULE_EN

  p_data->TaskSignalHandle = KAL_SemCreate("DNSc Task Signal",
                                           DEF_NULL,
                                           p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_data->TaskHandle = KAL_TaskAlloc("DNSc Task",
                                     (CPU_STK *)p_stk,
                                     stk_size_elements,
                                     DEF_NULL,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  KAL_TaskCreate(p_data->TaskHandle,
                 &DNScTask,
                 DEF_NULL,
                 task_prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  goto exit;

exit_release:
  KAL_SemDel(p_data->TaskSignalHandle);

exit:
#endif

  return;
}
#endif

/****************************************************************************************************//**
 *                                        DNScTask_ResolveHost()
 *
 * @brief    Function to submit a host resolution to the task or to perform host resolution.
 *
 * @param    p_host  Pointer to the Host object.
 *
 * @param    flags   Request flag option
 *
 * @param    p_cfg   Request configuration.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Resolution status:
 *               - DNSc_STATUS_PENDING       Host resolution is pending, call again to see the status. (Processed by DNSc's task)
 *               - DNSc_STATUS_RESOLVED      Host is resolved.
 *               - DNSc_STATUS_FAILED        Host resolution has failed.
 *******************************************************************************************************/
DNSc_STATUS DNScTask_HostResolve(DNSc_HOST_OBJ *p_host,
                                 DNSc_FLAGS    flags,
                                 DNSc_REQ_CFG  *p_cfg,
                                 RTOS_ERR      *p_err)
{
#ifdef DNSc_TASK_MODULE_EN
  RTOS_ERR local_err;
#endif
  DNSc_STATUS status = DNSc_STATUS_NONE;

  PP_UNUSED_PARAM(p_cfg);
  PP_UNUSED_PARAM(flags);

  DNScCache_HostInsert(p_host, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

#ifdef DNSc_TASK_MODULE_EN
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  KAL_SemPost(DNSc_DataPtr->TaskSignalHandle, KAL_OPT_POST_NONE, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, DNSc_STATUS_NONE);

#ifdef  DNSc_SIGNAL_TASK_MODULE_EN
  if (DEF_BIT_IS_SET(flags, DNSc_FLAG_NO_BLOCK) == DEF_NO) {
    status = DNSc_STATUS_UNKNOWN;
    KAL_SemPend(p_host->TaskSignal, KAL_OPT_PEND_BLOCKING, 0u, p_err);

    KAL_SemDel(p_host->TaskSignal);

    p_host->TaskSignal = KAL_SemHandleNull;

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      DNScCache_HostRemove(p_host);
      status = DNSc_STATUS_FAILED;
    }
  } else {
    status = DNSc_STATUS_PENDING;
  }
#endif // DNSc_SIGNAL_TASK_MODULE_EN

#else
  status = DNSc_STATUS_PENDING;
  while (status == DNSc_STATUS_PENDING) {
    CPU_INT16U dly = DNSc_DataPtr->CfgOpDly_ms;

    if (p_cfg != DEF_NULL) {
      dly = p_cfg->OpDly_ms;
    }

    status = DNScCache_ResolveHost(p_host, p_err);

    KAL_Dly(dly);
  }

  if (status == DNSc_STATUS_FAILED) {
    DNScCache_HostRemove(p_host);
  }

#endif // DNSc_TASK_MODULE_EN

exit:
  return (status);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                              DNScTask()
 *
 * @brief    DNSc's task.
 *
 * @param    p_arg   Pointer to task argument, should be the DNSc's configuration.
 *******************************************************************************************************/
#if 0
#ifdef  DNSc_TASK_MODULE_EN
static void DNScTask(void *p_arg)
{
  CPU_INT16U nb_req_active = 0u;
  CPU_INT16U nb_req_resolved = 0u;
  KAL_OPT    opt;
  RTOS_ERR   local_err;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    opt = KAL_OPT_PEND_NONE;
    if (nb_req_active > 0u) {
      DEF_BIT_SET(opt, KAL_OPT_PEND_NON_BLOCKING);
    }

    KAL_SemPend(DNSc_DataPtr->TaskSignalHandle, opt, 0, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      nb_req_active++;
    }

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    nb_req_resolved = DNScCache_ResolveAll(&local_err);
    if (nb_req_resolved < nb_req_active) {
      nb_req_active -= nb_req_resolved;
    } else {
      nb_req_active = 0u;
    }

    KAL_Dly(DNSc_DataPtr->CfgOpDly_ms);
  }
}
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_DNS_CLIENT_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
