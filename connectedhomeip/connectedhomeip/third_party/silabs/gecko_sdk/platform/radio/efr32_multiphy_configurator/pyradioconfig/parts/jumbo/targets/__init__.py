"""
Targets

How to add a new Target:
--------------------------
*  Add a new Python file in this directory (example: Target_[Name].py)
*  Inside the Py file, create a class that implements ITarget:

    >>> class Target_IC(ITarget):

*  Implement/override target_calculate() function.  This function sets and target specific config.
    Example:

    >>>     def target_calculate(self, model):
    >>>         # Add target-specific forces here
    >>>         model.vars.if_frequency_hz.value_forced = 0
    >>>         model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.FULLRATE
    >>>         model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT
    >>>

"""
import os
import glob
modules = glob.glob(os.path.dirname(__file__)+"/*.py")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*.pyc")
if len(modules) == 0:
    modules = glob.glob(os.path.dirname(__file__)+"/*$py.class")
__all__ = [ os.path.basename(f)[:-3] for f in modules]
