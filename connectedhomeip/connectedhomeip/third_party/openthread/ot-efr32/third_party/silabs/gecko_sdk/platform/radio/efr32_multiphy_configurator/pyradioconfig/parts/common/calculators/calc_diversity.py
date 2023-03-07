"""Core Antenna Diversity Calculator Package

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
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

class CALC_Diversity(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 0
        self._minor = 1
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Input variables

        # Internal variables
        # Must exposing the forced selection of a antenna 0 only as DISABLE enum
        var = self._addModelVariable(model, 'antdivmode', Enum, ModelVariableFormat.DECIMAL, 'Antenna diversity mode')
        member_data = [
            ['DISABLE', 0, 'Disabled. Only Antenna 0 used'],
            ['ANTSELFIRST'  , 2, 'Select-First algorithm'],
            ['ANTSELRSSI'   , 4, 'Select-Best algorithm based on RSSI value'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AntDivModeEnum',
            'List of supported antenna diversity mode',
            member_data)

        # Bools not allowed as advanced inputs due to GUI constraint. Using enum instead
        var = self._addModelVariable(model, 'antdivrepeatdis', Enum, ModelVariableFormat.DECIMAL, 'Repeated measurement of first antenna when Select-Best algorithm is used')
        member_data = [
            ['REPEATFIRST' , 0, 'Enable repeated measurement of first antenna'],
            ['NOREPEATFIRST', 1, 'Disable repeated measurement of first antenna'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AntDivRepeatDisModeEnum',
            'Enable or disable repeated measurement of first antenna',
            member_data)

        # Output software variables for RAIL to consume
        self._addModelVariable(model, 'div_antdivmode', int, ModelVariableFormat.DECIMAL, 'Antenna diversity mode')
        self._addModelVariable(model, 'div_antdivrepeatdis', int, ModelVariableFormat.DECIMAL, 'Repeated measurement of first antenna when Select-Best algorithm is used')

        # Calculation variable for reset period
        if model.part_family.lower() in ["jumbo", "nerio", "nixi"]:
            self._addModelVariable(model, 'div_demod_reset_period_hemi_usec', int, ModelVariableFormat.DECIMAL, 'Sequencer FW issues a reset to demod at this interval. Used in antenna diversity.')

    def calc_diversity_values(self, model):
        """apply inputs to antenna diversity output software variables for RAIL to consume
        TODO: consider preamble length--should be long enough to allow the diversity search algorithm to make a proper antenna selection
        Not handled here: MODEM_ROUTEPEN: ANT0PEN, ANT1PEN to enable these pins, and MODEM_ROUTELOC1 to route to GPIO

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # default value 0 ANTENNA0 Antenna 0 (ANT0=1, ANT1=0) is used. This is not exposed as one of the enums, so use 0 here.
        # default ANTDIVREPEATDIS 0
        model.vars.antdivmode.value = model.vars.antdivmode.var_enum.DISABLE
        model.vars.antdivrepeatdis.value = model.vars.antdivrepeatdis.var_enum.REPEATFIRST

        # unless set otherwise by advanced inputs
        antdivmode = model.vars.antdivmode.value
        antdivrepeatdis = model.vars.antdivrepeatdis.value

        subgig_band = model.vars.subgig_band.value

        flag_using_Viterbi_demod = False

        # Viterbi does not exist in Dumbo:
        if model.part_family.lower() in ["jumbo", "nerio", "nixi", "panther", "lynx", "leopard"]:
            flag_using_Viterbi_demod = (model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value == 1)

        if (not flag_using_Viterbi_demod):
            # not using coherent demod, OK to enable diversity
            model.vars.div_antdivmode.value = int(antdivmode)
            model.vars.div_antdivrepeatdis.value = int(antdivrepeatdis)
        else:
            # Viterbi demod, may not enable diversity

            model.vars.div_antdivmode.value = int(model.vars.antdivmode.var_enum.DISABLE)
            model.vars.div_antdivrepeatdis.value = int(model.vars.antdivrepeatdis.var_enum.REPEATFIRST)

            if (antdivmode != int(model.vars.antdivmode.var_enum.DISABLE)) and flag_using_Viterbi_demod:
                raise CalculationException("Cannot enable antenna diversity when using Viterbi demodulation.")

    def calc_diversity_div_demod_reset_period_hemi_usec_value(self, model):
        """
        Calculate period/interval for sequencer FW to issue a reset to demod.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        antdivmode = model.vars.antdivmode.value
        preamblebits = model.vars.preamble_length.value * 1.0
        baudrate = model.vars.baudrate.value * 1.0

        if model.part_family.lower() in ["jumbo", "nerio", "nixi"]:

            if antdivmode == model.vars.antdivmode.var_enum.DISABLE:
                # disable if not in diversity mode
                model.vars.div_demod_reset_period_hemi_usec.value = 0
            elif (antdivmode == model.vars.antdivmode.var_enum.ANTSELFIRST) or (antdivmode == model.vars.antdivmode.var_enum.ANTSELRSSI):
                # Units are half uSec. Use 1000x preamble time as the period
                model.vars.div_demod_reset_period_hemi_usec.value = int(preamblebits / baudrate * 1000 * 1e6 * 2)

    def calc_diversity_div_demod_reset_period_hemi_usec_reg(self, model):
        """
        Calculate period/interval for sequencer FW to issue a reset to demod.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        antdivmode = model.vars.antdivmode.value
        if model.part_family.lower() in ["jumbo", "nerio", "nixi"]:
            div_demod_reset_period_hemi_usec = model.vars.div_demod_reset_period_hemi_usec.value


            if (antdivmode == model.vars.antdivmode.var_enum.ANTSELFIRST) or (antdivmode == model.vars.antdivmode.var_enum.ANTSELRSSI):
                self._reg_write(model.vars.SEQ_MISC_PERIODIC_DEMOD_RST_EN, 1)
                pass
            elif (antdivmode == model.vars.antdivmode.var_enum.DISABLE):
                self._reg_write(model.vars.SEQ_MISC_PERIODIC_DEMOD_RST_EN, 0)
                pass


            self._reg_write(model.vars.SEQ_PERIODICDEMODRSTPERIOD_PERIOD, div_demod_reset_period_hemi_usec)

