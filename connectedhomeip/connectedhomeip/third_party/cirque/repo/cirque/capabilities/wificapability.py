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

from subprocess import PIPE
from pyroute2 import NetNS

from cirque.capabilities.basecapability import BaseCapability
from cirque.common.cirquelog import CirqueLog
from cirque.common.exceptions import (
    ConnectivityError,
    ContainerExecError,
    IpNetnsExecError,
    LoadKernelError,
    NameSpaceOperatingError,
    PHYDeviceError,
)
import cirque.common.utils as utils


class WiFiCapability(BaseCapability):
  DEFAULT_RADIOS = 16
  RUNTIME_NAMESPACE = "/var/run/netns"

  def __init__(self):
    self.logger = CirqueLog.get_cirque_logger(self.__class__.__name__)

  @property
  def name(self):
    return "WiFi"

  def get_docker_run_args(self, docker_node):
    return {
        "privileged": True,
    }

  def enable_capability(self, docker_node):
    if not WiFiCapability.is_mac80211_hwsim_loaded():
      WiFiCapability.load_kernel_mac80211_hwsim()
    try:
      self.__get_available_phy_device(docker_node)
      self.__phy_namespace_setup(docker_node)
      if docker_node.type != "wifi_ap":
        self.start_wpa_supplicant_service(docker_node)
    except Exception as e:
      self.logger.exception("{!r}".format(e))
      return -1
    self.logger.info("Node: {} successfully enabled wifi capability".format(
        docker_node.name))

  def disable_capability(self, docker_node):
    try:
      docker_node.container.exec_run("killall wpa_supplicant")
      self.__phy_namespace_restore(docker_node)
    except Exception as e:
      self.logger.exception("{!r}".format(e))
    self.logger.info("Node: {} successfully disabled wifi capablility".format(
        docker_node.name))

  def __get_available_phy_device(self, docker_node):
    ret = utils.host_run(self.logger, "iw dev")
    if ret.stderr != b"":
      raise PHYDeviceError("Error:{}".format(ret.stderr))

    if ret.returncode == 1:
      raise PHYDeviceError("run out of all the phy devices!")

    lines = ret.stdout.decode("utf-8").split("\n")
    lines = [line.strip() for line in lines]
    lines = [
        line for line in lines
        if line.startswith("phy") or line.startswith("Interface")
    ]
    devices = [(l1, l2.split()[-1])
               for l1, l2 in zip(lines, lines[1:])
               if l1.startswith("phy")]
    phy_device, interface = devices.pop()
    phy_device = "".join(phy_device.split("#"))
    docker_node.wlan_phy_device = phy_device
    docker_node.wlan_interface = interface
    self.logger.info("container {}: phy device {} interface {}".format(
        docker_node.name, docker_node.wlan_phy_device,
        docker_node.wlan_interface))

  def __phy_namespace_setup(self, docker_node):
    try:
      self.__mount_container_namespace_to_host(docker_node)
      self.__add_phy_device_to_container_namespace(docker_node)
      self.__bring_up_wifi_interface(docker_node)
    except Exception as e:
      docker_node.logger.exception("{!r}".format(e))
      self.__phy_namespace_restore(docker_node)
      raise NameSpaceOperatingError("{!r}".format(e))

  def start_wpa_supplicant_service(self, docker_node):
    command = "wpa_supplicant -B -i wlan0 \
            -c /etc/wpa_supplicant/wpa_supplicant.conf \
            -f /var/log/wpa_supplicant.log -t -dd"

    return docker_node.container.exec_run(command)

  def __mount_container_namespace_to_host(self, docker_node):
    if not os.path.isdir(self.RUNTIME_NAMESPACE):
      os.makedirs(self.RUNTIME_NAMESPACE)
    if not os.path.isdir(self.RUNTIME_NAMESPACE):
      raise RuntimeError("unable to create target folder: {}".format(
          self.RUNTIME_NAMESPACE))

    pid = docker_node.get_container_pid()
    sym_src = "/proc/{}/ns/net".format(pid)
    sym_dst = "/var/run/netns/{}".format(docker_node.name)
    os.symlink(sym_src, sym_dst)
    if not os.path.isfile(sym_dst):
      raise NameSpaceOperatingError(
          "unable mounting container namespace: {} to host".format(
              docker_node.name))

  def __add_phy_device_to_container_namespace(self, docker_node):
    ret = utils.host_run(
        docker_node.logger,
        "iw phy {} set netns name {}".format(docker_node.wlan_phy_device,
                                             docker_node.name))
    if ret.returncode != 0:
      raise NameSpaceOperatingError(
          "failed adding {} to container namespace: {}".format(
              docker_node.wlan_phy_device, docker_node.name))

  def __bring_up_wifi_interface(self, docker_node):
    commands = [
        "ip link set {} down".format(docker_node.wlan_interface),
        "ip link set {} name wlan0".format(docker_node.wlan_interface),
        "ip link set wlan0 up",
    ]
    for command in commands:
      ret = utils.netns_run(docker_node.logger, command, docker_node.name)
      if ret.returncode != 0:
        raise IpNetnsExecError("Error: {} on command: {}".format(
            ret.stderr, command))

  def __phy_namespace_restore(self, docker_node):
    self.logger.debug("running phy device namespace restore...")
    if os.path.isfile(os.path.join(self.RUNTIME_NAMESPACE, docker_node.name)):
      if hasattr(docker_node, "phy_device") and docker_node.phy_device:
        namespace = NetNS(docker_node.name)
        # return a list
        ifidx = namespace.link_lookup(ifname="wlan0")
        # find wlan0 in netns
        if len(ifidx) != 0:
          phy_device = docker_node.phy_device
          commands = [
              "ip addr flush dev wlan0",
              "ip link set wlan0 down",
              "ip link set wlan0 name {}".format(docker_node.wlan_interface),
          ]
          for command in commands:
            ret = utils.netns_run(docker_node.logger, command, docker_node.name)
            if ret.returncode != 0:
              raise IpNetnsExecError("Error: {} on command: {}".format(
                  ret.stderr, command))

          self.logger.debug("moving out {} device from namespace:{}".format(
              phy_device, docker_node.name))
          ret = utils.netns_run(docker_node.logger,
                                "iw phy {} set netns 1".format(phy_device),
                                docker_node.name)
          if ret.returncode != 0:
            raise IpNetnsExecError(
                "Error: {} on removing {} out of namespace".format(
                    ret.stderr, phy_device))
      # del created namespace
      self.logger.debug("removing explored namespace: {}".format(
          docker_node.name))
      ret = utils.host_run(docker_node.logger,
                           "ip netns del {}".format(docker_node.name))
      if ret.returncode != 0:
        raise NameSpaceOperatingError("Error: {} on delete netns: {}".format(
            ret.stderr, docker_node.name))

  @staticmethod
  def is_mac80211_hwsim_loaded():
    ret = utils.host_run(CirqueLog.get_cirque_logger(),
                         "lsmod | grep mac80211_hwsim")
    return ret.returncode == 0

  @staticmethod
  def load_kernel_mac80211_hwsim(radios=DEFAULT_RADIOS):
    logger = CirqueLog.get_cirque_logger()
    logger.info("kernel module mac80211_hwsim is not loaded, loading now...")
    ret = utils.host_run(
        logger, "modprobe mac80211_hwsim \
                                 radios={}".format(radios))
    if ret.returncode != 0:
      raise LoadKernelError("unable to load module mac80211_hwsim!!")
    utils.sleep_time(logger, 5, "loading mac80211_hwsim module")

  @property
  def description(self):
    return {}
