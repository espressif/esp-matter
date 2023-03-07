
from pyradioconfig.parts.bobcat.calculators.calc_modulator import Calc_Modulator_Bobcat
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from pyradioconfig.parts.common.calculators.calc_utilities import CALC_Utilities

class calc_modulator_viper(Calc_Modulator_Bobcat):


    def calc_tx_baud_rate_actual(self, model):
        """
        calculate actual TX baud rate from register settings

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value
        txbr_ratio = model.vars.txbr_ratio_actual.value

        tx_baud_rate = fxo / 2.0 / 8.0 * txbr_ratio

        model.vars.tx_baud_rate_actual.value = tx_baud_rate

    def calc_tx_br2m(self, model):
        """
        calculate br2m value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        baudrate = model.vars.baudrate.value
        if baudrate == 2000000:
            br2m = 1
        else:
            br2m = 0

        model.vars.br2m.value = br2m
        self._reg_write(model.vars.MODEM_TXMISC_BR2M, int(br2m))


    def calc_modindex_value(self, model):
        """
        calculate MODINDEX value
        Equations from Table 5.25 in EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        fxo = model.vars.xtal_frequency.value * 1.0
        modformat = model.vars.modulation_type.value
        freq_dev_hz = model.vars.deviation.value * 1.0
        shaping_filter_gain = model.vars.shaping_filter_gain_iqmod_actual.value
        baudrate = model.vars.baudrate.value

        if modformat == model.vars.modulation_type.var_enum.OQPSK:
            modindex = round(0.5 / 2.0 / shaping_filter_gain / 8.0 * 2**12)/2**12
        elif baudrate == 2000000:
            gain_adj = 0.5/0.4830
            modindex = round(freq_dev_hz * gain_adj / shaping_filter_gain / (fxo /2.0) * 2**12) / 2**12
        else:
            gain_adj = 0.5 / 0.4858
            modindex = round(freq_dev_hz * gain_adj / shaping_filter_gain / (fxo/4.0) * 2 ** 12) / 2 ** 12

        model.vars.modindex.value = modindex


    def calc_modindex_field(self, model):
        """
        convert desired modindex fractional value to MODINDEXM * 2^MODINDEXE
        Equations (5.13) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        modindex = model.vars.modindex.value * 2**8

        # convert fractional modindex into m * 2^e format
        m, e = CALC_Utilities().frac2exp(255, modindex)
        #flip sign of e for iqmod implementation
        e = -e

        # MODEINDEXE is a signed value
        if e < 0:
            e += 32

        # verify number fits into register
        if m > 255:
            m = 255

        if e > 31:
            e = 31

        if m < 0:
            m = 0

        self._reg_write(model.vars.MODEM_MODINDEX_MODINDEXM,  int(m))
        self._reg_write(model.vars.MODEM_MODINDEX_MODINDEXE,  int(e))

    def calc_modindex_actual(self, model):
        """
        given register settings return actual MODINDEX as fraction
        Equations (5.13) of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        m = model.vars.MODEM_MODINDEX_MODINDEXM.value
        e = model.vars.MODEM_MODINDEX_MODINDEXE.value

        # MODEINDEXE is a signed value
        if e > 15:
            e -= 32

        model.vars.modindex_actual.value = 1.0 * m / 2**8 / 2**e

    def calc_tx_freq_dev_actual(self, model):
        """
        given register setting return actual frequency deviation used in the modulator
        Using Equations in Table 5.25 of EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        fxo = model.vars.xtal_frequency.value * 1.0
        modformat = model.vars.modulation_type.value
        modindex = model.vars.modindex_actual.value
        shaping_filter_gain = model.vars.shaping_filter_gain_iqmod_actual.value
        br2m = model.vars.br2m.value

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
           modformat == model.vars.modulation_type.var_enum.FSK4:
            freq_dev_hz = modindex * (fxo /2.0 * shaping_filter_gain) / 2**(1.0-br2m)
        else:
            freq_dev_hz = 0.0

        model.vars.tx_deviation_actual.value = freq_dev_hz

    def calc_txbr_value(self, model):
        #Overriding this function due to variable name change

        #Load model values into local variables
        xtal_frequency_hz = model.vars.xtal_frequency_hz.value
        baudrate = model.vars.baudrate.value

        #calculate baudrate to fxo ratio
        #For the phase modulator this should always be based on xtal rate (https://jira.silabs.com/browse/MCUW_RADIO_CFG-1626)
        ratio =  (8.0 * baudrate) / (xtal_frequency_hz / 2.0)
        #Load local variables back into model variables
        model.vars.txbr_ratio.value = ratio

    def calc_txbr_reg(self, model):
        """
        given desired TX baudrate ratio calculate TXBRNUM and TXBRDEN
        that gets as close as possible to the ratio.
        Note that we start from the highest possible value for TXBRDEN
        and go down since having largest possible values in these register
        to have better phase resolution in OQPSK and MSK (see end of section
        5.6.5 in the manual)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        ratio = model.vars.txbr_ratio.value
        self._reg_write(model.vars.MODEM_TXBR_TXBRNUM,  int(round(ratio * 2**16)))

    def calc_txbr_actual(self, model):
        """
        given register values calculate actual TXBR ratio implemented

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        num = model.vars.MODEM_TXBR_TXBRNUM.value * 1.0
        ratio = num / (2**16)
        model.vars.txbr_ratio_actual.value = ratio

