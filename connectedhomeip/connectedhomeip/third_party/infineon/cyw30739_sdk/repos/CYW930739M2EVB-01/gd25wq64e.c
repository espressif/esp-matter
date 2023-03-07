/*
 * $ Copyright 2016-YEAR Cypress Semiconductor $
 */

/** @file
 *
 * Utilities for controlling GigaDevice Dual and Quad Serial Flash, GD25WQ64E.
 *
 */

#include "gd25wq64e.h"

#include <wiced.h>
#include <wiced_hal_pspi.h>
#include <wiced_rtos.h>

#include <wiced_bt_trace.h>

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/
/* Target SPI Interface. */
#ifndef GD25WQ64E_SPI_INTERFACE
#define GD25WQ64E_SPI_INTERFACE SPI2
#endif // GD25WQ64E_SPI_INTERFACE

/* SPI Interface Clock Rate. */
#ifndef GD25WQ64E_SPI_CLOCK_RATE
#define GD25WQ64E_SPI_CLOCK_RATE 24000000
#endif // GD25WQ64E_SPI_CLOCK_RATE

/* Delay between commands. */
#ifndef GD25WQ64E_DELAY_BETWEEN_COMMANDS
#define GD25WQ64E_DELAY_BETWEEN_COMMANDS 10 // ms
#endif // GD25WQ64E_DELAY_BETWEEN_COMMANDS

/* Maximum retry counts. */
#ifndef GD25WQ64E_MAX_RETRY_COUNTS
#define GD25WQ64E_MAX_RETRY_COUNTS 10
#endif // GD25WQ64E_MAX_RETRY_COUNTS

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    wiced_bool_t    initialized;
    uint32_t        size;

    struct
    {
        uint8_t     tx[GW25WQ64E_BYTES_PER_PAGE + 4];
        uint8_t     rx[GW25WQ64E_BYTES_PER_PAGE + 4];
    } data;

    /* Identification. */
    struct
    {
        uint8_t manufacture;
        uint8_t memory_type;
        uint8_t capacity;
    } id;

    /* Status Registers. */
    struct
    {
        GD25WQ64E_SR_1_t    sr1;
        GD25WQ64E_SR_2_t    sr2;
        GD25WQ64E_SR_3_t    sr3;
    } status_register;
} gd25wq64e_cb_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/
void            gd25wq64e_chip_erase(void);
uint32_t        gd25wq64e_data_read(uint32_t addr, uint8_t *p_data, uint32_t data_len);
#if 0
uint32_t        gd25wq64e_data_read_fast(uint32_t addr, uint8_t *p_data, uint32_t data_len);
#endif
uint32_t        gd25wq64e_data_write(uint32_t addr, uint8_t *p_data, uint32_t data_len);
void            gd25wq64e_release_from_deep_power_down(void);
void            gd25wq64e_reset(void);
void            gd25wq64e_sector_erase(uint32_t target_addr);
void            gd25wq64e_stand_by_wait(void);
void            gd25wq64e_status_register_read_1(void);
void            gd25wq64e_status_register_read_2(void);
void            gd25wq64e_status_register_read_3(void);
void            gd25wq64e_status_register_write_1(uint8_t value);
void            gd25wq64e_status_register_write_2(uint8_t value);
void            gd25wq64e_status_register_write_3(uint8_t value);
void            gd25wq64e_utils_data_display(uint8_t *p_data, uint32_t data_len);
wiced_bool_t    gd25wq64e_wip_check(void);
void            gd25wq64e_write_disable(void);
void            gd25wq64e_write_enable(void);
void            gd25wq64e_write_enable_volatile_status_register(void);

/******************************************************
 *               Variable Definitions
 ******************************************************/
static gd25wq64e_cb_t gd25wq64e_cb = {0};

/******************************************************
 *               Function Definitions
 ******************************************************/

/**
 *  \brief Initialized the serial flash.
 *
 */
wiced_bool_t wiced_platform_serial_flash_init(void)
{
    uint8_t i;

    if (gd25wq64e_cb.initialized)
    {
        return WICED_TRUE;
    }

    /* Initialize SPI interface. */
    wiced_hal_pspi_reset(GD25WQ64E_SPI_INTERFACE);
    wiced_hal_pspi_init(GD25WQ64E_SPI_INTERFACE, GD25WQ64E_SPI_CLOCK_RATE, SPI_MSB_FIRST, SPI_SS_ACTIVE_LOW, SPI_MODE_0);

    /* Reset flash. */
    gd25wq64e_reset();

    /* Read flash identification. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_READ_IDENTIFICATION;

    i = 0;
    while (1)
    {
        wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, 4, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
        gd25wq64e_cb.id.manufacture = gd25wq64e_cb.data.rx[1];
        gd25wq64e_cb.id.memory_type = gd25wq64e_cb.data.rx[2];
        gd25wq64e_cb.id.capacity    = gd25wq64e_cb.data.rx[3];

        /* Count flash size. */
        gd25wq64e_cb.size = 1;
        for (i = 0 ; i < gd25wq64e_cb.id.capacity ; i++)
        {
            gd25wq64e_cb.size = gd25wq64e_cb.size * 2;
        }

        /* Check flash size. */
        if (gd25wq64e_cb.size == GD25WQ64E_SIZE)
        {
            break;
        }

        i++;
        if (i > GD25WQ64E_MAX_RETRY_COUNTS)
        {
            return WICED_FALSE;
        }

        wiced_rtos_delay_microseconds(GD25WQ64E_DELAY_BETWEEN_COMMANDS);
    }

    gd25wq64e_status_register_read_1();
    gd25wq64e_status_register_read_2();
    gd25wq64e_status_register_read_3();

    /*
     * Set the output driver strength for Read operation.
     *
     * DRV1, DRV0 ---- Driver Strength
     *  0     0        100%
     *  0     1         75%
     *  1     0         50%
     *  1     1         25%
     * */
    gd25wq64e_cb.status_register.sr3.field.drv0 = 1;
    gd25wq64e_cb.status_register.sr3.field.drv1 = 0;
    gd25wq64e_status_register_write_3(gd25wq64e_cb.status_register.sr3.value);
    gd25wq64e_status_register_read_3();

    gd25wq64e_cb.initialized = WICED_TRUE;

    return WICED_TRUE;
}

/**
 * \brief Get the serial flash size.
 *
 * @retval size of the serial flash
 */
uint32_t wiced_platform_serial_flash_size_get(void)
{
    return gd25wq64e_cb.size;
}

/**
 * \brief Read data from serial flash.
 *
 * @param[in] addr      start address
 * @param[in] p_data    buffer to store the read data
 * @param[in] data_len  length of data to be read
 *
 * @retval total length of read data
 */
uint32_t wiced_platform_serial_flash_read(uint32_t addr, uint8_t *p_data, uint32_t data_len)
{
    /* Check state. */
    if (!gd25wq64e_cb.initialized)
    {
        return 0;
    }

    /* Check parameter. */
    if (!p_data)
    {
        return 0;
    }

    /* Check data range. */
    if (addr >= GD25WQ64E_SIZE)
    {
        return 0;
    }

    if ((GD25WQ64E_SIZE - addr) < data_len)
    {
        return 0;
    }

    return gd25wq64e_data_read(addr, p_data, data_len);
}

/**
 * \brief Write data to serial flash.
 *
 * @param[in] addr      start address
 * @param[in] p_data    buffer of the data to be written to the serial flash
 * @param[in] data_len  length of data to be written
 *
 * @retval total length of written data
 */
uint32_t wiced_platform_serial_flash_write(uint32_t addr, uint8_t *p_data, uint32_t data_len)
{
    /* Check state. */
    if (!gd25wq64e_cb.initialized)
    {
        return 0;
    }

    /* Check parameter. */
    if (!p_data)
    {
        return 0;
    }

    /* Check data range. */
    if (addr >= GD25WQ64E_SIZE)
    {
        return 0;
    }

    if ((GD25WQ64E_SIZE - addr) < data_len)
    {
        return 0;
    }

    return gd25wq64e_data_write(addr, p_data, data_len);
}

/**
 * \brief Get the serial flash total sector numbers.
 *
 * @retval total sectors of the serial flash
 */
uint32_t wiced_platform_serial_flash_sector_num_get(void)
{
    return (GW25WQ64E_BLOCKS * GW25WQ64E_SECTORS_PER_BLCOK);
}


/**
 * \brief Erase data in serial flash. (Set both parameters to 0 to erase whole serial flash)
 *
 * @param[in] start_sector  start index of sector to erase
 * @param[in] sector_num    number of sector(s) to be erases
 *
 */
void wiced_platform_serial_flash_erase(uint32_t start_sector, uint32_t sector_num)
{
    uint32_t target_addr;
    uint32_t i;

    /* Check state. */
    if (!gd25wq64e_cb.initialized)
    {
        return;
    }

    /* Check parameter. */
    if (start_sector >= wiced_platform_serial_flash_sector_num_get())
    {
        return;
    }

    if ((wiced_platform_serial_flash_sector_num_get() - start_sector) < sector_num)
    {
        return;
    }

    /* Erase whole chip if both the parameters are not set. */
    if (!start_sector && !sector_num)
    {
        gd25wq64e_chip_erase();
        return;
    }

    /* Erase target sector(s). */
    for (i = 0 ; i < sector_num ; i++)
    {
        /* Count target address. */
        target_addr = (start_sector + i) * (GW25WQ64E_BYTES_PER_PAGE * GW25WQ64E_PAGES_PER_SECTOR);

        /* Erase sector. */
        gd25wq64e_sector_erase(target_addr);
    }
}

/* ----------------- Static Functions ----------------*/
void gd25wq64e_write_enable(void)
{
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_WRITE_ENABLE;

    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);
}

void gd25wq64e_write_disable(void)
{
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_WRITE_DISABLE;

    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);
}

void gd25wq64e_write_enable_volatile_status_register(void)
{
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_VOLATILE_SR_WRITE_ENABLE;

    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);
}

void gd25wq64e_status_register_read_1(void)
{
    /* Read status register 1. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_READ_STATUS_REGISTER_1;

    wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, 2, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
    memcpy((void *) &gd25wq64e_cb.status_register.sr1, (void *) &gd25wq64e_cb.data.rx[1], sizeof(gd25wq64e_cb.status_register.sr1));
}

void gd25wq64e_status_register_read_2(void)
{
    /* Read status register 2. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_READ_STATUS_REGISTER_2;

    wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, 2, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
    memcpy((void *) &gd25wq64e_cb.status_register.sr2, (void *) &gd25wq64e_cb.data.rx[1], sizeof(gd25wq64e_cb.status_register.sr2));
}

void gd25wq64e_status_register_read_3(void)
{
    /* Read status register 3. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_READ_STATUS_REGISTER_3;

    wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, 2, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
    memcpy((void *) &gd25wq64e_cb.status_register.sr3, (void *) &gd25wq64e_cb.data.rx[1], sizeof(gd25wq64e_cb.status_register.sr3));
}

void gd25wq64e_status_register_write_1(uint8_t value)
{
    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return;
    }

    /* Enable write operation. */
    gd25wq64e_write_enable();

    /* Write data to serial flash. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_WRITE_STATUS_REGISTER_1;
    gd25wq64e_cb.data.tx[1] = value;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 2, gd25wq64e_cb.data.tx);

    /* Wait until write process is done. */
    gd25wq64e_stand_by_wait();
}

void gd25wq64e_status_register_write_2(uint8_t value)
{
    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return;
    }

    /* Enable write operation. */
    gd25wq64e_write_enable();

    /* Write data to serial flash. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_WRITE_STATUS_REGISTER_2;
    gd25wq64e_cb.data.tx[1] = value;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 2, gd25wq64e_cb.data.tx);

    /* Wait until write process is done. */
    gd25wq64e_stand_by_wait();
}

void gd25wq64e_status_register_write_3(uint8_t value)
{
    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return;
    }

    /* Enable write operation. */
    gd25wq64e_write_enable();
    //gd25wq64e_write_enable_volatile_status_register();

    /* Write data to serial flash. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_WRITE_STATUS_REGISTER_3;
    gd25wq64e_cb.data.tx[1] = value;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 2, gd25wq64e_cb.data.tx);

    /* Wait until write process is done. */
    gd25wq64e_stand_by_wait();
}

void gd25wq64e_release_from_deep_power_down(void)
{
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_RELEASE_FROME_DEEP_POWER_DOWN;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);

    /* Wait until write process is done. */
    gd25wq64e_stand_by_wait();
}

#if 0
uint32_t gd25wq64e_data_read_fast(uint32_t addr, uint8_t *p_data, uint32_t data_len)
{
    uint32_t read_size;
    uint32_t target_addr;
    uint32_t i;

    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return 0;
    }

    /* Read data from serial flash. */
    read_size = 0;
    target_addr = addr;
    i = 0;
    memset((void *) gd25wq64e_cb.data.tx, 0, GW25WQ64E_BYTES_PER_PAGE + 4);

    gd25wq64e_cb.data.tx[0] = GD25WQ64E_FAST_READ;
    while (read_size < data_len)
    {
        gd25wq64e_cb.data.tx[1] = (uint8_t) (target_addr >> 16);
        gd25wq64e_cb.data.tx[2] = (uint8_t) (target_addr >> 8);
        gd25wq64e_cb.data.tx[3] = (uint8_t) target_addr;
        gd25wq64e_cb.data.tx[4] = 0xff;    // dummy byte

        if ((data_len - read_size) > GW25WQ64E_BYTES_PER_PAGE)
        {
            wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, GW25WQ64E_BYTES_PER_PAGE + 4, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
            memcpy((void *) &p_data[i], (void *) &gd25wq64e_cb.data.rx[5], GW25WQ64E_BYTES_PER_PAGE);
            i += GW25WQ64E_BYTES_PER_PAGE;
            read_size += GW25WQ64E_BYTES_PER_PAGE;
            target_addr += GW25WQ64E_BYTES_PER_PAGE;
        }
        else
        {
            wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, data_len - read_size + 4, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
            memcpy((void *) &p_data[i], (void *) &gd25wq64e_cb.data.rx[5], data_len - read_size);
            i += (data_len - read_size);
            read_size += (data_len - read_size);
            target_addr += (data_len - read_size);
        }
    }

    return read_size;
}
#endif

uint32_t gd25wq64e_data_read(uint32_t addr, uint8_t *p_data, uint32_t data_len)
{
    uint32_t read_size;
    uint32_t target_addr;
    uint32_t i;

    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return 0;
    }

    /* Read data from serial flash. */
    read_size = 0;
    target_addr = addr;
    i = 0;

    while (read_size < data_len)
    {
        memset((void *) gd25wq64e_cb.data.tx, 0, GW25WQ64E_BYTES_PER_PAGE + 4);
        gd25wq64e_cb.data.tx[0] = GD25WQ64E_READ_DATA;
        gd25wq64e_cb.data.tx[1] = (uint8_t) (target_addr >> 16);
        gd25wq64e_cb.data.tx[2] = (uint8_t) (target_addr >> 8);
        gd25wq64e_cb.data.tx[3] = (uint8_t) target_addr;

        if ((data_len - read_size) > GW25WQ64E_BYTES_PER_PAGE)
        {
            wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, GW25WQ64E_BYTES_PER_PAGE + 4, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
            memcpy((void *) &p_data[i], (void *) &gd25wq64e_cb.data.rx[4], GW25WQ64E_BYTES_PER_PAGE);
            i += GW25WQ64E_BYTES_PER_PAGE;
            read_size += GW25WQ64E_BYTES_PER_PAGE;
            target_addr += GW25WQ64E_BYTES_PER_PAGE;
        }
        else
        {
            wiced_hal_pspi_exchange_data(GD25WQ64E_SPI_INTERFACE, data_len - read_size + 4, gd25wq64e_cb.data.tx, gd25wq64e_cb.data.rx);
            memcpy((void *) &p_data[i], (void *) &gd25wq64e_cb.data.rx[4], data_len - read_size);
            i += (data_len - read_size);
            read_size += (data_len - read_size);
            target_addr += (data_len - read_size);
        }
    }

    return read_size;
}

uint32_t gd25wq64e_data_write(uint32_t addr, uint8_t *p_data, uint32_t data_len)
{
    uint32_t written_size = 0;
    uint32_t target_addr = addr;
    uint32_t i = 0;

    while (written_size < data_len)
    {
        /* Check the status. */
        if (!gd25wq64e_wip_check())
        {
            return written_size;
        }

        /* Enable write operation. */
        gd25wq64e_write_enable();

        gd25wq64e_status_register_read_1();

        /* Write data to serial flash. */
        gd25wq64e_cb.data.tx[0] = GD25WQ64E_PAGE_PROGRAM;
        gd25wq64e_cb.data.tx[1] = (uint8_t) (target_addr >> 16);
        gd25wq64e_cb.data.tx[2] = (uint8_t) (target_addr >> 8);
        gd25wq64e_cb.data.tx[3] = (uint8_t) target_addr;

        if ((data_len - written_size) > GW25WQ64E_BYTES_PER_PAGE)
        {
            memcpy((void *) &gd25wq64e_cb.data.tx[4], (void *) &p_data[i], GW25WQ64E_BYTES_PER_PAGE);
            wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, GW25WQ64E_BYTES_PER_PAGE + 4, gd25wq64e_cb.data.tx);
            i += GW25WQ64E_BYTES_PER_PAGE;
            written_size += GW25WQ64E_BYTES_PER_PAGE;
            target_addr += GW25WQ64E_BYTES_PER_PAGE;
        }
        else
        {
            memcpy((void *) &gd25wq64e_cb.data.tx[4], (void *) &p_data[i], data_len - written_size);
            wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, data_len - written_size + 4, gd25wq64e_cb.data.tx);
            i += (data_len - written_size);
            written_size += (data_len - written_size);
            target_addr += (data_len - written_size);

            /* Wait until the write process is done. */
            gd25wq64e_stand_by_wait();
        }
    }

    return written_size;
}

void gd25wq64e_reset(void)
{
    /* Enable reset. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_ENABLE_RESET;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);

    /* Reset. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_RESET;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);
}

wiced_bool_t gd25wq64e_wip_check(void)
{
    uint8_t i = 0;

    while (1)
    {
        gd25wq64e_status_register_read_1();

        if (!gd25wq64e_cb.status_register.sr1.field.wip)
        {
            break;
        }

        i++;

        if (i > GD25WQ64E_MAX_RETRY_COUNTS)
        {
            return WICED_FALSE;
        }

        wiced_rtos_delay_microseconds(GD25WQ64E_DELAY_BETWEEN_COMMANDS);
    }

    return WICED_TRUE;
}

void gd25wq64e_stand_by_wait(void)
{
    while (1)
    {
        gd25wq64e_status_register_read_1();

        if (!gd25wq64e_cb.status_register.sr1.field.wip)
        {
            break;
        }

        wiced_rtos_delay_microseconds(GD25WQ64E_DELAY_BETWEEN_COMMANDS);
    }
}

void gd25wq64e_chip_erase(void)
{
    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return;
    }

    /* Enable write operation. */
    gd25wq64e_write_enable();

    /* Erase chip. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_CHIP_ERASE;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 1, gd25wq64e_cb.data.tx);

    /* Wait until the erase process is done. */
    gd25wq64e_stand_by_wait();
}

void gd25wq64e_sector_erase(uint32_t target_addr)
{
    /* Check the status. */
    if (!gd25wq64e_wip_check())
    {
        return;
    }

    /* Enable write operation. */
    gd25wq64e_write_enable();

    /* Erase sector. */
    gd25wq64e_cb.data.tx[0] = GD25WQ64E_SECTOR_ERASE;
    gd25wq64e_cb.data.tx[1] = (uint8_t) (target_addr >> 16);
    gd25wq64e_cb.data.tx[2] = (uint8_t) (target_addr >> 8);
    gd25wq64e_cb.data.tx[3] = (uint8_t) target_addr;
    wiced_hal_pspi_tx_data(GD25WQ64E_SPI_INTERFACE, 4, gd25wq64e_cb.data.tx);

    /* Wait until the erase process is done. */
    gd25wq64e_stand_by_wait();
}

void gd25wq64e_utils_data_display(uint8_t *p_data, uint32_t data_len)
{
    uint32_t i;

    printf("Data: ");
    for (i = 0 ; i < data_len ; i++)
    {
        if (i % 10 == 0)
        {
            printf("\n");
        }

        printf("0x%02X ", p_data[i]);
    }
    printf("\n");
}
