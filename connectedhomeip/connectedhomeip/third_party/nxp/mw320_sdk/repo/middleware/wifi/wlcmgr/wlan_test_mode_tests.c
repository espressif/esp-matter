/** @file wlan_test_mode_tests.c
 *
 *  @brief  This file provides WLAN Test Mode APIs
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

#include <wlan.h>
#include <cli.h>
#include <cli_utils.h>
#include <string.h>
#include <wm_net.h> /* for net_inet_aton */
#include <wifi.h>
#include <wlan_tests.h>

/*
 * NXP Test Framework (MTF) functions
 */

#ifdef CONFIG_RF_TEST_MODE

static bool rf_test_mode = false;

static void dump_wlan_set_rf_test_mode_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-test-mode \r\n");
    PRINTF("\r\n");
}

static void dump_wlan_set_rf_test_mode()
{
    PRINTF("RF Test Mode is not set\r\n");
    dump_wlan_set_rf_test_mode_usage();
}

static void wlan_rf_test_mode_set(int argc, char *argv[])
{
    int ret;

    if (argc != 1)
    {
        dump_wlan_set_rf_test_mode_usage();
        return;
    }

    ret = wlan_set_rf_test_mode();
    if (ret == WM_SUCCESS)
    {
        rf_test_mode = true;
        PRINTF("RF Test Mode configuration successful\r\n");
    }
    else
    {
        PRINTF("RF Test Mode configuration failed\r\n");
        dump_wlan_set_rf_test_mode_usage();
    }
}

static void dump_wlan_set_channel_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-channel <channel> \r\n");
    PRINTF("\r\n");
}

static void wlan_rf_channel_set(int argc, char *argv[])
{
    int ret;
    uint8_t channel;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 2)
    {
        dump_wlan_set_channel_usage();
        return;
    }

    channel = atoi(argv[1]);

    ret = wlan_set_rf_channel(channel);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Channel configuration successful\r\n");
    }
    else
    {
        PRINTF("Channel configuration failed\r\n");
        dump_wlan_set_channel_usage();
    }
}

static void dump_wlan_get_channel_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-get-rf-channel \r\n");
}

static void wlan_rf_channel_get(int argc, char *argv[])
{
    int ret;
    uint8_t channel;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 1)
    {
        dump_wlan_get_channel_usage();
        return;
    }

    ret = wlan_get_rf_channel(&channel);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Configured channel is: %d\r\n", channel);
    }
    else
    {
        PRINTF("Channel configuration read failed\r\n");
        dump_wlan_get_channel_usage();
    }
}

static void dump_wlan_set_rf_band_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-band <band> \r\n");
#ifdef CONFIG_5GHz_SUPPORT
    PRINTF("band: 0=2.4G, 1=5G \r\n");
#else
    PRINTF("band: 0=2.4G \r\n");
#endif
    PRINTF("\r\n");
}

static void wlan_rf_band_set(int argc, char *argv[])
{
    int ret;
    uint8_t band;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 2)
    {
        dump_wlan_set_rf_band_usage();
        return;
    }

    band = atoi(argv[1]);

    if (band != 0
#ifdef CONFIG_5GHz_SUPPORT
        && band != 1
#endif
    )
    {
        dump_wlan_set_rf_band_usage();
        return;
    }

    ret = wlan_set_rf_band(band);
    if (ret == WM_SUCCESS)
    {
        PRINTF("RF Band configuration successful\r\n");
    }
    else
    {
        PRINTF("RF Band configuration failed\r\n");
        dump_wlan_set_rf_band_usage();
    }
}

static void dump_wlan_get_rf_band_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-get-rf-band \r\n");
}

static void wlan_rf_band_get(int argc, char *argv[])
{
    int ret;
    uint8_t band;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 1)
    {
        dump_wlan_get_rf_band_usage();
        return;
    }

    ret = wlan_get_rf_band(&band);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Configured RF Band is: %s\r\n", band ? "5G" : "2.4G");
    }
    else
    {
        PRINTF("RF Band configuration read failed\r\n");
        dump_wlan_get_rf_band_usage();
    }
}

static void dump_wlan_set_bandwidth_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-bandwidth <bandwidth> \r\n");
    PRINTF("\r\n");
    PRINTF("\t<bandwidth>: \r\n");
    PRINTF("\t        0: 20MHz\r\n");
#ifdef CONFIG_5GHz_SUPPORT
    PRINTF("\t        1: 40MHz\r\n");
#endif
    PRINTF("\r\n");
}

static void wlan_rf_bandwidth_set(int argc, char *argv[])
{
    int ret;
    uint8_t bandwidth;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 2)
    {
        dump_wlan_set_bandwidth_usage();
        return;
    }

    bandwidth = atoi(argv[1]);

    ret = wlan_set_rf_bandwidth(bandwidth);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Bandwidth configuration successful\r\n");
    }
    else
    {
        PRINTF("Bandwidth configuration failed\r\n");
        dump_wlan_set_bandwidth_usage();
    }
}

static void dump_wlan_get_bandwidth_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-get-rf-bandwidth \r\n");
}

static void wlan_rf_bandwidth_get(int argc, char *argv[])
{
    int ret;
    uint8_t bandwidth;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 1)
    {
        dump_wlan_get_bandwidth_usage();
        return;
    }

    ret = wlan_get_rf_bandwidth(&bandwidth);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Configured bandwidth is: %s\r\n", bandwidth == 0 ? "20MHz" : bandwidth == 1 ? "40MHz" : "80MHz");
    }
    else
    {
        PRINTF("Bandwidth configuration read failed\r\n");
        dump_wlan_get_bandwidth_usage();
    }
}

static void dump_wlan_get_and_reset_per_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-get-rf-per \r\n");
}

static void wlan_rf_per_get(int argc, char *argv[])
{
    int ret;
    uint32_t rx_tot_pkt_count, rx_mcast_bcast_count, rx_pkt_fcs_error;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 1)
    {
        dump_wlan_get_and_reset_per_usage();
        return;
    }

    ret = wlan_get_rf_per(&rx_tot_pkt_count, &rx_mcast_bcast_count, &rx_pkt_fcs_error);
    if (ret == WM_SUCCESS)
    {
        PRINTF("PER is as below: \r\n");
        PRINTF("  Total Rx Packet Count                    : %d\r\n", rx_tot_pkt_count);
        PRINTF("  Total Rx Multicast/Broadcast Packet Count: %d\r\n", rx_mcast_bcast_count);
        PRINTF("  Total Rx Packets with FCS error          : %d\r\n", rx_pkt_fcs_error);
    }
    else
    {
        PRINTF("PER configuration read failed\r\n");
        dump_wlan_get_and_reset_per_usage();
    }
}

static void dump_wlan_set_tx_cont_mode_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-tx-cont-mode <enable_tx> <cw_mode> <payload_pattern> <cs_mode> <act_sub_ch> <tx_rate> \r\n");
    PRINTF("Enable                (0:disable, 1:enable)\r\n");
    PRINTF("Continuous Wave Mode  (0:disable, 1:enable)\r\n");
    PRINTF("Payload Pattern       (0 to 0xFFFFFFFF) (Enter hexadecimal value)\r\n");
    PRINTF("CS Mode               (Applicable only when continuous wave is disabled) (0:disable, 1:enable)\r\n");
    PRINTF("Active SubChannel     (0:low, 1:upper, 3:both)\r\n");
    PRINTF("Tx Data Rate          (Rate Index corresponding to legacy/HT/VHT rates)\r\n");
    PRINTF("\r\n");
}

static void wlan_rf_tx_cont_mode_set(int argc, char *argv[])
{
    int ret;
    uint32_t enable_tx, cw_mode, payload_pattern, cs_mode, act_sub_ch, tx_rate;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 7)
    {
        dump_wlan_set_tx_cont_mode_usage();
        return;
    }

    enable_tx       = atoi(argv[1]);
    cw_mode         = atoi(argv[2]);
    payload_pattern = strtol(argv[3], NULL, 16);
    cs_mode         = atoi(argv[4]);
    act_sub_ch      = atoi(argv[5]);
    tx_rate         = atoi(argv[6]);

    ret = wlan_set_rf_tx_cont_mode(enable_tx, cw_mode, payload_pattern, cs_mode, act_sub_ch, tx_rate);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Tx continuous configuration successful\r\n");
        PRINTF("  Enable                : %s\r\n", enable_tx ? "enable" : "disable");
        PRINTF("  Continuous Wave Mode  : %s\r\n", cw_mode ? "enable" : "disable");
        PRINTF("  Payload Pattern       : 0x%08X\r\n", payload_pattern);
        PRINTF("  CS Mode               : %s\r\n", cs_mode ? "enable" : "disable");
        PRINTF("  Active SubChannel     : %s\r\n", act_sub_ch == 0 ? "low" : act_sub_ch == 1 ? "upper" : "both");
        PRINTF("  Tx Data Rate          : %d\r\n", tx_rate);
    }
    else
    {
        PRINTF("Tx continuous configuration failed\r\n");
        dump_wlan_set_tx_cont_mode_usage();
    }
}

static void dump_wlan_set_tx_antenna_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-tx-antenna <antenna> \r\n");
    PRINTF("antenna: 1=Main, 2=Aux \r\n");
    PRINTF("\r\n");
}

static void wlan_rf_tx_antenna_set(int argc, char *argv[])
{
    int ret;
    uint8_t ant;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 2)
    {
        dump_wlan_set_tx_antenna_usage();
        return;
    }

    ant = atoi(argv[1]);

    if (ant != 1 && ant != 2)
    {
        dump_wlan_set_tx_antenna_usage();
        return;
    }

    ret = wlan_set_rf_tx_antenna(ant);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Tx Antenna configuration successful\r\n");
    }
    else
    {
        PRINTF("Tx Antenna configuration failed\r\n");
        dump_wlan_set_tx_antenna_usage();
    }
}

static void dump_wlan_get_tx_antenna_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-get-rf-tx-antenna \r\n");
}

static void wlan_rf_tx_antenna_get(int argc, char *argv[])
{
    int ret;
    uint8_t ant;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 1)
    {
        dump_wlan_get_tx_antenna_usage();
        return;
    }

    ret = wlan_get_rf_tx_antenna(&ant);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Configured Tx Antenna is: %s\r\n", ant == 1 ? "Main" : "Aux");
    }
    else
    {
        PRINTF("Tx Antenna configuration read failed\r\n");
        dump_wlan_get_tx_antenna_usage();
    }
}

static void dump_wlan_set_rx_antenna_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-rx-antenna <antenna> \r\n");
    PRINTF("antenna: 1=Main, 2=Aux \r\n");
    PRINTF("\r\n");
}

static void wlan_rf_rx_antenna_set(int argc, char *argv[])
{
    int ret;
    uint8_t ant;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 2)
    {
        dump_wlan_set_rx_antenna_usage();
        return;
    }

    ant = atoi(argv[1]);

    if (ant != 1 && ant != 2)
    {
        dump_wlan_set_rx_antenna_usage();
        return;
    }

    ret = wlan_set_rf_rx_antenna(ant);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Rx Antenna configuration successful\r\n");
    }
    else
    {
        PRINTF("Rx Antenna configuration failed\r\n");
        dump_wlan_set_rx_antenna_usage();
    }
}

static void dump_wlan_get_rx_antenna_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-get-rf-rx-antenna \r\n");
}

static void wlan_rf_rx_antenna_get(int argc, char *argv[])
{
    int ret;
    uint8_t ant;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 1)
    {
        dump_wlan_get_rx_antenna_usage();
        return;
    }

    ret = wlan_get_rf_rx_antenna(&ant);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Configured Rx Antenna is: %s\r\n", ant == 1 ? "Main" : "Aux");
    }
    else
    {
        PRINTF("Rx Antenna configuration read failed\r\n");
        dump_wlan_get_rx_antenna_usage();
    }
}

static void dump_wlan_set_tx_power_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF("wlan-set-rf-tx-power <tx_power> <modulation> <path_id> \r\n");
    PRINTF("Power       (0 to 24 dBm)\r\n");
    PRINTF("Modulation  (0: CCK, 1:OFDM, 2:MCS)\r\n");
    PRINTF("Path ID     (0: PathA, 1:PathB, 2:PathA+B)\r\n");
    PRINTF("\r\n");
}

static void wlan_rf_tx_power_set(int argc, char *argv[])
{
    int ret;
    uint8_t power;
    uint8_t mod;
    uint8_t path_id;

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 4)
    {
        dump_wlan_set_tx_power_usage();
        return;
    }

    power   = atoi(argv[1]);
    mod     = atoi(argv[2]);
    path_id = atoi(argv[3]);

    if (power > 24)
    {
        dump_wlan_set_rx_antenna_usage();
        return;
    }

    if (mod != 0 && mod != 1 && mod != 2)
    {
        dump_wlan_set_rx_antenna_usage();
        return;
    }

    if (path_id != 0 && path_id != 1 && path_id != 2)
    {
        dump_wlan_set_rx_antenna_usage();
        return;
    }

    ret = wlan_set_rf_tx_power(power, mod, path_id);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Tx Power configuration successful\r\n");
        PRINTF("  Power         : %d dBm\r\n", power);
        PRINTF("  Modulation    : %s\r\n", mod == 0 ? "CCK" : mod == 1 ? "OFDM" : "MCS");
        PRINTF("  Path ID       : %s\r\n", path_id == 0 ? "PathA" : path_id == 1 ? "PathB" : "PathA+B");
    }
    else
    {
        PRINTF("Tx Power configuration failed\r\n");
        dump_wlan_set_tx_power_usage();
    }
}

static void dump_wlan_set_tx_frame_usage()
{
    PRINTF("Usage:\r\n");
    PRINTF(
        "wlan-set-rf-tx-frame <start> <data_rate> <frame_pattern> <frame_len> <adjust_burst_sifs> <burst_sifs_in_us> "
        "<short_preamble> <act_sub_ch> <short_gi> <adv_coding> <tx_bf> <gf_mode> <stbc> <bssid>\r\n");
    PRINTF("Enable                 (0:disable, 1:enable)\r\n");
    PRINTF("Tx Data Rate           (Rate Index corresponding to legacy/HT/VHT rates)\r\n");
    PRINTF("Payload Pattern        (0 to 0xFFFFFFFF) (Enter hexadecimal value)\r\n");
    PRINTF("Payload Length         (1 to 0x400) (Enter hexadecimal value)\r\n");
    PRINTF("Adjust Burst SIFS3 Gap (0:disable, 1:enable)\r\n");
    PRINTF("Burst SIFS in us       (0 to 255us)\r\n");
    PRINTF("Short Preamble         (0:disable, 1:enable)\r\n");
    PRINTF("Active SubChannel      (0:low, 1:upper, 3:both)\r\n");
    PRINTF("Short GI               (0:disable, 1:enable)\r\n");
    PRINTF("Adv Coding             (0:disable, 1:enable)\r\n");
    PRINTF("Beamforming            (0:disable, 1:enable)\r\n");
    PRINTF("GreenField Mode        (0:disable, 1:enable)\r\n");
    PRINTF("STBC                   (0:disable, 1:enable)\r\n");
    PRINTF("BSSID                  (xx:xx:xx:xx:xx:xx)\r\n");
    PRINTF("\r\n");
}

static void wlan_rf_tx_frame_set(int argc, char *argv[])
{
    int ret;
    uint32_t enable;
    uint32_t data_rate;
    uint32_t frame_pattern;
    uint32_t frame_length;
    uint32_t adjust_burst_sifs;
    uint32_t burst_sifs_in_us;
    uint32_t short_preamble;
    uint32_t act_sub_ch;
    uint32_t short_gi;
    uint32_t adv_coding;
    uint32_t tx_bf;
    uint32_t gf_mode;
    uint32_t stbc;
    uint32_t bssid[MLAN_MAC_ADDR_LENGTH];

    if (!rf_test_mode)
    {
        dump_wlan_set_rf_test_mode();
        return;
    }

    if (argc != 15)
    {
        dump_wlan_set_tx_frame_usage();
        return;
    }

    enable            = atoi(argv[1]);
    data_rate         = atoi(argv[2]);
    frame_pattern     = strtol(argv[3], NULL, 16);
    frame_length      = strtol(argv[4], NULL, 16);
    adjust_burst_sifs = atoi(argv[5]);
    burst_sifs_in_us  = atoi(argv[6]);
    short_preamble    = atoi(argv[7]);
    act_sub_ch        = atoi(argv[8]);
    short_gi          = atoi(argv[9]);
    adv_coding        = atoi(argv[10]);
    tx_bf             = atoi(argv[11]);
    gf_mode           = atoi(argv[12]);
    stbc              = atoi(argv[13]);
    ret               = get_mac((const char *)argv[14], (char *)bssid, ':');
    if (ret)
    {
        dump_wlan_set_tx_frame_usage();
        return;
    }

    if (enable > 1 || frame_length < 1 || frame_length > 0x400 || burst_sifs_in_us > 255 || short_preamble > 1 ||
        act_sub_ch == 2 || act_sub_ch > 3 || short_gi > 1 || adv_coding > 1 || tx_bf > 1 || gf_mode > 1 || stbc > 1)
    {
        dump_wlan_set_rx_antenna_usage();
        return;
    }

    ret = wlan_set_rf_tx_frame(enable, data_rate, frame_pattern, frame_length, adjust_burst_sifs, burst_sifs_in_us,
                               short_preamble, act_sub_ch, short_gi, adv_coding, tx_bf, gf_mode, stbc, bssid);
    if (ret == WM_SUCCESS)
    {
        PRINTF("Tx Frame configuration successful\r\n");
        PRINTF("  Enable                    : %s\r\n", enable ? "enable" : "disable");
        PRINTF("  Tx Data Rate              : %d\r\n", data_rate);
        PRINTF("  Payload Pattern           : 0x%08X\r\n", frame_pattern);
        PRINTF("  Payload Length            : 0x%08X\r\n", frame_length);
        PRINTF("  Adjust Burst SIFS3 Gap    : %s\r\n", adjust_burst_sifs ? "enable" : "disable");
        PRINTF("  Burst SIFS in us          : %d us\r\n", burst_sifs_in_us);
        PRINTF("  Short Preamble            : %s\r\n", short_preamble ? "enable" : "disable");
        PRINTF("  Active SubChannel         : %s\r\n", act_sub_ch == 0 ? "low" : act_sub_ch == 1 ? "upper" : "both");
        PRINTF("  Short GI                  : %s\r\n", short_gi ? "enable" : "disable");
        PRINTF("  Adv Coding                : %s\r\n", adv_coding ? "enable" : "disable");
        PRINTF("  Beamforming               : %s\r\n", tx_bf ? "enable" : "disable");
        PRINTF("  GreenField Mode           : %s\r\n", gf_mode ? "enable" : "disable");
        PRINTF("  STBC                      : %s\r\n", stbc ? "enable" : "disable");
        PRINTF("  BSSID                     : ");
        print_mac((const char *)bssid);
        PRINTF("\r\n");
    }
    else
    {
        PRINTF("Tx Frame configuration failed\r\n");
        dump_wlan_set_tx_frame_usage();
    }
}

static struct cli_command wlan_test_mode_commands[] = {
    {"wlan-set-rf-test-mode", NULL, wlan_rf_test_mode_set},
    {"wlan-set-rf-channel", "<channel>", wlan_rf_channel_set},
    {"wlan-get-rf-channel", NULL, wlan_rf_channel_get},
    {"wlan-set-rf-band", "<band>", wlan_rf_band_set},
    {"wlan-get-rf-band", NULL, wlan_rf_band_get},
    {"wlan-set-rf-bandwidth", "<bandwidth>", wlan_rf_bandwidth_set},
    {"wlan-get-rf-bandwidth", NULL, wlan_rf_bandwidth_get},
    {"wlan-get-and-reset-rf-per", NULL, wlan_rf_per_get},
    {"wlan-set-rf-tx-cont-mode", "<enable_tx> <cw_mode> <payload_pattern> <cs_mode> <act_sub_ch> <tx_rate>",
     wlan_rf_tx_cont_mode_set},
    {"wlan-set-rf-tx-antenna", "<antenna>", wlan_rf_tx_antenna_set},
    {"wlan-get-rf-tx-antenna", NULL, wlan_rf_tx_antenna_get},
    {"wlan-set-rf-rx-antenna", "<antenna>", wlan_rf_rx_antenna_set},
    {"wlan-get-rf-rx-antenna", NULL, wlan_rf_rx_antenna_get},
    {"wlan-set-rf-tx-power", "<tx_power> <modulation> <path_id>", wlan_rf_tx_power_set},
    {"wlan-set-rf-tx-frame",
     "<start> <data_rate> <frame_pattern> <frame_len> <adjust_burst_sifs> <burst_sifs_in_us> <short_preamble> "
     "<act_sub_ch> <short_gi> <adv_coding> <tx_bf> <gf_mode> <stbc> <bssid>",
     wlan_rf_tx_frame_set},
};

int wlan_test_mode_cli_init(void)
{
    if (cli_register_commands(wlan_test_mode_commands, sizeof(wlan_test_mode_commands) / sizeof(struct cli_command)))
        return -WM_FAIL;

    return WM_SUCCESS;
}
#endif
