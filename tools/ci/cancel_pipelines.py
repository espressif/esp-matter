# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

import re
import os
import requests
from gitlab_api import GitLabAPI

def cancel_branch_pipelines(gitlab_api):
    url = f"{gitlab_api.gitlab_api_url}/projects/{gitlab_api.ci_project_id}/pipelines?ref={gitlab_api.ci_commit_ref_name}&status=running"
    headers = {"PRIVATE-TOKEN": gitlab_api.gitlab_token}
    response = requests.get(url, headers=headers)

    if response.status_code == 200:
        running_pipelines = response.json()

        for pipeline in running_pipelines:
            pipeline_id = pipeline.get('id')
            source = pipeline.get('source')
            ref = pipeline.get('ref')
            if pipeline_id != int(gitlab_api.ci_pipeline_id) and source != 'merge_request_event':
                print(f"Cancelling branch pipeline with pipeline ID: {pipeline_id}, Source:{source}, Ref: {ref}")
                gitlab_api.cancel_pipeline(pipeline_id)
            else:
                print(f"Skipping current branch pipeline with pipeline ID: {gitlab_api.ci_pipeline_id}")
    else:
        print(f"Failed to fetch branch pipelines, Status Code: {response.status_code}")

def cancel_merge_request_pipelines(gitlab_api):
    url = f"{gitlab_api.gitlab_api_url}/projects/{gitlab_api.ci_project_id}/pipelines?status=running"
    headers = {"PRIVATE-TOKEN": gitlab_api.gitlab_token}
    response = requests.get(url, headers=headers)

    if response.status_code == 200:
        running_pipelines = response.json()

        for pipeline in running_pipelines:
            pipeline_id = pipeline.get('id')
            source = pipeline.get('source')
            ref = pipeline.get('ref')
            merge_request_id_pattern = re.match(r"refs/merge-requests/(\d+)/head", ref)
            if merge_request_id_pattern:
               print(f"MR pipeline id: {merge_request_id_pattern.group(1)}")
               if source == "merge_request_event" and gitlab_api.ci_merge_request_iid == merge_request_id_pattern.group(1):
                  if pipeline_id == int(gitlab_api.ci_pipeline_id):
                      print(f"Skipping current MR pipeline with pipeline id: {pipeline_id} ref: {ref} source: {source}")
                  else:
                      print(f"Cancelling MR pipeline with pipeline id: {pipeline_id} ref:{ref} source: {source}")
                      gitlab_api.cancel_pipeline(pipeline_id)
            else:
                print(f"Skipping pipeline with pipeline ID: {pipeline_id} (ref: {ref}, source: {source})")

    else:
        print(f"Failed to fetch pipelines, Status: {response.status_code}")


def main():
    gitlab_api = GitLabAPI()
    cancel_branch_pipelines(gitlab_api)
    cancel_merge_request_pipelines(gitlab_api)

if __name__ == "__main__":
    main()

