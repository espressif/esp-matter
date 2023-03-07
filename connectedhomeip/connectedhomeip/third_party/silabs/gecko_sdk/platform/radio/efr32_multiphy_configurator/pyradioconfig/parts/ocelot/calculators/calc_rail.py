from pyradioconfig.parts.lynx.calculators.calc_rail import CalcRailLynx

class CalcRailOcelot(CalcRailLynx):
    def calc_rail_delays(self, model):
        """calc_rail_delays

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """
        # Ocelot+ has a built-in RX chain delay value
        baud_delay = model.vars.delay_adc_to_demod_symbols.value \
                     * model.vars.baud_per_symbol_actual.value
        rx_delay_ns = int((baud_delay * 1e9) / model.vars.baudrate.value)
        sync_delay_ns = int((model.vars.MODEM_CTRL0_FRAMEDETDEL.value * 8 * 1e9) \
                            / model.vars.baudrate.value)

        # With no FRAMEDETDEL these values will be identical
        model.vars.rx_sync_delay_ns.value = rx_delay_ns + sync_delay_ns
        model.vars.rx_eof_delay_ns.value = rx_delay_ns

        # Default to no TX delay
        model.vars.tx_eof_delay_ns.value = 0

