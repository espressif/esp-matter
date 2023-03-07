
from typing import List, Tuple
import numpy as np
import tensorflow_lite_support.metadata.schema_py_generated as _tflite_schema_fb
from tensorflow_lite_support.metadata.schema_py_generated import BuiltinOperator as TfliteOpCode
from .tflite_tensor import TfliteTensor


class TfliteLayerOptions(object):
    pass


class TfliteLayer(object):
    """Wrapper for TFLite flatbuffer layer"""

    @staticmethod
    def from_model(index, model, fb_operation):
        """Instantiate a TfliteLayer from then given TfliteModel flatbuffer operation"""
        fb_opcode = model.flatbuffer_model.OperatorCodes(fb_operation.OpcodeIndex())
        # See: https://github.com/tensorflow/community/pull/285/files
        # for why we return the max(DeprecatedBuiltinCode, BuiltinCode)
        opcode = max(fb_opcode.DeprecatedBuiltinCode(), fb_opcode.BuiltinCode())
        opcode_version = fb_opcode.Version()

        if opcode == TfliteOpCode.ADD:
            layer_cls = TfliteAddLayer
        elif opcode == TfliteOpCode.CONV_2D:
            layer_cls = TfliteConv2dLayer
        elif opcode == TfliteOpCode.FULLY_CONNECTED:
            layer_cls = TfliteFullyConnectedLayer
        elif opcode == TfliteOpCode.DEPTHWISE_CONV_2D:
            layer_cls = TfliteDepthwiseConv2dLayer
        elif opcode == TfliteOpCode.AVERAGE_POOL_2D or \
            opcode == TfliteOpCode.MAX_POOL_2D:
            layer_cls = TflitePooling2dLayer
        elif opcode == TfliteOpCode.RESHAPE:
            layer_cls = TfliteReshapeLayer
        else:
            layer_cls = TfliteLayer

        return layer_cls(
            index=index, 
            opcode=opcode, 
            opcode_version=opcode_version, 
            model=model, 
            fb_operation=fb_operation
        )

    
    def __init__(
        self, 
        index:int, 
        opcode:TfliteOpCode, 
        opcode_version:int, 
        model:_tflite_schema_fb.Model, 
        fb_operation:_tflite_schema_fb.Operator
    ):
        self._index:int = index 
        self._opcode:TfliteOpCode = opcode
        self._opcode_version:int = opcode_version
        self._opcode_str:str = _convert_object_value_to_string(TfliteOpCode(), self.opcode)

        self._inputs : List[TfliteTensor] = []
        self._outputs : List[TfliteTensor] = []
        
        for i in fb_operation.InputsAsNumpy():
            if i >= 0:
                self.inputs.append(model.get_tensor(i))
        for i in fb_operation.OutputsAsNumpy():
            if i >= 0:
                self.outputs.append(model.get_tensor(i))
        
        self._options:TfliteLayerOptions = self._get_options(fb_operation.BuiltinOptionsType(), fb_operation.BuiltinOptions())


    def __str__(self):
        return f'{self.name}'

    @property
    def index(self) -> int:
        """Index of this layer in the model"""
        return self._index

    @property
    def name(self) -> str:
        """Name of current layer as: op<index>-<OpCodeStr>"""
        return f'op{self._index}-{self._opcode_str}'

    @property
    def opcode(self) -> TfliteOpCode:
        """OpCode numeric value"""
        return self._opcode

    @property
    def opcode_str(self) -> str:
        """OpCode as a string"""
        return self._opcode_str

    @property 
    def options(self) -> TfliteLayerOptions:
        """Layer-specific options/config"""
        return self._options 
    
    @property 
    def inputs(self) -> List[TfliteTensor]:
        """List of layer input tensor(s)"""
        return self._inputs

    @property
    def n_inputs(self) -> int:
        """Return the number of inputs"""
        return len(self._inputs)

    @property 
    def outputs(self) -> List[TfliteTensor]:
        """List of layer output tensor(s)"""
        return self._outputs

    @property
    def n_outputs(self) -> int:
        """Return the number of outputs"""
        return len(self._outputs)

    def get_input_tensor(self, index=0) -> TfliteTensor:
        """Get layer input tensor as TfliteTensor"""
        if index >= self.n_inputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_inputs})') 
        return self._inputs[index]

    def get_input(self, index=0) -> np.ndarray:
        """Get layer input tensor as np.ndarray"""
        if index >= self.n_inputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_inputs})') 
        return self._inputs[index].data

    def get_output_tensor(self, index=0) -> TfliteTensor:
        """Layer output tensor as TfliteTensor"""
        if index >= self.n_outputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_outputs})') 
        return self._outputs[index]

    def get_output(self, index=0) -> np.ndarray:
        """Layer output tensor as np.ndarray"""
        if index >= self.n_outputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_outputs})') 
        return self._outputs[index].data


    def _get_options(self, opts_type, opts_buf):
        if opts_type == _tflite_schema_fb.BuiltinOptions.Conv2DOptions:
            return TfliteLayerOptionsConv2D(opts_buf)
        elif opts_type == _tflite_schema_fb.BuiltinOptions.TransposeConvOptions:
            return TfliteLayerOptionsTransposeConv(opts_buf)
        elif opts_type == _tflite_schema_fb.BuiltinOptions.DepthwiseConv2DOptions:
            return TfliteLayerOptionsDepthwiseConv2D(opts_buf)
        elif opts_type == _tflite_schema_fb.BuiltinOptions.Pool2DOptions:
            return TfliteLayerOptionsPool2D(opts_buf)
        elif opts_type == _tflite_schema_fb.BuiltinOptions.FullyConnectedOptions:
            return TfliteLayerOptionsFullyConnected(opts_buf)
        elif opts_type == _tflite_schema_fb.BuiltinOptions.AddOptions:
            return TfliteLayerOptionsAdd(opts_buf)
        else:
            return TfliteLayerOptionsUnsupported(opts_type)
        
    
        
class TfliteAddLayer(TfliteLayer):
    """ADD operation TfliteLayer"""
    @property
    def activation(self) -> str:
        """Fused activation"""
        return self._options.activation
    @property
    def input1_data(self) -> np.ndarray:
        """First input tensor data"""
        return self._inputs[0].data
    @property
    def input2_data(self) -> np.ndarray:
        """Second input tensor data"""
        return self._inputs[1].data
    @property
    def output_data(self) -> np.ndarray:
        """Output tensor data"""
        return self._outputs[0].data


class TfliteConv2dLayer(TfliteLayer):
    """CONV_2D operation TfliteLayer"""
    def __init__(self, *args, **kwargs):
        TfliteLayer.__init__(self, *args, **kwargs)
        filters_shape = self._inputs[1].data.shape
        self._kernel_size = (filters_shape[1], filters_shape[2])
        self._filters = filters_shape[3]
        self._bias_data = None if len(self._inputs) < 3 else self._inputs[2].data

    @property
    def filters(self) -> int:
        """The number of filters"""
        return self._filters
    @property
    def kernel_size(self) -> Tuple[int,int]:
        """Filters kernel size has height x width"""
        return self._kernel_size
    @property
    def strides(self) -> Tuple[int,int]:
        """Kernel stride height x width"""
        return (self._options.stride_height, self._options.stride_width)
    @property
    def padding(self) -> str:
        """Kernel padding"""
        return self._options.padding
    @property
    def activation(self) -> str:
        """Fused activation"""
        return self._options.activation
    @property
    def use_bias(self) -> bool:
        """Return if the layer uses a bias"""
        return self._bias_data is not None
    @property
    def input_data(self) -> np.ndarray:
        """Input tensor data"""
        return self._inputs[0].data
    @property
    def filters_data(self) -> np.ndarray:
        """Filters tensor data"""
        return self._inputs[1].data
    @property
    def bias_data(self) -> np.ndarray:
        """Bias tensor data (None if no bias used)"""
        return self._bias_data
    @property
    def output_data(self) -> np.ndarray:
        """Output tensor data"""
        return self._outputs[0].data


class TfliteFullyConnectedLayer(TfliteLayer):
    """FULLY_CONNECT operation TfliteLayer"""
    def __init__(self, *args, **kwargs):
        TfliteLayer.__init__(self, *args, **kwargs)
        self._bias_data = None if len(self._inputs) < 3 else self._inputs[2].data

    @property
    def accumulator_depth(self) -> int:
        """Number of weights to accumulate"""
        return self.weights_data.shape[-1]
    @property
    def units(self) -> int:
        """Number of neurons"""
        return self.output_data.shape[-1]
    @property
    def activation(self) -> str:
        """Fused activation"""
        return self._options.activation
    @property
    def use_bias(self) -> bool:
        """Return if the layer uses a bias"""
        return self._bias_data is not None
    @property
    def input_data(self) -> np.ndarray:
        """Input tensor data"""
        return self._inputs[0].data
    @property
    def weights_data(self) -> np.ndarray:
        """Weights tensor data"""
        return self._inputs[1].data
    @property
    def bias_data(self) -> np.ndarray:
        """Bias tensor data (None if no bias used)"""
        return self._bias_data
    @property
    def output_data(self) -> np.ndarray:
        """Output tensor data"""
        return self._outputs[0].data


class TfliteDepthwiseConv2dLayer(TfliteLayer):
    """DEPTHWISE_CONV_2D operation TfliteLayer"""
    def __init__(self, *args, **kwargs):
        TfliteLayer.__init__(self, *args, **kwargs)
        filters_shape = self._inputs[1].data.shape
        self._kernel_size = (filters_shape[1], filters_shape[2])
        self._bias_data = None if len(self._inputs) < 3 else self._inputs[2].data

    @property
    def multiplier(self) -> int:
        """Depth multiplier"""
        return self._options.multiplier
    @property
    def kernel_size(self) -> Tuple[int,int]:
        """Filters kernel size has height x width"""
        return self._kernel_size
    @property
    def strides(self) -> Tuple[int,int]:
        """Kernel stride height x width"""
        return (self._options.stride_height, self._options.stride_width)
    @property
    def padding(self) -> str:
        """Kernel padding"""
        return self._options.padding
    @property
    def activation(self) -> str:
        """Fused activation"""
        return self._options.activation
    @property
    def use_bias(self) -> bool:
        """Return if the layer uses a bias"""
        return self._bias_data is not None
    @property
    def input_data(self) -> np.ndarray:
        """Input tensor data"""
        return self._inputs[0].data
    @property
    def filters_data(self) -> np.ndarray:
        """Filters tensor data"""
        return self._inputs[1].data
    @property
    def bias_data(self) -> np.ndarray:
        """Bias tensor data (None if no bias used)"""
        return self._bias_data
    @property
    def output_data(self) -> np.ndarray:
        """Output tensor data"""
        return self._outputs[0].data


class TflitePooling2dLayer(TfliteLayer):
    """AVERAGE_POOL_2D or MAX_POOL_2D operation TfliteLayer"""
    def __init__(self, *args, **kwargs):
        TfliteLayer.__init__(self, *args, **kwargs)
 
    @property
    def pool_size(self) -> Tuple[int,int]:
        """Kernel size as height x width"""
        return (self._options.filter_height, self._options.filter_width)
    @property
    def strides(self) -> Tuple[int,int]:
        """Kernel stride as height x width"""
        return (self._options.stride_height, self._options.stride_width)
    @property
    def padding(self) -> str:
        """Kernel padding"""
        return self._options.padding
    @property
    def activation(self) -> str:
        """Fused activation"""
        return self._options.activation
    @property
    def input_data(self) -> np.ndarray:
        """Input tensor data"""
        return self._inputs[0].data
    @property
    def output_data(self) -> np.ndarray:
        """Output tensor data"""
        return self._outputs[0].data


class TfliteReshapeLayer(TfliteLayer):
    """RESHAPE operation TfliteLayer"""
    def __init__(self, *args, **kwargs):
        TfliteLayer.__init__(self, *args, **kwargs)

    @property
    def input_data(self) -> np.ndarray:
        """Input tensor data"""
        return self._inputs[0].data
    @property
    def output_data(self) -> np.ndarray:
        """Output tensor data"""
        return self._outputs[0].data

    @property
    def requires_copy(self) -> bool:
        """Return true if a memcpy is required, False if the reshape was done in-place"""
        return self._inputs[0].index != self._outputs[0].index

    @property
    def n_input_elements(self) -> int:
        """Return the number of input elements"""
        return self._inputs[0].shape.flat_size


class TfliteLayerOptionsUnsupported(TfliteLayerOptions):
    def __init__(self, opts_type):
        TfliteLayerOptions.__init__(self)
        self.opts_type = opts_type

    def __str__(self):
        return f'BuiltinOptionsType={self.opts_type}'

        
class TfliteLayerOptionsConv2D(TfliteLayerOptions):
    def __init__(self, opts_buf):
        TfliteLayerOptions.__init__(self)
    
        opts = _tflite_schema_fb.Conv2DOptions()
        opts.Init(opts_buf.Bytes, opts_buf.Pos)
        self.stride_width = opts.StrideW()
        self.stride_height = opts.StrideH()
        self.padding = _get_padding(opts.Padding())
        self.activation = _get_fused_activation(opts.FusedActivationFunction())
        
    def __str__(self):
        return f'Padding:{self.padding} stride:{self.stride_width}x{self.stride_height} activation:{self.activation}'

        
class TfliteLayerOptionsTransposeConv(TfliteLayerOptions):
    def __init__(self, opts_buf):
        TfliteLayerOptions.__init__(self)
    
        opts = _tflite_schema_fb.TransposeConvOptions()
        opts.Init(opts_buf.Bytes, opts_buf.Pos)
        self.stride_width = opts.StrideW()
        self.stride_height = opts.StrideH()
        self.padding = _get_padding(opts.Padding())

    def __str__(self):
        return f'Padding:{self.padding} stride:{self.stride_width}x{self.stride_height}'


class TfliteLayerOptionsDepthwiseConv2D(TfliteLayerOptions):
    def __init__(self, opts_buf):
        TfliteLayerOptions.__init__(self)
    
        opts = _tflite_schema_fb.DepthwiseConv2DOptions()
        opts.Init(opts_buf.Bytes, opts_buf.Pos)
        self.stride_width = opts.StrideW()
        self.stride_height = opts.StrideH()
        self.multiplier = opts.DepthMultiplier()
        self.padding = _get_padding(opts.Padding())
        self.activation = _get_fused_activation(opts.FusedActivationFunction())
        
    def __str__(self):
        return f'Multipler:{self.multiplier} padding:{self.padding} stride:{self.stride_width}x{self.stride_height} activation:{self.activation}'


class TfliteLayerOptionsPool2D(TfliteLayerOptions):
    def __init__(self, opts_buf):
        TfliteLayerOptions.__init__(self)
    
        opts = _tflite_schema_fb.Pool2DOptions()
        opts.Init(opts_buf.Bytes, opts_buf.Pos)
        self.stride_width = opts.StrideW()
        self.stride_height = opts.StrideH()
        self.filter_width = opts.FilterWidth()
        self.filter_height = opts.FilterHeight()
        self.padding = _get_padding(opts.Padding())
        self.activation = _get_fused_activation(opts.FusedActivationFunction())
        
    def __str__(self):
        return f'Padding:{self.padding} stride:{self.stride_width}x{self.stride_height} filter:{self.filter_width}x{self.filter_height} activation:{self.activation}'


class TfliteLayerOptionsFullyConnected(TfliteLayerOptions):
    def __init__(self, opts_buf):
        TfliteLayerOptions.__init__(self)
    
        opts = _tflite_schema_fb.FullyConnectedOptions()
        opts.Init(opts_buf.Bytes, opts_buf.Pos)
        self.activation = _get_fused_activation(opts.FusedActivationFunction())
        
    def __str__(self):
        return f'Activation:{self.activation}'


class TfliteLayerOptionsAdd(TfliteLayerOptions):
    def __init__(self, opts_buf):
        TfliteLayerOptions.__init__(self)
    
        opts = _tflite_schema_fb.AddOptions()
        opts.Init(opts_buf.Bytes, opts_buf.Pos)
        self.activation = _get_fused_activation(opts.FusedActivationFunction())
        
    def __str__(self):
        return f'Activation:{self.activation}'


def _get_padding(padding) -> str:
    return _convert_object_value_to_string(_tflite_schema_fb.Padding(), padding)
    
def _get_fused_activation(act) -> str:
    return _convert_object_value_to_string(_tflite_schema_fb.ActivationFunctionType(), act)
    
def _convert_object_value_to_string(obj, needle) -> str:
    for key in dir(obj):
        if getattr(obj, key) == needle:
            return key.lower()
        
    return 'None'

