/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <FreeRTOS.h>
#include <string.h>
#include <aos/kernel.h>

#include <vfs.h>
#include <device/vfs_gpio.h>
#include <hal/soc/soc.h>
#include <vfs_err.h>
#include <yloop_types.h>

/* gpio driver struct */
const struct file_ops gpio_ops =
{
    .open = vfs_gpio_open,
    .close = vfs_gpio_close,
    .read = vfs_gpio_read,
    .write = vfs_gpio_write,
    .ioctl = vfs_gpio_ioctl
};

int vfs_gpio_open(inode_t *inode, file_t *fp)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

            /*  init gpio device. */
            ret = hal_gpio_init(gpio_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_gpio_close(file_t *fp)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

            if (gpio_dev != NULL) {

                /* turns off hardware. */
                ret = hal_gpio_finalize(gpio_dev);
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

ssize_t vfs_gpio_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    uint8_t write_buf;           /* write buf */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

        if (gpio_dev != NULL) {
            if (GPIO_CONFIG_MODE_OUTPUT == gpio_dev->config){
                write_buf = *(uint8_t*)buf;
                switch (write_buf) {
                    case OUTPUT_LOW:
                        ret = hal_gpio_output_low(gpio_dev);
                        break;
                    case OUTPUT_HIGH:
                        ret = hal_gpio_output_high(gpio_dev);
                        break;
                    case OUTPUT_TOGGLE:
                        ret = hal_gpio_output_toggle(gpio_dev);
                        break;
                    default:
                        ret = -EINVAL;
                        break;
                }
            } else {
                ret = -EINVAL;
            }
        } else {
            ret = -EINVAL;
        }
    }
    return ret;
}

ssize_t vfs_gpio_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */
        gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

        if (gpio_dev != NULL) {
            if (GPIO_CONFIG_MODE_OUTPUT == gpio_dev->config) {
                *((uint8_t *)buf) = gpio_dev->level;
                ret = 0;
            }
            /* Judge GPIO MODE is OUTPUT OR INPUT
            OUTPUT return lastest output value ,INPUT read GPIO value*/
            if (GPIO_CONFIG_MODE_INPUT == gpio_dev->config) {
            /* get data from gpio. */
                ret = hal_gpio_input_get(gpio_dev, (uint8_t *)buf);
            }
            /* If the data is read correctly and the number of read data
            bytes is not negative, the return value is set to read bytes. */
            if (ret == 0) {
                ret = sizeof(uint8_t);
            }
        } else {
            ret = -EINVAL;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_gpio_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = -1;                /* return value */
    gpio_dev_t *gpio_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */
    gpio_dev = (gpio_dev_t *)(fp->node->i_arg);

    if (gpio_dev == NULL) {
        return -EINVAL;
    }

        switch(cmd) {
            case IOCTL_GPIO_PULL_UP:
                ret = hal_gpio_pulltype_set(gpio_dev, GPIO_CONFIG_PULL_UP);      /* gpio pullup */
                break;
            case IOCTL_GPIO_PULL_DOWN:
                ret = hal_gpio_pulltype_set(gpio_dev, GPIO_CONFIG_PULL_DOWN);    /* gpio pulldown */
                break;
            case IOCTL_GPIO_PULL_NONE:
                ret = hal_gpio_pulltype_set(gpio_dev, GPIO_CONFIG_PULL_NONE);    /* gpio pullnone */
                break;
            default:
                ret = -EINVAL;
                break;
        }
    return ret;
}
