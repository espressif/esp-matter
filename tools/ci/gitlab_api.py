# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

import os
import requests
import logging

class GitLabAPI:
    def __init__(self):
        self.gitlab_api_url = os.getenv("CI_API_V4_URL")
        self.gitlab_token = os.getenv("GITLAB_MR_COMMENT_TOKEN")
        self.ci_project_id = os.getenv("CI_PROJECT_ID")
        self.ci_merge_request_iid = os.getenv("CI_MERGE_REQUEST_IID")
        self.ci_pipeline_id = os.getenv("CI_PIPELINE_ID")
        self.ci_commit_ref_name = os.getenv("CI_COMMIT_REF_NAME")
        
        if not all([self.gitlab_api_url, self.gitlab_token, self.ci_project_id, self.ci_pipeline_id]):
            raise ValueError("Required GitLab environment variables are not set")

    def fetch_merge_request_description(self):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/merge_requests/{self.ci_merge_request_iid}"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        response = requests.get(url, headers=headers)
        response.raise_for_status()
        return response.json().get("description", "")

    def update_merge_request_description(self, updated_description):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/merge_requests/{self.ci_merge_request_iid}"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        data = {"description": updated_description}
        response = requests.put(url, headers=headers, json=data)
        response.raise_for_status()
        logging.info("Successfully updated the MR description.")

    def fetch_pipeline_for_commit(self, commit_sha, branch_name="main"):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/pipelines"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        params = {"ref": branch_name, "sha": commit_sha}
        response = requests.get(url, headers=headers, params=params)
        response.raise_for_status()
        pipelines = response.json()
        if not pipelines:
            raise ValueError(f"No pipeline found for commit: {commit_sha} on branch: {branch_name}.")
        return pipelines[0]['id']

    def fetch_merge_request_diff_versions(self):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/merge_requests/{self.ci_merge_request_iid}/versions"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        response = requests.get(url, headers=headers)
        response.raise_for_status()
        return response.json()

    def fetch_pipeline_jobs(self, pipeline_id):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/pipelines/{pipeline_id}/jobs"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        response = requests.get(url, headers=headers)
        response.raise_for_status()
        return response.json()

    def download_artifact(self, job_id, artifact_path, output_file):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/jobs/{job_id}/artifacts/{artifact_path}"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        with requests.get(url, headers=headers, stream=True) as response:
            response.raise_for_status()
            with open(output_file, 'wb') as f:
                for chunk in response.iter_content(chunk_size=8192):
                    f.write(chunk) 

    def cancel_pipeline(self, pipeline_id):
        url = f"{self.gitlab_api_url}/projects/{self.ci_project_id}/pipelines/{pipeline_id}/cancel"
        headers = {"PRIVATE-TOKEN": self.gitlab_token}
        response = requests.post(url, headers=headers)

        if response.status_code == 200:
            print(f"Successfully cancelled Pipeline ID: {pipeline_id}")
        else:
            print(f"Failed to cancel Pipeline ID: {pipeline_id}, Status Code: {response.status_code}")
