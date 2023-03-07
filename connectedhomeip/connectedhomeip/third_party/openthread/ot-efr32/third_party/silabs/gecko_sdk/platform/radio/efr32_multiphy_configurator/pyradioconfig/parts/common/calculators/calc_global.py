"""Core AGC Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from enum import Enum

from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum

from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat

from py_2_and_3_compatibility import *

class CALC_Global(ICalculator):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

    """
    Populates a list of needed variables for this calculator
    """
    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Inputs
        self._add_modulation_type_variable(model)
        self._add_bitrate_variable(model)
        self._addModelVariable(model,    'xtal_frequency_hz', int,        ModelVariableFormat.DECIMAL, units='Hz',   desc='EFR32 crystal clock frequency.')
        self._addModelVariable(model,    'rx_xtal_error_ppm', int,        ModelVariableFormat.DECIMAL, units='ppm',  desc='Crystal clock tolerance of receiver.')
        self._addModelVariable(model,    'tx_xtal_error_ppm',  int,       ModelVariableFormat.DECIMAL, units='ppm',  desc='Crystal clock tolerance of the transmitter.')
        self._addModelVariable(model,    'deviation',  int,               ModelVariableFormat.DECIMAL, units='Hz',   desc='Frequency deviation used in FSK modulation schemes')
        self._addModelVariable(model,    'base_frequency_hz', long,       ModelVariableFormat.DECIMAL, units='Hz',   desc='RF frequency of channel 0.')
        self._addModelVariable(model,    'dsss_chipping_code',  long,     ModelVariableFormat.HEX    ,               desc='DSSS chipping code')
        self._addModelVariable(model,    'dsss_len',  int,                ModelVariableFormat.DECIMAL,               desc='DSSS chipping code length in chips')
        self._addModelVariable(model,    'dsss_spreading_factor',  int,   ModelVariableFormat.DECIMAL,               desc='DSSS spreading factor = Tbit/Tchip')
        self._addModelVariable(model,    'baudrate_tol_ppm', int,         ModelVariableFormat.DECIMAL, units='ppm',  desc='Maximum baud rate error of the expected signal')
        self._addModelVariable(model,    'timing_sample_threshold',  int, ModelVariableFormat.DECIMAL,               desc='Start timing detection only if samples for timing detection are stronger than this threshold'   )
        self._addModelVariable(model,    'timing_detection_threshold',int,ModelVariableFormat.DECIMAL,               desc='Detect timing sequence if correlation value is larger than this threshold'   )
        self._addModelVariable(model,    'symbols_in_timing_window',int,  ModelVariableFormat.DECIMAL,               desc='Number of valid symbols to required to detect timing')
        self._addModelVariable(model,    'errors_in_timing_window',int,   ModelVariableFormat.DECIMAL,               desc='Number of symbols erros allowed in a timing window when detecting timing'   )
        self._addModelVariable(model,    'number_of_timing_windows',int,  ModelVariableFormat.DECIMAL,               desc='Number of timing windows to detect before searching for sync word'   )
        self._addModelVariable(model,    'sqi_threshold',int,             ModelVariableFormat.DECIMAL,               desc='Signal Quality Indicator (SQI) threshold on correlation values to qualify a symbol as strong or weak')
        self._addModelVariable(model,    'timing_resync_period',int,      ModelVariableFormat.DECIMAL,               desc='Timing resynchronization period in multiples of symbols_in_timing_window ')
        self._addModelVariable(model,    'agc_period',int,                ModelVariableFormat.DECIMAL,               desc='AGC power measurement period')
        self._addModelVariable(model,    'frequency_offset_period',int,   ModelVariableFormat.DECIMAL,               desc='Period used to estimate frequency offset in internal comp')
        self._addModelVariable(model,    'afc_period',int,                ModelVariableFormat.DECIMAL,               desc='Period used to estimate frequency offset in AFC loop')
        self._addModelVariable(model,    'agc_power_target',int,          ModelVariableFormat.DECIMAL,  units='dBm', desc='Power target in dBm at channel filter output')
        self._addModelVariable(model,    'rssi_period',int,               ModelVariableFormat.DECIMAL,               desc='RSSI calculation period')
        self._addModelVariable(model,    'agc_hysteresis',int,            ModelVariableFormat.DECIMAL,  units='dB',  desc='Hysteresis level for AGC in dB')
        self._addModelVariable(model,    'shaping_filter_param',  float,  ModelVariableFormat.DECIMAL,               desc='BT value for Gaussian  and roll-off factor for Raised Cosine pulse shaping filter.')
        self._addModelVariable(model,    'ook_slicer_level',  int,        ModelVariableFormat.DECIMAL,               desc='Level to slice OOK symbols at as referenced from signal.')

        # Preamble/sync variables
        self._addModelVariable(model,   'preamble_pattern',       int,    ModelVariableFormat.DECIMAL,               desc='Minimum repeated portion of the preamble such as binary 01 or 10. ')
        self._addModelVariable(model,   'preamble_pattern_len',   int,    ModelVariableFormat.DECIMAL, units='bits', desc='Length of the preamble pattern in bits. This will be set to 2 for a simple 01 or 10 preamble pattern.')
        self._addModelVariable(model,   'preamble_length',        int,    ModelVariableFormat.DECIMAL, units='bits', desc='Total length of the preamble.')
        self._addModelVariable(model,   'syncword_0',             long,   ModelVariableFormat.HEX,                   desc="Default sync word.  Stored with the last bit transmitted in the LSB.")
        self._addModelVariable(model,   'syncword_1',             long,   ModelVariableFormat.HEX,                   desc="Alternative sync word for dual sync word cases.  Stored with the last bit transmitted in the LSB.")
        self._addModelVariable(model,   'syncword_length',        int,    ModelVariableFormat.DECIMAL, units='bits', desc="Length of the sync word in bits.")
        self._addModelVariable(model,   'syncword_tx_skip',       bool,   ModelVariableFormat.DECIMAL,               desc="Sync Word TX Skip.")

        self._addModelVariable(model,   'asynchronous_rx_enable', bool,      ModelVariableFormat.ASCII, desc='Asynchronous RX mode enabled')


        # FSK MAP
        var = self._addModelVariable(model, 'fsk_symbol_map', Enum, ModelVariableFormat.DECIMAL, 'List of FSK symbol mappings')
        member_data = [
            ['MAP0', 0, '4FSK: 11, 10, 00, 01 in decreasing frequency, 2FSK: 1 high, 0 low frequency'],
            ['MAP1', 1, '4FSK: 01, 00, 10, 11 in decreasing frequency, 2FSK: 0 high, 1 low frequency'],
            ['MAP2', 2, '4FSK: 10, 11, 01, 00 in decreasing frequency, 2FSK: undefined'],
            ['MAP3', 3, '4FSK: 00, 01, 11, 10 in decreasing frequency, 2FSK: undefined'],
            ['MAP4', 4, '4FSK: 11, 01, 00, 10 in decreasing frequency, 2FSK: undefined'],
            ['MAP5', 5, '4FSK: 10, 00, 01, 11 in decreasing frequency, 2FSK: undefined'],
            ['MAP6', 6, '4FSK: 01, 11, 10, 00 in decreasing frequency, 2FSK: undefined'],
            ['MAP7', 7, '4FSK: 00, 10, 11, 01 in decreasing frequency, 2FSK: undefined'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FskSymbolMapEnum',
            'List of supported FSK symbol mappings',
            member_data)

        # DIFFENCMODE
        var = self._addModelVariable(model, 'diff_encoding_mode', Enum, ModelVariableFormat.DECIMAL, 'Differential encoding mode options')
        member_data = [
            ['DISABLED', 0, 'Differential encoding is disabled'],
            ['RR0'     , 1, 'Transmit the xor-ed value of the Raw symbol and the last Raw symbol. Initial Raw symbol is 0.'],
            ['RE0'     , 2, 'Transmit the xor-ed value of the Raw symbol and the last Encoded symbol. Initial Encoded symbol is 0.'],
            ['RR1'     , 3, 'Transmit the xor-ed value of the Raw symbol and the last Raw symbol. Initial Raw symbol is 1.'],
            ['RE1'     , 4, 'Transmit the xor-ed value of the Raw symbol and the last Encoded symbol. Initial Encoded symbol is 1.'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'DiffEncModeEnum',
            'List of supported Differential Encoding Modes',
            member_data)

        var = self._addModelVariable(model, 'frequency_comp_mode', Enum, ModelVariableFormat.DECIMAL, desc='Frequency compensation mode options')
        member_data = [
            ['DISABLED',  0, 'Neither Frequency Offset Compensation (AFC) nor internal frequency compensation is enabled'],
            ['INTERNAL_LOCK_AT_PREAMBLE_DETECT',  1, 'Internal frequency compensation is enabled only freezing at PREAMBLE_DETECT'],
            ['INTERNAL_LOCK_AT_FRAME_DETECT',  2, 'Internal frequency compensation is enabled only freezing at FRAME_DETECT'],
            ['INTERNAL_ALWAYS_ON',  3, 'Internal frequency compensation is enabled only and is always on'],
            ['AFC_FREE_RUNNING',  4, 'Frequency Offset Compensation (AFC) is enabled only and is always on'],
            ['AFC_START_AT_PREAMBLE_FREE_RUNNING',  5, 'AFC is enabled only starting at PREAMBLE_DETECT'],
            ['AFC_LOCK_AT_TIMING_DETECT',  6, 'Frequency Offset Compensation (AFC) is enabled only freezing at TIMING_DETECT'],
            ['AFC_LOCK_AT_PREAMBLE_DETECT',  7, 'Frequency Offset Compensation (AFC) is enabled only freezing at PREAMBLE_DETECT'],
            ['AFC_LOCK_AT_FRAME_DETECT',  8, 'Frequency Offset Compensation (AFC) is enabled only freezing at FRAME_DETECT'],
            ['AFC_START_AT_PREAMBLE_LOCK_AT_FRAME_DETECT',  9, 'Frequency Offset Compensation (AFC) is enabled only starting at PREAMBLE_DETECT and freezing at FRAME_DETECT'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FreqCompModeEnum',
            'TBD.',
            member_data)


        # Output fields

        # Add register groups
        self._add_TXBR_regs(model)

        # Add granular vars
        self._add_baudrate_variable(model)

        self._addModelRegister(model, 'MODEM.CF.CFOSR'                 , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CF.DEC0'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CF.DEC1'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CF.DEC1GAIN'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CF.DEC2'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.CODING'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.DIFFENCMODE'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.DSSSDOUBLE'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.DSSSLEN'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.DSSSSHIFTS'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.MAPFSK'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.MODFORMAT'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.SHAPING'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.COMPMODE'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.FREQOFFESTLIM'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.FREQOFFESTPER'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.PHASEDEMOD'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.RESYNCPER'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.SYNCBITS'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL1.SYNCDATA'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL2.DATAFILTER'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL2.DEVWEIGHTDIS'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL2.SQITHRESH'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL2.RXPINMODE'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL3.TSAMPDEL'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL3.TSAMPLIM'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL3.TSAMPMODE'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL4.DEVOFFCOMP'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL4.ISICOMP'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL4.OFFSETPHASEMASKING' , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL5.BRCALAVG'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL5.BRCALEN'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL5.TDEDGE'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL5.TREDGE'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.MODINDEX.FREQGAINE'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.MODINDEX.FREQGAINM'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.MODINDEX.MODINDEXE'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.MODINDEX.MODINDEXM'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.PRE.BASE'                 , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.PRE.BASEBITS'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.PRE.PREERRORS'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.PRE.TXBASES'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.RXBR.RXBRDEN'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.RXBR.RXBRINT'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.RXBR.RXBRNUM'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING0.COEFF0'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING0.COEFF1'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING0.COEFF2'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING0.COEFF3'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING1.COEFF4'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING1.COEFF5'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING1.COEFF6'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING1.COEFF7'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SHAPING2.COEFF8'          , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SYNC0.SYNC0'              ,long, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.SYNC1.SYNC1'              ,long, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.ADDTIMSEQ'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.FDM0THRESH'        , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.OFFSUBDEN'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.OFFSUBNUM'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.TIMINGBASES'       , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.TIMING.TIMTHRESH'         , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.DSSS0.DSSS0'              , long, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFCADJLIM.AFCADJLIM'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFC.AFCAVGPER'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFC.AFCDEL'               , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFC.AFCRXCLR'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFC.AFCRXMODE'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFC.AFCSCALEE'            , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.AFC.AFCSCALEM'            , int, ModelVariableFormat.HEX )

        self._addModelRegister(model, 'SEQ.MISC.SQBMODETX'             , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'SEQ.MISC.DYNAMIC_SLICER_SW_EN'  , int, ModelVariableFormat.HEX )

        # Internal variables
        self._addModelVariable(model,    'base'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'basebits'              ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'if_frequency_hz'       ,  int,    ModelVariableFormat.DECIMAL, units='Hz', desc='Intermediate Frequency')
        self._addModelVariable(model,    'cfosr'                 ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'dec0'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'dec1'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'dec1gain'              ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'dec2'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'freq_gain'             , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'iffilt_ratio'          , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'lodiv'                 ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'modindex'              , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rxbrden'               ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rxbrfrac'              , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rxbrint'               ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rxbrnum'               ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'timingbases'           ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'txbr_ratio'            , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'bandwidth_carson_hz'   , int,     ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'bandwidth_hz'          , int,     ModelVariableFormat.DECIMAL, units='Hz', desc='overwrites the auto-calculated RX filter bandwidth for the receiver. ')
        self._addModelVariable(model,    'freq_gain_scale'       , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'freq_offset_hz'        , int,     ModelVariableFormat.DECIMAL, units='Hz', desc='Frequency limit beyond which no AFC cancelation / frequency offset compensation occurs')
        self._addModelVariable(model,    'modulation_index'      , float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'fdm0_thresh'           ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'brcalavg'              ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'brcalen'               ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'sensitivity',            float,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'dsss_bits_per_symbol'  ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'dsss_shifts'           ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rx_baudrate_offset_hz' ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'rx_bitrate_offset_hz'  ,   int,   ModelVariableFormat.DECIMAL, units='Hz', desc='Nominal Rx bit rate offset wrt the value set in entry field Bitrate')
        self._addModelVariable(model,    'afc_scale'             ,  float,  ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'freq_offset_scale'     ,  float,  ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'frequency_offset_factor',      float, ModelVariableFormat.DECIMAL)
        self._addModelVariable(model,    'frequency_offset_factor_fxp',  long,  ModelVariableFormat.HEX)
        self._addModelVariable(model,    'dynamic_slicer_enabled',           bool, ModelVariableFormat.ASCII)
        self._addModelVariable(model,    'dynamic_slicer_threshold_values',  int,  ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model,    'dynamic_slicer_level_values',      int,  ModelVariableFormat.DECIMAL, is_array=True)
        self._addModelVariable(model,    'ook_ebno',                         float, ModelVariableFormat.DECIMAL)

        # Common output variables


        # Variables for the reverse path
        self._addModelActual(model,    'base_frequency', long,          ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'bandwidth',  int,               ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'syncword_length',  int,         ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'preamble_pattern_len',  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'dsss_len',  int,                ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'dsss_shifts',  int,             ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'baudrate_tol_ppm', int,         ModelVariableFormat.DECIMAL)

        self._addModelActual(model,    'if_frequency_hz'       ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'cfosr'                 ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'dec0'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'dec1'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'dec1gain'              ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'dec2'                  ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'freq_gain'             , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'iffilt_bandwidth'      ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'interpolation_gain'    , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'lodiv'                 ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'mod_format'            , str,     ModelVariableFormat.ASCII)
        self._addModelActual(model,    'modindex'              , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'oversampling_rate'     , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rx_baud_rate'          , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rx_deviation'          , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rx_synth_freq'         , long,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rxbrden'               ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rxbrfrac'              , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rxbrint'               ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'rxbrnum'               ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'shaping_filter_gain'   ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'synth_res'             , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'timingbases'           ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'timthresh'             ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'tx_baud_rate'          , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'baud_per_symbol'       ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'bits_per_symbol'       ,  int,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'tx_deviation'          , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'tx_synth_freq'         , long,    ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'txbr_ratio'            , float,   ModelVariableFormat.DECIMAL)

        self._addModelActual(model,    'iffilt_ratio'          , float,   ModelVariableFormat.DECIMAL)

        self._addModelActual(model,    'modulation_index'      , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'timing_window'         ,   int,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'resyncper'             , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'afc_scale'             , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'offsub_ratio'          , float,   ModelVariableFormat.DECIMAL)
        self._addModelActual(model,    'afc_limit_hz'          , float,   ModelVariableFormat.DECIMAL)

    def _add_modulation_type_variable(self, model):

        #This method is needed so that we can easily override the modulation type in future parts

        var = self._addModelVariable(model, 'modulation_type', Enum, ModelVariableFormat.DECIMAL,
                                     'Defines the modulation type.')
        member_data = [
            ['FSK2', 0, 'Frequency Shift Keying on two frequencies'],
            ['FSK4', 1, 'Frequency Shift Keying on four frequencies'],
            ['BPSK', 2,
             'Binary Phase Shift Keying: the 2 symbols are represented by 0 or 180 degree phase shifts wrt the carrier'],
            ['DBPSK', 3,
             'Differential Binary Phase Shift Keying: the 2 symbols are represented by 0 or 180 degree phase shifts wrt the preceding symbol'],
            ['OOK', 4, 'On Off Keying: the 2 symbols are represented by the presence / absence of the carrier'],
            ['ASK', 5,
             'Amplitude Shift Keying: the 2 symbols are represented by two different power levels of the carrier'],
            ['MSK', 6,
             'Minimum Shift Keying: Special case of FSK2 where the phase shift in one symbol is +/- 90 degree'],
            ['OQPSK', 7,
             'Offset Quadrature Phase Shift Keying: 4 state phase modulation with 0, 90, 180 and 270 degrees wrt the carrier. Only +/-90 degree changes are allowed at any one transition that take place at twice the symbol rate.'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'ModModeEnum',
            'Defines the modulation type.',
            member_data)

    def _add_bitrate_variable(self, model):
        #On parts prior to Ocelot the bitrate variable in the calculator represents chiprate
        self._addModelVariable(model, 'bitrate', int, ModelVariableFormat.DECIMAL, units='bps',
                               desc='Chip rate after channel coding and before symbol coding.')

    def _add_baudrate_variable(self, model):
        self._addModelVariable(model, 'baudrate', int, ModelVariableFormat.DECIMAL)

    def _add_TXBR_regs(self, model):
        self._addModelRegister(model, 'MODEM.TXBR.TXBRNUM', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'MODEM.TXBR.TXBRDEN', int, ModelVariableFormat.HEX)
