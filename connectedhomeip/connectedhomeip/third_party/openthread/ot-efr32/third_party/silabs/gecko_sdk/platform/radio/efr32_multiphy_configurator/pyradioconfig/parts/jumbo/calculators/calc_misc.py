"""Core CALC_Misc Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat
from pyradioconfig.parts.common.calculators.calc_misc import CALC_Misc
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

from py_2_and_3_compatibility import *

class CALC_Misc_jumbo(CALC_Misc):

    def calc_misc(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        part_family = model.part_family.lower()

        self._reg_write(model.vars.MODEM_CTRL0_FRAMEDETDEL, 0)

        self._reg_write(model.vars.MODEM_CTRL1_SYNCERRORS, 0)
        self._reg_write(model.vars.MODEM_CTRL1_SYNC1INV, 0)

        self._reg_write(model.vars.MODEM_CTRL2_BRDIVB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_BRDIVA, 0)
        if part_family == 'dumbo':              # Temp until it's time to change all of the reference cfg files
            self._reg_write(model.vars.MODEM_CTRL2_RATESELMODE, 0)

        self._reg_write(model.vars.MODEM_CTRL4_ADCSATDENS, 0)
        self._reg_write(model.vars.MODEM_CTRL4_ADCSATLEVEL, 6)
        self._reg_write(model.vars.MODEM_CTRL4_SOFTDSSSMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTRST, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTAVG, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTDEB, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTGAIN, 0)

        self._reg_write(model.vars.MODEM_PRE_DSSSPRE, 0)

        self._reg_write(model.vars.MODEM_TIMING_TSAGCDEL, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQSYNC, 0)
        self._reg_write(model.vars.MODEM_TIMING_FASTRESYNC, 0)

        self._reg_write(model.vars.MODEM_AFC_AFCTXMODE,       0)

        self._reg_write(model.vars.SEQ_MISC_SQBMODETX, 0)

        self._reg_write(model.vars.MODEM_DSATHD0_SPIKETHD, 0x64)
        self._reg_write(model.vars.MODEM_DSATHD0_UNMODTHD, 4)
        self._reg_write(model.vars.MODEM_DSATHD0_FDEVMINTHD, 12)
        self._reg_write(model.vars.MODEM_DSATHD0_FDEVMAXTHD, 0x78)

        self._reg_write(model.vars.MODEM_DSATHD1_POWABSTHD, 0x1388)
        self._reg_write(model.vars.MODEM_DSATHD1_POWRELTHD, 0)
        self._reg_write(model.vars.MODEM_DSATHD1_DSARSTCNT, 2)
        self._reg_write(model.vars.MODEM_DSATHD1_RSSIJMPTHD, 6)
        self._reg_write(model.vars.MODEM_DSATHD1_FREQLATDLY, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_PWRFLTBYP, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_AMPFLTBYP, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_PWRDETDIS, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_FREQSCALE, 0)

        #self._reg_write(model.vars.MODEM_DSACTRL_DSAMODE, 0)

        self._reg_write(model.vars.MODEM_DSACTRL_ARRTOLERTHD0, 2)
        self._reg_write(model.vars.MODEM_DSACTRL_ARRTOLERTHD1, 4)
        self._reg_write(model.vars.MODEM_DSACTRL_SCHPRD, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_FREQAVGSYM, 1)
        self._reg_write(model.vars.MODEM_DSACTRL_DSARSTON, 1)
        self._reg_write(model.vars.MODEM_DSACTRL_TIMEST, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_AGCDEBOUNDLY, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_LOWDUTY, 0)

        self._reg_write(model.vars.MODEM_VTTRACK_FREQTRACKMODE, 0)
        self._reg_write(model.vars.MODEM_VTTRACK_TIMTRACKTHD, 2)
        self._reg_write(model.vars.MODEM_VTTRACK_TIMEACQUTHD, 0xEE)
        self._reg_write(model.vars.MODEM_VTTRACK_TIMCHK, 0)
        self._reg_write(model.vars.MODEM_VTTRACK_TIMEOUTMODE, 1)
        self._reg_write(model.vars.MODEM_VTTRACK_TIMGEAR, 0)
        self._reg_write(model.vars.MODEM_VTTRACK_FREQBIAS, 0)

        self._reg_write(model.vars.MODEM_VTCORRCFG1_CORRSHFTLEN, 0x20)
        self._reg_write(model.vars.MODEM_VTCORRCFG1_VTFRQLIM, 0xC0)
        self._reg_write(model.vars.MODEM_VTCORRCFG0_EXPECTPATT, long(0x556B7))
        self._reg_write(model.vars.MODEM_VTCORRCFG0_EXPSYNCLEN, 8)
        self._reg_write(model.vars.MODEM_VTCORRCFG0_BUFFHEAD, 2)

        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VTDEMODEN, 0)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_HARDDECISION, 0)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VITERBIKSI1, 0x40)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VITERBIKSI2, 0x30)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VITERBIKSI3, 0x20)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_SYNTHAFC, 0)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_CORRCYCLE, 0)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_CORRSTPSIZE, 0)


        self._reg_write(model.vars.MODEM_CTRL5_DSSSCTD, 0)
        self._reg_write(model.vars.MODEM_CTRL5_BBSS, 0)
        self._reg_write(model.vars.MODEM_CTRL5_POEPER, 0)
        self._reg_write(model.vars.MODEM_CTRL5_FOEPREAVG, 0)
        self._reg_write(model.vars.MODEM_CTRL5_LINCORR, 0)
        self._reg_write(model.vars.MODEM_CTRL5_PRSDEBUG, 0)
        self._reg_write(model.vars.MODEM_CTRL5_RESYNCLIMIT, 0)

        self._reg_write(model.vars.MODEM_CTRL6_TDREW, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PREBASES, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT0, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT1, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT2, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT3, 0)
        self._reg_write(model.vars.MODEM_CTRL6_ARW, 0)
        self._reg_write(model.vars.MODEM_CTRL6_TIMTHRESHGAIN, 0)
        self._reg_write(model.vars.MODEM_CTRL6_CPLXCORREN, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXBRCALCDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL6_DEMODRESTARTALL, 0)

        self._reg_write(model.vars.MODEM_DSACTRL_AGCBAUDEN, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_RESTORE, 0)

        self._reg_write(model.vars.MODEM_CTRL0_DUALCORROPTDIS, 0)

        self._reg_write(model.vars.AGC_GAINSTEPLIM_EN2XFASTSTEPDOWN, 0)

        self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG0, 0)
        self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG1, 0)
        self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG2, 0)
        self._reg_write(model.vars.MODEM_INTAFC_FOEPREAVG3, 0)

        self._reg_write(model.vars.MODEM_CGCLKSTOP_FORCEOFF, 0)

        self._reg_write(model.vars.AGC_RSSISTEPTHR_DEMODRESTARTPER, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_DEMODRESTARTTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_POSSTEPTHR     , 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_NEGSTEPTHR     , 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_STEPPER        , 0)

        self._reg_write(model.vars.SYNTH_CTRL_DEMMODE        , 1)


        self._reg_write(model.vars.SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACRX, 3)
        self._reg_write(model.vars.SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDACTX, 3)
        self._reg_write(model.vars.SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTRX, 1)
        self._reg_write(model.vars.SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMINPUTTX, 1)
        self._reg_write(model.vars.SEQ_SYNTH_CTRL_DITHER_SETTINGS_DITHERDSMOUTPUTRX, 7)


    #
    # Enable the sequencer code to dynamically control BBSS threshold
    # if the coherent demod is being used with OQPSK modulation.
    # It really should be turned on for all demodulation types, but
    # we're limiting it to only OQPSK because that's the only one that's
    # been tested and the only one currently using the coherent demod
    #
    def calc_dynamic_bbss_sw_en(self, model):
        if model.vars.MODEM_CTRL1_PHASEDEMOD.value == 3 and model.vars.mod_format_actual.value == "OQPSK":
            model.vars.SEQ_MISC_DYNAMIC_BBSS_SW_EN.value = 1
        else:
            model.vars.SEQ_MISC_DYNAMIC_BBSS_SW_EN.value = 0

    def calc_dynamic_bbss_values(self, model):

        #Read in model variables
        dynamic_bbss_en = model.vars.SEQ_MISC_DYNAMIC_BBSS_SW_EN.value

        #If we are not using dynamic BBSS then the configuration fields are do-not-care
        if dynamic_bbss_en:
            do_not_care = False
        else:
            do_not_care = True

        #For now we don't have a calculation (only a fixed lookup for 802154 coh PHYs)
        #These values are from https://jira.silabs.com/browse/MCUW_RADIO_CFG-1619
        self._reg_write(model.vars.SEQ_BBSS_GROUP_1_IIR_SHIFT, 2, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_1_BBSS_HYSTERESIS, 0, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_1_BBSS_LEVEL_LOW, 98, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_1_BBS_LEVEL_1_OFFSET, 2, do_not_care=do_not_care) #Typo in reg model
        self._reg_write(model.vars.SEQ_BBSS_GROUP_2_BBSS_LEVEL_2_OFFSET, 9, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_2_SYNC_THRESH_MID, 200, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_2_BBSS_LEVEL_1_VALUE, 5, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_2_SYNC_THRESH_LOW, 120, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_VALUE, 7, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_3_BBSS_LEVEL_3_OFFSET, 8, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_3_BBSS_TIM_DETECTED_TIM_THRESH, 220, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_3_BBSS_LEVEL_2_VALUE, 6, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_4_DEFAULT_TIM_THRESH, 120, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_4_BBSS_LEVEL_5_VALUE, 9, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_OFFSET, 6, do_not_care=do_not_care)
        self._reg_write(model.vars.SEQ_BBSS_GROUP_4_BBSS_LEVEL_4_VALUE, 8, do_not_care=do_not_care)

    def calc_etsi_cat1_compatibility(self, model):
        model.vars.etsi_cat1_compatible.value = model.vars.etsi_cat1_compatible.var_enum.Normal

    def calc_ook_rssi_offset(self,model):
        #OOK RSSI offset measured experimentally for Jumbo
        model.vars.ook_rssi_offset.value = 12

    def calc_dynamic_slicer_sw_en(self, model):
        mod_format = model.vars.modulation_type.value

        #Always enable the dynamic slicer for OOK
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            model.vars.dynamic_slicer_enabled.value = True
        else:
            model.vars.dynamic_slicer_enabled.value = False

    def calc_dynamic_slicer_values(self, model):
        slicer_level = model.vars.ook_slicer_level.value
        baudrate_tol = model.vars.baudrate_tol_ppm.value

        # Check if this feature is enabled
        if model.vars.dynamic_slicer_enabled.value == True:
            if baudrate_tol <= 1000:
                # If the requested baudrate tolerance is less than or equal to 0.1%, then use a very conservative approach
                start_slicing_level = slicer_level  # Start with the programmed OOK slicing level
                slicing_safety_factor = 10  # Use a large safety factor to make sure we achieve the best senstivity
            elif baudrate_tol < 10000:
                # If the requested baudrate tolerance is high
                # Always start with the larger of the programmed level or 3(more robust)
                start_slicing_level = max(3,slicer_level)
                slicing_safety_factor = 6  # Use a smaller safety factor so that we more aggressively transition to larger slicing levels
            else:
                start_slicing_level = max(3,slicer_level)
                slicing_safety_factor = 3

            # Get the calculated sensitivity value and apply a safety factor
            start_slicer_threshold = int(round(model.vars.sensitivity.value)) + slicing_safety_factor + 6  # The 6dB here is the expected degradation in sensitivity from slicer 5

            # Set the emperically determined RSSI offset
            # We use this instead of the programmed RSSISHIFT, because the customer might have a FEM or other uses cases for that value
            rssi_offset_est = model.vars.ook_rssi_offset.value

            if model.vars.dynamic_slicer_level_values._value_forced == None:
                #Use 5 slicing levels
                model.vars.dynamic_slicer_level_values.value = [start_slicing_level, 5, 7, 10, 15]

                # Calculate the slicing thresholds
                if model.vars.dynamic_slicer_threshold_values._value_forced == None:
                    threshold_spacing = 2.5 #This is the spacing in dB per slicer code
                    model.vars.dynamic_slicer_threshold_values.value = [0]*(len(model.vars.dynamic_slicer_level_values.value)-1)
                    for i,slicer in enumerate(model.vars.dynamic_slicer_level_values.value[1:]):
                        model.vars.dynamic_slicer_threshold_values.value[i] = int(py2round(start_slicer_threshold + rssi_offset_est + (slicer-model.vars.dynamic_slicer_level_values.value[1])*threshold_spacing))

            # Finally, set the register field to let the firmware know this feature is enabled
            model.vars.SEQ_MISC_DYNAMIC_SLICER_SW_EN.value = 1
        else:
            model.vars.SEQ_MISC_DYNAMIC_SLICER_SW_EN.value = 0

    def calc_in_2fsk_opt_scope(self, model):

        # This function determines if a PHY is in 2FSK optimization scope

        modulation_type = model.vars.modulation_type.value
        viterbi_en_reg = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        antdivmode = model.vars.antdivmode.value
        part_family = model.part_family
        profile = model.profile.name
        dsss_len = model.vars.dsss_len.value
        etsi_cat1_compatible = model.vars.etsi_cat1_compatible.value

        is_2fsk = modulation_type == model.vars.modulation_type.var_enum.FSK2
        viterbi_en = viterbi_en_reg == 1
        antdiv_off = antdivmode == model.vars.antdivmode.var_enum.DISABLE
        in_base_profile = profile.lower() == 'base'
        not_spread = dsss_len == 0
        is_etsi = (etsi_cat1_compatible == model.vars.etsi_cat1_compatible.var_enum.Band_169) or \
                  (etsi_cat1_compatible == model.vars.etsi_cat1_compatible.var_enum.Band_868)

        part_list = ['dumbo', 'jumbo', 'nerio', 'nixi']
        in_part_list = part_family.lower() in part_list

        in_scope = is_2fsk and in_part_list and in_base_profile and not viterbi_en and antdiv_off and not_spread and not is_etsi

        model.vars.in_2fsk_opt_scope.value = in_scope

    def calc_protocol_id(self, model):
        profile_name = model.profile.name.lower()

        if "wisun" in profile_name:
            # This applies to all Wi-SUN Profiles
            protocol_id = model.vars.protocol_id.var_enum.WiSUN
        else:
            protocol_id = model.vars.protocol_id.var_enum.Custom

        model.vars.protocol_id.value = protocol_id

    def calc_stack_info(self, model):
        # Get parameters common to all protocols
        protocol_id = model.vars.protocol_id.value
        fec_tx_enable = model.vars.fec_tx_enable.value

        # Other parameters are stored in this list
        specific_parameters = []

        #For Wi-SUN OFDM, fec_tx_enable is DISABLED and we use phyModeId of MCS0
        #For Wi-SUN FSK, we use phyModeId for FEC ON/OFF depending on fec_tx_enable

        if protocol_id == model.vars.protocol_id.var_enum.WiSUN:
            wisun_operating_class = getattr(model.profile.inputs, "wisun_operating_class", None)

            if fec_tx_enable == model.vars.fec_tx_enable.var_enum.ENABLED:
                phy_id = model.vars.wisun_phy_mode_id.value[1]
            else:
                phy_id = model.vars.wisun_phy_mode_id.value[0]

            if wisun_operating_class is not None:
                # Get WiSUN specific parameters
                version = 0  # FAN1.0 version
                wisun_operating_class = model.vars.wisun_operating_class.value
                wisun_reg_domain = int(model.vars.wisun_reg_domain.value)
                # Fill in the specific parameters
                specific_parameters.extend([version, wisun_operating_class, wisun_reg_domain])
        else:
            phy_id = 0

        model.vars.stack_info.value = [int(protocol_id), int(phy_id)]
        model.vars.stack_info.value.extend(specific_parameters)
