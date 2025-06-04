# SPDX-License-Identifier: CC0-1.0

import pathlib
import pytest
import time
import re
import pexpect
import subprocess
import netifaces
from typing import Tuple
from pytest_embedded import Dut
import os
import yaml


CURRENT_DIR_LIGHT = str(pathlib.Path(__file__).parent)+'/light'
CHIP_TOOL_EXE = str(pathlib.Path(__file__).parent)+ '/../connectedhomeip/connectedhomeip/out/host/chip-tool'
OT_BR_EXAMPLE_PATH = str(pathlib.Path(__file__).parent)+'/thread_border_router'
OT_DATASET_HEXSTR = '0e08000000000001000035060004001fffe00708fdb824be22185de50c0402a0f7f8051020112014020519772011201402051977030d41706f6c6c6f6e54687265616404101fefc90ee1637d47ca75f87ec24f9403000300000f0208201120140205197701022201'
pytest_build_dir = CURRENT_DIR_LIGHT
pytest_matter_thread_dir = CURRENT_DIR_LIGHT+'|'+OT_BR_EXAMPLE_PATH


@pytest.mark.esp32c3
@pytest.mark.esp_matter_dut
@pytest.mark.parametrize(
    ' count, app_path, target, erase_all', [
        ( 1, pytest_build_dir, 'esp32c3', 'y'),
    ],
    indirect=True,
)

# Matter over wifi commissioning
def test_matter_commissioning_c3(dut:Dut) -> None:
    light = dut
    # BLE start advertising
    light.expect(r'chip\[DL\]\: Configuring CHIPoBLE advertising', timeout=20)
    # Start commissioning
    time.sleep(5)
    command = CHIP_TOOL_EXE + ' pairing ble-wifi 1 ChipTEH2 chiptest123 20202021 3840'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-off the light
    time.sleep(3)
    command = CHIP_TOOL_EXE + ' onoff toggle 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-on the light
    time.sleep(5)
    command = CHIP_TOOL_EXE + ' onoff toggle 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False

@pytest.mark.esp32c2
@pytest.mark.esp_matter_dut
@pytest.mark.parametrize(
    ' count, app_path, target, erase_all', [
        ( 1, pytest_build_dir, 'esp32c2', 'y'),
    ],
    indirect=True,
)

# Matter over wifi commissioning
def test_matter_commissioning_c2(dut:Dut) -> None:
    light = dut
    # BLE start advertising
    light.expect(r'chip\[DL\]\: Configuring CHIPoBLE advertising', timeout=20)
    # Start commissioning
    time.sleep(5)
    command = CHIP_TOOL_EXE + ' pairing ble-wifi 1 ChipTEH2 chiptest123 20202021 3840'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-off the light
    time.sleep(3)
    command = CHIP_TOOL_EXE + ' onoff toggle 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-on the light
    time.sleep(5)
    command = CHIP_TOOL_EXE + ' onoff toggle 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False

@pytest.mark.esp32c6
@pytest.mark.esp_matter_dut
@pytest.mark.parametrize(
    ' count, app_path, target, erase_all', [
        ( 1, pytest_build_dir, 'esp32c6', 'y'),
    ],
    indirect=True,
)

# Matter over wifi commissioning
def test_matter_commissioning_c6(dut:Dut) -> None:
    light = dut
    # BLE start advertising
    light.expect(r'chip\[DL\]\: Configuring CHIPoBLE advertising', timeout=20)
    # Start commissioning
    time.sleep(5)
    command = CHIP_TOOL_EXE + ' pairing ble-wifi 1 ChipTEH2 chiptest123 20202021 3840'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-off the light
    time.sleep(3)
    command = CHIP_TOOL_EXE + ' onoff toggle 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-on the light
    time.sleep(5)
    command = CHIP_TOOL_EXE + ' onoff toggle 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False


# get the host interface name
def get_host_interface_name() -> str:
    home_dir = os.path.expanduser("~")
    config_path = os.path.join(home_dir, "config", "env_config.yml")
    if os.path.exists(config_path):
        with open(config_path, 'r') as file:
            config = yaml.safe_load(file)
        interface_name = config.get("interface_name")
        if interface_name:
            return str(interface_name)
        else:
            print("Warning: Configuration file found but 'interface_name' is not defined.")
  
    if "eth1" in netifaces.interfaces():
        return "eth1"
    raise Exception("No valid network interface found. Please ensure 'eth1' exists or configure 'interface_name' in config/env_config file.")


# reset host interface
def reset_host_interface() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ifconfig ' + interface_name + ' down'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'ifconfig ' + interface_name + ' up'
        subprocess.call(command, shell=True, timeout=10)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


# set interface sysctl options
def set_interface_sysctl_options() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'sysctl -w net/ipv6/conf/' + interface_name + '/accept_ra=2'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'sysctl -w net/ipv6/conf/' + interface_name + '/accept_ra_rt_info_max_plen=128'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'sysctl -w net.ipv6.conf.all.forwarding=1'
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(2)
        assert flag


# initialize interface ipv6 address
def init_interface_ipv6_address() -> None:
    interface_name = get_host_interface_name()
    flag = False
    try:
        command = 'ip -6 route | grep ' + interface_name + " | grep ra | awk {'print $1'} | xargs -I {} ip -6 route del {}"
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(0.5)
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        command = 'ip -6 address show dev ' + interface_name + \
            " scope global | grep 'inet6' | awk {'print $2'} | xargs -I {} ip -6 addr del {} dev " + interface_name
        subprocess.call(command, shell=True, timeout=5)
        time.sleep(1)
        flag = True
    finally:
        time.sleep(1)
        assert flag


def fixture_Init_interface() -> bool:
    print('Init interface')
    init_interface_ipv6_address()
    reset_host_interface()
    time.sleep(30)
    set_interface_sysctl_options()
    return True


@pytest.mark.esp32h2
@pytest.mark.esp32s3
@pytest.mark.esp_matter_dut
@pytest.mark.parametrize(
    'count, app_path, target, erase_all', [
        ( 2, pytest_matter_thread_dir, 'esp32h2|esp32s3', 'y|y'),
    ],
    indirect=True,
)

# Matter over thread commissioning
def test_matter_commissioning_h2(dut:Tuple[Dut, Dut]) -> None:
    ot_br = dut[1]
    light = dut[0]
    # For matter over thread commissioning need to reset host interface
    fixture_Init_interface()
    # BLE start advertising
    light.expect(r'chip\[DL\]\: Configuring CHIPoBLE advertising', timeout=20)
    ot_br.expect(r'chip\[DL\]\: Configuring CHIPoBLE advertising', timeout=20)
    # Start commissioning OTBR
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' pairing ble-wifi 1 ChipTEH2 chiptest123 20202021 3584'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Set the active dataset and start Thread network
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' generalcommissioning arm-fail-safe 180 1 1 0'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' threadborderroutermanagement set-active-dataset-request hex:' + OT_DATASET_HEXSTR + ' 1 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' generalcommissioning commissioning-complete 1 0'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Start commissioning Light
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' pairing ble-thread 2 hex:' + OT_DATASET_HEXSTR +' 20202021 3840'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-off the light
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' onoff toggle 2 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
    # Use toggle command to turn-on the light
    time.sleep(2)
    command = CHIP_TOOL_EXE + ' onoff toggle 2 1'
    out_str = subprocess.getoutput(command)
    print(out_str)
    result = re.findall(r'Run command failure', str(out_str))
    if len(result) != 0:
      assert False
