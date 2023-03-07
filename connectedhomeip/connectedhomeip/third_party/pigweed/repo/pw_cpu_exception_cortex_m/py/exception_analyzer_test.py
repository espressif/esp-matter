#!/usr/bin/env python3
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
"""Tests dumped Cortex-M CPU state."""

import textwrap
import unittest

from pw_cpu_exception_cortex_m import exception_analyzer, cortex_m_constants
from pw_cpu_exception_cortex_m_protos import cpu_state_pb2
import pw_symbolizer

# pylint: disable=protected-access


class BasicFaultTest(unittest.TestCase):
    """Test basic fault analysis functions."""

    def test_empty_state(self):
        """Ensure an empty CPU state proto doesn't indicate an active fault."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertFalse(cpu_state_info.is_fault_active())

    def test_cfsr_fault(self):
        """Ensure a fault is active if CFSR bits are set."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_STKOF_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_MUNSTKERR_MASK
        )
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertTrue(cpu_state_info.is_fault_active())

    def test_icsr_fault(self):
        """Ensure a fault is active if ICSR says the handler is active."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.icsr = cortex_m_constants.PW_CORTEX_M_HARD_FAULT_ISR_NUM
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertTrue(cpu_state_info.is_fault_active())

    def test_cfsr_fields(self):
        """Ensure correct fields are returned when CFSR bits are set."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_STKOF_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_MUNSTKERR_MASK
        )
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        active_fields = [
            field.name for field in cpu_state_info.active_cfsr_fields()
        ]
        self.assertEqual(len(active_fields), 2)
        self.assertIn('STKOF', active_fields)
        self.assertIn('MUNSTKERR', active_fields)


class ExceptionCauseTest(unittest.TestCase):
    """Test exception cause analysis."""

    def test_empty_cpu_state(self):
        """Ensure empty CPU state has no known cause."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(cpu_state_info.exception_cause(), 'unknown exception')

    def test_unknown_exception(self):
        """Ensure CPU state with insufficient info has no known cause."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        # Set CFSR to a valid value.
        cpu_state_proto.cfsr = 0
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(cpu_state_info.exception_cause(), 'unknown exception')

    def test_single_usage_fault(self):
        """Ensure usage faults are properly identified."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = cortex_m_constants.PW_CORTEX_M_CFSR_STKOF_MASK
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(
            cpu_state_info.exception_cause(), 'usage fault [STKOF]'
        )

    def test_single_usage_fault_without_fields(self):
        """Ensure disabling show_active_cfsr_fields hides field names."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = cortex_m_constants.PW_CORTEX_M_CFSR_STKOF_MASK
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(cpu_state_info.exception_cause(False), 'usage fault')

    def test_multiple_faults(self):
        """Ensure multiple CFSR bits are identified and reported."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_STKOF_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_UNSTKERR_MASK
        )
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(
            cpu_state_info.exception_cause(),
            'usage fault, bus fault [UNSTKERR] [STKOF]',
        )

    def test_mmfar_missing(self):
        """Ensure if mmfar is valid but missing it is handled safely."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_MUNSTKERR_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_MMARVALID_MASK
        )
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(
            cpu_state_info.exception_cause(False),
            'memory management fault at ???',
        )

    def test_mmfar_valid(self):
        """Validate output format of valid MMFAR."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_MUNSTKERR_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_MMARVALID_MASK
        )
        cpu_state_proto.mmfar = 0x722470E4
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(
            cpu_state_info.exception_cause(False),
            'memory management fault at 0x722470e4',
        )

    def test_imprecise_bus_fault(self):
        """Check that imprecise bus faults are identified correctly."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_IMPRECISERR_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_IBUSERR_MASK
        )
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        self.assertEqual(
            cpu_state_info.exception_cause(False), 'imprecise bus fault'
        )


class TextDumpTest(unittest.TestCase):
    """Test larger state dumps."""

    def test_registers(self):
        """Validate output of general register dumps."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.pc = 0xDFADD966
        cpu_state_proto.mmfar = 0xAF2EA98A
        cpu_state_proto.r0 = 0xF3B235B1
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        expected_dump = '\n'.join(
            (
                'pc         0xdfadd966',
                'mmfar      0xaf2ea98a',
                'r0         0xf3b235b1',
            )
        )
        self.assertEqual(cpu_state_info.dump_registers(), expected_dump)

    def test_symbolization(self):
        """Ensure certain registers are symbolized."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        known_symbols = (
            pw_symbolizer.Symbol(0x0800A200, 'foo()', 'src/foo.c', 41),
            pw_symbolizer.Symbol(
                0x08000004, 'boot_entry()', 'src/vector_table.c', 5
            ),
        )
        symbolizer = pw_symbolizer.FakeSymbolizer(known_symbols)
        cpu_state_proto.pc = 0x0800A200
        cpu_state_proto.lr = 0x08000004
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto, symbolizer
        )
        expected_dump = '\n'.join(
            (
                'pc         0x0800a200 foo() (src/foo.c:41)',
                'lr         0x08000004 boot_entry() (src/vector_table.c:5)',
            )
        )
        self.assertEqual(cpu_state_info.dump_registers(), expected_dump)

    def test_dump_no_cfsr(self):
        """Validate basic CPU state dump."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.pc = 0xD2603058
        cpu_state_proto.mmfar = 0x8E4EB9A2
        cpu_state_proto.r0 = 0xDB5E7168
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        expected_dump = '\n'.join(
            (
                'Exception caused by a unknown exception.',
                '',
                'No active Crash Fault Status Register (CFSR) fields.',
                '',
                'All registers:',
                'pc         0xd2603058',
                'mmfar      0x8e4eb9a2',
                'r0         0xdb5e7168',
            )
        )
        self.assertEqual(str(cpu_state_info), expected_dump)

    def test_dump_with_cfsr(self):
        """Validate CPU state dump with CFSR bits set is formatted correctly."""
        cpu_state_proto = cpu_state_pb2.ArmV7mCpuState()
        cpu_state_proto.cfsr = (
            cortex_m_constants.PW_CORTEX_M_CFSR_PRECISERR_MASK
            | cortex_m_constants.PW_CORTEX_M_CFSR_BFARVALID_MASK
        )
        cpu_state_proto.pc = 0xD2603058
        cpu_state_proto.bfar = 0xDEADBEEF
        cpu_state_proto.mmfar = 0x8E4EB9A2
        cpu_state_proto.r0 = 0xDB5E7168
        cpu_state_info = exception_analyzer.CortexMExceptionAnalyzer(
            cpu_state_proto
        )
        expected_dump = textwrap.dedent(
            """
            Exception caused by a bus fault at 0xdeadbeef.

            Active Crash Fault Status Register (CFSR) fields:
            PRECISERR   Precise data bus error.
                A data bus error has occurred, and the PC value stacked for
                the exception return points to the instruction that caused
                the fault. When the processor sets this bit to 1, it writes
                the faulting address to the BFAR
            BFARVALID   BFAR is valid.

            All registers:
            pc         0xd2603058
            cfsr       0x00008200
            mmfar      0x8e4eb9a2
            bfar       0xdeadbeef
            r0         0xdb5e7168
        """.strip(
                '\n'
            )
        ).strip()
        self.assertEqual(str(cpu_state_info), expected_dump)


if __name__ == '__main__':
    unittest.main()
