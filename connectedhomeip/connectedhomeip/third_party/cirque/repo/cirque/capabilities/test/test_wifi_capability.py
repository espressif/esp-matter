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

import logging
import unittest
import re
import docker

import cirque.common.utils as utils

from cirque.common.cirquelog import CirqueLog
from cirque.capabilities.wificapability import WiFiCapability
from cirque.nodes.dockernode import DockerNode
from cirque.nodes.wifiapnode import WiFiAPNode


class TestWiFiFeature(unittest.TestCase):

  @classmethod
  def setUpClass(cls):
    CirqueLog.setup_cirque_logger(level=logging.DEBUG)
    docker_client = docker.from_env()

    # bring up WiFi AP Node
    cls.ap = WiFiAPNode(docker_client)
    cls.ap.run()
    cls.wifi_capability = WiFiCapability()

    cls.generic_node = DockerNode(
        docker_client, 'generic_node_image', capabilities=[cls.wifi_capability])
    kwargs = {'stdin_open': True, 'privileged': True}
    cls.generic_node.run(**kwargs)

    cls.ap_ssid = cls.ap.get_wifi_ssid()
    cls.ap_psk = cls.ap.get_wifi_password()

  @classmethod
  def tearDownClass(cls):
    print('tearing down tests...')
    cls.generic_node.stop()
    cls.ap.stop()

  def test_001_scan_available_wifi_network(self):
    ret = self.generic_node.container.exec_run('iwlist wlan0 scanning')
    self.assertEqual(ret.exit_code, 0)
    ssid = re.findall(rb'\s+ESSID:"([^"]+)"', ret.output)[0].decode('utf-8')
    self.assertEqual(ssid, self.ap_ssid)

  def test_002_connect_to_desired_wifi_network(self):
    print('')
    self.__connect_to_desired_wifi(self.generic_node, self.ap_ssid, self.ap_psk)

    self.assertTrue(
        hasattr(self.generic_node, 'wifi_ipv4') and self.generic_node.wifi_ipv4)

  def test_003_check_wifi_connectivity(self):
    print('')
    self.__check_wifi_connectivity(self.generic_node)

  def __request_ip_addr_from_dhcp_server(self, docker_node):
    ret = docker_node.container.exec_run('dhcpcd wlan0')
    ipaddr = re.search(rb'wlan0: leased (\d+\.\d+\.\d+\.\d+) for (\d+) seconds',
                       ret.output)
    self.assertIsNotNone(ipaddr)
    docker_node.wifi_ipv4 = ipaddr.group(1).decode('utf-8')
    default_route = re.search(
        rb'wlan0: adding default route via (\d+\.\d+\.\d+\.\d+)', ret.output)
    self.assertIsNotNone(default_route)
    docker_node.logger.info('Successfully requested ip: {} from: {}'.format(
        docker_node.wifi_ipv4,
        default_route.group(1).decode('utf-8')))

  def __connect_to_desired_wifi(self, docker_node, ssid, psk):
    docker_node.logger.info('connecting to desired ssid: {}'.format(ssid))
    docker_node.logger.info('flushing wlan0 ip address...')
    ret = docker_node.container.exec_run("sh -c 'wpa_passphrase {} {} >> \
                /etc/wpa_supplicant/wpa_supplicant.conf'".format(ssid, psk))
    ret = docker_node.container.exec_run('killall wpa_supplicant')
    ret = self.wifi_capability.start_wpa_supplicant_service(docker_node)
    self.assertEqual(ret.exit_code, 0)
    utils.sleep_time(docker_node.logger, 2, 'restart wpa_supplicant')
    self.__request_ip_addr_from_dhcp_server(docker_node)

  def __check_wifi_connectivity(self, docker_node):
    dhcpserver = docker_node.wifi_ipv4[:docker_node.wifi_ipv4.rfind('.')] + '.1'
    docker_node.logger.info(
        'checking connectivity with dhcp server: {}'.format(dhcpserver))
    ret = docker_node.container.exec_run('ping -c 3 {}'.format(dhcpserver))
    loss_num = re.search(rb'(\d+)% packet loss', ret.output)
    docker_node.logger.info(ret.output.rstrip().split(b'\n')[1])
    if loss_num.group(1) == b'100':
      self.fail('unable to ping dhcp server, no wifi connectivity!!')
    docker_node.logger.info(
        '{}% packet loss, connectivity to dhcp server is good!!'.format(
            loss_num.group(1).decode('utf-8')))


if __name__ == '__main__':
  suite = unittest.TestLoader().loadTestsFromTestCase(TestWiFiFeature)
  unittest.TextTestRunner(verbosity=2).run(suite)
