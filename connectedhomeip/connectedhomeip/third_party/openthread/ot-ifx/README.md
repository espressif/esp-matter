[![Build][ot-gh-action-build-svg]][ot-gh-action-build]

[ot-gh-action-build]: https://github.com/openthread/ot-ifx/actions?query=workflow%3ABuild+branch%3Amain+event%3Apush
[ot-gh-action-build-svg]: https://github.com/openthread/ot-ifx/workflows/Build/badge.svg?branch=main&event=push

---

# Table of Contents

- [OpenThread on CYW30739 Examples](#openthread-on-cyw30739-examples)
  - [Toolchain](#toolchain)
  - [Building](#building)
  - [Flash Binaries](#flash-binaries)
  - [Interact](#interact)
- [Contributing](#contributing)
- [License](#license)
- [Need help?](#need-help)

---

# OpenThread on Infineon CYW30739 Examples

This repo contains example platform drivers for the [Infineon CYW30739][30739].

[30739]: https://github.com/Infineon/30739A0

The example platform drivers are intended to present the minimal code necessary to support OpenThread.

## Toolchain

Download and install the [GNU toolchain for ARM Cortex-M][gnu-toolchain].

[gnu-toolchain]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm

In a Bash terminal, follow these instructions to install the GNU toolchain and other dependencies.

```bash
$ cd <path-to-ot-ifx>
$ git submodule update --init
$ ./script/bootstrap
```

## Building

In a Bash terminal, follow these instructions to build the CYW30739 examples.

```bash
$ cd <path-to-ot-ifx>
$ ./script/build platform=XXX board=YYY or ./script/build all
```

## Flash Binaries

### Enter Recovery Mode

Put the CYW30739 in to the recovery mode before running the flash script.

1. Press and hold the `RECOVERY` button on the board.
2. Press and hold the `RESET` button on the board.
3. Release the `RESET` button.
4. After one second, release the `RECOVERY` button.

### Run Flash Script

If the build completed successfully, the `elf` files may be found in `<path-to-cyw30739>/build/<board>/bin/`.

In a Bash terminal, follow these instructions to flash the binary.

```bash
$ cd <path-to-cyw30739>
$ ./script/flash [-h] [-v] -p <serial port> -b <board> -d <device_type: ftd, mtd>
```

After flash done, the log would be as below:

```bash
wiced_platform_bt_management_callback: event(21).
wiced_platform_application_thread_handler
system_post_init
Free RAM sizes: 256168
```

## Interact

1. Connect the CYW30739 board UART pins to an external UART board pins.
   - `CYW30739 D10` :left_right_arrow: `UART TX`
   - `CYW30739 D11` :left_right_arrow: `UART RX`
2. Open terminal to the external UART board (serial port settings: 3,000,000 8-N-1).
3. Type `help` for list of commands.
4. See [OpenThread CLI Reference README.md][cli] to learn more.

[cli]: https://github.com/openthread/openthread/blob/main/src/cli/README.md

# Contributing

We would love for you to contribute to OpenThread and help make it even better than it is today! See our [Contributing Guidelines](https://github.com/openthread/openthread/blob/main/CONTRIBUTING.md) for more information.

Contributors are required to abide by our [Code of Conduct](https://github.com/openthread/openthread/blob/main/CODE_OF_CONDUCT.md) and [Coding Conventions and Style Guide](https://github.com/openthread/openthread/blob/main/STYLE_GUIDE.md).

# License

OpenThread is released under the [BSD 3-Clause license](https://github.com/openthread/ot-ifx/blob/main/LICENSE). See the [`LICENSE`](https://github.com/openthread/ot-ifx/blob/main/LICENSE) file for more information.

Please only use the OpenThread name and marks when accurately referencing this software distribution. Do not use the marks in a way that suggests you are endorsed by or otherwise affiliated with Nest, Google, or The Thread Group.

# Need help?

OpenThread support is available on GitHub:

- Bugs and feature requests pertaining to the OpenThread on Infineon CYW30739 Examples — [submit to the openthread/ot-ifx Issue Tracker](https://github.com/openthread/ot-ifx/issues)
- OpenThread bugs and feature requests — [submit to the OpenThread Issue Tracker](https://github.com/openthread/openthread/issues)
- Community Discussion - [ask questions, share ideas, and engage with other community members](https://github.com/openthread/openthread/discussions)
