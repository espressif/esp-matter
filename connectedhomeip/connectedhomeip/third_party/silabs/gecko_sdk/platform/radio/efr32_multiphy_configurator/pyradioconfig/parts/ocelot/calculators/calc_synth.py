from pyradioconfig.parts.lynx.calculators.calc_synth import CALC_Synth_lynx
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from py_2_and_3_compatibility import *
from enum import Enum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
import numpy as np
#This file contains calculations related to the configuring the frequency synthesizer

class CALC_Synth_ocelot(CALC_Synth_lynx):

    fvcomin = 4450e6
    fvcomax = 5950e6

    def __init__(self):
        super().__init__()
        self.synth_freq_min_limit = 2300000000
        self.synth_freq_max_limit = 2900000000

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator
        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Build variables from Lynx
        calc_synth_lynx_obj = CALC_Synth_lynx()
        calc_synth_lynx_obj.buildVariables(model)

        # New variables
        self._addModelVariable(model, 'synth_rx_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.synth_rx_mode.var_enum = CreateModelVariableEnum(
            enum_name='SynthRxModeEnum',
            enum_desc='Defined Synth RX Mode',
            member_data=[
                ['MODE1', 0, 'RX Mode 1'],
                ['MODE2', 1, 'RX Mode 2'],
                ['MODE3', 2, 'RX Mode 3'],
            ])
        self._addModelActual(model, 'synth_rx_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.synth_rx_mode_actual.var_enum = model.vars.synth_rx_mode.var_enum


        self._addModelVariable(model, 'synth_tx_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.synth_tx_mode.var_enum = CreateModelVariableEnum(
            enum_name='SynthTxModeEnum',
            enum_desc='Defined Synth TX Mode',
            member_data=[
                ['MODE1', 0, 'TX Mode 1'],
                ['MODE2', 1, 'TX Mode 2'],
                ['MODE3', 2, 'TX Mode 3'],
                ['MODE4', 3, 'TX Mode 4'],
                ['MODE5', 4, 'TX Mode 5'],
                ['MODE6', 5, 'TX Mode 6'],
                ['MODE7', 6, 'TX Mode 7'],
                ['MODE8', 7, 'TX Mode 8'],
                ['MODE9', 8, 'TX Mode 9'],
                ['MODE10', 9, 'TX Mode 10']
            ])
        self._addModelActual(model, 'synth_tx_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.synth_tx_mode_actual.var_enum = model.vars.synth_tx_mode.var_enum


        self._addModelVariable(model, 'rf_band', Enum, ModelVariableFormat.DECIMAL)
        model.vars.rf_band.var_enum = CreateModelVariableEnum(
            enum_name='RFBandEnum',
            enum_desc='RF Band',
            member_data=[
                ['BAND_169', 0, '169 MHz Band'],
                ['BAND_315', 1, '315 MHz Band'],
                ['BAND_434', 2, '434 MHz Band'],
                ['BAND_490', 3, '490 MHz Band'],
                ['BAND_868', 4, '868 MHz Band'],
                ['BAND_915', 5, '915 MHz Band'],
                ['BAND_1432', 6, '1432 MHz Band'],
                ['BAND_2400', 7, '2400 MHz Band'],
            ])

        self._addModelVariable(model, 'synth_settling_mode', Enum, ModelVariableFormat.DECIMAL)
        model.vars.synth_settling_mode.var_enum = CreateModelVariableEnum(
            enum_name='SynthSettlingMode',
            enum_desc='Synth Settling Mode',
            member_data=[
                ['NORMAL', 0, 'Normal Operation Mode (Recommended)'],
                ['FAST', 1, 'Fast Settling (higher current)'],
                ['FASTEST', 2, 'Fastest Settling (higher current and phase noise)'],
            ])

        #Deprecated variable (needs to stay around as we have a deprecated Profile Input)
        self._addModelVariable(model, 'max_tx_power_dbm', int, ModelVariableFormat.DECIMAL,
                               desc='Maximum transmit power expected from this device (DEPRECATED)')

    def calc_rf_band(self, model):
       ### revised to include BAND_315
        rf_freq = model.vars.base_frequency_hz.value

        if ( rf_freq > 2.0e9 ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_2400
        elif ( rf_freq > 1.2e9 ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_1432
        elif ( rf_freq > 885e6 ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_915
        elif( rf_freq > 700e6 ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_868
        elif( rf_freq > 455e6 ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_490
        elif( rf_freq > 400e6  ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_434
        elif (rf_freq > 300e6 ):
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_315
        else:
            model.vars.rf_band.value = model.vars.rf_band.var_enum.BAND_169

    #


    def calc_synth_settling_mode(self, model):

        #Set synth settling mode to Normal for now
        model.vars.synth_settling_mode.value = model.vars.synth_settling_mode.var_enum.NORMAL

    def calc_rx_mode(self, model):

        # Revised for unified PHYs from Ranga per https://jira.silabs.com/browse/MCUW_RADIO_CFG-1477

        synth_settling_mode = model.vars.synth_settling_mode.value

        if synth_settling_mode == model.vars.synth_settling_mode.var_enum.FAST:
            model.vars.synth_rx_mode.value = model.vars.synth_rx_mode.var_enum.MODE3
        elif synth_settling_mode == model.vars.synth_settling_mode.var_enum.FASTEST:
            model.vars.synth_rx_mode.value = model.vars.synth_rx_mode.var_enum.MODE1
        else:
            model.vars.synth_rx_mode.value = model.vars.synth_rx_mode.var_enum.MODE2

    def calc_tx_mode(self, model):

        band = model.vars.rf_band.value
        etsi = model.vars.etsi_cat1_compatible.value
        baudrate = model.vars.baudrate.value
        modulation_type = model.vars.modulation_type.value

        # Revised TX settings from Ranga per https://jira.silabs.com/browse/MCUW_RADIO_CFG-1379
        # Revised for unified PHYs per https://jira.silabs.com/browse/MCUW_RADIO_CFG-1476

        # Fixed settings for OOK, DBPSK, BPSK
        if modulation_type == model.vars.modulation_type.var_enum.DBPSK:
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE8       # Sub-GHz, 3 MHz, QNC
        elif modulation_type == model.vars.modulation_type.var_enum.BPSK:
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE6       # Sub-GHz, 2 MHz, QNC
        elif modulation_type == model.vars.modulation_type.var_enum.OOK:
            model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE4       # Sub-GHz, 500 kHz, QNC
        else:
            # Settings based on band and baudrate
            if band == model.vars.rf_band.var_enum.BAND_2400:
                if baudrate > 1.5e6:
                    model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE10  # 2.4 GHz, 1.5 MHz, BLE 2Mbps
                else:
                    model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE9  # 2.4 GHz, 1 MHz, BLE 1 Mbps
            elif baudrate > 1250e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE8  # Sub-GHz, 3 MHz, QNC
            elif baudrate > 1000e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE7  # Sub-GHz, 2.5 MHz, QNC
            elif baudrate > 750e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE6  # Sub-GHz, 2 MHz, QNC
            elif baudrate > 500e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE1  # Sub-GHz, 1.5 MHz, QNC
            elif baudrate > 333e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE5  # Sub-GHz, 1 MHz, QNC
            elif baudrate > 200e3:
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE4  # Sub-GHz, 500 kHz, QNC
            elif etsi == model.vars.etsi_cat1_compatible.var_enum.Band_868:  # Mode 3 only for 868 MHz - 14/20/27 dBm
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE3  # Sub-GHz, 350 kHz, QNC
            else:  # Mode 2 default for all narrow-band PHYs and all Pout - 14/20/24/27/30 dBm
                model.vars.synth_tx_mode.value = model.vars.synth_tx_mode.var_enum.MODE2  # Sub-GHz, 300 kHz, LP

    def calc_rx_mode_reg(self, model):

        rx_mode = model.vars.synth_rx_mode.value
        ind = rx_mode.value

        # Revised for unified PHYs from Ranga per https://jira.silabs.com/browse/MCUW_RADIO_CFG-1477

        rx_mode_settings = {
            'SYNTH.LPFCTRL2RX.CASELRX':         [1,1,0],
            'SYNTH.LPFCTRL2RX.CAVALRX':         [8,16,0],
            'SYNTH.LPFCTRL2RX.CZSELRX':         [1,1,1],
            'SYNTH.LPFCTRL2RX.CZVALRX':         [70,128,254],
            'SYNTH.LPFCTRL2RX.CFBSELRX':        [0,0,0],
            'SYNTH.LPFCTRL2RX.LPFGNDSWENRX':    [0,0,0],
            'SYNTH.LPFCTRL2RX.MODESELRX':       [0,0,1],
            'SYNTH.LPFCTRL1RX.OP1BWRX':         [5,0,0],
            'SYNTH.LPFCTRL1RX.OP1COMPRX':       [13,7,10],
            'SYNTH.LPFCTRL1RX.RFBVALRX':        [0,0,2],
            'SYNTH.LPFCTRL1RX.RPVALRX':         [0,7,5],
            'SYNTH.LPFCTRL1RX.RZVALRX':         [3,14,9],
            'SYNTH.LPFCTRL2RX.LPFSWENRX':       [1,1,1],
            'SYNTH.DSMCTRLRX.REQORDERRX':       [0,0,0],
            'SYNTH.DSMCTRLRX.MASHORDERRX':      [0,1,0],
            'SYNTH.DSMCTRLRX.DEMMODERX':        [1,0,1],
            'SYNTH.DSMCTRLRX.LSBFORCERX':       [0,1,0],
            'SYNTH.DSMCTRLRX.DSMMODERX':        [0,1,0],
            'SYNTH.DSMCTRLRX.DITHERDACRX':      [3,0,3],
            'SYNTH.DSMCTRLRX.DITHERDSMOUTPUTRX':[7,0,7],
            'SYNTH.DSMCTRLRX.DITHERDSMINPUTRX': [1,0,1],
            'RAC.SYMMDCTRL.SYMMDMODERX':        [2,4,2],
            'RAC.SYTRIM1.SYLODIVLDOTRIMNDIORX': [4,4,4],
            'RAC.SYTRIM1.SYLODIVLDOTRIMCORERX': [0,0,0],
            'RAC.VCOCTRL.VCODETAMPLITUDERX':    [4,4,4],
            'RAC.SYTRIM0.SYCHPCURRRX':          [5,5,5],
            'RAC.SYTRIM0.SYCHPREPLICACURRADJ':  [1,1,1],
            'RAC.RX.SYCHPBIASTRIMBUFRX':        [1,0,1],
            'RAC.RX.SYPFDCHPLPENRX':            [0,1,0],
            'RAC.RX.SYPFDFPWENRX':              [0,1,0],
            'RAC.SYEN.SYCHPLPENRX':             [0,1,0],
            'RAC.SYTRIM0.SYCHPLEVPSRCRX':       [7,0,7],
            'RAC.SYTRIM0.SYCHPSRCENRX':         [1,0,1]}

        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CASELRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CASELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CAVALRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CAVALRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CZSELRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CZSELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CZVALRX,          rx_mode_settings['SYNTH.LPFCTRL2RX.CZVALRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_CFBSELRX,         rx_mode_settings['SYNTH.LPFCTRL2RX.CFBSELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_LPFGNDSWENRX,     rx_mode_settings['SYNTH.LPFCTRL2RX.LPFGNDSWENRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_MODESELRX,        rx_mode_settings['SYNTH.LPFCTRL2RX.MODESELRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_OP1BWRX,          rx_mode_settings['SYNTH.LPFCTRL1RX.OP1BWRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_OP1COMPRX,        rx_mode_settings['SYNTH.LPFCTRL1RX.OP1COMPRX'][ind] )
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_RFBVALRX,         rx_mode_settings['SYNTH.LPFCTRL1RX.RFBVALRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_RPVALRX,          rx_mode_settings['SYNTH.LPFCTRL1RX.RPVALRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1RX_RZVALRX,          rx_mode_settings['SYNTH.LPFCTRL1RX.RZVALRX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_LPFSWENRX,        rx_mode_settings['SYNTH.LPFCTRL2RX.LPFSWENRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_REQORDERRX,        rx_mode_settings['SYNTH.DSMCTRLRX.REQORDERRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_MASHORDERRX,       rx_mode_settings['SYNTH.DSMCTRLRX.MASHORDERRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DEMMODERX,         rx_mode_settings['SYNTH.DSMCTRLRX.DEMMODERX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_LSBFORCERX,        rx_mode_settings['SYNTH.DSMCTRLRX.LSBFORCERX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DSMMODERX,         rx_mode_settings['SYNTH.DSMCTRLRX.DSMMODERX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DITHERDACRX,       rx_mode_settings['SYNTH.DSMCTRLRX.DITHERDACRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DITHERDSMOUTPUTRX, rx_mode_settings['SYNTH.DSMCTRLRX.DITHERDSMOUTPUTRX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLRX_DITHERDSMINPUTRX,  rx_mode_settings['SYNTH.DSMCTRLRX.DITHERDSMINPUTRX'][ind])
        self._reg_write(model.vars.RAC_SYMMDCTRL_SYMMDMODERX,         rx_mode_settings['RAC.SYMMDCTRL.SYMMDMODERX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMNDIORX,  rx_mode_settings['RAC.SYTRIM1.SYLODIVLDOTRIMNDIORX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMCORERX,  rx_mode_settings['RAC.SYTRIM1.SYLODIVLDOTRIMCORERX'][ind])
        self._reg_write(model.vars.RAC_VCOCTRL_VCODETAMPLITUDERX,     rx_mode_settings['RAC.VCOCTRL.VCODETAMPLITUDERX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPCURRRX,           rx_mode_settings['RAC.SYTRIM0.SYCHPCURRRX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPREPLICACURRADJ,   rx_mode_settings['RAC.SYTRIM0.SYCHPREPLICACURRADJ'][ind])
        self._reg_write(model.vars.RAC_RX_SYCHPBIASTRIMBUFRX,         rx_mode_settings['RAC.RX.SYCHPBIASTRIMBUFRX'][ind])
        self._reg_write(model.vars.RAC_RX_SYPFDCHPLPENRX,             rx_mode_settings['RAC.RX.SYPFDCHPLPENRX'][ind])
        self._reg_write(model.vars.RAC_RX_SYPFDFPWENRX,               rx_mode_settings['RAC.RX.SYPFDFPWENRX'][ind])
        self._reg_write(model.vars.RAC_SYEN_SYCHPLPENRX,              rx_mode_settings['RAC.SYEN.SYCHPLPENRX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPLEVPSRCRX,        rx_mode_settings['RAC.SYTRIM0.SYCHPLEVPSRCRX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPSRCENRX,          rx_mode_settings['RAC.SYTRIM0.SYCHPSRCENRX'][ind])

    def calc_tx_mode_reg(self, model):

        tx_mode = model.vars.synth_tx_mode.value
        ind = tx_mode.value

        # Revised TX settings from Ranga per https://jira.silabs.com/browse/MCUW_RADIO_CFG-1379
        # Revised for unified PHYs per https://jira.silabs.com/browse/MCUW_RADIO_CFG-1476

        tx_mode_settings = {
            'SYNTH.LPFCTRL2TX.CASELTX':         [1,1,0,0,1,1,1,1,1,1],
            'SYNTH.LPFCTRL2TX.CAVALTX':         [8,16,0,0,15,6,4,2,15,8],
            'SYNTH.LPFCTRL2TX.CZSELTX':         [1,1,1,1,1,1,1,1,1,1],
            'SYNTH.LPFCTRL2TX.CZVALTX':         [70,128,255,215,126,40,22,6,126,70],
            'SYNTH.LPFCTRL2TX.CFBSELTX':        [0,0,0,0,0,0,0,0,0,0],
            'SYNTH.LPFCTRL2TX.LPFGNDSWENTX':    [0,0,0,0,0,0,0,0,0,0],
            'SYNTH.LPFCTRL2TX.MODESELTX':       [0,0,1,1,0,0,0,0,0,0],
            'SYNTH.LPFCTRL1TX.OP1BWTX':         [5,0,0,0,0,8,11,15,0,5],
            'SYNTH.LPFCTRL1TX.OP1COMPTX':       [13,7,10,11,13,13,14,15,13,13],
            'SYNTH.LPFCTR1TX.RFBVALTX':         [0,0,4,7,0,0,0,0,0,0],
            'SYNTH.LPFCTRL1TX.RPVALTX':         [0,7,3,1,0,0,0,0,0,0],
            'SYNTH.LPFCTRL1TX.RZVALTX':         [3,14,4,0,0,6,9,11,0,3],
            'SYNTH.LPFCTRL2TX.LPFSWENTX':       [1,1,1,1,1,1,1,1,1,1],
            'SYNTH.DSMCTRLTX.REQORDERTX':       [0,0,0,0,0,0,0,0,0,0],
            'SYNTH.DSMCTRLTX.MASHORDERTX':      [0,1,0,0,0,0,0,0,0,0],
            'SYNTH.DSMCTRLTX.DEMMODETX':        [1,0,1,1,1,1,1,1,1,1],
            'SYNTH.DSMCTRLTX.LSBFORCETX':       [0,1,0,0,0,0,0,0,0,0],
            'SYNTH.DSMCTRLTX.DSMMODETX':        [0,1,0,0,0,0,0,0,0,0],
            'SYNTH.DSMCTRLTX.DITHERDACTX':      [3,0,3,3,3,3,3,3,3,3],
            'SYNTH.DSMCTRLTX.DITHERDSMOUTPUTTX':[3,0,3,3,3,3,3,3,3,3],
            'SYNTH.DSMCTRLTX.DITHERDSMINPUTTX': [1,0,1,1,1,1,1,1,1,1],
            'RAC.SYMMDCTRL.SYMMDMODETX':        [2,4,2,2,2,2,2,2,2,2],
            'RAC.SYTRIM1.SYLODIVLDOTRIMNDIOTX': [4,4,4,4,4,4,4,4,4,4],
            'RAC.SYTRIM1.SYLODIVLDOTRIMCORETX': [0,0,0,0,0,0,0,0,0,0],
            'RAC.VCOCTRL.VCODETAMPLITUDETX':    [4,4,4,4,4,4,4,4,4,4],
            'RAC.TX.SYCHPBIASTRIMBUFTX':        [1,0,1,1,1,1,1,1,1,1],
            'RAC.TX.SYPFDCHPLPENTX':            [0,1,0,0,0,0,0,0,0,0],
            'RAC.TX.SYPFDFPWENTX':              [0,1,0,0,0,0,0,0,0,0],
            'RAC.SYEN.SYCHPLPENTX':             [0,1,0,0,0,0,0,0,0,0],
            'RAC.SYTRIM0.SYCHPLEVPSRCTX':       [7,0,7,7,7,7,7,7,7,7],
            'RAC.SYTRIM0.SYCHPSRCENTX':         [1,0,1,1,1,1,1,1,1,1]}

        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CASELTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CASELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CAVALTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CAVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CZSELTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CZSELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CZVALTX,            tx_mode_settings['SYNTH.LPFCTRL2TX.CZVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_CFBSELTX,           tx_mode_settings['SYNTH.LPFCTRL2TX.CFBSELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_LPFGNDSWENTX,       tx_mode_settings['SYNTH.LPFCTRL2TX.LPFGNDSWENTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_MODESELTX,          tx_mode_settings['SYNTH.LPFCTRL2TX.MODESELTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_OP1BWTX,            tx_mode_settings['SYNTH.LPFCTRL1TX.OP1BWTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_OP1COMPTX,          tx_mode_settings['SYNTH.LPFCTRL1TX.OP1COMPTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_RFBVALTX,           tx_mode_settings['SYNTH.LPFCTR1TX.RFBVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_RPVALTX,            tx_mode_settings['SYNTH.LPFCTRL1TX.RPVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL1TX_RZVALTX,            tx_mode_settings['SYNTH.LPFCTRL1TX.RZVALTX'][ind])
        self._reg_write(model.vars.SYNTH_LPFCTRL2TX_LPFSWENTX,          tx_mode_settings['SYNTH.LPFCTRL2TX.LPFSWENTX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_REQORDERTX,          tx_mode_settings['SYNTH.DSMCTRLTX.REQORDERTX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_MASHORDERTX,         tx_mode_settings['SYNTH.DSMCTRLTX.MASHORDERTX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DEMMODETX,           tx_mode_settings['SYNTH.DSMCTRLTX.DEMMODETX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_LSBFORCETX,          tx_mode_settings['SYNTH.DSMCTRLTX.LSBFORCETX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DSMMODETX,           tx_mode_settings['SYNTH.DSMCTRLTX.DSMMODETX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DITHERDACTX,         tx_mode_settings['SYNTH.DSMCTRLTX.DITHERDACTX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DITHERDSMOUTPUTTX,   tx_mode_settings['SYNTH.DSMCTRLTX.DITHERDSMOUTPUTTX'][ind])
        self._reg_write(model.vars.SYNTH_DSMCTRLTX_DITHERDSMINPUTTX ,   tx_mode_settings['SYNTH.DSMCTRLTX.DITHERDSMINPUTTX'][ind])
        self._reg_write(model.vars.RAC_SYMMDCTRL_SYMMDMODETX,           tx_mode_settings['RAC.SYMMDCTRL.SYMMDMODETX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMNDIOTX,    tx_mode_settings['RAC.SYTRIM1.SYLODIVLDOTRIMNDIOTX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMCORETX,    tx_mode_settings['RAC.SYTRIM1.SYLODIVLDOTRIMCORETX'][ind])
        self._reg_write(model.vars.RAC_VCOCTRL_VCODETAMPLITUDETX,       tx_mode_settings['RAC.VCOCTRL.VCODETAMPLITUDETX'][ind])
        self._reg_write(model.vars.RAC_TX_SYCHPBIASTRIMBUFTX,           tx_mode_settings['RAC.TX.SYCHPBIASTRIMBUFTX'][ind])
        self._reg_write(model.vars.RAC_TX_SYPFDCHPLPENTX,               tx_mode_settings['RAC.TX.SYPFDCHPLPENTX'][ind])
        self._reg_write(model.vars.RAC_TX_SYPFDFPWENTX,                 tx_mode_settings['RAC.TX.SYPFDFPWENTX'][ind])
        self._reg_write(model.vars.RAC_SYEN_SYCHPLPENTX,                tx_mode_settings['RAC.SYEN.SYCHPLPENTX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPLEVPSRCTX,          tx_mode_settings['RAC.SYTRIM0.SYCHPLEVPSRCTX'][ind])
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPSRCENTX,            tx_mode_settings['RAC.SYTRIM0.SYCHPSRCENTX'][ind])

    def calc_sytrim0_sychpcurrtx_reg(self, model):
        # This function calculates the SYTRIM0_SYCHPCURRTX field

        #For now, always use value 5
        sychpcurrtx = 5

        # Write the register
        self._reg_write(model.vars.RAC_SYTRIM0_SYCHPCURRTX, sychpcurrtx)

    def calc_adc_clock_config(self, model):
        #This function calculates both the ADC mode (e.g. fullrate, halfrate, etc) as well as the ADC clock divider path

        #Load model values into local variables
        bandwidth_hz = model.vars.bandwidth_hz.value

        if(bandwidth_hz < 100e3):
            #1/8 rate mode, noise density is lowest in 50-100kHz range
            #Use the HFXO for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.EIGHTHRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.HFXOMULT
        elif(bandwidth_hz < 1.25e6):
            #1/2 rate mode
            #Use the HFXO along with DPLL for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.HALFRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.HFXOMULT
        else:
            #Full rate mode
            #Use the divided down VCO for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.FULLRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.HFXOMULT

        #Load local variables back into model variables
        model.vars.adc_clock_mode.value = adc_clock_mode
        model.vars.adc_rate_mode.value = adc_rate_mode

    def calc_adc_clockmode_reg(self,model):
        #This function handles writes to the registers impacting ADC clock mode
        adc_clock_mode = model.vars.adc_clock_mode.value

        if( model.vars.adc_clock_mode.var_enum.HFXOMULT == adc_clock_mode ):
            self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCCLKSEL, 1)
        else:
            self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCCLKSEL, 0)

    def calc_adc_clockmode_actual(self, model):
        #This function calculates the actual value of the adc clock mode based on the register value used
        ifadc_clk_sel = model.vars.RAC_IFADCTRIM0_IFADCCLKSEL.value

        if( 1 == ifadc_clk_sel ):
            model.vars.adc_clock_mode_actual.value = model.vars.adc_clock_mode.var_enum.HFXOMULT
        else:
            model.vars.adc_clock_mode_actual.value = model.vars.adc_clock_mode.var_enum.VCODIV

    def calc_adc_rate_mode_reg(self,model):
        #This function handles writes to the registers impacting ADC clock mode and HFXO multiplier
        adc_rate_mode = model.vars.adc_rate_mode.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if(model.vars.adc_rate_mode.var_enum.FULLRATE == adc_rate_mode):
            self._reg_write(model.vars.RAC_IFADCPLLDCO_IFADCPLLDCOFULLRATE, 1)
            self._reg_write(model.vars.RAC_IFADCPLLEN_IFADCPLLENXOBYP, 0)
        elif (model.vars.adc_rate_mode.var_enum.HALFRATE == adc_rate_mode):
            self._reg_write(model.vars.RAC_IFADCPLLDCO_IFADCPLLDCOFULLRATE, 0)
            self._reg_write(model.vars.RAC_IFADCPLLEN_IFADCPLLENXOBYP, 0)
        else:
            # Eigthrate
            self._reg_write(model.vars.RAC_IFADCPLLEN_IFADCPLLENXOBYP, 1)
            self._reg_write(model.vars.RAC_IFADCPLLDCO_IFADCPLLDCOFULLRATE, 0)

    # FIXME: do we have VCO based on clock option in Sol? if not remove that path
    def calc_adc_target_freq(self, model):
        #This function calculates the target sample frequency based on the ADC clock configuration

        #Load model variables into local variables
        adc_rate_mode = model.vars.adc_rate_mode.value #Use the target rate mode for now because we haven't yet chosen the exact VCODIV value
        adc_clock_mode = model.vars.adc_clock_mode.value
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value

        if (model.vars.adc_clock_mode.var_enum.HFXOMULT == adc_clock_mode):
            if (adc_rate_mode == model.vars.adc_rate_mode.var_enum.FULLRATE):
                fadc_target = 8 * xtal_frequency_hz
            elif(adc_rate_mode==model.vars.adc_rate_mode.var_enum.HALFRATE):
                fadc_target = 4 * xtal_frequency_hz
            else:
                fadc_target = xtal_frequency_hz
        else:
            #Clock mode is VCODIV
            if(adc_rate_mode==model.vars.adc_rate_mode.var_enum.HALFRATE):
                fadc_target = 152.5e6
            else:
                fadc_target = 305e6   #Target full rate

        #Load local variables back into model variables
        model.vars.adc_target_freq.value = int(fadc_target)

    def calc_lo_adc_dividers(self,model):
        #This function calculates A,B,C, ADC dividers

        #Load model variables into local variables
        adc_clock_mode = model.vars.adc_clock_mode_actual.value
        flo = model.vars.lo_target_freq.value #We don't know the LO frequency yet, this is based on the center freq and target IF
        fadc_target = model.vars.adc_target_freq.value #We don't know the ADC frequency yet

        #Define VCO pull range constants
        #fvcomin = 4450e6
        #fvcomax = 5950e6

        #These array variables store all valid divider sets
        # lodiv_codes[div-1] is the combined divider code for divider A, B, C
        #    div_m_1  [0,1,2,3,4,5,6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23]
        lodiv_codes = [1,2,3,4,5,6,7,20,27,21, 0,22, 0,23,29,36, 0,30, 0,37,31, 0, 0,38]
        fosc = []
        lodiv = []
        #Loop through all C,B,A divider possibilities
        for div_m_1 in range(24):
            fvco = flo * (2 * (div_m_1 + 1))
            if (lodiv_codes[div_m_1] > 0) and (fvco < self.fvcomax) and (fvco > self.fvcomin):
                #If this is a valid value for the VCO, add to the list of valid divider combinations
                lodiv.append(div_m_1+1)
                fosc.append(fvco)

        #Now go through all of the possible solutions and pick the best one
        if (len(fosc) == 0):
            raise CalculationException('ERROR: no valid solution for VCO frequency in calc_lo_adc_dividers()')

        else:
            # If the ADC is based directly on the HFXO, then don't worry about the ADC divider and pick the first valid LO divider set
            if(adc_clock_mode==model.vars.adc_clock_mode.var_enum.HFXOMULT):
                adcdivfinal = 0 #This is a don't care, but make it 1 so that any calculations using the value don't divide by zero
                lodivfinal = lodiv[0]
            else:
                # Find target freq for ADC from potential candidates
                # Start with some initial values
                fadcerr = 1e20
                adcdivfinal = 1
                lodivfinal = 1
                for index in range(0,len(fosc)):
                    adcdiv = int(round(fosc[index] / 2.0 / fadc_target))
                    fadc = fosc[index] / (2 * adcdiv)

                    error = abs(fadc - fadc_target)

                    if (error < fadcerr):
                        #If the current divider set yields the lowest ADC error, then make that the final for now
                        fadcerr = error
                        adcdivfinal = adcdiv
                        lodivfinal = lodiv[index]

        #Calculate the A,B,C dividers from lodiv_reg and its 3 fields
        lodiv_reg  = lodiv_codes[lodivfinal-1]

        #Load local variables back into model variables
        model.vars.adc_vco_div.value = adcdivfinal
        model.vars.lodiv.value = lodivfinal

        #Write register
        self._reg_write(model.vars.SYNTH_DIVCTRL_LODIVFREQCTRL, lodiv_reg)

        # calculate proper variable flag to determine sub-GHz
        model.vars.subgig_band.value = bool(model.vars.lodiv_actual.value > 1)

    def calc_lodividers_actual(self, model):

        lodiv_reg = model.vars.SYNTH_DIVCTRL_LODIVFREQCTRL.value

        a_divider = 7 & lodiv_reg
        b_divider = 7 & (lodiv_reg >> 3)
        if b_divider == 0:
            b_divider = 1
        c_divider = 7 & (lodiv_reg >> 6)
        if c_divider == 0:
            c_divider = 1

        model.vars.a_divider_actual.value = a_divider
        model.vars.b_divider_actual.value = b_divider
        model.vars.c_divider_actual.value = c_divider

        model.vars.lodiv_actual.value = a_divider * b_divider * c_divider

    def calc_adc_vco_div_reg(self,model):
        #This function writes the registers associated with the VCO Divider going to ADC
        #Only care about this register when in VCO mode in XO mode exceptions are ignored
        adc_vco_div = model.vars.adc_vco_div.value
        adc_clock_mode = model.vars.adc_clock_mode_actual.value

        if adc_vco_div < 8:
            adc_vco_div = 8
            if adc_clock_mode==model.vars.adc_clock_mode.var_enum.VCODIV:
                raise CalculationException('WARNING: ADC_VCO_DIV is less than 8 in calc_adc_vco_div_reg()')

        if adc_vco_div > 23:
            adc_vco_div = 23
            if adc_clock_mode == model.vars.adc_clock_mode.var_enum.VCODIV:
                raise CalculationException('WARNING: ADC_VCO_DIV is greater than 8 in calc_adc_vco_div_reg()')

        # 16 codes [0:15]. Code 0 = div8 and code 15 = div23
        self._reg_write(model.vars.RAC_SYDIVCTRL_SYLODIVRADCDIV, adc_vco_div-8)

    def calc_adc_vco_div_actual(self,model):
        #This function calculates the actual ADC divider based on the register value
        sy_lo_divr_adc_div = model.vars.RAC_SYDIVCTRL_SYLODIVRADCDIV.value

        model.vars.adc_vco_div_actual.value = sy_lo_divr_adc_div + 8

    def calc_adc_freq_actual(self,model):
        #This function calculates the actual ADC sample frequency and error based on the registers

        #Load model variables into local variables
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value
        adc_vco_div_actual = model.vars.adc_vco_div_actual.value
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        fsynth = model.vars.rx_synth_freq_actual.value
        fadc_target = model.vars.adc_target_freq.value #The target frequency is used for computing error
        ifadcpll_dco_fullrate = model.vars.RAC_IFADCPLLDCO_IFADCPLLDCOFULLRATE.value
        ifadcpll_en_xo_bypass = model.vars.RAC_IFADCPLLEN_IFADCPLLENXOBYP.value

        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            #the ordering of the if statements is important - keep ifadcpll_en_xo_bypass on top
            if 1 == ifadcpll_en_xo_bypass:
                adc_freq_actual = xtal_frequency_hz
            elif 1 == ifadcpll_dco_fullrate:
                adc_freq_actual = xtal_frequency_hz * 8
            else:
                adc_freq_actual = xtal_frequency_hz * 4
        else:
            adc_freq_actual = int(fsynth / adc_vco_div_actual)

        # Compute the final ADC frequency percent error
        ferror = 100 * (fadc_target - adc_freq_actual) / float(fadc_target)

        #Load local variables back into model variables
        model.vars.adc_freq_actual.value = adc_freq_actual
        model.vars.adc_freq_error.value = ferror

    def calc_if_frequency_target(self,model):
        #This function calculates the target IF frequency

        #The lower bound for signal content is determined by the lowest usable frequency range of the ADC (DC filtering produces noise near DC, varies based on sample rate)
        #This upper bound for signal content is determined by the highest usable frequency range of the ADC (noise shaping, varies based on sample rate)

        #Load model variables into local variables
        bandwidth_hz = model.vars.bandwidth_hz.value
        adc_rate_mode = model.vars.adc_rate_mode.value #We can't use the actual rate mode, because the IF goes into calculating the actual VCO, which goes into final ADC divider

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.EIGHTHRATE:
            bandwidth_adc_hz = 150e3
            band_edge_min = 50e3
        elif adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            bandwidth_adc_hz = 1.25e6
            band_edge_min = 50e3
        else:
            bandwidth_adc_hz = 2.50e6
            band_edge_min = 100e3

        if_frequency_min = 80e3
        if_frequency_hz = max(if_frequency_min, band_edge_min + bandwidth_hz / 2)

        if ( (if_frequency_hz + bandwidth_hz / 2) > bandwidth_adc_hz):
            LogMgr.Warning("WARNING: IF + BW/2 > ADC Bandwidth")

        #Load local variables back into model variables
        model.vars.if_frequency_hz.value = int(if_frequency_hz)

    def calc_iffreq_reg(self, model):
        #This calculation writes the IF frequency to the register

        #Load model variables into local variables
        if_frequency_hz = model.vars.if_frequency_hz.value
        synth_res_actual = model.vars.synth_res_actual.value

        iffreq_reg = int(if_frequency_hz / synth_res_actual)

        #Load local variables back into model variables
        self._reg_write(model.vars.SYNTH_IFFREQ_IFFREQ, iffreq_reg)


    def calc_lodiv_value(self, model):
        #This calculation is now in the calc_demodulator file as it also computes the ADC divider
        pass

    def calc_lo_target_freq(self,model):
        #This function calculates the target LO frequency baed on RF, IF, and injection side

        #Load model variables into local variables
        lo_injection_side = model.vars.lo_injection_side.value
        rf_freq = model.vars.base_frequency_hz.value
        if_freq = model.vars.if_frequency_hz.value #We don't yet know the actual synth frequency to get the true IF

        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            lo_freq = rf_freq + if_freq
        else:
            lo_freq = rf_freq - if_freq

        #Load local variables back into model variables
        model.vars.lo_target_freq.value = lo_freq


    def calc_chsp_freq_reg(self, model):
        #This function was ported due to a variable name mismatch

        ch_spacing = model.vars.channel_spacing_hz.value * 1.0
        f0 = model.vars.base_frequency_hz.value * 1.0
        res = model.vars.synth_res_actual.value

        #channel spacing in terms of res
        chsp = py2round(ch_spacing / res)

        self._reg_write(model.vars.SYNTH_CHSP_CHSP, int(chsp))

        # frequency in terms of res
        freq = math.floor(f0 / res)

        self._reg_write(model.vars.SYNTH_FREQ_FREQ, long(freq))

    def calc_synth_res_actual(self, model):
        #This function was ported due to a variable name mismatch

        xtal_frequency_hz = model.vars.xtal_frequency_hz.value * 1.0
        lodiv = model.vars.lodiv_actual.value

        # Calculate frequency resolution
        # Correct reference clock to use here is the xtal
        res = xtal_frequency_hz / lodiv / pow(2, 19)

        model.vars.synth_res_actual.value = res

    def calc_check_synth_limits(self,model):
        #Overriding limit check from Common
        #TODO: add in limits if we want to check on Ocelot
        pass

    def calc_rx_synth_freq_actual(self, model):
        """
        calculate synthesizer frequency for RX
        Equation (5.31) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        chan0_freq = model.vars.SYNTH_FREQ_FREQ.value
        chno = 0
        chan_spacing = model.vars.SYNTH_CHSP_CHSP.value
        cal_offset = 0
        if_freq = model.vars.SYNTH_IFFREQ_IFFREQ.value
        loside = model.vars.SYNTH_IFFREQ_LOSIDE.value
        res = model.vars.synth_res_actual.value
        lodiv = model.vars.lodiv_actual.value

        if loside:
            rx_synth_freq = (chan0_freq + chno * chan_spacing + cal_offset + if_freq) * res * lodiv
        else:
            rx_synth_freq = (chan0_freq + chno * chan_spacing + cal_offset - if_freq) * res * lodiv

        model.vars.rx_synth_freq_actual.value = long(round(rx_synth_freq))

    #FIXME: do we want to create another file called calc_rac.py and place this function there?
    def calc_pga_lna_bw_reg(self, model):

        # Notes
        # Panther used max if_bandwidth_hz = 2.5MHz for BW3dB = 5MHz
        # This gave 1dB droop for mixer pole response and 1dB droop for pga pole response (2dB total).
        # For Ocelot, the initial approach is to scale this to 4 ranges for the 4 filter options
        # 0:  BW3dB = 5 MHz -> max IF BW = 2.5 MHz
        # 1:  BW3dB = 2.5 MHz -> max IF BW = 1.25 MHz
        # 2:  BW3dB = 1.25 MHz -> max IF BW = 0.625 MHz
        # 3:  BW3dB = 0.625 MHz -> max IF BW = 0.3125 MHz

        if_frequency_hz = model.vars.if_frequency_hz.value
        bandwidth_hz = model.vars.bandwidth_hz.value
        if_bandwidth_hz = if_frequency_hz + bandwidth_hz / 2.0

        # Initial Ocelot approach:  same BW3dB for both poles, H(f) > -2dB at IF bandwidth edge
        # Adjusted thresholds by -15% for -2dB frequency based on Ocelot Silicon measurements
        if if_bandwidth_hz < 0.260e6:
            # eighth-rate modes
            # LNABWADJ = 15 for BW3dB = 0.625 MHz
            # PGABWMODE = 2 for BW3dB = 0.625 MHz
            mixer_bw_sel = 15
            pga_bw_sel = 2
        elif if_bandwidth_hz < 0.85*0.625e6:
            # half-rate modes
            # LNABWADJ = 7 for BW3dB = 1.25 MHz
            # PGABWMODE = 3 for BW3dB = 1.25 MHz
            mixer_bw_sel = 7
            pga_bw_sel = 3
        elif if_bandwidth_hz < 0.85*1.25e6:
            # half-rate modes
            # LNABWADJ = 3 for BW3dB = 2.5 MHz
            # PGABWMODE = 1 for BW3dB = 2.5 MHz
            mixer_bw_sel = 3
            pga_bw_sel = 1
        else:
            # full-rate modes
            # LNABWADJ = 0 for BW3dB = 2.5 MHz
            # PGABWMODE = 0 for BW3dB = 2.5 MHz
            mixer_bw_sel = 0
            pga_bw_sel = 0

        #For now, set mixer_bw_sel_boost to the same value as mixer_bw_sel (MCUW_RADIO_CFG-1174)
        mixer_bw_sel_boost = mixer_bw_sel

        self._reg_write(model.vars.RAC_PGACTRL_PGABWMODE, pga_bw_sel)
        self._reg_write(model.vars.AGC_LNABOOST_LNABWADJ, mixer_bw_sel)
        self._reg_write(model.vars.AGC_LNABOOST_LNABWADJBOOST, mixer_bw_sel_boost)

    def calc_boostlna_reg(self, model):

        # Calculating the boostlna value
        # https://jira.silabs.com/browse/MCUW_RADIO_CFG-1612 always set to 0
        boostlna = 0

        #Write the register
        self._reg_write(model.vars.AGC_LNABOOST_BOOSTLNA, boostlna)

    def calc_lodiv_actual(self, model):
        # Don't want to calculate this for Ocelot
        pass

    def calc_ifadcplldcofilter_reg(self, model):

        adc_rate_mode = model.vars.adc_rate_mode.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.FULLRATE and \
           adc_clock_mode == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_IFADCPLLDCO_IFADCPLLDCOFILTER, reg)


    def calc_ifadcpolldcobiashalf_reg(self, model):
        adc_rate_mode = model.vars.adc_rate_mode.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE and \
                adc_clock_mode == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_IFADCPLLDCO_IFADCPLLDCOBIASHALF, reg)

    def calc_ifadcenhalfmode_reg(self, model):

        adc_rate_mode = model.vars.adc_rate_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE, reg)

    def calc_ifadctrim1_reg(self, model):
        adc_rate_mode = model.vars.adc_rate_mode.value
        adc_clock_mode = model.vars.adc_clock_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.EIGHTHRATE and \
           adc_clock_mode == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            reg = 1
            ifadctz = 4
            negres = 0
        else:
            reg = 0
            ifadctz = 1
            negres = 1

        self._reg_write(model.vars.RAC_IFADCTRIM1_IFADCENSUBGMODE, reg)
        self._reg_write(model.vars.RAC_IFADCTRIM1_IFADCENXOBYPASS, reg)
        self._reg_write(model.vars.RAC_IFADCTRIM1_IFADCTZ, ifadctz)
        self._reg_write(model.vars.RAC_IFADCTRIM1_IFADCENNEGRES, negres)

    def calc_pulsepairing_reg(self, model):
        fsynth = model.vars.rx_synth_freq_actual.value # fsynth =  fvco / 2
        fif = model.vars.if_frequency_hz_actual.value
        lo_injection_side = model.vars.lo_injection_side.value
        lodiv = model.vars.lodiv_actual.value
        if lo_injection_side == model.vars.lo_injection_side.var_enum.LOW_SIDE:
            fif = - fif

        model.vars.ppnd_0.value = 0
        model.vars.ppnd_1.value = 0
        model.vars.ppnd_2.value = 0
        model.vars.ppnd_3.value = 0
        if (lodiv >= 5):
            # only allow DCDC retiming for low frequency band
            # calculate 4 freqs equaly spaced over synth range
            # divide by 2 is to convert vco range to synth range
            fvcorange = self.fvcomax - self.fvcomin
            fvcostep = fvcorange / 4.0
            fsynth_list = np.arange(self.fvcomin+fvcostep/2,self.fvcomax-fvcostep/4,fvcostep)/2

            model.vars.ppnd_0.value = self.return_ppnd(fsynth_list[0], fif, lodiv)
            model.vars.ppnd_1.value = self.return_ppnd(fsynth_list[1], fif, lodiv)
            model.vars.ppnd_2.value = self.return_ppnd(fsynth_list[2], fif, lodiv)
            model.vars.ppnd_3.value = self.return_ppnd(fsynth_list[3], fif, lodiv)

    def return_ppnd(self, fsynth, fif, lodiv):

        dcdcdiv = 13 # assuming constant 13 for now
        Tdmin = 0.8e-6
        Tdmax = 0.95e-6

        Ndmin = int(math.ceil(Tdmin * fsynth / dcdcdiv))
        Ndmax = int(math.floor(Tdmax * fsynth / dcdcdiv))

        best_error = 9e9
        for Nd in range(Ndmin, Ndmax + 1):
            val = Nd * dcdcdiv * (1 / lodiv - fif / fsynth) + 0.5
            error = abs(val - round(val))
            if error < best_error:
                best_error = error
                best_Nd = Nd

        ppnd = best_Nd if best_Nd < 512 else 511

        return (ppnd - 1)

    def calc_synth_misc(self, model):

        #Setting these regs to static values
        self._reg_write(model.vars.RAC_SYNTHENCTRL_MMDPOWERBALANCEDISABLE, 0)

        # Setting these values to POR
        self._reg_write_default(model.vars.SYNTH_LPFCTRL2TX_VCMLVLTX)
        self._reg_write_default(model.vars.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM)

        # Fairly confident that these CAL values are not actually used, but Design can not confirm these are unused.
        # So if the value isn't forced then these are a do not care.
        if model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_OP1BWCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_OP1COMPCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_RZVALCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_RPVALCAL)
        if model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL.value_forced is None:
            self._reg_do_not_care(model.vars.SYNTH_LPFCTRL1CAL_RFBVALCAL)

    def calc_hfxo_retiming_table(self, model):
        # Initialize Table
        model.vars.lut_table_index.value = [0]
        model.vars.lut_freq.value = []
        model.vars.lut_freq_upper.value = []
        model.vars.lut_valid.value = []
        model.vars.lut_smuxdiv.value = []
        model.vars.lut_limitl.value = []
        model.vars.lut_limith.value = []
        model.vars.lut_dpll_freq_hz.value = []

        #setting for 1x HFXO frequency
        self.retime_print("calculating 1x HFXO freq")
        self.retime_main(model, model.vars.xtal_frequency_hz.value, 0, 0)

    def calc_txramp_txmodephaseflip_reg(self, model):
        #This function writes the TXMODEPHASEFLIP reg field

        #Read in model variables
        modulation_type = model.vars.modulation_type.value

        #Only set to 1 for DBPSK and BPSK
        if modulation_type == model.vars.modulation_type.var_enum.DBPSK or \
                modulation_type == model.vars.modulation_type.var_enum.BPSK:
            txmodephaseflip = 1
        else:
            txmodephaseflip = 0

        #Write the register
        self._reg_write(model.vars.RAC_TXRAMP_TXMODEPHASEFLIP, txmodephaseflip)