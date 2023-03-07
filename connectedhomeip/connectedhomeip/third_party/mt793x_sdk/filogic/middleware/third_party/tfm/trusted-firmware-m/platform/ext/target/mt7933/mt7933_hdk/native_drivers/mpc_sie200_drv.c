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
#include "smpu_drv.h"

#include <stddef.h>

#include "cmsis.h"

#define MPC_SIE200_BLK_CFG_OFFSET  12U

#define MPC_SIE200_CTRL_SEC_RESP      (1UL << 4UL)  /* MPC fault triggers a
                                                     * bus error */
#define MPC_SIE200_CTRL_AUTOINCREMENT (1UL << 8UL)  /* BLK_IDX auto increment */
#define MPC_SIE200_CTRL_SEC_LOCK_DOWN (1UL << 31UL) /* MPC Security lock down */

/* ARM MPC interrupt */
#define MPC_SIE200_INT_EN    1UL
#define MPC_SIE200_INT_STAT  1UL

/* ARM MPC state definitions */
#define MPC_SIE200_INITIALIZED  (1 << 0)

/* Error code returned by the internal driver functions */
enum smpu_intern_error_t{
    MPC_SIE200_INTERN_ERR_NONE = MPC_SIE200_ERR_NONE,
    MPC_SIE200_INTERN_ERR_NOT_IN_RANGE = MPC_SIE200_ERR_NOT_IN_RANGE,
    MPC_SIE200_INTERN_ERR_NOT_ALIGNED = MPC_SIE200_ERR_NOT_ALIGNED,
    MPC_SIE200_INTERN_ERR_INVALID_RANGE = MPC_SIE200_ERR_INVALID_RANGE,
    MPC_INTERN_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE =
                                   MPC_SIE200_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE,
    /* Calculated block index
       is higher than the maximum allowed by the MPC. It should never
       happen unless the controlled ranges of the MPC are misconfigured
       in the driver or if the IP has not enough LUTs to cover the
       range, due to wrong reported block size for example.
    */
    MPC_SIE200_INTERN_ERR_BLK_IDX_TOO_HIGH = -1,

};

/* ARM MPC memory mapped register access structure */
struct smpu_reg_map_t {
    volatile uint32_t ctrl;       /* (R/W) MPC Control */
    volatile uint32_t reserved00[3];
    volatile uint32_t rabn[3];
    volatile uint32_t reserved18;
    volatile uint32_t wabn[3];
    volatile uint32_t reserved28;
    volatile uint32_t apc[16];
    volatile uint32_t bk;
    volatile uint32_t reserved74[3];
    volatile uint32_t cfg[16];

};

enum smpu_error_t smpu_init(struct smpu_dev_t* dev,
                            const struct smpu_memory_range_t** range_list,
                            uint8_t nbr_of_ranges)
{
    if((range_list == NULL) || (nbr_of_ranges == 0)) {
        return MPC_SIE200_INVALID_ARG;
    }

    dev->data->range_list = range_list;
    dev->data->nbr_of_ranges = nbr_of_ranges;
    dev->data->state = MPC_SIE200_INITIALIZED;

    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_get_block_size(struct smpu_dev_t* dev,
                                                  uint32_t* blk_size)
{
    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }

    if(blk_size == 0) {
        return MPC_SIE200_INVALID_ARG;
    }

    /* Calculate the block size in byte according to the manual */
    *blk_size = (1 << (MPC_SIE200_BLK_CFG_OFFSET));

    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_config_region(struct smpu_dev_t* dev,
                                                const uint32_t base,
                                                const uint32_t limit,
                                                enum smpu_sec_attr_t attr)
{
    uint32_t cur = dev->data->cur_region;
    struct smpu_reg_map_t* p_mpc =
                                   (struct smpu_reg_map_t*)dev->cfg->base;

    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }
    if(cur > 15){
        return MPC_SIE200_ERR_NOT_IN_RANGE;
    }
    /* Sanity check to make sure the given range is within this MPCs range */
    if ((dev->data->range_list[cur]->base > base) ||
                    (dev->data->range_list[cur]->limit < limit) ) {
        return MPC_SIE200_ERR_NOT_IN_RANGE;
    }

    /*
     * The memory range should allow accesses in with the wanted security
     * attribute if it requires special attribute for successfull accesses
     */
    if(dev->data->range_list[cur]->attr != attr) {
        return MPC_SIE200_ERR_RANGE_SEC_ATTR_NON_COMPATIBLE;
    }

    if(cur > 15){
        return MPC_SIE200_ERR_NOT_IN_RANGE;
    }
    p_mpc->apc[cur] = attr;


    /*
     * Starts changing actual configuration so issue DMB to ensure every
     * transaction has completed by now
     */
    __DMB();

    /* Commit the configuration change */
    __DSB();
    __ISB();

    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_get_region_config(
                                               struct smpu_dev_t* dev,
                                               uint32_t base, uint32_t limit,
                                               enum smpu_sec_attr_t* attr)
{
    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_get_ctrl(struct smpu_dev_t* dev,
                                            uint32_t* ctrl_val)
{
    struct smpu_reg_map_t* p_mpc =
                                   (struct smpu_reg_map_t*)dev->cfg->base;

    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }

    if(ctrl_val == 0) {
        return MPC_SIE200_INVALID_ARG;
    }

    *ctrl_val = p_mpc->ctrl;

    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_set_ctrl(struct smpu_dev_t* dev,
                                            uint32_t mpc_ctrl)
{
    struct smpu_reg_map_t* p_mpc =
                                   (struct smpu_reg_map_t*)dev->cfg->base;

    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }

    p_mpc->ctrl = mpc_ctrl;

    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_get_sec_resp(struct smpu_dev_t* dev,
                                            enum smpu_sec_resp_t* sec_rep)
{

    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }

    return MPC_SIE200_ERR_NONE;
}

enum smpu_error_t smpu_irq_enable(struct smpu_dev_t* dev)
{
    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }

    return MPC_SIE200_ERR_NONE;
}

void smpu_irq_disable(struct smpu_dev_t* dev)
{
}

void smpu_clear_irq(struct smpu_dev_t* dev)
{
}

uint32_t smpu_irq_state(struct smpu_dev_t* dev)
{
}

enum smpu_error_t smpu_lock_down(struct smpu_dev_t* dev)
{

    if(!(dev->data->state & MPC_SIE200_INITIALIZED)) {
        return MPC_SIE200_NOT_INIT;
    }

    return MPC_SIE200_ERR_NONE;
}
