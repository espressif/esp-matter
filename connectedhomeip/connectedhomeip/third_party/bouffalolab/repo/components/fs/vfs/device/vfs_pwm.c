/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "device/vfs_pwm.h"
#include "hal/soc/soc.h"
#include "vfs_err.h"

#include <utils_log.h>

/* pwm driver struct */
const struct file_ops pwm_ops =
{
    .open = vfs_pwm_open,
    .close = vfs_pwm_close,
    .ioctl = vfs_pwm_ioctl
};

int vfs_pwm_open(inode_t *inode, file_t *fp)
{
    int ret = -1;              /* return value */
    pwm_dev_t *pwm_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* Initialize if the device is first opened. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pwm_dev = (pwm_dev_t *)(fp->node->i_arg);

            /*  init pwm device. */
            ret = hal_pwm_init_bydev(pwm_dev);
        } else {
            ret = VFS_SUCCESS;
        }
    } else {
        ret = -EINVAL;
    }

    return ret;
}

int vfs_pwm_close(file_t *fp)
{
    int ret = -1;              /* return value */
    pwm_dev_t *pwm_dev = NULL; /* device pointer */

    /* check empty pointer. */
    if ((fp != NULL) && (fp->node != NULL)) {

        /* close device if the device is last closed. */
        if (fp->node->refs == 1) {

            /* get the device pointer. */
            pwm_dev = (pwm_dev_t *)(fp->node->i_arg);  

            if (pwm_dev != NULL) {

                /* turns off hardware. */
                ret = hal_pwm_finalize_bydev(pwm_dev);
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

int vfs_pwm_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    int ret = -1;              /* return value */
    pwm_dev_t *pwm_dev = NULL; /* device pointer */
    float duty = 0;
    int32_t freq = 0;

    /* check empty pointer. */
    if ((fp == NULL) || (fp->node == NULL)) {
        return -EINVAL;
    }

    /* get the device pointer. */
    pwm_dev = (pwm_dev_t *)(fp->node->i_arg);  

    if (pwm_dev == NULL) {
        return -EINVAL;
    }

    switch(cmd) {
        case IOCTL_PWM_OUTPUT_START:
        {
            ret = hal_pwm_start_bydev(pwm_dev);
            log_info("ioctl start.\r\n");
        }
        break;
        case IOCTL_PWM_OUTPUT_STOP:
        {
            log_info("ioctl stop.\r\n");
            ret = hal_pwm_stop_bydev(pwm_dev);
        }
        break;
        case IOCTL_PWM_OUTPUT_DUTY:
        {
            log_info("ioctl set duty.\r\n");
            if (0 == arg) {
                return -EINVAL;
            }
            duty = *(float *)arg;
            ret = hal_pwm_set_duty_bydev(pwm_dev, duty);
        }
        break;
        case IOCTL_PWM_OUTPUT_FREQ:
        {
            log_info("ioctl set freq.\r\n");
            if (0 == arg) {
                return -EINVAL;
            }
            freq = *(int32_t *)arg;
            ret = hal_pwm_set_freq_bydev(pwm_dev, freq);
        }
        break;
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}
