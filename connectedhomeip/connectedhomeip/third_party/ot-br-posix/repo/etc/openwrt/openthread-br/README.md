# OpenThread Border Router on OpenWRT

## Build

This is for local development.

### 1. Add OpenThread feed

Assuming `OPENWRT_TOP_SRCDIR` is the root of openwrt sources.

```bash
echo src-link openthread "$(pwd)/etc/openwrt" >> ${OPENWRT_TOP_SRCDIR}/feeds.conf
cd "${OPENWRT_TOP_SRCDIR}"
./scripts/feeds update openthread
./scripts/feeds install openthread-br
```

### 2. Enable OpenThread Border Router

OpenThread is not selected by default, so use menuconfig to select openthread-br (OpenThread Border Router).

```bash
make menuconfig
```

In the configure window, use the Up and Down keys to move the cursor and the Left and Right keys to choose an action.

1. Select _Network_ to enter its submenu.
2. Enable _openthread-br_ by moving the cursor to it and pressing **Y**.
3. Select _Exit_ to exit.

### 3. Build OpenThread Border Router

```bash
make package/openthread-br/compile
```

or to do a verbose make for debugging:

```bash
make -j1 V=sc package/openthread-br/compile
```

### 4. Install

Copy the generated **ipk** file into OpenWRT, and install with **opkg**.

```bash
opkg install openthread-br-1.0*.ipk
```

## Usage

### Start

Start otbr-agent manually:

```bash
# Assuming that ttyUSB0 is a RCP with baudrate 115200.
/usr/sbin/otbr-agent 'spinel+hdlc+uart:///dev/ttyUSB0?uart-baudrate=115200'
```

Edit the service file `/etc/init.d/otbr-agent` if RCP device is not `/dev/ttyUSB0` and then start with:

```bash
service otbr-agent start
```

### Test

Enter the OpenWRT web GUI. There will be a new item _Thread_ under _Network_. Scan or create network here.
