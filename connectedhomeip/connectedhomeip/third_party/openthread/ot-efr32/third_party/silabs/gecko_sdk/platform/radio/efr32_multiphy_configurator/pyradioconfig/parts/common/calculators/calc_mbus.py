"""Core CALC_Ber Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

import inspect
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

from py_2_and_3_compatibility import *

class CALC_Mbus(ICalculator):

    """CALC_Mbus
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

        
    def buildVariables(self, model):

        self._add_mbus_mode_var(model)

        # Mbus Frame Format
        var = self._addModelVariable(model, 'mbus_frame_format', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported Mbus Frame Format Configurations')
        member_data = [
            ['NoFormat', 0, 'No frame formatting'],
            ['FrameA', 1, 'Mbus Format A '],
            ['FrameB', 2, 'Mbus Format B'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'MbusFrameFormatEnum',
            'List of supported Mbus frame formats',
            member_data)


        # Mbus Frame Format
        var = self._addModelVariable(model, 'mbus_symbol_encoding', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported Mbus Symbol Encoding Configurations')
        member_data = [
            ['NRZ', 0, 'NRZ'],
            ['Manchester', 1, 'Manchester'],
            ['MBUS_3OF6', 2, '3 of 6'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'MbusFrameFormatEnum',
            'List of supported Mbus frame formats',
            member_data)

    def _add_mbus_mode_var(self, model):
        var = self._addModelVariable(model, 'mbus_mode', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported Mbus Modes')
        member_data = [
            ['ModeC_M2O_100k', 0, 'Mode C Meter to Other, 100kbps'],
            ['ModeC_O2M_50k', 1, 'Mode C Other to Meter, 50kbps'],
            ['ModeF_2p4k', 2, 'Mode F, 2.4kbps'],
            ['ModeNg', 3, 'Mode Ng'],
            ['ModeN1a_4p8K', 4, 'Mode N1a, 4.8kbps'],
            ['ModeN1c_2p4K', 5, 'Mode N1c, 2.4kbps'],
            ['ModeR_4p8k', 6, 'Mode R, 2.8kbps'],
            ['ModeT_M2O_100k', 7, 'Mode T, Meter to Other, 100kbps'],
            ['ModeT_O2M_32p768k', 8, 'Mode T, Other to Meter, 100kbps'],
            ['ModeS_32p768k', 9, 'Mode S, 32.768kbps'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'MbusModeEnum',
            'List of supported Mbus modoes',
            member_data)

    def calc_mbus_syncwords(self, model):
        if model.profile.name.lower() == 'mbus':
            mode = model.vars.mbus_mode.value
            if   mode == model.vars.mbus_mode.var_enum.ModeC_M2O_100k or \
                 mode == model.vars.mbus_mode.var_enum.ModeC_O2M_50k:
                len = 26
                sync_a = long(0x03d54cd)
                sync_b = long(0x03d543d)
            elif mode == model.vars.mbus_mode.var_enum.ModeF_2p4k:
                len = 16
                sync_a = long(0xf68d)
                sync_b = long(0xf672)
            elif mode == model.vars.mbus_mode.var_enum.ModeNg or \
                 mode == model.vars.mbus_mode.var_enum.ModeN1a_4p8K or \
                 mode == model.vars.mbus_mode.var_enum.ModeN1c_2p4K:
                len = 16
                sync_a = long(0xf68d)
                sync_b = long(0xf672)
                # Note that the spec calls for a different 32-bit syncword for Ng mode,
                # but it's really just the same 2gfsk pattern as the other modes
                # expressed in 4gfsk notation.
            elif mode == model.vars.mbus_mode.var_enum.ModeR_4p8k:
                len = 18
                sync_a = long(0x7696)
                sync_b = long(0)
            elif mode == model.vars.mbus_mode.var_enum.ModeT_M2O_100k:
                len = 10
                sync_a = long(0x3d)
                sync_b = long(0)
            elif mode == model.vars.mbus_mode.var_enum.ModeT_O2M_32p768k:
                len = 18
                sync_a = long(0x7696)
                sync_b = long(0)
            elif mode == model.vars.mbus_mode.var_enum.ModeS_32p768k:
                len = 18
                sync_a = long(0x7696)
                sync_b = long(0)
            else:
                raise CalculationException("Can't calculate syncwords.  Unexpected Mbus mode selected!")

            # Set syncword 0/1 based on whether frame format A or B is selected
            # If only one syncword exists, don't flip them for frame B format
            if model.vars.mbus_frame_format.value != model.vars.mbus_frame_format.var_enum.FrameB or sync_b == 0:
                syncword_0 = sync_a
                syncword_1 = sync_b
            else:
                syncword_0 = sync_b
                syncword_1 = sync_a

            model.vars.syncword_0.value = syncword_0
            model.vars.syncword_1.value = syncword_1
            model.vars.syncword_length.value = len


