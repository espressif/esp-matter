/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <FreeRTOS.h>
#include <stream_buffer.h>
#include <task.h>
#include <string.h>
#include <aos/kernel.h>
#include <device/vfs_uart.h>
#include <hal/soc/soc.h>
#include <vfs_err.h>
#include <yloop_types.h>
#include <utils_log.h>
#include <blog.h>
#include <hosal_uart.h>
#include <libfdt.h>
#include <vfs_register.h>

typedef struct vfs_uart_dev {
	hosal_uart_dev_t uart;
    void         *rx_ringbuf_handle;
    void         *tx_ringbuf_handle;
    uint32_t     rx_buf_size;
    uint32_t     tx_buf_size;
    void         *mutex;
    void         *poll_cb;
    void         *fd;
    void         *poll_data;
    void         *taskhdl;
    uint8_t      read_block_flag;
    void         *priv;
} vfs_uart_dev_t;

/* uart driver struct */
const struct file_ops uart_ops =
{
    .open = vfs_uart_open,
    .close = vfs_uart_close,
    .read = vfs_uart_read,
    .write = vfs_uart_write,
    .poll = vfs_uart_poll,
    .ioctl = vfs_uart_ioctl,
    .sync = vfs_uart_sync,
};

static int __uart_rx_irq(void *p_arg)
{
    uint8_t tmp_buf[64];
    int length = 0;
    vfs_uart_dev_t *uart = (vfs_uart_dev_t *)p_arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    length = hosal_uart_receive(&uart->uart, tmp_buf, sizeof(tmp_buf));
    if (length > 0) {
        xStreamBufferSendFromISR(uart->rx_ringbuf_handle, tmp_buf,
                length, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    if (uart->poll_cb != NULL) {
        ((struct pollfd*)uart->fd)->revents |= POLLIN;
        ((poll_notify_t)uart->poll_cb)(uart->fd, uart->poll_data);
    }
    return 0;
}

static int __uart_tx_irq(void *p_arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vfs_uart_dev_t *uart = (vfs_uart_dev_t *)p_arg;
    uint8_t ch;
    size_t ret;

    ret = xStreamBufferReceiveFromISR(uart->tx_ringbuf_handle, &ch, 1,
                                      &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    if (ret == 1) {
        hosal_uart_send(&uart->uart, (const void *)&ch, 1);
    } else {
        hosal_uart_ioctl(&uart->uart, HOSAL_UART_TX_TRIGGER_OFF, NULL);
    }
    return 0;
}

int vfs_uart_open(inode_t *inode, file_t *fp)
{
    int ret = -1;                /* return value */
    vfs_uart_dev_t *uart_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {
            /* get the device pointer. */
            uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

            aos_mutex_new((aos_mutex_t*)&(uart_dev->mutex));
            uart_dev->rx_ringbuf_handle = xStreamBufferCreate(uart_dev->rx_buf_size, 1);
            uart_dev->tx_ringbuf_handle = xStreamBufferCreate(uart_dev->tx_buf_size, 1);
            if (uart_dev->rx_ringbuf_handle == NULL || uart_dev->tx_ringbuf_handle == NULL) {
                return -EINVAL;
            }

            /*  init uart device. */
            hosal_uart_callback_set(&uart_dev->uart, HOSAL_UART_TX_CALLBACK,
            		__uart_tx_irq, uart_dev);
            hosal_uart_callback_set(&uart_dev->uart, HOSAL_UART_RX_CALLBACK,
            		__uart_rx_irq, uart_dev);
            hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);
        }
        ret = VFS_SUCCESS;
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_uart_close(file_t *fp)
{
    int ret = -1;                /* return value */
    vfs_uart_dev_t *uart_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {
            /* get the device pointer. */
            uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

            if (uart_dev != NULL) {

                aos_mutex_free((aos_mutex_t*)&(uart_dev->mutex));
                vStreamBufferDelete(uart_dev->rx_ringbuf_handle);
                vStreamBufferDelete(uart_dev->tx_ringbuf_handle);
                /* turns off hardware. */
                ret = hosal_uart_finalize(&uart_dev->uart);
            } else {
                ret = -EINVAL;
            }
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

#define use_tick(now, old) ((uint32_t)(((int32_t)(now)) - ((uint32_t)(old))))

ssize_t vfs_uart_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    vfs_uart_dev_t *uart_dev = NULL; /* device pointer */
    uint32_t timeout;

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

        if ((nbytes > 0) && (uart_dev != NULL)) {
#if defined(CFG_USB_CDC_ENABLE)
            extern int usb_cdc_is_port_open(void);
            extern int usb_cdc_read(uint8_t *data, uint32_t len);
            if(usb_cdc_is_port_open()){
                return usb_cdc_read((uint8_t *)buf, nbytes);
            }
#endif

            aos_mutex_lock((aos_mutex_t*)&(uart_dev->mutex), AOS_WAIT_FOREVER);

            ret = 0;

            /* block */
            timeout = (UART_READ_CFG_BLOCK == uart_dev->read_block_flag) ? AOS_WAIT_FOREVER : 0;

            while (1) {
                ret += xStreamBufferReceive(uart_dev->rx_ringbuf_handle,
                                            buf + ret, nbytes - ret, timeout);
                if ((ret == nbytes) || (timeout == 0)) {
                    break;
                }
            }

            aos_mutex_unlock((aos_mutex_t*)&(uart_dev->mutex));
        } else {
            ret = -EINVAL;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

//TODO block write is needed
ssize_t vfs_uart_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    size_t total_bytes;          /* need to write */
    size_t written_bytes;         /* written bytes */
    vfs_uart_dev_t *uart_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

        if (uart_dev != NULL) {
#if defined(CFG_USB_CDC_ENABLE)
            extern int usb_cdc_is_port_open(void);
            extern int usb_cdc_write(const uint8_t *data, uint32_t len);
            if(usb_cdc_is_port_open()){
                return usb_cdc_write((const uint8_t *)buf, nbytes);
            }
#endif

            total_bytes = nbytes;
            written_bytes = 0;
            while(total_bytes > 0)
            {
                ret = xStreamBufferSend(uart_dev->tx_ringbuf_handle, buf+written_bytes, total_bytes, 0);
                /*Trigger UART Write Now*/
                if (ret > 0) {
                    hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_TX_TRIGGER_ON, NULL);
                    written_bytes+=ret;
                    total_bytes-=ret;
                }
            }
            ret = written_bytes;
        } else {
            ret = -EINVAL;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_uart_poll(file_t *fp, bool setup, poll_notify_t notify, struct pollfd *fd, void *opa)
{
    vfs_uart_dev_t *uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

    aos_mutex_lock((aos_mutex_t*)&(uart_dev->mutex), AOS_WAIT_FOREVER);
    if (!setup) {
        /*Enter Critical to protect callback from INT context and other task*/
        taskENTER_CRITICAL();
        //vPortEnterCritical();
        uart_dev->poll_cb = NULL;
        uart_dev->poll_data = NULL;
        taskEXIT_CRITICAL();
        //vPortExitCritical();
        goto out;
    }
    /*Protect callback from INT*/
    taskENTER_CRITICAL();
    //vPortEnterCritical();
    uart_dev->poll_cb = notify;
    uart_dev->fd = fd;
    uart_dev->poll_data = opa;
    taskEXIT_CRITICAL();
    //vPortExitCritical();

    if (xStreamBufferIsEmpty(uart_dev->rx_ringbuf_handle) != pdTRUE) {
        ((struct pollfd*)uart_dev->fd)->revents |= POLLIN;
        (*notify)(fd, opa);
    }
out:
    aos_mutex_unlock((aos_mutex_t*)&(uart_dev->mutex));

    return 0;
}

int uart_ioctl_cmd_waimode(vfs_uart_dev_t *uart_dev, int cmd, unsigned long arg)
{
    int ret = 0, once_ret = 0;
    TickType_t timeout, last_time, remain_time;
    uint32_t nbytes;
    uart_ioc_waitread_t *waitr_arg = (uart_ioc_waitread_t *)arg;

    if (NULL == waitr_arg) {
        return -EINVAL;
    }

    nbytes = waitr_arg->read_size;

    timeout = pdMS_TO_TICKS(waitr_arg->timeout);

    while (1) {
        last_time = xTaskGetTickCount();
        once_ret = xStreamBufferReceive(uart_dev->rx_ringbuf_handle,
                                    (uint8_t*)waitr_arg->buf + ret,
                                    nbytes - ret,
                                    timeout);
        ret += once_ret;
        if ((ret == nbytes) || (timeout == 0)) {
            break;
        }
        if (IOCTL_UART_IOC_WAITRDFULL_MODE == cmd) {
            remain_time = xTaskGetTickCount() - last_time;
            if (remain_time < timeout) {
                timeout -= remain_time;
                continue;
            }
        }
        if (IOCTL_UART_IOC_WAITENDBYTE_MODE == cmd) {
            if (ret == 0) {
                timeout = portMAX_DELAY;
                continue;
            } else if (once_ret == 0) {
                /*rcv some data and no data rcv timeout again exit*/
                break;
            } else {
                /*no time out and rcv some data timeout reset*/
                timeout = pdMS_TO_TICKS(waitr_arg->timeout);
                continue;
            }
        }
        break;
    }

    return ret;
}

int vfs_uart_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = 0;
    vfs_uart_dev_t *uart_dev = NULL;

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */
    uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

    if (uart_dev == NULL) {
        return -EINVAL;
    }

    aos_mutex_lock((aos_mutex_t*)&(uart_dev->mutex), AOS_WAIT_FOREVER);
    switch(cmd) {
        case IOCTL_UART_IOC_WAITRD_MODE:
        case IOCTL_UART_IOC_WAITRDFULL_MODE:
        case IOCTL_UART_IOC_WAITENDBYTE_MODE:
        {
            ret = uart_ioctl_cmd_waimode(uart_dev, cmd, arg);
        }
        break;
        case IOCTL_UART_IOC_FLUSH_MODE:
        {
        	hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_FLUSH, NULL);
        }
        break;
        case IOCTL_UART_IOC_BAUD_MODE:
        {
            hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_BAUD_SET, (void *)arg);
        }
        break;
        case IOCTL_UART_IOC_READ_BLOCK:
        {
            uart_dev->read_block_flag = UART_READ_CFG_BLOCK;
        }
        break;
        case IOCTL_UART_IOC_READ_NOBLOCK:
        {
            uart_dev->read_block_flag = UART_READ_CFG_NOBLOCK;
        }
        break;
        case IOCTL_UART_IOC_STOPBITS_SET:
        {
            hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_STOP_BITS_SET, (void *)arg);
        }
        break;

        case IOCTL_UART_IOC_PARITY_SET:
        {
            hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_PARITY_SET, (void *)arg);
        }
        break;

        case IOCTL_UART_IOC_HWFC_SET:
        {
            hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_FLOWMODE_SET, (void *)arg);
        }
        break;

        case IOCTL_UART_IOC_DATABITS_SET:
        {
            hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_DATA_WIDTH_SET, (void *)arg);
        }
        break;
        default:
        {
            ret =  -EINVAL;
        }
    }

    aos_mutex_unlock((aos_mutex_t*)&(uart_dev->mutex));

    return ret;
}

int vfs_uart_sync(file_t *fp)
{
    vfs_uart_dev_t *uart_dev = NULL;

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */
    uart_dev = (vfs_uart_dev_t *)(fp->node->i_arg);

    if (uart_dev == NULL) {
        return -EINVAL;
    }

    aos_mutex_lock((aos_mutex_t*)&(uart_dev->mutex), AOS_WAIT_FOREVER);
    hosal_uart_ioctl(&uart_dev->uart, HOSAL_UART_FLUSH, NULL);
    aos_mutex_unlock((aos_mutex_t*)&(uart_dev->mutex));

    return 0;
}

#define BL_FDT32_TO_U8(addr, byte_offset)   ((uint8_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U16(addr, byte_offset)  ((uint16_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))
#define BL_FDT32_TO_U32(addr, byte_offset)  ((uint32_t)fdt32_to_cpu(*(uint32_t *)((uint8_t *)addr + byte_offset)))

static int fdt_uart_module_init(const void *fdt, int uart_offset, const char *node[], int node_max)
{
    #define PIN_INVALID_VALUE 0XFF

    int offset1 = 0;
    int offset2 = 0;

    const uint32_t *addr_prop = 0;
    int lentmp = 0;
    const char *result = 0;
    int countindex = 0;
    int i, j;
    uint32_t rx_buf_size, tx_buf_size;

    int ret;
    uint8_t id;
    char *path = NULL;
    uint32_t baudrate;

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

    for (i = 0; i < node_max; i++) {
        offset1 = fdt_subnode_offset(fdt, uart_offset, node[i]);
        if (0 >= offset1) {
            blog_info("uart[%d] %s NULL.\r\n", i, node[i]);
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

        struct vfs_uart_dev *vfs_uart = calloc(sizeof(struct vfs_uart_dev), 1);
        if (vfs_uart == NULL) {
        	blog_error("no memory !!!!\r\n");
        	return -1;
        }
        HOSAL_UART_CFG_DECL(cfg, id, feature_pin[0].value, feature_pin[1].value, baudrate);
        vfs_uart->uart.config = cfg;
        hosal_uart_init(&vfs_uart->uart);
        vfs_uart->rx_buf_size = rx_buf_size;
        vfs_uart->tx_buf_size = tx_buf_size;

        ret = aos_register_driver(path, &uart_ops, vfs_uart);
        if (ret != VFS_SUCCESS) {
            return ret;
        }
    }
    #undef PIN_INVALID_VALUE
    return 0;
}

int vfs_uart_init(uint32_t fdt, uint32_t dtb_uart_offset, const char *node[], int node_max)
{
	static uint8_t inited = 0;

    if (inited == 1) {
        return VFS_SUCCESS;
    }

    fdt_uart_module_init((const void *)fdt, (int)dtb_uart_offset, node, node_max);

    inited = 1;

    return VFS_SUCCESS;
}

int vfs_uart_init_simple_mode(uint8_t id, uint8_t pin_tx, uint8_t pin_rx, int baudrate, const char *path)
{
    int ret;
    struct vfs_uart_dev *vfs_uart = calloc(sizeof(struct vfs_uart_dev), 1);

    if (vfs_uart == NULL) {
        blog_error("no memory !!!!\r\n");
        return -1;
    }

    HOSAL_UART_CFG_DECL(cfg, id, pin_tx, pin_rx, baudrate);
    vfs_uart->uart.config = cfg;
    hosal_uart_init(&vfs_uart->uart);
    vfs_uart->rx_buf_size = 1024;
    vfs_uart->tx_buf_size = 1024;

    ret = aos_register_driver(path, &uart_ops, vfs_uart);
    if (ret != VFS_SUCCESS) {
        return ret;
    }

    return VFS_SUCCESS;
}

