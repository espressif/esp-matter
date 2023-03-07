# Developer Tools

This repository contains hooks and templates to be reused by Open IoT SDK projects.

## Templates

We use pipeline templates to simplify our Gitlab CI workflow. All our templates are
located in templates folder in this project.

## Pre-Commit hooks

This repository provides [pre-commit](https://pre-commit.com/) hooks for various code quality checks.

### Using the pre-commit hooks

After `pre-commit` is installed, you need to setup a `.pre-commit-config.yaml` with the following content:

```
-   repo: https://git.gitlab.arm.com/engineering/iot-m-sw/open-iot-sdk/tools/developer-tools.git
    rev: c67e53e3d69c84be73b6027d5754eaf8950144e5
    hooks:
    -   id: cppcheck
    -   id: banned-api
```

You can then run for all files or on every commit as mentioned in the [quick start guide](https://pre-commit.com/index.html#quick-start).

## Pre-commit hooks provide in this repos

### cppcheck

Cppcheck is a static code analysis tool for the C and C++ programming languages.

`cppcheck` needs to be installed for running this hook. Follow the [official documentation](https://cppcheck.sourceforge.io/) to install `cppcheck`.

Refer to the [.pre-commit-hooks.yaml](.pre-commit-hooks.yaml) for the default options used for `cppcheck`.

You can override the default options by passing arguments in `args` that is suited for your project. For example, you may pass something like below in your `.pre-commit-config.yaml`:

```
- id: cppcheck
  name: cppcheck
  description: Run `cppcheck` against C/C++ source files
  language: system
  files: \.(c|cc|cpp|cu|c\+\+|cxx|tpp|txx)$
  entry: cppcheck --error-exitcode=1
  args: [
      "--enable=all"
  ]
```

### banned-api

This pre-commit could be used to check if certain unsafe C/C++ APIs are used in your code. By default, `banned-api` checks the [complete list](hooks/banned_api_list.txt) present in the repository.

You can override the default list by passing your list in `args`. For example, you may pass something like below in your `.pre-commit-config.yaml`:

```
-   repo: https://git.gitlab.oss.arm.com/engineering/iot-m-sw/open-iot-sdk/tools/developer-tools.git
    rev: c67e53e3d69c84be73b6027d5754eaf8950144e5
    hooks:
    -   id: cppcheck
    -   id: banned-api
        args: ["--banned_list_path", ".gitlab/ci/banned_apis.txt"]
```

# License and contributions

The software is provided under the Apache-2.0 license. All contributions to software and documents are licensed by contributors under the same license model as the software/document itself (ie. inbound == outbound licensing). Open IoT SDK may reuse software already licensed under another license, provided the license is permissive in nature and compatible with Apache v2.0.

Folders containing files under different permissive license than Apache 2.0 are listed in the LICENSE file.

Please see [CONTRIBUTING.md](CONTRIBUTING.md) for more information.

## Security issues reporting

If you find any security vulnerabilities, please do not report it in the GitLab issue tracker. Instead, send an email to the security team at arm-security@arm.com stating that you may have found a security vulnerability in the Open IoT SDK.

More details can be found at [Arm Developer website](https://developer.arm.com/support/arm-security-updates/report-security-vulnerabilities).
