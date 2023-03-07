# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Helpful commands for working with a Git repository."""

import logging
from pathlib import Path
import subprocess
from typing import Collection, Iterable, List, Optional, Pattern, Union

from pw_presubmit.tools import log_run, plural

_LOG = logging.getLogger(__name__)
PathOrStr = Union[Path, str]
PatternOrStr = Union[Pattern, str]

TRACKING_BRANCH_ALIAS = '@{upstream}'
_TRACKING_BRANCH_ALIASES = TRACKING_BRANCH_ALIAS, '@{u}'


def git_stdout(
    *args: PathOrStr, show_stderr=False, repo: PathOrStr = '.'
) -> str:
    return (
        log_run(
            ['git', '-C', str(repo), *args],
            stdout=subprocess.PIPE,
            stderr=None if show_stderr else subprocess.DEVNULL,
            check=True,
        )
        .stdout.decode()
        .strip()
    )


def _ls_files(args: Collection[PathOrStr], repo: Path) -> Iterable[Path]:
    """Returns results of git ls-files as absolute paths."""
    git_root = repo.resolve()
    for file in git_stdout('ls-files', '--', *args, repo=repo).splitlines():
        full_path = git_root / file
        # Modified submodules will show up as directories and should be ignored.
        if full_path.is_file():
            yield full_path


def _diff_names(
    commit: str, pathspecs: Collection[PathOrStr], repo: Path
) -> Iterable[Path]:
    """Returns absolute paths of files changed since the specified commit."""
    git_root = root(repo)
    for file in git_stdout(
        'diff',
        '--name-only',
        '--diff-filter=d',
        commit,
        '--',
        *pathspecs,
        repo=repo,
    ).splitlines():
        full_path = git_root / file
        # Modified submodules will show up as directories and should be ignored.
        if full_path.is_file():
            yield full_path


def tracking_branch(repo_path: Optional[Path] = None) -> Optional[str]:
    """Returns the tracking branch of the current branch.

    Since most callers of this function can safely handle a return value of
    None, suppress exceptions and return None if there is no tracking branch.

    Args:
      repo_path: repo path from which to run commands; defaults to Path.cwd()

    Raises:
      ValueError: if repo_path is not in a Git repository

    Returns:
      the remote tracking branch name or None if there is none
    """
    if repo_path is None:
        repo_path = Path.cwd()

    if not is_repo(repo_path or Path.cwd()):
        raise ValueError(f'{repo_path} is not within a Git repository')

    # This command should only error out if there's no upstream branch set.
    try:
        return git_stdout(
            'rev-parse',
            '--abbrev-ref',
            '--symbolic-full-name',
            TRACKING_BRANCH_ALIAS,
            repo=repo_path,
        )

    except subprocess.CalledProcessError:
        return None


def list_files(
    commit: Optional[str] = None,
    pathspecs: Collection[PathOrStr] = (),
    repo_path: Optional[Path] = None,
) -> List[Path]:
    """Lists files with git ls-files or git diff --name-only.

    Args:
      commit: commit to use as a base for git diff
      pathspecs: Git pathspecs to use in git ls-files or diff
      repo_path: repo path from which to run commands; defaults to Path.cwd()

    Returns:
      A sorted list of absolute paths
    """
    if repo_path is None:
        repo_path = Path.cwd()

    if commit in _TRACKING_BRANCH_ALIASES:
        commit = tracking_branch(repo_path)

    if commit:
        try:
            return sorted(_diff_names(commit, pathspecs, repo_path))
        except subprocess.CalledProcessError:
            _LOG.warning(
                'Error comparing with base revision %s of %s, listing all '
                'files instead of just changed files',
                commit,
                repo_path,
            )

    return sorted(_ls_files(pathspecs, repo_path))


def has_uncommitted_changes(repo: Optional[Path] = None) -> bool:
    """Returns True if the Git repo has uncommitted changes in it.

    This does not check for untracked files.
    """
    if repo is None:
        repo = Path.cwd()

    # Refresh the Git index so that the diff-index command will be accurate.
    # The `git update-index` command isn't reliable when run in parallel with
    # other processes that may touch files in the repo directory, so retry a
    # few times before giving up. The hallmark of this failure mode is the lack
    # of an error message on stderr, so if we see something there we can assume
    # it's some other issue and raise.
    retries = 6
    for i in range(retries):
        try:
            log_run(
                ['git', '-C', repo, 'update-index', '-q', '--refresh'],
                capture_output=True,
                check=True,
            )
        except subprocess.CalledProcessError as err:
            if err.stderr or i == retries - 1:
                raise
            continue
    # diff-index exits with 1 if there are uncommitted changes.
    return (
        log_run(
            ['git', '-C', repo, 'diff-index', '--quiet', 'HEAD', '--']
        ).returncode
        == 1
    )


def _describe_constraints(
    git_root: Path,
    repo_path: Path,
    commit: Optional[str],
    pathspecs: Collection[PathOrStr],
    exclude: Collection[Pattern[str]],
) -> Iterable[str]:
    if not git_root.samefile(repo_path):
        yield (
            f'under the {repo_path.resolve().relative_to(git_root.resolve())} '
            'subdirectory'
        )

    if commit in _TRACKING_BRANCH_ALIASES:
        commit = tracking_branch(git_root)
        if commit is None:
            _LOG.warning(
                'Attempted to list files changed since the remote tracking '
                'branch, but the repo is not tracking a branch'
            )

    if commit:
        yield f'that have changed since {commit}'

    if pathspecs:
        paths_str = ', '.join(str(p) for p in pathspecs)
        yield f'that match {plural(pathspecs, "pathspec")} ({paths_str})'

    if exclude:
        yield (
            f'that do not match {plural(exclude, "pattern")} ('
            + ', '.join(p.pattern for p in exclude)
            + ')'
        )


def describe_files(
    git_root: Path,
    repo_path: Path,
    commit: Optional[str],
    pathspecs: Collection[PathOrStr],
    exclude: Collection[Pattern],
    project_root: Optional[Path] = None,
) -> str:
    """Completes 'Doing something to ...' for a set of files in a Git repo."""
    constraints = list(
        _describe_constraints(git_root, repo_path, commit, pathspecs, exclude)
    )

    name = git_root.name
    if project_root and project_root != git_root:
        name = str(git_root.relative_to(project_root))

    if not constraints:
        return f'all files in the {name} repo'

    msg = f'files in the {name} repo'
    if len(constraints) == 1:
        return f'{msg} {constraints[0]}'

    return msg + ''.join(f'\n    - {line}' for line in constraints)


def root(repo_path: PathOrStr = '.', *, show_stderr: bool = True) -> Path:
    """Returns the repository root as an absolute path.

    Raises:
      FileNotFoundError: the path does not exist
      subprocess.CalledProcessError: the path is not in a Git repo
    """
    repo_path = Path(repo_path)
    if not repo_path.exists():
        raise FileNotFoundError(f'{repo_path} does not exist')

    return Path(
        git_stdout(
            'rev-parse',
            '--show-toplevel',
            repo=repo_path if repo_path.is_dir() else repo_path.parent,
            show_stderr=show_stderr,
        )
    )


def within_repo(repo_path: PathOrStr = '.') -> Optional[Path]:
    """Similar to root(repo_path), returns None if the path is not in a repo."""
    try:
        return root(repo_path, show_stderr=False)
    except subprocess.CalledProcessError:
        return None


def is_repo(repo_path: PathOrStr = '.') -> bool:
    """True if the path is tracked by a Git repo."""
    return within_repo(repo_path) is not None


def path(
    repo_path: PathOrStr,
    *additional_repo_paths: PathOrStr,
    repo: PathOrStr = '.',
) -> Path:
    """Returns a path relative to a Git repository's root."""
    return root(repo).joinpath(repo_path, *additional_repo_paths)


def commit_message(commit: str = 'HEAD', repo: PathOrStr = '.') -> str:
    return git_stdout('log', '--format=%B', '-n1', commit, repo=repo)


def commit_author(commit: str = 'HEAD', repo: PathOrStr = '.') -> str:
    return git_stdout('log', '--format=%ae', '-n1', commit, repo=repo)


def commit_hash(
    rev: str = 'HEAD', short: bool = True, repo: PathOrStr = '.'
) -> str:
    """Returns the commit hash of the revision."""
    args = ['rev-parse']
    if short:
        args += ['--short']
    args += [rev]
    return git_stdout(*args, repo=repo)


def discover_submodules(
    superproject_dir: Path, excluded_paths: Collection[PatternOrStr] = ()
) -> List[Path]:
    """Query git and return a list of submodules in the current project.

    Args:
        superproject_dir: Path object to directory under which we are looking
                          for submodules. This will also be included in list
                          returned unless excluded.
        excluded_paths: Pattern or string that match submodules that should not
                        be returned. All matches are done on posix style paths.

    Returns:
        List of "Path"s which were found but not excluded, this includes
        superproject_dir unless excluded.
    """
    discovery_report = git_stdout(
        'submodule',
        'foreach',
        '--quiet',
        '--recursive',
        'echo $toplevel/$sm_path',
        repo=superproject_dir,
    )
    module_dirs = [Path(line) for line in discovery_report.split()]
    # The superproject is omitted in the prior scan.
    module_dirs.append(superproject_dir)

    for exclude in excluded_paths:
        if isinstance(exclude, Pattern):
            for module_dir in reversed(module_dirs):
                if exclude.fullmatch(module_dir.as_posix()):
                    module_dirs.remove(module_dir)
        else:
            for module_dir in reversed(module_dirs):
                if exclude == module_dir.as_posix():
                    module_dirs.remove(module_dir)

    return module_dirs
