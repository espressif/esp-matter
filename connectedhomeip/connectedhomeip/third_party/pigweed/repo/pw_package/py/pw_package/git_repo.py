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
"""Install and check status of Git repository-based packages."""

import os
import pathlib
import shutil
import subprocess
from typing import Union
import urllib.parse

import pw_package.package_manager

PathOrStr = Union[pathlib.Path, str]


def git_stdout(
    *args: PathOrStr, show_stderr=False, repo: PathOrStr = '.'
) -> str:
    return (
        subprocess.run(
            ['git', '-C', repo, *args],
            stdout=subprocess.PIPE,
            stderr=None if show_stderr else subprocess.DEVNULL,
            check=True,
        )
        .stdout.decode()
        .strip()
    )


def git(*args: PathOrStr, repo: PathOrStr = '.') -> subprocess.CompletedProcess:
    return subprocess.run(['git', '-C', repo, *args], check=True)


class GitRepo(pw_package.package_manager.Package):
    """Install and check status of Git repository-based packages."""

    def __init__(
        self, url, *args, commit='', tag='', sparse_list=None, **kwargs
    ):
        super().__init__(*args, **kwargs)
        if not (commit or tag):
            raise ValueError('git repo must specify a commit or tag')

        self._url = url
        self._commit = commit
        self._tag = tag
        self._sparse_list = sparse_list

    def status(self, path: pathlib.Path) -> bool:
        # TODO(tonymd): Check the correct SHA is checked out here.
        if not os.path.isdir(path / '.git'):
            return False

        remote = git_stdout('remote', 'get-url', 'origin', repo=path)
        url = urllib.parse.urlparse(remote)
        if url.scheme == 'sso' or '.git.corp.google.com' in url.netloc:
            host = url.netloc.replace(
                '.git.corp.google.com',
                '.googlesource.com',
            )
            if not host.endswith('.googlesource.com'):
                host += '.googlesource.com'
            remote = 'https://{}{}'.format(host, url.path)

        commit = git_stdout('rev-parse', 'HEAD', repo=path)
        if self._commit and self._commit != commit:
            return False

        if self._tag:
            tag = git_stdout('describe', '--tags', repo=path)
            if self._tag != tag:
                return False

        # If it is a sparse checkout, sparse list shall match.
        if self._sparse_list:
            if not self.check_sparse_list(path):
                return False

        status = git_stdout('status', '--porcelain=v1', repo=path)
        return remote == self._url and not status

    def install(self, path: pathlib.Path) -> None:
        # If already installed and at correct version exit now.
        if self.status(path):
            return

        # Otherwise delete current version and clone again.
        if os.path.isdir(path):
            shutil.rmtree(path)

        if self._sparse_list:
            self.checkout_sparse(path)
        else:
            self.checkout_full(path)

    def checkout_full(self, path: pathlib.Path) -> None:
        # --filter=blob:none means we don't get history, just the current
        # revision. If we later run commands that need history it will be
        # retrieved on-demand. For small repositories the effect is negligible
        # but for large repositories this should be a significant improvement.
        if self._commit:
            git('clone', '--filter=blob:none', self._url, path)
            git('reset', '--hard', self._commit, repo=path)
        elif self._tag:
            git('clone', '-b', self._tag, '--filter=blob:none', self._url, path)

    def checkout_sparse(self, path: pathlib.Path) -> None:
        # sparse checkout
        git('init', path)
        git('remote', 'add', 'origin', self._url, repo=path)
        git('config', 'core.sparseCheckout', 'true', repo=path)

        # Add files to checkout by editing .git/info/sparse-checkout
        with open(path / '.git' / 'info' / 'sparse-checkout', 'w') as sparse:
            for source in self._sparse_list:
                sparse.write(source + '\n')

        # Either pull from a commit or a tag.
        target = self._commit if self._commit else self._tag
        git('pull', '--depth=1', 'origin', target, repo=path)

    def check_sparse_list(self, path: pathlib.Path) -> bool:
        sparse_list = (
            git_stdout('sparse-checkout', 'list', repo=path)
            .strip('\n')
            .splitlines()
        )
        return set(sparse_list) == set(self._sparse_list)
