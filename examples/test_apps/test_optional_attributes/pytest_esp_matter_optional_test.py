# SPDX-License-Identifier: CC0-1.0

import os
import re
import subprocess
import time

import pytest
from gitlab_api import GitLabAPI
from pytest_embedded import Dut

ESP_MATTER_PATH = os.environ["ESP_MATTER_PATH"]

CURRENT_DIR = os.path.join(
    ESP_MATTER_PATH, "examples", "test_apps", "test_optional_attributes"
)
TEST_SCRIPT_PATH = os.path.join(
    ESP_MATTER_PATH,
    "tools",
    "test_optional_attributes",
    "test_optional_attributes_framework.py",
)
PAA_CERTS_PATH = os.path.join(
    ESP_MATTER_PATH,
    "connectedhomeip",
    "connectedhomeip",
    "credentials",
    "development",
    "paa-root-certs",
)

pytest_build_dir = CURRENT_DIR

gitlab_api = GitLabAPI()
PYTEST_SSID = gitlab_api.ci_gitlab_pytest_ssid
PYTEST_PASSPHRASE = gitlab_api.ci_gitlab_pytest_passphrase


@pytest.mark.esp32c3
@pytest.mark.esp_matter_dut
@pytest.mark.parametrize(
    "count, app_path, target, erase_all",
    [
        (1, pytest_build_dir, "esp32c3", "y"),
    ],
    indirect=True,
)
def test_optional_attributes_c3(dut: Dut) -> None:
    dut.expect(r"Configuring CHIPoBLE advertising", timeout=20)
    time.sleep(5)

    command = (
        f"python3 {TEST_SCRIPT_PATH}"
        f" -n 1"
        f" --commissioning-method ble-wifi"
        f" --wifi-ssid {PYTEST_SSID}"
        f" --wifi-passphrase {PYTEST_PASSPHRASE}"
        f" --passcode 20202021"
        f" --discriminator 3840"
        f" --paa-trust-store-path {PAA_CERTS_PATH}"
    )

    out_str = subprocess.getoutput(command)
    print(out_str)

    passed = re.search(r"Passed:\s+(\d+)", out_str)
    failed = re.search(r"Failed:\s+(\d+)", out_str)

    if failed and int(failed.group(1)) > 0:
        assert False, (
            f"Optional attributes test failed. {failed.group(1)} failures detected."
        )

    if not passed or int(passed.group(1)) == 0:
        assert False, "Optional attributes test did not report any passed results."
