'''
Created on Apr 10, 2013

@author: sesuskic
'''

__all__ = ["RxChFlt_rtl"]

class RxChFlt_rtl(object):
    '''
    classdocs
    '''


    def __init__(self, rxcoearray):
        '''
        Constructor
        '''
        self.coe_m3 = (((rxcoearray[13] & 0x300) >> 6) + ((rxcoearray[12] & 0x300) >> 8)) & 0x0f
        self.coe_m2 = (((rxcoearray[11] & 0x300) >> 2) + ((rxcoearray[10] & 0x300) >> 4) + ((rxcoearray[9] & 0x300) >> 6) + ((rxcoearray[8] & 0x300) >> 8)) & 0xff
        self.coe_m1 = (((rxcoearray[7] & 0x300) >> 2) + ((rxcoearray[6] & 0x300) >> 4) + ((rxcoearray[5] & 0x300) >> 6) + ((rxcoearray[4] & 0x300) >> 8)) & 0xff
        self.coe_m0 = ((rxcoearray[3] & 0x300) >> 2) + ((rxcoearray[2] & 0x300) >> 4) + ((rxcoearray[1] & 0x300) >> 6) + ((rxcoearray[0] & 0x300) >> 8)
        self.coe_7_0 = [(x & 0xff) for x in rxcoearray]

