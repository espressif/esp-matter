/*
 * Copyright (c) 2018-2020 Arm Limited
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
 * \file gfc100_eflash_drv.c
 *
 * \brief Generic driver for GFC100 flash controller
 */

#include "gfc100_eflash_drv.h"
#include "gfc100_process_spec_api.h"

#define BITMASK(width) ((1U<<(width))-1)

/** The MSB Addr[21] selects between memory ranges*/
#define GFC100_EXTENDED_AREA_OFFSET      (1U << 21)

/* Process specific register map offset */
#define GFC100_PROCESS_SPEC_REG_MAP_OFF  0x1000U

#define WORD_ALIGN_16B_MASK              0xFU /* Masks the first 4 bits */
#define WORD_ALIGN_4B_MASK               0x3U /* Masks the first 2 bits */
#define IS_ADDR_4B_ALIGNED(addr) (((uint32_t)addr & WORD_ALIGN_4B_MASK) == 0U)

/** Currently only 128 bit word width is supported by the driver */
#define GFC100_SUPPORTED_WORD_BIT_WIDTH   128U

/** Read is done with maximum word width */
#define BITS_IN_BYTE                      8U
#define GFC100_READ_BYTE_SIZE             \
                               (GFC100_SUPPORTED_WORD_BIT_WIDTH / BITS_IN_BYTE)

/** Write is done in 4 byte chunks */
#define GFC100_WRITE_BYTE_SIZE            4U

/** GFC100 generic controller register map */
struct gfc100_reg_map_t {
    volatile uint32_t irq_enable_set;    /* 0x000 RW Interrupt enable */
    volatile uint32_t irq_enable_clr;    /* 0x004 RW Interrupt disable */
    volatile uint32_t irq_status_set;    /* 0x008 RW Interrupt status set */
    volatile uint32_t irq_status_clr;    /* 0x00C RW Interrupt status clear */
    volatile uint32_t irq_masked_status; /* 0x010 RO Interrupt masked status */
    volatile uint32_t ctrl;              /* 0x014 RW Control */
    volatile uint32_t status;            /* 0x018 RO Status */
    volatile uint32_t addr;              /* 0x01C RW Address */
    volatile uint32_t data0;             /* 0x020 RW Data 0 */
    volatile uint32_t data1;             /* 0x024 RO Data 1 */
    volatile uint32_t data2;             /* 0x028 RO Data 2 */
    volatile uint32_t data3;             /* 0x02C RO Data 3 */
    volatile uint32_t reserved[1000];    /* 0x030 Reserved */
    volatile uint32_t pidr4;             /* 0xFD0 RO Peripheral id register 4 */
    volatile uint32_t reserved2[3];      /* 0xFD4 Reserved */
    volatile uint32_t pidr0;             /* 0xFE0 RO Peripheral id register 0 */
    volatile uint32_t pidr1;             /* 0xFE4 RO Peripheral id register 1 */
    volatile uint32_t pidr2;             /* 0xFE8 RO Peripheral id register 2 */
    volatile uint32_t pidr3;             /* 0xFEC RO Peripheral id register 3 */
    volatile uint32_t cidr0;             /* 0xFF0 RO Component id register 0 */
    volatile uint32_t cidr1;             /* 0xFF4 RO Component id register 1 */
    volatile uint32_t cidr2;             /* 0xFF8 RO Component id register 2 */
    volatile uint32_t cidr3;             /* 0xFFC RO Component id register 3 */
};

/* Bit definition for the interrupt registers */
#define GFC100_CMD_ACCEPT_IRQ_POS       0U
#define GFC100_CMD_ACCEPT_IRQ_MASK      (1U<<GFC100_CMD_ACCEPT_IRQ_POS)
#define GFC100_CMD_SUCCESS_IRQ_POS      1U
#define GFC100_CMD_SUCCESS_IRQ_MASK     (1U<<GFC100_CMD_SUCCESS_IRQ_POS)
#define GFC100_CMD_FAIL_IRQ_POS         2U
#define GFC100_CMD_FAIL_IRQ_MASK        (1U<<GFC100_CMD_FAIL_IRQ_POS)
#define GFC100_CMD_REJECT_IRQ_POS       3U
#define GFC100_CMD_REJECT_IRQ_MASK      (1U<<GFC100_CMD_REJECT_IRQ_POS)
#define GFC100_CMD_OVERFLOW_IRQ_POS     4U
#define GFC100_CMD_OVERFLOW_IRQ_MASK    (1U<<GFC100_CMD_OVERFLOW_IRQ_POS)

#define GFC100_IRQ_MAX_NUMBER           5U
#define GFC100_ALL_IRQ_MASK             BITMASK(GFC100_IRQ_MAX_NUMBER)

/* Bit definitons for the control register */
#define GFC100_CTRL_CMD_POS             0U
#define GFC100_CTRL_CMD_WIDTH           3U
#define GFC100_CTRL_CMD_MASK            \
        (BITMASK(GFC100_CTRL_CMD_WIDTH)<<GFC100_CTRL_CMD_POS)
    #define CMD_READ                    0x1U
    #define CMD_WRITE                   0x2U
    #define CMD_ROW_WRITE               0x3U
    #define CMD_ERASE                   0x4U
    #define CMD_MASS_ERASE              0x7U
#define GFC100_CTRL_CMD_ABORT_POS       4U
#define GFC100_CTRL_CMD_ABORT_WIDTH     1U
#define GFC100_CTRL_CMD_ABORT_MASK      \
        (BITMASK(GFC100_CTRL_CMD_ABORT_WIDTH)<<GFC100_CTRL_CMD_ABORT_POS)

/* Bit definition for the status registers */
#define GFC100_CMD_STAT_PENDING_POS     0U
#define GFC100_CMD_STAT_PENDING_MASK    (1U<<GFC100_CMD_STAT_PENDING_POS)
#define GFC100_CMD_STAT_ACCEPT_POS      1U
#define GFC100_CMD_STAT_ACCEPT_MASK     (1U<<GFC100_CMD_STAT_ACCEPT_POS)
#define GFC100_CMD_STAT_SUCCESS_POS     2U
#define GFC100_CMD_STAT_SUCCESS_MASK    (1U<<GFC100_CMD_STAT_SUCCESS_POS)
#define GFC100_CMD_STAT_FAIL_POS        3U
#define GFC100_CMD_STAT_FAIL_MASK       (1U<<GFC100_CMD_STAT_FAIL_POS)
#define GFC100_CMD_STAT_FINISH_POS      4U
#define GFC100_CMD_STAT_FINISH_MASK     (1U<<GFC100_CMD_STAT_FINISH_POS)
#define GFC100_CMD_STAT_ARB_LOCKED_POS  5U
#define GFC100_CMD_STAT_ARB_LOCKED_MASK (1U<<GFC100_CMD_STAT_ARB_LOCKED_POS)

#define GFC100_CMD_SUCCEEDED_OR_FAILED \
                  (GFC100_CMD_STAT_SUCCESS_MASK | GFC100_CMD_STAT_FAIL_MASK)
#define GFC100_CMD_HAS_FINISHED        \
                  (GFC100_CMD_SUCCEEDED_OR_FAILED | GFC100_CMD_STAT_FINISH_MASK)

/**
 * \brief Enables or disables IRQs
 *
 * \param[in] dev       GFC100 device struct \ref gfc100_eflash_dev_t
 * \param[in] irq_mask  IRQs to enable/disable
 * \param[in] enable    True if the given IRQs need to be enabled, false
 *                      if they need to be disabled
 */
static void gfc100_eflash_irq_enable(struct gfc100_eflash_dev_t *dev,
                                    uint32_t irq_mask, bool enable)
{
    struct gfc100_reg_map_t *reg_map =
                             (struct gfc100_reg_map_t *)dev->cfg->base;

    if (enable) {
        reg_map->irq_enable_set = (irq_mask & GFC100_ALL_IRQ_MASK);
    } else {
        reg_map->irq_enable_clr = (irq_mask & GFC100_ALL_IRQ_MASK);
    }
}

void gfc100_eflash_init(struct gfc100_eflash_dev_t *dev, uint32_t sys_clk)
{
    /* The driver polls the status register of the controller rather
     * than using interrupts, so interrupts need to be disabled.
     */
    gfc100_eflash_irq_enable(dev, GFC100_ALL_IRQ_MASK, false);

    /* Call process specific API to the set timing parameters */
    gfc100_proc_spec_set_eflash_timing(
                   dev->cfg->base + GFC100_PROCESS_SPEC_REG_MAP_OFF, sys_clk);

    /* Store flash size */
    dev->data->flash_size = gfc100_get_eflash_size(dev);

    dev->data->is_initialized = true;
}

/**
 * \brief Clears IRQ status
 *
 * \param[in] reg_map   GFC100 register map struct \ref gfc100_reg_map_t
 */
static inline void clear_irq_status(struct gfc100_reg_map_t *reg_map)
{
    reg_map->irq_status_clr = GFC100_ALL_IRQ_MASK;
}

/**
 * \brief Waits for command ready flags after issuing a command
 *
 * \param[in] reg_map   GFC100 register map struct \ref gfc100_reg_map_t
 *
 * \return Returns status register of the device
 */
static uint32_t check_cmd_result(struct gfc100_reg_map_t *reg_map)
{
    uint32_t status = 0;

    while (!(status = (reg_map->status & GFC100_CMD_HAS_FINISHED))) {};

    if (status & GFC100_CMD_STAT_FINISH_MASK) {
        /* FINISH bit means the FAIL and SUCCESS status cannot be updated
         * as the interrupt status register still holds the status
         * of the previous command.
         * Clearing the interrupts is needed to the get the result
         * of the current command.
         */
        clear_irq_status(reg_map);

        /* Wait for the SUCCESS or FAIL bit to get set */
        while (!(status =
                        (reg_map->status & GFC100_CMD_SUCCEEDED_OR_FAILED))) {};
    }

    return status;
}

/**
 * \brief Triggers read command and blocks until command has finished
 *
 * \param[in] reg_map   GFC100 register map struct \ref gfc100_reg_map_t
 *
 * \return Returns status regster of the device
 */
static uint32_t trigger_read_cmd(struct gfc100_reg_map_t *reg_map,
                                 uint32_t addr)
{
    /* Wait until the previous command is pending */
    while ((reg_map->status & GFC100_CMD_STAT_PENDING_MASK)) {};

    /* Set the address to read from */
    reg_map->addr = addr;

    /* Initiate read command */
    reg_map->ctrl = (CMD_READ << GFC100_CTRL_CMD_POS);

    return (check_cmd_result(reg_map));
}

/**
 * \brief Copies data from GFC100 data registers
 *
 * \param[in]  reg_map  GFC100 register map struct \ref gfc100_reg_map_t
 * \param[out] to_ptr   Pointer to copy the data to
 * \param[in]  size     Number of bytes that needs to be copied
 * \param[in]  offset   Offset to the first byte that needs to be copied
 *
 * \return Returns the number of bytes that were copied
 *
 * \note The function checks the offset and the maximum size the data
 *       registers can hold and adjusts the size with them, so copies only
 *       the number of available bytes
 */
static uint32_t copy_from_data_regs(struct gfc100_reg_map_t *reg_map,
                                    void *to_ptr, uint32_t size,
                                    uint32_t offset)
{
    offset &= (GFC100_READ_BYTE_SIZE - 1);
    uint8_t *dst = (uint8_t *)to_ptr;
    volatile uint8_t *src = ((uint8_t *)&reg_map->data0) + offset;
    uint32_t i;

    /* Read is always done in 16 bytes chunks from a 16 byte aligned address.
     * If address is not aligned then HW will align it, so the SW needs to make
     * sure the right bytes are read from the right location.
     */

    /* Maximum 16 bytes can be copied from the 16 byte aligned address.
     * If the read size with the offset overlaps the 16 byte boundary,
     * only the bytes up to the boundary can be copied. */
    if (size + offset > GFC100_READ_BYTE_SIZE) {
        size = GFC100_READ_BYTE_SIZE - offset;
    }

    /* If the address or the size is not aligned then memcpy can
     * generate unaligned accesses which is not desired
     * on the APB, so instead of using memcpy this for cycle is used.
     */
    for (i=0; i<size; i++) {
        *dst = *src;
        src++;
        dst++;
    }

    return size;
}

enum gfc100_error_t gfc100_eflash_read(struct gfc100_eflash_dev_t *dev,
                                      uint32_t addr, void *data, uint32_t *len)
{
    struct gfc100_reg_map_t *reg_map =
                            (struct gfc100_reg_map_t *)dev->cfg->base;
    uint32_t remaining_len = *len;
    uint32_t status = 0U;
    uint32_t curr_read_len = 0U;
    uint32_t addr_align_off = 0U;

    if (dev->data->is_initialized == false) {
        return GFC100_ERROR_NOT_INITED;
    }

    if ((addr + *len) > dev->data->flash_size) {
        return GFC100_ERROR_OUT_OF_RANGE;
    }

    if (reg_map->status != 0) {
        /* Previous command is still pending,
         * or the arbitration is locked
         */
        return GFC100_ERROR_CMD_PENDING;
    }

    if (gfc100_proc_spec_get_eflash_word_width(dev->cfg->base + GFC100_PROCESS_SPEC_REG_MAP_OFF)
                                           != GFC100_SUPPORTED_WORD_BIT_WIDTH) {
        /* Curently only 128 bit word width is supported by the driver */
        return GFC100_ERROR_INVALID_WORD_WIDTH;
    }

    while (remaining_len && !(status & GFC100_CMD_STAT_FAIL_MASK)) {
        status = trigger_read_cmd(reg_map, addr);

        addr_align_off = addr & WORD_ALIGN_16B_MASK;

        curr_read_len = copy_from_data_regs(reg_map, data, remaining_len,
                                            addr_align_off);
        remaining_len -= curr_read_len;
        addr += curr_read_len;
        data = (void *)((uintptr_t)data + curr_read_len);

        /* Clear IRQ status before issuing the next command */
        clear_irq_status(reg_map);
    }

    if (status & GFC100_CMD_STAT_FAIL_MASK) {
        /* If there was error, then this last read was not successful */
        remaining_len += curr_read_len;
    }

    /* Adjust length to sign how many bytes were actually read */
    *len -= remaining_len;

    return ((status & GFC100_CMD_STAT_FAIL_MASK) ?
                       GFC100_ERROR_CMD_FAIL : GFC100_ERROR_NONE);
}

/**
 * \brief Triggers write command and blocks until command has finished
 *
 * \param[in] reg_map   GFC100 register map struct \ref gfc100_reg_map_t
 * \param[in] addr      Flash address to write to
 * \param[in] data      Data to write to the flash
 *
 * \return Returns status regster of the device
 */
static uint32_t trigger_write_cmd(struct gfc100_reg_map_t *reg_map,
                                  uint32_t addr, uint32_t data)
{
    uint32_t status;

    /* Set address and data to write */
    reg_map->addr = addr;
    reg_map->data0 = data;

    /* Initiate write command */
    reg_map->ctrl = (CMD_WRITE << GFC100_CTRL_CMD_POS);

    status = check_cmd_result(reg_map);

    return status;
}

enum gfc100_error_t gfc100_eflash_write(struct gfc100_eflash_dev_t *dev,
                                        uint32_t addr, const void *data,
                                        uint32_t *len)
{
    struct gfc100_reg_map_t *reg_map =
                                    (struct gfc100_reg_map_t *)dev->cfg->base;
    uint32_t status = 0U;
    uint32_t remaining_len = *len;

    if (dev->data->is_initialized == false) {
        return GFC100_ERROR_NOT_INITED;
    }

    if ((addr + *len) > dev->data->flash_size) {
        return GFC100_ERROR_OUT_OF_RANGE;
    }

    if (reg_map->status != 0) {
        /* Previous command is still pending,
         * or the arbitration is locked
         */
        return GFC100_ERROR_CMD_PENDING;
    }

    if (!IS_ADDR_4B_ALIGNED(addr) || !IS_ADDR_4B_ALIGNED(*len)) {
        /* Both address and length needs to be 4 byte aligned */
        return GFC100_ERROR_UNALIGNED_PARAM;
    }

    while (remaining_len && !(status & GFC100_CMD_STAT_FAIL_MASK)) {
        status = trigger_write_cmd(reg_map, addr, *(uint32_t *)data);
        addr += GFC100_WRITE_BYTE_SIZE;
        data = (void *)((uint32_t)data + GFC100_WRITE_BYTE_SIZE);
        remaining_len -= GFC100_WRITE_BYTE_SIZE;
        /* Clear IRQ status before issuing the next command */
        clear_irq_status(reg_map);
    }

    if (status & GFC100_CMD_STAT_FAIL_MASK) {
        /* If there was error, then this last write was not successful */
        remaining_len += GFC100_WRITE_BYTE_SIZE;
    }

    /* Adjust length to sign how many bytes were actually read */
    *len -= remaining_len;

    return ((status & GFC100_CMD_STAT_FAIL_MASK) ?
                       GFC100_ERROR_CMD_FAIL : GFC100_ERROR_NONE);
}


enum gfc100_error_t gfc100_eflash_row_write(struct gfc100_eflash_dev_t *dev,
                                           uint32_t addr, const void *data,
                                           uint32_t *len)
{
    struct gfc100_reg_map_t *reg_map =
                                    (struct gfc100_reg_map_t *)dev->cfg->base;
    uint32_t status = 0U;
    uint32_t remaining_len = *len;
    uint32_t flag = GFC100_CMD_STAT_ACCEPT_MASK;

    if (dev->data->is_initialized == false) {
        return GFC100_ERROR_NOT_INITED;
    }

    if ((addr + *len) > dev->data->flash_size) {
        return GFC100_ERROR_OUT_OF_RANGE;
    }

    if (reg_map->status != 0U) {
        /* Previous command is still pending,
         * or the arbitration is locked
         */
        return GFC100_ERROR_CMD_PENDING;
    }

    if (!IS_ADDR_4B_ALIGNED(addr) || !IS_ADDR_4B_ALIGNED(*len)) {
        /* Both address and length needs to be 4 byte aligned */
        return GFC100_ERROR_UNALIGNED_PARAM;
    }

    while (remaining_len) {
        /* Set address and data to write */
        reg_map->addr = addr;
        reg_map->data0 = *(uint32_t *)data;

        /* Initiate write command */
        reg_map->ctrl = (CMD_ROW_WRITE << GFC100_CTRL_CMD_POS);

        /* When the first command is sent we only need accept flag to be set.
         * After the first command we need two flags, accept flag from the
         * current, and success flag from the previous command.
         * Note: If interrupts are enabled or execution speed is limited for
         * other reason, there is a chance that the requested row write command
         * is not only accepted but executed already before the ACCEPT flag
         * is checked. If this happens, the code execution will stall.
         */
        while (!(((status = reg_map->status) & flag) == flag)) {
            if (status & GFC100_CMD_STAT_FAIL_MASK) {
                /* Adjust length to sign how many bytes were actually read */
                *len -= remaining_len;
                clear_irq_status(reg_map);
                return GFC100_ERROR_CMD_FAIL;
            }
         }
        flag = (GFC100_CMD_STAT_ACCEPT_MASK | GFC100_CMD_STAT_SUCCESS_MASK);

        /* Adjust data pointers and the length */
        addr += GFC100_WRITE_BYTE_SIZE;
        data = (void *)((uintptr_t)data + GFC100_WRITE_BYTE_SIZE);
        remaining_len -= GFC100_WRITE_BYTE_SIZE;

        /* Clear IRQ status before issuing the next command */
        clear_irq_status(reg_map);
    }

    /* Wait for the success flag of the last command to arrive */
    while (!(reg_map->status & GFC100_CMD_STAT_SUCCESS_MASK)) {};

    /* Adjust length to sign how many bytes were actually read */
    *len -= remaining_len;

    clear_irq_status(reg_map);

    return GFC100_ERROR_NONE;
}

enum gfc100_error_t gfc100_eflash_erase(struct gfc100_eflash_dev_t *dev,
                                       uint32_t addr,
                                       enum gfc100_erase_type_t erase)
{
    struct gfc100_reg_map_t *reg_map =
                                     (struct gfc100_reg_map_t *)dev->cfg->base;
    uint32_t status = 0U;

    if (dev->data->is_initialized == false) {
        return GFC100_ERROR_NOT_INITED;
    }

    if (reg_map->status != 0) {
        /* Previous command is still pending,
         * or the arbitration is locked
         */
        return GFC100_ERROR_CMD_PENDING;
    }

    switch (erase) {
        case GFC100_ERASE_PAGE:
            if (addr > dev->data->flash_size) {
                return GFC100_ERROR_OUT_OF_RANGE;
            }
            reg_map->addr = addr;
            reg_map->ctrl = (CMD_ERASE << GFC100_CTRL_CMD_POS);
            status = check_cmd_result(reg_map);
            /* Clear IRQ status before issuing the next command */
            clear_irq_status(reg_map);
            break;
        case GFC100_MASS_ERASE_MAIN_AREA:
            reg_map->addr = 0U;
            reg_map->ctrl = (CMD_MASS_ERASE << GFC100_CTRL_CMD_POS);
            status = check_cmd_result(reg_map);
            /* Clear IRQ status before issuing the next command */
            clear_irq_status(reg_map);
            break;
        case GFC100_MASS_ERASE_ALL:
            reg_map->addr = GFC100_EXTENDED_AREA_OFFSET;
            reg_map->ctrl = (CMD_MASS_ERASE << GFC100_CTRL_CMD_POS);
            status = check_cmd_result(reg_map);
            /* Clear IRQ status before issuing the next command */
            clear_irq_status(reg_map);
            break;
        default:
            return GFC100_ERROR_INVALID_PARAM;
    }

    return ((status == GFC100_CMD_STAT_FAIL_MASK) ?
                       GFC100_ERROR_CMD_FAIL : GFC100_ERROR_NONE);
}

bool gfc100_is_controller_locked(struct gfc100_eflash_dev_t *dev)
{
    struct gfc100_reg_map_t *reg_map =
                                     (struct gfc100_reg_map_t *)dev->cfg->base;

    return (bool)(reg_map->status & GFC100_CMD_STAT_ARB_LOCKED_MASK);
}

uint32_t gfc100_get_eflash_size(struct gfc100_eflash_dev_t *dev)
{
    return (gfc100_proc_spec_get_eflash_size(
                             dev->cfg->base + GFC100_PROCESS_SPEC_REG_MAP_OFF));
}

uint32_t gfc100_get_eflash_page_size(struct gfc100_eflash_dev_t *dev)
{
    return (gfc100_proc_spec_get_eflash_page_size(
                            dev->cfg->base + GFC100_PROCESS_SPEC_REG_MAP_OFF));
}

uint32_t gfc100_get_num_of_info_pages(struct gfc100_eflash_dev_t *dev)
{
    return (gfc100_proc_spec_get_num_of_info_pages(
                             dev->cfg->base + GFC100_PROCESS_SPEC_REG_MAP_OFF));
}

uint32_t gfc100_get_proc_spec_error(struct gfc100_eflash_dev_t *dev)
{
    return (gfc100_proc_spec_get_error_cause(
                             dev->cfg->base + GFC100_PROCESS_SPEC_REG_MAP_OFF));
}

