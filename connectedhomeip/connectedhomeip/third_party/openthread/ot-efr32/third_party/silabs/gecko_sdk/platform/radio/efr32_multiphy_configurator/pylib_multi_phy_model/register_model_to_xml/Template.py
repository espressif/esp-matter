#!/usr/bin/env python

#
# Generated Wed Mar 21 14:49:56 2018 by generateDS.py version 2.12d.
#
# Command line options:
#   ('-o', '..\\Bindings.py')
#   ('--super', 'Bindings')
#   ('-s', '..\\Template.py')
#   ('--subclass-suffix', '')
#   ('--member-specs', 'list')
#   ('-m', '')
#   ('-f', '')
#   ('--silence', '')
#
# Command line arguments:
#   .\CMSIS-SVD_Schema_1_0.xsd
#
# Command line:
#   generateDS_custom.py -o "..\Bindings.py" --super="Bindings" -s "..\Template.py" --subclass-suffix --member-specs="list" -m -f --silence .\CMSIS-SVD_Schema_1_0.xsd
#
# Current working directory (os.getcwd()):
#   xsd
#

import sys

import Bindings as supermod

etree_ = None
Verbose_import_ = False
(
    XMLParser_import_none, XMLParser_import_lxml,
    XMLParser_import_elementtree
) = range(3)
XMLParser_import_library = None
try:
    # lxml
    from lxml import etree as etree_
    XMLParser_import_library = XMLParser_import_lxml
    if Verbose_import_:
        print("running with lxml.etree")
except ImportError:
    try:
        # cElementTree from Python 2.5+
        import xml.etree.cElementTree as etree_
        XMLParser_import_library = XMLParser_import_elementtree
        if Verbose_import_:
            print("running with cElementTree on Python 2.5+")
    except ImportError:
        try:
            # ElementTree from Python 2.5+
            import xml.etree.ElementTree as etree_
            XMLParser_import_library = XMLParser_import_elementtree
            if Verbose_import_:
                print("running with ElementTree on Python 2.5+")
        except ImportError:
            try:
                # normal cElementTree install
                import cElementTree as etree_
                XMLParser_import_library = XMLParser_import_elementtree
                if Verbose_import_:
                    print("running with cElementTree")
            except ImportError:
                try:
                    # normal ElementTree install
                    import elementtree.ElementTree as etree_
                    XMLParser_import_library = XMLParser_import_elementtree
                    if Verbose_import_:
                        print("running with ElementTree")
                except ImportError:
                    raise ImportError(
                        "Failed to import ElementTree from any known place")


def parsexml_(*args, **kwargs):
    if (XMLParser_import_library == XMLParser_import_lxml and
            'parser' not in kwargs):
        # Use the lxml ElementTree compatible parser so that, e.g.,
        #   we ignore comments.
        kwargs['parser'] = etree_.ETCompatXMLParser()
    doc = etree_.parse(*args, **kwargs)
    return doc

#
# Globals
#

ExternalEncoding = 'ascii'

#
# Data representation classes
#


class writeConstraintType(supermod.writeConstraintType):
    def __init__(self, writeAsRead=None, useEnumeratedValues=None, range=None):
        super(writeConstraintType, self).__init__(writeAsRead, useEnumeratedValues, range, )
supermod.writeConstraintType.subclass = writeConstraintType
# end class writeConstraintType


class addressBlockType(supermod.addressBlockType):
    def __init__(self, offset=None, size=None, usage=None):
        super(addressBlockType, self).__init__(offset, size, usage, )
supermod.addressBlockType.subclass = addressBlockType
# end class addressBlockType


class interruptType(supermod.interruptType):
    def __init__(self, name=None, value=None):
        super(interruptType, self).__init__(name, value, )
supermod.interruptType.subclass = interruptType
# end class interruptType


class device(supermod.device):
    def __init__(self, schemaVersion=None, name=None, version=None, description=None, addressUnitBits=None, width=None, size=None, access=None, resetValue=None, resetMask=None, peripherals=None, vendorExtensions=None):
        super(device, self).__init__(schemaVersion, name, version, description, addressUnitBits, width, size, access, resetValue, resetMask, peripherals, vendorExtensions, )
supermod.device.subclass = device
# end class device


class rangeType(supermod.rangeType):
    def __init__(self, minimum=None, maximum=None):
        super(rangeType, self).__init__(minimum, maximum, )
supermod.rangeType.subclass = rangeType
# end class rangeType


class peripheralsType(supermod.peripheralsType):
    def __init__(self, peripheral=None):
        super(peripheralsType, self).__init__(peripheral, )
supermod.peripheralsType.subclass = peripheralsType
# end class peripheralsType


class peripheralType(supermod.peripheralType):
    def __init__(self, derivedFrom=None, name=None, version=None, description=None, groupName=None, prependToName=None, appendToName=None, disableCondition=None, baseAddress=None, size=None, access=None, resetValue=None, resetMask=None, addressBlock=None, interrupt=None, registers=None):
        super(peripheralType, self).__init__(derivedFrom, name, version, description, groupName, prependToName, appendToName, disableCondition, baseAddress, size, access, resetValue, resetMask, addressBlock, interrupt, registers, )
supermod.peripheralType.subclass = peripheralType
# end class peripheralType


class registersType(supermod.registersType):
    def __init__(self, register=None):
        super(registersType, self).__init__(register, )
supermod.registersType.subclass = registersType
# end class registersType


class registerType(supermod.registerType):
    def __init__(self, derivedFrom=None, dim=None, dimIncrement=None, dimIndex=None, name=None, displayName=None, description=None, alternateGroup=None, addressOffset=None, size=None, access=None, resetValue=None, resetMask=None, modifiedWriteValues=None, writeConstraint=None, readAction=None, fields=None):
        super(registerType, self).__init__(derivedFrom, dim, dimIncrement, dimIndex, name, displayName, description, alternateGroup, addressOffset, size, access, resetValue, resetMask, modifiedWriteValues, writeConstraint, readAction, fields, )
supermod.registerType.subclass = registerType
# end class registerType


class fieldsType(supermod.fieldsType):
    def __init__(self, field=None):
        super(fieldsType, self).__init__(field, )
supermod.fieldsType.subclass = fieldsType
# end class fieldsType


class fieldType(supermod.fieldType):
    def __init__(self, derivedFrom=None, name=None, description=None, lsb=None, msb=None, bitOffset=None, bitWidth=None, bitRange=None, access=None, modifiedWriteValues=None, writeConstraint=None, readAction=None, enumeratedValues=None):
        super(fieldType, self).__init__(derivedFrom, name, description, lsb, msb, bitOffset, bitWidth, bitRange, access, modifiedWriteValues, writeConstraint, readAction, enumeratedValues, )
supermod.fieldType.subclass = fieldType
# end class fieldType


class enumeratedValuesType(supermod.enumeratedValuesType):
    def __init__(self, derivedFrom=None, name=None, usage=None, enumeratedValue=None):
        super(enumeratedValuesType, self).__init__(derivedFrom, name, usage, enumeratedValue, )
supermod.enumeratedValuesType.subclass = enumeratedValuesType
# end class enumeratedValuesType


class enumeratedValueType(supermod.enumeratedValueType):
    def __init__(self, name=None, description=None, value=None, isDefault=None):
        super(enumeratedValueType, self).__init__(name, description, value, isDefault, )
supermod.enumeratedValueType.subclass = enumeratedValueType
# end class enumeratedValueType


class vendorExtensionsType(supermod.vendorExtensionsType):
    def __init__(self, anytypeobjs_=None):
        super(vendorExtensionsType, self).__init__(anytypeobjs_, )
supermod.vendorExtensionsType.subclass = vendorExtensionsType
# end class vendorExtensionsType


def get_root_tag(node):
    tag = supermod.Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = None
    rootClass = supermod.GDSClassesMapping.get(tag)
    if rootClass is None and hasattr(supermod, tag):
        rootClass = getattr(supermod, tag)
    return tag, rootClass


def parse(inFilename, silence=False):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'writeConstraintType'
        rootClass = supermod.writeConstraintType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
##     if not silence:
##         sys.stdout.write('<?xml version="1.0" ?>\n')
##         rootObj.export(
##             sys.stdout, 0, name_=rootTag,
##             namespacedef_='',
##             pretty_print=True)
    return rootObj


def parseEtree(inFilename, silence=False):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'writeConstraintType'
        rootClass = supermod.writeConstraintType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    mapping = {}
    rootElement = rootObj.to_etree(None, name_=rootTag, mapping_=mapping)
    reverse_mapping = rootObj.gds_reverse_node_mapping(mapping)
##     if not silence:
##         content = etree_.tostring(
##             rootElement, pretty_print=True,
##             xml_declaration=True, encoding="utf-8")
##         sys.stdout.write(content)
##         sys.stdout.write('\n')
    return rootObj, rootElement, mapping, reverse_mapping


def parseString(inString, silence=False):
    from StringIO import StringIO
    doc = parsexml_(StringIO(inString))
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'writeConstraintType'
        rootClass = supermod.writeConstraintType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
##     if not silence:
##         sys.stdout.write('<?xml version="1.0" ?>\n')
##         rootObj.export(
##             sys.stdout, 0, name_=rootTag,
##             namespacedef_='')
    return rootObj


def parseLiteral(inFilename, silence=False):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = 'writeConstraintType'
        rootClass = supermod.writeConstraintType
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
##     if not silence:
##         sys.stdout.write('#from Bindings import *\n\n')
##         sys.stdout.write('import Bindings as model_\n\n')
##         sys.stdout.write('rootObj = model_.rootClass(\n')
##         rootObj.exportLiteral(sys.stdout, 0, name_=rootTag)
##         sys.stdout.write(')\n')
    return rootObj


USAGE_TEXT = """
Usage: python ???.py <infilename>
"""


def usage():
    print(USAGE_TEXT)
    sys.exit(1)


def main():
    args = sys.argv[1:]
    if len(args) != 1:
        usage()
    infilename = args[0]
    parse(infilename)


if __name__ == '__main__':
    #import pdb; pdb.set_trace()
    main()
