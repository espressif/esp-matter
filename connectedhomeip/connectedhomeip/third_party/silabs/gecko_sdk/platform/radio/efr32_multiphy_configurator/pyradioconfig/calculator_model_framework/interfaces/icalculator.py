from abc import ABCMeta, abstractmethod
import inspect
import types
from pyradioconfig.calculator_model_framework.Utils.Version import Version
from pycalcmodel.core.variable import ModelVariable
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr
from host_py_rm_studio_internal.full.efr32xg1.revA2.static.common import errors

"""
Calculations interface file
"""
class ICalculator(object):

    _major = 0
    _minor = 0
    _patch = 0

    # Static variables
    actual_suffix = "_actual"

    """
    Returns profile version
    """
    @abstractmethod
    def getVersion(self):
        return Version(self._major, self._minor, self._patch)

    """
    Returns list of callable calc functions
    """
    @abstractmethod
    def getCalculationList(self):
        functionList = list()
        for functionName, functionPointer in inspect.getmembers(self, predicate=inspect.ismethod):
                if str(functionName).lower().startswith('calc_'):  # Check if the function name starts with "calc_"
                    if isinstance(functionPointer,types.MethodType):
                        #functionList.append(self.__class__.__name__ + "." + functionName)
                        functionList.append(functionPointer)
        return functionList

    """
    Populates a list of needed variables for this calculator
    """
    def buildVariables(self, modem_model):
        raise NotImplementedError('Call to abstract method getName()')

    """
    Adds variable to modem model
    """
    @abstractmethod
    def _addModelVariable(self, modem_model, name, var_type, format, desc=None, is_array=False,
                          forceable=True, units=None):
        var = ModelVariable(name, var_type, is_array, forceable=forceable)
        var.format = format

        if desc is not None:
            var.desc = desc

        if units is not None:
            var.units = units

        modem_model.vars.append(var)
        return var

    """
    Adds svd variable to modem model
    """
    @abstractmethod
    def _addModelRegister(self, modem_model, svd_mapping, var_type, format, desc=None, is_array=False,
                          forceable=True):
        name = svd_mapping.replace('.','_')
        var = ModelVariable(name, var_type, is_array, forceable=forceable)
        var.format = format

        if desc is not None:
            var.desc = desc

        var.svd_mapping = svd_mapping

        reg_model = getattr(modem_model, 'reg_model')
        if reg_model is not None:
            try:
                rm = reg_model.getObjectByName(svd_mapping)
            except:
                rm = None
            var.rm = rm

        modem_model.vars.append(var)
        return var

        
    """
    Adds reverse path "actual" variable to modem model
    
    A helper function is used to define these so we can be assured of at least having
    a specific naming convention.  Today we're only identifying reverse path 
    variables via the "actual" in the name.  By using this function, we could also
    add other flags, like setting a "don't allow this variable to be forced" bit.
    """
    @abstractmethod
    def _addModelActual(self, modem_model, name, var_type, format, desc=None, is_array=False):
        name_actual = name + self.actual_suffix
        var = ModelVariable(name_actual, var_type, is_array, forceable=False)
        var.format = format

        if desc is not None:
            var.desc = desc

        modem_model.vars.append(var)
        return var


    """
    Calculator Helper function
    """
    ### These are the main calls that should be use as they are flexible

    def _reg_write(self, varname, value=None, default=False, do_not_care=False, limit_upper=None,
                   limit_lower=None, check_saturation=False, allow_neg=False, neg_twos_comp=True):

        #First handle writing the register value

        if value is not None:
            #If there was a value passed in, use that
            #Negative values are not allowed, so if negative then use one's or two's compliment depending on neg_twos_comp
            if value >= 0:
               value_before_limiting = value
            else:
                if allow_neg:
                    if neg_twos_comp:
                        value_before_limiting = self._convert_twos_comp(value, varname.get_bit_width())
                    else:
                        value_before_limiting = self._convert_ones_comp(value, varname.get_bit_width())
                else:
                    LogMgr.Error('Error: attempting to assign negative value to %s when it is not allowed' % varname.name)

            #Now handle saturating or limiting the register value
            self._reg_limit_write(varname, value_before_limiting, limit_upper=limit_upper, limit_lower=limit_lower,
                                  check_saturation=check_saturation)

        elif default:
            self._reg_write_default(varname)

        #Now handle the don't care attribute
        if do_not_care:
            self._reg_do_not_care(varname)

    def _reg_write_by_name(self, model, reg_name_str, value=None, default=False, do_not_care=False,
                           limit_upper=None, limit_lower=None, check_saturation=False, neg_twos_comp=True):

        var = getattr(model.vars, reg_name_str)

        self._reg_write(var, value, default=default, do_not_care=do_not_care,
                        limit_upper=limit_upper, limit_lower=limit_lower, check_saturation=check_saturation,
                        neg_twos_comp=neg_twos_comp)

    def _reg_write_by_name_concat(self, model, reg_name_str1, reg_name_str2, value=None,
                                 default=False, do_not_care=False, limit_upper=None, limit_lower=None,
                                 check_saturation=False, neg_twos_comp=True):

        reg_name_str = reg_name_str1 + '_' + reg_name_str2

        self._reg_write_by_name(model, reg_name_str, value, default=default,
                                do_not_care=do_not_care, limit_upper=limit_upper, limit_lower=limit_lower,
                                check_saturation=check_saturation, neg_twos_comp=neg_twos_comp)

    ### These are lower-level calls that are used in some places in the calculator, but not recommended

    def _reg_write_default(self, varname):
        varname.set_to_reset_val()

    def _reg_do_not_care(self, varname):
        varname.value_do_not_care = True

    def _reg_limit_write(self, varname, value, limit_upper=None, limit_lower=None, check_saturation=False):

        #First check the value vs limit_upper and limit_lower parameters
        if limit_upper is not None:
            if value > limit_upper:
                value_after_upper_limit = limit_upper
                LogMgr.Debug("WARNING: Limit applied when writing to %s" % varname.name)
            else:
                value_after_upper_limit = value
        else:
            # No limit defined
            value_after_upper_limit = value

        if limit_lower is not None:
            if value_after_upper_limit < limit_lower:
                value_after_lower_limit = limit_lower
                LogMgr.Debug("WARNING: Limit applied when writing to %s" % varname.name)
            else:
                value_after_lower_limit = value_after_upper_limit
        else:
            value_after_lower_limit = value_after_upper_limit

        #If we want to check for saturation, then do that
        if check_saturation:
            self._reg_sat_write(varname,value_after_lower_limit)
        else:
            varname.value = value_after_lower_limit

    def _reg_sat_write(self, varname, value):
        bit_width = varname.get_bit_width()
        if value > (pow(2, bit_width) - 1):
            value = pow(2, bit_width) - 1
            LogMgr.Debug("WARNING: Saturation applied when writing to %s" % varname.name)
        varname.value = value

    def _convert_twos_comp(self, value, size):
        if value >= 0:
            return value
        else:
            return (1 << size) + value

    def _convert_ones_comp(self, value, size):
        if value >= 0:
            return value
        else:
            return (1 << (size-1)) - value
