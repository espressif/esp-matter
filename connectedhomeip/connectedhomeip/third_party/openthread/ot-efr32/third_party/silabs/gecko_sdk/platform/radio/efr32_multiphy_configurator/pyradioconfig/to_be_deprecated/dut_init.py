
from pyvsrpc import *
from pyefrdut import *
from pyrfdutapp import *


comType = VSRPC_COM_TYPES.JLINK_ARM_ASIC

# define the communication options
comOptions = JLINK_ARM_OPTIONS(
    # default to first device, else specify the exact serial number
    serialNum = 0,  
    # use USB connection
    interfaceType=JLINK_INTERFACE_TYPE_USB,
    # default rate
    rate=JLINK_DEFAULT_RATE_KHZ)

    
    
def dut_init():
    dut = EFR_RF_DUT(comType, comOptions, platform=EFRDutPlatform.EFR_BMB2)
    return dut
