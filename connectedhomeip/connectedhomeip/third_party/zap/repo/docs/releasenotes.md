# Release notes

2022.3.9:

- Enable .zap files to automatically re-load custom XML references (754270)

  2021.10.18:

- Limited support for Custom ZCL device types (446696)
  If a Custom ZCL device type has overlapping Profile and Device ID with the standard ZCL standard device types, the save / generation process might not behave correctly.
  A temporary workarounds is to remove the standard ZCL device type from the ZCL XML
  to accommodate the custom definition.
