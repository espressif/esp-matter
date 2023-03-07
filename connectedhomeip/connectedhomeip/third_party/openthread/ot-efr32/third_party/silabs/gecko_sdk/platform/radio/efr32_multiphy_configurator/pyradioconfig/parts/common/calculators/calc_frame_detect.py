"""CALC_Frame_Detect Calculator Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be returned by overriding the function:
        def getCalculationList(self):
"""

from collections import OrderedDict
import math

from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

from py_2_and_3_compatibility import *

class CALC_Frame_Detect(ICalculator):

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

        self._addModelVariable(model,  'preamble_string',  str,          ModelVariableFormat.ASCII, desc='Output string representing the preamble pattern in binary')
        self._addModelVariable(model,  'syncword_string',  str,          ModelVariableFormat.ASCII, desc='Output string representing the sync word in binary')
        self._addModelVariable(model,  'syncword_dualsync',bool,         ModelVariableFormat.ASCII, desc='Enable dual syncword detection')

        # Actual values
        self._addModelActual(model,    'syncword_0'     ,  long,         ModelVariableFormat.HEX, ) #  desc='Syncword 0 extracted from the register )
        self._addModelActual(model,    'syncword_1'     ,  long,         ModelVariableFormat.HEX, ) #  desc='Syncword 1 extracted from the register )

        self._addModelRegister(model, 'MODEM.CTRL1.DUALSYNC'           , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'MODEM.CTRL0.FDM0DIFFDIS'        , int, ModelVariableFormat.HEX )
        
    @staticmethod
    def flip_bits(input, numbits):
        """
        flips the order of bits in an input numbits wide
        Bits are flipped within the field defined by numbits

        Args:
            input (unknown) : input
            numbits (unknown) : numbits

        Returns:
            output (unknown) : unknown
        """

        output = long(0)
        # find index of LSB
        first_bit = numbits

        for bitnum in range(numbits):
            if (input & (1 << bitnum)):
                output = output | (1 << (numbits - 1 - bitnum))

        return output

    def calc_syncword_tx_skip(self, model):
        if model.vars.syncword_tx_skip.value:
            self._reg_write(model.vars.MODEM_CTRL1_SYNCDATA, 1)
        else:
            self._reg_write(model.vars.MODEM_CTRL1_SYNCDATA, 0)

    def calc_sync_words_reg(self, model):
        """
        write sync words from input to registers

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        syncword_length = model.vars.syncword_length.value
        syncword0_reg = long(self.flip_bits(model.vars.syncword_0.value, syncword_length))
        syncword1_reg = long(self.flip_bits(model.vars.syncword_1.value, syncword_length))

        if model.vars.ber_force_sync.value == True:
            syncword0_reg = long(0x1dd3d4a0)      # gdc:  Fix this after we get rid of the "_left" stuff above.
                                              # gdc:  Fix it so we just write syncword_0 before it gets flipped

        # When manchester invert is selected, then elsewhere we flip the entire fsk mapping,
        # which also flips the preamble and sync word.  We don't want the the preamble and
        # syncword flipped, so to fix it, we invert the preamble pattern and sync word register
        # to undo the fsk mapping flip.
        encoding = model.vars.symbol_encoding.value
        manchester_map = model.vars.manchester_mapping.value
        if encoding == model.vars.symbol_encoding.var_enum.Manchester and \
           manchester_map != model.vars.manchester_mapping.var_enum.Default:
            syncword_mask = (1 << syncword_length) - 1
            syncword0_reg ^= syncword_mask
            syncword1_reg ^= syncword_mask

        self._reg_write(model.vars.MODEM_SYNC0_SYNC0, syncword0_reg)
        self._reg_write(model.vars.MODEM_SYNC1_SYNC1, syncword1_reg)


    def calc_syncbits_reg(self, model):
        """
        write sync word length from input to register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        assert(model.vars.syncword_length.value > 0)
        
        if model.vars.ber_force_sync.value == True:
            syncword_length = 32
        else:
            syncword_length = model.vars.syncword_length.value
            
        self._reg_write(model.vars.MODEM_CTRL1_SYNCBITS,  syncword_length - 1)

            
    
    def calc_syncword_length_actual(self, model):
        """given register read back actual sync word length

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.syncword_length_actual.value = model.vars.MODEM_CTRL1_SYNCBITS.value + 1
    
    # Calculate the actual syncword from the register
    def calc_syncword_actual(self, model):
        syncword_length = model.vars.syncword_length_actual.value
        syncword0_reg = model.vars.MODEM_SYNC0_SYNC0.value
        syncword1_reg = model.vars.MODEM_SYNC1_SYNC1.value

        encoding = model.vars.symbol_encoding.value
        manchester_map = model.vars.manchester_mapping.value
        if encoding == model.vars.symbol_encoding.var_enum.Manchester and \
           manchester_map != model.vars.manchester_mapping.var_enum.Default:
            syncword_mask = (1 << syncword_length) - 1
            syncword0_reg ^= syncword_mask
            syncword1_reg ^= syncword_mask

        # if MSbit is set, will need to trip the leading '-' character from the binary string
        model.vars.syncword_0_actual.value = long(bin(syncword0_reg).replace('-','')[2:].zfill(syncword_length)[::-1], 2)
        model.vars.syncword_1_actual.value = long(bin(syncword1_reg).replace('-','')[2:].zfill(syncword_length)[::-1], 2)

    # Calculate a binary string that's
    def calc_syncword_string(self, model):
        syncword_length = model.vars.syncword_length_actual.value
        syncword = model.vars.syncword_0_actual.value
        model.vars.syncword_string.value = bin(syncword)[2:].zfill(syncword_length)


    def calc_preamble_string(self, model):
        preamble_pattern     = model.vars.preamble_pattern.value
        preamble_pattern_len = model.vars.preamble_pattern_len.value
        preamble_len      = model.vars.preamble_length.value

        repeats = int(preamble_len/preamble_pattern_len)
        preamble_pattern_string = ('{:0' + str(preamble_pattern_len) + 'b}').format(preamble_pattern)
        
        preamble_string = preamble_pattern_string*repeats
        
        model.vars.preamble_string.value = preamble_string
    

        
    def calc_timbases_val(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        preamblebits = model.vars.preamble_length.value
        syncbits = model.vars.syncword_length_actual.value
        modformat = model.vars.modulation_type.value
        basebits = model.vars.preamble_pattern_len_actual.value
        encoding = model.vars.symbol_encoding.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value
        baudrate_tol_req = model.vars.baudrate_tol_ppm.value
        baudrate = model.vars.baudrate.value
        dualsync = model.vars.syncword_dualsync.value

        # Estimate what the baudrate tol is with an OSR of 7 and resyncper of 1, assuming a 16-bit window
        est_baudrate_tol_16sym = 1.0e6 / (2 * 7 * 16.0 * 1) # Factor of 2 is to be conservative

        if model.vars.asynchronous_rx_enable.value is True:
            # when asynchronous direct mode is enabled set to max
            timingbases = 15
        elif encoding == model.vars.symbol_encoding.var_enum.DSSS:
            # for DSSS set to 1
            timingbases = 1
        else:
            if modformat == model.vars.modulation_type.var_enum.FSK4:
                timingbases = py2round(preamblebits / 8.0)
            elif modformat == model.vars.modulation_type.var_enum.OOK:
                if preamblebits < 8:
                    LogMgr.Warning("OOK RX is only supported for preamble lengths greater than or equal to 8 bits")
                    timingbases = 0
                elif preamblebits < 16:
                    timingbases = 2
                else:
                    timingbases = 4
            elif in_2fsk_opt_scope:
                if preamblebits < 8:
                    timingbases = 0     #Always use FDM0 for very short preambles
                elif preamblebits < 16:
                    if syncbits >= 16 and (5 * baudrate * syncbits) < 150e6 and not dualsync:   #If we have enough syncword bits use FMD0
                        timingbases = 0                         #If the baudrate and syncword are too large can cause OVF
                    else:
                        timingbases = math.floor(preamblebits/4.0)
                elif preamblebits < 32:
                    timingbases = 4
                else:
                    if est_baudrate_tol_16sym >= baudrate_tol_req:
                        timingbases = 8  #Only use a 16-bit timing window if we are confident we have sufficient baudrate tol
                    else:
                        timingbases = 4
            else:
                if preamblebits < 8:
                    timingbases = 0
                elif preamblebits < 10:
                    timingbases = 1
                elif preamblebits <= 32:
                    timingbases = 4
                else:
                    # MCUW_RADIO_CFG-827 Timing window is calculated wrongly if preamble pattern length is not 2
                    # Enforce Window must be less than half the preamble length
                    if (basebits * 8) < ( preamblebits / 2):
                        timingbases = 8
                    elif (basebits * 4) < ( preamblebits / 2):
                        timingbases = 4
                    elif (basebits * 1) < ( preamblebits / 2):
                        timingbases = 1
    
            if timingbases > 15:
                timingbases = 15

        model.vars.symbols_in_timing_window.value = int(timingbases * basebits)


    def calc_timbases_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        timingwindow = model.vars.symbols_in_timing_window.value * 1.0
        basebits = model.vars.preamble_pattern_len_actual.value

        if model.vars.ber_force_fdm0.value == True:
            timingbases = 0
        else:
            timingbases = int(math.ceil(timingwindow / basebits))

        self._reg_write(model.vars.MODEM_TIMING_TIMINGBASES,  timingbases)
    
    
    # TODO: tweak equation based on new PHYs
    def calc_addtimseq_val(self, model):
        """
        calculate additional timing sequences to detect given preamble length
        the equation used to calcualte ADDTIMSEQ is derived emprically and might need
        tweaking as we have more PHY providing additional data

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        preamblebits = model.vars.preamble_length.value * 1.0
        timingbases = model.vars.timingbases_actual.value
        timingwindow = model.vars.timing_window_actual.value
        mod_format = model.vars.modulation_type.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value

        if mod_format == model.vars.modulation_type.var_enum.OOK:
            #Always use 1 timing window for OOK
            addtimseq = 0
        elif in_2fsk_opt_scope:
            #Always use 1 timing window for 2FSK optimization
            addtimseq = 0
        else:
            if timingbases > 1:
                # looks like it is easier to get a working setting when this is set to 0
                # for now going to generate 0 until we find a better alternative
                addtimseq = math.floor(preamblebits / timingwindow) - 2
            else:
                addtimseq = 0
    
        # saturate addtimseq to fit into 4 bits
        if addtimseq > 15:
            addtimseq = 15
    
        if addtimseq < 0:
            addtimseq = 0

        model.vars.number_of_timing_windows.value = int(addtimseq) + 1

    def calc_addtimseq_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        addtimseq = model.vars.number_of_timing_windows.value

        # one search is done by default
        if addtimseq > 0:
            addtimseq -= 1

        self._reg_write(model.vars.MODEM_TIMING_ADDTIMSEQ, addtimseq)
    
    
    def calc_timbases_actual(self, model):
        """
        return actual TIMINGBASES value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.timingbases_actual.value = model.vars.MODEM_TIMING_TIMINGBASES.value

    
    def calc_timingwindow_actual(self, model):
        """
        calculate the size of the timing window. If timingbases == 0 we are in FDM0 mode where
        the timing window is set by number of sync bits. In FDM1 (ADDTIMSEQ = 0) and FDM2 (ADDTIMSEQ > 0)
        modes the timing window size is a product of timingbases and basebits.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        timingbases = model.vars.timingbases_actual.value
        basebits = model.vars.preamble_pattern_len_actual.value
        syncword_length = model.vars.syncword_length_actual.value
        spreading_factor = model.vars.dsss_spreading_factor.value

        if timingbases == 0:
            timing_window = syncword_length
        elif spreading_factor > 0:
            timing_window = timingbases * spreading_factor
        else:
            timing_window = timingbases * basebits

        model.vars.timing_window_actual.value = timing_window
    
    
    def calc_basebits_actual(self, model):
        """
        return actual base bits

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.preamble_pattern_len_actual.value = model.vars.MODEM_PRE_BASEBITS.value + 1
    
    
    def calc_basebits_reg(self, model):
        """
        set BASEBITS register using input

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        basebits = model.vars.preamble_pattern_len.value
    
        if basebits > 0:
            reg = basebits - 1
        else:
            reg = 0
    
        self._reg_write(model.vars.MODEM_PRE_BASEBITS,  reg)
    
    
    def calc_base_reg(self, model):
        """
        set BASE register using input
        The bits have to be flipped around before writing the register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        preamble_pattern_len = model.vars.preamble_pattern_len.value
        preamble_pattern     = model.vars.preamble_pattern.value

        # When manchester invert is selected, then elsewhere we flip the entire fsk mapping,
        # which also flips the preamble and sync word.  We don't want the the preamble and
        # syncword flipped, so to fix it, we invert the preamble pattern and sync word register
        # to undo the fsk mapping flip.
        encoding = model.vars.symbol_encoding.value
        manchester_map = model.vars.manchester_mapping.value
        if encoding == model.vars.symbol_encoding.var_enum.Manchester and \
           manchester_map != model.vars.manchester_mapping.var_enum.Default:
            preamble_pattern_mask = (1 << preamble_pattern_len) - 1
            preamble_pattern ^= preamble_pattern_mask

        modem_pre_base = self.flip_bits(preamble_pattern, preamble_pattern_len)
        modem_pre_base = int(modem_pre_base)
    
        self._reg_write(model.vars.MODEM_PRE_BASE,  modem_pre_base)
    
    
    #TODO: do we need to add dependency to bandwidth when calculating the timing treshold?
    #      we might want to reduce the threshold with increasing bandwidth
    def calc_timthresh_value(self, model):
        """
        calculate TIMTHRESH which is used in determining valid correlation values in timing detection

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modformat = model.vars.modulation_type.value
        timingwindow = model.vars.timing_window_actual.value
        async_direct_mode_enabled = model.vars.asynchronous_rx_enable.value
        bw_carson = model.vars.bandwidth_carson_hz.value
        bw_actual = model.vars.bandwidth_actual.value
        freq_gain_scale = model.vars.freq_gain_scale.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value

        #Calculate the percentage of the actual BW vs Carson rule
        bw_ratio = bw_actual / bw_carson
    
        # given modulation method determine nominal soft decision values
        if modformat == model.vars.modulation_type.var_enum.MSK or \
           modformat == model.vars.modulation_type.var_enum.OQPSK or \
           modformat == model.vars.modulation_type.var_enum.BPSK or \
           modformat == model.vars.modulation_type.var_enum.DBPSK:
            nominal_soft_decision = 64
        elif modformat == model.vars.modulation_type.var_enum.FSK2:
            if in_2fsk_opt_scope:
                if bw_ratio < 1.0:
                    nominal_soft_decision = 64 / freq_gain_scale * bw_ratio   #Adjust correlation threshold for low BW scenarios
                else:
                    nominal_soft_decision = 64 / freq_gain_scale
            else:
                nominal_soft_decision = 64
        elif modformat == model.vars.modulation_type.var_enum.FSK4:
            nominal_soft_decision = 64
        else:
            nominal_soft_decision = 0
    
        # given nominal soft decision value and timing window size determine optimal threshold
        # TODO: equation for thresh is empirically derived and does need tuning
        if nominal_soft_decision > 0:
            thresh = timingwindow * nominal_soft_decision / 32.0 - 1.0
        else:
            thresh = 0.0


        # in direct mode set the threshold as high as possible
        if async_direct_mode_enabled:
            thresh = 255.0
    
        model.vars.timing_detection_threshold.value = int(round(thresh))
    
    
    def calc_timthresh_reg(self, model):
        """
        given desired threshold set register value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        threshold = model.vars.timing_detection_threshold.value

        if threshold > 255.0:
            threshold = 255.0
            LogMgr.Warning("WARNING: threshold larger than max allowed 255!")

        self._reg_write(model.vars.MODEM_TIMING_TIMTHRESH, int(threshold))
    
    
    def calc_timthresh_actual(self, model):
        """
        given register value return actual threshold value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.timthresh_actual.value = model.vars.MODEM_TIMING_TIMTHRESH.value
    
    
    def calc_preerrors_val(self, model):
        """
        calculate PREERRORS field

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # FIXME: consider adding +1 to errors when AFC is enbabled - seems to work better

        dssslen = model.vars.dsss_len_actual.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value
        baudrate = model.vars.baudrate.value

        if dssslen == 0:
            if in_2fsk_opt_scope and baudrate > 1900000:
                preerrors = 1
            else:
                preerrors = 0
        else:
            preerrors = dssslen / 2.0

        # make sure we fit into 4 bits
        if preerrors > 15:
            preerrors = 15

        model.vars.errors_in_timing_window.value = int(round(preerrors))

    def calc_preerrors_reg(self, model):
        """
        write value to register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        preerrors = model.vars.errors_in_timing_window.value

        if preerrors > 15:
            preerrors = 15

        self._reg_write(model.vars.MODEM_PRE_PREERRORS, preerrors)

    def calc_dsss0_reg(self, model):
        """
        write DSSS symbol 0 register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        self._reg_write(model.vars.MODEM_DSSS0_DSSS0, model.vars.dsss_chipping_code.value)

    def calc_dsssshifts_val(self, model):
        """
        calculate DSSS shift value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        length = model.vars.dsss_len.value
        bps = model.vars.dsss_bits_per_symbol.value

        if bps <= 1:
            val = 0
        else:
            val = length / (pow(2,bps) / 2)

        model.vars.dsss_shifts.value = int(val)
    
    def calc_dsssshifts_reg(self, model):
        """
        write DSSS cyclic shifts number to generate new symbols when using DSSS

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = model.vars.dsss_shifts.value

        if val == 0:
            reg = 0
        elif val == 1:
            reg = 1
        elif val == 2:
            reg = 2
        elif val == 4:
            reg = 3
        elif val == 8:
            reg = 4
        elif val == 16:
            reg = 5
        else:
            raise CalculationException("Invalid dsss_shift value!")
            return
    
        self._reg_write(model.vars.MODEM_CTRL0_DSSSSHIFTS, reg)
    
    
    def calc_dsssshifts_actual(self, model):
        """
        given register setting return actual DSSS shifts value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        reg = model.vars.MODEM_CTRL0_DSSSSHIFTS.value
    
        if reg == 0:
            val = 0
        elif reg == 1:
            val = 1
        elif reg == 2:
            val = 2
        elif reg == 3:
            val = 4
        elif reg == 4:
            val = 8
        elif reg == 5:
            val = 16
    
        model.vars.dsss_shifts_actual.value = val
    
    
    def calc_dssslen_reg(self, model):
        """
        set DSSS length register

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        length = model.vars.dsss_len.value
        shifts = model.vars.dsss_shifts_actual.value
    
        if length == 0:
            reg = 0
        else:
            if shifts == 0:
                if length < 4 or length > 32:
                    raise CalculationException("dsss_shift value must be between 4 and 32")
                    return
            else:
                if not length % shifts == 0:
                    raise CalculationException("dsss_len must be an integer multiple of dsss_shifts")
                    return
            reg = length - 1
    
        self._reg_write(model.vars.MODEM_CTRL0_DSSSLEN, reg)
    
    
    def calc_dssslen_actual(self, model):
        """
        given register setting return actual DSSS length

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        dsss0 = model.vars.MODEM_DSSS0_DSSS0.value
    
        if dsss0 == 0:
            len = 0
        else:
            len = model.vars.MODEM_CTRL0_DSSSLEN.value + 1
    
        model.vars.dsss_len_actual.value = len
    
    
    def calc_dsssdouble_reg(self, model):
        """
        based on modulation used select if DSSS symbol's inverted version
        should also be a DSSS symbol

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        length = model.vars.dsss_len.value
        modulation = model.vars.modulation_type.value
    
        if length > 0:
            if modulation == model.vars.modulation_type.var_enum.OQPSK:
                dsssdouble = 2
            else:
                dsssdouble = 1
        else:
            dsssdouble = 0
    
        self._reg_write(model.vars.MODEM_CTRL0_DSSSDOUBLE, dsssdouble)
    
    def calc_dsss_bits_per_symbol(self, model):
        """
        calculate bits per symbol in DSSS mode

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        length = model.vars.dsss_len.value
        spreading_factor = model.vars.dsss_spreading_factor.value * 1.0

        if spreading_factor == 0:
            bps = 0
        else:
            bps = length / spreading_factor

        model.vars.dsss_bits_per_symbol.value = int(bps)

    def calc_tr_td_edge(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modformat = model.vars.modulation_type.value

        if modformat == model.vars.modulation_type.var_enum.BPSK or \
           modformat == model.vars.modulation_type.var_enum.DBPSK:
            self._reg_write(model.vars.MODEM_CTRL5_TDEDGE, 1)
            self._reg_write(model.vars.MODEM_CTRL5_TREDGE, 1)
        else:
            self._reg_write(model.vars.MODEM_CTRL5_TDEDGE, 0)
            self._reg_write(model.vars.MODEM_CTRL5_TREDGE, 0)

    def calc_diffencmode_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        DIFFENCMODE_LOOKUP = {
            model.vars.diff_encoding_mode.var_enum.DISABLED.value:  0,
            model.vars.diff_encoding_mode.var_enum.RR0.value:  1,
            model.vars.diff_encoding_mode.var_enum.RE0.value:  2,
            model.vars.diff_encoding_mode.var_enum.RR1.value:  3,
            model.vars.diff_encoding_mode.var_enum.RE1.value:  4,
        }

        self._reg_write(model.vars.MODEM_CTRL0_DIFFENCMODE,
                       DIFFENCMODE_LOOKUP[(model.vars.diff_encoding_mode.value).value])

    def calc_tsamplim_val(self, model):
        """
        set TSAMPLIM to a default value of 10. Have an intermediate variable so that we can
        overwrite it

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        modformat = model.vars.modulation_type.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value
        preamblebits = model.vars.preamble_length.value

        if model.vars.asynchronous_rx_enable.value is True:
            # for asynchronous direct mode we don't want the demod to change states so
            # keep the thresholds at the upper limit
            th = 65535
        elif modformat == model.vars.modulation_type.var_enum.OOK or \
           modformat == model.vars.modulation_type.var_enum.ASK:
            # for amplitude modulated signal we need to turn off TSAMPMODE as enabling it
            # switches the slicer level from FREQOFFESTLIM to TSAMPLIM which we don't want.
            th = 0
        elif in_2fsk_opt_scope and preamblebits >= 32:
            th = 0  # [MCUW_RADIO_CFG-1077] If we have at least 32 preamble bits then correlation is robust
        else:
            # nominal threshold of 10 seems to work well for most PHYs
            th = 10

        model.vars.timing_sample_threshold.value = th

    def calc_tsamplim_reg(self, model):
        """
        calculate TSAMPLIM register based on variable. Saturating to 100 based on the fact
        that we have not seen a register setting greater than 20 up to this point despite
        the the register being 16 bits

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        threshold = model.vars.timing_sample_threshold.value

        if threshold < 0:
            threshold = 0
        elif threshold > 100 and not model.vars.asynchronous_rx_enable.value is True:
            threshold = 100

        self._reg_write(model.vars.MODEM_CTRL3_TSAMPLIM, threshold)

    def calc_tsampmode_reg(self, model):
        """
        set TSAMPMODE if we need a non-zero TSAMPLIM value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        threshold = model.vars.timing_sample_threshold.value
        modformat = model.vars.modulation_type.value
        encoding = model.vars.symbol_encoding.value

        if threshold == 0 or \
           modformat == model.vars.modulation_type.var_enum.OOK or \
           modformat == model.vars.modulation_type.var_enum.ASK or \
            encoding == model.vars.symbol_encoding.var_enum.DSSS:
            mode = 0
        else:
            mode = 1

        self._reg_write(model.vars.MODEM_CTRL3_TSAMPMODE, mode)


    def calc_tsampdel_val(self, model):
        """
        We do not see a strong relation between performance and this delay parameter but
        using hand optimized results for about 50 PHYs we came up with a simple equation
        to calculate the delay parameter when TSAMPMODE is enabled.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        baudrate = model.vars.baudrate.value
        osr = model.vars.oversampling_rate_actual.value

        if model.vars.MODEM_CTRL3_TSAMPMODE.value == 1:
            tsampdel = py2round(2.5e6/baudrate/osr)
        else:
            tsampdel = 0

        if tsampdel > 3:
            tsampdel = 3

        self._reg_write(model.vars.MODEM_CTRL3_TSAMPDEL, int(tsampdel))

    def calc_dualsync(selfself, model):

        sync1 = model.vars.syncword_1_actual.value

        if sync1 > 0:
            model.vars.syncword_dualsync.value = True
        else:
            model.vars.syncword_dualsync.value = False


    def calc_dualsync_reg(self, model):

        dualsync = model.vars.syncword_dualsync.value
        timingbases = model.vars.timingbases_actual.value

        # dualsync is disabled in FDM0 mode (MCUW_RADIO_CFG-1732)
        if dualsync == False:
            reg_value = 0
        elif dualsync == True and timingbases == 0:
            LogMgr.Warning("Second syncword is not supported if preamble is shorter than 8 bits")
            reg_value = 0
        else:
            reg_value = 1

        self._reg_write(model.vars.MODEM_CTRL1_DUALSYNC, reg_value)


    def calc_rxpinmode_reg(self, model):

        if model.vars.asynchronous_rx_enable.value is True:
            mode = 1
        else:
            mode = 0

        self._reg_write(model.vars.MODEM_CTRL2_RXPINMODE, mode)

    def calc_fdm0diffdis_reg(self,model):
        mod_type = model.vars.modulation_type.value
        timingbases = model.vars.timingbases_actual.value

        #If using OOK and FDM0, enable FDM0DIFFDIS so that sliding window detection works properly
        if (mod_type==model.vars.modulation_type.var_enum.OOK) and timingbases==0:
            self._reg_write(model.vars.MODEM_CTRL0_FDM0DIFFDIS,1)
        else:
            self._reg_write(model.vars.MODEM_CTRL0_FDM0DIFFDIS, 0)
