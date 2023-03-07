"""
Lynx specific PHYs

How to add a new PHY:
--------------------------
*  Add a new Python file in this directory (example: Phy_[GroupName].py)
*  Inside the Py file, create a class that implements IPhy:

    >>> class PHYS_[GroupName](IPhy):

*  Each PHY is a single function in this Py file.
    Example PHY:

    >>> def PHY_ATnT_911_779(self, model):
    >>>     phy = self._makePhy(model, model.profiles.Base, 'PHY ATnT 911 779')
    >>>
    >>>     # Inputs
    >>>     phy.profile_inputs.base_frequency_hz.value = long(911779816)
    >>>     phy.profile_inputs.baudrate_tol_ppm.value = 1000
    >>>     phy.profile_inputs.bitrate.value = 38418
    >>>     phy.profile_inputs.channel_spacing_hz.value = 199814
    >>>     phy.profile_inputs.deviation.value = 39551
    >>>     phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
    >>>
    >>>
    >>>     # Packet Inputs
    >>>     phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
    >>>     phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.MSB_FIRST
    >>>     phy.profile_inputs.payload_white_en.value = True
    >>>     phy.profile_inputs.uart_coding.value = False
    >>>
    >>>     # Variable length includes header
    >>>     phy.profile_inputs.header_calc_crc.value = True
    >>>     phy.profile_inputs.header_en.value = True
    >>>     phy.profile_inputs.header_size.value = 1
    >>>     phy.profile_inputs.header_white_en.value = True
    >>>     phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.MSB_FIRST
    >>>
    >>>     # Advanced Inputs
    >>>     phy.profile_inputs.agc_hysteresis.value = 0
    >>>     phy.profile_inputs.bandwidth_hz.value = 135000
    >>>     phy.profile_inputs.freq_offset_hz.value = 23000
    >>>     phy.profile_inputs.symbols_in_timing_window.value = 10
    >>>     phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2120KHz

"""

import os
import glob
modules = glob.glob(os.path.dirname(__file__)+"/*.py")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*.pyc")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*$py.class")
__all__ = [ os.path.basename(f)[:-3] for f in modules]