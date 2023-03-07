
/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef LMAC_MSG_H_
#define LMAC_MSG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
// for MAC related elements (mac_addr, mac_ssid...)
#include "lmac_types.h"
#include <bl60x_fw_api.h>
#include <wifi_mgmr_ext.h>
#include "lmac_mac.h"
#define MAX_PSK_PASS_PHRASE_LEN 64

/*
 ****************************************************************************************
 */

/// Power Save mode setting
enum mm_ps_mode_state
{
    MM_PS_MODE_OFF,
    MM_PS_MODE_ON,
    MM_PS_MODE_ON_DYN,
};

/// Status/error codes used in the MAC software.
enum
{
    CO_OK,
    CO_FAIL,
    CO_EMPTY,
    CO_FULL,
    CO_BAD_PARAM,
    CO_NOT_FOUND,
    CO_NO_MORE_ELT_AVAILABLE,
    CO_NO_ELT_IN_USE,
    CO_BUSY,
    CO_OP_IN_PROGRESS,
};

/// WiFi Mode
typedef enum {
    /// 802.ll b
    WIFI_MODE_802_11B       = 0x01,
    /// 802.11 a
    WIFI_MODE_802_11A       = 0x02,
    /// 802.11 g
    WIFI_MODE_802_11G       = 0x04,
    /// 802.11n at 2.4GHz
    WIFI_MODE_802_11N_2_4   = 0x08,
    /// 802.11n at 5GHz
    WIFI_MODE_802_11N_5     = 0x10,
    /// 802.11ac at 5GHz
    WIFI_MODE_802_11AC_5    = 0x20,
    /// Reserved for future use
    WIFI_MODE_RESERVED      = 0x40,
} WiFi_Mode_t;

/// Remain on channel operation codes
enum mm_remain_on_channel_op
{
    MM_ROC_OP_START = 0,
    MM_ROC_OP_CANCEL,
};

#define DRV_TASK_ID 100

#define MSG_T(msg) (((msg) >> 10))
#define MSG_I(msg) ((msg) & ((1<<10)-1))

/// The two following definitions are needed for message structure consistency
/// Structure of a list element header
/// Message structure.
struct lmac_msg
{
    ke_msg_id_t     id;         ///< Message id.
    ke_task_id_t    dest_id;    ///< Destination kernel identifier.
    ke_task_id_t    src_id;     ///< Source kernel identifier.
    u32        param_len;  ///< Parameter embedded struct length.
    u32        param[];   ///< Parameter embedded struct. Must be word-aligned.
};

/// Interface types
enum
{
    /// ESS STA interface
    MM_STA,
    /// IBSS STA interface
    MM_IBSS,
    /// AP interface
    MM_AP,
    // Mesh Point interface
    MM_MESH_POINT,
};

/// Maximum number of words in the configuration buffer
#define PHY_CFG_BUF_SIZE     16

/// Structure containing the parameters of the PHY configuration
struct phy_cfg_tag
{
    /// Buffer containing the parameters specific for the PHY used
    u32_l parameters[PHY_CFG_BUF_SIZE];
};

/// Structure containing the parameters of the @ref MM_MONITOR_CFM message.
struct mm_monitor_cfm
{
    uint32_t status;
    uint32_t enable;
    uint32_t data[8];
};

/// Structure containing the parameters of the Trident PHY configuration
struct phy_trd_cfg_tag
{
    /// MDM type(nxm)(upper nibble) and MDM2RF path mapping(lower nibble)
    u8_l path_mapping;
    /// TX DC offset compensation
    u32_l tx_dc_off_comp;
};

/// Structure containing the parameters of the Karst PHY configuration
struct phy_karst_cfg_tag
{
    /// TX IQ mismatch compensation in 2.4GHz
    u32_l tx_iq_comp_2_4G[2];
    /// RX IQ mismatch compensation in 2.4GHz
    u32_l rx_iq_comp_2_4G[2];
    /// TX IQ mismatch compensation in 5GHz
    u32_l tx_iq_comp_5G[2];
    /// RX IQ mismatch compensation in 5GHz
    u32_l rx_iq_comp_5G[2];
    /// RF path used by default (0 or 1)
    u8_l path_used;
};

/// Structure containing the parameters of the @ref MM_START_REQ message
struct mm_start_req
{
    /// PHY configuration
    struct phy_cfg_tag phy_cfg;
    /// UAPSD timeout
    u32_l uapsd_timeout;
    /// Local LP clock accuracy (in ppm)
    u16_l lp_clk_accuracy;
};

/// Structure containing the parameters of the @ref MM_SET_CHANNEL_REQ message
struct mm_set_channel_req
{
    /// Band (2.4GHz or 5GHz)
    u8_l band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    u8_l type;
    /// Frequency for Primary 20MHz channel (in MHz)
    u16_l prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    u16_l center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    u16_l center2_freq;
    /// Index of the RF for which the channel has to be set (0: operating (primary), 1: secondary
    /// RF (used for additional radar detection). This parameter is reserved if no secondary RF
    /// is available in the system
    u8_l index;
    /// Max tx power for this channel
    s8_l tx_power;
};

/// Structure containing the parameters of the @ref MM_SET_CHANNEL_CFM message
struct mm_set_channel_cfm
{
    /// Radio index to be used in policy table
    u8_l radio_idx;
    /// TX power configured (in dBm)
    s8_l power;
};

/// Structure containing the parameters of the @ref MM_SET_DTIM_REQ message
struct mm_set_dtim_req
{
    /// DTIM period
    u8_l dtim_period;
};

/// Structure containing the parameters of the @ref MM_SET_POWER_REQ message
struct mm_set_power_req
{
    /// Index of the interface for which the parameter is configured
    u8_l inst_nbr;
    /// TX power (in dBm)
    s8_l power;
};

/// Structure containing the parameters of the @ref MM_SET_POWER_CFM message
struct mm_set_power_cfm
{
    /// Radio index to be used in policy table
    u8_l radio_idx;
    /// TX power configured (in dBm)
    s8_l power;
};

/// Structure containing the parameters of the @ref MM_SET_BEACON_INT_REQ message
struct mm_set_beacon_int_req
{
    /// Beacon interval
    u16_l beacon_int;
    /// Index of the interface for which the parameter is configured
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_SET_BEACON_INT_CFM message
struct mm_set_beacon_int_cfm
{
    u8_l status;
};

/// Structure containing the parameters of the @ref MM_SET_BASIC_RATES_REQ message
struct mm_set_basic_rates_req
{
    /// Basic rate set (as expected by bssBasicRateSet field of Rates MAC HW register)
    u32_l rates;
    /// Index of the interface for which the parameter is configured
    u8_l inst_nbr;
    /// Band on which the interface will operate
    u8_l band;
};

/// Structure containing the parameters of the @ref MM_SET_BSSID_REQ message
struct mm_set_bssid_req
{
    /// BSSID to be configured in HW
    struct mac_addr bssid;
    /// Index of the interface for which the parameter is configured
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_ADD_IF_REQ message.
struct mm_add_if_req
{
    /// Type of the interface (AP, STA, ADHOC, ...)
    u8_l type;
    /// MAC ADDR of the interface to start
    struct mac_addr addr;
    /// P2P Interface
    bool_l p2p;
};

/// Structure containing the parameters of the @ref MM_SET_EDCA_REQ message
struct mm_set_edca_req
{
    /// EDCA parameters of the queue (as expected by edcaACxReg HW register)
    u32_l ac_param;
    /// Flag indicating if UAPSD can be used on this queue
    bool_l uapsd;
    /// HW queue for which the parameters are configured
    u8_l hw_queue;
    /// Index of the interface for which the parameters are configured
    u8_l inst_nbr;
};

struct mm_set_idle_req
{
    u8_l hw_idle;
};

/// Structure containing the parameters of the @ref MM_SET_MODE_REQ message
struct mm_set_mode_req
{
    /// abgnMode field of macCntrl1Reg register
    u8_l abgnmode;
};

/// Structure containing the parameters of the @ref MM_SET_VIF_STATE_REQ message
struct mm_set_vif_state_req
{
    /// Association Id received from the AP (valid only if the VIF is of STA type)
    u16_l aid;
    /// Flag indicating if the VIF is active or not
    bool_l active;
    /// Interface index
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_ADD_IF_CFM message.
struct mm_add_if_cfm
{
    /// Status of operation (different from 0 if unsuccessful)
    u8_l status;
    /// Interface index assigned by the LMAC
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_REMOVE_IF_REQ message.
struct mm_remove_if_req
{
    /// Interface index assigned by the LMAC
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_VERSION_CFM message.
struct mm_version_cfm
{
    /// Version of the LMAC FW
    u32_l version_lmac;
    /// Version1 of the MAC HW (as encoded in version1Reg MAC HW register)
    u32_l version_machw_1;
    /// Version2 of the MAC HW (as encoded in version2Reg MAC HW register)
    u32_l version_machw_2;
    /// Version1 of the PHY (depends on actual PHY)
    u32_l version_phy_1;
    /// Version2 of the PHY (depends on actual PHY)
    u32_l version_phy_2;
    /// Supported Features
    u32_l features;
};

/// Structure containing the parameters of the @ref MM_STA_ADD_REQ message.
struct mm_sta_add_req
{
    /// PAID/GID
    u32_l paid_gid;
    /// Maximum A-MPDU size, in bytes, for HT frames
    u16_l ampdu_size_max_ht;
    /// MAC address of the station to be added
    struct mac_addr mac_addr;
    /// A-MPDU spacing, in us
    u8_l ampdu_spacing_min;
    /// Interface index
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_STA_ADD_CFM message.
struct mm_sta_add_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    u8_l status;
    /// Index assigned by the LMAC to the newly added station
    u8_l sta_idx;
    /// MAC HW index of the newly added station
    u8_l hw_sta_idx;
};

/// Structure containing the parameters of the @ref MM_STA_DEL_REQ message.
struct mm_sta_del_req
{
    /// Index of the station to be deleted
    u8_l sta_idx;
};

/// Structure containing the parameters of the @ref MM_STA_DEL_CFM message.
struct mm_sta_del_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    u8_l     status;
};

/// Structure containing the parameters of the SET_POWER_MODE REQ message.
struct mm_setpowermode_req
{
    u8_l mode;
    u8_l sta_idx;
};

/// Structure containing the parameters of the SET_POWER_MODE CFM message.
struct mm_setpowermode_cfm
{
    u8_l     status;
};

/// Structure containing the parameters of the @ref MM_MONITOR_REQ message.
struct mm_monitor_req
{
    uint32_t enable;
};

/// Structure containing the parameters of the @ref MM_MONITOR_CHANNEL_REQ message.
struct mm_monitor_channel_req
{
    uint32_t freq;
};

/// Structure containing the parameters of the @ref MM_MONITOR_CHANNEL_CFM message.
struct mm_monitor_channel_cfm
{
    uint32_t status;
    uint32_t freq;
    uint32_t data[8];
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_ADD_REQ message
struct mm_chan_ctxt_add_req
{
    /// Band (2.4GHz or 5GHz)
    u8_l band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    u8_l type;
    /// Frequency for Primary 20MHz channel (in MHz)
    u16_l prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    u16_l center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    u16_l center2_freq;
    /// Max tx power for this channel
    s8_l tx_power;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_ADD_REQ message
struct mm_chan_ctxt_add_cfm
{
    /// Status of the addition
    u8_l status;
    /// Index of the new channel context
    u8_l index;
};


/// Structure containing the parameters of the @ref MM_CHAN_CTXT_DEL_REQ message
struct mm_chan_ctxt_del_req
{
    /// Index of the new channel context to be deleted
    u8_l index;
};


/// Structure containing the parameters of the @ref MM_CHAN_CTXT_LINK_REQ message
struct mm_chan_ctxt_link_req
{
    /// VIF index
    u8_l vif_index;
    /// Channel context index
    u8_l chan_index;
    /// Indicate if this is a channel switch (unlink current ctx first if true)
    u8_l chan_switch;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_UNLINK_REQ message
struct mm_chan_ctxt_unlink_req
{
    /// VIF index
    u8_l vif_index;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_UPDATE_REQ message
struct mm_chan_ctxt_update_req
{
    /// Channel context index
    u8_l chan_index;
    /// Band (2.4GHz or 5GHz)
    u8_l band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    u8_l type;
    /// Frequency for Primary 20MHz channel (in MHz)
    u16_l prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    u16_l center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    u16_l center2_freq;
};

/// Structure containing the parameters of the @ref MM_CHAN_CTXT_SCHED_REQ message
struct mm_chan_ctxt_sched_req
{
    /// VIF index
    u8_l vif_index;
    /// Channel context index
    u8_l chan_index;
    /// Type of the scheduling request (0: normal scheduling, 1: derogatory
    /// scheduling)
    u8_l type;
};

/// Structure containing the parameters of the @ref MM_CHANNEL_SWITCH_IND message
struct mm_channel_switch_ind
{
    /// Index of the channel context we will switch to
    u8_l chan_index;
    /// Indicate if the switch has been triggered by a Remain on channel request
    bool_l roc;
    /// VIF on which remain on channel operation has been started (if roc == 1)
    u8_l vif_index;
    /// Indicate if the switch has been triggered by a TDLS Remain on channel request
    bool_l roc_tdls;
};

/// Structure containing the parameters of the @ref MM_CHANNEL_PRE_SWITCH_IND message
struct mm_channel_pre_switch_ind
{
    /// Index of the channel context we will switch to
    u8_l chan_index;
};

/// Structure containing the parameters of the @ref MM_CONNECTION_LOSS_IND message.
struct mm_connection_loss_ind
{
    /// VIF instance number
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_SET_PS_MODE_REQ message.
struct mm_set_ps_mode_req
{
    /// Power Save is activated or deactivated
    u8_l  new_state;
};

struct mm_set_denoise_req
{
    u8_l  denoise_mode;
};

/// Structure containing the parameters of the @ref MM_BCN_CHANGE_REQ message.
#define BCN_MAX_CSA_CPT 2
struct mm_bcn_change_req
{
    /// Pointer, in host memory, to the new beacon template
    u32_l bcn_ptr;
    /// Length of the beacon template
    u16_l bcn_len;
    /// Offset of the TIM IE in the beacon
    u16_l tim_oft;
    /// Length of the TIM IE
    u8_l tim_len;
    /// Index of the VIF for which the beacon is updated
    u8_l inst_nbr;
    /// Offset of CSA (channel switch announcement) counters (0 means no counter)
    u8_l csa_oft[BCN_MAX_CSA_CPT];
};


/// Structure containing the parameters of the @ref MM_TIM_UPDATE_REQ message.
struct mm_tim_update_req
{
    /// Association ID of the STA the bit of which has to be updated (0 for BC/MC traffic)
    u16_l aid;
    /// Flag indicating the availability of data packets for the given STA
    u8_l tx_avail;
    /// Index of the VIF for which the TIM is updated
    u8_l inst_nbr;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_REQ message.
struct mm_remain_on_channel_req
{
    /// Operation Code
    u8_l op_code;
    /// VIF Index
    u8_l vif_index;
    /// Band (2.4GHz or 5GHz)
    u8_l band;
    /// Channel type: 20,40,80,160 or 80+80 MHz
    u8_l type;
    /// Frequency for Primary 20MHz channel (in MHz)
    u16_l prim20_freq;
    /// Frequency for Center of the contiguous channel or center of Primary 80+80
    u16_l center1_freq;
    /// Frequency for Center of the non-contiguous secondary 80+80
    u16_l center2_freq;
    /// Duration (in ms)
    u32_l duration_ms;
    /// TX power (in dBm)
    s8_l tx_power;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_CFM message
struct mm_remain_on_channel_cfm
{
    /// Operation Code
    u8_l op_code;
    /// Status of the operation
    u8_l status;
    /// Channel Context index
    u8_l chan_ctxt_index;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_EXP_IND message
struct mm_remain_on_channel_exp_ind
{
    /// VIF Index
    u8_l vif_index;
    /// Channel Context index
    u8_l chan_ctxt_index;
};

/// Structure containing the parameters of the @ref MM_SET_UAPSD_TMR_REQ message.
struct mm_set_uapsd_tmr_req
{
    /// action: Start or Stop the timer
    u8_l  action;
    /// timeout value, in milliseconds
    u32_l  timeout;
};

/// Structure containing the parameters of the @ref MM_SET_UAPSD_TMR_CFM message.
struct mm_set_uapsd_tmr_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    u8_l     status;
};


/// Structure containing the parameters of the @ref MM_PS_CHANGE_IND message
struct mm_ps_change_ind
{
    /// Index of the peer device that is switching its PS state
    u8_l sta_idx;
    /// New PS state of the peer device (0: active, 1: sleeping)
    u8_l ps_state;
};

/// Structure containing the parameters of the @ref MM_TRAFFIC_REQ_IND message
struct mm_traffic_req_ind
{
    /// Index of the peer device that needs traffic
    u8_l sta_idx;
    /// Number of packets that need to be sent (if 0, all buffered traffic shall be sent)
    u8_l pkt_cnt;
    /// Flag indicating if the traffic request concerns U-APSD queues or not
    bool_l uapsd;
};

/// Structure containing the parameters of the @ref MM_SET_PS_OPTIONS_REQ message.
struct mm_set_ps_options_req
{
    /// VIF Index
    u8_l vif_index;
    /// Listen interval (0 if wake up shall be based on DTIM period)
    u16_l listen_interval;
    /// Flag indicating if we shall listen the BC/MC traffic or not
    bool_l dont_listen_bc_mc;
};

/// Structure containing the parameters of the @ref MM_CSA_COUNTER_IND message
struct mm_csa_counter_ind
{
    /// Index of the VIF
    u8_l vif_index;
    /// Updated CSA counter value
    u8_l csa_count;
};

/// Structure containing the parameters of the @ref MM_CHANNEL_SURVEY_IND message
struct mm_channel_survey_ind
{
    /// Frequency of the channel
    u16_l freq;
    /// Noise in dbm
    s8_l noise_dbm;
    /// Amount of time spent of the channel (in ms)
    u32_l chan_time_ms;
    /// Amount of time the primary channel was sensed busy
    u32_l chan_time_busy_ms;
};

/// Structure containing the parameters of the @ref MM_CFG_RSSI_REQ message
struct mm_cfg_rssi_req
{
    /// Index of the VIF
    u8_l vif_index;
    /// RSSI threshold
    s8_l rssi_thold;
    /// RSSI hysteresis
    u8_l rssi_hyst;
};

/// Structure containing the parameters of the @ref MM_RSSI_STATUS_IND message
struct mm_rssi_status_ind
{
    /// Index of the VIF
    u8_l vif_index;
    /// Status of the RSSI
    bool_l rssi_status;
    /// RSSI of current ind
    s8_l rssi;
};

/// Maximum number of SSIDs in a scan request
#define SCAN_SSID_MAX  1

/// Maximum number of 2.4GHz channels
#define SCAN_CHANNEL_2G4 14

/// Maximum number of 5GHz channels
#define SCAN_CHANNEL_5G  28

/// Maximum number of channels in a scan request
#define SCAN_CHANNEL_MAX (SCAN_CHANNEL_2G4 + SCAN_CHANNEL_5G)

/// Flag bits
#define SCAN_PASSIVE_BIT BIT(0)
#define SCAN_DISABLED_BIT BIT(1)

/// Definition of a channel to be scanned
struct scan_chan_tag
{
    /// Frequency of the channel
    u16_l freq;
    /// RF band (0: 2.4GHz, 1: 5GHz)
    u8_l band;
    /// Bit field containing additional information about the channel
    u8_l flags;
    /// Max tx_power for this channel (dBm)
    s8_l tx_power;
};

/// Structure containing the parameters of the @ref SCAN_START_REQ message
struct scan_start_req
{
    /// List of channel to be scanned
    struct scan_chan_tag chan[SCAN_CHANNEL_MAX];
    /// List of SSIDs to be scanned
    struct mac_ssid ssid[SCAN_SSID_MAX];
    /// BSSID to be scanned
    struct mac_addr bssid;
    /// MAC address used to send probe request
    struct mac_addr mac;
    /// Pointer (in host memory) to the additional IEs that need to be added to the ProbeReq
    /// (following the SSID element)
    u32_l add_ies;
    /// Length of the additional IEs
    u16_l add_ie_len;
    /// Index of the VIF that is scanning
    u8_l vif_idx;
    /// Number of channels to scan
    u8_l chan_cnt;
    /// Number of SSIDs to scan for
    u8_l ssid_cnt;
    /// no CCK - For P2P frames not being sent at CCK rate in 2GHz band.
    bool no_cck;
};

/// Structure containing the parameters of the @ref SCAN_START_CFM message
struct scan_start_cfm
{
    /// Status of the request
    u8_l status;
};

/// Structure containing the parameters of the @ref SCANU_START_REQ message
struct scanu_start_req
{
    /// List of channel to be scanned
    struct scan_chan_tag chan[SCAN_CHANNEL_MAX];
    /// List of SSIDs to be scanned
    struct mac_ssid ssid[SCAN_SSID_MAX];
    /// BSSID to be scanned (or WILDCARD BSSID if no BSSID is searched in particular)
    struct mac_addr bssid;
    /// MAC address used to send probe request
    struct mac_addr mac;
    /// Address (in host memory) of the additional IEs that need to be added to the ProbeReq
    /// (following the SSID element)
    u32_l add_ies;
    /// Length of the additional IEs
    u16_l add_ie_len;
    /// Index of the VIF that is scanning
    u8_l vif_idx;
    /// Number of channels to scan
    u8_l chan_cnt;
    /// Number of SSIDs to scan for
    u8_l ssid_cnt;
    /// no CCK - For P2P frames not being sent at CCK rate in 2GHz band.
    bool no_cck;
    /// MISC flags
    uint32_t flags;
    /// channel scan time
    uint32_t duration_scan;
};

struct scanu_raw_send_req
{
    void *pkt;
    uint32_t len;
};

struct scanu_raw_send_cfm
{
    uint32_t status;
};

/// Structure containing the parameters of the @ref SCANU_START_CFM message
struct scanu_start_cfm
{
    /// Status of the request
    u8_l status;
};

/// Parameters of the @SCANU_RESULT_IND message
struct scanu_result_ind
{
    /// Length of the frame
    uint16_t length;
    /// Frame control field of the frame.
    uint16_t framectrl;
    /// Center frequency on which we received the packet
    uint16_t center_freq;
    /// PHY band
    uint8_t band;
    /// Index of the station that sent the frame. 0xFF if unknown.
    uint8_t sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    uint8_t inst_nbr;
    /// Source mac of the received frame.
    uint8_t sa[6];
    /// timestamp of the received frame.
    uint32_t tsflo;
    uint32_t tsfhi;
    /// RSSI of the received frame.
    int8_t rssi;
    ///Abs. PPM of the received frame
    int8_t ppm_abs;
    ///Rel. PPM of the received frame
    int8_t ppm_rel;
    /// Flags
    uint8_t flags;
    /// Date rate of the received frame.
    uint8_t data_rate;
    /// Frame payload.
    uint32_t payload[];
};

/// Structure containing the parameters of the message.
struct scanu_fast_req
{
    /// The SSID to scan in the channel.
    struct mac_ssid ssid;
    /// BSSID.
    struct mac_addr bssid;
    /// Probe delay.
    u16_l probe_delay;
    /// Minimum channel time.
    u16_l minch_time;
    /// Maximum channel time.
    u16_l maxch_time;
    /// The channel number to scan.
    u16_l ch_nbr;
};


/// Structure containing the parameters of the @ref ME_START_REQ message
struct me_config_req
{
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// Lifetime of packets sent under a BlockAck agreement (expressed in TUs)
    u16_l tx_lft;
    /// Boolean indicating if HT is supported or not
    bool_l ht_supp;
    /// Boolean indicating if VHT is supported or not
    bool_l vht_supp;
    /// Boolean indicating if PS mode shall be enabled or not
    bool_l ps_on;
};

/// Structure containing the parameters of the @ref ME_CHAN_CONFIG_REQ message
struct me_chan_config_req
{
    /// List of 2.4GHz supported channels
    struct scan_chan_tag chan2G4[SCAN_CHANNEL_2G4];
    /// Number of 2.4GHz channels in the list
    u8_l chan2G4_cnt;
};

/// Structure containing the parameters of the @ref ME_SET_CONTROL_PORT_REQ message
struct me_set_control_port_req
{
    /// Index of the station for which the control port is opened
    u8_l sta_idx;
    /// Control port state
    bool_l control_port_open;
};

/// Structure containing the parameters of the @ref ME_TKIP_MIC_FAILURE_IND message
struct me_tkip_mic_failure_ind
{
    /// Address of the sending STA
    struct mac_addr addr;
    /// TSC value
    u64_l tsc;
    /// Boolean indicating if the packet was a group or unicast one (true if group)
    bool_l ga;
    /// Key Id
    u8_l keyid;
    /// VIF index
    u8_l vif_idx;
};

/// Structure containing the parameters of the @ref ME_STA_ADD_REQ message
struct me_sta_add_req
{
    /// MAC address of the station to be added
    struct mac_addr mac_addr;
    /// Supported legacy rates
    struct mac_rateset rate_set;
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// Flags giving additional information about the station
    u32_l flags;
    /// Association ID of the station
    u16_l aid;
    /// Bit field indicating which queues have U-APSD enabled
    u8_l uapsd_queues;
    /// Maximum size, in frames, of a APSD service period
    u8_l max_sp_len;
    /// Operation mode information (valid if bit @ref STA_OPMOD_NOTIF is
    /// set in the flags)
    u8_l opmode;
    /// Index of the VIF the station is attached to
    u8_l vif_idx;
#if (TDLS_ENABLE)
    /// Whether the the station is TDLS station
    bool_l tdls_sta;
#endif
    uint32_t tsflo;
    uint32_t tsfhi;
    int8_t rssi;
    uint8_t data_rate;
};

/// Structure containing the parameters of the @ref ME_STA_ADD_CFM message
struct me_sta_add_cfm
{
    /// Station index
    u8_l sta_idx;
    /// Status of the station addition
    u8_l status;
    /// PM state of the station
    u8_l pm_state;
};

/// Structure containing the parameters of the @ref ME_STA_DEL_REQ message.
struct me_sta_del_req
{
    /// Index of the station to be deleted
    u8_l sta_idx;
#if (TDLS_ENABLE)
    /// Whether the the station is TDLS station
    bool_l tdls_sta;
#endif
};

/// Structure containing the parameters of the @ref ME_TX_CREDITS_UPDATE_IND message.
struct me_tx_credits_update_ind
{
    /// Index of the station for which the credits are updated
    u8_l sta_idx;
    /// TID for which the credits are updated
    u8_l tid;
    /// Offset to be applied on the credit count
    s8_l credits;
};

/// Structure containing the parameters of the @ref ME_TRAFFIC_IND_REQ message.
struct me_traffic_ind_req
{
    /// Index of the station for which UAPSD traffic is available on host
    u8_l sta_idx;
    /// Flag indicating the availability of UAPSD packets for the given STA
    u8_l tx_avail;
    /// Indicate if traffic is on uapsd-enabled queues
    bool_l uapsd;
};

/// Structure containing the structure of a retry chain step
struct step
{
    /// Current calculated throughput
    u32_l tp;
    /// Index of the sample in the rate_stats table
    u16_l  idx;
};

/// Structure containing the rate control statistics
struct rc_rate_stats
{
    /// Number of attempts (per sampling interval)
    u16_l attempts;
    /// Number of success (per sampling interval)
    u16_l success;
    /// Estimated probability of success (EWMA)
    u16_l probability;
    /// Rate configuration of the sample
    u16_l rate_config;
    /// Number of times the sample has been skipped (per sampling interval)
    u8_l sample_skipped;
    /// Whether the old probability is available
    bool_l  old_prob_available;
    /// Number of times the AMPDU has been retried with this rate
    u8_l n_retry;
    // Whether the rate can be used in the retry chain
    bool_l rate_allowed;
};

/// Structure containing the parameters of the @ref ME_RC_SET_RATE_REQ message.
struct me_rc_set_rate_req
{
    /// Index of the station for which the fixed rate is set
    u8_l sta_idx;
    /// Rate configuration to be set
    u16_l fixed_rate_cfg;
    /// Force power table update
    u16_l power_table_req;
};


/// Structure containing the parameters of @ref SM_CONNECT_REQ message.
struct sm_connect_req
{
    /// SSID to connect to
    struct mac_ssid ssid;
    /// BSSID to connect to (if not specified, set this field to WILDCARD BSSID)
    struct mac_addr bssid;
    /// Channel on which we have to connect (if not specified, set -1 in the chan.freq field)
    struct scan_chan_tag chan;
    /// Connection flags (see @ref sm_connect_flags)
    u32_l flags;
    /// Control port Ethertype
    u16_l ctrl_port_ethertype;
#if 0
    /// Length of the association request IEs
    u16_l ie_len;
#endif
    /// Listen interval to be used for this connection
    u16_l listen_interval;
    /// Flag indicating if the we have to wait for the BC/MC traffic after beacon or not
    bool_l dont_wait_bcmc;
    /// Authentication type
    u8_l auth_type;
    /// UAPSD queues (bit0: VO, bit1: VI, bit2: BE, bit3: BK)
    u8_l uapsd_queues;
    /// VIF index
    u8_l vif_idx;
    /// retry counter for auth/aossoc
    u8_l counter_retry_auth_assoc;
#if 0
    /// Buffer containing the additional information elements to be put in the
    /// association request
    u32_l ie_buf[64];
#endif
    /// Enable embedded supplicant
    bool is_supplicant_enabled;
    //// Passphrase
    uint8_t phrase[MAX_PSK_PASS_PHRASE_LEN];
    uint8_t phrase_pmk[MAX_PSK_PASS_PHRASE_LEN];
};

/// Structure containing the parameters of the @ref SM_CONNECT_CFM message.
struct sm_connect_cfm
{
    /// Status. If 0, it means that the connection procedure will be performed and that
    /// a subsequent @ref SM_CONNECT_IND message will be forwarded once the procedure is
    /// completed
    u8_l status;
};

#define SM_ASSOC_IE_LEN   800
/// Structure containing the parameters of the @ref SM_CONNECT_IND message.
struct sm_connect_ind
{
    /// Status code of the connection procedure
    u16_l status_code;
    /// Reason code by AP
    u16_l reason_code;
    /// BSSID
    struct mac_addr bssid;
    /// Flag indicating if the indication refers to an internal roaming or from a host request
    bool_l roamed;
    /// Index of the VIF for which the association process is complete
    u8_l vif_idx;
    /// Index of the STA entry allocated for the AP
    u8_l ap_idx;
    /// Index of the LMAC channel context the connection is attached to
    u8_l ch_idx;
    /// Flag indicating if the AP is supporting QoS
    bool_l qos;
    /// ACM bits set in the AP WMM parameter element
    u8_l acm;
    /// Length of the AssocReq IEs
    u16_l assoc_req_ie_len;
    /// Length of the AssocRsp IEs
    u16_l assoc_rsp_ie_len;
    /// IE buffer
    u32_l assoc_ie_buf[SM_ASSOC_IE_LEN/4];

    u16_l aid;
    u8_l band;
    u16_l center_freq;
    u8_l width;
    u32_l center_freq1;
    u32_l center_freq2;

    /// EDCA parameters
    u32_l ac_param[AC_MAX];
    /// Pointer to the structure used for the diagnose module
    struct sm_tlv_list connect_diagnose;
};

/// Structure containing the parameters of the @ref SM_DISCONNECT_REQ message.
struct sm_disconnect_req
{
    /// Index of the VIF.
    u8_l vif_idx;
};

/// Structure containing the parameters of SM_ASSOCIATION_IND the message
struct sm_association_ind
{
    // MAC ADDR of the STA
    struct mac_addr     me_mac_addr;
};


/// Structure containing the parameters of the @ref SM_DISCONNECT_IND message.
struct sm_disconnect_ind
{
    /// Status code of the disconnection procedure
    u16_l status_code;
    /// Reason of the disconnection.
    u16_l reason_code;
    /// Index of the VIF.
    u8_l vif_idx;
    /// FT over DS is ongoing
    bool_l ft_over_ds;
    /// Pointer to the structure used for the diagnose module
    struct sm_tlv_list connect_diagnose;
};

/// Structure containing the parameters of the @ref SM_CONNECT_ABORT_REQ message.
struct sm_connect_abort_req
{
    /// Index of the VIF.
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref SM_CONNECT_ABORT_CFM message.
struct sm_connect_abort_cfm
{
    /// Status. If 0, it means that the disconnection procedure will be aborted and that
    /// a subsequent @ref SM_DISCONNECT_IND message will be forwarded once the procedure is
    /// completed.
    /// If 1, it means that the scan procdure will be aborted.
    uint8_t status;
};

struct
{
    /// TASK 
    uint32_t task;
    /// ELEMENT
    uint32_t element;
    /// length
    uint32_t length;
    /// buffer
    uint32_t buf[];
} cfg_start_req_u_tlv_t;

struct cfg_start_req
{
    /// TYPE: GET/SET/RESET/DUMP
    uint32_t ops;
    union {
        /// struct for get ELEMENT
        struct {
            /// TASK 
            uint32_t task;
            /// ELEMENT
            uint32_t element;
        } get[0];

        /// struct for reset ELEMENT
        struct {
            /// TASK 
            uint32_t task;
            /// ELEMENT
            uint32_t element;
        } reset[0];

        /// struct for set ELEMENT with TLV based
        struct {
            /// TASK 
            uint32_t task;
            /// ELEMENT
            uint32_t element;
            /// type
            uint32_t type;
            /// length
            uint32_t length;
            /// buffer
            uint32_t buf[];
        } set[0];
    } u;
};

struct cfg_start_cfm
{
    /// Status of the AP starting procedure
    uint8_t status;
};


/// Structure containing the parameters of the @ref APM_START_REQ message.
struct apm_start_req
{
    /// Basic rate set
    struct mac_rateset basic_rates;
    /// Control channel on which we have to enable the AP
    struct scan_chan_tag chan;
    /// Center frequency of the first segment
    u32_l center_freq1;
    /// Center frequency of the second segment (only in 80+80 configuration)
    u32_l center_freq2;
    /// Width of channel
    u8_l ch_width;
    /// Hidden ssid
    u8_l hidden_ssid;
    /// Address, in host memory, to the beacon template
    u32_l bcn_addr;
    /// Length of the beacon template
    u16_l bcn_len;
    /// Offset of the TIM IE in the beacon
    u16_l tim_oft;
    /// Beacon interval
    u16_l bcn_int;
    /// Flags
    u32_l flags;
    /// Control port Ethertype
    u16_l ctrl_port_ethertype;
    /// Length of the TIM IE
    u8_l tim_len;
    /// Index of the VIF for which the AP is started
    u8_l vif_idx;
    /// Enable APM Embedded
    bool apm_emb_enabled;
    /// rate set
    struct mac_rateset rate_set;
    /// Beacon dtim period
    uint8_t beacon_period;
    /// Qos is supported
    uint8_t qos_supported;
    /// SSID of the AP
    struct mac_ssid ssid;
    /// AP Security type
    uint8_t ap_sec_type;
    /// AP Passphrase
    uint8_t phrase[MAX_PSK_PASS_PHRASE_LEN];
    uint8_t phrase_tail[1];
    // Buf for storing IE
    uint8_t bcn_buf_len;
    uint8_t bcn_buf[64];
};

/// Structure containing the parameters of the @ref APM_START_CFM message.
struct apm_start_cfm
{
    /// Status of the AP starting procedure
    u8_l status;
    /// Index of the VIF for which the AP is started
    u8_l vif_idx;
    /// Index of the channel context attached to the VIF
    u8_l ch_idx;
    /// Index of the STA used for BC/MC traffic
    u8_l bcmc_idx;
};

/// Structure containing the parameters of the @ref APM_STOP_REQ message.
struct apm_stop_req
{
    /// Index of the VIF for which the AP has to be stopped
    u8_l vif_idx;
};

/// Structure containing the parameters of the @ref APM_CONF_MAX_STA_REQ message.
struct apm_conf_max_sta_req
{
    /// max Stattion supported
    u8_l max_sta_supported;
};

struct apm_sta_del_req
{
    /// Index of the AP VIF
    u8_l vif_idx;
    /// Index of the sta in AP mode
    u8_l sta_idx;
};

/// Structure containing the parameters of the @ref APM_STOP_REQ message.
struct apm_sta_del_cfm
{
    /// Status of deleting sta procedure
    u8_l status;
    /// Index of the AP VIF
    u8_l vif_idx;
    /// Index of the sta in AP mode
    u8_l sta_idx;
};

/// Structure containing the parameters of the @ref APM_STA_ADD_IND message.
struct apm_sta_add_ind
{
    /// Flags giving additional information about the station
    uint32_t flags;
    /// Station Mac address
    struct mac_addr sta_addr;
    /// Index of the VIF for which the sta joined
    uint8_t vif_idx;
    /// Station index
    uint8_t sta_idx;
    int8_t rssi;
    uint32_t tsflo;
    uint32_t tsfhi;
    uint8_t data_rate;
};

/// Structure containing the parameters of the @ref APM_STA_DEL_IND message.
struct apm_sta_del_ind
{
    /// status of the apm_sta disconnection.
    uint16_t status_code;
    /// Reason of the apm_sta disconnection.
    uint16_t reason_code;
    /// Station index
    uint8_t sta_idx;
};

/// Maximum length of the Mesh ID
#define MESH_MESHID_MAX_LEN     (32)
#endif // LMAC_MSG_H_
