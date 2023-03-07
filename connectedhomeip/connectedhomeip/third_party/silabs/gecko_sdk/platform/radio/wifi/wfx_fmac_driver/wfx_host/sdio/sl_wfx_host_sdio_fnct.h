/***************************************************************************//**
 * @file sl_wfx_host_sdio_fnct.h
 * @brief Network Device Driver - WFX Wi-Fi module
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_WFX_HOST_SDIO_FNCT_H
#define  SL_WFX_HOST_SDIO_FNCT_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************//**
 * Initialize sdio driver.
 *****************************************************************************/
void         sdio_fnct_init      (RTOS_ERR       *p_err);

/**************************************************************************//**
 * Read a byte from sdio.
 *****************************************************************************/
CPU_INT08U   sdio_fnct_rdbyte    (CPU_INT32U      reg_addr,
                                  RTOS_ERR       *p_err);

/**************************************************************************//**
 * Write a byte from sdio.
 *****************************************************************************/
void         sdio_fnct_wrbyte    (CPU_INT32U      reg_addr,
                                  CPU_INT08U      byte,
                                  RTOS_ERR       *p_err);

/**************************************************************************//**
 * Read a buffer from sdio.
 *****************************************************************************/
void         sdio_fnct_rd        (CPU_INT32U      reg_addr,
                                  CPU_INT08U     *p_buf,
                                  CPU_INT16U      buf_len,
                                  CPU_BOOLEAN     fixed_addr,
                                  RTOS_ERR       *p_err);

/**************************************************************************//**
 * Write a buffer to sdio.
 *****************************************************************************/
void         sdio_fnct_wr        (CPU_INT32U      reg_addr,
                                  CPU_INT08U     *p_buf,
                                  CPU_INT16U      buf_len,
                                  CPU_BOOLEAN     fixed_addr,
                                  RTOS_ERR       *p_err);

/**************************************************************************//**
 * Read a block from sdio.
 *****************************************************************************/
void         sdio_fnct_rdblk     (CPU_INT32U      reg_addr,
                                  CPU_INT08U     *p_buf,
                                  CPU_INT16U      blk_nbr,
                                  CPU_BOOLEAN     fixed_addr,
                                  RTOS_ERR       *p_err);

/**************************************************************************//**
 * Write a block to sdio.
 *****************************************************************************/
void         sdio_fnct_wrblk     (CPU_INT32U      reg_addr,
                                  CPU_INT08U     *p_buf,
                                  CPU_INT16U      blk_nbr,
                                  CPU_BOOLEAN     fixed_addr,
                                  RTOS_ERR       *p_err);

/**************************************************************************//**
 * Enable or disable sdio interrupt.
 *****************************************************************************/
void         sdio_fnct_int_en  (CPU_BOOLEAN     enable,
                                RTOS_ERR       *p_err);

/**************************************************************************//**
 * Register interrupt callback for sdio.
 *****************************************************************************/
void         sdio_fnct_int_reg    (void* callback);
#ifdef __cplusplus
} /*extern "C" */
#endif

#endif
