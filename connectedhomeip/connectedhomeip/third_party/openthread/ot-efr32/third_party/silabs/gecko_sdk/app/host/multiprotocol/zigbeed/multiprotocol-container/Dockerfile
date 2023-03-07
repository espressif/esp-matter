ARG SYSTEM_ARCH=arm32v7
ARG BASE_CONTAINER=${SYSTEM_ARCH}/otbr_cpc
FROM    ${BASE_CONTAINER}
ARG SYSTEM_ARCH=arm32v7a # trick to make the build environment have the variable defined

# EMZIGBEE-10110: Disable the "apt install" as we want to avoid fetching packages from the Internet
# for short term. Instead, we are not fetching packages from Nexus and install them locally.
# For long term, we would need to redesign the container build process.
#RUN DEBIAN_FRONTEND=noninteractive apt update && apt install -y socat tmux procps libreadline8 systemd init lsb-release libmbedtls-dev

# Install some non-essential software
#RUN DEBIAN_FRONTEND=noninteractive apt update && apt install -y nano gdb

# Install BlueZ
#RUN DEBIAN_FRONTEND=noninteractive apt update && apt install -y bluetooth bluez bluez-tools rfkill libbluetooth-dev

# Copy and Install packages locally
RUN mkdir mp_deb_arm32v7
RUN mkdir mp_deb_arm64v8
COPY mp_deb_arm32v7/* mp_deb_arm32v7/
COPY mp_deb_arm64v8/* mp_deb_arm64v8/

# Some packages need to be installed in a certain order and run
# dpkg again at the end to make sure all dependencies are met.
RUN if [ "$SYSTEM_ARCH" = "arm32v7" ]; then       \
        mv *.deb mp_deb_arm32v7;                  \
        dpkg -i mp_deb_arm32v7/linux-libc*.deb;   \
        dpkg -i mp_deb_arm32v7/rpcsvc-proto*.deb; \
        dpkg -i mp_deb_arm32v7/perl-modules*.deb; \
        dpkg -i mp_deb_arm32v7/lib*.deb;          \
        dpkg -i mp_deb_arm32v7/systemd_*.deb;     \
        dpkg -i mp_deb_arm32v7/*.deb;             \
        dpkg -i mp_deb_arm32v7/*.deb;             \
    elif [ "$SYSTEM_ARCH" = "arm64v8" ]; then     \
        mv *.deb mp_deb_arm64v8;                  \
        dpkg -i mp_deb_arm64v8/linux-libc*.deb;   \
        dpkg -i mp_deb_arm64v8/rpcsvc-proto*.deb; \
        dpkg -i mp_deb_arm64v8/perl-modules*.deb; \
        dpkg -i mp_deb_arm64v8/lib*.deb;          \
        dpkg -i mp_deb_arm64v8/systemd_*.deb;     \
        dpkg -i mp_deb_arm64v8/*.deb;             \
        dpkg -i mp_deb_arm64v8/*.deb;             \
    fi
# This will resolve the unconfigured packages
RUN apt-get install -f -y

RUN systemctl disable bluetooth.service
# Remove packages after install
RUN rm -fr mp_deb_arm32v7
RUN rm -fr mp_deb_arm64v8

# Remove "tail -f /var/log/syslog" from shell script
RUN sed -i '$ d' /app/etc/docker/docker_entrypoint.sh
# Fix 'pam_env' issue with unset locale
RUN locale >/etc/default/locale

# Add in systemd files
COPY _artifacts/systemd/* /etc/systemd/system/
RUN systemctl enable cpcd.service
ENTRYPOINT ["/sbin/init"]

# Install artifacts
COPY _artifacts/bin_${SYSTEM_ARCH}/* /usr/local/bin/
COPY _artifacts/etc/* /usr/local/etc/
COPY _artifacts/firmware/* /opt/
