"""Core CALC_Radio Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
#import math
from enum import Enum
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

class CALC_Radio(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Output fields
        self._addModelRegister(model, 'RAC.IFPGACTRL.BANDSEL'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.VLDO'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.CASCBIAS'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.TRIMVCASLDO'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.TRIMVCM'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.TRIMVREFLDO'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.TRIMVREGMIN'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.ENHYST'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFPGACTRL.ENOFFD'           , int, ModelVariableFormat.HEX )
                                                                             
        self._addModelRegister(model, 'RAC.IFADCCTRL.REALMODE'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.VLDOCLKGEN'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.REGENCLKDELAY'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.INPUTSCALE'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.OTA1CURRENT'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.OTA2CURRENT'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.OTA3CURRENT'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.VCM'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.VLDOSERIES'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.VLDOSERIESCURR'   , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.VLDOSHUNT'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFADCCTRL.INVERTCLK'        , int, ModelVariableFormat.HEX )
        
        self._addModelRegister(model, 'RAC.IFFILTCTRL.BANDWIDTH'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFFILTCTRL.CENTFREQ'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFFILTCTRL.VCM'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.IFFILTCTRL.VREG'            , int, ModelVariableFormat.HEX )
        
        self._addModelRegister(model, 'RAC.RFENCTRL.DEMEN'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.RFENCTRL.IFADCCAPRESET'     , int, ModelVariableFormat.HEX )


        #self._addModelVariable(model,  'synth_lpfbw',  int,          ModelVariableFormat.DECIMAL)


        var = self._addModelVariable(model, 'pll_bandwidth_rx', Enum, ModelVariableFormat.DECIMAL, 'PLL bandwidth in KHz in RX')
        if model.part_family.lower() in ["dumbo", "jumbo", "nerio", "nixi", "unit_test_part"]:
            member_data = [
                ['BW_250KHz', 0,  'PLL loop filter bandwidth is approximately 250 KHz'],
                ['BW_297KHz', 1,  'PLL loop filter bandwidth is approximately 297 KHz'],
                ['BW_354KHz', 2,  'PLL loop filter bandwidth is approximately 354 KHz'],
                ['BW_420KHz', 3,  'PLL loop filter bandwidth is approximately 420 KHz'],
                ['BW_500KHz', 6,  'PLL loop filter bandwidth is approximately 500 KHz'],
                ['BW_594KHz', 7,  'PLL loop filter bandwidth is approximately 594 KHz'],
                ['BW_1260KHz', 8, 'PLL loop filter bandwidth is approximately 1260 KHz'],
                ['BW_1500KHz', 9, 'PLL loop filter bandwidth is approximately 1500 KHz'],
                ['BW_1780KHz', 10, 'PLL loop filter bandwidth is approximately 1780 KHz'],
                ['BW_2120KHz', 11, 'PLL loop filter bandwidth is approximately 2120 KHz'],
                ['BW_2520KHz', 14, 'PLL loop filter bandwidth is approximately 2520 KHz'],
                ['BW_3000KHz', 15, 'PLL loop filter bandwidth is approximately 3000 KHz'],
            ]
        else:
            member_data = [
                ['BW_100KHz', 0, 'PLL loop filter bandwidth is approximately 100 KHz'],
                ['BW_150KHz', 1, 'PLL loop filter bandwidth is approximately 150 KHz'],
                ['BW_200KHz', 2, 'PLL loop filter bandwidth is approximately 200 KHz'],
                ['BW_250KHz', 3, 'PLL loop filter bandwidth is approximately 250 KHz'],
                ['BW_300KHz', 6, 'PLL loop filter bandwidth is approximately 300 KHz'],
            ]

        var.var_enum = CreateModelVariableEnum(
            'PLLBandwdithEnum',
            'List of supported PLL bandwidth settings',
            member_data)

        var = self._addModelVariable(model, 'pll_bandwidth_tx', Enum, ModelVariableFormat.DECIMAL, 'PLL bandwidth in KHz in TX')
        if model.part_family.lower() in ["dumbo", "jumbo", "nerio", "nixi", "unit_test_part"]:
            member_data = [
                ['BW_250KHz', 0,  'PLL loop filter bandwidth is approximately 250 KHz'],
                ['BW_297KHz', 1,  'PLL loop filter bandwidth is approximately 297 KHz'],
                ['BW_354KHz', 2,  'PLL loop filter bandwidth is approximately 354 KHz'],
                ['BW_420KHz', 3,  'PLL loop filter bandwidth is approximately 420 KHz'],
                ['BW_500KHz', 6,  'PLL loop filter bandwidth is approximately 500 KHz'],
                ['BW_594KHz', 7,  'PLL loop filter bandwidth is approximately 594 KHz'],
                ['BW_1260KHz', 8, 'PLL loop filter bandwidth is approximately 1260 KHz'],
                ['BW_1500KHz', 9, 'PLL loop filter bandwidth is approximately 1500 KHz'],
                ['BW_1780KHz', 10, 'PLL loop filter bandwidth is approximately 1780 KHz'],
                ['BW_2120KHz', 11, 'PLL loop filter bandwidth is approximately 2120 KHz'],
                ['BW_2520KHz', 14, 'PLL loop filter bandwidth is approximately 2520 KHz'],
                ['BW_3000KHz', 15, 'PLL loop filter bandwidth is approximately 3000 KHz'],
            ]
        else:
            member_data = [
                ['BW_750KHz',  0, 'PLL loop filter bandwidth is approximately 750 KHz'],
                ['BW_1000KHz', 1, 'PLL loop filter bandwidth is approximately 1000 KHz'],
                ['BW_1200KHz', 2, 'PLL loop filter bandwidth is approximately 1200 KHz'],
                ['BW_1500KHz', 3, 'PLL loop filter bandwidth is approximately 1500 KHz'],
                ['BW_2000KHz', 6, 'PLL loop filter bandwidth is approximately 2000 KHz'],
                ['BW_2500KHz', 7, 'PLL loop filter bandwidth is approximately 2500 KHz'],
                ['BW_3000KHz', 8, 'PLL loop filter bandwidth is approximately 3000 KHz'],
            ]

        var.var_enum = CreateModelVariableEnum(
            'PLLBandwdithEnum',
            'List of supported PLL bandwidth settings',
            member_data)

        self._addModelVariable(model, 'pll_bandwidth_miracle_mode', bool, ModelVariableFormat.ASCII, 'Set to force the synth pll into miracle mode (whatever that means).')

        # Don't write this register directly any more.  Write the version in sequencer code instead.
        #self._addModelRegister(model, 'RAC.LPFCTRL.LPFBW'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SEQ.SYNTHLPFCTRLRX.SYNTHLPFCTRLRX' , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SEQ.SYNTHLPFCTRLTX.SYNTHLPFCTRLTX' , int, ModelVariableFormat.HEX )

        # Misc fields that were being written by the firmware to non-default values
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.TRIMAUXPLLCLK'  , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.TRIMTRSWGATEV'  , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.TRIMVCASLDO'    , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.TRIMVREFLDO'    , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.TRIMVREGMIN'    , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.TRIMAUXBIAS'    , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.LNAMIXCTRL1.ENBIASCAL'      , int, ModelVariableFormat.HEX )   

        self._addModelRegister(model, 'RAC.VCOCTRL.VCOAMPLITUDE'       , int, ModelVariableFormat.HEX )   
        if model.part_family.lower() in ["dumbo", "jumbo", "nerio", "nixi", "panther", "lynx", "leopard", "unit_test_part"]:
            self._addModelRegister(model, 'RAC.VCOCTRL.VCODETAMPLITUDE'    , int, ModelVariableFormat.HEX )
        elif model.part_family.lower() not in ['bobcat']:
            self._addModelRegister(model, 'RAC.VCOCTRL.VCODETAMPLITUDERX'    , int, ModelVariableFormat.HEX )   
            self._addModelRegister(model, 'RAC.VCOCTRL.VCODETAMPLITUDETX'    , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.VCOCTRL.VCODETEN'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.VCOCTRL.VCODETMODE'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'RAC.VCOCTRL.VCOAREGCURR'        , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.VCOCTRL.VCOCREGCURR'        , int, ModelVariableFormat.HEX )   
        self._addModelRegister(model, 'RAC.VCOCTRL.VCODIVCURR'         , int, ModelVariableFormat.HEX )   

        self._addModelRegister(model, 'SYNTH.CTRL.DITHERDSMOUTPUT'     , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.DITHERDAC'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.DITHERDSMINPUT'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.DSMMODE'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.LSBFORCE'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.LOCKTHRESHOLD'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.AUXLOCKTHRESHOLD'    , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.PRSMUX0'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SYNTH.CTRL.PRSMUX1'             , int, ModelVariableFormat.HEX )
        #self._addModelRegister(model, 'SYNTH.CTRL.TRISTATEPOSTPONE'    , int, ModelVariableFormat.HEX )
        #self._addModelRegister(model, 'SYNTH.CTRL.INTEGERMODE'         , int, ModelVariableFormat.HEX )
        #self._addModelRegister(model, 'SYNTH.CTRL.MMDSCANTESTEN'       , int, ModelVariableFormat.HEX )
        

    # table from section 10.0 in rx_iffilt_stop.docx
    # maps register enum to IFFILT bandwidth and IF frequency
    iffilt_bw = OrderedDict()
    iffilt_bw[0] = 570000
    iffilt_bw[1] = 630000
    iffilt_bw[2] = 790000
    iffilt_bw[3] = 890000
    iffilt_bw[4] = 1040000
    iffilt_bw[5] = 1140000
    iffilt_bw[6] = 1250000
    iffilt_bw[7] = 1400000
    iffilt_bw[8] = 1585000
    iffilt_bw[9] = 1660000
    iffilt_bw[10] = 1820000
    iffilt_bw[11] = 2140000
    iffilt_bw[12] = 2280000
    iffilt_bw[13] = 2600000
    iffilt_bw[14] = 2800000
    iffilt_bw[15] = 3040000

    iffilt_ratio = OrderedDict()
    iffilt_ratio[0] = 0.0
    iffilt_ratio[1] = 0.385
    iffilt_ratio[2] = 0.5
    iffilt_ratio[3] = 0.55
    iffilt_ratio[4] = 0.6
    iffilt_ratio[5] = 0.65
    iffilt_ratio[6] = 0.675
    iffilt_ratio[7] = 0.7


    #TODO: combine IFFILT bw and ratio calculations to ensure that
    #  ratio*bw is close to fc that we already determined
    # or at least the IF filter covers the digital filter span
    def calc_iffilt_bw_reg(self, model):
        """
        find smallest IFFILT bandwidth we can use with given digital filter
        bandwidth settings

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        bw_dig = model.vars.bandwidth_actual.value

        # find smallest bandwidth setting that is larger than bw_dig
        for bw_reg, bw_ana in self.iffilt_bw.items():
            if bw_ana > bw_dig:
                break

        self._reg_write(model.vars.RAC_IFFILTCTRL_BANDWIDTH,  bw_reg)


    def calc_iffilt_bw_actual(self, model):
        """
        given register setting return IFFILT bandwidth

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.iffilt_bandwidth_actual.value = self.iffilt_bw[model.vars.RAC_IFFILTCTRL_BANDWIDTH.value]


    def calc_iffilt_ratio_value(self, model):
        """
        given already decided center (IF) frequency and IFFILT bandwidth
        find ratio value that would center the IFFILT around the IF frequency.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fc = model.vars.if_frequency_hz_actual.value
        bw_ana = model.vars.iffilt_bandwidth_actual.value

        # calculate IF (center) frequency to BW ratio
        target_ratio = float(fc) / bw_ana

        model.vars.iffilt_ratio.value = target_ratio


    def calc_iffilt_ratio_reg(self, model):
        """
        Given desired ratio find best register setting

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        target_ratio = model.vars.iffilt_ratio.value

        # find smallest ratio that is larger than the target ratio
        best_error = 9e9
        for index, ratio in self.iffilt_ratio.items():
            error = abs(target_ratio - ratio)
            if error < best_error:
                best_error = error
                best_index = index

        self._reg_write(model.vars.RAC_IFFILTCTRL_CENTFREQ,  best_index)


    def calc_iffilt_ratio_actual(self, model):
        """
        given register setting return actual ratio used in the PHY

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.iffilt_ratio_actual.value = self.iffilt_ratio[model.vars.RAC_IFFILTCTRL_CENTFREQ.value]


    def calc_bandsel_reg(self, model):
        """
        given LODIV setting set BANDSEL register
        for 2.4 GHz LODIV is always set to 1 so if LODIV is one set BANDSEL to 2P4G band (0)
        else set to SUBG mode (1)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        if (model.vars.lodiv_actual.value == 1):
            self._reg_write(model.vars.RAC_IFPGACTRL_BANDSEL, 0)
        else:
            self._reg_write(model.vars.RAC_IFPGACTRL_BANDSEL, 1)


    def calc_realmode_reg(self, model):
        """
        Given channel bandwidth choose between real and complex ADC modes

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        bw = model.vars.bandwidth_actual.value

        # discovered that if RX signal fits into the ADC bandwidth we get better
        # performance with real mode. So check if channel bandwidth is less than
        # 1 MHz and choose real mode else choose default complex mode.
        if bw < 1e6:
            realmode = 1   # set ADC in real-mode
        else:
            realmode = 0

        self._reg_write(model.vars.RAC_IFADCCTRL_REALMODE, realmode)
        
        
    def calc_lpfbwtx(self, model):
        """
        calculate frequency synthesizer's LPF bandwidth for TX and write to register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # Due to a HW problem with PLL overshooting choose target BW of frequency synthesize
        # LPF as 1.5 times the channel bandwidth chosen.
        target_bw_khz = 1.5 * model.vars.bandwidth_actual.value / 1000.0

        if target_bw_khz < 250:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_250KHz
        elif target_bw_khz < 297:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_297KHz
        elif target_bw_khz < 354:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_354KHz
        elif target_bw_khz < 420:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_420KHz
        elif target_bw_khz < 500:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_500KHz
        elif target_bw_khz < 594:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_594KHz
        elif target_bw_khz < 1260:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1260KHz
        elif target_bw_khz < 1500:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1500KHz
        elif target_bw_khz < 1780:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1780KHz
        elif target_bw_khz < 2120:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2120KHz
        elif target_bw_khz < 2520:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_2520KHz
        else:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_3000KHz

        # anything smaller than 2 is not performing well at this point
        # use 2 as the smallest setting for now based on feedback from Ravi
        if model.vars.pll_bandwidth_tx.value.value < 2:
            model.vars.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_354KHz


    def calc_lpfbwrx(self, model):
        """
        calculate frequency synthesizer's LPF bandwidth for TX and write to register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # John Wang recommends that we use the LPFBW=2 setting in the RX mode to improve the phase noise.
        # The phase noise fundamentally limits the ACS (if the RX channel filter is not a limitation in itself).
        baudrate_hz = model.vars.baudrate.value

        # based on Ravi's feedback we should do this only for PHYs with datarate < 100kbps
        if baudrate_hz < 100e3:
            model.vars.pll_bandwidth_rx.value = model.vars.pll_bandwidth_rx.var_enum.BW_354KHz
        else:
            model.vars.pll_bandwidth_rx.value = model.vars.pll_bandwidth_rx.var_enum.BW_250KHz


    def calc_reg_lpfbwtx_lpfbwrx(self, model):
        """
        Write the rx and tx sequencer registers for the SYNTH_LPFCTRL register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        pll_bandwidth_tx = model.vars.pll_bandwidth_tx.value.value
        pll_bandwidth_rx = model.vars.pll_bandwidth_rx.value.value
        # We need to set some bits in the upper part of SYNTHLPFCTRL
        # Since we don't have proper fields defined for them in the 
        # sequencer register, just hard-code a constant... at least for today
        #
        # This is really setting:
        #
        # bit 17:       LPFGNDSWITCHINGEN = 1   - LPF input ground switching enable
        # bit 16:       LPFSWITCHINGEN    = 1   - Enables switching in the LPF
        # bits 15:14    LPFINPUTCAP       = 0x3 - Controls the input capacitance of the LPF
        #
        synthlpfctrl_upper_bits = 0x3c000

        #
        # We also conditionally set bit 4, the miracle mode bit.  Usually this is
        # cleared, but it can be forced to a 1.  So set the global variable to
        # 0, then read it back.  The readback will be a 1 if it was forced to a 1,
        # and otherwise it will get set to 0.
        #
        model.vars.pll_bandwidth_miracle_mode.value = False
        if model.vars.pll_bandwidth_miracle_mode.value == True:
            synthlpfctrl_upper_bits |= 0x10

        synthlpfctrlrx = synthlpfctrl_upper_bits | pll_bandwidth_rx
        synthlpfctrltx = synthlpfctrl_upper_bits | pll_bandwidth_tx
        
        self._reg_write(model.vars.SEQ_SYNTHLPFCTRLRX_SYNTHLPFCTRLRX,  synthlpfctrlrx)
        self._reg_write(model.vars.SEQ_SYNTHLPFCTRLTX_SYNTHLPFCTRLTX,  synthlpfctrltx)


    def calc_reg_ifadcctrl(self, model):
        """
        gdc:  Set based on request from Euisoo:  https://jira.silabs.com/browse/LABATEPDB-36

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.RAC_IFADCCTRL_VLDOCLKGEN,          3)
        self._reg_write(model.vars.RAC_IFADCCTRL_REGENCLKDELAY,       4)

        self._reg_write(model.vars.RAC_IFADCCTRL_INPUTSCALE,          0)
        self._reg_write(model.vars.RAC_IFADCCTRL_OTA1CURRENT,         2)
        self._reg_write(model.vars.RAC_IFADCCTRL_OTA2CURRENT,         2)
        self._reg_write(model.vars.RAC_IFADCCTRL_OTA3CURRENT,         2)
        self._reg_write(model.vars.RAC_IFADCCTRL_VCM,                 3)
        self._reg_write(model.vars.RAC_IFADCCTRL_VLDOSERIES,          3)
        self._reg_write(model.vars.RAC_IFADCCTRL_VLDOSERIESCURR,      3)
        self._reg_write(model.vars.RAC_IFADCCTRL_VLDOSHUNT,           2)
        self._reg_write(model.vars.RAC_IFADCCTRL_INVERTCLK,           0)
        
        

    def calc_reg_ifpgactrl(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # The following will be written with constants from Ravi's spreadsheet
        self._reg_write(model.vars.RAC_IFPGACTRL_VLDO,          3)
        self._reg_write(model.vars.RAC_IFPGACTRL_CASCBIAS,      7)
        self._reg_write(model.vars.RAC_IFPGACTRL_TRIMVCASLDO,   1)
        self._reg_write(model.vars.RAC_IFPGACTRL_TRIMVCM,       3)
        self._reg_write(model.vars.RAC_IFPGACTRL_TRIMVREFLDO,   0)
        self._reg_write(model.vars.RAC_IFPGACTRL_TRIMVREGMIN,   1)
        self._reg_write(model.vars.RAC_IFPGACTRL_ENHYST,        0)
        self._reg_write(model.vars.RAC_IFPGACTRL_ENOFFD,        0)

    def calc_reg_iffiltctrl(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.RAC_IFFILTCTRL_VCM,          2)
        self._reg_write(model.vars.RAC_IFFILTCTRL_VREG,         4)
    

    def calc_demen(self, model):
        """
        DEM stands for dynamic element matching.  Has nothing to do with demod.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.RAC_RFENCTRL_DEMEN,        1)

    def calc_reg_lnamixctrl1(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_TRIMAUXPLLCLK,  0)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_TRIMTRSWGATEV,  0)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_TRIMVCASLDO,    1)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_TRIMVREFLDO,    0)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_TRIMVREGMIN,    1)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_TRIMAUXBIAS,    0)
        self._reg_write(model.vars.RAC_LNAMIXCTRL1_ENBIASCAL,      0)

    def calc_reg_vcoctrl(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.RAC_VCOCTRL_VCOAMPLITUDE,      10)
        self._reg_write(model.vars.RAC_VCOCTRL_VCODETAMPLITUDE,    7)
        self._reg_write(model.vars.RAC_VCOCTRL_VCODETEN,           1)
        self._reg_write(model.vars.RAC_VCOCTRL_VCODETMODE,         1)
        self._reg_write(model.vars.RAC_VCOCTRL_VCOAREGCURR,        1)
        self._reg_write(model.vars.RAC_VCOCTRL_VCOCREGCURR,        2)
        self._reg_write(model.vars.RAC_VCOCTRL_VCODIVCURR,        15)

    def calc_reg_synthctrl(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        self._reg_write(model.vars.SYNTH_CTRL_DITHERDSMOUTPUT,     7)
        self._reg_write(model.vars.SYNTH_CTRL_DITHERDAC,           3)     
        self._reg_write(model.vars.SYNTH_CTRL_DITHERDSMINPUT,      1)
        self._reg_write(model.vars.SYNTH_CTRL_DSMMODE,             0)
        self._reg_write(model.vars.SYNTH_CTRL_LSBFORCE,            0)
        self._reg_write(model.vars.SYNTH_CTRL_LOCKTHRESHOLD,       3)    
        self._reg_write(model.vars.SYNTH_CTRL_AUXLOCKTHRESHOLD,    5)    
        self._reg_write(model.vars.SYNTH_CTRL_PRSMUX0,             0)      
        self._reg_write(model.vars.SYNTH_CTRL_PRSMUX1,             0)    
        #self._reg_write(model.vars.SYNTH_CTRL_TRISTATEPOSTPONE,    0)   
        #self._reg_write(model.vars.SYNTH_CTRL_INTEGERMODE,         0)   
        #self._reg_write(model.vars.SYNTH_CTRL_MMDSCANTESTEN,       0)   

    def calc_analog_misc(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        # Set this to a 1 just to match the register default
        # It really is a don't care, but it makes sense to set it to 1
        # to make some diff's simpler
        self._reg_write(model.vars.RAC_RFENCTRL_IFADCCAPRESET, 1)

