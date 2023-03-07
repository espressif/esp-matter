# Open IoT SDK

## License and contributions

The software is provided under the [Apache-2.0 license](LICENSE-apache-2.0.txt). All contributions to software and documents are licensed by contributors under the same license model as the software/document itself (ie. inbound == outbound licensing). Open IoT SDK may reuse software already licensed under another license, provided the license is permissive in nature and compatible with Apache v2.0.

Folders containing files under different permissive license than Apache 2.0 are listed in the [LICENSE](LICENSE.md) file.

To report a security vulnerability, please send an email to the security team at arm-security@arm.com.

For any other contribution, please see [CONTRIBUTING.md](CONTRIBUTING.md) for more information.

## Overview

The Open IoT SDK is a collection of software that has been tested to work well
together. The primary value delivered by the SDK is in making available a set
of components that integrate without conflict, easing the development of IoT
applications. The SDK also provides a few simple examples demonstrating how to
use the included components in a few example configurations.

The `components` directory contains all components that comprise the Open IoT
SDK. Some of the external repositories contain multiple components. The SDK
ships with CMake files to help make consumption of its components more
straightforward than using the components directly.

The following components are not fully integrated yet and are shown as early preview:
`components/`
- `NimBLE/` - in-progress
- `TinyUSB/` - in-progress

## Getting Started

### Prerequisites

For details about both required and optional software, see [Prerequisites.md](./docs/Prerequisites.md).

### Examples

In this Open IoT SDK repository, integration examples are maintained in the form of templates in the
[`templates`](./templates) directory. This helps to avoid duplication when an example has multiple flavors based on
various Real Time Operating Systems (RTOS) and/or target platforms. Those templates are *not* ready to build directly
but need to be converted to full examples which you can built.

#### Officially generated examples

You can get full, ready-to-use examples we have officially generated from the latest Open IoT SDK from

    https://git.gitlab.arm.com/iot/open-iot-sdk/examples/sdk-examples

and follow the `README.md` inside any example you are interested in.

#### Generating examples manually

Alternatively, you can manually convert templates to examples by running the following command:

```
cmake -S templates -B tmp
# Note: You can delete tmp/ now as it only contains temporary CMake cache files
```

This is useful for trying out your locally modified or added example templates.

This produces examples in `__codegen/examples` and integration tests in `__codegen/tests`, each with subdirectories
`<EXAMPLE OR TEST NAME>/<RTOS>/<PLATFORM>`, because some examples and tests support multiple RTOSes and platforms. For
example, `__codegen/examples/cmsis-rtos-api/cmsis-rtx/corstone-300` is the CMSIS RTOS API example based on CMSIS RTX and
running on the Corstone-300 platform.

You can change your working directory to an example you are interested in and follow its `README.md` to build and run
the example.

Tip: `__codegen` is the default output directory, but you can specify your own directory by appending
`-D OUTPUT_DIR=<MY_OUTPUT>` to the `cmake` command above, replacing `<MY_OUTPUT>` with an absolute or relative directory
of your choice.

### Developer documentation

More information such as how to configure the SDK for your own application and contribute to the SDK can be found inside
our [documentation](./docs/README.md).

## Security issues reporting

If you find any security vulnerabilities, please do not report it in the GitLab issue tracker. Instead, send an email to the security team at arm-security@arm.com stating that you may have found a security vulnerability in the Open IoT SDK.

More details can be found at [Arm Developer website](https://developer.arm.com/support/arm-security-updates/report-security-vulnerabilities).
