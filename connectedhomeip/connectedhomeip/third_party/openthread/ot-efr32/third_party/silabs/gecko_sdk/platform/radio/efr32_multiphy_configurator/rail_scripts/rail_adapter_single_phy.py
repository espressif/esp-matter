from pycalcmodel.core.output import ModelOutputType
from rail_scripts.rail_adapter import *
from rail_scripts._version import __version__

from rail_scripts import config

class RAILAdapter_SinglePhy(RAILAdapter):

  def __init__(self, **kwargs):
    self.rc_version = kwargs["rc_version"] if "rc_version" in kwargs else None
    setattr(self, "version", __version__)
    self.instanceDict = kwargs.get("dictionary")
    regex = re.compile(r'rail_api_(\d+).x')
    # Captures RAIL major version into an int
    # We use "get", to avoid raising "KeyError"; default to "rail_api_2.x"
    adapter_name = kwargs.get("adapter_name", "rail_api_2.x")
    re_match = regex.match(adapter_name)
    if re_match:
      self.rail_version= int(re_match.group(1))
    else:
      raise Exception("Unknown RAIL Adapter name: {}".format(adapter_name))
    with open(os.path.join(RAILAdapter.current_dir,"rail_model_single_phy.yml")) as f:
      self.yamlobject = None
      if hasattr(yaml, 'FullLoader'):
        self.yamlobject = yaml.load(f.read(), Loader=yaml.FullLoader)
      else:
        self.yamlobject = yaml.load(f.read())
    self.railModel = RAILModel(self.yamlobject)
    self._railModelPopulated = False

  def _encodeAction(self, phyConfigEntry, address, length, values=[]):
    if length > 0:
      newModemConfigEntry = phyConfigEntry.modemConfigEntries.addNewElement()
      encodedAddress = self._encodeWriteAddress(address, length)
      firstValue = values[0]
      newModemConfigEntry.encodedAction.value = encodedAddress
      newModemConfigEntry.encodedValues.value = [firstValue]

      for val in values[1:]:
        newModemConfigEntry.encodedValues.value.append(val)

  def formatModemConfigEntries(self, phyConfigEntry, registerEntries):
    if self.rail_version == 1:
      # RAIL 1.x requires an action for every write
      for registerAddress, registerValue in registerEntries:
        self._encodeAction(phyConfigEntry, registerAddress, 1, [registerValue])
    else:
      # RAIL 2.x supports packed radio configurations
      currentAddress = 0
      length = 0
      values = []
      for registerAddress, registerValue in registerEntries:
        if (registerAddress - currentAddress) == (4 * length):
          length += 1
          values.append(registerValue)
        else:
          self._encodeAction(phyConfigEntry, currentAddress, length, values)
          currentAddress = registerAddress
          length = 1
          values = [registerValue]
      # One final write for whatever is left
      self._encodeAction(phyConfigEntry, currentAddress, length, values)

  def _generateModemConfigEntries(self, phyConfigEntry, model, regs):
    # Write the address of the phyInfo structure to SEQ.PHYINFO.ADDRESS
    address = self._getRegAddress("SEQ","PHYINFO")
    regs.append((address, phyConfigEntry.phyInfo))

    # Write the address of the block decoding table to BLOCKRAMADDR when
    # BLOCKWHITEMODE == BLOCKLOOKUP. Include the register even when the table
    # is absent for speed with continuous write, and make the potential
    # future implementation of RAIL_LIB-1138 easier
    address = self._getRegAddress("FRC","BLOCKRAMADDR")
    if phyConfigEntry.frameCodingTable.values:
      regs.append((address, phyConfigEntry.frameCodingTable))
    else:
      regs.append((address, 0))

    # Write the address of the convDecodeBuffer to CONVRAMADDR when
    # fecEnabled, include even when absent for speed
    address = self._getRegAddress("FRC","CONVRAMADDR")

    if self.partFamily.lower() in ["dumbo","jumbo","nerio","nixi"]:
      # Series - 1 point to allocated buffer, or just leave at 0 if not needed.
      if phyConfigEntry.fecEnabled.value:
        convDecodeBuffer = "convDecodeBuffer"
      else:
        convDecodeBuffer = 0
    elif self.partFamily.lower() in ["panther"]:
      # On Panther, we decided to ALWAYS write the FRC_CONVRAMADDR to HIGH RAM offset 0
      convDecodeBuffer = 0
    else:
      # On rest of series-2, we decided to ALWAYS write the FRC_CONVRAMADDR to FRCRAM offset 0
      convDecodeBuffer = 0x4000

    regs.append((address, convDecodeBuffer))

    regs.sort() # Put the registers in the right order again

    # Write the address of the last Dynamic Slicer Configuration link
    # if there are any present, exclude the write if false
    if len(phyConfigEntry.dynamicSlicerConfigEntries) > 0 and \
       self.partFamily not in ["panther", "lynx", "ocelot", "leopard"]:
        address = self._getRegAddress("SEQ","DYNAMIC_CHPWR_TABLE")
        regs.append((address, phyConfigEntry.dynamicSlicerConfigEntries.lastElement))

    return regs

  # Takes a model instance and writes it to a rm object
  # register_dict keeps track of written registers
  def _writeModelToRmDevice(self, radio_model, rm, register_dict):
    # write the registers to the rm         fixme:  This should use the profile instead
    for output in radio_model.profile.get_outputs([ModelOutputType.SVD_REG_FIELD, ModelOutputType.SEQ_REG_FIELD]):

        fieldname = output.var.svd_mapping
        registerName = self._getRegNameFromFieldName(fieldname)
        reg_do_not_care = output.var.value_do_not_care

        if registerName in WRITE_ONLY_REGISTERS and not reg_do_not_care:
          register_dict[registerName] = output.var_value
        else:
          register_dict[registerName] = ''        # Add this register to the register dictionary

        if (output.var_value != None and not reg_do_not_care):
          register_write_exec_string = "rm.{0}.io={1}".format(fieldname, output.var_value)
          try:
            exec(register_write_exec_string)
          except Exception as e:
            print("Error Executing field write:")
            print(register_write_exec_string)
            print(e)
            sys.exit(1)
        elif not reg_do_not_care:
          print("Field {} does not have a valid value".format(fieldname))

    # Don't let the configurator dictate channel information to us. We use the
    # RAIL_ChannelConfig_t via SYNTH_Config to determine the below registers,
    # so removing them saves space and potential headache.
    # Also, BLOCKRAMADDR and CONVRAMADDR should only be written by us (RAIL)
    # We use pop(X, None) to avoid raising KeyError in case the register is not
    # in the dictionary
    for register in config.PROTECTED_REGS:
      x = register_dict.pop(register, None)

  def _generatePhyInfoStructure(self, phyConfigEntry, model):
    # Get a local reference to model.profile.outputs to use here
    outputs = model.profile.outputs

    # For antenna diversity, first grab the relevant outputs from the calculator
    antDivMode = outputs.get_output('div_antdivmode').var_value
    antDivRepeatDisable = outputs.get_output('div_antdivrepeatdis').var_value

    # Then, generate the register value than maps to the fields we care about
    antDivConfiguration = 0x0
    antDivConfiguration |= (antDivMode << self.rm.MODEM.CTRL3.ANTDIVMODE.bitOffset)
    antDivConfiguration |= (antDivRepeatDisable << self.rm.MODEM.CTRL3.ANTDIVREPEATDIS.bitOffset)

    phyConfigEntry.phyInfo._uniqueName = phyConfigEntry.name + "_phyInfo"
    data = phyConfigEntry.phyInfo.phyInfoData

    data.version.value = 0 if self.rail_version == 1 else 5
    data.freqOffsetFactor.value = outputs.get_output('frequency_offset_factor').var_value
    data.freqOffsetFactor_fxp.value = int(outputs.get_output('frequency_offset_factor_fxp').var_value or 0)
    data.antDivRxAutoConfig.value = antDivConfiguration
    data.src1Denominator.value = int(outputs.get_output('src1_calcDenominator').var_value or 0)
    data.src2Denominator.value = int(outputs.get_output('src2_calcDenominator').var_value or 0)
    data.txBaudRate.value = outputs.get_output('tx_baud_rate_actual').var_value
    data.baudPerSymbol.value = outputs.get_output('baud_per_symbol_actual').var_value
    data.bitsPerSymbol.value = outputs.get_output('bits_per_symbol_actual').var_value
    data.synthCache.value = int(model.vars.SYNTH_IFFREQ_IFFREQ.value or 0) \
                            | (model.vars.lodiv_actual.value << 25)
    if self.partFamily.lower() not in ["dumbo","jumbo","nerio","nixi"]:
      # Cap DEC0 at 3, since the decimation value for all values above 3 is 8.
      # Also don't use value 2, in case that's useful in the future
      DEC0_MAP = [0, 1, 1, 3, 3]
      cache40 = (model.vars.SYNTH_IFFREQ_LOSIDE.value << 20)            \
                | (DEC0_MAP[int(model.vars.MODEM_CF_DEC0.value or 0)] << 22)
      if ("MODEM_SRCCHF_BWSEL" in model.vars) and (model.vars.MODEM_SRCCHF_BWSEL.value is not None):
        cache40 |= (int(model.vars.MODEM_SRCCHF_BWSEL.value >= 2) << 21)
      if "RAC_IFADCTRIM0_IFADCENHALFMODE" in model.vars:
        cache40 |= model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value << 24
      data.synthCache.value |= cache40

  def _generateDynamicSlicerConfiguration(self, phyConfigEntry, model):
    # Check the feature is enabled in the model output
    if model.profile.outputs.get_output('dynamic_slicer_enabled').var_value == True:
      # Get the two arrays that must be defined
      dynamicSlicerThresholdValues = model.profile.outputs.get_output('dynamic_slicer_threshold_values').var_value
      dynamicSlicerLevelValues = model.profile.outputs.get_output('dynamic_slicer_level_values').var_value

      # Use dynamicSlicerLevelValues to traverse the loop since
      # it has one more item than dynamic_slicer_threshold_values
      for i, level in enumerate(dynamicSlicerLevelValues):

        # We need to define the links in reverse order because they need to
        # reference the previous one (linked list), by doing this we avoid the
        # need for forward declarations in the C code.
        dynSlicerConfigNum = len(dynamicSlicerLevelValues)-i-1
        newDynamicSlicerEntryName = phyConfigEntry.name+"_dynamicSlicerConfig"+str(dynSlicerConfigNum)

        # The threshold is stored using 2's complement in the LSB, thus the short mask (0xFF)
        # The value is stored in a uint16_t to optimize packing.
        threshold = (dynamicSlicerThresholdValues[-1] if i == 0 else dynamicSlicerThresholdValues[-i]) & 0xFF

        # The level is stored as a uint16_t, properly aligned to make the sequencer
        # code more efficient; thus the shiftValue-16.
        level = (dynamicSlicerLevelValues[-i-1] << self.rm.MODEM.CTRL1.FREQOFFESTLIM.bitOffset-16) & 0xFFFF

        newDynamicSlicerConfigEntry = phyConfigEntry.dynamicSlicerConfigEntries.addNewElement(newDynamicSlicerEntryName)
        newDynamicSlicerConfigEntry.threshold.value = threshold
        newDynamicSlicerConfigEntry.level.value = level
        newDynamicSlicerConfigEntry.next.value = None if i == 0 else phyConfigEntry.dynamicSlicerConfigEntries._elements[i-1]

  # For a particular PHY get the frame config
  def _generateFrameTypeStructures(self, phyConfigEntry, model):
    phyConfigEntry.frameLengthList.values = model.vars.frame_type_lengths.value
    phyConfigEntry.frameValidList.values = model.vars.frame_type_valid.value
    phyConfigEntry.frameFilterList.values = model.vars.frame_type_filter.value
    phyConfigEntry.frameType.offset.value = model.vars.frame_type_loc.value
    phyConfigEntry.frameType.mask.value = model.vars.frame_type_mask.value
    phyConfigEntry.frameType.frameLen.value = phyConfigEntry.frameLengthList
    phyConfigEntry.frameType.isValid.value = phyConfigEntry.frameValidList
    phyConfigEntry.frameType.addressFilter.value = phyConfigEntry.frameFilterList
    phyConfigEntry.frameType.variableAddrLoc.value = False

  def _generateIrCalStructure(self, phyConfigEntry, model):
    # Get a local reference to model.profile.outputs to use here
    outputs = model.profile.outputs

    irCalConfig = [
      outputs.get_output('ircal_auxndiv').var_value,
      outputs.get_output('ircal_auxlodiv').var_value,
      outputs.get_output('ircal_rampval').var_value,
      outputs.get_output('ircal_rxamppll').var_value,
      outputs.get_output('ircal_rxamppa').var_value,
      outputs.get_output('ircal_manufconfigvalid').var_value,
      outputs.get_output('ircal_pllconfigvalid').var_value,
      outputs.get_output('ircal_paconfigvalid').var_value,
      outputs.get_output('ircal_bestconfig').var_value,
      outputs.get_output('ircal_useswrssiaveraging').var_value,
      outputs.get_output('ircal_numrssitoavg').var_value,
      outputs.get_output('ircal_throwawaybeforerssi').var_value,
      outputs.get_output('ircal_delayusbeforerssi').var_value % 256,
      outputs.get_output('ircal_delayusbeforerssi').var_value / 256,
      outputs.get_output('ircal_delayusbetweenswrssi').var_value % 256,
      outputs.get_output('ircal_delayusbetweenswrssi').var_value / 256,
      outputs.get_output('ircal_agcrssiperiod').var_value,

      # recreate these Jumbo settings at the end for backwards compatibility with RAIL 1.x
      outputs.get_output('ircal_useswrssiaveraging2').var_value,
      outputs.get_output('ircal_numrssitoavg2').var_value,
      outputs.get_output('ircal_throwawaybeforerssi2').var_value,
      outputs.get_output('ircal_delayusbeforerssi2').var_value % 256,
      outputs.get_output('ircal_delayusbeforerssi2').var_value / 256,
      outputs.get_output('ircal_delayusbetweenswrssi2').var_value % 256,
      outputs.get_output('ircal_delayusbetweenswrssi2').var_value / 256,

      # Allow changing of default PA output power during IR cal (specifically for split TX/RX paths).
      outputs.get_output('ircal_power_level').var_value,
    ]

    phyConfigEntry.irCalConfig.values = irCalConfig

  def _generateChannelStructures(self, phyConfigEntry, model):
    newChannelConfigEntry = phyConfigEntry.channelConfigEntries.addNewElement()
    newChannelConfigEntry.baseFrequency.value = model.vars.base_frequency.value
    newChannelConfigEntry.channelSpacing.value = model.vars.channel_spacing.value
    newChannelConfigEntry.physicalChannelOffset.value = 0
    newChannelConfigEntry.channelNumberStart.value = 0
    newChannelConfigEntry.channelNumberEnd.value = 20
    newChannelConfigEntry.maxPower.value = "RAIL_TX_POWER_MAX"

  def _convertRmToRegisterList(self, rm, register_dict):
    regs = []
    for registerName in register_dict.keys():
      (block, register) = registerName.split(".")
      if block not in EXCLUDE_BLOCK_LIST:
        if register_dict[registerName] != "":
          # This is a workaround for write only registers
          regs.append(self._regOutput(block, register, register_dict[registerName]))
        else:
          value = eval(".".join(["rm", block, register, "io"]))
          regs.append(self._regOutput(block, register, value))

    # Check for duplicates since we won't know what to do with them
    regs = sorted(regs)

    prevAddr = None
    prevValue = None
    for reg in regs:
      if prevAddr != None and reg[0] == prevAddr:
        if prevValue != reg[1]:
          # raise Exception("Duplicate non-identical set of register 0x%.8x!" % reg[0])
          print("Error: Conflicting definition for register 0x%.8x" % (reg[0]))
        else:
          print("Warning: Duplicate definition of register 0x%.8x" % (reg[0]))
      prevAddr = reg[0]
      prevValue = reg[1]

    return regs

  # -------- External ---------------------------------------------------------
  def setInstanceDict(self, instanceDict):
    self.instanceDict = instanceDict

  def populateModel(self):
    # Empty output strings

    #Check if we have an instanceDict
    if self.instanceDict == None:
      print("No instanceDict configured. Please call the setInstanceDict method.")

    # In case populateModel gets called multiple times, start the internal
    # railModel object from scratch
    if self.railModel or self._railModelPopulated:
      self.railModel = RAILModel(self.yamlobject)
      self._railModelPopulated = False

    # Parsing all config files for a customer
    for phy in sorted(self.instanceDict):

      # Remove "PHY_"
      configName = phy[4:]
      model = self.instanceDict[phy]
      self.partFamily = model.part_family.lower()

      if self.rc_version is None:
        self.rc_version = model.calc_version

      # Create a proper rm object depending on partFamily
      rm_factory = RM_Factory(self.partFamily.upper())
      self.rm = rm_factory()

      # Create a register dictionary
      registerDict = {}

      # Write model instance to RM device
      # rm and registerDict are both modified here
      self._writeModelToRmDevice(model, self.rm, registerDict)

      # Start off by creating a new instance of phyConfigEntry, use the phy name
      newPhyConfigEntry = self.railModel.phyConfigEntries.addNewElement(configName)

      #Handle Frame Type Configurations
      self._generateFrameTypeStructures(newPhyConfigEntry, model)

      #Handle IR Cal Settings
      self._generateIrCalStructure(newPhyConfigEntry, model)

      self._generatePhyInfoStructure(newPhyConfigEntry, model)

      # self._generateFrameCoding(model)
      codingArray = model.profile.outputs.get_output('frame_coding_array_packed').var_value

      newPhyConfigEntry.frameCodingTable.values = codingArray

      # Handle Dynamic Slicer Configuration (for OOK PHYs)
      self._generateDynamicSlicerConfiguration(newPhyConfigEntry, model)

      #Handle Channel Lists
      self._generateChannelStructures(newPhyConfigEntry, model)

      # Generic Model Info
      newPhyConfigEntry.xtalFrequency.value = model.vars.xtal_frequency.value
      newPhyConfigEntry.baseFrequency.value = model.vars.base_frequency.value
      newPhyConfigEntry.bitrate.value = model.vars.bitrate.value
      newPhyConfigEntry.modType.value = model.vars.modulation_type.value
      newPhyConfigEntry.deviation.value = model.vars.deviation.value
      newPhyConfigEntry.synthResolution.value = model.vars.synth_res_actual.value
      newPhyConfigEntry.fecEnabled.value = bool(model.profile.outputs.get_output('fec_enabled').var_value)
      newPhyConfigEntry.convDecodeBufferSize.value = model.profile.outputs.get_output('frc_conv_decoder_buffer_size').var_value

      # Check fecEnabled flag and convDecodeBufferSize are correctly configured
      if newPhyConfigEntry.fecEnabled.value:
        assert newPhyConfigEntry.convDecodeBufferSize.value > 0, "Incorrect configuration for FEC Enabled"

      # On Panther, we decided to ALWAYS write the FRC_CONVRAMADDR to HIGH RAM,
      # without instantiating the Conv Decoder Buffer
      if self.partFamily.lower() in ["panther", "lynx", "leopard"]:
        # This "hack" prevents the template from rendering the instantiation of
        # the convDecodeBuffer
        newPhyConfigEntry.convDecodeBufferSize.value = 0

      regs = self._convertRmToRegisterList(self.rm, registerDict)

      regs = self._generateModemConfigEntries(newPhyConfigEntry, model, regs)

    self.formatModemConfigEntries(newPhyConfigEntry, regs)

    # We can now mark the _railModelPopulated flag as True
    self._railModelPopulated = True
