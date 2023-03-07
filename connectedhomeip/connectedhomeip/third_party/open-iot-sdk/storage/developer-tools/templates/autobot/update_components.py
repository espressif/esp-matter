# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
import os
import click
import re
import sys
import git
import logging
import gitlab
import datetime
import textwrap

# global variable for passing information between callback and main
updated_repositories = []


def get_latest_commit(git_url, branch="HEAD"):
    # Finds the latest commit hash of a git branch
    g = git.cmd.Git()
    return g.ls_remote(git_url, branch).split()[0]


def get_remote_refs(git_url):
    # Finds all the tag of remote git repository
    g = git.cmd.Git()
    return g.ls_remote("--tags", git_url).split("\n")


def latest_git_ref(git_url):
    # Finds the latest git ref of a remote git repository.
    # If latest ref has a tag associated to it, returns tag.

    # find the last commit in "HEAD"
    latest_commit = get_latest_commit(git_url)
    logging.debug(f"Last HEAD commit for {git_url}: {latest_commit}")

    remote_refs = get_remote_refs(git_url)
    logging.debug(f"{remote_refs}")

    # Each line in remote_ref will contain a commit hash
    # followed by spaces and a tag name
    for ref in remote_refs:
        # if commit is matched return tag
        if ref != "" and ref.split()[0] == latest_commit:
            # annotated tags end with "^{}"
            # strip out prefix "refs/tags/" and annotated tags suffix
            logging.debug(f"Latest ref for {latest_commit} is {ref}")
            return True, ref.split()[1].split("refs/tags/")[1].split("^{}")[0]
    return False, latest_commit


def find_cmake_file_paths(path):
    # find all the cmake files in a given path
    cmake_files = []
    for root, dirs, files in os.walk(path):
        for file in files:
            if file.endswith(".cmake") or file.endswith("CMakeLists.txt"):
                cmake_files.append(os.path.join(root, file))
    return cmake_files


def update_in_single_occurrence(original_string):
    # Return updated string with latest git ref for a single occurence of
    # FetchContent_Declare|ExternalProject_Add

    # original string will be of format
    # .....
    # GIT_REPOSITORY  <git_repo>
    # GIT_TAG         <git_tag>
    # ......

    # find component_git_repo
    m = re.search(
        r"(?P<component_label>.*)[\s]+GIT_REPOSITORY[\s]+(?P<component_git_repo>.*)",
        original_string,
    )

    # if no GIT_REPOSITORY is specified, do not do anything
    # return original string
    if m is None:
        return original_string

    component_git_repo = m.group("component_git_repo")
    component_label = m.group("component_label")

    is_tag, git_ref = latest_git_ref(component_git_repo)

    # find current git tag present in the string
    m = re.search(r"GIT_TAG[\s]+(?P<git_tag>.*)", original_string)

    # if no GIT_TAG is specified, do not do anything
    # return original string
    if m is None:
        return original_string

    if git_ref != m.group(1):
        global updated_repositories
        updated_repositories.append(
            {
                "component_label": component_label.strip(),
                "component_git_repo": component_git_repo,
                "original_ref": m.group(1),
                "updated_ref": git_ref,
            }
        )

    # replace the string after GIT_TAG
    replaced_string = re.sub(r"(GIT_TAG\s*) (.*)", rf"\1 {git_ref}", original_string)

    # GIT_SHALLOW field should be set to OFF if reference is updated to a hash
    m = re.search(r"GIT_SHALLOW[\s]+(?P<git_shallow>(OFF|ON))", original_string)
    if not is_tag and m is not None:
        replaced_string = re.sub(r"(GIT_SHALLOW\s*) (.*)", r"\1 OFF", replaced_string)
    elif is_tag and m is not None:
        replaced_string = re.sub(r"(GIT_SHALLOW\s*) (.*)", r"\1 ON", replaced_string)

    logging.debug(f"String After update:\n {replaced_string}")
    return replaced_string


def _single_occurrence_re_sub_cb(matchobj):
    # call back function for re.sub
    return update_in_single_occurrence(matchobj.group(0))


def get_components(path_to_search):
    # Get git repository urls used in FetchContent_Declare or ExternalProject_Add
    # in a given path.

    cmake_files = find_cmake_file_paths(path_to_search)

    components = []
    for cmake_file in cmake_files:
        logging.info(f"Processing {cmake_file}")
        with open(cmake_file) as the_file:
            text = the_file.read()
            fetch = re.compile(
                r"(FetchContent_Declare|ExternalProject_Add).*?\(.*?GIT_REPOSITORY[\s]+(?P<git_repo>[^\n]*).*?\)",  # noqa: E501
                re.DOTALL,
            )
            for iter in fetch.finditer(text):
                if iter.group("git_repo") is not None:
                    components.append(iter.group("git_repo"))

    return components


def update_component(path, component_git_url):
    # Update git reference for a component's git repository found in
    # FetchContent_Declare or ExternalProject_Add
    global updated_repositories
    updated_repositories.clear()
    fetch = re.compile(
        rf"(FetchContent_Declare|ExternalProject_Add)?\(((?!FetchContent_Declare|ExternalProject_Add).)*{component_git_url}.*?\)",  # noqa: E501
        re.DOTALL,
    )

    try:
        cmake_files = find_cmake_file_paths(path)
        logging.info(f"Found cmake files {cmake_files} in {path}")
        update_status = "already_up-to-date"
        for cmake_file in cmake_files:
            logging.info(f"Processing {cmake_file}")
            with open(cmake_file, "r+") as the_file:
                text = the_file.read()
                # find the function declaration with either FetchContent_Declare
                # or ExternalProject_Add
                replaced_file_contents = fetch.sub(
                    _single_occurrence_re_sub_cb,
                    text,
                )

                if text != replaced_file_contents:
                    the_file.truncate(0)
                    the_file.seek(0)
                    the_file.write(replaced_file_contents)
                    update_status = "updated"
    except git.exc.GitError:
        update_status = "error"

    return {"status": update_status, "entries": updated_repositories}


def get_source_branch_if_MR_present(project, change_identification):
    # Get Merge Request source branch
    # if BEGIN_ID:<change_identification>:END_ID
    #   is found get branch_name frm MR
    # else returns None.
    mr_source_branch_name = None
    for mr in project.mergerequests.list(state="opened", all=True):
        logging.info(f"MR title: {mr.title}")
        try:
            if (
                change_identification
                == mr.description.split("BEGIN_ID:")[1].split(":END_ID")[0]
            ):
                mr_source_branch_name = mr.source_branch
                logging.info(
                    f"MR exists for {change_identification}, \
                        push changes to {mr_source_branch_name}"
                )
                break
        except IndexError:
            continue
    return mr_source_branch_name


def get_source_branch_if_no_MR_present(repo, remote_name, component_label):
    """
    get source branch name to create an MR.
    There is a chance that branch with name scheduled/bot/{component_label} exists
    in the repo. If so, append a timestamp.
    """
    branch_name = f"scheduled/bot/{component_label}"
    logging.debug(branch_name)
    logging.debug(repo.remote().refs)
    for ref in repo.remote().refs:
        if str(ref) == f"{remote_name}/scheduled/bot/{component_label}":
            branch_name += "_" + str(datetime.datetime.now().timestamp()).split(".")[0]
            logging.info(f"Branch name exists, create with {branch_name}")
            break
    return branch_name


@click.command()
@click.option(
    "--workspace_path",
    default=".",
    help="Workspace path to search and replace the git hash",
)
@click.option(
    "--project-id",
    required=True,
    help="GitLab project id",
)
@click.option(
    "--verbose",
    default=False,
    is_flag=True,
    help="Enable verbose logging",
)
@click.option(
    "--target-branch",
    required=True,
    help="Target branch of Merge Request.",
)
@click.option(
    "--server-url",
    default="https://gitlab.arm.com",
    required=True,
    help="GitLab server url",
)
@click.option(
    "--private-token",
    required=True,
    help="Private token for GitLab API requests",
)
def update_cmake_files(
    workspace_path, server_url, project_id, private_token, target_branch, verbose
):
    """
    This program parses .cmake files in the workspace directory and
    updates the git revision to the latest git commit hash for repositories
    specified using methods FetchContent_Declare or ExternalProject_Add.
    Once the .cmake files are updated, for each of the components,
    this program either creates a new Merge Request or update an existing one.
    """

    if verbose:
        logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
    else:
        logging.basicConfig(stream=sys.stdout, level=logging.INFO)

    gl = gitlab.Gitlab(url=server_url, private_token=private_token)
    project = gl.projects.get(project_id)

    updated_repositories.clear()
    remote_name = "origin"
    repo = git.Repo(".")

    repo.remotes.origin.fetch()

    for component_repo in get_components(workspace_path):
        logging.info(f"Component Repo: {component_repo}")

        logging.info(f"Reset {repo} {remote_name}/{target_branch}")

        # Loop is run for each components and makes the workspace dirty.
        # MR needs to be created for one per component. Hence, do a git reset
        repo.git.reset("--hard", f"{remote_name}/{target_branch}")

        diff = repo.git.diff(f"{remote_name}/{target_branch}")
        logging.info(f"Diff: {diff}")

        result = update_component(workspace_path, component_repo)
        logging.info(f"Update Component Result: {result}")

        diff = repo.git.diff(f"{remote_name}/{target_branch}")
        logging.info(f"Diff: {diff}")

        if result["status"] == "already_up-to-date":
            # No newer version of the component available
            # continue with next component
            logging.info(
                f"Target branch already has latest git ref \
                for component {component_repo}"
            )
            continue
        elif result["status"] == "error":
            # Had an error while updating the component
            # continue with next component
            # TODO: send a notification
            logging.error(f"Error while updating component {component_repo}")
            continue
        elif result["status"] == "updated":
            logging.debug(f"Update Component Result: {result}")
            pass

        to_ref = result["entries"][0]["updated_ref"]
        component_label = result["entries"][0]["component_label"]

        # if MR exists gets source branch
        mr_source_branch_name = get_source_branch_if_MR_present(project, component_repo)

        MR_TITLE = f"autobot: Update {component_label}"

        COMMIT_MESSAGE = (
            MR_TITLE
            + "\n\n"
            + textwrap.fill(f"Update {component_label} to {to_ref}.", width=72)
        )

        branch_name = get_source_branch_if_no_MR_present(
            repo, remote_name, component_label
        )

        # commit to a temporary branch for comparing, force pushing etc"
        repo.git.checkout("-b", f"tmp_{branch_name}")
        repo.git.commit("-asm", f"{COMMIT_MESSAGE}")

        diff = repo.git.diff(f"{remote_name}/{target_branch}")
        logging.info(f"Diff: {diff}")

        if mr_source_branch_name is not None:
            # we are here because there is an existing MR
            diff = repo.git.diff(f"{remote_name}/{mr_source_branch_name}")
            if "" == diff:
                # There is no difference with existing MR
                # This means, MR already has updates needed for the component
                logging.info(
                    f"No new changes identified for component {component_repo}"
                )
                continue
            else:
                # if there is new change, force push
                logging.info(
                    f"Push new changes identified for component {component_repo}"
                )
                repo.git.push("--force", "origin", f"HEAD:{mr_source_branch_name}")
        else:
            # MR doesn't exist
            # push changes and create an MR
            description = f"""
            Do not delete the line below.
            This is used for checking if MR already exists by ci bot. BEGIN_ID:{component_repo}:END_ID
            """  # noqa: E501

            logging.info(f"Create MR for component {component_repo}")
            repo.git.push("origin", f"HEAD:{branch_name}")
            project.mergerequests.create(
                {
                    "source_branch": branch_name,
                    "target_branch": target_branch,
                    "title": "Draft: " + MR_TITLE,
                    "labels": ["auto_update"],
                    "description": description,
                }
            )
        diff = repo.git.diff(f"{remote_name}/{target_branch}")
        logging.info(f"Diff: {diff}")


if __name__ == "__main__":
    update_cmake_files()
