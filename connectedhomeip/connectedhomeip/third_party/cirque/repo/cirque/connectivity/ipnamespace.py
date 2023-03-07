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

import pyroute2
from pyroute2 import IPRoute
import subprocess


class IpNameSpace:

  def __init__(self, bridge):
    self.ns_id = bridge.get_new_ns_id()
    self.namespace_name = 'ns{}{}'.format(bridge.bridge_id, self.ns_id)
    self.bridge = bridge
    self.netns = None
    self.host_interface_id = None
    self.host_interface_name = 'veth{}{}'.format(bridge.bridge_id, self.ns_id)
    self.ns_interface_name = 'vetp{}{}'.format(bridge.bridge_id, self.ns_id)
    self.ipv4_addr, self.ipv6_addr = map(str, bridge.get_new_device_addr())

  def __setup_veth_pair(self):
    with IPRoute() as ipr:
      ipr.link(
          'add',
          ifname=self.host_interface_name,
          peer=self.ns_interface_name,
          kind='veth')
      self.host_interface_id = ipr.link_lookup(
          ifname=self.host_interface_name)[0]
      ns_interface_id = ipr.link_lookup(ifname=self.ns_interface_name)[0]
      ipr.link('set', index=ns_interface_id, net_ns_fd=self.namespace_name)
      ipr.link(
          'set',
          index=self.host_interface_id,
          master=self.bridge.bridge_link_id)
      ipr.link('set', index=self.host_interface_id, state='up')

  def __setup_ip_namespace_route(self):
    ns_interface_id = self.netns.link_lookup(ifname=self.ns_interface_name)[0]
    self.netns.link('set', index=ns_interface_id, state='up')
    self.netns.addr(
        'add',
        index=ns_interface_id,
        address=self.ipv4_addr,
        prefixlen=self.bridge.ipv4_prefix_len)
    self.netns.addr(
        'add',
        index=ns_interface_id,
        address=self.ipv6_addr,
        prefixlen=self.bridge.ipv6_prefix_len)
    self.netns.route('add', dst='0.0.0.0/0', gateway=str(self.bridge.ipv4_addr))
    self.netns.route('add', dst='::/0', gateway=str(self.bridge.ipv6_addr))

  def open(self):
    self.netns = pyroute2.NetNS(self.namespace_name)
    self.__setup_veth_pair()
    self.__setup_ip_namespace_route()

  def close(self):
    if self.host_interface_id is not None:
      with IPRoute() as ipr:
        ipr.link('delete', ifname=self.host_interface_name)
      self.host_interface_id = None
    if self.netns is not None:
      self.netns.close()
      self.netns.remove()
      self.netns = None

  def __del__(self):
    self.close()
