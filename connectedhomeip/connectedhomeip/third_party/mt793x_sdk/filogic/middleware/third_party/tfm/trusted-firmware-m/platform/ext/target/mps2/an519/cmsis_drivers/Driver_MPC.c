/*
 * Copyright (c) 2016-2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Driver_MPC.h"

#include "cmsis.h"
#include "platform_retarget_dev.h"
#include "RTE_Device.h"

/* driver version */
#define ARM_MPC_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,0)

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_MPC_API_VERSION,
    ARM_MPC_DRV_VERSION
};

static ARM_DRIVER_VERSION ARM_MPC_GetVersion(void)
{
    return DriverVersion;
}

/*
 * \brief Translates error codes from native API to CMSIS API.
 *
 * \param[in] err  Error code to translate (\ref mpc_sie200_error_t).
 *
 * \return Returns CMSIS error code.
 */
static int32_t error_trans(enum mpc_sie200_error_t err)
{
    switch(err) {
    case MPC_SIE200_ERR_NONE:
        return ARM_DRIVER_OK;
    case MPC_SIE200_INVALID_ARG:
        return ARM_DRIVER_ERROR_PARAMETER;
    case MPC_SIE200_NOT_INIT:
        return ARM_MPC_ERR_NOT_INIT;
    case MPC_SIE200_ERR_NOT_IN_RANGE:
        return ARM_MPC_ERR_NOT_IN_RANGE;
    case MPC_SIE200_ERR_NOT_ALIGNED:
        return ARM_MPC_ERR_NOT_ALIGNED;
    case MPC_SIE200_ERR_INVALID_RANGE:
        return ARM_MPC_ERR_INVALID_RANGE;
    case MPC_SIE200_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE:
        return ARM_MPC_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE;
    /* default:  The default is not defined intentionally to force the
     *           compiler to check that all the enumeration values are
     *           covered in the switch.
	 */
    }
}

#if (RTE_ISRAM0_MPC)
/* Ranges controlled by this ISRAM0_MPC */
static struct mpc_sie200_memory_range_t MPC_ISRAM0_RANGE_S = {
    .base  = MPC_ISRAM0_RANGE_BASE_S,
    .limit = MPC_ISRAM0_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_ISRAM0_RANGE_NS = {
    .base  = MPC_ISRAM0_RANGE_BASE_NS,
    .limit = MPC_ISRAM0_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM0_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_ISRAM0_RANGE_LIST[MPC_ISRAM0_RANGE_LIST_LEN]=
    {&MPC_ISRAM0_RANGE_S, &MPC_ISRAM0_RANGE_NS};

/* ISRAM0_MPC Driver wrapper functions */
static int32_t ISRAM0_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_ISRAM0_DEV_S,
                          MPC_ISRAM0_RANGE_LIST,
                          MPC_ISRAM0_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM0_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_ISRAM0_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_ISRAM0_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_ISRAM0_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_ISRAM0_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_ISRAM0_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM0_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_ISRAM0_DEV_S);

    return error_trans(ret);
}

static void ISRAM0_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_ISRAM0_DEV_S);
}


static void ISRAM0_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_ISRAM0_DEV_S);
}

static uint32_t ISRAM0_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_ISRAM0_DEV_S);
}

static int32_t ISRAM0_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_ISRAM0_DEV_S);
}

/* ISRAM0_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_ISRAM0_MPC;
ARM_DRIVER_MPC Driver_ISRAM0_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM0_MPC_Initialize,
    .Uninitialize     = ISRAM0_MPC_Uninitialize,
    .GetBlockSize     = ISRAM0_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM0_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM0_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM0_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM0_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM0_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM0_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM0_MPC_ClearInterrupt,
    .InterruptState   = ISRAM0_MPC_InterruptState,
    .LockDown         = ISRAM0_MPC_LockDown,
};
#endif /* RTE_ISRAM0_MPC */

#if (RTE_ISRAM1_MPC)
/* Ranges controlled by this ISRAM1_MPC */
static struct mpc_sie200_memory_range_t MPC_ISRAM1_RANGE_S = {
    .base  = MPC_ISRAM1_RANGE_BASE_S,
    .limit = MPC_ISRAM1_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_ISRAM1_RANGE_NS = {
    .base  = MPC_ISRAM1_RANGE_BASE_NS,
    .limit = MPC_ISRAM1_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM1_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_ISRAM1_RANGE_LIST[MPC_ISRAM1_RANGE_LIST_LEN]=
    {&MPC_ISRAM1_RANGE_S, &MPC_ISRAM1_RANGE_NS};

/* ISRAM1_MPC Driver wrapper functions */
static int32_t ISRAM1_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_ISRAM1_DEV_S,
                          MPC_ISRAM1_RANGE_LIST,
                          MPC_ISRAM1_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM1_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_ISRAM1_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_ISRAM1_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_ISRAM1_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_ISRAM1_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_ISRAM1_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM1_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_ISRAM1_DEV_S);

    return error_trans(ret);
}

static void ISRAM1_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_ISRAM1_DEV_S);
}


static void ISRAM1_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_ISRAM1_DEV_S);
}

static uint32_t ISRAM1_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_ISRAM1_DEV_S);
}

static int32_t ISRAM1_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_ISRAM1_DEV_S);
}

/* ISRAM1_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_ISRAM1_MPC;
ARM_DRIVER_MPC Driver_ISRAM1_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM1_MPC_Initialize,
    .Uninitialize     = ISRAM1_MPC_Uninitialize,
    .GetBlockSize     = ISRAM1_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM1_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM1_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM1_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM1_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM1_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM1_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM1_MPC_ClearInterrupt,
    .InterruptState   = ISRAM1_MPC_InterruptState,
    .LockDown         = ISRAM1_MPC_LockDown,
};
#endif /* RTE_ISRAM1_MPC */

#if (RTE_ISRAM2_MPC)
/* Ranges controlled by this ISRAM2_MPC */
static struct mpc_sie200_memory_range_t MPC_ISRAM2_RANGE_S = {
    .base  = MPC_ISRAM2_RANGE_BASE_S,
    .limit = MPC_ISRAM2_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_ISRAM2_RANGE_NS = {
    .base  = MPC_ISRAM2_RANGE_BASE_NS,
    .limit = MPC_ISRAM2_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM2_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_ISRAM2_RANGE_LIST[MPC_ISRAM2_RANGE_LIST_LEN]=
    {&MPC_ISRAM2_RANGE_S, &MPC_ISRAM2_RANGE_NS};

/* ISRAM2_MPC Driver wrapper functions */
static int32_t ISRAM2_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_ISRAM2_DEV_S,
                          MPC_ISRAM2_RANGE_LIST,
                          MPC_ISRAM2_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM2_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_ISRAM2_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_ISRAM2_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_ISRAM2_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_ISRAM2_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_ISRAM2_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM2_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_ISRAM2_DEV_S);

    return error_trans(ret);
}

static void ISRAM2_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_ISRAM2_DEV_S);
}


static void ISRAM2_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_ISRAM2_DEV_S);
}

static uint32_t ISRAM2_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_ISRAM2_DEV_S);
}

static int32_t ISRAM2_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_ISRAM2_DEV_S);
}

/* ISRAM2_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_ISRAM2_MPC;
ARM_DRIVER_MPC Driver_ISRAM2_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM2_MPC_Initialize,
    .Uninitialize     = ISRAM2_MPC_Uninitialize,
    .GetBlockSize     = ISRAM2_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM2_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM2_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM2_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM2_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM2_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM2_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM2_MPC_ClearInterrupt,
    .InterruptState   = ISRAM2_MPC_InterruptState,
    .LockDown         = ISRAM2_MPC_LockDown,
};
#endif /* RTE_ISRAM2_MPC */

#if (RTE_ISRAM3_MPC)
/* Ranges controlled by this ISRAM3_MPC */
static struct mpc_sie200_memory_range_t MPC_ISRAM3_RANGE_S = {
    .base  = MPC_ISRAM3_RANGE_BASE_S,
    .limit = MPC_ISRAM3_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_ISRAM3_RANGE_NS = {
    .base  = MPC_ISRAM3_RANGE_BASE_NS,
    .limit = MPC_ISRAM3_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_ISRAM3_RANGE_LIST_LEN  2u
static const struct mpc_sie200_memory_range_t* MPC_ISRAM3_RANGE_LIST[MPC_ISRAM3_RANGE_LIST_LEN]=
    {&MPC_ISRAM3_RANGE_S, &MPC_ISRAM3_RANGE_NS};

/* ISRAM3_MPC Driver wrapper functions */
static int32_t ISRAM3_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_ISRAM3_DEV_S,
                          MPC_ISRAM3_RANGE_LIST,
                          MPC_ISRAM3_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ISRAM3_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_ISRAM3_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_ISRAM3_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_ISRAM3_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_GetRegionConfig(uintptr_t base,
                                          uintptr_t limit,
                                          ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_ISRAM3_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_ConfigRegion(uintptr_t base,
                                       uintptr_t limit,
                                       ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_ISRAM3_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t ISRAM3_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_ISRAM3_DEV_S);

    return error_trans(ret);
}

static void ISRAM3_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_ISRAM3_DEV_S);
}


static void ISRAM3_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_ISRAM3_DEV_S);
}

static uint32_t ISRAM3_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_ISRAM3_DEV_S);
}

static int32_t ISRAM3_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_ISRAM3_DEV_S);
}

/* ISRAM3_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_ISRAM3_MPC;
ARM_DRIVER_MPC Driver_ISRAM3_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = ISRAM3_MPC_Initialize,
    .Uninitialize     = ISRAM3_MPC_Uninitialize,
    .GetBlockSize     = ISRAM3_MPC_GetBlockSize,
    .GetCtrlConfig    = ISRAM3_MPC_GetCtrlConfig,
    .SetCtrlConfig    = ISRAM3_MPC_SetCtrlConfig,
    .ConfigRegion     = ISRAM3_MPC_ConfigRegion,
    .GetRegionConfig  = ISRAM3_MPC_GetRegionConfig,
    .EnableInterrupt  = ISRAM3_MPC_EnableInterrupt,
    .DisableInterrupt = ISRAM3_MPC_DisableInterrupt,
    .ClearInterrupt   = ISRAM3_MPC_ClearInterrupt,
    .InterruptState   = ISRAM3_MPC_InterruptState,
    .LockDown         = ISRAM3_MPC_LockDown,
};
#endif /* RTE_ISRAM3_MPC */

#if (RTE_CODE_SRAM1_MPC)
/* Ranges controlled by this SRAM1_MPC */
static struct mpc_sie200_memory_range_t MPC_CODE_SRAM1_RANGE_S = {
    .base  = MPC_CODE_SRAM1_RANGE_BASE_S,
    .limit = MPC_CODE_SRAM1_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_CODE_SRAM1_RANGE_NS = {
    .base  = MPC_CODE_SRAM1_RANGE_BASE_NS,
    .limit = MPC_CODE_SRAM1_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_CODE_SRAM1_RANGE_LIST_LEN  2u
static const struct  mpc_sie200_memory_range_t* MPC_CODE_SRAM1_RANGE_LIST[MPC_CODE_SRAM1_RANGE_LIST_LEN]=
    {&MPC_CODE_SRAM1_RANGE_S, &MPC_CODE_SRAM1_RANGE_NS};

/* SRAM1_MPC Driver wrapper functions */
static int32_t SRAM1_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_CODE_SRAM1_DEV_S,
                          MPC_CODE_SRAM1_RANGE_LIST,
                          MPC_CODE_SRAM1_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t SRAM1_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t SRAM1_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_CODE_SRAM1_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t SRAM1_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_CODE_SRAM1_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t SRAM1_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_CODE_SRAM1_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t SRAM1_MPC_GetRegionConfig(uintptr_t base,
                                         uintptr_t limit,
                                         ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_CODE_SRAM1_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t SRAM1_MPC_ConfigRegion(uintptr_t base,
                                      uintptr_t limit,
                                      ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_CODE_SRAM1_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t SRAM1_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_CODE_SRAM1_DEV_S);

    return error_trans(ret);
}

static void SRAM1_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_CODE_SRAM1_DEV_S);
}


static void SRAM1_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_CODE_SRAM1_DEV_S);
}

static uint32_t SRAM1_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_CODE_SRAM1_DEV_S);
}

static int32_t SRAM1_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_CODE_SRAM1_DEV_S);
}

/* SRAM1_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_SRAM1_MPC;
ARM_DRIVER_MPC Driver_SRAM1_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = SRAM1_MPC_Initialize,
    .Uninitialize     = SRAM1_MPC_Uninitialize,
    .GetBlockSize     = SRAM1_MPC_GetBlockSize,
    .GetCtrlConfig    = SRAM1_MPC_GetCtrlConfig,
    .SetCtrlConfig    = SRAM1_MPC_SetCtrlConfig,
    .ConfigRegion     = SRAM1_MPC_ConfigRegion,
    .GetRegionConfig  = SRAM1_MPC_GetRegionConfig,
    .EnableInterrupt  = SRAM1_MPC_EnableInterrupt,
    .DisableInterrupt = SRAM1_MPC_DisableInterrupt,
    .ClearInterrupt   = SRAM1_MPC_ClearInterrupt,
    .InterruptState   = SRAM1_MPC_InterruptState,
    .LockDown         = SRAM1_MPC_LockDown,
};
#endif /* RTE_CODE_SRAM1_MPC */

#if (RTE_CODE_SRAM2_MPC)
/* Ranges controlled by this SRAM2_MPC */
static struct mpc_sie200_memory_range_t MPC_CODE_SRAM2_RANGE_S = {
    .base  = MPC_CODE_SRAM2_RANGE_BASE_S,
    .limit = MPC_CODE_SRAM2_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_CODE_SRAM2_RANGE_NS = {
    .base  = MPC_CODE_SRAM2_RANGE_BASE_NS,
    .limit = MPC_CODE_SRAM2_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_CODE_SRAM2_RANGE_LIST_LEN  2u
static const struct  mpc_sie200_memory_range_t* MPC_CODE_SRAM2_RANGE_LIST[MPC_CODE_SRAM2_RANGE_LIST_LEN]=
    {&MPC_CODE_SRAM2_RANGE_S, &MPC_CODE_SRAM2_RANGE_NS};

/* SRAM2_MPC Driver wrapper functions */
static int32_t SRAM2_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_CODE_SRAM2_DEV_S,
                          MPC_CODE_SRAM2_RANGE_LIST,
                          MPC_CODE_SRAM2_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t SRAM2_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t SRAM2_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_CODE_SRAM2_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t SRAM2_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_CODE_SRAM2_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t SRAM2_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_CODE_SRAM2_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t SRAM2_MPC_GetRegionConfig(uintptr_t base,
                                         uintptr_t limit,
                                         ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_CODE_SRAM2_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t SRAM2_MPC_ConfigRegion(uintptr_t base,
                                      uintptr_t limit,
                                      ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_CODE_SRAM2_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t SRAM2_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_CODE_SRAM2_DEV_S);

    return error_trans(ret);
}

static void SRAM2_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_CODE_SRAM2_DEV_S);
}


static void SRAM2_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_CODE_SRAM2_DEV_S);
}

static uint32_t SRAM2_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_CODE_SRAM2_DEV_S);
}

static int32_t SRAM2_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_CODE_SRAM2_DEV_S);
}

/* SRAM2_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_SRAM2_MPC;
ARM_DRIVER_MPC Driver_SRAM2_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = SRAM2_MPC_Initialize,
    .Uninitialize     = SRAM2_MPC_Uninitialize,
    .GetBlockSize     = SRAM2_MPC_GetBlockSize,
    .GetCtrlConfig    = SRAM2_MPC_GetCtrlConfig,
    .SetCtrlConfig    = SRAM2_MPC_SetCtrlConfig,
    .ConfigRegion     = SRAM2_MPC_ConfigRegion,
    .GetRegionConfig  = SRAM2_MPC_GetRegionConfig,
    .EnableInterrupt  = SRAM2_MPC_EnableInterrupt,
    .DisableInterrupt = SRAM2_MPC_DisableInterrupt,
    .ClearInterrupt   = SRAM2_MPC_ClearInterrupt,
    .InterruptState   = SRAM2_MPC_InterruptState,
    .LockDown         = SRAM2_MPC_LockDown,
};
#endif /* RTE_CODE_SRAM2_MPC */

#if (RTE_CODE_SRAM3_MPC)
/* Ranges controlled by this SRAM3_MPC */
static struct mpc_sie200_memory_range_t MPC_CODE_SRAM3_RANGE_S = {
    .base  = MPC_CODE_SRAM3_RANGE_BASE_S,
    .limit = MPC_CODE_SRAM3_RANGE_LIMIT_S,
    .attr  = MPC_SIE200_SEC_ATTR_SECURE
};

static struct mpc_sie200_memory_range_t MPC_CODE_SRAM3_RANGE_NS = {
    .base  = MPC_CODE_SRAM3_RANGE_BASE_NS,
    .limit = MPC_CODE_SRAM3_RANGE_LIMIT_NS,
    .attr  = MPC_SIE200_SEC_ATTR_NONSECURE
};

#define MPC_CODE_SRAM3_RANGE_LIST_LEN  2u
static const struct  mpc_sie200_memory_range_t* MPC_CODE_SRAM3_RANGE_LIST[MPC_CODE_SRAM3_RANGE_LIST_LEN]=
    {&MPC_CODE_SRAM3_RANGE_S, &MPC_CODE_SRAM3_RANGE_NS};

/* SRAM3_MPC Driver wrapper functions */
static int32_t SRAM3_MPC_Initialize(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_init(&MPC_CODE_SRAM3_DEV_S,
                          MPC_CODE_SRAM3_RANGE_LIST,
                          MPC_CODE_SRAM3_RANGE_LIST_LEN);

    return error_trans(ret);
}

static int32_t SRAM3_MPC_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t SRAM3_MPC_GetBlockSize(uint32_t* blk_size)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_block_size(&MPC_CODE_SRAM3_DEV_S, blk_size);

    return error_trans(ret);
}

static int32_t SRAM3_MPC_GetCtrlConfig(uint32_t* ctrl_val)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_ctrl(&MPC_CODE_SRAM3_DEV_S, ctrl_val);

    return error_trans(ret);
}

static int32_t SRAM3_MPC_SetCtrlConfig(uint32_t ctrl)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_set_ctrl(&MPC_CODE_SRAM3_DEV_S, ctrl);

    return error_trans(ret);
}

static int32_t SRAM3_MPC_GetRegionConfig(uintptr_t base,
                                         uintptr_t limit,
                                         ARM_MPC_SEC_ATTR* attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_get_region_config(&MPC_CODE_SRAM3_DEV_S, base, limit,
                                       (enum mpc_sie200_sec_attr_t*)attr);

    return error_trans(ret);
}

static int32_t SRAM3_MPC_ConfigRegion(uintptr_t base,
                                      uintptr_t limit,
                                      ARM_MPC_SEC_ATTR attr)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_config_region(&MPC_CODE_SRAM3_DEV_S, base, limit,
                                   (enum mpc_sie200_sec_attr_t)attr);

    return error_trans(ret);
}

static int32_t SRAM3_MPC_EnableInterrupt(void)
{
    enum mpc_sie200_error_t ret;

    ret = mpc_sie200_irq_enable(&MPC_CODE_SRAM3_DEV_S);

    return error_trans(ret);
}

static void SRAM3_MPC_DisableInterrupt(void)
{
    mpc_sie200_irq_disable(&MPC_CODE_SRAM3_DEV_S);
}


static void SRAM3_MPC_ClearInterrupt(void)
{
    mpc_sie200_clear_irq(&MPC_CODE_SRAM3_DEV_S);
}

static uint32_t SRAM3_MPC_InterruptState(void)
{
    return mpc_sie200_irq_state(&MPC_CODE_SRAM3_DEV_S);
}

static int32_t SRAM3_MPC_LockDown(void)
{
    return mpc_sie200_lock_down(&MPC_CODE_SRAM3_DEV_S);
}

/* SRAM3_MPC Driver CMSIS access structure */
extern ARM_DRIVER_MPC Driver_SRAM3_MPC;
ARM_DRIVER_MPC Driver_SRAM3_MPC = {
    .GetVersion       = ARM_MPC_GetVersion,
    .Initialize       = SRAM3_MPC_Initialize,
    .Uninitialize     = SRAM3_MPC_Uninitialize,
    .GetBlockSize     = SRAM3_MPC_GetBlockSize,
    .GetCtrlConfig    = SRAM3_MPC_GetCtrlConfig,
    .SetCtrlConfig    = SRAM3_MPC_SetCtrlConfig,
    .ConfigRegion     = SRAM3_MPC_ConfigRegion,
    .GetRegionConfig  = SRAM3_MPC_GetRegionConfig,
    .EnableInterrupt  = SRAM3_MPC_EnableInterrupt,
    .DisableInterrupt = SRAM3_MPC_DisableInterrupt,
    .ClearInterrupt   = SRAM3_MPC_ClearInterrupt,
    .InterruptState   = SRAM3_MPC_InterruptState,
    .LockDown         = SRAM3_MPC_LockDown,
};
#endif /* RTE_CODE_SRAM3_MPC */
