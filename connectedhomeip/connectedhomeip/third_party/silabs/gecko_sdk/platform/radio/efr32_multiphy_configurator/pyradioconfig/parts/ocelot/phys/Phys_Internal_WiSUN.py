from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy
from pyradioconfig.parts.ocelot.phys.phys_studio_wisun_fan_1_0 import PhysStudioWisunFanOcelot
from pyradioconfig.parts.ocelot.phys.phys_studio_wisun_han import PhysStudioWisunHanOcelot
from py_2_and_3_compatibility import *

class PHYS_Internal_WiSUN_Ocelot(IPhy):

    # Reference \\silabs.com\mcuandwireless\026 Shared Docs\0260_Standards\std_body\IEEE_802_15_4\IEEE Std 802.15.4-2015.pdf
    # Ch 20. SUN FSK PHY

    # Owner: Created by Mark Gorday for lab-testing of Ocelot WiSUN using Sol AGC settings
    # JIRA Link:
    def PHY_Internal_WISUN_868MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        # modify Studio PHY
        phy = PhysStudioWisunFanOcelot().PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(model,phy_name='PHY_Internal_WISUN_868MHz_2GFSK_100kbps_2a_EU')

        # Sol xtal frequency of 40MHz
        phy.profile_inputs.xtal_frequency_hz.value = 40000000

        # Sol AGC Settings from OPT1
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.FULLRATE
        phy.profile_outputs.AGC_GAINSTEPLIM1_PNINDEXMAX.override = 16 # Ocelot PostSi JIRA 1253
        phy.profile_outputs.AGC_GAINRANGE_PNGAINSTEP.override = 3
        phy.profile_outputs.AGC_CTRL4_PERIODRFPKD.override = 4000
        phy.profile_outputs.AGC_CTRL4_RFPKDPRDGEAR.override = 4   # 6.5usec dispngainup period
        phy.profile_outputs.AGC_AGCPERIOD0_PERIODHI.override = 36
        phy.profile_outputs.AGC_AGCPERIOD1_PERIODLOW.override = 240      # STF cycle = 6 usec
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 29  # PERIODHI-SETTLETIMEIF-1
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION1.override = 100
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION2.override = 100
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION3.override = 100
        phy.profile_outputs.AGC_HICNTREGION1_HICNTREGION4.override = 100
        phy.profile_outputs.AGC_AGCPERIOD0_MAXHICNTTHD.override = 100  # > PERIODHI means disabled
        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 2
        phy.profile_outputs.AGC_CTRL0_DISCFLOOPADJ.override = 1

        return phy

    # Owner: Created by Mark Gorday for lab-testing of Ocelot WiSUN using Sol AGC settings
    # JIRA Link:
    def PHY_Internal_WISUN_915MHz_2GFSK_150kbps_3_NA(self, model, phy_name=None):
        # modify Studio PHY
        phy = PhysStudioWisunFanOcelot().PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(model,phy_name='PHY_Internal_WISUN_915MHz_2GFSK_150kbps_3_NA')

        # Sol xtal frequency of 40MHz
        phy.profile_inputs.xtal_frequency_hz.value = 40000000

        # Sol AGC Settings from OPT4
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.FULLRATE
        phy.profile_outputs.AGC_GAINSTEPLIM1_PNINDEXMAX.override = 16 # Ocelot PostSi JIRA 1253
        phy.profile_outputs.AGC_GAINRANGE_PNGAINSTEP.override = 3
        phy.profile_outputs.AGC_CTRL4_PERIODRFPKD.override = 4000
        phy.profile_outputs.AGC_CTRL4_RFPKDPRDGEAR.override = 2  # 24usec dispngainup period
        phy.profile_outputs.AGC_AGCPERIOD0_PERIODHI.override = 36
        phy.profile_outputs.AGC_AGCPERIOD1_PERIODLOW.override = 960      # STF cycle = 24 usec
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 29  # PERIODHI-SETTLETIMEIF-1
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION1.override = 100
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION2.override = 100
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION3.override = 100
        phy.profile_outputs.AGC_HICNTREGION1_HICNTREGION4.override = 100
        phy.profile_outputs.AGC_AGCPERIOD0_MAXHICNTTHD.override = 100  # > PERIODHI means disabled
        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 2
        phy.profile_outputs.AGC_CTRL0_DISCFLOOPADJ.override = 1

        return phy

    # Owner: Young-Joon Choi
    # For WiSun test with Antenna Diversity Enabled
    def PHY_Internal_WISUN_915MHz_2GFSK_150kbps_3_NA_antdiv(self, model, phy_name=None):
        # modify Studio phy
        phy = PhysStudioWisunFanOcelot().PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(model,
                                                                                            'PHY_Internal_WISUN_915MHz_2GFSK_150kbps_3_NA_antdiv')
        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.PHDEMODANTDIV
        phy.profile_inputs.skip2ant.value = model.vars.skip2ant.var_enum.SKIP2ANT

        model.vars.MODEM_REALTIMCFE_TRACKINGWIN.value_forced = 3
        model.vars.MODEM_TRECPMDET_PMMINCOSTTHD.value_forced = 180 # reduce per floor at AGC

    # Owner: Young-Joon Choi
    # For WiSun test with Antenna Diversity Enabled
    def PHY_Internal_WISUN_868MHz_2GFSK_50kbps_1a_EU_antdiv(self, model, phy_name=None):
        # modify Studio phy
        phy = PhysStudioWisunFanOcelot().PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(model,
                                                                                            'PHY_Internal_WISUN_868MHz_2GFSK_50kbps_1a_EU_antdiv')
        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.PHDEMODANTDIV
        phy.profile_inputs.skip2ant.value = model.vars.skip2ant.var_enum.SKIP2ANT

        model.vars.MODEM_PHDMODANTDIV_SKIPRSSITHD.value_forced = 248
        model.vars.MODEM_REALTIMCFE_TRACKINGWIN.value_forced = 3

        # Need to increase from calculated value. Otherwise hump near sensitivity if attenuation > 0
        model.vars.MODEM_BCRDEMODARR0_PHSPIKETHD.value_forced = 12

    # Owner: Young-Joon Choi
    # For WiSun test with Antenna Diversity Enabled
    def PHY_Internal_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET_antdiv(self, model, phy_name=None):
        # modify Studio phy
        phy = PhysStudioWisunHanOcelot().PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET(model, phy_name='PHY_Internal_WISUN_920MHz_2GFSK_50kbps_1b_JP_ECHONET_antdiv')
        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.PHDEMODANTDIV
        phy.profile_inputs.skip2ant.value = model.vars.skip2ant.var_enum.SKIP2ANT

        model.vars.MODEM_PHDMODANTDIV_SKIPRSSITHD.value_forced = 248
        model.vars.MODEM_REALTIMCFE_TRACKINGWIN.value_forced = 3

    # Owner: Young-Joon Choi
    # For WiSun test with Antenna Diversity Enabled
    def PHY_Internal_WISUN_868MHz_2GFSK_100kbps_2a_EU_antdiv(self, model, phy_name=None):
        # modify Studio PHY
        phy = PhysStudioWisunFanOcelot().PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(model,
                                                                                           phy_name='PHY_Internal_WISUN_868MHz_2GFSK_100kbps_2a_EU_antdiv')
        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.PHDEMODANTDIV
        phy.profile_inputs.skip2ant.value = model.vars.skip2ant.var_enum.SKIP2ANT

        model.vars.MODEM_REALTIMCFE_TRACKINGWIN.value_forced = 3

        model.vars.MODEM_TRECPMDET_PMMINCOSTTHD.value_forced = 200

    # Owner: Young-Joon Choi
    # For WiSun test with Antenna Diversity Enabled
    def PHY_Internal_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET_antdiv(self, model, phy_name=None):
        # modify Studio PHY
        phy = PhysStudioWisunHanOcelot().PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET(model, phy_name='PHY_Internal_WISUN_920MHz_2GFSK_100kbps_2b_JP_ECHONET_antdiv')

        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.PHDEMODANTDIV
        phy.profile_inputs.skip2ant.value = model.vars.skip2ant.var_enum.SKIP2ANT

        model.vars.MODEM_REALTIMCFE_TRACKINGWIN.value_forced = 3

    #Used for validation with FEC enabled. This is simply a pointer to the non-FEC PHY with one parameter set to TX with FEC.
    def PHY_ValOnly_WISUN_915MHz_2GFSK_50kbps_1b_NA_FEC(self, model, phy_name=None):
        phy = PhysStudioWisunFanOcelot().PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(model, phy_name='PHY_ValOnly_WISUN_915MHz_2GFSK_50kbps_1b_NA_FEC')
        phy.profile_inputs.fec_tx_enable.value = model.vars.fec_tx_enable.var_enum.ENABLED

        return phy

