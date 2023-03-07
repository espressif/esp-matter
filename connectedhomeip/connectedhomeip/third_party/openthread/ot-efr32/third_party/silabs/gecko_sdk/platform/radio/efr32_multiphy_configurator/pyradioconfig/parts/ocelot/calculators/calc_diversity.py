from pyradioconfig.parts.common.calculators.calc_diversity import CALC_Diversity
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

class CALC_Diversity_Ocelot(CALC_Diversity):

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
            ['DISABLE', 0, 'Antenna 0 used'],
            ['ANTENNA1', 1, 'Antenna 1 is used'],
            ['ANTSELFIRST', 2, 'Select-First algorithm'],
            ['ANTSELCORR', 3, 'Select-Best algorithm based on correlation'],
            ['ANTSELRSSI', 4, 'Select-Best algorithm based on RSSI value'],
            ['PHDEMODANTDIV', 5, 'Select PHASE Demod ANT-DIV algorithm'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AntDivModeEnum',
            'List of supported antenna diversity mode',
            member_data)

        # Bools not allowed as advanced inputs due to GUI constraint. Using enum instead
        var = self._addModelVariable(model, 'antdivrepeatdis', Enum, ModelVariableFormat.DECIMAL,
                                     'Repeated measurement of first antenna when Select-Best algorithm is used')
        member_data = [
            ['REPEATFIRST', 0, 'Enable repeated measurement of first antenna'],
            ['NOREPEATFIRST', 1, 'Disable repeated measurement of first antenna'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AntDivRepeatDisModeEnum',
            'Enable or disable repeated measurement of first antenna',
            member_data)

        var = self._addModelVariable(model, 'skip2ant', Enum, ModelVariableFormat.DECIMAL,
                                     'Skip 2nd antenna check with phase demod antenna diversity')
        member_data = [
            ['SKIP2ANT', 0, 'Enable repeated measurement of first antenna'],
            ['NOSKIP2ANT', 1, 'Disable repeated measurement of first antenna'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'Skip2AntModeEnum',
            'Enable or disable Skip 2nd antenna check with phase demod antenna diversity',
            member_data)

        # Output software variables for RAIL to consume
        self._addModelVariable(model, 'div_antdivmode', int, ModelVariableFormat.DECIMAL, 'Antenna diversity mode')
        self._addModelVariable(model, 'div_antdivrepeatdis', int, ModelVariableFormat.DECIMAL,
                               'Repeated measurement of first antenna when Select-Best algorithm is used')

        # Calculation variable for reset period
        if model.part_family.lower() in ["jumbo", "nerio", "nixi"]:
            self._addModelVariable(model, 'div_demod_reset_period_hemi_usec', int, ModelVariableFormat.DECIMAL,
                                   'Sequencer FW issues a reset to demod at this interval. Used in antenna diversity.')

        self._addModelVariable(model, 'antdiv_adprethresh_scale', float, ModelVariableFormat.DECIMAL,
                                     'Set adpretrehsh as a scaled value of timthresh ')

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

        # # unless set otherwise by advanced inputs
        antdivmode = model.vars.antdivmode.value
        antdivrepeatdis = model.vars.antdivrepeatdis.value

        model.vars.div_antdivmode.value = int(antdivmode)
        model.vars.div_antdivrepeatdis.value = int(antdivrepeatdis)

    def calc_adprethresh_scale(self, model):
        model.vars.antdiv_adprethresh_scale.value = 0.0

    def calc_phdmodantdiv_antdivrepeatdis(self, model):
        model.vars.skip2ant.value = model.vars.skip2ant.var_enum.NOSKIP2ANT

    def calc_phdmodantdiv_antdivrepeatdis_reg(self, model):
        antdivmode = model.vars.antdivmode.value
        skip2ant = model.vars.skip2ant.value

        reg = 1 # : skip 2nd antenna check by default

        # : For phase demod antdiv mode, set skip2ant register
        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            if skip2ant == model.vars.skip2ant.var_enum.NOSKIP2ANT:
                reg = 0

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIP2ANT, reg)
        else:
            self._reg_do_not_care(model.vars.MODEM_PHDMODANTDIV_SKIP2ANT)

    def calc_phdemodantdiv_skiprssithd_reg(self, model):
        """
        If the first antenna RSSI value is higher than the SKIPRSSITHD, then skip the second antenna check.

        Args:
            model:

        Returns: None

        """

        demod_select = model.vars.demod_select.value
        antdivmode = model.vars.antdivmode.value

        # : TODO update calculation since this value depends on the sensitivity
        if demod_select == model.vars.demod_select.var_enum.BCR:
            reg_val = 240
        elif demod_select == model.vars.demod_select.var_enum.TRECS_SLICER \
                or demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI:
            reg_val = 251

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIPRSSITHD, reg_val)
        else:
            self._reg_do_not_care(model.vars.MODEM_PHDMODANTDIV_SKIPRSSITHD)

    def calc_phdemodantdiv_skipcorrthd_reg(self, model):
        """
        If the first antenna correlation value is higher than the SKIPCORRTHD, then skip the second antenna check.

        Args:
            model:

        Returns:

        """
        antdivmode = model.vars.antdivmode.value

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIPCORRTHD, 100)
        else:
            self._reg_do_not_care(model.vars.MODEM_PHDMODANTDIV_SKIPCORRTHD)

    def calc_phdemodantdiv_decision_regions(self, model):
        """
        Phase demod antenna diversity determines the best antenna using both correlation and RSSI measurements.
        Decision to prioritize RSSI vs. Correlation is dependent on the absolute value of RSSI or correlation, which
        is known as the decision regions.

        Args:
            model:

        Returns:

        """
        antdivmode = model.vars.antdivmode.value
        demod_select = model.vars.demod_select.value

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            # : if rssicorr{x} = 1, then for x region, use RSSI based decision to select best antenna
            rssicorr0 = 1
            rssicorr1 = 1
            rssicorr2 = 1
            rssicorr3 = 1

            # : decision thresholds
            rssianddivthd = 20
            corranddivthd = 100

        if antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR0, rssicorr0)
            self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR1, rssicorr1)
            self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR2, rssicorr2)
            self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR3, rssicorr3)
            self._reg_write(model.vars.MODEM_PHANTDECSION_RSSIANDDIVTHD, rssianddivthd)
            self._reg_write(model.vars.MODEM_PHANTDECSION_CORRANDDIVTHD, corranddivthd)
        else:
            self._reg_do_not_care(model.vars.MODEM_PHANTDECSION_RSSICORR0)
            self._reg_do_not_care(model.vars.MODEM_PHANTDECSION_RSSICORR1)
            self._reg_do_not_care(model.vars.MODEM_PHANTDECSION_RSSICORR2)
            self._reg_do_not_care(model.vars.MODEM_PHANTDECSION_RSSICORR3)
            self._reg_do_not_care(model.vars.MODEM_PHANTDECSION_RSSIANDDIVTHD)
            self._reg_do_not_care(model.vars.MODEM_PHANTDECSION_CORRANDDIVTHD)

    def calc_phdemodantdiv_antenna_wait(self, model):
        #Number of clock cycles to wait before switching to another antenna. Base clock is symbol rate.
        #This field does matter even when antdiv is disabled

        antwait = 20 # : Recommended value by He Gou

        self._reg_write(model.vars.MODEM_PHDMODANTDIV_ANTWAIT, antwait)

    def calc_phdmodctrl_rssifltbyp(self, model):
        # do not bypass rssi filter. otherwise, sensitivity degradation if signal power difference is small.
        #This field does matter even when antdiv is disabled
        rssifltbyp = 0

        self._reg_write(model.vars.MODEM_PHDMODCTRL_RSSIFLTBYP, rssifltbyp)

    def calc_diversity_adprethresh(self, model):
        timthresh = model.vars.MODEM_TIMING_TIMTHRESH.value
        adprethresh_scale = model.vars.antdiv_adprethresh_scale.value

        # : Per David Rault - Scale adprethresh by a factor of timing threshold
        adprethresh = adprethresh_scale * timthresh
        adprethresh_int = int(round(adprethresh))

        # : if adprethresh register is non-zero, enable preamble threshold for antenna diversity
        if adprethresh_int > 0:
            self._reg_write(model.vars.MODEM_ANTDIVCTRL_ENADPRETHRESH, 1)
        else:
            self._reg_write(model.vars.MODEM_ANTDIVCTRL_ENADPRETHRESH, 0)

        self._reg_write(model.vars.MODEM_ANTDIVCTRL_ADPRETHRESH, adprethresh_int)