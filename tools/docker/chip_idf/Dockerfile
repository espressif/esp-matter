ARG VERSION=latest
FROM ghcr.io/project-chip/chip-build:${VERSION} as build

# Use IDF_CHECKOUT_REF to specify a tag, a branch or a specific commit ID.

ARG IDF_CLONE_URL=https://github.com/espressif/esp-idf.git
ARG IDF_CHECKOUT_REF=v5.2.3

# install requirements for pytest
RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -fy --fix-missing --no-install-recommends \
        dbus \
        bluez \
    && : # last line

RUN set -x \
    && mkdir -p /tmp/esp-idf \
    && cd /tmp/esp-idf \
    && git init \
    && git remote add origin $IDF_CLONE_URL \
    && git fetch origin --depth=1 --recurse-submodules ${IDF_CHECKOUT_REF} \
    && git checkout FETCH_HEAD \
    && git submodule update --init --recursive  --depth 1 \
    && : # last line

FROM ghcr.io/project-chip/chip-build:${VERSION}

ENV IDF_PATH=/opt/espressif/esp-idf/
ENV IDF_TOOLS_PATH=/opt/espressif/tools

COPY --from=build /tmp/esp-idf /opt/espressif/esp-idf

ARG IDF_CHECKOUT_REF
RUN if [ "$IDF_CHECKOUT_REF" = "v4.4.3" ]; then \
    sed -i 's/^gdbgui==/# gdbgui ==/' /opt/espressif/esp-idf/requirements.txt; \
    fi

RUN apt-get update && apt-get install -y python3-virtualenv

# Setup the ESP-IDF
WORKDIR /opt/espressif/esp-idf
RUN set -x \
    && ./install.sh \
    && : # last line

ARG CHIP_CLONE_URL=https://github.com/espressif/connectedhomeip.git
ARG CHIP_CHECKOUT_REF=9f6d627e0262e1d023986291948bb4e845be803e

WORKDIR /opt/espressif
ENV CHIP_SUBMODULE_PATH=/opt/espressif/connectedhomeip

RUN set -x \
    && mkdir -p $CHIP_SUBMODULE_PATH \
    && cd $CHIP_SUBMODULE_PATH \
    && git init \
    && git remote add origin $CHIP_CLONE_URL \
    && git fetch origin --depth=1  ${CHIP_CHECKOUT_REF} \
    && git checkout FETCH_HEAD \
    && ./scripts/checkout_submodules.py --platform esp32 linux --shallow \
    && bash -c "source scripts/bootstrap.sh" \
    && ./scripts/build_python.sh --enable_ble true --chip_detail_logging true -i out/py-env \
    && bash -c "source out/py-env/bin/activate" \
    && ./scripts/examples/gn_build_example.sh examples/chip-tool out/host \
    && : # last line

COPY entrypoint.sh /opt/espressif/entrypoint.sh
ENTRYPOINT [ "/opt/espressif/entrypoint.sh" ]
CMD [ "/bin/bash" ]
