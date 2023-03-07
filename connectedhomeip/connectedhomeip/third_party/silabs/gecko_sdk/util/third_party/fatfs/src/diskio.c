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

#include "diskio.h"
#include "microsd.h"

static DSTATUS stat = STA_NOINIT;  /* Disk status */
static UINT CardType;

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
  BYTE drv  /* Physical drive nmuber (0) */
)
{
  BYTE n, cmd, ty, ocr[4];

  if (drv) return STA_NOINIT;                   /* Supports only single drive */
  if (stat & STA_NODISK) return stat;           /* No card in the socket */

  MICROSD_PowerOn();                            /* Force socket power on */
  MICROSD_SpiClkSlow();                         /* Start with low SPI clock. */
  for (n = 10; n; n--) MICROSD_XferSpi(0xff);   /* 80 dummy clocks */

  ty = 0;
  if (MICROSD_SendCmd(CMD0, 0) == 1) {          /* Enter Idle state */
    MICROSD_TimeOutSet(1000);                   /* Initialization timeout of 1000 msec */
    if (MICROSD_SendCmd(CMD8, 0x1AA) == 1) {    /* SDv2? */
      for (n = 0; n < 4; n++) ocr[n] = MICROSD_XferSpi(0xff); /* Get trailing return value of R7 resp */
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) {   /* The card can work at vdd range of 2.7-3.6V */
        while (!MICROSD_TimeOutElapsed() && MICROSD_SendCmd(ACMD41, 0x40000000)); /* Wait for leaving idle state (ACMD41 with HCS bit) */
        if (!MICROSD_TimeOutElapsed() && MICROSD_SendCmd(CMD58, 0) == 0) {        /* Check CCS bit in the OCR */
          for (n = 0; n < 4; n++) ocr[n] = MICROSD_XferSpi(0xff);
          ty = (ocr[0] & 0x40) ? CT_SD2|CT_BLOCK : CT_SD2; /* SDv2 */
        }
      }
    } else {                                    /* SDv1 or MMCv3 */
      if (MICROSD_SendCmd(ACMD41, 0) <= 1) {
        ty = CT_SD1; cmd = ACMD41;              /* SDv1 */
      } else {
        ty = CT_MMC; cmd = CMD1;                /* MMCv3 */
      }
      while (!MICROSD_TimeOutElapsed() && MICROSD_SendCmd(cmd, 0));     /* Wait for leaving idle state */
      if (MICROSD_TimeOutElapsed() || MICROSD_SendCmd(CMD16, 512) != 0) /* Set read/write block length to 512 */
        ty = 0;
    }
  }
  CardType = ty;
  MICROSD_Deselect();

  if (ty) {                                     /* Initialization succeded */
    stat &= ~STA_NOINIT;                        /* Clear STA_NOINIT */
    MICROSD_SpiClkFast();                       /* Speed up SPI clock. */
  } else {                                      /* Initialization failed */
    MICROSD_PowerOff();
    stat |= STA_NOINIT;                         /* Set STA_NOINIT */
  }

  return stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE drv                        /* Physical drive nmuber (0) */
)
{
  if (drv) return STA_NOINIT;     /* Supports only single drive */
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

  if (!(CardType & CT_BLOCK)) sector *= 512;  /* Convert to byte address if needed */

  if (count == 1) {                           /* Single block read */
    if ((MICROSD_SendCmd(CMD17, sector) == 0) /* READ_SINGLE_BLOCK */
      && MICROSD_BlockRx(buff, 512))
      count = 0;
  }
  else {                                        /* Multiple block read */
    if (MICROSD_SendCmd(CMD18, sector) == 0) {  /* READ_MULTIPLE_BLOCK */
      do {
        if (!MICROSD_BlockRx(buff, 512)) break;
        buff += 512;
      } while (--count);
      MICROSD_SendCmd(CMD12, 0);                /* STOP_TRANSMISSION */
    }
  }
  MICROSD_Deselect();

  return count ? RES_ERROR : RES_OK;
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

  if (!(CardType & CT_BLOCK)) sector *= 512;  /* Convert to byte address if needed */

  if (count == 1) {                           /* Single block write */
    if ((MICROSD_SendCmd(CMD24, sector) == 0) /* WRITE_BLOCK */
      && MICROSD_BlockTx(buff, 0xFE))
      count = 0;
  }
  else {                                      /* Multiple block write */
    if (CardType & CT_SDC) MICROSD_SendCmd(ACMD23, count);
    if (MICROSD_SendCmd(CMD25, sector) == 0) {/* WRITE_MULTIPLE_BLOCK */
      do {
        if (!MICROSD_BlockTx(buff, 0xFC)) break;
        buff += 512;
      } while (--count);
      if (!MICROSD_BlockTx(0, 0xFD))          /* STOP_TRAN token */
        count = 1;
    }
  }
  MICROSD_Deselect();

  return count ? RES_ERROR : RES_OK;
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
  BYTE n, csd[16], *ptr = buff;
  DWORD csize;


  if (drv) return RES_PARERR;
  if (stat & STA_NOINIT) return RES_NOTRDY;

  res = RES_ERROR;
  switch (ctrl) {
    case CTRL_SYNC :                /* Flush dirty buffer if present */
      if (MICROSD_Select()) {
        MICROSD_Deselect();
        res = RES_OK;
      }
      break;

    case CTRL_INVALIDATE :          /* Used when unmounting */
      stat = STA_NOINIT;            /* Set disk status */
      res = RES_OK;
      break;

    case GET_SECTOR_COUNT :         /* Get number of sectors on the disk (WORD) */
      if ((MICROSD_SendCmd(CMD9, 0) == 0) && MICROSD_BlockRx(csd, 16)) {
        if ((csd[0] >> 6) == 1) {                     /* SDv2? */
          csize = csd[9] + ((WORD)csd[8] << 8) + 1;
          *(DWORD*)buff = (DWORD)csize << 10;
        } else {                                      /* SDv1 or MMCv2 */
          n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
          csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
          *(DWORD*)buff = (DWORD)csize << (n - 9);
        }
        res = RES_OK;
      }
      break;

    case GET_SECTOR_SIZE :          /* Get sectors on the disk (WORD) */
      *(WORD*)buff = 512;
      res = RES_OK;
      break;

    case GET_BLOCK_SIZE :           /* Get erase block size in unit of sectors (DWORD) */
      if (CardType & CT_SD2) {      /* SDv2? */
        if (MICROSD_SendCmd(ACMD13, 0) == 0) {    /* Read SD status */
          MICROSD_XferSpi(0xff);
          if (MICROSD_BlockRx(csd, 16)) {         /* Read partial block */
            for (n = 64 - 16; n; n--) MICROSD_XferSpi(0xff); /* Purge trailing data */
            *(DWORD*)buff = 16UL << (csd[10] >> 4);
            res = RES_OK;
          }
        }
      } else {                      /* SDv1 or MMCv3 */
        if ((MICROSD_SendCmd(CMD9, 0) == 0) && MICROSD_BlockRx(csd, 16)) {  /* Read CSD */
          if (CardType & CT_SD1) {	/* SDv1 */
            *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
          } else {                  /* MMCv3 */
            *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
          }
          res = RES_OK;
        }
      }
      break;

    case MMC_GET_TYPE :             /* Get card type flags (1 byte) */
      *ptr = CardType;
      res = RES_OK;
      break;

    case MMC_GET_CSD :              /* Receive CSD as a data block (16 bytes) */
      if ((MICROSD_SendCmd(CMD9, 0) == 0)       /* READ_CSD */
        && MICROSD_BlockRx(buff, 16))
        res = RES_OK;
      break;

    case MMC_GET_CID :              /* Receive CID as a data block (16 bytes) */
      if ((MICROSD_SendCmd(CMD10, 0) == 0)      /* READ_CID */
        && MICROSD_BlockRx(buff, 16))
        res = RES_OK;
      break;

    case MMC_GET_OCR :              /* Receive OCR as an R3 resp (4 bytes) */
      if (MICROSD_SendCmd(CMD58, 0) == 0) {     /* READ_OCR */
        for (n = 0; n < 4; n++)
          *((BYTE*)buff+n) = MICROSD_XferSpi(0xff);
        res = RES_OK;
      }
      break;

    case MMC_GET_SDSTAT :           /* Receive SD statsu as a data block (64 bytes) */
      if (MICROSD_SendCmd(ACMD13, 0) == 0) {    /* SD_STATUS */
        MICROSD_XferSpi(0xff);
        if (MICROSD_BlockTx(buff, 64))
          res = RES_OK;
      }
      break;

    default:
      res = RES_PARERR;
  }

  MICROSD_Deselect();

  return res;
}
