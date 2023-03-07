from pyradioconfig.parts.common.calculators.frame_coding import CALC_Whitening

class CALC_Frame_Coding_Ocelot(CALC_Whitening):

    def calc_frame_coding_var(self, model):
        #On Ocelot, we removed frame_coding as a Profile Input, so we need to calculate the variable from symbol_encoding

        symbol_encoding = model.vars.symbol_encoding.value

        if symbol_encoding == model.vars.symbol_encoding.var_enum.UART_NO_VAL:
            frame_coding = model.vars.frame_coding.var_enum.UART_NO_VAL
        elif symbol_encoding == model.vars.symbol_encoding.var_enum.UART_VAL:
            frame_coding = model.vars.frame_coding.var_enum.UART_VAL
        elif symbol_encoding == model.vars.symbol_encoding.var_enum.MBUS_3OF6:
            frame_coding = model.vars.frame_coding.var_enum.MBUS_3OF6
        else:
            frame_coding = model.vars.frame_coding.var_enum.NONE

        #Write the model variable
        model.vars.frame_coding.value = frame_coding