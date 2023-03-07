/***************************************************************************//**
 * @file
 * @brief This is a simple wrapper for the stdlib memory management functions
 *   like malloc, calloc, realloc and free in order to make them
 *   thread safe.
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

#include "sl_malloc.h"
#include "em_core.h"
#include <stdlib.h>

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib malloc in a critical section.
 *
 * @param[in] size
 *   number of bytes to allocate.
 *
 * @return
 *   Either a pointer to the allocated space or a null pointer.
 ******************************************************************************/
void *sl_malloc(size_t size)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  void *ptr = malloc(size);
  CORE_EXIT_CRITICAL();
  return ptr;
}

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib calloc in a critical section.
 *
 * @param[in] nmemb
 *   number of element to allocate space for.
 *
 * @param[in] size
 *   size of each element.
 *
 * @return
 *   Either a pointer to the allocated space or a null pointer.
 ******************************************************************************/
void *sl_calloc(size_t nmemb, size_t size)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  void *ptr = calloc(nmemb, size);
  CORE_EXIT_CRITICAL();
  return ptr;
}

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib realloc in a critical section.
 *
 * @param[in] ptr
 *   ptr is a pointer to a memory that has previously been allocated. If ptr
 *   is a null pointer then this function behaves like sl_malloc.
 *
 * @param[in] size
 *   number of bytes to allocate.
 *
 * @return
 *   Either a pointer to the allocated space or a null pointer.
 ******************************************************************************/
void *sl_realloc(void * ptr, size_t size)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  void *p = realloc(ptr, size);
  CORE_EXIT_CRITICAL();
  return p;
}

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib free in a critical section.
 *
 * @param[in] ptr
 *   ptr is a pointer to a memory that has previously been allocated.
 ******************************************************************************/
void sl_free(void * ptr)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  free(ptr);
  CORE_EXIT_CRITICAL();
}
