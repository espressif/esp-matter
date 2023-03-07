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
"""Tools to analyze Cortex-M CPU state context captured during an exception."""

from typing import Optional, Tuple

from pw_cpu_exception_cortex_m import cortex_m_constants
from pw_cpu_exception_cortex_m_protos import cpu_state_pb2
import pw_symbolizer

# These registers are symbolized when dumped.
_SYMBOLIZED_REGISTERS = (
    'pc',
    'lr',
    'bfar',
    'mmfar',
    'msp',
    'psp',
    'r0',
    'r1',
    'r2',
    'r3',
    'r4',
    'r5',
    'r6',
    'r7',
    'r8',
    'r9',
    'r10',
    'r11',
    'r12',
)


class CortexMExceptionAnalyzer:
    """This class provides helper functions to dump a ArmV7mCpuState proto."""

    def __init__(
        self, cpu_state, symbolizer: Optional[pw_symbolizer.Symbolizer] = None
    ):
        self._cpu_state = cpu_state
        self._symbolizer = symbolizer
        self._active_cfsr_fields: Optional[
            Tuple[cortex_m_constants.BitField, ...]
        ] = None

    def active_cfsr_fields(self) -> Tuple[cortex_m_constants.BitField, ...]:
        """Returns a list of BitFields for each active CFSR flag."""

        if self._active_cfsr_fields is not None:
            return self._active_cfsr_fields

        temp_field_list = []
        if self._cpu_state.HasField('cfsr'):
            for bit_field in cortex_m_constants.PW_CORTEX_M_CFSR_BIT_FIELDS:
                if self._cpu_state.cfsr & bit_field.bit_mask:
                    temp_field_list.append(bit_field)
        self._active_cfsr_fields = tuple(temp_field_list)
        return self._active_cfsr_fields

    def is_fault_active(self) -> bool:
        """Returns true if the current CPU state indicates a fault is active."""
        if self._cpu_state.HasField('cfsr') and self._cpu_state.cfsr != 0:
            return True
        if self._cpu_state.HasField('icsr'):
            exception_number = (
                self._cpu_state.icsr
                & cortex_m_constants.PW_CORTEX_M_ICSR_VECTACTIVE_MASK
            )
            if (
                cortex_m_constants.PW_CORTEX_M_HARD_FAULT_ISR_NUM
                <= exception_number
                <= cortex_m_constants.PW_CORTEX_M_USAGE_FAULT_ISR_NUM
            ):
                return True
        return False

    def is_nested_fault(self) -> bool:
        """Returns true if the current CPU state indicates a nested fault."""
        if not self.is_fault_active():
            return False
        if (
            self._cpu_state.HasField('hfsr')
            and self._cpu_state.hfsr
            & cortex_m_constants.PW_CORTEX_M_HFSR_FORCED_MASK
        ):
            return True
        return False

    def exception_cause(self, show_active_cfsr_fields=True) -> str:
        """Analyzes CPU state to tries and classify the exception.

        Examples:
            show_active_cfsr_fields=False
              unknown exception
              memory management fault at 0x00000000
              usage fault, imprecise bus fault

            show_active_cfsr_fields=True
              usage fault [DIVBYZERO]
              memory management fault at 0x00000000 [DACCVIOL] [MMARVALID]
        """
        cause = ''
        # The CFSR can accumulate multiple exceptions.
        split_major_cause = lambda cause: cause if not cause else cause + ', '

        if self._cpu_state.HasField('cfsr') and self.is_fault_active():
            if (
                self._cpu_state.cfsr
                & cortex_m_constants.PW_CORTEX_M_CFSR_USAGE_FAULT_MASK
            ):
                cause += 'usage fault'

            if (
                self._cpu_state.cfsr
                & cortex_m_constants.PW_CORTEX_M_CFSR_MEM_FAULT_MASK
            ):
                cause = split_major_cause(cause)
                cause += 'memory management fault'
                if (
                    self._cpu_state.cfsr
                    & cortex_m_constants.PW_CORTEX_M_CFSR_MMARVALID_MASK
                ):
                    addr = (
                        '???'
                        if not self._cpu_state.HasField('mmfar')
                        else f'0x{self._cpu_state.mmfar:08x}'
                    )
                    cause += f' at {addr}'

            if (
                self._cpu_state.cfsr
                & cortex_m_constants.PW_CORTEX_M_CFSR_BUS_FAULT_MASK
            ):
                cause = split_major_cause(cause)
                if (
                    self._cpu_state.cfsr
                    & cortex_m_constants.PW_CORTEX_M_CFSR_IMPRECISERR_MASK
                ):
                    cause += 'imprecise '
                cause += 'bus fault'
                if (
                    self._cpu_state.cfsr
                    & cortex_m_constants.PW_CORTEX_M_CFSR_BFARVALID_MASK
                ):
                    addr = (
                        '???'
                        if not self._cpu_state.HasField('bfar')
                        else f'0x{self._cpu_state.bfar:08x}'
                    )
                    cause += f' at {addr}'
            if show_active_cfsr_fields:
                for field in self.active_cfsr_fields():
                    cause += f' [{field.name}]'

        return cause if cause else 'unknown exception'

    def dump_registers(self) -> str:
        """Dumps all captured CPU registers as a multi-line string."""
        registers = []
        for field in self._cpu_state.DESCRIPTOR.fields:
            if self._cpu_state.HasField(field.name):
                register_value = getattr(self._cpu_state, field.name)
                register_str = f'{field.name:<10} 0x{register_value:08x}'
                if (
                    self._symbolizer is not None
                    and field.name in _SYMBOLIZED_REGISTERS
                ):
                    symbol = self._symbolizer.symbolize(register_value)
                    if symbol.name:
                        register_str += f' {symbol}'
                registers.append(register_str)
        return '\n'.join(registers)

    def dump_active_active_cfsr_fields(self) -> str:
        """Dumps CFSR flags with their descriptions as a multi-line string."""
        fields = []
        for field in self.active_cfsr_fields():
            fields.append(f'{field.name:<11} {field.description}')
            if isinstance(field.long_description, tuple):
                long_desc = '    {}'.format(
                    '\n    '.join(field.long_description)
                )
                fields.append(long_desc)
        return '\n'.join(fields)

    def __str__(self):
        dump = [f'Exception caused by a {self.exception_cause(False)}.', '']
        if self.active_cfsr_fields():
            dump.extend(
                (
                    'Active Crash Fault Status Register (CFSR) fields:',
                    self.dump_active_active_cfsr_fields(),
                    '',
                )
            )
        else:
            dump.extend(
                (
                    'No active Crash Fault Status Register (CFSR) fields.',
                    '',
                )
            )
        dump.extend(
            (
                'All registers:',
                self.dump_registers(),
            )
        )
        return '\n'.join(dump)


def process_snapshot(
    serialized_snapshot: bytes,
    symbolizer: Optional[pw_symbolizer.Symbolizer] = None,
) -> str:
    """Returns the stringified result of a SnapshotCpuStateOverlay message run
    though a CortexMExceptionAnalyzer.
    """
    snapshot = cpu_state_pb2.SnapshotCpuStateOverlay()
    snapshot.ParseFromString(serialized_snapshot)

    if snapshot.HasField('armv7m_cpu_state'):
        state_analyzer = CortexMExceptionAnalyzer(
            snapshot.armv7m_cpu_state, symbolizer
        )
        return f'{state_analyzer}\n'

    return ''
