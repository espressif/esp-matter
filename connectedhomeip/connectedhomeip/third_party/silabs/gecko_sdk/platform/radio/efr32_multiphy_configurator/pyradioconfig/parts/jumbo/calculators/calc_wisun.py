from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

class CALC_WiSUN_Jumbo(ICalculator):

    def buildVariables(self, model):
        var = self._addModelVariable(model, 'wisun_mode', Enum, ModelVariableFormat.DECIMAL, 'Wi-SUN Operating Mode (PhyMode)')
        member_data = [
            ['Mode1a', 0, '2FSK 50kbps mi=0.5'],
            ['Mode1b', 1, '2FSK 50kbps mi=1.0'],
            ['Mode2a', 2, '2FSK 100kbps mi=0.5'],
            ['Mode2b', 3, '2FSK 100kbps mi=1.0'],
            ['Mode3',  4, '2FSK 150kbps mi=0.5'],
            ['Mode4a', 5, '2FSK 200kbps mi=0.5'],
            ['Mode4b', 6, '2FSK 200kbps mi=1.0'],
            ['Mode5',  7, '2FSK 300kbps mi=0.5']
            ]
        var.var_enum = CreateModelVariableEnum(
            'WiSUNModeEnum',
            'List of supported WiSUN Modes',
            member_data)

        var = self._addModelVariable(model, 'wisun_han_mode', Enum, ModelVariableFormat.DECIMAL,
                                     'Wi-SUN Operating Mode (PhyMode)')
        member_data = [
            ['Mode1b', 1, '2FSK 50kbps mi=1.0'],
            ['Mode2b', 3, '2FSK 100kbps mi=1.0'],
        ] #Leaving numbering the same as wisun_mode to allow for easy expansion
        var.var_enum = CreateModelVariableEnum(
            'WiSUNHanModeEnum',
            'List of supported WiSUN Modes',
            member_data)

        var = self._addModelVariable(model, 'wisun_reg_domain', Enum, ModelVariableFormat.HEX,
                                     'Wi-SUN Regulatory Domain')
        member_data = [
            ['WW', 0x00, 'Worldwide'],
            ['NA', 0x01, 'North America'],
            ['JP', 0x02, 'Japan'],
            ['EU', 0x03, 'Europe'],
            ['CN', 0x04, 'China'],
            ['IN', 0x05, 'India'],
            ['MX', 0x06, 'Mexico'],
            ['BZ', 0x07, 'Brazil'],
            ['AZ_NZ', 0x08, 'Australia and New Zealand'],
            ['KR', 0x09, 'Korea'],
            ['PH', 0x0A, 'Philippines'],
            ['MY', 0x0B, 'Malaysia'],
            ['HK', 0x0C, 'Hong Kong'],
            ['SG', 0x0D, 'Singapore'],
            ['TH', 0x0E, 'Thailand'],
            ['VN', 0x0F, 'Vietnam'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'WiSUNRegDomainEnum',
            'List of supported WiSUN Regulatory Domains',
            member_data)

        self._addModelVariable(model, 'wisun_operating_class', int, ModelVariableFormat.DECIMAL, desc='WiSUN Operating Class')
        self._addModelVariable(model, 'wisun_phy_mode_id', int, ModelVariableFormat.DECIMAL, desc='WiSUN PhyModeID', is_array=True)
        self._addModelVariable(model, 'wisun_mode_switch_phr', int, ModelVariableFormat.DECIMAL, desc='WiSUN Mode Switch PHR', is_array=True)


    def calc_wisun_freq_spacing(self, model):

        #This calculation only applies to Wi-SUN FSK (FAN/HAN)
        if model.profile.name.lower() in ['wisun_fan_1_0', 'wisun_han']:

            # Read the reg domain and operating class
            wisun_reg_domain = model.vars.wisun_reg_domain.value
            wisun_operating_class = model.vars.wisun_operating_class.value
            reg_domain_enum = model.vars.wisun_reg_domain.var_enum

            error = False
            if wisun_reg_domain == reg_domain_enum.CN:
                if wisun_operating_class == 1:
                    base_frequency_hz = 470.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 779.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 3:
                    base_frequency_hz = 779.4e6
                    channel_spacing_hz = 400e3
                elif wisun_operating_class == 4:
                    base_frequency_hz = 920.625e6
                    channel_spacing_hz = 250e3
                else:
                    error = True
                    base_frequency_hz = 470.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.EU:
                if wisun_operating_class == 1:
                    base_frequency_hz = 863.1e6
                    channel_spacing_hz = 100e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 863.1e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 3:
                    base_frequency_hz = 870.1e6
                    channel_spacing_hz = 100e3
                elif wisun_operating_class == 4:
                    base_frequency_hz = 870.2e6
                    channel_spacing_hz = 200e3
                else:
                    error = True
                    base_frequency_hz = 863.1e6
                    channel_spacing_hz = 100e3
            elif wisun_reg_domain == reg_domain_enum.IN:
                if wisun_operating_class == 1:
                    base_frequency_hz = 865.1e6
                    channel_spacing_hz = 100e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 865.1e6
                    channel_spacing_hz = 200e3
                else:
                    error = True
                    base_frequency_hz = 865.1e6
                    channel_spacing_hz = 100e3
            elif wisun_reg_domain == reg_domain_enum.SG:
                if wisun_operating_class == 1:
                    base_frequency_hz = 866.1e6
                    channel_spacing_hz = 100e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 866.1e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 3:
                    base_frequency_hz = 866.3e6
                    channel_spacing_hz = 400e3
                elif wisun_operating_class == 4:
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class== 5:
                    base_frequency_hz = 920.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 866.1e6
                    channel_spacing_hz = 100e3
            elif wisun_reg_domain == reg_domain_enum.MX:
                if wisun_operating_class == 1:
                    base_frequency_hz = 902.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 902.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 902.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.NA:
                if wisun_operating_class == 1:
                    base_frequency_hz = 902.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 902.4e6
                    channel_spacing_hz = 400e3
                elif wisun_operating_class == 3:
                    base_frequency_hz = 902.6e6
                    channel_spacing_hz = 600e3
                else:
                    error = True
                    base_frequency_hz = 902.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.BZ:
                if wisun_operating_class == 1:
                    base_frequency_hz = 902.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 902.4e6
                    channel_spacing_hz = 400e3
                elif wisun_operating_class == 3:
                    base_frequency_hz = 902.6e6
                    channel_spacing_hz = 600e3
                else:
                    error = True
                    base_frequency_hz = 902.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.AZ_NZ:
                if wisun_operating_class == 1:
                    base_frequency_hz = 915.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 915.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 915.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.KR:
                if wisun_operating_class == 1:
                    base_frequency_hz = 917.1e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 917.3e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 917.1e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.PH:
                if wisun_operating_class == 1:
                    base_frequency_hz = 915.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 915.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 915.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.MY:
                if wisun_operating_class == 1:
                    base_frequency_hz = 919.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 919.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 919.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.HK:
                if wisun_operating_class == 1:
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 920.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.TH:
                if wisun_operating_class == 1:
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 920.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.VN:
                if wisun_operating_class == 1:
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 920.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 920.2e6
                    channel_spacing_hz = 200e3
            elif wisun_reg_domain == reg_domain_enum.JP:
                if wisun_operating_class == 1:
                    base_frequency_hz = 920.6e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 920.9e6
                    channel_spacing_hz = 400e3
                elif wisun_operating_class == 3:
                    base_frequency_hz = 920.8e6
                    channel_spacing_hz = 600e3
                else:
                    error = True
                    base_frequency_hz = 920.6e6
                    channel_spacing_hz = 200e3
            else: #WW
                if wisun_operating_class == 1:
                    base_frequency_hz = 2400.2e6
                    channel_spacing_hz = 200e3
                elif wisun_operating_class == 2:
                    base_frequency_hz = 2400.4e6
                    channel_spacing_hz = 400e3
                else:
                    error = True
                    base_frequency_hz = 2400.2e6
                    channel_spacing_hz = 200e3

            if error:
                LogMgr.Error("Error: Unsupported Wi-SUN Operating Class for selected Regulatory Domain")

            #Write the model vars
            model.vars.base_frequency_hz.value = int(base_frequency_hz)
            model.vars.channel_spacing_hz.value = int(channel_spacing_hz)

    def calc_wisun_phy_mode_id(self, model):
        # This function calculates the PhyModeID for Wi-SUN OFDM and Wi-SUN FSK PHYs
        # For OFDM, the variable contains an array of MCS0-6 values
        # For FSK, the variable contains an array of FEC off/on values

        profile_name = model.profile.name.lower()

        if "wisun" in profile_name:
            wisun_mode = model.vars.wisun_mode.value
            wisun_phy_mode_id = [0] * 2
            phy_mode =  int(wisun_mode) + 1
            wisun_phy_mode_id[0] = (0 << 4) | phy_mode
            wisun_phy_mode_id[1] = (1 << 4) | phy_mode

            #Write the variable
            model.vars.wisun_phy_mode_id.value = wisun_phy_mode_id

    def calc_wisun_mode_switch_phr(self, model):
        #This function calculates the PHR associated with the calculated PhyModeID
        #For OFDM, the variable contains an array of MCS0-6 values
        #For FSK, the variable contains an array of FEC off/on values

        profile_name = model.profile.name.lower()

        if "wisun" in profile_name:
            wisun_phy_mode_id = model.vars.wisun_phy_mode_id.value
            wisun_mode_switch_phr = []
            for phy_mode_id in wisun_phy_mode_id:
                wisun_mode_switch_phr.append(self._make_phr(phy_mode_id))

            #Write the variable
            model.vars.wisun_mode_switch_phr.value = wisun_mode_switch_phr

    def _make_phr(self, phy_mode_id):

        wisun_bch_gen_poly = 0b10011

        mode_switch_field = 0b1
        mode_switch_shift = 15

        reserved_field = 0b00
        reserved_shift = 13

        phy_mode_id_field = phy_mode_id
        phy_mode_id_shift = 5

        first_11_bits = ((mode_switch_field << mode_switch_shift) | (reserved_field << reserved_shift) | (phy_mode_id_field << phy_mode_id_shift)) >> 5
        checksum_field = self._get_bch_15_11_checksum(first_11_bits, wisun_bch_gen_poly)
        checksum_shift = 1

        first_15_bits = ((first_11_bits << 5) | (checksum_field << checksum_shift)) >> 1
        parity_field = self._get_parity(first_15_bits)
        parity_shift = 0

        phr = (first_15_bits << 1) | (parity_field << parity_shift)

        #Now we need to reverse the order of bits for transmission
        phr = int(format(phr, '016b')[::-1],2)

        return phr

    def _get_bch_15_11_checksum(self, data, gen_poly):
        #This function calculates the BCH checksum, which is the shifted data divided by the generator polynomial

        data_shifted = data << 4 #The 4 here comes from the BCH(15,11) definition as 4 = 15-11

        data_bits = len("{0:b}".format(data_shifted))
        gen_poly_bits = len("{0:b}".format(gen_poly))

        #Initialize checksum
        checksum = data_shifted

        #Shift gen polynomial to left align with data
        gen_poly_shifted = gen_poly << (data_bits - gen_poly_bits)

        for bit_position in range (data_bits-gen_poly_bits+1):
            if (checksum & (1<<(data_bits-bit_position-1))) != 0:
                #Only XOR if there is a 1 in the MSB of the current checksum
                checksum = checksum ^ (gen_poly_shifted >> bit_position)

        return checksum

    def _get_parity(self, data):

        #Optimized algorithm from Sophie Gault
        parity = 0
        while (data != 0):
            parity ^= 1  # toggle parity bit
            data = data & (data - 1)
        return parity
