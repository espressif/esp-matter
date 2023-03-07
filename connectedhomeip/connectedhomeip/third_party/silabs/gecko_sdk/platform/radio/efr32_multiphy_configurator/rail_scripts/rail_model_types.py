from collections import OrderedDict
from enum import Enum, unique
import sys
import re
import copy

thisModule = sys.modules[__name__]

supportedClassType = [
    'RAILModelElement',             # Base-type element, shouldn't be used by itself
    'RAILModelInfo',                # Simple single-element data type, stored value can
                                    # be anything, but it's thought mainly to store
                                    # simple atomic pieces of information
    'RAILModelObjectArray',         # Stores an array of complex (made-up) elements, all
                                    # elements stored are of the same type
    'RAILModelSimpleArray',         # A simple array of basic (C-types) types
    'RAILModelStruct',              # Stores a collection of elements within the same
                                    # context/namespace, analogous to a C-struct
    'RAILModelObjectCollection',    # Stores a "collection" of elements of multiple (and
                                    # unrelated) types; used mainly to aggregate elements
                                    # of all the types defined above
]

@unique
class ModelElementType(Enum):
    INFO                = 0
    SIMPLE_ARRAY        = 1
    OBJECT_ARRAY        = 2
    STRUCT              = 3
    OBJECT_COLLECTION   = 4

@unique
class FieldDataType(Enum):
    BOOL        = 0
    UINT8_T     = 1
    INT8_T      = 2
    UINT16_T    = 3
    INT16_T     = 4
    UINT32_T    = 5
    INT32_T     = 6
    UINT64_T    = 7
    INT64_T     = 8
    FLOAT       = 9
    STRING      = 10
    VOID        = 11

# @TODO: Implement Slots across these classes to prevent unintended creation of new
# attributes that can go around the API and cause clients to run into unexpected issues

class StructField(object):

    def __init__(self, model=None, name=None, dataType=None, isPtr=False, isArray=False, desc=None, value=None):
        self._name = name
        self._dataType  =  eval(dataType) if type(dataType) is str else dataType
        self._isPtr  = isPtr
        self._isArray = isArray
        self._desc = desc
        if self._isArray:
            self._value = value if value is not None else []
        else:
            self._value = value

    def __str__(self):
        return('StructField({_name}, {_dataType}, {_isPtr}, {_isArray}, {_desc}, {_value})'.format(**self.__dict__))

    @property
    def name(self):
        return self._name

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        self._value = value

class RAILModelElement(object):

    def __init__(self, model=None, name=None, type=ModelElementType.INFO):
        self.__dict__['_name'] = name
        self.__dict__['_type'] = type

    def __str__(self):
        return('RAILModelElement({_name}, {_type})'.format(**self.__dict__))

    @property
    def type(self):
        return self._type

    @property
    def name(self):
        return self._name

    @name.setter
    def name(self, nameValue):
        self._name = nameValue

class RAILModelInfo(RAILModelElement):

    def __init__(self, model=None, name=None):
        super(RAILModelInfo, self).__init__(model, name, ModelElementType.INFO)
        self._value = None

    def __str__(self):
        return('RAILModelInfo({_name}, {_type}, {_value})'.format(**self.__dict__))

    def __eq__(self, other):
        return self._value == other._value

    def __ne__(self, other):
        return self._value != other._value

    @property
    def value(self):
        return self._value

    @value.setter
    def value(self, value):
        self._value = value

    def resolve(self, context=OrderedDict()):
        context[self._name] = self._value

class RAILModelObjectCollection(RAILModelElement):

    def __init__(self, model=None, name=None, _elements=None, **kwargs):
        super(RAILModelObjectCollection, self).__init__(model, name, ModelElementType.OBJECT_COLLECTION)
        self._elements =[]
        self.lastElement = None
        for element in _elements:
            for key, value in element.items():
                if key in supportedClassType:
                    className = getattr(thisModule, key)
                    instance = className(model, **value)
                    # Add the new class instance as an attribute to the
                    # RAILModelObjectArray being created
                    # if the type being created is RAILModelObjectArray, only
                    # create a template of the object
                    setattr(self, instance.name, instance)
                    self._elements.append(instance)
        for element in kwargs:
            setattr(self, element, kwargs[element])

    def __eq__(self, other):

        if type(self) == type(other):
            # Check the length of _elements, if they are not the same, return False
            # right away
            if len(self._elements) != len(other._elements):
                return False
            for element in self._elements:
                otherElementName = "other.{}".format(element.name)
                try:
                    otherElement = eval(otherElementName)
                    if element != otherElement:
                        return False
                except AttributeError:
                    return False
            return True
        else:
            return False

    def __ne__(self, other):
        result = self.__eq__(other)
        return not result

    def __len__(self):
        return len(self._elements)

    def resolve(self, context=OrderedDict()):
        localContext = OrderedDict()
        for element in self._elements:
            element.resolve(localContext)
        context[self.name] = localContext

class RAILModelObjectArray(RAILModelElement):

    def __init__(self, model=None, name=None, autoInit=0, _elements=None):
        super(RAILModelObjectArray, self).__init__(model, name, ModelElementType.OBJECT_ARRAY)
        self._elements =[]
        self.lastElement = None
        numElements = len(_elements)
        # assert numElements == 1, \
        #     "RAILModelObjectArray supports a single element, currently there are {} elements defined".format(numElements)
        # elementType = _elements[0].keys()[0]
        # elementContent = _elements[0].values()[0]
        for key, value in _elements[0].items():
            elementType = key
            elementContent = value
            break

        assert elementType in supportedClassType, \
            "{} is not a supported type for RAILModelObjectArray".format(elementType)
        className = getattr(thisModule, elementType)
        instance = className(model, **elementContent)
        # Add the new class instance as an attribute to the
        # RAILModelObjectArray being created
        # if the type being created is RAILModelObjectArray,
        # only create a template of the objectType
        objectType = "_"+instance.name
        setattr(self, objectType, instance)
        setattr(self, "_objectType", objectType)

        for i in range(autoInit):
            self.addNewElement(getattr(self, "_objectType"))

    def __eq__(self, other):
        # Check the length of _elements, if they are not the same, return False
        # right away
        try:
            if len(self._elements) != len(other._elements):
                return False
            for element in self._elements:
                otherElementName = "other.{}".format(element._uniqueName)
                try:
                    otherElement = eval(otherElementName)
                    if element != otherElement:
                        return False
                except AttributeError:
                    return False
            return True
        except AttributeError:
            return False

    def __ne__(self, other):
        try:
            result = self.__eq__(other)
            return not result
        except AttributeError:
            return False

    def __len__(self):
        return len(self._elements)

    def newElement(self, name=None):
        newObjectName = name if name is not None else getattr(self, "_objectType")[1:]
        # Create a regex pattern based on the incoming 'name'
        pattern = re.compile("^"+newObjectName+r"(?:_(\d+))?$")

        # Check if there's an attribute defined for the desired object
        # desiredObjectName = name if name is not None else getattr(self, "_objectType")
        # if hasattr(self, desiredObjectName):
        # If there is, get max index of all instances
        maxIndex = -1
        match = None
        # desiredObject = getattr(self, desiredObjectName)
        objectTypeName = getattr(self, "_objectType")
        desiredObject = getattr(self, objectTypeName)

        # Search all attributes in the object to see if there are
        # instances of this type already defined
        found = False
        for k in self.__dict__:
            match = re.search(pattern, k)
            # if we find a match, get the index number and compare it
            # against the current max, update if necessary
            if match:
                found = True
                if match.group(1):
                    index = int(match.group(1))
                    maxIndex = index if index > maxIndex else maxIndex

        # Create a new instance of the desiredObject and add it as an attribute
        # to the current instance and as an item in the _elements array
        newElement = copy.deepcopy(desiredObject)
        newElement.name = newObjectName

        if maxIndex >= 0 or found or name is None:
            uniqueName = newObjectName + "_" + str(maxIndex + 1)
        else:
            uniqueName = newObjectName
        newElement._uniqueName = uniqueName
        return newElement

    def addElement(self, element, smartIndex=True):
        if smartIndex:
            try:
                oldElement = getattr(self, element.name)
                # Rename old element and update maxIndex
                oldElementOldName = oldElement._uniqueName
                newElementNewName = oldElement._uniqueName + "_0"
                oldElement._uniqueName = newElementNewName
                setattr(self, newElementNewName, oldElement)
                delattr(self, oldElementOldName)
                element._uniqueName = oldElementOldName + "_1"
            except AttributeError:
                # The object does not exist, simply move along
                pass

        self._elements.append(element)
        setattr(self, element._uniqueName, element)
        self.lastElement = element

    def addNewElement(self, name=None, autoIndex=True):
        newElement = self.newElement(name)
        self.addElement(newElement, autoIndex)
        return newElement

    def resolve(self, context=OrderedDict()):
        localContext = OrderedDict()
        for i, element in enumerate(self._elements):
            elementContext = OrderedDict()
            element.resolve(elementContext)
            # Use the uniqueName to name the element
            # localContext[element._uniqueName] = elementContext.values()[0]
            for key, value in elementContext.items():
                localContext[element._uniqueName] = value
                break
        context[self.name] = localContext

class RAILModelSimpleArray(RAILModelElement):

    def __init__(self, model=None, name=None, dataType=None, isPtr=False, **kwargs):
        super(RAILModelSimpleArray, self).__init__(model, name, ModelElementType.SIMPLE_ARRAY)
        self._dataType = eval(dataType) if type(dataType) is str else dataType
        self._isPtr  = isPtr
        self._values = []
        for element in kwargs:
            setattr(self, element, kwargs[element])

    def __str__(self):
        return('RAILModelSimpleArray({_name}, {_type}, {_dataType}, {_values})'.format(**self.__dict__))

    def __len__(self):
        return len(self._values)

    @property
    def dataType(self):
        return self._dataType

    @dataType.setter
    def dataType(self, value):
        _dataType = value

    @property
    def values(self):
        return self._values

    @values.setter
    def values(self, values):
        if values is not None:
            for i in values:
                self._values.append(i)

    def append(self, value):
        self._values.append(value)

    def resolve(self, context=OrderedDict()):
        context[self.name] = self._values

class RAILModelStruct(RAILModelElement):

    def __init__(self, model=None, name=None, **kwargs):
        super(RAILModelStruct, self).__init__(model, name, ModelElementType.STRUCT)
        assert type(kwargs) == dict and '_fields' in kwargs, "Invalid RAILModelStruct data format"
        self._fields = []
        for field in kwargs['_fields']:
            newField = StructField(model, **field)
            self._fields.append(newField)
            setattr(self, newField.name, newField)

    def __setattr__(self, name, value):
        # assert type(value) == StructField, \
        #     "New attribute '{}' is not of type StructField".format(name)
        object.__setattr__(self, name, value)

    def __eq__(self, other):
        for field in self._fields:
            otherFieldName = "other.{}".format(field.name)
            try:
                otherField = eval(otherFieldName)
                if field.value != otherField.value:
                    return False
            except AttributeError:
                return False
        return True

    def __ne__(self, other):
        result = self.__eq__(other)
        return not result

    def addField(self, name, dataType, is_ptr=False, is_array=False, desc=None):
        field = StructField(name, dataType, is_ptr, is_array, desc)
        self._fields.append(newField)
        setattr(self, field.name, field)

    def getField(self, name):
        return self._fields[name]

    def resolve(self, context=OrderedDict()):
        localContext = OrderedDict()
        for field in self._fields:
            localContext[field.name] = field.value
        context[self.name] = localContext
