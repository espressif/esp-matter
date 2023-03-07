# Cirque

## Introduction

Cirque simulates complex network topologies based upon docker nodes. On a single Linux machine, it can create multiple nodes with network stacks that are independent from each other. Some nodes may be connected to simulated Thread networks, others may connect to simulated BLE or WiFi. Cirque provides a service (gRPC or Flask REST) to create, destroy and manage multiple home environments with multiple virtual devices and radio capabilities between these devices.

## Installation:
### Prerequisites
```
sudo apt-get install bazel socat psmisc tigervnc-standalone-server tigervnc-viewer python3-pip python3-venv python3-setuptools
sudo pip3 install pycodestyle==2.5.0
```
### Make

```
make install
```

To install cirque without grpc support, just passing NO_GRPC environment variable to make:

```
make NO_GRPC=1 install
```

Note: You can consider running Cirque within a `virtualenv`

```
python3 -m venv venv
source venv/bin/activate
```

## Uninstallation:
```
make uninstall
```

## Fetures
- *IPvlan*: Allow user to be able to create multiple real devices and multiple
  virtual devices within the same private network. 

- *WiFi*: We provide a WiFi AP which used to lease ip addresses to virtual
  devices, therefore, virtual devices could talk to each other and also outside
  the world though simulated radio (linux kernel module: mac80211_hwsim)

- *Thread*: Support thread protocol.


## Test:
The below runs unit tests and integration tests including `test_flask_virtual_home.py` and `test_grpc_virtual_home.py`

```
sudo sh run_tests.sh
```


# Directory Structure

The Cirque repository is structured as follows:

| File / Folder | Contents |
|----|----|
| `ARCHITECTURE.md` | Cirque architecture file. |
| `cirque/` | Implementation of Cirque. |
| `cirque/capabilities/` | Virtual node capabilities implementation.|
| `cirque/common/` | Cirque utility folder including logging, exception and etc.|
| `cirque/connectivity/` | Cirque connectivity handling implementation. |
| `cirque/grpc/` | gRPC service.|
| `cirque/home` | Virtual home implementation.|
| `cirque/nodes` | Cirque Docker node implementation. |
| `cirque/resources` | Reference generic node and wifi AP docker Files. |
| `cirque/restservice` | Cirque rest service. |
| `cirque/proto` | Cirque gRPC proto files. |
| `dependency_modules.sh` | Convenience script to prepare test docker nodes and radio emulator.|
| `LICENSE` | Cirque license file (Apache 2.0). |
| `requirements.txt` | Python pip requirement file. |
| `utils` | Cirque Build utilities. |
| `WORKSPACE` | Bazel workspace. |
| `BUILD` | Bazel Build file.|
| `contributing.md` | Guidelines for contributing to Cirque. |
| `examples/` | Cirque integration examples. |
| `setup.py` | Build script for setuptools. |
| `README.md` | This file. |
| `run_tests.sh` | Cirque unit and integration test script. |
| `version` | Release version tag. |
