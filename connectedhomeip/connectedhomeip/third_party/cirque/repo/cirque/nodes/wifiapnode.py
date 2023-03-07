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
import time
import random

from subprocess import PIPE

import cirque.common.utils as utils
from cirque.capabilities.wificapability import WiFiCapability
from cirque.common.exceptions import (
    ContainerExecError,
    IpNetnsExecError,
)
from cirque.nodes.dockernode import DockerNode

RUNTIME_NAMESPACE = "/var/run/netns"
CHAR_SRC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"


class WiFiAPNode(DockerNode):

  def __init__(self,
               docker_client,
               ssid=None,
               password=None,
               container_name=None,
               base_image="mac80211_ap_image"):
    super().__init__(docker_client, node_type="wifi_ap", base_image=base_image)
    random.seed(time.time())
    self.ssid = ssid
    self.password = password
    self.container_name = container_name
    self.wifi_capability = WiFiCapability()
    self.capabilities.append(self.wifi_capability)
    if not self.ssid:
      self.ssid = self.get_wifi_ssid()
    if not self.password:
      self.password = self.get_wifi_password()

  def __del__(self):
    if hasattr(self, "container") and self.container:
      self.stop()

  @property
  def type(self):
    return "wifi_ap"

  @property
  def description(self):
    inspection = self.inspect()
    return {"type": "wifi_ap", "ssid": self.ssid, "psk": self.password}

  def stop(self):
    if hasattr(self, "container") and self.container:
      self.__teardown()
      self.container.stop()
      del self.container

  def run(self):
    kwargs = {"stdin_open": True, "cap_add": ["NET_ADMIN"]}

    if self.container_name:
      kwargs.update({"name": self.container_name})

    super().run(**kwargs)
    self.logger.info("Creating WiFi AP node: {}".format(self.name))
    self.__setup_namespace_networking_env()
    self.__setup_network_forward_rules()
    self.__update_hostapd_ssid()
    self.__update_hostapd_password()
    self.__run_hostapd()
    self.__run_dnsmasq()
    utils.sleep_time(self.logger, 2, "creating WiFi AP node")

  def get_wifi_ssid(self):
    return self.ssid if self.ssid else \
        "-".join(["wifiap",
                  "".join(random.choice(CHAR_SRC) for _ in range(5))])

  def get_wifi_password(self):
    return self.password if self.password else \
        "".join(random.choice(CHAR_SRC) for _ in range(8))

  def __run_hostapd(self):
    ret = self.container.exec_run(
        "/usr/sbin/hostapd -B /etc/hostapd/hostapd.conf")
    if ret.exit_code != 0:
      raise ContainerExecError("unable to start up hostapd server!!")

  def __run_dnsmasq(self):
    ret = self.container.exec_run("/usr/sbin/dnsmasq -C /etc/dnsmasq.conf")
    if ret.exit_code != 0:
      raise ContainerExecError("unable to start up dnsmasq server!!")

  def __setup_namespace_networking_env(self):
    network_commands = [
        "ip addr flush dev wlan0",
        "ip addr add 10.0.1.1/24 dev wlan0",
    ]

    for command in network_commands:
      ret = utils.netns_run(self.logger, command, self.name)
      if ret.returncode != 0:
        raise IpNetnsExecError("failed to run cmd: {}\n {}".format(
            command, ret.stderr))

    ret = self.container.exec_run("route add -net 10.0.1.0/24 gw 10.0.1.1")
    if ret.exit_code != 0:
      raise ContainerExecError("unable to set default gw for wlan0")

  def __setup_network_forward_rules(self):
    forward_rules = [
        "iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE",
        "iptables -A FORWARD -i eth0 -o wlan0 -m \
                state --state RELATED,ESTABLISHED -j ACCEPT",
        "iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT",
    ]

    for rule in forward_rules:
      ret = self.container.exec_run(rule)
      if ret.exit_code != 0:
        raise ContainerExecError("unable to set forward rule: {}".format(rule))

  def __update_hostapd_ssid(self):
    ret = self.container.exec_run("sed -i 's/ssid=/ssid={}/' \
                /etc/hostapd/hostapd.conf".format(self.ssid))
    if ret.exit_code != 0:
      raise ContainerExecError("failed to set wifi ap ssid: {}".format(
          self.ssid))

  def __update_hostapd_password(self):
    ret = self.container.exec_run(
        "sed -i 's/wpa_passphrase=/wpa_passphrase={}/' \
                /etc/hostapd/hostapd.conf".format(self.password))
    if ret.exit_code != 0:
      raise ContainerExecError("failed to set wifi ap password: {}".format(
          self.pasword))

  def __teardown(self):
    self.logger.info("stopping hostapd...")
    self.container.exec_run("killall hostapd")
    self.logger.info("stopping dnsmasq...")
    self.container.exec_run("killall dnsmasq")
    self.wifi_capability.disable_capability(self)
