[![Build][ot-gh-action-build-svg]][ot-gh-action-build]

[ot-gh-action-build]: https://github.com/openthread/ot-efr32/actions?query=workflow%3ABuild+branch%3Amain+event%3Apush
[ot-gh-action-build-svg]: https://github.com/openthread/ot-efr32/workflows/Build/badge.svg?branch=main&event=push

---

# OpenThread on EFR32 Examples

This repo contains example platform drivers for the [Silicon Labs EFR32 Mighty Gecko][efr32mg].

[efr32mg]: https://www.silabs.com/support/getting-started/mesh-networking/thread/mighty-gecko

The example platform drivers are intended to present the minimal code necessary to support OpenThread.

When using this repo to generate a solution that requires two different projects, such as an RCP & OTBR, make sure they are relying on the same OpenThread version to ensure maximum compatibility. You can check which commit this repo relies upon in the "openthread" submodule.

If your OTBR project was generated using the Silabs GSDK / Simplicity Studio, we recommend to also use it to generate the RCP project and not this repo. This will mitigate potential incompatibility issues due to mismatched OpenThread versions.

To learn more about building and running the examples please check:

- [OpenThread on EFR32][efr32-page]

[efr32-page]: ./src/README.md

# Contributing

We would love for you to contribute to OpenThread and help make it even better than it is today! See our [Contributing Guidelines](https://github.com/openthread/openthread/blob/main/CONTRIBUTING.md) for more information.

Contributors are required to abide by our [Code of Conduct](https://github.com/openthread/openthread/blob/main/CODE_OF_CONDUCT.md) and [Coding Conventions and Style Guide](https://github.com/openthread/openthread/blob/main/STYLE_GUIDE.md).

# License

OpenThread is released under the [BSD 3-Clause license](https://github.com/openthread/ot-efr32/blob/main/LICENSE). See the [`LICENSE`](https://github.com/openthread/ot-efr32/blob/main/LICENSE) file for more information.

Please only use the OpenThread name and marks when accurately referencing this software distribution. Do not use the marks in a way that suggests you are endorsed by or otherwise affiliated with Nest, Google, or The Thread Group.

# Need help?

OpenThread support is available on GitHub:

- Bugs and feature requests pertaining to the OpenThread on EFR32 Examples — [submit to the openthread/ot-efr32 Issue Tracker](https://github.com/openthread/ot-efr32/issues)
- OpenThread bugs and feature requests — [submit to the OpenThread Issue Tracker](https://github.com/openthread/openthread/issues)
- Community Discussion - [ask questions, share ideas, and engage with other community members](https://github.com/openthread/openthread/discussions)
