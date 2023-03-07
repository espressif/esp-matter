
import os
from host_py_rm_studio_internal import rm_dynamic_import

_PKG_PATH = os.path.dirname(os.path.realpath(__file__))


class RM_EFR32XG28_Info(object):
    short_name = 'efr32xg28'
    val_die_name = 'EFR32XG28XFULL'
    arm_core_revs = []
    rtl_revs = ['A0']
    di_revs = []
    pte_revs = []
    pkg_path = _PKG_PATH
    all_revs = arm_core_revs + rtl_revs + di_revs + pte_revs


def RM_Factory(rev_name):
    return rm_dynamic_import('MARGAY', rev_name)


def RM_GetInfo():
    return RM_EFR32XG28_Info
