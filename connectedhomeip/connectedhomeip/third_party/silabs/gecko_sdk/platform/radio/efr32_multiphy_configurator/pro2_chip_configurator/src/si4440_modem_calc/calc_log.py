'''
Created on Mar 1, 2013

@author: sesuskic
'''
from .decode_api import enook
from ..version_info.version_info import get_version_string

__all__ = ["CalcLog", "Pro2CalcException"]

class Pro2CalcException(Exception):
    def __init__(self, text):
        super(Pro2CalcException, self).__init__(text)

class CalcLog(object):
    def __init__(self, filename=''):
        header = '# Calculator version {:s}\n'.format(get_version_string())
        self._filename = filename
        self._log_list = []
        if self._filename != '':
            fid_log = open(self._filename, 'w')  # 'w'->'a' w/ prior writes
            fid_log.write(header)
            fid_log.close()
        else:
            self._log_list.append(header)
        return
    
    def add_to_log(self, text, raiseerror=False):
        if self._filename != '':
            fid_log = open(self._filename, 'a')
            fid_log.write(text)
            fid_log.close()
        else:
            self._log_list.append(text)
        if raiseerror:
            raise Pro2CalcException(text)
    
    def write_to_calc_log(self, modem_calc):
        if (modem_calc.IQ_CAL_setting == 0):
            self._log_list.append('# Done successfully! \n')
            self._log_list.append('# ------------------------Key Chip Settings------------------------ \n')
            self._log_list.append('# RF is {:6.6f} MHz; {:3.1f} ppm {:8.0f} Hz crystal\n'.format(modem_calc.inputs.API_fc / 1e6, modem_calc.crystal_tol, modem_calc.inputs.API_freq_xo))
            if (enook(modem_calc.inputs.API_modulation_type)):
                self._log_list.append('# Symbol rate is {:6.0f} sps; OOK RX BW is {:6.0f} Hz\n'.format(modem_calc.inputs.API_Rsymb, modem_calc.inputs.API_RXBW))
            else:
                self._log_list.append('# Symbol rate is {:6.0f} sps; Fd is {:6.0f} Hz; Modulation BW is {:6.0f} Hz; modulation index is {:2.3f} \n'.format(modem_calc.inputs.API_Rsymb, modem_calc.demodulator.fields.df, modem_calc.demodulator.BW_mod, modem_calc.demodulator.fields.hModInd))
            #    print('3) Modulation BW is {:6.0f} Hz; modulation index is {:2.3f} \n'.format(BW_mod,hModInd/2**9))
            pro2bar = False
            try:
                foo = modem_calc.demodulator.fields.ndec3
                pro2bar = True
            except:
                self._log_list.append('# Max frequency error(single end) between TX and Rx is {:6.0f} Hz \n'.format(modem_calc.demodulator.fields.tx_rx_err))
            if pro2bar and hasattr(modem_calc.demodulator.fields, "rx_hopping_en"):
                if (modem_calc.demodulator.fields.rx_hopping_en == 1):
                    self._log_list.append('# Max frequency error(single end) between TX and Rx is {:6.0f} Hz \n'.format(modem_calc.tx_rx_err))
                else:
                    self._log_list.append('# Max frequency error(single end) between TX and Rx is {:6.0f} Hz \n'.format(modem_calc.demodulator.fields.tx_rx_err))

            if (not(enook(modem_calc.inputs.API_modulation_type))):
                if (modem_calc.inputs.API_BER_mode):
                    self._log_list.append('# In BER testing mode, PLL AFC is disabled. \n')
                elif (modem_calc.inputs.API_afc_en == 1 or modem_calc.inputs.API_afc_en == 2):
                    self._log_list.append('# AFC correction value is fedback to PLL, and AFC loop will be reset if estimated frequency error exceeds {:6.0f} Hz\n'.format(modem_calc.demodulator.fields.afclim * (float(modem_calc.demodulator.fields.afc_gain) / float(2 ** 6)) * (modem_calc.modulator.fields.dsm_ratio)))
                else:
                    self._log_list.append('# AFC correction value is not fedback to PLL.  Internal modem AFC is still enabled. \n')
            self._log_list.append('# RX IF frequency is  {:6.0f} Hz \n'.format(modem_calc.if_freq))
            self._log_list.append('# RX OSR OF BCR is {:6.3f}  \n'.format(modem_calc.demodulator.fields.OSR_rx_BCR / 2 ** 3))
            self._log_list.append('# WB filter {:d} (BW = {:6.2f} kHz);  NB-filter {:d} (BW = {:4.2f} kHz) \n'.format(modem_calc.demodulator.filter_k1, modem_calc.demodulator.RX_CH_BW_k1, modem_calc.demodulator.filter_k2, modem_calc.demodulator.RX_CH_BW_k2))
            self._log_list.append('# Decby-2 bypass is set to {:d}\n'.format(modem_calc.demodulator.fields.dwn2byp))
            self._log_list.append('# Decby-3 bypass is set to {:d}\n'.format(modem_calc.demodulator.fields.dwn3byp))
            bar = False
            try:
                foo = modem_calc.demodulator.fields.ndec3
                bar = True
            except:
                self._log_list.append('# nDec-0 is {:d};  nDec-1 is {:d}; nDec-2 is {:d}  \n'.format(int(modem_calc.demodulator.fields.ndec0), int(modem_calc.demodulator.fields.ndec1), int(modem_calc.demodulator.fields.ndec2)))
            if bar:
                self._log_list.append('# nDec-0 is {:d};  nDec-1 is {:d}; nDec-2 is {:d}; nDec-3 is {:d}  \n'.format(int(modem_calc.demodulator.fields.ndec0), int(modem_calc.demodulator.fields.ndec1), int(modem_calc.demodulator.fields.ndec2), int(modem_calc.demodulator.fields.ndec3)))
                if hasattr(modem_calc.demodulator.fields, "rx_hopping_en"):
                    if (modem_calc.demodulator.fields.rx_hopping_en == 1):
                        self._log_list.append('# DSA rx-hopping is used for super low data rate; Hopping Step Size is{:6.0f} Hz;Hopping CH number is {:d}.\n' .format(modem_calc.inputs.API_fhst, modem_calc.demodulator.fields.fh_ch_number))
                        if(modem_calc.inputs.API_pm_len < modem_calc.demodulator.fields.fh_ch_number * 4 + 8):
                            self._log_list.append('# Warning: {:d}-bit preamble is not long enough to run DSA rx-hopping for the super low data rate.\n'.format(int(modem_calc.inputs.API_pm_len)))
                            self._log_list.append('#          {:d}-bit preamble is required for the current application.\n'.format(modem_calc.demodulator.fields.fh_ch_number * 4 + 8))

            # print('12)AFC loop gain is   {6.0f} \n'.format(afc_gain))
            # print('13)AFC loop limiter is   {6.0f} \n'.format(rtl_afclim))
            if (modem_calc.demodulator.fields.nonstdpk == 1):
                self._log_list.append('# async demodulator is used.\n')
            else:
                self._log_list.append('# sync demodulator is used.\n')
            if (modem_calc.warning2log):  # add warning msg to log file if any
                self._log_list.append('\n# Note: \n')
                self._log_list.append(modem_calc.warning2log)
        if self._filename == '':
            return
        # write to calc_log.txt
        fid_log = open(self._filename, 'a');
        if (modem_calc.IQ_CAL_setting == 0):
            fid_log.write("".join(self._log_list))
        fid_log.write('\n\n')  # append two lines at end to seperate multiple cases
        fid_log.close()
        return
    
    def log(self):
        return "".join(self._log_list)

