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
 *  AIROC BLEHIDD COMMON APIs for Human Interface Device Profile (HID) Device over LE
 *
 * This file provides definitions of the interfaces for AIROC
 * BLEHIDD Application development.
 *
 */

#ifndef __WICED_HIDD_LIB_H__
#define __WICED_HIDD_LIB_H__

#include "bt_types.h"
#include "wiced.h"
#include "wiced_bt_cfg.h"
#include "hidevent.h"

/*
#pragma pack(1)


typedef PACKED struct
{
    uint8_t eventCode;
    uint8_t length;
    // parameter payload follows
} HCI_EVT_HDR;


#pragma pack()

*/

/*
 * Report or boot mode
*/
enum
{
    PROTOCOL_BOOT = 0,
    PROTOCOL_REPORT = 1
};

/*
 * HID Device Report Type
*/
enum wiced_hidd_report_type_e
{
    WICED_HID_REPORT_TYPE_OTHER         = 0,
    WICED_HID_REPORT_TYPE_INPUT         = 1,
    WICED_HID_REPORT_TYPE_OUTPUT        = 2,
    WICED_HID_REPORT_TYPE_FEATURE       = 3,
    WICED_HID_CLIENT_CHAR_CONF          = 4,  // not a standard report type.
};
typedef uint8_t  wiced_hidd_report_type_t; // see #wiced_hidd_report_type_e

/*
 * Callback wiced_blehidd_write_cback_t
 *
 * The callbacks for incoming HID reports (OUTPUT/FEATURE),  client configuration characteristic descriptors or other write command/request
*/
typedef void (wiced_blehidd_write_cback_t)(wiced_hidd_report_type_t reportType, uint8_t reportId, void *payload, uint16_t payloadSize);

/*
 * Data structure for application event queue (circular)
*/
typedef struct
{
    HidEvent eventFifoOverflow;

    /// Maximum size of elements. Provided during construction.
    uint8_t evtq_elementSize;

    /// Maximum number of elements that can be placed in the queue. Provided during construction.
    uint8_t evtq_maxNumElements;

    /// Location where the queue starts. Provided during construction.
    uint8_t *evtq_bufStart;

    /// Number of elements currently in the queue
    uint8_t evtq_curNumElements;

    /// Read index into the queue
    uint8_t evtq_readIndex;

    /// Write index into the queue
    uint8_t evtq_writeIndex;
} wiced_hidd_app_event_queue_t;

/*
 * Data structure supporting sending/receiving HID reports via HOGP (HID over Gatt Profile)
 *
 * Every outgoing/incoming HID report needs to have at least one entry as bellowed in the HID report table
 * Each outgoing HID report (INPUT/FEATURE) have two entries:
 *     -One for sending HID report via Gatt Notification
 *     -One for client to write client configuration characteristic descriptor value.
 * Each incoming HID report (OUTPUT/FEATURE)  have one entry.
*/
typedef struct
{
    uint8_t                         reportId;                   //HID report ID
    wiced_hidd_report_type_t        reportType;                 // value see enum wiced_hidd_report_type_e
    uint16_t                        handle;                     // Gatt attribute handle
    uint8_t                         sendNotification;           // WICED_TRUE or WICED_FALSE. client should write it
    wiced_blehidd_write_cback_t*    writeCallback;              // callback function to handle client write command/request
    uint16_t                        clientConfigBitmap;         //the bit in client configuration characteristic descriptor bitmap for the specific HID report(outgoing)
} wiced_blehidd_report_gatt_characteristic_t;

/** BR/EDR link power state/mode  */
enum
{
    /// Max power requested
    POWER_MODE_HIGH = 1,

    ///power in between
    POWER_MODE_LOW,

    /// Power mode idle
    POWER_MODE_IDLE,

    /// Link suspend requested
    POWER_MODE_BFC_SUSPEND = 0xF0,

    /// Link disconnect requested
    POWER_MODE_DISCONNECT = 0xF1
};

/** Sniff type */
enum
{
    /// Normal sniff to be negotiated with host/peer
    SNIFF_NORMAL = 0,

    /// Asymmetric sniff.
    SNIFF_ASYMMETRIC
};


/** Sniff parameters */
typedef struct
{
    /// Accceptable max sniff interval, has to be even
    uint16_t minInterval;

    /// Acceptable min sniff intercal,, has to be even
    uint16_t maxInterval;

    /// Number of sniff attempts
    uint16_t attempts;

    /// Sniff timeout
    uint16_t timeout;
} wiced_bt_hidd_pm_sniff_params;

/** parameters at any given power state */
typedef struct
{
    /// The current power mode to achieve
    uint8_t mode;

    /// Number of times to attempt to negotiate with the host should we fail to achieve
    uint8_t maxNegotiationAttempts;

    /// Number of milliseconds to move to the next mode
    uint32_t timeoutToNextInMs;

    /// The type of sniff to negotiate
    uint8_t type;

    /// The multiplier to use on the base sniff interval if type is asymmetric
    uint8_t asymmetricMultiplier;

    /// The parameters for this type of sniff.
    wiced_bt_hidd_pm_sniff_params sniffParams;

} wiced_bt_hidd_pm_pwr_state_t;

enum wiced_hidd_pwmbuz_freq_e
{
    WICED_PWMBUZ_125 = 0,
    WICED_PWMBUZ_250 = 1,
    WICED_PWMBUZ_500 = 2,
    WICED_PWMBUZ_1000 = 3,
    WICED_PWMBUZ_2000 = 4,
    WICED_PWMBUZ_4000 = 5,
    WICED_PWMBUZ_8000 = 6,
    WICED_PWMBUZ_FREQ_MAX = 7,
    WICED_PWMBUZ_FREQ_MANUAL = 0xFF,
};

/*******************************************************
******common APIs for BR/EDR/LE (wiced_hidd_xxx) **********
*******************************************************/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup    wiced_bt_hid     Human Interface Device (HID)
 * @ingroup     wicedbt
*/

/**
 *  @addtogroup  hidd_functions      HIDD Library API
 *  @ingroup     wiced_bt_hid
 *
 *  HIDD Functions
 *
 *  @{
 */

///////////////////////////////////////////////////////////////////////////////
/// Does the necessary initialization for HID Device
///
/// \param dev_type - BT_DEVICE_TYPE_BREDR or BT_DEVICE_TYPE_BLE or BT_DEVICE_TYPE_BREDR_BLE
///
/// \return none.
///////////////////////////////////////////////////////////////////////////////
void wiced_hidd_app_init(wiced_bt_device_type_t dev_type);

////////////////////////////////////////////////////////////////////////////////
/// Register with the controller with callback function, which will be called just before the poll event from the central if enabled.
///For LE link, if enabled, the callback function will be called before every connect event
/// For BR_EDR link, if enabled, the callback function will be called before every sniff or sniff subrate event.
///
/// \param transport_type - BT_TRANSPORT_BR_EDR/BT_TRANSPORT_LE
/// \param peer_bdaddr - peer device bd address
/// \param enabled - enable/disable the callback from controller
/// \param callback - callback function

///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hidd_register_callback_for_poll_event(wiced_bt_transport_t transport_type, BD_ADDR peer_bdaddr, wiced_bool_t enabled, void (*callback)(void *, uint32_t));

////////////////////////////////////////////////////////////////////////////////
/// Constructor for the circular queue. The queue will be empty upon creation.
///
/// \param theq  - pointer to the circular queue
/// \param bufStart_c - pointer to buffer where queue data will be stored. Must
///                               have enough space to store elementSize_c*maxNumElements_c bytes
/// \param elementSize_c  - maximum size of each element
/// \param maxNumElements_c - size of the queue, i.e. the maximum number of elements
///                                            that can be kept in the queue. This number must 2 or more. One of the
///                                            elements will be used to provide an overflow slot functionality
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hidd_event_queue_init(wiced_hidd_app_event_queue_t* theq, void *bufStart_c, uint8_t elementSize_c, uint8_t maxNumElements_c);

////////////////////////////////////////////////////////////////////////////////
/// Discards all elements in the queue, including any elements in the overflow slot.
///
/// \param none
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hidd_event_queue_flush(wiced_hidd_app_event_queue_t* theq);

////////////////////////////////////////////////////////////////////////////////
/// Gets the number of elements currently in the queue
///
/// \param theq  - pointer to the circular queue
///
/// \return -   Number of elements in the queue
////////////////////////////////////////////////////////////////////////////////
uint8_t wiced_hidd_event_queue_get_num_elements(wiced_hidd_app_event_queue_t* theq);

////////////////////////////////////////////////////////////////////////////////
/// Returns pointer to the first element in the queue. If the queue is empty, returns NULL.
///
/// \param theq  - pointer to the circular queue
///
/// \return    pointer to the next element in the queue if the queue is not empty
///               NULL if the queue is empty
////////////////////////////////////////////////////////////////////////////////
void *wiced_hidd_event_queue_get_current_element(wiced_hidd_app_event_queue_t* theq);

////////////////////////////////////////////////////////////////////////////////
/// Removes the current element from the queue. Does nothing if the queue is empty.
///
/// \param theq  - pointer to the circular queue
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_hidd_event_queue_remove_current_element(wiced_hidd_app_event_queue_t* theq);

/////////////////////////////////////////////////////////////////////////////////
/// Queue the given event into the event fifo.
/// If the event fifo is full, it queues an overflow event.
///
/// \param theq  - pointer to the circular queue
/// \param event - pointer to event to queue
/// \param len  -length of event
/// \param pollSeqn  -the poll SEQN
///
/// \return none
/////////////////////////////////////////////////////////////////////////////////
void wiced_hidd_event_queue_add_event_with_overflow(wiced_hidd_app_event_queue_t* theq, HidEvent *event, uint8_t len, uint8_t pollSeqn);

////////////////////////////////////////////////////////////////////////////////
/// Get the current native Bluetooth clock
/// This counter is 28 bits, and ticks every 312.5 us and is adjusted for
/// drift through sleep, etc. To compute differences and times elapsed,
/// use wiced_hidd_get_bt_clocks_since().
///
/// \param none
///
/// \return the counter value.
////////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hidd_get_current_native_bt_clock(void);

////////////////////////////////////////////////////////////////////////////////
/// Compute the time elapsed since "before", in BT clocks.
/// This functions handles rollovers. Clock resets will cause
/// a large value to be returned (i.e. expiring any timers).
///
/// \param before - the previous counter value, as was returned by wiced_hidd_get_current_native_bt_clock().
///
/// \return the time elapsed, in BT clocks (312.5 us).
////////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hidd_get_bt_clocks_since(uint32_t before);

////////////////////////////////////////////////////////////////////////////////
/// Check if transport (UART, USB etc.) detection polling is still on-going
///
/// \param none
///
/// \return WICED_TRUE/WICED_FALSE
////////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hidd_is_transport_detection_polling_on(void);

////////////////////////////////////////////////////////////////////////////////
/// Check if transport (UART, USB etc.) is detected
///
/// \param none
///
/// \return WICED_TRUE/WICED_FALSE
////////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hidd_is_transport_detected(void);


/*******************************************************
****************LE only APIs (wiced_blehidd_xxx) ***********
*******************************************************/
///////////////////////////////////////////////////////////////////////////////
/// Check if the current LE connection is encrypted
///
/// \param none
///
/// \return WICED_TRUE  - encrypted
///            WICED_FALSE  -not encrypted
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_blehidd_is_link_encrypted(void);

///////////////////////////////////////////////////////////////////////////////
/// set the encrypted flag for the current LE connection
/// we ask application to set the flag, so that it will be saved in always on (AON) memory when
/// entering shutdown sleep (SDS) and restored when exiting SDS
///
/// \param is_encrypted - WICED_TRUE/WICED_FALSE
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_set_link_encrypted_flag(wiced_bool_t is_encrypted);

///////////////////////////////////////////////////////////////////////////////
/// Check if the current LE connected device is bonded with us
///
/// \param none
///
/// \return WICED_TRUE  - bonded
///            WICED_FALSE  -not bonded
///////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_blehidd_is_device_bonded(void);

///////////////////////////////////////////////////////////////////////////////
/// set the bonded flag for the current LE connected device.  Application needs to call this
/// function when successful pairing is competed and after each successful reconnection.
/// we ask application to set the flag, so that it will be saved in always on (AON) memory when
/// entering shutdown sleep (SDS) and restored when exiting SDS
///
/// \param is_bonded - WICED_TRUE/WICED_FALSE
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_set_device_bonded_flag(wiced_bool_t is_bonded);

///////////////////////////////////////////////////////////////////////////////
/// Get the peer (current LE connected) device BD Address
///
/// \param none
///
/// \return BD Address
///////////////////////////////////////////////////////////////////////////////
uint8_t *wiced_blehidd_get_peer_addr(void);

///////////////////////////////////////////////////////////////////////////////
/// Get the peer (current LE connected) device address Type
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
uint8_t wiced_blehidd_get_peer_addr_type(void);

///////////////////////////////////////////////////////////////////////////////
/// Get Connection Interval
///
/// \param none
///
/// \return current connection interval
///////////////////////////////////////////////////////////////////////////////
uint16_t wiced_blehidd_get_connection_interval(void);

///////////////////////////////////////////////////////////////////////////////
/// Get Connection Peripheral Latency
///
/// \param none
///
/// \return current connection peripheral latency
///////////////////////////////////////////////////////////////////////////////
uint16_t wiced_blehidd_get_peripheral_latency(void);

///////////////////////////////////////////////////////////////////////////////
/// Get Supervision Timeout
///
/// \param none
///
/// \return current Supervision Timeout
///////////////////////////////////////////////////////////////////////////////
uint16_t wiced_blehidd_get_supervision_timeout(void);

///////////////////////////////////////////////////////////////////////////////
/// Get Connection Handle, which is returned by controller in the LE connection complete event
///
/// \param none
///
/// \return current connection handle
///////////////////////////////////////////////////////////////////////////////
uint16_t wiced_blehidd_get_connection_handle(void);

/////////////////////////////////////////////////////////////////////////////////
/// register HID report table for sending and receiving
///
/// \param map - point to HID report table
/// \param num - number of items in the HID report table
/////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_register_report_table(wiced_blehidd_report_gatt_characteristic_t* table, uint32_t num);

/////////////////////////////////////////////////////////////////////////////////
/// send HID report as GATT notification
///
/// \param gatts_conn_id - GATT connection ID
/// \param reportID - report ID
/// \param reportType - report type.
/// \param data - pointer to report data
/// \param length - length of the report data
///
/// \return 0 - successful
///         others - failed
/////////////////////////////////////////////////////////////////////////////////
wiced_bt_gatt_status_t wiced_blehidd_send_report(uint16_t gatts_conn_id, uint8_t reportID, wiced_hidd_report_type_t reportType, uint8_t *data, uint8_t length);

/////////////////////////////////////////////////////////////////////////////////////////////
/// Handle GATT write cmd/req for HID reports
///
/// \param data - pointer point to GATT attribute
///
/// \return 0 - successful
///          others - failed
/////////////////////////////////////////////////////////////////////////////////////////////
wiced_bt_gatt_status_t wiced_blehidd_write_handler(void *data);

#ifdef WICED_HIDD_PWM_BUZZ
////////////////////////////////////////////////////////////////////////////////
/// set PWM BUZ gpio pin
///
/// \param gpio - LHL GPIO pin
/// \param offVal - the GPIO output val when buz is off
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_pwm_buz_init(uint8_t gpio, uint8_t offVal);

////////////////////////////////////////////////////////////////////////////////
/// set PWM frequency setting for BUZ
///
/// \param preset - PWM frequency, enum defined as wiced_hidd_pwmbuz_freq_e
///     enum wiced_hidd_pwmbuz_freq_e
///     {
///         WICED_PWMBUZ_125 = 0,
///         WICED_PWMBUZ_250 = 1,
///         WICED_PWMBUZ_500 = 2,
///         WICED_PWMBUZ_1000 = 3,
///         WICED_PWMBUZ_2000 = 4,
///         WICED_PWMBUZ_4000 = 5,
///         WICED_PWMBUZ_8000 = 6,
///         WICED_PWMBUZ_FREQ_MAX = 7,
///         WICED_PWMBUZ_FREQ_MANUAL = 0xFF,
///     };
/// \param init_value - PWM initial value for the counter
/// \param toggle_val - PWM value when a toggle happens
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_pwm_buz_freq(uint8_t preset, uint16_t init_value, uint16_t toggle_val);

////////////////////////////////////////////////////////////////////////////////
/// PWM BUZ on
///
/// \param id - PWM ID
///                  PWM HW block has 6 PWM channels each with its own 10 bit counter.
///                  The first PWM id is PWM0;
///             enum
///             {
///                 PWM0  = 0,
///                 PWM1  = 1,
///                 PWM2  = 2,
///                 PWM3  = 3,
///                 PWM4  = 4,
///                 PWM5  = 5,
///                 MAX_PWMS = 6
///             };
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_pwm_buz_on(uint8_t id);

////////////////////////////////////////////////////////////////////////////////
/// PWM BUZ off
///
/// \param id - PWM ID
///                  PWM HW block has 6 PWM channels each with its own 10 bit counter.
///                  The first PWM id is PWM0;
///             enum
///             {
///                 PWM0  = 0,
///                 PWM1  = 1,
///                 PWM2  = 2,
///                 PWM3  = 3,
///                 PWM4  = 4,
///                 PWM5  = 5,
///                 MAX_PWMS = 6
///             };
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_pwm_buz_off(uint8_t id);

#endif // WICED_HIDD_PWM_BUZZ

////////////////////////////////////////////////////////////////////////////////
// Get agreed ATT_MTU size with peer device since it can be changed dynamically by exchange MTU req
///
/// \param bda - peer device BD Address
///
/// \return ATT_MTU size
////////////////////////////////////////////////////////////////////////////////
uint16_t wiced_blehidd_get_att_mtu_size(BD_ADDR bda);

////////////////////////////////////////////////////////////////////////////////
/// Allow applications to disable peripheral latency.
/// Audio and gestural work best when peripheral latency is disabled.
///
/// \param allow - WICED_TRUE (allow peripheral latency) or WICED_FALSE (disable peripheral latency)
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_allow_peripheral_latency(wiced_bool_t allow);

////////////////////////////////////////////////////////////////////////////////
/// Allow applications to set asymmetric peripheral latency in peripheral side.
/// when central doesn't accept peripheral's connection parameter update request, peripheral can enable
/// asymmetric peripheral latency to lower power consumption
///
/// \param connHandle - connection handle
/// \param latency  - peripheral latency
///
/// \return none
////////////////////////////////////////////////////////////////////////////////
void wiced_blehidd_set_asym_peripheral_latency(uint16_t connHandle, uint16_t latency);

////////////////////////////////////////////////////////////////////////////////
/// Check if wake up from ShutDown Sleep is due to receiving LE connect request
///
/// \param none
///
/// \return WICED_TRUE/WICED_FALSE
////////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_blehidd_is_wakeup_from_conn_req(void);

/*******************************************************
*****************BR/EDR only APIs************************
*******************************************************/

/////////////////////////////////////////////////////////////////////////////////
/// set the default Link Supervision Timeout value
///
/// \param slots - New supervision timeout value in slots (625us)
/////////////////////////////////////////////////////////////////////////////////
void wiced_bthidd_setDefaultLinkSupervisionTimeout(UINT16 slots);

#ifdef WICED_BT_HID_POWER_MANAGEMENT_SUPPORTED
/////////////////////////////////////////////////////////////////////////////////
///  Configure power mgmt (active/sniff/sniff subrate) parameters.
///
/// \param      params : paramters  for Power management states
/// \param      numParams : number of Power management states. Maximum is 8
/// \param      params : paramters  for Sniff Subrate (SSR) Power management states
/// \param      numParams : number of Sniff Subrate (SSR) Power management states. Maximum is 3
///
/// \return       WICED_TRUE if successful, otherwise WICED_FALSE
/////////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_bt_hidd_configure_power_management_params(const wiced_bt_hidd_pm_pwr_state_t *params, uint8_t numParams,
const wiced_bt_hidd_pm_pwr_state_t *ssrParams, uint8_t numSSRParams);

////////////////////////////////////////////////////////////////////////////////////////
/// Request to stop actively managing this link's power transition between active, sniff and sniff subrate
///
/// \param      none
/// \return none
////////////////////////////////////////////////////////////////////////////////////////
void wiced_bt_hidd_power_management_pause(void);

/////////////////////////////////////////////////////////////////////////////////////////
/// Request to resume actively managing this link's power transiton between active, sniff and sniff subrate
/// Since we don't know what state we are currently in, we will always go back
/// to max power state and start from there using the current list of params.
///
/// \param      none
/// \return none
/////////////////////////////////////////////////////////////////////////////////////////
void wiced_bt_hidd_power_management_unpause(void);

/////////////////////////////////////////////////////////////////////////////////
/// switch between active POWER_MODE_HIGH states
///
/// \param      index : the index of the new activated POWER_MODE_HIGH state
/// \param      exitSSR: if currently in SSR, the value of exitSSR will decide if it needs to unsniff and re-negotiate new sniff intervals or not.
///
/// \return none
/////////////////////////////////////////////////////////////////////////////////
void wiced_bt_hidd_power_management_switch_active_high_state(uint8_t index, wiced_bool_t exitSSR);

#endif // WICED_BT_HID_POWER_MANAGEMENT_SUPPORTED

/** @} hidd_functions */

/** @} wicedbt_hidd_ble */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __WICED_HIDD_LIB_H__
