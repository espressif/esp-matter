/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_SPI_H
#define AOS_VFS_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs_inode.h"
#include "stdint.h"

/*section for SPI IOCTRL*/
/* Read / Write of SPI mode (SPI_MODE_0..SPI_MODE_3) (limited to 8 bits) */
#define IOCTL_SPI_IOC_RD_MODE               0 /* Read and write synchronization modification */
#define IOCTL_SPI_IOC_WR_MODE               1 /* Read and write synchronization modification */
/* Read / Write SPI bit justification */
#define IOCTL_SPI_IOC_RD_LSB_FIRST          2 /* Unimplemented */
#define IOCTL_SPI_IOC_WR_LSB_FIRST          3 /* Unimplemented */
/* Read / Write SPI device word length (1..N) */
#define IOCTL_SPI_IOC_RD_BITS_PER_WORD      4 /* Unimplemented */
#define IOCTL_SPI_IOC_WR_BITS_PER_WORD      5 /* Unimplemented */
/* Read / Write SPI device default max speed hz */
#define IOCTL_SPI_IOC_RD_MAX_SPEED_HZ       6 /* Unimplemented */
#define IOCTL_SPI_IOC_WR_MAX_SPEED_HZ       7 /* Unimplemented */
/* Read / Write of the SPI mode field */
#define IOCTL_SPI_IOC_RD_MODE32             8 /* Unimplemented */
#define IOCTL_SPI_IOC_WR_MODE32             9 /* Unimplemented */
#define IOCTL_SPI_IOC_MESSAGE(N)            (9 + N)

/* Read / Write of SPI mode (SPI_MODE_0..SPI_MODE_3) (limited to 8 bits) */
#define IOCTL_SPI_CPHA      (0x01)
#define IOCTL_SPI_CPOL      (0x02)
#define IOCTL_SPI_MODE_0    (0|0)
#define IOCTL_SPI_MODE_1    (0|IOCTL_SPI_CPHA)
#define IOCTL_SPI_MODE_2    (IOCTL_SPI_CPOL|0)
#define IOCTL_SPI_MODE_3    (IOCTL_SPI_CPOL|IOCTL_SPI_CPHA)

typedef struct spi_ioc_transfer {
    uint32_t   tx_buf;               /* uint64_t to uint32_t */
    uint32_t   rx_buf;               /* uint64_t to uint32_t */
    uint32_t   len;
    uint32_t   speed_hz;
    uint16_t   delay_usecs;          /* Unimplemented */
    uint16_t   delay_msecs;          /* delay ms, bl add*/
    uint8_t    bits_per_word;        /* Unimplemented */
    uint8_t    cs_change;            /* 0: Keep CS activated */
    uint8_t    tx_nbits;             /* Unimplemented */
    uint8_t    rx_nbits;             /* Unimplemented */
    uint8_t    word_delay_usecs;     /* Unimplemented */
    uint8_t    pad;                  /* Unimplemented */
} spi_ioc_transfer_t;

/* spi driver struct */
extern const struct file_ops spi_ops;

/**
 * This function is used to open spi device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_spi_open(inode_t *inode, file_t *fp);

/**
 * This function is used to close spi device.
 *
 * @param[in]  fp  device pointer.
 *
 * @return  0 on success, others on failure with errno set appropriately.
 */
int vfs_spi_close(file_t *fp);

/**
 * This function is used to get data from spi.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return  The positive non-zero number of bytes read on success,
 * 0 on read nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_spi_read(file_t *fp, void *buf, size_t nbytes);

/**
 * This function is used to send data through spi.
 *
 * @param[in]   fp      device pointer.
 * @param[out]  buf     data buffer for data.
 * @param[in]   nbytes  the maximum size of the user-provided buffer.
 *
 * @return   The positive non-zero number of bytes write on success,
 * 0 on write nothing, or negative on failure with errno set appropriately.
 */
ssize_t vfs_spi_write(file_t *fp, const void *buf, size_t nbytes);

int vfs_spi_ioctl(file_t *fp, int cmd, unsigned long arg);

#ifdef __cplusplus
}
#endif

#endif /* AOS_VFS_SPI_H */

