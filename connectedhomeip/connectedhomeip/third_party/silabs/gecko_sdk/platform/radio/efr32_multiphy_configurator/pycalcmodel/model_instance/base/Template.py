#!/usr/bin/env python

#
# Generated Tue Oct 13 17:05:21 2020 by generateDS.py version 2.12d.
#
# Command line options:
#   ('-o', '..\\..\\model_instance\\base\\Bindings.py')
#   ('--super', 'Bindings')
#   ('-s', '..\\..\\model_instance\\base\\Template.py')
#   ('--subclass-suffix', '')
#   ('--external-encoding', 'ascii')
#   ('-m', '')
#   ('-f', '')
#   ('--silence', '')
#
# Command line arguments:
#   .\model_instance.xsd
#
# Command line:
#   ..\..\generateDS\generateDS.py -o "..\..\model_instance\base\Bindings.py" --super="Bindings" -s "..\..\model_instance\base\Template.py" --subclass-suffix --external-encoding="ascii" -m -f --silence .\model_instance.xsd
#
# Current working directory (os.getcwd()):
#   model_instance
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


class features(supermod.features):
    def __init__(self, feature=None):
        super(features, self).__init__(feature, )
supermod.features.subclass = features
# end class features


class var_overrides(supermod.var_overrides):
    def __init__(self, value=None):
        super(var_overrides, self).__init__(value, )
supermod.var_overrides.subclass = var_overrides
# end class var_overrides


class overrides(supermod.overrides):
    def __init__(self, value=None):
        super(overrides, self).__init__(value, )
supermod.overrides.subclass = overrides
# end class overrides


class defaults(supermod.defaults):
    def __init__(self, value=None):
        super(defaults, self).__init__(value, )
supermod.defaults.subclass = defaults
# end class defaults


class var_values(supermod.var_values):
    def __init__(self, value=None):
        super(var_values, self).__init__(value, )
supermod.var_values.subclass = var_values
# end class var_values


class values(supermod.values):
    def __init__(self, value=None):
        super(values, self).__init__(value, )
supermod.values.subclass = values
# end class values


class model(supermod.model):
    def __init__(self, part_family=None, part_revision=None, calc_version=None, xsd_version=None, desc=None, processed=None, result_code=None, error_message=None, timestamp=None, target='IC', phys=None, profiles=None, variables=None, features=None, logs=None):
        super(model, self).__init__(part_family, part_revision, calc_version, xsd_version, desc, processed, result_code, error_message, timestamp, target, phys, profiles, variables, features, logs, )
supermod.model.subclass = model
# end class model


class featureType(supermod.featureType):
    def __init__(self, name=None, desc=None, value=None):
        super(featureType, self).__init__(name, desc, value, )
supermod.featureType.subclass = featureType
# end class featureType


class physType(supermod.physType):
    def __init__(self, phy=None):
        super(physType, self).__init__(phy, )
supermod.physType.subclass = physType
# end class physType


class phyType(supermod.phyType):
    def __init__(self, name=None, readable_name=None, desc=None, group_name=None, profile_name=None, act_logic=None, tags=None, locked=False, guid=None, profile_inputs=None, profile_outputs=None):
        super(phyType, self).__init__(name, readable_name, desc, group_name, profile_name, act_logic, tags, locked, guid, profile_inputs, profile_outputs, )
supermod.phyType.subclass = phyType
# end class phyType


class profile_inputsType(supermod.profile_inputsType):
    def __init__(self, profile_input=None):
        super(profile_inputsType, self).__init__(profile_input, )
supermod.profile_inputsType.subclass = profile_inputsType
# end class profile_inputsType


class profile_inputType(supermod.profile_inputType):
    def __init__(self, is_array=None, readable_name=None, var_name=None, category=None, values=None):
        super(profile_inputType, self).__init__(is_array, readable_name, var_name, category, values, )
supermod.profile_inputType.subclass = profile_inputType
# end class profile_inputType


class profile_outputsType(supermod.profile_outputsType):
    def __init__(self, profile_output=None):
        super(profile_outputsType, self).__init__(profile_output, )
supermod.profile_outputsType.subclass = profile_outputsType
# end class profile_outputsType


class profile_outputType(supermod.profile_outputType):
    def __init__(self, is_array=None, readable_name=None, category=None, var_name=None, overrides=None):
        super(profile_outputType, self).__init__(is_array, readable_name, category, var_name, overrides, )
supermod.profile_outputType.subclass = profile_outputType
# end class profile_outputType


class profilesType(supermod.profilesType):
    def __init__(self, profile=None):
        super(profilesType, self).__init__(profile, )
supermod.profilesType.subclass = profilesType
# end class profilesType


class profileType(supermod.profileType):
    def __init__(self, name=None, readable_name=None, category=None, desc=None, default=None, act_logic=None, inputs=None, forces=None, outputs=None, default_phys=None):
        super(profileType, self).__init__(name, readable_name, category, desc, default, act_logic, inputs, forces, outputs, default_phys, )
supermod.profileType.subclass = profileType
# end class profileType


class inputsType(supermod.inputsType):
    def __init__(self, input=None):
        super(inputsType, self).__init__(input, )
supermod.inputsType.subclass = inputsType
# end class inputsType


class inputType1(supermod.inputType1):
    def __init__(self, is_array=None, input_type=None, value_limit_min=None, value_limit_max=None, fractional_digits=None, deprecated=None, default_visiblity=None, units_multiplier=None, readable_name=None, category=None, var_name=None, var_values=None, defaults=None):
        super(inputType1, self).__init__(is_array, input_type, value_limit_min, value_limit_max, fractional_digits, deprecated, default_visiblity, units_multiplier, readable_name, category, var_name, var_values, defaults, )
supermod.inputType1.subclass = inputType1
# end class inputType1


class forcesType(supermod.forcesType):
    def __init__(self, force=None):
        super(forcesType, self).__init__(force, )
supermod.forcesType.subclass = forcesType
# end class forcesType


class forceType(supermod.forceType):
    def __init__(self, is_array=None, var_name=None, values=None):
        super(forceType, self).__init__(is_array, var_name, values, )
supermod.forceType.subclass = forceType
# end class forceType


class outputsType(supermod.outputsType):
    def __init__(self, output=None):
        super(outputsType, self).__init__(output, )
supermod.outputsType.subclass = outputsType
# end class outputsType


class outputType2(supermod.outputType2):
    def __init__(self, is_array=None, output_type=None, value_limit_min=None, value_limit_max=None, fractional_digits=None, readable_name=None, category=None, var_name=None, var_values=None, var_overrides=None):
        super(outputType2, self).__init__(is_array, output_type, value_limit_min, value_limit_max, fractional_digits, readable_name, category, var_name, var_values, var_overrides, )
supermod.outputType2.subclass = outputType2
# end class outputType2


class default_physType(supermod.default_physType):
    def __init__(self, default_phy=None):
        super(default_physType, self).__init__(default_phy, )
supermod.default_physType.subclass = default_physType
# end class default_physType


class default_phyType(supermod.default_phyType):
    def __init__(self, phy_name=None):
        super(default_phyType, self).__init__(phy_name, )
supermod.default_phyType.subclass = default_phyType
# end class default_phyType


class variablesType(supermod.variablesType):
    def __init__(self, variable=None):
        super(variablesType, self).__init__(variable, )
supermod.variablesType.subclass = variablesType
# end class variablesType


class variableType(supermod.variableType):
    def __init__(self, name=None, type_=None, is_array=None, format=None, desc=None, forceable=None, svd_mapping=None, units=None, value_do_not_care=False, enum=None, values=None, access_read=None, access_write=None):
        super(variableType, self).__init__(name, type_, is_array, format, desc, forceable, svd_mapping, units, value_do_not_care, enum, values, access_read, access_write, )
supermod.variableType.subclass = variableType
# end class variableType


class enumType(supermod.enumType):
    def __init__(self, name=None, desc=None, members=None):
        super(enumType, self).__init__(name, desc, members, )
supermod.enumType.subclass = enumType
# end class enumType


class membersType(supermod.membersType):
    def __init__(self, member=None):
        super(membersType, self).__init__(member, )
supermod.membersType.subclass = membersType
# end class membersType


class memberType(supermod.memberType):
    def __init__(self, name=None, value=None, desc=None):
        super(memberType, self).__init__(name, value, desc, )
supermod.memberType.subclass = memberType
# end class memberType


class valuesType(supermod.valuesType):
    def __init__(self, calculated=None, forced=None):
        super(valuesType, self).__init__(calculated, forced, )
supermod.valuesType.subclass = valuesType
# end class valuesType


class calculatedType(supermod.calculatedType):
    def __init__(self, value=None):
        super(calculatedType, self).__init__(value, )
supermod.calculatedType.subclass = calculatedType
# end class calculatedType


class forcedType(supermod.forcedType):
    def __init__(self, value=None):
        super(forcedType, self).__init__(value, )
supermod.forcedType.subclass = forcedType
# end class forcedType


class access_readType(supermod.access_readType):
    def __init__(self, name=None):
        super(access_readType, self).__init__(name, )
supermod.access_readType.subclass = access_readType
# end class access_readType


class access_writeType(supermod.access_writeType):
    def __init__(self, name=None):
        super(access_writeType, self).__init__(name, )
supermod.access_writeType.subclass = access_writeType
# end class access_writeType


class logsType(supermod.logsType):
    def __init__(self, log=None):
        super(logsType, self).__init__(log, )
supermod.logsType.subclass = logsType
# end class logsType


class logType3(supermod.logType3):
    def __init__(self, type_=None, valueOf_=None):
        super(logType3, self).__init__(type_, valueOf_, )
supermod.logType3.subclass = logType3
# end class logType3


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
        rootTag = 'features'
        rootClass = supermod.features
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
        rootTag = 'features'
        rootClass = supermod.features
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
        rootTag = 'features'
        rootClass = supermod.features
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
        rootTag = 'features'
        rootClass = supermod.features
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
