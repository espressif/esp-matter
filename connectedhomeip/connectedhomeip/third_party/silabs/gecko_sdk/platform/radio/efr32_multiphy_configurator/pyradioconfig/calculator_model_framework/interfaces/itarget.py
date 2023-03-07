from abc import ABCMeta, abstractmethod

from py_2_and_3_compatibility import *

"""
Profile interface file
"""
class ITarget(object):

    _targetName = "" #Contains the name of the target (this is the variable stored under CalcManager to designate the execution target)
    _description = "" #Verbose description of the target
    _store_config_output = False #Controls whether or not we store "golden CFGs" in the config_output folder for this part + target
    _cfg_location = "" #Location of the target CFGs under both calculated_output and config_output (if stored)
    _tag = "" #This tag can be used by individual PHYs to indicate target compatibility (e.g. '-FPGA' indicates will not run on FPGA)
    IC_str = "IC" #Constant that we can use to refer to "IC" across the calculator
    SIM_str = "Sim" #Constant that we can use to refer to "Sim" across the calculator

    """
    Returns target readable and searchable name
    """
    @abstractmethod
    def getName(self):
        # Since this is used in code to reference the target, it cannot have white space or spaces
        name = self._targetName.strip()
        name = name.replace(" ", "_")
        return name

    """
    Returns target description
    """
    @abstractmethod
    def getDescription(self):
        return self._description

    """
    Returns target CFG location and storage info
    """
    @abstractmethod
    def getCFGInfo(self):
        return self._cfg_location,self._store_config_output

    """
    Returns target tag
    """
    @abstractmethod
    def getTargetTag(self):
        return self._tag

    """
    Run target specific configuration
    Returns: Populated profile model
    """
    @abstractmethod
    def target_calculate(self, model):
        pass


