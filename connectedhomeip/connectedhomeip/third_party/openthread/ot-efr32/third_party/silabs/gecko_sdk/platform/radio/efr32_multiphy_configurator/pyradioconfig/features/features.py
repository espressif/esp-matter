"""
This class is where all the features are defined for pyradioconfig.
Features should not be defined anywhere else!
"""

from pycalcmodel.core.feature import ModelFeature

class Features(object):
    @staticmethod
    def build(model):
        """Builds features into model

        Args:
            model (MOdelRoot) : Data model that features are added to
        """
        Features.build_feature(model, 'TX', 'Transmit', value=True)
        Features.build_feature(model, 'RX', 'Receive', value=True)
        Features.build_feature(model, 'BLE', 'Bluetooth Low-Energy', value=True)
        Features.build_feature(model, 'SUBGIG', 'Proprietary Sub-GHz', value=True)
        Features.build_feature(model, 'GIG_2_4', 'Proprietary 2.4GHz', value=True)

    @staticmethod
    def build_feature(model, name, desc='', value=False):
        """Builds features into model

        Args:
            model (MOdelRoot) : Data model that features are added to
            name (str) : Name of new feature to add
            desc (str) : Feature description
            value (boolean) : Whether feature is enabled or disabled
        """
        model.features.append(ModelFeature(name, desc=desc, value=value))
