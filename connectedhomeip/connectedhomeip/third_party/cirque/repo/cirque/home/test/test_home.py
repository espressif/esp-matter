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

import os
import logging
import unittest

from cirque.common.cirquelog import CirqueLog
from cirque.home.home import CirqueHome
from cirque.common.utils import sleep_time, host_run


class TestHome(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    CirqueLog.setup_cirque_logger(level=logging.DEBUG)
    cls.home = CirqueHome()
    cls.weave_crt_path = os.environ['WEAVE_CRT_PATH'] \
        if 'WEAVE_CRT_PATH' in os.environ else None

  @classmethod
  def tearDownClass(cls):
    del cls.home

  def test_001_create_home(self):
    home_config = {
        'device0': {
            'type': 'generic_node_image',
            'capability': ['Thread', 'WiFi'],
            'rcp_mode': True,
        },
        'device1': {
            'type': 'wifi_ap',
            'base_image': 'mac80211_ap_image'
        }
    }
    if self.weave_crt_path is not None:
      home_config['device0']['capability'].append('Weave')
      home_config['device0']['weave_config_file'] = self.weave_crt_path
    home_id = self.home.create_home(home_config)
    self.assertEqual(home_id, self.home.home_id)
    description = self.home.get_home_devices()
    generic_node_id = next(node_id for node_id, desc in description.items()
                           if desc['type'] == 'generic_node_image')
    wifi_ap_id = next(node_id for node_id, desc in description.items()
                      if desc['type'] == 'wifi_ap')
    self.assertIsNotNone(generic_node_id)
    self.assertIsNotNone(wifi_ap_id)
    sleep_time(self.home.logger, 20, 'waiting for processes to be launched')
    self.__verify_process_in_container(generic_node_id, 'otbr-agent')
    self.__verify_process_in_container(generic_node_id, 'wpa_supplicant')
    if self.weave_crt_path is not None:
      self.__verify_process_in_container(generic_node_id, 'nldaemon')

  def test_002_add_device(self):
    device_config = {
        'type': 'generic_node_image',
        'capability': ['Thread', 'WiFi'],
        'rcp_mode': True,
    }
    if self.weave_crt_path is not None:
      device_config['capability'].append('Weave')
      device_config['weave_config_file'] = self.weave_crt_path
    node_id = self.home.add_device(device_config)
    self.assertIsNotNone(node_id)
    self.home.logger.info('{}'.format(device_config))
    sleep_time(self.home.logger, 20, 'waiting for processes to be launched')
    self.__verify_process_in_container(node_id, 'otbr-agent')
    self.__verify_process_in_container(node_id, 'wpa_supplicant')

  def test_003_destroy_home(self):
    self.home.destroy_home()
    self.assertIsNone(self.home.home)

  def test_004_docker_network_access(self):
    home = CirqueHome()
    logger = home.logger
    ping_cmd = 'ping -w 2 -c 2 -i 0.5'
    home.create_home({
        'device0': {
            'type': 'generic_node_image',
        },
        'device1': {
            'type': 'generic_node_image',
        },
    })
    devices = home.get_home_devices()
    device_ids = list(devices.keys())
    device0_id = device_ids[0]
    device1_id = device_ids[1]
    device0_ip = \
        home.home['devices'][device0_id].description['ipv4_addr']
    device1_ip = \
        home.home['devices'][device1_id].description['ipv4_addr']
    self.assertEqual(
        host_run(logger, '{} {}'.format(ping_cmd, device0_ip)).returncode, 0)
    self.__verify_ping_in_container(
        device0_id, device1_ip, ping_cmd=ping_cmd, should_succeed=False)
    self.__verify_ping_in_container(
        device0_id, '8.8.8.8', ping_cmd=ping_cmd, should_succeed=True)
    internal_node_id = home.add_device({
        'type': 'generic_node_image',
        'docker_network': 'internal',
    })
    internal_node_ip = \
        home.home['devices'][internal_node_id].description['ipv4_addr']
    self.assertEqual(
        host_run(logger, '{} {}'.format(ping_cmd, internal_node_ip)).returncode,
        0)
    self.__verify_ping_in_container(
        internal_node_id, '8.8.8.8', ping_cmd=ping_cmd, should_succeed=False)
    home.destroy_home()

  def __verify_ping_in_container(self,
                                 node_id,
                                 address,
                                 ping_cmd='ping -c 1',
                                 should_succeed=True):
    docker_client = self.home.docker_client
    container = docker_client.containers.get(node_id)
    ret = container.exec_run(' '.join([ping_cmd, address]))
    if should_succeed:
      self.assertEqual(ret.exit_code, 0)
    else:
      self.assertNotEqual(ret.exit_code, 0)

  def __verify_process_in_container(self, node_id, process_name):
    docker_client = self.home.docker_client
    container = docker_client.containers.get(node_id)
    self.assertIsNotNone(container)
    processes = container.top()['Processes']
    target_process = next(
        p for p in processes if p[-1].find(process_name) != -1)
    self.assertIsNotNone(target_process)


if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestHome)
  unittest.TextTestRunner(verbosity=2).run(suite)
