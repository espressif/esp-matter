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
from pyradioconfig.parts.common.calculators.calc_agc import CALC_AGC

class CALC_AGC_jumbo(CALC_AGC):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0


    def calc_agc_clock_cycle(self, model):
       return


    def calc_fastloopdel_reg(self, model):
        """calculate FASTLOOPDEL based on corresponding delay

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value
        fast_loop_delay = model.vars.fast_loop_delay.value

        # TODO: add missing -1 to equation once verified
        delay = int(math.ceil(fast_loop_delay * fxo))

        if delay < 0:
            delay = 0
        elif delay > 31:
            delay = 31

        self._reg_write(model.vars.AGC_CTRL2_FASTLOOPDEL, delay)

    def calc_lnaslicesdel_reg(self, model):
        """calculate LNASLICESDEL based on corresponding delay

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        fxo = model.vars.xtal_frequency.value
        lna_slices_delay = model.vars.lna_slices_delay.value

        # TODO: add missing -1 to equation once verified
        delay = int(math.ceil(lna_slices_delay * fxo))

        if delay < 0:
            delay = 0
        elif delay > 31:
            delay = 31

        self._reg_write(model.vars.AGC_LOOPDEL_LNASLICESDEL, delay)

    def calc_ifpgadel_reg(self, model):
        """calculate IFPGADEL based on corresponding delay

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        fxo = model.vars.xtal_frequency.value
        if_pga_delay = model.vars.if_pga_delay.value

        # TODO: add missing -1 to equation once verified
        delay = int(math.ceil(if_pga_delay * fxo))

        if delay < 0:
            delay = 0
        elif delay > 31:
            delay = 31

        self._reg_write(model.vars.AGC_LOOPDEL_IFPGADEL, delay)

    def calc_pkdwait_reg(self, model):
        """calculate PKDWAIT based on agc_speed

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value
        f_if = float(model.vars.if_frequency_hz.value)
        freq_dev_hz = model.vars.deviation.value

        rfpkd_en = model.vars.RAC_SGLNAMIXCTRL1_ENRFPKD.value
        rfpkd_switch_del = model.vars.AGC_RFPEAKDET_RFPKDSWITCHDEL.value

        # per guideline in internal document
        wait = fxo / (4 * (f_if - freq_dev_hz))

        #MCUW_RADIO_CFG-673
        if rfpkd_en == 1:
            if rfpkd_switch_del == 0:
                wait = 120 + wait
            elif rfpkd_switch_del == 1:
                wait = 200 + wait

        if wait < 0:
            wait = 0
        elif wait > 1023:
            wait = 1023

        self._reg_write(model.vars.AGC_LOOPDEL_PKDWAIT, int(wait))


    def calc_slowdecaycnt_reg(self, model):

        mod_format = model.vars.modulation_type.value
        baudrate_hz = model.vars.baudrate.value
        encoding = model.vars.symbol_encoding.value

        if encoding == model.vars.symbol_encoding.var_enum.DSSS or \
           encoding == model.vars.symbol_encoding.var_enum.Manchester:

            reg = 2

        elif mod_format == model.vars.modulation_type.var_enum.FSK2 or \
             mod_format == model.vars.modulation_type.var_enum.FSK4:

            if baudrate_hz > 500e3:
                reg = 2
            else:
                reg = 1

        else:
            reg = 8

        self._reg_write(model.vars.AGC_GAINSTEPLIM_SLOWDECAYCNT, reg)


    def calc_agc_misc(self, model):
        """Sets the agc variables to some default state until we figure out how to set them

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        self._reg_write(model.vars.AGC_CTRL0_ADCRESETDURATION,    0)

        self._reg_write(model.vars.AGC_CTRL1_SUBPERIOD, 			0)
        self._reg_write(model.vars.AGC_CTRL1_SUBINT,			    0)
        self._reg_write(model.vars.AGC_CTRL1_SUBNUM,              0)
        self._reg_write(model.vars.AGC_CTRL1_SUBDEN, 				0)

        self._reg_write(model.vars.AGC_CTRL2_ADCRSTSTARTUP, 		1)
        self._reg_write(model.vars.AGC_CTRL2_MAXPWRVAR,      		0)

        self._reg_write(model.vars.AGC_MANGAIN_MANGAININDEX, 		0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAININDEXEN,	0)

        #MCUW_RADIO_CFG-673
        self._reg_write(model.vars.AGC_RFPEAKDET_RFPKDSWITCHDEL, 0)

        etsi = model.vars.etsi_cat1_compatible.value
        if etsi == model.vars.etsi_cat1_compatible.var_enum.Band_169:
            self._reg_write(model.vars.AGC_IFPEAKDET_PKDTHRESH2,	12)
            self._reg_write(model.vars.AGC_IFPEAKDET_PKDTHRESH1,	6)
        else:
            self._reg_write(model.vars.AGC_IFPEAKDET_PKDTHRESH2,	8)
            self._reg_write(model.vars.AGC_IFPEAKDET_PKDTHRESH1,	2)


        self._reg_write(model.vars.AGC_RFPEAKDET_RFPKDTHRESH1, 5)
        self._reg_write(model.vars.AGC_RFPEAKDET_RFPKDTHRESH2, 13)


    def calc_agc_reg(self, model):
        """given frequency band select between two AGC settings

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        subgig_band = model.vars.subgig_band.value

        self._reg_write(model.vars.AGC_GAININDEX_MININDEXDEGEN, 	    0)
        self._reg_write(model.vars.AGC_GAININDEX_MININDEXPGA, 		0)
        self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXSLICES, 	    6)
        self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXDEGEN, 	    3)
        self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXPGA, 		12)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNASLICES,  0)
        self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNASLICESREG, 0)

        if subgig_band:
            self._reg_write(model.vars.AGC_GAINRANGE_MAXGAIN, 			62)
            self._reg_write(model.vars.AGC_GAINRANGE_MINGAIN, 			112)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXATTEN, 	    18)
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNAATTEN,  0)

        else:
            self._reg_write(model.vars.AGC_GAINRANGE_MAXGAIN, 			60)
            self._reg_write(model.vars.AGC_GAINRANGE_MINGAIN, 			122)
            self._reg_write(model.vars.AGC_GAININDEX_NUMINDEXATTEN, 	    12)
            self._reg_write(model.vars.AGC_MANGAIN_MANGAINLNAATTEN,  12)


    def calc_agc_scheme(self, model):

        etsi = model.vars.etsi_cat1_compatible.value

        if etsi == model.vars.etsi_cat1_compatible.var_enum.Band_169:
            model.vars.agc_scheme.value = model.vars.agc_scheme.var_enum.SCHEME_4
        else:
            model.vars.agc_scheme.value = model.vars.agc_scheme.var_enum.SCHEME_1

    def calc_agc_index_min_atten_reg(self, model):

        scheme = model.vars.agc_scheme.value
        numindexslices = model.vars.AGC_GAININDEX_NUMINDEXSLICES.value

        if scheme == model.vars.agc_scheme.var_enum.SCHEME_1 or \
           scheme == model.vars.agc_scheme.var_enum.SCHEME_4:
            indexminatten = 0
        else:
            indexminatten = numindexslices

        self._reg_write(model.vars.AGC_MININDEX_INDEXMINATTEN, indexminatten)


    def calc_agc_index_min_slices_reg(self, model):

        scheme = model.vars.agc_scheme.value
        numindexatten= model.vars.AGC_GAININDEX_NUMINDEXATTEN.value

        if scheme == model.vars.agc_scheme.var_enum.SCHEME_2 or \
           scheme == model.vars.agc_scheme.var_enum.SCHEME_3:
            indexminslices = 0
        else:
            indexminslices = numindexatten

        self._reg_write(model.vars.AGC_MININDEX_INDEXMINSLICES, indexminslices)


    def calc_agc_index_min_degen_reg(self, model):

        scheme = model.vars.agc_scheme.value
        numindexslices = model.vars.AGC_GAININDEX_NUMINDEXSLICES.value
        numindexatten = model.vars.AGC_GAININDEX_NUMINDEXATTEN.value
        numindexpga = model.vars.AGC_GAININDEX_NUMINDEXPGA.value

        # not following convention of reading in input variables at top of function because
        # this function combined with calc_agc_index_min_pga_reg() prevent each other from
        # running in that case.
        if scheme == model.vars.agc_scheme.var_enum.SCHEME_1:
            indexminslices = model.vars.AGC_MININDEX_INDEXMINSLICES.value
            indexmindegen = numindexslices + indexminslices

        elif scheme == model.vars.agc_scheme.var_enum.SCHEME_2:
            indexminatten = model.vars.AGC_MININDEX_INDEXMINATTEN.value
            indexmindegen = numindexatten + indexminatten

        else:
            indexminpga = model.vars.AGC_MININDEX_INDEXMINPGA.value
            indexmindegen = numindexpga + indexminpga

        self._reg_write(model.vars.AGC_MININDEX_INDEXMINDEGEN, indexmindegen)


    def calc_agc_index_min_pga_reg(self, model):

        scheme = model.vars.agc_scheme.value
        numindexslices = model.vars.AGC_GAININDEX_NUMINDEXSLICES.value
        numindexatten = model.vars.AGC_GAININDEX_NUMINDEXATTEN.value
        numindexdegen = model.vars.AGC_GAININDEX_NUMINDEXDEGEN.value

        # not following convention of reading in input variables at top of function because
        # this function combined with calc_agc_index_min_degen_reg() prevent each other from
        # running in that case.
        if scheme == model.vars.agc_scheme.var_enum.SCHEME_4:
            indexminslices = model.vars.AGC_MININDEX_INDEXMINSLICES.value
            indexminpga = numindexslices + indexminslices

        elif scheme == model.vars.agc_scheme.var_enum.SCHEME_3:
            indexminatten = model.vars.AGC_MININDEX_INDEXMINATTEN.value
            indexminpga = numindexatten + indexminatten

        else:
            indexmindegen = model.vars.AGC_MININDEX_INDEXMINDEGEN.value
            indexminpga = numindexdegen + indexmindegen

        self._reg_write(model.vars.AGC_MININDEX_INDEXMINPGA, indexminpga)


    def calc_faststepdown_reg(self, model):
        """choose FASTSTEPDOWN value based on agc_speed

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        agc_speed = model.vars.agc_speed.value
        etsi = model.vars.etsi_cat1_compatible.value

        if etsi == model.vars.etsi_cat1_compatible.var_enum.Band_169:
            step = 1
        else:
            if agc_speed == model.vars.agc_speed.var_enum.FAST.value:
                step = 5
            elif agc_speed == model.vars.agc_speed.var_enum.SLOW.value:
                step = 3  # TODO: come up with a value for this one
            else:
                step = 3

        self._reg_write(model.vars.AGC_GAINSTEPLIM_FASTSTEPDOWN, step)


    def calc_agc_faststepup_reg(self, model):
        """set FASTSTEPUP based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        mod_format = model.vars.modulation_type.value
        etsi = model.vars.etsi_cat1_compatible.value

        # in OOK mode use max step size of 5
        if etsi == model.vars.etsi_cat1_compatible.var_enum.Band_169:
            step = 1
        else:
            step = 2

        self._reg_write(model.vars.AGC_GAINSTEPLIM_FASTSTEPUP, step)


    def calc_agc_adcattenmode_code(self, model):

        etsi = model.vars.etsi_cat1_compatible.value

        if etsi == model.vars.etsi_cat1_compatible.var_enum.Band_169:
            self._reg_write(model.vars.AGC_GAINSTEPLIM_ADCATTENCODE, 1)
            self._reg_write(model.vars.AGC_GAINSTEPLIM_ADCATTENMODE, 1)
        else:
            self._reg_write(model.vars.AGC_GAINSTEPLIM_ADCATTENCODE, 0)
            self._reg_write(model.vars.AGC_GAINSTEPLIM_ADCATTENMODE, 0)


    def calc_agc_cfloopstepmax_reg(self, model):
        """set CFLOOPSTEPMAX based on modulation method

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        mod_format = model.vars.modulation_type.value

        if mod_format == model.vars.modulation_type.var_enum.OOK:
            # in OOK mode disable the channel filter loop
            step = 0
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

        # in OOK mode, lock once the syncword is detected (CFLOOP is disabled)
        if mod_format == model.vars.modulation_type.var_enum.OOK:
            mode = 2
        else:
            mode = 0

        self._reg_write(model.vars.AGC_CTRL0_MODE, mode)