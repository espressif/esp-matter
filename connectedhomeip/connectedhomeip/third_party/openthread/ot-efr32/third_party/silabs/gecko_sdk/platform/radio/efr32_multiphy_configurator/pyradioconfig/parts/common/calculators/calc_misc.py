"""Core CALC_Misc Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_Misc(ICalculator):

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
        self._addModelRegister(model, 'MODEM.CTRL0.FRAMEDETDEL'        , int, ModelVariableFormat.HEX )


        self._addModelRegister(model, 'MODEM.CTRL1.SYNC1INV'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.SYNCERRORS'         , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'MODEM.CTRL2.BRDIVA'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL2.BRDIVB'             , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'MODEM.CTRL4.ADCSATDENS'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL4.ADCSATLEVEL'        , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'MODEM.CTRL5.BRCALMODE'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL5.DETDEL'             , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'MODEM.PRE.DSSSPRE'              , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'MODEM.TIMING.FASTRESYNC'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.TIMSEQSYNC'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.TSAGCDEL'          , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'MODEM.AFC.AFCTXMODE'            , int, ModelVariableFormat.HEX )

        self._addModelVariable(model, 'in_2fsk_opt_scope', bool, ModelVariableFormat.DECIMAL)

        self._addModelVariable(model, 'protocol_id', Enum, ModelVariableFormat.DECIMAL, 'Protocol ID')
        model.vars.protocol_id.var_enum = CreateModelVariableEnum(
            enum_name='ProtocolIDEnum',
            enum_desc='List of supported protocols',
            member_data=[
                ['Custom', 0, 'Custom stack'],
                ['EmberPHY', 1, 'EFR32 EmberPHY (Zigbee/Thread)'],
                ['Thread', 2, 'Thread on RAIL'],
                ['BLE', 3, 'BLE'],
                ['Connect', 4, 'Connect on RAIL'],
                ['Zigbee', 5, 'Zigbee on rail'],
                ['ZWave', 6, 'ZWave on RAIL'],
                ['WiSUN', 7, 'WiSUN on RAIL']
            ])

        self._addModelVariable(model, 'stack_info', int, ModelVariableFormat.DECIMAL,
                               desc='information dedicated to stack', is_array=True)


    def calc_misc(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.MODEM_CTRL0_FRAMEDETDEL, 0)

        self._reg_write(model.vars.MODEM_CTRL1_SYNCERRORS, 0)
        self._reg_write(model.vars.MODEM_CTRL1_SYNC1INV, 0)

        self._reg_write(model.vars.MODEM_CTRL2_BRDIVB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_BRDIVA, 0)

        self._reg_write(model.vars.MODEM_CTRL4_ADCSATDENS, 0)
        self._reg_write(model.vars.MODEM_CTRL4_ADCSATLEVEL, 6)

        self._reg_write(model.vars.MODEM_CTRL5_DETDEL, 0)
        self._reg_write(model.vars.MODEM_CTRL5_BRCALMODE, 0)

        self._reg_write(model.vars.MODEM_PRE_DSSSPRE, 0)

        self._reg_write(model.vars.MODEM_TIMING_TSAGCDEL, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQSYNC, 0)
        self._reg_write(model.vars.MODEM_TIMING_FASTRESYNC, 0)

        self._reg_write(model.vars.MODEM_AFC_AFCTXMODE,       0)

        self._reg_write(model.vars.SEQ_MISC_SQBMODETX, 0)


    #Disable dynamic slicer on Dumbo by default
    def calc_dynamic_slicer_sw_en(self, model):
        model.vars.dynamic_slicer_enabled.value = False

    #If someone enables dynamic slicing then use a simple schedule by default (can be overridden)
    #This is a very rough guess and may not work with some PHY configurations, hence disabling the feature by default
    def calc_dynamic_slicer_values(self, model):

        # Check if this feature is enabled
        if model.vars.dynamic_slicer_enabled.value == True:
            slicer_level = model.vars.ook_slicer_level.value

            #Calculate the slicing schedule
            model.vars.dynamic_slicer_level_values.value=[slicer_level,5,10]
            model.vars.dynamic_slicer_threshold_values.value = [-65,-35]

            # Finally, set the register field to let the firmware know this feature is enabled
            model.vars.SEQ_MISC_DYNAMIC_SLICER_SW_EN.value = 1
        else:
            model.vars.SEQ_MISC_DYNAMIC_SLICER_SW_EN.value = 0

    def calc_in_2fsk_opt_scope(self, model):

        # This function determines if a PHY is in 2FSK optimization scope

        modulation_type = model.vars.modulation_type.value
        antdivmode = model.vars.antdivmode.value
        part_family = model.part_family
        profile = model.profile.name
        dsss_len = model.vars.dsss_len.value

        is_2fsk = modulation_type == model.vars.modulation_type.var_enum.FSK2
        antdiv_off = antdivmode == model.vars.antdivmode.var_enum.DISABLE
        in_base_profile = profile.lower() == 'base'
        not_spread = dsss_len == 0

        part_list = ['dumbo', 'jumbo', 'nerio', 'nixi']
        in_part_list = part_family.lower() in part_list

        in_scope = is_2fsk and in_part_list and in_base_profile and antdiv_off and not_spread

        model.vars.in_2fsk_opt_scope.value = in_scope

    def calc_protocol_id(self, model):
        model.vars.protocol_id.value = model.vars.protocol_id.var_enum.Custom

    def calc_stack_info(self, model):
        protocol_id = model.vars.protocol_id.value
        model.vars.stack_info.value = [int(protocol_id), 0]
