from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_INTERNAL
from py_2_and_3_compatibility import *

class Phy_Internal_Base(object):
    """
    Common Internal Phy functions live here
    """

    @staticmethod
    def MODEM_to_ZIF_for_FPGA_OTA_card(phy, model):
        """MODEM_to_ZIF_for_FPGA_OTA_card

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # Put modem in zero IF as that is how the ota card needs to be configured.
        phy.profile_outputs.MODEM_MIXCTRL_DIGIQSWAPEN.override = 0
        phy.profile_outputs.MODEM_DIGMIXCTRL_DIGMIXFREQ.override = 0
        phy.profile_outputs.MODEM_DCCOMP_DCCOMPEN.override = 0
        phy.profile_outputs.SYNTH_IFFREQ_LOSIDE.override = 0 # low side
        phy.profile_outputs.SYNTH_IFFREQ_IFFREQ.override = 0

    @staticmethod
    def AGC_FAST_LOOP_base(phy, model):
        phy.profile_outputs.AGC_CTRL0_PWRTARGET.override = 245
        phy.profile_outputs.AGC_CTRL0_MODE.override = 2
        phy.profile_outputs.AGC_CTRL0_DISCFLOOPADJ.override = 1
        phy.profile_outputs.AGC_CTRL0_ADCATTENCODE.override = 0
        phy.profile_outputs.AGC_CTRL0_ADCATTENMODE.override = 0
        phy.profile_outputs.AGC_CTRL0_DISPNGAINUP.override = 0
        phy.profile_outputs.AGC_CTRL0_DISPNDWNCOMP.override = 0
        phy.profile_outputs.AGC_CTRL0_DISRESETCHPWR.override = 0
        phy.profile_outputs.AGC_CTRL0_AGCRST.override = 0
		
        phy.profile_inputs.rssi_period.value = 3
        # phy.profile_outputs.AGC_CTRL1_RSSIPERIOD.override = 3
        phy.profile_outputs.AGC_CTRL1_PWRPERIOD.override = 1
        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            phy.profile_outputs.AGC_CTRL1_SUBPERIOD.override = 0
            phy.profile_outputs.AGC_CTRL1_SUBNUM.override = 0
            phy.profile_outputs.AGC_CTRL1_SUBDEN.override = 0
            phy.profile_outputs.AGC_CTRL1_SUBINT.override = 0
		
        phy.profile_outputs.AGC_CTRL2_PRSDEBUGEN.override = 0
        phy.profile_outputs.AGC_CTRL2_DMASEL.override = 0
        phy.profile_outputs.AGC_CTRL2_SAFEMODE.override = 0
        phy.profile_outputs.AGC_CTRL2_SAFEMODETHD.override = 3
        phy.profile_outputs.AGC_CTRL2_REHICNTTHD.override = 7
        phy.profile_outputs.AGC_CTRL2_RELOTHD.override = 4
        phy.profile_outputs.AGC_CTRL2_RELBYCHPWR.override = 3
        phy.profile_outputs.AGC_CTRL2_RELTARGETPWR.override = 236
        phy.profile_outputs.AGC_CTRL2_DISRFPKD.override = 0
		
        phy.profile_outputs.AGC_CTRL3_RFPKDDEB.override = 1
        phy.profile_outputs.AGC_CTRL3_RFPKDDEBTHD.override = 1
        phy.profile_outputs.AGC_CTRL3_RFPKDDEBPRD.override = 40
        phy.profile_outputs.AGC_CTRL3_RFPKDDEBRST.override = 10
        phy.profile_outputs.AGC_CTRL3_IFPKDDEB.override = 1
        phy.profile_outputs.AGC_CTRL3_IFPKDDEBTHD.override = 1
        phy.profile_outputs.AGC_CTRL3_IFPKDDEBPRD.override = 40
        phy.profile_outputs.AGC_CTRL3_IFPKDDEBRST.override = 10
		

        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            phy.profile_outputs.AGC_AGCPERIOD_PERIODHI.override = 14
            phy.profile_outputs.AGC_AGCPERIOD_PERIODLO.override = 45
            phy.profile_outputs.AGC_AGCPERIOD_MAXHICNTTHD.override = 9
            phy.profile_outputs.AGC_AGCPERIOD_SETTLETIMEIF.override = 6
            phy.profile_outputs.AGC_AGCPERIOD_SETTLETIMERF.override = 14
            phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.override = 4
            phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPDEL.override = 0
            phy.profile_outputs.AGC_GAINSTEPLIM_HYST.override = 3
            phy.profile_outputs.AGC_GAINSTEPLIM_MAXPWRVAR.override = 0
            phy.profile_outputs.AGC_GAINSTEPLIM_TRANRSTAGC.override = 0
            phy.profile_outputs.AGC_GAINRANGE_LNAINDEXBORDER.override = 7
            phy.profile_outputs.AGC_GAINRANGE_PGAINDEXBORDER.override = 8
        phy.profile_outputs.AGC_GAINRANGE_GAININCSTEP.override = 1
        phy.profile_outputs.AGC_GAINRANGE_LATCHEDHISTEP.override = 0
        phy.profile_outputs.AGC_GAINRANGE_PNGAINSTEP.override = 4
        phy.profile_outputs.AGC_GAINRANGE_HIPWRTHD.override = 3
        
        phy.profile_outputs.AGC_MANGAIN_MANGAINEN.override = 0
        phy.profile_outputs.AGC_MANGAIN_MANGAINIFPGA.override = 7
        phy.profile_outputs.AGC_MANGAIN_MANGAINLNA.override = 1
        phy.profile_outputs.AGC_MANGAIN_MANGAINPN.override = 1

        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            phy.profile_outputs.AGC_HICNTREGION_HICNTREGION0.override = 4
            phy.profile_outputs.AGC_HICNTREGION_HICNTREGION1.override = 5
            phy.profile_outputs.AGC_HICNTREGION_HICNTREGION2.override = 6
            phy.profile_outputs.AGC_HICNTREGION_HICNTREGION3.override = 7
            phy.profile_outputs.AGC_HICNTREGION_HICNTREGION4.override = 8
        
        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 5
        
        phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTPER.override = 0
        phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTTHR.override = 0
        phy.profile_outputs.AGC_RSSISTEPTHR_POSSTEPTHR.override = 0

        if model.part_family.lower() in ["panther", "lynx", "leopard"]:
            ##### RF peak detector threshold configuration
            phy.profile_outputs.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.override = 2

		##### IF peak detector configuration
		# enable I/Q latch
        phy.profile_outputs.RAC_PGACTRL_PGAENLATCHI.override = 1
        phy.profile_outputs.RAC_PGACTRL_PGAENLATCHQ.override = 1
		# set IF peak detector threshold
        phy.profile_outputs.RAC_PGACTRL_PGATHRPKDHISEL.override = 5
        phy.profile_outputs.RAC_PGACTRL_PGATHRPKDLOSEL.override = 1

    @staticmethod
    def AGC_SLOW_LOOP_base(phy, model):
        phy.profile_outputs.AGC_CTRL0_PWRTARGET.override = 245
        phy.profile_outputs.AGC_CTRL0_MODE.override = 2
        phy.profile_outputs.AGC_CTRL0_DISCFLOOPADJ.override = 0
        phy.profile_outputs.AGC_CTRL0_ADCATTENCODE.override = 0
        phy.profile_outputs.AGC_CTRL0_ADCATTENMODE.override = 0
        phy.profile_outputs.AGC_CTRL0_DISPNGAINUP.override = 0
        phy.profile_outputs.AGC_CTRL0_DISPNDWNCOMP.override = 0
        phy.profile_outputs.AGC_CTRL0_DISRESETCHPWR.override = 0
        phy.profile_outputs.AGC_CTRL0_AGCRST.override = 0

        # phy.profile_outputs.AGC_CTRL1_RSSIPERIOD.override = 8
        phy.profile_inputs.rssi_period.value = 8
        phy.profile_outputs.AGC_CTRL1_PWRPERIOD.override = 1
        phy.profile_outputs.AGC_CTRL1_SUBPERIOD.override = 0
        phy.profile_outputs.AGC_CTRL1_SUBNUM.override = 0
        phy.profile_outputs.AGC_CTRL1_SUBDEN.override = 0
        phy.profile_outputs.AGC_CTRL1_SUBINT.override = 0

        phy.profile_outputs.AGC_CTRL2_PRSDEBUGEN.override = 0
        phy.profile_outputs.AGC_CTRL2_DMASEL.override = 0
        phy.profile_outputs.AGC_CTRL2_SAFEMODE.override = 0
        phy.profile_outputs.AGC_CTRL2_SAFEMODETHD.override = 3
        phy.profile_outputs.AGC_CTRL2_REHICNTTHD.override = 7
        phy.profile_outputs.AGC_CTRL2_RELOTHD.override = 4
        phy.profile_outputs.AGC_CTRL2_RELBYCHPWR.override = 3
        phy.profile_outputs.AGC_CTRL2_RELTARGETPWR.override = 236
        phy.profile_outputs.AGC_CTRL2_DISRFPKD.override = 0

        phy.profile_outputs.AGC_CTRL3_RFPKDDEB.override = 1
        phy.profile_outputs.AGC_CTRL3_RFPKDDEBTHD.override = 2
        phy.profile_outputs.AGC_CTRL3_RFPKDDEBPRD.override = 40
        phy.profile_outputs.AGC_CTRL3_RFPKDDEBRST.override = 10
        phy.profile_outputs.AGC_CTRL3_IFPKDDEB.override = 0
        phy.profile_outputs.AGC_CTRL3_IFPKDDEBTHD.override = 2
        phy.profile_outputs.AGC_CTRL3_IFPKDDEBPRD.override = 40
        phy.profile_outputs.AGC_CTRL3_IFPKDDEBRST.override = 10

        phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPSTEPMAX.override = 4
        phy.profile_outputs.AGC_GAINSTEPLIM_CFLOOPDEL.override = 40
        phy.profile_outputs.AGC_GAINSTEPLIM_HYST.override = 3
        phy.profile_outputs.AGC_GAINSTEPLIM_MAXPWRVAR.override = 0
        phy.profile_outputs.AGC_GAINSTEPLIM_TRANRSTAGC.override = 0

        phy.profile_outputs.AGC_AGCPERIOD_PERIODHI.override = 14
        phy.profile_outputs.AGC_AGCPERIOD_PERIODLO.override = 45
        phy.profile_outputs.AGC_AGCPERIOD_MAXHICNTTHD.override = 7
        phy.profile_outputs.AGC_AGCPERIOD_SETTLETIMEIF.override = 6
        phy.profile_outputs.AGC_AGCPERIOD_SETTLETIMERF.override = 14

        phy.profile_outputs.AGC_GAINRANGE_LNAINDEXBORDER.override = 7
        phy.profile_outputs.AGC_GAINRANGE_PGAINDEXBORDER.override = 8
        phy.profile_outputs.AGC_GAINRANGE_GAININCSTEP.override = 1
        phy.profile_outputs.AGC_GAINRANGE_LATCHEDHISTEP.override = 0
        phy.profile_outputs.AGC_GAINRANGE_PNGAINSTEP.override = 3
        phy.profile_outputs.AGC_GAINRANGE_HIPWRTHD.override = 3

        phy.profile_outputs.AGC_MANGAIN_MANGAINEN.override = 0
        phy.profile_outputs.AGC_MANGAIN_MANGAINIFPGA.override = 7
        phy.profile_outputs.AGC_MANGAIN_MANGAINLNA.override = 1
        phy.profile_outputs.AGC_MANGAIN_MANGAINPN.override = 1

        phy.profile_outputs.AGC_HICNTREGION_HICNTREGION0.override = 3
        phy.profile_outputs.AGC_HICNTREGION_HICNTREGION1.override = 4
        phy.profile_outputs.AGC_HICNTREGION_HICNTREGION2.override = 5
        phy.profile_outputs.AGC_HICNTREGION_HICNTREGION3.override = 6
        phy.profile_outputs.AGC_HICNTREGION_HICNTREGION4.override = 8

        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 0
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 3

        phy.profile_outputs.AGC_RSSISTEPTHR_POSSTEPTHR.override = 3
        phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTTHR.override = 0xab
        phy.profile_outputs.AGC_RSSISTEPTHR_DEMODRESTARTPER.override = 5

        ##### RF peak detector threshold configuration
        phy.profile_outputs.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.override = 2

        ##### IF peak detector configuration
        # enable I/Q latch
        phy.profile_outputs.RAC_PGACTRL_PGAENLATCHI.override = 1
        phy.profile_outputs.RAC_PGACTRL_PGAENLATCHQ.override = 1
        # set IF peak detector threshold
        phy.profile_outputs.RAC_PGACTRL_PGATHRPKDHISEL.override = 5
        phy.profile_outputs.RAC_PGACTRL_PGATHRPKDLOSEL.override = 1

    @staticmethod
    def GFSK_915M_base(phy, model):
        """GFSK_915M_base

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        phy.profile_inputs.base_frequency_hz.value = long(915000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.deviation.value = 50000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 40
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.syncword_0.value = long(0xf68d)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        PHY_COMMON_FRAME_INTERNAL(phy, model)

        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.agc_speed.value = model.vars.agc_speed.var_enum.FAST

    @staticmethod
    def GFSK_2400M_base(phy, model):
        """GFSK_2400M_base

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        phy.profile_inputs.base_frequency_hz.value = long(2450000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.channel_spacing_hz.value = 1000000
        phy.profile_inputs.deviation.value = 50000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 40
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 0.5
        phy.profile_inputs.syncword_0.value = long(0xf68d)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 16
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000

        PHY_COMMON_FRAME_INTERNAL(phy, model)

        phy.profile_inputs.agc_power_target.value = -8
        phy.profile_inputs.symbols_in_timing_window.value = 14
        phy.profile_inputs.timing_detection_threshold.value = 20
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.agc_speed.value = model.vars.agc_speed.var_enum.FAST