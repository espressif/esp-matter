/*******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*! \file   mtk_wireless.h
 *    \brief  This file is for 802.11 type define
 */


#ifndef _GL_MTK_WIRELESS_H
#define _GL_MTK_WIRELESS_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */


/* SIOCSIWAUTH/SIOCGIWAUTH parameters (0 .. 4095)
 * (IW_AUTH_INDEX mask in struct iw_param flags; this is the index of the
 * parameter that is being set/get to; value will be read/written to
 * struct iw_param value field)
 */
#define IW_AUTH_WPA_VERSION             0
#define IW_AUTH_CIPHER_PAIRWISE         1
#define IW_AUTH_CIPHER_GROUP            2
#define IW_AUTH_KEY_MGMT                3
#define IW_AUTH_TKIP_COUNTERMEASURES    4
#define IW_AUTH_DROP_UNENCRYPTED        5
#define IW_AUTH_80211_AUTH_ALG          6
#define IW_AUTH_WPA_ENABLED             7
#define IW_AUTH_RX_UNENCRYPTED_EAPOL    8
#define IW_AUTH_ROAMING_CONTROL         9
#define IW_AUTH_PRIVACY_INVOKED        10

#if CFG_SUPPORT_802_11W
#define IW_AUTH_MFP                    12
#define IW_AUTH_MFP_DISABLED    0	/* MFP disabled */
#define IW_AUTH_MFP_OPTIONAL    1	/* MFP optional */
#define IW_AUTH_MFP_REQUIRED    2	/* MFP required */
#endif

/* IW_AUTH_WPA_VERSION values (bit field) */
#define IW_AUTH_WPA_VERSION_DISABLED    0x00000001
#define IW_AUTH_WPA_VERSION_WPA         0x00000002
#define IW_AUTH_WPA_VERSION_WPA2        0x00000004

/* IW_AUTH_PAIRWISE_CIPHER and IW_AUTH_GROUP_CIPHER values (bit field) */
#define IW_AUTH_CIPHER_NONE     0x00000001
#define IW_AUTH_CIPHER_WEP40    0x00000002
#define IW_AUTH_CIPHER_TKIP     0x00000004
#define IW_AUTH_CIPHER_CCMP     0x00000008
#define IW_AUTH_CIPHER_WEP104   0x00000010
#define IW_AUTH_CIPHER_AES_CMAC 0x00000020

/* IW_AUTH_KEY_MGMT values (bit field) */
#define IW_AUTH_KEY_MGMT_802_1X     1
#define IW_AUTH_KEY_MGMT_PSK        2
#define IW_AUTH_KEY_MGMT_WPA_NONE   4

/* IW_AUTH_80211_AUTH_ALG values (bit field) */
#define IW_AUTH_ALG_OPEN_SYSTEM 0x00000001
#define IW_AUTH_ALG_SHARED_KEY  0x00000002
#define IW_AUTH_ALG_LEAP        0x00000004
#define IW_AUTH_ALG_FT          0x00000008
#define IW_AUTH_ALG_SAE         0x00000010

/* IW_AUTH_ROAMING_CONTROL values */
#define IW_AUTH_ROAMING_ENABLE  0 /* driver/firmware based roaming */
#define IW_AUTH_ROAMING_DISABLE 1 /* user space program used for roaming
				   * control
				   */

#define IW_ENCODE_SEQ_MAX_SIZE  8
/* struct iw_encode_ext ->alg */
#define IW_ENCODE_ALG_NONE  0
#define IW_ENCODE_ALG_WEP   1
#define IW_ENCODE_ALG_TKIP  2
#define IW_ENCODE_ALG_CCMP  3
#define IW_ENCODE_ALG_PMK   4
#if CFG_SUPPORT_802_11W
#define IW_ENCODE_ALG_AES_CMAC  5
#endif

/* struct iw_encode_ext ->ext_flags */
#define IW_ENCODE_EXT_TX_SEQ_VALID  0x00000001
#define IW_ENCODE_EXT_RX_SEQ_VALID  0x00000002
#define IW_ENCODE_EXT_GROUP_KEY     0x00000004
#define IW_ENCODE_EXT_SET_TX_KEY    0x00000008

#define IW_PMKSA_ADD        1
#define IW_PMKSA_REMOVE     2
#define IW_PMKSA_FLUSH      3

#define IW_PMKID_LEN        16

#define IWEVGENIE   0x8C05	/* Generic IE (WPA, RSN, WMM, ..)
				 * (scan results); This includes id and
				 * length fields. One IWEVGENIE may
				 * contain more than one IE. Scan
				 * results may contain one or more
				 * IWEVGENIE events.
				 */
#define IWEVMICHAELMICFAILURE 0x8C06	/* Michael MIC failure
					 * (struct iw_michaelmicfailure)
					 */
#define IWEVASSOCREQIE  0x8C07	/* IEs used in (Re)Association Request.
				 * The data includes id and length
				 * fields and may contain more than one
				 * IE. This event is required in
				 * Managed mode if the driver
				 * generates its own WPA/RSN IE. This
				 * should be sent just before
				 * IWEVREGISTERED event for the
				 * association.
				 */
#define IWEVASSOCRESPIE 0x8C08	/* IEs used in (Re)Association
				 * Response. The data includes id and
				 * length fields and may contain more
				 * than one IE. This may be sent
				 * between IWEVASSOCREQIE and
				 * IWEVREGISTERED events for the
				 * association.
				 */
#define IWEVPMKIDCAND   0x8C09	/* PMKID candidate for RSN
				 * pre-authentication
				 * (struct iw_pmkid_cand)
				 */

/* Statistics flags (bitmask in updated) */
#define IW_QUAL_QUAL_UPDATED    0x1	/* Value was updated since last read */
#define IW_QUAL_LEVEL_UPDATED   0x2
#define IW_QUAL_NOISE_UPDATED   0x4
#define IW_QUAL_QUAL_INVALID    0x10	/* Driver doesn't provide value */
#define IW_QUAL_LEVEL_INVALID   0x20
#define IW_QUAL_NOISE_INVALID   0x40

enum {
	IEEE80211_FILTER_TYPE_BEACON = 1 << 0,
	IEEE80211_FILTER_TYPE_PROBE_REQ = 1 << 1,
	IEEE80211_FILTER_TYPE_PROBE_RESP = 1 << 2,
	IEEE80211_FILTER_TYPE_ASSOC_REQ = 1 << 3,
	IEEE80211_FILTER_TYPE_ASSOC_RESP = 1 << 4,
	IEEE80211_FILTER_TYPE_AUTH = 1 << 5,
	IEEE80211_FILTER_TYPE_DEAUTH = 1 << 6,
	IEEE80211_FILTER_TYPE_DISASSOC = 1 << 7,
	/* used to check the valid filter bits */
	IEEE80211_FILTER_TYPE_ALL = 0xFF
};

#if CFG_SUPPORT_WAPI
#define IW_AUTH_WAPI_ENABLED     0x20
#define IW_ENCODE_ALG_SMS4  0x20
#endif

#if CFG_SUPPORT_WAPI		/* Android+ */
#define IW_AUTH_KEY_MGMT_WAPI_PSK   3
#define IW_AUTH_KEY_MGMT_WAPI_CERT  4
#endif
#define IW_AUTH_KEY_MGMT_WPS  5

#if CFG_SUPPORT_802_11W
#define IW_AUTH_KEY_MGMT_802_1X_SHA256 7
#define IW_AUTH_KEY_MGMT_PSK_SHA256 8
#endif

enum nl80211_reg_rule_flags {
	NL80211_RRF_NO_OFDM             = 1<<0,
	NL80211_RRF_NO_CCK              = 1<<1,
	NL80211_RRF_NO_INDOOR           = 1<<2,
	NL80211_RRF_NO_OUTDOOR          = 1<<3,
	NL80211_RRF_DFS                 = 1<<4,
	NL80211_RRF_PTP_ONLY            = 1<<5,
	NL80211_RRF_PTMP_ONLY           = 1<<6,
	NL80211_RRF_NO_IR               = 1<<7,
	__NL80211_RRF_NO_IBSS           = 1<<8,
	NL80211_RRF_AUTO_BW             = 1<<11,
	NL80211_RRF_IR_CONCURRENT       = 1<<12,
	NL80211_RRF_NO_HT40MINUS        = 1<<13,
	NL80211_RRF_NO_HT40PLUS         = 1<<14,
	NL80211_RRF_NO_80MHZ            = 1<<15,
	NL80211_RRF_NO_160MHZ           = 1<<16,
};

#define NL80211_RRF_PASSIVE_SCAN       NL80211_RRF_NO_IR
#define NL80211_RRF_NO_IBSS            NL80211_RRF_NO_IR
#define NL80211_RRF_NO_IR              NL80211_RRF_NO_IR
#define NL80211_RRF_NO_HT40            (NL80211_RRF_NO_HT40MINUS |\
	NL80211_RRF_NO_HT40PLUS)
#define NL80211_RRF_GO_CONCURRENT      NL80211_RRF_IR_CONCURRENT
/* For backport compatibility with older userspace */
#define NL80211_RRF_NO_IR_ALL          (NL80211_RRF_NO_IR |\
	__NL80211_RRF_NO_IBSS)

/*
 * needed by mgmt/rlm_domain.c
 * enum ieee80211_channel_flags - channel flags
 *
 * Channel flags set by the regulatory control code.
 *
 * @IEEE80211_CHAN_DISABLED: This channel is disabled.
 * @IEEE80211_CHAN_NO_IR: do not initiate radiation, this includes
 *      sending probe requests or beaconing.
 * @IEEE80211_CHAN_RADAR: Radar detection is required on this channel.
 * @IEEE80211_CHAN_NO_HT40PLUS: extension channel above this channel
 *      is not permitted.
 * @IEEE80211_CHAN_NO_HT40MINUS: extension channel below this channel
 *      is not permitted.
 * @IEEE80211_CHAN_NO_OFDM: OFDM is not allowed on this channel.
 * @IEEE80211_CHAN_NO_80MHZ: If the driver supports 80 MHz on the band,
 *      this flag indicates that an 80 MHz channel cannot use this
 *      channel as the control or any of the secondary channels.
 *      This may be due to the driver or due to regulatory bandwidth
 *      restrictions.
 * @IEEE80211_CHAN_NO_160MHZ: If the driver supports 160 MHz on the band,
 *      this flag indicates that an 160 MHz channel cannot use this
 *      channel as the control or any of the secondary channels.
 *      This may be due to the driver or due to regulatory bandwidth
 *      restrictions.
 * @IEEE80211_CHAN_INDOOR_ONLY: see %NL80211_FREQUENCY_ATTR_INDOOR_ONLY
 * @IEEE80211_CHAN_GO_CONCURRENT: see %NL80211_FREQUENCY_ATTR_GO_CONCURRENT
 * @IEEE80211_CHAN_NO_20MHZ: 20 MHz bandwidth is not permitted
 *      on this channel.
 * @IEEE80211_CHAN_NO_10MHZ: 10 MHz bandwidth is not permitted
 *      on this channel.
 *
 */
enum ieee80211_channel_flags {
	IEEE80211_CHAN_DISABLED         = 1<<0,
	IEEE80211_CHAN_NO_IR            = 1<<1,
	/* hole at 1<<2 */
	IEEE80211_CHAN_RADAR            = 1<<3,
	IEEE80211_CHAN_NO_HT40PLUS      = 1<<4,
	IEEE80211_CHAN_NO_HT40MINUS     = 1<<5,
	IEEE80211_CHAN_NO_OFDM          = 1<<6,
	IEEE80211_CHAN_NO_80MHZ         = 1<<7,
	IEEE80211_CHAN_NO_160MHZ        = 1<<8,
	IEEE80211_CHAN_INDOOR_ONLY      = 1<<9,
	IEEE80211_CHAN_IR_CONCURRENT    = 1<<10,
	IEEE80211_CHAN_NO_20MHZ         = 1<<11,
	IEEE80211_CHAN_NO_10MHZ         = 1<<12,
};

/* needed by mgmt/rlm_domain.c */
#define IEEE80211_CHAN_NO_HT40 \
	(IEEE80211_CHAN_NO_HT40PLUS | IEEE80211_CHAN_NO_HT40MINUS)

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */

#endif /* _GL_MTK_WIRELESS_H */
