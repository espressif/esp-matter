'''
Created on Jan 25, 2013

@author: sesuskic
'''

UNMOD = 0
OOK = 1
_2FSK = 2
_2GFSK = 3
_4FSK = 4
_4GFSK = 5

def engfsk(API_modulation_type):
    return API_modulation_type == _2GFSK or API_modulation_type == _4GFSK

def en4gfsk(API_modulation_type):
    return API_modulation_type == _4FSK or API_modulation_type == _4GFSK

def enook(API_modulation_type):
    return API_modulation_type == OOK

def en2fsk(API_modulation_type):
    return API_modulation_type == _2FSK or API_modulation_type == _2GFSK

def unmod(API_modulation_type):
    return API_modulation_type == UNMOD

def get_fixed_if_mode(API_if_mode):
    return API_if_mode == 2

def get_zero_if(API_if_mode):
    return API_if_mode == 1

def ook_zero_IF(API_modulation_type, API_if_mode):
    return enook(API_modulation_type) * get_zero_if(API_if_mode)


def get_ramp_dly(API_modulation_type, API_if_mode):
    ramp_dly = 0
    
    ''' decoding modulation type '''
    if (API_modulation_type == 0) : # clean carrier mode
        ramp_dly= 0   # ramp delay
    elif (API_modulation_type == 1) : # OOK mode
        ramp_dly= 0   # ramp delay
    elif (API_modulation_type == 2) : # 2FSK mode
        ramp_dly= 1   # ramp delay
    elif (API_modulation_type == 3) :  # 2GFSK mode
        ramp_dly= 1   # ramp delay
    elif (API_modulation_type == 4) :  # 4FSK mode
        ramp_dly= 4   # ramp delay
    elif (API_modulation_type == 5) :  # 4GFSK mode
        ramp_dly= 5   # ramp delay
    else : # lump into (API_modulation_type ==-3)
        ramp_dly= 4   # ramp delay

    ''' override ramp_dly for zero-IF ook mode '''
    if (get_zero_if(API_if_mode) and enook(API_modulation_type)) :
        ramp_dly = 2
        
    return ramp_dly
    
'''
Fdev is defined as inner deviation
                        4GFSK
   -3           -1         0         1           3
                 <--Fdev--> <--Fdev-->
                        2GFSK
   -3                      0                     3
    <---------Fdev--------> <---------Fdev------->
'''
def get_df(API_modulation_type, API_Fdev):
    # API_Fdev unit is Hz, 19bit
    if (en4gfsk(API_modulation_type)) :
        df = API_Fdev*3     # in Hz, fsk outer deviation, 21-bit
    elif(enook(API_modulation_type)) :
        df = 0       # in Hz, fsk outer deviation, 21-bit
    else :
        df = API_Fdev       # in Hz, fsk outer deviation, 21-bit
    
    return df
