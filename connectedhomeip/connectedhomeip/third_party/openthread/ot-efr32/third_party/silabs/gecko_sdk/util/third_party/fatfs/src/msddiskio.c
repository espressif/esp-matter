/***************************************************************************//**
 * # License
 * 
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is Third Party Software licensed by Silicon Labs from a third party
 * and is governed by the sections of the MSLA applicable to Third Party
 * Software and the additional terms set forth below.
 * 
 ******************************************************************************/

/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "em_usb.h"
#include "diskio.h"
#include "msdh.h"

static volatile DSTATUS stat = STA_NOINIT;  /* Disk status */

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
  BYTE drv                    /* Physical drive nmuber (0) */
)
{
  if (drv) return STA_NOINIT;         /* Supports only single drive */
  if (stat & STA_NODISK) return stat; /* No card in the socket */

  /* Initialization (always) succeded */
  stat &= ~STA_NOINIT;        /* Clear STA_NOINIT */

  return stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE drv                    /* Physical drive nmuber (0) */
)
{
  if (drv) return STA_NOINIT; /* Supports only single drive */
  return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
  BYTE drv,       /* Physical drive nmuber (0) */
  BYTE *buff,     /* Pointer to the data buffer to store read data */
  DWORD sector,   /* Start sector number (LBA) */
  BYTE count      /* Sector count (1..255) */
)
{
  if (drv || !count) return RES_PARERR;
  if (stat & STA_NOINIT) return RES_NOTRDY;

  return MSDH_ReadSectors( sector, count, buff ) ? RES_OK : RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
  BYTE drv,           /* Physical drive nmuber (0) */
  const BYTE *buff,   /* Pointer to the data to be written */
  DWORD sector,       /* Start sector number (LBA) */
  BYTE count          /* Sector count (1..255) */
)
{
  if (drv || !count) return RES_PARERR;
  if (stat & STA_NOINIT) return RES_NOTRDY;
  if (stat & STA_PROTECT) return RES_WRPRT;

  return MSDH_WriteSectors( sector, count, buff ) ? RES_OK : RES_ERROR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
  BYTE drv,     /* Physical drive nmuber (0) */
  BYTE ctrl,    /* Control code */
  void *buff    /* Buffer to send/receive data block */
)
{
  DRESULT res;

  if (drv) return RES_PARERR;
  if (stat & STA_NOINIT) return RES_NOTRDY;

  res = RES_ERROR;

  switch (ctrl) {
    case CTRL_SYNC :    /* Flush dirty buffer if present */
      /* Not implemented, should be SCSI cmd SYNC_CACHE. */
      res = RES_OK;
      break;

    case CTRL_INVALIDATE :    /* Used when unmounting */
      stat = STA_NOINIT;      /* Set disk status */
      res = RES_OK;
      break;

    case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
      if ( MSDH_GetSectorCount( buff ) )
      {
        res = RES_OK;
      }
      break;

    case GET_SECTOR_SIZE :    /* Get sectors on the disk (WORD) */
      if ( MSDH_GetSectorSize( buff ) )
      {
        res = RES_OK;
      }
      break;

    case GET_BLOCK_SIZE :     /* Get erase block size in unit of sectors (DWORD) */
      /* Use sector size */
      if ( MSDH_GetBlockSize( buff ) )
      {
        res = RES_OK;
      }
      break;

    case MMC_GET_TYPE :     /* Get card type flags (1 byte) */
    case MMC_GET_CSD :      /* Receive CSD as a data block (16 bytes) */
    case MMC_GET_CID :      /* Receive CID as a data block (16 bytes) */
    case MMC_GET_OCR :      /* Receive OCR as an R3 resp (4 bytes) */
    case MMC_GET_SDSTAT :   /* Receive SD status as a data block (64 bytes) */
    default:
      res = RES_PARERR;
  }

  return res;
}
