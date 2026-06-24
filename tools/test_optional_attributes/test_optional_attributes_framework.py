# Copyright 2026 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import logging
from mobly import asserts
import matter.clusters as Clusters
from matter.testing.decorators import async_test_body
from matter.testing.matter_testing import MatterBaseTest
from matter.testing.runner import default_matter_test_main

class TestOptionalAttributes(MatterBaseTest):

    # Mapping of Cluster Object to list of Attribute Objects to check
    # We use the actual Cluster objects from matter.clusters
    OPTIONAL_ATTRIBUTES = {
        Clusters.BasicInformation: [
            Clusters.BasicInformation.Attributes.ManufacturingDate,
            Clusters.BasicInformation.Attributes.PartNumber,
            Clusters.BasicInformation.Attributes.ProductURL,
            Clusters.BasicInformation.Attributes.ProductLabel,
            Clusters.BasicInformation.Attributes.SerialNumber,
            Clusters.BasicInformation.Attributes.LocalConfigDisabled,
            Clusters.BasicInformation.Attributes.Reachable,
            # TODO:Intentionally skip ProductAppearance for now, it requires to set nvs storage.
            # Clusters.BasicInformation.Attributes.ProductAppearance,
        ],
        Clusters.BooleanStateConfiguration: [
            # TODO:Intentionally skip DefaultSensitivityLevel, AlarmsEnabled, optional features conformance.
            # Clusters.BooleanStateConfiguration.Attributes.DefaultSensitivityLevel,
            #Clusters.BooleanStateConfiguration.Attributes.AlarmsEnabled,
            Clusters.BooleanStateConfiguration.Attributes.SensorFault,
        ],
        Clusters.Descriptor: [
            # Clusters.Descriptor.Attributes.EndpointUniqueId 
            # Note: EndpointUniqueId might not be available in all older versions of the controller definitions
            # We will check dynamically if possible, or skip if attribute object doesn't exist
        ],
        Clusters.ElectricalEnergyMeasurement: [
            Clusters.ElectricalEnergyMeasurement.Attributes.CumulativeEnergyReset,
        ],
        Clusters.ElectricalPowerMeasurement: [
            # TODO:Intentionally skip Ranges for now, requires initialization setup.
            # Clusters.ElectricalPowerMeasurement.Attributes.Ranges,
            # Clusters.ElectricalPowerMeasurement.Attributes.Voltage,
            # TODO:Intentionally skip other attributes for now, has features conformance.
            # Clusters.ElectricalPowerMeasurement.Attributes.ActiveCurrent,
            # Clusters.ElectricalPowerMeasurement.Attributes.ReactiveCurrent,
            # Clusters.ElectricalPowerMeasurement.Attributes.ApparentCurrent,
            # Clusters.ElectricalPowerMeasurement.Attributes.ReactivePower,
            # Clusters.ElectricalPowerMeasurement.Attributes.ApparentPower,
            # Clusters.ElectricalPowerMeasurement.Attributes.RMSVoltage,
            # Clusters.ElectricalPowerMeasurement.Attributes.RMSCurrent,
            # Clusters.ElectricalPowerMeasurement.Attributes.RMSPower,
            # Clusters.ElectricalPowerMeasurement.Attributes.Frequency,
            # Clusters.ElectricalPowerMeasurement.Attributes.PowerFactor,
            # Clusters.ElectricalPowerMeasurement.Attributes.NeutralCurrent,
        ],
        Clusters.EthernetNetworkDiagnostics: [
            Clusters.EthernetNetworkDiagnostics.Attributes.PHYRate,
            Clusters.EthernetNetworkDiagnostics.Attributes.FullDuplex,
            Clusters.EthernetNetworkDiagnostics.Attributes.CarrierDetect,
            Clusters.EthernetNetworkDiagnostics.Attributes.TimeSinceReset,
        ],
        Clusters.GeneralDiagnostics: [
            Clusters.GeneralDiagnostics.Attributes.TotalOperationalHours,
            Clusters.GeneralDiagnostics.Attributes.BootReason,
            Clusters.GeneralDiagnostics.Attributes.ActiveHardwareFaults,
            Clusters.GeneralDiagnostics.Attributes.ActiveRadioFaults,
            Clusters.GeneralDiagnostics.Attributes.ActiveNetworkFaults,
        ],
        Clusters.OccupancySensing: [
            Clusters.OccupancySensing.Attributes.HoldTime,
            Clusters.OccupancySensing.Attributes.HoldTimeLimits,
        ],
        Clusters.HepaFilterMonitoring: [
            Clusters.HepaFilterMonitoring.Attributes.InPlaceIndicator,
            Clusters.HepaFilterMonitoring.Attributes.LastChangedTime,
        ],
        Clusters.SoftwareDiagnostics: [
            Clusters.SoftwareDiagnostics.Attributes.ThreadMetrics,
            Clusters.SoftwareDiagnostics.Attributes.CurrentHeapFree,
            Clusters.SoftwareDiagnostics.Attributes.CurrentHeapUsed,
        ],
        Clusters.TimeSynchronization: [
            Clusters.TimeSynchronization.Attributes.TimeSource,
        ],
        Clusters.WiFiNetworkDiagnostics: [
            Clusters.WiFiNetworkDiagnostics.Attributes.CurrentMaxRate,
        ],
        Clusters.TemperatureMeasurement: [
            Clusters.TemperatureMeasurement.Attributes.Tolerance,
        ],
        Clusters.FlowMeasurement: [
            Clusters.FlowMeasurement.Attributes.Tolerance,
        ],
        Clusters.PressureMeasurement: [
            Clusters.PressureMeasurement.Attributes.Tolerance,
            Clusters.PressureMeasurement.Attributes.ScaledTolerance,
        ],
        Clusters.RelativeHumidityMeasurement: [
            Clusters.RelativeHumidityMeasurement.Attributes.Tolerance,
        ],
    }

    # Add EndpointUniqueId dynamically if it exists
    if hasattr(Clusters.Descriptor.Attributes, 'EndpointUniqueId'):
        OPTIONAL_ATTRIBUTES[Clusters.Descriptor].append(Clusters.Descriptor.Attributes.EndpointUniqueId)

    @async_test_body
    async def test_optional_attributes(self):
        dev_ctrl = self.default_controller
        node_id = self.dut_node_id
        # We assume endpoint 1 for most application clusters, or we can probe the endpoint.
        # For simplicity in this test, we might iterate endpoints or just default to 1.
        # Better: Read the descriptor to find endpoints.
        
        logging.info(f"Reading from Node ID: {node_id}")

        # 1. Get List of Endpoints
        endpoint_list = await self.read_single_attribute_check_success(
            cluster=Clusters.Descriptor,
            attribute=Clusters.Descriptor.Attributes.PartsList,
            dev_ctrl=dev_ctrl,
            node_id=node_id,
            endpoint=0
        )
        endpoints = [0] + list(endpoint_list)
        logging.info(f"Found Endpoints: {endpoints}")

        failures = []
        successes = []

        for cluster_class, attributes in self.OPTIONAL_ATTRIBUTES.items():
            cluster_name = cluster_class.__name__
            
            # Find which endpoint has this cluster
            target_endpoint = None
            for ep in endpoints:
                # Read ServerList
                server_list = await self.read_single_attribute_check_success(
                    cluster=Clusters.Descriptor,
                    attribute=Clusters.Descriptor.Attributes.ServerList,
                    dev_ctrl=dev_ctrl,
                    node_id=node_id,
                    endpoint=ep
                )
                if cluster_class.id in server_list:
                    target_endpoint = ep
                    break
            
            if target_endpoint is None:
                logging.warning(f"Cluster {cluster_name} not found on any endpoint. Skipping attributes.")
                continue

            logging.info(f"Checking {cluster_name} on Endpoint {target_endpoint}")

            # Read AttributeList to verify presence
            # AttributeList is global attribute 0xFFFB
            # We can use the generated cluster object for this if available, or just standard read
            try:
                # Using the standard read to get the list of supported attributes
                # We can't always rely on the high-level object for 'AttributeList' if it's not generated
                # typically it is in Globals?
                # Actually, standard read returns the decoded structure.
                # Let's try reading the specific attribute and catch errors.
                pass
            except Exception as e:
                logging.error(f"Failed to prepare check for {cluster_name}: {e}")
                continue

            for attribute_def in attributes:
                attr_name = attribute_def.__name__
                try:
                    val = await self.read_single_attribute_check_success(
                        cluster=cluster_class,
                        attribute=attribute_def,
                        dev_ctrl=dev_ctrl,
                        node_id=node_id,
                        endpoint=target_endpoint
                    )
                    successes.append(f"{cluster_name}::{attr_name} = {val}")
                    logging.info(f"  [PASS] {attr_name}: {val}")
                except Exception as e:
                    # If it fails, it might be UnsupportedAttribute if not implemented
                    failures.append(f"{cluster_name}::{attr_name} - {str(e)}")
                    logging.error(f"  [FAIL] {attr_name}: {e}")

        logging.info("-" * 40)
        logging.info("Test Results:")
        logging.info(f"Passed: {len(successes)}")
        logging.info(f"Failed: {len(failures)}")
        
        if failures:
            for f in failures:
                logging.error(f"  {f}")
            asserts.fail("Some optional attributes failed to read.")

if __name__ == "__main__":
    default_matter_test_main()
