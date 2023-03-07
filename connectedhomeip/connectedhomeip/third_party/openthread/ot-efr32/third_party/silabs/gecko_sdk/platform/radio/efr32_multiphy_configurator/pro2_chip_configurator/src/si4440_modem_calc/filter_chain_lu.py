'''
Created on Jan 28, 2013

@author: sesuskic
'''

import math
from .decode_api import enook

__all__ = ["FilterChainLu"]

class FilterChainLu(object):

    def _init_newOSRt(self, modem_calc, newOSRt, iqcal):
        self.newOSRt = newOSRt
        decimLEN = len(self.decim_table)
    
        highband = (modem_calc.inputs.API_fc>=525e6)
        if (iqcal>0) : # prepare for IQCAl mode, NB 
            if (highband>0):
                Rsymb = 4e3
            else :
                Rsymb = 2e3
            chBW = 1.5*Rsymb/1e3
            mod_ook = 0
            OSRtune = 0 
        else :
            Rsymb = modem_calc.inputs.API_Rsymb
            chBW = modem_calc.demodulator.Ch_Fil_Bw
            mod_ook = enook(modem_calc.inputs.API_modulation_type)
            OSRtune = modem_calc.inputs.API_OSRtune
        #print('FilterChainLU: highband = {:2d}, Rsymb = {:3f}ksps'.format(highband, Rsymb/1e3 ) )
        #print('API_fc = {:.3f}MHz, Rsymb = {:.3f}ksps'.format( modem_calc.inputs.API_fc/1e6, Rsymb/1e3 ) )

        if (mod_ook==1) : # require ndec0>0, => OSR_rx_CH > 2*OOKminOSR=14
            OSR_rx_CH = max( (4*chBW*1000/Rsymb), modem_calc.demodulator.OOKminOSR*2 ) # set target > 14
        else: 
            OSR_rx_CH = max(4*chBW*1000/Rsymb, 8);    # set target > 8 (h~=1, BW=2Rs)
    
        OSR_after_dec8 = modem_calc.inputs.API_freq_xo/8/Rsymb   # OSR after decim-by-8
        dec_rate = OSR_after_dec8/OSR_rx_CH        # target programmable decim rate
    
        tableLU = [dec_rate < x for x in self.decim_table]
        if (sum(tableLU) == 0) :
            idx = decimLEN - 1                # use max decim rate
        else :  
            idx = max( tableLU.index(True) - 1, 0)    # limit min to decim rate = 1 
    
        self.new_idx = idx          # index used in final decim ratio if OSRtune==0
        #OSR_chfil_i = OSR_after_dec8/decim_table[idx]    # init OSR at chfil
    
    
        # move decimation ratio up or down based on API_OSRtune (in FSK)
        if (not(mod_ook) and OSRtune!=0) : # FSK with non-default OSRtune
            self.new_idx = idx - OSRtune # new dec ratio stepped up/down
            if (OSRtune>0) : # step up OSR: always give a solution even if BW too big
                if (self.new_idx <0) :             # absolute boundary of min decim-ratio
                    self.new_idx = 0
                    self.newOSRt = idx - self.new_idx    # true OSRtune applied
                    #modem_calc.warning2log += '# No room to step up OSR by {:d} steps,  instead OSRtune={:d} steps is applied\n'.format(API_OSRtune, newOSRt)
            else : # API_OSRtune<0
                if (self.new_idx>decimLEN-1) :      # absolute boundary of max decim-ratio
                    self.new_idx = decimLEN - 1
                    self.newOSRt = idx - self.new_idx    # true OSRtune applied
                    #modem_calc.warning2log += '# No room to step up OSR by {:d} steps,  instead OSRtune={:d} steps is applied\n'.format(API_OSRtune, newOSRt)
                OSR_chfil = OSR_after_dec8/self.decim_table[self.new_idx]   # OSR at chfil
                if (OSR_chfil<7) :
                    self.newOSRt = idx - self.new_idx 
                    self._init_newOSRt(modem_calc, self.newOSRt)
        
    def __init__(self, modem_calc):
        
        self.decim_table = modem_calc.demodulator.decim_table
        self.nDec = 0
        self.pre_dec_val = 1
        self.filter_k1 = 1
        self.decim_ratio_iqcal = 1

        # run another time for IQ_CAL mode, save decim-ratio_iqcal
        if (modem_calc.IQ_CAL_setting==0) :
            self._init_newOSRt(modem_calc, 0, 1)
            self.decim_ratio_iqcal = self.decim_table[self.new_idx]

        self._init_newOSRt(modem_calc, modem_calc.inputs.API_OSRtune, 0)
        prog_decim_rate = self.decim_table[self.new_idx]

        logd = math.log(prog_decim_rate, 2)
        # determine register values from decim-ratio.  stick to old one, always use
        # 1st poly decim-2 before ndec
        if (math.floor(logd)!=logd) :    # decim-3 is used
            self.pre_dec_val = 3 
            self.nDec = math.log(prog_decim_rate/3, 2)
        else : # skip decim-3
            if (logd<1) :
                self.pre_dec_val = 1
                self.nDec = 0
            else :
                self.pre_dec_val = 2
                self.nDec = logd - 1  # >=0
    
        # Jira-1249: for revB OOK, constrain nDec<=3 due to timing issues 
        if (modem_calc.inputs.API_modulation_type==1) : 
            self.nDec = min(self.nDec, 3)
            

        # determine channel filter index
        BW_norm = modem_calc.demodulator.Ch_Fil_Bw*self.pre_dec_val*2**self.nDec # unit is kHz
    
        tmp = [BW_norm > x for x in modem_calc.demodulator.BW_table]
        if (sum(tmp) == 0) :     
            self.filter_k1 = 15 
            if (( modem_calc.demodulator.BW_table[self.filter_k1-1]/BW_norm >1.1 ) and ( self.newOSRt>0 ) ) :
                modem_calc.warning2log + '# Obtained channel filter bandwidth larger than required, try decrease OSRtune\n'
                # print(('CALC:input:OSRtune', 'Obtained channel filter bandwidth larger than required, try decrease OSRtune\n'))
        elif ( tmp.index(True)==0 ) :
            self.filter_k1 = 1
            if  ((modem_calc.demodulator.BW_table[self.filter_k1-1]/BW_norm < 0.9 ) and ( self.newOSRt<0 )) :
                modem_calc.warning2log += '# Obtained channel filter bandwidth less than desired, try increase OSRtune\n'
                # print('CALC:input:OSRtune Obtained channel filter bandwidth less than desired, try increase OSRtune\n')
        else :
            self.filter_k1 = max( tmp.index(True), 0)
