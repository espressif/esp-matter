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
* Generic OnOff Model definitions.
*
*/
#ifndef __MESH_MODEL_UTILS_H__
#define __MESH_MODEL_UTILS_H__

#include "wiced_bt_mesh_models.h"

#define TRACE_NONE      0
#define TRACE_ERROR     1
#define TRACE_CRITICAL  1
#define TRACE_WARNING   2
#define TRACE_INFO      3
#define TRACE_DEBUG     4

// These definitions should be before #include "wiced_bt_trace.h"
#if defined(WICED_BT_MESH_TRACE_ENABLE)

#define FID_MMDL_APP__MESH_CONFIG_CLIENT_C                      33
#define FID_MMDL_APP__MESH_FW_PROVIDER_C                        34
#define FID_MMDL_APP__MESH_HEALTH_CLIENT_C                      35
#define FID_MMDL_APP__MESH_PROVISIONER_C                        36
#define FID_MMDL_APP__MESH_PROXY_CLIENT_C                       37

#define FID_MMDL_APP__BLOB_TRANSFER_CLIENT_C                    41
#define FID_MMDL_APP__BLOB_TRANSFER_SERVER_C                    42
#define FID_MMDL_APP__FW_DISTRIBUTOR_C                          43
#define FID_MMDL_APP__FW_UPDATE_SERVER_C                        44
#define FID_MMDL_APP__GENERIC_BATTERY_CLIENT_C                  45
#define FID_MMDL_APP__GENERIC_BATTERY_SERVER_C                  46
#define FID_MMDL_APP__GENERIC_DEFAULT_TRANSITION_TIME_CLIENT_C  47
#define FID_MMDL_APP__GENERIC_DEFAULT_TRANSITION_TIME_SERVER_C  48
#define FID_MMDL_APP__GENERIC_LEVEL_CLIENT_C                    49
#define FID_MMDL_APP__GENERIC_LEVEL_SERVER_C                    50
#define FID_MMDL_APP__GENERIC_LOCATION_CLIENT_C                 51
#define FID_MMDL_APP__GENERIC_LOCATION_SERVER_C                 52
#define FID_MMDL_APP__GENERIC_ONOFF_CLIENT_C                    53
#define FID_MMDL_APP__GENERIC_ONOFF_SERVER_C                    54
#define FID_MMDL_APP__GENERIC_POWER_LEVEL_CLIENT_C              55
#define FID_MMDL_APP__GENERIC_POWER_LEVEL_SERVER_C              56
#define FID_MMDL_APP__GENERIC_POWER_ONOFF_CLIENT_C              57
#define FID_MMDL_APP__GENERIC_POWER_ONOFF_SERVER_C              58
#define FID_MMDL_APP__GENERIC_PROPERTY_CLIENT_C                 59
#define FID_MMDL_APP__GENERIC_PROPERTY_SERVER_C                 60
#define FID_MMDL_APP__LIGHT_CTL_CLIENT_C                        61
#define FID_MMDL_APP__LIGHT_CTL_SERVER_C                        62
#define FID_MMDL_APP__LIGHT_HSL_CLIENT_C                        63
#define FID_MMDL_APP__LIGHT_HSL_SERVER_C                        64
#define FID_MMDL_APP__LIGHT_LC_CLIENT_C                         65
#define FID_MMDL_APP__LIGHT_LC_SERVER_C                         66
#define FID_MMDL_APP__LIGHT_LIGHTNESS_CLIENT_C                  67
#define FID_MMDL_APP__LIGHT_LIGHTNESS_SERVER_C                  68
#define FID_MMDL_APP__LIGHT_XYL_CLIENT_C                        69
#define FID_MMDL_APP__LIGHT_XYL_SERVER_C                        70
#define FID_MMDL_APP__MESH_GATT_CLIENT_C                        71
#define FID_MMDL_APP__MESH_MODEL_SERVER_C                       72
#define FID_MMDL_APP__MESH_MODEL_UTILS_C                        73
#define FID_MMDL_APP__REMOTE_PROVISION_SERVER_C                 74
#define FID_MMDL_APP__SCENE_CLIENT_C                            75
#define FID_MMDL_APP__SCENE_SERVER_C                            76
#define FID_MMDL_APP__SCHEDULER_CLIENT_C                        77
#define FID_MMDL_APP__SCHEDULER_SERVER_C                        78
#define FID_MMDL_APP__SENSOR_CLIENT_C                           79
#define FID_MMDL_APP__SENSOR_SERVER_C                           80
#define FID_MMDL_APP__TIME_CLIENT_C                             81
#define FID_MMDL_APP__TIME_SERVER_C                             82

// for non-embedded build get rid of TRACE... definitions in afx.h
#if defined(MESH_OVER_GATT_ONLY)
#undef MMDL_TRACE0
#undef MMDL_TRACE1
#undef MMDL_TRACE2
#undef MMDL_TRACE3
#endif

// for embedded build
#if !defined(MESH_OVER_GATT_ONLY)

// enable wiced trace if it is disabled but mesh core trace is enabled, because mesh core traces are using WICED_BT_TRACE_ENABLE macros.
#if !defined(WICED_BT_TRACE_ENABLE)
#define WICED_BT_TRACE_ENABLE
#endif

#include "wiced_bt_trace.h"
#endif

// array of trace levels for all source files
extern uint8_t  mesh_model_trace_level;

#if (!defined(MESH_OVER_GATT_ONLY) && !defined(WICED_BT_MESH_TRACE_ENABLE))
#define MMDL_TRACE0(l, s)
#define MMDL_TRACE1(l, s, p1)
#define MMDL_TRACE2(l, s, p1, p2)
#define MMDL_TRACE3(l, s, p1, p2, p3)
#define MMDL_TRACE4(l, s, p1, p2, p3, p4)
#define MMDL_TRACEN(l, p, len)
#else   // (TRACE_COMPILE_LEVEL == 0) || (!defined(MESH_OVER_GATT_ONLY) && !defined(WICED_BT_MESH_CORE_TRACE_ENABLE))
// for embedded build
#if !defined(MESH_OVER_GATT_ONLY)

// prints bytes array. It is replacement of the related wiced function which prints bad trace when it works via wiced HCI.
extern void mesh_dump_hex(uint8_t* p, uint32_t len);

#define MMDL_TRACEN(l, p, len) {if(l <= mesh_model_trace_level) mesh_dump_hex((uint8_t*)(p), (uint16_t)(len));}

// if we want to use encoded trace mode (when trace texts are removed from the binary library)
#ifndef MESH_CORE_DONT_USE_TRACE_ENCODER

void mesh_trace(uint32_t fid, uint32_t line, uint32_t var);

// Trace Variable Format
// uint32_t x 1
#define TVF_D(x)            (uint32_t)(x)
// uint16_t x 2
#define TVF_WW(x,y)         (uint32_t)((((uint32_t)(x) << 16) & 0xFFFF0000) | ((uint32_t)(y) & 0x0000FFFF))
// uint8_t x 2
#define TVF_BB(x,y)         (uint32_t)((((uint32_t)(x) <<  8) & 0x0000FF00) | ((uint32_t)(y) & 0x000000FF))
// uint8_t x 2 + uint16_t
#define TVF_BBW(x,y,z)      TVF_WW(TVF_BB(x,y), z)
// uint8_t x 4
#define TVF_BBBB(x,y,z,w)   TVF_WW(TVF_BB(x, y), TVF_BB(z, w))

#ifndef WICEDX_LINUX

#define MMDL_TRACE0(l, s)                    {if(l <= mesh_model_trace_level) mesh_trace(FID, __LINE__, 0);}
#define MMDL_TRACE1(l, s, p1)                {if(l <= mesh_model_trace_level) mesh_trace(FID, __LINE__, (uint32_t)(p1));}
#define MMDL_TRACE2(l, s, p1, p2)            {if(l <= mesh_model_trace_level) mesh_trace(FID, __LINE__, TVF_WW((uint16_t)(p1), (uint16_t)(p2)));}
#define MMDL_TRACE3(l, s, p1, p2, p3)        {if(l <= mesh_model_trace_level) mesh_trace(FID, __LINE__, TVF_BBW((uint8_t)(p1), (uint8_t)(p2), (uint16_t)(p3)));}
#define MMDL_TRACE4(l, s, p1, p2, p3, p4)    {if(l <= mesh_model_trace_level) mesh_trace(FID, __LINE__, TVF_BBBB((uint8_t)(p1), (uint8_t)(p2), (uint8_t)(p3), (uint8_t)(p4)));}
#define MMDL_TRACEN(l, p, len)               {if(l <= mesh_model_trace_level) mesh_dump_hex((uint8_t*)(p), (uint16_t)(len));}

#else

void ble_trace0(const char* p_str);
void ble_trace1(const char* fmt_str, uint32_t p1);
void ble_trace2(const char* fmt_str, uint32_t p1, uint32_t p2);
void ble_trace3(const char* fmt_str, uint32_t p1, uint32_t p2, uint32_t p3);
void ble_trace4(const char* fmt_str, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4);
void ble_tracen(const char* p_str, uint32_t len);

#define MMDL_TRACE0(l, s)                    {if(l <= mesh_model_trace_level) ble_trace0(s);}
#define MMDL_TRACE1(l, s, p1)                {if(l <= mesh_model_trace_level) ble_trace1(s, p1);}
#define MMDL_TRACE2(l, s, p1, p2)            {if(l <= mesh_model_trace_level) ble_trace2(s, p1, p2);}
#define MMDL_TRACE3(l, s, p1, p2, p3)        {if(l <= mesh_model_trace_level) ble_trace3(s, p1, p2, p3);}
#define MMDL_TRACE4(l, s, p1, p2, p3, p4)    {if(l <= mesh_model_trace_level) ble_trace4(s, p1, p2, p3, p4);}
#define MMDL_TRACEN(l, p, len)               {if(l <= mesh_model_trace_level) mesh_dump_hex(p, len);}
#endif

#else   // MESH_CORE_DONT_USE_TRACE_ENCODER
// we don't want to use encoded trace mode

// if we want to print node time at each trace line then use mesh_trace_time()
#ifdef MESH_TRACE_TIME
void mesh_trace_time(void);
#define MESH_TRACE_TIME_FUNC     mesh_trace_time
#else
#define MESH_TRACE_TIME_FUNC()
#endif

#define MMDL_TRACE0(l, s)                    {if(l <= mesh_model_trace_level) { MESH_TRACE_TIME_FUNC(); WICED_BT_TRACE(s); } }
#define MMDL_TRACE1(l, s, p1)                {if(l <= mesh_model_trace_level) { MESH_TRACE_TIME_FUNC(); WICED_BT_TRACE(s, p1); } }
#define MMDL_TRACE2(l, s, p1, p2)            {if(l <= mesh_model_trace_level) { MESH_TRACE_TIME_FUNC(); WICED_BT_TRACE(s, p1, p2); } }
#define MMDL_TRACE3(l, s, p1, p2, p3)        {if(l <= mesh_model_trace_level) { MESH_TRACE_TIME_FUNC(); WICED_BT_TRACE(s, p1, p2, p3); } }
#define MMDL_TRACE4(l, s, p1, p2, p3, p4)    {if(l <= mesh_model_trace_level) { MESH_TRACE_TIME_FUNC(); WICED_BT_TRACE(s, p1, p2, p3, p4); } }

#endif  // MESH_CORE_DONT_USE_TRACE_ENCODER
#else   // #ifndef MESH_OVER_GATT_ONLY
// For non-embedded build
extern void ods(char* fmt_str, ...);

#define WICED_BT_TRACE ods

void ble_trace0(const char* p_str);
void ble_trace1(const char* fmt_str, uint32_t p1);
void ble_trace2(const char* fmt_str, uint32_t p1, uint32_t p2);
void ble_trace3(const char* fmt_str, uint32_t p1, uint32_t p2, uint32_t p3);
void ble_trace4(const char* fmt_str, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4);
void ble_tracen(const char* p_str, uint32_t len);

#define MMDL_TRACE0(l, s)                    {if(l <= mesh_model_trace_level) ble_trace0(s);}
#define MMDL_TRACE1(l, s, p1)                {if(l <= mesh_model_trace_level) ble_trace1(s, p1);}
#define MMDL_TRACE2(l, s, p1, p2)            {if(l <= mesh_model_trace_level) ble_trace2(s, p1, p2);}
#define MMDL_TRACE3(l, s, p1, p2, p3)        {if(l <= mesh_model_trace_level) ble_trace3(s, p1, p2, p3);}
#define MMDL_TRACE4(l, s, p1, p2, p3, p4)    {if(l <= mesh_model_trace_level) ble_trace4(s, p1, p2, p3, p4);}
#define MMDL_TRACEN(l, p, len)               {if(l <= mesh_model_trace_level) ble_tracen(p, len);}

#endif  // #ifndef MESH_OVER_GATT_ONLY
#endif  // (TRACE_COMPILE_LEVEL == 0) || (!defined(MESH_OVER_GATT_ONLY) && !defined(WICED_BT_MESH_CORE_TRACE_ENABLE))

#else // !define WICED_BT_MESH_TRACE_ENABLE
#define MMDL_TRACE0(l, s)
#define MMDL_TRACE1(l, s, p1)
#define MMDL_TRACE2(l, s, p1, p2)
#define MMDL_TRACE3(l, s, p1, p2, p3)
#define MMDL_TRACE4(l, s, p1, p2, p3, p4)
#define MMDL_TRACEN(l, p, len)


// // enable wiced trace if it is disabled but mesh trace is enabled
// #if !defined(WICED_BT_TRACE_ENABLE)
// #define WICED_BT_TRACE_ENABLE
// #endif
// #define WICED_BT_MESH_MODEL_TRACE(...) {if (wiced_bt_mesh_model_trace_enabled) WICED_BT_TRACE(__VA_ARGS__); }
// #define WICED_BT_MESH_MODEL_TRACE_ARRAY(p, l) {if (wiced_bt_mesh_model_trace_enabled) mesh_dump_hex((p), (l)); }
// #else
// #define WICED_BT_MESH_MODEL_TRACE(...)
// #define WICED_BT_MESH_MODEL_TRACE_ARRAY(p, l)
//
// #include "wiced_bt_trace.h"
//
// extern uint8_t wiced_bt_mesh_model_trace_enabled;

#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern wiced_bt_mesh_core_config_t *mesh_core_config;

extern void mesh_dump_hex(uint8_t *p, uint32_t len);

// Generic definition for all model handle functions
typedef wiced_bool_t (*wiced_bt_mesh_model_handle_cb_t) (uint8_t element_idx, uint16_t src_id, uint16_t app_key_idx, uint8_t ttl, uint16_t opcode, const uint8_t *params, uint16_t params_len);

typedef PACKED struct
{
    uint16_t                        model_id;
    wiced_bt_mesh_model_handle_cb_t model_handle;
} wiced_mesh_model_callback_list_item_t;

typedef PACKED struct
{
    uint8_t                                max_handles;
    uint8_t                                registered_handles_cnt;
    wiced_mesh_model_callback_list_item_t  cb_list[WICED_BT_MESH_MODELS_MAX_UNIQUE_MODELS_ON_NODE];
} wiced_mesh_model_handler_list_t;

typedef PACKED struct
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t days_in_month;
    uint8_t day_of_week;
} wiced_bt_mesh_time_t;

//Insert given multi octet value into given buffer in LITTLE ENDIEN FORMAT
//Buffer pointer must be a byte array
#define SETBUF_LE8(val8, buf_ptr, len_var)         buf_ptr[len_var++] = val8 & 0xFF;
#define SETBUF_LE16(val16, buf_ptr, len_var)       buf_ptr[len_var++] = (uint16_t)val16 & 0x00FF; buf_ptr[len_var++] = (((uint16_t)val16 & 0xFF00) >> 8);
#define SETBUF_LE32(val32, buf_ptr, len_var)       buf_ptr[len_var++] = (uint32_t)val32 & 0x00FF; buf_ptr[len_var++] = (((uint32_t)val32 & 0xFF00) >> 8);buf_ptr[len_var++] = (((uint32_t)val32 & 0xFF0000) >> 16);buf_ptr[len_var++] = (((uint32_t)val32 & 0xFF000000) >> 24);

#define SETBUF_LE8_ptr(val8, buf_ptr)              *buf_ptr++ = val8 & 0xFF;
#define SETBUF_LE16_ptr(val16, buf_ptr)            *buf_ptr++ = val16 & 0x00FF; *buf_ptr++ = ((val16 & 0xFF00) >> 8);
#define SETBUF_LE24_ptr(val24, buf_ptr)            *buf_ptr++ = val24 & 0x00FF; *buf_ptr++ = ((val24 & 0xFF00) >> 8); *buf_ptr++ = ((val24 & 0xFF0000) >> 16);
#define SETBUF_LE32_ptr(val32, buf_ptr)            *buf_ptr++ = val32 & 0x00FF; *buf_ptr++ = ((val32 & 0xFF00) >> 8); *buf_ptr++ = ((val32 & 0xFF0000) >> 16); *buf_ptr++ = ((val32 & 0xFF000000) >> 24);


//Extracts multi octate value in LITTLE ENDIEN format from given buffer location w.r.t to beginning of the buffer
//begining of buffer is index zero i.e. buf[0].
#define GETBUF_LE16(buf_ptr, start)        (buf_ptr[start] | (buf_ptr[start+1] << 8))
#define GETBUF_LE24(buf_ptr, start)        (buf_ptr[start] | (buf_ptr[start+1] << 8) | (buf_ptr[start+2] << 16))
#define GETBUF_LE32(buf_ptr, start)        (buf_ptr[start] | (buf_ptr[start+1] << 8) | (buf_ptr[start+2] << 16) | (buf_ptr[start+3] << 24))


// Generic Default Transition Time state
#define GENDEFTRANSTIME_STEP_RESOLUTION_MASK        0xC0
#define GENDEFTRANSTIME_STEP_RESOLUTION_100MS       0x00
#define GENDEFTRANSTIME_STEP_RESOLUTION_1S          0x40
#define GENDEFTRANSTIME_STEP_RESOLUTION_10S         0x80
#define GENDEFTRANSTIME_STEP_RESOLUTION_10M         0xC0

#define GENDEFTRANSTIME_STEPS_MASK                  0x3F
#define GENDEFTRANSTIME_STEPS_IMMEDIATE             0x00
#define GENDEFTRANSTIME_STEPS_MAX                   0x3E
#define GENDEFTRANSTIME_STEPS_UNKNOWN               0x3F

#define GET_OPCODE_LEN(opcode)     (opcode <= 0x7E ? 1 : (opcode >=0x8000 && opcode <= 0xBFFF) ? 2 : (opcode>=0xC00000) ? 3 : 0xFF)

/**
* Converts the given encoded transition time value in to the
* units of SMART_MESH_APP_FINE_TIMEOUT_IN_MS.
*
* Parameters:
*   trans_time    :Encoded transition time value as defined in
*                model spec
*   Return        :value in number of fine timer ticks. Returns
*                zero for invalid value
*/
uint32_t get_transtime_in_ms(uint8_t trans_time);

/**
* Returns the number of fine timer ticks from the power-up
*
* Parameters:
*   <none>
*
*   Return        : Number of app ine timer ticks
*/
uint64_t get_app_finetimer_ticks(void);

/**
* Converts the given opcode to the corresponding byte stream
* Note: It does not check the validity of the opcode.
*
* Parameters:
*   *p:            Pointer to the stream buffer
*    op:            1/2/3 byte opcode
*   Return:        pointer to the next empty location in the stream buffer
*               after adding opcode bytes
*/
uint8_t* mesh_opcode_to_stream(uint8_t *p, uint32_t op);

/**
* Converts the given transition time in milliseconds to correctly encoded value
* as per the mesh spec
* Parameters:
*   time_ms        : Transition time in milliseconds. Note, final transition time accuracy
*                   will depend upon the length of the time.
*
*   Return        : Encoded transition time
*/
uint8_t mesh_time_ms_to_transtime(uint32_t time_ms);

/**
 * Initialize mesh transition server
 * Parameters:
 *   element_idx Index of the element of the device
 *   p_ref_data Reference data to be returned in the callback
 */
void mesh_model_transition_server_init(uint8_t element_idx, wiced_bool_t is_provisioned);

/**
 * Callback from the Transition procedure that the values have been updated
 * Parameters:
 *   p_present  Array of present values.  Sizeof the array is the same as when transaction was started
 *   ref_data   Pinter as passed to the transaction start
 *   remaining_time Remaining time in the transition.  If 0, the transition is completed and this is the last time the callback is executed.
 */
typedef void (wiced_bt_mesh_transition_callback_t) (uint32_t *p_present, void *ref_data, uint32_t remaining_time);

/**
 * Start the Transition procedure
 * Parameters:
 *   element_idx Index of the element of the device
 *   transition_time Transition time in milliseconds
 *   delay Delay before starting the transition
 *   target Array of the target values to transition to
 *   present Array of the present values to transition from
 *   num_values Number of elements in the Target and Present arrays
 *   p_callback Callback to be executed on the transition process and when transition is completed
 *   p_ref_data Reference data to be returned in the callback
 */
void mesh_server_transition_start(uint8_t element_idx, uint32_t transition_time, uint32_t delay, uint32_t *target, uint32_t *present, uint32_t num_values, wiced_bt_mesh_transition_callback_t *p_callback, void *p_ref_data);

/**
 * Start the Transition procedure
 * Parameters:
 *   element_idx Index of the element of the device
 */
void mesh_server_transition_abort(uint8_t element_idx);

/**
 * \brief Return the value of the default transition time set for the element in milliseconds.
 *
 * @param       element_idx Index of the element on the device
 *
 * @return      default transition time value.
 */
uint32_t mesh_default_transition_time_get(uint8_t element_idx);

/**
 * Registers the given SIG model handler into the list of handlers to be called during opcode processing
 */
wiced_bool_t mesh_sig_model_handle_register(wiced_bt_mesh_model_handle_cb_t model_handle, uint16_t model_id);

/**
 * This function can be called to cancel sending of the current message.
 * If request is in the queue, the callback is executed.
 */
void wiced_bt_mesh_models_utils_ack_received(wiced_bt_mesh_event_t **p_out_event_queue, uint16_t dst);

/**
 * This function can be called to check if message with one of the opcodes is in the queue for transmission.
 */
wiced_bool_t wiced_bt_mesh_models_utils_opcode_queued(wiced_bt_mesh_event_t **p_out_event_queue, uint16_t dst, uint16_t *opcode_list, int num_opcodes);

/**
 * This request can be used to cancel outstanding writes to a specific destination from the specific queue.
 * No callbacks will be executed.
 */
void wiced_bt_mesh_models_utils_cancel_send(wiced_bt_mesh_event_t **p_out_event_queue, uint16_t dst);

/**
 * This function can be called send message through the core.  The function keeps track of the output event and executes provided callback
 * if message is not canceled.
 *
 * @param       p_event Fully prepared @ref wiced_bt_mesh_event_t structure with information on where and how event should be sent
 * @param       p_out_event Pointer to the queue of the events sent by the current modules. The pointer shall not be null if there is callback
 * @param       is_command If true the packet a command originated by the current device. Based on this parameter core selects different value for random delay before the packet is scheduled for transmission.
 * @param       buffer Pointer to the data to be sent out.
 * @param       len Size of the data to be sent out. The maximum length is about 377 bytes (depends on the size of the opcode).
 * @param       complete_callback The pointer to the callback to be executed when the packet is queued for the transmission. If parameter is null, the core assumes the application releases the ownership of the p_event. If not null, application shall release the p_event, typically in the contents of the callback.
 *
 * @return      WICED_TRUE if success.
 */
wiced_result_t wiced_bt_mesh_models_utils_send(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_event_t **p_out_event, wiced_bool_t is_command, uint16_t opcode, uint8_t *p_buffer, uint16_t len, wiced_bt_mesh_core_send_complete_callback_t complete_callback);

/**
 * This function can be called to extend reply timeout, for example, when a friend has replied on behalf of an LPN
 *
 * @param       p_event event that has been used in initial wiced_bt_mesh_models_utils_send function
 *
 * @return      WICED_TRUE if success.
 */
wiced_bool_t wiced_bt_mesh_models_utils_update_reply_timeout(wiced_bt_mesh_event_t* p_event);

/**
 * The access layer has completed sending of the packet.
 * Returns TRUE if event was dequeued.  FALSE if timer is still running and another complete will follow when timer expires.
 */
wiced_bool_t wiced_bt_mesh_models_utils_send_complete(wiced_bt_mesh_event_t **p_out_event_queue, uint16_t dst);

/**
 * Cancel all send requests.
 */
void wiced_bt_mesh_models_utils_cancel_all(wiced_bt_mesh_event_t **p_out_event_queue);

extern uint32_t utl_sqrt(uint32_t input);

/**
 * \brief Return the value of the OnPowerUp state.
 *
 * @param       element_idx Index of the element on the device
 * @param       onpowerup Pointer to the variable to be filled with current on power up state.
 *
 * @return      WICED_TRUE if success.
*/
uint8_t mesh_power_onoff_server_onpowerup_get(uint8_t element_idx);

/*
* Get Light Lightness Actual state
*/
uint16_t mesh_light_lightness_actual_get(uint8_t element_idx);

/*
* Set Light Lightness Actual state
*/
void mesh_light_lightness_actual_set(uint8_t element_idx, uint16_t level, uint32_t remaining_time);

/*
 * Get Value of the Light Lightness Linear state
 */
uint16_t mesh_light_lightness_linear_get(uint8_t element_idx);

/*
 * Set Value of the Light Lightness Linear state
 */
void mesh_light_lightness_linear_set(uint8_t element_idx, uint16_t value, uint32_t remaining_time);

/*
 * Get Value of the Light Lightness Last state
 */
uint16_t mesh_light_lightness_server_last_get(uint8_t element_idx);

/*
 * Set Value of the Light Lightness Last state
 */
void mesh_light_lightness_server_last_set(uint8_t element_idx, uint16_t value);

/*
 * Get Value of the Light Lightness Linear Last state
 */
uint16_t mesh_light_lightness_server_linear_last_get(uint8_t element_idx);

/*
 * Set Value of the Light Lightness Linear Last state
 */
void mesh_light_lightness_server_linear_last_set(uint8_t element_idx, uint16_t value);

/*
 * Process command from the light controller to transition to new target linear lightness
 */
void mesh_light_lightness_server_lc_transition_start(uint8_t element_idx, uint16_t actual_target, uint16_t linear_target, uint32_t lux_level_current, uint32_t lux_level_target, uint32_t transition_time, uint32_t delay);

/*
 * Return current Light Lightness Default value
 */
uint16_t mesh_light_lightness_server_default_get(uint8_t element_idx);

uint16_t mesh_light_lightness_actual_last_get(uint8_t element_idx);
uint16_t mesh_light_lightness_linear_last_get(uint8_t element_idx);

/*
* Set current Light Lightness Default value
*/
void mesh_light_lightness_server_default_set(uint8_t element_idx, uint16_t default_value);

void mesh_light_lightness_set(uint8_t element_idx, wiced_bool_t store, uint16_t linear, uint16_t actual, uint32_t remaining_time);
void light_lightness_actual_set(uint8_t element_idx, uint16_t value);
void light_lightness_linear_set(uint8_t element_idx, uint16_t value);

/*
 * Set target onoff state for the element
 */
void mesh_power_onoff_server_target_onoff_set(uint8_t element_idx, uint8_t onoff);

/*
 * Get target onoff state for the element
 */
uint8_t mesh_power_onoff_server_target_onoff_get(uint8_t element_idx);

/*
 * Set present onoff state for the element
 */
void mesh_power_onoff_server_present_onoff_set(uint8_t element_idx, uint8_t onoff);

uint8_t mesh_power_onoff_server_get_onoff(uint8_t element_idx);
void    mesh_power_onoff_server_set_onoff(uint8_t element_idx, uint8_t onoff, uint32_t remaining_time);
uint8_t mesh_onoff_server_get_onoff(uint8_t element_idx);

/*
 * Set target onoff state for the element
 */
void mesh_onoff_server_target_onoff_set(uint8_t element_idx, uint8_t onoff);

/*
 * Get target onoff state for the element
 */
uint8_t mesh_onoff_server_target_onoff_get(uint8_t element_idx);

/*
 * Set present onoff state for the element
 */
void mesh_onoff_server_present_onoff_set(uint8_t element_idx, uint8_t onoff);

void mesh_onoff_server_onoff_set(uint8_t element_idx, uint8_t onoff, uint32_t remaining_time);

void mesh_onoff_execute_set_onoff(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_onoff_set_data_t *p_onoff_set);
void mesh_scene_server_execute_recall(wiced_bt_mesh_event_t *p_event, uint16_t scene_number, uint32_t transition_time, uint32_t delay);

/*
 * Set new level
 */
void mesh_level_server_level_set(uint8_t element_idx, int16_t level, uint32_t remaining_time);

/*
 * Get level of the element
 */
int16_t mesh_level_server_level_get(uint8_t element_idx);

/*
 * Get current time
 */
wiced_bool_t mesh_time_server_get(wiced_bt_mesh_time_t *p_current_time);

// definitions from mesh_core.h. Otherwise models uses these function without definition
extern uint32_t mesh_read_node_info(int id, uint8_t* data, uint16_t len, wiced_result_t *p_result);
extern uint32_t mesh_write_node_info(int id, const uint8_t* data, uint16_t len, wiced_result_t *p_result);

uint8_t uint32_to_log(uint32_t period);
uint32_t uint32_from_log(uint8_t log);

// same as uint32_to_log() but for heartbeat publication/subscription count/period - it is based on ext**(log-1)
uint8_t uint32_to_log_heartbeat(uint32_t period);
// same as uint32_from_log() but for heartbeat publication/subscription count/period - it is based on ext**(log-1)
uint32_t uint32_from_log_heartbeat(uint8_t log);

// A device may have 1, 2, or 3 models implemented to control the color. One of them is running as a startup
// controller, and tell application about the currently set color
#define MESH_UTILS_COLOR_CONTROLLER_HSL   0
#define MESH_UTILS_COLOR_CONTROLLER_CTL   1
#define MESH_UTILS_COLOR_CONTROLLER_XYL   2
void mesh_model_set_color_light_controller(uint8_t element_idx, uint8_t controller);

#ifndef TIMER_PARAM_TYPE
#define TIMER_PARAM_TYPE WICED_TIMER_PARAM_TYPE
#endif

#ifdef NEW_WICED_STACK
#define wiced_memory_permanent_allocate wiced_bt_get_buffer
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MESH_MODEL_UTILS_H__ */
