from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_154

from py_2_and_3_compatibility import *

class PHYS_Sigfox(IPhy):

    # #original PHY from s029 used to derive the following DBPSK PHY
    # def _PHY_Sigfox_868MHz_DBPSK_2Mbps_forced(self, model):
    #     phy = self._makePhy(model, model.profiles.Base, readable_name='100bps 868MHz DBPSK PHY', phy_name=phy_name)
    #     phy.profile_outputs.MODEM_CF_CFOSR.override = 0
    #     phy.profile_outputs.MODEM_CF_DEC0.override = 1
    #     phy.profile_outputs.MODEM_CF_DEC1.override = 0
    #     phy.profile_outputs.MODEM_CF_DEC2.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_CODING.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_DIFFENCMODE.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_DSSSDOUBLE.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_DSSSLEN.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_DSSSSHIFTS.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_MAPFSK.override = 0
    #     phy.profile_outputs.MODEM_CTRL0_MODFORMAT.override = 3
    #     phy.profile_outputs.MODEM_CTRL0_SHAPING.override = 0
    #     phy.profile_outputs.MODEM_CTRL1_COMPMODE.override = 3
    #     phy.profile_outputs.MODEM_CTRL1_FREQOFFESTLIM.override = 0
    #     phy.profile_outputs.MODEM_CTRL1_FREQOFFESTPER.override = 0
    #     phy.profile_outputs.MODEM_CTRL1_PHASEDEMOD.override = 1
    #     phy.profile_outputs.MODEM_CTRL1_RESYNCPER.override = 2
    #     phy.profile_outputs.MODEM_CTRL1_SYNCBITS.override = 15
    #     phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 0
    #     phy.profile_outputs.MODEM_CTRL2_DEVWEIGHTDIS.override = 0
    #     phy.profile_outputs.MODEM_CTRL2_SQITHRESH.override = 0
    #     phy.profile_outputs.MODEM_CTRL3_TSAMPDEL.override = 2
    #     phy.profile_outputs.MODEM_CTRL3_TSAMPLIM.override = 0
    #     phy.profile_outputs.MODEM_CTRL3_TSAMPMODE.override = 0
    #     phy.profile_outputs.MODEM_CTRL4_ADCSATLEVEL.override = 1
    #     phy.profile_outputs.MODEM_CTRL4_DEVOFFCOMP.override = 0
    #     phy.profile_outputs.MODEM_CTRL4_ISICOMP.override = 0
    #     phy.profile_outputs.MODEM_CTRL4_OFFSETPHASEMASKING.override = 1
    #     phy.profile_outputs.MODEM_CTRL5_BRCALAVG.override = 0
    #     phy.profile_outputs.MODEM_CTRL5_BRCALEN.override = 0
    #     phy.profile_outputs.MODEM_CTRL5_TDEDGE.override = 1
    #     phy.profile_outputs.MODEM_CTRL5_TREDGE.override = 1
    #     phy.profile_outputs.MODEM_MODINDEX_FREQGAINE.override = 2
    #     phy.profile_outputs.MODEM_MODINDEX_FREQGAINM.override = 5
    #     phy.profile_outputs.MODEM_MODINDEX_MODINDEXE.override = 25
    #     phy.profile_outputs.MODEM_MODINDEX_MODINDEXM.override = 16
    #     phy.profile_outputs.MODEM_PRE_BASE.override = 2
    #     phy.profile_outputs.MODEM_PRE_BASEBITS.override = 1
    #     phy.profile_outputs.MODEM_PRE_PREERRORS.override = 1
    #     phy.profile_outputs.MODEM_PRE_TXBASES.override = 64
    #     phy.profile_outputs.MODEM_RXBR_RXBRDEN.override = 5
    #     phy.profile_outputs.MODEM_RXBR_RXBRINT.override = 2
    #     phy.profile_outputs.MODEM_RXBR_RXBRNUM.override = 2
    #     phy.profile_outputs.MODEM_SHAPING0_COEFF0.override = 4
    #     phy.profile_outputs.MODEM_SHAPING0_COEFF1.override = 10
    #     phy.profile_outputs.MODEM_SHAPING0_COEFF2.override = 19
    #     phy.profile_outputs.MODEM_SHAPING0_COEFF3.override = 34
    #     phy.profile_outputs.MODEM_SHAPING1_COEFF4.override = 50
    #     phy.profile_outputs.MODEM_SHAPING1_COEFF5.override = 65
    #     phy.profile_outputs.MODEM_SHAPING1_COEFF6.override = 74
    #     phy.profile_outputs.MODEM_SHAPING1_COEFF7.override = 79
    #     phy.profile_outputs.MODEM_SHAPING2_COEFF8.override = 0
    #     phy.profile_outputs.MODEM_SYNC0_SYNC0.override =  long(2391391958)
    #     phy.profile_outputs.MODEM_SYNC1_SYNC1.override = long(0)
    #     phy.profile_outputs.MODEM_TIMING_ADDTIMSEQ.override = 0
    #     phy.profile_outputs.MODEM_TIMING_FDM0THRESH.override = 0
    #     phy.profile_outputs.MODEM_TIMING_OFFSUBDEN.override = 0
    #     phy.profile_outputs.MODEM_TIMING_OFFSUBNUM.override = 0
    #     phy.profile_outputs.MODEM_TIMING_TIMINGBASES.override = 8
    #     phy.profile_outputs.MODEM_TIMING_TIMTHRESH.override = 75
    #     phy.profile_outputs.MODEM_TXBR_TXBRDEN.override = 105
    #     phy.profile_outputs.MODEM_TXBR_TXBRNUM.override = 252
    #     phy.profile_outputs.MODEM_DSSS0_DSSS0.override = long(0)
    #     phy.profile_outputs.MODEM_AFCADJLIM_AFCADJLIM.override = 0
    #     phy.profile_outputs.MODEM_AFC_AFCAVGPER.override = 0
    #     phy.profile_outputs.MODEM_AFC_AFCDEL.override = 0
    #     phy.profile_outputs.MODEM_AFC_AFCRXCLR.override = 0
    #     phy.profile_outputs.MODEM_AFC_AFCRXMODE.override = 0
    #     phy.profile_outputs.MODEM_AFC_AFCSCALEE.override = 0
    #     phy.profile_outputs.MODEM_AFC_AFCSCALEM.override = 0
    #     phy.profile_outputs.RAC_IFADCCTRL_VLDOCLKGEN.override = 2
    #     phy.profile_outputs.RAC_IFADCCTRL_REGENCLKDELAY.override = 2
    #     phy.profile_outputs.RAC_IFADCCTRL_INPUTSCALE.override = 1
    #     phy.profile_outputs.RAC_IFADCCTRL_VCM.override = 2
    #     phy.profile_outputs.RAC_IFADCCTRL_VLDOSERIES.override = 2
    #     phy.profile_outputs.RAC_IFADCCTRL_VLDOSERIESCURR.override = 2
    #     phy.profile_outputs.SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX.override = 245771
    #     phy.profile_outputs.RAC_IFPGACTRL_VLDO.override = 0
    #     phy.profile_outputs.RAC_IFPGACTRL_CASCBIAS.override = 0
    #     phy.profile_outputs.RAC_IFPGACTRL_TRIMVCASLDO.override = 0
    #     phy.profile_outputs.RAC_IFPGACTRL_TRIMVCM.override = 0
    #     phy.profile_outputs.RAC_IFPGACTRL_TRIMVREGMIN.override = 0
    #     phy.profile_outputs.SYNTH_CHSP_CHSP.override = 40960
    #     phy.profile_outputs.SYNTH_DIVCTRL_LODIVFREQCTRL.override = 3
    #     phy.profile_outputs.SYNTH_FREQ_FREQ.override =  long(35553280)
    #     phy.profile_outputs.AGC_CTRL0_PWRTARGET.override = 4
    #     phy.profile_outputs.AGC_CTRL2_ADCRSTSTARTUP.override = 0
    #     phy.profile_outputs.AGC_CTRL2_HYST.override = 0
    #     phy.profile_outputs.AGC_CTRL2_CFLOOPDEL.override = 40
    #     phy.profile_outputs.AGC_GAINRANGE_MAXGAIN.override = 62
    #     phy.profile_outputs.AGC_GAINRANGE_MINGAIN.override = 112
    #     phy.profile_outputs.AGC_GAININDEX_MININDEXDEGEN.override = 0
    #     phy.profile_outputs.AGC_GAININDEX_MININDEXPGA.override = 0
    #     phy.profile_outputs.AGC_GAININDEX_NUMINDEXATTEN.override = 18
    #     phy.profile_outputs.AGC_GAININDEX_NUMINDEXSLICES.override = 6
    #     phy.profile_outputs.AGC_GAININDEX_NUMINDEXDEGEN.override = 3
    #     phy.profile_outputs.AGC_GAININDEX_NUMINDEXPGA.override = 12
    #     phy.profile_outputs.AGC_MININDEX_INDEXMINPGA.override = 27
    #     phy.profile_outputs.AGC_MININDEX_INDEXMINDEGEN.override = 24
    #     phy.profile_outputs.AGC_MININDEX_INDEXMINSLICES.override = 18
    #     phy.profile_outputs.AGC_MININDEX_INDEXMINATTEN.override = 0
    #     phy.profile_outputs.AGC_MANGAIN_MANGAINLNAATTEN.override = 0
    #     phy.profile_outputs.AGC_MANGAIN_MANGAINLNASLICES.override = 0
    #     phy.profile_outputs.AGC_MANGAIN_MANGAINLNASLICESREG.override = 0
    #     phy.profile_outputs.RAC_LNAMIXCTRL1_TRIMTRSWGATEV.override = 3
    #     phy.profile_outputs.RAC_LNAMIXCTRL1_TRIMVCASLDO.override = 0
    #     phy.profile_outputs.RAC_LNAMIXCTRL1_TRIMVREGMIN.override = 0
    #     phy.profile_outputs.RAC_VCOCTRL_VCODETAMPLITUDE.override = 10
    #     phy.profile_outputs.RAC_VCOCTRL_VCOAREGCURR.override = 0
    #     phy.profile_outputs.RAC_VCOCTRL_VCOCREGCURR.override = 0
    #     phy.profile_outputs.RAC_VCOCTRL_VCODIVCURR.override = 10
    #     phy.profile_outputs.SYNTH_CTRL_DITHERDSMOUTPUT.override = 1
    #     phy.profile_outputs.SYNTH_CTRL_DITHERDAC.override = 1
    #
    # # PHY derived from the above PHY
    # def _PHY_Sigfox_868MHz_DBPSK_2Mbps(self, model):
    #     phy = self._makePhy(model, model.profiles.Base, readable_name='100bps 868MHz DBPSK PHY', phy_name=phy_name)
    #     # Inputs
    #     phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
    #     phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
    #     phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.DBPSK
    #     phy.profile_inputs.bitrate.value = 2000000
    #     phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.NONE
    #     phy.profile_inputs.shaping_filter_param.value = 0.5
    #     phy.profile_inputs.dsss_chipping_code.value = long(0)
    #     phy.profile_inputs.dsss_len.value = 0
    #     phy.profile_inputs.dsss_spreading_factor.value = 0
    #     phy.profile_inputs.syncword_length.value = 32
    #     phy.profile_inputs.preamble_pattern.value = 1
    #     phy.profile_inputs.preamble_pattern_len.value = 2
    #     phy.profile_inputs.syncword_0.value = long(0x6B7D9171)
    #     phy.profile_inputs.syncword_1.value = long(0x0)
    #     phy.profile_inputs.agc_power_target.value = 4
    #     phy.profile_inputs.errors_in_timing_window.value = 1
    #     phy.profile_inputs.rx_xtal_error_ppm.value = 20
    #     phy.profile_inputs.tx_xtal_error_ppm.value = 20
    #     phy.profile_inputs.xtal_frequency_hz.value = 38400000
    #     phy.profile_inputs.base_frequency_hz.value =  long(868000000)
    #     phy.profile_inputs.preamble_length.value = 128
    #     phy.profile_inputs.channel_spacing_hz.value = 0
    #     phy.profile_inputs.deviation.value = 0
    #     phy.profile_inputs.agc_hysteresis.value = 0
    #     phy.profile_inputs.agc_settling_delay.value = 40
    #     phy.profile_inputs.baudrate_tol_ppm.value = 0
    #     phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
    #     phy.profile_inputs.sqi_threshold.value = 0
    #     phy.profile_inputs.timing_sample_threshold.value = 0
    #     phy.profile_inputs.number_of_timing_windows.value = 0
    #     phy.profile_inputs.timing_detection_threshold.value = 75

    def PHY_Sigfox_868MHz_DBPSK_100bps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='100bps 868MHz DBPSK PHY', phy_name=phy_name)
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.DBPSK
        phy.profile_inputs.bitrate.value = 100
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Root_Raised_Cosine
        phy.profile_inputs.shaping_filter_param.value = 0.0
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.syncword_length.value = 13
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.syncword_0.value = long(0x6B)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.agc_power_target.value = 4
        phy.profile_inputs.errors_in_timing_window.value = 1
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.base_frequency_hz.value =  long(868000000)
        phy.profile_inputs.preamble_length.value = 16
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.deviation.value = 0
        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_settling_delay.value = 40
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.sqi_threshold.value = 0
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.number_of_timing_windows.value = 0
        phy.profile_inputs.timing_detection_threshold.value = 75
        #phy.profile_outputs.SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX.override = 0x3C00F
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_3000KHz
        PHY_COMMON_FRAME_154(phy, model)

        return phy

    def PHY_Sigfox_915MHz_DBPSK_600bps(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='100bps 868MHz DBPSK PHY', phy_name=phy_name)
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.DBPSK
        phy.profile_inputs.bitrate.value = 600
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Root_Raised_Cosine
        phy.profile_inputs.shaping_filter_param.value = 0.0
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.syncword_length.value = 13
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.syncword_0.value = long(0x6B)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.agc_power_target.value = 4
        phy.profile_inputs.errors_in_timing_window.value = 1
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.base_frequency_hz.value =  long(915000000)
        phy.profile_inputs.preamble_length.value = 16
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.deviation.value = 0
        phy.profile_inputs.agc_hysteresis.value = 0
        phy.profile_inputs.agc_settling_delay.value = 40
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        phy.profile_inputs.sqi_threshold.value = 0
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.number_of_timing_windows.value = 0
        phy.profile_inputs.timing_detection_threshold.value = 75
        #phy.profile_outputs.SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX.override = 0x3C00F
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_3000KHz
        PHY_COMMON_FRAME_154(phy, model)

        return phy
        
    def PHY_Sigfox_915MHz_GFSK_600bps_WIP(self, model, phy_name=None):

        '''
        Sigfox downlink (Rx):
        This PHY still needs to be improved to meet -126dBm 10% PER spec, current measurement is arount -124dBm.

        The Rx configuration
        has a special feature. As the base station measures the frequency on the uplink
        packet and adjusts the downlink frequency to it, there is no need to use any
        AFC algorithm in the receiver and the narrowest possible filter bandwidth can
        be configured. That said the specification is still quite challenging at -126
        dBm at 10% PER.
        '''
        phy = self._makePhy(model, model.profiles.Base, readable_name='Sigfox 915MHz GFSK 600bps PHY', phy_name=phy_name)

        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.base_frequency_hz.value =  long(915000000)
        phy.profile_inputs.channel_spacing_hz.value = 0             # spacing is too narrow to use the spacing parameter
        phy.profile_inputs.baudrate_tol_ppm.value = 0

        phy.profile_inputs.bandwidth_hz.value = 2700
        # phy.profile_outputs.MODEM_MODINDEX_FREQGAINM.override = 5
        # phy.profile_outputs.MODEM_MODINDEX_FREQGAINE.override = 4
        phy.profile_inputs.if_frequency_hz.value = 400000

        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0

        # 2GFSK
        # 600 bps data rate
        # Deviation = 800 Hz
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.bitrate.value = 600
        phy.profile_inputs.deviation.value = 800

        # BT=1
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 1.0

        phy.profile_inputs.symbols_in_timing_window.value = 16
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.agc_speed.value = model.vars.agc_speed.var_enum.FAST


        # 5 bytes of 0x55 preamble
        phy.profile_inputs.preamble_pattern.value = 1               #
        phy.profile_inputs.preamble_pattern_len.value = 2           #
        phy.profile_inputs.preamble_length.value = 40               #

        # 13 bit sync word of b1001 0001 0011 1
        phy.profile_inputs.syncword_0.value = long(0x1227)               #
        phy.profile_inputs.syncword_1.value = long(0x0)                  #
        phy.profile_inputs.syncword_length.value = 13               #

        # 16 byte payload

        # payload byte wise LSB first
        phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST


        #Packet Inputs
        phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.FIXED_LENGTH
        phy.profile_inputs.fixed_length_size.value = 16
        phy.profile_inputs.payload_white_en.value = False
        phy.profile_inputs.payload_crc_en.value = True

        # Variable length includes header
        phy.profile_inputs.header_en.value = False
        phy.profile_inputs.header_size.value = 0
        phy.profile_inputs.header_calc_crc.value = False
        phy.profile_inputs.header_white_en.value = False

        # CRC Inputs
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CRC_16                     # CRC polynomial:   CRC_16
        phy.profile_inputs.crc_seed.value =  long(0x00000000)                                             # CRC seed:         0x0000
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST    # CRC byte order:   MSB first
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST      # CRC bit order:    LSB first
        phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.LSB_FIRST    # CRC engine feed:  LSB first
        phy.profile_inputs.crc_pad_input.value = False
        phy.profile_inputs.crc_invert.value = False

        # Misc frame configuration
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0


        phy.profile_inputs.timing_detection_threshold.value = 4
        phy.profile_outputs.MODEM_CF_DEC1GAIN.override = 1
        phy.profile_inputs.timing_sample_threshold.value = 2
        phy.profile_inputs.freq_offset_hz.value = 0


        phy.profile_inputs.errors_in_timing_window.value = 1
        phy.profile_inputs.agc_settling_delay.value = 24
        #phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON

        # Do not include 2FSK optimizations for proprietary PHYs
        phy.profile_inputs.in_2fsk_opt_scope.value = False

        return phy