"""
Wrapper layer (adapter) for host_py_radio_config to be consumed by Simplicity Studio.
This wrapper allows the "radio configurator" to support the Pro2 style dictionary interface, where the input and output values are defined by key/value pairs.
"""
from set_pymath_paths import *
from .efr32multiphyconfigurator import Efr32MultiPHYConfigurator
from .efr32multiphycfginput import Efr32MultiPHYCfgInput
from .efr32multiphycategory import Efr32MultiPHYCategory
