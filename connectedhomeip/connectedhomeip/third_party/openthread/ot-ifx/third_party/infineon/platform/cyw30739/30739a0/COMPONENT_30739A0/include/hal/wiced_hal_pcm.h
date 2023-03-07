/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 *
 * Defines the utilities for configuring the I2S
 */

#ifndef _WICED_HAL_PCM_H_
#define _WICED_HAL_PCM_H_

/** AIROC PCM/I2S role */
typedef enum
{
     WICED_HAL_PCM_SLAVE = 0, /**< PCM/I2S role type(slave) */
     WICED_HAL_PCM_MASTER,    /**< [DEFAULT] PCM/I2S role type(master) */
} wiced_hal_pcm_role_t;

/** AIROC PCM/I2S clock rate */
typedef enum
{
    WICED_HAL_PCM_RATE_128K = 0, /**< PCM clock rate (128Kbps)*/
    WICED_HAL_PCM_RATE_256K,     /**< PCM clock rate (256Kbps)*/
    WICED_HAL_PCM_RATE_512K,     /**< PCM clock rate (512Kbps)*/
    WICED_HAL_PCM_RATE_1024K,    /**< PCM clock rate (1024Kbps)*/
    WICED_HAL_PCM_RATE_2048K     /**< PCM clock rate (2048bps)*/
} wiced_hal_pcm_clk_t;

/** AIROC PCM/I2S data order */
typedef enum
{
    WICED_HAL_PCM_MSB_FIRST = 0, /**< PCM data format config for MSB first out */
    WICED_HAL_PCM_LSB_FIRST,     /**< PCM data format config for LSB first out */
} wiced_hal_pcm_data_order_t;

/** AIROC PCM/I2S frame type */
typedef enum
{
    WICED_HAL_PCM_FRAME_TYPE_SHORT = 0, /**< PCM data format config for short frame sync */
    WICED_HAL_PCM_FRAME_TYPE_LONG,      /**< PCM data format config for long frame sync */
} wiced_hal_pcm_sync_type_t;

/** AIROC PCM/I2S Right justify */
typedef enum
{
    WICED_HAL_PCM_DISABLE_RIGHT_JUSTIFY = 0, /**< Used default data justify. Fill data shifted out last*/
    WICED_HAL_PCM_ENABLE_RIGHT_JUSTIFY,      /**< Used right justify to sync data. Fill data shifted out first */
} wiced_hal_pcm_right_justify_t;

/**
 * AIROC PCM/I2S fillData
 *
 * The total number of bits per slot is always sixteen.
 * Unused bits will be filled as defined by fillData.
 * */
typedef enum
{
    WICED_HAL_PCM_FILL_0S = 0,
    WICED_HAL_PCM_FILL_1S,
    WICED_HAL_PCM_FILL_SIGNED,
    WICED_HAL_PCM_FILL_PROG
} wiced_hal_pcm_fill_data_t;

/** AIROC HAL PCM/I2S path */
typedef enum
{
    WICED_HAL_I2S_MODE = 0,
    WICED_HAL_PCM_MODE
}wiced_hal_pcm_path_t;

/** AIROC PCM parameter config */
typedef struct
{
    wiced_hal_pcm_data_order_t lsbFirst;        /**< pcm packet format 0:MSB_FIRST 1:LSB_FIRST */
    uint8_t                    fillBits;        /**< pcm fill bits */
    wiced_hal_pcm_fill_data_t  fillData;        /**< pcm fill data 0:FILL_0S 1:FILL_1S 2:FILL_SIGNED */
    uint8_t                    fillNum;         /**< pcm fill number */
    wiced_hal_pcm_right_justify_t rightJustify; /**< 0:no ; 1:yes  pcm only */
    wiced_hal_pcm_sync_type_t  frame_type;      /**< frame type: 0: short; 1: long */
}wiced_hal_pcm_param_t;

/** AIROC PCM/I2s config */
typedef struct{
    wiced_hal_pcm_path_t   mode;          /**< 0:I2S, 1:PCM */
    wiced_hal_pcm_role_t   role;          /**< I2S/PCM interface role: 0: slave; 1: master */
    wiced_hal_pcm_param_t  pcm_param;     /**< Configure pcm1 param here */
}wiced_hal_pcm_config_t;


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Function         wiced_hal_set_pcm_config
 *
 * To set the pcm configuration parameter
 *
 * @param[in]      pcm_config : strucure pointer
 *
 * @return          void
 *
 */
void wiced_hal_set_pcm_config (wiced_hal_pcm_config_t *pcm_config);

/**
 * Function         wiced_hal_get_pcm_config
 *
 * To get the current pcm configuration
 *
 * @param[in]      void
 *
 * @return          structure wiced_bt_pcm_config_t pointer
 *
 * Default values :  mode = WICED_BT_I2S_MODE
 *                   role = WICED_BT_I2SPCM_MASTER
 *                   pcm_param = NULL
 */
wiced_hal_pcm_config_t* wiced_hal_get_pcm_config(void);

/**
 * Function         wiced_hal_pcm_select_pads
 *
 * Selects the PCM clock, sync, data out and data in, LHL pins for the PCM peripheral to use.
 *
 *
 * @param[in]    pcm_clk              : PCM Clock Pin
 * @param[in]    pcm_sync           : PCM Sync Pin
 * @param[in]    pcm_out             : PCM Data Out Pin
 * @param[in]    pcm_in               : PCM Data In Pin
 *
 * @return     : None
 */
void wiced_hal_pcm_select_pads( uint8_t pcm_clk, uint8_t pcm_sync,
                            uint8_t pcm_out, uint8_t pcm_in );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _WICED_HAL_PCM_H_
