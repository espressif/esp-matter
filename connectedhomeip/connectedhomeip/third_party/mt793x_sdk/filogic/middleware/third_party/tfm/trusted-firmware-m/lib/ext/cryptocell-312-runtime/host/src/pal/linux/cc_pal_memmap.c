/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/************* Include Files ****************/
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "cc_pal_types.h"
#include "cc_pal_memmap.h"

/************************ Defines ******************************/
static int halFileH = -1;

/************************ Enums ******************************/

/************************ Typedefs ******************************/

/************************ Global Data ******************************/

/************************ Private Functions ******************************/

/************************ Public Functions ******************************/

/**
 * @brief This function purpose is to return the base virtual address that maps the
 *        base physical address
 *
 * @param[in] physicalAddress - Starts physical address of the I/O range to be mapped.
 * @param[in] mapSize - Number of bytes that were mapped
 * @param[out] ppVirtBuffAddr - Pointer to the base virtual address to which the physical pages were mapped
 *
 * @return Returns a non-zero value in case of failure
 */
uint32_t CC_PalMemMap(CCDmaAddr_t physicalAddress,
                   uint32_t mapSize,
               uint32_t **ppVirtBuffAddr)
{
        /* Open device file if not already opened */
        if (halFileH >= 0) { /* already opened */
            return 0;
        }

        halFileH = open("/dev/mem", O_RDWR|O_SYNC);
        if (halFileH < 0) {
            return 1;
        }
        (void)fcntl(halFileH, F_SETFD, FD_CLOEXEC);

        *ppVirtBuffAddr = (uint32_t *)mmap(0, mapSize, PROT_READ|PROT_WRITE, MAP_SHARED, halFileH, physicalAddress);
        if ((*ppVirtBuffAddr == NULL) || (*ppVirtBuffAddr == MAP_FAILED)) {
            close(halFileH);
            halFileH = -1;
            return 2;
        }
        return 0;
}/* End of CC_PalMemMap */


/**
 * @brief This function purpose is to Unmaps a specified address range previously mapped
 *        by CC_PalMemMap
 *
 *
 * @param[in] pVirtBuffAddr - Pointer to the base virtual address to which the physical
 *            pages were mapped
 * @param[in] mapSize - Number of bytes that were mapped
 *
 * @return Returns a non-zero value in case of failure
 */
uint32_t CC_PalMemUnMap(uint32_t *pVirtBuffAddr,
                     uint32_t mapSize)
{
        if (halFileH < 0) {
            return 1;
        }

        munmap(pVirtBuffAddr, mapSize);
        close(halFileH);
        halFileH = -1;
        return 0;
}/* End of CC_PalMemUnMap */
