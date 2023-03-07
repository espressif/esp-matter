[![Build Status][ot-gh-action-build-svg]][ot-gh-action-build] [![Docker Status][ot-gh-action-docker-svg]][ot-gh-action-docker] [![Build Status][otbr-travis-svg]][otbr-travis] [![Coverage Status][otbr-codecov-svg]][otbr-codecov]

---

# OpenThread Border Router

Per the [Thread Specification](http://threadgroup.org/ThreadSpec), a Thread Border Router connects a Thread network to other IP-based networks, such as Wi-Fi or Ethernet. A Thread network requires a Border Router to connect to other networks.

A Thread Border Router minimally supports the following functions:

- End-to-end IP connectivity via routing between Thread devices and other external IP networks
- External Thread Commissioning (for example, a mobile phone) to authenticate and join a Thread device to a Thread network

<a href="https://www.threadgroup.org/What-is-Thread#certifiedproducts">
<img src="/doc/images/certified.svg" alt="Thread Certified Component" width="150px" align="right">
</a>

OpenThread's implementation of a Border Router is called OpenThread Border Router (OTBR). **OTBR is a Thread Certified Component** on the Raspberry Pi 3B with a Nordic nRF52840 NCP.

OTBR includes a number of features, including:

- Web UI for configuration and management
- Thread Border Agent to support an External Commissioner
- DHCPv6 Prefix Delegation to obtain IPv6 prefixes for a Thread network
- NAT64 for connecting to IPv4 networks
- DNS64 to allow Thread devices to initiate communications by name to an IPv4-only server
- Docker support

More information about Thread can be found at [threadgroup.org](http://threadgroup.org/). Thread is a registered trademark of the Thread Group, Inc.

[ot-gh-action-build]: https://github.com/openthread/ot-br-posix/actions?query=workflow%3ABuild+branch%3Amain+event%3Apush
[ot-gh-action-build-svg]: https://github.com/openthread/ot-br-posix/workflows/Build/badge.svg?branch=main&event=push
[ot-gh-action-docker]: https://github.com/openthread/ot-br-posix/actions?query=workflow%3ADocker+branch%3Amain+event%3Apush
[ot-gh-action-docker-svg]: https://github.com/openthread/ot-br-posix/workflows/Docker/badge.svg?branch=main&event=push
[otbr-travis]: https://travis-ci.org/openthread/ot-br-posix
[otbr-travis-svg]: https://travis-ci.org/openthread/ot-br-posix.svg?branch=main
[otbr-codecov]: https://codecov.io/gh/openthread/ot-br-posix
[otbr-codecov-svg]: https://codecov.io/gh/openthread/ot-br-posix/branch/main/graph/badge.svg

## Getting started

The quickest way to get started with OTBR is to try the Docker version. Run OTBR in a Docker container on any Linux-based system or a Raspberry Pi 3B, using either a physical or emulated NCP. See the [Docker Support guide on openthread.io](https://openthread.io/guides/border-router/docker) for more info.

OTBR also runs directly on supported platforms like the Raspberry Pi 3B. If you're interested in building and configuring OTBR directly, or to learn more about the OTBR architecture, then see the rest of our end-user documentation at [openthread.io](https://openthread.io/guides/border_router).

> Note: For users in China, end-user documentation is available at [openthread.google.cn](https://openthread.google.cn/guides/border-router).

If you're interested in contributing to OpenThread Border Router, read on.

# Contributing

We would love for you to contribute to OpenThread Border Router and help make it even better than it is today! See our [Contributing Guidelines](https://github.com/openthread/ot-br-posix/blob/main/CONTRIBUTING.md) for more information.

Contributors are required to abide by our [Code of Conduct](https://github.com/openthread/ot-br-posix/blob/main/CODE_OF_CONDUCT.md) and [Coding Conventions and Style Guide](https://github.com/openthread/ot-br-posix/blob/main/STYLE_GUIDE.md).

We follow the philosophy of [Scripts to Rule Them All](https://github.com/github/scripts-to-rule-them-all).

# Versioning

OpenThread Border Router follows the [Semantic Versioning guidelines](http://semver.org/) for release cycle transparency and to maintain backwards compatibility. OpenThread Border Router's versioning is independent of the Thread protocol specification version but will clearly indicate which version of the specification it currently supports.

# License

OpenThread Border Router is released under the [BSD 3-Clause license](https://github.com/openthread/ot-br-posix/blob/main/LICENSE). See the [`LICENSE`](https://github.com/openthread/ot-br-posix/blob/main/LICENSE) file for more information.

Please only use the OpenThread name and marks when accurately referencing this software distribution. Do not use the marks in a way that suggests you are endorsed by or otherwise affiliated with Nest, Google, or The Thread Group.

# Need help?

OpenThread support is available on GitHub:

- Bugs and feature requests pertaining to the OpenThread Border Router — [submit to the openthread/ot-br-posix Issue Tracker](https://github.com/openthread/ot-br-posix/issues)
- OpenThread bugs and feature requests — [submit to the OpenThread Issue Tracker](https://github.com/openthread/openthread/issues)
- Community Discussion - [ask questions, share ideas, and engage with other community members](https://github.com/openthread/openthread/discussions)

## OpenThread

To learn more about OpenThread, see the [OpenThread repository](https://github.com/openthread/openthread).
