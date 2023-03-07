/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <device/vfs_uart.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <hal/soc/uart.h>
#include <aos/kernel.h>

#include "bl_uart.h"
#include "hal_uart.h"

#include <libfdt.h>

#include <blog.h>

typedef struct uart_priv_data {
    aos_mutex_t    mutex;
} uart_priv_data_t;

static int8_t inited;
static uart_dev_t *dev_uart0 = NULL;
static uart_dev_t *dev_uart1 = NULL;

static int uart_dev_malloc(uart_dev_t **pdev)
{
    if (*pdev) {
        blog_error("arg err.\r\n");
        return -1;
    }

    *pdev = pvPortMalloc(sizeof(uart_dev_t));
    if (*pdev == 0) {
        blog_error("mem err.\r\n");
        return -1;
    }
    memset(*pdev, 0, sizeof(uart_dev_t));

    (*pdev)->read_block_flag = UART_READ_CFG_NOBLOCK;
    (*pdev)->priv = NULL;
    (*pdev)->priv = pvPortMalloc(sizeof(uart_priv_data_t));
    if ((*pdev)->priv == NULL) {
        blog_error("mem err.\r\n");
        return -1;
    }
    memset((*pdev)->priv, 0, sizeof(uart_priv_data_t));

    return 0;
}

static void uart_dev_setdef(uart_dev_t **pdev, uint8_t id)
{
    if (*pdev == NULL) {
        blog_error("mem err.\r\n");
        return;
    }

    (*pdev)->port = id;
    (*pdev)->read_block_flag = UART_READ_CFG_NOBLOCK;
    
    (*pdev)->config.baud_rate = 115200;
    (*pdev)->config.data_width = DATA_WIDTH_8BIT;
    (*pdev)->config.parity = NO_PARITY;
    (*pdev)->config.stop_bits = STOP_BITS_1;
    (*pdev)->config.flow_control = FLOW_CONTROL_DISABLED;
    (*pdev)->config.mode = MODE_TX_RX;
}

static int dev_uart_init(uint8_t id, const char *path, uint32_t rx_buf_size, uint32_t tx_buf_size)
{
    uart_dev_t **pdev = NULL;
    int ret;

    if ((id >= 3) || (path == 0)) {
        blog_error("arg err.\r\n");
        return -1;
    }

    switch (id) {
        case 0:
        {
            pdev = &dev_uart0;
        } break;
        case 1:
        {
            pdev = &dev_uart1;
        } break;
        default:
        {
            blog_error("err.\r\n");
            return -1;
        } break;
    }

    if (uart_dev_malloc(pdev) != 0) {
        return -1;
    }
    (*pdev)->rx_buf_size = rx_buf_size;
    (*pdev)->tx_buf_size = tx_buf_size;
   // (*pdev)->ring_rx_buffer = pvPortMalloc((*pdev)->rx_buf_size);
   // (*pdev)->ring_tx_buffer = pvPortMalloc((*pdev)->tx_buf_size);

   // if ((*pdev)->ring_rx_buffe == NULL || (*pdev)->ring_rx_buffe == NULL ) {
   //     return -1;
   // }

    uart_dev_setdef(pdev, id);
    ret = aos_register_driver(path, &uart_ops, *pdev);
    if (ret != VFS_SUCCESS) {
        return ret;
    }

    return 0;
}

int32_t hal_uart_send_trigger(uart_dev_t *uart)
{
    bl_uart_int_tx_enable(uart->port);
    return 0;
}

int32_t hal_uart_send_trigger_off(uart_dev_t *uart)
{
    bl_uart_int_tx_disable(uart->port);
    return 0;
}

int32_t hal_uart_init(uart_dev_t *uart)
{
    uart_priv_data_t *data;
    uint8_t parity;

    data = uart->priv;
    if (aos_mutex_new(&(data->mutex))) {
        /*we should assert here?*/
        return -1;
    }

    bl_uart_getdefconfig(uart->port, &parity);

    if (parity == UART_PARITY_NONE) {
        uart->config.parity = NO_PARITY;
    } else if (parity == UART_PARITY_ODD) {
        uart->config.parity = ODD_PARITY;
    } else {
        uart->config.parity = EVEN_PARITY;
    }

    bl_uart_int_enable(uart->port);

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t *recv_size, uint32_t timeout)
{
    int ch;
    uint32_t counter = 0;

    while (counter < expect_size && (ch = bl_uart_data_recv(uart->port)) >= 0) {
        ((uint8_t*)data)[counter] = ch;
        counter++;
    }

    *recv_size = counter;
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    uint32_t i = 0;

    while (i < size) {
        bl_uart_data_send(uart->port, ((uint8_t*)data)[i]);
        i++;
    }
    return 0;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    uart_priv_data_t *data;

    data = uart->priv;
    bl_uart_int_disable(uart->port);
    aos_mutex_free(&(data->mutex));
    return 0;
}

/*TODO better glue for ring buffer?*/
int32_t hal_uart_notify_register(uart_dev_t *uart, hal_uart_int_t type, void (*cb)(void *arg))
{
    if (type == UART_TX_INT) {
        bl_uart_int_tx_notify_register(uart->port, cb, uart);
    } else if (type == UART_RX_INT) {
        bl_uart_int_rx_notify_register(uart->port, cb, uart);
    } else {
        return -1;
    }

    return 0;
}

int32_t hal_uart_notify_unregister(uart_dev_t *uart, hal_uart_int_t type, void (*cb)(void *arg))
{
    if (type == UART_TX_INT) {
        bl_uart_int_tx_notify_unregister(uart->port, cb, uart);
    } else if (type == UART_RX_INT) {
        bl_uart_int_rx_notify_unregister(uart->port, cb, uart);
    } else {
        return -1;
    }

    return 0;
}

#define BL_FDT32_TO_U8(addr, byte_offset)   ((uint8_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U16(addr, byte_offset)  ((uint16_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U32(addr, byte_offset)  ((uint32_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))

static void fdt_uart_module_init(const void *fdt, int uart_offset)
{
    #define UART_MODULE_MAX 2
    #define PIN_INVALID_VALUE 0XFF

    int offset1 = 0;
    int offset2 = 0;

    const uint32_t *addr_prop = 0;
    int lentmp = 0;
    const char *result = 0;
    int countindex = 0;
    int i, j;
    uint32_t rx_buf_size, tx_buf_size;

    uint8_t id;
    char *path = NULL;
    uint32_t baudrate;

    const char *uart_node[UART_MODULE_MAX] = {
        "uart@4000A000",
        "uart@4000A100",
    };

    struct _feature_pin {
        char *featue_name;
        char *pin_name;
        uint8_t value;
    } feature_pin[4] = {
        {
            .featue_name = "tx",
            .pin_name = "tx",
            .value = PIN_INVALID_VALUE
        },
        {
            .featue_name = "rx",
            .pin_name = "rx",
            .value = PIN_INVALID_VALUE
        },
        {
            .featue_name = "cts",
            .pin_name = "cts",
            .value = PIN_INVALID_VALUE
        },
        {
            .featue_name = "rts",
            .pin_name = "rts",
            .value = PIN_INVALID_VALUE
        }
    };

    for (i = 0; i < UART_MODULE_MAX; i++) {
        offset1 = fdt_subnode_offset(fdt, uart_offset, uart_node[i]);
        if (0 >= offset1) {
            blog_info("uart[%d] %s NULL.\r\n", i, uart_node[i]);
            continue;
        }

        countindex = fdt_stringlist_count(fdt, offset1, "status");
        if (countindex != 1) {
            blog_info("uart[%d] status_countindex = %d NULL.\r\n", i, countindex);
            continue;
        }
        result = fdt_stringlist_get(fdt, offset1, "status", 0, &lentmp);
        if ((lentmp != 4) || (memcmp("okay", result, 4) != 0)) {
            blog_info("uart[%d] status = %s\r\n", i, result);
            continue;
        }

        /* set path */
        countindex = fdt_stringlist_count(fdt, offset1, "path");
        if (countindex != 1) {
            blog_info("uart[%d] path_countindex = %d NULL.\r\n", i, countindex);
            continue;
        }
        result = fdt_stringlist_get(fdt, offset1, "path", 0, &lentmp);
        if ((lentmp < 0) || (lentmp > 32))
        {
            blog_info("uart[%d] path lentmp = %d\r\n", i, lentmp);
        }
        path = (char *)result;

        /* set baudrate */
        addr_prop = fdt_getprop(fdt, offset1, "baudrate", &lentmp);
        if (addr_prop == NULL) {
            blog_info("uart[%d] baudrate NULL.\r\n", i);
            continue;
        }
        baudrate = BL_FDT32_TO_U32(addr_prop, 0);

        /* set id */
        addr_prop = fdt_getprop(fdt, offset1, "id", &lentmp);
        if (addr_prop == NULL) {
            blog_info("uart[%d] id NULL.\r\n", i);
            continue;
        }
        id = BL_FDT32_TO_U8(addr_prop, 0);

        /* set buffer size */
        offset2 = fdt_subnode_offset(fdt, offset1, "buf_size");
        if (0 >= offset2) {
            blog_info("uart[%d] buf_size NULL, will use default.\r\n", i);
            rx_buf_size = 512;
            tx_buf_size = 512;
        } else {
            addr_prop = fdt_getprop(fdt, offset2, "rx_size", &lentmp);
            if (addr_prop == NULL) {
                blog_info("uart[%d] %s NULL.\r\n", i, "rx_size");
                continue;
            }
            rx_buf_size = BL_FDT32_TO_U32(addr_prop, 0);
            addr_prop = fdt_getprop(fdt, offset2, "tx_size", &lentmp);
            if (addr_prop == NULL) {
                blog_info("uart[%d] %s NULL.\r\n", i, "tx_size");
                continue;
            }
            tx_buf_size = BL_FDT32_TO_U32(addr_prop, 0);
        }
        blog_info("uart[%d] rx_buf_size %d, tx_buf_size %d\r\n", i, rx_buf_size, tx_buf_size);

        for (j = 0; j < 4; j++) {
            offset2 = fdt_subnode_offset(fdt, offset1, "feature");
            if (0 >= offset2) {
                blog_info("uart[%d] feature NULL.\r\n", i);
                continue;
            }
            countindex = fdt_stringlist_count(fdt, offset2, feature_pin[j].featue_name);
            if (countindex != 1) {
                blog_info("uart[%d] %s countindex = %d.\r\n", i, feature_pin[j].featue_name, countindex);
                continue;
            }
            result = fdt_stringlist_get(fdt, offset2, feature_pin[j].featue_name, 0, &lentmp);
            if ((lentmp != 4) || (memcmp("okay", result, 4) != 0)) {
                blog_info("uart[%d] %s status = %s lentmp = %d\r\n", i, feature_pin[j].featue_name, result, lentmp);
                continue;
            }

            /* get pin_name */
            offset2 = fdt_subnode_offset(fdt, offset1, "pin");
            if (0 >= offset2) {
                blog_info("uart[%d] pin NULL.\r\n", i);
                break;
            }
            addr_prop = fdt_getprop(fdt, offset2, feature_pin[j].pin_name, &lentmp);
            if (addr_prop == NULL) {
                blog_info("uart[%d] %s NULL.\r\n", i, feature_pin[j].pin_name);
                continue;
            }
            feature_pin[j].value = BL_FDT32_TO_U8(addr_prop, 0);
        }
        blog_info("id = %d, %s = %d, %s = %d, %s = %d, %s = %d baudrate = %ld.\r\n",
            id,
            feature_pin[0].pin_name, feature_pin[0].value,
            feature_pin[1].pin_name, feature_pin[1].value,
            feature_pin[2].pin_name, feature_pin[2].value,
            feature_pin[3].pin_name, feature_pin[3].value,
            baudrate);

        /*Flush UART FIFO*/
        bl_uart_flush(id);

        bl_uart_init(id, feature_pin[0].value, feature_pin[1].value,
            feature_pin[2].value, feature_pin[3].value, baudrate);

        blog_info("bl_uart_init %d ok.\r\n", id);
        blog_info("bl_uart_init %d baudrate = %ld ok.\r\n", id, baudrate);

        if (dev_uart_init(id, (const char *)path, rx_buf_size, tx_buf_size) != 0) {
            blog_error("dev_uart_init err.\r\n");
        }
    }
    #undef UART_MODULE_MAX
    #undef PIN_INVALID_VALUE
}

int vfs_uart_init_simple_mode(uint8_t id, uint8_t pin_tx, uint8_t pin_rx, int baudrate, const char *path)
{
    bl_uart_flush(id);

    bl_uart_init(id, pin_tx, pin_rx, 255, 255, baudrate);

    if (dev_uart_init(id, path, 128, 128) != 0) {
        blog_error("dev_uart_init err.\r\n");
    }

    return 0;
}

int vfs_uart_init(uint32_t fdt, uint32_t dtb_uart_offset)
{
    if (inited == 1) {
        return VFS_SUCCESS;
    }

    fdt_uart_module_init((const void *)fdt, (int)dtb_uart_offset);

    inited = 1;

    return VFS_SUCCESS;
}

int32_t hal_uart_send_flush(uart_dev_t *uart, uint32_t timeout)
{
    bl_uart_flush(uart->port);                                                                                                                                                                 
    return 0;
}

void hal_uart_setbaud(uart_dev_t *uart, uint32_t baud)
{
    bl_uart_setbaud(uart->port, baud);
}

void hal_uart_setconfig(uart_dev_t *uart, uint32_t baud, hal_uart_parity_t parity)
{
    bl_uart_setconfig(uart->port, baud, parity);
}

