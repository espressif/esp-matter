
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


#ifndef _MAC_H_
#define _MAC_H_

/**
 ****************************************************************************************
 * @defgroup MAC MAC
 * @ingroup COMMON
 * @brief  Common defines,structures
 *
 * This module contains defines commonaly used for MAC
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "lmac_types.h"


/*
 * DEFINES
 ****************************************************************************************
 */
/// duration of a Time Unit in microseconds
#define TU_DURATION                     1024

/// max number of channels in the 2.4 GHZ band
#define MAC_DOMAINCHANNEL_24G_MAX       14

/// max number of channels in the 5 GHZ band
#define MAC_DOMAINCHANNEL_5G_MAX        45

/// Mask to test if it's a basic rate - BIT(7)
#define MAC_BASIC_RATE                  0x80
/// Mask for extracting/checking word alignment
#define WORD_ALIGN                      3

#define MAX_AMSDU_LENGTH                7935

/*
 * MACRO DEFINITIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * Compare two MAC addresses.
 * The MAC addresses MUST be 16 bit aligned.
 * @param[in] addr1_ptr Pointer to the first MAC address.
 * @param[in] addr2_ptr Pointer to the second MAC address.
 * @return True if equal, false if not.
 ****************************************************************************************
 */
#define MAC_ADDR_CMP(addr1_ptr, addr2_ptr)                                              \
    ((*(((u8*)(addr1_ptr)) + 0) == *(((u8*)(addr2_ptr)) + 0)) &&            \
     (*(((u8*)(addr1_ptr)) + 1) == *(((u8*)(addr2_ptr)) + 1)) &&            \
     (*(((u8*)(addr1_ptr)) + 2) == *(((u8*)(addr2_ptr)) + 2)) &&            \
     (*(((u8*)(addr1_ptr)) + 3) == *(((u8*)(addr2_ptr)) + 3)) &&            \
     (*(((u8*)(addr1_ptr)) + 4) == *(((u8*)(addr2_ptr)) + 4)) &&            \
     (*(((u8*)(addr1_ptr)) + 5) == *(((u8*)(addr2_ptr)) + 5)))

/**
 ****************************************************************************************
 * Compare two MAC addresses whose alignment is not known.
 * @param[in] __a1 Pointer to the first MAC address.
 * @param[in] __a2 Pointer to the second MAC address.
 * @return True if equal, false if not.
 ****************************************************************************************
 */
#define MAC_ADDR_CMP_PACKED(__a1, __a2)                                                 \
    (memcmp(__a1, __a2, MAC_ADDR_LEN) == 0)

/**
 ****************************************************************************************
 * Copy a MAC address.
 * The MAC addresses MUST be 16 bit aligned.
 * @param[in] addr1_ptr Pointer to the destination MAC address.
 * @param[in] addr2_ptr Pointer to the source MAC address.
 ****************************************************************************************
 */
#define MAC_ADDR_CPY(addr1_ptr, addr2_ptr)                                              \
    *(((u16*)(addr1_ptr)) + 0) = *(((u16*)(addr2_ptr)) + 0);                  \
    *(((u16*)(addr1_ptr)) + 1) = *(((u16*)(addr2_ptr)) + 1);                  \
    *(((u16*)(addr1_ptr)) + 2) = *(((u16*)(addr2_ptr)) + 2)

/**
 ****************************************************************************************
 * Compare two SSID.
 * @param ssid1_ptr Pointer to the first SSID structure.
 * @param ssid2_ptr Pointer to the second SSID structure.
 * @return True if equal, false if not.
 ****************************************************************************************
 */
#define MAC_SSID_CMP(ssid1_ptr,ssid2_ptr)                                               \
    (((ssid1_ptr)->length == (ssid2_ptr)->length) &&                                    \
     (memcmp((&(ssid1_ptr)->array[0]), (&(ssid2_ptr)->array[0]), (ssid1_ptr)->length) == 0))

/// Check if MAC address is a group address: test the multicast bit.
#define MAC_ADDR_GROUP(mac_addr_ptr) ((*(mac_addr_ptr)) & 1)

/// MAC address length in bytes.
#define MAC_ADDR_LEN 6

/// MAC address structure.
struct mac_addr
{
    /// Array of bytes that make up the MAC address.
    u8_l array[MAC_ADDR_LEN];
};

/// SSID maximum length.
#define MAC_SSID_LEN 32

/// SSID.
struct mac_ssid
{
    /// Actual length of the SSID.
    u8_l length;
    /// Array containing the SSID name.
    u8_l array[MAC_SSID_LEN];
    u8_l array_tail[1];//for MAX SSID ISSUE
};

/// MAC RATE-SET
#define MAC_RATESET_LEN             12
#define MAC_OFDM_PHY_RATESET_LEN    8
#define MAC_EXT_RATES_OFF      8
struct mac_rateset
{
    u8_l     length;
    u8_l     array[MAC_RATESET_LEN];
};

/// MAC RATES
#define MAC_MCS_WORD_CNT            3
struct mac_rates
{
    /// MCS 0 to 76
    u32 mcs[MAC_MCS_WORD_CNT];
    /// Legacy rates (1Mbps to 54Mbps)
    u16 legacy;
};

/// IV/EIV data
#define MAC_IV_LEN  4
#define MAC_EIV_LEN 4
struct rx_seciv
{
    u8 iv[MAC_IV_LEN];
    u8 ext_iv[MAC_EIV_LEN];
};

/// MAC MCS SET
#define MAX_MCS_LEN 16 // 16 * 8 = 128
struct mac_mcsset
{
    u8 length;
    u8 array[MAX_MCS_LEN];
};

/// MAC Secret Key
#define MAC_WEP_KEY_CNT          4  // Number of WEP keys per virtual device
#define MAC_WEP_KEY_LEN         13  // Max size of a WEP key (104/8 = 13)
struct mac_wep_key
{
    u8 array[MAC_WEP_KEY_LEN]; // Key material
};


/// MAC Secret Key
#define MAC_SEC_KEY_LEN         32  // TKIP keys 256 bits (max length) with MIC keys
struct mac_sec_key
{
    u8_l length;                         // Key material length
    u32_l array[MAC_SEC_KEY_LEN/4];        // Key material
};

/// MAC channel list
/// @todo: fix that number
#define MAC_MAX_CH 40
struct mac_ch_list
{
    /// Number of channels in channel list.
    u16 nbr;
    /// List of the channels.
    u8 list[MAC_MAX_CH];
};


struct mac_country_subband
{
    // First channel number of the triplet.
    u8 first_chn;
    // Max number of channel number for the triplet.
    u8 nbr_of_chn;
    // Maximum allowed transmit power.
    u8 max_tx_power;
};

#define MAX_COUNTRY_LEN         3
#define MAX_COUNTRY_SUBBAND     5
struct mac_country
{
    // Length of the country string
    u8 length;
    // Country  string 2 char.
    u8 string[MAX_COUNTRY_LEN];
    // channel info triplet
    struct mac_country_subband subband[MAX_COUNTRY_SUBBAND];
};

/// MAC HT CAPABILITY
struct mac_htcapability
{
    u16_l       ht_capa_info;
    u8_l        a_mpdu_param;
    u8_l        mcs_rate[MAX_MCS_LEN];
    u16_l       ht_extended_capa;
    u32_l       tx_beamforming_capa;
    u8_l        asel_capa;
};

/// MAC VHT CAPABILITY
struct mac_vhtcapability
{
    u32_l       vht_capa_info;
    u16_l       rx_mcs_map;
    u16_l       rx_highest;
    u16_l       tx_mcs_map;
    u16_l       tx_highest;
};


/// MAC HT CAPABILITY
struct mac_htoprnelmt
{
    u8     prim_channel;
    u8     ht_oper_1;
    u16    ht_oper_2;
    u16    ht_oper_3;
    u8     mcs_rate[MAX_MCS_LEN];

};

/// MAC QOS CAPABILITY
struct mac_qoscapability
{
    u8  qos_info;
};

/// RSN information element
#define MAC_RAW_RSN_IE_LEN 34
struct mac_raw_rsn_ie
{
    u8 data[2 + MAC_RAW_RSN_IE_LEN];
};

#define MAC_RAW_ENC_LEN 0x1A
struct mac_wpa_frame
{
    u8 array[MAC_RAW_ENC_LEN];
};

#define MAC_WME_PARAM_LEN          16
struct mac_wmm_frame
{
    u8 array [MAC_WME_PARAM_LEN];
};

/// BSS load element
struct mac_bss_load
{
    u16 sta_cnt;
    u8  ch_utilization;
    u16 avail_adm_capacity;
};

///EDCA Parameter Set Element
struct  mac_edca_param_set
{
    u8         qos_info;
    u32        ac_be_param_record;
    u32        ac_bk_param_record;
    u32        ac_vi_param_record;
    u32        ac_vo_param_record;
};


///MAC Twenty Forty BSS

struct mac_twenty_fourty_bss
{
    u8 bss_coexistence;
};

/// MAC BA PARAMETERS
struct mac_ba_param
{
    struct mac_addr   peer_sta_address;     ///< Peer STA MAC Address to which BA is Setup
    u16          buffer_size;          ///< Number of buffers available for this BA
    u16          start_sequence_number;///< Start Sequence Number of BA
    u16          ba_timeout;           ///< BA Setup timeout value
    u8           dev_type;             ///< BA Device Type Originator/Responder
    u8           block_ack_policy;     ///< BLOCK-ACK Policy Setup Immedaite/Delayed
    u8           buffer_cnt;           ///< Number of buffers required for BA Setup
};

/// MAC TS INFO field
struct mac_ts_info
{
    u8   traffic_type;
    u8   ack_policy;
    u8   access_policy;
    u8   dir;
    u8   tsid;
    u8   user_priority;
    bool      aggregation;
    bool      apsd;
    bool      schedule;
};

/// MAC TSPEC PARAMETERS
struct mac_tspec_param
{
    struct mac_ts_info ts_info;
    u16  nominal_msdu_size;
    u16  max_msdu_size;
    u32  min_service_interval;
    u32  max_service_interval;
    u32  inactivity_interval;
    u32  short_inactivity_interval;
    u32  service_start_time;
    u32  max_burst_size;
    u32  min_data_rate;
    u32  mean_data_rate;
    u32  min_phy_rate;
    u32  peak_data_rate;
    u32  delay_bound;
    u16  medium_time;
    u8   surplusbwallowance;
};

/// Scan result element, parsed from beacon or probe response frames.
struct mac_scan_result
{
    /// Network BSSID.
    struct mac_addr bssid;
    /// Network type (IBSS or ESS).
    u16 bsstype;
    /// Network channel number.
    u16 ch_nbr;
    /// Network beacon period.
    u16 beacon_period;
    u32 timestamp_high;
    u32 timestamp_low;
    u16 dtim_period;
    u16 ibss_parameter;
    u16 cap_info;
    struct mac_rateset rate_set;
    struct mac_bss_load bss_load;
    u8 country_element[3];
    struct mac_edca_param_set edca_param;
    struct mac_raw_rsn_ie rsn_ie;
    struct mac_qoscapability qos_cap;
    struct mac_htcapability ht_cap;
    u8 sec_ch_oft;
    struct mac_twenty_fourty_bss twenty_fourty_bss;
    bool valid_flag;
    u8 rssi;
};

/// Structure containing the information required to perform a measurement request
struct mac_request_set
{

    u8             mode;       ///<As specified by standard
    u8             type;       ///< 0: Basic request, 1: CCA request, 2: RPI histogram request
    u16            duration;   ///< In TU
    uint64_t            start_time; ///< TSF time
    u8             ch_number;  ///< channel to be measured
};

/// Structure containing the information returned from a measurement process
struct mac_report_set
{
    u8             mode;       ///<As specified by standard
    u8             type;       ///< 0: Basic request, 1: CCA request, 2: RPI histogram request
    u16            duration;   ///< In TU
    uint64_t            start_time; ///< TSF time
    u8             ch_number;  ///< channel to be measured
    u8             map;        ///< As specified by standard
    u8             cca_busy_fraction;  ///<As specified by standard
    u8             rpi_histogram[8];   ///<As specified by standard
};

/// Structure containing the MAC SW and MAC HW version information
struct mac_version
{
    char mac_sw_version[16];
    char mac_sw_version_date[48];
    char mac_sw_build_date[48];
    u32 mac_hw_version1;
    u32 mac_hw_version2;
};

/// Structure containing some of the properties of a BSS. @todo Add required fields during
/// AP/IBSS mode implementation
struct mac_bss_conf
{
    /// Flags (ERP, QoS, etc.).
    u32 flags;
    /// Beacon period
    u16 beacon_period;
};

/// Traffic ID enumeration
enum
{
    TID_0,
    TID_1,
    TID_2,
    TID_3,
    TID_4,
    TID_5,
    TID_6,
    TID_7,
    TID_MGT,
    TID_MAX
};

/// Access Category enumeration
enum
{
    AC_BK = 0,
    AC_BE,
    AC_VI,
    AC_VO,
    AC_MAX
};

/// SCAN type
enum
{
    SCAN_PASSIVE,
    SCAN_ACTIVE
};

/// rates
enum
{
    MAC_RATE_1MBPS   =   2,
    MAC_RATE_2MBPS   =   4,
    MAC_RATE_5_5MBPS =  11,
    MAC_RATE_6MBPS   =  12,
    MAC_RATE_9MBPS   =  18,
    MAC_RATE_11MBPS  =  22,
    MAC_RATE_12MBPS  =  24,
    MAC_RATE_18MBPS  =  36,
    MAC_RATE_24MBPS  =  48,
    MAC_RATE_36MBPS  =  72,
    MAC_RATE_48MBPS  =  96,
    MAC_RATE_54MBPS  = 108
};

/// Station flags
enum
{
    /// Bit indicating that a STA has QoS (WMM) capability
    STA_QOS_CAPA = 1 << 0,
    /// Bit indicating that a STA has HT capability
    STA_HT_CAPA = 1 << 1,
    /// Bit indicating that a STA has VHT capability
    STA_VHT_CAPA = 1 << 2,
    /// Bit indicating that a STA has MFP capability
    STA_MFP_CAPA = 1 << 3,
    /// Bit indicating that the STA included the Operation Notification IE
    STA_OPMOD_NOTIF = 1 << 4,
};

/// Connection flags
enum
{
    /// Flag indicating whether the control port is controlled by host or not
    CONTROL_PORT_HOST = 1 << 0,
    /// Flag indicating whether the control port frame shall be sent unencrypted
    CONTROL_PORT_NO_ENC = 1 << 1,
    /// Flag indicating whether HT shall be disabled or not
    DISABLE_HT = 1 << 2,
    /// Flag indicating whether WPA or WPA2 authentication is in use
    WPA_WPA2_IN_USE = 1 << 3,
    /// Flag indicating whether MFP is in use
    MFP_IN_USE = 1 << 4,
};

/*
* GLOBAL VARIABLES
****************************************************************************************
*/
extern const u8 mac_tid2ac[];

extern const u8 mac_id2rate[];

extern const u16 mac_mcs_params_20[];

extern const u16 mac_mcs_params_40[];

/// @}

#endif // _MAC_H_
