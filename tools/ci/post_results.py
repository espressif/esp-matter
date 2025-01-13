# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

import os
import subprocess
import requests
import glob
import argparse
import logging

# Gitlab Configurations
gitlab_api_url = os.getenv("CI_API_V4_URL")
gitlab_token = os.getenv("GITLAB_MR_COMMENT_TOKEN")
ci_project_id = os.getenv("CI_PROJECT_ID")
ci_merge_request_iid = os.getenv("CI_MERGE_REQUEST_IID")

# Fetch the id of the pipeline for a branch with the specified commit id (default main branch)
def fetch_pipeline_for_commit(commit_sha, branch_name="main"):
    url = f"{gitlab_api_url}/projects/{ci_project_id}/pipelines"
    headers = {"PRIVATE-TOKEN": gitlab_token}
    params = {"ref": branch_name, "sha": commit_sha}
    response = requests.get(url, headers=headers, params=params)
    response.raise_for_status()
    pipelines = response.json()
    if not pipelines:
        raise ValueError(f"No pipeline found for commit: {commit_sha} on branch: {branch_name}")
    return pipelines[0]['id']

# Fetch the versions for the gitlab MR.
def fetch_merge_request_diff_versions():
    url = f"{gitlab_api_url}/projects/{ci_project_id}/merge_requests/{ci_merge_request_iid}/versions"
    headers = {"PRIVATE-TOKEN": gitlab_token}
    response = requests.get(url, headers=headers)
    response.raise_for_status()
    return response.json()

# Fetch the jobs specific to a pipeline id.
def fetch_pipeline_jobs(pipeline_id):
    url = f"{gitlab_api_url}/projects/{ci_project_id}/pipelines/{pipeline_id}/jobs"
    headers = {"PRIVATE-TOKEN": gitlab_token}
    response = requests.get(url, headers=headers)
    response.raise_for_status()
    return response.json()

# Download the reference map file for the MR base commit.
def download_ref_map_file(chip_name, job_id, output_file):
    ref_artifact_path = f"examples/light/build_{chip_name}_default/light.map"
    url = f"{gitlab_api_url}/projects/{ci_project_id}/jobs/{job_id}/artifacts/{ref_artifact_path}"
    headers = {"PRIVATE-TOKEN": gitlab_token}
    with requests.get(url, headers=headers, stream=True) as response:
        response.raise_for_status()
        with open(output_file, 'wb') as f:
            for chunk in response.iter_content(chunk_size=8192):
                f.write(chunk)

# Locate the map file artifact for the current pipeline.
def locate_current_map_file(example):
   pattern = f"examples/{example}/build*/{example}.map"
   artifact_file_paths = glob.glob(pattern, recursive=True)
   if not artifact_file_paths:
        raise FileNotFoundError("No map file found!")
   return artifact_file_paths[0]

# Execute esp_idf_size diff command to find increase/decrease in firmware size.
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

# Post the results to gitlab MR.
def post_results_to_gitlab_mr(output, chip_name, example):
    if not all([gitlab_api_url, gitlab_token, ci_project_id, ci_merge_request_iid]):
        print("Missing required environment variables. Results not posted.")
        return

    markdown_output = f"<details open><summary><b>Static Memory Footprint for target: {chip_name}, example: {example}</b></summary>\n\n```\n{output}\n```\n</details>"
    url = f"{gitlab_api_url}/projects/{ci_project_id}/merge_requests/{ci_merge_request_iid}/notes"
    headers = {"PRIVATE-TOKEN": gitlab_token}
    data = {"body": markdown_output}
    response = requests.post(url, headers=headers, json=data)
    if response.status_code == 201:
        print("Successfully posted results to GitLab MR.")
    else:
        print("Failed to post results to GitLab MR.")

def main():

    logging.basicConfig(level=logging.WARNING, format="%(asctime)s - %(levelname)s - %(message)s")
    parser = argparse.ArgumentParser(description="Process build results and post to GitLab.")
    parser.add_argument("--chip", required=True, help="Specify the chip name (e.g., C2, H2)")
    parser.add_argument("--ref_map_file", required=True, help="Specify the reference main branch map file")
    parser.add_argument("--job_name", required=True, help = "Specify the job name for the job id search")
    parser.add_argument("--example", required=True, help = "Specify the example name for the memory footprint")
    args = parser.parse_args()

    try:
        diff_versions = fetch_merge_request_diff_versions()
        base_version = diff_versions[0]
        base_commit_sha = base_version["base_commit_sha"]

        base_commit_pipeline_id = fetch_pipeline_for_commit(base_commit_sha, branch_name="main")
        jobs = fetch_pipeline_jobs(base_commit_pipeline_id)

        target_job_id = next((job["id"] for job in jobs if job["name"] == args.job_name), None)
        if not target_job_id:
            raise ValueError("Target job not found.")

        download_ref_map_file(args.chip, target_job_id, args.ref_map_file)
        current_map_file = locate_current_map_file(args.example)

        size_diff_output = execute_idf_size_command(args.ref_map_file, current_map_file)
        post_results_to_gitlab_mr(size_diff_output, args.chip, args.example)
    except FileNotFoundError as e:
        logging.error(f"Error occurred while posting results to GitLab MR: File not found {e}")
    except Exception as e:
        logging.error(f"Error occurred while posting results to GitLab MR: An Unexpected error occurred:{e}")

if __name__ == "__main__":
    main()


