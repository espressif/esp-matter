# Espressif's SDK for Matter

Docker image for [Espressif's SDK for Matter](https://github.com/espressif/esp-matter),
the official Matter development framework for ESP32-series SoCs.

Each image bundles a copy of esp-matter, esp-idf, and all tools required to build Matter applications.

It provides a ready-to-use build environment for esp-matter projects suitable both for local development
(mount your project and run idf.py build) and for automated/CI builds.

## Tags

Every tag is `<line>` with an optional `_idf_<version>` suffix.

| Tag                          | Built from                                          | When to use                                                                                       |
|------------------------------|-----------------------------------------------------|---------------------------------------------------------------------------------------------------|
| `latest`                     | `main` branch                                       | You want the latest esp-matter development state                                                  |
| `release-vX.Y`               | `release/vX.Y` branch                               | You want a released esp-matter version, e.g. `release-v1.6` (recommended for reproducible builds) |
| `release-vX.Y_idf_<version>` | `release/vX.Y` branch with specific esp-idf release | e.g. `release-v1.6_idf_v6.0.2`. Without the suffix you get the default IDF for that line          |

Recent images are multi-arch (`linux/amd64`, `linux/arm64`); older images (≤ v1.3) are `linux/amd64` only.

## Usage

Build a project in the current directory with `idf.py`:

```bash
docker run --rm -v $PWD:/project -w /project espressif/esp-matter:latest idf.py build
```

Build one of the bundled examples (e.g. `light`) for a target:

```bash
docker run --rm -v $PWD:/work -w /work espressif/esp-matter:latest \
  bash -c "cp -r /opt/espressif/esp-matter/examples/light . && \
           cd light && idf.py set-target esp32c3 build"
```

Pin a specific release + IDF combination:

```bash
docker run --rm -v $PWD:/project -w /project \
  espressif/esp-matter:release-v1.5_idf_v6.0.2 idf.py build
```

> Flashing/monitoring from inside the container needs the serial device passed through
> (`--device=/dev/ttyUSB0`); building does not.

## Building a custom image

The `tools/docker/` Dockerfile in the repo accepts build args to point at a fork or pin
refs — `ESP_MATTER_CLONE_URL`, `ESP_MATTER_CHECKOUT_REF`, `IDF_CLONE_URL`,
`IDF_CHECKOUT_REF`, `IDF_CLONE_SHALLOW`. See
[`tools/docker/README.md`](https://github.com/espressif/esp-matter/blob/main/tools/docker/README.md).

## Links

- Source & issues: <https://github.com/espressif/esp-matter>
- Documentation: <https://docs.espressif.com/projects/esp-matter/>
