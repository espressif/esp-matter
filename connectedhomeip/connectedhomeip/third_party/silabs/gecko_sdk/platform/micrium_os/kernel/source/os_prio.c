/***************************************************************************//**
 * @file
 * @brief Kernel - Priority Management
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_KERNEL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MICRIUM_SOURCE
#include "../include/os.h"
#include "os_priv.h"

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const CPU_CHAR *os_prio__c = "$Id: $";
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_DATA OSPrioTbl[OS_PRIO_TBL_SIZE];                           // Declare the array local to this file to allow for...
                                                                // ... optimization. This allows the table to be    ...
                                                                // ... located in fast memory.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               OS_PrioInit()
 *
 * @brief    This function is called by the Kernel to initialize the list of ready priorities.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PrioInit(void)
{
  CPU_DATA i;

  //                                                               Clear the bitmap table ... no task is ready.
  for (i = 0u; i < OS_PRIO_TBL_SIZE; i++) {
    OSPrioTbl[i] = 0u;
  }
}

/****************************************************************************************************//**
 *                                           OS_PrioGetHighest()
 *
 * @brief    This function is called by other Kernel services to determine the highest priority task
 *           waiting on the event.
 *
 * @return   The priority of the Highest Priority Task (HPT) waiting for the event.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
OS_PRIO OS_PrioGetHighest(void)
{
#if   (OS_CFG_PRIO_MAX == 8)                                    // Optimize for less than word size nbr of priorities.
  return ((OS_PRIO)CPU_CntLeadZeros08((CPU_INT08U)(OSPrioTbl[0] >> 24u)));
#elif (OS_CFG_PRIO_MAX == 16)                                   // Optimize for less than word size nbr of priorities.
  return ((OS_PRIO)CPU_CntLeadZeros16((CPU_INT16U)(OSPrioTbl[0] >> 16u)));
#elif (OS_CFG_PRIO_MAX <= DEF_INT_CPU_NBR_BITS)                 // Optimize for less than word size nbr of priorities.
  return ((OS_PRIO)CPU_CntLeadZeros(OSPrioTbl[0]));
#elif (OS_CFG_PRIO_MAX <= (2 * DEF_INT_CPU_NBR_BITS))           // Optimize for    2x the word size nbr of priorities.
  if (OSPrioTbl[0] == 0u) {
    return ((OS_PRIO)CPU_CntLeadZeros(OSPrioTbl[1]) + DEF_INT_CPU_NBR_BITS);
  } else {
    return ((OS_PRIO)CPU_CntLeadZeros(OSPrioTbl[0]));
  }
#else
  uint32_t i    = 0u;
  OS_PRIO  prio = 0u;

  while (i < OS_PRIO_TBL_SIZE && OSPrioTbl[i] == 0u) {          // Search the bitmap table for the highest priority.
    prio += DEF_INT_CPU_NBR_BITS;                               // Compute the step of each CPU_DATA entry.
    i++;
  }

  if (i < OS_PRIO_TBL_SIZE) {
    prio += (OS_PRIO)CPU_CntLeadZeros(OSPrioTbl[i]);            // Find the position of the first bit set at the entry.
  }

  return (prio);
#endif
}

/****************************************************************************************************//**
 *                                               OS_PrioInsert()
 *
 * @brief    This function is called to insert a priority in the priority table.
 *
 * @param    prio    The priority to insert.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PrioInsert(OS_PRIO prio)
{
#if   (OS_CFG_PRIO_MAX <= DEF_INT_CPU_NBR_BITS)                 // Optimize for less than word size nbr of priorities.
  OSPrioTbl[0] |= 1u << ((DEF_INT_CPU_NBR_BITS - 1u) - prio);

#elif (OS_CFG_PRIO_MAX <= (2 * DEF_INT_CPU_NBR_BITS))           // Optimize for    2x the word size nbr of priorities.
  if (prio < DEF_INT_CPU_NBR_BITS) {
    OSPrioTbl[0] |= 1u << ((DEF_INT_CPU_NBR_BITS - 1u) - prio);
  } else {
    OSPrioTbl[1] |= 1u << ((DEF_INT_CPU_NBR_BITS - 1u) - (prio - DEF_INT_CPU_NBR_BITS));
  }

#else
  CPU_DATA bit_nbr;
  OS_PRIO  ix;

  ix = prio / DEF_INT_CPU_NBR_BITS;
  bit_nbr = (CPU_DATA) prio & (DEF_INT_CPU_NBR_BITS - 1u);
  OSPrioTbl[ix] |= 1u << ((DEF_INT_CPU_NBR_BITS - 1u) - bit_nbr);
#endif
}

/****************************************************************************************************//**
 *                                               OS_PrioRemove()
 *
 * @brief    This function is called to remove a priority in the priority table.
 *
 * @param    prio    The priority to remove.
 *
 * @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
 *******************************************************************************************************/
void OS_PrioRemove(OS_PRIO prio)
{
#if   (OS_CFG_PRIO_MAX <= DEF_INT_CPU_NBR_BITS)                 // Optimize for less than word size nbr of priorities.
  OSPrioTbl[0] &= ~(1u << ((DEF_INT_CPU_NBR_BITS - 1u) - prio));

#elif (OS_CFG_PRIO_MAX <= (2 * DEF_INT_CPU_NBR_BITS))           // Optimize for    2x the word size nbr of priorities.
  if (prio < DEF_INT_CPU_NBR_BITS) {
    OSPrioTbl[0] &= ~(1u << ((DEF_INT_CPU_NBR_BITS - 1u) - prio));
  } else {
    OSPrioTbl[1] &= ~(1u << ((DEF_INT_CPU_NBR_BITS - 1u) - (prio - DEF_INT_CPU_NBR_BITS)));
  }

#else
  CPU_DATA bit_nbr;
  OS_PRIO  ix;

  ix = prio / DEF_INT_CPU_NBR_BITS;
  bit_nbr = (CPU_DATA) prio & (DEF_INT_CPU_NBR_BITS - 1u);
  OSPrioTbl[ix] &= ~(1u << ((DEF_INT_CPU_NBR_BITS - 1u) - bit_nbr));
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_KERNEL_AVAIL))
