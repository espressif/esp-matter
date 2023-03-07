from typing import List

import numpy as np
import tensorflow_lite_support.metadata.schema_py_generated as _tflite_schema_fb



class TfliteShape(tuple):
    """Wrapper for tensor shape"""
    def __new__ (cls, shape):
        if isinstance(shape, int):
            return super(TfliteShape, cls).__new__(cls, tuple(shape))
        else:
            return super(TfliteShape, cls).__new__(cls, tuple([int(x) for x in shape]))
    def __str__(self):
        return 'x'.join(f'{x}' for x in self)
    
    @property
    def flat_size(self) -> int:
        """Total number of elements or flatten size"""
        n = 1 
        for x in self:
            n *= x 
        return n


class TfliteQuantization(object):
    """Wrapper for tensor quantization"""

    def __init__(self, quantization: _tflite_schema_fb.QuantizationParameters):
        self.fb_quantization = quantization

        if quantization is None:
            self._quantization_dimension = -1
            self._scale = []
            self._zeropoint = []

        else:
            self._quantization_dimension = quantization.QuantizedDimension()
            scale_fb = quantization.ScaleAsNumpy()
            scale_zp = quantization.ZeroPointAsNumpy()
            if isinstance(scale_fb, np.ndarray):
                self._scale = [float(x) for x in scale_fb]
                self._zeropoint = [int(x) for x in scale_zp]
            else:
                self._scale = []
                self._zeropoint = []

    @property 
    def scale(self) -> List[float]:
        """Quantization scalers"""
        return self._scale 

    @property
    def zeropoint(self) -> List[int]:
        """Quantization zero points"""
        return self._zeropoint

    @property
    def quantization_dimension(self) -> int:
        """Quantization dimension"""
        return self._quantization_dimension


    @property
    def n_channels(self) -> int:
        """Number of channels. This is the number of elements in @ref scale and @ref zeropoint"""
        return len(self.scale)

    


class TfliteTensor(object):
    """Wrapper for TFLite flatbuffer tensor"""
    def __init__(self, index, fb_buffers, fb_tensor):
        self._index = index
        self._shape = TfliteShape(fb_tensor.ShapeAsNumpy())
        self._name = '' if not fb_tensor.Name() else fb_tensor.Name().decode("utf-8")

        t = fb_tensor.Type()
        if t == _tflite_schema_fb.TensorType.FLOAT32:
            dtype = np.float32
        elif t == _tflite_schema_fb.TensorType.FLOAT16:
            self.dtype = np.float16
        elif t == _tflite_schema_fb.TensorType.INT32:
            dtype = np.int32
        elif t == _tflite_schema_fb.TensorType.UINT8:
            dtype = np.uint8
        elif t == _tflite_schema_fb.TensorType.INT64:
            dtype = np.int64
        elif t == _tflite_schema_fb.TensorType.INT16:
            dtype = np.int16
        elif t == _tflite_schema_fb.TensorType.INT8:
            dtype = np.int8
        else:
            raise Exception(f'Unsupported .tflite tensor data type: {t}')

        self._dtype = dtype
        
        buffer = fb_buffers(fb_tensor.Buffer())
        if buffer.DataLength() == 0:
            s = self.shape if len(self.shape) > 1 else (0,)
            self._data = np.zeros(s, dtype=dtype)
        else:
            buffer_np = buffer.DataAsNumpy()
            a = np.frombuffer(buffer_np.tobytes(), dtype)
            if len(self.shape) > 1:
                self._data = a.reshape(self.shape)
            else:
                self._data = a

        self._quantization = TfliteQuantization(fb_tensor.Quantization())

    @property
    def index(self) -> int:
        """Flatbuffer index"""
        return self._index

    @property
    def name(self) -> str:
        """Tensor name"""
        return self._name

    @property 
    def shape(self) -> TfliteShape:
        """Tensor shape"""
        return self._shape

    @property
    def dtype(self) -> np.dtype:
        """Tensor data type"""
        return self._dtype 

    @property
    def dtype_str(self) -> str:
        """Tensor data type as a string"""
        return self._dtype.__name__.replace('numpy.', '')

    @property 
    def data(self) -> np.ndarray:
        """Tensor data"""
        return self._data

    @property
    def quantization(self) -> TfliteQuantization:
        """Tensor quantization details"""
        return self._quantization

    def shape_dtype_str(self, include_batch=False) -> str:
        """Return the shape and data-type of this tensor as a string: <dim0>x<dim1>x... (<dtype>)"""
        shape = self.shape
        if not include_batch and len(shape) > 1:
            shape = shape[1:]

        return f'{"x".join(f"{d}" for d in shape)} ({self.dtype_str})'

    def __str__(self):
        return f'{self.name}, dtype:{self.dtype_str}, shape:{self.shape}'

