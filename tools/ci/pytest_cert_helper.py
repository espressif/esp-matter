# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: Apache-2.0

import pathlib
import re
import subprocess
from pytest_embedded import Dut
import os
from prettytable import PrettyTable
import json
import time
from gitlab_api import GitLabAPI
from results_formatter import ResultsFormatter

PYTEST_PATH = str((pathlib.Path(__file__).parent / '../../connectedhomeip/connectedhomeip/src/python_testing').resolve())

gitlab_api = GitLabAPI()
PYTEST_SSID = gitlab_api.ci_gitlab_pytest_ssid
PYTEST_PASSPHRASE = gitlab_api.ci_gitlab_pytest_passphrase

def load_test_commands(certification_tests: str):
    if not PYTEST_SSID or not PYTEST_PASSPHRASE:
        raise ValueError("CI_GITLAB_PYTEST_SSID and CI_GITLAB_PYTEST_PASSPHRASE must be set as environment variables")

    json_path = os.path.join(os.path.abspath(certification_tests))
    with open(json_path, 'r') as f:
        data = json.load(f)

    common_args_template = data.get("common_args", "")
    common_args = common_args_template.format(WIFI_SSID=PYTEST_SSID, WIFI_PASSPHRASE=PYTEST_PASSPHRASE)

    test_commands = []
    test_cases = data.get("test_cases", {})

    for test_case_name, test_config in test_cases.items():
        script = test_config["script"]
        args = test_config.get("args", "")

        if "test_case" in test_config:
            test_param = f"--tests {test_config['test_case']}"
            storage_path = f"--storage-path logs/{test_param}.json"
            command = f"python3 {script} {common_args} {storage_path} {test_param} {args}".strip()
        else:
            storage_path = f"--storage-path logs/{test_case_name}.json"
            command = f"python3 {script} {common_args} {storage_path} {args}".strip()

        test_commands.append({
            "name": test_case_name,
            "command": command
        })

    return test_commands

def clean_environment():
    clean_up_command = "rm -rf /tmp/chip_*"
    subprocess.getoutput(clean_up_command)
    cleanup_log_file = "rm -rf logs"
    subprocess.getoutput(cleanup_log_file)

def execute_test_command(full_command, dut:Dut, retry_attempts=2):
    light = dut
    for attempt in range(retry_attempts):
        print(f"Attempt {attempt + 1} for command: {full_command}")
        test_out_str = subprocess.getoutput(full_command)
        print(f"Test output: {test_out_str}")
        if "INFO:root:Final result: PASS !" in test_out_str:
            print(f"Test passed on attempt {attempt + 1}.")
            clean_environment()
            time.sleep(5)
            return True
        else:
            print(f"Test failed on attempt {attempt + 1}.")
            time.sleep(10)
            if attempt < retry_attempts - 1:
                clean_environment()
                light.write('matter esp factoryreset')
                time.sleep(10)
    return False

def generate_markdown_results(results_table, chunk_id=None):
    summary_title = f"Python Certification Test Results {chunk_id}" if chunk_id else "Test Results"
    markdown_results = [
        "<!-- Expandable Section -->",
        f"<details><summary>{summary_title}</summary>",
        "",
        "| Test Case | Result |",
        "|-----------|--------|"
    ]
    for test_case_name, result in results_table._rows:
        markdown_results.append(f"| {test_case_name.strip()} | {result.strip()} |")
    markdown_results.extend(["", "</details>", "<!-- End Expandable Section -->"])
    return "\n".join(markdown_results)

def update_mr_description_with_results(markdown_content, chunk_id=None):
    try:
        gitlab_api = GitLabAPI()
        description = gitlab_api.fetch_merge_request_description()
        updated_description = ResultsFormatter.update_cert_test_results_section(description, markdown_content, chunk_id=chunk_id)
        gitlab_api.update_merge_request_description(updated_description)
        print("Successfully updated MR description with test results.")
    except Exception as e:
        print(f"Failed to update MR description: {e}")

def run_python_certification_tests(dut:Dut, certification_tests:str) -> None:
    light = dut
    test_commands = load_test_commands(certification_tests)

    num_commands = len(test_commands)
    mid_index = (num_commands+1) // 2

    # Split the test commands into two chunks to parallelize the tests
    command_chunks = {
        "1": test_commands[:mid_index],
        "2": test_commands[mid_index:]
    }

    results_table = PrettyTable()
    results_table.field_names = ["Test Case", "Result"]

    test_chunk_key = os.getenv("TEST_CHUNK", "1")
    selected_commands = command_chunks.get(test_chunk_key, [])

    for index, test_item in enumerate(selected_commands, start=1):
        test_case_name = test_item["name"]
        test_command = test_item["command"]
        print(f"Executing command {index} in chunk {test_chunk_key}.")
        full_command = f"cd {PYTEST_PATH} && {test_command}"
        result = execute_test_command(full_command, light)
        test_case_marker = "PASS" if result else "FAIL"
        results_table.add_row([test_case_name, test_case_marker])
        print("Resetting the device for the next test case...")
        light.write('matter esp factoryreset')
        time.sleep(10)

    markdown_content = generate_markdown_results(results_table, chunk_id=test_chunk_key)
    update_mr_description_with_results(markdown_content, chunk_id=test_chunk_key)
