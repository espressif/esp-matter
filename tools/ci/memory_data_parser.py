# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

import subprocess

class StaticMemoryParser:
    @staticmethod
    def execute_idf_size_command(old_file_path, new_file_path):
        try:
            result = subprocess.run(
                ["python", "-m", "esp_idf_size", "--diff", old_file_path, new_file_path],
                capture_output=True,
                text=True,
                check=True,
            )
            return result.stdout
        except subprocess.CalledProcessError as e:
            raise

class DynamicMemoryParser:
    @staticmethod
    def extract_heap_dump(log_file):
        cmd = f"sed -n '/HEAP-DUMP-START/,/HEAP-DUMP-END/p' {log_file}"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.stdout.splitlines()

    @staticmethod
    def parse_heap_dump(extracted_lines):
        parsed_logs = []
        current_state = None

        for line in extracted_lines:
            if "state:" in line:
                current_state = line.split("state:")[1].strip()
            elif "Current Free Memory" in line:
                current_free_mem = line.split()[-1]
            elif "Largest Free Block" in line:
                largest_free_block = line.split()[-1]
            elif "Min. Ever Free Size" in line:
                min_ever_free_size = line.split()[-1]
                parsed_logs.append([current_state, current_free_mem, largest_free_block, min_ever_free_size])
        
        return parsed_logs


