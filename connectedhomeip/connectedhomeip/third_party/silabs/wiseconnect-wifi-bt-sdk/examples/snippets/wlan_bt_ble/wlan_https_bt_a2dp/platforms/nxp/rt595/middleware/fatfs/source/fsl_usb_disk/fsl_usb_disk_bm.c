/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ffconf.h"
/* This fatfs subcomponent is disabled by default
 * To enable it, define following macro in ffconf.h */
#ifdef USB_DISK_ENABLE

#include "fsl_usb_disk.h" /* FatFs lower layer API */


/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief host msd ufi command callback.
 *
 * This function is used as callback function for ufi command .
 *
 * @param param      NULL.
 * @param data       data buffer pointer.
 * @param dataLength data length.
 * @status           transfer result status.
 */
static void USB_HostMsdUfiCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status);

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern usb_host_handle g_HostHandle;

usb_host_class_handle g_UsbFatfsClassHandle;
static uint32_t s_FatfsSectorSize;
/* command on-going state. It should set to 1 when start command, it is set to 0 in the callback */
static volatile uint8_t ufiIng;
/* command callback status */
static volatile usb_status_t ufiStatus;

#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_UsbTransferBuffer[FF_MAX_SS];
#else
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_UsbTransferBuffer[20];
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/

static void USB_HostMsdUfiCallback(void *param, uint8_t *data, uint32_t dataLength, usb_status_t status)
{
    ufiIng = 0;
    ufiStatus = status;
}

static inline void USB_HostControllerTaskFunction(usb_host_handle hostHandle)
{
#if ((defined USB_HOST_CONFIG_KHCI) && (USB_HOST_CONFIG_KHCI))
    USB_HostKhciTaskFunction(hostHandle);
#endif /* USB_HOST_CONFIG_KHCI */
#if ((defined USB_HOST_CONFIG_EHCI) && (USB_HOST_CONFIG_EHCI))
    USB_HostEhciTaskFunction(hostHandle);
#endif /* USB_HOST_CONFIG_EHCI */
#if ((defined USB_HOST_CONFIG_IP3516HS) && (USB_HOST_CONFIG_IP3516HS > 0U))
        USB_HostIp3516HsTaskFunction(g_HostHandle);
#endif /* USB_HOST_CONFIG_IP3516HS */
#if ((defined USB_HOST_CONFIG_OHCI) && (USB_HOST_CONFIG_OHCI > 0U))
        USB_HostOhciTaskFunction(g_HostHandle);
#endif /* USB_HOST_CONFIG_OHCI */
}

DSTATUS USB_HostMsdInitializeDisk(BYTE pdrv)
{
    uint32_t address;

    /* test unit ready */
    ufiIng = 1;
    if (g_UsbFatfsClassHandle == NULL)
    {
        return RES_ERROR;
    }
    if (USB_HostMsdTestUnitReady(g_UsbFatfsClassHandle, 0, USB_HostMsdUfiCallback, NULL) != kStatus_USB_Success)
    {
        return STA_NOINIT;
    }
    while (ufiIng) /* wait the command */
    {
        USB_HostControllerTaskFunction(g_HostHandle);
    }

    /*request sense */
    ufiIng = 1;
    if (g_UsbFatfsClassHandle == NULL)
    {
        return RES_ERROR;
    }
    if (USB_HostMsdRequestSense(g_UsbFatfsClassHandle, 0, s_UsbTransferBuffer, sizeof(usb_host_ufi_sense_data_t),
                                USB_HostMsdUfiCallback, NULL) != kStatus_USB_Success)
    {
        return STA_NOINIT;
    }
    while (ufiIng) /* wait the command */
    {
        USB_HostControllerTaskFunction(g_HostHandle);
    }

    /* get the sector size */
    ufiIng = 1;
    if (g_UsbFatfsClassHandle == NULL)
    {
        return RES_ERROR;
    }
    if (USB_HostMsdReadCapacity(g_UsbFatfsClassHandle, 0, s_UsbTransferBuffer, sizeof(usb_host_ufi_read_capacity_t),
                                USB_HostMsdUfiCallback, NULL) != kStatus_USB_Success)
    {
        return STA_NOINIT;
    }
    else
    {
        while (ufiIng)
        {
            USB_HostControllerTaskFunction(g_HostHandle);
        }
        if (ufiStatus == kStatus_USB_Success)
        {
            address = (uint32_t)&s_UsbTransferBuffer[0];
            address = (uint32_t)((usb_host_ufi_read_capacity_t *)(address))->blockLengthInBytes;
            s_FatfsSectorSize = USB_LONG_FROM_BIG_ENDIAN_ADDRESS(((uint8_t *)address));
        }
        else
        {
            s_FatfsSectorSize = 512;
        }
    }

    return 0x00;
}

DSTATUS USB_HostMsdGetDiskStatus(BYTE pdrv)
{
    return 0x00;
}

DRESULT USB_HostMsdReadDisk(BYTE pdrv, BYTE *buff, DWORD sector, UINT count)
{
    DRESULT fatfs_code = RES_ERROR;
    usb_status_t status = kStatus_USB_Success;
    uint32_t retry = USB_HOST_FATFS_RW_RETRY_TIMES;
    uint8_t *transferBuf;
    uint32_t sectorCount;
    uint32_t sectorIndex;
#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
    uint32_t index;
#endif

    if (!count)
    {
        return RES_PARERR;
    }

#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
    transferBuf = s_UsbTransferBuffer;
    sectorCount = 1;
    for (index = 0; index < count; ++index)
    {
        sectorIndex = sector + index;
#else
        transferBuf = buff;
        sectorCount = count;
        sectorIndex = sector;
#endif
        retry = USB_HOST_FATFS_RW_RETRY_TIMES;
        while (retry--)
        {
            ufiIng = 1;
            if (g_UsbFatfsClassHandle == NULL)
            {
                return RES_ERROR;
            }
            status = USB_HostMsdRead10(g_UsbFatfsClassHandle, 0, sectorIndex, (uint8_t *)transferBuf,
                                       (uint32_t)(s_FatfsSectorSize * sectorCount), sectorCount, USB_HostMsdUfiCallback, NULL);
            if (status != kStatus_USB_Success)
            {
                fatfs_code = RES_ERROR;
            }
            else
            {
                while (ufiIng)
                {
                    USB_HostControllerTaskFunction(g_HostHandle);
                }
                if (ufiStatus == kStatus_USB_Success)
                {
                    fatfs_code = RES_OK;
                    break;
                }
                else
                {
                    fatfs_code = RES_NOTRDY;
                }
            }
        }
#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
        memcpy(buff + index * s_FatfsSectorSize, s_UsbTransferBuffer, s_FatfsSectorSize);
    }
#endif
    return fatfs_code;
}

DRESULT USB_HostMsdWriteDisk(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    DRESULT fatfs_code = RES_ERROR;
    usb_status_t status = kStatus_USB_Success;
    uint32_t retry = USB_HOST_FATFS_RW_RETRY_TIMES;
    const uint8_t *transferBuf;
    uint32_t sectorCount;
    uint32_t sectorIndex;
#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
    uint32_t index;
#endif

    if (!count)
    {
        return RES_PARERR;
    }

#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
    transferBuf = (const uint8_t *)s_UsbTransferBuffer;
    sectorCount = 1;
    for (index = 0; index < count; ++index)
    {
        sectorIndex = sector + index;
        memcpy(s_UsbTransferBuffer, buff + index * s_FatfsSectorSize, s_FatfsSectorSize);
#else
        transferBuf = buff;
        sectorCount = count;
        sectorIndex = sector;
#endif
        retry = USB_HOST_FATFS_RW_RETRY_TIMES;
        while (retry--)
        {
            ufiIng = 1;
            if (g_UsbFatfsClassHandle == NULL)
            {
                return RES_ERROR;
            }
            status = USB_HostMsdWrite10(g_UsbFatfsClassHandle, 0, sectorIndex, (uint8_t *)transferBuf,
                                        (uint32_t)(s_FatfsSectorSize * sectorCount), sectorCount, USB_HostMsdUfiCallback, NULL);
            if (status != kStatus_USB_Success)
            {
                fatfs_code = RES_ERROR;
            }
            else
            {
                while (ufiIng)
                {
                    USB_HostControllerTaskFunction(g_HostHandle);
                }
                if (ufiStatus == kStatus_USB_Success)
                {
                    fatfs_code = RES_OK;
                    break;
                }
                else
                {
                    fatfs_code = RES_NOTRDY;
                }
            }
        }
#if (defined(USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE) && (USB_HOST_CONFIG_BUFFER_PROPERTY_CACHEABLE)) ||\
    ((!(defined(FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION) && FSL_FEATURE_HAS_NO_NONCACHEABLE_SECTION)) && defined(FSL_FEATURE_L1ICACHE_LINESIZE_BYTE))
    }
#endif
    return fatfs_code;
}

DRESULT USB_HostMsdIoctlDisk(BYTE pdrv, BYTE cmd, void *buff)
{
    uint32_t address;
    DRESULT fatfs_code = RES_ERROR;
    usb_status_t status = kStatus_USB_Success;
    uint32_t value;

    switch (cmd)
    {
        case GET_SECTOR_COUNT:
        case GET_SECTOR_SIZE:
            if (!buff)
            {
                return RES_ERROR;
            }

            ufiIng = 1;
            if (g_UsbFatfsClassHandle == NULL)
            {
                return RES_ERROR;
            }
            status = USB_HostMsdReadCapacity(g_UsbFatfsClassHandle, 0, s_UsbTransferBuffer,
                                             sizeof(usb_host_ufi_read_capacity_t), USB_HostMsdUfiCallback, NULL);
            if (status != kStatus_USB_Success)
            {
                fatfs_code = RES_ERROR;
            }
            else
            {
                while (ufiIng)
                {
                    USB_HostControllerTaskFunction(g_HostHandle);
                }
                if (ufiStatus == kStatus_USB_Success)
                {
                    fatfs_code = RES_OK;
                }
                else
                {
                    fatfs_code = RES_NOTRDY;
                }
            }

            if (fatfs_code == RES_OK)
            {
                if (GET_SECTOR_COUNT == cmd) /* Get number of sectors on the disk (DWORD) */
                {
                    address = (uint32_t)&s_UsbTransferBuffer[0];
                    address = (uint32_t)((usb_host_ufi_read_capacity_t *)(address))->lastLogicalBlockAddress;
                    value = USB_LONG_FROM_BIG_ENDIAN_ADDRESS(((uint8_t *)address));
                    ((uint8_t *)buff)[0] = ((uint8_t*)&value)[0];
                    ((uint8_t *)buff)[1] = ((uint8_t*)&value)[1];
                    ((uint8_t *)buff)[2] = ((uint8_t*)&value)[2];
                    ((uint8_t *)buff)[3] = ((uint8_t*)&value)[3];
                }
                else /* Get the sector size in byte */
                {
                    address = (uint32_t)&s_UsbTransferBuffer[0];
                    address = (uint32_t)((usb_host_ufi_read_capacity_t *)(address))->blockLengthInBytes;
                    value = USB_LONG_FROM_BIG_ENDIAN_ADDRESS(((uint8_t *)address));
                    ((uint8_t *)buff)[0] = ((uint8_t*)&value)[0];
                    ((uint8_t *)buff)[1] = ((uint8_t*)&value)[1];
                    ((uint8_t *)buff)[2] = ((uint8_t*)&value)[2];
                    ((uint8_t *)buff)[3] = ((uint8_t*)&value)[3];
                }
            }
            break;

        case GET_BLOCK_SIZE:
            if (!buff)
            {
                return RES_ERROR;
            }
            *(uint32_t *)buff = 0;
            fatfs_code = RES_OK;
            break;

        case CTRL_SYNC:
            fatfs_code = RES_OK;
            break;

        default:
            fatfs_code = RES_PARERR;
            break;
    }
    return fatfs_code;
}
#endif /* USB_DISK_ENABLE */
