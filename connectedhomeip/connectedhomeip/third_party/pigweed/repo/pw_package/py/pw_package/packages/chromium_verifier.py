# Copyright 2021 The Pigweed Authors
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
"""Install and check status of BoringSSL + Chromium verifier."""

import pathlib
from typing import Sequence
import pw_package.git_repo
import pw_package.package_manager

# List of sources to checkout for chromium verifier.
# The list is hand-picked. It is currently only tested locally (i.e. the list
# compiles and can run certificate chain verification). Unittest will be added
# in pw_tls_client that uses the this package, so that it can be used as a
# criterion for rolling.
CHROMIUM_VERIFIER_LIBRARY_SOURCES = [
    'base/*',
    '!base/check.h',
    '!base/check_op.h',
    '!base/logging.h',
    'build/buildflag.h',
    'build/write_buildflag_header.py',
    'crypto',
    'net/base',
    'net/cert',
    'net/data',
    'net/der',
    'testing/gtest/include',
    'testing/gmock/include',
    'third_party/abseil-cpp',
    'third_party/boringssl',
    'third_party/googletest',
    'time/internal/cctz/include/cctz/civil_time_detail.h',
    'url/gurl.h',
    'url/third_party/mozilla/url_parse.h',
    'url/url_canon.h',
    'url/url_canon_ip.h',
    'url/url_canon_stdstring.h',
    'url/url_constants.h',
    'net/test/test_certificate_data.h',
    'net/cert/internal/path_builder_unittest.cc',
    'third_party/modp_b64',
]

CHROMIUM_VERIFIER_UNITTEST_SOURCES = [
    # TODO(pwbug/394): Look into in necessary unittests to port.
    'net/cert/internal/path_builder_unittest.cc',
]

CHROMIUM_VERIFIER_SOURCES = (
    CHROMIUM_VERIFIER_LIBRARY_SOURCES + CHROMIUM_VERIFIER_UNITTEST_SOURCES
)


def chromium_verifier_repo_path(
    chromium_verifier_install: pathlib.Path,
) -> pathlib.Path:
    """Return the sub-path for repo checkout of chromium verifier"""
    return chromium_verifier_install / 'src'


def chromium_third_party_boringssl_repo_path(
    chromium_verifier_repo: pathlib.Path,
) -> pathlib.Path:
    """Returns the path of third_party/boringssl library in chromium repo"""
    return chromium_verifier_repo / 'third_party' / 'boringssl' / 'src'


def chromium_third_party_googletest_repo_path(
    chromium_verifier_repo: pathlib.Path,
) -> pathlib.Path:
    """Returns the path of third_party/googletest in chromium repo"""
    return chromium_verifier_repo / 'third_party' / 'googletest' / 'src'


class ChromiumVerifier(pw_package.package_manager.Package):
    """Install and check status of Chromium Verifier"""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, name='chromium_verifier', **kwargs)
        self._chromium_verifier = pw_package.git_repo.GitRepo(
            name='chromium_verifier',
            url='https://chromium.googlesource.com/chromium/src',
            commit='04ebce24d98339954fb1d2a67e68da7ca81ca47c',
            sparse_list=CHROMIUM_VERIFIER_SOURCES,
        )

        # The following is for checking out necessary headers of
        # boringssl and googletest third party libraries that chromium verifier
        # depends on. The actual complete libraries will be separate packages.

        self._boringssl = pw_package.git_repo.GitRepo(
            name='boringssl',
            url=''.join(
                [
                    'https://pigweed.googlesource.com',
                    '/third_party/boringssl/boringssl',
                ]
            ),
            commit='9f55d972854d0b34dae39c7cd3679d6ada3dfd5b',
            sparse_list=['include'],
        )

        self._googletest = pw_package.git_repo.GitRepo(
            name='googletest',
            url=''.join(
                [
                    'https://chromium.googlesource.com/',
                    'external/github.com/google/googletest.git',
                ]
            ),
            commit='53495a2a7d6ba7e0691a7f3602e9a5324bba6e45',
            sparse_list=[
                'googletest/include',
                'googlemock/include',
            ],
        )

    def install(self, path: pathlib.Path) -> None:
        # Checkout chromium verifier
        chromium_repo = chromium_verifier_repo_path(path)
        self._chromium_verifier.install(chromium_repo)

        # Checkout third party boringssl headers
        boringssl_repo = chromium_third_party_boringssl_repo_path(chromium_repo)
        self._boringssl.install(boringssl_repo)

        # Checkout third party googletest headers
        googletest_repo = chromium_third_party_googletest_repo_path(
            chromium_repo
        )
        self._googletest.install(googletest_repo)

    def status(self, path: pathlib.Path) -> bool:
        chromium_repo = chromium_verifier_repo_path(path)
        if not self._chromium_verifier.status(chromium_repo):
            return False

        boringssl_repo = chromium_third_party_boringssl_repo_path(chromium_repo)
        if not self._boringssl.status(boringssl_repo):
            return False

        googletest_repo = chromium_third_party_googletest_repo_path(
            chromium_repo
        )
        if not self._googletest.status(googletest_repo):
            return False

        return True

    def info(self, path: pathlib.Path) -> Sequence[str]:
        return (
            f'{self.name} installed in: {path}',
            'Enable by running "gn args out" and adding this line:',
            f'  dir_pw_third_party_chromium_verifier = {path}',
        )


pw_package.package_manager.register(ChromiumVerifier)
