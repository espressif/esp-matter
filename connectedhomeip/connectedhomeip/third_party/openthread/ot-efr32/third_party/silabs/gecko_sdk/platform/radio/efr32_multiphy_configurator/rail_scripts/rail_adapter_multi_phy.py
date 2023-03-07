from functools import cmp_to_key

from pyradioconfig.calculator_model_framework.Utils.CalcStatus import CalcStatus
from rail_scripts.rail_adapter import *
from rail_scripts._version import __version__

from rail_scripts import config

from enum import IntEnum

import itertools

# Update kRAILVersion to be used in phyInfoData.
kRAILVersion = 13

class ConcPhyEnum(IntEnum):
  CONC_PHY_NONE = 0
  CONC_PHY_BASE = 1
  CONC_PHY_VT = 2
  CONC_PHY_9_6_NON_HOP = 3

class ProtocolIDEnum(IntEnum):
  CUSTOM = 0
  EMBERPHY = 1
  THREAD = 2
  BLE = 3
  CONNECT = 4
  ZIGBEE = 5
  ZWAVE = 6
  WISUN = 7

class RAIL_OptArgInput(object):
  channel_timing_name = "channel_timing_name"
  conc_phy_opt_hop = "conc_phy_opt_hop"
  viterbidemod = "MODEM.VITERBIDEMOD"

class RAIL_ConcPhy:
  @staticmethod
  def RAIL_IsConcPhyVt(opt_arguments):
    for optional_argument in opt_arguments:
      if (optional_argument.key == RAIL_OptArgInput.conc_phy_opt_hop) and (int(optional_argument.value) == ConcPhyEnum.CONC_PHY_VT):
        # if virtual concurrent PHY
        return True

    # virtual concurrnet PHY not found
    return False

def parsePower(value):
  # Just use a big number for maxpower so that comparisons work even if future
  # chips have different max power defines
  if type(value) == str and value == "RAIL_TX_POWER_MAX":
    return 100000

  # Otherwise assume that we were given an int and do some double checking
  assert type(value) == int, "Power must be specified as an integer value"
  return value

class RAILAdapter_MultiPhy(RAILAdapter):

  def __init__(self, **kwargs):
    self.rc_version = kwargs["rc_version"] if "rc_version" in kwargs else None
    setattr(self, "version", __version__)
    self.mphyConfig = kwargs["mphyConfig"]
    regex = re.compile(r'rail_api_(\d+).x')
    # Captures RAIL major version into an int
    # We use "get", to avoid raising "KeyError"; default to "rail_api_2.x"
    adapter_name = kwargs.get("adapter_name", "rail_api_2.x")
    re_match = regex.match(adapter_name)
    if re_match:
      self.rail_version= int(re_match.group(1))
    else:
      raise Exception("Unknown RAIL Adapter name: {}".format(adapter_name))
    with open(os.path.join(RAILAdapter.current_dir,"rail_model_multi_phy.yml")) as f:
      self.yamlobject = None
      if hasattr(yaml, 'FullLoader'):
        self.yamlobject = yaml.load(f.read(), Loader=yaml.FullLoader)
      else:
        self.yamlobject = yaml.load(f.read())
    self.railModel = RAILModel(self.yamlobject)
    self._railModelPopulated = False

  def _encodeAction(self, modemConfig, address, length, values=[], names=[], debug_print=False):
    # Get address for protected field regs on the current chip
    PROTECTED_FIELD_REG_VALUES = {}
    for register in config.PROTECTED_FIELDS[self.partFamily.lower()].keys():
      registerObject = getattr(self.rm, register.split('.')[0], None)
      if registerObject == None:
        continue
      registerObject = getattr(registerObject, register.split('.')[1], None)
      if registerObject == None:
        continue
      base = registerObject.baseAddress
      offset = registerObject.addressOffset
      protectionMask = 0
      for field in config.PROTECTED_FIELDS[self.partFamily.lower()][register]:
        fieldObject = getattr(registerObject, field, None)
        if fieldObject == None:
          continue
        bitWidth = fieldObject.bitWidth
        bitOffset = fieldObject.bitOffset
        protectionMask |= ((2 ** bitWidth - 1) << bitOffset)
      PROTECTED_FIELD_REG_VALUES[base + offset] = { 'protectionMask' : protectionMask, 'name': register }

    while length > 0:
      currentLength = length
      currentValues = values
      currentNames = names
      currentAddress = address
      addressConflict = None
      for protectedAddress in PROTECTED_FIELD_REG_VALUES.keys():
        if (protectedAddress >= currentAddress) and (protectedAddress < (currentAddress + length * 4)):
          if (addressConflict == None) or (protectedAddress < addressConflict):
            addressConflict = protectedAddress

      if addressConflict != None:
        currentLength = int((addressConflict - address) / 4)
        currentValues = values[:currentLength]
        currentNames = names[:currentLength]

      if currentLength > 0:
        newModemConfigEntry = modemConfig.addNewElement()
        try:
          encodedAddress = self._encodeWriteAddress(currentAddress, currentLength)
        except:
          print("Unable to encode {} at address {}.  Check mapping!".format(register, currentAddress))
          exit(1)
        firstValue = currentValues[0]
        firstName = currentNames[0]
        newModemConfigEntry.encodedAction.value = encodedAddress
        newModemConfigEntry.encodedValues.value = [firstValue]
        newModemConfigEntry.encodedRegNames.value = ["Write: " + firstName]
        # Remember the 'end' in ARRAY[start:end] is EXclusive, while the start is INclusive
        for val in currentValues[1:]:
          newModemConfigEntry.encodedValues.value.append(val)

        for name in currentNames[1:]:
          newModemConfigEntry.encodedRegNames.value.append(name)

      if addressConflict != None:
        if debug_print:
          print("Conflict found on address %08x, trying to set to %08x." % (addressConflict, values[currentLength]))
          print("This is %s, with the following protected fields:" % PROTECTED_FIELD_REG_VALUES[addressConflict]['name'])
          print(config.PROTECTED_FIELDS[self.partFamily.lower()][PROTECTED_FIELD_REG_VALUES[addressConflict]['name']])
          print("Splitting to:")

        protectionMask = PROTECTED_FIELD_REG_VALUES[addressConflict]['protectionMask']
        # First clear out unprotected fields
        if ((values[currentLength] & ~protectionMask) != ~protectionMask) and (protectionMask != (2 ** 32 - 1)):
          encodedAddress = self._encodeWriteAddress(addressConflict, radio_action=config.RADIO_ACTION_AND)
          newModemConfigEntry = modemConfig.addNewElement()
          newModemConfigEntry.encodedAction.value = encodedAddress
          newModemConfigEntry.encodedValues.value = [protectionMask]
          newModemConfigEntry.encodedRegNames.value = ["AND: " + names[currentLength]]
          if debug_print:
            print("AND with %08x" % newModemConfigEntry.encodedValues.value[0])

        # We only need to OR things in if the value isn't 0
        if ((values[currentLength] & ~protectionMask) != 0) and (protectionMask != (2 ** 32 - 1)):
          encodedAddress = self._encodeWriteAddress(addressConflict, radio_action=config.RADIO_ACTION_OR)
          newModemConfigEntry = modemConfig.addNewElement()
          newModemConfigEntry.encodedAction.value = encodedAddress
          newModemConfigEntry.encodedValues.value = [values[currentLength] & ~protectionMask]
          newModemConfigEntry.encodedRegNames.value = ["OR: " + names[currentLength]]
          if debug_print:
            print("OR with %08x" % newModemConfigEntry.encodedValues.value[0])

      # Add one here, to account for the conflicted address. Keep in mind though, we don't
      # actually have to check if we found a conflicted address here though. If we didn't,
      # we're done forming this array, and so we'll fail the while check at the top of this
      # loop with or without checking for a conflicted address.
      length -= (currentLength + 1)
      values = values[currentLength + 1 :]
      address = currentAddress + (currentLength + 1) * 4
      names = names[currentLength + 1 :]

  def optimizeRadioConfig(self, base_info):
    def _optimizeWrite(base_streak, regs_channels, expensive, cheap):
      # Compute cost to move write in word units. The write costs 1 + len words.
      # This results in a net cost based on the number of places it's added vs.
      # the one place it's removed.
      net_cost_to_add = (len(regs_channels) - 1) * (1 + len(base_streak))
      # Now lower our cost. For every add that has a neighboring write on the
      # high or low side, we subtract one word due to continuous write synergy
      for add in regs_channels:
        above = False
        below = False
        for address, _, __ in add:
          above |= (address == base_streak[0][0] + 4 * len(base_streak))
          below |= (address == base_streak[0][0] - 4)
        net_cost_to_add -= 1 if above else 0
        net_cost_to_add -= 1 if below else 0
      # If the net cost is 0, it probably saves time due to write synergy
      if (net_cost_to_add <= 0):
        cheap.extend(base_streak)
      else:
        expensive.extend(base_streak)

    # This algorithm handles a single add poorly. Leave everything in the base
    if len(base_info["add"]) <= 1:
      return

    regs_channels = [x[0] for x in base_info["add"]]
    new_base = []
    add_to_entries = []
    # Chunk base into it's continuous writes, and see
    base_streak = [base_info["base"][0]]
    for registerAddress, registerValue, registerName in base_info["base"][1:]:
      if base_streak == [] or registerAddress == (base_streak[0][0] + 4 * len(base_streak)):
        base_streak.append((registerAddress, registerValue, registerName))
      else:
        _optimizeWrite(base_streak, regs_channels, new_base, add_to_entries)
        base_streak = [(registerAddress, registerValue, registerName)]
    _optimizeWrite(base_streak, regs_channels, new_base, add_to_entries)

    for add in regs_channels:
      add.extend(add_to_entries)
      add.sort()
    base_info["base"] = new_base

    # Move PHYINFO (and associated registers in a continuous write) to the start
    address = self._getRegAddress("SEQ","PHYINFO")
    allWrites = [new_base]
    allWrites.extend(regs_channels)
    for add in allWrites:
      current = address
      writeToMove = []
      for i, register in enumerate(add):
        if register[0] == current:
          writeToMove.append(add.pop(i))
          current += 4
      add[0:0] = writeToMove

    do_print = False # change to True to enable extra debug print
    # Make sure there is no duplicate address
    allWrites = [new_base]
    allWrites.extend(regs_channels)
    if (self.partFamily in ["ocelot", "margay"]):
      bcrdemoctrlReg = self.rm.MODEM.BCRDEMODCTRL
      viterbidemodReg = self.rm.MODEM.VITERBIDEMOD
    for j, chunkWrite in enumerate(allWrites):
      if (do_print):
        if (j == 0):
          print("Total number of register on 'base configuration' is {}".format(len(chunkWrite)))
        else:
          print("Total number of register on {} is {}".format(base_info["add"][j - 1][1][3].name, len(chunkWrite)))
        print("chunkWrite len {}".format(len(chunkWrite)))
      if (len(chunkWrite) == 0):
        if (do_print):
          print("chunkWrite is empty")
      else:
        if (do_print):
          print("chunkWrite[0]: {}".format(chunkWrite[0]))
        prevAddress = chunkWrite[0]
        for i, register in enumerate(chunkWrite):
          if ((self.partFamily in ["ocelot", "margay"]) and (j != 0) and ((base_info["add"][j - 1][1][1].entryType.value == ConcPhyEnum.CONC_PHY_VT) or (base_info["add"][j - 1][1][1].entryType.value == ConcPhyEnum.CONC_PHY_9_6_NON_HOP))):
            # apply calculator workaround for ZWave concurrent PHY
            # need to make sure  BCR demod is enable and viterbi demod is disabled for virtual concurrent PHY (i.e. Zwave 9.6K concurrent PHY)
            if (register[0] == self._getRegAddress("MODEM", "BCRDEMODCTRL")):
              # Enable BCR demod
              # reg[1] = ((reg[1] | MODEM_BCRDEMODCTRL_BCRDEMODEN) & ~MODEM_BCRDEMODCTRL_BBPMDETEN)
              bcrdemoctrlReg.io = register[1]
              bcrdemoctrlReg.BCRDEMODEN.io = 1
              #bcrdemoctrlReg.BBPMDETEN.io = 0 #Calculator should be calculating the right value now
              if (do_print):
                print("Found BCRDEMODCTRL Addr: {}, Value: {}".format(bcrdemoctrlReg.baseAddress + bcrdemoctrlReg.addressOffset, hex(bcrdemoctrlReg.io)))
              chunkWrite[i] = (register[0], bcrdemoctrlReg.io, "MODEM.BCRDEMODCTRL")
              if (do_print):
                print("New BCRDEMODCTRL {}".format(chunkWrite[i]))
            elif (register[0] == self._getRegAddress("MODEM", "VITERBIDEMOD")):
              # Disable Viterbi demod
              # reg[1] = (reg[1] & ~_MODEM_VITERBIDEMOD_VTDEMODEN_MASK)
              viterbidemodReg.io = register[1]
              viterbidemodReg.VTDEMODEN.io = 0
              if (do_print):
                print("Found VITERBIDEMOD Addr: {}, Value: {}".format(viterbidemodReg.baseAddress + viterbidemodReg.addressOffset, hex(viterbidemodReg.io)))
              chunkWrite[i] = (register[0], viterbidemodReg.io, "MODEM.VITERBIDEMOD")
              if (do_print):
                print("New VITERBIDEMOD {}".format(chunkWrite[i]))

          if (i != 0):
            if (prevAddress[0] == register[0]):
              print("Found duplicate address {} @ {}".format(register, i))
              if (prevAddress[1] != register[1]):
                print("Duplicate address value mismatch, {} != {}".format(prevAddress[1], register[1]))
                raise
          prevAddress = register

    if self.partFamily.lower() in ["ocelot", "margay"]:
      # Conc. PHY optimization
      # for virtual concurrent PHY (i.e. Zwave 9.6K concurrent PHY), this sequence has to be followed when writing the deltaAdd register set:
      #    --- MODEM_SRCCHF, MODEM_BCRDEMODCTRL, MODEM_VITERBIDEMOD has to be written first
      #    --- MODEM_TRECPMPATT has to be written last
      regNameLists = ["SRCCHF", "BCRDEMODCTRL", "VITERBIDEMOD", "CTRL0", "SYNC0", "FRMSCHTIME", "TRECPMPATT"]
      regLists = []
      for regNameList in regNameLists:
        regLists.append(self._getRegAddress("MODEM", regNameList))
      if (do_print):
        for regIdx, reg in enumerate(regLists):
          for add in allWrites:
            for i, register in enumerate(add):
              if register[0] == reg:
                print("Name: {}, Addr: {}, Value: {}, index {}".format(regNameLists[regIdx], register[0], register[1], i))
      regs_channels = [x[0] for x in base_info["add"]]
      # order of the first few register write
      firstWriteLists = ["SRCCHF", "BCRDEMODCTRL", "VITERBIDEMOD"]
      # order of the last few register write
      lastWriteLists = ["TRECPMPATT"]
      # the middle register write will be in ascending address as before (CTRL0, CTRL1, SYNC0, FRMSCHTIME, SQEXT, BCRDEMODCTRL)
      for add_channel in base_info["add"]:
        if (do_print):
          print("Optimizing conc. PHY: {}".format(add_channel[1][3].name))
          #multiPhyConfigEntry.optional_arguments.argument.conc_phy_opt_hop.var_value
        if (add_channel[1][1].entryType.value == ConcPhyEnum.CONC_PHY_VT):
          # if virtual concurrent PHY
          reg_lists = add_channel[0]
          firstWrites = []
          lastWrites = []
          # Find the registers that has to be executed first
          for list in firstWriteLists:
            list_addr = self._getRegAddress("MODEM", list)
            regFound = False
            for i, reg in enumerate(reg_lists):
              if reg[0] == list_addr:
                if (do_print):
                  print("Found Name: {}, Addr: {}, Value: {}".format(list, list_addr, hex(reg[1])))

                firstWrites.append(reg_lists.pop(i))
                regFound = True
            if (regFound != True):
              print("Error {} is not found".format(list))
              raise

          # Find the registers that has to be executed last
          for list in lastWriteLists:
            regFound = False
            list_addr = self._getRegAddress("MODEM", list)
            for i, reg in enumerate(reg_lists):
              if reg[0] == list_addr:
                if (do_print):
                  print("Found Name: {}, Addr: {}, Value: {}".format(list, list_addr, hex(reg[1])))
                lastWrites.append(reg_lists.pop(i))
                regFound = True
            if (regFound != True):
              print("Error: {} is not found".format(list))
              raise

          # Relocate the register update
          reg_lists[1:1] = firstWrites # move the first set of register @ the beginning of deltaAdd list
          for x in lastWrites:
            reg_lists.append(x) # move the last set of register @ the end of deltaAdd list

  def formatModemConfigEntries(self, configName, phyConfigEntry, registerEntries, base=False, subtract=False):
    if self.rail_version == 1:
      # RAIL 1.x requires an action for every write
      for registerAddress, registerValue in registerEntries:
        self._encodeAction(phyConfigEntry, registerAddress, 1, [registerValue])
    else:
      # RAIL 2.x supports packed radio configurations
      currentAddress = 0
      length = 0
      values = []
      names = []

      # Create a new modemConfig element, and grab appropriate references based on
      # whether this is a regular or subtract node.
      if base == True:
        newModemConfig = self.railModel.multiPhyConfig.commonStructures.modemConfigEntriesBase.newElement(configName + "_modemConfigBase")
        currentModemConfigs = self.railModel.multiPhyConfig.commonStructures.modemConfigEntriesBase
        currentPhyConfigEntryModemConfigEntry = phyConfigEntry.modemConfigEntryBase
      elif subtract == True:
        newModemConfig = self.railModel.multiPhyConfig.commonStructures.modemConfigEntriesSubtract.newElement(configName + "_modemConfigSubtract")
        currentModemConfigs = self.railModel.multiPhyConfig.commonStructures.modemConfigEntriesSubtract
        currentPhyConfigEntryModemConfigEntry = phyConfigEntry.modemConfigEntrySubtract
      else:
        newModemConfig = self.railModel.multiPhyConfig.commonStructures.modemConfigEntries.newElement(phyConfigEntry.name + "_modemConfig")
        currentModemConfigs = self.railModel.multiPhyConfig.commonStructures.modemConfigEntries
        currentPhyConfigEntryModemConfigEntry = phyConfigEntry.modemConfigEntry

      for registerAddress, registerValue, registerName in registerEntries:
        if (registerAddress - currentAddress) == (4 * length):
          length += 1
          values.append(registerValue)
          names.append(registerName)
        else:
          self._encodeAction(newModemConfig, currentAddress, length, values, names)
          currentAddress = registerAddress
          length = 1
          values = [registerValue]
          names = [registerName]
      # One final write for whatever is left
      self._encodeAction(newModemConfig, currentAddress, length, values, names)

      # Check the size of the newModemConfig is greater than 0 before adding it
      # to the currentModemConfigs structure
      if len(newModemConfig._elements) > 0:

        # Traverse existing modemConfigEntries and check for duplicates
        entryFound = False
        for modemConfig in currentModemConfigs._elements:
          if modemConfig == newModemConfig:
            # Register the entry with the current phyConfigEntry
            currentPhyConfigEntryModemConfigEntry.value = modemConfig
            entryFound = True
            break

        if not entryFound:
          # Found no duplicates, so add the newModemConfig to the phyInfoEntries object
          currentModemConfigs.addElement(newModemConfig)
          # Register the entry with the current phyConfigEntry
          currentPhyConfigEntryModemConfigEntry.value = newModemConfig

  def _generateModemConfigEntries(self, phyConfigEntry, model, regs):
    if len(regs) > 0:
      # Write the address of the phyInfo structure to SEQ.PHYINFO.ADDRESS
      address = self._getRegAddress("SEQ","PHYINFO")
      regs.append((address, phyConfigEntry.phyInfoEntry.value, "SEQ.PHYINFO"))

      # Write the address of the block decoding table to BLOCKRAMADDR when
      # BLOCKWHITEMODE == BLOCKLOOKUP. Include the register even when the table
      # is absent for speed with continuous write, and make the potential
      # future implementation of RAIL_LIB-1138 easier
      address = self._getRegAddress("FRC","BLOCKRAMADDR")
      regs.append((address, phyConfigEntry.frameCodingTableEntry.value or 0, "FRC.BLOCKRAMADDR"))

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
        if self.partFamily.lower() in ["sol"]:
          convDecodeBuffer = 0x6000
        else:
          convDecodeBuffer = 0x4000

      regs.append((address, convDecodeBuffer, "FRC.CONVRAMADDR"))

      # Write the address of the last Dynamic Slicer Configuration link
      # if there are any present, exclude the write if false
      dynamicSlicerTableEntry = phyConfigEntry.dynamicSlicerTableEntry.value
      if dynamicSlicerTableEntry and len(dynamicSlicerTableEntry._elements) > 0 and \
         self.partFamily in ["dumbo", "jumbo", "nerio", "nixi", "bobcat"]:
          address = self._getRegAddress("SEQ","DYNAMIC_CHPWR_TABLE")
          regs.append((address, phyConfigEntry.dynamicSlicerTableEntry.value.lastElement, "SEQ.DYNAMIC_CHPWR_TABLE"))

      regs.sort() # Put the registers in the right order again

    return regs

  # Takes a model instance and writes it to a rm object
  def _writeModelToRmDevice(self, isBaseConfig, baseChannelConfig, channelConfigEntry, phyConfigEntry, rm, debug_print=False):

    if isBaseConfig:

      if baseChannelConfig.base_channel_reference:
        # If "base_channel_reference" feature is being used, (it's not 'None')
        # we need to make sure the phy_config_base is indeed empty
        assert len(baseChannelConfig.phy_config_base) == 0, \
          "Length of baseChannelConfig.phy_config_base ({}) is not 0".format(len(baseChannelConfig.phy_config_base))
        try:
          # Attempt to get the phy_config_base from the base_channel_configuration
          # in mphyConfig that matches the name of base_channel_reference;
          # use the first element returned by the filter (index 0)
          baseChannelConfig.phy_config_base = \
          list(filter(lambda x: x.name == baseChannelConfig.base_channel_reference, \
                 self.mphyConfig.base_channel_configurations.base_channel_configuration))[0].phy_config_base
        except:
          raise Exception("Invalid configuration for base_channel_configuration '{}'".format(baseChannelConfig.name))

    registers_base = baseChannelConfig.phy_config_base
    registers_channel = channelConfigEntry.phy_config_delta_add
    registers_subtract = baseChannelConfig.phy_config_delta_subtract

    # Don't let the configurator dictate channel information to us. We use the
    # RAIL_ChannelConfig_t via SYNTH_Config to determine the below registers_channel,
    # so removing them saves space and potential headache.
    # Also, BLOCKRAMADDR and CONVRAMADDR should only be written by us (RAIL)
    # We use pop(X, None) to avoid raising KeyError in case the register is not
    # in the dictionary
    for register in config.PROTECTED_REGS:
      for register_set in [ registers_base, registers_channel ]:
        x = register_set.pop(register, None)
        if (x != None) and debug_print:
          print("Protected register found in register set, removing.")
          print(x)

    self.registers_base = registers_base
    self.registers_subtract = registers_subtract
    self.registers_channel = registers_channel

  def _generatePhyInfoStructure(self, phyConfigEntry, baseConfigOptions, channelConfigOptions, model):
    # Get a local reference to model.profile.outputs to use here
    outputs = model.profile.outputs

    # For antenna diversity, first grab the relevant outputs from the calculator
    antDivMode = outputs.get_output('div_antdivmode').var_value
    antDivRepeatDisable = outputs.get_output('div_antdivrepeatdis').var_value

    # Then, generate the register value than maps to the fields we care about
    antDivConfiguration = 0x0
    antDivConfiguration |= (antDivMode << self.rm.MODEM.CTRL3.ANTDIVMODE.bitOffset)
    antDivConfiguration |= (antDivRepeatDisable << self.rm.MODEM.CTRL3.ANTDIVREPEATDIS.bitOffset)

    # Create a new phyInfo element
    newPhyInfoEntry = self.railModel.multiPhyConfig.commonStructures.phyInfoEntries.newElement("phyInfo")
    data = newPhyInfoEntry.phyInfoData

    data.version.value = 0 if self.rail_version == 1 else kRAILVersion
    data.freqOffsetFactor.value = outputs.get_output('frequency_offset_factor').var_value
    data.freqOffsetFactor_fxp.value = int(outputs.get_output('frequency_offset_factor_fxp').var_value or 0)
    data.frameTypeConfig.value = phyConfigEntry.frameTypeEntry.value
    data.irCalConfig.value = phyConfigEntry.irCalConfigEntry.value
    data.timingConfig.value = phyConfigEntry.timingConfigEntry.value
    if phyConfigEntry.rffpllConfigEntry.value is not None:
      data.rffpllConfig.value = phyConfigEntry.rffpllConfigEntry.value
    if phyConfigEntry.dcdcRetimingConfigEntry.value is not None:
      data.dcdcRetimingConfig.value = phyConfigEntry.dcdcRetimingConfigEntry.value
    if self.partFamily.lower() not in ["dumbo", "jumbo", "nerio", "nixi", "panther"]:
      data.hfxoRetimingConfig.value = phyConfigEntry.hfxoRetimingTableEntry.value
    if phyConfigEntry.txIrCalConfigEntry.value is not None:
      data.txIrCalConfig.value = phyConfigEntry.txIrCalConfigEntry.value
    if getattr(outputs, 'rssi_adjust_db', None) != None:
      rssiAdjustDb = int(outputs.get_output('rssi_adjust_db').var_value)
    else:
      rssiAdjustDb = 0

    data.antDivRxAutoConfig.value = antDivConfiguration
    data.src1Denominator.value = int(outputs.get_output('src1_calcDenominator').var_value or 0)
    data.src2Denominator.value = int(outputs.get_output('src2_calcDenominator').var_value or 0)

    modType = model.vars.modulation_type.value
    if hasattr(model.vars.modulation_type.var_enum, 'OFDM') and modType == model.vars.modulation_type.var_enum.OFDM:
      # In OFDM txBaudRate contains the symbol rate
      data.txBaudRate.value = outputs.get_output('ofdm_symbol_rate').var_value
    else:
      data.txBaudRate.value = outputs.get_output('tx_baud_rate_actual').var_value

    data.rxBaudRate.value = model.vars.rx_baud_rate_actual.value #outputs.get_output('rx_baud_rate_actual').var_value
    data.baudPerSymbol.value = outputs.get_output('baud_per_symbol_actual').var_value
    data.bitsPerSymbol.value = outputs.get_output('bits_per_symbol_actual').var_value
    data.synthCache.value = int(model.vars.SYNTH_IFFREQ_IFFREQ.value or 0) \
                            | (model.vars.lodiv_actual.value << 25)
    data.zWaveChannelHopTiming.value = int(outputs.get_output('rx_ch_hopping_delay_usec').var_value or 0)
    data.rateInfo.value = (rssiAdjustDb & 0xFF) << 16 | data.baudPerSymbol.value << 8 | data.bitsPerSymbol.value

    if self.partFamily.lower() not in ["dumbo","jumbo","nerio","nixi"]:
      # Cap DEC0 at 3, since the decimation value for all values above 3 is 8.
      # Also don't use value 2, in case that's useful in the future
      DEC0_MAP = [0, 1, 1, 3, 3, 3, 3, 3]
      cache40 = (model.vars.SYNTH_IFFREQ_LOSIDE.value << 20)            \
                | (DEC0_MAP[int(model.vars.MODEM_CF_DEC0.value or 0)] << 22)
      if ("MODEM_SRCCHF_BWSEL" in model.vars) and (model.vars.MODEM_SRCCHF_BWSEL.value is not None):
        cache40 |= (int(model.vars.MODEM_SRCCHF_BWSEL.value >= 2) << 21)
      if "RAC_IFADCTRIM0_IFADCENHALFMODE" in model.vars:
        cache40 |= model.vars.RAC_IFADCTRIM0_IFADCENHALFMODE.value << 24
      data.synthCache.value |= cache40

    am_low_ramplev = getattr(outputs, 'am_low_ramplev', None)
    if am_low_ramplev is None:
      data.paParams.value = 0
    else:
      data.paParams.value = am_low_ramplev.var_value

    psm_max_sleep_us = getattr(outputs, 'psm_max_sleep_us', None)
    if psm_max_sleep_us is None:
      data.psmMaxSleepUs.value = 0
    else:
      data.psmMaxSleepUs.value = psm_max_sleep_us.var_value

    # Traverse existing irCalConfigEntries and check for duplicates
    entryFound = False
    for phyInfoEntry in self.railModel.multiPhyConfig.commonStructures.phyInfoEntries._elements:
      if phyInfoEntry == newPhyInfoEntry:
        # Register the entry with the current phyConfigEntry
        phyConfigEntry.phyInfoEntry.value = phyInfoEntry
        entryFound = True
        break

    if not entryFound:
      # Found no duplicates, so add the newPhyInfoEntry to the phyInfoEntries object
      self.railModel.multiPhyConfig.commonStructures.phyInfoEntries.addElement(newPhyInfoEntry)
      # Register the entry with the current phyConfigEntry
      phyConfigEntry.phyInfoEntry.value = newPhyInfoEntry

  def _generateDynamicSlicerConfiguration(self, phyConfigEntry, baseConfigOptions, model):
    # Check the feature is enabled in the model output
    if model.profile.outputs.get_output('dynamic_slicer_enabled').var_value == True:
      # Get the two arrays that must be defined
      dynamicSlicerThresholdValues = model.profile.outputs.get_output('dynamic_slicer_threshold_values').var_value
      dynamicSlicerLevelValues = model.profile.outputs.get_output('dynamic_slicer_level_values').var_value

      # Create a new dynamicSlicerTableEntry object where to add the actual configs
      dynamicSlicerTableEntries = self.railModel.multiPhyConfig.commonStructures.dynamicSlicerTableEntries
      dynSlicerTableEntriesLen = len(dynamicSlicerTableEntries._elements)
      newDynamicSlicerTableEntryName = baseConfigOptions.get("dynamic_slicer_name", phyConfigEntry.name) \
                                       + "_dynamicSlicerTable" + str(dynSlicerTableEntriesLen)
      newDynamicSlicerTableEntry = dynamicSlicerTableEntries.newElement(newDynamicSlicerTableEntryName)

      # Use dynamicSlicerLevelValues to traverse the loop since
      # it has one more item than dynamic_slicer_threshold_values
      for i, level in enumerate(dynamicSlicerLevelValues):

        # We need to define the links in reverse order because they need to
        # reference the previous one (linked list), by doing this we avoid the
        # need for forward declarations in the C code.
        dynSlicerConfigNum = len(dynamicSlicerLevelValues)-i-1
        newDynamicSlicerConfigName = newDynamicSlicerTableEntryName+"Config"+str(dynSlicerConfigNum)

        # The threshold is stored using 2's complement in the LSB, thus the short mask (0xFF)
        # The value is stored in a uint16_t to optimize packing.
        threshold = (dynamicSlicerThresholdValues[-1] if i == 0 else dynamicSlicerThresholdValues[-i]) & 0xFF

        # The level is stored as a uint16_t, properly aligned to make the sequencer
        # code more efficient; thus the shiftValue-16.
        level = (dynamicSlicerLevelValues[-i-1] << self.rm.MODEM.CTRL1.FREQOFFESTLIM.bitOffset-16) & 0xFFFF

        # Create a new Dynamic Slicer Config
        newDynamicSlicerConfig = newDynamicSlicerTableEntry.addNewElement(newDynamicSlicerConfigName)
        newDynamicSlicerConfig.threshold.value = threshold
        newDynamicSlicerConfig.level.value = level
        newDynamicSlicerConfig.next.value = None if i == 0 else newDynamicSlicerTableEntry._elements[i-1]

      # Traverse the existing Dynamic Slicer Table Entries, and check this is not
      # a duplicate (same set of dynamicSlicerConfig entries)
      entryFound = False
      for i, dynamicSlicerTableEntry in enumerate(dynamicSlicerTableEntries._elements):
        if dynamicSlicerTableEntry._elements == newDynamicSlicerTableEntry._elements:
          # Register the entry with the current phyConfigEntry
          phyConfigEntry.dynamicSlicerTableEntry.value = dynamicSlicerTableEntry
          entryFound = True
          break

      if not entryFound:
        # Add newDynamicSlicerTableEntry entry in common structures
        dynamicSlicerTableEntries.addElement(newDynamicSlicerTableEntry)
        # Register the new entry with the current phyConfigEntry
        phyConfigEntry.dynamicSlicerTableEntry.value = newDynamicSlicerTableEntry

  # For a particular PHY get the frame config
  def _generateFrameTypeStructures(self, phyConfigEntry, model):

    newFrameLength = model.vars.frame_type_lengths.value
    if newFrameLength:
      # Traverse existing frameLengthEntries and check for duplicates
      entryFound = False
      commonStructures = self.railModel.multiPhyConfig.commonStructures
      for i, frameLengthEntry in enumerate(commonStructures.frameLengthEntries._elements):
        if frameLengthEntry.values == newFrameLength:
          # Register the entry with the current phyConfigEntry
          phyConfigEntry.frameLengthEntry.value = frameLengthEntry
          entryFound = True
          break
      if not entryFound:
        # Create a new frameLength entry in common structures
        newFrameLengthEntry = commonStructures.frameLengthEntries.addNewElement("frameLengthList")
        newFrameLengthEntry.values = newFrameLength
        # Register the new entries with the current phyConfigEntry
        phyConfigEntry.frameLengthEntry.value = newFrameLengthEntry

      # Create new frameTypeConfig entry
      newFrameTypeEntry = commonStructures.frameTypeEntries.newElement("frameTypeConfig")
      newFrameTypeEntry.offset.value = model.vars.frame_type_loc.value
      newFrameTypeEntry.mask.value = model.vars.frame_type_mask.value
      newFrameTypeEntry.frameLen.value = phyConfigEntry.frameLengthEntry
      newFrameTypeEntry.isValid.value = model.vars.frame_type_valid.value
      newFrameTypeEntry.addressFilter.value = model.vars.frame_type_filter.value
      newFrameTypeEntry.variableAddrLoc.value = False

      # Traverse existing frameTypeEntries and check for duplicates
      entryFound = False
      for i, frameTypeEntry in enumerate(commonStructures.frameTypeEntries._elements):
        if frameTypeEntry == newFrameTypeEntry:
          # Register the entry with the current phyConfigEntry
          phyConfigEntry.frameTypeEntry.value = frameTypeEntry
          entryFound = True
          break
      if not entryFound:
        # Create a new frameType entry in common structures
        commonStructures.frameTypeEntries.addElement(newFrameTypeEntry)
        # Register the new entry with the current phyConfigEntry
        phyConfigEntry.frameTypeEntry.value = newFrameTypeEntry

  def _generateIrCalStructure(self, phyConfigEntry, model, isBaseConfig, baseConfigAttr):
    # Get a local reference to model.profile.outputs to use here
    outputs = model.profile.outputs

    legacyIrConfig = True
    if (self.partFamily.lower() in ["ocelot", "sol", "margay"]):
      # For the subG chip,
      # if there is a 2.4Ghz PHY being built, it should use the same ircal coefficient
      # as all the 2.4GHz PHYs should get the coefficient from DEVINFO
      if (model.vars.lodiv_actual.value != 1):
        legacyIrConfig = False
      agcMangainReg = self.rm.AGC.MANGAIN
      agcMangainReg.io = 0
      agcMangainReg.MANGAINEN.io = 1
      agcMangainReg.MANGAINIFPGA.io = 4
      agcMangainReg.MANGAINLNA.io = 1
      agcMangainReg.MANGAINPN.io = 1

      if (self.partFamily.lower() in ["sol"]):
        modemIrcalReg = self.rm.FEFILT1.IRCAL if self.rm.SEQ.MODEMINFO.SOFTMODEM_DEMOD_EN else self.rm.FEFILT0.IRCAL
        modemIrcalReg.io = 0
        modemIrcalReg.MURSHF.io = 28
        modemIrcalReg.MUISHF.io = 38
      else:
        modemIrcalReg = self.rm.MODEM.IRCAL
        modemIrcalReg.io = 0
        modemIrcalReg.MURSHF.io = 24
        modemIrcalReg.MUISHF.io = 34

      modemIrcalReg.IRCALEN.io = 1

      newIrCalConfig = [
        agcMangainReg.io & 0xFF,
        (agcMangainReg.io >> 8) & 0xFF,
        (agcMangainReg.io >> 16) & 0xFF,
        (agcMangainReg.io >> 24) & 0xFF,
        modemIrcalReg.io & 0xFF,
        (modemIrcalReg.io >> 8) & 0xFF,
        (modemIrcalReg.io >> 16) & 0xFF,
        (modemIrcalReg.io >> 24) & 0xFF,
        False, #pteConfigIsValid
        True, #pllLoopbackConfigIsValid
        False, #paLoopbackConfigIsValid
        2, #bestConfig = IRCAL_TYPE_PLL_LOOPBACK
        100 & 0xFF,  #delayMsResult, 100ms
        (100 >> 8) & 0xFF,
        1, #divx
        1, #divr
        47, #divn
        0, #bw
        0, #freq
        7, #slice
      ]
    else:
      newIrCalConfig = [
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

    # Traverse existing irCalConfigEntries and check for duplicates
    entryFound = False
    commonStructures = self.railModel.multiPhyConfig.commonStructures

    if legacyIrConfig is False:
      if (isBaseConfig):
        # for non-2.4Ghz, Create a new RAIL_ChannelConfigEntryAttr_t entry in
        # common structures, so each PHY has its own ircal coefficient
        newRailChannelConfigEntryAttr = commonStructures.railChannelConfigEntryAttrEntries.addNewElement("channelConfigEntryAttr")
        newRailChannelConfigEntryAttr.calValues.value = 2
        phyConfigEntry.channelConfigEntryAttr.value = newRailChannelConfigEntryAttr
      else:
        phyConfigEntry.channelConfigEntryAttr.value = baseConfigAttr

    for i, irCalConfigEntry in enumerate(commonStructures.irCalConfigEntries._elements):
      if irCalConfigEntry.values == newIrCalConfig:
        # Register the entry with the current phyConfigEntry
        phyConfigEntry.irCalConfigEntry.value = irCalConfigEntry
        if legacyIrConfig is True:
          phyConfigEntry.channelConfigEntryAttr.value = commonStructures.railChannelConfigEntryAttrEntries._elements[i]
        #print("irCalConfigEntry {}".format(irCalConfigEntry.name))
        entryFound = True
        break

    if not entryFound:
      # Create a new irCalConfig entry in common structures
      newIrCalConfigEntry = commonStructures.irCalConfigEntries.addNewElement("irCalConfig")
      newIrCalConfigEntry.values = newIrCalConfig
      if legacyIrConfig is True:
        # Create a new RAIL_ChannelConfigEntryAttr_t entry in common structures
        newRailChannelConfigEntryAttr = commonStructures.railChannelConfigEntryAttrEntries.addNewElement("channelConfigEntryAttr")
        if (self.partFamily.lower() in ["dumbo","jumbo","nerio","nixi", "lynx", "leopard"]):
          newRailChannelConfigEntryAttr.calValues.value = 1
        else:
          newRailChannelConfigEntryAttr.calValues.value = 2 #panther has 2 RF paths
        phyConfigEntry.channelConfigEntryAttr.value = newRailChannelConfigEntryAttr
      # Register the new entries with the current phyConfigEntry
      phyConfigEntry.irCalConfigEntry.value = newIrCalConfigEntry

    return phyConfigEntry.channelConfigEntryAttr.value

  def _generateTxIrCalStructure(self, phyConfigEntry, model):
    outputs = model.profile.outputs

    newtxIrCalConfig = None
    if (hasattr(outputs, 'softmodem_txircal_params')):
      softmodem_txircal_params = outputs.get_output('softmodem_txircal_params').var_value
      softmodem_txircal_freq = outputs.get_output('softmodem_txircal_freq').var_value

      if softmodem_txircal_params is not None:
        # Create a list containing all info
        newtxIrCalConfig = [softmodem_txircal_freq]
        newtxIrCalConfig.extend(softmodem_txircal_params)

    commonStructures = self.railModel.multiPhyConfig.commonStructures

    newTxIrCalBand = False
    # Check for duplicates in the commonStructures containing all phy configurations
    for i, txIrCalConfigEntry in enumerate(commonStructures.txIrCalConfigEntries._elements):
      # if parameters changed, then another array must be created
      if txIrCalConfigEntry.values != newtxIrCalConfig:
        newTxIrCalBand = True

      phyConfigEntry.txIrCalConfigEntry.value = txIrCalConfigEntry

    if newtxIrCalConfig is not None:
      # If there is no txIRCAL config yet, then create one
      if phyConfigEntry.txIrCalConfigEntry.value == None or newTxIrCalBand:
        newTxIrCalConfigEntry = commonStructures.txIrCalConfigEntries.addNewElement("txIrCalConfig")
        newTxIrCalConfigEntry.values = newtxIrCalConfig
        phyConfigEntry.txIrCalConfigEntry.value = newTxIrCalConfigEntry

  def _generateStackInfo(self, phyConfigEntry, model):
    outputs = model.profile.outputs

    if (hasattr(outputs, 'stack_info')):
      stack_info = outputs.get_output('stack_info').var_value
      # Only reference non default values
      if stack_info != [0, 0]:
        newStackInfo = stack_info
      else:
        newStackInfo = None
        phyConfigEntry.stackInfo.value = None

      if newStackInfo is not None:
        commonStructures = self.railModel.multiPhyConfig.commonStructures

        # Create a new config in commonStructures by default
        newStackInfoConfig = True
        # Check for duplicates in the commonStructures containing all phy configurations
        for i, stackInfoConfigEntry in enumerate(commonStructures.stackInfoEntries._elements):
          phyConfigEntry.stackInfo.value = stackInfoConfigEntry

          if stackInfoConfigEntry.values == newStackInfo:
            # Duplicate found: no new config will be added to commonStructures
            newStackInfoConfig = False
            break

        if phyConfigEntry.stackInfo.value == None or newStackInfoConfig:
          length = 0

          # Get commonStructures length for name uniqueness purposes
          if commonStructures.stackInfoEntries.lastElement is not None:
            length = len(commonStructures.stackInfoEntries._elements)

          # Create a new config in commonStructures
          newStackInfoConfigEntry = commonStructures.stackInfoEntries.addNewElement("stackInfo_%s" % length)
          newStackInfoConfigEntry.values = newStackInfo
          # Store that config in the linked phyConfigEntry
          phyConfigEntry.stackInfo.value = newStackInfoConfigEntry


  def _generateDcdcRetimingStructure(self, phyConfigEntry, model):
    if (self.partFamily in ["ocelot", "margay"]):
      # Get a local reference to model.profile.outputs to use here
      outputs = model.profile.outputs

      if (outputs.get_output('ppnd_0').var_value is not None) and (outputs.get_output('ppnd_0').var_value != 0):
        newDcdcRetimingConfig = [
          outputs.get_output('ppnd_0').var_value,
          outputs.get_output('ppnd_1').var_value,
          outputs.get_output('ppnd_2').var_value,
          outputs.get_output('ppnd_3').var_value
        ]

        # Traverse existing dcdcRetimingConfigEntries and check for duplicates
        entryFound = False
        commonStructures = self.railModel.multiPhyConfig.commonStructures

        for i, dcdcRetimingConfigEntry in enumerate(commonStructures.dcdcRetimingConfigEntries._elements):
          if dcdcRetimingConfigEntry.values == newDcdcRetimingConfig:
            # Register the entry with the current phyConfigEntry
            phyConfigEntry.dcdcRetimingConfigEntry.value = dcdcRetimingConfigEntry
            entryFound = True
            break

        if not entryFound:
          # Create a new dcdcRetimingConfig entry in common structures
          newDcdcRetimingConfigEntry = commonStructures.dcdcRetimingConfigEntries.addNewElement("dcdcRetimingConfig")
          newDcdcRetimingConfigEntry.values = newDcdcRetimingConfig
          # Register the new entries with the current phyConfigEntry
          phyConfigEntry.dcdcRetimingConfigEntry.value = newDcdcRetimingConfigEntry

  def _generateTimingStructure(self, phyConfigEntry, model):
    # Get a local reference to model.profile.outputs to use here
    outputs = model.profile.outputs

    if outputs.get_output('rx_sync_delay_ns').var_value is not None:
      if self.partFamily in ["jumbo", "nerio"]:
        newTimingConfig = [
          outputs.get_output('rx_sync_delay_ns').var_value,
          outputs.get_output('tx_eof_delay_ns').var_value
        ]
      else:
        newTimingConfig = [
          outputs.get_output('rx_sync_delay_ns').var_value,
          outputs.get_output('rx_eof_delay_ns').var_value,
          outputs.get_output('tx_eof_delay_ns').var_value
        ]

      # Traverse existing dcdcRetimingConfigEntries and check for duplicates
      entryFound = False
      commonStructures = self.railModel.multiPhyConfig.commonStructures

      for i, timingConfigEntry in enumerate(commonStructures.timingConfigEntries._elements):
        if timingConfigEntry.values == newTimingConfig:
          # Register the entry with the current phyConfigEntry
          phyConfigEntry.timingConfigEntry.value = timingConfigEntry
          entryFound = True
          break

      if not entryFound:
        # Create a new timingConfig entry in common structures
        newTimingConfigEntry = commonStructures.timingConfigEntries.addNewElement("timingConfig")
        newTimingConfigEntry.values = newTimingConfig
        # Register the new entries with the current phyConfigEntry
        phyConfigEntry.timingConfigEntry.value = newTimingConfigEntry

  def _generateHfxoRetimingStructure(self, phyConfigEntry, model):
    if self.partFamily.lower() not in ["dumbo", "jumbo", "nerio", "nixi", "panther"]:
      commonStructures = self.railModel.multiPhyConfig.commonStructures
      newHfxoRetimingConfigEntry = commonStructures.hfxoRetimingTableEntries.newElement("hfxoRetimingConfigEntries")

      # Get the arrays
      hfxoRetimingFreqValues = model.vars.lut_freq.value
      hfxoRetimingFreqUpperValues = model.vars.lut_freq_upper.value
      hfxoRetimingValidValues = model.vars.lut_valid.value
      hfxoRetimingSmuxDivValues = model.vars.lut_smuxdiv.value
      hfxoRetimingLimitLValues = model.vars.lut_limitl.value
      hfxoRetimingLimitHValues = model.vars.lut_limith.value
      hfxoRetimingDpllValues = model.vars.lut_dpll_freq_hz.value
      hfxoRetimingTblIdxValues = model.vars.lut_table_index.value
      hfxoRetimingIdxStart = 0
      byteOffset = 4 + (8 * len(hfxoRetimingTblIdxValues))

      # Populate the rest of the table
      for tbl_idx in range(len(hfxoRetimingTblIdxValues)):
        # insert lowest range
        newHfxoRetimingConfigStruct = newHfxoRetimingConfigEntry.hfxoRetimingBandConfigEntries.addNewElement("hfxoRetimingBandConfigStruct")
        newHfxoRetimingConfigStruct.loFreqUpperRange.value = hfxoRetimingFreqValues[hfxoRetimingIdxStart]
        newHfxoRetimingConfigStruct.valid.value = 0
        newHfxoRetimingConfigStruct.sMuxDiv.value = 0
        newHfxoRetimingConfigStruct.hfxoLimitL.value = 0
        newHfxoRetimingConfigStruct.hfxoLimitH.value = 0

        for i in range(hfxoRetimingTblIdxValues[tbl_idx]):
          newHfxoRetimingConfigStruct = newHfxoRetimingConfigEntry.hfxoRetimingBandConfigEntries.addNewElement("hfxoRetimingBandConfigStruct")
          newHfxoRetimingConfigStruct.loFreqUpperRange.value = hfxoRetimingFreqUpperValues[hfxoRetimingIdxStart + i]
          newHfxoRetimingConfigStruct.valid.value = hfxoRetimingValidValues[hfxoRetimingIdxStart + i]
          if (hfxoRetimingSmuxDivValues[hfxoRetimingIdxStart + i] > 0):
            newHfxoRetimingConfigStruct.sMuxDiv.value = hfxoRetimingSmuxDivValues[hfxoRetimingIdxStart + i]
          else:
            newHfxoRetimingConfigStruct.sMuxDiv.value = 0
          if (hfxoRetimingLimitLValues[hfxoRetimingIdxStart + i] > 0):
            newHfxoRetimingConfigStruct.hfxoLimitL.value = hfxoRetimingLimitLValues[hfxoRetimingIdxStart + i]
          else:
            newHfxoRetimingConfigStruct.hfxoLimitL.value = 0
          if (newHfxoRetimingConfigStruct.hfxoLimitL.value > 7):
            newHfxoRetimingConfigStruct.valid.value = 0
          if (hfxoRetimingLimitHValues[hfxoRetimingIdxStart + i] > 0):
            newHfxoRetimingConfigStruct.hfxoLimitH.value = hfxoRetimingLimitHValues[hfxoRetimingIdxStart + i]
          else:
            newHfxoRetimingConfigStruct.hfxoLimitH.value = 0
          if (newHfxoRetimingConfigStruct.hfxoLimitH.value > 7):
            newHfxoRetimingConfigStruct.valid.value = 0

        hfxoRetimingIdxStart = hfxoRetimingIdxStart + hfxoRetimingTblIdxValues[tbl_idx]
        newHfxoRetimingBandConfigStruct = newHfxoRetimingConfigEntry.hfxoRetimingInfoConfigEntries.addNewElement("hfxoRetimingBandConfigStruct")
        newHfxoRetimingBandConfigStruct.numBand.value = hfxoRetimingTblIdxValues[tbl_idx] + 1
        newHfxoRetimingBandConfigStruct.dpllClock.value = hfxoRetimingDpllValues[tbl_idx]
        newHfxoRetimingBandConfigStruct.offset.value = byteOffset
        byteOffset = byteOffset + (6 * (hfxoRetimingTblIdxValues[tbl_idx] + 1))

      # Traverse existing hfxoRetimingConfigEntries and check for duplicates
      entryFound = False
      for i, hfxoRetimingConfigEntry in enumerate(commonStructures.hfxoRetimingTableEntries._elements):
        if hfxoRetimingConfigEntry == newHfxoRetimingConfigEntry:
          # Register the entry with the current phyConfigEntry
          phyConfigEntry.hfxoRetimingTableEntry.value = hfxoRetimingConfigEntry
          entryFound = True
          break

      if not entryFound:
        # Create a new newHfxoRetimingConfig entry in common structures
        commonStructures.hfxoRetimingTableEntries.addElement(newHfxoRetimingConfigEntry)
        # Register the new entries with the current phyConfigEntry
        phyConfigEntry.hfxoRetimingTableEntry.value = newHfxoRetimingConfigEntry

  def _generateRffpllStructure(self, phyConfigEntry, model):
    # Get a local reference to model.profile.outputs to use here
    outputs = model.profile.outputs

    if (hasattr(outputs, 'fpll_div_array')):
      fpll_div_array = outputs.get_output('fpll_div_array').var_value
      fpll_divx = fpll_div_array[0]
      fpll_divy = fpll_div_array[1]
      fpll_divn = fpll_div_array[2]

      newRffpllConfig = [
        fpll_divx | (fpll_divy << 8) | (fpll_divn << 16),
        int(outputs.get_output('fpll_divx_freq').var_value),
        int(outputs.get_output('fpll_divy_freq').var_value),
      ]

      # Traverse existing rffpllConfigEntries, check for duplicates, and ensure
      # all configs are identical.
      commonStructures = self.railModel.multiPhyConfig.commonStructures
      for i, rffpllConfigEntry in enumerate(commonStructures.rffpllConfigEntries._elements):
        assert rffpllConfigEntry.values == newRffpllConfig, "Multiple RFFPLL configurations not supported"

        # Register the entry with the current phyConfigEntry
        phyConfigEntry.rffpllConfigEntry.value = rffpllConfigEntry

      # Create a new rffpllConfig entry in common structures
      if phyConfigEntry.rffpllConfigEntry.value == None:
        newRffpllConfigEntry = commonStructures.rffpllConfigEntries.addNewElement("rffpllConfig")
        newRffpllConfigEntry.values = newRffpllConfig
        phyConfigEntry.rffpllConfigEntry.value = newRffpllConfigEntry

  def _generateFrameCodingTable(self, phyConfigEntry, model):

    codingArray = model.profile.outputs.get_output('frame_coding_array_packed').var_value
    if codingArray:
      # Traverse existing frameCodingTableEntries and check for duplicates
      entryFound = False
      commonStructures = self.railModel.multiPhyConfig.commonStructures

      for i, frameCodingTableEntry in enumerate(commonStructures.frameCodingTableEntries._elements):
        if frameCodingTableEntry.values == codingArray:
          # Register the entry with the current phyConfigEntry
          phyConfigEntry.frameCodingTableEntry.value = frameCodingTableEntry
          entryFound = True
          break

      if not entryFound:
        # Create a new frameCodingTable entry in common structures
        newframeCodingTableEntry = commonStructures.frameCodingTableEntries.addNewElement("frameCodingTable")
        newframeCodingTableEntry.values = codingArray
        # Register the new entry with the current phyConfigEntry
        phyConfigEntry.frameCodingTableEntry.value = newframeCodingTableEntry

  def _generateChannelStructures(self, multiPhyConfigEntry, phyConfigEntry, channelConfigEntry):

    # Create a new Channel Config Entry
    newChannelConfigEntry = multiPhyConfigEntry.channelConfigEntries.newElement()
    newChannelConfigEntry.modemConfigDeltaAdd.value = phyConfigEntry.modemConfigEntry.value if len(channelConfigEntry.phy_config_delta_add) > 0 else None
    newChannelConfigEntry.baseFrequency.value = channelConfigEntry.base_frequency
    newChannelConfigEntry.channelSpacing.value = channelConfigEntry.channel_spacing
    newChannelConfigEntry.physicalChannelOffset.value = channelConfigEntry.physical_channel_offset
    newChannelConfigEntry.channelNumberStart.value = channelConfigEntry.channel_number_start
    newChannelConfigEntry.channelNumberEnd.value = channelConfigEntry.channel_number_end
    newChannelConfigEntry.maxPower.value = channelConfigEntry.max_power #"RAIL_TX_POWER_MAX"
    newChannelConfigEntry.attr.value = phyConfigEntry.channelConfigEntryAttr.value
    newChannelConfigEntry.entryType.value = phyConfigEntry.entryType.value
    newChannelConfigEntry.stackInfo.value = phyConfigEntry.stackInfo.value #Not serialized in multiphy XML

    # Traverse existing channelConfigEntries and check for duplicates
    entryFound = False
    for channelConfigEntry in multiPhyConfigEntry.channelConfigEntries._elements:
      if channelConfigEntry == newChannelConfigEntry:
        entryFound = True
        break

    if not entryFound:
      # Found no duplicates, so add the newChannelConfigEntry to the channelConfigEntries object
      multiPhyConfigEntry.channelConfigEntries.addElement(newChannelConfigEntry)

  def _generateChannelConfigs(self, railModel):

    for multiPhyConfigEntry in railModel.multiPhyConfig.multiPhyConfigEntries._elements:
      # Populate the channelConfig object
      channelConfig = multiPhyConfigEntry.channelConfig
      channelConfig.modemConfigBase.value = multiPhyConfigEntry.phyConfigEntries._elements[0].modemConfigEntryBase.value
      channelConfig.modemConfigDeltaSubtract.value = multiPhyConfigEntry.phyConfigEntries._elements[0].modemConfigEntrySubtract.value
      channelConfig.channelConfigEntries.value = multiPhyConfigEntry.channelConfigEntries
      channelConfig.length.value = len(multiPhyConfigEntry.channelConfigEntries._elements)
      channelConfig.signature.value = 0

  def _orderChannelConfigEntries(self, railModel):

    # The algorithm to order the channels is as follows:

    # Sort all channel entries using the channelNumberStart first, by maxPower
    # second, and by channelNumberEnd last.

    # First step, sort the channelConfigEntries, define a custom compare function
    def compareChannelConfigEntries(a, b):
      if a.channelNumberStart.value < b.channelNumberStart.value:
        return -1
      elif a.channelNumberStart.value > b.channelNumberStart.value:
        return 1
      else:
        # Now look at maxPower
        if parsePower(a.maxPower.value) < parsePower(b.maxPower.value):
          return -1
        elif parsePower(a.maxPower.value) > parsePower(b.maxPower.value):
          return 1
        else:
          if a.channelNumberEnd.value < b.channelNumberEnd.value:
              return -1
          elif a.channelNumberEnd.value > b.channelNumberEnd.value:
              return 1
          else:
            return 0

    for multiPhyConfigEntry in railModel.multiPhyConfig.multiPhyConfigEntries._elements:

      # Now, do the actual sort, using the compare function defined above
      channelConfigEntries = multiPhyConfigEntry.channelConfigEntries._elements
      channelConfigEntriesSorted = sorted(channelConfigEntries, key=cmp_to_key(compareChannelConfigEntries))

      # Next, set the reference entry to the first element of the newly sorted
      # array, and call this the reference channel config entry. Iterate through
      # the remaining channel entries and check those that have maxPower <= than
      # the current reference; if the channelNumberEnd is <= to the one in the
      # reference, AND the the channelNumberStart is > the one in the reference
      # entry, move the current channel entry to the position currently occupied
      # by the reference config entry. When an item is moved, start over the loop
      # with the moved item now becoming the reference channel config entry.
      # If we traverse all the remaining items, and no changes in position occur,
      # we need to move the reference to the next channel config entry.

      # We need a variable to kick us out of this loop
      changed = False
      reference = 0
      while reference < len(channelConfigEntriesSorted):

        # Grab the maxPower for the current reference entry
        referenceChannelEntry = channelConfigEntriesSorted[reference]

        for idx, channelConfigEntry in enumerate(channelConfigEntriesSorted[reference+1:]):
          # Set this variable to False to kick us out in case nothing gets updated
          changed = False
          if parsePower(channelConfigEntry.maxPower.value) <= parsePower(referenceChannelEntry.maxPower.value):
            if channelConfigEntry.channelNumberEnd.value <= referenceChannelEntry.channelNumberEnd.value and \
              channelConfigEntry.channelNumberStart.value > referenceChannelEntry.channelNumberStart.value:
              itemToMove = channelConfigEntriesSorted.pop(idx + reference+1)
              channelConfigEntriesSorted.insert(reference, itemToMove)
              changed = True
              break
        if not changed:
          reference += 1

      # Now that all the entries are correctly sorted, replace the elements in the
      # multiPhyConfigEntry.channelConfigEntries._elements list
      multiPhyConfigEntry.channelConfigEntries._elements = channelConfigEntriesSorted

  def _resolveConvDecoderBuffer(self, railModel):

    maxConvDecodeBufferSize = 0
    # On Panther, we decided to ALWAYS write the FRC_CONVRAMADDR to HIGH RAM offset 0
    # On Lynx, we decided to ALWAYS write the FRC_CONVRAMADDR to FRCRAM offset 0
    if self.partFamily.lower() not in ["panther", "lynx", "leopard"]:
      for multiPhyConfigEntry in railModel.multiPhyConfig.multiPhyConfigEntries._elements:
        for phyConfigEntry in multiPhyConfigEntry.phyConfigEntries._elements:
          if phyConfigEntry.convDecodeBufferSize.value > maxConvDecodeBufferSize:
            maxConvDecodeBufferSize = phyConfigEntry.convDecodeBufferSize.value

    railModel.multiPhyConfig.commonStructures.convDecodeBufferSize.value = maxConvDecodeBufferSize


  def _convertRmToRegisterList(self, registers):
    regs = []
    if registers is not None:
      for registerName in registers.keys():
        (block, register) = registerName.split(".")
        if block not in EXCLUDE_BLOCK_LIST:
          # registers[registerName].access == 'read-only'
          # if registers[registerName] != "":
          #   # This is a workaround for write only registers
          #   regs.append(self._regOutput(block, register, registers[registerName]))
          # else:
            eval_string = "registers[registerName].io"
            value = eval(eval_string)
            regs.append(self._regOutput(block, register, value, registerName))

      # Check for duplicates since we won't know what to do with them
      regs = sorted(regs)

      prevAddr = None
      prevValue = None
      for reg in regs:
        if prevAddr != None and reg[0] == prevAddr:
          if prevValue != reg[1]:
            # raise Exception("Duplicate non-identical set of register 0x%.8x!" % reg[0])
            print("Error: Conflicting definition for register at 0x%.8x" % reg[0])
          else:
            print("Warning: Duplicate definition of register at 0x%.8x" % reg[0])
        prevAddr = reg[0]
        prevValue = reg[1]

    return regs

  def _genModeSwitchPhrs(self, radioConfigModel, phyConfigEntry):
    if radioConfigModel.part_family.lower() in ['sol'] and hasattr(radioConfigModel.profile.outputs, 'wisun_phy_mode_id'):
      wisun_phy_mode_ids = getattr(radioConfigModel.profile.outputs, 'wisun_phy_mode_id', None)
      wisun_mode_switch_phrs = getattr(radioConfigModel.profile.outputs, 'wisun_mode_switch_phr', None)
      mode_switch_dict = self.railModel.multiPhyConfig.commonStructures.modeSwitchPhyModeIds.value

      # Make a copy of mode switch variables
      phyModeIdsIter = copy.deepcopy(wisun_phy_mode_ids)
      phrIter = copy.deepcopy(wisun_mode_switch_phrs)

      if wisun_phy_mode_ids is not None and wisun_mode_switch_phrs is not None:
        # Verify that these mode switch infos are not already saved in mode_switch_dict
        for index, phyModeId in enumerate(wisun_phy_mode_ids.var_value):
          if phyModeId in mode_switch_dict.keys():
            # If phyModeId is already in the array: it's a duplicate, then remove it and the PHR from lists
            phyModeIdsIter.var_value.remove(phyModeId)
            phrIter.var_value.remove(mode_switch_dict[phyModeId])
          else:
            # It's a new element: save it and its associated PHR
            mode_switch_dict[phyModeId] = wisun_mode_switch_phrs.var_value[index]

        # Verify that modified list are not empty
        if phyModeIdsIter.var_value and phrIter.var_value:
          # Extract elements left from the modified list: those are new elements
          phyConfigEntry.modeSwitchPhr.value = zip(phyModeIdsIter.var_value, phrIter.var_value)

        # Update dict
        self.railModel.multiPhyConfig.commonStructures.modeSwitchPhyModeIds.value = mode_switch_dict
      else:
        phyConfigEntry.modeSwitchPhr.value = None

  # -------- External ---------------------------------------------------------
  def setInstanceDict(self, mphyConfig):
    self.mphyConfig = mphyConfig

  def setSignatures(self, signatures):
    mPhyEntries = self.railModel.multiPhyConfig.multiPhyConfigEntries._elements
    for sig, multiPhyConfigEntry in zip(signatures, mPhyEntries):
      multiPhyConfigEntry.channelConfig.signature.value = sig

  def populateModel(self):

    #Check if we have an mphyConfig
    if self.mphyConfig == None:
      print("No mphyConfig configured. Please call the setInstanceDict method.")

    # In case populateModel gets called multiple times, start the internal
    # railModel object from scratch
    if self.railModel or self._railModelPopulated:
      self.railModel = RAILModel(self.yamlobject)
      self._railModelPopulated = False

    self.partFamily = self.mphyConfig.part_family

    # Create a proper rm object depending on partFamily
    rm_factory = RM_Factory(self.partFamily.upper())
    self.rm = rm_factory()

    # Create a structure that will be added to on a per-base channel reference basis.
    # We use this to move writes between base and add to optimize keeping
    # continuous writes as a block
    radio_configs = OrderedDict()

    # Create a dict that will be used to avoid duplicates in wisun_modeSwitchPhrs struct
    self.railModel.multiPhyConfig.commonStructures.modeSwitchPhyModeIds.value = OrderedDict()

    # Go through all base channel configurations (NOTE!, ask Rick to rename!)
    baseConfigAttr = 0
    for baseChannelConfig in self.mphyConfig.base_channel_configurations.base_channel_configuration:

      configName = baseChannelConfig.name

      baseConfigOptions = {}
      # Extract the optional arguments for this baseChannelConfig
      for argument in baseChannelConfig.optional_arguments.argument:
        baseConfigOptions[argument.key] = argument.value

      # Start off by creating a new instance of multiPhyConfigEntry, use the baseChannelConfig name
      multiPhyConfigEntry = self.railModel.multiPhyConfig.multiPhyConfigEntries.addNewElement(configName)
      # multiPhyConfigEntry.signature.signature.value = 0

      # Now, iterate through all the channel configs and mark the "base" (right now, it's always the first entry)
      for index, channelConfigEntry in enumerate(baseChannelConfig.channel_config_entries.channel_config_entry):
        isBaseConfig = index == 0

        channelConfigOptions = {}
        # Extract the optional arguments for this channelConfigEntry
        for argument in channelConfigEntry.optional_arguments.argument:
          channelConfigOptions[argument.key] = argument.value

        # Create a new phy configEntry config
        phyConfigEntry = multiPhyConfigEntry.phyConfigEntries.addNewElement(channelConfigEntry.name)

        radioConfigModel = channelConfigEntry.radio_configurator_output_model

        # Guard against PHYs which may not have a name or a profile. The
        # template omits a define for falsy values (None or "")
        if radioConfigModel and hasattr(radioConfigModel, "phy") \
           and hasattr(radioConfigModel.phy, "name"):
          multiPhyConfigEntry.phyName.value = radioConfigModel.phy.name
        else:
          multiPhyConfigEntry.phyName.value = ""
        if radioConfigModel and hasattr(radioConfigModel, "profile") \
           and hasattr(radioConfigModel.profile, "name"):
          multiPhyConfigEntry.profile.value = radioConfigModel.profile.name
        else:
          multiPhyConfigEntry.profile.value = ""

        if radioConfigModel.result_code != CalcStatus.Failure.value:
          if self.rc_version is None:
            self.rc_version = radioConfigModel.calc_version

          # Write model instance to RM device
          self._writeModelToRmDevice(isBaseConfig, baseChannelConfig, channelConfigEntry, phyConfigEntry, self.rm)

          #Handle Frame Type Configurations
          self._generateFrameTypeStructures(phyConfigEntry, radioConfigModel)

          #Handle IR Cal Settings
          baseConfigAttr = self._generateIrCalStructure(phyConfigEntry, radioConfigModel, isBaseConfig, baseConfigAttr)

          #Handle DCDC Retiming Settings
          self._generateDcdcRetimingStructure(phyConfigEntry, radioConfigModel)

          #Handle timing structures
          self._generateTimingStructure(phyConfigEntry, radioConfigModel)

          #Handle HFXO Retiming Settings
          self._generateHfxoRetimingStructure(phyConfigEntry, radioConfigModel)

          #Handle RFFPLL Settings
          self._generateRffpllStructure(phyConfigEntry, radioConfigModel)

          # Handle TX IR Cal Settings
          self._generateTxIrCalStructure(phyConfigEntry, radioConfigModel)

          self._generatePhyInfoStructure(phyConfigEntry, baseConfigOptions, channelConfigOptions, radioConfigModel)

          #Handle Frame Coding tables
          self._generateFrameCodingTable(phyConfigEntry, radioConfigModel)

          # Handle Dynamic Slicer Configuration (for OOK PHYs)
          self._generateDynamicSlicerConfiguration(phyConfigEntry, baseConfigOptions, radioConfigModel)

          # Generic Model Info
          phyConfigEntry.xtalFrequency.value = radioConfigModel.vars.xtal_frequency.value
          phyConfigEntry.baseFrequency.value = radioConfigModel.vars.base_frequency.value
          phyConfigEntry.bitrate.value = radioConfigModel.vars.bitrate.value
          phyConfigEntry.modType.value = radioConfigModel.vars.modulation_type.value
          phyConfigEntry.deviation.value = radioConfigModel.vars.deviation.value
          phyConfigEntry.synthResolution.value = radioConfigModel.vars.synth_res_actual.value
          phyConfigEntry.fecEnabled.value = bool(radioConfigModel.profile.outputs.get_output('fec_enabled').var_value)
          phyConfigEntry.convDecodeBufferSize.value = radioConfigModel.profile.outputs.get_output('frc_conv_decoder_buffer_size').var_value

          self._generateStackInfo(phyConfigEntry, radioConfigModel)

          # Populate modeSwitchPhyModeIds
          self._genModeSwitchPhrs(radioConfigModel, phyConfigEntry)

          # Extract the concurrent PHY optional argument marker
          phyConfigEntry.entryType.value = 0 # initialize as non-concurrent PHY
          for optional_argument in baseChannelConfig.optional_arguments.argument:
            if (optional_argument.key == 'conc_phy_opt_hop'):
              phyConfigEntry.entryType.value = int(optional_argument.value)
              break

          # Check fecEnabled flag and convDecodeBufferSize are correctly configured
          if phyConfigEntry.fecEnabled.value:
            assert phyConfigEntry.convDecodeBufferSize.value > 0, "Incorrect configuration for FEC Enabled"

          regs_channel = self._convertRmToRegisterList(self.registers_channel)
          regs_base = self._convertRmToRegisterList(self.registers_base)
          regs_subtract = self._convertRmToRegisterList(self.registers_subtract)

          regs_channel = self._generateModemConfigEntries(phyConfigEntry, radioConfigModel, regs_channel)
          # NOTE! Special case: If regs_channel is empty, and the base is not, it means we need to include
          # the registers normally in the channel specific modemConfig in the base
          if not regs_channel:
            regs_base = self._generateModemConfigEntries(phyConfigEntry, radioConfigModel, regs_base)
          regs_subtract = self._generateModemConfigEntries(phyConfigEntry, radioConfigModel, regs_subtract)

          # Package metadata in a struct for unpacking after optimization
          meta = (configName, phyConfigEntry, multiPhyConfigEntry, channelConfigEntry)
          reference = baseChannelConfig.base_channel_reference
          reference = configName if reference is None else reference

          if not reference in radio_configs:
            radio_configs[reference] = {
              "base": regs_base,
              "subtract": regs_subtract,
              "add": [(regs_channel, meta)],
            }
          else:
            radio_configs[reference]["add"].append((regs_channel, meta))
        else:
          self._railModelPopulated = False
          print('Radio configurator had a failure, exiting rail scripts.')
          return

    # Optimize and write radio configs
    for _, radio_config in radio_configs.items():
      self.optimizeRadioConfig(radio_config)

      regs_base = radio_config["base"]
      regs_subtract = radio_config["subtract"]

      for regs_channel, meta in radio_config["add"]:
        configName = meta[0]
        phyConfigEntry = meta[1]
        multiPhyConfigEntry = meta[2]
        channelConfigEntry = meta[3]

        self.formatModemConfigEntries(configName, phyConfigEntry, regs_base, True)
        self.formatModemConfigEntries(configName, phyConfigEntry, regs_channel)
        self.formatModemConfigEntries(configName, phyConfigEntry, regs_subtract, False, True)

        #Handle Channel Lists
        self._generateChannelStructures(multiPhyConfigEntry, phyConfigEntry, channelConfigEntry)

    # Populate the channelConfigs objects
    self._generateChannelConfigs(self.railModel)

    # Sort all the channel config entries in the model
    self._orderChannelConfigEntries(self.railModel)

    # Resolve convDecoderBuffer
    self._resolveConvDecoderBuffer(self.railModel)

    # We can now mark the _railModelPopulated flag as True
    self._railModelPopulated = True
