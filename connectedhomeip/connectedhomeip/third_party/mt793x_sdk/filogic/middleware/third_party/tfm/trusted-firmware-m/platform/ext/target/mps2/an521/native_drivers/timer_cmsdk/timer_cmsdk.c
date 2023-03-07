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

/**
 * \file timer_cmsdk.c
 * \brief Generic driver for CMSDK APB Timers.
 *        The timer is a 32-bit down-counter with the following features:
 *        - optional programmable external clock source
 *        - programmable interrupt source, triggered if counter reaches 0
 *        - automatic reload if counter reaches 0
 */

#include "timer_cmsdk.h"

/** Setter bit manipulation macro */
#define SET_BIT(WORD, BIT_INDEX) ((WORD) |= (1U << (BIT_INDEX)))
/** Clearing bit manipulation macro */
#define CLR_BIT(WORD, BIT_INDEX) ((WORD) &= ~(1U << (BIT_INDEX)))
/** Getter bit manipulation macro */
#define GET_BIT(WORD, BIT_INDEX) (bool)(((WORD) & (1U << (BIT_INDEX))))

/**
 * \brief Timer register map structure
 *
 */
struct cmsdk_timer_reg_map_t {
    volatile uint32_t ctrl;    /* Offset: 0x000 (R/W) control register */
    volatile uint32_t value;   /* Offset: 0x004 (R/W) current value register */
    volatile uint32_t reload;  /* Offset: 0x008 (R/W) reload value register */
    union {
        volatile uint32_t intstatus;  /* Offset: 0x00C (R/ ) interrupt
                                       * status register */
        volatile uint32_t intclear;   /* Offset: 0x00C ( /W) interrupt
                                       * clear register */
    }intreg;
};

/**
 * \brief CTRL register bit definitions
 *
 */
enum ctrl_reg_bits_t{
    CTRL_REG_ENUM_ENABLE_INDEX = 0,
    CTRL_REG_ENUM_EXTERNAL_INPUT_ENABLE_INDEX = 1,
    CTRL_REG_ENUM_EXTERNAL_INPUT_CLOCK_INDEX = 2,
    CTRL_REG_ENUM_IRQ_ENABLE_INDEX = 3
};

/**
 * \brief INTSTATUS/INTCLEAR register bit definitions
 *
 */
enum interrupt_reg_bits_t{
    INTERRUPT_REG_ENUM_STATUS_AND_CLEAR_INDEX = 0
};

void cmsdk_timer_init(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;

    if (dev->data->is_initialized == 0) {
        register_map->ctrl = 0;
        register_map->reload = CMSDK_TIMER_DEFAULT_RELOAD;
        dev->data->is_initialized = 1;
    }
}

bool cmsdk_timer_is_initialized(const struct cmsdk_timer_dev_t* dev)
{
    return dev->data->is_initialized;
}

void cmsdk_timer_enable_external_input(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    SET_BIT(register_map->ctrl, CTRL_REG_ENUM_EXTERNAL_INPUT_ENABLE_INDEX);
}

void cmsdk_timer_disable_external_input(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    CLR_BIT(register_map->ctrl, CTRL_REG_ENUM_EXTERNAL_INPUT_ENABLE_INDEX);
}

bool cmsdk_timer_is_external_input_enabled(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return GET_BIT(register_map->ctrl,
                   CTRL_REG_ENUM_EXTERNAL_INPUT_ENABLE_INDEX);
}

void cmsdk_timer_set_clock_to_internal(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    CLR_BIT(register_map->ctrl, CTRL_REG_ENUM_EXTERNAL_INPUT_CLOCK_INDEX);
}

void cmsdk_timer_set_clock_to_external(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    SET_BIT(register_map->ctrl, CTRL_REG_ENUM_EXTERNAL_INPUT_CLOCK_INDEX);
}

bool cmsdk_timer_is_clock_external(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return GET_BIT(register_map->ctrl,
                   CTRL_REG_ENUM_EXTERNAL_INPUT_CLOCK_INDEX);
}

void cmsdk_timer_enable(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    SET_BIT(register_map->ctrl, CTRL_REG_ENUM_ENABLE_INDEX);
}

void cmsdk_timer_disable(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    CLR_BIT(register_map->ctrl, CTRL_REG_ENUM_ENABLE_INDEX);
}

bool cmsdk_timer_is_enabled(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return GET_BIT(register_map->ctrl, CTRL_REG_ENUM_ENABLE_INDEX);
}

void cmsdk_timer_enable_interrupt(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    SET_BIT(register_map->ctrl, CTRL_REG_ENUM_IRQ_ENABLE_INDEX);
}

void cmsdk_timer_disable_interrupt(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    CLR_BIT(register_map->ctrl, CTRL_REG_ENUM_IRQ_ENABLE_INDEX);
}

bool cmsdk_timer_is_interrupt_enabled(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return GET_BIT(register_map->ctrl, CTRL_REG_ENUM_IRQ_ENABLE_INDEX);
}

bool cmsdk_timer_is_interrupt_active(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return GET_BIT(register_map->intreg.intstatus,
                   INTERRUPT_REG_ENUM_STATUS_AND_CLEAR_INDEX);
}

void cmsdk_timer_clear_interrupt(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    SET_BIT(register_map->intreg.intclear,
            INTERRUPT_REG_ENUM_STATUS_AND_CLEAR_INDEX);
}

uint32_t cmsdk_timer_get_current_value(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return register_map->value;
}

void cmsdk_timer_set_reload_value(const struct cmsdk_timer_dev_t* dev,
                                uint32_t reload)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    register_map->reload = reload;
}

void cmsdk_timer_reset(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    register_map->value = register_map->reload;
}

uint32_t cmsdk_timer_get_reload_value(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return register_map->reload;
}

uint32_t cmsdk_timer_get_elapsed_value(const struct cmsdk_timer_dev_t* dev)
{
    struct cmsdk_timer_reg_map_t* register_map =
            (struct cmsdk_timer_reg_map_t*)dev->cfg->base;
    return register_map->reload - register_map->value;
}
