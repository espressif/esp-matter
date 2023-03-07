/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __HAL_CCNI_H__
#define __HAL_CCNI_H__
#include "hal_platform.h"

#ifdef HAL_CCNI_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup CCNI
 * @{
 * This section introduces the Cross Core Notification Interface(CCNI) HAL APIs including terms and acronyms, details on how to use this driver, supported features, enums, structures and functions.
 * Every CCNI event is assigned to a specific user, the event handler routine is predefined. The ccni driver will be initialized at system initiation stage.
 * @section HAL_CCNI_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b CCNI                 | Cross Core notification interface  |
 *
 *
 * @section HAL_CCNI_Driver_Usage_Chapter  How to use this driver
 *
 * - How to configure a CCNI event \n
 *   - It looks at bits 0 to 31 as events 0 to 31, keeps events 0 to 7 used with a message and events 8 to 31 used without a message.\n
 *
 *   - We strongly advise that you do not change the default configuration.\n
 *      (A) There are some events without a message which had been used as system default.\n
 *      (B) There are events with a message which uses reserved memeory.\n
 *
 * - How to define an event \n
 *   The CCNI module had 32 * 12 events. The driver defines a special name for every event in hal_ccni_config.h for easy use.\n
 *
 *  - Step 1: Decide to use an event with or without a message.\n
 *
 *  - Step 2: Find an event that is not used.\n
 *      (A) Check the event usage status in \\mcu\\driver\\chip\\ab155x\\inc\\hal_ccni_config.h. The event in default status can be used.\n
 *      (B) Check the ccni_*_to_*_function_table[] in hal_ccni_config.c; The event is not used if the isr is default_isr.\n
 *           For example, if you want to apply an event for CM4 to DSP0, please check ccni_cm4_to_dsp0_function_table[] to find an unused event.\n
 *      (C) If you want an event with a message, please select an event within event 0 to 7;\n
 *      (D) If you want an event without a message, please select an event within event 8 to 31.\n
 *
 *  - Step 3: Replace the event name in \\mcu\\driver\\chip\\ab155x\\inc\\hal_ccni_config.h.\n
 *    We strongly advise that you use a name which implies the event function.
 *    @code
 *    //define EVENT 8 for exception from DSP0 to CM4
      #define IRQGEN_DSP0_TO_CM4_EXCEPTION (HAL_CCNI_EVENT8|CCNI_EVENT_SRC_DSP0|CCNI_EVENT_DST_CM4)
 *    @endcode

 * - Step 4: Implement isr handler.\n
 *    @code
 *    //sample code for isr handler
 *    void user_event_handler_routine(hal_ccni_event_t event, void *msg)
 *    {
 *        uint32_t *pMsg = msg;
 *        hal_ccni_status_t status;
 *        //mask event first
 *        status = hal_ccni_mask_event(event);
 *        // the msg points to the message address if the event is with message. Otherwise, the msg is equal to NULL.
 *        if( pMsg != NULL) {
 *            uint32_t address;
 *            //Suppose CM4 received an EVENT from DSP0 with sharing memory and the memory address is in pMsg[0]. \n
 *            //Receiver should do memory address remap by hal_memview_infrasys_to_cm4(), otherwise it can not get right data.
 *            hal_memview_infrasys_to_cm4(pMsg[0]);
 *            log_hal_info("pMsg[0] = %x, pMsg[1] = %x \r\n", pMsg[0],pMsg[1]);
 *        }
 *
 *        //user deal with the event!
 *
 *        //If the event is managed here, please clear and unmask the event when it is complete
 *        //or the event will be managed at a thread level. Please clear and unmask the event when it is complete.
 *        //Otherwise, it cannot receive the event anymore.
 *        status = hal_ccni_clear_event(event);  // clear the event.
 *        status = hal_ccni_unmask_event(event); // unmask the event.
 *    }
 *    @endcode
 *
 * - Step 5: Register the isr handler to ccni_*_to_*_function_table[] in \\mcu\\driver\\chip\\ab155x\\inc\\hal_ccni_config.h. \n
 *
 * - Use the CCNI driver to send an event without a message to the other core.\n
 *
 *   - Step 1. Predefine the CCNI event and event handler routine in the source code.\n
 *
 *   - Step 2. Call #hal_ccni_set_event() to trigger an event.\n
 *
 *   - Step 3. Should check the return value, and do error handling if return value is not #HAL_CCNI_STATUS_OK. \n
 * - Sample code:
 *    @code
 *       //CM4 send an event IRQGEN_CM4_TO_DSP0_EVENT9 to DSP0.
 *       uint32_t status;
 *       hal_ccni_event_t event = IRQGEN_CM4_TO_DSP0_EVENT9;
 *       // the second parameter should be NULL for event without message.
 *       ret = hal_ccni_set_event(event, NULL);
 *       if (HAL_CCNI_STATUS_OK != ret) {
 *           // Error handler;
 *       }
 *
 *    @endcode
 *
 * - Using the CCNI driver to send an event with a message to other core.\n
 *    @code
 *       //CM4 send an event CCNI_CM4_TO_DSP0_EVENT1 to DSP0 with const value in message.
 *       uint32_t status;
         //CM4 send event CCNI_CM4_TO_DSP0_EVENT1 to DSP0
 *       hal_ccni_event_t event = CCNI_CM4_TO_DSP0_EVENT1;
 *       uint32_t msg_array[2];
 *       //the destination core will receive the next 8-byte message.
 *       msg_array[0] = 0x80;
 *       msg_array[1] = 0xa5;
 *       //the second parameter should not be NULL.
 *       ret = hal_ccni_set_event(event, msg_array);
 *       if (HAL_CCNI_STATUS_OK !=  ret) {
 *           //Error handler;
 *       }
 *
 *       //CM4 send an event CCNI_CM4_TO_DSP0_EVENT2 to DSP0 and notify send 128 Bytes data with sharing memory.
 *       uint32_t status;
 *       uint32_t sharing_memory_address;
 *       //CM4 send event CCNI_CM4_TO_DSP0_EVENT2 to DSP0
 *       hal_ccni_event_t event = CCNI_CM4_TO_DSP0_EVENT2;
 *       uint32_t msg_array[2];
 *       //the destination core will receive the next 8-byte message.
 *       sharing_memory_address = 0xabc;  // the memory 0xabc should be defined with #ATTR_SHARE_RWDATA.
 *       //Please use hal_memview_cm4_to_infrasys() to do address remap.
 *       msg_array[0] = hal_memview_cm4_to_infrasys(sharing_memory_address);
 *       //Please mind the the receiver should call hal_memview_dsp0_to_infrasys(msg_array[0]) to remap the address, otherwise the receiver can not get the right data.
 *       msg_array[1] = 0x80;
 *       //the second parameter should not be NULL.
 *       ret = hal_ccni_set_event(event, msg_array);
 *       if (HAL_CCNI_STATUS_OK != ret) {
 *           //Error handler;
 *       }
 *
 *       // It is an example when got HAL_CCNI_STATUS_BUSY at #hal_ccni_set_event;
 *       uint32_t status;
 *       uint32_t count;
 *       do{
 *           status = hal_ccni_set_event(CCNI_CM4_TO_DSP0_EVENT11, NULL);
 *           if(status == HAL_CCNI_STATUS_BUSY) {
 *               hal_gpt_delay_ms(1);
 *               count++;
 *           }
 *           if (count > 10) {
 *               break;   //wait a while and exit if it is overtime.
 *           }
 *       }while(status == HAL_CCNI_STATUS_BUSY);
 *
 *    @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/** @defgroup hal_ccni_enum Enums
  * @{
  */

/** @brief  This enum defines the return status of the CCNI API.  */
typedef enum {
    HAL_CCNI_STATUS_INVALID_PARAMETER = -4,         /**< Invalid parameter error. */
    HAL_CCNI_STATUS_BUSY              = -3,         /**< CCNI event is used by other module. */
    HAL_CCNI_STATUS_ERROR             = -2,         /**< CCNI unspecified error. */
    HAL_CCNI_STATUS_NO_INIT           = -1,         /**< CCNIK module is not initiazed. */
    HAL_CCNI_STATUS_OK                = 0           /**< No error occurred. */
} hal_ccni_status_t;

/** @brief  This enum defines the CCNI event status. */
typedef enum {
    HAL_CCNI_EVENT_STATUS_IDLE = 0,            /**< Event is idle.    */
    HAL_CCNI_EVENT_STATUS_BUSY = 1             /**< Event is in used. */
} hal_ccni_event_status_t;


/**
  * @}
  */

/** @defgroup hal_ccni_typedef Typedef
  * @{
  */

/**@brief CCNI event type defination */
typedef uint32_t hal_ccni_event_t;

/**@brief CCNI interrupt handling route type defination */
typedef struct {
    void (*hal_ccni_callback)(hal_ccni_event_t event, void *msg);   /**< Define the type of call back function. */
} hal_ccni_function_t;

/**@brief CCNI message format. It has eight bytes which are copied to the event receiver which is free to use. For example, the user can use it as a data point and data length, two data, a bit map of 64 bits, etc. */
typedef struct {
    uint32_t ccni_message[2];     /**< Define the CCNI message. */
} hal_ccni_message_t;

/**
  * @}
  */


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function trigger an interrupt of the destination core.
 * @param[in] event is the event name, please use the name define in hal_ccni_config.h.
 * @param[in] message is point which point to a eight bytes buffer when event is event 0 to 7.
              message should be NULL when event is event 8 to 31.\n
 * @return    #HAL_CCNI_STATUS_OK means this function return successfully.\n
 *            #HAL_CCNI_STATUS_INVALID_PARAMETER means an invalid parameter is given by user.\n
 *            #HAL_CCNI_STATUS_BUSY means event is using by other module.\n
 *            #HAL_CCNI_STATUS_NO_INIT means CCNI module is not initiated.\n
 * @note      Please mind if the function returns #HAL_CCNI_STATUS_BUSY. It has race condition risk if it does #hal_ccni_set_event after #hal_ccni_query_event_status.\n
 */
hal_ccni_status_t hal_ccni_set_event(hal_ccni_event_t event, hal_ccni_message_t *message);

/**
 * @brief This function clear the event, it should be done by the event received core.
 * @param[in] event is the event name, please use the name define in hal_ccni_config.h.
 * @return    #HAL_CCNI_STATUS_OK means this function return successfully.\n
 *            #HAL_CCNI_STATUS_INVALID_PARAMETER means an invalid parameter is given by user.\n
 *            #HAL_CCNI_STATUS_BUSY means event is using by other module.\n
 *            #HAL_CCNI_STATUS_NO_INIT means CCNI module is not initiated.\n
 */
hal_ccni_status_t hal_ccni_clear_event(hal_ccni_event_t event);

/**
 * @brief This function use to query the event status.
 * @param[in] event is the event name, please use the name define in hal_ccni_config.h
 * @param[out] data keep the query result, the data is valid only the function returns #HAL_CCNI_STATUS_OK:
 *             #HAL_CCNI_EVENT_STATUS_BUSY means event is busy.\n
 *             #HAL_CCNI_EVENT_STATUS_IDLE means event is idle.\n
 * @return    #HAL_CCNI_STATUS_OK means this function return successfully.\n
 *            #HAL_CCNI_STATUS_INVALID_PARAMETER means an invalid parameter is given by user.\n
 *            #HAL_CCNI_STATUS_BUSY means event is using by other module.\n
 *            #HAL_CCNI_STATUS_NO_INIT means CCNI module is not initiated.\n
 * @note      The function is only used to query the event status. Do not do #hal_ccni_set_event directly when it returns HAL_CCNI_STATUS_BUSY as it has race condition risk.\n
 */
hal_ccni_status_t hal_ccni_query_event_status(hal_ccni_event_t event, uint32_t *data);

/**
 * @brief This function clear the event, it should be done by the event received core.
 * @param[in] event is the event name, please use the name define in hal_ccni_config.h
 * @return    #HAL_CCNI_STATUS_OK means this function return successfully.\n
 *            #HAL_CCNI_STATUS_INVALID_PARAMETER means an invalid parameter is given by user.\n
 *            #HAL_CCNI_STATUS_BUSY means event is using by other module.\n
 *            #HAL_CCNI_STATUS_NO_INIT means CCNI module is not initiated.\n
 */
hal_ccni_status_t hal_ccni_mask_event(hal_ccni_event_t event);

/**
 * @brief This function unmask the event, it should be done by the event received core.
 * @param[in] event is the event name, please use the name define in hal_ccni_config.h
 * @return    #HAL_CCNI_STATUS_OK means this function return successfully.\n
 *            #HAL_CCNI_STATUS_INVALID_PARAMETER means an invalid parameter is given by user.\n
 *            #HAL_CCNI_STATUS_BUSY means event is using by other module.\n
 *            #HAL_CCNI_STATUS_NO_INIT means CCNI module is not initiated.\n
 */
hal_ccni_status_t hal_ccni_unmask_event(hal_ccni_event_t event);

/**
 * @brief This function initiates the CCNI driver. CCNI will be initiated at system start,user needn't do it.
 * @return    #HAL_CCNI_STATUS_OK means this function return successfully.\n
 *            #HAL_CCNI_STATUS_INVALID_PARAMETER means an invalid parameter is given by user.\n
 *            #HAL_CCNI_STATUS_BUSY means event is using by other module.\n
 *            #HAL_CCNI_STATUS_NO_INIT means CCNI module is not initiated.\n
 */
hal_ccni_status_t hal_ccni_init(void);

/**
 * @brief This function reset CCNI driver, don't advice to do it.
 */
hal_ccni_status_t hal_ccni_deinit(void);

#include "hal_ccni_config.h"


/**
* @}
* @}
*/

#endif /* #ifdef HAL_CCNI_MODULE_ENABLED */
#endif /* #ifndef __HAL_CCNI_H__ */

