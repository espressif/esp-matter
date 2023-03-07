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
"""Internal Bazel helpers for downloading CIPD packages."""

load(
    ":cipd_repository_list_templates.bzl",
    "CIPD_INIT_BZL_TEMPLATE",
    "CIPD_REPOSITORY_TEMPLATE",
)

_CIPD_HOST = "https://chrome-infra-packages.appspot.com"

def platform_normalized(rctx):
    """Normalizes the platform to match CIPDs naming system.

    Args:
        rctx: Repository context.

    Returns:
        str: Normalized string.
    """

    # Chained if else used because Bazel's rctx.os.name is not stable
    # between different versions of windows i.e. windows 10 vs windows
    # server.
    if "windows" in rctx.os.name:
        return "windows"
    elif "linux" == rctx.os.name:
        return "linux"
    elif "mac os x" == rctx.os.name:
        return "mac"
    else:
        fail("Could not normalize os:", rctx.os.name)

# TODO(b/234879770): Enable unused variable check.
# buildifier: disable=unused-variable
def arch_normalized(rctx):
    """Normalizes the architecture string to match CIPDs naming system.

    Args:
        rctx: Repository context.

    Returns:
        str: Normalized architecture.
    """

    # TODO(b/234879770): Find a way to get host architecture information from a
    # repository context.
    return "amd64"

def get_client_cipd_version(rctx):
    """Gets the CIPD client version from the config file.

    Args:
        rctx: Repository context.

    Returns:
        str: The CIPD client version tag to use.
    """
    return rctx.read(rctx.attr._cipd_version_file).strip()

def _platform(rctx):
    return "{}-{}".format(platform_normalized(rctx), arch_normalized(rctx))

def get_client_cipd_digest(rctx):
    """Gets the CIPD client digest from the digest file.

    Args:
        rctx: Repository context.

    Returns:
        str: The CIPD client digest.
    """
    platform = _platform(rctx)
    digest_file = rctx.read(rctx.attr._cipd_digest_file)
    digest_lines = [
        digest
        for digest in digest_file.splitlines()
        # Remove comments from version file
        if not digest.startswith("#") and digest
    ]

    for line in digest_lines:
        (digest_platform, digest_type, digest) = \
            [element for element in line.split(" ") if element]
        if digest_platform == platform:
            if digest_type != "sha256":
                fail("Bazel only supports sha256 type digests.")
            return digest
    fail("Could not find CIPD digest that matches this platform.")

def cipd_client_impl(rctx):
    platform = _platform(rctx)
    path = "/client?platform={}&version={}".format(
        platform,
        get_client_cipd_version(rctx),
    )
    rctx.download(
        output = "cipd",
        url = _CIPD_HOST + path,
        sha256 = get_client_cipd_digest(rctx),
        executable = True,
    )
    rctx.file("BUILD", "exports_files([\"cipd\"])")

def cipd_repository_base(rctx):
    cipd_path = rctx.path(rctx.attr._cipd_client).basename
    ensure_path = rctx.name + ".ensure"
    rctx.template(
        ensure_path,
        Label("@pigweed//pw_env_setup/bazel/cipd_setup:ensure.tpl"),
        {
            "%{path}": rctx.attr.path,
            "%{tag}": rctx.attr.tag,
        },
    )
    rctx.execute([cipd_path, "ensure", "-root", ".", "-ensure-file", ensure_path])

def cipd_repository_impl(rctx):
    cipd_repository_base(rctx)
    rctx.file("BUILD", """
exports_files(glob([\"**/*\"]))

filegroup(
    name = "all",
    srcs = glob(["**/*"]),
    visibility = ["//visibility:public"],
)
""")

def _cipd_path_to_repository_name(path, platform):
    """ Converts a cipd path to a repository name

    Args:
        path: The cipd path.
        platform: The cipd platform name.

    Example:
        print(_cipd_path_to_repository_name(
            "infra/3pp/tools/cpython3/windows-amd64",
            "linux-amd64"
        ))
        >> cipd_infra_3pp_tools_cpython3_windows_amd64
    """
    return "cipd_" + \
           path.replace("/", "_") \
               .replace("${platform}", platform) \
               .replace("-", "_")

def _cipd_dep_to_cipd_repositories_str(dep, indent):
    """ Converts a CIPD dependency to a CIPD repositories string

    Args:
        dep: The CIPD dependency.
        indent: The indentation to use.
    """
    return "\n".join([CIPD_REPOSITORY_TEMPLATE.format(
        name = _cipd_path_to_repository_name(dep["path"], platform),
        path = dep["path"].replace("${platform}", platform),
        tag = dep["tags"][0],
        indent = indent,
    ) for platform in dep["platforms"]])

def cipd_deps_impl(repository_ctx):
    """ Generates a CIPD dependencies file """
    pigweed_deps = json.decode(
        repository_ctx.read(repository_ctx.attr._pigweed_packages_json),
    )["packages"] + json.decode(
        repository_ctx.read(repository_ctx.attr._python_packages_json),
    )["packages"]
    repository_ctx.file("BUILD", "exports_files(glob([\"**/*\"]))\n")

    repository_ctx.file("cipd_init.bzl", CIPD_INIT_BZL_TEMPLATE.format(
        cipd_deps = "\n".join([
            _cipd_dep_to_cipd_repositories_str(dep, indent = "    ")
            for dep in pigweed_deps
        ]),
    ))
