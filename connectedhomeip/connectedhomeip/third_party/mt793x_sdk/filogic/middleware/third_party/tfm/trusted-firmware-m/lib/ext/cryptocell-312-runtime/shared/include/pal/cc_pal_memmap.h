/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 @addtogroup cc_pal_memmap
 @{
 */

/*!
 @file
 @brief This file contains functions for memory mapping.

       \note None of the described functions validate the input parameters, so
       that the behavior of the APIs in case of an illegal parameter is dependent
       on the behavior of the operating system.
 */

#ifndef _CC_PAL_MEMMAP_H
#define _CC_PAL_MEMMAP_H


#ifdef __cplusplus
extern "C"
{
#endif


#include "cc_pal_types.h"
#include "cc_address_defs.h"


/*----------------------------
      PUBLIC FUNCTIONS
-----------------------------------*/

/*!
  @brief This function returns the base virtual address that maps the base
  physical address.

  @return \c 0 on success.
  @return A non-zero value in case of failure.
 */
uint32_t CC_PalMemMap(
        /*! [in] The starting physical address of the I/O range to be mapped. */
        CCDmaAddr_t physicalAddress,
        /*! [in] The number of bytes that were mapped. */
        uint32_t mapSize,
        /*! [out] A pointer to the base virtual address to which the physical
        pages were mapped. */
        uint32_t **ppVirtBuffAddr);


/*!
  @brief This function unmaps a specified address range that was previously
  mapped by #CC_PalMemMap.

  @return \c 0 on success.
  @return A non-zero value in case of failure.
 */
uint32_t CC_PalMemUnMap(
        /*! [in] A pointer to the base virtual address to which the physical
        pages were mapped. */
        uint32_t *pVirtBuffAddr,
        /*! [in] The number of bytes that were mapped. */
        uint32_t mapSize);

#ifdef __cplusplus
}
#endif

/*!
 @}
 */

#endif
