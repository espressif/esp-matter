'''
Created on Apr 8, 2013

@author: sesuskic
'''
from .decode_api import engfsk, en4gfsk, enook, en2fsk, unmod, get_fixed_if_mode, get_zero_if, ook_zero_IF, get_ramp_dly
from .dict2xml import dict2xml

__all = ["ModemCalcData"]

class ModemCalcData(object):
    def populate(self, modem_calc):
        self._fields = modem_calc.modulator.fields.get_field_map().copy()
        self._fields.update(modem_calc.demodulator.fields.get_field_map())
        self._registers = modem_calc.registers
        self._inputs = modem_calc.original_inputs.__dict__
        self._api_decoder = ApiDecoder(modem_calc)
        self._data = {'ModemCalcData': {'fields': self._fields,
                               'registers': self._registers.registers,
                               'ModemCalcInputs': self._inputs,
                               'apidecode': self._api_decoder.__dict__,
                               'if_freq': modem_calc.if_freq,
                               'if_freq_iqcal': modem_calc.if_freq_iqcal,
                               'api_list': modem_calc.api_list._api_list
                               }
                      }
    def get(self):
        return self._to_xml()

    def _to_xml(self):
        xml = dict2xml(self._data)
        return xml.display()

class ApiDecoder(object):
    def __init__(self, modem_calc):
        self.engfsk = int(engfsk(modem_calc.inputs.API_modulation_type))
        self.en4gfsk  = int(en4gfsk(modem_calc.inputs.API_modulation_type))
        self.enook = int(enook(modem_calc.inputs.API_modulation_type))
        self.en2fsk = int(en2fsk(modem_calc.inputs.API_modulation_type))
        self.unmod = int(unmod(modem_calc.inputs.API_modulation_type))
        self.fixed_if_mode = int(get_fixed_if_mode(modem_calc.inputs.API_if_mode))
        self.zero_if = int(get_zero_if(modem_calc.inputs.API_if_mode))
        self.ook_zero_IF = int(ook_zero_IF(modem_calc.inputs.API_modulation_type,
                                                  modem_calc.inputs.API_if_mode))
        self.ramp_dly = int(get_ramp_dly(modem_calc.inputs.API_modulation_type,
                                                modem_calc.inputs.API_if_mode))
    
