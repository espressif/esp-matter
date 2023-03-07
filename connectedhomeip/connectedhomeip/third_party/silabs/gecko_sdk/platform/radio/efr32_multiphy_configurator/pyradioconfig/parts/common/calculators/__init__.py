"""
This is where the core (default / global) calcaultor functions are defined.

How to add a new calculator:
--------------------------
*  Add a new Python file in this directory (example: calc_[type].py)
*  Inside the Py file, create a class that implements ICalculator:

    >>> class CALC_[TYPE](ICalculator):

*  Override/implement the buildVariables() function, where varaibles for this calcaultor type are defined.

    >>> def buildVariables(self, model):
    >>>    self._addModelVariable(model,'cost_bandwidth', float, ModelVariableFormat.DECIMAL)

*  Add algorithm (calcaultor functions) that start with calc_[algoritm]().
    First parameter should be the data model that it will read and write from.

    >>> def calc_baudrate(self, model):

*  Global Calculators can be overridden by part specific functions.

*  Part specific implementations of calcaultion functions do not live here, but live in [part family] specific directories

"""

import os
import glob
modules = glob.glob(os.path.dirname(__file__)+"/*.py")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*.pyc")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*$py.class")
__all__ = [ os.path.basename(f)[:-3] for f in modules]