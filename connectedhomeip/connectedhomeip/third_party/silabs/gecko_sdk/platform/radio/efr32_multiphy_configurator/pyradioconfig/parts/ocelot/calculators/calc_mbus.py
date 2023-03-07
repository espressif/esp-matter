from pyradioconfig.parts.common.calculators.calc_mbus import CALC_Mbus
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from py_2_and_3_compatibility import *
from enum import Enum


class CALC_Mbus_Ocelot(CALC_Mbus):

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
            ['ModeN_6p4k', 10, 'Mode N, 6.4kbps'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'MbusModeEnum',
            'List of supported Mbus modoes',
            member_data)

    def calc_mbus_syncwords(self, model):

        if model.profile.name.lower() == 'mbus':
            mode = model.vars.mbus_mode.value
            if mode == model.vars.mbus_mode.var_enum.ModeN_6p4k:
                #New logic for ModeN 6.4kbps
                len = 16
                sync_a = long(0xf68d)
                sync_b = long(0xf672)

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

            else:
                #Call existing logic for other Mbus modes
                super().calc_mbus_syncwords(model)