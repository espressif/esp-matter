import xml.etree.ElementTree as ET

def wrap(inputDataXmlStr):
  radiConfXmlRoot = ET.fromstring(inputDataXmlStr)

  base_channel_configurations = radiConfXmlRoot.find("base_channel_configurations")
  for base_ch_conf in base_channel_configurations.findall("base_channel_configuration"):
    # Return the original XML if it is already in the wrapped format
    if base_ch_conf.get("profile").find("serializableObject:Efr32RadioConfiguratorProfile:", 0) == -1:
      return inputDataXmlStr
      
    # Set profile_inputs: set values and add categories
    profile_inputs = base_ch_conf.find("profile_inputs")
    if profile_inputs is not None:
      for input in profile_inputs.findall("input"):
        # Add category and "."
        key = input.find("key")
        key.text = key.text.lower()
    else:
      print("'" + base_ch_conf.get("name") + "' protocol has no 'profile_inputs' section!")

    # Set channel_config_entries
    channel_config_entries = base_ch_conf.find("channel_config_entries")
    for channel_conf_entry in channel_config_entries.findall("channel_config_entry"):
      # Set profile_input_overrides: set values and add categories
      profile_input_overrides = channel_conf_entry.find("profile_input_overrides")
      if profile_input_overrides is not None:
        for override in profile_input_overrides.findall("override"):
            # Add category and "."
            key = override.find("key")
            key.text = key.text.lower()

  # Make an XML string from the result
  xmlStr = ET.tostring(radiConfXmlRoot, encoding="utf-8")
  return xmlStr
