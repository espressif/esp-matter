'''
Created on Jan 29, 2013

@author: sesuskic
'''

import math
from .trueround import trueround

__all__ = ["freq_ctl"]

def _elementwiseAnd(*args):
    return [all(tup) for tup in zip(*args)]

def freq_ctl(modem_calc, rev_c0=False):
    
    # First, quantized the rf frequency to 24 bit; add 2**21 offset to shift the 24 bit range to 120MHz and 1080MHz.
    # this is the carrier frequency value customers to program into the part.

    ''' %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% '''
    # fixed settings
    vco_proc_var = 1.0
    #unit_kv_at_fmax = 8e6 * vco_proc_var # Hz/V
    #frac_bits = 19
    #freq_resolution = 30e6/2**frac_bits

    #We do not use the fc_d to compute the integer part of the division word
    #anymore... (8/10/2011) We use fc_d only to calculate outdiv
    fc_d = math.floor(modem_calc.inputs.API_fc/modem_calc.inputs.API_freq_xo*2.0**19) - 2**21
    
    # vco design parameters - vco_max and min to be measured and tweaks to be done in FW.
    if ( modem_calc.inputs.API_Chip_Version==0 or modem_calc.inputs.API_Chip_Version==2 or rev_c0): # normal banded part: v0
        vco_max = 4.2e9 * vco_proc_var
        vco_min = 3.4e9 * vco_proc_var
    else :
        # super continuous band: v1  [2.68, 4] GHz => 12/17/12 by Dave S. #'s
        vco_max = 3841.3215e6 * vco_proc_var
        vco_min = 2817.1845e6 * vco_proc_var

    # first, cacluate freq band values to be stored in ROM/OTP
    # frequency ranges for outdiv ratio of 4, 6, 8, 12, 16, 24
    OUTDIV = sorted(modem_calc.modulator.OUTDIV_SY_SEL_MAP)
    fmin = [vco_min/x for x in OUTDIV]
    fmax = [vco_max/x for x in OUTDIV]
    
    # override for OUTDIV=10 per Dave S. on 10/09/2013
    # rev_c0==pro2plus?  
    if rev_c0:
        # fmax[2] = 425e6  # div-10 covers [345, 425] MHz 
        fmin[1] = 420e6  # div-8 covers [420, 525] MHz per si4440-1618 & Dave's email 8/8/2014


    # # frequency converage check
    # fmin = [max(x, 120e6) for x in fmin]  # lower bound to 120MHz
    # for i in range(0,4) :
    #     if (fmin[i] > fmax[i+1]) :
    #         print('frequency gap between {:0.4g} {:0.4g}'.format(fmin(i),fmax(i+1)))


    # 08/24/2011: check whether RF Fc lies within covered range
    # Tufan mentions for v0 & outdiv==6, vco_max is guaranteed to work only up to 4.02GHz (over PVT corners)
    if ( ( modem_calc.inputs.API_Chip_Version==0 or modem_calc.inputs.API_Chip_Version==2) and not(rev_c0)) : #and (outdiv==6) )
        fmax[1] = 4.02e9/6
        fmin[2] = 420e6     # added 12/17/12 per Dave S.

    # find the freq threshold for switching between 6 output div ratios.
    # quantize the frequencies to 10 bit to be stored in ROM. The values have
    # offset the same as fc_d; coarse freq LSB=30M/2**19*2**14=0.9375MHz
    fth = [0.0, 0.0, 0.0, 0.0, 0.0]
    for i in range(0,5) :
        fth[i] = ( fmin[i]+fmax[i+1] )/2


    # 12/17/2012: for the continuous-freq part (si4464), boundary is finalized, decide outdiv according to table 
    if ( (modem_calc.inputs.API_Chip_Version==1 or modem_calc.inputs.API_Chip_Version==3) and not(rev_c0)): # continuous part boundary set in middle of overlap
        fth = [x*1e6 for x in [705.0, 475.0, 353.0, 237.5, 177.0]]

    fth_d = [ ((x/modem_calc.inputs.API_freq_xo*2**19)-2**21)/2**14 for x in fth]

    ########################################################################
    # below is the state machine to be implemented in MCU
    # take frequency hopping deviation into target RF frequency. This target
    # frequency shall also be used for vco calibration
    
    # figure out which frequency band freq falls into and choose output div ratio
    # 10 bit coarse freq_d
    ####Note:8.10.2011 The below code is not implemented by the MCU anymore.
    #### It is done offline and its results are loaded onto the chip
    ####Technically this 10 bit precise calculations can all be converted back
    ####to full floating point accuracy. We are leaving it in use for the
    ####computation of outdiv. (TCK/8.10.2011)
    freq_d_coarse = fc_d/2**14


    modem_calc.modulator.fields.outdiv = OUTDIV[5]
    for i in range(4, -1, -1) :
        if freq_d_coarse >= fth_d[i] :    
            modem_calc.modulator.fields.outdiv = OUTDIV[i]

    outdiv_ord = OUTDIV.index(modem_calc.modulator.fields.outdiv)   # order of divider ratio 

    # no need to check when IQcal
    # if ( modem_calc.inputs.API_fc<fmin[outdiv_ord] ) or (modem_calc.inputs.API_fc>fmax[outdiv_ord] ) : # and (config.IQ_CAL_setting==0)
    #     fminTxt = [math.ceil(x/1e6) for x in fmin]
    #     fmaxTxt = [math.floor(x/1e6) for x in fmax]
    #     modem_calc.calc_log.add_to_log('RF carrier frequency = {:3.2f} MHz is out of range!\n'.format(modem_calc.inputs.API_fc/1e6) +
    #                         'Change RF frequency and retry.\nCovered RF carrier frequencies are' +
    #                         '\n\t[ {} \n\t {} ]'.format(fminTxt, fmaxTxt) + '\tMHz\n', True)

    # next caculate the integer and frac input feeding into DSM, absence of
    # fofs and freq modulation
    if (modem_calc.inputs.API_hi_pfm_div_mode) : # means the fbdivider pre-scaler is div/2 otherwise div/4
        ps_fbdiv = 2
    else :
        ps_fbdiv = 4

    # We do not use the fc_d to compute the integer part of the division word
    # anymore... (8/10/2011) We use fc_d only to calculate outdiv
    fc_d = (modem_calc.modulator.fields.outdiv * modem_calc.inputs.API_fc) /modem_calc.inputs.API_freq_xo*2**19

    modem_calc.modulator.fields.fbdiv_int = int(math.floor( fc_d/ps_fbdiv/2**19 ))
    fbdiv_frac_fp = fc_d/ps_fbdiv/2**19 - modem_calc.modulator.fields.fbdiv_int
    modem_calc.modulator.fields.fbdiv_frac_19b = math.floor(fbdiv_frac_fp*2**19)

    #convert the FH channel from Hz to Unitless frac ( aka fractional DSM input)
    modem_calc.modulator.fields.fhst_16b = trueround((modem_calc.inputs.API_fhst * float(modem_calc.modulator.fields.outdiv)) / (ps_fbdiv * modem_calc.inputs.API_freq_xo) * 2**19)
    #print('\toutdiv = {:d}, fbdiv_int = {:d}\n'.format(outdiv, fbdiv_int)) # debug print


    # calculate carrier freq Fc for use during IQ calibration 
    # need same divider ratio as in operation mode

    # if (modem_calc.IQ_CAL_setting==0) :
    #     mult = modem_calc.inputs.API_fc/modem_calc.inputs.API_freq_xo                  # multiple of xtal
    #     xtal_harmonic_ord = math.ceil( mult )    # take higher end integer to avoid 1050MHz
    #
    #     IQ_CAL_Fc_range = [(x*modem_calc.inputs.API_freq_xo + modem_calc.inputs.API_freq_xo/64) for x in [xtal_harmonic_ord-1,  xtal_harmonic_ord]] # use FIF-freq in RXLO in IQcal
    #     valid = _elementwiseAnd([(x > fmin[outdiv_ord]) for x in IQ_CAL_Fc_range],
    #                           [(x < fmax[outdiv_ord]) for x in IQ_CAL_Fc_range]) # valid locking freq?
    #     if ( sum(valid)>1 ) : # both freq are valid, use closest freq
    #         if ((mult>(xtal_harmonic_ord - 0.5))):
    #             # top half
    #             modem_calc.IQ_CAL_Fc = IQ_CAL_Fc_range[1]
    #         else:
    #             # lower half
    #             modem_calc.IQ_CAL_Fc = IQ_CAL_Fc_range[0]
    #     else : # at least one freq is valid
    #         modem_calc.IQ_CAL_Fc  = IQ_CAL_Fc_range[valid.index(True)]
    #     modem_calc.IQ_CAL_Fc = modem_calc.IQ_CAL_Fc - modem_calc.inputs.API_freq_xo/64    # remove the margin

# revC0 Covered RF carrier frequencies are
#     [ 142,  175 ]
#     [ 213,  262 ]
#     [ 284,  350 ]
#     [ 340,  420 ]
#     [ 425,  525 ]
#     [ 850,  1050 ]   MHz   # 07/15/13 based on [4 8 10 12 16 24]

# ## si4460/1/3  Covered RF carrier frequencies [4 6 8 12 16 24]
#     [ 141.67, 175.00 ]
#     [ 212.50, 262.50 ]
#     [ 283.33, 350.00 ]
#     [ 425.00, 525.00 ]      #=> [420, 525] per Dave S. on 12/17/12
#     [ 566.67, 670.00 ]
#     [ 850.00, 1050.00 ]   MHz
# 
# ## si4464   Covered RF carrier frequencies per Dave S. on 12/17/12
#     [ 118,  160 ]
#     [ 177,  240 ]
#     [ 235,  320 ]
#     [ 353,  480 ]
#     [ 470,  640 ]
#     [ 705,  960 ]   MHz
#         VCO           Hole Size
# Div    2817.1845    3841.3215     
# 24    118     160     
# 16    177     234.99    17
# 12    235     320     
#  8    353     469.99    33
#  6    470     640     
#  4    705     960     65
