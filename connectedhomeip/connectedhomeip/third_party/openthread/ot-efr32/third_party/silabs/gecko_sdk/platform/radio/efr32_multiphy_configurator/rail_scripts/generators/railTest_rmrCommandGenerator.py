#!/usr/bin/python
#

import os
import re
import numbers
from rail_scripts._version import __version__
from  rail_scripts.rail_adapter import RAILAdapter
from rail_scripts.rail_adapter_single_phy import RAILAdapter_SinglePhy
from rail_scripts.rail_adapter_multi_phy import RAILAdapter_MultiPhy
from collections import OrderedDict
import itertools
import jinja2 as jinja2  # Refactored jinja2  naming  to isolate jinja2 inside Studio's hacked up Jython, which causes issues when multiple Jinja2 environments are used
import ast

import sys
if sys.version_info[0] > 2:
  xrange = range
  basestring = str
  itertools.izip = zip


class RailTest_rmrConfigGenerator:
  # Constant enumeration that matches railtest
  RMR_STRUCT_PHY_INFO = 0
  RMR_STRUCT_IRCAL_CONFIG = 1
  RMR_STRUCT_MODEM_CONFIG = 2
  RMR_STRUCT_FRAME_TYPE_CONFIG = 3
  RMR_STRUCT_FRAME_LENGTH_LIST = 4
  RMR_STRUCT_FRAME_CODING_TABLE = 5
  RMR_STRUCT_CHANNEL_CONFIG_ATTRIBUTES = 6
  RMR_STRUCT_CHANNEL_CONFIG_ENTRY = 7
  RMR_STRUCT_CONV_DECODE_BUFFER = 8
  RMR_STRUCT_DCDC_RETIMING_CONFIG = 9
  RMR_STRUCT_HFXO_RETIMING_CONFIG = 10
  RMR_STRUCT_RFFPLL_CONFIG = 11
  RMR_STRUCT_NULL = 255

  _RAIL_INTERNAL_CONSTANTS = {"RAIL_TX_POWER_MAX": [255, 127]}

  _RMR_STRUCTS = {'phyInfo': RMR_STRUCT_PHY_INFO,
                  'irCalConfig': RMR_STRUCT_IRCAL_CONFIG,
                  'modemConfig': RMR_STRUCT_MODEM_CONFIG,
                  'frameTypeConfig': RMR_STRUCT_FRAME_TYPE_CONFIG,
                  'frameLengthList': RMR_STRUCT_FRAME_LENGTH_LIST,
                  'frameCodingTable': RMR_STRUCT_FRAME_CODING_TABLE,
                  'channelConfigAttributes': RMR_STRUCT_CHANNEL_CONFIG_ATTRIBUTES,
                  'channelConfigEntry': RMR_STRUCT_CHANNEL_CONFIG_ENTRY,
                  'convDecodeBuffer': RMR_STRUCT_CONV_DECODE_BUFFER,
                  'dcdcRetimingConfig': RMR_STRUCT_DCDC_RETIMING_CONFIG,
                  'hfxoRetimingConfig': RMR_STRUCT_HFXO_RETIMING_CONFIG,
                  'rffpllConfig': RMR_STRUCT_RFFPLL_CONFIG,
                  'null': RMR_STRUCT_NULL}

  _TEMPLATE_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'templates')
  _SUPPORTED_MODEM_CONFIG_REFERENCES = {'phyInfo': RMR_STRUCT_PHY_INFO,
                                        'convDecodeBuffer': RMR_STRUCT_CONV_DECODE_BUFFER,
                                        'frameCodingTable': RMR_STRUCT_FRAME_CODING_TABLE,}

  """
  Class takes an input file and parses it into header file content
  """
  # ------------------------------------------------------------------------
  def __init__(self, railAdapter):
    setattr(self, "version", __version__)
    self.rail_version = railAdapter.rail_version
    self.rc_version = railAdapter.rc_version
    # Instantiate jinja environment and register template path with FileSystemLoader
    self.jinja_env = jinja2.Environment(loader=jinja2.FileSystemLoader(RailTest_rmrConfigGenerator._TEMPLATE_PATH or './'))

    # Register custom filters with jinja
    self.jinja_env.filters['formatModemConfigEntries'] = self.formatModemConfigEntries
    self.jinja_env.filters['formatFrameTypeConfigLengths'] = self.formatFrameTypeConfigLengths
    self.jinja_env.filters['formatFrameCodingTable'] = self.formatFrameCodingTable
    self.jinja_env.filters['uint32ToBytes'] = self.uint32ToBytes
    self.jinja_env.filters['uint16ToBytes'] = self.uint16ToBytes
    self.jinja_env.filters['listToInt'] = self.listToInt
    self.jinja_env.filters['baudsBits'] = self.baudsBits
    self.jinja_env.filters['unrollAppendIndexLength'] = self.unrollAppendIndexLength
    # self.jinja_env.filters['unrollAppendIndexLength'] = self.unrollAppendIndexLength
    self.jinja_env.globals['firstDictValue'] = firstDictValue
    self.jinja_env.globals['len'] = len
    self.singlePhy = isinstance(railAdapter, RAILAdapter_SinglePhy)
    if self.singlePhy:
      self.template_path_railtest = "railtest_rmr.j2"
    else:
      self.template_path_railtest = "railtest_multiphy_rmr.j2"

    # We need the railAdapter object to be populated before we can generate the
    # context. If the railAdapter object is not populated by the caller, we can
    # try to populate it here, but if the inputs have not been set properly, it
    # may still fail.
    if not railAdapter.modelPopulated:
      railAdapter.populateModel()

    self.context = railAdapter.generateRailModelContext()
    self.context['filename'] = "railTestRmr.txt"
    self.context['title'] = "Radio Config"
    self.context['rail_version'] = self.rail_version
    self.context['rc_version'] = self.rc_version
    self.context['ra_version'] = self.version
    self.context['rmr_structs'] = self._RMR_STRUCTS
    if isinstance(railAdapter, RAILAdapter_SinglePhy):
      phyName = railAdapter.railModel.phyConfigEntries._elements[0]._uniqueName
      self.context['modemConfigRefs'] = self.getModemConfigReferences(self.context['phyConfigEntries'][phyName]['modemConfigEntries'])
    else:
      # It's easier to get the configuration entry we care about in here rather than trying to sort it in the jinja template.
      firstModemConfigEntryBaseName = railAdapter.railModel.multiPhyConfig.multiPhyConfigEntries._elements[0].phyConfigEntries._elements[0].modemConfigEntryBase.value._uniqueName
      firstChannelConfigEntry = railAdapter.railModel.multiPhyConfig.multiPhyConfigEntries._elements[0].channelConfigEntries._elements[0]
      if not firstChannelConfigEntry.modemConfigDeltaAdd.value:
        modemConfigEntries = self.context['multiPhyConfig']['commonStructures']['modemConfigEntriesBase'][firstModemConfigEntryBaseName]
      else:
        firstModemConfigEntriesList = list(self.context['multiPhyConfig']['commonStructures']['modemConfigEntriesBase'][firstModemConfigEntryBaseName].items())
        firstModemConfigDeltaAddName = firstChannelConfigEntry.modemConfigDeltaAdd.value.name
        firstModemConfigDeltaAddEntriesList = list(self.context['multiPhyConfig']['commonStructures']['modemConfigEntries'][firstModemConfigDeltaAddName].items())
        modemConfigEntries = OrderedDict(firstModemConfigDeltaAddEntriesList + firstModemConfigEntriesList)
      self.context['modemConfigEntries'] = modemConfigEntries
      self.context['modemConfigRefs'] = self.getModemConfigReferences(modemConfigEntries)

  # -------- Internal ---------------------------------------------------------
  # Recipe taken from: https://docs.python.org/2/library/itertools.html
  # Takes an iterable (such as a list) and returns data into fixed-length chunks
  # or blocks. Slightly modified so that it only returns n-sized blocks.
  # grouper('ABCDEFG', 3) --> ABC DEF
  def grouper(self, iterable, n):
    args = [iter(iterable)] * n
    return itertools.izip(*args)

  # Helper function for the writeRmrStructure command. Takes in a list and
  # adds the index where the unrollLength sized chunk must be written.
  # unrollAppendIndexLength([A, B, C, D, E, F, G, H, I, J], 4, 0) -->
  # [0, 4, A, B, C, D,
  #  4, 4, E, F, G, H,
  #  8, 2, I, J]
  #
  # If the items in the list are integers, each item will be converted into
  # wordWidth/8 items of width wordWidth, starting with the LSB. This is a way
  # to represent, for example, 32-bit integers as 4 8-bit values.
  # For example, unrollAppendIndexLength([A, B, C], 8, 0, 32) -->
  # [0, 8, (A>>0) & 0xFF, (A>>8) & 0xFF, (A>>16) & 0xFF, (A>>24) & 0xFF, (B>>0) & 0xFF, (B>>8) & 0xFF, (B>>16) & 0xFF, (B>>24) & 0xFF,
  #  8, 4, (C>>0) & 0xFF, (C>>8) & 0xFF, (C>>16) & 0xFF, (C>>24) & 0xFF,
  #
  # Intended to be called with batch(unrollLength + 2)
  def unrollAppendIndexLength(self, items, unrollLength, startingIndex, wordWidth = 8):
    assert wordWidth == 8 or wordWidth == 16 or wordWidth == 32, \
      "Error! The word width of each item in items must be 8, 16, or 32"

    if wordWidth == 16:
      newItems = []
      for item in items:
        newItems.extend(self.uint16ToBytes(item))
      items = newItems
      startingIndex *= 2
    elif wordWidth == 32:
      newItems = []
      for item in items:
        newItems.extend(self.uint32ToBytes(item))
      items = newItems
      startingIndex *= 4

    listToReturn = []
    groups = self.grouper(items, unrollLength)
    currentIndex = startingIndex
    for group in groups:
      listToReturn.append(currentIndex)
      listToReturn.append(len(group))
      listToReturn.extend(group)
      currentIndex += len(group)
    remainingItems = len(items) % unrollLength
    if (remainingItems) != 0:
      listToReturn.append(currentIndex)
      listToReturn.append(remainingItems)
      listToReturn.extend(items[-remainingItems:])
    return listToReturn

  # Dumb function to avoid arithmetic in jinja
  def baudsBits(self, baudsPerSymbol, bitsPerSymbol):
    return ((baudsPerSymbol << 8) | bitsPerSymbol)

  def getModemConfigReferences(self, modemConfigEntries):
    # Let's check for references in the modemConfigEntries
    modemConfigRef = []
    modemConfigArray = self.flattenModemConfigEntries(modemConfigEntries)
    for item in modemConfigArray:
      if not isinstance(item, numbers.Number):
        item_name = item if type(item) is str else item.name
        if item_name in self._SUPPORTED_MODEM_CONFIG_REFERENCES.keys():
          modemConfigRef.append([self.RMR_STRUCT_MODEM_CONFIG,
                                modemConfigArray.index(item),
                                self._SUPPORTED_MODEM_CONFIG_REFERENCES[item_name]])
    return modemConfigRef

  def uint32ToBytes(self, word):
    assert isinstance(word, numbers.Number), "Error! Non-number argument received"
    byte_list = []
    for i in xrange(4):
      byte_list.append(word%256)
      word /= 256
      word = int(word)
    return byte_list

  def uint16ToBytes(self, word):
    if isinstance(word, numbers.Number):
      return [word % 256, int(word / 256)]
    elif word in self._RAIL_INTERNAL_CONSTANTS.keys():
      # Return RAIL_TX_POWER_MAX, 0x7FFF. Least significant byte first.
      return self._RAIL_INTERNAL_CONSTANTS[word]
    elif isinstance(word, basestring):
      word_eval = ast.literal_eval(word)  # Handle string values, that need to be converted to numbers
      return self.uint16ToBytes(word_eval)
    else:
      assert False, "Undefined RAIL constant received: " + word

  def formatFrameTypeConfigLengths(self, frameConfigLengths):
    byte_list = []
    for frameConfigLength in frameConfigLengths:
      byte_list.extend(self.uint16ToBytes(frameConfigLength))
    return byte_list

  def formatFrameCodingTable(self, frameCodingEntries):
    byte_list = []
    for item in frameCodingEntries:
      byte_list.extend(self.uint32ToBytes(item))
    return byte_list

  def flattenModemConfigEntries(self, modemConfigEntries):
    modemConfigArray = []
    for modemConfigEntryName, modemConfigEntry in modemConfigEntries.items():
      modemConfigArray.append(modemConfigEntry['encodedAction'])
      modemConfigArray.extend(modemConfigEntry['encodedValues'])
    return modemConfigArray

  def formatModemConfigEntries(self, modemConfigEntries):
    modemConfigArrayBytes = []
    modemConfigArray = self.flattenModemConfigEntries(modemConfigEntries)
    # Add the stop command at the end
    modemConfigArray.append(0xFFFFFFFF)
    for item in modemConfigArray:
      if isinstance(item, numbers.Number):
        modemConfigArrayBytes.extend(self.uint32ToBytes(int(item)))
      else:
        # This is a reference. Write 0. Let the printed commands update
        # the pointer to the reference later on
        modemConfigArrayBytes.extend(self.uint32ToBytes(0))
    return modemConfigArrayBytes

  def listToInt(self, inputList):
    encodedList = 0
    if not isinstance(inputList, list):
      inputList = inputList.values
    for (i, valid) in enumerate(inputList):
      if valid:
        encodedList |= 1 << i
    return encodedList

  # Render function to encapsulate jinja
  def render(self, tpl_path, context=None):
    path, filename = os.path.split(tpl_path)
    if context is None:
      context = self.context
    return self.jinja_env.get_template(filename).render(context)


def firstDictValue(ordered_dictionary):
  for key, value in ordered_dictionary.items():
    return value
  return None