# Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
from update_components import (
    latest_git_ref,
    update_in_single_occurrence,
    update_cmake_files,
    get_source_branch_if_no_MR_present,
    get_source_branch_if_MR_present,
    update_component,
)
from click.testing import CliRunner
from unittest.mock import patch, MagicMock
import pytest
import git
import gitlab
import os
import logging


this_dir = os.path.dirname(os.path.realpath(__file__))

# remote_refs pointing to tags
remote_refs = [
    "f791776d4e7d2628091572ad51448c0b476ab042        refs/tags/annotated_tag",
    "7191d9786063d035e13fb1c4294ba1b179bf8dbc        refs/tags/annotated_tag^{}",
    "e05bb32dcb53a8b7cbeb36c717ab57f9d6786c9e        refs/tags/light_weight_tag",
    "bb32e0521b53a312312eb36c717ab57f9d679d67        refs/tags/another_tag",
]


@pytest.mark.parametrize(
    "old_ref, expected_ref, expected_is_ref_a_tag",
    [
        ("7191d9786063d035e13fb1c4294ba1b179bf8dbc", "annotated_tag", True),
        ("e05bb32dcb53a8b7cbeb36c717ab57f9d6786c9e", "light_weight_tag", True),
        # no tag associated with the commit-hash, hence commit-hash will be returned
        (
            "d97860719163d0351c4294ba1b179bf8dbce13fb",
            "d97860719163d0351c4294ba1b179bf8dbce13fb",
            False,
        ),
    ],
)
@patch("update_components.get_remote_refs", MagicMock(return_value=remote_refs))
def test_latest_git_ref(old_ref, expected_ref, expected_is_ref_a_tag):
    with patch("update_components.get_latest_commit", MagicMock(return_value=old_ref)):
        assert latest_git_ref("") == (expected_is_ref_a_tag, expected_ref)


@pytest.mark.parametrize(
    "old_ref, old_git_shallow, new_ref, new_git_shallow, is_ref_a_tag",
    [
        ("7191d9786063d035e13fb1c4294ba1b179bf8dbc", "OFF", "latest_tag", "ON", True),
        ("latest_tag", "ON", "7191d9786063d035e13fb1c4294ba1b179bf8dbc", "OFF", False),
        (
            "7191d9786063d035e13fb1c4294ba1b179bf8dbc",
            "OFF",
            "7191d9786063d035e13fb1c4294ba1b179bf8dbc",
            "OFF",
            False,
        ),
    ],
)
def test_update_in_single_occurrence(
    old_ref, old_git_shallow, new_ref, new_git_shallow, is_ref_a_tag
):
    string_template = """
    FetchContent_Declare(
    TinyUSB
    GIT_REPOSITORY  https://github.com/hathach/tinyusb.git
    GIT_TAG         %s
    GIT_SHALLOW     %s
    )
    """
    with patch(
        "update_components.latest_git_ref",
        MagicMock(return_value=(is_ref_a_tag, new_ref)),
    ):
        assert update_in_single_occurrence(
            string_template % (old_ref, old_git_shallow)
        ) == string_template % (new_ref, new_git_shallow)


def test_source_branch_if_no_MR_present(caplog):
    caplog.set_level(logging.INFO)
    remote_refs = [
        "origin/fix/sahbab01/rename_config",
        "origin/fj-ctest-emac",
        "origin/main",
        "origin/scheduled/bot/EXISTING_BRANCH",
        "origin/scheduled/bot/aws-iot-device-sdk",
    ]
    with patch("datetime.datetime", MagicMock()) as mock_date_time:
        with patch("git.Repo", MagicMock()) as mock_repo:
            mock_date_time.now().timestamp = MagicMock(return_value="123.456")
            mock_repo.remote().refs = remote_refs
            # if branch is present in remote, append with timestamp
            assert (
                get_source_branch_if_no_MR_present(
                    mock_repo, "origin", "EXISTING_BRANCH"
                )
                == "scheduled/bot/EXISTING_BRANCH_123"
            )
            # if branch is not present in remote, return the branch
            assert (
                get_source_branch_if_no_MR_present(mock_repo, "origin", "NEW_COMPONENT")
                == "scheduled/bot/NEW_COMPONENT"
            )


def test_source_branch_if_MR_present(caplog):
    caplog.set_level(logging.INFO)
    with patch("gitlab.Gitlab", MagicMock()) as mocked_gl:

        class MR:
            def __init__(self, description, source_branch):
                self.title = ""
                self.description = description
                self.source_branch = source_branch

        # if component url is present inside BEGIN_ID:{component_url}:END_ID
        # then return source_branch
        component_url = "a_component_url"
        description = f"BEGIN_ID:{component_url}:END_ID"
        source_branch = "a_source_branch"
        mocked_gl().projects().get().mergerequests.list = MagicMock(
            return_value=[MR(description, source_branch)]
        )
        assert (
            get_source_branch_if_MR_present(mocked_gl().projects().get(), component_url)
            == source_branch
        )

        # if no matching description is present inside BEGIN_ID:{component_url}:END_ID
        # then return source_branch
        description = ""
        component_url = "a_component_url"
        source_branch = "a_source_branch"
        mocked_gl().projects().get().mergerequests.list = MagicMock(
            return_value=[MR(description, source_branch)]
        )
        assert (
            get_source_branch_if_MR_present(mocked_gl().projects().get(), component_url)
            is None
        )

        # if no MRs are present return None
        mocked_gl().projects().get().mergerequests.list = MagicMock()
        assert (
            get_source_branch_if_MR_present(mocked_gl().projects().get(), component_url)
            is None
        )


def _prefix_test_data_path(file_name):
    return os.path.join(this_dir, "test_data", file_name)


@pytest.fixture()
def restore_input_cmake_files():
    backup_contents = []
    file_names = []

    def _store_in_memory(in_files):
        nonlocal backup_contents
        nonlocal file_names
        file_names = in_files
        for i in file_names:
            with open(i) as the_file:
                backup_contents.append(the_file.read())

    yield _store_in_memory

    # restore
    for i in file_names:
        with open(i, "w") as the_file:
            the_file.write(backup_contents[file_names.index(i)])


@pytest.mark.parametrize(
    "input_cmake_files, expected_updated_cmake_files",
    [
        # check with a simple file with single entry
        (
            [_prefix_test_data_path("single_repo_original")],
            [_prefix_test_data_path("single_repo_after_update")],
        ),
        (
            [_prefix_test_data_path("no_update_needed_original")],
            [_prefix_test_data_path("no_update_needed_original")],
        ),
    ],
)
@patch("update_components.latest_git_ref", MagicMock(return_value=(True, "latest_tag")))
def test_update_one_component(
    restore_input_cmake_files, input_cmake_files, expected_updated_cmake_files
):

    with patch(
        "update_components.find_cmake_file_paths",
        MagicMock(return_value=input_cmake_files),
    ):
        restore_input_cmake_files(input_cmake_files)
        update_component("", "https://github.com/hathach/tinyusb.git")

        for input, expected in zip(input_cmake_files, expected_updated_cmake_files):
            assert open(input).read() == open(expected).read()


def test_update_and_create_mrs_already_up_to_date(caplog):
    caplog.set_level(logging.INFO)

    git.Repo = MagicMock()
    gitlab.Gitlab = MagicMock()

    with patch(
        "update_components.get_source_branch_if_MR_present",
        MagicMock(return_value=None),
    ):
        with patch(
            "update_components.get_components",
            MagicMock(return_value=["https://github.com/hathach/tinyusb.git"]),
        ):
            with patch(
                "update_components.update_component",
                MagicMock(return_value={"status": "already_up-to-date"}),
            ):
                CliRunner().invoke(
                    update_cmake_files,
                    '--workspace_path "" --project-id "" \
                    --private-token "" --target-branch ""',
                )
                gitlab.Gitlab().projects.get().mergerequests.create.assert_not_called()
                git.Repo().git.push.assert_not_called()


def test_update_and_create_mrs_mr_exists_and_with_new_changes(caplog):
    caplog.set_level(logging.INFO)

    git.Repo = MagicMock()
    gitlab.Gitlab = MagicMock()

    with patch(
        "update_components.get_source_branch_if_MR_present",
        MagicMock(return_value="source_branch"),
    ):
        with patch(
            "update_components.get_components",
            MagicMock(return_value=["https://github.com/hathach/tinyusb.git"]),
        ):
            with patch("update_components.update_component", MagicMock()):
                CliRunner().invoke(
                    update_cmake_files,
                    '--workspace_path "" --project-id "" \
                    --private-token "" --target-branch ""',
                )
                gitlab.Gitlab().projects.get().mergerequests.create.assert_not_called()
                git.Repo().git.push.assert_called_with(
                    "--force", "origin", "HEAD:source_branch"
                )


def test_update_and_create_mrs_mr_doesnot_exist(caplog):
    caplog.set_level(logging.INFO)

    git.Repo = MagicMock()
    gitlab.Gitlab = MagicMock()

    with patch(
        "update_components.get_source_branch_if_MR_present",
        MagicMock(return_value=None),
    ):
        with patch(
            "update_components.get_components", MagicMock(return_value=["test"])
        ):
            with patch("update_components.update_component", MagicMock()):
                CliRunner().invoke(
                    update_cmake_files,
                    '--workspace_path "" --project-id "" \
                    --private-token "" --target-branch ""',
                )
                gitlab.Gitlab().projects.get().mergerequests.create.assert_called_once()
                git.Repo().git.push.assert_called_once()


def test_update_and_create_mrs_mr_exists_and_no_new_changes(caplog):
    caplog.set_level(logging.INFO)

    git.Repo = MagicMock()
    gitlab.Gitlab = MagicMock()
    git.Repo().git.diff = MagicMock(return_value="")

    with patch(
        "update_components.get_source_branch_if_MR_present",
        MagicMock(return_value="source_branch"),
    ):
        with patch(
            "update_components.get_components",
            MagicMock(return_value=["https://github.com/hathach/tinyusb.git"]),
        ):
            with patch("update_components.update_component", MagicMock()):
                CliRunner().invoke(
                    update_cmake_files,
                    '--workspace_path "" --project-id "" \
                    --private-token "" --target-branch ""',
                )
                gitlab.Gitlab().projects.get().mergerequests.create.assert_not_called()
                git.Repo().git.push.assert_not_called()
