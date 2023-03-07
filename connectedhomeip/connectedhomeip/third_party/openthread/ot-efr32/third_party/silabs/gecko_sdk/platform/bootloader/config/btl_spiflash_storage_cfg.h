/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader SPI Flash storage
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_SPIFLASH_STORAGE_CONFIG_H
#define BTL_SPIFLASH_STORAGE_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h> SPI Flash Storage Configuration

/*****************************************************************************
* Atmel/Adesto flashes
*
*****************************************************************************/
// <q BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041> Support Adesto AT25SF041 (4 Mb)
// <i> Default: 1
// <i> Enable support for the Adesto AT25SF041 SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041                    1

// <q BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A> Support Atmel AT25DF041A (4 Mb)
// <i> Default: 0
// <i> Enable support for the Atmel AT25DF041A SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A                    0

// <q BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A> Support Atmel AT25DF081A (8 Mb)
// <i> Default: 0
// <i> Enable support for the Atmel AT25DF081A SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A                    0

/*****************************************************************************
* ISSI flashes
*
*****************************************************************************/
// <q BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B> Support ISSI IS25LQ025B (256 kb)
// <i> Default: 0
// <i> Enable support for the ISSI IS25LQ025B SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B                    0

// <q BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B> Support ISSI IS25LQ512B (512 kb)
// <i> Default: 0
// <i> Enable support for the ISSI IS25LQ512B SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B                    0

// <q BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B> Support ISSI IS25LQ010B (1 Mb)
// <i> Default: 0
// <i> Enable support for the ISSI IS25LQ010B SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B                    0

// <q BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B> Support ISSI IS25LQ020B (2 Mb)
// <i> Default: 0
// <i> Enable support for the ISSI IS25LQ020B SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B                    0

// <q BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B> Support ISSI IS25LQ040B (4 Mb)
// <i> Default: 1
// <i> Enable support for the ISSI IS25LQ040B SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B                    1

/*****************************************************************************
* Macronix flashes
*
*****************************************************************************/
// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E> Support Macronix MX25L2006E (2 Mb)
// <i> Default: 0
// <i> Enable support for the Macronix MX25L2006E SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E               0

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E> Support Macronix MX25L4006E (4 Mb)
// <i> Default: 0
// <i> Enable support for the Atmel AT25DF041A SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E               0

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E> Support Macronix MX25L8006E (8 Mb)
// <i> Default: 0
// <i> Enable support for the Macronix MX25L8006E SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E               0

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E> Support Macronix MX25L1606E (16 Mb)
// <i> Default: 0
// <i> Enable support for the Macronix MX25L1606E SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E               0

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E> Support Macronix MX25U1635E (16 Mb 2V)
// <i> Default: 0
// <i> Enable support for the Macronix MX25U1635E SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E               0

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F> Support Macronix MX25R8035F (8 Mb LP)
// <i> Default: 1
// <i> Enable support for the Macronix MX25R8035F SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F               1

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25R3235F> Support Macronix MX25R3235F (32 Mb ULP)
// <i> Default: 1
// <i> Enable support for the Macronix MX25R3235F SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R3235F               1

// <q BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F> Support Macronix MX25R6435F  (64 Mb LP)
// <i> Default: 0
// <i> Enable support for the Macronix MX25R6435F SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F               0 

/*****************************************************************************
* Numonyx flashes
*
*****************************************************************************/
// <q BTL_STORAGE_SPIFLASH_NUMONYX_M25P20> Support Numonyx M25P20 (2 Mb)
// <i> Default: 0
// <i> Enable support for the Numonyx M25P20 SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P20                    0

// <q BTL_STORAGE_SPIFLASH_NUMONYX_M25P40> Support Numonyx M25P40 (4 Mb)
// <i> Default: 0
// <i> Enable support for the Numonyx M25P40 SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P40                    0

// <q BTL_STORAGE_SPIFLASH_NUMONYX_M25P80> Support Numonyx M25P80 (8 Mb)
// <i> Default: 0
// <i> Enable support for the Numonyx M25P80 SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P80                    0

// <q BTL_STORAGE_SPIFLASH_NUMONYX_M25P16> Support Numonyx M25P16 (16 Mb)
// <i> Default: 0
// <i> Enable support for the Numonyx M25P16 SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P16                    0

/*****************************************************************************
* Spansion flashes
*
*****************************************************************************/
// <q BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K> Support Spansion S25FL208K (8 Mb)
// <i> Default: 0
// <i> Enable support for the Spansion S25FL208K SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K                0

/*****************************************************************************
* Winbond flashes
*
*****************************************************************************/

// <q BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV> Support Winbond W25X20BV (2 Mb)
// <i> Default: 0
// <i> Enable support for the Winbond W25X20BV SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV                  0

// <q BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV> Support Winbond W26Q80BV (8 Mb)
// <i> Default: 0
// <i> Enable support for the Winbond W26Q80BV SPI flash in the bootloader. Adding support for multiple SPI flashes in the bootloader requires more flash memory.
#define BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV                  0

// </h>
// <<< end of configuration section >>>







#endif // BTL_SPIFLASH_STORAGE_CONFIG_H
