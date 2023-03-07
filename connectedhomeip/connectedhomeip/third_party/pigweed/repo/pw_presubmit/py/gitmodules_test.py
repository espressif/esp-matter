#!/usr/bin/env python3
# Copyright 2022 The Pigweed Authors
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
"""Tests for gitmodules."""

from pathlib import Path
import tempfile
from typing import Optional
import unittest
from unittest.mock import MagicMock

from pw_presubmit import gitmodules, PresubmitFailure


def dotgitmodules(
    name: str = 'foo',
    url: Optional[str] = None,
    host: Optional[str] = None,
    branch: Optional[str] = 'main',
):
    cfg = f'[submodule "{name}"]\n'
    cfg += f'path = {name}\n'
    if url is None and host is None:
        host = 'host'
    if host:
        cfg += f'url = https://{host}.googlesource.com/{name}\n'
    else:
        assert url
        cfg += f'url = {url}\n'
    if branch:
        cfg += f'branch = {branch}\n'
    return cfg


class TestGitmodules(unittest.TestCase):
    """Test gitmodules check."""

    def setUp(self):
        self.ctx: MagicMock = None

    def _run(self, config: gitmodules.Config, contents: str) -> None:
        self.ctx = MagicMock()
        self.ctx.fail = MagicMock()

        with tempfile.TemporaryDirectory() as tempdir:
            path = Path(tempdir) / '.gitmodules'
            with path.open('w') as outs:
                outs.write(contents)

            gitmodules.process_gitmodules(self.ctx, config, path)

    def test_ok_default(self) -> None:
        self._run(gitmodules.Config(), dotgitmodules(url='../foo'))
        self.ctx.fail.assert_not_called()

    def test_ok_restrictive(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allow_non_googlesource_hosts=False,
            allowed_googlesource_hosts=('host',),
            require_relative_urls=True,
            allow_sso=False,
            allow_git_corp_google_com=False,
            require_branch=True,
        )
        self._run(cfg, dotgitmodules(url='../foo'))
        self.ctx.fail.assert_not_called()

    def test_validate_ok(self) -> None:
        def validator(ctx, path, name, props) -> None:
            _ = name
            if 'bad' in props['url']:
                ctx.fail('bad', path)

        cfg: gitmodules.Config = gitmodules.Config(validator=validator)
        self._run(cfg, dotgitmodules(host='host'))
        self.ctx.fail.assert_not_called()

    def test_validate_fail(self) -> None:
        def validator(ctx, path, name, props) -> None:
            _ = name
            if 'bad' in props['url']:
                ctx.fail('bad', path)

        cfg: gitmodules.Config = gitmodules.Config(validator=validator)
        self._run(cfg, dotgitmodules(host='badhost'))
        self.ctx.fail.assert_called()

    def test_non_google_ok(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allow_non_googlesource_hosts=True
        )
        self._run(cfg, dotgitmodules(url='https://github.com/foo/bar'))
        self.ctx.fail.assert_not_called()

    def test_non_google_fail(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allow_non_googlesource_hosts=False
        )
        self._run(cfg, dotgitmodules(url='https://github.com/foo/bar'))
        self.ctx.fail.assert_called()

    def test_bad_allowed_googlesource_hosts(self) -> None:
        with self.assertRaises(PresubmitFailure):
            cfg: gitmodules.Config = gitmodules.Config(
                allowed_googlesource_hosts=('pigweed-review',)
            )
            self._run(cfg, dotgitmodules())

    def test_bad_type_allowed_googlesource_hosts(self) -> None:
        with self.assertRaises(AssertionError):
            cfg: gitmodules.Config = gitmodules.Config(
                allowed_googlesource_hosts=('pigweed')
            )
            self._run(cfg, dotgitmodules())

    def test_allowed_googlesource_hosts_ok(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allowed_googlesource_hosts=(
                'pigweed',
                'pigweed-internal',
            )
        )
        self._run(cfg, dotgitmodules(host='pigweed-internal'))
        self.ctx.fail.assert_not_called()

    def test_allowed_googlesource_hosts_fail(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allowed_googlesource_hosts=('pigweed-internal',)
        )
        self._run(cfg, dotgitmodules(host='pigweed'))
        self.ctx.fail.assert_called()

    def test_require_relative_urls_ok(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(require_relative_urls=False)
        self._run(cfg, dotgitmodules(host='foo'))
        self.ctx.fail.assert_not_called()

    def test_require_relative_urls_fail(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(require_relative_urls=True)
        self._run(cfg, dotgitmodules(host='foo'))
        self.ctx.fail.assert_called()

    def test_allow_sso_ok(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(allow_sso=True)
        self._run(cfg, dotgitmodules(url='sso://host/foo'))
        self.ctx.fail.assert_not_called()

    def test_allow_sso_fail(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(allow_sso=False)
        self._run(cfg, dotgitmodules(url='sso://host/foo'))
        self.ctx.fail.assert_called()

    def test_allow_git_corp_google_com_ok(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allow_git_corp_google_com=True
        )
        self._run(cfg, dotgitmodules(url='https://foo.git.corp.google.com/bar'))
        self.ctx.fail.assert_not_called()

    def test_allow_git_corp_google_com_fail(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(
            allow_git_corp_google_com=False
        )
        self._run(cfg, dotgitmodules(url='https://foo.git.corp.google.com/bar'))
        self.ctx.fail.assert_called()

    def test_require_branch_ok(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(require_branch=False)
        self._run(cfg, dotgitmodules(branch=None))
        self.ctx.fail.assert_not_called()

    def test_require_branch_fail(self) -> None:
        cfg: gitmodules.Config = gitmodules.Config(require_branch=True)
        self._run(cfg, dotgitmodules(branch=None))
        self.ctx.fail.assert_called()


if __name__ == '__main__':
    unittest.main()
