# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Cortex-M architecture related constants."""

import collections

# Cortex-M (ARMv7-M + ARMv8-M) related constants.
# These values are from the ARMv7-M Architecture Reference Manual DDI 0403E.b
# and ARMv8-M Architecture Reference Manual DDI 0553A.e.
# https =//static.docs.arm.com/ddi0403/e/DDI0403E_B_armv7m_arm.pdf
# https =//static.docs.arm.com/ddi0553/a/DDI0553A_e_armv8m_arm.pdf

# Exception ISR number. (ARMv7-M Section B1.5.2)
# When the ISR number (accessible from ICSR and PSR) is zero, it indicates the
# core is in thread mode.
PW_CORTEX_M_THREAD_MODE_ISR_NUM = 0x0
PW_CORTEX_M_NMI_ISR_NUM = 0x2
PW_CORTEX_M_HARD_FAULT_ISR_NUM = 0x3
PW_CORTEX_M_MEM_FAULT_ISR_NUM = 0x4
PW_CORTEX_M_BUS_FAULT_ISR_NUM = 0x5
PW_CORTEX_M_USAGE_FAULT_ISR_NUM = 0x6

# Masks for Interrupt Control and State Register ICSR (ARMv7-M Section B3.2.4)
PW_CORTEX_M_ICSR_VECTACTIVE_MASK = (1 << 9) - 1

# Masks for individual bits of HFSR. (ARMv7-M Section B3.2.16)
PW_CORTEX_M_HFSR_FORCED_MASK = 0x1 << 30

# Masks for different sections of CFSR. (ARMv7-M Section B3.2.15)
PW_CORTEX_M_CFSR_MEM_FAULT_MASK = 0x000000FF
PW_CORTEX_M_CFSR_BUS_FAULT_MASK = 0x0000FF00
PW_CORTEX_M_CFSR_USAGE_FAULT_MASK = 0xFFFF0000

# Masks for individual bits of CFSR. (ARMv7-M Section B3.2.15)
# Memory faults (MemManage Status Register) =
PW_CORTEX_M_CFSR_MEM_FAULT_START = 0x1
PW_CORTEX_M_CFSR_IACCVIOL_MASK = PW_CORTEX_M_CFSR_MEM_FAULT_START << 0
PW_CORTEX_M_CFSR_DACCVIOL_MASK = PW_CORTEX_M_CFSR_MEM_FAULT_START << 1
PW_CORTEX_M_CFSR_MUNSTKERR_MASK = PW_CORTEX_M_CFSR_MEM_FAULT_START << 3
PW_CORTEX_M_CFSR_MSTKERR_MASK = PW_CORTEX_M_CFSR_MEM_FAULT_START << 4
PW_CORTEX_M_CFSR_MLSPERR_MASK = PW_CORTEX_M_CFSR_MEM_FAULT_START << 5
PW_CORTEX_M_CFSR_MMARVALID_MASK = PW_CORTEX_M_CFSR_MEM_FAULT_START << 7
# Bus faults (BusFault Status Register) =
PW_CORTEX_M_CFSR_BUS_FAULT_START = 0x1 << 8
PW_CORTEX_M_CFSR_IBUSERR_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 0
PW_CORTEX_M_CFSR_PRECISERR_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 1
PW_CORTEX_M_CFSR_IMPRECISERR_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 2
PW_CORTEX_M_CFSR_UNSTKERR_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 3
PW_CORTEX_M_CFSR_STKERR_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 4
PW_CORTEX_M_CFSR_LSPERR_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 5
PW_CORTEX_M_CFSR_BFARVALID_MASK = PW_CORTEX_M_CFSR_BUS_FAULT_START << 7
# Usage faults (UsageFault Status Register) =
PW_CORTEX_M_CFSR_USAGE_FAULT_START = 0x1 << 16
PW_CORTEX_M_CFSR_UNDEFINSTR_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 0
PW_CORTEX_M_CFSR_INVSTATE_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 1
PW_CORTEX_M_CFSR_INVPC_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 2
PW_CORTEX_M_CFSR_NOCP_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 3
PW_CORTEX_M_CFSR_STKOF_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 4
PW_CORTEX_M_CFSR_UNALIGNED_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 8
PW_CORTEX_M_CFSR_DIVBYZERO_MASK = PW_CORTEX_M_CFSR_USAGE_FAULT_START << 9

# TODO(amontanez): We could probably make a whole module on bit field handling
# in python.
BitField = collections.namedtuple(
    'BitField', ['name', 'bit_mask', 'description', 'long_description']
)

# Information about faults from:
# * ARM Cortex-M4 Devices Generic User Guide 4.3.10
# * ARM Cortex-M33 Devices Generic User Guide 4.2.11

PW_CORTEX_M_CFSR_BIT_FIELDS = [
    BitField(
        'IACCVIOL',
        PW_CORTEX_M_CFSR_IACCVIOL_MASK,
        'Instruction access violation fault.',
        (
            'The processor attempted an instruction fetch from a location',
            'that does not permit execution. The PC value stacked for the',
            'exception return points to the faulting instruction.',
        ),
    ),
    BitField(
        'DACCVIOL',
        PW_CORTEX_M_CFSR_DACCVIOL_MASK,
        'Data access violation fault.',
        (
            'The processor attempted a load or store at a location that',
            'does not permit the operation. The PC value stacked for the',
            'exception return points to the faulting instruction. The',
            'processor has loaded the MMFAR with the address of the',
            'attempted access.',
        ),
    ),
    BitField(
        'MUNSTKERR',
        PW_CORTEX_M_CFSR_MUNSTKERR_MASK,
        'MemManage fault on unstacking for a return from exception.',
        (
            'Unstack for an exception return has caused one or more access',
            'violations. This fault is chained to the handler. This means',
            'that when this bit is 1, the original return stack is still',
            'present. The processor has not adjusted the SP from the',
            'failing return, and has not performed a new save. The',
            'processor has not written a fault address to the MMAR.',
        ),
    ),
    BitField(
        'MSTKERR',
        PW_CORTEX_M_CFSR_MSTKERR_MASK,
        'MemManage fault on stacking for exception entry.',
        (
            'When this bit is 1, the SP is still adjusted but the values',
            'in the context area on the stack might be incorrect. The',
            'processor has not written a fault address to the MMAR.',
        ),
    ),
    BitField(
        'MLSPERR',
        PW_CORTEX_M_CFSR_MLSPERR_MASK,
        'MemManage Fault during FPU lazy state preservation.',
        '',
    ),
    BitField(
        'MMARVALID',
        PW_CORTEX_M_CFSR_MMARVALID_MASK,
        'MMFAR register is valid.',
        '',
    ),
    BitField(
        'IBUSERR',
        PW_CORTEX_M_CFSR_IBUSERR_MASK,
        'Instruction bus error.',
        (
            'The processor attempted to issue an invalid instruction. It',
            'detects the instruction bus error on prefetching, but this',
            'flag is only set to 1 if it attempts to issue the faulting',
            'instruction. When this bit is set, the processor has not',
            'written a fault address to the BFAR.',
        ),
    ),
    BitField(
        'PRECISERR',
        PW_CORTEX_M_CFSR_PRECISERR_MASK,
        'Precise data bus error.',
        (
            'A data bus error has occurred, and the PC value stacked for',
            'the exception return points to the instruction that caused',
            'the fault. When the processor sets this bit to 1, it writes',
            'the faulting address to the BFAR',
        ),
    ),
    BitField(
        'IMPRECISERR',
        PW_CORTEX_M_CFSR_IMPRECISERR_MASK,
        'Imprecise data bus error.',
        (
            'A data bus error has occurred, but the return address in the',
            'stack frame is not related to the instruction that caused the',
            'error. This is an asynchronous fault. Therefore, if it is',
            'detected when the priority of the current processes is higher',
            'than the BusFault priority, the BusFault becomes pending and',
            'becomes active only when the processor returns from all higher',
            'priority processes. If a precise fault occurs before the',
            'processor enters the handler for the imprecise BusFault, the',
            'handler detects both IMPRECISERR set to 1 and one of the',
            'precise fault status bits set to 1',
        ),
    ),
    BitField(
        'UNSTKERR',
        PW_CORTEX_M_CFSR_UNSTKERR_MASK,
        'BusFault on Unstacking for a return from exception.',
        (
            'Unstack for an exception return has caused one or more',
            'BusFaults. This fault is chained to the handler. This means',
            'when the processor sets this bit to 1, the original return',
            'stack is still present. The processor does not adjust the SP',
            'from the failing return, does not perform a new save, and does',
            'not write a fault address to the BFAR',
        ),
    ),
    BitField(
        'STKERR',
        PW_CORTEX_M_CFSR_STKERR_MASK,
        'BusFault on Stacking for Exception Entry.',
        (
            'Stacking for an exception entry has caused one or more',
            'BusFaults. When the processor sets this bit to 1, the SP is',
            'still adjusted but the values in the context area on the stack',
            'might be incorrect. The processor does not write a fault',
            'address to the BFAR',
        ),
    ),
    BitField(
        'LSPERR',
        PW_CORTEX_M_CFSR_LSPERR_MASK,
        'BusFault during FPU lazy state preservation.',
        '',
    ),
    BitField(
        'BFARVALID', PW_CORTEX_M_CFSR_BFARVALID_MASK, 'BFAR is valid.', ''
    ),
    BitField(
        'UNDEFINSTR',
        PW_CORTEX_M_CFSR_UNDEFINSTR_MASK,
        'Undefined Instruction UsageFault.',
        (
            'The processor has attempted to execute an undefined',
            'instruction. When this bit is set to 1, the PC value stacked',
            'for the exception return points to the undefined instruction.',
            'An undefined instruction is an instruction that the processor',
            'cannot decode.',
        ),
    ),
    BitField(
        'INVSTATE',
        PW_CORTEX_M_CFSR_INVSTATE_MASK,
        'Invalid State UsageFault.',
        (
            'The processor has attempted to execute an instruction that',
            'makes illegal use of the EPSR. The PC value stacked for the',
            'exception return points to the instruction that attempt',
            'illegal use of the EPSR',
        ),
    ),
    BitField(
        'INVPC',
        PW_CORTEX_M_CFSR_INVPC_MASK,
        'Invalid PC Load UsageFault.',
        (
            'The processor has attempted an illegal load of EXC_RETURN',
            'to the PC, as a result of an invalid context, or an invalid',
            'EXC_RETURN value. The PC value stacked for the exception',
            'return points to the instruction that tried to perform the',
            'illegal load of the PC',
        ),
    ),
    BitField(
        'NOCP',
        PW_CORTEX_M_CFSR_NOCP_MASK,
        'Coprocessor disabled or not present.',
        '',
    ),
    BitField('STKOF', PW_CORTEX_M_CFSR_STKOF_MASK, 'Stack overflowed.', ''),
    BitField(
        'UNALIGNED',
        PW_CORTEX_M_CFSR_UNALIGNED_MASK,
        'Unaligned access UsageFault.',
        (
            'The processor has made an unaligned memory access. This fault',
            'can be enabled or disabled using the UNALIGN_TRP bit in the',
            'CCR. Unaligned LDM, STM, LDRD, and STRD instructions always',
            'fault irrespective of the CCR setting.',
        ),
    ),
    BitField(
        'DIVBYZERO',
        PW_CORTEX_M_CFSR_DIVBYZERO_MASK,
        'Divide by zero.',
        (
            'The processor has executed an SDIV or UDIV instruction with',
            'a divisor of 0. The PC value stacked for the exception',
            'return points to the instruction that performed the divide',
            'by zero. This fault can be enabled or disabled using the',
            'DIV_0_TRP bit in the CCR.',
        ),
    ),
]
