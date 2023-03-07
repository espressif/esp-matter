import imp
import os
import datetime

from host_py_rm_studio_internal import RM_Factory
from pyradioconfig.to_be_deprecated.rm_io import *

# Useful tools to load a Python register model (RM) and list all fields, reset values, etc. in a format that is easily
# diff-able.


def rm_register_list():
    family = 'OCELOT'
    for rev in ['A0']:
        factory_rev = RM_Factory(family, rev)
        rm = factory_rev()
        # rm.dump('c:/temp/lynx_offline_RM_dump.txt')


        rm_all_register_fields = list()
        rm_all_register_fields_reset_values = list()
        # parse periphs
        for periph in rm.zz_pdict:
            for reg in eval('rm.{}'.format(periph)).zz_rdict:
                for field in eval('rm.{}.{}'.format(periph, reg)).zz_fdict:
                    rm_all_register_fields.append('{}_{}_{}'.format(periph, reg, field))
                    rm_all_register_fields_reset_values.append(rm_read_field_reset_value(rm,'{}.{}.{}'.format(periph, reg, field)))

    # generate a .CSV of
    # PERIPH_REG_FIELD, RESETVALUE,
    output_file_path =  r'rm_register_list_output_{}_{}.csv'.format(family, datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S"))
    fp = open(output_file_path, "w")

    # generate a .py of
    # self._reg_write(model.vars.PERIPH_REG_FIELD, RESETVALUE)
    output_file_path =  r'rm_register_list_output_write_reset_value_{}_{}.csv'.format(family, datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S"))
    fp_write_reset_value = open(output_file_path, "w")

    # generate a. py file of
    # profile.outputs.append(ModelOutput(model.vars.PERIPH_REG_FIELD, '',         ModelOutputType.SVD_REG_FIELD, readable_name='PERIPH.REG.FIELD'           ))
    output_file_path =  r'rm_register_list_profile_outputs_append_{}_{}.py'.format(family, datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S"))
    fp_write_profile_outputs_append = open(output_file_path, "w")

    # generate a .py file of
    # self._addModelRegister(model, 'AGC.AGCPERIOD.MAXHICNTTHD', int, ModelVariableFormat.HEX)
    output_file_path =  r'rm_register_list_add_model_register_{}_{}.py'.format(family, datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S"))
    fp_write_add_model_register = open(output_file_path, "w")

    for x in rm_all_register_fields:
        value = rm_read_field_reset_value(rm,x.replace('_','.'))
        # value = eval('rm.{}.{}.{}.io'.format(periph, reg, field))

        # PERIPH_REG_FIELD, RESETVALUE,
        fp.write(x + ',' + str(value) + ',\n')

        # self._reg_write(model.vars.PERIPH_REG_FIELD, RESETVALUE)
        fp_write_reset_value.write('self._reg_write(model.vars.{}, {})'.format(x,str(value)) + '\n')

        # profile.outputs.append(ModelOutput(model.vars.PERIPH_REG_FIELD, '',         ModelOutputType.SVD_REG_FIELD, readable_name='PERIPH.REG.FIELD'           ))
        fp_write_profile_outputs_append.write('profile.outputs.append(ModelOutput(model.vars.{}, \'\',         ModelOutputType.SVD_REG_FIELD, readable_name=\'{}\'           ))\n'.format(x,x.replace('_','.')))

        # self._addModelRegister(model, 'AGC.AGCPERIOD.MAXHICNTTHD', int, ModelVariableFormat.HEX)
        fp_write_add_model_register.write('self._addModelRegister(model, \'{}\', int, ModelVariableFormat.HEX)\n'.format(x.replace('_','.')))


    fp.close()
    fp_write_reset_value.close()
    fp_write_profile_outputs_append.close()
    fp_write_add_model_register.close()


if __name__ == '__main__':
    rm_register_list()