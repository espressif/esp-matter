import os
import sys
import xml.etree.ElementTree as ET
from efr32multiphyconfig import Efr32MultiPHYConfigurator
from pycalcmodel.core.output import ModelOutputType
# from memory_profiler import profile
import json
import enum

class File(object):
  def __init__(self, name, source_code):
    self.name = name
    self.source_code = source_code

def generationResults(calc_multi_phy_model):
  generationResults = []
  for base_ch_conf in calc_multi_phy_model.base_channel_configurations.base_channel_configuration:
    for ch_conf_ent in base_ch_conf.channel_config_entries.channel_config_entry:
      output_model = ch_conf_ent.radio_configurator_output_model
      logs = []
      for _log in output_model.logs:
        log = {}
        log["type"] = _log.log_type.name
        log["message"] = _log.message.split("\n") if type(_log.message) == str else _log.message
        logs.append(log)
      cfg_logs = []
      for output in output_model.profile.get_outputs([ModelOutputType.INFO]):
        cfg_log = {}
        cfg_log["name"] = output.readable_name
        cfg_log["value"] = str(output.var_value) if isinstance(output.var_value, enum.Enum)  else output.var_value
        cfg_logs.append(cfg_log)
      for output in output_model.profile.get_outputs([ModelOutputType.LINKED_IO]):
        cfg_log = {}
        cfg_log["name"] = output.readable_name
        cfg_log["value"] = str(output.var_value) if isinstance(output.var_value, enum.Enum)  else output.var_value
        cfg_logs.append(cfg_log)

      channelGenRes = {
        "channel_name": base_ch_conf.name + "-" + ch_conf_ent.name,
        "result_code": str(output_model.result_code),
        "error_message": output_model.error_message,
        "logs": logs,
        "cfg_logs": cfg_logs
      }
      generationResults.append(channelGenRes)
  return generationResults

# Run modem calculator 
#   - xmlFilePath: the Multi PHY Radio Configuration XML's file path
#   - targetDirPath: the output directory path e.g.: ../autogen/
# @profile
def generate(xmlFilePath, targetDirPath):

  # Check the given paths
  if not os.path.isfile(xmlFilePath):
    print("XML file path does not exist!")
    return(1)

  with open(xmlFilePath, "r", encoding='utf-8') as xml_file:
    xmlStr = xml_file.read()

  ####################################################################################################################
  # Configuration wrapper, which make the input names to lower case                                                  #
  ####################################################################################################################
  from ConfigurationWrapper import wrap
  xmlStr = wrap(xmlStr)
  # tree = ET.ElementTree(ET.fromstring(xmlStr))
  # tree.write("wrappedInput.xml", encoding="utf-8", xml_declaration=True, default_namespace=None, method="xml")
  ####################################################################################################################

  # Set the model type
  args = {"multi_phy_config_model": xmlStr}
  # Run the Multi PHY Radio Configurator
  configurator = Efr32MultiPHYConfigurator()
  data = configurator.configure(**args)
  calc_multi_phy_model = data["multi_phy_config_model_memory_object"]
  if (calc_multi_phy_model is None):
    print("Output model was not found!")
    sys.exit(1)

  # Files from calc_multi_phy_model.output_files
  filesFromOutputFiles = ["rail_config.h", \
                          "rail_config.c"]
  #filesFromOutputFiles.append("rail_test_commands.txt")
  # Get the files from the model
  outputFiles = [file for file in calc_multi_phy_model.output_files.file if file.name in filesFromOutputFiles]

  # Get generation results
  genResults = generationResults(calc_multi_phy_model)
  resultsFile = File("radioconf_generation_log.json", json.dumps(genResults, indent=2))

  # Generated files' list
  files = outputFiles
  files.append(resultsFile)

  print("\n### Created files: ###\n")

  for file in files:
    full_path = os.path.join(targetDirPath, file.name)
    os.makedirs(os.path.normpath(os.path.dirname(full_path)), exist_ok=True)
    with open(full_path, "w") as f:
      print("        " + file.name)
      f.write(file.source_code)

def inputParser(xmls):
  xmll = []
  for x in xmls:
    if os.path.isdir(x):
      xmll.extend([os.path.join(x, f) for f in os.listdir(x)])
    else:
      xmll.append(x)
      
  # maximum one .radioconf file is allowed, although more is still tolerated
  # .radioconf file(s) are moved to front of the list
  radioconfl = [x for x in xmll if x.lower().endswith(".radioconf")]
  
  if len(radioconfl) == 0:
    print("Warning: no 'radioconf' file in list!")
    sys.exit(1)

  if len(radioconfl) > 1:
    print("Warning: multiple 'radioconf' files in list!")
    
  radioconfl.sort()
  return radioconfl[0]

if __name__ == '__main__':

  import argparse
  parser = argparse.ArgumentParser(description='This Wrapper enables to use the EFR32 Multi PHY Radio Configurator as a CLI tool.')

  parser.add_argument('inputs',
                      nargs='+',
                      help='EFR32 Multi PHY XML files or directories to find XML files. Separate input with ;')
  parser.add_argument('-o', '--outdir',
                      help='Output directory path e.g.: ../autogen/')
  args = parser.parse_args()

  # Paths
  xmlFilePath = ""
  targetDirPath = ""

  if args.inputs is not None:
    xmlFilePath = inputParser(args.inputs)

  if args.outdir is not None:
    targetDirPath = args.outdir

  generate(xmlFilePath, targetDirPath)


