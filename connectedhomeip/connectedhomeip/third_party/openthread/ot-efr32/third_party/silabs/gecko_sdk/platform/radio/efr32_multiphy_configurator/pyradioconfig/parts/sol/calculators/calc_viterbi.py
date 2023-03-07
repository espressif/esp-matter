from pyradioconfig.parts.bobcat.calculators.calc_viterbi import Calc_Viterbi_Bobcat
from math import *

class Calc_Viterbi_Sol(Calc_Viterbi_Bobcat):

    def calc_swcoeffen_reg(self, model):

        afc1shot_en = model.vars.MODEM_AFC_AFCONESHOT.value
        fefilt_selected = model.vars.fefilt_selected.value

        reg = 1 if afc1shot_en else 0

        self._reg_write_by_name_concat(model, fefilt_selected, 'CHFCTRL_SWCOEFFEN', reg)

        self._reg_write(model.vars.MODEM_VTCORRCFG1_KSI3SWENABLE, reg)

    def calc_vtdemoden_reg(self, model):
        demod_sel = model.vars.demod_select.value
        modformat = model.vars.modulation_type.value

        # enable viterbi demod when it is selected
        if demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI or \
                demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER or \
                (demod_sel == model.vars.demod_select.var_enum.SOFT_DEMOD and
                 modformat == model.vars.modulation_type.var_enum.FSK2):
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VTDEMODEN, reg)

    def calc_realtimcfe_rtschmode_reg(self, model):
        #This function calculates the RTSCHMODE register field for TRECS

        #Read in model variables
        dualsync = model.vars.syncword_dualsync.value
        demod_select = model.vars.demod_select.value
        mod_type = model.vars.modulation_type.value

        #Calculate the register value based on whether we are using TRECS and whether dual syncword detect is enabled
        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
                demod_select == model.vars.demod_select.var_enum.TRECS_SLICER or \
                (demod_select == model.vars.demod_select.var_enum.SOFT_DEMOD and \
                 mod_type == model.vars.modulation_type.var_enum.FSK2):

            #If dual syncword detection is enabled, then stop using CFE and hard slice syncword w TRECS
            if dualsync:
                rtschmode = 1
            else:
                rtschmode = 0
        else:
            rtschmode = 0

        #Write the register
        self._reg_write(model.vars.MODEM_REALTIMCFE_RTSCHMODE, rtschmode)

    # improving calculation for PMOFFSET in Sol compared to Ocelot
    # could use this for Ocelot as well but don't want to change it at this point
    def calc_pmoffset_reg(self, model):

        osr = model.vars.MODEM_TRECSCFG_TRECSOSR.value
        rtschmode = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value
        pmdetthd = model.vars.MODEM_PHDMODCTRL_PMDETTHD.value
        freq_offset_hz = model.vars.freq_offset_hz.value
        deviation = model.vars.deviation.value

        if rtschmode == 1:
            #Special case for dual syncword detection case where hard slicing on syncword is required
            #because frequency tolerance is more difficult when RTSCHMODE is 1
            if deviation !=0 and freq_offset_hz/deviation > 2:
                #For very high offset cases we need to use a minimal PMOFFSET value to ensure no noisy samples in offset est
                pmoffset = 2
            else:
                pmoffset = osr * 2 + 2
        else:
            # + 2 for processing delay. See expsynclen register description. These are used in the same way.
            pmoffset = osr * pmdetthd + 2

        self._reg_write(model.vars.MODEM_TRECSCFG_PMOFFSET,  pmoffset)

    def calc_realtimcfe_trackingwin(self, model):
        #This function calculates the REALTIMCFE_TRACKINGWIN reg field

        #Read in model variables
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        freq_offset_hz = model.vars.freq_offset_hz.value
        deviation = model.vars.deviation.value
        baudrate_tol_ppm = model.vars.baudrate_tol_ppm.value

        if vtdemoden:
            #Reduce tracking window size if high relative frequency offset or baudrate offset is configured
            #In extreme freq offset cases the freq mapping will saturate, and then tracking is critical
            #to quickly bring the signal to center
            if (deviation != 0 and freq_offset_hz/deviation > 2) or baudrate_tol_ppm >= 10000:
                trackingwin = 3 #Do not set below 3 as this causes floor in Apps + Design setups
            elif (deviation != 0 and freq_offset_hz/deviation > 1) or baudrate_tol_ppm >= 5000:
                trackingwin = 5
            else:
                trackingwin = 7
        else:
            trackingwin = 0
        #Write the reg
        self._reg_write(model.vars.MODEM_REALTIMCFE_TRACKINGWIN, trackingwin)

    def calc_pmendschen(self, model):
        # This function enables a timeout for frame detect based on the end of preamble
        #This is taken rom Ocelot but modified to write pmendschen in BCR demod

        # Read in model vars
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        rtschmode = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value
        fast_detect_enable = (model.vars.fast_detect_enable.value == model.vars.fast_detect_enable.var_enum.ENABLED)
        bcr_demod_en = model.vars.bcr_demod_en.value

        # Calculate the register
        if vtdemoden:
            if rtschmode == 1 and fast_detect_enable:
                # If we are using hard slicing, then we can enable PMENDSCH
                # Only do this if we are in a PSM case
                pmendschen = 1
            else:
                pmendschen = 0
        elif (bcr_demod_en == 1):
            pmendschen = 1
        else:
            pmendschen = 0

        # Write the register
        self._reg_write(model.vars.MODEM_FRMSCHTIME_PMENDSCHEN, pmendschen)

    def calc_frc_spare_bugfix(self, model):
        #This method calculates the FRC_SPARE register. Bit 0 of this register is used to activate a workaround on
        #rev A1 which controls frc_rxframe_end_ahead_disable. This improves baudrate tol for TRECS PHYs that use DFL
        #with a 2B frame length field

        #Read in model vars
        demod_select = model.vars.demod_select.value
        var_length_shift = model.vars.var_length_shift.value #Bit position of dynamic length field
        var_length_numbits = model.vars.var_length_numbits.value #Number of bits in dynamic length field

        #Disable rxframe_end_ahead if TRECS and we are crossing a byte boundary w DFL field
        if (demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or
            demod_select == model.vars.demod_select.var_enum.TRECS_SLICER) and \
                ((var_length_numbits + var_length_shift) > 8): #Check for crossing of byte boundary
            frc_rxframe_end_ahead_disable = 1
        else:
            frc_rxframe_end_ahead_disable = 0

        #Write the reg
        self._reg_write(model.vars.FRC_SPARE_SPARE, frc_rxframe_end_ahead_disable) #No other useful bits in this field