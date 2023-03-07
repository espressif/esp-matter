# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#
# The file provides an example of using Cirque Flask Service to:
# 1. create cirque virtual home
# 2. create several devices in virtual home
# 3. form a Thread network
# 4. how devices scan for virtual wifi ap
# 5. how devices request ip address from virtual wifi ap
# 6. test network connectivities between devices
#
# prerequisite:
# 1. python3 -m venv venv-test
# 2. source venv-test/bin/activate
#      pip install --upgrade pip
#      pip install --upgrade setuptools
#      pip sintall wheel
# 3. $ cd ~/cirque-core
#    $ pip install .
# 4. deactivate
#
# Run test:
# a. in a terminal, bring up cirque grpc service:
#    $ cd ~/cirque-core
#    $ $ bazel build //cirque/grpc:service
#    && sudo ./bazel-bin/cirque/grpc/service
# b. in another terminal, run test
#    $ source venv-test/bin/activate
#    $ cd ~/cirque-core
#    $ python example/test_grpc_virtual_home.py
#

import logging
import unittest
import os
import re
import time

import grpc
from google.rpc import code_pb2, status_pb2

import cirque.proto.capability_pb2 as capability_pb2
import cirque.proto.device_pb2 as device_pb2
import cirque.proto.service_pb2 as service_pb2
import cirque.proto.service_pb2_grpc as service_pb2_grpc

from cirque.proto.device_pb2 import DeviceSpecification

from cirque.common.cirquelog import CirqueLog
from cirque.common.utils import sleep_time

from cirque.proto.capability_pb2 import (WeaveCapability, ThreadCapability,
                                         WiFiCapability, XvncCapability,
                                         InteractiveCapability,
                                         LanAccessCapability)

DEVICE_CONFIG = {
    'device0': {
        'device_type': 'Generic Node',
        'base_image': 'generic_node_image',
        'wifi_capability': WiFiCapability(),
        'thread_capability': ThreadCapability(rcp_mode=True),
        'interactive_capability': InteractiveCapability()
    },
    'device1': {
        'device_type': 'GenericNode',
        'base_image': 'generic_node_image',
        'wifi_capability': WiFiCapability(),
        'thread_capability': ThreadCapability(rcp_mode=True),
        'interactive_capability': InteractiveCapability()
    },
    'wifi-ap': {
        'device_type': 'wifi_ap',
        'base_image': 'mac80211_ap_image',
    }
}


class TestGrpcVirtualHome(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    CirqueLog.setup_cirque_logger(level=logging.INFO)
    cls.logger = CirqueLog.get_cirque_logger('GrpcVirtualHome')

    cls.channel = grpc.insecure_channel('localhost:50051')
    cls.stub = service_pb2_grpc.CirqueServiceStub(cls.channel)

  @classmethod
  def tearDownClass(cls):

    cls.logger.info('tearing down test class')

    home_ids = cls.stub.ListCirqueHomes(
        service_pb2.ListCirqueHomesRequest()).home_id

    cls.logger.info('created home ids: {}'.format(home_ids))

    for home_id in home_ids:
      cls.logger.info('stopping home: {}'.format(home_id))
      cls.stub.StopCirqueHome(
          service_pb2.StopCirqueHomeRequest(home_id=home_id))

    cls.channel.close()

  def setUp(self):
    pass

  def test_001_create_home(self):
    home_id = self.stub.CreateCirqueHome(
        service_pb2.CreateCirqueHomeRequest()).home_id

    self.logger.info('\nhome id: {} created!'.format(home_id))

    self.assertTrue(
        home_id == self.stub.ListCirqueHomes(
            service_pb2.ListCirqueHomesRequest()).home_id[0],
        'created home_id could not find in the cirque service!!')

  def test_002_create_devices(self):

    home_id = self.stub.ListCirqueHomes(
        service_pb2.ListCirqueHomesRequest()).home_id[0]

    device_ids = set()
    device_config = create_device_from_config()

    for _ in DEVICE_CONFIG:
      device_id = self.stub.CreateCirqueDevice(
          service_pb2.CreateCirqueDeviceRequest(
              home_id=home_id,
              specification=device_pb2.DeviceSpecification(
                  **next(device_config)))).device.device_id

      device_ids.add(device_id)

    self.logger.info('\ncreated device ids:')
    list(map(print, device_ids))

    devices = self.stub.ListCirqueHomeDevices(
        service_pb2.ListCirqueHomeDevicesRequest(home_id=home_id))

    device_ids_from_request = set(
        [device.device_id for device in devices.devices])
    self.assertTrue(device_ids == device_ids_from_request,
                    'device created did not match from devices query command!')

  def test_003_connect_to_thread_network(self):
    home_id = self.stub.ListCirqueHomes(
        service_pb2.ListCirqueHomesRequest()).home_id[0]
    devices = self.stub.ListCirqueHomeDevices(
        service_pb2.ListCirqueHomeDevicesRequest(home_id=home_id))
    devices = [
        d for d in devices.devices
        if d.device_specification.base_image == 'generic_node_image'
    ]

    self.logger.info('Running commands to form Thread network')
    for device in devices:
      self.stub.ExecuteDeviceCommand(
          service_pb2.ExecuteDeviceCommandRequest(
              home_id=home_id,
              device_id=device.device_id,
              command="bash -c 'ot-ctl panid 0x1234 && \
                             ot-ctl ifconfig up && \
                             ot-ctl thread start'"))
    self.logger.info('Waiting for Thread network to be formed...')
    time.sleep(10)
    roles = set()
    for device in devices:
      reply = self.stub.ExecuteDeviceCommand(
          service_pb2.ExecuteDeviceCommandRequest(
              home_id=home_id,
              device_id=device.device_id,
              command='ot-ctl state'))
      roles.add(reply.output.split()[0])
    self.assertIn('leader', roles)
    self.assertTrue('router' in roles or 'child' in roles)

  def test_004_scan_available_wifi_network(self):

    home_id = self.stub.ListCirqueHomes(
        service_pb2.ListCirqueHomesRequest()).home_id[0]

    devices = self.stub.ListCirqueHomeDevices(
        service_pb2.ListCirqueHomeDevicesRequest(home_id=home_id))

    device_ids = [
        device.device_id
        for device in devices.devices
        if device.device_specification.device_type != 'wifi_ap'
    ]

    ssid = [
        device.device_description.ssid
        for device in devices.devices
        if device.device_specification.device_type == 'wifi_ap'
    ][0]

    self.logger.info('\nwifi ap ssid: {}'.format(ssid))

    network_scan_command = 'iwlist wlan0 scanning'

    for device_id in device_ids:
      ret = self.stub.ExecuteDeviceCommand(
          service_pb2.ExecuteDeviceCommandRequest(
              home_id=home_id,
              device_id=device_id,
              command=network_scan_command,
              streaming=False))

      ssid_from_dev = re.search(r'\s+ESSID:"(.+)"', ret.output).group(1)
      self.logger.info('\nssid: {} scanned by\ndevice: {}'.format(
          ssid_from_dev, device_id))
      self.assertTrue(ssid == ssid_from_dev,
                      'ssid from device did not match what is from wifi_ap!!')

  def test_005_connect_to_desired_wifi_network(self):

    home_id = self.stub.ListCirqueHomes(
        service_pb2.ListCirqueHomesRequest()).home_id[0]

    devices = self.stub.ListCirqueHomeDevices(
        service_pb2.ListCirqueHomeDevicesRequest(home_id=home_id))

    device_ids = [
        device.device_id
        for device in devices.devices
        if device.device_specification.device_type != 'wifi_ap'
    ]

    ssid, psk = [(device.device_description.ssid, device.device_description.psk)
                 for device in devices.devices
                 if device.device_specification.device_type == 'wifi_ap'][0]

    self.logger.info('\nwifi ap ssid: {}, psk: {}'.format(ssid, psk))

    for device_id in device_ids:
      self.logger.info('\ndevice: {}\n connecting to desired ssid: {}'.format(
          device_id, ssid))
      write_psk_to_wpa_supplicant_config(self.logger, self.stub, home_id,
                                         device_id, ssid, psk)

      kill_existing_wpa_supplicant(self.logger, self.stub, home_id, device_id)

      start_wpa_supplicant(self.logger, self.stub, home_id, device_id)
    time.sleep(5)

  def test_006_device_connectivity(self):

    home_id = self.stub.ListCirqueHomes(
        service_pb2.ListCirqueHomesRequest()).home_id[0]

    devices = self.stub.ListCirqueHomeDevices(
        service_pb2.ListCirqueHomeDevicesRequest(home_id=home_id))

    device_ids = [
        device.device_id
        for device in devices.devices
        if device.device_specification.device_type != 'wifi_ap'
    ]

    device_addrs = list()

    request_addr_command = 'dhcpcd wlan0'

    for device_id in device_ids:
      self.logger.info(
          '\nrequesting ip address from wifi ap by\ndevice:{}'.format(
              device_id))

      ret = self.stub.ExecuteDeviceCommand(
          service_pb2.ExecuteDeviceCommandRequest(
              home_id=home_id,
              device_id=device_id,
              command=request_addr_command,
              streaming=False))

      ipaddr = re.search(
          r'wlan0: leased (\d+\.\d+\.\d+\.\d+) for (\d+) seconds',
          ret.output).group(1)

      self.logger.info('\nip address requested: {}'.format(ipaddr))

      device_addrs.append((device_id, ipaddr))

    self.logger.info('\npinging from device: {} to\ndevice: {}'.format(
        device_addrs[0][0], device_addrs[1][0]))
    ping_command = 'ping -c 3 {}'

    ret = self.stub.ExecuteDeviceCommand(
        service_pb2.ExecuteDeviceCommandRequest(
            home_id=home_id,
            device_id=device_addrs[0][0],
            command=ping_command.format(device_addrs[1][1]),
            streaming=False))

    loss = re.search(r'(\d+)% packet loss', ret.output).group(1)

    self.assertNotEqual(
        loss, '100', 'unable to ping device: {}, no wifi connectivity!!'.format(
            device_addrs[1][1]))
    self.logger.info('ping loss rate: {}%'.format(loss))


def create_device_from_config():
  for device in DEVICE_CONFIG:
    yield DEVICE_CONFIG[device]


def write_psk_to_wpa_supplicant_config(logger, stub, home_id, device_id, ssid,
                                       psk):

  logger.info(
      '\ndevice id: {}\nwriting ssid, psk to wpa_supplicant config'.format(
          device_id))

  write_psk_command = ''.join([
      "sh -c 'wpa_passphrase {} {} >> ".format(ssid, psk),
      "/etc/wpa_supplicant/wpa_supplicant.conf'"
  ])

  return stub.ExecuteDeviceCommand(
      service_pb2.ExecuteDeviceCommandRequest(
          home_id=home_id,
          device_id=device_id,
          command=write_psk_command,
          streaming=False))


def kill_existing_wpa_supplicant(logger, stub, home_id, device_id):

  logger.info('\ndevice id: {}\nkill existing wpa_supplicant'.format(device_id))

  kill_wpa_supplicant_command = 'killall wpa_supplicant'

  return stub.ExecuteDeviceCommand(
      service_pb2.ExecuteDeviceCommandRequest(
          home_id=home_id,
          device_id=device_id,
          command=kill_wpa_supplicant_command,
          streaming=False))


def start_wpa_supplicant(logger, stub, home_id, device_id):

  logger.info(
      '\ndevice id: {}\nstarting wpa_supplicant on device'.format(device_id))

  start_wpa_supplicant_command = ''.join([
      'wpa_supplicant -B -i wlan0 ',
      '-c /etc/wpa_supplicant/wpa_supplicant.conf ',
      '-f /var/log/wpa_supplicant.log -t -dd'
  ])

  return stub.ExecuteDeviceCommand(
      service_pb2.ExecuteDeviceCommandRequest(
          home_id=home_id,
          device_id=device_id,
          command=start_wpa_supplicant_command,
          streaming=False))


if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestGrpcVirtualHome)
  unittest.TextTestRunner(verbosity=2).run(suite)
