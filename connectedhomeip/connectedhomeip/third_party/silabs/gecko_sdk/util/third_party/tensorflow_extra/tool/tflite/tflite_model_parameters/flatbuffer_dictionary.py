# pylint: disable=unused-wildcard-import, wildcard-import
from typing import Union, List
import flatbuffers

from .schema.Dictionary import *
from .schema.BinaryValue import *
from .schema.BoolValue import *
from .schema.DoubleValue import *
from .schema.Entry import *
from .schema.FloatValue import *
from .schema.Int16Value import *
from .schema.Int32Value import *
from .schema.Int64Value import *
from .schema.Int8Value import *
from .schema.StringList import *
from .schema.StringValue import *
from .schema.Uint8Value import *
from .schema.Uint16Value import *
from .schema.Uint32Value import *
from .schema.Uint64Value import *
from .schema.Value import *



FLATBUFFER_SCHEMA_VERSION = 1



class FlatbufferDictionary(dict):
    """ FlatbufferDictionary

    This class allows for adding scalar values to a standard Python dictionary,
    serializing the dictionary into a flatbuffer, and later de-serializing
    to another Python dictionary.

    A flatbuffer dictionary is a collection of key/value pairs where:
    - key - Name of parameter as a string
    - value - Value of parameter as a specific scalr data type

    The dictionary is serialized using the [Flatbuffer](https://google.github.io/flatbuffers)
    schema [dictionary.fbs](./dictionary.fbs)

    NOTE: The FlatbufferDictionary object inheriets the standard Python 'dict' class.

    """
    def __init__(self, *args, **kwargs):
        self.update(*args, **kwargs)

    def put(self, key:str, value:Union[str,int,float,bool,List[str],bytes], dtype:str=None):
        """Put an entry into the dictionary
        
        This API allows for specifying the value's datatype.
        Alternatively, you can use the standard Python dictionary syntax, e.g.:

        my_params.set('foo', 42, 'int32')
        OR
        my_params['foo'] = 42

        Args:
            key: The dictionary key to insert or overwrite
            value: The value of the entry. Must have a type of: str,int,float,bool,List[str], or bytes
            dtype: Optional. Force the value to have a specific data type. 
                   Must be a string and one of: bool,int8,int16,int32,int64,uint8,uint16,uint32,uint64,float,double,str,str_list,bin
        """
        # Validate the value type and dtype
        _, value = _get_dtype_and_value(value, dtype)
        if dtype is not None:
            value = DictionaryValue(dtype=dtype, value=value)
        super(FlatbufferDictionary, self).__setitem__(key, value)


    def __setitem__(self, key, value):
        self.put(key, value)


    def serialize(self) -> bytes:
        """Serialize the current dictionary entries into a flatbuffer
        
        Returns:
            Serialized dictionary flatbuffer bytes
        """

        builder = flatbuffers.Builder(0)
        
        entry_offsets = []
        for key, value in self.items():
            key_offset = builder.CreateString(key)
            value_type, value_offset = _generate_value(builder, value)
            if not value_offset:
                continue

            EntryStart(builder)
            EntryAddKey(builder, key_offset)
            EntryAddValue(builder, value_offset)
            EntryAddValueType(builder, value_type)
            entry_offsets.append(EntryEnd(builder))
 
        DictionaryStartEntriesVector(builder, len(entry_offsets))
        for o in reversed(entry_offsets):
            builder.PrependUOffsetTRelative(o)
        entries_offset = builder.EndVector(len(entry_offsets))
       

        DictionaryStart(builder)
        DictionaryAddSchemaVersion(builder, FLATBUFFER_SCHEMA_VERSION)
        DictionaryAddEntries(builder, entries_offset)
        root = DictionaryEnd(builder)

        builder.Finish(root)

        return bytes(builder.Output())

    
    @staticmethod
    def deserialize(serialized_data:bytes):
        """Instantiate a FlatbufferDictionary object with the given serialized flatbuffer binary data"""

        # Load the flatbuffer serialized data
        params_fb = Dictionary.GetRootAsDictionary(serialized_data, 0)

        # Validate the schema version
        schema_version = params_fb.SchemaVersion()
        if not schema_version:
            raise RuntimeError('Flatbuffer missing schema version')

        if schema_version > FLATBUFFER_SCHEMA_VERSION:
            raise RuntimeError(f'Flatbuffer schema version ({schema_version}) not supported (max supported version:{FLATBUFFER_SCHEMA_VERSION})')

        # Instantiate a new TfliteModelParameters object
        model_parameters = FlatbufferDictionary()

        # Populate the TfliteModelParameters object with the
        # entries from the serialized flatbuffer data
        try:
            for i in range(params_fb.EntriesLength()):
                entry_fb = params_fb.Entries(i)
                key = entry_fb.Key().decode('utf-8')
                fb_value = entry_fb.Value()
                fb_type = entry_fb.ValueType()
                model_parameters[key] =  _parse_value(fb_type, fb_value)
        except Exception as e:
            raise RuntimeError(f'Failed to parse flatbuffer, err {e}').with_traceback(e.__traceback__)

        return model_parameters


    def summary(self) -> str:
        """Generate a summary of the dictionary"""
        s = ''

        for key in sorted(self):
            value = self[key]
            if isinstance(value, list):
                value_str = ','.join(value)
            elif isinstance(value, bytes):
                value_str = f'<{len(value)} bytes>'
            else:
                value_str = f'{value}'
            
            s += f'{key}: {value_str}\n'

        return s.strip()

    def __str__(self):
        return self.summary()



class DictionaryValue:
    """Dictonary entry value
    
    A dictionary value contains both
    the actual scalar value and a corresponding data type
    """
    def __init__(self, value, dtype):
        self.value = value 
        self.dtype = dtype
    
    def __str__(self):
        return f'{self.dtype}:{self.value}'




#######################################################################
# Internal flatbuffer parsing/generation code





_TYPE_MAP = {}
_TYPE_MAP['bool']    = Value.boolean
_TYPE_MAP['int8']    = Value.i8
_TYPE_MAP['int16']   = Value.i16
_TYPE_MAP['int32']   = Value.i32
_TYPE_MAP['int64']   = Value.i64
_TYPE_MAP['uint8']   = Value.u8
_TYPE_MAP['uint16']  = Value.u16
_TYPE_MAP['uint32']  = Value.u32
_TYPE_MAP['uint64']  = Value.u64
_TYPE_MAP['float']   = Value.f32
_TYPE_MAP['double']  = Value.f64
_TYPE_MAP['str']     = Value.str
_TYPE_MAP['str_list'] = Value.str_list 
_TYPE_MAP['bin']     = Value.bin 


def _get_dtype_and_value(value, dtype=None) -> tuple:
    """Return the value's corresponding data type"""
    if isinstance(value, DictionaryValue):
        dtype = value.dtype 
        value = value.value
    
    if dtype is not None:
        if dtype not in _TYPE_MAP:
            raise ValueError(f'Unknown data type: {dtype}, valid types: {",".join(_TYPE_MAP.keys())}')
        if dtype == 'bool':
            try:
                bool(value)
            except Exception as e:
                raise ValueError(f'Failed to convert value to bool, err: {e}')
        elif dtype in ('int8', 'int16', 'int32', 'int64','uint8', 'uint16','uint32','uint64'):
            try:
                int(value)
            except Exception as e:
                raise ValueError(f'Failed to convert value to int, err: {e}')
        elif dtype in ('float','double'):
            try:
                float(value)
            except Exception as e:
                raise ValueError(f'Failed to convert value to float, err: {e}')
        elif dtype == 'str':
            try:
                str(value)
            except Exception as e:
                raise ValueError(f'Failed to convert value to str, err: {e}')
        elif dtype == 'str_list':
            if not isinstance(value, (list,tuple,set)):
                raise ValueError('dtype=str_list but value is not an instance of a list')
            for e in value:
                if not isinstance(e, str):
                    raise ValueError('Only list of strings are supported')
        elif dtype == 'bin':
            try:
                bytes(value)
            except Exception as e:
                raise ValueError(f'Failed to convert value to bytes, err: {e}')

        return dtype, value

    if isinstance(value, bool):
        dtype = 'bool'
    elif isinstance(value, int):
        if value < 0:
            if value >= -(2 ** 7):
                dtype = 'int8'
            elif value >= -(2 ** 15):
                dtype = 'int16'
            elif value >= -(2 ** 32):
                dtype = 'int32'
            elif value >= -(2 ** 63):
                dtype = 'int64'
            else:
                raise ValueError('Value overflow')
        else:
            if value < (2**8):
                dtype = 'uint8'
            elif value < (2**16):
                dtype = 'uint16'
            elif value < (2**32):
                dtype = 'uint32'
            elif value < (2**64):
                dtype = 'uint64'
            else:
                raise ValueError('Value overflow')
    elif isinstance(value, float):
        if value >= 1.175494351e-38 and value <= 3.402823466e+38:
            dtype = 'float'
        else:
            dtype = 'double'
    elif isinstance(value, str):
        dtype = 'str'
    elif isinstance(value, (bytes, bytearray)):
        dtype = 'bin'
    elif isinstance(value, (list,tuple,set)):
        for e in value:
            if not isinstance(e, str):
                raise ValueError('Only list of strings are supported')
            
        dtype = 'str_list'
    else:
        raise ValueError('Data type could not be automatically determined, you must manually specify the data type')
    
    return dtype, value 


def _generate_value(builder, value) -> tuple:
    """Convert the Python value into a flatbuffer value"""
    dtype, value = _get_dtype_and_value(value)
    
    if dtype == 'null' or value is None:
        return 0, None

    dtype = _TYPE_MAP[dtype]

    if dtype == Value.boolean:
        BoolValueStart(builder)
        BoolValueAddValue(builder, bool(value))
        offset = BoolValueEnd(builder)
    elif dtype == Value.i8:
        Int8ValueStart(builder)
        Int8ValueAddValue(builder, int(value))
        offset = Int8ValueEnd(builder)
    elif dtype == Value.i16:
        Int16ValueStart(builder)
        Int16ValueAddValue(builder, int(value))
        offset = Int16ValueEnd(builder)
    elif dtype == Value.i32:
        Int32ValueStart(builder)
        Int32ValueAddValue(builder, int(value))
        offset = Int32ValueEnd(builder)
    elif dtype == Value.i64:
        Int64ValueStart(builder)
        Int64ValueAddValue(builder, int(value))
        offset = Int64ValueEnd(builder)
    elif dtype == Value.u8:
        Uint8ValueStart(builder)
        Uint8ValueAddValue(builder, int(value))
        offset = Uint8ValueEnd(builder)
    elif dtype == Value.u16:
        Uint16ValueStart(builder)
        Uint16ValueAddValue(builder, int(value))
        offset = Uint16ValueEnd(builder)
    elif dtype == Value.u32:
        Uint32ValueStart(builder)
        Uint32ValueAddValue(builder, int(value))
        offset = Uint32ValueEnd(builder)
    elif dtype == Value.u64:
        Uint64ValueStart(builder)
        Uint64ValueAddValue(builder, int(value))
        offset = Uint64ValueEnd(builder)
    elif dtype == Value.f32:
        FloatValueStart(builder)
        FloatValueAddValue(builder, float(value))
        offset = FloatValueEnd(builder)
    elif dtype == Value.f64:
        DoubleValueStart(builder)
        DoubleValueAddValue(builder, float(value))
        offset = DoubleValueEnd(builder)
    elif dtype == Value.str:
        s_offset = builder.CreateString(str(value))
        StringValueStart(builder)
        StringValueAddData(builder, s_offset)
        offset = StringValueEnd(builder)
    elif dtype == Value.bin:
        b_offset = builder.CreateByteVector(bytes(value))
        BinaryValueStart(builder)
        BinaryValueAddData(builder, b_offset)
        offset = BinaryValueEnd(builder)
    elif dtype == Value.str_list:
        s_offsets = []
        for s in value:
            s_offsets.append(builder.CreateString(str(s)))
            
        StringListStartDataVector(builder, len(s_offsets))
        for o in reversed(s_offsets):
            builder.PrependUOffsetTRelative(o)
        v = builder.EndVector(len(s_offsets))

        StringListStart(builder)
        StringListAddData(builder, v)
        offset = StringListEnd(builder)
    else:
        raise ValueError(f'Unknown data type: {dtype}')

    return dtype, offset


def _parse_value(fb_type, fb_value):
    """Convert the flatbuffer value into a Python value"""
    if fb_type == Value.boolean:
        v = BoolValue()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.i8:
        v = Int8Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.i16:
        v = Int16Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.i32:
        v = Int32Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.i64:
        v = Int64Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.u8:
        v = Uint8Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.u16:
        v = Uint16Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.u32:
        v = Uint32Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.u64:
        v = Uint64Value()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.f32:
        v = FloatValue()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.f64:
        v = DoubleValue()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Value()
    elif fb_type == Value.str:
        v = StringValue()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.Data().decode('utf-8')
    elif fb_type == Value.bin:
        v = BinaryValue()
        v.Init(fb_value.Bytes, fb_value.Pos)
        return v.DataAsNumpy().tobytes()
    elif fb_type == Value.str_list:
        retval = []
        fb_vector = StringList()
        fb_vector.Init(fb_value.Bytes, fb_value.Pos)
        for i in range(fb_vector.DataLength()):
            s = fb_vector.Data(i).decode('utf-8')
            retval.append(s)
        return retval
    else:
        raise RuntimeError(f'Unknown flatbuffer data type: {fb_type}')
