'''
Created on Jan 30, 2013

@author: sesuskic
'''
'''
 modified 9/13/2012 by Eric
 DOC week July 11-15, 2011: change PLLBW to default 200kHz, unless DataRate/Dev requires larger PLLBW
 use a LUT from test bench results provided by Tufan
   PLL_BW :  0 = 600 kHz loop BW
             1 = 450 kHz loop BW
             2 = 300 kHz loop BW
             3 = 200 kHz loop BW
             4 = 100 kHz loop BW
             5 = 65 kHz loop BW
  below table from July 2011 by Tufan: PLL BW for both Rx/Tx modes
 switch $PLL_BW  {
         0 { set_property $propgrp(SYNTH) $propgrplen(SYNTH) 0x00 {0x37 0x04 0x0B 0x06 0x02 0x00 0x03} }
         1 { set_property $propgrp(SYNTH) $propgrplen(SYNTH) 0x00 {0x31 0x04 0x0B 0x04 0x07 0x70 0x03} }
         2 { set_property $propgrp(SYNTH) $propgrplen(SYNTH) 0x00 {0x2C 0x0E 0x0B 0x04 0x0C 0x73 0x03} }
         3 { set_property $propgrp(SYNTH) $propgrplen(SYNTH) 0x00 {0x2A 0x0D 0x06 0x05 0x0F 0x7F 0x03} }
         4 { set_property $propgrp(SYNTH) $propgrplen(SYNTH) 0x00 {0x26 0x0C 0x05 0x05 0x1F 0x7F 0x03} }
     }
 0x2300  SYNTH_PFDCP_CPFF        0x0E    Feed forward charge pump current selection.
 0x2301  SYNTH_PFDCP_CPINT       0x04    Integration charge pump current selection.
 0x2302  SYNTH_VCO_KV            0x06    Scaling on integrated-path Kv and feed forward path Kv.
 0x2303  SYNTH_LPFILT3           0x03    Loop filter of R2 value
 0x2304  SYNTH_LPFILT2           0x03    value of C2 for loop filter
 0x2305  SYNTH_LPFILT1           0x00    value of C1 and C3 for loop filter
 0x2306  SYNTH_LPFILT0           0x03    Bias current of the loop filter.
 0x2307  SYNTH_VCO_KVCAL         0x05    Scaling entire KV of VCO.
 set SYNTH_PFDCP_CPFF bitand(bitxor(icpff_ctl,32),63)  we need to flip the bit<5>
 set SYNTH_PFDCP_CPINT bitand(bitxor(icpint_ctl,8),15)  we need to flip bit<3>
 set SYNTH_VCO_KV (kvff_ctl *4 + kvint_ctl)
 set SYNTH_LPFILT3 (rff_ctl-1)
 set SYNTH_LPFILT2 cff_ctl
 set SYNTH_LPFILT1 cff13_ctl
 set SYNTH_LPFILT0 3
'''
import math

def pll_setting_lut(modem_calc):

    # modified 9/13/2012 according to Eric email
    # 1) If DR <= 75kbps, use chip default loop BW (approx loop BW = 200 kHz)
    # 2) If 75kbps < DR < 125kbps, use loop BW = 300 kHz
    # 3) If 125kbs <= DR < 200kbps, use loop BW = 450kHz
    # 4) If DR >= 200kbps, use loop BW = 600 kHz

    # DOC week July 11-15, 2011: change PLLBW to default 200kHz, unless DataRate/Dev requires larger PLLBW
    # use a LUT from test bench results provided by Tufan
    # #  PLL_BW :  0 = 600 kHz loop BW
    # #            1 = 450 kHz loop BW
    # #            2 = 300 kHz loop BW
    # #            3 = 200 kHz loop BW
    # #            4 = 100 kHz loop BW
    # #            5 = 65 kHz loop BW
    # 
    # modified on July 5, 2012 by request of Eric: instead of (PLLBW v modBW), 
    # only DataRate is used to select PLL-BW: ensure PLLBW/Rs >=3 (PLLBW vs API_Rsymb)
    # shall we differentiate TX and RX?  now it's decided for TX only
    if (modem_calc.inputs.API_modulation_type >1) : # FSK
        if (modem_calc.inputs.API_Rsymb <= 75e3) :
            PLL_BW = 3
            pll_fc = 200000
        elif (modem_calc.inputs.API_Rsymb < 125e3) :    # (75,125)k, use 300kHz PLLBW
            PLL_BW = 2
            pll_fc = 300000
        elif (modem_calc.inputs.API_Rsymb < 200e3) :    # ([125, 200)k, use 450k PLLBW
            PLL_BW = 1
            pll_fc = 450000
        else :
            PLL_BW = 0             # Rs >= 200k,  use 600kHz PLLBW
            pll_fc = 600000
    else :
        PLL_BW = 3     # use default for OOK
        pll_fc = 200000


    # use LUT from empirical data to config, only depending on PLLBW

    LUT = [[0x01, 0x05, 0x0B, 0x05, 0x02, 0x00, 0x03],
           [0x39, 0x04, 0x0B, 0x05, 0x04, 0x01, 0x03],
           [0x34, 0x04, 0x0B, 0x04, 0x07, 0x70, 0x03],
           [0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03],
           [0x2A, 0x0D, 0x06, 0x05, 0x0F, 0x7F, 0x03],
           [0x26, 0x0C, 0x05, 0x05, 0x1F, 0x7F, 0x03]]


    modem_calc.modulator.fields.icpff_ctl = (LUT[PLL_BW][0] ^ 32) & 63
    modem_calc.modulator.fields.icpint_ctl = (LUT[PLL_BW][1] ^ 8) & 15
    tmp = LUT[PLL_BW][2]
    modem_calc.modulator.fields.kvff_ctl = int(math.floor(tmp/4))
    modem_calc.modulator.fields.kvint_ctl = int(tmp-4*modem_calc.modulator.fields.kvff_ctl)
    modem_calc.modulator.fields.rff_ctl = (LUT[PLL_BW][3]) + 1
    modem_calc.modulator.fields.cff_ctl = LUT[PLL_BW][4]
    modem_calc.modulator.fields.cff13_ctl = LUT[PLL_BW][5]
    modem_calc.modulator.fields.lpf_ff_bias = LUT[PLL_BW][6]    # SYNTH_LPFILT0
    modem_calc.modulator.fields.pll_delay = 6* 10**6/(2*math.pi *pll_fc)

