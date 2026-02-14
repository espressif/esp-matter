---
name: Report Certification Failure
about: Create a bug report for Matter device certification failure
title: '[CERT-FAILURE]'
labels: 'certification'
---

**Failed Test Case**
- Test Case ID (e.g., TC-ACL-2.4):
- Test Case Name:
- Test Step Where Failure Occurred:

**Describe the issue**
Please describe the certification failure in detail.

**DUT details**
Please provide the following information:
- Matter version:
- ESP-Matter Commit Id:
- ESP-IDF Commit Id:
- SoC (eg: ESP32 or ESP32-C3):
- sdkconfig:
- Partition table:
- Endpoint Hierarchy:

**Test Harness details**
- Test Harness Version:
- Test Harness SHA:
- Test Harness SDK SHA:

**Attachments**
Please attach the following files:
- [ ] PICS items
- [ ] Device logs (with debug level)
- [ ] Test harness logs

**Checklist**
- [ ] I have gone through the [GitHub issues](https://github.com/espressif/esp-matter/issues) and the [Matter Programming Guide](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/) and did not find a similar issue.
- [ ] I have verified that the PICS items match the cluster configuration on my DUT.
- [ ] I have verified that the PICS items are correct as per the test plan.
- [ ] I have run the test locally and verified that the issue is reproducible. (You can run tests locally by following instructions here: [Python tests](https://github.com/project-chip/connectedhomeip/blob/master/docs/testing/python.md#running-tests-locally) and [YAML tests](https://github.com/project-chip/connectedhomeip/blob/master/docs/testing/yaml.md#running-yaml-tests))
