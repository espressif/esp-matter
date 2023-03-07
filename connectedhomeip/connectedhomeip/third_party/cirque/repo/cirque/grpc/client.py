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

import argparse
import time
import logging
import os

import grpc
from google.rpc import code_pb2, status_pb2

import cirque.proto.capability_pb2 as capability_pb2
import cirque.proto.device_pb2 as device_pb2
import cirque.proto.service_pb2_grpc as service_pb2_grpc
import cirque.proto.service_pb2 as service_pb2
from cirque.proto.device_pb2 import DeviceSpecification

from cirque.common.cirquelog import CirqueLog
from cirque.common.utils import sleep_time
from cirque.proto.capability_pb2 import (WeaveCapability, ThreadCapability,
                                         WiFiCapability, XvncCapability,
                                         InteractiveCapability,
                                         LanAccessCapability, MountPair,
                                         MountCapability,
                                         TrafficControlCapability)


def client_sample(weave_cert_path):
  CirqueLog.setup_cirque_logger()
  logger = CirqueLog.get_cirque_logger()
  with grpc.insecure_channel('localhost:50051') as channel:
    stub = service_pb2_grpc.CirqueServiceStub(channel)
    home_id = stub.CreateCirqueHome(
        service_pb2.CreateCirqueHomeRequest()).home_id
    mount_capability = MountCapability()
    mount_capability.mount_pairs.append(
        MountPair(
            host_path=os.path.abspath(os.path.relpath(__file__)),
            target_path='/tmp/test'))
    device_0_id = stub.CreateCirqueDevice(
        service_pb2.CreateCirqueDeviceRequest(
            home_id=home_id,
            specification=device_pb2.DeviceSpecification(
                device_type='test',
                base_image='sdk_border_router',
                weave_capability=WeaveCapability(
                    weave_certificate_path=weave_cert_path),
                thread_capability=ThreadCapability(),
                wifi_capability=WiFiCapability(),
                xvnc_capability=XvncCapability(localhost=True),
                mount_capability=mount_capability))).device.device_id
    device_1_id = stub.CreateCirqueDevice(
        service_pb2.CreateCirqueDeviceRequest(
            home_id=home_id,
            specification=device_pb2.DeviceSpecification(
                device_type='mobile',
                base_image='mobile_node_image',
                interactive_capability=InteractiveCapability(),
                lan_access_capability=LanAccessCapability(),
            ))).device.device_id
    device_info = stub.QueryCirqueDevice(
        service_pb2.QueryCirqueDeviceRequest(
            home_id=home_id, device_id=device_1_id))
    logger.info(device_info)
    logger.info('Waiting for device to fully launch')
    sleep_time(logger, 10)
    logs = stub.GetCirqueDeviceLog(
        service_pb2.GetCirqueDeviceLogRequest(
            home_id=home_id, device_id=device_0_id, tail=10))
    logger.info(logs.log)
    stub.StopCirqueDevice(
        service_pb2.StopCirqueDeviceRequest(
            home_id=home_id, device_id=device_0_id))
    stub.StopCirqueDevice(
        service_pb2.StopCirqueDeviceRequest(
            home_id=home_id, device_id=device_1_id))
    stub.StopCirqueHome(service_pb2.StopCirqueHomeRequest(home_id=home_id))


def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('--weave-cert-path')
  args = parser.parse_args()

  client_sample(args.weave_cert_path)


if __name__ == '__main__':
  main()
