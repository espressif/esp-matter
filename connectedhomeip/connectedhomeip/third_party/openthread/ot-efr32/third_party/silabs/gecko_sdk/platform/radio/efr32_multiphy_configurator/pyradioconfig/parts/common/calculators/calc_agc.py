"""Core AGC Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

from py_2_and_3_compatibility import *

class CALC_AGC(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Input variables

        # Internal variables
        self._addModelActual(model,   'agcperiod'             ,float, ModelVariableFormat.DECIMAL)
        self._addModelActual(model,   'rssi_period_sym'            ,int,  ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'agc_settling_delay'    ,  int, ModelVariableFormat.DECIMAL, desc='Delay between two AGC gain adjustments in AGC clock cycles')
        self._addModelVariable(model, 'agc_clock_cycle'       ,float, ModelVariableFormat.DECIMAL)

        var = self._addModelVariable(model, 'agc_speed', Enum, ModelVariableFormat.DECIMAL, 'AGC Speed')
        member_data = [
            ['NORMAL'   , 0, 'Recommended default setting'],
            ['FAST'     , 1, 'Aggressive AGC setting'],
            ['SLOW'     , 2, 'Slow AGC setting'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AgcMode',
            'List of supported AGC Speed Modes',
            member_data)

        self._addModelVariable(model, 'lna_slices_delay'      ,float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'fast_loop_delay'       ,float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model, 'if_pga_delay'          ,float, ModelVariableFormat.DECIMAL)

        # Output variables
        self._addModelRegister(model, 'AGC.CTRL0.ADCRESETDURATION'     , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL0.PWRTARGET'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL0.MODE'                 , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.CTRL1.AGCPERIOD'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL1.RSSIPERIOD'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL1.SUBPERIOD'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL1.SUBINT'               , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL1.SUBNUM'               , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL1.SUBDEN'               , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.CTRL2.ADCRSTSTARTUP'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL2.MAXPWRVAR'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL2.HYST'                 , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL2.FASTLOOPDEL'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.CTRL2.CFLOOPDEL'            , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.GAINSTEPLIM.CFLOOPSTEPMAX'  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAINSTEPLIM.FASTSTEPUP'     , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAINSTEPLIM.FASTSTEPDOWN'   , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.LOOPDEL.LNASLICESDEL'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.LOOPDEL.IFPGADEL'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.LOOPDEL.PKDWAIT'            , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.GAINRANGE.MAXGAIN'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAINRANGE.MINGAIN'          , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.GAININDEX.MININDEXDEGEN'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAININDEX.MININDEXPGA'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAININDEX.NUMINDEXATTEN'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAININDEX.NUMINDEXSLICES'   , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAININDEX.NUMINDEXDEGEN'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.GAININDEX.NUMINDEXPGA'      , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'AGC.MININDEX.INDEXMINPGA'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MININDEX.INDEXMINDEGEN'     , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MININDEX.INDEXMINSLICES'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MININDEX.INDEXMINATTEN'     , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'AGC.MANGAIN.MANGAININDEX'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MANGAIN.MANGAININDEXEN'     , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MANGAIN.MANGAINLNAATTEN'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MANGAIN.MANGAINLNASLICES'   , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.MANGAIN.MANGAINLNASLICESREG', int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'AGC.IFPEAKDET.PKDTHRESH1', int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'AGC.IFPEAKDET.PKDTHRESH2', int, ModelVariableFormat.HEX )

        if model.part_family.lower() in ["jumbo", "nerio", "nixi"]:

            # MCUW_RADIO_CFG-673
            # following register only for Jumbo, Nerio and Nixi
            self._addModelRegister(model, 'AGC.RFPEAKDET.RFPKDSWITCHDEL', int, ModelVariableFormat.HEX)

            # MCUW_RADIO_CFG-656 : AGC_RFPEAKDET fields are not overridable
            self._addModelRegister(model, 'AGC.RFPEAKDET.RFPKDTHRESH2', int, ModelVariableFormat.HEX)
            self._addModelRegister(model, 'AGC.RFPEAKDET.RFPKDTHRESH1', int, ModelVariableFormat.HEX)

    def calc_agc_reg(self, model):
        """given frequency band select between two AGC settings

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        subgig_band = model.vars.subgig_band.value

        if subgig_band:
            self._reg_write(model.vars.AGC_GAINRANGE_MAXGAIN, 62)
            self._reg_write(model.vars.AGC_GAINRANGE_MINGAIN, 112)

            self._reg_write(model.vars.AGC_GAININDEX_MININDEXDEGEN,      0)
            self._reg_write(model.vars.AGC_GAININDEX_MININDEXPGA,        0)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXATTEN,     18)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXSLICES,     6)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXDEGEN,      3)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXPGA,       12)

            self._reg_write(model.vars.AGC_MININDEX_INDEXMINPGA,        27)
            self._reg_write(model.vars.AGC_MININDEX_INDEXMINDEGEN,      24)
            self._reg_write(model.vars.AGC_MININDEX_INDEXMINSLICES,     18)
            self._reg_write(model.vars.AGC_MININDEX_INDEXMINATTEN,       0)

            # MCUW_RADIO_CFG-1
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNAATTEN,      0)
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNASLICES,     0)
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNASLICESREG,  0)

        else:
            self._reg_write(model.vars.AGC_GAINRANGE_MAXGAIN,           60)
            self._reg_write(model.vars.AGC_GAINRANGE_MINGAIN,          122)

            self._reg_write(model.vars.AGC_GAININDEX_MININDEXDEGEN,      0)
            self._reg_write(model.vars.AGC_GAININDEX_MININDEXPGA,        0)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXATTEN,     12)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXSLICES,     6)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXDEGEN,      3)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXPGA,       12)

            self._reg_write(model.vars.AGC_MININDEX_INDEXMINPGA,        21)
            self._reg_write(model.vars.AGC_MININDEX_INDEXMINDEGEN,      18)
            self._reg_write(model.vars.AGC_MININDEX_INDEXMINSLICES,     12)
            self._reg_write(model.vars.AGC_MININDEX_INDEXMINATTEN,       0)

            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNAATTEN,     12)
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNASLICES,     0)
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNASLICESREG,  0)


    def calc_agc_misc(self, model):
        """Sets the agc variables to some default state until we figure out how to set them

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.AGC_CTRL0_ADCRESETDURATION,    0)

        self._reg_write(model.vars.AGC_CTRL1_SUBPERIOD,           0)
        self._reg_write(model.vars.AGC_CTRL1_SUBINT,              0)
        self._reg_write(model.vars.AGC_CTRL1_SUBNUM,              0)
        self._reg_write(model.vars.AGC_CTRL1_SUBDEN,              0)
    
        self._reg_write(model.vars.AGC_CTRL2_ADCRSTSTARTUP,       1)
        self._reg_write(model.vars.AGC_CTRL2_MAXPWRVAR,           0)

        self._reg_write(model.vars.AGC_MANGAIN_MANGAININDEX,      0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAININDEXEN,    0)

        self._reg_write(model.vars.AGC_IFPEAKDET_PKDTHRESH1,      2)
        self._reg_write(model.vars.AGC_IFPEAKDET_PKDTHRESH2,      8)



    def calc_agc_faststepup_reg(self, model):
        """set FASTSTEPUP based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        mod_format = model.vars.modulation_type.value

        # in OOK mode use max step size of 5
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            step = 0
        else:
            step = 2

        self._reg_write(model.vars.AGC_GAINSTEPLIM_FASTSTEPUP, step)

    def calc_agc_cfloopstepmax_reg(self, model):
        """set CFLOOPSTEPMAX based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mod_format = model.vars.modulation_type.value
        bw = model.vars.bandwidth_actual.value
        baudrate = model.vars.baudrate.value

        # in OOK mode use max step size of 5 unless we are in a relatively
        # narrowband OOK case. In narrowband case for dumbo only disable
        # slow loop as it is causing an error floor.
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            if bw < 5*baudrate:
                step = 0
            else:
                step = 5
        elif mod_format == model.vars.modulation_type.var_enum.FSK4:
            step = 1
        else:
            step = 8

        self._reg_write(model.vars.AGC_GAINSTEPLIM_CFLOOPSTEPMAX, step)

    def calc_agc_mode_reg(self, model):
        """set MODE based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mod_format = model.vars.modulation_type.value

        # in OOK mode use LOCKPREDET lock gain once preamble is detected
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            mode = 1
        else:
            mode = 0

        self._reg_write(model.vars.AGC_CTRL0_MODE, mode)

    def calc_agc_speed_val(self, model):
        """set agc_speed based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mod_format = model.vars.modulation_type.value

        # in OOK mode use FAST AGC
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            agc_speed = model.vars.agc_speed.var_enum.FAST
        else:
            agc_speed = model.vars.agc_speed.var_enum.NORMAL

        model.vars.agc_speed.value = agc_speed


    def calc_pwrtarget_val(self, model):
        """set agc_power_target based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mod_format = model.vars.modulation_type.value

        if mod_format == model.vars.modulation_type.var_enum.OOK:
            model.vars.agc_power_target.value = 0
        else:
            model.vars.agc_power_target.value = -2

    def calc_pwrtarget_reg(self, model):
        """set PWTARGET register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # level in dBm (can be negative)
        level = model.vars.agc_power_target.value

        self._reg_write(model.vars.AGC_CTRL0_PWRTARGET, level, allow_neg=True)

    def calc_rssiperiod_val(self, model):
        """calculate RSSIPERIOD as fixed number 3 for now

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        model.vars.rssi_period.value = 3

    def calc_rssiperiod_reg(self, model):
        """program RSSIPERIOD register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        period = model.vars.rssi_period.value

        if period > 15:
            period = 15

        # Minimum RSSIPERIOD field value is 1 corresponding to a period of 2
        # See https://jira.silabs.com/browse/MCUW_RADIO_CFG-851
        if period < 1:
            period = 1

        self._reg_write(model.vars.AGC_CTRL1_RSSIPERIOD, period)

    def calc_hyst_val(self, model):
        """calculate HYST as a fixed number 3 for now

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.agc_hysteresis.value = 3

    def calc_hyst_reg(self, model):
        """program HYST register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        hyst = model.vars.agc_hysteresis.value

        if hyst > 15:
            hyst = 15

        self._reg_write(model.vars.AGC_CTRL2_HYST, hyst)

    def calc_agcperiod_value(self, model):
        """calculate AGCPERIOD as fixed number 1 for now

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.agc_period.value = 1

    def calc_agcperiod_reg(self, model):
        """program AGCPERIOD register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = model.vars.agc_period.value
        self._reg_write(model.vars.AGC_CTRL1_AGCPERIOD, val)

    def calc_agcperiod_actual(self, model):
        """calculate actual AGCPERIOD from register value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = 2**(model.vars.AGC_CTRL1_AGCPERIOD.value * 1.0)

        model.vars.agcperiod_actual.value = val

    def calc_agc_settling_delay(self, model):
        """calculate AGC settling delay which is basically the group delay of decimation and
        channel filters through the datapath plus AGC period at the AGC clock rate

        DEC0 filter(12) -> DEC0 -> DEC1 filter(3) -> DEC1 -> Channel filter (10)
        we pad the resulting delay by scaling it with 1.45 to account for variations

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        agcperiod = model.vars.agcperiod_actual.value
        osr = model.vars.oversampling_rate_actual.value

        cfloopdel = int(1.45*py2round(((12.0+3.0)/dec0+3.0)/dec1+10.0 + agcperiod * osr * dec2))

        if cfloopdel > 63:
            cfloopdel = 63

        model.vars.agc_settling_delay.value = cfloopdel

    def calc_cfloopdel_reg(self, model):
        """program CFLOOPDEL based on calculated value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        cfloopdel = model.vars.agc_settling_delay.value

        self._reg_write(model.vars.AGC_CTRL2_CFLOOPDEL, cfloopdel)

    def calc_agc_clock_cycle(self, model):
        """programmable AGC delays are set in terms of AGC clock cycles so calculate that here

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value * 1.0
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value

        model.vars.agc_clock_cycle.value = (dec0 * dec1) / fxo

    def calc_agc_delays(self, model):
        """function to set delays based on agc_speed input. For now keeping all delays the same
        delays represent analog block delays to different AGC components

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_speed = model.vars.agc_speed.value

        if agc_speed == model.vars.agc_speed.var_enum.FAST.value:

            model.vars.lna_slices_delay.value = 400e-9
            model.vars.fast_loop_delay.value = 250e-9
            model.vars.if_pga_delay.value = 300e-9

        elif agc_speed == model.vars.agc_speed.var_enum.SLOW.value:

            model.vars.lna_slices_delay.value = 400e-9
            model.vars.fast_loop_delay.value = 250e-9
            model.vars.if_pga_delay.value = 300e-9

        else:
            model.vars.lna_slices_delay.value = 400e-9
            model.vars.fast_loop_delay.value = 250e-9
            model.vars.if_pga_delay.value = 300e-9

    def calc_faststepdown_reg(self, model):
        """choose FASTSTEPDOWN value based on agc_speed

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_speed = model.vars.agc_speed.value

        if agc_speed == model.vars.agc_speed.var_enum.FAST.value:
            step = 5
        elif agc_speed == model.vars.agc_speed.var_enum.SLOW.value:
            step = 3  # TODO: come up with a value for this one
        else:
            step = 3

        self._reg_write(model.vars.AGC_GAINSTEPLIM_FASTSTEPDOWN, step)


    def calc_pkdwait_reg(self, model):
        """calculate PKDWAIT based on agc_speed

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_speed = model.vars.agc_speed.value

        if agc_speed == model.vars.agc_speed.var_enum.FAST.value:
            wait = 3
        elif agc_speed == model.vars.agc_speed.var_enum.SLOW.value:
            wait = 2  # TODO: come up with a value for this one
        else:
            wait = 2

        self._reg_write(model.vars.AGC_LOOPDEL_PKDWAIT, wait)


    def calc_fastloopdel_reg(self, model):
        """calculate FASTLOOPDEL based on corresponding delay

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_clock_cycle = model.vars.agc_clock_cycle.value
        fast_loop_delay = model.vars.fast_loop_delay.value

        # TODO: add missing -1 to equation once verified
        delay = int(math.ceil(fast_loop_delay / agc_clock_cycle))

        if delay < 0:
            delay = 0
        elif delay > 15:
            delay = 15

        self._reg_write(model.vars.AGC_CTRL2_FASTLOOPDEL, delay)

    def calc_lnaslicesdel_reg(self, model):
        """calculate LNASLICESDEL based on corresponding delay

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_clock_cycle = model.vars.agc_clock_cycle.value
        lna_slices_delay = model.vars.lna_slices_delay.value

        # TODO: add missing -1 to equation once verified
        delay = int(math.ceil(lna_slices_delay / agc_clock_cycle))

        if delay < 0:
            delay = 0
        elif delay > 15:
            delay = 15

        self._reg_write(model.vars.AGC_LOOPDEL_LNASLICESDEL, delay)

    def calc_ifpgadel_reg(self, model):
        """calculate IFPGADEL based on corresponding delay

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_clock_cycle = model.vars.agc_clock_cycle.value
        if_pga_delay = model.vars.if_pga_delay.value

        # TODO: add missing -1 to equation once verified
        delay = int(math.ceil(if_pga_delay / agc_clock_cycle))

        if delay < 0:
            delay = 0
        elif delay > 15:
            delay = 15

        self._reg_write(model.vars.AGC_LOOPDEL_IFPGADEL, delay)

    def calc_rssi_period_actual(self, model):

        #Read in the RSSI period register and take 2 to that power to compute the symbols in the period
        rssi_period_reg = model.vars.AGC_CTRL1_RSSIPERIOD.value
        rssi_period_sym_actual = 2**rssi_period_reg

        model.vars.rssi_period_sym_actual.value = rssi_period_sym_actual

