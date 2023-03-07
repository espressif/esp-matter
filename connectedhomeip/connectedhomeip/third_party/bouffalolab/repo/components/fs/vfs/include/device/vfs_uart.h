/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_UART_H
#define AOS_VFS_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"

#define IOCTL_UART_IOC_CLEAN_MODE                1 /* clean rx ringbuf */
#define IOCTL_UART_IOC_FLUSH_MODE                2 /* flush */
#define IOCTL_UART_IOC_BAUD_MODE                 3 /* baud */
#define IOCTL_UART_IOC_WAITRD_MODE               4 /* waitread */
#define IOCTL_UART_IOC_WAITRDFULL_MODE           5 /* waitread full */
#define IOCTL_UART_IOC_READ_BLOCK                6 /* read block */
#define IOCTL_UART_IOC_READ_NOBLOCK              7 /* read noblock */
#define IOCTL_UART_IOC_PARITY_SET                8 /* set parity */
#define IOCTL_UART_IOC_PARITY_GET                9 /* get parity */
#define IOCTL_UART_IOC_STOPBITS_SET              10 /* set stop bits */
#define IOCTL_UART_IOC_STOPBITS_GET              11 /* get stop bits */
#define IOCTL_UART_IOC_HWFC_SET                  12 /* set hwfc */
#define IOCTL_UART_IOC_HWFC_GET                  13 /* get hwfc */
#define IOCTL_UART_IOC_DATABITS_SET              14 /* set data bits */
#define IOCTL_UART_IOC_DATABITS_GET              15 /* set data bits */
#define IOCTL_UART_IOC_WAITENDBYTE_MODE          16 /* last byte timeout */

#define UART_READ_CFG_BLOCK   1
#define UART_READ_CFG_NOBLOCK 2

typedef enum {
    IO_UART_PARITY_NONE,
    IO_UART_PARITY_ODD,
    IO_UART_PARITY_EVEN,
} ioc_uart_parity_t;

typedef struct _uart_ioctrl_wait_read {
    char *buf;
    int read_size;
    uint32_t timeout; /* ms */
} uart_ioc_waitread_t;

/* uart driver struct */
extern const struct file_ops uart_ops;

/**
 * This function is used to open uart device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_uart_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close uart device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_uart_close(file_t *fp);

/**
 * This function is used to get data from uart.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return  The positive non-zero number of bytes read on success,
 * 0 on read nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_uart_read(file_t *fp, void *buf, size_t nbytes);

/**
 * This function is used to send data through uart.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return   The positive non-zero number of bytes write on success,
 * 0 on write nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_uart_write(file_t *fp, const void *buf, size_t nbytes);

int vfs_uart_init(uint32_t fdt, uint32_t dtb_uart_offset, const char *node[], int node_max);

int vfs_uart_poll(file_t *fp, bool setup, poll_notify_t notify, struct pollfd *fd, void *opa);

int vfs_uart_ioctl(file_t *fp, int cmd, unsigned long arg);

int vfs_uart_sync(file_t *fp);

int vfs_uart_init_simple_mode(uint8_t id, uint8_t pin_tx, uint8_t pin_rx, int baudrate, const char *path);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_UART_H */

