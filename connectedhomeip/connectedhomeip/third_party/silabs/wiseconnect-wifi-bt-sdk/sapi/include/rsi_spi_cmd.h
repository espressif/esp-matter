/*******************************************************************************
* @file  rsi_spi_cmd.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef _SPICMDAPI_H_
#define _SPICMDAPI_H_

/******************************************************
 * *                      Macros
 * ******************************************************/
//C1 Register Bit Field Defines
#define RSI_C1_INIT_CMD 0x15
// sent to spi interface after reset/powerup to init the spi interface

//STM 32 Init Sequence
#define RSI_RS9116_INIT_CMD 0x005c4a12

#define RSI_C1_INIT_RESP 0x55
// response from spi interface to successful init

#define RSI_C176_INIT      0b00111111 //@ and
#define RSI_C176_RW        0b01000000 //@ or
#define RSI_C15_RD         0b11011111 //@ and
#define RSI_C15_WR         0b00100000 //@ or
#define RSI_C14_SPISLV     0b11101111 //@ and
#define RSI_C14_AHBBUS     0b00010000 //@ or
#define RSI_C13_AHBMEM     0b11110111 //@ and
#define RSI_C13_AHBFRM     0b00001000 //@ or
#define RSI_C12_02BITXFR   0b11111011 //@ and
#define RSI_C12_16BITXFR   0b00000100 //@ or
#define RSI_C110_4BYTESLEN 0b11111100
// and number of C bytes transferred, usually 4
#define RSI_C110_1BYTESLEN 0b00000001
// or/~and number of C bytes transferred
#define RSI_C110_2BYTESLEN 0b00000010
// or/~and number of C bytes transferred
#define RSI_C110_3BYTESLEN 0b00000011
// or number of C bytes transferred

/*=========================================================*/
// C2 Register Defines
#define RSI_C276_08BIT      0b00111111 //@ and
#define RSI_C276_32BIT      0b01000000 //@ or
#define RSI_C250_SPIADDR    0b00111111 //@ and
#define RSI_C250_DATAPACKET 0b00000010 //@ or
#define RSI_C250_MGMTPACKET 0b00000100 //@ or

//#define RSI_C3_XFERLENLSB  0x00                 //@ set value
//#define RSI_C4_XFERLENMSB  0x00                 //@ set value

/*==========================================================*/
// C1 Register Defines
// Internal Read
#define RSI_C1INTREAD2BYTES 0x42
// (((((0x00 & C176_INT) & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_2BYTESLEN // 01000010
#define RSI_C1INTREAD1BYTES 0x41

// Memory ReadWrite (AHB Master Read/Write, Internal Legacy Name)
// Memory read/write is normally done using 16-bit transfer length with 4 C bytes transferred
#define RSI_C1MEMWR16BIT1BYTE 0x75
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) | C110_1BYTESLEN // 01110101
#define RSI_C1MEMRD16BIT1BYTE 0x55
// (((0x00 | C176_RW & C15_RD | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) | C110_1BYTESLEN // 01010101

#define RSI_C1MEMWR16BIT4BYTE 0x74
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) & C110_4BYTESLEN // 01110100
#define RSI_C1MEMRD16BIT4BYTE 0x54
// ((((0x00 | C176_RW & C15_RD) | C14_AHBBUS) & C13_AHBMEM) & C12_16BITXFR) & C110_4BYTESLEN // 01010100

// Normally, 2-bit transfer length is not used for memory read/write
#define RSI_C1MEMWR02BIT1BYTE 0x71
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01110001
#define RSI_C1MEMRD02BIT1BYTE 0x51
// (((0x00 | C176_RW & C15_RD | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01010001
#define RSI_C1MEMWR02BIT4BYTE 0x70
// (((0x00 | C176_RW | C15_WR | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) & C110_4BYTESLEN // 01110000
#define RSI_C1MEMRD02BIT4BYTE 0x50
// ((((0x00 | C176_RW & C15_RD) | C14_AHBBUS) & C13_AHBMEM) & C12_02BITXFR) & C110_4BYTESLEN // 01010000

// Frame ReadWrite
// Frame read/writes normally  use 16-bit transfer length
#define RSI_C1FRMWR16BIT1BYTE 0x7d
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_1BYTESLEN // 01111101
#define RSI_C1FRMRD16BIT1BYTE 0x5d
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_1BYTESLEN // 01011101

#define RSI_C1FRMWR16BIT4BYTE 0x7c
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_4BYTESLEN // 01111100
#define RSI_C1FRMRD16BIT4BYTE 0x5c
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_16BITXFR) | C110_4BYTESLEN // 01011100

// Frame read/writes normally do not use 2-bit transfer length
#define RSI_C1FRMWR02BIT1BYTE 0x79
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_1BYTESLEN // 01111001
#define RSI_C1FRMRD02BIT1BYTE 0x59
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_1BYTESLEN // 01011001

#define RSI_C1FRMWR02BIT4BYTE 0x78
// ((C176_RW | C15_WR | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_4BYTESLEN // 01111000
#define RSI_C1FRMRD02BIT4BYTE 0x58
// ((C176_RW & C15_RD | C14_AHBBUS | C13_AHBFRM) & C12_02BITXFR) | C110_4BYTESLEN // 01011000

// SPI Register ReadWrite
#define RSI_C1SPIREGWR16BIT4BYTE 0x64
// ((((C176_RW | C15_WR) & C14_SPISLV) & C13_AHBMEM) & C12_16BITXFR) | C110_4BYTESLEN // 01100100
#define RSI_C1SPIREGRD16BIT4BYTE 0x44
// ((((C176_RW & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_16BITXFR) | C110_4BYTESLEN // 01000100

#define RSI_C1SPIREGWR02BIT4BYTE 0x60
// ((((C176_RW | C15_WR) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_4BYTESLEN // 01100000
#define RSI_C1SPIREGRD02BIT4BYTE 0x40
// ((((C176_RW & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_4BYTESLEN // 01000000

#define RSI_C1SPIREGWR02BIT1BYTE 0x61
// ((((C176_RW | C15_WR) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01100001
#define RSI_C1SPIREGRD02BIT1BYTE 0x41
// ((((C176_RW & C15_RD) & C14_SPISLV) & C13_AHBMEM) & C12_02BITXFR) | C110_1BYTESLEN // 01000001

//C2 Register Defines
#define RSI_C2RDWR4BYTE 0x40 //@ 0x00 | C276_32BIT | C250_DATAPACKET   // 01000010
#define RSI_C2RDWR4BYTE 0x40 //@ 0x00 | C276_32BIT | C250_MGMTPACKET   // 01000100

#define RSI_C2RDWR1BYTE 0x00 //@ (0x00 & C276_08BIT) | C250_DATAPACKET // 00000010
#define RSI_C2RDWR1BYTE 0x00 //@ (0x00 & C276_08BIT) | C250_MGMTPACKET
//@ 00000100
#define RSI_C2MGMT 0x04
#define RSI_C2DATA 0x02
#define RSI_C2WPS  0x08

#define RSI_C2SPIADDR1BYTE  0x00 //@(0x00 & C276_08BIT) | C250_SPIADDR   // 00xxxxxx
#define RSI_C2MEMRDWRNOCARE 0x00 //@0x00 or ANYTHING                     // 00000000
#define RSI_C2SPIADDR4BYTE  0x40 //@(0x00 | C276_32BIT) | C250_SPIADDR   // 01xxxxxx

#define RSI_C1INTWRITE1BYTES 0x61
#define RSI_C1INTWRITE2BYTES 0x62

/*====================================================*/
// Constant Defines
// SPI Status

// SPI success
#define RSI_SPI_SUCCESS 0x58

// SPI busy error
#define RSI_SPI_BUSY 0x54

// SPI fail
#define RSI_SPI_FAIL 0x52

// SPI return codes
#define RSI_SUCCESS 0

// module buffer full  error code
#define RSI_ERROR_BUFFER_FULL -3

// module in sleep error code
#define RSI_ERROR_IN_SLEEP -4

// SPI communication

// SPI Start Token
#define RSI_SPI_START_TOKEN 0x55

// SPI Internal Register Offset

// register access method
#define RSI_SPI_INT_REG_ADDR 0x00

// register access method
#define RSI_SPI_MODE_REG_ADDR 0x08

// Interrupt mask register
#define RSI_INT_MASK_REG_ADDR 0x41050000

// Interrupt clear register
#define RSI_INT_CLR_REG_ADDR 0x22000010

//SPI Mode Register
#define RSI_SPI_MODE_LOW  0x00
#define RSI_SPI_MODE_HIGH 0x01

#ifndef RSI_HWTIMER
//approximate ticks for timeout implementation
#define RSI_INC_TIMER_2  rsi_driver_cb_non_rom->rsi_spiTimer2++
#define RSI_INC_TIMER_1  rsi_driver_cb_non_rom->rsi_spiTimer1++
#define RSI_INC_TIMER_3  rsi_driver_cb_non_rom->rsi_spiTimer3++
#define RSI_INC_TIMER_4  rsi_driver_cb_non_rom->rsi_spiTimer4++
#define RSI_RESET_TIMER1 rsi_driver_cb_non_rom->rsi_spiTimer1 = 0
#define RSI_RESET_TIMER2 rsi_driver_cb_non_rom->rsi_spiTimer2 = 0
#define RSI_RESET_TIMER3 rsi_driver_cb_non_rom->rsi_spiTimer3 = 0
#define RSI_RESET_TIMER4 rsi_driver_cb_non_rom->rsi_spiTimer4 = 0
#else
#define RSI_INC_TIMER_3  rsi_driver_cb_non_rom->rsi_spiTimer3
#define RSI_INC_TIMER_2  rsi_driver_cb_non_rom->rsi_spiTimer2
#define RSI_INC_TIMER_1  rsi_driver_cb_non_rom->rsi_spiTimer1
#define RSI_INC_TIMER_4  rsi_driver_cb_non_rom->rsi_spiTimer4
#define RSI_RESET_TIMER1 rsi_driver_cb_non_rom->rsi_spiTimer1 = 0
#define RSI_RESET_TIMER2 rsi_driver_cb_non_rom->rsi_spiTimer2 = 0
#define RSI_RESET_TIMER3 rsi_driver_cb_non_rom->rsi_spiTimer3 = 0
#define RSI_RESET_TIMER4 rsi_driver_cb_non_rom->rsi_spiTimer4 = 0
void rsi_timer_interrupt_handler(void);
#endif

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

int16_t rsi_send_c1c2(uint8_t c1, uint8_t c2);
int16_t rsi_send_c3c4(uint8_t c3, uint8_t c4);
int16_t rsi_spi_wait_start_token(uint32_t timeout, uint8_t mode);
int16_t rsi_clear_interrupt(uint8_t interruptClear);
int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf);
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);
int16_t rsi_reg_rd(uint8_t regAddr, uint8_t *dBuf);
int16_t rsi_reg_rd2(uint8_t regAddr, uint16_t *dBuf);
int16_t rsi_reg_wr(uint8_t regAddr, uint8_t *dBuf);
int16_t rsi_pre_dsc_rd(uint8_t *dbuf);
int16_t rsi_pkt_rd(uint8_t *pkt, uint16_t dummy_len, uint16_t total_len);
int16_t rsi_set_intr_mask(uint8_t interruptMask);
int16_t rsi_spi_iface_init(void);
void rsi_ulp_wakeup_init(void);
int16_t rsi_device_interrupt_status(uint8_t *int_status);
int16_t rsi_spi_high_speed_enable(void);
int16_t rsi_set_intr_type(uint32_t interruptMaskVal);
int16_t rsi_spi_pkt_len(uint16_t *length);
uint8_t rsi_get_intr_status(void);
#endif
