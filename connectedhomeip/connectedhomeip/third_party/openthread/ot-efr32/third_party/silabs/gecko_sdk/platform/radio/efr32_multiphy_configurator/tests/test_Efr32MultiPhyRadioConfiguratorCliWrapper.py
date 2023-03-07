import os
from Efr32MultiPhyRadioConfiguratorCliWrapper import generate
import unittest

class TestEfr32MultiPhyRadioConfiguratorCliWrapper(unittest.TestCase):

    def test_auto_denali_gecko_radio_config_updated(self):
        current_dir = os.path.dirname(__file__)
        xmlFilePath = os.path.join(current_dir, r"input_files/auto-denali_gecko-radio-config_updated.xml")
        targetDirPath = os.path.join(current_dir, r"output_files")

        generate(xmlFilePath, targetDirPath)


    def test_auto_denali_gecko_radio_config(self):
        current_dir = os.path.dirname(__file__)
        xmlFilePath = os.path.join(current_dir, r"input_files/auto-denali_gecko-radio-config.xml")
        targetDirPath = os.path.join(current_dir, r"output_files")

        generate(xmlFilePath, targetDirPath)

