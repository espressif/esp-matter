#!c:\Python27\python.exe
"""
Synopsis:
    Generate Python classes from XML Schema definition.
    Input is read from in_xsd_file or, if "-" (dash) arg, from stdin.
    Output is written to files named in "-o" and "-s" options.
Usage:
    python generateDS.py [ options ] <xsd_file>
    python generateDS.py [ options ] -
Options:
    -h, --help               Display this help information.
    -o <outfilename>         Output file name for data representation classes
    -s <subclassfilename>    Output file name for subclasses
    -p <prefix>              Prefix string to be pre-pended to the class names
    -f                       Force creation of output files.  Do not ask.
    -a <namespaceabbrev>     Namespace abbreviation, e.g. "xsd:".
                             Default = 'xs:'.
    -b <behaviorfilename>    Input file name for behaviors added to subclasses
    -m                       Generate properties for member variables
    -c <xmlcatalogfilename>  Input file name to load an XML catalog
    --one-file-per-xsd       Create a python module for each XSD processed.
    --output-directory="XXX" Used in conjunction with --one-file-per-xsd.
                             The directory where the modules will be created.
    --module-suffix="xxx"    To be used in conjunction with --one-file-per-xsd.
                             Append XXX to the end of each file created.
    --subclass-suffix="XXX"  Append XXX to the generated subclass names.
                             Default="Sub".
    --root-element="XX"      When parsing, assume XX is root element of
    --root-element="XX|YY"   instance docs.  Default is first element defined
                             in schema.  If YY is added, then YY is used as the
                             top level class; if YY omitted XX is the default.
                             class. Also see section "Recognizing the top level
                             element" in the documentation.
    --super="XXX"            Super module name in generated in subclass
                             module. Default="???"
    --validator-bodies=path  Path to a directory containing files that provide
                             bodies (implementations) of validator methods.
    --use-getter-setter      Generate getter and setter methods.  Values:
                             "old" - Name getters/setters getVar()/setVar().
                             "new" - Name getters/setters get_var()/set_var().
                             "none" - Do not generate getter/setter methods.
                             Default is "new".
    --user-methods= <module>,
    -u <module>              Optional module containing user methods.  See
                             section "User Methods" in the documentation.
    --no-dates               Do not include the current date in the generated
                             files. This is useful if you want to minimize
                             the amount of (no-operation) changes to the
                             generated python code.
    --no-versions            Do not include the current version in the
                             generated files. This is useful if you want
                             to minimize the amount of (no-operation)
                             changes to the generated python code.
    --no-process-includes    Do not process included XML Schema files.  By
                             default, generateDS.py will insert content
                             from files referenced by <include ... />
                             elements into the XML Schema to be processed.
    --silence                Normally, the code generated with generateDS
                             echoes the information being parsed. To prevent
                             the echo from occurring, use the --silence switch.
                             Also note optional "silence" parameter on
                             generated functions, e.g. parse, parseString, etc.
    --namespacedef='xmlns:abc="http://www.abc.com"'
                             Namespace definition to be passed in as the
                             value for the namespacedef_ parameter of
                             the export() method by the generated
                             parse() and parseString() functions.
                             Default=''.
    --external-encoding=<encoding>
                             Encode output written by the generated export
                             methods using this encoding.  Default, if omitted,
                             is the value returned by sys.getdefaultencoding().
                             Example: --external-encoding='utf-8'.
    --member-specs=list|dict
                             Generate member (type) specifications in each
                             class: a dictionary of instances of class
                             MemberSpec_ containing member name, type,
                             and array or not.  Allowed values are
                             "list" or "dict".  Default: do not generate.
    --export=<export-list>   Specifies export functions to be generated.
                             Value is a whitespace separated list of
                             any of the following:
                                 write -- write XML to file
                                 literal -- write out python code
                                 etree -- build element tree (can serialize
                                     to XML)
                             Example: "write etree"
                             Default: "write"
    -q, --no-questions       Do not ask questions, for example,
                             force overwrite.
    --session=mysession.session
                             Load and use options from session file. You can
                             create session file in generateds_gui.py.  Or,
                             copy and edit sample.session from the
                             distribution.
    --fix-type-names="oldname1:newname1;oldname2:newname2;..."
                             Fix up (replace) complex type names.
    --version                Print version and exit.

Usage example:

    $ python generateDS.py -f -o sample_lib.py sample_api.xsd

creates (with force over-write) sample_lib.py from sample_api.xsd.

    $ python generateDS.py -o sample_lib.py -s sample_app1.py \\
            --member-specs=dict sample_api.xsd

creates sample_lib.py superclass and sample_app1.py subclass modules;
also generates member specifications in each class (in a dictionary).

"""


## LICENSE

## Copyright (c) 2003 Dave Kuhlman

## Permission is hereby granted, free of charge, to any person obtaining
## a copy of this software and associated documentation files (the
## "Software"), to deal in the Software without restriction, including
## without limitation the rights to use, copy, modify, merge, publish,
## distribute, sublicense, and/or sell copies of the Software, and to
## permit persons to whom the Software is furnished to do so, subject to
## the following conditions:

## The above copyright notice and this permission notice shall be
## included in all copies or substantial portions of the Software.

## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
## EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
## MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
## IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
## CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
## TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
## SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


import sys
import os.path
import time
import getopt
import urllib2
import imp
from xml.sax import handler, make_parser
import logging
import keyword
import StringIO
import textwrap

# Default logger configuration
logging.basicConfig(
    level=logging.DEBUG,
    format='%(asctime)s %(levelname)s %(message)s',
    #filemode='w',      # Can uncomment this line or change to append ('a')
    #filename='generateDS.log'  # uncomment to send messages to file
)
# Disable logging messages level INFO and below (includes DEBUG).
logging.disable(logging.INFO)

## import warnings
## warnings.warn('importing IPShellEmbed', UserWarning)
## from IPython.Shell import IPShellEmbed
## args = ''
## ipshell = IPShellEmbed(args,
##     banner = 'Dropping into IPython',
##     exit_msg = 'Leaving Interpreter, back to program.')

# Then use the following line where and when you want to drop into the
# IPython shell:
#    ipshell('<some message> -- Entering ipshell.\\nHit Ctrl-D to exit')


#
# Global variables etc.
#

#
# Do not modify the following VERSION comments.
# Used by updateversion.py.
##VERSION##
VERSION = '2.12d'
##VERSION##

GenerateProperties = 0
UseGetterSetter = 'new'
MemberSpecs = None
DelayedElements = set()
DelayedElements_subclass = set()
AlreadyGenerated = set()
AlreadyGenerated_subclass = set()
PostponedExtensions = []
ElementsForSubclasses = []
ElementDict = {}
fqnToElementDict = {}
fqnToModuleNameMap = {}
Force = False
NoQuestions = False
NoDates = False
NoVersion = False
Dirpath = []
ExternalEncoding = sys.getdefaultencoding()
Namespacedef = ''

NamespacesDict = {}
prefixToNamespaceMap = {}
MappingTypes = {}
Targetnamespace = ""

NameTable = {
    'type': 'type_',
    'float': 'float_',
    'build': 'build_',
}
for kw in keyword.kwlist:
    NameTable[kw] = '%s_' % kw


SubclassSuffix = 'Sub'
RootElement = None
AttributeGroups = {}
ElementGroups = {}
SubstitutionGroups = {}
#
# SubstitutionGroups can also include simple types that are
#   not (defined) elements.  Keep a list of these simple types.
#   These are simple types defined at top level.
SimpleElementDict = {}
SimpleTypeDict = {}
ValidatorBodiesBasePath = None
UserMethodsPath = None
UserMethodsModule = None
XsdNameSpace = ''
CurrentNamespacePrefix = 'xs:'
AnyTypeIdentifier = '__ANY__'
ExportWrite = True
ExportEtree = False
ExportLiteral = True
FixTypeNames = None
SingleFileOutput = True
OutputDirectory = None
ModuleSuffix = ""

SchemaToPythonTypeMap = {}

# Initialize constants.
CurrentNamespacePrefix = None
StringType = None
TokenType = None
IntegerType = None
DecimalType = None
PositiveIntegerType = None
NegativeIntegerType = None
NonPositiveIntegerType = None
NonNegativeIntegerType = None
BooleanType = None
FloatType = None
DoubleType = None
ElementType = None
ComplexTypeType = None
GroupType = None
SequenceType = None
ChoiceType = None
AttributeGroupType = None
AttributeType = None
SchemaType = None
DateTimeType = None
DateType = None
TimeType = None
GYearType = None
GYearMonthType = None
GMonthType = None
GMonthDayType = None
GDayType = None
DateTimeGroupType = None
SimpleContentType = None
ComplexContentType = None
ExtensionType = None
IDType = None
IDREFType = None
IDREFSType = None
IDTypes = None
NameType = None
NCNameType = None
QNameType = None
NameTypes = None
AnyAttributeType = None
SimpleTypeType = None
RestrictionType = None
WhiteSpaceType = None
ListType = None
EnumerationType = None
UnionType = None
AnyType = None
AnnotationType = None
DocumentationType = None
OtherSimpleTypes = None
Base64Type = None


def set_type_constants(nameSpace):
    global CurrentNamespacePrefix, \
        StringType, TokenType, \
        IntegerType, DecimalType, \
        PositiveIntegerType, NegativeIntegerType, \
        NonPositiveIntegerType, NonNegativeIntegerType, \
        BooleanType, FloatType, DoubleType, \
        ElementType, ComplexTypeType, GroupType, SequenceType, ChoiceType, \
        AttributeGroupType, AttributeType, SchemaType, \
        DateTimeType, DateType, TimeType, \
        DateTimeGroupType, \
        GYearType, GYearMonthType, GMonthType, GMonthDayType, GDayType, \
        SimpleContentType, ComplexContentType, ExtensionType, \
        IDType, IDREFType, IDREFSType, IDTypes, \
        NameType, NCNameType, QNameType, NameTypes, \
        AnyAttributeType, SimpleTypeType, RestrictionType, \
        WhiteSpaceType, ListType, EnumerationType, UnionType, \
        Base64Type, AnyType, \
        AnnotationType, DocumentationType, \
        OtherSimpleTypes
    CurrentNamespacePrefix = nameSpace
    AttributeGroupType = nameSpace + 'attributeGroup'
    AttributeType = nameSpace + 'attribute'
    BooleanType = nameSpace + 'boolean'
    ChoiceType = nameSpace + 'choice'
    SimpleContentType = nameSpace + 'simpleContent'
    ComplexContentType = nameSpace + 'complexContent'
    ComplexTypeType = nameSpace + 'complexType'
    GroupType = nameSpace + 'group'
    SimpleTypeType = nameSpace + 'simpleType'
    RestrictionType = nameSpace + 'restriction'
    WhiteSpaceType = nameSpace + 'whiteSpace'
    AnyAttributeType = nameSpace + 'anyAttribute'
    DateTimeType = nameSpace + 'dateTime'
    TimeType = nameSpace + 'time'
    DateType = nameSpace + 'date'
    GYearType = nameSpace + 'gYear'
    GYearMonthType = nameSpace + 'gYearMonth'
    GMonthType = nameSpace + 'gMonth'
    GMonthDayType = nameSpace + 'gMonthDay'
    GDayType = nameSpace + 'gDay'
    DateTimeGroupType = (
        DateTimeType, DateType, TimeType,
        GYearType, GYearMonthType, GMonthType, GMonthDayType, GDayType,
    )
    IntegerType = (
        nameSpace + 'integer',
        nameSpace + 'unsignedShort',
        nameSpace + 'unsignedLong',
        nameSpace + 'unsignedInt',
        nameSpace + 'unsignedByte',
        nameSpace + 'byte',
        nameSpace + 'short',
        nameSpace + 'long',
        nameSpace + 'int',
    )
    DecimalType = nameSpace + 'decimal'
    PositiveIntegerType = nameSpace + 'positiveInteger'
    NegativeIntegerType = nameSpace + 'negativeInteger'
    NonPositiveIntegerType = nameSpace + 'nonPositiveInteger'
    NonNegativeIntegerType = nameSpace + 'nonNegativeInteger'
    DoubleType = nameSpace + 'double'
    ElementType = nameSpace + 'element'
    ExtensionType = nameSpace + 'extension'
    FloatType = nameSpace + 'float'
    IDREFSType = nameSpace + 'IDREFS'
    IDREFType = nameSpace + 'IDREF'
    IDType = nameSpace + 'ID'
    IDTypes = (IDREFSType, IDREFType, IDType, )
    SchemaType = nameSpace + 'schema'
    SequenceType = nameSpace + 'sequence'
    StringType = (
        nameSpace + 'string',
        nameSpace + 'duration',
        nameSpace + 'anyURI',
        nameSpace + 'hexBinary',
        nameSpace + 'normalizedString',
        nameSpace + 'NMTOKEN',
        nameSpace + 'ID',
        nameSpace + 'Name',
        nameSpace + 'language',
    )
    Base64Type = nameSpace + 'base64Binary'
    TokenType = nameSpace + 'token'
    NameType = nameSpace + 'Name'
    NCNameType = nameSpace + 'NCName'
    QNameType = nameSpace + 'QName'
    NameTypes = (NameType, NCNameType, QNameType, )
    ListType = nameSpace + 'list'
    EnumerationType = nameSpace + 'enumeration'
    UnionType = nameSpace + 'union'
    AnnotationType = nameSpace + 'annotation'
    DocumentationType = nameSpace + 'documentation'
    AnyType = nameSpace + 'any'
    OtherSimpleTypes = (
        nameSpace + 'ENTITIES',
        nameSpace + 'ENTITY',
        nameSpace + 'ID',
        nameSpace + 'IDREF',
        nameSpace + 'IDREFS',
        nameSpace + 'NCName',
        nameSpace + 'NMTOKEN',
        nameSpace + 'NMTOKENS',
        nameSpace + 'NOTATION',
        nameSpace + 'Name',
        nameSpace + 'QName',
        nameSpace + 'anyURI',
        nameSpace + 'base64Binary',
        nameSpace + 'hexBinary',
        nameSpace + 'boolean',
        nameSpace + 'byte',
        nameSpace + 'date',
        nameSpace + 'dateTime',
        nameSpace + 'time',
        nameSpace + 'decimal',
        nameSpace + 'double',
        nameSpace + 'duration',
        nameSpace + 'float',
        nameSpace + 'gDay',
        nameSpace + 'gMonth',
        nameSpace + 'gMonthDay',
        nameSpace + 'gYear',
        nameSpace + 'gYearMonth',
        nameSpace + 'int',
        nameSpace + 'integer',
        nameSpace + 'language',
        nameSpace + 'long',
        nameSpace + 'negativeInteger',
        nameSpace + 'nonNegativeInteger',
        nameSpace + 'nonPositiveInteger',
        nameSpace + 'normalizedString',
        nameSpace + 'positiveInteger',
        nameSpace + 'short',
        nameSpace + 'string',
        nameSpace + 'time',
        nameSpace + 'token',
        nameSpace + 'unsignedByte',
        nameSpace + 'unsignedInt',
        nameSpace + 'unsignedLong',
        nameSpace + 'unsignedShort',
        nameSpace + 'anySimpleType',
    )

    global SchemaToPythonTypeMap
    SchemaToPythonTypeMap = {
        BooleanType: 'bool',
        DecimalType: 'float',
        DoubleType: 'float',
        FloatType: 'float',
        NegativeIntegerType: 'int',
        NonNegativeIntegerType: 'int',
        NonPositiveIntegerType: 'int',
        PositiveIntegerType: 'int',
    }
    SchemaToPythonTypeMap.update(dict((x, 'int') for x in IntegerType))

#
# For debugging.
#

# Print only if DEBUG is true.
DEBUG = 0


def dbgprint(level, msg):
    if DEBUG and level > 0:
        print(msg)


def pplist(lst):
    for count, item in enumerate(lst):
        print('%d. %s' % (count, item))


#
# Representation of element definition.
#

def showLevel(outfile, level):
    for idx in range(level):
        outfile.write('    ')


class XschemaElementBase:
    def __init__(self):
        pass


class SimpleTypeElement(XschemaElementBase):
    def __init__(self, name):
        XschemaElementBase.__init__(self)
        self.name = name
        self.base = None
        self.collapseWhiteSpace = 0
        # Attribute definitions for the current attributeGroup, if
        #   there is one.
        self.attributeGroup = None
        # Attribute definitions for the currect element.
        self.attributeDefs = {}
        self.complexType = 0
        # Enumeration values for the current element.
        self.values = list()
        # The other simple types this is a union of.
        self.unionOf = list()
        self.simpleType = 0
        self.listType = 0
        self.documentation = ''

    def setName(self, name):
        self.name = name

    def getName(self):
        return self.name

    def setBase(self, base):
        self.base = base

    def getBase(self):
        return self.base

    def setSimpleType(self, simpleType):
        self.simpleType = simpleType

    def getSimpleType(self):
        return self.simpleType

    def getAttributeGroups(self):
        return self.attributeGroups

    def setAttributeGroup(self, attributeGroup):
        self.attributeGroup = attributeGroup

    def getAttributeGroup(self):
        return self.attributeGroup

    def setListType(self, listType):
        self.listType = listType

    def isListType(self):
        return self.listType

    def __str__(self):
        s1 = '<"%s" SimpleTypeElement instance at 0x%x>' % \
            (self.getName(), id(self))
        return s1
    __repr__ = __str__

    def resolve_list_type(self):
        if self.isListType():
            return 1
        elif self.getBase() in SimpleTypeDict:
            base = SimpleTypeDict[self.getBase()]
            return base.resolve_list_type()
        else:
            return 0


class XschemaElement(XschemaElementBase):
    def __init__(self, attrs, targetNamespace=None):
        XschemaElementBase.__init__(self)
        self.cleanName = ''
        self.attrs = dict(attrs)
        name_val = ''
        type_val = ''
        ref_val = ''
        self.prefix = None
        self.namespace = None
        self.is_root_element = False
        self.targetNamespace = targetNamespace
        self.fullyQualifiedName = None
        self.fullyQualifiedType = None
        self.type = 'NoneType'

        if 'name' in self.attrs:
            self.prefix, name_val = get_prefix_and_value(self.attrs['name'])
            self.fullyQualifiedName = "%s:%s" % (targetNamespace, name_val)

        if 'type' in self.attrs:
            if (len(XsdNameSpace) > 0 and
                    self.attrs['type'].startswith(XsdNameSpace)):
                type_val = self.attrs['type']
            else:
                self.prefix, type_val = get_prefix_and_value(
                    self.attrs['type'])
                self.type = type_val
        if 'ref' in self.attrs:
            self.prefix, ref_val = get_prefix_and_value(self.attrs['ref'])
            if self.prefix in prefixToNamespaceMap:
                xmlns = prefixToNamespaceMap[self.prefix]
                fqn = "%s:%s" % (xmlns, ref_val)
                if fqn in fqnToElementDict:
                    referencedElement = fqnToElementDict[fqn]
                    type_val = referencedElement.getType()
                    if type_val == "NoneType":
                        # anonymous types
                        type_val = referencedElement.getName()
                    name_val = ref_val

        if type_val and not name_val:
            name_val = type_val
        if ref_val and not name_val:
            name_val = ref_val
        if ref_val and not type_val:
            type_val = ref_val
        if name_val:
            self.attrs['name'] = name_val
        if type_val:
            self.attrs['type'] = type_val
        if ref_val:
            self.attrs['ref'] = ref_val
        # fix_abstract
        abstract_type = attrs.get('abstract', 'false').lower()
        self.abstract_type = abstract_type in ('1', 'true')
        self.default = self.attrs.get('default')
        self.name = name_val
        self.children = []
        self.optional = False
        self.minOccurs = 1
        self.maxOccurs = 1
        self.complex = 0
        self.complexType = 0
        self.mixed = 0
        self.base = None
        self.mixedExtensionError = 0
        self.collapseWhiteSpace = 0
        # Attribute definitions for the currect element.
        self.attributeDefs = {}
        # Attribute definitions for the current attributeGroup, if
        #   there is one.
        self.attributeGroup = None
        # List of names of attributes for this element.
        # We will add the attribute defintions in each of these groups
        #   to this element in annotate().
        self.attributeGroupNameList = []
        # similar things as above, for groups of elements
        self.elementGroup = None
        self.topLevel = 0
        # Does this element contain an anyAttribute?
        self.anyAttribute = 0
        self.explicit_define = 0
        self.simpleType = None
        # Enumeration values for the current element.
        self.values = list()
        # The parent choice for the current element.
        self.choice = None
        self.listType = 0
        self.simpleBase = []
        self.documentation = ''
        self.restrictionBase = None
        self.simpleContent = False
        self.extended = False

    def addChild(self, element):
        self.children.append(element)

    def getChildren(self):
        return self.children

    def getName(self):
        return self.name

    def getFullyQualifiedName(self):
        return self.fullyQualifiedName

    def getFullyQualifiedType(self):
        typeName = self.type
        if not typeName or typeName == "NoneType":
            typeName = self.name
        if typeName and not self.fullyQualifiedType:
            namespace = self.targetNamespace
            if self.prefix:
                xmlns = prefixToNamespaceMap[self.prefix]
                if xmlns:
                    namespace = xmlns
            self.fullyQualifiedType = "%s:%s" % (namespace, typeName)

        return self.fullyQualifiedType

    def getCleanName(self):
        return self.cleanName

    def getUnmappedCleanName(self):
        return self.unmappedCleanName

    def setName(self, name):
        self.name = name

    def getAttrs(self):
        return self.attrs

    def setAttrs(self, attrs):
        self.attrs = attrs

    def getMinOccurs(self):
        return self.minOccurs

    def getMaxOccurs(self):
        return self.maxOccurs

    def getOptional(self):
        return self.optional

    def getRawType(self):
        return self.type

    def setExplicitDefine(self, explicit_define):
        self.explicit_define = explicit_define

    def isExplicitDefine(self):
        return self.explicit_define

    def isAbstract(self):
        return self.abstract_type

    def setListType(self, listType):
        self.listType = listType

    def isListType(self):
        return self.listType

    def getType(self):
        returnType = self.type
        if self.type in ElementDict:
            typeObj = ElementDict[self.type]
            typeObjType = typeObj.getRawType()
            if is_builtin_simple_type(typeObjType):
                returnType = typeObjType
        return returnType

    def isComplex(self):
        return self.complex

    def setIsRootElement(self, is_root_elem):
        self.is_root_element = is_root_elem

    def isRootElement(self):
        return self.is_root_element

    def addAttributeDefs(self, attrs):
        self.attributeDefs.append(attrs)

    def getAttributeDefs(self):
        return self.attributeDefs

    def isMixed(self):
        return self.mixed

    def setMixed(self, mixed):
        self.mixed = mixed

    def setBase(self, base):
        self.base = base

    def getBase(self):
        return self.base

    def getMixedExtensionError(self):
        return self.mixedExtensionError

    def getAttributeGroups(self):
        return self.attributeGroups

    def addAttribute(self, name, attribute):
        self.attributeGroups[name] = attribute

    def setAttributeGroup(self, attributeGroup):
        self.attributeGroup = attributeGroup

    def getAttributeGroup(self):
        return self.attributeGroup

    def setElementGroup(self, elementGroup):
        self.elementGroup = elementGroup

    def getElementGroup(self):
        return self.elementGroup

    def setTopLevel(self, topLevel):
        self.topLevel = topLevel

    def getTopLevel(self):
        return self.topLevel

    def setAnyAttribute(self, anyAttribute):
        self.anyAttribute = anyAttribute

    def getAnyAttribute(self):
        return self.anyAttribute

    def setSimpleType(self, simpleType):
        self.simpleType = simpleType

    def getSimpleType(self):
        return self.simpleType

    def setDefault(self, default):
        self.default = default

    def getDefault(self):
        return self.default

    def getSimpleBase(self):
        return self.simpleBase

    def setSimpleBase(self, simpleBase):
        self.simpleBase = simpleBase

    def addSimpleBase(self, simpleBase):
        self.simpleBase.append(simpleBase)

    def getRestrictionBase(self):
        return self.restrictionBase

    def setRestrictionBase(self, base):
        self.restrictionBase = base

    def getRestrictionBaseObj(self):
        rBaseObj = None
        rBaseName = self.getRestrictionBase()
        if rBaseName and rBaseName in ElementDict:
            rBaseObj = ElementDict[rBaseName]
        return rBaseObj

    def setSimpleContent(self, simpleContent):
        self.simpleContent = simpleContent

    def getSimpleContent(self):
        return self.simpleContent

    def getExtended(self):
        return self.extended

    def setExtended(self, extended):
        self.extended = extended

    def show(self, outfile, level):
        if self.name == 'Reference':
            showLevel(outfile, level)
            outfile.write('Name: %s  Type: %s  id: %d\n' % (
                self.name, self.getType(), id(self),))
            showLevel(outfile, level)
            outfile.write('  - Complex: %d  MaxOccurs: %d  '
                          'MinOccurs: %d\n' % (
                              self.complex, self.maxOccurs, self.minOccurs))
            showLevel(outfile, level)
            outfile.write('  - Attrs: %s\n' % self.attrs)
            showLevel(outfile, level)
            #outfile.write('  - AttributeDefs: %s\n' % self.attributeDefs)
            outfile.write('  - AttributeDefs:\n')
            for key, value in self.getAttributeDefs().items():
                showLevel(outfile, level + 1)
                outfile.write('- key: %s  value: %s\n' % (key, value, ))
        for child in self.children:
            child.show(outfile, level + 1)

    def annotate(self):
        # resolve group references within groups
        for grp in ElementGroups.values():
            expandGroupReferences(grp)
        # Recursively expand group references
        visited = set()
        self.expandGroupReferences_tree(visited)
        self.collect_element_dict()
        self.annotate_find_type()
        self.annotate_tree()
        self.fix_dup_names()
        self.coerce_attr_types()
        self.checkMixedBases()
        self.markExtendedTypes()

    def markExtendedTypes(self):
        base = self.getBase()
        if base and base in ElementDict:
            parent = ElementDict[base]
            parent.setExtended(True)
        for child in self.children:
            child.markExtendedTypes()

    def expandGroupReferences_tree(self, visited):
        if self.getName() in visited:
            return
        visited.add(self.getName())
        expandGroupReferences(self)
        for child in self.children:
            child.expandGroupReferences_tree(visited)

    def collect_element_dict(self):
        base = self.getBase()
        if (self.getTopLevel() or len(self.getChildren()) > 0 or
                len(self.getAttributeDefs()) > 0 or base):
            ElementDict[self.name] = self
        for child in self.children:
            child.collect_element_dict()

    def build_element_dict(self, elements):
        base = self.getBase()
        if (self.getTopLevel() or len(self.getChildren()) > 0 or
                len(self.getAttributeDefs()) > 0 or base):
            if self.name not in elements:
                elements[self.name] = self
        for child in self.children:
            child.build_element_dict(elements)

    def get_element(self, element_name):
        if self.element_dict is None:
            self.element_dict = dict()
            self.build_element_dict(self.element_dict)
        return self.element_dict.get(element_name)

    # If it is a mixed-content element and it is defined as
    #   an extension, then all of its bases (base, base of base, ...)
    #   must be mixed-content.  Mark it as an error, if not.
    def checkMixedBases(self):
        self.rationalizeMixedBases()
        self.collectSimpleBases()
        self.checkMixedBasesChain(self, self.mixed)
        for child in self.children:
            child.checkMixedBases()

    def collectSimpleBases(self):
        if self.base:
            self.addSimpleBase(self.base.encode('utf-8'))
        if self.simpleBase:
            base1 = SimpleTypeDict.get(self.simpleBase[0])
            if base1:
                base2 = base1.base or None
            else:
                base2 = None
            while base2:
                self.addSimpleBase(base2.encode('utf-8'))
                base2 = SimpleTypeDict.get(base2)
                if base2:
                    base2 = base2.getBase()

    def rationalizeMixedBases(self):
        mixed = self.hasMixedInChain()
        if mixed:
            self.equalizeMixedBases()

    def hasMixedInChain(self):
        if self.isMixed():
            return True
        base = self.getBase()
        if base and base in ElementDict:
            parent = ElementDict[base]
            return parent.hasMixedInChain()
        else:
            return False

    def equalizeMixedBases(self):
        if not self.isMixed():
            self.setMixed(True)
        base = self.getBase()
        if base and base in ElementDict:
            parent = ElementDict[base]
            parent.equalizeMixedBases()

    def checkMixedBasesChain(self, child, childMixed):
        base = self.getBase()
        if base and base in ElementDict:
            parent = ElementDict[base]
            if childMixed != parent.isMixed():
                self.mixedExtensionError = 1
                return
            parent.checkMixedBasesChain(child, childMixed)

    def resolve_type(self):
        self.complex = 0
        # If it has any attributes, then it's complex.
        attrDefs = self.getAttributeDefs()
        if len(attrDefs) > 0:
            self.complex = 1
            # type_val = ''
        type_val = self.resolve_type_1()
        if type_val == AnyType:
            return AnyType
        if type_val in SimpleTypeDict:
            self.addSimpleBase(type_val.encode('utf-8'))
            simple_type = SimpleTypeDict[type_val]
            list_type = simple_type.resolve_list_type()
            self.setListType(list_type)
        if type_val:
            if type_val in ElementDict:
                type_val1 = type_val
                # The following loop handles the case where an Element's
                # reference element has no sub-elements and whose type is
                # another simpleType (potentially of the same name). Its
                # fundamental function is to avoid the incorrect
                # categorization of "complex" to Elements which are not and
                # correctly resolve the Element's type as well as its
                # potential values. It also handles cases where the Element's
                # "simpleType" is so-called "top level" and is only available
                # through the global SimpleTypeDict.
                i = 0
                while True:
                    element = ElementDict[type_val1]
                    # Resolve our potential values if present
                    self.values = element.values
                    # If the type is available in the SimpleTypeDict, we
                    # know we've gone far enough in the Element hierarchy
                    # and can return the correct base type.
                    t = element.resolve_type_1()
                    if t in SimpleTypeDict:
                        type_val1 = SimpleTypeDict[t].getBase()
                        if type_val1 and not is_builtin_simple_type(type_val1):
                            type_val1 = strip_namespace(type_val1)
                        break
                    # If the type name is the same as the previous type name
                    # then we know we've fully resolved the Element hierarchy
                    # and the Element is well and truely "complex". There is
                    # also a need to handle cases where the Element name and
                    # its type name are the same (ie. this is our first time
                    # through the loop). For example:
                    #   <xsd:element name="ReallyCool" type="ReallyCool"/>
                    #   <xsd:simpleType name="ReallyCool">
                    #     <xsd:restriction base="xsd:string">
                    #       <xsd:enumeration value="MyThing"/>
                    #     </xsd:restriction>
                    #   </xsd:simpleType>
                    if t == type_val1 and i != 0:
                        break
                    if t not in ElementDict:
                        type_val1 = t
                        break
                    type_val1 = t
                    i += 1
                if is_builtin_simple_type(type_val1):
                    type_val = type_val1
                else:
                    self.complex = 1
            elif type_val in SimpleTypeDict:
                count = 0
                type_val1 = type_val
                while True:
                    element = SimpleTypeDict[type_val1]
                    type_val1 = element.getBase()
                    if type_val1 and not is_builtin_simple_type(type_val1):
                        type_val1 = strip_namespace(type_val1)
                    if type_val1 is None:
                        # Something seems wrong.  Can't find base simple type.
                        #   Give up and use default.
                        type_val = StringType[0]
                        break
                    if type_val1 in SimpleTypeDict:
                        count += 1
                        if count > 10:
                            # Give up.  We're in a loop.  Use default.
                            type_val = StringType[0]
                            break
                    else:
                        type_val = type_val1
                        break
                # Add the namespace prefix to the simple type if needed.
                if len(type_val.split(':')) == 1:
                    type_val = CurrentNamespacePrefix + type_val
            else:
                if is_builtin_simple_type(type_val):
                    pass
                else:
                    type_val = StringType[0]
        else:
            type_val = StringType[0]
        return type_val

    def resolve_type_1(self):
        type_val = ''
        if 'type' in self.attrs:
            type_val = self.attrs['type']
            if type_val in SimpleTypeDict:
                self.simpleType = type_val
        elif 'ref' in self.attrs:
            type_val = strip_namespace(self.attrs['ref'])
        elif 'name' in self.attrs:
            type_val = strip_namespace(self.attrs['name'])
            #type_val = self.attrs['name']
        return type_val

    def annotate_find_type(self):
        if self.type == AnyTypeIdentifier:
            pass
        else:
            type_val = self.resolve_type()
            self.attrs['type'] = type_val
            self.type = type_val
            self.fullyQualifiedType = None
        if not self.complex:
            SimpleElementDict[self.name] = self
        for child in self.children:
            child.annotate_find_type()

    def annotate_tree(self):
        # If there is a namespace, replace it with an underscore.
        if self.base:
            self.base = strip_namespace(self.base)
        self.unmappedCleanName = cleanupName(self.name)
        self.cleanName = mapName(self.unmappedCleanName)
        self.replace_attributeGroup_names()
        # Resolve "maxOccurs" attribute
        if 'maxOccurs' in self.attrs.keys():
            maxOccurs = self.attrs['maxOccurs']
        elif self.choice and 'maxOccurs' in self.choice.attrs.keys():
            maxOccurs = self.choice.attrs['maxOccurs']
        else:
            maxOccurs = 1
        # Resolve "minOccurs" attribute
        if 'minOccurs' in self.attrs.keys():
            minOccurs = self.attrs['minOccurs']
        elif self.choice and 'minOccurs' in self.choice.attrs.keys():
            minOccurs = self.choice.attrs['minOccurs']
        else:
            minOccurs = 1
        # Cleanup "minOccurs" and "maxOccurs" attributes
        try:
            minOccurs = int(minOccurs)
            if minOccurs == 0:
                self.optional = True
        except ValueError:
            err_msg('*** %s  minOccurs must be integer.\n' % self.getName())
            sys.exit(1)
        try:
            if maxOccurs == 'unbounded':
                maxOccurs = 99999
            else:
                maxOccurs = int(maxOccurs)
        except ValueError:
            err_msg('*** %s  maxOccurs must be integer or "unbounded".\n' % (
                self.getName(), ))
            sys.exit(1)
        self.minOccurs = minOccurs
        self.maxOccurs = maxOccurs

        # If it does not have a type, then make the type the same as the name.
        if self.type == 'NoneType' and self.name:
            self.type = self.name
        # Is it a mixed-content element definition?
        if 'mixed' in self.attrs.keys():
            mixed = self.attrs['mixed'].strip()
            if mixed == '1' or mixed.lower() == 'true':
                self.mixed = 1
        # If this element has a base and the base is a simple type and
        #   the simple type is collapseWhiteSpace, then mark this
        #   element as collapseWhiteSpace.
        base = self.getBase()
        if base and base in SimpleTypeDict:
            parent = SimpleTypeDict[base]
            if (isinstance(parent, SimpleTypeElement) and
                    parent.collapseWhiteSpace):
                self.collapseWhiteSpace = 1
        # Do it recursively for all descendents.
        for child in self.children:
            child.annotate_tree()

    #
    # For each name in the attributeGroupNameList for this element,
    #   add the attributes defined for that name in the global
    #   attributeGroup dictionary.
    # We do this recursively because an attribute group can reference
    #   other attribute groups.
    def replace_attributeGroup_names(self):
        for groupName in self.attributeGroupNameList:
            self.replace_attributeGroup_names_1(groupName)

    def replace_attributeGroup_names_1(self, groupName):
        key = None
        if groupName in AttributeGroups:
            key = groupName
        else:
            # Looking for name space prefix
            key1 = strip_namespace(groupName)
            if key1 != groupName and key1 in AttributeGroups:
                key = key1
        if key is not None:
            attrGroup = AttributeGroups[key]
            for name in attrGroup.getKeys():
                if (name in AttributeGroups or
                        strip_namespace(name) in AttributeGroups):
                    self.replace_attributeGroup_names_1(name)
                else:
                    attr = attrGroup.get(name)
                    self.attributeDefs[name] = attr
        else:
            err_msg('*** Error. attributeGroup %s not defined.\n' % (
                groupName, ))

    def __str__(self):
        s1 = '<XschemaElement name: "%s" type: "%s">' % \
            (self.getName(), self.getType(), )
        return s1
    __repr__ = __str__

    def fix_dup_names(self):
        # Patch-up names that are used for both a child element and
        #   an attribute.
        #
        attrDefs = self.getAttributeDefs()
        # Collect a list of child element names.
        #   Must do this for base (extension) elements also.
        elementNames = set()
        self.collectElementNames(elementNames, 0)
        replaced = []
        # Create the needed new attributes.
        keys = attrDefs.keys()
        for key in keys:
            attr = attrDefs[key]
            name = attr.getName()
            mappedName = mapName(name)
            if mappedName in elementNames:
                newName = mappedName + '_attr'
                newAttr = XschemaAttribute(newName)
                newAttr.setOrig_name(name)
                attrDefs[newName] = newAttr
                replaced.append(name)
        # Remove the old (replaced) attributes.
        for name in replaced:
            del attrDefs[name]
        for child in self.children:
            child.fix_dup_names()

    def collectElementNames(self, elementNames, count):
        for child in self.children:
            elementNames.add(child.getCleanName())
        base = self.getBase()
        if base and base in ElementDict:
            parent = ElementDict[base]
            count += 1
            if count > 100:
                msg = 'Extension/restriction recursion detected.  ' \
                    'Suggest you check definitions of types ' \
                    '%s and %s.'
                msg = msg % (self.getName(), parent.getName(), )
                raise RuntimeError(msg)
            parent.collectElementNames(elementNames, count)

    def coerce_attr_types(self):
        attrDefs = self.getAttributeDefs()
        for idx, name in enumerate(attrDefs):
            attr = attrDefs[name]
            attrType = attr.getData_type()
            if (attrType == IDType or
                    attrType == IDREFType or
                    attrType == IDREFSType):
                attr.setData_type(StringType[0])
        for child in self.children:
            child.coerce_attr_types()
# end class XschemaElement


class XschemaAttributeGroup:
    def __init__(self, name='', group=None):
        self.name = name
        if group:
            self.group = group
        else:
            self.group = {}

    def setName(self, name):
        self.name = name

    def getName(self):
        return self.name

    def setGroup(self, group):
        self.group = group

    def getGroup(self):
        return self.group

    def get(self, name, default=None):
        if name in self.group:
            return self.group[name]
        else:
            return default

    def getKeys(self):
        return self.group.keys()

    def add(self, name, attr):
        self.group[name] = attr

    def delete(self, name):
        if name in self.group:
            del self.group[name]
            return 1
        else:
            return 0
# end class XschemaAttributeGroup


class XschemaGroup:
    def __init__(self, ref):
        self.ref = ref
# end class XschemaGroup


class XschemaAttribute:
    def __init__(
            self,
            name,
            data_type='xs:string',
            use='optional',
            default=None):
        self.name = name
        self.data_type = data_type
        self.use = use
        self.default = default
        # Enumeration values for the attribute.
        self.values = list()
        # If we change the name, e.g. because an attribute and child have
        # the same name, save the old name here.
        self.orig_name = None

    def setName(self, name):
        self.name = name

    def getName(self):
        return self.name

    def setData_type(self, data_type):
        self.data_type = data_type

    def getData_type(self):
        return self.data_type

    def getType(self):
        returnType = self.data_type
        if self.data_type in SimpleElementDict:
            typeObj = SimpleElementDict[self.data_type]
            typeObjType = typeObj.getRawType()
            if (typeObjType in StringType or
                    typeObjType == TokenType or
                    typeObjType in DateTimeGroupType or
                    typeObjType == Base64Type or
                    typeObjType in IntegerType or
                    typeObjType == DecimalType or
                    typeObjType == PositiveIntegerType or
                    typeObjType == NegativeIntegerType or
                    typeObjType == NonPositiveIntegerType or
                    typeObjType == NonNegativeIntegerType or
                    typeObjType == BooleanType or
                    typeObjType == FloatType or
                    typeObjType == DoubleType):
                returnType = typeObjType
        return returnType

    def setUse(self, use):
        self.use = use

    def getUse(self):
        return self.use

    def setDefault(self, default):
        self.default = default

    def getDefault(self):
        return self.default

    def setOrig_name(self, orig_name):
        self.orig_name = orig_name

    def getOrig_name(self):
        return self.orig_name
# end class XschemaAttribute


#
# SAX handler
#
class XschemaHandler(handler.ContentHandler):
    def __init__(self):
        handler.ContentHandler.__init__(self)
        self.stack = []
        self.root = None
        self.inElement = 0
        self.inComplexType = 0
        self.inNonanonymousComplexType = 0
        self.inSequence = 0
        self.sequenceStack = []
        self.inChoice = 1
        self.inAttribute = 0
        self.attributeGroupLevel = 0
        self.inSimpleType = 0
        self.inSimpleContent = 0
        self.inRestrictionType = 0
        self.inAnnotationType = 0
        self.inDocumentationType = 0
        # The last attribute we processed.
        self.lastAttribute = None
        # Simple types that exist in the global context and may be used to
        # qualify the type of many elements and/or attributes.
        self.topLevelSimpleTypes = list()
        # The current choice type we're in
        self.currentChoice = None
        self.firstElement = True

    def getRoot(self):
        return self.root

    def extractSchemaNamespace(self, attrs):
        schemaUri = 'http://www.w3.org/2001/XMLSchema'
        keys = [x for x, v in attrs.items() if v == schemaUri]
        if not keys:
            return None
        keys = [x[6:] for x in keys if x.startswith('xmlns')]
        if not keys:
            return None
        return keys[0]

    def startElement(self, name, attrs):
        global Targetnamespace, NamespacesDict, XsdNameSpace, fqnToElementDict
        logging.debug("Start element: %s %s" % (name, repr(attrs.items())))
        if len(self.stack) == 0 and self.firstElement:
            self.firstElement = False
            schemaNamespace = self.extractSchemaNamespace(attrs)
            if schemaNamespace:
                XsdNameSpace = schemaNamespace
                set_type_constants(schemaNamespace + ':')
            else:
                if len(name.split(':')) == 1:
                    XsdNameSpace = ''
                    set_type_constants('')
        if name == SchemaType:
            self.inSchema = 1
            element = XschemaElement(attrs)
            if len(self.stack) == 1:
                element.setTopLevel(1)
            self.stack.append(element)
            # If there is an attribute "xmlns" and its value is
            #   "http://www.w3.org/2001/XMLSchema", then remember and
            #   use that namespace prefix.
            for name, value in attrs.items():
                if name[:6] == 'xmlns:':
                    prefix = name[6:]
                    nameSpace = prefix + ':'
                    NamespacesDict[value] = nameSpace
                    prefixToNamespaceMap[prefix] = value
                elif name == 'targetNamespace':
                    Targetnamespace = value
                    element.targetNamespace = value
        elif (name == ElementType or
                ((name == ComplexTypeType) and (len(self.stack) == 1))):
            self.inElement = 1
            self.inNonanonymousComplexType = 1
            element = XschemaElement(attrs, Targetnamespace)
            fqn = element.getFullyQualifiedName()
            if fqn:
                fqnToElementDict[fqn] = element

            if element.prefix in prefixToNamespaceMap:
                element.namespace = prefixToNamespaceMap[element.prefix]

            if self.sequenceStack:
                minOccurs, maxOccurs = self.sequenceStack[-1]
                if 'minOccurs' not in attrs and minOccurs is not None:
                    element.attrs['minOccurs'] = minOccurs
                if 'maxOccurs' not in attrs and maxOccurs is not None:
                    element.attrs['maxOccurs'] = maxOccurs

            if not 'type' in attrs.keys() and not 'ref' in attrs.keys():
                element.setExplicitDefine(1)
            if len(self.stack) == 1:
                element.setTopLevel(1)
            if 'substitutionGroup' in attrs.keys() and 'name' in attrs.keys():
                substituteName = attrs['name']
                headName = attrs['substitutionGroup']
                _, headName = get_prefix_and_value(headName)
                if headName not in SubstitutionGroups:
                    SubstitutionGroups[headName] = []
                SubstitutionGroups[headName].append(substituteName)
            if name == ComplexTypeType:
                element.complexType = 1
            if self.inChoice and self.currentChoice:
                element.choice = self.currentChoice
            self.stack.append(element)
        elif name == ComplexTypeType:
            # If it have any attributes and there is something on the stack,
            #   then copy the attributes to the item on top of the stack.
            if len(self.stack) > 1 and len(attrs) > 0:
                parentDict = self.stack[-1].getAttrs()
                for key in attrs.keys():
                    parentDict[key] = attrs[key]
            self.inComplexType = 1
        elif name == AnyType:
            element = XschemaElement(attrs)
            element.type = AnyTypeIdentifier
            self.stack.append(element)
        elif name == GroupType:
            element = XschemaElement(attrs)
            if len(self.stack) == 1:
                element.setTopLevel(1)
            self.stack.append(element)
        elif name == SequenceType:
            self.inSequence = 1
            self.sequenceStack.append(
                [attrs.get('minOccurs'), attrs.get('maxOccurs')])
        elif name == ChoiceType:
            self.currentChoice = XschemaElement(attrs)
            self.inChoice = 1
        elif name == AttributeType:
            self.inAttribute = 1
            if 'name' in attrs.keys():
                name = attrs['name']
            elif 'ref' in attrs.keys():
                name = strip_namespace(attrs['ref'])
            else:
                name = 'no_attribute_name'
            if 'type' in attrs.keys():
                data_type = attrs['type']
            else:
                data_type = StringType[0]
            if 'use' in attrs.keys():
                use = attrs['use']
            else:
                use = 'optional'
            if 'default' in attrs.keys():
                default = attrs['default']
            else:
                default = None
            if self.stack[-1].attributeGroup:
                # Add this attribute to a current attributeGroup.
                attribute = XschemaAttribute(name, data_type, use, default)
                self.stack[-1].attributeGroup.add(name, attribute)
            else:
                # Add this attribute to the element/complexType.
                attribute = XschemaAttribute(name, data_type, use, default)
                self.stack[-1].attributeDefs[name] = attribute
            self.lastAttribute = attribute
        elif name == AttributeGroupType:
            if self.attributeGroupLevel >= 1:
                # We are in an attribute group and are declaring a reference
                #   to another attribute group.
                if 'ref' in attrs.keys():
                    self.stack[-1].attributeGroup.add(attrs['ref'], None)
            else:
                # If it has attribute 'name', then we are defining a new
                #   attribute group.
                #   Prepare to save it as an attributeGroup.
                if 'name' in attrs.keys():
                    name = strip_namespace(attrs['name'])
                    attributeGroup = XschemaAttributeGroup(name)
                    element = XschemaElement(attrs)
                    if len(self.stack) == 1:
                        element.setTopLevel(1)
                    element.setAttributeGroup(attributeGroup)
                    self.stack.append(element)
                # If it has attribute 'ref', add it to the list of
                #   attributeGroups for this element/complexType.
                if 'ref' in attrs.keys():
                    self.stack[-1].attributeGroupNameList.append(attrs['ref'])
            self.attributeGroupLevel += 1
        elif name == SimpleContentType:
            self.inSimpleContent = 1
            if len(self.stack) > 0:
                self.stack[-1].setSimpleContent(True)
        elif name == ComplexContentType:
            pass
        elif name == ExtensionType:
            if 'base' in attrs.keys() and len(self.stack) > 0:
                extensionBase = attrs['base']
                if (extensionBase in StringType or
                        extensionBase in IDTypes or
                        extensionBase in NameTypes or
                        extensionBase == TokenType or
                        extensionBase in DateTimeGroupType or
                        extensionBase == Base64Type or
                        extensionBase in IntegerType or
                        extensionBase == DecimalType or
                        extensionBase == PositiveIntegerType or
                        extensionBase == NegativeIntegerType or
                        extensionBase == NonPositiveIntegerType or
                        extensionBase == NonNegativeIntegerType or
                        extensionBase == BooleanType or
                        extensionBase == FloatType or
                        extensionBase == DoubleType or
                        extensionBase in OtherSimpleTypes):
                    if (len(self.stack) > 0 and
                            isinstance(self.stack[-1], XschemaElement)):
                        self.stack[-1].addSimpleBase(
                            extensionBase.encode('utf-8'))
                else:
                    self.stack[-1].setBase(extensionBase)
        elif name == AnyAttributeType:
            # Mark the current element as containing anyAttribute.
            self.stack[-1].setAnyAttribute(1)
        elif name == SimpleTypeType:
            # fixlist
            if self.inAttribute:
                pass
            elif self.inSimpleType <= 0:
                # Save the name of the simpleType, but ignore everything
                #   else about it (for now).
                if 'name' in attrs.keys():
                    stName = cleanupName(attrs['name'])
                elif len(self.stack) > 0:
                    stName = cleanupName(self.stack[-1].getName())
                else:
                    stName = None
                # If the parent is an element, mark it as a simpleType.
                if len(self.stack) > 0:
                    self.stack[-1].setSimpleType(1)
                element = SimpleTypeElement(stName)
                SimpleTypeDict[stName] = element
                self.stack.append(element)
            self.inSimpleType += 1
        elif name == RestrictionType:
            if self.inAttribute:
                if 'base' in attrs:
                    self.lastAttribute.setData_type(attrs['base'])
            else:
                # If we are in a simpleType, capture the name of
                #   the restriction base.
                if ((self.inSimpleType >= 1 or self.inSimpleContent) and
                        'base' in attrs.keys()):
                    self.stack[-1].setBase(attrs['base'])
                else:
                    if 'base' in attrs:
                        self.stack[-1].setRestrictionBase(attrs['base'])
            self.inRestrictionType = 1
        elif name == EnumerationType:
            if self.inAttribute and 'value' in attrs:
                # We know that the restriction is on an attribute and the
                # attributes of the current element are un-ordered so the
                # instance variable "lastAttribute" will have our attribute.
                self.lastAttribute.values.append(attrs['value'])
            elif self.inElement and 'value' in attrs:
                # We're not in an attribute so the restriction must have
                # been placed on an element and that element will still be
                # in the stack. We search backwards through the stack to
                # find the last element.
                element = None
                if self.stack:
                    for entry in reversed(self.stack):
                        if isinstance(entry, XschemaElement):
                            element = entry
                            break
                if element is None:
                    err_msg('Cannot find element to attach '
                            'enumeration: %s\n' % (attrs['value']), )
                    sys.exit(1)
                element.values.append(attrs['value'])
            elif self.inSimpleType >= 1 and 'value' in attrs:
                # We've been defined as a simpleType on our own.
                self.stack[-1].values.append(attrs['value'])
        elif name == UnionType:
            # Union types are only used with a parent simpleType and we want
            # the parent to know what it's a union of.
            parentelement = self.stack[-1]
            if (isinstance(parentelement, SimpleTypeElement) and
                    'memberTypes' in attrs):
                for member in attrs['memberTypes'].split(" "):
                    self.stack[-1].unionOf.append(member)
        elif name == WhiteSpaceType and self.inRestrictionType:
            if 'value' in attrs:
                if attrs.getValue('value') == 'collapse':
                    self.stack[-1].collapseWhiteSpace = 1
        elif name == ListType:
            self.inListType = 1
            # fixlist
            if self.inSimpleType >= 1:
                self.stack[-1].setListType(1)
                if 'itemType' in attrs:
                    self.stack[-1].setBase(attrs['itemType'])
        elif name == AnnotationType:
            self.inAnnotationType = 1
        elif name == DocumentationType:
            if self.inAnnotationType:
                self.inDocumentationType = 1
        logging.debug("Start element stack: %d" % len(self.stack))

    def endElement(self, name):
        logging.debug("End element: %s" % (name))
        logging.debug("End element stack: %d" % (len(self.stack)))
        if name == SimpleTypeType:      # and self.inSimpleType:
            self.inSimpleType -= 1
            if self.inAttribute:
                pass
            elif self.inSimpleType <= 0:
                # If the simpleType is directly off the root, it may be used to
                # qualify the type of many elements and/or attributes so we
                # don't want to loose it entirely.
                simpleType = self.stack.pop()
                # fixlist
                if len(self.stack) == 1:
                    self.topLevelSimpleTypes.append(simpleType)
                    self.stack[-1].setListType(simpleType.isListType())
        elif name == RestrictionType and self.inRestrictionType:
            self.inRestrictionType = 0
        elif (name == ElementType or
                (name == ComplexTypeType and self.stack[-1].complexType)):
            self.inElement = 0
            self.inNonanonymousComplexType = 0
            if len(self.stack) >= 2:
                element = self.stack.pop()
                self.stack[-1].addChild(element)
            if name == ElementType and len(self.stack) == 1:
                element.setIsRootElement(True)
        elif name == AnyType:
            if len(self.stack) >= 2:
                element = self.stack.pop()
                self.stack[-1].addChild(element)
        elif name == ComplexTypeType:
            self.inComplexType = 0
        elif name == SequenceType:
            self.inSequence = 0
            self.sequenceStack.pop()
        elif name == ChoiceType:
            self.currentChoice = None
            self.inChoice = 0
        elif name == AttributeType:
            self.inAttribute = 0
        elif name == AttributeGroupType:
            if self.attributeGroupLevel >= 2:
                # We are inside an attribute group.
                pass
            else:
                if self.stack[-1].attributeGroup:
                    # The top of the stack contains an XschemaElement which
                    #   contains the definition of an attributeGroup.
                    #   Save this attributeGroup in the
                    #   global AttributeGroup dictionary.
                    attributeGroup = self.stack[-1].attributeGroup
                    name = attributeGroup.getName()
                    AttributeGroups[name] = attributeGroup
                    self.stack[-1].attributeGroup = None
                    self.stack.pop()
                else:
                    # This is a reference to an attributeGroup.
                    # We have already added it to the list of
                    # attributeGroup names.
                    # Leave it.  We'll fill it in during annotate.
                    pass
            self.attributeGroupLevel -= 1
        elif name == GroupType:
            element = self.stack.pop()
            name = element.getAttrs()['name']
            elementGroup = XschemaGroup(element.name)
            ref = element.getAttrs().get('ref')
            if len(self.stack) == 1 and ref is None:
                # This is the definition
                ElementGroups[name] = element
            elif len(self.stack) > 1 and ref is not None:
                # This is a reference. Add it to the parent's children. We
                # need to preserve the order of elements.
                element.setElementGroup(elementGroup)
                self.stack[-1].addChild(element)
        elif name == SchemaType:
            self.inSchema = 0
            if len(self.stack) != 1:
                # fixlist
                err_msg('*** error stack.  len(self.stack): %d\n' % (
                    len(self.stack), ))
                sys.exit(1)
            if self.root:       # change made to avoide logging error
                logging.debug("Previous root: %s" % (self.root.name))
            else:
                logging.debug("Prvious root:   None")
            self.root = self.stack[0]
            if self.root:
                logging.debug("New root: %s" % (self.root.name))
            else:
                logging.debug("New root: None")
        elif name == SimpleContentType:
            self.inSimpleContent = 0
        elif name == ComplexContentType:
            pass
        elif name == ExtensionType:
            pass
        elif name == ListType:
            self.inListType = 0
        elif name == AnnotationType:
            self.inAnnotationType = 0
        elif name == DocumentationType:
            if self.inAnnotationType:
                self.inDocumentationType = 0

    def characters(self, chrs):
        if self.inDocumentationType:
            # If there is an annotation/documentation element, save it.
            if len(self.stack) > 1 and len(chrs) > 0:
                self.stack[-1].documentation += chrs
        elif self.inElement:
            pass
        elif self.inComplexType:
            pass
        elif self.inSequence:
            pass
        elif self.inChoice:
            pass


#
# Code generation
#

def generateExportFn_1(wrt, child, name, namespace, fill):
    cleanName = cleanupName(name)
    mappedName = mapName(cleanName)
    child_type = child.getType()
    if child_type == DateTimeType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_datetime(self.%s, " \
            "input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == DateType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_date(self.%s, " \
            "input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == TimeType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_time(self.%s, " \
            "input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    elif (child_type in StringType or
            child_type == TokenType or
            child_type in DateTimeGroupType):
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        # fixlist
        if (child.getSimpleType() in SimpleTypeDict and
                SimpleTypeDict[child.getSimpleType()].isListType()):
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_string(quote_xml" \
                "(' '.join(self.%s)), " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            namespace = 'namespace_'
            if child.prefix and 'ref' in child.attrs:
                namespace = "'%s:'" % child.prefix
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(%s, self.gds_format_string(quote_xml(self.%s)" \
                ", input_name='%s'), " \
                "%s, eol_))\n" % \
                (fill, name, name, namespace, mappedName, name, namespace, )
        wrt(s1)
    elif (child_type in IntegerType or
            child_type == PositiveIntegerType or
            child_type == NonPositiveIntegerType or
            child_type == NegativeIntegerType or
            child_type == NonNegativeIntegerType):
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_integer_list(self.%s, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_integer(self.%s, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == BooleanType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_boolean_list(" \
                "self.%s, input_name='%s'), " \
                "namespace_, eol_))\n" % (
                    fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_boolean(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % (
                    fill, name, name, mappedName, name, )
        wrt(s1)
    elif (child_type == FloatType or
            child_type == DecimalType):
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_float_list(self.%s, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_float(self.%s, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == DoubleType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_double_list(self.%s, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_double(self.%s, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == Base64Type:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_base64(self.%s, input_name='%s'), " \
            "namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    else:
        wrt("%s        if self.%s is not None:\n" % (fill, mappedName))
        # name_type_problem
        if False:        # name == child.getType():
            s1 = "%s            self.%s.export(outfile, level, " \
                "namespace_, pretty_print=pretty_print)\n" % \
                (fill, mappedName)
        else:
            namespace = 'namespace_'
            if child.prefix and 'ref' in child.attrs:
                namespace += "='%s:'" % child.prefix
            s1 = "%s            self.%s.export(outfile, level, %s, " \
                "name_='%s', pretty_print=pretty_print)\n" % \
                (fill, mappedName, namespace, name)
        wrt(s1)
# end generateExportFn_1


def generateExportFn_2(wrt, child, name, namespace, fill):
    cleanName = cleanupName(name)
    mappedName = mapName(cleanName)
    child_type = child.getType()
    # fix_simpletype
    wrt("%s    for %s_ in self.%s:\n" % (fill, cleanName, mappedName, ))
    if child_type == DateTimeType:
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_datetime(%s_, input_name='%s')" \
            ", namespace_, eol_))\n" % \
            (fill, name, name, cleanName, name, )
        wrt(s1)
    elif child_type == DateType:
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_date(%s_, input_name='%s'), " \
            "namespace_, eol_))\n" % \
            (fill, name, name, cleanName, name, )
        wrt(s1)
    elif child_type == TimeType:
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_time(%s_, input_name='%s'), " \
            "namespace_, eol_))\n" % \
            (fill, name, name, cleanName, name, )
        wrt(s1)
    elif (child_type in StringType or
            child_type == TokenType or
            child_type in DateTimeGroupType):
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        wrt("%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% "
            "(namespace_, self.gds_format_string(quote_xml(%s_)"
            ", input_name='%s'), namespace_, eol_))\n" %
            (fill, name, name, cleanName, name,))
    elif (child_type in IntegerType or
            child_type == PositiveIntegerType or
            child_type == NonPositiveIntegerType or
            child_type == NegativeIntegerType or
            child_type == NonNegativeIntegerType):
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_integer_list(" \
                "%s_, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        else:
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_integer(%s_, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        wrt(s1)
    elif child_type == BooleanType:
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_boolean_list(" \
                "%s_, input_name='%s'), " \
                "namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        else:
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_boolean(" \
                "%s_, input_name='%s'), " \
                "namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        wrt(s1)
    elif (child_type == FloatType or
            child_type == DecimalType):
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_float_list(%s_, " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        else:
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_float(" \
                "%s_, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        wrt(s1)
    elif child_type == DoubleType:
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_double_list(" \
                "%s_, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        else:
            s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_double(%s_, input_name='%s'), " \
                "namespace_, eol_))\n" % \
                (fill, name, name, cleanName, name, )
        wrt(s1)
    elif child_type == Base64Type:
        wrt('%s        showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s        outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_base64(%s_, input_name='%s'), " \
            "namespace_, eol_))\n" % \
            (fill, name, name, cleanName, name, )
        wrt(s1)
    else:
        # name_type_problem
        if False:        # name == child.getType():
            s1 = "%s        %s_.export(outfile, level, namespace_, " \
                "pretty_print=pretty_print)\n" % (fill, cleanName)
        else:
            namespace = 'namespace_'
            if child.prefix and 'ref' in child.attrs:
                namespace += "='%s:'" % child.prefix
            s1 = "%s        %s_.export(outfile, level, %s, " \
                "name_='%s', pretty_print=pretty_print)\n" % \
                (fill, cleanName, namespace, name)
        wrt(s1)
# end generateExportFn_2


def generateExportFn_3(wrt, child, name, namespace, fill):
    cleanName = cleanupName(name)
    mappedName = mapName(cleanName)
    child_type = child.getType()
    # fix_simpletype
    if child_type == DateTimeType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_datetime(" \
            "self.%s, input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == DateType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_date(" \
            "self.%s, input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == TimeType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_time(" \
            "self.%s, input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    elif (child_type in StringType or
            child_type == TokenType or
            child_type in DateTimeGroupType):
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        # fixlist
        if (child.getSimpleType() in SimpleTypeDict and
                SimpleTypeDict[child.getSimpleType()].isListType()):
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_string(" \
                "quote_xml(' '.join(self.%s)), " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_string(" \
                "quote_xml(self.%s), " \
                "input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif (child_type in IntegerType or
            child_type == PositiveIntegerType or
            child_type == NonPositiveIntegerType or
            child_type == NegativeIntegerType or
            child_type == NonNegativeIntegerType):
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_integer_list(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_integer(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == BooleanType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_boolean_list(" \
                "self.%s, input_name='%s'), " \
                "namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name)
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_boolean(" \
                "self.%s, input_name='%s'), " \
                "namespace_, eol_))\n" % (fill, name, name, mappedName, name)
        wrt(s1)
    elif (child_type == FloatType or
            child_type == DecimalType):
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_float_list(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_float(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == DoubleType:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        if child.isListType():
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_double_list(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        else:
            s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
                "(namespace_, self.gds_format_double(" \
                "self.%s, input_name='%s'), namespace_, eol_))\n" % \
                (fill, name, name, mappedName, name, )
        wrt(s1)
    elif child_type == Base64Type:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        wrt('%s            showIndent(outfile, level, pretty_print)\n' % fill)
        s1 = "%s            outfile.write('<%%s%s>%%s</%%s%s>%%s' %% " \
            "(namespace_, self.gds_format_base64(" \
            "self.%s, input_name='%s'), namespace_, eol_))\n" % \
            (fill, name, name, mappedName, name, )
        wrt(s1)
    else:
        wrt("%s        if self.%s is not None:\n" % (fill, mappedName))
        # name_type_problem
        if False:        # name == child.getType():
            s1 = "%s            self.%s.export(" \
                "outfile, level, namespace_, pretty_print=pretty_print)\n" % \
                (fill, mappedName)
        else:
            namespace = 'namespace_'
            if child.prefix and 'ref' in child.attrs:
                namespace += "='%s:'" % child.prefix
            s1 = "%s            self.%s.export(outfile, level, %s, " \
                "name_='%s', pretty_print=pretty_print)\n" % \
                (fill, mappedName, namespace, name)
        wrt(s1)
# end generateExportFn_3


def generateToEtree(wrt, element, Targetnamespace):
    name = element.getName()
    wrt("    def to_etree(self, parent_element=None, "
        "name_='%s', mapping_=None):\n"
        % (name,))
    parentName, parent = getParentName(element)
    if parentName and not element.getRestrictionBaseObj():
        elName = element.getCleanName()
        wrt("        element = super(%s, self).to_etree("
            "parent_element, name_, mapping_)\n" % (elName, ))
    else:
        wrt("        if parent_element is None:\n")
        wrt("            element = etree_.Element('{%s}' + name_)\n" % (
            Targetnamespace,))
        wrt("        else:\n")
        wrt("            element = etree_.SubElement(parent_element, "
            "'{%s}' + name_)\n" % (Targetnamespace,))
    if element.getExtended():
        wrt("        if self.extensiontype_ is not None:\n")
        wrt("            element.set("
            "'{http://www.w3.org/2001/XMLSchema-instance}type', "
            "self.extensiontype_)\n")
    generateToEtreeAttributes(wrt, element)
    generateToEtreeChildren(wrt, element, Targetnamespace)
    wrt("        if mapping_ is not None:\n")
    wrt("            mapping_[self] = element\n")
    wrt("        return element\n")
# end generateToEtree


def generateToEtreeChildren(wrt, element, Targetnamespace):
    if len(element.getChildren()) > 0:
        if element.isMixed():
            wrt("        for item_ in self.content_:\n")
            wrt("            item_.to_etree(element)\n")
        else:
            for child in element.getChildren():
                unmappedName = child.getName()
                name = child.getCleanName()
                child_type = child.getType()
                if child_type == AnyTypeIdentifier:
                    if child.getMaxOccurs() > 1:
                        wrt('        for obj_ in self.anytypeobjs_:\n')
                        wrt("            obj_.to_etree(element)\n")
                    else:
                        wrt('        if self.anytypeobjs_ is not None:\n')
                        wrt("            self.anytypeobjs_.to_etree("
                            "element)\n")
                else:
                    if child.getMaxOccurs() > 1:
                        wrt("        for %s_ in self.%s:\n" % (name, name,))
                    else:
                        wrt("        if self.%s is not None:\n" % (name,))
                        wrt("            %s_ = self.%s\n" % (name, name,))
                    if child_type == DateTimeType:
                        wrt("            etree_.SubElement("
                            "element, '{%s}%s').text = self."
                            "gds_format_datetime(%s_)\n" % (
                                Targetnamespace, unmappedName, name))
                    elif child_type == DateType:
                        wrt("            etree_.SubElement("
                            "element, '{%s}%s').text = self."
                            "gds_format_date(%s_)\n" % (
                                Targetnamespace, unmappedName, name))
                    elif child_type == TimeType:
                        wrt("            etree_.SubElement("
                            "element, '{%s}%s').text = self."
                            "gds_format_time(%s_)\n" % (
                                Targetnamespace, unmappedName, name))
                    elif (child_type in StringType or
                            child_type == TokenType or
                            child_type in DateTimeGroupType):
                        wrt("            etree_.SubElement(element, "
                            "'{%s}%s').text = "
                            "self.gds_format_string(%s_)\n" % (
                                Targetnamespace, unmappedName, name))
                    elif child_type in IntegerType or \
                            child_type == PositiveIntegerType or \
                            child_type == NonPositiveIntegerType or \
                            child_type == NegativeIntegerType or \
                            child_type == NonNegativeIntegerType:
                        if child.isListType():
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_integer_list"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                        else:
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_integer"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                    elif child_type == BooleanType:
                        if child.isListType():
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_boolean_list"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                        else:
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_boolean"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                    elif child_type == FloatType or \
                            child_type == DecimalType:
                        if child.isListType():
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_float_list"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                        else:
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_float"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                    elif child_type == DoubleType:
                        if child.isListType():
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_double_list"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                        else:
                            wrt("            etree_.SubElement(element, "
                                "'{%s}%s').text = self.gds_format_double"
                                "(%s_)\n" % (
                                    Targetnamespace, unmappedName, name))
                    elif child_type == Base64Type:
                        wrt("            etree_.SubElement(element, "
                            "'{%s}%s').text = self.gds_format_base64"
                            "(%s_)\n" % (
                                Targetnamespace, unmappedName, name))
                    else:
                        wrt("            %s_.to_etree(element, name_='%s', "
                            "mapping_=mapping_)\n" % (
                                name, unmappedName,))
#end generateToEtreeChildren


def generateToEtreeAttributes(wrt, element):
    attrDefs = element.getAttributeDefs()
    for key in attrDefs.keys():
        attrDef = attrDefs[key]
        name = attrDef.getName()
        cleanName = mapName(cleanupName(name))
        attrType = attrDef.getType()
        wrt("        if self.%s is not None:\n" % (cleanName, ))
        if (attrType in StringType or
                attrType in IDTypes or
                attrType == TokenType):
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_string(self.%s))\n''' % (name, cleanName, )
        elif (attrType in IntegerType or
                attrType == PositiveIntegerType or
                attrType == NonPositiveIntegerType or
                attrType == NegativeIntegerType or
                attrType == NonNegativeIntegerType):
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_integer(self.%s))\n''' % (
                    name, cleanName, )
        elif attrType == BooleanType:
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_boolean(self.%s))\n''' % (name, cleanName, )
        elif (attrType == FloatType or
                attrType == DecimalType):
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_float(self.%s))\n''' % (name, cleanName, )
        elif attrType == DoubleType:
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_double(self.%s))\n''' % (name, cleanName, )
        elif attrType == DateTimeType:
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_datetime(self.%s))\n''' % (
                    name, cleanName, )
        elif attrType == DateType:
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_date(self.%s))\n''' % (name, cleanName, )
        elif attrType == TimeType:
            s1 = '''            element.set('%s', ''' \
                '''self.gds_format_time(self.%s))\n''' % (name, cleanName, )
        else:
            s1 = '''            element.set('%s', self.%s)\n''' % (
                name, cleanName, )
        wrt(s1)
# end generateToEtreeAttributes


def generateExportAttributes(wrt, element, hasAttributes):
    if len(element.getAttributeDefs()) > 0:
        hasAttributes += 1
        attrDefs = element.getAttributeDefs()
        for key in attrDefs.keys():
            attrDef = attrDefs[key]
            name = attrDef.getName()
            orig_name = attrDef.getOrig_name()
            if orig_name is None:
                orig_name = name
            cleanName = mapName(cleanupName(name))
            if True:            # attrDef.getUse() == 'optional':
                wrt("        if self.%s is not None and '%s' not in "
                    "already_processed:\n" % (
                        cleanName, cleanName, ))
                wrt("            already_processed.add('%s')\n" % (
                    cleanName, ))
                indent = "    "
            else:
                indent = ""
            attrDefType = attrDef.getType()
            if attrDef.getType() == DateTimeType:
                s1 = '''%s        outfile.write(' %s="%%s"' %% ''' \
                    '''self.gds_format_datetime(self.%s, ''' \
                    '''input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name)
            elif attrDef.getType() == DateType:
                s1 = '''%s        outfile.write(' %s="%%s"' %% ''' \
                    '''self.gds_format_date(self.%s, input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name)
            elif attrDef.getType() == TimeType:
                s1 = '''%s        outfile.write(' %s="%%s"' %% ''' \
                    '''self.gds_format_time(self.%s, input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name)
            elif (attrDefType in StringType or
                    attrDefType in IDTypes or
                    attrDefType == TokenType or
                    attrDefType in DateTimeGroupType):
                s1 = '''%s        outfile.write(' %s=%%s' %% ''' \
                    '''(self.gds_format_string(quote_attrib(''' \
                    '''self.%s)''' \
                    ''', input_name='%s'), ))\n''' % \
                    (indent, orig_name, cleanName, name, )
            elif (attrDefType in IntegerType or
                    attrDefType == PositiveIntegerType or
                    attrDefType == NonPositiveIntegerType or
                    attrDefType == NegativeIntegerType or
                    attrDefType == NonNegativeIntegerType):
                s1 = '''%s        outfile.write(' %s="%%s"' %% ''' \
                    '''self.gds_format_integer(self.%s, ''' \
                    '''input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name, )
            elif attrDefType == BooleanType:
                s1 = '''%s        outfile.write(' %s="%%s"' %% ''' \
                    '''self.gds_format_boolean(''' \
                    '''self.%s, input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name, )
            elif attrDefType == FloatType or attrDefType == DecimalType:
                s1 = '''%s        outfile.write(' %s="%%s"' %% self.''' \
                    '''gds_format_float(self.%s, input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name)
            elif attrDefType == DoubleType:
                s1 = '''%s        outfile.write(' %s="%%s"' %% ''' \
                    '''self.gds_format_double(self.%s, ''' \
                    '''input_name='%s'))\n''' % (
                        indent, orig_name, cleanName, name)
            else:
                s1 = '''%s        outfile.write(' %s=%%s' %% ''' \
                    '''(quote_attrib(self.%s), ))\n''' % (
                        indent, orig_name, cleanName, )
            wrt(s1)
    if element.getExtended():
        wrt("        if self.extensiontype_ is not None and 'xsi:type' "
            "not in already_processed:\n")
        wrt("            already_processed.add('xsi:type')\n")
        wrt("            outfile.write("
            "' xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"')\n")
        wrt('''            outfile.write('''
            '''' xsi:type="%s"' % self.extensiontype_)\n''')
    return hasAttributes
# end generateExportAttributes


def generateExportChildren(wrt, element, hasChildren, namespace):
    fill = '        '
    if len(element.getChildren()) > 0:
        hasChildren += 1
        if element.isMixed():
            wrt('%sif not fromsubclass_:\n' % (fill, ))
            wrt("%s    for item_ in self.content_:\n" % (fill, ))
            wrt("%s        item_.export(outfile, level, item_.name, "
                "namespace_, pretty_print=pretty_print)\n" % (fill, ))
        else:
            wrt('%sif pretty_print:\n' % (fill, ))
            wrt("%s    eol_ = '\\n'\n" % (fill, ))
            wrt('%selse:\n' % (fill, ))
            wrt("%s    eol_ = ''\n" % (fill, ))
            any_type_child = None
            for child in element.getChildren():
                unmappedName = child.getName()
                name = child.getCleanName()
                # fix_abstract
                type_element = None
                abstract_child = False
                type_name = child.getAttrs().get('type')
                if type_name:
                    type_element = ElementDict.get(type_name)
                if type_element and type_element.isAbstract():
                    abstract_child = True
                if child.getType() == AnyTypeIdentifier:
                    any_type_child = child
                else:
                    if abstract_child and child.getMaxOccurs() > 1:
                        wrt("%sfor %s_ in self.%s:\n" % (
                            fill, name, name,))
                        wrt("%s    %s_.export(outfile, level, namespace_, "
                            "name_='%s', pretty_print=pretty_print)\n" % (
                                fill, name, name, ))
                    elif abstract_child:
                        wrt("%sif self.%s is not None:\n" % (fill, name, ))
                        wrt("%s    self.%s.export(outfile, level, "
                            "namespace_, name_='%s', "
                            "pretty_print=pretty_print)\n" % (
                                fill, name, name, ))
                    elif child.getMaxOccurs() > 1:
                        generateExportFn_2(
                            wrt, child, unmappedName, namespace, '    ')
                    else:
                        if (child.getOptional()):
                            generateExportFn_3(
                                wrt, child, unmappedName, namespace, '')
                        else:
                            generateExportFn_1(
                                wrt, child, unmappedName, namespace, '')
            if any_type_child is not None:
                if any_type_child.getMaxOccurs() > 1:
                    wrt('        for obj_ in self.anytypeobjs_:\n')
                    wrt("            obj_.export(outfile, level, "
                        "namespace_, pretty_print=pretty_print)\n")
                else:
                    wrt('        if self.anytypeobjs_ is not None:\n')
                    wrt("            self.anytypeobjs_.export(outfile, "
                        "level, namespace_, pretty_print=pretty_print)\n")
    return hasChildren
# end generateExportChildren


def countChildren(element, count):
    count += len(element.getChildren())
    base = element.getBase()
    if base and base in ElementDict:
        parent = ElementDict[base]
        count = countChildren(parent, count)
    return count


def getParentName(element):
    base = element.getBase()
    rBase = element.getRestrictionBaseObj()
    parentName = None
    parentObj = None
    if base and base in ElementDict:
        parentObj = ElementDict[base]
        parentName = parentObj.getCleanName()
    elif rBase:
        base = element.getRestrictionBase()
        parentObj = ElementDict[base]
        parentName = parentObj.getCleanName()
    return parentName, parentObj


def generateExportFn(wrt, prefix, element, namespace, nameSpacesDef):
    childCount = countChildren(element, 0)
    name = element.getName()
    base = element.getBase()
    wrt("    def export(self, outfile, level, namespace_='%s', "
        "name_='%s', namespacedef_='%s', pretty_print=True):\n" %
        (namespace, name, nameSpacesDef))
    wrt('        if pretty_print:\n')
    wrt("            eol_ = '\\n'\n")
    wrt('        else:\n')
    wrt("            eol_ = ''\n")
    # We need to be able to export the original tag name.
    wrt("        if self.original_tagname_ is not None:\n")
    wrt("            name_ = self.original_tagname_\n")
    wrt('        showIndent(outfile, level, pretty_print)\n')
    wrt("        outfile.write('<%s%s%s' % (namespace_, name_, "
        "namespacedef_ and ' ' + namespacedef_ or '', ))\n")
    wrt("        already_processed = set()\n")
    wrt("        self.exportAttributes(outfile, level, "
        "already_processed, namespace_, name_='%s')\n" %
        (name, ))
    # fix_abstract
    if base and base in ElementDict:
        base_element = ElementDict[base]
        # fix_derived
        if base_element.isAbstract():
            pass
    if childCount == 0 and element.isMixed():
        wrt("        outfile.write('>')\n")
        wrt("        self.exportChildren(outfile, level + 1, "
            "namespace_, name_, pretty_print=pretty_print)\n")
        wrt("        outfile.write('</%s%s>%s' % (namespace_, name_, eol_))\n")
    else:
        wrt("        if self.hasContent_():\n")
        # Added to keep value on the same line as the tag no children.
        if element.getSimpleContent():
            wrt("            outfile.write('>')\n")
            if not element.isMixed():
                wrt("            outfile.write(str(self.valueOf_)"
                    ")\n")
        else:
            wrt("            outfile.write('>%s' % (eol_, ))\n")
        wrt("            self.exportChildren(outfile, level + 1, "
            "namespace_='%s', name_='%s', pretty_print=pretty_print)\n" %
            (namespace, name))
        # Put a condition on the indent to require children.
        if childCount != 0:
            wrt('            showIndent(outfile, level, pretty_print)\n')
        wrt("            outfile.write('</%s%s>%s' % (namespace_, "
            "name_, eol_))\n")
        wrt("        else:\n")
        wrt("            outfile.write('/>%s' % (eol_, ))\n")
    wrt("    def exportAttributes(self, outfile, level, "
        "already_processed, namespace_='%s', name_='%s'):\n" %
        (namespace, name, ))
    hasAttributes = 0
    if element.getAnyAttribute():
        wrt("""\
        unique_counter = 0
        for name, value in self.anyAttributes_.items():
            xsinamespaceprefix = 'xsi'
            xsinamespace1 = 'http://www.w3.org/2001/XMLSchema-instance'
            xsinamespace2 = '{%s}' % (xsinamespace1, )
            if name.startswith(xsinamespace2):
                name1 = name[len(xsinamespace2):]
                name2 = '%s:%s' % (xsinamespaceprefix, name1, )
                if name2 not in already_processed:
                    already_processed.add(name2)
                    outfile.write(' %s=%s' % (name2, quote_attrib(value), ))
            else:
                mo = re_.match(Namespace_extract_pat_, name)
                if mo is not None:
                    namespace, name = mo.group(1, 2)
                    if name not in already_processed:
                        already_processed.add(name)
                        if namespace == 'http://www.w3.org/XML/1998/namespace':
                            outfile.write(' %s=%s' % (
                                name, quote_attrib(value), ))
                        else:
                            unique_counter += 1
                            outfile.write(' xmlns:yyy%d=\"%s\"' % (
                                unique_counter, namespace, ))
                            outfile.write(' yyy%d:%s=%s' % (
                                unique_counter, name, quote_attrib(value), ))
                else:
                    if name not in already_processed:
                        already_processed.add(name)
                        outfile.write(' %s=%s' % (
                            name, quote_attrib(value), ))\n""")
    parentName, parent = getParentName(element)
    if parentName:
        hasAttributes += 1
        elName = element.getCleanName()
        wrt("        super(%s, self).exportAttributes("
            "outfile, level, already_processed, namespace_, name_='%s')\n" %
            (elName, name, ))
    hasAttributes += generateExportAttributes(wrt, element, hasAttributes)
    if hasAttributes == 0:
        wrt("        pass\n")
    wrt("    def exportChildren(self, outfile, level, namespace_='%s', "
        "name_='%s', fromsubclass_=False, pretty_print=True):\n" %
        (namespace, name, ))
    hasChildren = 0
    # Generate call to exportChildren in the superclass only if it is
    #  an extension, but *not* if it is a restriction.
    parentName, parent = getParentName(element)
    if parentName and not element.getRestrictionBaseObj():
        hasChildren += 1
        elName = element.getCleanName()
        wrt("        super(%s, self).exportChildren(outfile, level, "
            "namespace_, name_, True, pretty_print=pretty_print)\n" %
            (elName, ))
    hasChildren += generateExportChildren(wrt, element, hasChildren, namespace)
    if childCount == 0:   # and not element.isMixed():
        wrt("        pass\n")
# end generateExportFn


#
# Generate exportLiteral method.
#

def generateExportLiteralFn_1(wrt, child, name, fill):
    cleanName = cleanupName(name)
    mappedName = mapName(cleanName)
    childType = child.getType()
    if childType == AnyTypeIdentifier:
        wrt('%s        if self.anytypeobjs_ is not None:\n' % (fill, ))
        wrt('%s            showIndent(outfile, level)\n' % fill)
        wrt("%s            outfile.write('anytypeobjs_=model_.anytypeobjs_("
            "\\n')\n" % (fill, ))
        wrt("%s            self.anytypeobjs_.exportLiteral(outfile, level)\n" %
            (fill, ))
        wrt('%s            showIndent(outfile, level)\n' % fill)
        wrt("%s            outfile.write('),\\n')\n" % (fill, ))
    else:
        wrt('%s        if self.%s is not None:\n' % (fill, mappedName, ))
        if childType == DateTimeType:
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s="
                "model_.GeneratedsSuper.gds_parse_datetime(\"%%s\"),\\n' %% "
                "self.gds_format_datetime(self.%s, input_name='%s'))\n" %
                (fill, name, mappedName, name, ))
        elif childType == DateType:
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s="
                "model_.GeneratedsSuper.gds_parse_date(\"%%s\"),\\n' %% "
                "self.gds_format_date(self.%s, input_name='%s'))\n" %
                (fill, name, mappedName, name, ))
        elif childType == TimeType:
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s="
                "model_.GeneratedsSuper.gds_parse_time(\"%%s\"),\\n' %% "
                "self.gds_format_time(self.%s, input_name='%s'))\n" %
                (fill, name, mappedName, name, ))
        elif (childType in StringType or
                childType in IDTypes or
                childType == TokenType or
                childType in DateTimeGroupType):
            wrt('%s            showIndent(outfile, level)\n' % fill)
            if (child.getSimpleType() in SimpleTypeDict and
                    SimpleTypeDict[child.getSimpleType()].isListType()):
                wrt("%s            if self.%s:\n" % (fill, mappedName, ))
                wrt("%s                outfile.write('%s=%%s,\\n' %% "
                    "quote_python(' '.join(self.%s))"
                    ") \n" %
                    (fill, mappedName, mappedName, ))
                wrt("%s            else:\n" % (fill, ))
                wrt("%s                outfile.write('%s=None,\\n')\n" %
                    (fill, mappedName, ))
            else:
                wrt("%s            outfile.write('%s=%%s,\\n' %% "
                    "quote_python(self.%s))\n" %
                    (fill, mappedName, mappedName, ))
        elif (childType in IntegerType or
                childType == PositiveIntegerType or
                childType == NonPositiveIntegerType or
                childType == NegativeIntegerType or
                childType == NonNegativeIntegerType):
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s=%%d,\\n' %% self.%s)\n" %
                (fill, mappedName, mappedName, ))
        elif childType == BooleanType:
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s=%%s,\\n' %% self.%s)\n" %
                (fill, mappedName, mappedName, ))
        elif (childType == FloatType or
                childType == DecimalType):
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s=%%f,\\n' %% self.%s)\n" %
                (fill, mappedName, mappedName, ))
        elif childType == DoubleType:
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s=%%e,\\n' %% self.%s)\n" %
                (fill, name, mappedName, ))
        else:
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('%s=model_.%s(\\n')\n" %
                (fill, mappedName, mapName(cleanupName(child.getType()))))
            if name == child.getType():
                s1 = "%s            self.%s.exportLiteral(" \
                    "outfile, level)\n" % \
                    (fill, mappedName)
            else:
                s1 = "%s            self.%s.exportLiteral(" \
                    "outfile, level, name_='%s')\n" % \
                    (fill, mappedName, name)
            wrt(s1)
            wrt('%s            showIndent(outfile, level)\n' % fill)
            wrt("%s            outfile.write('),\\n')\n" % (fill, ))
# end generateExportLiteralFn_1


def generateExportLiteralFn_2(wrt, child, name, fill):
    childType = child.getType()
    if childType == DateTimeType:
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write("
            "'model_.GeneratedsSuper.gds_parse_datetime"
            "(\"%%s\"),\\n' %% self.gds_format_datetime("
            "%s_, input_name='%s'))\n" % (fill, name, name, ))
    elif childType == DateType:
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('model_.GeneratedsSuper.gds_parse_date"
            "(\"%%s\"),\\n' %% self.gds_format_date("
            "%s_, input_name='%s'))\n" % (fill, name, name, ))
    elif childType == TimeType:
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('model_.GeneratedsSuper.gds_parse_time"
            "(\"%%s\"),\\n' %% self.gds_format_time("
            "%s_, input_name='%s'))\n" % (fill, name, name, ))
    elif (childType in StringType or
            childType == TokenType or
            childType in DateTimeGroupType):
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('%%s,\\n' %% quote_python(%s_)"
            ")\n" % (fill, name))
    elif (childType in IntegerType or
            childType == PositiveIntegerType or
            childType == NonPositiveIntegerType or
            childType == NegativeIntegerType or
            childType == NonNegativeIntegerType):
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('%%d,\\n' %% %s_)\n" % (fill, name))
    elif childType == BooleanType:
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('%%s,\\n' %% %s_)\n" % (fill, name))
    elif (childType == FloatType or
            childType == DecimalType):
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('%%f,\\n' %% %s_)\n" % (fill, name))
    elif childType == DoubleType:
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('%%e,\\n' %% %s_)\n" % (fill, name))
    else:
        wrt('%s        showIndent(outfile, level)\n' % fill)
        name1 = mapName(cleanupName(child.getType()))
        wrt("%s        outfile.write('model_.%s(\\n')\n" % (fill, name1, ))
        if name == child.getType():
            s1 = "%s        %s_.exportLiteral(outfile, level)\n" % (
                fill, cleanupName(child.getType()), )
        else:
            s1 = "%s        %s_.exportLiteral(outfile, level, name_='%s')\n" \
                % (fill, name, child.getType(), )
        wrt(s1)
        wrt('%s        showIndent(outfile, level)\n' % fill)
        wrt("%s        outfile.write('),\\n')\n" % (fill, ))
# end generateExportLiteralFn_2


def generateExportLiteralFn(wrt, prefix, element):
    wrt("    def exportLiteral(self, outfile, level, name_='%s'):\n" % (
        element.getName(), ))
    wrt("        level += 1\n")
    wrt("        already_processed = set()\n")
    wrt("        self.exportLiteralAttributes(outfile, level, "
        "already_processed, name_)\n")
    wrt("        if self.hasContent_():\n")
    wrt("            self.exportLiteralChildren(outfile, level, name_)\n")
    childCount = countChildren(element, 0)
    if element.getSimpleContent() or element.isMixed():
        wrt("        showIndent(outfile, level)\n")
        wrt("        outfile.write('valueOf_ = \"\"\"%s\"\"\",\\n' % "
            "(self.valueOf_,))\n")
    wrt("    def exportLiteralAttributes(self, outfile, level, "
        "already_processed, name_):\n")
    count = 0
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        count += 1
        name = attrDef.getName()
        cleanName = cleanupName(name)
        mappedName = mapName(cleanName)
        attrType = attrDef.getType()
        if attrType in SimpleTypeDict:
            attrType = SimpleTypeDict[attrType].getBase()
        if attrType in SimpleTypeDict:
            attrType = SimpleTypeDict[attrType].getBase()
        wrt("        if self.%s is not None and '%s' not in "
            "already_processed:\n" % (
                mappedName, mappedName, ))
        wrt("            already_processed.add('%s')\n" % (
            mappedName, ))
        if attrType == DateTimeType:
            wrt('            showIndent(outfile, level)\n')
            wrt("            outfile.write('%s="
                "model_.GeneratedsSuper.gds_parse_datetime(\"%%s\"),\\n' %% "
                "self.gds_format_datetime(self.%s, input_name='%s'))\n" %
                (name, mappedName, name, ))
        elif attrType == DateType:
            wrt('            showIndent(outfile, level)\n')
            wrt("            outfile.write('%s="
                "model_.GeneratedsSuper.gds_parse_date(\"%%s\"),\\n' %% "
                "self.gds_format_date(self.%s, input_name='%s'))\n" %
                (name, mappedName, name, ))
        elif attrType == TimeType:
            wrt('            showIndent(outfile, level)\n')
            wrt("            outfile.write('%s="
                "model_.GeneratedsSuper.gds_parse_time(\"%%s\"),\\n' %% "
                "self.gds_format_time(self.%s, input_name='%s'))\n" %
                (name, mappedName, name, ))
        elif (attrType in StringType or
                attrType in IDTypes or
                attrType == TokenType or
                attrType == NCNameType):
            wrt("            showIndent(outfile, level)\n")
            wrt("            outfile.write('%s=\"%%s\",\\n' %% "
                "(self.%s,))\n" %
                (mappedName, mappedName,))
        elif (attrType in IntegerType or
                attrType == PositiveIntegerType or
                attrType == NonPositiveIntegerType or
                attrType == NegativeIntegerType or
                attrType == NonNegativeIntegerType):
            wrt("            showIndent(outfile, level)\n")
            wrt("            outfile.write('%s=%%d,\\n' %% (self.%s,))\n" %
                (mappedName, mappedName,))
        elif attrType == BooleanType:
            wrt("            showIndent(outfile, level)\n")
            wrt("            outfile.write('%s=%%s,\\n' %% (self.%s,))\n" %
                (mappedName, mappedName,))
        elif (attrType == FloatType or
                attrType == DecimalType):
            wrt("            showIndent(outfile, level)\n")
            wrt("            outfile.write('%s=%%f,\\n' %% (self.%s,))\n" %
                (mappedName, mappedName,))
        elif attrType == DoubleType:
            wrt("            showIndent(outfile, level)\n")
            wrt("            outfile.write('%s=%%e,\\n' %% (self.%s,))\n" %
                (mappedName, mappedName,))
        else:
            wrt("            showIndent(outfile, level)\n")
            wrt("            outfile.write('%s=%%s,\\n' %% (self.%s,))\n" %
                (mappedName, mappedName,))
    if element.getAnyAttribute():
        count += 1
        wrt('        for name, value in self.anyAttributes_.items():\n')
        wrt('            showIndent(outfile, level)\n')
        wrt("            outfile.write('%s=\"%s\",\\n' % (name, value,))\n")
    parentName, parent = getParentName(element)
    if parentName:
        count += 1
        elName = element.getCleanName()
        wrt("        super(%s, self).exportLiteralAttributes("
            "outfile, level, already_processed, name_)\n" %
            (elName, ))
    if count == 0:
        wrt("        pass\n")
    wrt("    def exportLiteralChildren(self, outfile, level, name_):\n")
    parentName, parent = getParentName(element)
    if parentName:
        elName = element.getCleanName()
        wrt("        super(%s, self).exportLiteralChildren("
            "outfile, level, name_)\n" %
            (elName, ))
    for child in element.getChildren():
        name = child.getName()
        name = cleanupName(name)
        mappedName = mapName(name)
        if element.isMixed():
            wrt("        showIndent(outfile, level)\n")
            wrt("        outfile.write('content_ = [\\n')\n")
            wrt('        for item_ in self.content_:\n')
            wrt('            item_.exportLiteral(outfile, level, name_)\n')
            wrt("        showIndent(outfile, level)\n")
            wrt("        outfile.write('],\\n')\n")
        else:
            # fix_abstract
            type_element = None
            abstract_child = False
            type_name = child.getAttrs().get('type')
            if type_name:
                type_element = ElementDict.get(type_name)
            if type_element and type_element.isAbstract():
                abstract_child = True
            if abstract_child:
                pass
            else:
                type_name = name
            if child.getMaxOccurs() > 1:
                if child.getType() == AnyTypeIdentifier:
                    wrt("        showIndent(outfile, level)\n")
                    wrt("        outfile.write('anytypeobjs_=[\\n')\n")
                    wrt("        level += 1\n")
                    wrt("        for anytypeobjs_ in self.anytypeobjs_:\n")
                    wrt("            anytypeobjs_.exportLiteral("
                        "outfile, level)\n")
                    wrt("        level -= 1\n")
                    wrt("        showIndent(outfile, level)\n")
                    wrt("        outfile.write('],\\n')\n")
                else:
                    wrt("        showIndent(outfile, level)\n")
                    wrt("        outfile.write('%s=[\\n')\n" % (mappedName, ))
                    wrt("        level += 1\n")
                    wrt("        for %s_ in self.%s:\n" % (name, mappedName))
                    generateExportLiteralFn_2(wrt, child, name, '    ')
                    wrt("        level -= 1\n")
                    wrt("        showIndent(outfile, level)\n")
                    wrt("        outfile.write('],\\n')\n")
            else:
                generateExportLiteralFn_1(wrt, child, type_name, '')
    if childCount == 0 or element.isMixed():
        wrt("        pass\n")
# end generateExportLiteralFn

#
# Generate build method.
#


def generateBuildAttributes(wrt, element, hasAttributes):
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        hasAttributes += 1
        name = attrDef.getName()
        orig_name = attrDef.getOrig_name()
        if orig_name is None:
            orig_name = name
        cleanName = cleanupName(name)
        mappedName = mapName(cleanName)
        atype = attrDef.getType()
        if atype in SimpleTypeDict:
            atype = SimpleTypeDict[atype].getBase()
        if atype == DateTimeType:
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt('            try:\n')
            wrt("                self.%s = self.gds_parse_datetime("
                "value)\n" % (mappedName, ))
            wrt('            except ValueError, exp:\n')
            wrt("                raise ValueError("
                "'Bad date-time attribute (%s): %%s' %% exp)\n" %
                (name, ))
        elif atype == DateType:
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt('            try:\n')
            wrt("                self.%s = self.gds_parse_date("
                "value)\n" % (mappedName, ))
            wrt('            except ValueError, exp:\n')
            wrt("                raise ValueError("
                "'Bad date attribute (%s): %%s' %% exp)\n" %
                (name, ))
        elif atype == TimeType:
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt('            try:\n')
            wrt("                self.%s = self.gds_parse_time("
                "value)\n" % (mappedName, ))
            wrt('            except ValueError, exp:\n')
            wrt("                raise ValueError("
                "'Bad time attribute (%s): %%s' %% exp)\n" %
                (name, ))
        elif (atype in IntegerType or
                atype == PositiveIntegerType or
                atype == NonPositiveIntegerType or
                atype == NegativeIntegerType or
                atype == NonNegativeIntegerType):
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt('            try:\n')
            wrt("                self.%s = int(value)\n" % (mappedName, ))
            wrt('            except ValueError, exp:\n')
            wrt("                raise_parse_error("
                "node, 'Bad integer attribute: %s' % exp)\n")
            if atype == PositiveIntegerType:
                wrt('            if self.%s <= 0:\n' % mappedName)
                wrt("                raise_parse_error("
                    "node, 'Invalid PositiveInteger')\n")
            elif atype == NonPositiveIntegerType:
                wrt('            if self.%s > 0:\n' % mappedName)
                wrt("                raise_parse_error("
                    "node, 'Invalid NonPositiveInteger')\n")
            elif atype == NegativeIntegerType:
                wrt('            if self.%s >= 0:\n' % mappedName)
                wrt("                raise_parse_error("
                    "node, 'Invalid NegativeInteger')\n")
            elif atype == NonNegativeIntegerType:
                wrt('            if self.%s < 0:\n' % mappedName)
                wrt("                raise_parse_error("
                    "node, 'Invalid NonNegativeInteger')\n")
        elif atype == BooleanType:
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt("            if value in ('true', '1'):\n")
            wrt("                self.%s = True\n" % mappedName)
            wrt("            elif value in ('false', '0'):\n")
            wrt("                self.%s = False\n" % mappedName)
            wrt('            else:\n')
            wrt("                raise_parse_error("
                "node, 'Bad boolean attribute')\n")
        elif atype == FloatType or atype == DoubleType or atype == DecimalType:
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt('            try:\n')
            wrt("                self.%s = float(value)\n" %
                (mappedName, ))
            wrt('            except ValueError, exp:\n')
            wrt("                raise ValueError('Bad float/double "
                "attribute (%s): %%s' %% exp)\n" %
                (name, ))
        elif atype == TokenType:
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt("            self.%s = value\n" % (mappedName, ))
            wrt("            self.%s = ' '.join(self.%s.split())\n" %
                (mappedName, mappedName, ))
        else:
            # Assume attr['type'] in StringType or attr['type'] == DateTimeType
            wrt("        value = find_attr_value_('%s', node)\n" % (
                orig_name, ))
            wrt("        if value is not None and '%s' not in "
                "already_processed:\n" %
                (name, ))
            wrt("            already_processed.add('%s')\n" % (name, ))
            wrt("            self.%s = value\n" % (mappedName, ))
        typeName = attrDef.getType()
        if typeName and typeName in SimpleTypeDict:
            wrt("            self.validate_%s(self.%s)    "
                "# validate type %s\n" %
                (typeName, mappedName, typeName, ))
    if element.getAnyAttribute():
        hasAttributes += 1
        wrt('        self.anyAttributes_ = {}\n')
        wrt('        for name, value in attrs.items():\n')
        wrt("            if name not in already_processed:\n")
        wrt('                self.anyAttributes_[name] = value\n')
    if element.getExtended():
        hasAttributes += 1
        wrt("        value = find_attr_value_('xsi:type', node)\n")
        wrt("        if value is not None and 'xsi:type' not in "
            "already_processed:\n")
        wrt("            already_processed.add('xsi:type')\n")
        wrt("            self.extensiontype_ = value\n")
    return hasAttributes
# end generateBuildAttributes


def generateBuildMixed_1(wrt, prefix, child, headChild, keyword, delayed):
    origName = child.getName()
    name = child.getCleanName()
    childType = child.getType()
    mappedName = mapName(name)
    if (childType in StringType or
            childType == TokenType or
            childType in DateTimeGroupType):
        wrt("        %s nodeName_ == '%s' and child_.text is not None:\n" % (
            keyword, origName, ))
        wrt("            valuestr_ = child_.text\n")
        if childType == TokenType:
            wrt('            valuestr_ = re_.sub(String_cleanup_pat_, '
                '" ", valuestr_).strip()\n')
        wrt("            obj_ = self.mixedclass_("
            "MixedContainer.CategorySimple,\n")
        wrt("                MixedContainer.TypeString, '%s', valuestr_)\n" %
            origName)
        wrt("            self.content_.append(obj_)\n")
    elif (childType in IntegerType or
            childType == PositiveIntegerType or
            childType == NonPositiveIntegerType or
            childType == NegativeIntegerType or
            childType == NonNegativeIntegerType):
        wrt("        %s nodeName_ == '%s' and child_.text is not None:\n" % (
            keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            try:\n")
        wrt("                ival_ = int(sval_)\n")
        wrt("            except (TypeError, ValueError) as exp:\n")
        wrt("                raise_parse_error(child_, "
            "'requires integer: %s' % exp)\n")
        if childType == PositiveIntegerType:
            wrt("            if ival_ <= 0:\n")
            wrt("                raise_parse_error(child_, "
                "'Invalid positiveInteger')\n")
        if childType == NonPositiveIntegerType:
            wrt("            if ival_ > 0:\n")
            wrt("                raise_parse_error(child_, "
                "'Invalid nonPositiveInteger)\n")
        if childType == NegativeIntegerType:
            wrt("            if ival_ >= 0:\n")
            wrt("                raise_parse_error(child_, "
                "'Invalid negativeInteger')\n")
        if childType == NonNegativeIntegerType:
            wrt("            if ival_ < 0:\n")
            wrt("                raise_parse_error(child_, "
                "'Invalid nonNegativeInteger')\n")
        wrt("            obj_ = self.mixedclass_("
            "MixedContainer.CategorySimple,\n")
        wrt("                MixedContainer.TypeInteger, '%s', ival_)\n" % (
            origName, ))
        wrt("            self.content_.append(obj_)\n")
    elif childType == BooleanType:
        wrt("        %s nodeName_ == '%s' and child_.text is not None:\n" % (
            keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            if sval_ in ('true', '1'):\n")
        wrt("                ival_ = True\n")
        wrt("            elif sval_ in ('false', '0'):\n")
        wrt("                ival_ = False\n")
        wrt("            else:\n")
        wrt("                raise_parse_error(child_, 'requires boolean')\n")
        wrt("        obj_ = self.mixedclass_(MixedContainer.CategorySimple,\n")
        wrt("            MixedContainer.TypeInteger, '%s', ival_)\n" %
            origName)
        wrt("        self.content_.append(obj_)\n")
    elif (childType == FloatType or
            childType == DoubleType or
            childType == DecimalType):
        wrt("        %s nodeName_ == '%s' and child_.text is not None:\n" % (
            keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            try:\n")
        wrt("                fval_ = float(sval_)\n")
        wrt("            except (TypeError, ValueError) as exp:\n")
        wrt("                raise_parse_error(child_, "
            "'requires float or double: %s' % exp)\n")
        wrt("            obj_ = self.mixedclass_("
            "MixedContainer.CategorySimple,\n")
        wrt("                MixedContainer.TypeFloat, '%s', fval_)\n" %
            origName)
        wrt("            self.content_.append(obj_)\n")
    elif childType == Base64Type:
        wrt("        %s nodeName_ == '%s' and child_.text is not None:\n" % (
            keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            try:\n")
        wrt("                bval_ = base64.b64decode(sval_)\n")
        wrt("            except (TypeError, ValueError) as exp:\n")
        wrt("                raise_parse_error(child_, "
            "'requires base64 encoded string: %s' % exp)\n")
        wrt("            obj_ = self.mixedclass_("
            "MixedContainer.CategorySimple,\n")
        wrt("                MixedContainer.TypeBase64, '%s', bval_)\n" %
            origName)
        wrt("            self.content_.append(obj_)\n")
    else:
        # Perhaps it's a complexType that is defined right here.
        # Generate (later) a class for the nested types.
        type_element = None
        abstract_child = False
        type_name = child.getAttrs().get('type')
        if type_name:
            type_element = ElementDict.get(type_name)
        if type_element and type_element.isAbstract():
            abstract_child = True
        if not delayed and not child in DelayedElements:
            DelayedElements.add(child)
            DelayedElements_subclass.add(child)
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        if abstract_child:
            wrt(TEMPLATE_ABSTRACT_CHILD % (mappedName, ))
        else:
            type_obj = ElementDict.get(childType)
            if type_obj is not None and type_obj.getExtended():
                wrt("            class_obj_ = self.get_class_obj_("
                    "child_, %s%s)\n" % (
                        prefix, cleanupName(mapName(childType)), ))
                wrt("            class_obj_ = %s%s.factory()\n")
            else:
                wrt("            obj_ = %s%s.factory()\n" % (
                    prefix, cleanupName(mapName(childType))))
            wrt("            obj_.build(child_)\n")

        wrt("            obj_ = self.mixedclass_("
            "MixedContainer.CategoryComplex,\n")
        wrt("                MixedContainer.TypeNone, '%s', obj_)\n" %
            origName)
        wrt("            self.content_.append(obj_)\n")

        # Generate code to sort mixed content in their class
        # containers
        s1 = "            if hasattr(self, 'add_%s'):\n" % (origName, )
        s1 += "              self.add_%s(obj_.value)\n" % (origName, )
        s1 += "            elif hasattr(self, 'set_%s'):\n" % (origName, )
        s1 += "              self.set_%s(obj_.value)\n" % (origName, )
        wrt(s1)
# end generateBuildMixed_1


def generateBuildMixed(wrt, prefix, element, keyword, delayed, hasChildren):
    for child in element.getChildren():
        generateBuildMixed_1(wrt, prefix, child, child, keyword, delayed)
        hasChildren += 1
        keyword = 'elif'
        # Does this element have a substitutionGroup?
        #   If so generate a clause for each element in the substitutionGroup.
        if child.getName() in SubstitutionGroups:
            for memberName in SubstitutionGroups[child.getName()]:
                if memberName in ElementDict:
                    member = ElementDict[memberName]
                    generateBuildMixed_1(
                        wrt, prefix, member, child, keyword, delayed)
    wrt("        if not fromsubclass_ and child_.tail is not None:\n")
    wrt("            obj_ = self.mixedclass_(MixedContainer.CategoryText,\n")
    wrt("                MixedContainer.TypeNone, '', child_.tail)\n")
    wrt("            self.content_.append(obj_)\n")
    return hasChildren


def generateBuildStandard_1(
        wrt, prefix, child, headChild, element, keyword, delayed):
    origName = child.getName()
    name = cleanupName(child.getName())
    mappedName = mapName(name)
    headName = cleanupName(headChild.getName())
    attrCount = len(child.getAttributeDefs())
    childType = child.getType()
    if childType == DateTimeType:
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            dval_ = self.gds_parse_datetime(sval_)\n")
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(dval_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = dval_\n" % (mappedName, ))
    elif childType == DateType:
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            dval_ = self.gds_parse_date(sval_)\n")
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(dval_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = dval_\n" % (mappedName, ))
    elif childType == TimeType:
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            dval_ = self.gds_parse_time(sval_)\n")
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(dval_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = dval_\n" % (mappedName, ))
    elif (attrCount == 0 and
            (childType in StringType or
                childType == TokenType or
                childType in DateTimeGroupType or
                child.isListType())):
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            %s_ = child_.text\n" % name)
        if childType == TokenType:
            wrt('            %s_ = re_.sub('
                'String_cleanup_pat_, " ", %s_).strip()\n' % (name, name))
        if child.isListType():
            if (childType in IntegerType or
                    childType == PositiveIntegerType or
                    childType == NonPositiveIntegerType or
                    childType == NegativeIntegerType or
                    childType == NonNegativeIntegerType):
                wrt("            %s_ = self.gds_validate_integer_list("
                    "%s_, node, '%s')\n" %
                    (name, name, name, ))
            elif childType == BooleanType:
                wrt("            %s_ = self.gds_validate_boolean_list("
                    "%s_, node, '%s')\n" %
                    (name, name, name, ))
            elif (childType == FloatType or
                    childType == DecimalType):
                wrt("            %s_ = self.gds_validate_float_list("
                    "%s_, node, '%s')\n" %
                    (name, name, name, ))
            elif childType == DoubleType:
                wrt("            %s_ = self.gds_validate_double_list("
                    "%s_, node, '%s')\n" %
                    (name, name, name, ))
        else:
            wrt("            %s_ = self.gds_validate_string("
                "%s_, node, '%s')\n" %
                (name, name, name, ))
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(%s_)\n" % (mappedName, name, ))
        else:
            wrt("            self.%s = %s_\n" % (mappedName, name, ))
    elif (childType in IntegerType or
            childType == PositiveIntegerType or
            childType == NonPositiveIntegerType or
            childType == NegativeIntegerType or
            childType == NonNegativeIntegerType):
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            try:\n")
        wrt("                ival_ = int(sval_)\n")
        wrt("            except (TypeError, ValueError) as exp:\n")
        wrt("                raise_parse_error(child_, "
            "'requires integer: %s' % exp)\n")
        if childType == PositiveIntegerType:
            wrt("            if ival_ <= 0:\n")
            wrt("                raise_parse_error("
                "child_, 'requires positiveInteger')\n")
        elif childType == NonPositiveIntegerType:
            wrt("            if ival_ > 0:\n")
            wrt("                raise_parse_error("
                "child_, 'requires nonPositiveInteger')\n")
        elif childType == NegativeIntegerType:
            wrt("            if ival_ >= 0:\n")
            wrt("                raise_parse_error("
                "child_, 'requires negativeInteger')\n")
        elif childType == NonNegativeIntegerType:
            wrt("            if ival_ < 0:\n")
            wrt("                raise_parse_error("
                "child_, 'requires nonNegativeInteger')\n")
        wrt("            ival_ = self.gds_validate_integer("
            "ival_, node, '%s')\n" %
            (name, ))
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(ival_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = ival_\n" % (mappedName, ))
    elif childType == BooleanType:
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            if sval_ in ('true', '1'):\n")
        wrt("                ival_ = True\n")
        wrt("            elif sval_ in ('false', '0'):\n")
        wrt("                ival_ = False\n")
        wrt("            else:\n")
        wrt("                raise_parse_error(child_, 'requires boolean')\n")
        wrt("            ival_ = self.gds_validate_boolean(ival_, "
            "node, '%s')\n" %
            (name, ))
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(ival_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = ival_\n" % (mappedName, ))
    elif (childType == FloatType or
            childType == DoubleType or
            childType == DecimalType):
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            try:\n")
        wrt("                fval_ = float(sval_)\n")
        wrt("            except (TypeError, ValueError) as exp:\n")
        wrt("                raise_parse_error("
            "child_, 'requires float or double: %s' % exp)\n")
        wrt("            fval_ = self.gds_validate_float("
            "fval_, node, '%s')\n" %
            (name, ))
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(fval_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = fval_\n" % (mappedName, ))
    elif childType == Base64Type:
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        wrt("            sval_ = child_.text\n")
        wrt("            if sval_ is not None:\n")
        wrt("                try:\n")
        wrt("                    bval_ = base64.b64decode(sval_)\n")
        wrt("                except (TypeError, ValueError) as exp:\n")
        wrt("                    raise_parse_error(child_, "
            "'requires base64 encoded string: %s' % exp)\n")
        wrt("                bval_ = self.gds_validate_base64("
            "bval_, node, '%s')\n" %
            (name, ))
        wrt("            else:\n")
        wrt("                bval_ = None\n")
        if child.getMaxOccurs() > 1:
            wrt("            self.%s.append(bval_)\n" % (mappedName, ))
        else:
            wrt("            self.%s = bval_\n" % (mappedName, ))
    else:
        # Perhaps it's a complexType that is defined right here.
        # Generate (later) a class for the nested types.
        # fix_abstract
        type_element = None
        abstract_child = False
        type_name = child.getAttrs().get('type')
        if type_name:
            type_element = ElementDict.get(type_name)
        if type_element and type_element.isAbstract():
            abstract_child = True
        if not delayed and not child in DelayedElements:
            DelayedElements.add(child)
            DelayedElements_subclass.add(child)
        wrt("        %s nodeName_ == '%s':\n" % (keyword, origName, ))
        # Is this a simple type?
        if child.getSimpleType():
            wrt("            obj_ = None\n")
        else:
            # name_type_problem
            # fix_abstract
            if type_element:
                type_name = type_element.getType()
            elif origName in ElementDict:
                type_name = ElementDict[origName].getType()
            else:
                type_name = childType
            type_name = cleanupName(mapName(type_name))
            if abstract_child:
                wrt(TEMPLATE_ABSTRACT_CHILD % (mappedName, ))
            else:
                type_obj = ElementDict.get(type_name)
                if type_obj is not None and type_obj.getExtended():
                    wrt("            class_obj_ = self.get_class_obj_("
                        "child_, %s%s)\n" %
                        (prefix, type_name, ))
                    wrt("            obj_ = class_obj_.factory()\n")
                else:
                    wrt("            obj_ = %s%s.factory()\n" % (
                        prefix, type_name, ))
                wrt("            obj_.build(child_)\n")
        if headChild.getMaxOccurs() > 1:
            substitutionGroup = child.getAttrs().get('substitutionGroup')
            if substitutionGroup is not None:
                _, name = get_prefix_and_value(substitutionGroup)
            else:
                name = mappedName
            s1 = "            self.%s.append(obj_)\n" % (name, )
        else:
            substitutionGroup = child.getAttrs().get('substitutionGroup')
            if substitutionGroup is not None:
                name = substitutionGroup
            else:
                name = headName
            s1 = "            self.%s = obj_\n" % (mappedName, )
        wrt(s1)
        wrt("            obj_.original_tagname_ = '%s'\n" % (origName, ))
    #
    # If this child is defined in a simpleType, then generate
    #   a validator method.
    typeName = None
    if child.getSimpleType():
        #typeName = child.getSimpleType()
        typeName = cleanupName(child.getName())
    elif (childType in ElementDict and
            ElementDict[childType].getSimpleType()):
        typeName = ElementDict[childType].getType()
    # fixlist
    if (child.getSimpleType() in SimpleTypeDict and
            SimpleTypeDict[child.getSimpleType()].isListType()):
        wrt("            self.%s = self.%s.split()\n" % (
            mappedName, mappedName, ))
    typeName = child.getSimpleType()
    if typeName and typeName in SimpleTypeDict:
        wrt("            self.validate_%s(self.%s)    # validate type %s\n" % (
            typeName, mappedName, typeName, ))
# end generateBuildStandard_1


def transitiveClosure(m, e):
    t = []
    if e in m:
        t += m[e]
        for f in m[e]:
            t += transitiveClosure(m, f)
    return t


def generateBuildStandard(wrt, prefix, element, keyword, delayed, hasChildren):
    any_type_child = None
    for child in element.getChildren():
        if child.getType() == AnyTypeIdentifier:
            any_type_child = child
        else:
            generateBuildStandard_1(
                wrt, prefix, child, child,
                element, keyword, delayed)
            hasChildren += 1
            keyword = 'elif'
            # Does this element have a substitutionGroup?
            #   If so generate a clause for each element in the
            #   substitutionGroup.
            childName = child.getName()
            if childName in SubstitutionGroups:
                for memberName in transitiveClosure(
                        SubstitutionGroups, childName):
                    memberName = cleanupName(memberName)
                    if memberName in ElementDict:
                        member = ElementDict[memberName]
                        generateBuildStandard_1(
                            wrt, prefix, member, child,
                            element, keyword, delayed)
    if any_type_child is not None:
        type_name = element.getType()
        if any_type_child.getMaxOccurs() > 1:
            if keyword == 'if':
                fill = ''
            else:
                fill = '    '
                wrt("        else:\n")
            wrt("        %sobj_ = self.gds_build_any(child_, '%s')\n" % (
                fill, type_name, ))
            wrt("        %sif obj_ is not None:\n" % (fill, ))
            wrt('            %sself.add_anytypeobjs_(obj_)\n' % (fill, ))
        else:
            if keyword == 'if':
                fill = ''
            else:
                fill = '    '
                wrt("        else:\n")
            wrt("        %sobj_ = self.gds_build_any(child_, '%s')\n" % (
                fill, type_name, ))
            wrt("        %sif obj_ is not None:\n" % (fill, ))
            wrt('            %sself.set_anytypeobjs_(obj_)\n' % (fill, ))
        hasChildren += 1
    return hasChildren
# end generateBuildStandard


def generateBuildFn(wrt, prefix, element, delayed):
    base = element.getBase()
    wrt('    def build(self, node):\n')
    wrt('        already_processed = set()\n')
    wrt('        self.buildAttributes(node, node.attrib, already_processed)\n')
    if element.isMixed() or element.getSimpleContent():
        wrt("        self.valueOf_ = get_all_text_(node)\n")
    if element.isMixed():
        wrt("        if node.text is not None:\n")
        wrt("            obj_ = self.mixedclass_("
            "MixedContainer.CategoryText,\n")
        wrt("                MixedContainer.TypeNone, '', node.text)\n")
        wrt("            self.content_.append(obj_)\n")
    wrt('        for child in node:\n')
    wrt("            if isinstance(child.tag, str):\n")
    wrt("                nodeName_ = Tag_pattern_.match(child.tag).groups()[-1]\n")
    wrt("                self.buildChildren(child, node, nodeName_)\n")
    wrt('        return self\n')
    wrt('    def buildAttributes(self, node, attrs, already_processed):\n')
    hasAttributes = 0
    hasAttributes = generateBuildAttributes(wrt, element, hasAttributes)
    parentName, parent = getParentName(element)
    if parentName:
        hasAttributes += 1
        elName = element.getCleanName()
        wrt('        super(%s, self).buildAttributes('
            'node, attrs, already_processed)\n' %
            (elName, ))
    if hasAttributes == 0:
        wrt('        pass\n')
    wrt('    def buildChildren(self, child_, node, nodeName_, '
        'fromsubclass_=False):\n')
    keyword = 'if'
    hasChildren = 0
    if element.isMixed():
        hasChildren = generateBuildMixed(
            wrt, prefix, element, keyword, delayed, hasChildren)
    else:      # not element.isMixed()
        hasChildren = generateBuildStandard(
            wrt, prefix, element, keyword, delayed, hasChildren)
    # Generate call to buildChildren in the superclass only if it is
    #  an extension, but *not* if it is a restriction.
    base = element.getBase()
    if base and not element.getSimpleContent():
        elName = element.getCleanName()
        wrt("        super(%s, self).buildChildren("
            "child_, node, nodeName_, True)\n" % (elName, ))
    if hasChildren == 0:
        wrt("        pass\n")
# end generateBuildFn


def countElementChildren(element, count):
    count += len(element.getChildren())
    base = element.getBase()
    if base and base in ElementDict:
        parent = ElementDict[base]
        countElementChildren(parent, count)
    return count


def buildCtorArgs_multilevel(element, childCount):
    content = []
    addedArgs = {}
    add = content.append
    buildCtorArgs_multilevel_aux(addedArgs, add, element)
    eltype = element.getType()
    if (element.getSimpleContent() or
            element.isMixed() or
            eltype in SimpleTypeDict or
            CurrentNamespacePrefix + eltype in OtherSimpleTypes):
        add(", valueOf_=None")
    if element.isMixed():
        add(', mixedclass_=None')
        add(', content_=None')
    if element.getExtended():
        add(', extensiontype_=None')
    s1 = ''.join(content)
    return s1


def buildCtorArgs_multilevel_aux(addedArgs, add, element):
    parentName, parentObj = getParentName(element)
    if parentName:
        buildCtorArgs_multilevel_aux(addedArgs, add, parentObj)
    buildCtorArgs_aux(addedArgs, add, element)


def buildCtorArgs_aux(addedArgs, add, element):
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        name = attrDef.getName()
        default = attrDef.getDefault()
        mappedName = name.replace(':', '_')
        mappedName = cleanupName(mapName(mappedName))
        if mappedName in addedArgs:
            continue
        addedArgs[mappedName] = 1
        try:
            atype = attrDef.getData_type()
        except KeyError:
            atype = StringType
        if (atype in StringType or
                atype == TokenType or
                atype in DateTimeGroupType):
            if default is None:
                add(", %s=None" % mappedName)
            else:
                default1 = escape_string(default)
                add(", %s='%s'" % (mappedName, default1))
        elif atype in IntegerType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(', %s=%s' % (mappedName, default))
        elif atype == PositiveIntegerType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(', %s=%s' % (mappedName, default))
        elif atype == NonPositiveIntegerType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(', %s=%s' % (mappedName, default))
        elif atype == NegativeIntegerType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(', %s=%s' % (mappedName, default))
        elif atype == NonNegativeIntegerType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(', %s=%s' % (mappedName, default))
        elif atype == BooleanType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                if default in ('false', '0'):
                    add(', %s=%s' % (mappedName, "False"))
                else:
                    add(', %s=%s' % (mappedName, "True"))
        elif atype == FloatType or atype == DoubleType or atype == DecimalType:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(', %s=%s' % (mappedName, default))
        else:
            if default is None:
                add(', %s=None' % mappedName)
            else:
                add(", %s='%s'" % (mappedName, default, ))
    for child in element.getChildren():
        cleanName = child.getCleanName()
        if cleanName in addedArgs:
            continue
        addedArgs[cleanName] = 1
        default = child.getDefault()
        if child.getType() == AnyTypeIdentifier:
            add(', anytypeobjs_=None')
        elif child.getMaxOccurs() > 1:
            add(', %s=None' % cleanName)
        else:
            childType = child.getType()
            if (childType in StringType or
                    childType == TokenType or
                    childType == Base64Type or
                    childType in DateTimeGroupType):
                if default is None:
                    add(", %s=None" % cleanName)
                else:
                    default1 = escape_string(default)
                    add(", %s='%s'" % (cleanName, default1, ))
            elif (childType in IntegerType or
                    childType == PositiveIntegerType or
                    childType == NonPositiveIntegerType or
                    childType == NegativeIntegerType or
                    childType == NonNegativeIntegerType):
                if default is None:
                    add(', %s=None' % cleanName)
                else:
                    add(', %s=%s' % (cleanName, default, ))
            elif childType == BooleanType:
                if default is None:
                    add(', %s=None' % cleanName)
                else:
                    if default in ('false', '0'):
                        add(', %s=%s' % (cleanName, "False", ))
                    else:
                        add(', %s=%s' % (cleanName, "True", ))
            elif (childType == FloatType or
                    childType == DoubleType or
                    childType == DecimalType):
                if default is None:
                    add(', %s=None' % cleanName)
                else:
                    add(', %s=%s' % (cleanName, default, ))
            else:
                add(', %s=None' % cleanName)
# end buildCtorArgs_aux


MixedCtorInitializers = '''\
        if mixedclass_ is None:
            self.mixedclass_ = MixedContainer
        else:
            self.mixedclass_ = mixedclass_
        if content_ is None:
            self.content_ = []
        else:
            self.content_ = content_
        self.valueOf_ = valueOf_
'''


def generateCtor(wrt, element):
    elName = element.getCleanName()
    childCount = countChildren(element, 0)
    s2 = buildCtorArgs_multilevel(element, childCount)
    wrt('    def __init__(self%s):\n' % s2)
    # Save the original tag name.  This is needed when there is a
    # xs:substitutionGroup and we later (e.g. during export) do not know
    # which member of the xs:substitutionGroup this specific element
    # came from.
    wrt('        self.original_tagname_ = None\n')
    parentName, parent = getParentName(element)
    if parentName:
        if parentName in AlreadyGenerated:
            args = buildCtorParams(element, parent, childCount)
            s2 = ''.join(args)
            if len(args) > 254:
                wrt('        arglist_ = (%s)\n' % (s2, ))
                wrt('        super(%s, self).__init__(*arglist_)\n' %
                    (elName, ))
            else:
                wrt('        super(%s, self).__init__(%s)\n' % (elName, s2, ))
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        mappedName = cleanupName(attrDef.getName())
        name = mapName(mappedName)
        attrType = attrDef.getType()
        if attrType == DateTimeType:
            wrt("        if isinstance(%s, basestring):\n" % (name, ))
            wrt("            initvalue_ = datetime_.datetime.strptime("
                "%s, '%%Y-%%m-%%dT%%H:%%M:%%S')\n" % (name, ))
            wrt("        else:\n")
            wrt("            initvalue_ = %s\n" % (name, ))
            wrt("        self.%s = initvalue_\n" % (name, ))
        elif attrType == DateType:
            wrt("        if isinstance(%s, basestring):\n" % (name, ))
            wrt("            initvalue_ = datetime_.datetime.strptime("
                "%s, '%%Y-%%m-%%d').date()\n" % (name, ))
            wrt("        else:\n")
            wrt("            initvalue_ = %s\n" % (name, ))
            wrt("        self.%s = initvalue_\n" % (name, ))
        elif attrType == TimeType:
            wrt("        if isinstance(%s, basestring):\n" % (name, ))
            wrt("            initvalue_ = datetime_.datetime.strptime("
                "%s, '%%H:%%M:%%S').time()\n" % (name, ))
            wrt("        else:\n")
            wrt("            initvalue_ = %s\n" % (name, ))
            wrt("        self.%s = initvalue_\n" % (name, ))
        else:
            pythonType = SchemaToPythonTypeMap.get(attrDef.getType())
            attrVal = "_cast(%s, %s)" % (pythonType, name, )
            wrt('        self.%s = %s\n' % (name, attrVal, ))
    # Generate member initializers in ctor.
    for child in element.getChildren():
        name = cleanupName(child.getCleanName())
        logging.debug("Constructor child: %s" % name)
        logging.debug("Dump: %s" % child.__dict__)
        childType = child.getType()
        if childType == AnyTypeIdentifier:
            if child.getMaxOccurs() > 1:
                wrt('        if anytypeobjs_ is None:\n')
                wrt('            self.anytypeobjs_ = []\n')
                wrt('        else:\n')
                wrt('            self.anytypeobjs_ = anytypeobjs_\n')
            else:
                wrt('        self.anytypeobjs_ = anytypeobjs_\n')
        elif childType == DateTimeType and child.getMaxOccurs() <= 1:
            wrt("        if isinstance(%s, basestring):\n" % (name, ))
            wrt("            initvalue_ = datetime_.datetime.strptime("
                "%s, '%%Y-%%m-%%dT%%H:%%M:%%S')\n" % (name, ))
            wrt("        else:\n")
            wrt("            initvalue_ = %s\n" % (name, ))
            if child.getMaxOccurs() > 1:
                wrt("        self.%s.append(initvalue_)\n" % (name, ))
            else:
                wrt("        self.%s = initvalue_\n" % (name, ))
        elif childType == DateType and child.getMaxOccurs() <= 1:
            wrt("        if isinstance(%s, basestring):\n" % (name, ))
            wrt("            initvalue_ = datetime_.datetime.strptime("
                "%s, '%%Y-%%m-%%d').date()\n" % (name, ))
            wrt("        else:\n")
            wrt("            initvalue_ = %s\n" % (name, ))
            if child.getMaxOccurs() > 1:
                wrt("        self.%s.append(initvalue_)\n" % (name, ))
            else:
                wrt("        self.%s = initvalue_\n" % (name, ))
        elif childType == TimeType and child.getMaxOccurs() <= 1:
            wrt("        if isinstance(%s, basestring):\n" % (name, ))
            wrt("            initvalue_ = datetime_.datetime.strptime("
                "%s, '%%H:%%M:%%S').time()\n" % (name, ))
            wrt("        else:\n")
            wrt("            initvalue_ = %s\n" % (name, ))
            if child.getMaxOccurs() > 1:
                wrt("        self.%s.append(initvalue_)\n" % (name, ))
            else:
                wrt("        self.%s = initvalue_\n" % (name, ))
        else:
            if child.getMaxOccurs() > 1:
                wrt('        if %s is None:\n' % (name, ))
                wrt('            self.%s = []\n' % (name, ))
                wrt('        else:\n')
                wrt('            self.%s = %s\n' % (name, name))
            elif child.getMaxOccurs() == 1 and child.complex == 1:
                wrt('        if %s is None:\n' % (name,))
                wrt("            self.%s = %s()\n" %
                    (name, child.getType()))
                wrt('        else:\n')
                wrt('            self.%s = %s\n' % (name, name))
            else:
                typeObj = ElementDict.get(child.getType())
                if (child.getDefault() and
                        typeObj is not None and
                        typeObj.getSimpleContent()):
                    wrt('        if %s is None:\n' % (name, ))
                    wrt("            self.%s = globals()['%s']('%s')\n" %
                        (name, child.getType(), child.getDefault(), ))
                    wrt('        else:\n')
                    wrt('            self.%s = %s\n' % (name, name))
                else:
                    wrt('        self.%s = %s\n' % (name, name))
    eltype = element.getType()
    if (element.getSimpleContent() or
            element.isMixed() or
            eltype in SimpleTypeDict or
            CurrentNamespacePrefix + eltype in OtherSimpleTypes):
        wrt('        self.valueOf_ = valueOf_\n')
    if element.getAnyAttribute():
        wrt('        self.anyAttributes_ = {}\n')
    if element.getExtended():
        wrt('        self.extensiontype_ = extensiontype_\n')
    if element.isMixed():
        wrt(MixedCtorInitializers)
# end generateCtor


#
# Attempt to retrieve the body (implementation) of a validator
#   from a directory containing one file for each simpleType.
#   The name of the file should be the same as the name of the
#   simpleType with and optional ".py" extension.
def getValidatorBody(stName):
    retrieved = 0
    if ValidatorBodiesBasePath:
        found = 0
        path = '%s%s%s.py' % (ValidatorBodiesBasePath, os.sep, stName, )
        if os.path.exists(path):
            found = 1
        else:
            path = '%s%s%s' % (ValidatorBodiesBasePath, os.sep, stName, )
            if os.path.exists(path):
                found = 1
        if found:
            infile = open(path, 'r')
            lines = infile.readlines()
            infile.close()
            lines1 = []
            for line in lines:
                if not line.startswith('##'):
                    lines1.append(line)
            s1 = ''.join(lines1)
            retrieved = 1
    if not retrieved:
        s1 = '        pass\n'
    return s1
# end getValidatorBody


# Generate get/set/add/insert member functions.
def generateGettersAndSetters(wrt, element):
    for child in element.getChildren():
        if child.getType() == AnyTypeIdentifier:
            wrt('    def get_anytypeobjs_(self): return self.anytypeobjs_\n')
            wrt('    def set_anytypeobjs_('
                'self, anytypeobjs_): self.anytypeobjs_ = anytypeobjs_\n')
            if child.getMaxOccurs() > 1:
                wrt('    def add_anytypeobjs_(self, value): '
                    'self.anytypeobjs_.append(value)\n')
                wrt('    def insert_anytypeobjs_(self, index, value): '
                    'self._anytypeobjs_[index] = value\n')
        else:
            name = cleanupName(child.getCleanName())
            unmappedName = cleanupName(child.getName())
            capName = make_gs_name(unmappedName)
            wrt('    def get%s(self): return self.%s\n' % (capName, name))
            wrt('    def set%s(self, %s): self.%s = %s\n' %
                (capName, name, name, name))
            if child.getMaxOccurs() > 1:
                wrt('    def add%s(self, value): self.%s.append(value)\n' %
                    (capName, name))
                wrt('    def insert%s(self, index, value): '
                    'self.%s[index] = value\n' %
                    (capName, name))
            if GenerateProperties:
                wrt('    %sProp = property(get%s, set%s)\n' %
                    (unmappedName, capName, capName))
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        name = cleanupName(attrDef.getName().replace(':', '_'))
        mappedName = mapName(name)
        wrt('    def get%s(self): return self.%s\n' %
            (make_gs_name(name), mappedName))
        gsName = make_gs_name(name)
        wrt('    def set%s(self, %s): self.%s = %s\n' % (
            gsName, mappedName, mappedName, mappedName))
        if GenerateProperties:
            wrt('    %sProp = property(get%s, set%s)\n' %
                (name, gsName, gsName))
    if element.getSimpleContent() or element.isMixed():
        wrt('    def get%s_(self): return self.valueOf_\n' % (
            make_gs_name('valueOf'), ))
        wrt('    def set%s_(self, valueOf_): self.valueOf_ = valueOf_\n' % (
            make_gs_name('valueOf'), ))
    if element.getAnyAttribute():
        wrt('    def get%s_(self): return self.anyAttributes_\n' % (
            make_gs_name('anyAttributes'), ))
        wrt('    def set%s_(self, anyAttributes_): '
            'self.anyAttributes_ = anyAttributes_\n' %
            (make_gs_name('anyAttributes'), ))
    if element.getExtended():
        wrt('    def get%s_(self): return self.extensiontype_\n' % (
            make_gs_name('extensiontype'), ))
        wrt('    def set%s_(self, extensiontype_): '
            'self.extensiontype_ = extensiontype_\n' %
            (make_gs_name('extensiontype'), ))
# end generateGettersAndSetters


# Generate validator methods.
def generateValidatorDefs(wrt, element):
    generatedSimpleTypes = []
    for child in element.getChildren():
        #
        # If this child is defined in a simpleType, then generate
        #   a validator method.
        typeName = child.getSimpleType()
        if (typeName and
                typeName in SimpleTypeDict and
                typeName not in generatedSimpleTypes):
            generatedSimpleTypes.append(typeName)
            wrt('    def validate_%s(self, value):\n' % (typeName, ))
            if typeName in SimpleTypeDict:
                stObj = SimpleTypeDict[typeName]
                wrt('        # Validate type %s, a restriction '
                    'on %s.\n' %
                    (typeName, stObj.getBase(), ))
            else:
                wrt('        # validate type %s\n' % (typeName, ))
            wrt(getValidatorBody(typeName))
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        typeName = attrDef.getType()
        if (typeName and
                typeName in SimpleTypeDict and
                typeName not in generatedSimpleTypes):
            generatedSimpleTypes.append(typeName)
            wrt('    def validate_%s(self, value):\n' % (typeName, ))
            if typeName in SimpleTypeDict:
                stObj = SimpleTypeDict[typeName]
                wrt('        # Validate type %s, a restriction on %s.\n' % (
                    typeName, stObj.getBase(), ))
            else:
                wrt('        # validate type %s\n' % (typeName, ))
            wrt(getValidatorBody(typeName))
# end generateValidatorDefs


#
# Generate a class variable whose value is a list of tuples, one
#   tuple for each member data item of the class.
#   Each tuble has 3 elements: (1) member name, (2) member data type,
#   (3) container/list or not (maxoccurs > 1).
def generateMemberSpec(wrt, element):
    generateDict = MemberSpecs and MemberSpecs == 'dict'
    if generateDict:
        content = ['    member_data_items_ = {']
    else:
        content = ['    member_data_items_ = [']
    add = content.append
    for attrName, attrDef in element.getAttributeDefs().items():
        item1 = attrName
        item2 = attrDef.getType()
        item3 = 0
        if generateDict:
            item = "        '%s': MemberSpec_('%s', '%s', %d)," % (
                item1, item1, item2, item3, )
        else:
            item = "        MemberSpec_('%s', '%s', %d)," % (
                item1, item2, item3, )
        add(item)
    for child in element.getChildren():
        name = cleanupName(child.getCleanName())
        item1 = name
        simplebase = child.getSimpleBase()
        if simplebase:
            if len(simplebase) == 1:
                item2 = "'%s'" % (simplebase[0], )
            else:
                item2 = simplebase
        else:
            element1 = ElementDict.get(name)
            if element1:
                item2 = "'%s'" % element1.getType()
            else:
                item2 = "'%s'" % (child.getType(), )
        if child.getMaxOccurs() > 1:
            item3 = 1
        else:
            item3 = 0
        if generateDict:
            item = "        '%s': MemberSpec_('%s', %s, %d)," % (
                item1, item1, item2, item3, )
        else:
            #item = "        ('%s', '%s', %d)," % (item1, item2, item3, )
            item = "        MemberSpec_('%s', %s, %d)," % (
                item1, item2, item3, )
        add(item)
    simplebase = element.getSimpleBase()
    if element.getSimpleContent() or element.isMixed():
        if len(simplebase) == 1:
            simplebase = "'%s'" % (simplebase[0], )
        if generateDict:
            item = "        'valueOf_': MemberSpec_('valueOf_', %s, 0)," % (
                simplebase, )
        else:
            item = "        MemberSpec_('valueOf_', %s, 0)," % (
                simplebase, )
        add(item)
    elif element.isMixed():
        if generateDict:
            item = "        'valueOf_': MemberSpec_('valueOf_', '%s', 0)," % (
                'xs:string', )
        else:
            item = "        MemberSpec_('valueOf_', '%s', 0)," % (
                'xs:string', )
        add(item)
    if generateDict:
        add('    }')
    else:
        add('    ]')
    wrt('\n'.join(content))
    wrt('\n')
# end generateMemberSpec


def generateUserMethods(wrt, element):
    if not UserMethodsModule:
        return
    specs = UserMethodsModule.METHOD_SPECS
    name = cleanupName(element.getCleanName())
    values_dict = {'class_name': name, }
    for spec in specs:
        if spec.match_name(name):
            source = spec.get_interpolated_source(values_dict)
            wrt(source)


def generateHascontentMethod(wrt, element):
    wrt('    def hasContent_(self):\n')
    wrt('        if (\n')
    firstTime = True
    for child in element.getChildren():
        if child.getType() == AnyTypeIdentifier:
            name = 'anytypeobjs_'
        else:
            name = child.getCleanName()
        if not firstTime:
            wrt(' or\n')
        firstTime = False
        if child.getMaxOccurs() > 1:
            wrt('            self.%s' % (name, ))
        else:
            wrt('            self.%s is not None' % (name, ))
    if element.getSimpleContent() or element.isMixed():
        if not firstTime:
            wrt(' or\n')
        firstTime = False
        wrt('            self.valueOf_')
    parentName, parent = getParentName(element)
    if parentName:
        elName = element.getCleanName()
        if not firstTime:
            wrt(' or\n')
        firstTime = False
        wrt('            super(%s, self).hasContent_()' % (elName, ))
    wrt('\n        ):\n')
    wrt('            return True\n')
    wrt('        else:\n')
    wrt('            return False\n')


def generateClasses(wrt, prefix, element, delayed, nameSpacesDef=''):
    logging.debug("Generating class for: %s" % element)
    mappedName = element.getCleanName()
    parentName, base = getParentName(element)
    logging.debug("Element base: %s" % base)
    if not element.isExplicitDefine():
        logging.debug("Not an explicit define, returning.")
        if element.isComplex() and element.getName() != element.getType():
            MappingTypes[element.getName()] = element.getType()
        return
    # If this element is an extension (has a base) and the base has
    #   not been generated, then postpone it.
    if parentName:
        if parentName not in AlreadyGenerated:
            PostponedExtensions.append(element)
            return
    if mappedName in AlreadyGenerated:
        return
    AlreadyGenerated.add(mappedName)
    if element.getMixedExtensionError():
        err_msg('*** Element %s extension chain contains mixed and '
                'non-mixed content.  Not generated.\n' %
                (element.getName(), ))
        return
    ElementsForSubclasses.append(element)
    name = element.getCleanName()
    if parentName:
        s1 = 'class %s%s(%s):\n' % (prefix, name, parentName,)
    else:
        s1 = 'class %s%s(GeneratedsSuper):\n' % (prefix, name)
    wrt(s1)
    # If this element has documentation, generate a doc-string.
    if element.documentation:
        s2 = ' '.join(element.documentation.strip().split())
        s2 = s2.encode('utf-8')
        s2 = textwrap.fill(s2, width=68, subsequent_indent='    ')
        if s2[0] == '"' or s2[-1] == '"':
            s2 = '    """ %s """\n' % (s2, )
        else:
            s2 = '    """%s"""\n' % (s2, )
        wrt(s2)
    if UserMethodsModule or MemberSpecs:
        generateMemberSpec(wrt, element)
    wrt('    subclass = None\n')
    parentName, parent = getParentName(element)
    superclass_name = 'None'
    if parentName and parentName in AlreadyGenerated:
        superclass_name = parentName
    wrt('    superclass = %s\n' % (superclass_name, ))
    generateCtor(wrt, element)
    wrt('    def factory(*args_, **kwargs_):\n')
    wrt('        if %s%s.subclass:\n' % (prefix, name))
    wrt('            return %s%s.subclass(*args_, **kwargs_)\n' % (
        prefix, name))
    wrt('        else:\n')
    wrt('            return %s%s(*args_, **kwargs_)\n' % (prefix, name))
    wrt('    factory = staticmethod(factory)\n')
    if UseGetterSetter != 'none':
        generateGettersAndSetters(wrt, element)
    generateValidatorDefs(wrt, element)
    if element.namespace:
        namespace = element.namespace
    elif element.targetNamespace in NamespacesDict:
        namespace = NamespacesDict[element.targetNamespace]
    elif Targetnamespace in NamespacesDict:
        namespace = NamespacesDict[Targetnamespace]
    else:
        namespace = ''
    generateHascontentMethod(wrt, element)
    if ExportWrite:
        generateExportFn(wrt, prefix, element, namespace, nameSpacesDef)
    if ExportEtree:
        generateToEtree(wrt, element, Targetnamespace)
    if ExportLiteral:
        generateExportLiteralFn(wrt, prefix, element)
    generateBuildFn(wrt, prefix, element, delayed)
    generateUserMethods(wrt, element)
    wrt('# end class %s\n' % name)
    wrt('\n\n')
# end generateClasses


TEMPLATE_HEADER = """\
#!/usr/bin/env python
# -*- coding: utf-8 -*-

#
# Generated %s by generateDS.py%s.
#
# Command line options:
%s
#
# Command line arguments:
#   %s
#
# Command line:
#   %s
#
# Current working directory (os.getcwd()):
#   %s
#

import sys
import getopt
import re as re_
import base64
import datetime as datetime_

from py_2_and_3_compatibility import *

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
# User methods
#
# Calls to the methods in these classes are generated by generateDS.py.
# You can replace these methods by re-implementing the following class
#   in a module named generatedssuper.py.

try:
    from generatedssuper import GeneratedsSuper
except ImportError as exp:

    class GeneratedsSuper(object):
        tzoff_pattern = re_.compile(r'(\+|-)((0\d|1[0-3]):[0-5]\d|14:00)$')
        class _FixedOffsetTZ(datetime_.tzinfo):
            def __init__(self, offset, name):
                self.__offset = datetime_.timedelta(minutes=offset)
                self.__name = name
            def utcoffset(self, dt):
                return self.__offset
            def tzname(self, dt):
                return self.__name
            def dst(self, dt):
                return None
        def gds_format_string(self, input_data, input_name=''):
            return input_data
        def gds_validate_string(self, input_data, node, input_name=''):
            if not input_data:
                return ''
            else:
                return input_data
        def gds_format_base64(self, input_data, input_name=''):
            return base64.b64encode(input_data)
        def gds_validate_base64(self, input_data, node, input_name=''):
            return input_data
        def gds_format_integer(self, input_data, input_name=''):
            return '%%d' %% input_data
        def gds_validate_integer(self, input_data, node, input_name=''):
            return input_data
        def gds_format_integer_list(self, input_data, input_name=''):
            return '%%s' %% input_data
        def gds_validate_integer_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                try:
                    float(value)
                except (TypeError, ValueError):
                    raise_parse_error(node, 'Requires sequence of integers')
            return input_data
        def gds_format_float(self, input_data, input_name=''):
            return ('%%.15f' %% input_data).rstrip('0')
        def gds_validate_float(self, input_data, node, input_name=''):
            return input_data
        def gds_format_float_list(self, input_data, input_name=''):
            return '%%s' %% input_data
        def gds_validate_float_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                try:
                    float(value)
                except (TypeError, ValueError):
                    raise_parse_error(node, 'Requires sequence of floats')
            return input_data
        def gds_format_double(self, input_data, input_name=''):
            return '%%e' %% input_data
        def gds_validate_double(self, input_data, node, input_name=''):
            return input_data
        def gds_format_double_list(self, input_data, input_name=''):
            return '%%s' %% input_data
        def gds_validate_double_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                try:
                    float(value)
                except (TypeError, ValueError):
                    raise_parse_error(node, 'Requires sequence of doubles')
            return input_data
        def gds_format_boolean(self, input_data, input_name=''):
            return ('%%s' %% input_data).lower()
        def gds_validate_boolean(self, input_data, node, input_name=''):
            return input_data
        def gds_format_boolean_list(self, input_data, input_name=''):
            return '%%s' %% input_data
        def gds_validate_boolean_list(self, input_data, node, input_name=''):
            values = input_data.split()
            for value in values:
                if value not in ('true', '1', 'false', '0', ):
                    raise_parse_error(
                        node,
                        'Requires sequence of booleans '
                        '("true", "1", "false", "0")')
            return input_data
        def gds_validate_datetime(self, input_data, node, input_name=''):
            return input_data
        def gds_format_datetime(self, input_data, input_name=''):
            if input_data.microsecond == 0:
                _svalue = '%%04d-%%02d-%%02dT%%02d:%%02d:%%02d' %% (
                    input_data.year,
                    input_data.month,
                    input_data.day,
                    input_data.hour,
                    input_data.minute,
                    input_data.second,
                )
            else:
                _svalue = '%%04d-%%02d-%%02dT%%02d:%%02d:%%02d.%%s' %% (
                    input_data.year,
                    input_data.month,
                    input_data.day,
                    input_data.hour,
                    input_data.minute,
                    input_data.second,
                    ('%%f' %% (float(input_data.microsecond) / 1000000))[2:],
                )
            if input_data.tzinfo is not None:
                tzoff = input_data.tzinfo.utcoffset(input_data)
                if tzoff is not None:
                    total_seconds = tzoff.seconds + (86400 * tzoff.days)
                    if total_seconds == 0:
                        _svalue += 'Z'
                    else:
                        if total_seconds < 0:
                            _svalue += '-'
                            total_seconds *= -1
                        else:
                            _svalue += '+'
                        hours = total_seconds // 3600
                        minutes = (total_seconds - (hours * 3600)) // 60
                        _svalue += '{0:02d}:{1:02d}'.format(hours, minutes)
            return _svalue
        @classmethod
        def gds_parse_datetime(cls, input_data):
            tz = None
            if input_data[-1] == 'Z':
                tz = GeneratedsSuper._FixedOffsetTZ(0, 'UTC')
                input_data = input_data[:-1]
            else:
                results = GeneratedsSuper.tzoff_pattern.search(input_data)
                if results is not None:
                    tzoff_parts = results.group(2).split(':')
                    tzoff = int(tzoff_parts[0]) * 60 + int(tzoff_parts[1])
                    if results.group(1) == '-':
                        tzoff *= -1
                    tz = GeneratedsSuper._FixedOffsetTZ(
                        tzoff, results.group(0))
                    input_data = input_data[:-6]
            if len(input_data.split('.')) > 1:
                dt = datetime_.datetime.strptime(
                    input_data, '%%Y-%%m-%%dT%%H:%%M:%%S.%%f')
            else:
                dt = datetime_.datetime.strptime(
                    input_data, '%%Y-%%m-%%dT%%H:%%M:%%S')
            dt = dt.replace(tzinfo=tz)
            return dt
        def gds_validate_date(self, input_data, node, input_name=''):
            return input_data
        def gds_format_date(self, input_data, input_name=''):
            _svalue = '%%04d-%%02d-%%02d' %% (
                input_data.year,
                input_data.month,
                input_data.day,
            )
            try:
                if input_data.tzinfo is not None:
                    tzoff = input_data.tzinfo.utcoffset(input_data)
                    if tzoff is not None:
                        total_seconds = tzoff.seconds + (86400 * tzoff.days)
                        if total_seconds == 0:
                            _svalue += 'Z'
                        else:
                            if total_seconds < 0:
                                _svalue += '-'
                                total_seconds *= -1
                            else:
                                _svalue += '+'
                            hours = total_seconds // 3600
                            minutes = (total_seconds - (hours * 3600)) // 60
                            _svalue += '{0:02d}:{1:02d}'.format(hours, minutes)
            except AttributeError:
                pass
            return _svalue
        @classmethod
        def gds_parse_date(cls, input_data):
            tz = None
            if input_data[-1] == 'Z':
                tz = GeneratedsSuper._FixedOffsetTZ(0, 'UTC')
                input_data = input_data[:-1]
            else:
                results = GeneratedsSuper.tzoff_pattern.search(input_data)
                if results is not None:
                    tzoff_parts = results.group(2).split(':')
                    tzoff = int(tzoff_parts[0]) * 60 + int(tzoff_parts[1])
                    if results.group(1) == '-':
                        tzoff *= -1
                    tz = GeneratedsSuper._FixedOffsetTZ(
                        tzoff, results.group(0))
                    input_data = input_data[:-6]
            dt = datetime_.datetime.strptime(input_data, '%%Y-%%m-%%d')
            dt = dt.replace(tzinfo=tz)
            return dt.date()
        def gds_validate_time(self, input_data, node, input_name=''):
            return input_data
        def gds_format_time(self, input_data, input_name=''):
            if input_data.microsecond == 0:
                _svalue = '%%02d:%%02d:%%02d' %% (
                    input_data.hour,
                    input_data.minute,
                    input_data.second,
                )
            else:
                _svalue = '%%02d:%%02d:%%02d.%%s' %% (
                    input_data.hour,
                    input_data.minute,
                    input_data.second,
                    ('%%f' %% (float(input_data.microsecond) / 1000000))[2:],
                )
            if input_data.tzinfo is not None:
                tzoff = input_data.tzinfo.utcoffset(input_data)
                if tzoff is not None:
                    total_seconds = tzoff.seconds + (86400 * tzoff.days)
                    if total_seconds == 0:
                        _svalue += 'Z'
                    else:
                        if total_seconds < 0:
                            _svalue += '-'
                            total_seconds *= -1
                        else:
                            _svalue += '+'
                        hours = total_seconds // 3600
                        minutes = (total_seconds - (hours * 3600)) // 60
                        _svalue += '{0:02d}:{1:02d}'.format(hours, minutes)
            return _svalue
        @classmethod
        def gds_parse_time(cls, input_data):
            tz = None
            if input_data[-1] == 'Z':
                tz = GeneratedsSuper._FixedOffsetTZ(0, 'UTC')
                input_data = input_data[:-1]
            else:
                results = GeneratedsSuper.tzoff_pattern.search(input_data)
                if results is not None:
                    tzoff_parts = results.group(2).split(':')
                    tzoff = int(tzoff_parts[0]) * 60 + int(tzoff_parts[1])
                    if results.group(1) == '-':
                        tzoff *= -1
                    tz = GeneratedsSuper._FixedOffsetTZ(
                        tzoff, results.group(0))
                    input_data = input_data[:-6]
            if len(input_data.split('.')) > 1:
                dt = datetime_.datetime.strptime(input_data, '%%H:%%M:%%S.%%f')
            else:
                dt = datetime_.datetime.strptime(input_data, '%%H:%%M:%%S')
            dt = dt.replace(tzinfo=tz)
            return dt.time()
        def gds_str_lower(self, instring):
            return instring.lower()
        def get_path_(self, node):
            path_list = []
            self.get_path_list_(node, path_list)
            path_list.reverse()
            path = '/'.join(path_list)
            return path
        Tag_strip_pattern_ = re_.compile(r'\{.*\}')
        def get_path_list_(self, node, path_list):
            if node is None:
                return
            tag = GeneratedsSuper.Tag_strip_pattern_.sub('', node.tag)
            if tag:
                path_list.append(tag)
            self.get_path_list_(node.getparent(), path_list)
        def get_class_obj_(self, node, default_class=None):
            class_obj1 = default_class
            if 'xsi' in node.nsmap:
                classname = node.get('{%%s}type' %% node.nsmap['xsi'])
                if classname is not None:
                    names = classname.split(':')
                    if len(names) == 2:
                        classname = names[1]
                    class_obj2 = globals().get(classname)
                    if class_obj2 is not None:
                        class_obj1 = class_obj2
            return class_obj1
        def gds_build_any(self, node, type_name=None):
            return None
        @classmethod
        def gds_reverse_node_mapping(cls, mapping):
            return dict(((v, k) for k, v in mapping.iteritems()))


#
# If you have installed IPython you can uncomment and use the following.
# IPython is available from http://ipython.scipy.org/.
#

## from IPython.Shell import IPShellEmbed
## args = ''
## ipshell = IPShellEmbed(args,
##     banner = 'Dropping into IPython',
##     exit_msg = 'Leaving Interpreter, back to program.')

# Then use the following line where and when you want to drop into the
# IPython shell:
#    ipshell('<some message> -- Entering ipshell.\\nHit Ctrl-D to exit')

#
# Globals
#

ExternalEncoding = '%s'
Tag_pattern_ = re_.compile(r'({.*})?(.*)')
String_cleanup_pat_ = re_.compile(r"[\\n\\r\\s]+")
Namespace_extract_pat_ = re_.compile(r'{(.*)}(.*)')

#
# Support/utility functions.
#


def showIndent(outfile, level, pretty_print=True):
    if pretty_print:
        for idx in range(level):
            outfile.write('    ')


def quote_xml(inStr):
    if inStr is None:
        return ''
    s1 = (isinstance(inStr, basestring) and inStr or
          '%%s' %% inStr)
    s1 = s1.replace('&', '&amp;')
    s1 = s1.replace('<', '&lt;')
    s1 = s1.replace('>', '&gt;')
    return s1


def quote_attrib(inStr):
    s1 = (isinstance(inStr, basestring) and inStr or
          '%%s' %% inStr)
    s1 = s1.replace('&', '&amp;')
    s1 = s1.replace('<', '&lt;')
    s1 = s1.replace('>', '&gt;')
    if '"' in s1:
        if "'" in s1:
            s1 = '"%%s"' %% s1.replace('"', "&quot;")
        else:
            s1 = "'%%s'" %% s1
    else:
        s1 = '"%%s"' %% s1
    return s1


def quote_python(inStr):
    s1 = inStr
    if s1.find("'") == -1:
        if s1.find('\\n') == -1:
            return "'%%s'" %% s1
        else:
            return "'''%%s'''" %% s1
    else:
        if s1.find('"') != -1:
            s1 = s1.replace('"', '\\\\"')
        if s1.find('\\n') == -1:
            return '"%%s"' %% s1
        else:
            return '\"\"\"%%s\"\"\"' %% s1


def get_all_text_(node):
    if node.text is not None:
        text = node.text
    else:
        text = ''
    for child in node:
        if child.tail is not None:
            text += child.tail
    return text


def find_attr_value_(attr_name, node):
    attrs = node.attrib
    attr_parts = attr_name.split(':')
    value = None
    if len(attr_parts) == 1:
        value = attrs.get(attr_name)
    elif len(attr_parts) == 2:
        prefix, name = attr_parts
        namespace = node.nsmap.get(prefix)
        if namespace is not None:
            value = attrs.get('{%%s}%%s' %% (namespace, name, ))
    return value


class GDSParseError(Exception):
    pass


def raise_parse_error(node, msg):
    if XMLParser_import_library == XMLParser_import_lxml:
        msg = '%%s (element %%s/line %%d)' %% (
            msg, node.tag, node.sourceline, )
    else:
        msg = '%%s (element %%s)' %% (msg, node.tag, )
    raise GDSParseError(msg)


class MixedContainer:
    # Constants for category:
    CategoryNone = 0
    CategoryText = 1
    CategorySimple = 2
    CategoryComplex = 3
    # Constants for content_type:
    TypeNone = 0
    TypeText = 1
    TypeString = 2
    TypeInteger = 3
    TypeFloat = 4
    TypeDecimal = 5
    TypeDouble = 6
    TypeBoolean = 7
    TypeBase64 = 8
    def __init__(self, category, content_type, name, value):
        self.category = category
        self.content_type = content_type
        self.name = name
        self.value = value
    def getCategory(self):
        return self.category
    def getContenttype(self, content_type):
        return self.content_type
    def getValue(self):
        return self.value
    def getName(self):
        return self.name
    def export(self, outfile, level, name, namespace, pretty_print=True):
        if self.category == MixedContainer.CategoryText:
            # Prevent exporting empty content as empty lines.
            if self.value.strip():
                outfile.write(self.value)
        elif self.category == MixedContainer.CategorySimple:
            self.exportSimple(outfile, level, name)
        else:    # category == MixedContainer.CategoryComplex
            self.value.export(outfile, level, namespace, name, pretty_print)
    def exportSimple(self, outfile, level, name):
        if self.content_type == MixedContainer.TypeString:
            outfile.write('<%%s>%%s</%%s>' %% (
                self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeInteger or \\
                self.content_type == MixedContainer.TypeBoolean:
            outfile.write('<%%s>%%d</%%s>' %% (
                self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeFloat or \\
                self.content_type == MixedContainer.TypeDecimal:
            outfile.write('<%%s>%%f</%%s>' %% (
                self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeDouble:
            outfile.write('<%%s>%%g</%%s>' %% (
                self.name, self.value, self.name))
        elif self.content_type == MixedContainer.TypeBase64:
            outfile.write('<%%s>%%s</%%s>' %% (
                self.name, base64.b64encode(self.value), self.name))
    def to_etree(self, element):
        if self.category == MixedContainer.CategoryText:
            # Prevent exporting empty content as empty lines.
            if self.value.strip():
                if len(element) > 0:
                    if element[-1].tail is None:
                        element[-1].tail = self.value
                    else:
                        element[-1].tail += self.value
                else:
                    if element.text is None:
                        element.text = self.value
                    else:
                        element.text += self.value
        elif self.category == MixedContainer.CategorySimple:
            subelement = etree_.SubElement(element, '%%s' %% self.name)
            subelement.text = self.to_etree_simple()
        else:    # category == MixedContainer.CategoryComplex
            self.value.to_etree(element)
    def to_etree_simple(self):
        if self.content_type == MixedContainer.TypeString:
            text = self.value
        elif (self.content_type == MixedContainer.TypeInteger or
                self.content_type == MixedContainer.TypeBoolean):
            text = '%%d' %% self.value
        elif (self.content_type == MixedContainer.TypeFloat or
                self.content_type == MixedContainer.TypeDecimal):
            text = '%%f' %% self.value
        elif self.content_type == MixedContainer.TypeDouble:
            text = '%%g' %% self.value
        elif self.content_type == MixedContainer.TypeBase64:
            text = '%%s' %% base64.b64encode(self.value)
        return text
    def exportLiteral(self, outfile, level, name):
        if self.category == MixedContainer.CategoryText:
            showIndent(outfile, level)
            outfile.write(
                'model_.MixedContainer(%%d, %%d, "%%s", "%%s"),\\n' %% (
                    self.category, self.content_type, self.name, self.value))
        elif self.category == MixedContainer.CategorySimple:
            showIndent(outfile, level)
            outfile.write(
                'model_.MixedContainer(%%d, %%d, "%%s", "%%s"),\\n' %% (
                    self.category, self.content_type, self.name, self.value))
        else:    # category == MixedContainer.CategoryComplex
            showIndent(outfile, level)
            outfile.write(
                'model_.MixedContainer(%%d, %%d, "%%s",\\n' %% (
                    self.category, self.content_type, self.name,))
            self.value.exportLiteral(outfile, level + 1)
            showIndent(outfile, level)
            outfile.write(')\\n')


class MemberSpec_(object):
    def __init__(self, name='', data_type='', container=0):
        self.name = name
        self.data_type = data_type
        self.container = container
    def set_name(self, name): self.name = name
    def get_name(self): return self.name
    def set_data_type(self, data_type): self.data_type = data_type
    def get_data_type_chain(self): return self.data_type
    def get_data_type(self):
        if isinstance(self.data_type, list):
            if len(self.data_type) > 0:
                return self.data_type[-1]
            else:
                return 'xs:string'
        else:
            return self.data_type
    def set_container(self, container): self.container = container
    def get_container(self): return self.container


def _cast(typ, value):
    if typ is None or value is None:
        return value
    return typ(value)

#
# Data representation classes.
#

"""

# Fool (and straighten out) the syntax highlighting.
# DUMMY = """


def format_options_args(options, args):
    options1 = '\n'.join(['#   %s' % (item, ) for item in options])
    args1 = '\n'.join(['#   %s' % (item, ) for item in args])
    program = sys.argv[0]
    options2 = ''
    for name, value in options:
        if value:
            if name.startswith('--'):
                options2 += ' %s="%s"' % (name, value, )
            else:
                options2 += ' %s "%s"' % (name, value, )
        else:
            options2 += ' %s' % name
    args1 = ' '.join(args)
    command_line = '%s%s %s' % (program, options2, args1, )
    return options1, args1, command_line


def generateHeader(wrt, prefix, options, args, externalImports):
    tstamp = (not NoDates and time.ctime()) or ''
    if NoVersion:
        version = ''
    else:
        version = ' version %s' % VERSION
    options1, args1, command_line = format_options_args(options, args)
    current_working_directory = os.path.split(os.getcwd())[1]
    s1 = TEMPLATE_HEADER % (
        tstamp, version,
        options1, args1,
        command_line, current_working_directory,
        ExternalEncoding, )
    wrt(s1)
    for externalImport in externalImports:
        wrt(externalImport + "\n")

    wrt("\n")


TEMPLATE_MAIN = """\
USAGE_TEXT = \"\"\"
Usage: python <%(prefix)sParser>.py [ -s ] <in_xml_file>
\"\"\"


def usage():
    print(USAGE_TEXT)
    sys.exit(1)


def get_root_tag(node):
    tag = Tag_pattern_.match(node.tag).groups()[-1]
    rootClass = GDSClassesMapping.get(tag)
    if rootClass is None:
        rootClass = globals().get(tag)
    return tag, rootClass


def parse(inFileName=None, silence=False, xml_string=None):
    if inFileName is not None:
        doc = parsexml_(inFileName)
        rootNode = doc.getroot()
    elif xml_string is not None:
        try:
            rootNode = etree_.fromstring(xml_string, parser=etree_.XMLParser(huge_tree=True))
        except TypeError as te:
            rootNode = etree_.fromstring(xml_string)
    else:
        raise Exception('Nothing to load supplied.')

    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = %(prefix)s%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
#silence#    if not silence:
#silence#        sys.stdout.write('<?xml version="1.0" ?>\\n')
#silence#        rootObj.export(
#silence#            sys.stdout, 0, name_=rootTag,
#silence#            namespacedef_='%(namespacedef)s',
#silence#            pretty_print=True)
    return rootObj


def parseEtree(inFileName, silence=False):
    doc = parsexml_(inFileName)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = %(prefix)s%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    mapping = {}
    rootElement = rootObj.to_etree(None, name_=rootTag, mapping_=mapping)
    reverse_mapping = rootObj.gds_reverse_node_mapping(mapping)
#silence#    if not silence:
#silence#        content = etree_.tostring(
#silence#            rootElement, pretty_print=True,
#silence#            xml_declaration=True, encoding="utf-8")
#silence#        sys.stdout.write(content)
#silence#        sys.stdout.write('\\n')
    return rootObj, rootElement, mapping, reverse_mapping


def parseString(inString, silence=False):
    from StringIO import StringIO
    doc = parsexml_(StringIO(inString))
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = %(prefix)s%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
#silence#    if not silence:
#silence#        sys.stdout.write('<?xml version="1.0" ?>\\n')
#silence#        rootObj.export(
#silence#            sys.stdout, 0, name_=rootTag,
#silence#            namespacedef_='%(namespacedef)s')
    return rootObj


def parseLiteral(inFileName, silence=False):
    doc = parsexml_(inFileName)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = %(prefix)s%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
#silence#    if not silence:
#silence#        sys.stdout.write('#from %(module_name)s import *\\n\\n')
#silence#        sys.stdout.write('import %(module_name)s as model_\\n\\n')
#silence#        sys.stdout.write('rootObj = model_.rootClass(\\n')
#silence#        rootObj.exportLiteral(sys.stdout, 0, name_=rootTag)
#silence#        sys.stdout.write(')\\n')
    return rootObj


def main():
    args = sys.argv[1:]
    if len(args) == 1:
        parse(args[0])
    else:
        usage()


if __name__ == '__main__':
    #import pdb; pdb.set_trace()
    main()

"""


# Fool (and straighten out) the syntax highlighting.
# DUMMY = '''


def generateMain(outfile, prefix, root):
    exportDictLine = "GDSClassesMapping = {\n"
    for classType in MappingTypes:
        mappedName = mapName(cleanupName(MappingTypes[classType]))
        if mappedName in AlreadyGenerated:
            exportDictLine += "    '%s': %s,\n" % (
                classType, mappedName,
                )
    exportDictLine += "}\n\n\n"
    outfile.write(exportDictLine)
    children = root.getChildren()
    rootClass = None
    if children:
        name = RootElement or children[0].getName()
        elType = cleanupName(children[0].getType())
        if RootElement:
            roots = RootElement.split('|')
            if len(roots) > 1:
                rootElement = roots[0]
                rootClass = roots[1]
            else:
                rootElement = roots[0]
        else:
            rootElement = elType
    else:
        name = ''
        if RootElement:
            roots = RootElement.split('|')
            if len(roots) > 1:
                rootElement = roots[0]
                rootClass = roots[1]
            else:
                rootElement = roots[0]
        else:
            rootElement = ''
    if rootClass is None:
        rootClass = rootElement
    if Namespacedef:
        namespace = Namespacedef
    elif Targetnamespace:
        if Targetnamespace in NamespacesDict:
            namespace = 'xmlns:%s="%s"' % (
                NamespacesDict[Targetnamespace].rstrip(':'), Targetnamespace, )
        else:
            namespace = ''
    else:
        namespace = ''
    params = {
        'prefix': prefix,
        'cap_name': cleanupName(make_gs_name(name)),
        'name': name,
        'cleanname': cleanupName(name),
        'module_name': os.path.splitext(os.path.basename(outfile.name))[0],
        'rootElement': rootElement,
        'rootClass': rootClass,
        'namespacedef': namespace,
    }
    s1 = TEMPLATE_MAIN % params
    outfile.write(s1)


def buildCtorParams(element, parent, childCount):
    content = []
    addedArgs = {}
    add = content.append
##     if not element.isMixed():
##         buildCtorParams_aux(addedArgs, add, parent)
    buildCtorParams_aux(addedArgs, add, parent)
    if element.getSimpleContent() or element.isMixed():
        add("valueOf_, ")
    if element.isMixed():
        add('mixedclass_, ')
        add('content_, ')
    if element.getExtended():
        add('extensiontype_, ')
    return content


def buildCtorParams_aux(addedArgs, add, element):
    parentName, parentObj = getParentName(element)
    if parentName:
        buildCtorParams_aux(addedArgs, add, parentObj)
    attrDefs = element.getAttributeDefs()
    for key in attrDefs:
        attrDef = attrDefs[key]
        name = attrDef.getName()
        name = cleanupName(mapName(name))
        if name not in addedArgs:
            addedArgs[name] = 1
            add('%s, ' % name)
    for child in element.getChildren():
        if child.getType() == AnyTypeIdentifier:
            add('anytypeobjs_, ')
        else:
            name = child.getCleanName()
            if name not in addedArgs:
                addedArgs[name] = 1
                add('%s, ' % name)


def get_class_behavior_args(classBehavior):
    argList = []
    args = classBehavior.getArgs()
    args = args.getArg()
    for arg in args:
        argList.append(arg.getName())
    argString = ', '.join(argList)
    return argString


#
# Retrieve the implementation body via an HTTP request to a
#   URL formed from the concatenation of the baseImplUrl and the
#   implUrl.
# An alternative implementation of get_impl_body() that also
#   looks in the local file system is commented out below.
#
def get_impl_body(classBehavior, baseImplUrl, implUrl):
    impl = '        pass\n'
    if implUrl:
        if baseImplUrl:
            implUrl = '%s%s' % (baseImplUrl, implUrl)
        try:
            implFile = urllib2.urlopen(implUrl)
            impl = implFile.read()
            implFile.close()
        except urllib2.HTTPError:
            err_msg('*** Implementation at %s not found.\n' % implUrl)
        except urllib2.URLError:
            err_msg('*** Connection refused for URL: %s\n' % implUrl)
    return impl

###
### This alternative implementation of get_impl_body() tries the URL
###   via http first, then, if that fails, looks in a directory on
###   the local file system (baseImplUrl) for a file (implUrl)
###   containing the implementation body.
###
##def get_impl_body(classBehavior, baseImplUrl, implUrl):
##    impl = '        pass\n'
##    if implUrl:
##        trylocal = 0
##        if baseImplUrl:
##            implUrl = '%s%s' % (baseImplUrl, implUrl)
##        try:
##            implFile = urllib2.urlopen(implUrl)
##            impl = implFile.read()
##            implFile.close()
##        except:
##            trylocal = 1
##        if trylocal:
##            try:
##                implFile = file(implUrl)
##                impl = implFile.read()
##                implFile.close()
##            except:
##                print('*** Implementation at %s not found.' % implUrl)
##    return impl


def generateClassBehaviors(wrt, classBehaviors, baseImplUrl):
    for classBehavior in classBehaviors:
        behaviorName = classBehavior.getName()
        #
        # Generate the core behavior.
        argString = get_class_behavior_args(classBehavior)
        if argString:
            wrt('    def %s(self, %s, *args):\n' % (behaviorName, argString))
        else:
            wrt('    def %s(self, *args):\n' % (behaviorName, ))
        implUrl = classBehavior.getImpl_url()
        impl = get_impl_body(classBehavior, baseImplUrl, implUrl)
        wrt(impl)
        wrt('\n')
        #
        # Generate the ancillaries for this behavior.
        ancillaries = classBehavior.getAncillaries()
        if ancillaries:
            ancillaries = ancillaries.getAncillary()
        if ancillaries:
            for ancillary in ancillaries:
                argString = get_class_behavior_args(ancillary)
                if argString:
                    wrt('    def %s(self, %s, *args):\n' %
                        (ancillary.getName(), argString))
                else:
                    wrt('    def %s(self, *args):\n' % (ancillary.getName(), ))
                implUrl = ancillary.getImpl_url()
                impl = get_impl_body(classBehavior, baseImplUrl, implUrl)
                wrt(impl)
                wrt('\n')
        #
        # Generate the wrapper method that calls the ancillaries and
        #   the core behavior.
        argString = get_class_behavior_args(classBehavior)
        if argString:
            wrt('    def %s_wrapper(self, %s, *args):\n' %
                (behaviorName, argString))
        else:
            wrt('    def %s_wrapper(self, *args):\n' % (behaviorName, ))
        if ancillaries:
            for ancillary in ancillaries:
                role = ancillary.getRole()
                if role == 'DBC-precondition':
                    wrt('        if not self.%s(*args)\n' %
                        (ancillary.getName(), ))
                    wrt('            return False\n')
        if argString:
            wrt('        result = self.%s(%s, *args)\n' %
                (behaviorName, argString))
        else:
            wrt('        result = self.%s(*args)\n' % (behaviorName, ))
        if ancillaries:
            for ancillary in ancillaries:
                role = ancillary.getRole()
                if role == 'DBC-postcondition':
                    wrt('        if not self.%s(*args)\n' %
                        (ancillary.getName(), ))
                    wrt('            return False\n')
        wrt('        return result\n')
        wrt('\n')


def generateSubclass(wrt, element, prefix, xmlbehavior,  behaviors, baseUrl):
    if not element.isComplex():
        return
    mappedName = element.getCleanName()
    if mappedName in AlreadyGenerated_subclass:
        return
    AlreadyGenerated_subclass.add(mappedName)
    name = element.getCleanName()
    wrt('class %s%s%s(supermod.%s):\n' % (prefix, name, SubclassSuffix, name))
    childCount = countChildren(element, 0)
    s1 = buildCtorArgs_multilevel(element, childCount)
    wrt('    def __init__(self%s):\n' % s1)
    args = buildCtorParams(element, element, childCount)
    s1 = ''.join(args)
    if len(args) > 254:
        wrt('        arglist_ = (%s)\n' % (s1, ))
        wrt('        super(%s%s%s, self).__init__(*arglist_)\n' %
            (prefix, name, SubclassSuffix, ))
    else:
        #wrt('        supermod.%s%s.__init__(%s)\n' % (prefix, name, s1))
        wrt('        super(%s%s%s, self).__init__(%s)\n' % (
            prefix, name, SubclassSuffix, s1, ))
    if xmlbehavior and behaviors:
        wrt('\n')
        wrt('    #\n')
        wrt('    # XMLBehaviors\n')
        wrt('    #\n')
        # Get a list of behaviors for this class/subclass.
        classDictionary = behaviors.get_class_dictionary()
        if name in classDictionary:
            classBehaviors = classDictionary[name]
        else:
            classBehaviors = None
        if classBehaviors:
            generateClassBehaviors(wrt, classBehaviors, baseUrl)
    wrt('supermod.%s.subclass = %s%s\n' % (name, name, SubclassSuffix))
    wrt('# end class %s%s%s\n' % (prefix, name, SubclassSuffix))
    wrt('\n\n')


TEMPLATE_SUBCLASS_HEADER = """\
#!/usr/bin/env python

#
# Generated %s by generateDS.py%s.
#
# Command line options:
%s
#
# Command line arguments:
#   %s
#
# Command line:
#   %s
#
# Current working directory (os.getcwd()):
#   %s
#

import sys

import %s as supermod

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

ExternalEncoding = '%s'

#
# Data representation classes
#


"""

TEMPLATE_SUBCLASS_FOOTER = """\
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
        rootTag = '%(rootElement)s'
        rootClass = supermod.%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
#silence#    if not silence:
#silence#        sys.stdout.write('<?xml version="1.0" ?>\\n')
#silence#        rootObj.export(
#silence#            sys.stdout, 0, name_=rootTag,
#silence#            namespacedef_='%(namespacedef)s',
#silence#            pretty_print=True)
    return rootObj


def parseEtree(inFilename, silence=False):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = supermod.%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
    mapping = {}
    rootElement = rootObj.to_etree(None, name_=rootTag, mapping_=mapping)
    reverse_mapping = rootObj.gds_reverse_node_mapping(mapping)
#silence#    if not silence:
#silence#        content = etree_.tostring(
#silence#            rootElement, pretty_print=True,
#silence#            xml_declaration=True, encoding="utf-8")
#silence#        sys.stdout.write(content)
#silence#        sys.stdout.write('\\n')
    return rootObj, rootElement, mapping, reverse_mapping


def parseString(inString, silence=False):
    from StringIO import StringIO
    doc = parsexml_(StringIO(inString))
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = supermod.%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
#silence#    if not silence:
#silence#        sys.stdout.write('<?xml version="1.0" ?>\\n')
#silence#        rootObj.export(
#silence#            sys.stdout, 0, name_=rootTag,
#silence#            namespacedef_='%(namespacedef)s')
    return rootObj


def parseLiteral(inFilename, silence=False):
    doc = parsexml_(inFilename)
    rootNode = doc.getroot()
    rootTag, rootClass = get_root_tag(rootNode)
    if rootClass is None:
        rootTag = '%(rootElement)s'
        rootClass = supermod.%(rootClass)s
    rootObj = rootClass.factory()
    rootObj.build(rootNode)
    # Enable Python to collect the space used by the DOM.
    doc = None
#silence#    if not silence:
#silence#        sys.stdout.write('#from %(super)s import *\\n\\n')
#silence#        sys.stdout.write('import %(super)s as model_\\n\\n')
#silence#        sys.stdout.write('rootObj = model_.rootClass(\\n')
#silence#        rootObj.exportLiteral(sys.stdout, 0, name_=rootTag)
#silence#        sys.stdout.write(')\\n')
    return rootObj


USAGE_TEXT = \"\"\"
Usage: python ???.py <infilename>
\"\"\"


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
"""

TEMPLATE_ABSTRACT_CLASS = """\
class %(clsname)s(object):
    subclass = None
    superclass = None
# fix valueof
    def __init__(self, valueOf_=''):
        raise NotImplementedError(
            'Cannot instantiate abstract class %(clsname)s (__init__)')
    def factory(*args_, **kwargs_):
        raise NotImplementedError(
            'Cannot instantiate abstract class %(clsname)s (factory)')
    factory = staticmethod(factory)
    def build(self, node_):
        raise NotImplementedError(
            'Cannot build abstract class %(clsname)s')
        attrs = node_.attributes
        # fix_abstract
        type_name = attrs.getNamedItemNS(
            'http://www.w3.org/2001/XMLSchema-instance', 'type')
        if type_name is not None:
            self.type_ = globals()[type_name.value]()
            self.type_.build(node_)
        else:
            raise NotImplementedError(
                'Class %%s not implemented (build)' %% type_name.value)
# end class %(clsname)s


"""

TEMPLATE_ABSTRACT_CHILD = """\
            type_name_ = child_.attrib.get(
                '{http://www.w3.org/2001/XMLSchema-instance}type')
            if type_name_ is None:
                type_name_ = child_.attrib.get('type')
            if type_name_ is not None:
                type_names_ = type_name_.split(':')
                if len(type_names_) == 1:
                    type_name_ = type_names_[0]
                else:
                    type_name_ = type_names_[1]
                class_ = globals()[type_name_]
                obj_ = class_.factory()
                obj_.build(child_)
            else:
                raise NotImplementedError(
                    'Class not implemented for <%s> element')
"""


def getNamespace(element):
    namespace = ''
    if element.targetNamespace in NamespacesDict:
        namespace = 'xmlns:%s="%s"' % (
            NamespacesDict[element.targetNamespace].rstrip(':'),
            element.targetNamespace, )
    elif Namespacedef:
        namespace = Namespacedef
    elif Targetnamespace in NamespacesDict:
            namespace = 'xmlns:%s="%s"' % (
                NamespacesDict[Targetnamespace].rstrip(':'),
                Targetnamespace, )
    return namespace


def generateSubclasses(root, subclassFilename, behaviorFilename,
                       prefix, options, args, superModule='xxx'):
    subclassFile = makeFile(subclassFilename)
    wrt = subclassFile.write
    if subclassFile:
        # Read in the XMLBehavior file.
        xmlbehavior = None
        behaviors = None
        baseUrl = None
        if behaviorFilename:
            try:
                # Add the currect working directory to the path so that
                #   we use the user/developers local copy.
                sys.path.insert(0, '.')
                import xmlbehavior_sub as xmlbehavior
            except ImportError:
                err_msg('*** You have requested generation of '
                        'extended methods.\n')
                err_msg('*** But, no xmlbehavior module is available.\n')
                err_msg('*** Generation of extended behavior '
                        'methods is omitted.\n')
            if xmlbehavior:
                behaviors = xmlbehavior.parse(behaviorFilename)
                behaviors.make_class_dictionary(cleanupName)
                baseUrl = behaviors.getBase_impl_url()
        wrt = subclassFile.write
        tstamp = (not NoDates and time.ctime()) or ''
        if NoVersion:
            version = ''
        else:
            version = ' version %s' % VERSION
        options1, args1, command_line = format_options_args(options, args)
        current_working_directory = os.path.split(os.getcwd())[1]
        wrt(TEMPLATE_SUBCLASS_HEADER % (tstamp, version,
            options1, args1,
            command_line, current_working_directory,
            superModule, ExternalEncoding, ))
        for element in ElementsForSubclasses:
            generateSubclass(
                wrt, element, prefix, xmlbehavior, behaviors, baseUrl)
        children = root.getChildren()
        rootClass = None
        if children:
            name = children[0].getName()
            elType = cleanupName(children[0].getType())
            if RootElement:
                roots = RootElement.split('|')
                if len(roots) > 1:
                    rootElement = roots[0]
                    rootClass = roots[1]
                else:
                    rootElement = roots[0]
            else:
                rootElement = elType
        else:
            name = ''
            if RootElement:
                roots = RootElement.split('|')
                if len(roots) > 1:
                    rootElement = roots[0]
                    rootClass = roots[1]
                else:
                    rootElement = roots[0]
            else:
                rootElement = ''
        if rootClass is None:
            rootClass = rootElement
        if Namespacedef:
            namespace = Namespacedef
        elif Targetnamespace:
            if Targetnamespace in NamespacesDict:
                namespace = 'xmlns:%s="%s"' % (
                    NamespacesDict[Targetnamespace].rstrip(':'),
                    Targetnamespace, )
            else:
                namespace = ''
        else:
            namespace = ''
        params = {
            'cap_name': make_gs_name(cleanupName(name)),
            'name': name,
            'cleanname': cleanupName(name),
            'module_name': os.path.splitext(
                os.path.basename(subclassFilename))[0],
            'root': root,
            'rootElement': rootElement,
            'rootClass': rootClass,
            'namespacedef': namespace,
            'super': superModule,
        }
        wrt(TEMPLATE_SUBCLASS_FOOTER % params)
        subclassFile.close()


def getUsedNamespacesDefs(element):
    processedPrefixes = []
    nameSpacesDef = getNamespace(element)
    for child in element.getChildren():
        if child.prefix not in processedPrefixes and \
           child.prefix in prefixToNamespaceMap:
            spaceDef = 'xmlns:%s="%s" ' % (
                child.prefix, prefixToNamespaceMap[child.prefix])
            if spaceDef.strip() not in nameSpacesDef:
                nameSpacesDef += ' ' + spaceDef
            processedPrefixes.append(child.prefix)
    return nameSpacesDef


def generateFromTree(wrt, prefix, elements, processed):
    for element in elements:
        nameSpacesDef = getUsedNamespacesDefs(element)
        name = element.getCleanName()
        if 1:     # if name not in processed:
            processed.append(name)
            generateClasses(wrt, prefix, element, 0, nameSpacesDef)
            children = element.getChildren()
            if children:
                generateFromTree(wrt, prefix, element.getChildren(), processed)


def generateSimpleTypes(wrt, prefix, simpleTypeDict):
    for simpletype in simpleTypeDict.keys():
        wrt('class %s(object):\n' % simpletype)
        wrt('    pass\n')
        wrt('\n\n')


def getImportsForExternalXsds(root):
    '''
    If we are creating a file per xsd, then
    we need to import any external classes we use
    '''
    externalImports = []
    if not SingleFileOutput:
        childStack = list(root.getChildren())
        while len(childStack) > 0:
            child = childStack.pop()
            if child.namespace and child.namespace != root.targetNamespace:
                fqn = child.getFullyQualifiedType()
                if fqn in fqnToModuleNameMap and fqn in fqnToElementDict:
                    schemaElement = fqnToElementDict[fqn]
                    moduleName = fqnToModuleNameMap[fqn]
                    type = schemaElement.getType()
                    if type == "xs:string":
                        type = schemaElement.getName()
                    externalImports.append("from %s%s import %s" %
                                           (moduleName, ModuleSuffix,
                                            type))
            for subChild in child.getChildren():
                childStack.append(subChild)
    return externalImports


def generate(outfileName, subclassFilename, behaviorFilename,
             prefix, root, options, args, superModule):
    global DelayedElements, DelayedElements_subclass, AlreadyGenerated
    # Create an output file.
    # Note that even if the user does not request an output file,
    #   we still need to go through the process of generating classes
    #   because it produces data structures needed during generation of
    #   subclasses.
    MappingTypes.clear()
    AlreadyGenerated = set()
    outfile = None
    if outfileName:
        outfile = makeFile(outfileName)
    if not outfile:
        outfile = os.tmpfile()
    wrt = outfile.write
    processed = []
    externalImports = getImportsForExternalXsds(root)
    generateHeader(wrt, prefix, options, args, externalImports)
    #generateSimpleTypes(outfile, prefix, SimpleTypeDict)
    DelayedElements = set()
    DelayedElements_subclass = set()
    elements = root.getChildren()
    generateFromTree(wrt, prefix, elements, processed)
    while 1:
        if len(DelayedElements) <= 0:
            break
        element = DelayedElements.pop()
        name = element.getCleanName()
        if name not in processed:
            processed.append(name)
            generateClasses(wrt, prefix, element, 1)
    #
    # Generate the elements that were postponed because we had not
    #   yet generated their base class.
    while 1:
        if len(PostponedExtensions) <= 0:
            break
        element = PostponedExtensions.pop()
        parentName, parent = getParentName(element)
        if parentName:
            if (parentName in AlreadyGenerated or
                    parentName in SimpleTypeDict):
                generateClasses(wrt, prefix, element, 1)
            else:
                PostponedExtensions.insert(0, element)
    #
    # Disable the generation of SAX handler/parser.
    # It failed when we stopped putting simple types into ElementDict.
    # When there are duplicate names, the SAX parser probably does
    #   not work anyway.
    generateMain(outfile, prefix, root)
    outfile.close()
    if subclassFilename:
        generateSubclasses(
            root, subclassFilename, behaviorFilename,
            prefix, options, args, superModule)


def makeFile(outFileName):
    outFile = None
    if (not Force) and os.path.exists(outFileName):
        if NoQuestions:
            sys.stderr.write(
                'File %s exists.  Change output file or use -f (force).\n' %
                outFileName)
            sys.exit(1)
        else:
            reply = raw_input(
                'File %s exists.  Overwrite? (y/n): ' % outFileName)
            if reply == 'y':
                outFile = file(outFileName, 'w')
    else:
        outFile = file(outFileName, 'w')
    return outFile


def mapName(oldName):
    newName = oldName
    if oldName in NameTable:
        newName = NameTable[oldName]
    return newName


def cleanupName(oldName):
    newName = oldName.replace(':', '_')
    newName = newName.replace('-', '_')
    newName = newName.replace('.', '_')
    return newName


def make_gs_name(oldName):
    if UseGetterSetter == 'old':
        newName = oldName.capitalize()
    elif UseGetterSetter == 'new':
        newName = '_%s' % oldName
    else:
        newName = ''
    return newName


def strip_namespace(val):
    return val.split(':')[-1]


def get_prefix_and_value(val):
    if ':' in val:
        return val.split(':')
    return None, val


def escape_string(instring):
    s1 = instring
    s1 = s1.replace('\\', '\\\\')
    s1 = s1.replace("'", "\\'")
    return s1


def is_builtin_simple_type(type_val):
    if (type_val in StringType or
            type_val == TokenType or
            type_val in DateTimeGroupType or
            type_val in IntegerType or
            type_val == DecimalType or
            type_val == PositiveIntegerType or
            type_val == NonPositiveIntegerType or
            type_val == NegativeIntegerType or
            type_val == NonNegativeIntegerType or
            type_val == BooleanType or
            type_val == FloatType or
            type_val == DoubleType or
            type_val in OtherSimpleTypes):
        return True
    else:
        return False


def parseAndGenerate(
        outfileName, subclassFilename, prefix,
        xschemaFileName, behaviorFilename, catalogFilename,
        processIncludes, options, args, superModule='???'):
    global DelayedElements, DelayedElements_subclass, \
        AlreadyGenerated, SaxDelayedElements, \
        AlreadyGenerated_subclass, UserMethodsPath, UserMethodsModule
    DelayedElements = set()
    DelayedElements_subclass = set()
    AlreadyGenerated = set()
    AlreadyGenerated_subclass = set()
    if UserMethodsPath:
        # UserMethodsModule = __import__(UserMethodsPath)
        path_list = UserMethodsPath.split('.')
        mod_name = path_list[-1]
        mod_path = os.sep.join(path_list[:-1])
        module_spec = imp.find_module(mod_name, [mod_path, ])
        UserMethodsModule = imp.load_module(mod_name, *module_spec)
##    parser = saxexts.make_parser("xml.sax.drivers2.drv_pyexpat")
    if xschemaFileName == '-':
        infile = sys.stdin
    else:
        infile = open(xschemaFileName, 'r')
    if SingleFileOutput:
        parser = make_parser()
        dh = XschemaHandler()
    ##    parser.setDocumentHandler(dh)
        parser.setContentHandler(dh)
        if processIncludes:
            import process_includes
            outfile = StringIO.StringIO()
            process_includes.process_include_files(
                infile, outfile,
                inpath=xschemaFileName,
                catalogpath=catalogFilename,
                fixtypenames=FixTypeNames)
            outfile.seek(0)
            infile = outfile
        parser.parse(infile)
        root = dh.getRoot()
        root.annotate()
##     print('-' * 60)
##     root.show(sys.stdout, 0)
##     print('-' * 60)
    #debug_show_elements(root)
        generate(
            outfileName, subclassFilename, behaviorFilename,
            prefix, root, options, args, superModule)
        # Generate __all__.  When using the parser as a module it is useful
        # to isolate important classes from internal ones. This way one
        # can do a reasonably safe "from parser import *"
        if outfileName:
            exportableClassList = [
                '"%s"' % name
                for name in AlreadyGenerated]
            exportableClassList.sort()
            exportableClassNames = ',\n    '.join(exportableClassList)
            exportLine = "\n__all__ = [\n    %s\n]\n" % exportableClassNames
            outfile = open(outfileName, "a")
            outfile.write(exportLine)
            outfile.close()
    else:
        import process_includes
        rootPaths = process_includes.get_all_root_file_paths(
            infile, inpath=xschemaFileName,
            catalogpath=catalogFilename)
        roots = []
        rootInfos = []
        for path in rootPaths:
            if path.startswith('http:') or path.startswith('ftp:'):
                try:
                    urlfile = urllib2.urlopen(path)
                    content = urlfile.read()
                    urlfile.close()
                    rootFile = StringIO.StringIO()
                    rootFile.write(content)
                    rootFile.seek(0)
                except urllib2.HTTPError:
                    msg = "Can't find file %s." % (path, )
                    raise IOError(msg)
            else:
                rootFile = open(path, 'r')
            parser = make_parser()
            dh = XschemaHandler()
            parser.setContentHandler(dh)
            parser.parse(rootFile)
            root = dh.getRoot()
            roots.append(root)
            rootFile.close()
        for root in roots:
            root.annotate()
            moduleName = None
            modulePath = None
            # use the first root element to set
            # up the module name and path
            for child in root.getChildren():
                if child.isRootElement():
                    typeName = child.getType()
                    if typeName.startswith("xs:"):
                        # no need to create a module for
                        # xs: types
                        continue
                    # convert to lower camel case if needed.
                    if "-" in typeName:
                        tokens = typeName.split("-")
                        typeName = ''.join([t.title() for t in tokens])
                    moduleName = typeName[0].lower() + typeName[1:]
                    modulePath = (
                        OutputDirectory +
                        os.sep + moduleName +
                        ModuleSuffix + ".py")
                    fqnToModuleNameMap[child.getFullyQualifiedType()] = \
                        moduleName
                    fqnToModuleNameMap[child.getFullyQualifiedName()] = \
                        moduleName
                    break
            rootInfos.append((root, modulePath))
        for root, modulePath in rootInfos:
            if modulePath:
                generate(
                    modulePath, subclassFilename, behaviorFilename,
                    prefix, root, options, args, superModule)
                # Generate __all__.  When using the parser as a module
                # it is useful
                # to isolate important classes from internal ones. This way one
                # can do a reasonably safe "from parser import *"
                exportableClassList = [
                    '"%s"' % name
                    for name in AlreadyGenerated]
                exportableClassList.sort()
                exportableClassNames = ',\n    '.join(exportableClassList)
                exportLine = "\n__all__ = [\n    %s\n]\n" % (
                    exportableClassNames, )
                outfile = open(modulePath, "a")
                outfile.write(exportLine)
                outfile.close()


# Function that gets called recursively in order to expand nested references
# to element groups
def _expandGR(grp, visited):
    # visited is used for loop detection
    children = []
    changed = False
    for child in grp.children:
        groupRef = child.getElementGroup()
        if not groupRef:
            children.append(child)
            continue
        ref = groupRef.ref
        referencedGroup = ElementGroups.get(ref, None)
        if referencedGroup is None:
            ref = strip_namespace(ref)
            referencedGroup = ElementGroups.get(ref, None)
        if referencedGroup is None:
            err_msg('*** Reference to unknown group %s\n' % groupRef.ref)
            continue
        visited.add(id(grp))
        if id(referencedGroup) in visited:
            #err_msg('*** Circular reference for %s' % groupRef.attrs['ref'])
            err_msg('*** Circular reference for %s\n' % groupRef.ref)
            continue
        changed = True
        _expandGR(referencedGroup, visited)
        children.extend(referencedGroup.children)
    if changed:
        # Avoid replacing the list with a copy of the list
        grp.children = children


def expandGroupReferences(grp):
    visited = set()
    _expandGR(grp, visited)


def debug_show_elements(root):
    #print('ElementDict:', ElementDict)
    print('=' * 50)
    for name, obj in ElementDict.iteritems():
        print('element:', name, obj.getName(), obj.type)
    print('=' * 50)
    #ipshell('debug')
##     root.show(sys.stdout, 0)
##     print('=' * 50)
##     response = raw_input('Press Enter')
##     root.show(sys.stdout, 0)
##     print('=' * 50)
##     print(']]] root: ', root, '[[[')


def load_config():
    try:
        #print('1. updating NameTable')
        import generateds_config
        NameTable.update(generateds_config.NameTable)
        #print('2. updating NameTable')
        #print('3. NameTable:', NameTable)
    except ImportError:
        pass


def fixSilence(txt, silent):
    if silent:
        txt = txt.replace('#silence#', '## ')
    else:
        txt = txt.replace('#silence#', '')
    return txt


def err_msg(msg):
    sys.stderr.write(msg)


USAGE_TEXT = __doc__


def usage():
    print(USAGE_TEXT)
    sys.exit(1)


def main():
    global Force, GenerateProperties, SubclassSuffix, RootElement, \
        ValidatorBodiesBasePath, UseGetterSetter, \
        UserMethodsPath, XsdNameSpace, \
        Namespacedef, NoDates, NoVersion, \
        TEMPLATE_MAIN, TEMPLATE_SUBCLASS_FOOTER, Dirpath, \
        ExternalEncoding, MemberSpecs, NoQuestions, \
        ExportWrite, ExportEtree, ExportLiteral, \
        FixTypeNames, SingleFileOutput, OutputDirectory, \
        ModuleSuffix
    outputText = True
    args = sys.argv[1:]
    try:
        options, args = getopt.getopt(
            args, 'hfyo:s:p:a:b:c:mu:q',
            [
                'help', 'subclass-suffix=',
                'root-element=', 'super=',
                'validator-bodies=', 'use-getter-setter=',
                'user-methods=', 'no-process-includes', 'silence',
                'namespacedef=', 'external-encoding=',
                'member-specs=', 'no-dates', 'no-versions',
                'no-questions', 'session=', 'fix-type-names=',
                'version', 'export=',
                'one-file-per-xsd', 'output-directory=',
                'module-suffix='
            ])
    except getopt.GetoptError:
        usage()
    prefix = ''
    outFilename = None
    subclassFilename = None
    behaviorFilename = None
    nameSpace = 'xs:'
    superModule = '???'
    processIncludes = 1
    namespacedef = ''
    ExternalEncoding = sys.getdefaultencoding()
    NoDates = False
    NoVersion = False
    NoQuestions = False
    showVersion = False
    xschemaFileName = None
    catalogFilename = None
    for option in options:
        if option[0] == '--session':
            sessionFilename = option[1]
            from libgenerateDS.gui import generateds_gui_session
            from xml.etree import ElementTree as etree
            doc = etree.parse(sessionFilename)
            rootNode = doc.getroot()
            sessionObj = generateds_gui_session.sessionType()
            sessionObj.build(rootNode)
            if sessionObj.get_input_schema():
                xschemaFileName = sessionObj.get_input_schema()
            if sessionObj.get_output_superclass():
                outFilename = sessionObj.get_output_superclass()
            if sessionObj.get_output_subclass():
                subclassFilename = sessionObj.get_output_subclass()
            if sessionObj.get_force():
                Force = True
            if sessionObj.get_prefix():
                prefix = sessionObj.get_prefix()
            if sessionObj.get_empty_namespace_prefix():
                nameSpace = ''
            elif sessionObj.get_namespace_prefix():
                nameSpace = sessionObj.get_namespace_prefix()
            if sessionObj.get_behavior_filename():
                behaviorFilename = sessionObj.get_behavior_filename()
            if sessionObj.get_properties():
                GenerateProperties = True
            if sessionObj.get_subclass_suffix():
                SubclassSuffix = sessionObj.get_subclass_suffix()
            if sessionObj.get_root_element():
                RootElement = sessionObj.get_root_element()
            if sessionObj.get_superclass_module():
                superModule = sessionObj.get_superclass_module()
            if sessionObj.get_old_getters_setters():
                UseGetterSetter = option[1]
            if sessionObj.get_validator_bodies():
                ValidatorBodiesBasePath = sessionObj.get_validator_bodies()
                if not os.path.isdir(ValidatorBodiesBasePath):
                    err_msg('*** Option validator-bodies must specify '
                            'an existing path.\n')
                    sys.exit(1)
            if sessionObj.get_user_methods():
                UserMethodsPath = sessionObj.get_user_methods()
            if sessionObj.get_no_dates():
                NoDates = True
            if sessionObj.get_no_versions():
                NoVersion = True
            if sessionObj.get_no_process_includes():
                processIncludes = 0
            if sessionObj.get_silence():
                outputText = False
            if sessionObj.get_namespace_defs():
                namespacedef = sessionObj.get_namespace_defs()
            if sessionObj.get_external_encoding():
                ExternalEncoding = sessionObj.get_external_encoding()
            if sessionObj.get_member_specs() in ('list', 'dict'):
                MemberSpecs = sessionObj.get_member_specs()
            break
    for option in options:
        if option[0] == '-h' or option[0] == '--help':
            usage()
        elif option[0] == '-p':
            prefix = option[1]
        elif option[0] == '-o':
            outFilename = option[1]
        elif option[0] == '-s':
            subclassFilename = option[1]
        elif option[0] == '-f':
            Force = 1
        elif option[0] == '-a':
            nameSpace = option[1]
        elif option[0] == '-b':
            behaviorFilename = option[1]
        elif option[0] == '-c':
            catalogFilename = option[1]
        elif option[0] == '-m':
            GenerateProperties = 1
        elif option[0] == '--no-dates':
            NoDates = True
        elif option[0] == '--no-versions':
            NoVersion = True
        elif option[0] == '--subclass-suffix':
            SubclassSuffix = option[1]
        elif option[0] == '--root-element':
            RootElement = option[1]
        elif option[0] == '--super':
            superModule = option[1]
        elif option[0] == '--validator-bodies':
            ValidatorBodiesBasePath = option[1]
            if not os.path.isdir(ValidatorBodiesBasePath):
                err_msg('*** Option validator-bodies must specify an '
                        'existing path.\n')
                sys.exit(1)
        elif option[0] == '--use-getter-setter':
            if option[1] in ('old', 'new', 'none'):
                UseGetterSetter = option[1]
            else:
                err_msg('*** Option use-getter-setter must '
                        '"old" or "new" or "none".\n')
                sys.exit(1)
        elif option[0] in ('-u', '--user-methods'):
            UserMethodsPath = option[1]
        elif option[0] == '--no-process-includes':
            processIncludes = 0
        elif option[0] == "--silence":
            outputText = False
        elif option[0] == "--namespacedef":
            namespacedef = option[1]
        elif option[0] == '--external-encoding':
            ExternalEncoding = option[1]
        elif option[0] in ('-q', '--no-questions'):
            NoQuestions = True
        elif option[0] == "--fix-type-names":
            FixTypeNames = option[1]
        elif option[0] == '--version':
            showVersion = True
        elif option[0] == '--member-specs':
            MemberSpecs = option[1]
            if MemberSpecs not in ('list', 'dict', ):
                raise RuntimeError(
                    'Option --member-specs must be "list" or "dict".')
        elif option[0] == '--export':
            ExportWrite = False
            ExportEtree = False
            ExportLiteral = False
            tmpoptions = option[1].split()
            if 'write' in tmpoptions:
                ExportWrite = True
            if 'etree' in tmpoptions:
                ExportEtree = True
            if 'literal' in tmpoptions:
                ExportLiteral = True
        elif option[0] == '--one-file-per-xsd':
            SingleFileOutput = False
        elif option[0] == "--output-directory":
            OutputDirectory = option[1]
        elif option[0] == "--module-suffix":
            ModuleSuffix = option[1]
    if showVersion:
        print('generateDS.py version %s' % VERSION)
        sys.exit(0)
    XsdNameSpace = nameSpace
    Namespacedef = namespacedef
    set_type_constants(nameSpace)
    if behaviorFilename and not subclassFilename:
        err_msg(USAGE_TEXT)
        err_msg('\n*** Error.  Option -b requires -s\n')
    if xschemaFileName is None:
        if len(args) != 1:
            usage()
        else:
            xschemaFileName = args[0]
    silent = not outputText
    TEMPLATE_MAIN = fixSilence(TEMPLATE_MAIN, silent)
    TEMPLATE_SUBCLASS_FOOTER = fixSilence(TEMPLATE_SUBCLASS_FOOTER, silent)
    load_config()
    parseAndGenerate(
        outFilename, subclassFilename, prefix,
        xschemaFileName, behaviorFilename, catalogFilename,
        processIncludes, options, args, superModule=superModule)


if __name__ == '__main__':
    #logging.basicConfig(level=logging.DEBUG,)
    #import pdb; pdb.set_trace()
    #import ipdb; ipdb.set_trace()
    # use the following to debug after an exception (post mortem debbing).
    #try:
    #    main()
    #except:
    #    import pdb; pdb.post_mortem()

    #"-o ..\multi_phy_configuration_model\Bindings.py --super=Bindings -s ..\multi_phy_configuration_model\Template.py --subclass-suffix="" --member-specs=list -m -f --silence .\multi_phy_configuration_model.xsd"
    # current_dir = os.path.dirname(__file__)
    # binding_path = os.path.realpath( os.path.join(current_dir, '..', 'multi_phy_configuration_model/Bindings.py') )
    # sys.argv.append('-o')
    # sys.argv.append('../multi_phy_configuration_model/Bindings.py')
    # sys.argv.append('--super=Bindings')
    # sys.argv.append('-s')
    # sys.argv.append('../multi_phy_configuration_model/Template.py')
    # sys.argv.append('--subclass-suffix=""')
    # sys.argv.append('-m')
    # sys.argv.append('-f')
    # sys.argv.append('--silence')
    # sys.argv.append('./multi_phy_configuration_model.xsd')

    main()
