
"""
This module is copied into the register map package and is consumed 
by top-level device class to exclude the registers from an unfiltered 
dut.rm.dump() operation. The use can still dump these excluded 
registers by explicitly listing them in the regFilterList passed to
the dut.rm.dump() method. 

Note that this EXCLUDED_REGS list will support the following string formats:
  'PERIPHERAL'                 # all registers in this peripheral are excluded
  'PERIPHERAL.REGISTER'        # this register is excluded
  'PERIPHERAL.REGISTER.FIELD'  # the parent register is excluded

This list can be modified at runtime via the excludeDumpRegistersByName()
method. Use getExcludedDumpRegisters() method to get the current exclude
list. This list also removes registers from inclusion in the 
dut.rm.buildRegFilterList() method.

This list should not be confused with the excluded_reg attribute list in
the Filter class in the logic.py module. The Filter.excluded_reg list 
removes the register from the register map package altogether.

"""


# To modify this list, locate the master copy under the rm_filters\[format]
# package under the die rev subdirectories contained in the host_rm_svd_internal
# container repo.

EXCLUDED_REGS = [
]

