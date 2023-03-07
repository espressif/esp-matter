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
/*
 ** Id: //Department/DaVinci/BRANCHES/MT6620_WIFI_DRIVER_V2_3/include
 *      /CFG_Wifi_File.h#1
 */

/*! \file   CFG_Wifi_File.h
 *    \brief  Collection of NVRAM structure used for YuSu project
 *
 *    In this file we collect all compiler flags and detail the driver behavior
 *    if enable/disable such switch or adjust numeric parameters.
 */

#ifndef _CFG_WIFI_FILE_H
#define _CFG_WIFI_FILE_H

/*******************************************************************************
 *                         C O M P I L E R   F L A G S
 *******************************************************************************
 */

/*******************************************************************************
 *                    E X T E R N A L   R E F E R E N C E S
 *******************************************************************************
 */
#include "gl_typedef.h"

/*******************************************************************************
 *                              C O N S T A N T S
 *******************************************************************************
 */

/*******************************************************************************
 *                             D A T A   T Y P E S
 *******************************************************************************
 */

/*******************************************************************************
 *                            P U B L I C   D A T A
 *******************************************************************************
 */
/* Connac define */
struct WIFI_NVRAM_2G4_TX_POWER_T {
	uint8_t uc24GhzTxPowerForCck1M;
	uint8_t uc24GhzTxPowerForCck2M;
	uint8_t uc24GhzTxPowerForCck55M;
	uint8_t uc24GhzTxPowerForCck11M;
	uint8_t uc24GhzTxPowerForOfdm6M;
	uint8_t uc24GhzTxPowerForOfdm9M;
	uint8_t uc24GhzTxPowerForOfdm12M;
	uint8_t uc24GhzTxPowerForOfdm18M;
	uint8_t uc24GhzTxPowerForOfdm24M;
	uint8_t uc24GhzTxPowerForOfdm36M;
	uint8_t uc24GhzTxPowerForOfdm48M;
	uint8_t uc24GhzTxPowerForOfdm54M;
	uint8_t uc24GTxPowerForHt20Mcs0_8_16_24;
	uint8_t uc24GTxPowerForHt20Mcs1_9_17_25;
	uint8_t uc24GTxPowerForHt20Mcs2_10_18_26;
	uint8_t uc24GTxPowerForHt20Mcs3_11_19_27;
	uint8_t uc24GTxPowerForHt20Mcs4_12_20_28;
	uint8_t uc24GTxPowerForHt20Mcs5_13_21_29;
	uint8_t uc24GTxPowerForHt20Mcs6_14_22_30;
	uint8_t uc24GTxPowerForHt20Mcs7_15_23_31;
	uint8_t uc24GTxPowerForHt40Mcs0_8_16_24;
	uint8_t uc24GTxPowerForHt40Mcs1_9_17_25;
	uint8_t uc24GTxPowerForHt40Mcs2_10_18_26;
	uint8_t uc24GTxPowerForHt40Mcs3_11_19_27;
	uint8_t uc24GTxPowerForHt40Mcs4_12_20_28;
	uint8_t uc24GTxPowerForHt40Mcs5_13_21_29;
	uint8_t uc24GTxPowerForHt40Mcs6_14_22_30;
	uint8_t uc24GTxPowerForHt40Mcs7_15_23_31;
	uint8_t uc24GTxPowerForVht20Mcs0;
	uint8_t uc24GTxPowerForVht20Mcs1;
	uint8_t uc24GTxPowerForVht20Mcs2;
	uint8_t uc24GTxPowerForVht20Mcs3;
	uint8_t uc24GTxPowerForVht20Mcs4;
	uint8_t uc24GTxPowerForVht20Mcs5;
	uint8_t uc24GTxPowerForVht20Mcs6;
	uint8_t uc24GTxPowerForVht20Mcs7;
	uint8_t uc24GTxPowerForVht20Mcs8;
	uint8_t uc24GTxPowerForVht20Mcs9;
	uint8_t uc24GTxPowerForVht40Mcs0;
	uint8_t uc24GTxPowerForVht40Mcs1;
	uint8_t uc24GTxPowerForVht40Mcs2;
	uint8_t uc24GTxPowerForVht40Mcs3;
	uint8_t uc24GTxPowerForVht40Mcs4;
	uint8_t uc24GTxPowerForVht40Mcs5;
	uint8_t uc24GTxPowerForVht40Mcs6;
	uint8_t uc24GTxPowerForVht40Mcs7;
	uint8_t uc24GTxPowerForVht40Mcs8;
	uint8_t uc24GTxPowerForVht40Mcs9;
	uint8_t uc24GTxPowerForHt40Mcs32;
	uint8_t uc24GTxPowerDeltaForLgVht40DuplicateMode;
};

struct WIFI_NVRAM_5G_TX_POWER_T {
	uint8_t uc5GhzTxPowerForOfdm6M;
	uint8_t uc5GhzTxPowerForOfdm9M;
	uint8_t uc5GhzTxPowerForOfdm12M;
	uint8_t uc5GhzTxPowerForOfdm18M;
	uint8_t uc5GhzTxPowerForOfdm24M;
	uint8_t uc5GhzTxPowerForOfdm36M;
	uint8_t uc5GhzTxPowerForOfdm48M;
	uint8_t uc5GhzTxPowerForOfdm54M;
	uint8_t uc5GTxPowerForHt20Mcs0_8_16_24;
	uint8_t uc5GTxPowerForHt20Mcs1_9_17_25;
	uint8_t uc5GTxPowerForHt20Mcs2_10_18_26;
	uint8_t uc5GTxPowerForHt20Mcs3_11_19_27;
	uint8_t uc5GTxPowerForHt20Mcs4_12_20_28;
	uint8_t uc5GTxPowerForHt20Mcs5_13_21_29;
	uint8_t uc5GTxPowerForHt20Mcs6_14_22_30;
	uint8_t uc5GTxPowerForHt20Mcs7_15_23_31;
	uint8_t uc5GTxPowerForHt40Mcs0_8_16_24;
	uint8_t uc5GTxPowerForHt40Mcs1_9_17_25;
	uint8_t uc5GTxPowerForHt40Mcs2_10_18_26;
	uint8_t uc5GTxPowerForHt40Mcs3_11_19_27;
	uint8_t uc5GTxPowerForHt40Mcs4_12_20_28;
	uint8_t uc5GTxPowerForHt40Mcs5_13_21_29;
	uint8_t uc5GTxPowerForHt40Mcs6_14_22_30;
	uint8_t uc5GTxPowerForHt40Mcs7_15_23_31;
	uint8_t uc5GTxPowerForVht20Mcs0;
	uint8_t uc5GTxPowerForVht20Mcs1;
	uint8_t uc5GTxPowerForVht20Mcs2;
	uint8_t uc5GTxPowerForVht20Mcs3;
	uint8_t uc5GTxPowerForVht20Mcs4;
	uint8_t uc5GTxPowerForVht20Mcs5;
	uint8_t uc5GTxPowerForVht20Mcs6;
	uint8_t uc5GTxPowerForVht20Mcs7;
	uint8_t uc5GTxPowerForVht20Mcs8;
	uint8_t uc5GTxPowerForVht20Mcs9;
	uint8_t uc5GTxPowerForVht40Mcs0;
	uint8_t uc5GTxPowerForVht40Mcs1;
	uint8_t uc5GTxPowerForVht40Mcs2;
	uint8_t uc5GTxPowerForVht40Mcs3;
	uint8_t uc5GTxPowerForVht40Mcs4;
	uint8_t uc5GTxPowerForVht40Mcs5;
	uint8_t uc5GTxPowerForVht40Mcs6;
	uint8_t uc5GTxPowerForVht40Mcs7;
	uint8_t uc5GTxPowerForVht40Mcs8;
	uint8_t uc5GTxPowerForVht40Mcs9;
	uint8_t uc5GTxPowerForVht80Mcs0;
	uint8_t uc5GTxPowerForVht80Mcs1;
	uint8_t uc5GTxPowerForVht80Mcs2;
	uint8_t uc5GTxPowerForVht80Mcs3;
	uint8_t uc5GTxPowerForVht80Mcs4;
	uint8_t uc5GTxPowerForVht80Mcs5;
	uint8_t uc5GTxPowerForVht80Mcs6;
	uint8_t uc5GTxPowerForVht80Mcs7;
	uint8_t uc5GTxPowerForVht80Mcs8;
	uint8_t uc5GTxPowerForVht80Mcs9;
	uint8_t uc5GTxPowerForVht160Mcs0;
	uint8_t uc5GTxPowerForVht160Mcs1;
	uint8_t uc5GTxPowerForVht160Mcs2;
	uint8_t uc5GTxPowerForVht160Mcs3;
	uint8_t uc5GTxPowerForVht160Mcs4;
	uint8_t uc5GTxPowerForVht160Mcs5;
	uint8_t uc5GTxPowerForVht160Mcs6;
	uint8_t uc5GTxPowerForVht160Mcs7;
	uint8_t uc5GTxPowerForVht160Mcs8;
	uint8_t uc5GTxPowerForVht160Mcs9;
	uint8_t uc5GTxPowerForHt40Mcs32;
	uint8_t uc5GTxPowerDeltaForLgVht40DuplicateMode;
	uint8_t uc5GTxPowerDeltaForLgVht80DuplicateMode;
	uint8_t uc5GTxPowerDeltaForLgVht160DuplicateMode;
	uint8_t uc20_5BwTxPowerDeltaFor5G;
	uint8_t uc20_10BwTxPowerDeltaFor5G;
};

struct WIFI_NVRAM_FE_LOSS_T {
	uint8_t u1TxFeLoss;
	uint8_t u1RxFeLoss;
};

struct WIFI_NVRAM_TSSI_T {
	uint8_t u1PATssiSlope;
	uint8_t u1PATssiOffset;
	uint8_t u1TxTargetPower;
};


struct WIFI_NVRAM_2G4_WF0_PATH_T {
	struct WIFI_NVRAM_FE_LOSS_T rFeLoss[14]; /*CH1 ~ CH14*/
	struct WIFI_NVRAM_TSSI_T rTssi[1];

	uint8_t uc2G4Tx0TssiOffTxPower;
	uint8_t aucTx2G4TxPowerOffsetLowCh[14];
	uint8_t aucTx2G4DpdG0OffsetrateH_L[6];
	uint8_t auc2G4Wf0Lna_Gain_CalDiff[2];
	uint8_t aucReserved0[2];
};

struct WIFI_NVRAM_5G_TX_POWER_OFFSET_T {
	uint8_t ucTxPowerOffsetLow;
	uint8_t ucTxPowerOffsetHigh;
};

struct WIFI_NVRAM_5G_TX_DPD_G0_OFFSET_T {
	uint8_t ucTxPowerOffsetLow;
	uint8_t ucTxPowerOffsetHigh;
};

struct WIFI_NVRAM_5G_WF0_PATH_T {
	struct WIFI_NVRAM_FE_LOSS_T rFeLoss[8];	/*Group 1 ~ Group 8*/
	struct WIFI_NVRAM_TSSI_T rTssi[8];		 /*Group 1 ~ Group 8*/
	uint8_t uc5GTx0TssiOffTxPower;
	/*Group 1 ~ Group 8*/
	struct WIFI_NVRAM_5G_TX_POWER_OFFSET_T rTxPowerOffset[8];

	uint8_t aucTx5GDpdG0OffsetrateH_L[48];
	uint8_t auc5GWf0Lna_Gain_CalDiff[16];
	uint8_t aucReserved3[3];

};

struct WIFI_NVRAM_TSSI_PA_CELL_OFFSET_T {
	uint8_t  ucTssiPACellOffsetHPA;
	uint8_t  ucTssiPACellOffsetMPA;
};


struct WIFI_NVRAM_DYNAMIC_IPA_SWITCH_T {
	uint8_t  ucPACckThreshold;
	uint8_t  ucPALowRateThreshold;
	uint8_t  ucPAMidRateThreshold;
	uint8_t  ucPAHighRateThreshold;
};

struct WIFI_NVRAM_2G4_ELNA_PARAM_T {
	uint8_t  ucELNAReceivedModeGain;
	uint8_t  ucELNAReceivedModeP1dB;
	uint8_t  ucELNABypassModeGain;
	uint8_t  ucELNABypassdModeP1dB;
};

struct WIFI_NVRAM_THERMO_COMP_T {
	uint8_t uc24GTssiOffMinus7StepNumber;
	uint8_t uc24GTssiOffMinus6StepNumber;
	uint8_t uc24GTssiOffMinus5StepNumber;
	uint8_t uc24GTssiOffMinus4StepNumber;
	uint8_t uc24GTssiOffMinus3StepNumber;
	uint8_t uc24GTssiOffMinus2StepNumber;
	uint8_t uc24GTssiOffMinus1StepNumber;
	uint8_t uc24GTssiOffMinus0StepNumber;
	uint8_t uc24GTssiOff24GReferenceStep;
	uint8_t uc24GTssiOff24GReferenceTemp;
	uint8_t uc24GTssiOffPlus1StepNumber;
	uint8_t uc24GTssiOffPlus2StepNumber;
	uint8_t uc24GTssiOffPlus3StepNumber;
	uint8_t uc24GTssiOffPlus4StepNumber;
	uint8_t uc24GTssiOffPlus5StepNumber;
	uint8_t uc24GTssiOffPlus6StepNumber;
	uint8_t uc24GTssiOffPlus7StepNumber;
};

struct WIFI_NVRAM_EPA_DPD_BOUND_T {
	uint8_t ucEpaDpdOnLowrBound;
	uint8_t ucEpaDpdOnUpperBound;
	uint8_t ucEpaDpdProtection[2];
};

struct WIFI_NVRAM_2G4_COMMON_T {
	struct WIFI_NVRAM_TSSI_PA_CELL_OFFSET_T rTssiPACellOffset[1];
	struct WIFI_NVRAM_DYNAMIC_IPA_SWITCH_T rDynamIPASwitch[3];
	struct WIFI_NVRAM_2G4_ELNA_PARAM_T r2G4ELNAParam[1];
	struct WIFI_NVRAM_THERMO_COMP_T rThermoComp;
	struct WIFI_NVRAM_EPA_DPD_BOUND_T rEPADpdBound[3];
	uint8_t aucReserved0[1];
};

struct WIFI_NVRAM_5G_ELNA_PARAM_T {
	uint8_t ucElnaReceivedModeGain;
	uint8_t ucElnaReceivedModeNf;
	uint8_t ucElnaReceivedModeP1Db;
	uint8_t ucElnaBypassModeGain;
	uint8_t ucElnaBypassModeNf;
	uint8_t ucElnaBypassModeP1Db;
};

struct WIFI_NVRAM_5G_COMMON_T {
	/*Group 1 ~ Group 8*/
	struct WIFI_NVRAM_TSSI_PA_CELL_OFFSET_T rTssiPACellOffset[8];
	struct WIFI_NVRAM_DYNAMIC_IPA_SWITCH_T rDynamIPASwitch[3];
	struct WIFI_NVRAM_5G_ELNA_PARAM_T r5GELNAParam[3];
	struct WIFI_NVRAM_THERMO_COMP_T rThermoComp;
	struct WIFI_NVRAM_EPA_DPD_BOUND_T rEPADpdBound[3];
	uint8_t aucReserved5[5];
};


/* end Connac TX Power define */

/* duplicated from nic_cmd_event.h to avoid header dependency */
struct TX_PWR_PARAM {
	int8_t cTxPwr2G4Cck;	/* signed, in unit of 0.5dBm */
	int8_t cTxPwr2G4Dsss;	/* signed, in unit of 0.5dBm */
	int8_t acReserved[2];

	int8_t cTxPwr2G4OFDM_BPSK;
	int8_t cTxPwr2G4OFDM_QPSK;
	int8_t cTxPwr2G4OFDM_16QAM;
	int8_t cTxPwr2G4OFDM_Reserved;
	int8_t cTxPwr2G4OFDM_48Mbps;
	int8_t cTxPwr2G4OFDM_54Mbps;

	int8_t cTxPwr2G4HT20_BPSK;
	int8_t cTxPwr2G4HT20_QPSK;
	int8_t cTxPwr2G4HT20_16QAM;
	int8_t cTxPwr2G4HT20_MCS5;
	int8_t cTxPwr2G4HT20_MCS6;
	int8_t cTxPwr2G4HT20_MCS7;

	int8_t cTxPwr2G4HT40_BPSK;
	int8_t cTxPwr2G4HT40_QPSK;
	int8_t cTxPwr2G4HT40_16QAM;
	int8_t cTxPwr2G4HT40_MCS5;
	int8_t cTxPwr2G4HT40_MCS6;
	int8_t cTxPwr2G4HT40_MCS7;

	int8_t cTxPwr5GOFDM_BPSK;
	int8_t cTxPwr5GOFDM_QPSK;
	int8_t cTxPwr5GOFDM_16QAM;
	int8_t cTxPwr5GOFDM_Reserved;
	int8_t cTxPwr5GOFDM_48Mbps;
	int8_t cTxPwr5GOFDM_54Mbps;

	int8_t cTxPwr5GHT20_BPSK;
	int8_t cTxPwr5GHT20_QPSK;
	int8_t cTxPwr5GHT20_16QAM;
	int8_t cTxPwr5GHT20_MCS5;
	int8_t cTxPwr5GHT20_MCS6;
	int8_t cTxPwr5GHT20_MCS7;

	int8_t cTxPwr5GHT40_BPSK;
	int8_t cTxPwr5GHT40_QPSK;
	int8_t cTxPwr5GHT40_16QAM;
	int8_t cTxPwr5GHT40_MCS5;
	int8_t cTxPwr5GHT40_MCS6;
	int8_t cTxPwr5GHT40_MCS7;
};

struct TX_AC_PWR {
	int8_t c11AcTxPwr_BPSK;
	int8_t c11AcTxPwr_QPSK;
	int8_t c11AcTxPwr_16QAM;
	int8_t c11AcTxPwr_MCS5_MCS6;
	int8_t c11AcTxPwr_MCS7;
	int8_t c11AcTxPwr_MCS8;
	int8_t c11AcTxPwr_MCS9;
	int8_t c11AcTxPwrVht40_OFFSET;
	int8_t c11AcTxPwrVht80_OFFSET;
	int8_t c11AcTxPwrVht160_OFFSET;
	int8_t acReverse[2];
};

struct RSSI_PATH_COMPASATION {
	int8_t c2GRssiCompensation;
	int8_t c5GRssiCompensation;
};

struct PWR_5G_OFFSET {
	int8_t cOffsetBand0;	/* 4.915-4.980G */
	int8_t cOffsetBand1;	/* 5.000-5.080G */
	int8_t cOffsetBand2;	/* 5.160-5.180G */
	int8_t cOffsetBand3;	/* 5.200-5.280G */
	int8_t cOffsetBand4;	/* 5.300-5.340G */
	int8_t cOffsetBand5;	/* 5.500-5.580G */
	int8_t cOffsetBand6;	/* 5.600-5.680G */
	int8_t cOffsetBand7;	/* 5.700-5.825G */
};

struct PWR_PARAM {
	uint32_t au4Data[28];
	uint32_t u4RefValue1;
	uint32_t u4RefValue2;
};

struct AC_PWR_SETTING_STRUCT {
	uint8_t c11AcTxPwr_BPSK;
	uint8_t c11AcTxPwr_QPSK;
	uint8_t c11AcTxPwr_16QAM;
	uint8_t c11AcTxPwr_MCS5_MCS6;
	uint8_t c11AcTxPwr_MCS7;
	uint8_t c11AcTxPwr_MCS8;
	uint8_t c11AcTxPwr_MCS9;
	uint8_t c11AcTxPwr_Reserved;
	uint8_t c11AcTxPwrVht40_OFFSET;
	uint8_t c11AcTxPwrVht80_OFFSET;
	uint8_t c11AcTxPwrVht160_OFFSET;
};

struct BANDEDGE_5G {
	uint8_t uc5GBandEdgePwrUsed;
	uint8_t c5GBandEdgeMaxPwrOFDM20;
	uint8_t c5GBandEdgeMaxPwrOFDM40;
	uint8_t c5GBandEdgeMaxPwrOFDM80;

};

struct NEW_EFUSE_MAPPING2NVRAM {
	uint8_t ucReverse1[8];
	uint16_t u2Signature;
	struct BANDEDGE_5G r5GBandEdgePwr;
	uint8_t ucReverse2[14];

	/* 0x50 */
	uint8_t aucChOffset[3];
	uint8_t ucChannelOffsetVaild;
	uint8_t acAllChannelOffset;
	uint8_t aucChOffset3[11];

	/* 0x60 */
	uint8_t auc5GChOffset[8];
	uint8_t uc5GChannelOffsetVaild;
	uint8_t aucChOffset4[7];

	/* 0x70 */
	struct AC_PWR_SETTING_STRUCT r11AcTxPwr;
	uint8_t uc11AcTxPwrValid;

	uint8_t ucReverse4[20];

	/* 0x90 */
	struct AC_PWR_SETTING_STRUCT r11AcTxPwr2G;
	uint8_t uc11AcTxPwrValid2G;

	uint8_t ucReverse5[40];
};

struct WIFI_CFG_PARAM_STRUCT {
	/* NVRAM offset[0] ~ offset[255] */
	uint16_t u2Part1OwnVersion;
	uint16_t u2Part1PeerVersion;
	uint8_t aucMacAddress[6];
	uint8_t aucCountryCode[2];
	uint8_t aucOldTxPwr0[185];

	uint8_t ucSupport5GBand;
	uint8_t aucOldTxPwr1[4];

	uint8_t ucRegChannelListMap;
	uint8_t ucRegChannelListIndex;
	uint8_t aucRegSubbandInfo[36];
	uint8_t ucEnable5GBand;	/* move from 256+ offset to here */
	uint8_t aucReserved0[256 - 241];

	/* NVRAM offset[256] ~ offset[511] */
	uint8_t ucTypeID0;
	uint8_t ucTypeLen0;
	uint8_t ucTxPowerControl; /*0: disable, 1: enable*/
	uint16_t u2PowerTotalSize;
	uint8_t ucTypeID1;
	uint8_t ucTypeLen1;
	struct WIFI_NVRAM_2G4_TX_POWER_T r2G4TxPower;
	uint8_t ucTypeID2;
	uint8_t ucTypeLen2;
	struct WIFI_NVRAM_5G_TX_POWER_T r5GTxpower;
	uint8_t ucTypeID3;
	uint8_t ucTypeLen3;
	struct WIFI_NVRAM_2G4_WF0_PATH_T r2G4WF0path;
	uint8_t ucTypeID4;
	uint8_t ucTypeLen4;
	struct WIFI_NVRAM_5G_WF0_PATH_T  r5GWF0path;
	uint8_t ucTypeID5;
	uint8_t ucTypeLen5;
	struct WIFI_NVRAM_2G4_COMMON_T r2G4Cmm;
	uint8_t ucTypeID6;
	uint8_t ucTypeLen6;
	struct WIFI_NVRAM_5G_COMMON_T r5GCmm;

	/* NVRAM offset[768] ~ offset[1023] */
	/* NVRAM offset[1024] ~ offset[1279] */
	/* NVRAM offset[1280] ~ offset[1535] */
	/* NVRAM offset[1536] ~ offset[1791] */
	/* NVRAM offset[1792] ~ offset[2047] */
	uint8_t aucReserved1[1346];
};


#if (CFG_SUPPORT_NVRAM_FRAGMENT == 1)
struct WIFI_NVRAM_TAG_FORMAT {
	uint8_t u1NvramTypeID;
	uint8_t u1NvramTypeLenLsb;
	uint8_t u1NvramTypeLenMsb;
};
#endif /*#if (CFG_SUPPORT_NVRAM_FRAGMENT == 1)*/
/*******************************************************************************
 *                           P R I V A T E   D A T A
 *******************************************************************************
 */

/*******************************************************************************
 *                                 M A C R O S
 *******************************************************************************
 */
#ifndef DATA_STRUCT_INSPECTING_ASSERT
#define DATA_STRUCT_INSPECTING_ASSERT(expr) \
		{switch (0) {case 0: case (expr): default:; } }
#endif

#define CFG_FILE_WIFI_REC_SIZE    sizeof(struct WIFI_CFG_PARAM_STRUCT)

/*******************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 *******************************************************************************
 */

/*******************************************************************************
 *                              F U N C T I O N S
 *******************************************************************************
 */
#ifndef _lint
/* We don't have to call following function to inspect the data structure.
 * It will check automatically while at compile time.
 * We'll need this to guarantee the same member order in different structures
 * to simply handling effort in some functions.
 */
static __KAL_INLINE__ void nvramOffsetCheck(void)
{
	DATA_STRUCT_INSPECTING_ASSERT(
		OFFSET_OF(struct WIFI_CFG_PARAM_STRUCT, ucTypeID0) == 256);

	DATA_STRUCT_INSPECTING_ASSERT(
		sizeof(struct WIFI_CFG_PARAM_STRUCT) == 2048);
}
#endif

#endif /* _CFG_WIFI_FILE_H */
