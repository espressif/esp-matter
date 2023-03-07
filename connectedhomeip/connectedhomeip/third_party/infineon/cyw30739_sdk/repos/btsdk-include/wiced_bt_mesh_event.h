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


/**************************************************************************//**
* \file <wiced_bt_mesh_event.h>
*
* Mesh Model definitions
*
*/
#ifndef __MESH_EVENT_H__
#define __MESH_EVENT_H__

#include "wiced_timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @anchor WICED_BT_MESH_EVENT_KEY_IDX
* @name Bits of the wiced_bt_mesh_event_t::app_key_idx
* \details The following is the bits meaning of the field app_key_idx in the structure wiced_bt_mesh_event_t.
* @{
*/
/**
* Bits meaning
*/
// Bits of the field wiced_bt_mesh_event_t::app_key_idx
#define WICED_BT_MESH_EVENT_KEY_IDX_DEFAULT_NETKEYIDX   0xffff  /**<  Use device key with default (first) net key idx. */
#define WICED_BT_MESH_EVENT_KEY_IDX_FLAG_DEVKEY         0x8000  /**<  1 means it is protected by device key and remaining part contains net_key_idx. 0 means it is protected by application key and remaining part contains app_key_idx. */
#define WICED_BT_MESH_EVENT_KEY_IDX_FLAG_INT_IDX        0x4000  /**<  1 means it is internal key index in the same order as added. 0 means it is global key index. */
#define WICED_BT_MESH_EVENT_KEY_IDX_MSK_KEYIDX          0x0fff  /**<  Mask of the app_key_index/net_key_index depending on value 0/1 of the bit WICED_BT_MESH_EVENT_KEY_IDX_FLAG_DEVKEY */
/** @} WICED_BT_MESH_EVENT_KEY_IDX */

/**
 * @addtogroup  wiced_bt_mesh_core          Mesh Core Library API
 * @ingroup     wiced_bt_mesh
 *
 * Mesh Core library of the AIROC BTSDK provide a simple method for an application to integrate
 * Bluetooth Mesh functionality.
 *
 * @{
 */

/**
 * Mesh event structure is exchanged between the app and the mesh models library
 */
typedef struct wiced_bt_mesh_event__t
{
    struct wiced_bt_mesh_event__t *p_next;              /**< Pointer to the next event in the queue */
    void            *complete_callback; /**< pointer to the send complete callback */
    uint16_t        company_id;     /**< Company ID for the Model ID */
    uint16_t        model_id;       /**< Model ID */
    uint16_t        opcode;         /**< Opcode of the message to be transmitted or that has been received */
    uint16_t        hci_opcode;     /**< HCI Opcode received of the message from the MCU being processed */
    uint8_t         element_idx;    /**< Element Index of the source or destination of the message */
    uint8_t         ttl;            /**< Time to leave to be used when transmitting or as received from the peer */
    uint16_t        src;            /**< Address of the source mesh node */
    uint16_t        dst;            /**< Address of the destination mesh node */
    uint16_t        app_key_idx;    /**< Application key index used to decrypt when message was received or which should be used to encrypt to send the message */
    uint16_t        data_len;       /**< Length of data corresponding to the event */
#define CREDENTIAL_FLAG_FLOODING    0x00    /*< Flooding Credentials  */
#define CREDENTIAL_FLAG_FRENDSHIP   0x01    /*< Friendship Credentials  */
#define CREDENTIAL_FLAG_DIRECTED    0x02    /*< Directed Forwarding Credentials  */
    uint8_t         credential_flag;/**< One of the CREDENTIAL_FLAG_XXX value. It comes from publication or credentials of the received message */
    uint8_t         retrans_cnt;    /**< Number of retransmissions for each message. Should be <= 0x7f. It is ignored using 0 value or if it is reply (reply == WICED_TRUE). */
                                    /**< If bit 0x80 is set, the retransmission count and time applies to number of network rexmits, no access layer rexmits, the time is in 10ms ticks */
                                    /**< The number of transmissions is the retrans_cnt + 1. The transmission interval = (retrans_time + 1) * 10 */
#define RETRANSMIT_TIMER_TICK   50
#define RETRANSMIT_TIMER2_TICK  10
    uint8_t         retrans_time;   /**< Interval between retransmissions in 50-millisecond steps. If bit 0x80 of the retrans_cnt is set then The transmission interval = (retrans_time + 1) * 10 */
    uint8_t         reply;          /**< If TRUE the reply is expected */
#define REPLY_TIMER_TICK        50
    uint8_t         reply_timeout;  /**< Time to wait for the peer model layer acknowledgment in 50-millisecond steps */
    union
    {
#define TX_STATUS_COMPLETED         0
#define TX_STATUS_FAILED            1
#define TX_STATUS_ACK_RECEIVED      2
        uint8_t         tx_flag;    /**< Transmission failed or timeout occurred waiting for peer reply */
#define RPL_DELAY_DONT_SAVE         0xff
        uint8_t         rpl_delay;   /**< Model indicates how SEQ shall be saved by the core. It is delay in seconds to save SEQ in RPL. 0-save immediatly; 0xff - don't save; */
    } status;
#define TAG_USE_DIRECTED            0x01
#define TAG_IMMUTABLE_CREDENTIALS   0x02
    uint8_t         flags;
    uint16_t        friend_addr;    /**< core sets it to friend address when calls complete_callback if segmented message has been sent and acked by the friend OnBehalfOf LPN. Otherwise it is 0 */
    uint8_t         send_segmented; /**< if non-0 then core uses segmentation to send that message even if it fits into unsegmented message. */
    int8_t          rssi;           /**< RSSI of the received message */
    wiced_timer_t   timer;
} wiced_bt_mesh_event_t;

/**
 * \brief Create message reply mesh event from the received mesh event
 * \details This function doesn't create but just updates received mesh event to be used to send message back to the originator.
 * After application or a models library calls Mesh Models Library or Mesh Core library passing the pointer to the mesh event, it loses the ownership of the event and should not
 * use it again.
 *
 * @param       p_event information about the message received.
 *
 * @return      Pointer to the input p_event updated for response
 */
wiced_bt_mesh_event_t *wiced_bt_mesh_create_reply_event(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Create the copy of the existing mesh event
 * \details This function creates new mesh event with same content as input mesh event.
 *
 * @param       p_event mesh event to copy.
 *
 * @return      Pointer to the created mesh event
 */
wiced_bt_mesh_event_t *wiced_bt_mesh_copy_event(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Create mesh event for an unsolicited message.
 * \details In case dst is 0 the function takes all information from the model's publication or fails if publication is not configured for the specified model.
 * In case non-0 dst the function uses specified dst and app_key_idx and fills all other fields with default values.
 * In special case with company_id equals to 0xffff the function creates a message event with default ttl.
 *
 * @param       element_index   Element index.
 * @param       company_id      Company ID.
 * @param       model_id        Model ID.
 * @param       dst             Destination address. If parameter is 0, the function finds publication and take uses its fields for the mesh event.
 * @param       app_key_idx     Application key index used to decrypt when message was received or which should be used to encrypt to send the message. Or it can be net key index - see @ref WICED_BT_MESH_EVENT_KEY_IDX "Bits of the wiced_bt_mesh_event_t::app_key_idx")
 *
 * @return      p_event Pointer to a newly allocated mesh event.
 */
wiced_bt_mesh_event_t *wiced_bt_mesh_create_event(uint8_t element_index, uint16_t company_id, uint16_t model_id, uint16_t dst, uint16_t app_key_idx);

/**
 * \brief Release mesh event
 * \details The application should call this function when it receives the mesh event in the callback and the reply value is set to 0.
 *
 * @param       p_event information for the message.
 *
 * @return      None
 */
void wiced_bt_mesh_release_event(wiced_bt_mesh_event_t *p_event);

/* @} wiced_bt_mesh_core */

#ifdef __cplusplus
}
#endif

#endif /* __MESH_EVENT_H__ */
