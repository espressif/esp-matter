
import os

# from pyrmsvd import *
from pycalcmodel.core.output import ModelOutputType
from pyradioconfig.to_be_deprecated.cmsis_data import EFR32XG1XFULL_REG_ADDR_NAME_MAP

#
#
#
def build_rm_object(part_family='dumbo'):
    part_family = part_family.lower()
    # if os.name == "nt":
    # create the register map object

    # MSW: need library py_sys_predator to do this...
    # But could host_py_rm_studio_internal suffice instead?

    from host_py_rm_studio_internal import RM_Factory
    rm_device_class = RM_Factory(part_family.upper(), None)
    rm = rm_device_class(None, 'dut_1')

    # elif os.name == "posix":
    #     if part_family == 'dumbo':
    #         SVD_FN = '../f038_radio_config/scripts/EFR32XG1XFULL_SEQ.svd'
    #     elif part_family == 'jumbo':
    #         # SVD_FN = '../f038_radio_config/scripts/EFR32XG2XFULL_SEQ.svd'
    #         SVD_FN = '../f004_register_generator/autogen_vc/toolchains/xml-cmsissvd/EFR32XG3XFULL.svd'
    #
    #
    #     DUT_LABEL = 'dut_1'
    #
    #     # Here we have a direct JLINK ARM connection
    #     accessMgr = Offline_AccessManager(DUT_LABEL)
    #
    #     # register the SVD reader/writer functions
    #     rmIO = RegisterMap_IO(accessMgr.ReadRegister,
    #                       accessMgr.WriteRegister)
    #
    #     # create the register map object
    #     rm = RM_Device(SVD_FN, rmIO, DUT_LABEL)

    return rm


#
# I really wish the register model had this method.  :(
#
def rm_read_field(rm, register_name):
    register_eval_string = "rm." + register_name + ".io"
    register_read_value = eval(register_eval_string)
    return register_read_value

def rm_read_field_reset_value(rm, register_name):
    register_eval_string = "(((rm." + register_name + ".zz_reg.resetValue & rm." + register_name + ".zz_reg.resetMask))  >> rm." + register_name + ".bitOffset) & (pow(2,rm." + register_name + ".bitWidth) -1)"
    register_read_reset_value = eval(register_eval_string )
    return register_read_reset_value

    
#
# I really wish the register model had this method.  :(
#
def rm_write_field(rm, fieldname, value):
        #assert (value is not None), "value of %s was None" % fieldname
        if value is not None:
            register_write_exec_string = "rm." + fieldname + ".io=" + str(value)
            try:
                exec(register_write_exec_string)
            except Exception as e:
                print("Error rm_write_field: {0}".format(register_write_exec_string))
                raise e



#
#
#
def regname_from_fieldname (fieldname):
    (block, reg, field) = fieldname.split('.')
    return block + '.' + reg
    

def regreset_from_fieldname (rm, fieldname):
    register_eval_string = "rm." + regname_from_fieldname(fieldname) + ".resetValue"
    register_read_reset_value = eval(register_eval_string)
    return register_read_reset_value
    

def resetmask_from_fieldname (rm, fieldname):
    register_eval_string = "((rm." + fieldname + ".zz_reg.resetMask >> rm." + fieldname + ".bitOffset) & (pow(2,rm." + fieldname + ".bitWidth) - 1 )) << rm." + fieldname + ".bitOffset"
    register_read_mask_value = eval(register_eval_string)
    return(register_read_mask_value)


def fieldname_from_fieldname (fieldname):
    (block, reg, field) = fieldname.split('.')
    return field
    

def regname_from_addr(addr):
    register = EFR32XG1XFULL_REG_ADDR_NAME_MAP[int(addr, 16)]
    
    return register
    

def is_readonly(fieldname):
    return 0                # fixme:  Make this routine work
    

#
# This is called with a model as an input and it points to a register model
# that could be a real dut or not.  It writes the profile output fields to the
# register model, then reads back any register that it wrote.  It returns
# a dictionary of registers and values.
#
def model_to_register_dict(model, rm=None):

    # Build a register model if one wasn't specified
    if rm == None:
        rm = build_rm_object(model.part_family)
    
    register_dict = dict()

    # write the registers to the rm
    for output in model.profile.get_outputs([ModelOutputType.SVD_REG_FIELD]):
        if output.var_value is not None:
            fieldname = output.var.svd_mapping
            register_dict[regname_from_fieldname(fieldname)] = ''        # Add this register to the register dictionary
            rm_write_field(rm, fieldname, output.var_value)

    # Read them back
    for register_name in register_dict.keys():
        register_dict[register_name] = rm_read_field(rm, register_name)

    return register_dict
    
# Similar to model_to_register_dict, but creates a register dictionary with fields.
# Don't need to create intermediate rm.
def model_to_register_dict_expand(model, rm=None):

    # Build a register model if one wasn't specified
    if rm == None:
        rm = build_rm_object(model.part_family)
    
    register_dict = dict()

    # write the fields to the dictionary
    for output in model.profile.get_outputs([ModelOutputType.SVD_REG_FIELD]):
        if (output.var_value is not None) and (output.var.value_do_not_care == False):
            fieldname = output.var.svd_mapping
            regname = regname_from_fieldname(fieldname)
            if regname not in register_dict:
                register_dict[regname] = dict()
                register_dict[regname]["zzreset"] = regreset_from_fieldname(rm, fieldname)
            register_dict[regname][fieldname_from_fieldname(fieldname)] = output.var_value
            register_dict[regname]["zzreset"] = register_dict[regname]["zzreset"] & ((1 << 32) + ~resetmask_from_fieldname(rm, fieldname))

    return register_dict
    
