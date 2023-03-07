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

from threading import Lock
import subprocess
import ipaddress

from pyroute2 import IPRoute


class EthBridge:

  @staticmethod
  def manage_nat_rule(network, action):
    iptable_cmd = 'iptables' if network.version == 4 else 'ip6tables'
    assert action in {'add', 'del', 'delete'}
    action_arg = '-A' if action == 'add' else '-D'
    subprocess.call([
        iptable_cmd, '-t', 'nat', action_arg, 'POSTROUTING', '-s',
        str(network), '-j', 'MASQUERADE'
    ])

  def __init__(self, bridge_name, bridge_id, ipv4_network, ipv6_network):
    self.ipv4_network = ipv4_network
    self.__ipv4_addr_gen = iter(self.ipv4_network.hosts())
    self.ipv6_network = ipv6_network
    self.__ipv6_addr_gen = iter(self.ipv6_network.hosts())
    self.__addr_mtx = Lock()
    self.ipv4_addr, self.ipv6_addr = self.get_new_device_addr()
    self.bridge_name = bridge_name
    self.bridge_id = bridge_id
    self.ns_id = 0

  def open(self):
    with IPRoute() as ipr:
      ipr.link('add', ifname=self.bridge_name, kind='bridge')
      self.bridge_link_id = ipr.link_lookup(ifname=self.bridge_name)[0]
      print(self.bridge_link_id)
      ipr.addr(
          'add',
          index=self.bridge_link_id,
          address=str(self.ipv4_addr),
          prefixlen=self.ipv4_network.prefixlen)
      ipr.addr(
          'add',
          index=self.bridge_link_id,
          address=str(self.ipv6_addr),
          prefixlen=self.ipv6_network.prefixlen)
      EthBridge.manage_nat_rule(self.ipv4_network, action='add')
      EthBridge.manage_nat_rule(self.ipv6_network, action='add')
      ipr.link('set', index=self.bridge_link_id, state='up')

  def get_new_device_addr(self):
    with self.__addr_mtx:
      return next(self.__ipv4_addr_gen), next(self.__ipv6_addr_gen)

  @property
  def ipv4_prefix_len(self):
    return self.ipv4_network.prefixlen

  @property
  def ipv6_prefix_len(self):
    return self.ipv6_network.prefixlen

  def get_new_ns_id(self):
    self.ns_id += 1
    return self.ns_id

  def close(self):
    if self.bridge_name is not None:
      with IPRoute() as ipr:
        ipr.link('delete', ifname=self.bridge_name)
      self.bridge_name = None
      self.bridge_id = None
      self.bridge_link_id = None
    if self.ipv4_network is not None:
      EthBridge.manage_nat_rule(self.ipv4_network, action='del')
    self.ipv4_network = None
    if self.ipv6_network is not None:
      EthBridge.manage_nat_rule(self.ipv6_network, action='del')
    self.ipv6_network = None

  def __del__(self):
    self.close()
