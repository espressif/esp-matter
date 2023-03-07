/***************************************************************************//**
 * @file
 * @brief Entropy module to use random number generators as entropy sources
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if defined(RTOS_MODULE_ENTROPY_AVAIL)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <stdio.h>

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/platform_mgr.h>

#include  <entropy/include/entropy.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define RNG_MAX_COUNT   10

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static ENTROPY_RNG_DRV_INFO *entropy_rng_drv_info[RNG_MAX_COUNT] = {NULL};
static CPU_INT32U            entropy_rng_drv_count = 0;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Entropy_Init()
 *
 * @brief    Initialize the Entropy module.
 *
 *******************************************************************************************************/
void Entropy_Init(RTOS_ERR *p_err)
{
  CPU_INT32U i;

  entropy_rng_drv_count = 0;

  for (i = 0; i < RNG_MAX_COUNT; i++) {
    entropy_rng_drv_info[i] = NULL;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                          Entropy_AddSource()
 *
 * @brief    Add an entropy source to be used for random number generation. The entropy source
 *           is typically a bsp driver with the name "rng0", "rng1", etc.
 *
 *******************************************************************************************************/
void Entropy_AddSource(CPU_CHAR *itemName, RTOS_ERR *p_err)
{
  ENTROPY_RNG_PM_ITEM *p_entropy_pm_item = NULL;

  p_entropy_pm_item = (ENTROPY_RNG_PM_ITEM *) PlatformMgrItemGetByName(itemName, p_err);

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (p_entropy_pm_item == NULL || p_entropy_pm_item->BusDrvInfoPtr == NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return;
  }

  p_entropy_pm_item->BusDrvInfoPtr->BSP_API_Ptr->Open();
  entropy_rng_drv_info[entropy_rng_drv_count++] = p_entropy_pm_item->BusDrvInfoPtr;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                          Entropy_Generate()
 *
 * @brief    Generate random data using entropy sources.
 *
 *******************************************************************************************************/
void Entropy_Generate(CPU_INT08U *buf, CPU_INT32U len, RTOS_ERR *p_err)
{
  CPU_INT32U i    = 0;
  CPU_INT32U glen = 0;

  for (i = 0; i < len; i++) {
    buf[i] = 0;
  }

  for (i = 0; i < entropy_rng_drv_count; i++) {
    glen = entropy_rng_drv_info[i]->BSP_API_Ptr->Generate(buf, len);
    if (glen != len) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
      return;
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_ENTROPY_AVAIL
