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

def make_dist():
    return default_python_distribution()

def make_exe(dist):
    python_config = PythonInterpreterConfig(
         run_module='pw_env_setup.env_setup',
    )

    exe = dist.to_python_executable(
        name="pw_env_setup",
        config=python_config,
        extension_module_filter='all',
        include_sources=True,
        include_resources=True,
        include_test=False,
    )
    exe.add_python_resources(dist.pip_install(["${PW_ROOT}/pw_env_setup/py"]))

    return exe

def make_embedded_data(exe):
    return exe.to_embedded_data()

def make_install(exe):
    files = FileManifest()
    files.add_python_resource(".", exe)
    return files

register_target("dist", make_dist)
register_target("exe", make_exe, depends=["dist"], default=True)
register_target("embedded", make_embedded_data, depends=["exe"],
                default_build_script=True)
register_target("install", make_install, depends=["exe"])

resolve_targets()

# END OF COMMON USER-ADJUSTED SETTINGS.
#
# Everything below this is typically managed by PyOxidizer and doesn't need
# to be updated by people.

PYOXIDIZER_VERSION = "0.6.0"
PYOXIDIZER_COMMIT = "UNKNOWN"
