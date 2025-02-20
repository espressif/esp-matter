# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

import re
from tabulate import tabulate

class ResultsFormatter:
    @staticmethod
    def update_memory_results_title(description):
        header_start = "<!-- START: Memory Header -->"
        header_end = "<!-- END: Memory Header -->"
        if header_start in description and header_end in description:
            return description

        header_section_content = "#### Gitlab CI Memory Numbers (Do Not Edit) \n"
        header_section = f"{header_start}\n{header_section_content}{header_end}"

        updated_description = description.strip() + "\n\n" + header_section
        return updated_description

    @staticmethod
    def update_static_memory_results_section(description, chip_name, example, output):
        marker_start = f"<!-- START: Memory Results for {chip_name} -->"
        marker_end = f"<!-- END: Memory Results for {chip_name} -->"

        chip_section_content = (
            f"<details><summary><b>Static Memory Footprint for target: {chip_name}, example: {example}</b></summary>\n\n"
            f"```{output}```\n"
            f"</details>\n"
        )

        chip_section = f"{marker_start}\n{chip_section_content}{marker_end}"

        if marker_start in description and marker_end in description:
            updated_description = re.sub(
                rf"{re.escape(marker_start)}.*?{re.escape(marker_end)}",
                chip_section,
                description,
                flags=re.DOTALL,
            )
        else:
            updated_description = description.strip() + "\n\n" + chip_section

        return updated_description

    @staticmethod
    def update_heap_memory_results_section(description, chip_name, example, output):
        marker_start = f"<!-- START: Heap Memory Results for {chip_name} -->"
        marker_end = f"<!-- END: Heap Memory Results for {chip_name} -->"

        chip_section_content = (
            f"<details><summary><b>Dynamic Memory Footprint for target: {chip_name}, example: {example}</b></summary>\n\n"
            f"```{output}\n```\n"
            f"</details>\n"
        )

        chip_section = f"{marker_start}\n{chip_section_content}{marker_end}"

        if marker_start in description and marker_end in description:
            updated_description = re.sub(
                rf"{re.escape(marker_start)}.*?{re.escape(marker_end)}",
                chip_section,
                description,
                flags=re.DOTALL,
            )
        else:
            updated_description = description.strip() + "\n\n" + chip_section

        return updated_description

    @staticmethod
    def format_heap_dump(parsed_logs):
        headers = ["State", "Current Free Memory", "Largest Free Block", "Min. Ever Free Size"]
        return tabulate(parsed_logs, headers=headers, tablefmt="grid")
