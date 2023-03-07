
import os
import warnings

from typing import List, Tuple, Dict, Union, Iterator
#Removed import of prettytable because the uc-generation environment does not support it, and it is not used!
#from prettytable import PrettyTable

import numpy as np

# Disable the "DeprecationWarning" found in the flatbuffer package
warnings.filterwarnings("ignore", category=DeprecationWarning)

import tensorflow_lite_support.metadata.schema_py_generated as _tflite_schema_fb
from tensorflow_lite_support.metadata.schema_py_generated import BuiltinOperator as TfliteOpCode # pylint: disable=unused-import
from tensorflow_lite_support.metadata.schema_py_generated import flatbuffers

from .tflite_tensor import TfliteTensor
from .tflite_layer import TfliteLayer



TFLITE_FILE_IDENTIFIER = b"TFL3"




class TfliteModel(object):
    """Class to access a .tflite model flatbuffer's layers and tensors

    Refer to:
    https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/schema/schema_v3.fbs

    for more details on the .tflite flatbuffer schema

    Example:
        .. highlight:: python
        .. code-block:: python

            from tflite_model import TfliteModel

            # Load you .tflite model file
            model = TfliteModel.load_flatbuffer_file('some/path/my_model.tflite')

            # Print a summary of the model
            print(tflite_model.summary())

            # Iterate through each layer of the model
            for layer in tflite_model.layers:
                # See TfliteLayer for additional info
                print(layer)


            # Update the model's description
            # This updates the .tflite's "description" field (which will be displayed in GUIs like https://netron.app)
            tflite_model.description = "My awesome model"
            print(f'New model description: {tflite_model.description}')

            # Save a new .tflite with the updated description
            tflite_model.save('some/path/my_new_model.tflite')

            # Add some metadata to the .tflite
            metadata = 'this is metadata'.encode('utf-8')
            tflite_model.add_metadata('my_metadata', metadata)

            # Retrieve all the metadata in the .tflite
            all_metadata = tflite_model.get_all_metadata()
            for key, data in all_metadata.items():
                print(f'{key}: length={len(data)} bytes')

            # Save a new .tflite with the updated metadata
            tflite_model.save('some/path/my_new_model.tflite')

            # You must have Tensorflow instance to perform this step
            # This will run inference with the given buffer and return 
            # the results. The input_buffer can be:
            # - a single sample as a numpy array
            # - a numpy array of 1 or more samples
            # - A Python generator that returns (batch_x, batch_y)
            # inference_results = tflite_model.predict(..)
    """

    @staticmethod
    def load_flatbuffer_file(path: str, cwd=None):
        """Load a .tflite flatbuffer file"""
        found_path = _existing_path(path, cwd=cwd)
        if found_path is None:
            raise FileNotFoundError(f'.tflite model file not found: {path}')

        with open(found_path, 'rb') as f:
            flatbuffer_data = f.read()
        
        return TfliteModel(flatbuffer_data=flatbuffer_data, path=found_path)


    def __init__(self, flatbuffer_data: bytes, path: str=None):
        self.path = path 
        self._interpreter = None
        self._interpreter_batch_size = -1
        self._input0 : TfliteTensor = None
        self._output0 : TfliteTensor = None
        self._flatbuffer_data : bytes = flatbuffer_data
        self._subgraph: _tflite_schema_fb.SubGraph = None
        self._model:_tflite_schema_fb.Model = None 
        self._load_model()
    
    @property
    def path(self) -> str:
        """Path to .tflite file
        Returns None if no path was specified. 
        The path is normalized and backslashes are converted to forward slash
        """
        return None if self._path is None else os.path.normpath(self._path).replace('\\', '/')
    @path.setter
    def path(self, v: str):
        """Path to .tflite file"""
        if v is not None:
            v = v.replace('\\', '/')
        self._path = v

    @property
    def filename(self) -> str:
        """File name of associated .tflite model file
        Return None if not path is set"""
        if self._path:
            return os.path.basename(self._path)
        else:
            return None
    
    @property
    def description(self) -> str:
        """Get/set model description
        
        @note save() must be called for changes to persist
        """
        return '' if not self._model.Description() else self._model.Description().decode('utf-8')
    @description.setter
    def description(self, desc: str):
        updated_model = _tflite_schema_fb.ModelT.InitFromObj(_tflite_schema_fb.Model.GetRootAsModel(self._flatbuffer_data, 0))
        updated_model.description = desc
        self._update_model(updated_model)

    @property
    def flatbuffer_data(self) -> bytes:
        """Flatbuffer binary data"""
        return bytes(self._flatbuffer_data)
    
    @property
    def flatbuffer_size(self) -> int:
        """Size of the model flatbuffer in bytes"""
        return len(self.flatbuffer_data)

    @property
    def flatbuffer_model(self) -> _tflite_schema_fb.Model:
        """Flatbuffer schema Model object"""
        return self._model
    
    @property
    def n_inputs(self) -> int:
        """Return the number of model inputs"""
        return self._subgraph.InputsLength()

    @property
    def inputs(self) -> List[TfliteTensor]:
        """List of all input tensors"""
        retval = []
        for index in self._subgraph.InputsAsNumpy():
            retval.append(self.get_tensor(index))
            
        return retval 

    @property
    def n_outputs(self) -> int:
        """Return the number of model outputs"""
        return self._subgraph.OutputsLength()

    @property
    def outputs(self) -> List[TfliteTensor]:
        """List of all output tensors"""
        retval = []
        for index in self._subgraph.OutputsAsNumpy():
            retval.append(self.get_tensor(index))
            
        return retval 
    
    @property
    def layers(self) -> List[TfliteLayer]:
        """List of all model layers"""
        retval = []
        for i in range(self._subgraph.OperatorsLength()):
            layer = TfliteLayer.from_model(i, self, self._subgraph.Operators(i))
            retval.append(layer)
        return retval
    
    
    def summary(self) -> str:
        """Generate a summary of the model"""
        if self._flatbuffer_data is None:
            return 'Not loaded'
        
        t = PrettyTable()
        t.field_names = [
            'Index', 
            'OpCode',  
            'Input(s)',
            'Output(s)', 
            'Config'
        ]

        for i, layer in enumerate(self.layers):
            inputs = '\n'.join([x.shape_dtype_str(include_batch=False) for x in layer.inputs])
            outputs = '\n'.join([x.shape_dtype_str(include_batch=False) for x in layer.outputs])
            t.add_row([
                i, 
                layer.opcode_str, 
                inputs,
                outputs,
                f'{layer.options}'
            ])

        t.align = 'l'
        return t.get_string()


    def get_tensor(self, index : int) -> TfliteTensor:
        """Return a specific model tensor as a TfliteTensor """
        if index >= self._subgraph.TensorsLength():
            raise IndexError(f'Index overflow ({index} >= {self._subgraph.TensorsLength()})') 
        return TfliteTensor(index, self.flatbuffer_model.Buffers, self._subgraph.Tensors(index))


    def get_tensor_data(self, index : int) -> np.ndarray:
        """Return a specific model tensor as a np.ndarray """
        return self.get_tensor(index=index).data


    def get_input_tensor(self, index: int = 0) -> TfliteTensor:
        """Return a model input tensor as a TfliteTensor"""
        if index >= self.n_inputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_inputs})') 
        tensor_index = self._subgraph.InputsAsNumpy()[index]
        return self.get_tensor(tensor_index)


    def get_input(self, index: int = 0) -> np.ndarray:
        """Return a model input as a np.ndarray"""
        if index >= self.n_inputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_inputs})') 
        tensor_index = self._subgraph.InputsAsNumpy()[index]
        return self.get_tensor_data(tensor_index)


    def get_output_tensor(self, index: int = 0) -> TfliteTensor:
        """Return a model output tensor as a TfliteTensor"""
        if index >= self.n_inputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_outputs})') 
        tensor_index = self._subgraph.OutputsAsNumpy()[index]
        return self.get_tensor(tensor_index)


    def get_output(self, index: int = 0) -> np.ndarray:
        """Return a model output tensor as a np.ndarray"""
        if index >= self.n_inputs:
            raise IndexError(f'Index overflow ({index} >= {self.n_outputs})') 
        tensor_index = self._subgraph.OutputsAsNumpy()[index]
        return self.get_tensor_data(tensor_index)
    

    def get_all_metadata(self) -> Dict[str,bytes]:
        """Return all model metadata as a dictionary"""
        retval = {}
        for i in range(self._model.MetadataLength()):
            meta = self._model.Metadata(i)
            name = meta.Name().decode("utf-8")
            buffer_index = meta.Buffer()
            metadata = self._model.Buffers(buffer_index)
            retval[name] = metadata.DataAsNumpy().tobytes()

        return retval

    
    def get_metadata(self, tag : str) -> bytes:
        """Return model metadata with specified tag"""
        metadata_buf = None
        for i in range(self._model.MetadataLength()):
            meta = self._model.Metadata(i)
            if meta.Name().decode("utf-8") == tag:
                buffer_index = meta.Buffer()
                metadata = self._model.Buffers(buffer_index)
                metadata_buf = metadata.DataAsNumpy().tobytes()
                break
    
        return metadata_buf 
    
    
    def add_metadata(self, tag :str, value: bytes):
        """Set or add metadata to model

        Note:
            :func:`~tflite_model.TfliteModel.save` must be called for changes to persist
        
        Args:
            tag (str): The key to use to lookup the metadata
            value (bytes): The metadata value as a binary blob to add to the .tflite
        """
        if not tag or not value:
            raise Exception('Must provide valid tag and value arguments')

        metadata_buf = bytearray(value)
        
        updated_model = _tflite_schema_fb.ModelT.InitFromObj(_tflite_schema_fb.Model.GetRootAsModel(self._flatbuffer_data, 0))
        buffer_field = _tflite_schema_fb.BufferT()
        buffer_field.data = metadata_buf
    
        is_populated = False
        if not updated_model.metadata:
            updated_model.metadata = []
        else:
            # Check if metadata has already been populated.
            for meta in updated_model.metadata:
                if meta.name.decode("utf-8") == tag:
                    is_populated = True
                    updated_model.buffers[meta.buffer] = buffer_field
    
        if not is_populated:
            if not updated_model.buffers:
                updated_model.buffers = []
            updated_model.buffers.append(buffer_field)
            # Creates a new metadata field.
            metadata_field = _tflite_schema_fb.MetadataT()
            metadata_field.name = tag
            metadata_field.buffer = len(updated_model.buffers) - 1
            updated_model.metadata.append(metadata_field)
    
        self._update_model(updated_model)


    def remove_metadata(self, tag: str) -> bool:
        """Remove model metadata with specified tag"""
        # TODO: Add support for removing metadata
        raise NotImplementedError('Removing metadata from the model is not currently supported')


    def save(self, output_path: str = None):
        """Save flatbuffer data to file
            If output_path is specified then write to new file,
            otherwise overwrite existing file
        """
        if output_path is None:
            output_path = self.path

        if not output_path:
            raise Exception('No output path specified')

        # Create the model's output directory if necessary
        out_dir = os.path.dirname(output_path)
        if out_dir:
            os.makedirs(out_dir, exist_ok=True)
        
        with open(output_path, 'wb') as f:
            f.write(self._flatbuffer_data)
    

    def predict(
        self, 
        x:Union[np.ndarray, Iterator],
        y_dtype=None,
        **kwargs
    ) -> np.ndarray:
        """Invoke the TfLite interpreter with the given input sample and return the results
        
        Args:
            x: The input samples(s) as a numpy array or data generator.
                If x is a numpy array then it must have the same shape as the model input
                or it must be a vector (i.e. batch) of samples having the same shape as the model input.
                The data type must either be the same as the model input's OR it must be a float32,
                in which case the input sample will automatically be quantized using the model input's
                quantizing scaler/zeropoint.
                If x is a generator, then each iteration must return a tuple: batch_x, batch_y
                batch_x must  be a vector (i.e. batch) of samples having the same shape as the model input
                batch_y is ignored.
            y_dtype: The return value's data type. By default, data type is None in which case the model output is directly returned.
                     If y_dtype=np.float32 then the model output is de-quantized to float32 using the model's output 
                     quantization scaler/zeropoint (if necessary)
        
        Returns:
            Output of model inference, y. If x was a single sample, then y is a single result. Otherwise
            y is a vector (i.e. batch) of model results.
            If y_dtype is given, the y if automatically converted/de-quantized to the given dtype.
        """

        # This expects either
        # [n_samples, input_shape...]
        # OR
        # [input_shape ...]
        if isinstance(x, np.ndarray):
            input_shape = self.inputs[0].shape
            is_single_sample = False
            if len(x.shape) == len(input_shape[1:]):
                is_single_sample = True
                # Add the batch dimension if we were only given a single sample
                x = np.expand_dims(x, axis=0)
                self._allocate_tflite_interpreter(batch_size=1)
            else:
                self._allocate_tflite_interpreter(batch_size=x.shape[0])

            # If the input sample isn't the same as the model input dtype,
            # then we need to manually convert it first 
            # NOTE: If the model input type is float32 then
            #       quantization is done automatically inside the model
            x = self.quantize_to_input_dtype(x)

            # Then set model input tensor
            self._interpreter.set_tensor(self._input0.index, x)
            # Execute the model
            self._interpreter.invoke()

            # Get the model results
            y = self._interpreter.get_tensor(self._output0.index)

            # Convert the output data type to float32 if necessary
            # NOTE: If the model output type is float32 then
            #       de-quantization is done automatically inside the model

            if y_dtype == np.float32:
                y = self.dequantize_output_to_float32(y)

            # Remove the batch dimension if we were only given a single sample
            if is_single_sample:
                y = np.squeeze(y, axis=0)

            return y 

        # Else if we were given a data generator
        else:
            n_samples = 0
            batch_results = []
            for batch_x, _ in x: 
                self._allocate_tflite_interpreter(batch_size=batch_x.shape[0])

                # If the input sample isn't the same as the model input dtype,
                # then we need to manually convert it first 
                batch_x = self.quantize_to_input_dtype(batch_x)

                # The set model input tensor
                self._interpreter.set_tensor(self._input0.index, batch_x)
                # Execute the model
                self._interpreter.invoke()
               
                # Get the model results
                batch_y = self._interpreter.get_tensor(self._output0.index)

                if y_dtype == np.float32:
                    # Convert the output data type to float32 if necessary
                    batch_y = self.dequantize_output_to_float32(batch_y)
                        
                batch_results.append(batch_y)
                n_samples += len(batch_y)

            if len(batch_results) == 0:
                raise Exception('No batch samples where generated by the data given data generator')

            batch_size = batch_results[0].shape[0]
            output_shape = batch_results[0].shape[1:]

            if hasattr(x, 'max_samples'):
                n_samples = x.max_samples

            y = np.zeros((n_samples, *output_shape), dtype=batch_y.dtype)

            for batch_index, batch in enumerate(batch_results):
                for result_index, result in enumerate(batch):
                    index = batch_index * batch_size + result_index
                    if index >= n_samples:
                        break
                    y[index,:] = result
            
            return y
    

    def quantize_to_input_dtype(self, x):
        """Quantize the input sample(s) to the model's input dtype (if necessary)"""

        if x.dtype == self._input0.dtype:
            return x 

        if x.dtype != np.float32:
            raise Exception('The sample input must be float32 or the same dtype as the model input')

        # Convert from float32 to the model input data type
        x = (x / self._input0.quantization.scale[0]) + self._input0.quantization.zeropoint[0]
        return x.astype(self._input0.dtype)


    def dequantize_output_to_float32(self, y):
        """De-quantize the model output to float32 (if necessary)"""
        if y.dtype == np.float32:
            return y 

        y = y.astype(np.float32)
        return (y - self._output0.quantization.zeropoint[0]) * self._output0.quantization.scale[0]



    def _allocate_tflite_interpreter(self, batch_size=1):
        if self._interpreter is None or self._interpreter_batch_size != batch_size:
            try:
                import tensorflow as tf
            except ModuleNotFoundError as e:
                raise ModuleNotFoundError(f'You must first install the "tensorflow" Python package to run inference, err: {e}')
            
            self._interpreter_batch_size = batch_size
            self._interpreter = tf.lite.Interpreter(model_path=self._path)
            self._input0 = self.get_input_tensor(0)
            self._output0 = self.get_output_tensor(0)

            new_input_shape = (batch_size, *self._input0.shape[1:])
            new_output_shape = (batch_size, *self._output0.shape[1:])

            self._interpreter.resize_tensor_input(self._input0.index , new_input_shape)
            self._interpreter.resize_tensor_input(self._output0.index, new_output_shape)
                
            self._interpreter.allocate_tensors()


    
    def _update_model(self, updated_model):
        b = flatbuffers.Builder(0)
        b.Finish(updated_model.Pack(b), TFLITE_FILE_IDENTIFIER)
        self._flatbuffer_data = b.Output()
        self._load_model()
    
    
    def _load_model(self):
        self._model = _tflite_schema_fb.Model.GetRootAsModel(self._flatbuffer_data, 0)
        if self._model.SubgraphsLength() > 1:
            raise Exception('Only one model subgraph currently supported')

        schema_version = self._model.Version()
        if schema_version != 3:
            raise Exception('TF-Lite schema v3 is only supported')
        
        self._subgraph = self._model.Subgraphs(0)
    

def _existing_path(path: str, cwd=None):
    if path is None:
        return None 

    if cwd:
        found_path = f'{cwd}/{path}'
        if os.path.exists(found_path):
            return found_path 

    if os.path.exists(path):
        return path

    return None