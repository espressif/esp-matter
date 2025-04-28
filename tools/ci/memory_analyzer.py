# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

import argparse
import logging
import glob
from memory_data_parser import StaticMemoryParser, DynamicMemoryParser
from gitlab_api import GitLabAPI
from results_formatter import ResultsFormatter

def locate_current_map_file(chip, example):
    pattern = f"examples/{example}/build_{chip}_default/{example}.map"
    artifact_file_paths = glob.glob(pattern, recursive=True)
    if not artifact_file_paths:
        raise FileNotFoundError(f"No map file found for the example {example} with target chip {chip}")
    return artifact_file_paths[0]

def process_static_memory(gitlab_api, formatter, chip, example, ref_map_file, job_name):
    try:
        # Get base commit information
        diff_versions = gitlab_api.fetch_merge_request_diff_versions()
        base_version = diff_versions[0]
        base_commit_sha = base_version["base_commit_sha"]

        # Get pipeline and job information
        base_commit_pipeline_id = gitlab_api.fetch_pipeline_for_commit(base_commit_sha, branch_name="main")
        jobs = gitlab_api.fetch_pipeline_jobs(base_commit_pipeline_id)

        target_job_id = next((job["id"] for job in jobs if job["name"] == job_name), None)
        if not target_job_id:
            raise ValueError("Target job not found.")

        # Get map files
        current_map_file = locate_current_map_file(chip, example)
        artifact_path = f"examples/{example}/build_{chip}_default/{example}.map"
        gitlab_api.download_artifact(target_job_id, artifact_path, ref_map_file)

        # Process static memory data
        size_diff_output = StaticMemoryParser.execute_idf_size_command(ref_map_file, current_map_file)

        # Update MR description with static memory results
        description = gitlab_api.fetch_merge_request_description()
        description = formatter.update_memory_results_title(description)
        description = formatter.update_static_memory_results_section(description, chip, example, size_diff_output)
        gitlab_api.update_merge_request_description(description)

        return True
    except Exception as e:
        logging.error(f"Error processing static memory: {str(e)}")
        return False

def process_dynamic_memory(gitlab_api, formatter, chip, example, log_file):
    try:
        # Extract and parse heap dump
        extracted_lines = DynamicMemoryParser.extract_heap_dump(log_file)
        parsed_logs = DynamicMemoryParser.parse_heap_dump(extracted_lines)

        if parsed_logs:
            # Format heap dump data
            formatted_output = formatter.format_heap_dump(parsed_logs)

            # Update MR description with heap memory results
            description = gitlab_api.fetch_merge_request_description()
            description = formatter.update_memory_results_title(description)
            description = formatter.update_heap_memory_results_section(description, chip, example, formatted_output)
            gitlab_api.update_merge_request_description(description)
            return True
        else:
            logging.warning("No heap dump data found in the log file.")
            return False
    except Exception as e:
        logging.error(f"Error processing dynamic memory: {str(e)}")
        return False

def main():
    logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(levelname)s - %(message)s")

    parser = argparse.ArgumentParser(description="Process and post memory analysis results.")
    parser.add_argument("--chip", required=True, help="Target chip (e.g., esp32c2, esp32h2)")
    parser.add_argument("--example", required=True, help="Target example (e.g., light, light_switch)")
    parser.add_argument("--ref_map_file", help="Reference main branch map file path")
    parser.add_argument("--job_name", help="Job name for the job id search")
    parser.add_argument("--log_file", help="Path to the log file for heap dump analysis")

    args = parser.parse_args()

    gitlab_api = GitLabAPI()
    formatter = ResultsFormatter()

    # Process static memory if required parameters are provided
    if all([args.ref_map_file, args.job_name]):
        if process_static_memory(gitlab_api, formatter, args.chip, args.example, args.ref_map_file, args.job_name):
            logging.info("Static memory analysis completed successfully")
        else:
            logging.error("Static memory analysis failed")

    # Process dynamic memory if log file is provided
    if args.log_file:
        if process_dynamic_memory(gitlab_api, formatter, args.chip, args.example, args.log_file):
            logging.info("Dynamic memory analysis completed successfully")
        else:
            logging.error("Dynamic memory analysis failed")

if __name__ == "__main__":
    main() 
