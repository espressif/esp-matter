#!/usr/bin/env python3
# Copyright 2026 Espressif Systems (Shanghai) PTE LTD
# SPDX-License-Identifier: Apache-2.0
#
# Create backport MRs for a merged esp-matter MR, based on its
# "needs backport vX.Y[.Z]" labels.
#
# For each labelled version this script:
#   1. creates a branch backport/vX.Y/<source-branch> off origin/release/vX.Y
#   2. cherry-picks the MR's commits onto it
#   3. pushes the branch and opens an MR:
#        title:       "[vX.Y]: <original title>"
#        description: "Backporting !<iid>"
#        assignee:    original MR author
#
# Versions that already have a backport MR are skipped, so re-running is safe.
# When every labelled version has its backport, the original MR gets the
# "backports created" label.
#
# Usage:
#   GITLAB_TOKEN=<token> ./backport.py <MR IID or MR URL> [--dry-run]

import argparse
import json
import os
import re
import subprocess
import sys
import urllib.parse
import urllib.request

BACKPORT_LABEL_RE = re.compile(r"^needs backport v(\d+\.\d+(?:\.\d+)?)$")


class GitlabApi:
    def __init__(self, api_url, project, token):
        self.base = f"{api_url}/projects/{urllib.parse.quote_plus(str(project))}"
        self.token = token

    def request(self, path, method="GET", body=None):
        req = urllib.request.Request(f"{self.base}/{path}", method=method)
        req.add_header("PRIVATE-TOKEN", self.token)
        data = None
        if body is not None:
            req.add_header("Content-Type", "application/json")
            data = json.dumps(body).encode()
        with urllib.request.urlopen(req, data=data) as resp:
            return json.loads(resp.read().decode())

    def mr(self, iid):
        return self.request(f"merge_requests/{iid}")

    def mr_commits(self, iid):
        # API returns newest first; reverse for cherry-pick order
        return list(
            reversed(self.request(f"merge_requests/{iid}/commits?per_page=100"))
        )

    def existing_backport(self, iid, target_branch):
        # A backport MR targets the release branch and references !<iid> in its description
        mrs = self.request(
            f"merge_requests?target_branch={urllib.parse.quote_plus(target_branch)}"
            f"&search={urllib.parse.quote_plus(f'!{iid}')}&in=description&per_page=50"
        )
        pattern = re.compile(rf"!{iid}(?!\d)")
        for m in mrs:
            if m["state"] in ("opened", "merged") and pattern.search(
                m.get("description") or ""
            ):
                return m
        return None

    def create_mr(self, payload):
        return self.request("merge_requests", method="POST", body=payload)

    def update_labels(self, iid, add=None, remove=None):
        body = {}
        if add:
            body["add_labels"] = add
        if remove:
            body["remove_labels"] = remove
        return self.request(f"merge_requests/{iid}", method="PUT", body=body)

    def add_note(self, iid, body):
        return self.request(
            f"merge_requests/{iid}/notes", method="POST", body={"body": body}
        )


def git(repo_dir, *args, check=True):
    result = subprocess.run(
        ["git", "-C", repo_dir] + list(args),
        capture_output=True,
        text=True,
        check=False,
    )
    if check and result.returncode != 0:
        raise RuntimeError(f"git {' '.join(args)} failed:\n{result.stderr.strip()}")
    return result


def backport_one(api, repo_dir, mr, version, commits, push_url, dry_run):
    """Create branch + MR for one version. Returns created MR dict or None on conflict."""
    iid = mr["iid"]
    target_branch = f"release/v{version}"
    bp_branch = f"backport/v{version}/{mr['source_branch']}"

    git(repo_dir, "fetch", "origin", target_branch)

    # Start the backport branch from the release branch head
    git(repo_dir, "checkout", "-B", bp_branch, f"origin/{target_branch}")

    shas = [c["id"] for c in commits]
    print(f"  cherry-picking {len(shas)} commit(s) onto {target_branch}")
    result = git(repo_dir, "cherry-pick", *shas, check=False)
    if result.returncode != 0:
        git(repo_dir, "cherry-pick", "--abort", check=False)
        print(
            f"  CONFLICT: backport to {target_branch} needs manual resolution:\n"
            f"    git checkout -b {bp_branch} origin/{target_branch}\n"
            f"    git cherry-pick {' '.join(shas)}"
        )
        return None

    if dry_run:
        print(
            f'  dry-run: would push {bp_branch} and create MR "[v{version}]: {mr["title"]}"'
        )
        return {"web_url": "(dry-run)"}

    git(repo_dir, "push", push_url, f"HEAD:refs/heads/{bp_branch}")

    payload = {
        "source_branch": bp_branch,
        "target_branch": target_branch,
        "title": f"[v{version}]: {mr['title']}",
        "description": f"Backporting !{iid}",
        "remove_source_branch": True,
        "assignee_id": mr["author"]["id"],
        "reviewer_ids": [],
        "labels": "backport",
    }
    new_mr = api.create_mr(payload)
    print(f"  created: {new_mr['web_url']}")
    return new_mr


def main():
    parser = argparse.ArgumentParser(
        description='Create backport MRs from "needs backport vX.Y" labels'
    )
    parser.add_argument("mr", help="MR IID or MR web URL")
    parser.add_argument(
        "--api-url",
        default=os.environ.get("CI_API_V4_URL"),
        help="GitLab API v4 base URL (default: $CI_API_V4_URL)",
    )
    parser.add_argument(
        "--project",
        default=os.environ.get("CI_PROJECT_ID"),
        help="project id or url-encoded path (default: $CI_PROJECT_ID)",
    )
    parser.add_argument(
        "--repo-dir", default=os.environ.get("CI_PROJECT_DIR", os.getcwd())
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="cherry-pick locally but do not push or create MRs",
    )
    args = parser.parse_args()

    token = os.environ.get("GITLAB_TOKEN")
    if not token:
        sys.exit("GITLAB_TOKEN environment variable is required")

    url_match = re.match(
        r"(?P<base>https://[^/]+)/(?P<project>.+)/-/merge_requests/(?P<iid>\d+)",
        args.mr,
    )
    if url_match:
        iid = int(url_match.group("iid"))
        api = GitlabApi(
            f"{url_match.group('base')}/api/v4", url_match.group("project"), token
        )
        host = url_match.group("base")
        project_path = url_match.group("project")
    else:
        if not args.api_url or not args.project:
            sys.exit(
                "Pass the full MR URL, or provide --api-url and --project "
                "(auto-set in CI via CI_API_V4_URL/CI_PROJECT_ID)."
            )
        iid = int(args.mr)
        api = GitlabApi(args.api_url, args.project, token)
        host = args.api_url.split("/api/")[0]
        project_path = os.environ.get("CI_PROJECT_PATH") or (
            str(args.project) if "/" in str(args.project) else None
        )
        if not project_path:
            sys.exit(
                "Cannot determine project path for pushing — pass the full MR URL, "
                "use a path-style --project (group/project), or set CI_PROJECT_PATH."
            )

    push_url = (
        f"{host.replace('https://', f'https://oauth2:{token}@')}/{project_path}.git"
    )

    mr = api.mr(iid)
    print(f"MR !{iid}: {mr['title']} [{mr['state']}]")

    if mr["state"] != "merged":
        sys.exit("MR is not merged yet — merge it first, then run this job/script.")

    versions = sorted(
        m.group(1) for m in map(BACKPORT_LABEL_RE.match, mr["labels"]) if m
    )
    if not versions:
        sys.exit('No "needs backport vX.Y" labels on this MR — add labels first.')
    print(f"Backport versions: {', '.join(versions)}")

    commits = api.mr_commits(iid)
    repo_dir = args.repo_dir
    if git(repo_dir, "status", "--porcelain", "--untracked-files=no").stdout.strip():
        sys.exit(
            f"Working tree at {repo_dir} has uncommitted changes — commit/stash them first, "
            "or point --repo-dir at a clean clone/worktree."
        )
    start_ref = git(repo_dir, "rev-parse", "--abbrev-ref", "HEAD").stdout.strip()
    if start_ref == "HEAD":  # detached (CI)
        start_ref = git(repo_dir, "rev-parse", "HEAD").stdout.strip()

    # Make sure the MR's commits are available locally (they are reachable from the
    # target branch the MR was merged into, typically main).
    git(repo_dir, "fetch", "origin", mr["target_branch"])

    failed, created, new_mrs = [], [], []
    try:
        for version in versions:
            print(f"--- v{version}")
            existing = api.existing_backport(iid, f"release/v{version}")
            if existing:
                print(f"  already exists: {existing['web_url']}")
                created.append((version, existing))
                continue
            result = backport_one(
                api, repo_dir, mr, version, commits, push_url, args.dry_run
            )
            if result:
                created.append((version, result))
                new_mrs.append(version)
            else:
                failed.append(version)
    finally:
        git(repo_dir, "checkout", "--force", start_ref, check=False)

    if not args.dry_run and (new_mrs or failed):
        lines = [f"Backport status for !{iid}:", ""]
        lines += [f"- `release/v{v}`: {m['web_url']}" for v, m in created]
        lines += [
            f"- `release/v{v}`: cherry-pick conflict — needs a manual backport"
            for v in failed
        ]
        api.add_note(iid, "\n".join(lines))
        print("Posted status comment on the original MR.")

    if not args.dry_run and len(created) == len(versions):
        api.update_labels(iid, add="backports created", remove="backports missing")
        print(
            "All backports created — set 'backports created', removed 'backports missing'."
        )

    if failed:
        sys.exit(f"Backports with conflicts (create manually): {', '.join(failed)}")


if __name__ == "__main__":
    main()
