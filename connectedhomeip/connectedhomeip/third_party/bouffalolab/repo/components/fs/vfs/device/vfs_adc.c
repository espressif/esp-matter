/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>
#include <aos/kernel.h>

#include <vfs.h>
#include <device/vfs_adc.h>
#include <hal/soc/soc.h>
#include <vfs_err.h>
#include <yloop_types.h>

static int vfs_adc_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    adc_dev_t *adc_dev = NULL; /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            adc_dev = (adc_dev_t *)(fp->node->i_arg);

            /*  init adc device. */
            ret = hal_adc_init(adc_dev);

            /*config mutex when init is OK*/
            if (VFS_SUCCESS == ret) {
                aos_mutex_new((aos_mutex_t*)&(adc_dev->mutex));
            }
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_adc_close(file_t *fp)
{
    int ret = -1;              /* return value */
    adc_dev_t *adc_dev = NULL; /* adc device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            adc_dev = (adc_dev_t *)(fp->node->i_arg);

            if (adc_dev != NULL) {

                aos_mutex_free((aos_mutex_t*)&(adc_dev->mutex));

                /* turns off an ADC hardwar. */
                ret = hal_adc_finalize(adc_dev);
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

static ssize_t vfs_adc_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = -1;              /* return value */
    uint32_t adc_value;        /* adc value buf */
    adc_dev_t *adc_dev = NULL; /* adc device pointer */

    if (sizeof(adc_value) != nbytes) {
        ret = -EINVAL;
        return ret;
    }

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* get the device pointer. */       
        adc_dev = (adc_dev_t *)(fp->node->i_arg);

        if (adc_dev != NULL) {

            /* get adc sampled value. */ 
            ret = hal_adc_value_get(adc_dev, &adc_value, HAL_WAIT_FOREVER);
            /* If the data is got successfully, set the return value to nbytes. */
            if (0 == ret) {
                ret = nbytes;
                memcpy(buf, &adc_value, sizeof(adc_value));//we use memcpy since the user may pass unaligned pointer to us
            } else {
                /*alig ret value to the standard Error no*/
                ret = -EIO;
            }
        } else {
            ret = -EINVAL;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

static void vfs_adc_notify(void *arg)
{
    /*this function is called in the interrupt routine, no task is shceduled*/
    adc_dev_t *adc_dev = (adc_dev_t*)arg;

    if (adc_dev->poll_cb != NULL) {
        ((struct pollfd*)adc_dev->fd)->revents |= POLLIN;
        ((poll_notify_t)adc_dev->poll_cb)(adc_dev->fd, adc_dev->poll_data);
    }

    return;
}

static int vfs_adc_poll(file_t *fp, bool setup, poll_notify_t notify, struct pollfd *fd, void *opa)
{
    adc_dev_t *adc_dev = (adc_dev_t *)(fp->node->i_arg);

    aos_mutex_lock((aos_mutex_t*)&(adc_dev->mutex), AOS_WAIT_FOREVER);
    if (!setup) {
        /*Enter Critical to protect callback from INT context and other task*/
        taskENTER_CRITICAL();
        adc_dev->poll_cb = NULL;
        adc_dev->poll_data = NULL;
        hal_adc_notify_unregister(adc_dev, vfs_adc_notify);
        taskEXIT_CRITICAL();
        goto out;
    }
    /*Protect callback from INT*/
    taskENTER_CRITICAL();
    adc_dev->poll_cb = notify;
    adc_dev->fd = fd;
    adc_dev->poll_data = opa;
    hal_adc_notify_register(adc_dev, vfs_adc_notify);
    taskEXIT_CRITICAL();

out:
    aos_mutex_unlock((aos_mutex_t*)&(adc_dev->mutex));

    return 0;
}

static int vfs_adc_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = -1;
    adc_dev_t *adc_dev = NULL;
    int *level;

    /*we only support arch which have the same size between pointer and unsigned long*/
    if (sizeof(int *) != sizeof(arg)) {
        return -EINVAL;
    }
    level = (int*)arg;//level in is armed

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */       
    adc_dev = (adc_dev_t *)(fp->node->i_arg);

    if (adc_dev == NULL) {
        return -EINVAL;
    }
    
    switch(cmd) {
        case IOCTL_ADC_TRIGGER_DISABLE:
        /*Disable low level trigger, such as INT*/
        {
            ret = hal_adc_notify_register_config_disable(adc_dev);
            if (ret) {
                ret = -EIO;
            }
        }
        break;
        case IOCTL_ADC_TRIGGER_ENABLE_HIGH_LEVEL:
        /*High voltage trigger*/
        {
            ret = hal_adc_notify_register_config_high(adc_dev);
            if (ret) {
                ret = -EIO;
            }
        }
        break;
        case IOCTL_ADC_TRIGGER_ENABLE_LOW_LEVEL:
        /*Low voltage trigger*/
        {
            ret = hal_adc_notify_register_config_low(adc_dev);
            if (ret) {
                ret = -EIO;
            }
        }
        break;
        case IOCTL_ADC_TRIGGER_ENABLE_LEVEL_CUSTOM_HIGHER:
        /*Custom voltage trigger, such as battery monitor*/
        {
            if (NULL == level) {
                ret = -EINVAL;
            } else {
                ret = hal_adc_notify_register_config_higher(adc_dev, *level);
                if (ret) {
                    ret = -EIO;
                }
            }
        }
        break;
        case IOCTL_ADC_TRIGGER_ENABLE_LEVEL_CUSTOM_LOWER:
        {
            if (NULL == level) {
                ret = -EINVAL;
            } else {
                ret = hal_adc_notify_register_config_lower(adc_dev, *level);
                if (ret) {
                    ret = -EIO;
                }
            }
        }
        break;
        default:
        {
            ret =  -EINVAL;
        }
    }

    return ret;
}

/* adc driver struct */
const struct file_ops adc_ops =
{
    .open = vfs_adc_open,
    .read = vfs_adc_read,
    .poll = vfs_adc_poll,
    .ioctl = vfs_adc_ioctl,
    .close = vfs_adc_close,
};

