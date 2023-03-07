
from pycalcmodel.core.variable import ModelVariable
from enum import Enum
import ast

from pycalcmodel.py2_and_3_compatibility import *

__all__ = [ 'get_xml_str_values',
            'cast_value_from_xml',
            'get_dummy_var_value' ]

DUMMY_VAR_VALUE = {
    bool    : False,
    complex : 0j,
    float   : 0.0,
    int     : 0,
    long    : long(0),
    str     : '',
}


def get_xml_str_values(is_array, value):
    if value is None:
        return None
    elif is_array:
        values_str = [ str(x) for x in value ]
    else:
        values_str = [ str(value) ]
    return values_str


def cast_value_from_xml(var, xml_value):
    #print("variable {} with type {} and value {}".format(var.name, type(xml_value), xml_value))
    assert isinstance(var, ModelVariable), "FATAL ERROR: var must be ModelVariable instance"
    assert isinstance(xml_value, list), "FATAL ERROR: xml_value should be a list"
    if xml_value == []:
        return None
    elif xml_value == [ None ]:
        return None
    elif var.var_type in (complex, float, int, long, str):
        if var.is_array:
            values = []
            for str_value in xml_value:
                values.append((var.var_type)(str_value))
            return values
        else:
            if len(xml_value) == 0:
                return None
            else:
                return (var.var_type)(xml_value[0])
    elif var.var_type is bool:
        if var.is_array:
            values = []
            for str_value in xml_value:
                values.append(ast.literal_eval(str_value))
            return values
        else:
            if len(xml_value) == 0:
                return None
            else:
                return ast.literal_eval(xml_value[0])
    elif var.var_type == Enum:
        if (len(xml_value[0].split('.')) != 2):
            raise ValueError("Invalid enum XML value '{}' for " \
                             "variable '{}' with enum '{}'".format(xml_value[0],
                                                                   var.name,
                                                                   var.var_enum.__name__))
        else:
            enum_name, member_name = xml_value[0].split('.')
            if enum_name != var.var_enum.__name__:
                raise ValueError("Invalid enum name '{}' for " \
                                 "variable '{}' with enum '{}'".format(enum_name,
                                                                       var.name,
                                                                       var.var_enum.__name__))
            elif member_name not in var.var_enum.__members__:
                raise ValueError("Invalid enum member '{}' for " \
                                 "variable '{}' with enum '{}'".format(member_name,
                                                                       var.name,
                                                                       var.var_enum.__name__))
            else:
                return getattr(var.var_enum, member_name)
    else:
        raise ValueError("Invalid XML value '' for var_type '{}'".format(xml_value, var.var_type))


def get_dummy_var_value(var):
    assert isinstance(var, ModelVariable)
    if var.var_type == Enum:
        # get first value in the enum
        return var.var_enum.__members__.items()[0][1].value
    else:
        # get a standard dummy value
        return DUMMY_VAR_VALUE[var.var_type]

