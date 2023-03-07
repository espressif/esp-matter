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

import atexit
import docker
import os
import time
import uuid

import cirque.nodes as nodes

from cirque.common.cirquelog import CirqueLog
from cirque.connectivity.homelan import HomeLan
from cirque.connectivity.threadsimpipe import ThreadSimPipe
from cirque.nodes.wifiapnode import WiFiAPNode
from cirque.nodes.dockernode import DockerNode
from cirque.capabilities.interactivecapability import InteractiveCapability
from cirque.capabilities.dockernetworkcapability import DockerNetworkCapability
from cirque.capabilities.lanaccesscapability import LanAccessCapability
from cirque.capabilities.mountcapability import MountCapability
from cirque.capabilities.threadcapability import ThreadCapability
from cirque.capabilities.weavecapability import WeaveCapability
from cirque.capabilities.wificapability import WiFiCapability
from cirque.capabilities.xvnccapability import XvncCapability
from cirque.capabilities.trafficcontrolcapability \
    import TrafficControlCapability


class CirqueHome:

  def __init__(self, home_id=None):
    self.docker_client = docker.from_env()
    if home_id is None:
      self.home_id = str(uuid.uuid4())
    else:
      self.home_id = home_id
    self.home = {'home_id': self.home_id, 'devices': {}}
    self.thread_petitions = {}
    # network
    self.external_lan = None
    self.internal_lan = None
    self.ipv6_lan = None
    self.ipvlan_lan = None
    atexit.register(self.destroy_home)
    self.logger = CirqueLog.get_cirque_logger('home')

  def __next_thread_node_id(self, petition):
    if petition in self.thread_petitions:
      petition_id = self.thread_petitions[petition]['petition_id']
    else:
      petition_id = ThreadSimPipe.get_next_petition()
      self.thread_petitions[petition] = {
          'petition_id': petition_id,
          'ncp_id': 0
      }
    self.thread_petitions[petition]['ncp_id'] += 1
    return self.thread_petitions[petition]['ncp_id']

  def create_home(self, home_config):
    self.logger.info('creating home: {}'.format(self.home_id))
    for device_config in home_config.values():
      self.add_device(device_config)
    return self.home_id

  def add_device(self, device_config):
    self.logger.info('Adding device to home {}: {}'.format(
        self.home_id, device_config))
    capabilities = []
    device_type = device_config['type']
    if 'base_image' in device_config:
      base_image = device_config['base_image']
    else:
      base_image = device_config['type']

    # configure docker network
    # bluetooth feature uses host network, can not create customize
    # networks.
    if 'Bluetooth' in device_config['capability']:
      pass
    elif 'docker_network' in device_config and \
            device_config['docker_network'] == 'Internal':
      if not self.internal_lan:
          self.internal_lan = HomeLan(
            '{}_internal'.format(self.home_id), internal=True)
      capabilities.append(self.__make_internal_network_capability())
    elif 'docker_network' in device_config and \
            device_config['docker_network'] == 'Ipv6':
      if not self.ipv6_lan:
        self.ipv6_lan = HomeLan('{}_ipv6'.format(self.home_id), ipv6=True)
      capabilities.append(self.__make_ipv6_network_capability())
    elif 'docker_network' in device_config and \
            device_config['docker_network'] == 'IpvLan':
      if not self.ipvlan_lan:
        self.ipvlan_lan = HomeLan('{}_ipvlan'.format(self.home_id))
      capabilities.append(self.__make_ipvlan_network_capability())
    else:
      if not self.external_lan:
        self.external_lan = HomeLan('{}_external'.format(self.home_id))
      capabilities.append(self.__make_external_network_capability())

    if 'type' not in device_config:
      self.logger.critical('Cannot create device, type unspecified')
      return
    if 'capability' in device_config:
      for capability_name in device_config['capability']:
        capability = self.__make_capability(capability_name, device_config)
        if capability is not None:
          capabilities.append(capability)
    if device_type == 'wifi_ap':
      device_node = WiFiAPNode(self.docker_client, base_image=base_image)
    else:
      device_node = DockerNode(
          self.docker_client, device_type, capabilities, base_image=base_image)
    device_node.run()
    if device_node.id is not None:
      self.home['devices'][device_node.id] = device_node
    return device_node.id

  def __make_capability(self, capability, device_config):
    factory_functions = {
        'Interactive': self.__make_interactive_capability,
        'LanAccess': self.__make_lan_access_capability,
        'Thread': self.__make_thread_capability,
        'Weave': self.__make_weave_capability,
        'WiFi': self.__make_wifi_capability,
        'Xvnc': self.__make_xvnc_capability,
        'Mount': self.__make_mount_capability,
        'TrafficControl': self.__make_trafficcontrolcapability,
    }
    if capability not in factory_functions:
      self.logger.critical('Unsupported capability {}'.format(capability))
      return None
    else:
      return factory_functions[capability](capability, device_config)

  # 4 network settings
  def __make_external_network_capability(self):
    return DockerNetworkCapability(self.external_lan.name, 'external')

  def __make_internal_network_capability(self):
    return DockerNetworkCapability(self.internal_lan.name, 'internal')

  def __make_ipv6_network_capability(self):
    return DockerNetworkCapability(self.ipv6_lan.name, 'ipv6')

  def __make_ipvlan_network_capability(self):
    return DockerNetworkCapability(self.ipvlan_lan.name, 'ipvlan')

  def __make_bluetooth_capability(self, capability, device_config):
      num_infs = device_config.get('num_infs', 2)
      return BlueToothCapability(num_btvirts=num_infs)

  def __make_trafficcontrolcapability(self, capability, device_config):
    return TrafficControlCapability(
        latencyMs=device_config.get('traffic_control').get('latencyMs', 0),
        loss=device_config.get('traffic_control').get('loss', 0))

  def __make_interactive_capability(self, capability, device_config):
    return InteractiveCapability()

  def __make_lan_access_capability(self, capability, device_config):
    if 'docker_network' in device_config and \
            device_config['docker_network'] == 'internal':
      home_lan = self.internal_lan
    else:
      home_lan = self.external_lan
    return LanAccessCapability(home_lan)

  def __make_thread_capability(self, capability, device_config):
    petition = device_config['thread_petition'] \
        if 'thread_petition' in device_config else 0
    daemons = device_config['thread_daemon'] \
        if 'thread_daemon' in device_config else ['wpantund']
    rcp = 'rcp_mode' in device_config and device_config['rcp_mode']
    node_id = self.__next_thread_node_id(petition)
    return ThreadCapability(node_id, petition, daemons=daemons, rcp=rcp)

  def __make_weave_capability(self, capability, device_config):
    if 'weave_config_file' not in device_config:
      self.logger.critical('Weave configuration file not found, \
                cannot initialize Weave capability')
      return None
    weave_provision_path = os.path.expanduser(
        device_config['weave_config_file'])
    target_path = device_config['weave_config_target_path'] \
        if 'weave_config_target_path' in device_config else None
    return WeaveCapability(weave_provision_path, target_path)

  def __make_wifi_capability(self, capability, device_config):
    return WiFiCapability()

  def __make_internal_network_capability(self):
    return DockerNetworkCapability(self.internal_lan.name, 'internal')

  def __make_external_network_capability(self):
    return DockerNetworkCapability(self.external_lan.name, 'external')

  def __make_ipv6_network_capability(self):
    return DockerNetworkCapability(self.ipv6_lan.name, 'ipv6')

  def __make_ipvlan_network_capability(self):
    return DockerNetworkCapability(self.ipvlan_lan.name, 'ipvlan')

  def __make_xvnc_capability(self, capability, device_config):
    localhost = device_config['xvnc_localhost'] \
        if 'xvnc_localhost' in device_config else True
    display_id = device_config['display_id'] \
        if 'display_id' in device_config else 0
    docker_display_id = device_config['docker_display_id'] \
        if 'docker_display_id' in device_config else 0
    return XvncCapability(localhost, display_id, docker_display_id)

  def __make_mount_capability(self, capability, device_config):
    mount_pairs = device_config['mount_pairs']
    return MountCapability(mount_pairs)

  def __make_trafficcontrolcapability(self, capability, device_config):
    return TrafficControlCapability(
        latencyMs=device_config.get('traffic_control').get('latencyMs', 0),
        loss=device_config.get('traffic_control').get('loss', 0))

  def get_wifiap_ssid_psk(self, node_id=None):

    def ssid_psk(node):
      ssid = node.get_wifi_ssid()
      psk = node.get_wifi_password()
      return (ssid, psk)

    if node_id and node_id in self.home['devices']:
      node = self.home['devices'][node_id]
      return ssid_psk(node)
    for node in self.home['devices'].values():
      if node.type == 'wifi_ap':
        return ssid_psk(node)
    return ''

  def get_home_devices(self):
    return {
        node.id: self.get_device_state(node.id)
        for node in self.home['devices'].values()
    }

  def get_device_state(self, node_id):
    if node_id in self.home['devices']:
      node = self.home['devices'][node_id]
      return {
          'name': node.name,
          'type': node.type,
          'base_image': node.base_image,
          'id': node.id,
          'capability': {
              cap.name: cap.description for cap in node.capabilities
          },
          'description': node.description,
      }
    else:
      return None

  def stop_device(self, node_id):
    if node_id not in self.home['devices']:
      return ''
    self.home['devices'][node_id].stop()
    del self.home['devices'][node_id]
    return node_id

  def get_device_log(self, node_id, tail='all'):
    if node_id not in self.home['devices']:
      return ''
    return self.home['devices'][node_id].get_device_log(tail)

  def destroy_home(self):
    if not self.home:
      return
    for node in self.home['devices'].values():
      node.stop()
    for lan in ('external_lan', 'internal_lan', 'ipv6_lan', 'ipvlan_lan'):
      if getattr(self, lan):
        getattr(self, lan).close()
    self.docker_client.containers.prune()
    self.docker_client.networks.prune()
    self.home = None
    return self.home_id

  @property
  def devices(self):
    return self.home['devices']

  def execute_device_cmd(self, cmd, node_id=None, stream=False):
    ret = None
    if node_id is not None:
      device_node = self.home['devices'][node_id]
      ret = device_node.container.exec_run(cmd, stream=stream)
      self.logger.info('cmd is: {} and ret: {}'.format(cmd, ret))
    return ret
