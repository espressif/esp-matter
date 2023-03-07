/*
 * Copyright 2019 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_MEMORY_H_
#define _FSL_MEMORY_H_

#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.memory"
#endif
/* The DSP subsystem local TCM start address, refer to Reference Manual for detailed information */
#define FSL_MEM_DSP_TCM_BEGIN 0x00800000U
/* The DSP subsystem local TCM end address, refer to Reference Manual for detailed information */
#define FSL_MEM_DSP_TCM_END 0x00BFFFFFU

/* The alias start address for CM33 subsystem */
#define FSL_MEM_M33_ALIAS_BEGIN 0x20000000U
/* The alias end address for CM33 subsystem */
#define FSL_MEM_M33_ALIAS_END 0x203FFFFFU
/*
  This alias allows the DSP TCM to be mapped to the CM33 system address space without the need
  to enable and configure SMMU.
  DSP TCM addresses 0x0080_0000 to 0x00BF_FFFF are mapped to the CM33 system address space 0x2000_0000 to 0x203F_FFFF
*/
#define FSL_MEM_M33_ALIAS_OFFSET 0x1F800000U

#if defined(CPU_MIMXRT595SFFOA_dsp)
/* System level TCM memory address = CM33 subsystem local TCM address + FSL_FEATURE_TCM_OFFSET */
#define FSL_MEM_DSP_TCM_OFFSET 0x1F800000U
#else
#error "Device is not supported by this driver!"
#endif

typedef enum _mem_direction
{
    kMEMORY_Local2DMA = 0,
    kMEMORY_DMA2Local,
} mem_direction_t;

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif
/*!
 * @brief Convert the memory map address.
 *
 * This function convert the address between system mapped address and native mapped address.
 * There maybe offset between subsystem native address and system address for some memory,
 * this funciton convert the address to different memory map.
 * @param addr address need to be converted.
 * @param direction convert direction.
 * @return the converted address
 */
static inline uint32_t MEMORY_ConvertMemoryMapAddress(uint32_t addr, mem_direction_t direction)
{
    uint32_t dest;

    switch (direction)
    {
        case kMEMORY_Local2DMA:
        {
            if ((addr >= FSL_MEM_DSP_TCM_BEGIN) && (addr <= FSL_MEM_DSP_TCM_END))
            {
                dest = addr + FSL_MEM_DSP_TCM_OFFSET;
            }
            else if ((addr >= FSL_MEM_M33_ALIAS_BEGIN) && (addr <= FSL_MEM_M33_ALIAS_END))
            {
                dest = addr - FSL_MEM_M33_ALIAS_OFFSET;
            }
            else
            {
                dest = addr;
            }
            break;
        }
        case kMEMORY_DMA2Local:
        {
            if ((addr >= (FSL_MEM_DSP_TCM_BEGIN + FSL_MEM_DSP_TCM_OFFSET)) &&
                (addr <= (FSL_MEM_DSP_TCM_END + FSL_MEM_DSP_TCM_OFFSET)))
            {
                dest = addr - FSL_MEM_DSP_TCM_OFFSET;
            }
            else if (addr <= (FSL_MEM_M33_ALIAS_END - FSL_MEM_M33_ALIAS_OFFSET))
            {
                dest = addr + FSL_MEM_M33_ALIAS_OFFSET;
            }
            else
            {
                dest = addr;
            }
            break;
        }
        default:
            dest = addr;
            break;
    }

    return dest;
}
#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _FSL_MEMORY_H_ */
