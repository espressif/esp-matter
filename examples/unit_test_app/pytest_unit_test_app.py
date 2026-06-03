# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded_qemu.dut import QemuDut


def run_group(dut: QemuDut, group: str, timeout: int = 120) -> None:
    """Run all Unity cases matching a group tag, then verify no failures.

    pytest-embedded records Unity results without raising on failure,
    so we check dut.testsuite afterwards to surface failures to pytest.
    """
    cases = [c for c in dut.test_menu if group in c.groups]
    assert cases, f'No cases for group "{group}" (parsed {len(dut.test_menu)} total)'

    dut.run_all_single_board_cases(group=group, timeout=timeout)

    failed = dut.testsuite.failed_cases
    if failed:
        names = [tc.name for tc in failed]
        pytest.fail(f"{len(failed)} failed in [{group}]: {', '.join(names)}")


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_get_val(dut: QemuDut) -> None:
    run_group(dut, "get_val")


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_get_val_type(dut: QemuDut) -> None:
    run_group(dut, "get_val_type")


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_update_report(dut: QemuDut) -> None:
    run_group(dut, "report")
    run_group(dut, "update")


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_jsontlv(dut: QemuDut) -> None:
    run_group(dut, "jsontlv")


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_lifecycle(dut: QemuDut) -> None:
    run_group(dut, "cluster_lifecycle")


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32c3
def test_attribute_create_value_persistence(dut: QemuDut) -> None:
    """Runs TEST_CASE_MULTIPLE_STAGES cases (both stages, including across SW reset)."""
    dut.run_all_single_board_cases(
        name=["attribute::create preserves max value after reboot"],
        timeout=120,
    )
    failed = dut.testsuite.failed_cases
    if failed:
        names = [tc.name for tc in failed]
        pytest.fail(f"{len(failed)} failed: {', '.join(names)}")
